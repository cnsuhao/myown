//--------------------------------------------------------------------
// 文件名:		LibEventHttpService.h
// 内  容:		libevent http网络服务实现
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_LIB_EVENT_HTTP_SERVICE_H__
#define __NET_LIB_EVENT_HTTP_SERVICE_H__
#include "NetService.h"
#include <event2/event.h>
#include <thread>
#include "lockutil.h"
#include "IHttpAction.h"
#include <map>

class LibEventHttpService : public NetService
{
protected:
	struct event_config* m_pevConfig;
	struct event_base* m_pevBase;
	struct evhttp* m_evHttp;
	struct evhttp_bound_socket* m_evhttpSocket;

	struct event_base*	m_pevClient;
	struct event*		m_pev_msg_notify;

	std::thread m_worker;
	CLockUtil m_locker;

	typedef struct evhttp_request http_request;
	std::map<http_request*, IHttpAction*> m_requests;

	void PushRequest( http_request* req, IHttpAction* action )
	{
		CAutoLock guard(m_locker);
		std::map<http_request*, IHttpAction*>::iterator itr = m_requests.find(req);
		if (itr != m_requests.end())
		{
			if (action == itr->second)
			{
				return;
			}

			delete itr->second;
		}
		m_requests[req] = action;
	}

	IHttpAction* PopRequest(struct evhttp_request* req)
	{
		CAutoLock guard(m_locker);
		IHttpAction* action = NULL;
		std::map<http_request*, IHttpAction*>::iterator itr = m_requests.find(req);
		if (itr != m_requests.end())
		{
			action = itr->second;
			m_requests.erase(itr);
		}

		return action;
	}

	friend bool StartHttpRequest(LibEventHttpService* service, IHttpAction* action);
	friend void OnLibEventHttpResponse(struct evhttp_request *req, void *ctx);
public:
	LibEventHttpService(INetIoDesc* desc)
		: NetService( desc )
		, m_pevConfig( NULL )
		, m_pevBase( NULL )
		, m_evHttp(NULL)
		, m_evhttpSocket( NULL )
		, m_pevClient( NULL )
	{
	}

	virtual bool Startup();
	virtual bool Stop();

	struct event_base* GetEventBase()
	{
		return m_pevBase;
	}

	struct event_base* GetClientBase()
	{
		return m_pevClient;
	}

	virtual bool StartHttpRequest( IHttpAction* action );
};

#endif // END __NET_LIB_EVENT_HTTP_SERVICE_H__