#include "LibEventHttpConnection.h"
#include "event2/event.h"
#include "base/INetService.h"
#include "event2/keyvalq_struct.h"
#include "event2/bufferevent.h"

#define HTTP_CONNECT_TIME_OUT	90
void on_http_response(struct evhttp_request *req, void *ctx);

LibEventHttpConnection::LibEventHttpConnection() : NetConnection(1024, 1024)
	, m_httpConnection(NULL)
	, m_pNotify(NULL)
	, m_bIsSSL( false )
{
}

bool LibEventHttpConnection::Initilize(struct evhttp_connection* phttp, struct event* pNotify)
{
	if (NULL != phttp)
	{
		evhttp_connection_set_timeout(phttp, HTTP_CONNECT_TIME_OUT);
	}
	m_httpConnection = phttp;
	m_pNotify = pNotify;
	return true;
}

void LibEventHttpConnection::OnRequest(request_ptr request, std::shared_ptr<INetAction>& action)
{
	assert(IsAccept());
	if (IsAccept())
	{
		// 设置当前请求( 一次只能处理一个 )
		// 不主动关闭, 由底层管理
		//evhttp_connection_set_timeout(conn, HTTP_SERVER_TIME_OUT);
		INetIoHandler* pHandler = m_pService->GetHandler();
		if (NULL != pHandler)
		{
			m_requests[request] = action;
			pHandler->OnAction(this, action);
			//http->SetState(EM_ACTION_RESULT);
			//pConn->Reply(http);
		}
	}
}

void LibEventHttpConnection::OnResponse( request_ptr req )
{
	if ( m_pService == NULL || NULL == m_httpConnection )
	{
		printf("LibEventHttpConnection::OnResponse(connection already release)");
		return;
	}

	// 设定5秒后无操作就关闭吧
	evhttp_connection_set_timeout(m_httpConnection, 5);

	if (NULL == req)
	{
		return ;
	}

	// 查找当前请求数据
	RequestList::iterator itr = m_requests.find(req);
	if (itr == m_requests.end())
	{
		// 可能超时已被删除
		return;
	}

	std::shared_ptr<INetAction>& action = itr->second;

	if (NULL == action.get())
	{
		m_requests.erase(itr);
		return;
	}

	IHttpRequest* request = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* response = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == request || NULL == response)
	{
		// 数据出错了，什么也不响应
		m_requests.erase(itr);
		return;
	}

	int nResCode = 0;
	nResCode = evhttp_request_get_response_code(req);
	if (nResCode == HTTP_MOVETEMP)
	{
		// TODO: 转时不支持redirect
		//struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
		//const char *new_location = evhttp_find_header(headers, "Location");
		//if (NULL != new_location)
		//{
		//	request->RemoveAllHeaders();
		//	for (struct evkeyval *header = headers->tqh_first; NULL != header;
		//		header = header->next.tqe_next) {
		//		request->AddHeader(header->key, header->value);
		//	}
		//	// TODO:???
		//	//request->SetURI(new_location);
		//	//pService->StartRequest(action);
		//}
	}


	struct evkeyvalq *headers = evhttp_request_get_input_headers(req);
	for (struct evkeyval *header = headers->tqh_first; NULL != header;
		header = header->next.tqe_next) {
		response->AddHeader(header->key, header->value);
	}

	struct evbuffer* buf = evhttp_request_get_input_buffer(req);
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[1024] = { 0 };
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
		{
			response->AppendText(cbuf, n);
		}
	}

	// action由逻辑层管理
	action->SetState(EM_ACTION_RESULT);
	response->SetResult(nResCode);
	INetIoHandler* pHandler = m_pService->GetHandler();
	if (NULL != pHandler)
	{
		pHandler->OnReply(this, action);
	}

	// 一定要最后删除 不然aciton会引用到空
	m_requests.erase(itr);
}

