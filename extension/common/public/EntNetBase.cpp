#include "EntNetBase.h"
#include <stdio.h>
#include "public/Module.h"
#include "EntNetManager.h"
#include "FsLogger.h"
#include "utils/string_util.h"

DECLARE_ABSTRACT_ENTITY("FxExtension", EntNetBase, IEntity)

DECLARE_METHOD_0(bool, EntNetBase, Startup)
DECLARE_METHOD_0(bool, EntNetBase, Stop)
DECLARE_METHOD_1(bool, EntNetBase, IsWhiteIP, const char*)
DECLARE_METHOD_VOID_1(EntNetBase, SetWhiteTables, const char*)
DECLARE_METHOD_2(bool, EntNetBase, SetValue, const char*, const char*)
DECLARE_METHOD_2(const char*, EntNetBase, GetValue, const char*, const char*)
DECLARE_METHOD_2(int, EntNetBase, GetIntValue, const char*, int)
DECLARE_METHOD_2(float, EntNetBase, GetFloatValue, const char*, float)

DECLARE_PROPERTY_GET(PERSISTID, EntNetBase, ID, GetID)
DECLARE_PROPERTY(const char*, EntNetBase, IP, GetIP, SetIP)
DECLARE_PROPERTY(unsigned short, EntNetBase, Port, GetListenPort, SetListenPort)
DECLARE_PROPERTY(int, EntNetBase, ReadBuf, GetReadBufSize, SetReadBufSize)
DECLARE_PROPERTY(int, EntNetBase, SendBuf, GetSendBufSize, SetSendBufSize)
DECLARE_PROPERTY(int, EntNetBase, LogLevel, GetLogLevel, SetLogLevel)
DECLARE_PROPERTY_GET(bool, EntNetBase, AnyIP, AnyIP)
DECLARE_PROPERTY(bool, EntNetBase, IsReplyKeep, GetReplyKeep, SetReplyKeep)

DECLARE_METHOD_VOID_2(EntNetBase, WriteMessage, const char*, int)

EntNetBase::EntNetBase()
	: m_strIP("0.0.0.0")
	, m_nPort(0)
	, m_nMaxSize(1024)
	, m_nReadBuf( 1024 )
	, m_nSendBuf( 1024 )
	, m_pIoDesc( NULL )
	, m_pService( NULL )
	, m_pLinkManaer( NULL )
	, m_pHandler( NULL )
	, m_pNetConnFactory( NULL )
	, m_pActionPacker( NULL )
	, m_bReplyKeep( false )
{
	
}

EntNetBase::~EntNetBase()
{
	if (NULL != m_pIoDesc)
	{
		delete m_pIoDesc;
	}
}

bool EntNetBase::Init(const IVarList& args)
{
	FS_LOG_INFO("EntNetBase::Init(const IVarList& args)");

	m_pLinkManaer = new ActionHandleLinkManager();
	m_pHandler = new NetHandler( GetNetKey(), *m_pLinkManaer );
	m_pHandler->SetAsyncCompletedHandle(ASYNC_ACTION_COMPLETED_BINDER(EntNetBase::OnCompletedAction, this));
	m_pHandler->SetUnBindAction(ACTION_BINDER(EntNetBase::OnUnHandleAction, this));

	OnInit();

	return true;
}

bool EntNetBase::Shut()
{
	FS_LOG_INFO("EntNetBase::Shut()");
	if (NULL != m_pService)
	{
		Stop();
	}

	OnShut();

	if (NULL != m_pIoDesc)
	{
		delete m_pIoDesc;
		m_pIoDesc = NULL;
	}

	if (NULL != m_pHandler)
	{
		delete m_pHandler;
		m_pHandler = NULL;
	}

	if (NULL != m_pLinkManaer)
	{
		delete m_pLinkManaer;
		m_pLinkManaer = NULL;
	}

	return true;
}

