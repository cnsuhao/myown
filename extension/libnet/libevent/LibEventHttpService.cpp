#include "LibEventHttpService.h"
#include <event2/buffer.h>
#include <event2/listener.h>
#include "event2/http.h"
#include "action/IHttpAction.h"
#include <event2/keyvalq_struct.h>
#include <event2/rpc.h>
#include "LibEventHttpConnection.h"
#include <event2/dns.h>
#include <event2/bufferevent_ssl.h>
#include <memory>
#include <assert.h>
#include "event2/thread.h"
#include "openssl/err.h"

#define FRAME_TIME_OUT 15
void LibEventHttpService::on_quit_service_cb(evutil_socket_t, short, void* p)
{
	LibEventHttpService* pService = (LibEventHttpService*)p;
	if (NULL != pService)
	{
		pService->NotifyQuit();
	}
}

void LibEventHttpService::on_flush_service_cb(evutil_socket_t, short, void* p)
{
	LibEventHttpService* pService = (LibEventHttpService*)p;
	if (NULL != pService)
	{
		pService->NotifyFlush();
	}
}

void LibEventHttpService::on_frame_service_cb(evutil_socket_t, short, void* p)
{
	LibEventHttpService* pService = (LibEventHttpService*)p;
	if (NULL != pService)
	{
		pService->NotifyFrame();
	}
}

void on_http_closed(struct evhttp_connection* conn, void* ctx)
{
	LibEventHttpConnection* pNetConn = (LibEventHttpConnection*)ctx;
	if (NULL != pNetConn)
	{
		LibEventHttpService* service = dynamic_cast<LibEventHttpService*>(pNetConn->GetService());
		if (NULL != service)
		{
			// 销毁ssl
#ifdef NET_SLL_SUPPORT
			//struct bufferevent *bev = evhttp_connection_get_bufferevent(conn);
			//if (NULL != bev)
			//{
			//	SSL* ssl = bufferevent_openssl_get_ssl(bev);
			//	if (NULL != ssl && NULL != service->GetSSL() )
			//	{
			//		service->GetSSL()->FreeSSL(ssl);
			//	}
			//}
#endif
			if (!pNetConn->IsClosed())
			{
				pNetConn->SetState(EM_CONN_STATE_CLOSING);
			}


			service->RemoveConnection(pNetConn->GetConnId(), EM_NET_REASON_SELF_DISCONNECT, 0);
		}
		else
		{
			pNetConn->Close(EM_NET_REASON_SELF_DISCONNECT);
		}
	}
}

bool parse_http_request(IHttpRequest* httpReq, struct evhttp_request* evReq)
{
	int nMethod = evhttp_request_get_command(evReq);
	httpReq->SetMethod( (EM_HTTP_METHOD)nMethod );

	struct evkeyvalq *headers = evhttp_request_get_input_headers(evReq);
	for (struct evkeyval *header = headers->tqh_first; NULL != header;
		header = header->next.tqe_next) {
		httpReq->AddHeader( header->key, header->value );
	}

	struct evbuffer* buf = evhttp_request_get_input_buffer( evReq );
	while ( evbuffer_get_length(buf) ) {
		int n;
		char cbuf[1024];
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
		{
			httpReq->AppendText(cbuf, n);
		}
	}

	httpReq->SetURI(evhttp_request_get_uri(evReq));
	return true;
}

bool reply_http_request(IHttpResponse* httpRes, struct evhttp_request* evReq)
{
	const std::map<std::string, std::string> headers = httpRes->GetHeaders();
	evkeyvalq* hq = evhttp_request_get_output_headers(evReq);
	if (NULL != hq)
	{
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
		{
			evhttp_add_header(hq, itr->first.c_str(), itr->second.c_str());
		}
	}

	size_t nLen = httpRes->GetTextLen();
	evbuffer* buf = evbuffer_new();
	if (nLen == 0)
	{
		const char* szReason = httpRes->GetReason();
		if (NULL != szReason)
		{
			evbuffer_add(buf, szReason, strlen(szReason));
		}
	}
	else
	{
		evbuffer_add(buf, httpRes->GetText(), nLen);
	}

	evhttp_send_reply(evReq, httpRes->GetResult(), httpRes->GetReason(), buf );
	evbuffer_free(buf);
	return true;
}

