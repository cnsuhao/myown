#include "EntHttpService.h"
#include "public/Module.h"
#include <stdio.h>
#include "EntNetManager.h"
#include "action/IHttpAction.h"
#include "FsLogger.h"
#include "SoloCharge.h"
#include "rapidxml/rapidxml.hpp"
#include "../libnet/action/HttpDataFormater.h"
#include "action/HttpData.h"
#include "server/OuterErr.h"
#include "utils/util_func.h"
#include "utils/json.h"
#include "utils/string_util.h"

DECLARE_ENTITY("FxCharge", EntHttpService, EntNetBase)
DECLARE_PROPERTY(const char*, EntHttpService, LoginVerifyUrl, GetLoginVerifyUrl, SetLoginVerifyUrl)
DECLARE_PROPERTY(int, EntHttpService, ReqReuseNum, GetReqResueNum, SetReqResueNum)
DECLARE_PROPERTY(int, EntHttpService, ReqRetries, GetReqRetries, SetReqRetries)

DECLARE_METHOD_VOID_2(EntHttpService, AddReqDefHeader, const char*, const char*)
DECLARE_METHOD_VOID_1(EntHttpService, RemoveReqDefHeader, const char*)
DECLARE_METHOD_VOID_0(EntHttpService, ClearReqDefHeaders)

DECLARE_METHOD_VOID_1(EntHttpService, AddPostUser, const char*)
DECLARE_METHOD_VOID_1(EntHttpService, RemovePostUser, const char*)
DECLARE_METHOD_VOID_0(EntHttpService, ClearPostUser)

DECLARE_METHOD_0(std::string, EntHttpService, DumpPostUser)


EntHttpService::EntHttpService()
: m_nReqReuseNum( 1 )
, m_nReqRetries( 0 )
{

}

bool EntHttpService::Init(const IVarList& args)
{
	FS_LOG_INFO("EntHttpService::Init(const IVarList& args)");
	m_pNetConnFactory = NetCreator::CreateConnectionFactory(true);
	return EntNetBase::Init(args);
}

bool EntHttpService::Shut()
{
	FS_LOG_INFO("EntHttpService::Shut()");
	if (EntNetBase::Shut())
	{
		if (NULL != m_pNetConnFactory)
		{
			NetCreator::FreeConnectionFactory(m_pNetConnFactory);
			m_pNetConnFactory = NULL;
		}
		return true;
	}
	return false;
}

bool EntHttpService::Startup()
{
	IEntity* pEntity = GetCore()->LookupEntity("SoloCharge");
	m_pMainEntity = dynamic_cast<SoloCharge*>(pEntity);
	if (NULL == m_pMainEntity)
	{
		FS_LOG_FATAL("not found main service");
		return false;
	}

	if (EntNetBase::Startup())
	{
		//HttpAction* actoin = new HttpAction();
		//IHttpRequest* request = (IHttpRequest*) actoin->Request();
		////request->SetURI("http://192.168.3.100:8080/index.php");
		////request->SetURI("http://192.168.2.10");
		////request->SetURI("http://www.bjxyfm.com/index.php/product");
		////request->SetURI("www.baidu.com");
		////request->SetURI("http://bbs.csdn.net/topics/390131855");
		////request->SetURI("http://www.codeif.com");
		//request->SetReqTimeout(30000);
		////request->AddHeader("Connection", "keep-alive");
		//m_pService->Send( NULL_NID, std::shared_ptr<INetAction>( actoin ) );

		if (!m_pService->Listen()) 
		{
			FS_LOG_ERROR("listen server failed");
			return false;
		}

		//for (int i = 0; i < 10000; ++i)
		//{
		//	HttpAction* actoin = new HttpAction();
		//	IHttpRequest* request = (IHttpRequest*)actoin->Request();
		//	request->SetURI("http://192.168.9.7:3000/v1/userService/receive/item");
		//	request->SetMethod(EM_HTTP_METHOD_POST);
		//	HttpData _http_data(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
		//	_http_data.AddValue("token", "adfasf");
		//	_http_data.AddValue("gameId", "aa");
		//	_http_data.AddValue("areaId", "ddd");
		//	_http_data.AddValue("clientIP", "aaa");

		//	std::string _http_text;
		//	_http_data.toString(_http_text);
		//	request->SetText(_http_text.c_str(), _http_text.size());
		//	m_pService->Send(NULL_NID, std::shared_ptr<INetAction>(actoin));
		//}
		g_pCore->AddExecute(this);
		return true;
	}

	FS_LOG_FATAL("start server fatal");
	return false;
}