bool LibEventHttpConnection::Send(std::shared_ptr<INetAction>& action)
{
	assert(NULL != m_httpConnection);
	assert(!IsAccept());
	assert(action->IsRequest());
	if (IsAccept())
	{
		return false;
	}

	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	assert(NULL != req);
	if (NULL == req)
	{
		return false;
	}

	if (NULL == m_httpConnection)
	{
		return false;
	}
	
	// 每次重新设置http超时
	unsigned int nTimeOut = req->GetTimeOut() / 1000;
	if (nTimeOut > 0)
	{
		evhttp_connection_set_timeout(m_httpConnection, nTimeOut);
	}

	// 成功创建请求上下文 添加到等待回应列表中去
	struct evhttp_request* evreq = evhttp_request_new( on_http_response, this );
	m_requests[evreq] = action;

	struct evkeyvalq* header = evhttp_request_get_output_headers(evreq);
	if (NULL != header)
	{
		evhttp_clear_headers(header);
		const std::map<std::string, std::string>& headers = req->GetHeaders();
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin();
			itr != headers.end();
			++itr)
		{
			evhttp_add_header(header, itr->first.c_str(), itr->second.c_str());
		}
	}

	const char *query = req->GetQuery();
	TFastStr<char, 128> path = req->GetPath();;
	if (path.empty())
	{
		path.append("/");
	}
	if (NULL != query && query[0] != 0)
	{
		path.append('?');
		path.append(query);
	}
	if (req->GetMethod() == EM_HTTP_METHOD_POST)
	{
		struct evbuffer* buf = NULL;
		buf = evhttp_request_get_output_buffer(evreq);
		if ( NULL != buf)
		{
			if ( req->GetTextLen() > 0 )
			{
				evbuffer_add( buf, req->GetText(), req->GetTextLen() );
			}
		}
		if (evhttp_make_request(m_httpConnection, evreq, EVHTTP_REQ_POST, path.c_str()) == 0)
		{
			if (NULL != m_pNotify)
			{
				event_active(m_pNotify, 0, 0);
			}
			return true;
		}
	}
	else if (req->GetMethod() == EM_HTTP_METHOD_GET)
	{
		if ( req->GetTextLen() > 0 )
		{
			if (NULL == query || query[0] == 0)
			{
				path.append('?');
			}
			else
			{
				path.append('&');
			}
			path.append( req->GetText() );
		}

		if (0 == evhttp_make_request(m_httpConnection, evreq, EVHTTP_REQ_GET, path.c_str()))
		{
			if (NULL != m_pNotify)
			{
				event_active(m_pNotify, 0, 0);
			}
			return true;
		}
	}

	// 失败了删除数据
	m_requests.erase(evreq);
	evhttp_request_free(evreq);
	return false;
}

bool LibEventHttpConnection::Reply(std::shared_ptr<INetAction>& action)
{
	assert(NULL != m_httpConnection);
	assert(!action->IsRequest());
	assert(IsAccept());
	if (NULL == m_httpConnection)
	{
		return false;
	}

	if ( !IsAccept() )
	{
		return false;
	}

	// 不是请求直接忽略
	request_ptr req = NULL;
	for (RequestList::iterator itr = m_requests.begin(); itr != m_requests.end(); ++itr)
	{
		if ( itr->second.get() == action.get() )
		{
			req = itr->first;
			m_requests.erase(req);
			break;
		}
	}
	if (NULL == req)
	{
		return false;
	}

	IHttpResponse* response = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == response)
	{
		TFastStr<char, 32> strReason = "internal error";
		int nErrCode = HTTP_INTERNAL;
		struct evbuffer* buf = evbuffer_new();
		evbuffer_add(buf, strReason.c_str(), strReason.size());
		evhttp_send_reply(req, nErrCode, strReason.c_str(), buf);
		evbuffer_free(buf);
		return false;
	}

	const std::map<std::string, std::string> headers = response->GetHeaders();
	evkeyvalq* hq = evhttp_request_get_output_headers(req);
	if (NULL != hq)
	{
		evhttp_clear_headers(hq);
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
		{
			evhttp_add_header(hq, itr->first.c_str(), itr->second.c_str());
		}
	}

	size_t nLen = response->GetTextLen();
	evbuffer* buf = evbuffer_new();
	if (nLen == 0)
	{
		const char* szReason = response->GetReason();
		if (NULL != szReason)
		{
			evbuffer_add(buf, szReason, strlen(szReason));
		}
	}
	else
	{
		evbuffer_add(buf, response->GetText(), nLen);
	}

	evhttp_send_reply(req, response->GetResult(), response->GetReason(), buf);
	evbuffer_free(buf);
	return true;
}

