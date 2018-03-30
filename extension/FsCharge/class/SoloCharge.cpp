#include "EntNetManager.h"
#include "SoloCharge.h"
#include <stdio.h>
#include "public/Module.h"
#include "FsLogger.h"
#include "EntHttpService.h"
#include "rapidxml\rapidxml.hpp"
#include "server/OuterErr.h"
#include "utils/util_func.h"
#include "log4cplus/logger.h"
#include "utils/string_util.h"
#include "utils/IniFile.h"
#ifdef ANALY_BY_USER
#include "FsGame/Analytics/SensorsAnalyticsCollector.h"
#else
#include "FsGame/Analytics/SensorsAnalyticsCollectorEx.h"
#endif

DECLARE_ENTITY("FxCharge", SoloCharge, EntNetBase)
DECLARE_METHOD_VOID_2( SoloCharge, SetNoVerifyUser, const char*, int )
DECLARE_METHOD_VOID_0(SoloCharge, ClearNoVerifyUser)
DECLARE_METHOD_0(std::string, SoloCharge, DumpNoVerifyUser)

DECLARE_PROPERTY(unsigned int, SoloCharge, ClientVersion, GetClientVersion, SetClientVersion)
DECLARE_PROPERTY(const char*, SoloCharge, AppId, GetAppId, SetAppId)
DECLARE_PROPERTY(bool, SoloCharge, Verify, GetVerify, SetVerify)
DECLARE_PROPERTY(bool, SoloCharge, AccIncompleted, GetAccIncompleted, SetAccIncompleted)

enum EmClientVersionResult
{
	EM_CLIENT_VERSION_SUCCEED,		// 成功通过
	EM_CLIENT_VERSION_LOWER,		// 版本过低
};

extern ICore* g_pCore;

// 最大缓存72小时
const int64_t LOGIN_STRING_CACHE_MAX = 60 * 60 * 72;
// 下线后缓存4小时
const int64_t LOGIN_STRING_CACHE_OUT = 60 * 60 * 4;


#define GAME_READY_CHECK_INTERVAL 3
const char* GAME_INI_PATH = "../res/game.ini";

SoloCharge::SoloCharge()
	: 
	//, m_pWorldSrv( NULL )
	 m_pHttpService( NULL )
	, m_bGameReady(false)
	, m_verifyKey(0)
	, m_nThreshold(10)
	, m_pAnaCollector( NULL )
	, m_nClientVersion( -1 )
	, m_bAccInCompleted( true )
{

}

bool SoloCharge::Init(const IVarList& args)
{
	FS_LOG_INFO( "SoloCharge::Init(const IVarList& args)" );
	m_pNetConnFactory = NetCreator::CreateConnectionFactory(false);
	this->m_pActionPacker = &m_packer;

	// 神策日志
	log4cplus::Logger _anaLogger = log4cplus::Logger::getInstance("ANA");

	// 读取游戏配置
	const char *path = g_pCore->GetResourcePath();
	std::string strGameIni(path);
	strGameIni.append(GAME_INI_PATH);
	CIniFile iniFile(strGameIni.c_str());
	if (!iniFile.LoadFromFile())
	{
		Assert(false);
		return false;
	}
	int nDeployId = iniFile.ReadInteger("game_info", "game_deploy_id", 0);
	int nGameId = iniFile.ReadInteger("game_info", "game_id", 0);
	const char* pszGameName = iniFile.ReadString("game_info", "game_name", "");
	const char* pszGameVer = iniFile.ReadString("game_info", "game_ver", "1.0");
	const char* pszDistrictName = iniFile.ReadString("game_info", "district_name", "");
	int pszChannelId = iniFile.ReadInteger("game_info", "channelid", 0);
	int production = iniFile.ReadInteger("game_info", "production", 0);

	m_pAnaCollector = new GameAnalytics(_anaLogger,
		nDeployId,
		nGameId,
		0,
		0,
		pszChannelId,
		production,
		StringUtil::StringAsUtf8String(pszGameVer).c_str(),
		StringUtil::StringAsUtf8String(pszGameName).c_str(),
		StringUtil::StringAsUtf8String(pszDistrictName).c_str());

	m_pAnaCollector->InitAction();


	return EntNetBase::Init(args);
}