bool EntHttpService::Stop()
{
	g_pCore->RemoveExecute(this);
	return EntNetBase::Stop();
}

void EntHttpService::SetReqResueNum(int nNum)
{
	m_nReqReuseNum = nNum;
	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->SetReqReuseNum( nNum );
		}
	}
}

int EntHttpService::GetReqResueNum()
{
	return m_nReqReuseNum;
}

void EntHttpService::SetReqRetries(int nNum)
{
	m_nReqRetries = nNum;
	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->SetReqRetries(nNum);
		}
	}
}

int EntHttpService::GetReqRetries()
{
	return m_nReqRetries;
}

void EntHttpService::AddReqDefHeader(const char* pszKey, const char* pszValue)
{
	if (StringUtil::CharIsNull( pszKey ) || StringUtil::CharIsNull( pszValue) )
	{
		return;
	}

	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->SetReqDefHeader(pszKey, pszValue);
		}
	}
}

void EntHttpService::RemoveReqDefHeader(const char* pszKey)
{
	if (StringUtil::CharIsNull(pszKey))
	{
		return;
	}

	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->RemoveReqDefHeader(pszKey);
		}
	}
}

void EntHttpService::ClearReqDefHeaders()
{
	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->ClearReqDefHeaers();
		}
	}
}

void EntHttpService::AddPostUser(const char* pszUserPost)
{
	if (NULL != pszUserPost)
	{
		m_vecAllowPostUser.push_back(pszUserPost);
	}
}

void EntHttpService::RemovePostUser(const char* pszUserPost)
{
	if (NULL != pszUserPost)
	{
		for (std::vector<std::string>::iterator itr = m_vecAllowPostUser.begin();
			itr != m_vecAllowPostUser.end(); ++itr)
		{
			std::string& strPost = *itr;
			if (strncmp(strPost.c_str(), pszUserPost, strPost.size()) == 0)
			{
				m_vecAllowPostUser.erase(itr);
				break;
			}
		}
	}
}

void EntHttpService::ClearPostUser()
{
	m_vecAllowPostUser.clear();
}

std::string EntHttpService::DumpPostUser()
{
	std::string strDump;

	for (std::vector<std::string>::iterator itr = m_vecAllowPostUser.begin();
		itr != m_vecAllowPostUser.end(); ++itr)
	{
		if (!strDump.empty())
		{
			strDump.append(" | ");
		}

		strDump.append(*itr);
	}

	return strDump;
}

void EntHttpService::OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	if (action->IsRequest())
	{
#ifdef TEST
		IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
		IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
		if (NULL == req || res == NULL)
		{
			return;
		}

		char szMsg[1024] = { 0 };
		sprintf(szMsg, "uri:%s", req->GetURI());
		WriteMessage(szMsg);
		sprintf(szMsg, "text:%s", req->GetMsgBody());
		WriteMessage(szMsg);

		const std::map<std::string, std::string>& headers = req->GetHeaders();
		sprintf(szMsg, "headers:(%d)", headers.size());
		WriteMessage(szMsg);
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
		{
			sprintf(szMsg, "\t%s:%s", itr->first.c_str(), itr->second.c_str());
			WriteMessage(szMsg);
		}

		const std::map<std::string, std::string>& cookies = req->GetCookies();
		sprintf(szMsg, "cookies:(%d)", cookies.size());
		WriteMessage(szMsg);
		for (std::map<std::string, std::string>::const_iterator itr = cookies.begin(); itr != cookies.end(); ++itr)
		{
			sprintf(szMsg, "\t%s:%s", itr->first.c_str(), itr->second.c_str());
			WriteMessage(szMsg);
		}

		const std::map<std::string, std::string>& datas = req->GetDatas();
		sprintf(szMsg, "datas:(%d)", datas.size());
		WriteMessage(szMsg);
		for (std::map<std::string, std::string>::const_iterator itr = datas.begin(); itr != datas.end(); ++itr)
		{
			sprintf(szMsg, "\t%s:%s", itr->first.c_str(), itr->second.c_str());
			WriteMessage(szMsg);
		}

		res->AddHeader("Content-Type", "text/html");
		char* szRes = "<html><head><title>http test</title></head><body>sfdxdsfsdfsdfsdf</body></html>";

		res->SetBody(szRes, strlen(szRes)).SetReason("OK").SetResult(200);
		action->SetState(EM_ACTION_REPLY);
#endif
	}
}