#ifdef NET_SLL_SUPPORT
struct bufferevent* on_env_cb(struct event_base* base, void* arg)
{
	struct bufferevent* r;
	SSL_CTX *ctx = (SSL_CTX *)arg;

	r = bufferevent_openssl_socket_new(base, -1, SSL_new(ctx), BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);
	return r;
}
#endif

void on_http_request(struct evhttp_request *req, void *ctx)
{
	if (NULL == req)
	{
		return;
	}

	TFastStr<char, 32> strReason = "internal error";
	int nErrCode = HTTP_INTERNAL;
	
	std::shared_ptr<INetAction> http( new HttpAction );
	LibEventHttpService* pService = (LibEventHttpService*)ctx;
	if (NULL == pService)
	{
		goto ERR_REPLY;
	}

	{
		// parse http request
		IHttpRequest* httpReq = dynamic_cast<IHttpRequest*>(http->Request());
		IHttpResponse* httpRes = dynamic_cast<IHttpResponse*>(http->Response());
		if (NULL == httpReq || httpRes == NULL)
		{
			goto ERR_REPLY;
		}

		httpRes->SetRequest(httpReq);
		if (!parse_http_request(httpReq, req))
		{
			goto ERR_REPLY;
		}

		{
			evhttp_connection* conn = evhttp_request_get_connection(req);
			if (NULL != conn)
			{
				LibEventHttpConnection* pConn = pService->MakeRspConnection( conn );
				// 创建失败清除
				if (NULL == pConn)
				{
					evhttp_connection_set_timeout(conn, 1);
					evhttp_connection_set_closecb(conn, NULL, NULL);
					goto ERR_REPLY;
				}
				else
				{
					// 未连接的先调用连接成功回调
					if (!pConn->IsConnected())
					{
						pConn->OnConnected();
					}
					pConn->OnRequest(req, http);
				}
				return;
			}
		}
	}

ERR_REPLY:
	{
		struct evbuffer* buf = evbuffer_new();
		evbuffer_add(buf, strReason.c_str(), strReason.size());
		evhttp_send_reply(req, nErrCode, strReason.c_str(), buf);
		evbuffer_free(buf);
	}
}

void on_http_response(struct evhttp_request *req, void *ctx)
{
	LibEventHttpConnection* pNetConn = (LibEventHttpConnection*)ctx;
	if (NULL == pNetConn)
	{
		return;
	}

	if (NULL != req)
	{
		// 未连接的先调用连接成功回调
		if ( pNetConn->GetState() == EM_CONN_STATE_CONNECTING )
		{
			pNetConn->OnConnected();
		}
		pNetConn->OnResponse(req);
	}
	else
	{
#ifdef NET_SLL_SUPPORT
		if ( pNetConn->IsSSL() )
		{
			evhttp_connection* conn = pNetConn->GetHttpContext();
			if (NULL != conn)
			{
				struct bufferevent *bev = evhttp_connection_get_bufferevent(conn);
				if (NULL != bev)
				{
					unsigned long oslerr;
					int printed_err = 0;
					int errcode = EVUTIL_SOCKET_ERROR();
					fprintf(stderr, "some request failed - no idea which one though!\n");
					/* Print out the OpenSSL error queue that libevent
					* squirreled away for us, if any. */
					char buffer[1024];
					while ((oslerr = bufferevent_get_openssl_error(bev))) {
						ERR_error_string_n(oslerr, buffer, sizeof(buffer));
						fprintf(stderr, "%s\n", buffer);
						printed_err = 1;
					}
					/* If the OpenSSL error queue was empty, maybe it was a
					* socket error; let's try printing that. */
					if (!printed_err)
						fprintf(stderr, "socket error = %s (%d)\n", evutil_socket_error_to_string(errcode),
						errcode);
				}
			}
		}
#endif

		// 关闭自己 http下不会回调close
		LibEventHttpService* pService = dynamic_cast<LibEventHttpService*>(pNetConn->GetService());
		if (pService != NULL)
		{
			pService->RemoveConnection(pNetConn->GetConnId(), EM_NET_REASON_PEER_DISCONNECT, 0);
		}		
	}
}