bool SoloCharge::Shut()
{
	FS_LOG_INFO("SoloCharge::Shut");
	if (EntNetBase::Shut())
	{
		if (NULL != m_pNetConnFactory)
		{
			NetCreator::FreeConnectionFactory(m_pNetConnFactory);
			m_pNetConnFactory = NULL;
		}
		m_pActionPacker = NULL;

		if (NULL != m_pAnaCollector)
		{
			delete m_pAnaCollector;
			m_pAnaCollector = NULL;
		}

		CVarList res;
		GetCore()->RunScript("main.lua", "on_cleanup", res);
		return true;
	}

	return false;
}

bool SoloCharge::Startup()
{
	IEntity* pEntity = GetCore()->LookupEntity("EntHttpService");
	m_pHttpService = dynamic_cast<EntHttpService*>(pEntity);
	if (NULL == m_pHttpService)
	{
		FS_LOG_FATAL("not found http service.");
		return false;
	}
	
	if (EntNetBase::Startup())
	{
		if (!m_pService->Listen())
		{
			FS_LOG_ERROR("listen server failed");
			return false;
		}
		return true;
	}

	FS_LOG_FATAL("start server fatal.");
	return false;
}

bool SoloCharge::Stop()
{
	m_pHttpService = NULL;
	g_pCore->SetQuit(true);
	return EntNetBase::Stop();
}

void SoloCharge::Execute(float seconds)
{
	EntNetBase::Execute(seconds);
	GetCore()->Execute();


	//if (!m_bGameReady && NULL != m_pWorldSrv)
	//{
	//	time_t tNow = time(NULL);
	//	if (tNow - m_tcheckTime >= GAME_READY_CHECK_INTERVAL)
	//	{
	//		RequestGameState();
	//		m_tcheckTime = tNow;
	//	}
	//}

	
	DelOverTimerReq();


}

EMPackResult SoloCharge::Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen /*= NULL*/) const
{
	return m_packer.Pack(conn, stream, action, nOutLen);
}

EMPackResult SoloCharge::UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen /*= NULL*/) const
{
	return m_packer.UnPack(conn, stream, action, nOutLen);
}

int SoloCharge::GetNetKey() const
{
	return EM_NET_KEY_TCP;
}

void SoloCharge::SetNoVerifyUser( const char* pszAcc, int nValidNum )
{
	assert(NULL != pszAcc);
	if (NULL != pszAcc)
	{
		if (nValidNum == 0)
		{
			m_mapNoVeifyUsers.erase(pszAcc);
		}
		else
		{
			m_mapNoVeifyUsers[pszAcc] = nValidNum;
		}
	}
}

void SoloCharge::ClearNoVerifyUser()
{
	m_mapNoVeifyUsers.clear();
}

std::string SoloCharge::DumpNoVerifyUser()
{
	std::string strDump;

	for (auto itr = m_mapNoVeifyUsers.begin();
						itr != m_mapNoVeifyUsers.end(); ++itr)
	{
		if (!strDump.empty())
		{
			strDump.append(" | ");
		}

		strDump.append(itr->first);
		strDump.append(":");
		strDump.append( StringUtil::IntAsString(itr->second) );
	}

	return strDump;
}

void SoloCharge::OnInit()
{
	EntNetBase::OnInit();
#ifdef TEST
	BindAction("TEXT", ACTION_BINDER(SoloCharge::OnAction, this), 0);
#else
	BindAction("unknown", ACTION_BINDER(SoloCharge::OnAction, this), 0);
	BindAction("register", ACTION_BINDER(SoloCharge::OnActionServerRegister, this), 0);
	BindAction("login", ACTION_BINDER(SoloCharge::OnActionLogin, this), 0);
	BindAction("logout", ACTION_BINDER(SoloCharge::OnActionLogout, this), 0);
	BindAction("keep", ACTION_BINDER(SoloCharge::OnActionKeep, this), 0);
	BindAction("custom", ACTION_BINDER(SoloCharge::OnCustom, this), 0);
#endif
}

//void SoloCharge::OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
//{
//	if (NULL == m_pWorldSrv)
//	{
//		m_pWorldSrv = new ServerInfo;
//	}
//	m_pWorldSrv->ConnId = sessid;
//	EntNetBase::OnConnection(serid, sessid, pAction);
//
//	m_tcheckTime = time(NULL);
//}
//
//void SoloCharge::OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
//{
//	if (NULL != m_pWorldSrv)
//	{
//		if (m_pWorldSrv->ConnId == sessid)
//		{
//			delete m_pWorldSrv;
//			m_pWorldSrv = NULL;
//		}
//	}
//
//	m_bGameReady = false;
//	ServerManager::GetInstance()->RemoveByNID(sessid);
//	EntNetBase::OnClose(serid, sessid, pAction);
//}

