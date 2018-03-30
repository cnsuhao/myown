//--------------------------------------------------------------------
// 文件名:		LibEventHttpConnection.h
// 内  容:		libevent http网络连接定义
// 说  明:		
// 创建日期:		2016年5月26日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_LIBEVENT_HTTP_CONNECTION_H__
#define __NET_LIBEVENT_HTTP_CONNECTION_H__

#include "base/NetConnection.h"
#include "public/FastStr.h"
#include "action/IHttpAction.h"
#include "event2/http.h"
#include "event2/buffer.h"

class LibEventHttpConnection : public NetConnection
{
public:
	typedef struct evhttp_request* request_ptr;
	typedef std::map<request_ptr, std::shared_ptr<INetAction>> RequestList;
protected:

	struct evhttp_connection*	m_httpConnection;
	struct event*				m_pNotify;

	// 未决的请求
	RequestList					m_requests;	
	bool m_bIsSSL;
public:
	LibEventHttpConnection();

	bool Initilize(struct evhttp_connection* phttp, struct event* pNotify);

	// 服务端收到请求
	void OnRequest(request_ptr request, std::shared_ptr<INetAction>& action);
	// 客户端收到服务器回应
	void OnResponse(request_ptr req);

	// 获取到连接对象
	struct evhttp_connection* GetHttpContext() const
	{
		return m_httpConnection;
	}

	void SetSSL(bool ssl)
	{
		m_bIsSSL;
	}

	bool IsSSL() const
	{
		return m_bIsSSL;
	}

	virtual bool Send(std::shared_ptr<INetAction>& action);
	virtual bool Reply(std::shared_ptr<INetAction>& action);

	virtual bool Close(EMNetReason reason /* = EM_NET_REASON_PEER_DISCONNECT */);
	virtual void OnClosed(EMNetReason reason, int nErrCode);

	size_t GetReqLength()
	{
		return m_requests.size();
	}
protected:
	virtual void PostSend(){ assert(false); };
private:
	void SetDefaultHeader(IHttpRequest* req,
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
};

#endif // END __NET_LIBEVENT_HTTP_CONNECTION_H__