void EntNetBase::Execute(float seconds)
{
	//if (NULL != m_pService)
	//{
		//m_pService->Process();
	//}
	if (NULL != m_pHandler)
	{
		m_pHandler->Process();
	}
}


void EntNetBase::SetLogLevel(int nLevel)
{
#ifndef USE_LOG4CPLUS
	g_pLogger->SetLogLevel((EMLoggerLevel)nLevel);
#endif
}

int EntNetBase::GetLogLevel() const
{
#ifndef USE_LOG4CPLUS
	return g_pLogger->GetLogLevel();
#endif

	return 0;
}

void EntNetBase::SetWhiteTables(const char* ips)
{
	if (NULL == ips || ips[0] == 0)
	{
		m_bAnyIP = true;
		return;
	}

	m_strWhiteTables.clear();
	m_bAnyIP = false;
	size_t _nLen = strlen(ips);
	char* _ipl = new char[_nLen + 1];
	strcpy(_ipl, ips);
	_ipl[_nLen] = 0;

	char* _ip = _ipl;
	char* _token = NULL;
	do
	{
		_token = strchr(_ip, ',');
		if (NULL != _token)
		{
			*_token = '\0';

			m_strWhiteTables.push_back(_ip);
			if (strcmp(_ip, "*"))
			{
				m_bAnyIP = true;
				break;
			}
			_ip = _token + 1;
		}
	} while (_token);

	if (NULL != _ip && _ip[0] != 0)
	{
		m_strWhiteTables.push_back(_ip);
		if (strcmp(_ip, "*") == 0)
		{
			m_bAnyIP = true;
		}
	}

	delete[] _ipl;
}

bool EntNetBase::IsWhiteIP(const char* ip) const
{
	if (AnyIP())
	{
		return true;
	}

	if (NULL == ip || ip[0] == 0)
	{
		return false;
	}

	for (std::list<std::string>::const_iterator itr = m_strWhiteTables.begin();
		itr != m_strWhiteTables.end(); ++itr)
	{
		if (strcmp(itr->c_str(), ip) == 0)
		{
			return true;
		}
	}

	return false;
}

bool EntNetBase::SetValue(const char* pszKey, const char* pszValue)
{
	if (StringUtil::CharIsNull(pszKey))
	{
		return false;
	}
	if (NULL == pszValue)
	{
		return false;
	}

	m_mpaValues[pszKey] = pszValue;
	return true;
}

const char* EntNetBase::GetValue(const char* pszKey, const char* defValue)
{
	if ( StringUtil::CharIsNull(pszKey) )
	{
		return defValue;
	}

	auto itr_ = m_mpaValues.find(pszKey);
	if (itr_ == m_mpaValues.end())
	{
		return defValue;
	}

	return itr_->second.c_str();
}

int EntNetBase::GetIntValue(const char* pszKey, int defValue)
{
	const char* pszValue = GetValue(pszKey, NULL);
	return StringUtil::StringAsInt(pszValue, defValue);
}

float EntNetBase::GetFloatValue(const char* pszKey, float defValue)
{
	const char* pszValue = GetValue(pszKey, NULL);
	return StringUtil::StringAsFloat(pszValue, defValue);
}

void EntNetBase::WriteMessage(const char* msg, int nLogLevel) const
{
	if (NULL != msg)
	{
		switch ( nLogLevel )
		{
		case EM_LOGGER_LEVEL_FATAL:
			FS_LOG_FATAL(msg);
			break;
		case EM_LOGGER_LEVEL_ERROR:
			FS_LOG_ERROR(msg);
			break;
		case EM_LOGGER_LEVEL_WARN:
			FS_LOG_WARN(msg);
			break;
		case EM_LOGGER_LEVEL_INFO:
			FS_LOG_INFO(msg);
			break;
		case EM_LOGGER_LEVEL_DEBUG:
			FS_LOG_DEBUG(msg);
			break;
		case EM_LOGGER_LEVEL_TRACE:
			FS_LOG_TRACE(msg);
			break;
		default:
			break;
		}

	}
}