bool LibEventHttpConnection::Close(EMNetReason reason /* = EM_NET_REASON_PEER_DISCONNECT */)
{
	// 关闭连接
	if ( m_eState == EM_CONN_STATE_CONNECTED || m_eState == EM_CONN_STATE_CONNECTING )
	{
		m_bIsPassived = false;
		m_eState = EM_CONN_STATE_CLOSING;

		// 主动关闭，将所有请求当作超时处理
		if (NULL != m_pService)
		{
			INetIoHandler* pHandler = m_pService->GetHandler();
			if (NULL != pHandler)
			{
				for (RequestList::iterator itr = m_requests.begin();
					itr != m_requests.end();
					++itr)
				{
					itr->second->SetState(EM_ACTION_RESULT);
					IHttpResponse* pRes = dynamic_cast<IHttpResponse*>(itr->second->Response());
					if (NULL != pRes)
					{
						pRes->SetReason("connection failed.");
						pRes->SetResult(EM_HTTP_CODE_TIMEOUT);
					}
					pHandler->OnTimeout(this, itr->second);
				}

				m_requests.clear();
			}
		}
	}

	return true;
}

void LibEventHttpConnection::OnClosed(EMNetReason reason, int nErrCode)
{
	// 先调用父类改变状态
	NetConnection::OnClosed(reason, nErrCode);

	// 判断未超时的 重新发送
	if (NULL != m_pService)
	{
		time_t nNow = time(NULL);
		INetIoHandler* pHandler = m_pService->GetHandler();
		for (RequestList::iterator itr = m_requests.begin();
			itr != m_requests.end();
			++itr)
		{
			IHttpRequest* request = dynamic_cast<IHttpRequest*>(itr->second->Request());
			if (NULL != request)
			{
				// * 不能用这种方式处理重发 libevent http关闭事件错误时会在response前发生
				//time_t tNow = request->GetSpawnTime();
				//time_t diff = (nNow - tNow + 1) * 1000;
				//time_t timeout = (time_t)request->GetTimeOut();
				//if (diff < timeout)
				//{
				//	// 未超时的继续发送
				//	request->SetReqTimeout((unsigned)(timeout - diff));
				//	m_pService->Send(NULL_NID, itr->second);
				//}
				//else
				if ( NULL != pHandler )
				{
					itr->second->SetState(EM_ACTION_RESULT);
					IHttpResponse* pRes = dynamic_cast<IHttpResponse*>(itr->second->Response());
					if (NULL != pRes)
					{
						pRes->SetReason("connection failed.");
						pRes->SetResult(EM_HTTP_CODE_TIMEOUT);
					}
					pHandler->OnTimeout(this, itr->second);
				}
			}
		}
	}

	if (NULL != m_httpConnection && !IsAccept())
	{
		//struct bufferevent* bev = evhttp_connection_get_bufferevent(m_httpConnection);
		//if (NULL != bev)
		//{
		//	bufferevent_free(bev);
		//}
		evhttp_connection_free(m_httpConnection);
	}
	m_httpConnection = NULL;
	// 删除所有请求
	m_requests.clear();
}
