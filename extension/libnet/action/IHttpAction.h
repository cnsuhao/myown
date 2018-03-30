//--------------------------------------------------------------------
// 文件名:		IHttpAction.h
// 内  容:		http动作定义
// 说  明:		
// 创建日期:		2016年5月24日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __IHTTP_ACTION_H__
#define __IHTTP_ACTION_H__

#include "INetAction.h"
#include "public/FastStr.h"
#include <map>
#include <string>
#include "HttpData.h"

enum EM_HTTP_CODE
{
	EM_HTTP_CODE_OK			=	200,	/**< request completed ok */
	EM_HTTP_CODE_NOCONTENT	=	204,	/**< request does not have content */
	EM_HTTP_CODE_MOVEPERM	=	301,	/**< the uri moved permanently */
	EM_HTTP_CODE_MOVETEMP	=	302,	/**< the uri moved temporarily */
	EM_HTTP_CODE_NOTMODIFIED=	304,	/**< page was not modified from last */
	EM_HTTP_CODE_BADREQUEST	=	400,	/**< invalid http request was made */
	EM_HTTP_CODE_NOTFOUND	=	404,	/**< could not find content for uri */
	EM_HTTP_CODE_BADMETHOD	=	405, 	/**< method not allowed for this uri */
	EM_HTTP_CODE_ENTITYTOOLARGE =	413,	/**<  */
	EM_HTTP_CODE_EXPECTATIONFAILED=	417,	/**< we can't handle this expectation */
	EM_HTTP_CODE_INTERNAL        =  500,     /**< internal error */
	EM_HTTP_CODE_NOTIMPLEMENTED  =  501,     /**< not implemented */
	EM_HTTP_CODE_SERVUNAVAIL	=	503,	/**< the server is not available */


	EM_HTTP_CODE_TIMEOUT	=	-1,	/* 超时 */
};

enum EM_HTTP_METHOD
{
	EM_HTTP_METHOD_GET = 1 << 0,
	EM_HTTP_METHOD_POST = 1 << 1,
	EM_HTTP_METHOD_HEAD = 1 << 2,
	EM_HTTP_METHOD_PUT = 1 << 2,
	EM_HTTP_METHOD_DELETE = 1 << 4,
	EM_HTTP_METHOD_OPTIONS = 1 << 5,
	EM_HTTP_METHOD_TRACE = 1 << 6,
	EM_HTTP_METHOD_CONNECT = 1 << 7,
	EM_HTTP_METHOD_PATCH = 1 << 8,

	EM_HTTP_METHOD_UNKOWN = 1 << 31,
};

class IHttpRequest : public INetRequest
{
protected:
	unsigned m_nTimeout;
public:
	IHttpRequest() : m_nTimeout(30000)
	{
	}

	virtual void SetReqTimeout(unsigned timeout)
	{
		m_nTimeout = timeout;
	}

	virtual unsigned GetTimeOut() const
	{
		return m_nTimeout;
	}

	~IHttpRequest() = 0;

	virtual IHttpRequest& SetMethod(EM_HTTP_METHOD method) = 0;
	virtual IHttpRequest& SetURI(const char* uri) = 0;

	virtual EM_HTTP_METHOD GetMethod() const = 0;
	virtual const char* GetURI() const = 0;
	virtual const char* GetPath() const = 0;
	virtual const char* GetQuery() const = 0;
	virtual const char* GetHost() const = 0;
	virtual unsigned short GetPort() const = 0;

	virtual IHttpRequest& SetText(const char* pText, size_t nLen) = 0;
	virtual IHttpRequest& AppendText(const char* pText, size_t nLen) = 0;
	virtual const char* GetText() const = 0;
	virtual size_t GetTextLen() const = 0;

	virtual const std::map<std::string, std::string>& GetHeaders() const = 0;

	virtual const char* GetHeader(const char* key) const = 0;
	virtual IHttpRequest&  AddHeader(const char* key, const char* value) = 0;
	virtual void RemoveHeader(const char* key) = 0;
	virtual void RemoveAllHeaders() = 0;

	virtual void AttachHttpData(std::shared_ptr<HttpData>& data) = 0;
	virtual std::shared_ptr<HttpData>& GetAttachData() = 0;
	virtual const std::shared_ptr<HttpData>& GetAttachData() const = 0;

	virtual bool IsHttps() const = 0;

	virtual time_t GetSpawnTime() const = 0;
};
inline IHttpRequest::~IHttpRequest(){};