void SoloCharge::OnActionServerRegister(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_ServerRegister* pAction = dynamic_cast<NetWorldAction_ServerRegister*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'register' action");
		return;
	}

	NetWorldMessage_ServerRgister* pMsg = dynamic_cast<NetWorldMessage_ServerRgister*>(pAction->Request());
	if (NULL == pMsg)
	{
		FS_LOG_ERROR("action 'register' not has message");
		return;
	}

	FS_LOG_INFO_FORMAT("connect info:%s\nprotocol:%s\n"
							"index:2 value:%d[int]\n"
							"index:3 value:%d[int]\n"
							"index:4 value:%s[string] name:srvinfo\n"
							"index:5 value:%s[string] name:md5\n"
							"index:6 value:%d[int]", pAction->coninfo.c_str(), pAction->protocol.c_str(),
							pMsg->value_of_2,
							pMsg->value_of_3,
							pMsg->srvinfo.c_str(),
							pMsg->enc.c_str(),
							pMsg->value_of_6);
}

void SoloCharge::OnActionLogin(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_Login* pAction = dynamic_cast<NetWorldAction_Login*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'login' action");
		return;
	}

	NetWorldMessage_Login_Req* pReq = dynamic_cast<NetWorldMessage_Login_Req*>(pAction->Request());
	if (NULL == pReq)
	{
		FS_LOG_ERROR("action 'login' not has request");
		return;
	}

#ifdef TEST
	FS_LOG_INFO_FORMAT("connect info:%s\nprotocol:%s\n"
							"index:2 value:%d[int] name:loginType\n"
							"index:3 value:%s[string] name:loginName\n"
							"index:4 value:%s[string] name:token\n"
							"index:5 value:%s[string] name:ip\n"
							"index:6 value:%d[int] name:port\n"
							"index:7 value:%s[string] name:loginString\n"
							"index:8 value:%d[int] name:clientType", pAction->coninfo.c_str(), pAction->protocol.c_str(),
							pReq->loginType,
							pReq->name.c_str(),
							pReq->token.c_str(),
							pReq->ip.c_str(),
							pReq->port,
							pReq->loginString.c_str(),
							pReq->clientType);