void EntHttpService::OnReply(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	if (action->IsResult())
	{
		IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
		IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
		if (NULL == req || res == NULL)
		{
			return;
		}
		
		const char* protocol = req->GetHeader("protocol");
		FS_LOG_DEBUG_FORMAT("on recive http reply(protocol:%s", protocol);

		time_t tNow = time(NULL);
		time_t tCreate = req->GetSpawnTime();
		time_t tExceed = tNow - tCreate;

#ifdef _DEBUG
		FS_LOG_DEBUG_FORMAT("http request cost time:%lds", (unsigned)tExceed);
#else
		if (tExceed > 1)
		{
			FS_LOG_WARN_FORMAT("http request exceed:%lds", (unsigned)tExceed);
		}
#endif

		if (strcmp(protocol, "login") == 0)
		{
			OnRspGameLogin(action);
		}
	}
#ifdef _DEBUG
	DumpAction(action);
#endif // _DEBUG
}

int EntHttpService::GetNetKey() const
{
	return EM_NET_KEY_HTTP;
}

INetIoDesc* EntHttpService::GetIoDesc()
{
	if (NULL == m_pIoDesc)
	{
		HttpIoDesc* pIoDesc = new HttpIoDesc();
		m_pIoDesc = pIoDesc;

		pIoDesc->SetIP(m_strIP.c_str())
			.SetKey(GetNetKey())
			.SetMaxAccept(m_nMaxSize)
			.SetListenPort(m_nPort)
			.SetReadBufSize(m_nReadBuf)
			.SetSendBufSize(m_nSendBuf);

		pIoDesc->SetReqRetries(m_nReqRetries)
			.SetReqReuseNum( m_nReqReuseNum );
		pIoDesc->SetReqDefHeader("Accept-Encoding", "gzip, deflate, br");
		pIoDesc->SetReqDefHeader("Accept", "*/*");
	}

	return m_pIoDesc;
}

void EntHttpService::OnInit()
{
	EntNetBase::OnInit();

	BindAction("HTTP", ACTION_BINDER(EntHttpService::OnAction, this), 0);
	BindAction("HTTP", ACTION_BINDER(EntHttpService::OnReply, this), 0);
}

