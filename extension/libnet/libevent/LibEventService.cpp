#include "LibEventService.h"
#include <event2\buffer.h>
#include <event2\listener.h>
#include "LibEventConnection.h"

#define FRAME_TIME_OUT 15

void LibEventService::on_quit_service_cb( evutil_socket_t, short, void* p )
{
	LibEventService* pService = (LibEventService*)p;
	if (NULL != pService)
	{
		pService->NotifyQuit();
	}
}

void LibEventService::on_flush_service_cb(evutil_socket_t, short, void* p)
{
	LibEventService* pService = (LibEventService*)p;
	if (NULL != pService)
	{
		pService->NotifyFlush();
	}
}

void LibEventService::on_frame_service_cb(evutil_socket_t, short, void* p)
{
	LibEventService* pService = (LibEventService*)p;
	if (NULL != pService)
	{
		pService->NotifyFrame();
	}
}


void LibEventService::on_recv_cb( struct bufferevent *bev, void *ctx )
{
	struct evbuffer * input = bufferevent_get_input(bev);
	if (evbuffer_get_length(input))
	{
		LibEventConnection* pConn = (LibEventConnection*)ctx;
		if (NULL == pConn)
		{
			return;
		}
		
		DataBuffer& db = pConn->GetRecvBufer();
		char* pb = db.GetBuffer();
		while (evbuffer_get_length(input))
		{
			int nSpace = (int)db.GetLeftSpace();
			if (0 == nSpace)
			{
				pConn->Close( EM_NET_REASON_LOGIC_ERRROR );
				break;
			}

			int nReadSize = evbuffer_remove(input, pb + db.GetWritePos(), nSpace);
			if (nReadSize < 0)
			{
				pConn->Close(EM_NET_REASON_SOCKET_ERRROR);
				break;
			}

			if (!pConn->OnRecv(NULL, (size_t)nReadSize))
			{
				break;
			}
		}
	}
}

void LibEventService::on_send_cb(struct bufferevent *bev, void *ctx)
{
	//struct evbuffer * output = bufferevent_get_output(bev);
	//size_t nSize = evbuffer_get_length(output);
	//if (nSize > 0)
	//{
	//	evbuffer_drain(output, nSize);
		LibEventConnection* pConn = (LibEventConnection*)ctx;
		if (NULL == pConn)
		{
			return;
		}

		pConn->OnSend(NULL, 0);
	//}
}

void LibEventService::on_event_cb(struct bufferevent *bev, short error, void *ctx)
{
	if (error & BEV_EVENT_EOF || error & BEV_EVENT_ERROR || error & BEV_EVENT_TIMEOUT)
	{
		LibEventConnection* pConn = (LibEventConnection*)ctx;
		if (NULL == pConn)
		{
			return;
		}

		pConn->SetState(EM_CONN_STATE_CLOSING);
		LibEventService* service = (LibEventService*)pConn->GetService();
		if (NULL != service)
		{
			service->RemoveConnection(pConn->GetConnId(), EM_NET_REASON_SELF_DISCONNECT, error);
		}
		else
		{
			delete pConn;
		}
	}
	else if ( error & BEV_EVENT_CONNECTED )
	{
		LibEventConnection* pConn = (LibEventConnection*)ctx;
		if (NULL != pConn)
		{
			pConn->OnConnected();
		}
	}
}

void LibEventService::on_service_accept_cb( struct evconnlistener* listener,
			evutil_socket_t fd,
			struct sockaddr* sa, int nSockLen, void *ud )
{

	LibEventService* pService = (LibEventService*)(ud);
	if (NULL == pService)
	{
		evutil_closesocket(fd);
		return;
	}

	if (!pService->IsRunning())
	{
		evutil_closesocket(fd);
	}

	char* ip = NULL;
	unsigned short port = 0;
	struct sockaddr_in* sin = (struct sockaddr_in*)sa;
	if (NULL != sin)
	{
		ip = inet_ntoa(sin->sin_addr);
		port = sin->sin_port;
	}
	std::string strIP;
	if (NULL != ip)
	{
		strIP = ip;
	}

	pService->NewConntion(fd, ip, port, 0, NULL, true);
}


bool LibEventService::Startup()
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
	//event_config_set_flag(m_pevConfig, EVENT_BASE_FLAG_STARTUP_IOCP);