LibEventHttpService::LibEventHttpService(INetIoDesc* desc)
	: NetService( desc )
	, m_pevBase(NULL)
	, m_pevDNS(NULL)
	, m_evHttp(NULL)
	, m_evhttpSocket(NULL)
	, m_pFlushNotify(NULL)
	, m_pQuitNotify(NULL)
	, m_pFrameNotify(NULL)
#ifdef NET_SLL_SUPPORT
	, m_pSSL( NULL )
#endif
{
	//SetReqDefHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:46.0) Gecko/20100101 Firefox/46.0");
	//SetReqDefHeader("Accept-Language ", "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
	//SetReqDefHeader("Connection", "keep-alive");
	
	// SetReqDefHeader("Accept-Encoding", "gzip, deflate, br");
	// SetReqDefHeader("Accept", "*/*");

	try
	{
		m_pHttpDesc = dynamic_cast<HttpIoDesc*>(desc);
	}
	catch (...)
	{
	}
}

bool LibEventHttpService::Startup()
{
	if ( NULL == m_pIoDesc )
	{
		return false;
	}

	if (NULL != m_pevBase)
	{
		return false;
	}

	struct event_config* cfg = event_config_new();
	if (NULL == cfg)
	{
		return false;
	}
#ifdef WIN32
	//event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	//evthread_use_windows_threads();
#endif
	m_pevBase = event_base_new_with_config(cfg);
	if (NULL == m_pevBase)
	{
		return false;
	}

	event_config_free(cfg);
	cfg = NULL;

	/* Extract and display the address we're listening on. */
	/*
	{
	struct sockaddr_storage ss;
	evutil_socket_t fd;
	ev_socklen_t socklen = sizeof(ss);
	char addrbuf[128];
	void *inaddr;
	const char *addr;
	int got_port = -1;
	fd = evhttp_bound_socket_get_fd(handle);
	memset(&ss, 0, sizeof(ss));
	if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
	perror("getsockname() failed");
	return 1;
	}
	if (ss.ss_family == AF_INET) {
	got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
	inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
	}
	else if (ss.ss_family == AF_INET6) {
	got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
	inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
	}
	else {
	fprintf(stderr, "Weird address family %d\n",
	ss.ss_family);
	return 1;
	}
	addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
	sizeof(addrbuf));
	if (addr) {
	printf("Listening on %s:%d\n", addr, got_port);
	evutil_snprintf(uri_root, sizeof(uri_root),
	"http://%s:%d", addr, got_port);
	}
	else {
	fprintf(stderr, "evutil_inet_ntop failed\n");
	return 1;
	}
	}*/

	// 退出通知
	{
		m_pQuitNotify = event_new(m_pevBase, -1, EV_PERSIST, on_quit_service_cb, this);
		if (NULL == m_pQuitNotify)
			goto CLEAR;
		event_add(m_pQuitNotify, NULL);
	}
	{
		// 刷新消息通知
		m_pFlushNotify = event_new(m_pevBase, -1, EV_PERSIST, on_flush_service_cb, this);
		if (NULL == m_pFlushNotify)
			goto CLEAR;
		event_add(m_pFlushNotify, NULL);
	}

	{
		m_pFrameNotify = event_new(m_pevBase, -1, EV_PERSIST, on_frame_service_cb, this);
		if (NULL == m_pFrameNotify)
			goto CLEAR;

		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 0;
		tv.tv_usec = FRAME_TIME_OUT * 1000;
		event_add(m_pFrameNotify, &tv);
	}

#ifdef NET_SLL_SUPPORT
	// 是否支持https
	m_pSSL = new MySSL();
	if (!m_pSSL->Init())
	{
		goto CLEAR;
	}
#endif

	// 启动线程
	{
		//std::thread worker(OnLibEventHttpWorker, m_pevBase);
		//m_worker.swap(worker);
	}	
	return NetService::Startup();

CLEAR:
	Stop( 0 );
	return false;
}

bool LibEventHttpService::Stop( int nWaitMillsec )
{
	if (NULL != m_evhttpSocket)
	{
		evhttp_del_accept_socket(m_evHttp, m_evhttpSocket);
		m_evhttpSocket = NULL;
	}

	if (NULL != m_pQuitNotify)
	{
		event_active(m_pQuitNotify, 0, 0);
	}

	if (NetService::Stop( nWaitMillsec ))
	{
		Cleanup();
		return true;
	}

	return false;
}