#endif

	NetWorldMessage_Login_Ack* pAck = dynamic_cast<NetWorldMessage_Login_Ack*>(pAction->Response());
	if (NULL == pAck)
	{
		FS_LOG_ERROR("action 'login' not has response");
		return;
	}

	//pAck->loginString = pReq->loginString;
	//pAck->name = pReq->name;
	//pAck->result = 1;		// 1为成功
	//pAction->SetState(EM_ACTION_REPLY);

	if (pReq->name.empty() || pReq->loginString.empty() )
	{
		FS_LOG_ERROR( "on login request failed(name is empty or login string is empty)" );
		pAck->result = CAS_ERR_ACCOUNT_PSWD;
		pAck->vaild_string = pReq->loginString;
		pAck->login_type = pReq->loginType;
		pAction->SetState(EM_ACTION_REPLY);
		return;
	}

	if (m_bGameReady)
	{
		FS_LOG_INFO_FORMAT("on game login(name:%s pwd:%s loginString:%s)",
							pReq->name.c_str(), pReq->token.c_str(), pReq->loginString.c_str());
		//if (pReq->loginType == 100)
		{
			// 不允许未完成的账号的请求登陆
			if ( !m_bAccInCompleted )
			{ 
				// 如果在连接中 直接忽略
				auto itr = m_userPendding.find(pReq->name);
				if (itr != m_userPendding.end())
				{
					FS_LOG_WARN_FORMAT("repeat login!!! don't allow incomplete accunt repeat login (name:%s)", pReq->name.c_str());
					return;
				}			
			}

			// 是否忽略验证用户
			do
			{
				if ( m_bEnableVerify )
				{
					auto itrNoVerify = m_mapNoVeifyUsers.find(pReq->name);
					if ( itrNoVerify == m_mapNoVeifyUsers.end() )
					{
						//  跳出while
						break;
					}

					if (itrNoVerify->second > 0)
					{
						--itrNoVerify->second;
						if (itrNoVerify->second == 0)
						{
							m_mapNoVeifyUsers.erase(itrNoVerify);
						}
					}
				}

				// 直接成功
				pAction->SetState(EM_ACTION_REPLY);
				pAck->result = OUTER_RESULT_SUCCEED;
				pAck->vaild_string = pReq->loginString;
				pAck->login_type = pReq->loginType;
				FS_LOG_INFO_FORMAT("No verify user login success (name:%s)", pReq->name.c_str());
				return;
			} while (false);
			
			//重新登录
			auto it = m_logonStringChche.find(pReq->name);
			if (it != m_logonStringChche.end())
			{
				LoginStringInfo& cache = it->second;
				if (pReq->loginString.compare(cache.strLoginString) == 0)
				{//重连成功
					pAction->SetState(EM_ACTION_REPLY);
					pAck->result = OUTER_RESULT_SUCCEED;
					pAck->vaild_string = pReq->loginString;
					pAck->login_type = pReq->loginType;
					FS_LOG_INFO_FORMAT("login  Relink success (name:%s)", pReq->name.c_str());

					// 增加key缓存时间
					cache.tExpiredTime = util_get_utc_time() + LOGIN_STRING_CACHE_MAX;
					return;
				}
				//else
				//{//失败
				//	pAction->SetState(EM_ACTION_REPLY);
				//	pAck->result = MGS_ERR_VALIDATE_FAILED;
				//	FS_LOG_WARN_FORMAT("login  Relink error (name:%s)", pReq->name.c_str());
				//	return;
				//}
			}
		}

		// 版本检查
		unsigned int nClientVersion = 0;
		size_t tVersionPos = pReq->loginString.find_last_of('$');
		std::string strLoginString;
		if (tVersionPos != std::string::npos)
		{
			strLoginString = pReq->loginString.substr(0, tVersionPos);
			const char* pszVersion = pReq->loginString.c_str() + tVersionPos + 1;
			try
			{
				nClientVersion = StringUtil::StringAsInt(pszVersion);
			}
			catch (...)
			{
				FS_LOG_WARN_FORMAT("client version exception(loginstring:%s)", pReq->loginString.c_str());
			}
		}
		else
		{
			strLoginString = pReq->loginString;
		}
		int nCheckResult = CheckClientVersion(nClientVersion);
		if (nCheckResult != EM_CLIENT_VERSION_SUCCEED)
		{
			FS_LOG_WARN_FORMAT("client version to lower (name:%s loginstring:%s client version:%d version:%d)",
													pReq->name.c_str(), pReq->loginString.c_str(), nClientVersion, m_nClientVersion);
			pAck->name = pReq->name;
			pAck->result = APP_CLIENT_VERSION_TOOLOWER;					// 客户端版本过低
			pAck->login_type = pReq->loginType;
			pAction->SetState(EM_ACTION_REPLY);
			return;
		}

		{
			//重新链接

			char verifyStr[20] = { 0 };
			sprintf(verifyStr, "%x", m_verifyKey++);
			LogicReq& logicReq = m_userLogicReq[verifyStr];
			logicReq.m_action = action;
			logicReq.m_sessid = sid;			
			logicReq.m_timestamp = time(NULL);

			int64_t nGameId = 0;
			int64_t nAreaId = 0;
			if (NULL != m_pAnaCollector)
			{
				nGameId = m_pAnaCollector->GetGameId();
				nAreaId = m_pAnaCollector->GetDistrictId();
			}

			if ( !m_bAccInCompleted )
			{
				m_userPendding.insert(pReq->name);
			}

			m_pHttpService->OnGameLogin(strLoginString.c_str(), pReq->name.c_str(), verifyStr, nGameId, nAreaId, pReq->ip.c_str(), m_strAppId.c_str() );
		}		
	}
	else
	{
		FS_LOG_INFO_FORMAT("on login request failed, game server not ready (name:%s)", pReq->name.c_str() );
		pAck->name = pReq->name;
		pAck->result = MGS_ERR_CHAREGE_FAILED;	// MGS_ERR_CHAREGE_FAILED;  服务器与计费服务器的连接故障
		pAck->login_type = pReq->loginType;
		pAction->SetState(EM_ACTION_REPLY);
	}
}