bool EntNetBase::Startup()
{
	if (NULL != m_pService)
	{
		return false;
	}

	INetIoDesc* desc = GetIoDesc();
	if (NULL == desc)
	{
		return false;
	}

	if (EntNetManager::Instance().StartServer(desc, m_pHandler,m_pNetConnFactory, m_pActionPacker, this))
	{
		m_pService = EntNetManager::Instance().Lookup(desc->GetIoKey());
	}
	return NULL != m_pService;
}

bool EntNetBase::Stop()
{
	int nKey = GetNetKey();
	if (EntNetManager::Instance().StopServer(nKey))
	{
		EntNetManager::Instance().RemoveServer(nKey);
		m_pService = NULL;
	}

	//Execute(1);
	//if (NULL != m_pHandler)
	//{
	//	m_pHandler->Reset();
	//}

	return true;
}

bool EntNetBase::AllowNewConnect()
{
	INetService* service = GetService();
	if (NULL != service)
	{
		const INetIoDesc* desc = service->GetDesc();
		if (NULL != desc)
		{
			if (service->GetConnCount() >= desc->GetMaxAccept())
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool EntNetBase::AllowConnect(const char* szRemoteIp, int nRemotePort)
{
	if (m_bAnyIP)
	{
		return true;
	}

	return IsWhiteIP(szRemoteIp);
}

INetIoDesc* EntNetBase::GetIoDesc()
{
	if (NULL == m_pIoDesc)
	{
		NetIoDesc* pIoDesc = new NetIoDesc();
		m_pIoDesc = pIoDesc;

		pIoDesc->SetIP(m_strIP.c_str())
			.SetKey(GetNetKey())
			.SetMaxAccept(m_nMaxSize)
			.SetListenPort(m_nPort)
			.SetReadBufSize(m_nReadBuf)
			.SetSendBufSize(m_nSendBuf);
	}

	return m_pIoDesc;
}


bool EntNetBase::BindAction(int nActionId, ACTION_EVENT action, int nPriority)
{
	return	m_pHandler->BindAction(nActionId, action, nPriority);
}

bool EntNetBase::BindAction(const char* nActionKey, ACTION_EVENT action, int nPriority)
{
	return	m_pHandler->BindAction(nActionKey, action, nPriority);
}

void EntNetBase::OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	SYSActionEnter* pEnter = dynamic_cast<SYSActionEnter*>(pAction.get());
	if (NULL == pEnter)
	{
		FS_LOG_ERROR_FORMAT( "OnConnection[serid:%d sessid:%lld, action not is SYSActionEnter]", serid, sessid );
	}
	else
	{
		FS_LOG_INFO_FORMAT("OnConnection[serid:%d sessid : %lld ip : %s, port:%d is accept:%s]",
			serid, sessid,
			pEnter->GetPeerHost() ? pEnter->GetPeerHost() : "",
			pEnter->GetPeerPort(), pEnter->GetAccept() ? "true":"false");

	}
}
void EntNetBase::OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	SYSActionExit* pExit = dynamic_cast<SYSActionExit*>(pAction.get());
	if (NULL == pExit)
	{
		FS_LOG_ERROR_FORMAT("OnClose[serid:%d sessid:%lld, action not is SYSActionExit]", serid, sessid);
	}
	else
	{
		FS_LOG_INFO_FORMAT("OnClose[serid:%d sessid:%lld ip:%s, port:%d is accept:%s]",
								serid, sessid,
								pExit->GetPeerHost() ? pExit->GetPeerHost() : "",
								pExit->GetPeerPort(), pExit->GetAccept() ? "true" : "false");
	}
}

void EntNetBase::OnUnRegisterAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	SYSActionUnRegister* pUnRegister = dynamic_cast<SYSActionUnRegister*>(pAction.get());
	if (NULL == pUnRegister)
	{
		FS_LOG_ERROR_FORMAT("OnUnRegisterAction[serid:%d sessid:%lld, action not is SYSActionUnRegister]", serid, sessid);
	}
	else
	{
		FS_LOG_INFO_FORMAT("OnUnRegisterAction[serid:%d sessid:%lld action:%d]",
									serid, sessid,
									pUnRegister->GetRawActionID());
	}
}
void EntNetBase::OnRawDataAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	SYSActionRawData* pRawAction = dynamic_cast<SYSActionRawData*>(pAction.get());
	if (NULL == pRawAction)
	{
		FS_LOG_ERROR_FORMAT("OnRawDataAction[serid:%d sessid:%lld, action not is SYSActionRawData]", serid, sessid);
	}
	else
	{
		FS_LOG_INFO_FORMAT("OnRawDataAction[serid:%d sessid:%lld action:%d]",
									serid, sessid,
									pRawAction->GetRawActionID());

	}
}
void EntNetBase::OnErrAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	FS_LOG_ERROR_FORMAT("serid:%d sessid : %lld", serid, sessid);

	BinaryMessage* pMessage = dynamic_cast<BinaryMessage*>( pAction->Request() );
	DumpBinMessage("recv error packet, dump bin message", pMessage);
}