class IHttpResponse : public INetResponse
{
protected:
	IHttpRequest* m_pRequest;
	int m_nResult;
public:
	IHttpResponse() : m_pRequest(NULL), m_nResult(-1)
	{
	}

	void SetResult(int nCode)
	{
		m_nResult = nCode;
	}

	int GetResult() const
	{
		return m_nResult;
	}

	virtual IHttpResponse& SetText(const char* pText, size_t nLen) = 0;
	virtual IHttpResponse& AppendText(const char* pText, size_t nLen) = 0;
	virtual const char* GetText() const = 0;
	virtual size_t GetTextLen() const = 0;

	virtual IHttpResponse& SetReason(const char* szReason) = 0;
	virtual const char* GetReason() const = 0;

	virtual IHttpResponse& SetRequest(IHttpRequest* req)
	{
		m_pRequest = req;
		return *this;
	}
	virtual IHttpRequest* GetRequest() const
	{
		return m_pRequest;
	}

	virtual const std::map<std::string, std::string>& GetHeaders() const = 0;
	virtual const char* GetHeader(const char* key) const = 0;
	virtual IHttpResponse&  AddHeader(const char* key, const char* value) = 0;
	virtual void RemoveHeader(const char* key) = 0; 
	virtual void RemoveAllHeaders() = 0;
};



class HttpRequest : public IHttpRequest
{
	typedef TFastStr<char, 16> UrlString;
protected:
	EM_HTTP_METHOD		m_reqMethod;
	UrlString	m_strUri;
	std::map<std::string, std::string> m_headers;

	std::string m_strText;

	UrlString m_strPath;
	UrlString m_strQuery;
	UrlString m_strHost;
	unsigned short m_nPort;

	std::shared_ptr<HttpData> m_ptrAttachData;

	bool m_bIsHttps;
	time_t m_nCreateTime;		// 创建时间
public:

	HttpRequest()
		: m_reqMethod(EM_HTTP_METHOD_GET)
		, m_nPort(0)
		, m_bIsHttps(false)
		, m_nCreateTime(time(NULL))
	{
	}

	virtual bool IsHttps() const
	{
		return m_bIsHttps;
	}

	virtual void SetHttps(bool bHttps)
	{
		m_bIsHttps = bHttps;
	}

	virtual IHttpRequest& SetMethod(EM_HTTP_METHOD method)
	{
		m_reqMethod = method;
		return *this;
	}
	virtual IHttpRequest& SetURI(const char* uri)
	{
		m_strUri = uri;

		m_strHost = "localhost";
		m_nPort = 80;
		m_strPath.clear();
		m_strQuery.clear();

		if (m_strUri.size() > 0)
		{
			size_t nProtocolEndPos = m_strUri.find("://");
			if (nProtocolEndPos == std::string::npos)
			{
				nProtocolEndPos = 0;
			}
			else
			{
				nProtocolEndPos += 3;
			}

			if (strncmp("https://", m_strUri.c_str(), 8) == 0)
			{
				m_nPort = 443;
				m_bIsHttps = true;
			}

			do
			{
				size_t nHostBeginPos = nProtocolEndPos;
				size_t nHostEndPos = m_strUri.find(":", nHostBeginPos);
				if (nHostEndPos == std::string::npos)
				{
					nHostEndPos = m_strUri.find("/", nHostBeginPos);
					if (nHostEndPos == std::string::npos)
					{
						nHostEndPos = m_strUri.size();
					}
				}
				else
				{
					size_t nPortBeginPos = nHostEndPos + 1;
					size_t nPortEndPos = m_strUri.find("/", nPortBeginPos);
					
					if (nPortEndPos == std::string::npos)
					{
						nPortEndPos = m_strUri.size();
					}
					UrlString strPort = m_strUri.substr( nPortBeginPos, nPortEndPos - nPortBeginPos );
					if (strPort.size() > 0)
					{
						m_nPort = atoi(strPort.c_str());
					}
				}

				if (nHostEndPos - nHostBeginPos > 0)
				{
					m_strHost = m_strUri.substr(nHostBeginPos, nHostEndPos - nHostBeginPos);
				}

				if (nHostEndPos >= m_strUri.size())
				{
					break;
				}


				size_t nPathBeginPos = m_strUri.find("/", nHostEndPos);
				if (nPathBeginPos == std::string::npos)
				{
					m_strPath = "/";
				}
				else
				{
					size_t nPathEndPos = m_strUri.find("?", nPathBeginPos);
					if (nPathEndPos == std::string::npos)
					{
						nPathEndPos = m_strUri.size();
					}
					else
					{
						m_strQuery = m_strUri.substr(nPathEndPos + 1, m_strUri.size() - nPathEndPos);
					}
					m_strPath = m_strUri.substr(nPathBeginPos, nPathEndPos - nPathBeginPos);
				}
			} while (false);
		}

		return *this;
	}