void SoloCharge::OnActionLogout(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_Logout* pAction = dynamic_cast<NetWorldAction_Logout*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'logout' action");
		return;
	}

	NetWorldMessage_Logout* pReq = dynamic_cast<NetWorldMessage_Logout*>(pAction->Request());
	if (NULL == pReq)
	{
		FS_LOG_ERROR("action 'logout' not has request");
		return;
	}

#ifdef TEST
	FS_LOG_INFO_FORMAT("connect info:%s\nprotocol:%s\n"
							"index:2 value:%d[int]\n"
							"index:3 value:%s[string]\n"
							"index:4 value:%d[int]\n"
							"index:5 value:%s[string] name:logoutName\n", pAction->coninfo.c_str(), pAction->protocol.c_str(),
							pReq->value_of_1,
							pReq->value_of_2.c_str(),
							pReq->value_of_3,
							pReq->name.c_str());
#endif

		auto it = m_logonStringChche.find(pReq->name);
		if (it != m_logonStringChche.end())
		{
			// 降低缓存时间
			LoginStringInfo& cache = it->second;
			cache.tExpiredTime = util_get_utc_time() + LOGIN_STRING_CACHE_OUT;
			//m_logonStringChche.erase(it);
		}

		// 如果在登陆中  从登陆中删除
		if (!m_bAccInCompleted)
		{
			auto itr = m_userPendding.find(pReq->name);
			if (itr != m_userPendding.end())
			{
				m_userPendding.erase(itr);
				// 记录一个日志
				FS_LOG_WARN_FORMAT("user '%s' login incomplete.", pReq->name.c_str());
			}
		}

		// 记录退出信息
		if (NULL != m_pAnaCollector)
		{
			m_pAnaCollector->OnUserLogout( pReq->name.c_str() );
		}

		FS_LOG_DEBUG_FORMAT("logout!!![%s]", pReq->name.c_str());
}

void SoloCharge::OnActionKeep(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_Keep* pAction = dynamic_cast<NetWorldAction_Keep*>(action.get());
	if (NULL == pAction)
	{
		return;
	}

	/*	
	//char szMsg[1024] = { 0 };
	//sprintf(szMsg, "connect info:%s\nprotocol:%s", pAction->coninfo.c_str(), pAction->protocol.c_str());
	//WriteMessage(szMsg);
	*/
	if (m_bReplyKeep)
	{
		if (NULL != m_pService)
		{
			m_pService->Send(sid, action);
		}
	}
}

void SoloCharge::OnCustom(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_Custom* pAction = dynamic_cast<NetWorldAction_Custom*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_DEBUG("recv empty 'custom' action");
		return;
	}

	NetWorldMessage_Custom* pReq = dynamic_cast<NetWorldMessage_Custom*>(pAction->Request());
	if (NULL == pReq)
	{
		FS_LOG_ERROR("action 'custom' not has request");
	}

	if (pReq->cargs.GetCount() < 1)
	{
		FS_LOG_DEBUG("custom message param error, not any param");
		return;
	}

	int cmd = pReq->cargs.IntVal(0);
	switch (cmd)
	{
	case NET_WORLD_CUSTOM_LOGIN_READY:
		OnHandleGameReady(serid, sid, action, pReq, 1);
		break;
	default:
		break;
	}
}

void SoloCharge::OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
#ifdef TEST
	TextAction* txtAction = dynamic_cast<TextAction*>(action.get());
	if (NULL == txtAction)
	{
		WriteMessage("recv empty action");
		return;
	}

	NetTextMessage* txtMessage = dynamic_cast<NetTextMessage*>(txtAction->Request());
	if (NULL == txtMessage)
	{
		WriteMessage("recv empty message");
		return;
	}

	WriteMessage(txtMessage->GetMsgBody());
	// reply
	m_pService->Send(sid, action);
