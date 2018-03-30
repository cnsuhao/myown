//--------------------------------------------------------------------
// 文件名:		LibEventService.h
// 内  容:		libevent网络服务实现
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_LIB_EVENT_SERVICE_H__
#define __NET_LIB_EVENT_SERVICE_H__
#include "base/NetService.h"
#include <event2/event.h>
#include <thread>

class LibEventService : public NetService
{
protected:
	struct event_base*		m_pevBase;
	struct evconnlistener*	m_pevListener;

	struct event*			m_pFlushNotify;
	struct event*			m_pQuitNotify;
	struct event*			m_pFrameNotify;

public:
	LibEventService( INetIoDesc* desc )
		: NetService( desc )
		, m_pevBase( NULL )
		, m_pevListener( NULL )
		, m_pFlushNotify( NULL )
		, m_pQuitNotify( NULL )
		, m_pFrameNotify( NULL )
	{
	}

	virtual bool Startup();
	virtual bool Stop(int nWaitMillsec);

	virtual bool Listen();
	virtual bool Connect(const char* ip, unsigned short port, int nTimout, int ud);

	struct event_base* GetEventBase()
	{
		return m_pevBase;
	}

protected:
	virtual void Run();
	virtual void Cleanup();
private:
	// 服务线程调用
	void NotifyQuit();
	void NotifyFlush();
	void NotifyFrame();

	static void on_quit_service_cb(evutil_socket_t, short, void* p);
	static void on_flush_service_cb(evutil_socket_t, short, void* p);
	static void on_frame_service_cb(evutil_socket_t, short, void* p);
	static void on_service_accept_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int nSockLen, void *ud);
	static void on_event_cb(struct bufferevent *bev, short error, void *ctx);
	static void on_recv_cb(struct bufferevent *bev, void *ctx);
	static void on_send_cb(struct bufferevent *bev, void *ctx);

protected:
	virtual NetConnection* InnerNewConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context, bool isAccept);
	virtual void InnerDeneyConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context);
};

#endif // END __NET_LIB_EVENT_SERVICE_H__