	virtual EM_HTTP_METHOD GetMethod() const
	{
		return m_reqMethod;
	}

	virtual const char* GetURI() const
	{
		return m_strUri.c_str();
	}

	virtual const char* GetPath() const
	{
		return m_strPath.c_str();
	}

	virtual const char* GetQuery() const
	{
		return m_strQuery.c_str();
	}

	virtual const char* GetHost() const
	{
		return m_strHost.c_str();
	}
	virtual unsigned short GetPort() const
	{
		return m_nPort;
	}

	virtual const std::map<std::string, std::string>& GetHeaders() const
	{
		return m_headers;
	}

	virtual const char* GetHeader(const char* key) const
	{
		std::map<std::string, std::string>::const_iterator itr = m_headers.find(key);
		if (m_headers.end() != itr)
		{
			return itr->second.c_str();
		}

		return "";
	}

	virtual IHttpRequest&  AddHeader(const char* key, const char* value)
	{
		m_headers[key] = value;
		return *this;
	}

	virtual void RemoveHeader(const char* key)
	{
		std::map<std::string, std::string>::iterator itr = m_headers.find(key);
		if (m_headers.end() != itr)
		{
			m_headers.erase(itr);
		}
	}

	virtual void RemoveAllHeaders()
	{
		m_headers.clear();
	}

	virtual IHttpRequest& SetText(const char* pText, size_t nLen)
	{
		m_strText.clear();
		m_strText.assign(pText, pText + nLen);
		return *this;
	}

	virtual IHttpRequest& AppendText(const char* pText, size_t nLen)
	{
		m_strText.insert(m_strText.end(), pText, pText + nLen);
		return *this;
	}

	virtual const char* GetText() const
	{
		return m_strText.c_str();
	}

	virtual size_t GetTextLen() const
	{
		return m_strText.size();
	}

	virtual void AttachHttpData(std::shared_ptr<HttpData>& data)
	{
		m_ptrAttachData = data;
	}
	virtual std::shared_ptr<HttpData>& GetAttachData()
	{
		return m_ptrAttachData;
	}
	virtual const std::shared_ptr<HttpData>& GetAttachData() const
	{
		return m_ptrAttachData;
	}

	virtual time_t GetSpawnTime() const
	{
		return m_nCreateTime;
	}
};

class HttpResponse : public IHttpResponse
{
protected:
	std::map<std::string, std::string> m_headers;
	std::string m_strText;
	TFastStr<char, 16> m_strReason;
public:
	virtual IHttpResponse& SetText(const char* pText, size_t nLen)
	{
		m_strText.clear();
		m_strText.assign(pText, pText + nLen);
		return *this;
	}

	virtual IHttpResponse& AppendText(const char* pText, size_t nLen)
	{
		m_strText.insert( m_strText.end(), pText, pText + nLen);
		return *this;
	}

	virtual const char* GetText() const
	{
		return m_strText.c_str();
	}

	virtual size_t GetTextLen() const
	{
		return m_strText.size();
	}

	virtual IHttpResponse& SetReason(const char* szReason)
	{
		m_strReason = szReason;
		return *this;
	}
	virtual const char* GetReason() const
	{
		return m_strReason.c_str();
	}

	virtual const std::map<std::string, std::string>& GetHeaders() const
	{
		return m_headers;
	}

	virtual const char* GetHeader(const char* key) const
	{
		std::map<std::string, std::string>::const_iterator itr = m_headers.find(key);
		if (m_headers.end() != itr)
		{
			return itr->second.c_str();
		}

		return "";
	}

	virtual IHttpResponse&  AddHeader(const char* key, const char* value)
	{
		m_headers[key] = value;
		return *this;
	}

	virtual void RemoveHeader(const char* key)
	{
		std::map<std::string, std::string>::iterator itr = m_headers.find(key);
		if (m_headers.end() != itr)
		{
			m_headers.erase(itr);
		}
	}

	virtual void RemoveAllHeaders()
	{
		m_headers.clear();
	}
};

typedef INetAction IHttpAction;

class HttpAction : public NetReqAction<0, HttpRequest, HttpResponse>
{
public:
	virtual const char* ActionKey() const
	{
		return	"HTTP";
	}
};
#endif // END __IHTTP_ACTION_H__