#else 
	NetWorldAction_Unknown* pAction = dynamic_cast<NetWorldAction_Unknown*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'unknown' action");
		return;
	}

	IVarList& args = pAction->cargs;
	FS_LOG_INFO_FORMAT( "param count:%d", args.GetCount());
	for (size_t i = 0; i < args.GetCount(); ++i)
	{
		switch (args.GetType(i))
		{
		case	VTYPE_BOOL:		// 布尔
			FS_LOG_INFO_FORMAT( "index:%d bool value:%s", i, args.BoolVal(i) ? true : false);
			break;
		case	VTYPE_INT:		// 32位整数
			FS_LOG_INFO_FORMAT( "index:%d int value:%d", i, args.IntVal(i));
			break;
		case	VTYPE_INT64:	// 64位整数
			FS_LOG_INFO_FORMAT( "index:%d int64 value:%lld", i, args.Int64Val(i));
			break;
		case	VTYPE_FLOAT:	// 单精度浮点数
			FS_LOG_INFO_FORMAT( "index:%d float value:%f", i, args.FloatVal(i));
			break;
		case	VTYPE_DOUBLE:	// 双精度浮点数
			FS_LOG_INFO_FORMAT( "index:%d double value:%llf", i, args.DoubleVal(i));
			break;
		case	VTYPE_STRING:	// 字符串
			FS_LOG_INFO_FORMAT( "index:%d string value:%s", i, args.StringVal(i));
			break;
		case	VTYPE_WIDESTR:	// 宽字符串
			FS_LOG_INFO_FORMAT( "index:%d wide value:%s", i, (char*)args.WideStrVal(i));
			break;
		case	VTYPE_OBJECT:	// 对象号
			FS_LOG_INFO_FORMAT( "index:%d object value:", i);
			break;
		case	VTYPE_POINTER:	// 指针
			FS_LOG_INFO_FORMAT( "index:%d pointer value:", i);
			break;
		case	VTYPE_USERDATA:	// 用户数据
			FS_LOG_INFO_FORMAT("index:%d user value:", i);
			break;
		case	VTYPE_TABLE:	// 表
			FS_LOG_INFO_FORMAT("index:%d table value:", i);
			break;
		default:
			FS_LOG_INFO_FORMAT("index:%d value:unkown type", i);
			break;
		}
	}
#endif
}