void EntNetBase::OnUnHandleAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	FS_LOG_WARN_FORMAT("OnUnHandleAction[serid:%d sessid : %lld action : %d(or key : %s)]",
						serid, sessid, pAction->ActionId(), pAction->ActionKey() ? pAction->ActionKey() : "");
}

void EntNetBase::OnCompletedAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	if ( pAction->IsReply() && NULL != m_pService )
	{
		m_pService->Send(sessid, pAction);
	}
}

void EntNetBase::OnEmptyAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	FS_LOG_ERROR_FORMAT("serid:%d sessid : %lld", serid, sessid);

	BinaryMessage* pMessage = dynamic_cast<BinaryMessage*>(pAction->Request());
	DumpBinMessage("recv empty packet, dump bin message", pMessage);
}

void EntNetBase::DumpBinMessage(const char* pszDesc, BinaryMessage* bin)
{
	if (NULL != pszDesc)
	{
		FS_LOG_ERROR_FORMAT("%s", pszDesc);
		printf("%s\n", pszDesc);
	}
	if (NULL == bin)
	{
		FS_LOG_ERROR_FORMAT("data:(null)");
		printf("data:(null)\n");
	}
	else
	{
		size_t nLen = bin->GetMsgLen();
		const char* pszBuf = bin->GetMsgBody();
		char* pszContent = new char[nLen * 3 + 1];
		memset(pszContent, 0, nLen * 3 + 1);
		for (size_t i = 0; i < nLen; ++i)
		{
			sprintf_s(pszContent + i * 3, 3, "%02x ", *(pszBuf + i));
		}

		FS_LOG_ERROR_FORMAT("data:%s", pszContent);
		printf("data:%s\n", pszContent);

		delete[] pszContent;
	}
}


void EntNetBase::OnInit()
{
	BindAction(SYS_ACTION_ENTER, ACTION_BINDER(EntNetBase::OnConnection, this), -1024);
	BindAction(SYS_ACTION_EXIT, ACTION_BINDER(EntNetBase::OnClose, this), -1024);
	BindAction(SYS_ACTION_UNREGISTER, ACTION_BINDER(EntNetBase::OnUnRegisterAction, this), -1024);
	BindAction(SYS_ACTION_RAW_DATA, ACTION_BINDER(EntNetBase::OnRawDataAction, this), -1024);
	BindAction(SYS_ACTION_ERR_DATA, ACTION_BINDER(EntNetBase::OnErrAction, this), -1024);
	BindAction(SYS_ACTION_EMTPY, ACTION_BINDER(EntNetBase::OnEmptyAction, this), -1024);
}
void EntNetBase::OnShut()
{
}