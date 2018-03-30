#include "LibEventHttpService.h"
#include "LibEventSocket.h"
#include <event2\buffer.h>
#include <event2\listener.h>
#include "event2\http.h"
#include "IHttpAction.h"
#include "event2\keyvalq_struct.h"
#include "event2\rpc.h"

int OnLibEventHttpWorker(void* args)
{
	struct event_base* pBase = (struct event_base*)args;
	if (NULL == pBase)
	{
		return -1;
	}
	event_base_dispatch(pBase);
	return 0;
}

bool ParseHttpRequest(IHttpRequest* httpReq, struct evhttp_request* evReq)
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
			httpReq->AppendBody(cbuf, n);
		}
	}

	httpReq->SetURI(evhttp_request_get_uri(evReq));
	return true;
}

bool ReplyHttpRequest(IHttpResponse* httpRes, struct evhttp_request* evReq)
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

	size_t nLen = httpRes->GetMsgLen();
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
		evbuffer_add(buf, httpRes->GetMsgBody(), nLen);
	}

	evhttp_send_reply(evReq, httpRes->GetResult(), httpRes->GetReason(), buf );
	evbuffer_free(buf);
	return true;
}

void OnLibEventHttpRequest(struct evhttp_request *req, void *ctx)
{
	if (NULL == req)
	{
		return;
	}

	TFastStr<char, 32> strReason = "internal error";
	int nErrCode = HTTP_INTERNAL;
	
	LibEventHttpService* service = (LibEventHttpService*)ctx;
	if (NULL == service)
	{
		goto ERR_REPLY;
	}
	INetIoHandler* handler = service->GetHandler();
	if (NULL == handler)
	{
		goto ERR_REPLY;
	}

	{
		// parse http request
		HttpAction http;
		IHttpRequest* httpReq = dynamic_cast<IHttpRequest*>(http.Request());
		IHttpResponse* httpRes = dynamic_cast<IHttpResponse*>(http.Response());
		if (NULL == httpReq || httpRes == NULL)
		{
			goto ERR_REPLY;
		}

		httpRes->SetRequest(httpReq);
		if (!ParseHttpRequest(httpReq, req))
		{
			goto ERR_REPLY;
		}

		handler->OnAction(NULL, &http);
		if (ReplyHttpRequest(httpRes, req))
		{
			return;
		}
		strReason = httpRes->GetReason();
		nErrCode = httpRes->GetResult();
	}

ERR_REPLY:
	{
		struct evbuffer* buf = evbuffer_new();
		evbuffer_add(buf, strReason.c_str(), strReason.size());
		evhttp_send_reply(req, nErrCode, strReason.c_str(), buf);
		evbuffer_free(buf);
	}
}

bool StartHttpRequest(LibEventHttpService* service, IHttpAction* action);
void OnLibEventHttpResponse(struct evhttp_request *req, void *ctx)
{
	LibEventHttpService* service = (LibEventHttpService*)ctx;
	IHttpAction* action = NULL;
	if (service == NULL)
	{
		goto END;
	}
	action = service->PopRequest(req);
	if (NULL == action)
	{
		goto END;
	}
	IHttpRequest* request = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* response = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == request || NULL == response)
	{
		goto END;
	}

	int nResCode = evhttp_request_get_response_code(req);
	response->SetResult( nResCode );
	bool bHandle = true;
	if (nResCode == HTTP_MOVETEMP)
	{
		struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
		const char *new_location = evhttp_find_header(headers, "Location");
		if (NULL != new_location)
		{
			request->SetURI(new_location);
			StartHttpRequest(service, action);
			bHandle = false;
		}
	}

	if (bHandle)
	{
		struct evkeyvalq *headers = evhttp_request_get_input_headers(req);
		for (struct evkeyval *header = headers->tqh_first; NULL != header;
			header = header->next.tqe_next) {
			response->AddHeader(header->key, header->value);
		}

		struct evbuffer* buf = evhttp_request_get_input_buffer(req);
		while (evbuffer_get_length(buf)) {
			int n;
			char cbuf[1024];
			n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
			if (n > 0)
			{
				response->AppendBody(cbuf, n);
			}
		}

		INetIoHandler* handler = service->GetHandler();
		if (NULL != handler)
		{
			if (handler->OnReply(NULL, action))
			{
				delete action;
			}
		}
		else
		{
			delete action;
		}
	}