bool LibEventHttpService::Listen()
{
	m_evHttp = evhttp_new(m_pevBase);
	if (NULL == m_evHttp)
	{
		return false;
	}

	m_pevDNS = evdns_base_new(m_pevBase, 1);
#ifdef NET_SLL_SUPPORT
	if (m_pIoDesc->IsSSL())
	{
		/* This is the magic that lets evhttp use SSL. */
		evhttp_set_bevcb(m_evHttp, on_env_cb, m_pSSL->Context());
	}
#endif
	evhttp_set_gencb(m_evHttp, on_http_request, this);
	m_evhttpSocket = evhttp_bind_socket_with_handle(m_evHttp, m_pIoDesc->ListenIP(), m_pIoDesc->ListenPort());
	if (NULL == m_evhttpSocket)
	{
		return false;
	}

	return true;
}


bool LibEventHttpService::Send(const NID& id, const char* szMsg, size_t nLen)
{
	assert(false);
	return false;
}

bool LibEventHttpService::Send(const NID& id, std::shared_ptr<INetAction>& action)
{
	this->Post([this, id, action]()->void{

		assert(NULL != m_pevBase);
		std::shared_ptr<INetAction> p = action;
		LibEventHttpConnection* pConn = NULL;

		IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
		if (NULL == req)
		{
			return;
		}

		unsigned short nPort = req->GetPort();
		const char* host = req->GetHost();
		if (nPort == 0 || host == NULL || host[0] == 0)
		{
			return;
		}

		if (id == NULL_NID)
		{
			pConn = MakeReqConnection(host, nPort, req->IsHttps());
		}
		else
		{
			pConn = dynamic_cast<LibEventHttpConnection*>(this->Lookup(id));
		}

		if (NULL == pConn)
		{
			if (NULL != m_pHandler)
			{
				p->SetState(EM_ACTION_RESULT);
				IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
				if (NULL != res)
				{
					res->SetReason("connection failed.");
					res->SetResult(EM_HTTP_CODE_SERVUNAVAIL);
					m_pHandler->OnTimeout(NULL, p);
				}
			}
		}
		else
		{
			FillReqDefHeaer(req);
			const char* value = req->GetHeader("Host");
			if (NULL == value || value[0] == 0)
			{
				req->AddHeader("Host", host);
			}

			std::shared_ptr<INetAction> p = action;
			if (p->IsRequest())
			{
				if (!pConn->Send(p))
				{
					pConn->Close(EM_NET_REASON_SELF_DISCONNECT);
				}
			}
			else
			{
				pConn->Reply(p);
			}
		}
	});

	return true;
}

void LibEventHttpService::Run()
{
	if (NULL != m_pevBase)
	{
		event_base_dispatch(m_pevBase);
	}

	// 最后一次处理
	Process();
}

void LibEventHttpService::Cleanup()
{
	if (m_pFlushNotify) {
		event_free(m_pFlushNotify);
		m_pFlushNotify = NULL;
	}

	if (m_pQuitNotify) {
		event_free(m_pQuitNotify);
		m_pQuitNotify = NULL;
	}

	if (m_pFrameNotify) {
		event_free(m_pFrameNotify);
		m_pFrameNotify = NULL;
	}

	if (NULL != m_evhttpSocket)
	{
		evhttp_del_accept_socket(m_evHttp, m_evhttpSocket);
		m_evhttpSocket = NULL;
	}
	if (NULL != m_evHttp)
	{
		evhttp_free(m_evHttp);
		m_evHttp = NULL;
	}

	if (NULL != m_pevDNS)
	{
		evdns_base_free(m_pevDNS, 1);
		m_pevDNS = NULL;
	}

	if (NULL != m_pevBase)
	{
		event_base_free(m_pevBase);
		m_pevBase = NULL;
	}

#ifdef NET_SLL_SUPPORT
	if (NULL != m_pSSL)
	{
		m_pSSL->Release();
		delete m_pSSL;
		m_pSSL = NULL;
	}
#endif
	NetService::Cleanup();
}

void LibEventHttpService::NotifyQuit()
{
	event_base_loopexit(m_pevBase, 0);
}