void SoloCharge::DelOverTimerReq()
{
	// 判断LoginString过期
	time_t tNow = util_get_utc_time();
	for (MapLoginStringCache::iterator itr = m_logonStringChche.begin(), next = itr; itr != m_logonStringChche.end(); itr = next)
	{
		++next;
		if (itr->second.tExpiredTime < tNow)
		{
			m_logonStringChche.erase(itr);
		}
	}


	
	for (auto it = m_userLogicReq.begin(); it != m_userLogicReq.end();)
	{
		if (it->second.m_timestamp + LOGIN_TIMER_OVER < tNow)
		{
			if (!m_bAccInCompleted)
			{
				NetWorldMessage_Login_Req* pReq = dynamic_cast<NetWorldMessage_Login_Req*>( it->second.m_action->Request() );
				if (NULL != pReq)
				{
					m_userPendding.erase(pReq->name);
				}
			}
			it = m_userLogicReq.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void SoloCharge::OnHandleGameReady(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action, NetWorldMessage_Custom* pReq, size_t nArgsOffset)
{
	if (pReq->cargs.GetCount() < nArgsOffset)
	{
		FS_LOG_ERROR_FORMAT("game ready messge param error(count:%d)", pReq->cargs.GetCount());
		return;
	}

	FS_LOG_INFO_FORMAT("on game ready(count:%d)", pReq->cargs.GetCount());
	m_bGameReady = pReq->cargs.IntVal(nArgsOffset++) != 0;
	if (m_bGameReady)
	{
		FS_LOG_INFO_FORMAT( "on charge start working" );
	}

	if (m_bGameReady)
	{
		int64_t nGameId = pReq->cargs.Int64Val(nArgsOffset++);
		int64_t nServerId = pReq->cargs.Int64Val(nArgsOffset++);
		int64_t nDistrictId = pReq->cargs.Int64Val(nArgsOffset++);
		int64_t nChannelId = pReq->cargs.Int64Val(nArgsOffset++);
		int64_t nProduction = pReq->cargs.Int64Val(nArgsOffset++);
		int64_t nDeployId = pReq->cargs.Int64Val(nArgsOffset++);

		if (NULL != m_pAnaCollector)
		{
			FS_LOG_INFO("Set Game Data::BEGIN");
			FS_LOG_INFO_FORMAT("game id:%lld", nGameId);
			//FS_LOG_INFO_FORMAT("server id:%lld", nServerId);
			FS_LOG_INFO_FORMAT("district id:%lld", nDistrictId);
			FS_LOG_INFO_FORMAT("channel id:%lld", nChannelId);
			FS_LOG_INFO_FORMAT("production:%lld", nProduction);
			FS_LOG_INFO("Set Game Data::END");

			m_pAnaCollector->SetGameId(nGameId);
			//m_pAnaCollector->SetServerId(nServerId);
			m_pAnaCollector->SetDistrictId(nDistrictId);
			m_pAnaCollector->SetChannelId(nChannelId);
			m_pAnaCollector->SetProductionId(nProduction);
			m_pAnaCollector->SetDeployId(nDeployId);
		}
		else
		{
			FS_LOG_ERROR("Ana collector not created");
		}

		// 设置appid
		if (m_strAppId.empty())
		{
			std::stringstream ssAppId;
			ssAppId << nDistrictId;
			m_strAppId = ssAppId.str();
		}
	}
	// 回应请求
	pReq->cargs.Clear();
	pReq->cargs.AddInt(NET_WORLD_CUSTOM_LOGIN_STATE_ACK);
	if (NULL != m_pService)
	{
		m_pService->Send(sid, action);
	}
}

int SoloCharge::CheckClientVersion(unsigned int nClientVersion) const
{
	if (nClientVersion < m_nClientVersion)
	{
		return EM_CLIENT_VERSION_LOWER;
	}

	return EM_CLIENT_VERSION_SUCCEED;
}

//void SoloCharge::RequestGameState()
//{
//	if (NULL != m_pWorldSrv && NULL != m_pService)
//	{
//		NetWorldAction_Custom* p = new NetWorldAction_Custom();
//		NetWorldMessage_Custom* pReq = dynamic_cast<NetWorldMessage_Custom*>( p->Request() );
//		pReq->cargs.AddInt(NET_WORLD_LOGIN_STATE_ACK);
//		std::shared_ptr<INetAction> pAction(p);
//		m_pService->Send(m_pWorldSrv->ConnId, pAction);
//	}
//}

void SoloCharge::OnLoginResult(const char*name, int isResult,const char* verifyStr)
{
	USERLOGICREQ::iterator  it = m_userLogicReq.find( verifyStr );
	if (it == m_userLogicReq.end()){
		FS_LOG_INFO_FORMAT("virify seq invalid(name:%s)", name);
		return;
	}

	LogicReq& req = it->second;
	FS_LOG_DEBUG_FORMAT("on login result(name:%s result:%s code:%d)", 
							NULL == name ? "xx__unkown" : name, 
							isResult == OUTER_RESULT_SUCCEED ? "true" : "false",
							isResult );
	time_t tNow = time(NULL);
	time_t tCost = tNow - req.m_timestamp;
	if (tCost > m_nThreshold)
	{
		FS_LOG_WARN_FORMAT("on login reply delay(delay:%lld name:%s result:%s)", tCost, NULL == name ? "xx__unkown" : name, isResult ? "true" : "false");
	}

	NetWorldAction_Login* pAction = dynamic_cast<NetWorldAction_Login*>(it->second.m_action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR_FORMAT("recv empty 'login' action (name:%s)", name);
		return;
	}

	NetWorldMessage_Login_Req* pReq = dynamic_cast<NetWorldMessage_Login_Req*>(pAction->Request());
	if (NULL == pReq)
	{
		FS_LOG_ERROR_FORMAT("action 'login' not has request(name:%s)", name);
		return;
	}

	NetWorldMessage_Login_Ack* pAck = dynamic_cast<NetWorldMessage_Login_Ack*>(pAction->Response());
	if (NULL == pAck)
	{
		FS_LOG_ERROR_FORMAT("action 'login' not has response(name:%s)", name);
		return;
	}

	pAck->login_type = pReq->loginType;
	pAck->name = pReq->name;
	pAck->result = isResult;		// 1为成功
	pAck->vaild_string = pReq->loginString;
	pAction->SetState(EM_ACTION_REPLY);

	if (isResult == OUTER_RESULT_SUCCEED)
	{
		// 缓存key
		LoginStringInfo& cache = m_logonStringChche[name];
		cache.strLoginString = pReq->loginString;
		cache.tExpiredTime = util_get_utc_time() + LOGIN_STRING_CACHE_MAX;
	}
	
	m_pService->Send(it->second.m_sessid, it->second.m_action);

	// 清除记录
	if (!m_bAccInCompleted)
	{
		m_userPendding.erase(pReq->name);
	}
	m_userLogicReq.erase(it);
}