END:
	struct evhttp_connection* conn = evhttp_request_get_connection(req);
	const struct evhttp_uri* uri = evhttp_request_get_evhttp_uri(req);
	if (NULL != conn)
	{
		evhttp_connection_free(conn);
	}
	if (NULL != uri)
	{
		evhttp_uri_free((struct evhttp_uri*)uri);
	}
	//evhttp_request_free(req);
}

void OnLibEventHttpClientClosed( struct evhttp_connection* conn, void* arg )
{

}

void OnLibEventHttpClinetUpdate(evutil_socket_t fd, short, void * ctx)
{
	LibEventHttpService* service = (LibEventHttpService*)ctx;
	if (service != NULL)
	{
		struct event_base* base = service->GetClientBase();
		if (NULL != base)
		{
			event_base_loop(base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
		}
	}
}

inline void SetDefaultHeader(IHttpRequest* req, 
						struct evkeyvalq* header,
						const char* key,
						const char* defValue)
{
	const char* value = req->GetHeader(key);
	if (NULL == value || value[0] == 0)
	{
		evhttp_add_header(header, key, defValue);
	}
}

bool StartHttpRequest(LibEventHttpService* service, IHttpAction* action)
{
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	if (NULL == req)
	{
		return false;
	}

	struct event_base* base = service->GetClientBase();
	struct evhttp_request* evReq = NULL;
	struct evhttp_connection* conn = NULL;
	struct evbuffer* buf = NULL;
	struct evkeyvalq* header = NULL;
	struct evhttp_uri *uri = evhttp_uri_parse(req->GetURI());
	if (NULL == uri)
	{
		return false;
	}

	std::string data;
	int nPort = evhttp_uri_get_port(uri);
	if (nPort == -1) nPort = 80;
	const char* host = evhttp_uri_get_host(uri);
	evReq = evhttp_request_new(OnLibEventHttpResponse, service);
	if (NULL == evReq)
	{
		goto FAILED;
	}

	
	conn = evhttp_connection_base_new(base,
							NULL,
							host, nPort);
	if (NULL == conn)
	{
		goto FAILED;
	}

	evhttp_connection_set_timeout(conn, req->GetReqTimeOut());
	header = evhttp_request_get_output_headers(evReq);
	if (NULL != header)
	{
		const std::map<std::string, std::string>& headers = req->GetHeaders();
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin();
			itr != headers.end();
			++itr)
		{
			evhttp_add_header(header, itr->first.c_str(), itr->second.c_str());
		}

		SetDefaultHeader(req, header, "User-Agent", "jamboy-client");
		SetDefaultHeader(req, header, "Connection", "Keep-Alive");
		SetDefaultHeader(req, header, "Accept", "text/html,application/xhtml+xml,application/xml,application/json");
		char szHost[128] = { 0 };
		sprintf(szHost, "%s:%d", host, nPort);
		SetDefaultHeader(req, header, "Host", szHost);
	}

	const std::map<std::string, std::string>& datas = req->GetDatas();
	for (std::map<std::string, std::string>::const_iterator itr = datas.begin();
		itr != datas.end();
		++itr) 
	{
		data.append("&");
		data.append(itr->first);
		data.append("=");
		data.append(itr->second);
	}

	buf = evhttp_request_get_output_buffer(evReq);
	if (data.size() > 1 && NULL != buf)
	{
		evbuffer_add(buf, data.c_str() + 1, data.size() - 1);
	}
	service->PushRequest( evReq, action );
	evhttp_connection_set_closecb(conn, NULL, NULL);
	
	if (req->GetMethod() == EM_HTTP_METHOD_POST)
	{
		const char *path = evhttp_uri_get_path(uri);
		evhttp_make_request(conn, evReq, EVHTTP_REQ_POST, path ? path : "/");
		return true;
	}
	else if (req->GetMethod() == EM_HTTP_METHOD_GET)
	{
		const char *query = evhttp_uri_get_query(uri);
		TFastStr<char, 128> path = evhttp_uri_get_path(uri);
		if (NULL != query && query[0] != 0)
		{
			path.append('?');
			path.append(query);
			path.append(data.c_str());
		}
		else
		{
			if (path.size() == 0)
			{
				path.append('/');
			}

			if (data.size() > 0)
			{
				data[0] = '?';
				path.append(data.c_str());
			}
		}
		evhttp_make_request(conn, evReq, EVHTTP_REQ_GET, path.c_str());
		return true;
	}


FAILED:
	if (NULL != uri)
	{
		evhttp_uri_free(uri);
	}

	if (NULL != conn)
	{
		evhttp_connection_free(conn);
	}

	if (NULL != evReq)
	{
		evhttp_request_free(evReq);
	}
	return false;
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

	m_pevConfig = event_config_new();
	if (NULL == m_pevConfig)
	{
		return false;
	}
#ifdef WIN32
	event_config_set_flag(m_pevConfig, EVENT_BASE_FLAG_STARTUP_IOCP);
#endif
	m_pevBase = event_base_new_with_config( m_pevConfig );
	if (NULL == m_pevBase)
	{
		return false;
	}

	m_evHttp = evhttp_new( m_pevBase );
	if (NULL == m_evHttp)
	{
		goto CLEAR;
	}

	evhttp_set_gencb( m_evHttp, OnLibEventHttpRequest, this );
	m_evhttpSocket = evhttp_bind_socket_with_handle(m_evHttp, m_pIoDesc->ListenIP(), m_pIoDesc->ListenPort());
	if (NULL == m_evhttpSocket)
	{
		goto CLEAR;
	}

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

	// config client
	{
		m_pevClient = event_base_new();
		if (NULL == m_pevClient)
		{
			return false;
		}
		m_pev_msg_notify = event_new(m_pevBase, -1, EV_PERSIST, OnLibEventHttpClinetUpdate, this);
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		event_add(m_pev_msg_notify, &tv);
	}

	// Æô¶¯Ïß³Ì
	{
		std::thread worker(OnLibEventHttpWorker, m_pevBase);
		m_worker.swap(worker);
	}	
	return NetService::Startup();

CLEAR:
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
	event_base_free(m_pevBase);
	m_pevBase = NULL;
	return false;
}