void LibEventHttpService::NotifyFlush()
{
	Process();
}

void LibEventHttpService::NotifyFrame()
{
	Process();
	if (!IsRunning())
	{
		if (NULL != m_pQuitNotify)
		{
			event_active(m_pQuitNotify, 0, 0);
		}
	}
}

void LibEventHttpService::FillReqDefHeaer(IHttpRequest* req) const
{
	assert(NULL != req);
	if (NULL == m_pHttpDesc)
	{
		return;
	}

	const std::map<std::string, std::string>& defHeaders = m_pHttpDesc->GetDefHeaders();
	for (std::map<std::string, std::string>::const_iterator itr = defHeaders.begin();
						itr != defHeaders.end(); ++itr)
	{
		const char* value = req->GetHeader( itr->first.c_str() );
		if (NULL == value || value[0] == 0)
		{
			req->AddHeader(itr->first.c_str(), itr->second.c_str());
		}
	}
}

void LibEventHttpService::AddReqConnection(struct evhttp_connection* evcon, LibEventHttpConnection* pConn)
{
	assert(NULL != evcon);
	assert(NULL != pConn);
	if ( NULL != pConn && NULL != evcon )
	{
		m_mapRequests[evcon] = pConn;
	}
}

void LibEventHttpService::RemoveReqConnection( struct evhttp_connection* evcon )
{
	assert(NULL != evcon);
	if (NULL != evcon)
	{
		m_mapRequests.erase(evcon);
	}
}

LibEventHttpConnection* LibEventHttpService::GetReqConnection(const std::string& strHost, unsigned short port)
{
	int nReuseNum = 0;
	if (NULL != m_pHttpDesc)
	{
		nReuseNum = m_pHttpDesc->GetReqReuseNum();
	}

	if (nReuseNum <= 1)
	{
		return NULL;
	}

	for (std::map<struct evhttp_connection*, LibEventHttpConnection*>::const_iterator itr = m_mapRequests.begin();
					itr != m_mapRequests.end(); ++itr)
	{
		LibEventHttpConnection* pConn = itr->second;
		if (pConn->RemoteIP() == NULL)
		{
			continue;
		}

		if (strHost.compare(pConn->RemoteIP()) == 0 && port == pConn->RemotePort())
		{
			if ((int)pConn->GetReqLength() < nReuseNum)
			{
				return pConn;
			}
		}
	}

	return NULL;
}

void LibEventHttpService::ClearReqConnection()
{
	m_mapRequests.clear();
}

LibEventHttpConnection* LibEventHttpService::MakeReqConnection( const std::string& strHost, unsigned short port, bool bIsHppts )
{
	LibEventHttpConnection* pConn = GetReqConnection( strHost, port );

	// 不存在 创建新连接
	if ( NULL == pConn )
	{
		struct evhttp_connection* conn = NULL;
		if (bIsHppts)
		{
#ifdef NET_SLL_SUPPORT
			SSL *ssl = m_pSSL->CreateSSL();
			if (!ssl)
			{
				printf("ERR:create ssl failed.");
			}
			else
			{
				struct bufferevent *bev = bufferevent_openssl_socket_new(
					m_pevBase, -1, ssl, BUFFEREVENT_SSL_CONNECTING,
					0
					| BEV_OPT_CLOSE_ON_FREE
					| BEV_OPT_DEFER_CALLBACKS
					);
				conn = evhttp_connection_base_bufferevent_new(m_pevBase, m_pevDNS, bev, strHost.c_str(), port);
				if (NULL == conn)
				{
					bufferevent_free(bev);
					m_pSSL->FreeSSL(ssl);
				}
			}
#else
			printf("ERR:http server don't support ssl");
#endif
		}
		else
		{
			conn = evhttp_connection_base_new(m_pevBase, m_pevDNS, strHost.c_str(), port);
			//struct bufferevent *bev = bufferevent_socket_new(m_pevBase, -1, BEV_OPT_CLOSE_ON_FREE);
			//conn = evhttp_connection_base_bufferevent_new(m_pevBase, m_pevDNS, bev, strHost.c_str(), port);
			//if (NULL == conn)
			//{
			//	bufferevent_free(bev);
			//}
		}

		if (NULL != conn)
		{
			pConn = dynamic_cast<LibEventHttpConnection*>(this->InnerNewConnection(0, strHost, port, 0, conn, false));
			// 创建失败清除
			if (NULL == pConn)
			{
				evhttp_connection_set_closecb(conn, NULL, NULL);
				if (bIsHppts)
				{
#ifdef NET_SLL_SUPPORT
					struct bufferevent *bev = evhttp_connection_get_bufferevent(conn);
					if (NULL != bev)
					{
						SSL* ssl = bufferevent_openssl_get_ssl(bev);
						if (NULL != ssl)
						{
							m_pSSL->FreeSSL(ssl);
						}
					}
#endif
				}
				evhttp_connection_free(conn);
			}
			else
			{
				AddReqConnection(conn, pConn);
			}
		}
		else
		{
			printf("ERR:http server create evhttp_connection failed");
		}
	}
	return pConn;
}