#endif

	m_pevBase = event_base_new_with_config(cfg);
	//m_pevBase = event_base_new();
	if (NULL == m_pevBase)
	{
		return false;
	}
	if (cfg) {
		event_config_free(cfg);
		cfg = NULL;
	}

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

	return NetService::Startup();

CLEAR:
	Cleanup();
	return false;
}

bool LibEventService::Stop(int nWaitMillsec)
{
	if (NULL != m_pevListener)
	{
		evconnlistener_free(m_pevListener);
		m_pevListener = NULL;
	}

	if (NULL != m_pQuitNotify)
	{
		event_active(m_pQuitNotify, 0, 0);
	}

	if ( NetService::Stop(nWaitMillsec) )
	{
		Cleanup();
		return true;
	}

	return false;
}

bool LibEventService::Listen()
{
	if (NULL == m_pevBase)
	{
		return false;
	}

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_pIoDesc->ListenPort());
	const char* ip = m_pIoDesc->ListenIP();
	if (NULL != ip && ip[0] != 0)
	{
		sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	m_pevListener = evconnlistener_new_bind(m_pevBase,
		on_service_accept_cb,
		(void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		-1,
		(struct sockaddr*)&sin, sizeof(sin));
	if (m_pevListener == NULL)
	{
		return false;
	}

	return true;
}

bool LibEventService::Connect(const char* ip, unsigned short port, int nTimout, int ud)
{
	if (NULL == ip || 0 == port) return false;

	evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
	std::string strIP = ip;
	this->Post([=]()->void{
		this->NewConntion(fd, strIP.c_str(), port, ud, NULL, false);
	});	
	return true;
}

void LibEventService::Run()
{
	if (NULL != m_pevBase)
	{
		event_base_dispatch( m_pevBase );
	}

	Process();
}

void LibEventService::NotifyQuit()
{
	event_base_loopexit(m_pevBase, NULL);
}

void LibEventService::NotifyFlush()
{
	Process();
}

void LibEventService::NotifyFrame()
{
	Process();
}

void LibEventService::Cleanup()
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

	if (NULL != m_pevListener)
	{
		evconnlistener_free(m_pevListener);
		m_pevListener = NULL;
	}

	if (NULL != m_pevBase)
	{
		event_base_free(m_pevBase);
		m_pevBase = NULL;
	}

	NetService::Cleanup();
}

NetConnection* LibEventService::InnerNewConnection(NETFD fd,
			const std::string& ip, unsigned short port, int key, 
			void* context, bool isAccept )
{
	NetConnection* pConn = NetService::InnerNewConnection(fd, ip, port, key, context, isAccept);
	if (NULL == pConn)
	{
		// 关闭socket
		evutil_closesocket(fd);
		return pConn;
	}

	LibEventConnection* pevConn = dynamic_cast<LibEventConnection*>(pConn);
	if (NULL == pevConn)
	{
		RemoveConnection(pConn->GetConnId(), EM_NET_REASON_LOGIC_ERRROR, 0);
		// 关闭socket
		evutil_closesocket(fd);
		return NULL;
	}

	struct bufferevent* pb = bufferevent_socket_new( GetEventBase(), fd, 0 );
	if (NULL == pb)
	{
		RemoveConnection(pevConn->GetConnId(), EM_NET_REASON_LOGIC_ERRROR, 0);
		// 关闭socket
		evutil_closesocket(fd);
		return NULL;
	}

	// 连接处理
	if (!isAccept)
	{
		struct sockaddr_in my_address;
		memset(&my_address, 0, sizeof(my_address));
		my_address.sin_family = AF_INET;
		my_address.sin_addr.s_addr = htonl(inet_addr( ip.c_str() ));
		my_address.sin_port = htons(port);
		if ( bufferevent_socket_connect(pb, (struct sockaddr*)&my_address, sizeof(my_address)) != 0 )
		{
			RemoveConnection(pevConn->GetConnId(), EM_NET_REASON_CONNECT_FAILED, 0);
			// 关闭socket
			evutil_closesocket(fd);
			return NULL;
		}
	}

	// 初始化
	if (!pevConn->Initilize( pb, fd, m_pFlushNotify, on_recv_cb, on_send_cb, on_event_cb) )
	{
		RemoveConnection( pevConn->GetConnId(), EM_NET_REASON_LOGIC_ERRROR, 0 );
		// 关闭socket
		evutil_closesocket(fd);
		return NULL;
	}

	return pConn;
}

void LibEventService::InnerDeneyConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context)
{
	// 关闭socket
	evutil_closesocket(fd);
}