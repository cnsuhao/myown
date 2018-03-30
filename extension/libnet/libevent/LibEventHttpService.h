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
#include "base/NetService.h"
#include <event2/event.h>
#include <thread>
#include "utils/lockutil.h"
#include "action/IHttpAction.h"
#include <map>
#include <list>
#if EVENT__NUMERIC_VERSION >= 0x02010600
#define NET_SLL_SUPPORT
#endif
#ifdef NET_SLL_SUPPORT
#define EVENT__HAVE_OPENSSL
#include "openssl/my_ssl.h"
#endif

class LibEventHttpConnection;
class LibEventHttpService : public NetService
{
protected:
	struct event_base* m_pevBase;
	struct evdns_base* m_pevDNS;
	struct evhttp* m_evHttp;
	struct evhttp_bound_socket* m_evhttpSocket;

	struct event*			m_pFlushNotify;
	struct event*			m_pQuitNotify;
	struct event*			m_pFrameNotify;

	// 当前未决的请求列表(conn)
	std::map<struct evhttp_connection*, LibEventHttpConnection*>	m_mapRequests;
	// 当前响应的列表(conn)
	std::map<struct evhttp_connection*, LibEventHttpConnection*>	m_mpaReplies;

	HttpIoDesc* m_pHttpDesc;

#ifdef NET_SLL_SUPPORT
	// SSL支持
	MySSL* m_pSSL;
#endif
public:
	LibEventHttpService(INetIoDesc* desc);
	virtual bool Startup();
	virtual bool Stop( int nWaitMillsec );
	virtual bool Listen();

	struct event_base* GetEventBase() const
	{
		return m_pevBase;
	}

	struct evdns_base* GetDnsBase() const
	{
		return m_pevDNS;
	};

#ifdef NET_SLL_SUPPORT
	MySSL* GetSSL()
	{
		return m_pSSL;
	}
#endif

	virtual bool Send(const NID& id, const char* szMsg, size_t nLen);
	virtual bool Send(const NID& id, std::shared_ptr<INetAction>& action);

public:
	virtual void RemoveConnection(const NID& nid, EMNetReason reason, int nErrCode) override;
protected:
	virtual void Run();
	void Cleanup();

	void FillReqDefHeaer( IHttpRequest* req ) const;

	// 活动请求连接池管理
	void AddReqConnection( struct evhttp_connection* evcon, LibEventHttpConnection* pConn );
	void RemoveReqConnection(struct evhttp_connection* evcon);
	LibEventHttpConnection* GetReqConnection( const std::string& strHost, unsigned short port );
	void ClearReqConnection();
	LibEventHttpConnection* MakeReqConnection(const std::string& strHost, unsigned short port, bool bIsHppts );

	virtual NetConnection* InnerNewConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context, bool isAccept);
	virtual NetConnection* InnerRemoveConnection(const NID& nid);

	// 活动的响应连接管理
	LibEventHttpConnection* MakeRspConnection( struct evhttp_connection* evconn );
	void RemoveRspConnection( struct evhttp_connection* evconn );
protected:

	// 服务线程调用
	void NotifyQuit();
	void NotifyFlush();
	void NotifyFrame();

	static void on_quit_service_cb(evutil_socket_t, short, void* p);
	static void on_flush_service_cb(evutil_socket_t, short, void* p);
	static void on_frame_service_cb(evutil_socket_t, short, void* p);

	friend void on_http_request(struct evhttp_request *req, void *ctx);
};

#endif // END __NET_LIB_EVENT_HTTP_SERVICE_H__