void LibEventHttpService::RemoveConnection(const NID& nid, EMNetReason reason, int nErrCode)
{
	LibEventHttpConnection* pConn = dynamic_cast<LibEventHttpConnection*>( Lookup(nid) );
	if (NULL != pConn)
	{
		if ( pConn->IsAccept() )
		{
			RemoveRspConnection( pConn->GetHttpContext() );
		}
		else
		{
			RemoveReqConnection(pConn->GetHttpContext());
		}
	}
	NetService::RemoveConnection(nid, reason, nErrCode);
}

NetConnection* LibEventHttpService::InnerNewConnection(NETFD fd, const std::string& ip, unsigned short port, 
					int key, void* context, bool isAccept)
{
	NetConnection* pConn = NetService::InnerNewConnection(fd, ip, port, key, context, isAccept);
	if (NULL != pConn)
	{
		pConn->SetState(EM_CONN_STATE_CONNECTING);
		LibEventHttpConnection* pevConn = dynamic_cast<LibEventHttpConnection*>(pConn);
		if (NULL != pevConn)
		{
			struct evhttp_connection* conn = (struct evhttp_connection*)context;
			evhttp_connection_set_closecb(conn, on_http_closed, pevConn);
			if (NULL != m_pHttpDesc &&  m_pHttpDesc->GetReqRetires() > 0)
			{
				evhttp_connection_set_retries(conn, m_pHttpDesc->GetReqRetires());
			}
			pevConn->Initilize(conn, m_pFlushNotify);
			return pevConn;
		}
		RemoveConnection(pConn->GetConnId(), EM_NET_REASON_LOGIC_ERRROR, 0);
	}

	return NULL;
}

NetConnection* LibEventHttpService::InnerRemoveConnection(const NID& nid)
{
	NetConnection* pConn = NetService::InnerRemoveConnection(nid);
	//* 连接未保存连接池
	if (NULL != pConn && pConn->IsAccept())
	{
		LibEventHttpConnection* pHttpConn = dynamic_cast<LibEventHttpConnection*>(pConn);
		if (NULL != pHttpConn)
		{
			RemoveRspConnection(pHttpConn->GetHttpContext());
		}
	}

	return pConn;
}

LibEventHttpConnection* LibEventHttpService::MakeRspConnection(struct evhttp_connection* evconn )
{
	LibEventHttpConnection* pConn = NULL;
	std::map<struct evhttp_connection*, LibEventHttpConnection*>::iterator itr = m_mpaReplies.find(evconn);
	if (itr != m_mpaReplies.end())
	{
		pConn = itr->second;
	}

	if (NULL == pConn)
	{
		char* host = "";
		unsigned short nPort = 0;
		evhttp_connection_get_peer(evconn, &host, &nPort);
		std::string strHost;
		if (NULL != host)
		{
			strHost = host;
		}
		pConn = dynamic_cast<LibEventHttpConnection*>(InnerNewConnection(0, strHost, nPort, 0, evconn, true));
		assert(NULL != pConn);
		if (NULL == pConn)
		{
			return pConn;
		}
		m_mpaReplies[evconn] = pConn;
	}
	return pConn;
}

void LibEventHttpService::RemoveRspConnection( struct evhttp_connection* evconn )
{
	if (NULL != evconn)
	{
		m_mpaReplies.erase(evconn);
	}
}