bool LibEventHttpService::Stop()
{
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

	if (NULL != m_pev_msg_notify)
	{
		event_del(m_pev_msg_notify);
		event_free(m_pev_msg_notify);
	}

	if ( NULL == m_pevClient )
	{
		event_base_loopexit(m_pevClient, 0);
		event_base_free(m_pevClient);
		m_pevClient = NULL; 
	}

	if (NULL != m_pevBase)
	{
		event_base_loopexit(m_pevBase, 0); 
		event_base_free(m_pevBase);
		m_pevBase = NULL;
	}
	
	if (m_worker.joinable())
	{
		m_worker.join();
	}

	for (std::vector<NetConnection*>::iterator itr = m_vecConns.begin(); itr != m_vecConns.end(); ++itr)
	{
		NetConnection* conn = *itr;
		if (NULL != conn)
		{
			conn->Close(EM_NET_REASON_SERVER_DISCONNECT);
			ISocketDesc* socket = conn->GetDesc();
			if (NULL != socket)
			{
				delete socket;
			}

			m_pFactory->FreeConnection(conn);
		}
	}
	m_vecConns.clear();
	while (!m_lstFreeIndex.empty())
	{
		m_lstFreeIndex.pop();
	}

	return NetService::Stop();
}

bool LibEventHttpService::StartHttpRequest( IHttpAction* action )
{
	return ::StartHttpRequest( this, action );
}