void EntHttpService::OnGameLogin(const char*token, const char*name, const char*verifyStr, int64_t gameId, int64_t nAreaId, const char* pszIP, const char* pszAppid)
{
	std::shared_ptr<INetAction> actoin = std::make_shared<HttpAction>();
	IHttpRequest* request = dynamic_cast<IHttpRequest*>(actoin->Request());
	if (request == NULL)
	{
		return;
	}

	Assert(!m_strLoginVerifyUrl.empty());
	FS_LOG_DEBUG_FORMAT("on login http(name:%s pass:%s)", name, token);

	request->SetURI( m_strLoginVerifyUrl.c_str() );
	HttpData _http_data(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
	_http_data.AddValue("token", token);
	_http_data.AddValue("gameId", gameId);
	_http_data.AddValue("serverId", nAreaId);
	_http_data.AddValue("clientIp", pszIP);

	std::string _http_text;
	_http_data.toString(_http_text);
	request->SetText(_http_text.c_str(), _http_text.size());

	request->AddHeader("X-JB-Id", pszAppid == NULL ? "" : pszAppid);
	request->AddHeader("protocol", "login");
	request->AddHeader("userName", name);
	request->AddHeader("verifyStr", verifyStr);
	this->m_pService->Send(NULL_NID, actoin);
}

void EntHttpService::OnRspGameLogin(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == req || res == NULL)
	{
		FS_LOG_FATAL_FORMAT("logic data error");
		return;
	}

	const char* userName = req->GetHeader("userName");
	const char* verifyStr = req->GetHeader("verifyStr");
	FS_LOG_INFO_FORMAT("http response game login(name:%s, verify_str:%s)", userName, verifyStr);

	int nResult = MGS_ERR_VALIDATE_FAILED;
	if (res->GetResult() == EM_HTTP_CODE_OK)
	{
		const size_t MAX_HTTP_RESPONSE = 1024;
		Assert(res->GetTextLen() < MAX_HTTP_RESPONSE);
		const char * body = res->GetText();
		if (res->GetTextLen() > 0 && res->GetTextLen() < MAX_HTTP_RESPONSE - 1)
		{
			FS_LOG_DEBUG_FORMAT("login reply user:%s data:%s", userName, body);

			Json::Reader reader;
			Json::Value root;
			try
			{
				do 
				{
					bool parsedOk = reader.parse(body, body + res->GetTextLen(), root);
					if (!parsedOk)
					{
						FS_LOG_FATAL_FORMAT("login reply data parse failed(data:%s)", body);
						break;
					}

					const char* ERR_CODE_SUCCEED = "10000" ;
					const char* ERR_CODE_WHITELIST = "10003";
					const char* ERR_CODE_OPEN_TIME = "10002";

					std::string strCode = root["code"].asString();
					if (strcmp(strCode.c_str(), ERR_CODE_SUCCEED) == 0)
					{
						nResult = OUTER_RESULT_SUCCEED;
						// 限制用户名后缀
						if ( m_vecAllowPostUser.size() > 0 )
						{
							nResult = CAS_ERR_DISTRICT_TEST_NOT_BELONG;
							Json::Value& detail = root["obj"];
							std::string& strName = detail["account"].asString();
							if (strName.empty())
							{
								break;
							}

							size_t nLen = strName.size();
							for (std::vector<std::string>::iterator itr = m_vecAllowPostUser.begin();
														itr != m_vecAllowPostUser.end(); ++itr)
							{
								std::string& strPost = *itr;
								if (nLen <= strPost.size())
								{
									continue;
								}

								size_t nPos = nLen - itr->size();
								const char* pszCurPost = strName.c_str() + nPos;
								if (strncmp(strPost.c_str(), pszCurPost, strPost.size()) == 0)
								{
									nResult = OUTER_RESULT_SUCCEED;
									break;
								}
							}
						}
						break;
					}

					if (strcmp(strCode.c_str(), ERR_CODE_WHITELIST) == 0)
					{
						nResult = CAS_ERR_LOGIN_LIMITED;
						FS_LOG_INFO_FORMAT("login reply white limit(code:%s name:%s)", strCode.c_str(), userName);
						break;
					}

					if (strcmp(strCode.c_str(), ERR_CODE_WHITELIST) == 0)
					{
						nResult = CAS_ERR_SERVER_FORBID;
						FS_LOG_INFO_FORMAT("login reply server not open(code:%s name:%s)", strCode.c_str(), userName);
						break;
					}

					FS_LOG_INFO_FORMAT("login reply code status failed(code:%s name:%s )", strCode.c_str(), userName);
				} while (false);
			}
			catch (...)
			{
				FS_LOG_FATAL_FORMAT("login reply handle exception (name:%s verify_str:%s)", userName, verifyStr);
			}
		}
		else
		{
			FS_LOG_FATAL_FORMAT("parse login data error(size:%d,name:%s).", res->GetTextLen(), userName);
		}
	}
	else
	{
		const char* pReason = res->GetReason();
		FS_LOG_FATAL_FORMAT("login verify failed(http code:%d reason:%s)",
			res->GetResult(), NULL == pReason ? "" : pReason);

		nResult = CAS_ERR_CONNECT_TIMEOUT;
	}

	m_pMainEntity->OnLoginResult(userName, nResult, verifyStr);

}

void EntHttpService::DumpAction(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
	assert(req != NULL);
	if (NULL == req)
	{
		return;
	}

	FS_LOG_INFO("HTTP DUMP:::BEGIN");
	FS_LOG_INFO_FORMAT("ACTION:::state:%d", action->GetState());
	FS_LOG_INFO("REQ HEAD:::BEGIN");
	const std::map<std::string, std::string>& header = req->GetHeaders();
	for (std::map<std::string, std::string>::const_iterator itr = header.begin(); itr != header.end(); ++itr)
	{
		FS_LOG_INFO_FORMAT("%s:%s", itr->first.c_str(), itr->second.c_str());
	}
	FS_LOG_INFO("REQ HEAD:::END");
	FS_LOG_INFO_FORMAT("REQ INFO:::url:%s query:%s param:%s method:%d", req->GetURI(), req->GetQuery(), req->GetText(), req->GetMethod());
	if (NULL != res)
	{
		FS_LOG_INFO("RES HEAD:::BEGIN");
		const std::map<std::string, std::string>& header = res->GetHeaders();
		for (std::map<std::string, std::string>::const_iterator itr = header.begin(); itr != header.end(); ++itr)
		{
			FS_LOG_INFO_FORMAT("%s:%s", itr->first.c_str(), itr->second.c_str());
		}
		FS_LOG_INFO("RES HEAD:::END");
		FS_LOG_INFO_FORMAT("RES INFO:::result:%d len:%d data:%s", res->GetResult(), res->GetTextLen(), res->GetText());
	}
	FS_LOG_INFO("HTTP DUMP:::END");
}