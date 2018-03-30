#include "EntHttp.h"
#include "public/Module.h"
#include <stdio.h>
#include "EntNetManager.h"
#include "action/IHttpAction.h"
#include "FsLogger.h"
#include "SoloGmcc.h"
#include "ProtocolsID.h"
#include "utils/json.h"
#include "utils/string_util.h"
#include "PayedOrder.h"
#include "action/HttpData.h"
#include "../libnet/action/HttpDataFormater.h"
#include "../libnet/action/HttpJsonFormater.h"
#include "base64.h"
#include "ServerInfo.h"
#include "../libnet/openssl/ssl_env.h"
#include "utils/util_func.h"
#include "EntRpc.h"
#include "cpptempl.h"
#include <io.h>
#include "rapidxml/rapidxml_utils.hpp"
#include <iosfwd>

DECLARE_ENTITY("FsGmcc", EntHttp, EntNetBase)
DECLARE_PROPERTY(const char*, EntHttp, PushUrl, GetPushUrl, SetPushUrl)
DECLARE_PROPERTY(const char*, EntHttp, PushKey, GetPushKey, SetPushKey)
DECLARE_PROPERTY(const char*, EntHttp, PushSecret, GetPushSecret, SetPushSecret)
DECLARE_PROPERTY(const char*, EntHttp, PushSound, GetPushSound, SetPushSound)
DECLARE_PROPERTY(int, EntHttp, PushBuilderId, GetPushBuilderId, SetPushBuilderId)
DECLARE_PROPERTY(int, EntHttp, PushTimeLive, GetPushTimeLive, SetPushTimeLive)
DECLARE_PROPERTY(bool, EntHttp, IsProduction, IsProduction, SetProduction)

DECLARE_PROPERTY(int, EntHttp, ReqReuseNum, GetReqResueNum, SetReqResueNum)
DECLARE_PROPERTY(int, EntHttp, ReqRetries, GetReqRetries, SetReqRetries)

DECLARE_METHOD_VOID_2(EntHttp, AddReqDefHeader, const char*, const char*)
DECLARE_METHOD_VOID_1(EntHttp, RemoveReqDefHeader, const char*)
DECLARE_METHOD_VOID_0(EntHttp, ClearReqDefHeaders)

// 宏定义
#define MY_FUNCTION_NAME __FUNCTION__
#define EXCEPTION_BEGIN() try {
#define EXCEPTION_END() 	\
		}	\
		catch (std::runtime_error & err)	\
		{	\
			FS_LOG_FATAL_FORMAT("%s exception(msg:%s)", MY_FUNCTION_NAME, err.what());	\
		}	\
		catch (...)	\
		{	\
			FS_LOG_FATAL_FORMAT("%s exception(unknown)", MY_FUNCTION_NAME);	\
		}	

#define HTTP_CONTENT_SIZE		10240			// 10k
#define TIMEOUT_OF_HTTP_REQUEST 30000			// 30秒
#define TIMEOUT_OF_PROFILE_REQUEST 10000		// 10秒
#define	TIMEOUT_OF_RETRIES_NUM	10				// 最多重试10次
#define HTTP_CACHE_RESEND_TIMER	60				// 1分钟

#define PATH_CONFIG_NAME	"../res/config/config.json"
#define PATH_CONFIG_BAK		"../res/config/bak/"

//#define CONFIG_KEY_FIRST_PAYED		"first_payed_reward"		// 首充配置
//#define CONFIG_KEY_ACTIVITY_SHOP	"activity_shop"				// 商城活动配置 
//#define CONFIG_KEY_LEVELING_CONFIG	"leveling_config"			// 冲级活动配置 

enum EmDataResult
{
	EM_RESULT_DATA_SUCCEED,				// 成功
	EM_RESULT_DATA_FAILED,				// 解释数据错 或 错误码不一致(返回不是10000)
	EM_RESULT_DATA_HTTP_ERR,			// http返回不是200
};

EntHttp::EntHttp()
	: m_nDefaultBuilderId( 0 )
	, m_nTimeToLive( 0 )
	, m_bEnvProduction( false )
	, m_pMainEntity( NULL )
	, m_pRpcService( NULL )
	, m_nReqReuseNum(1)
	, m_nReqRetries(0)

{

}

bool EntHttp::Init(const IVarList& args)
{
	FS_LOG_INFO("EntHttpService::Init(const IVarList& args)");
	m_pNetConnFactory = NetCreator::CreateConnectionFactory(true);
	return EntNetBase::Init(args);
}

bool EntHttp::Shut()
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

bool EntHttp::Startup()
{
	// 初始化配置信息
	if (!ConfigRequestUrl())
	{
		return false;
	}

	// 配置openssl信息
	//ssl_env_set_method(EM_ENV_SSL_METHOD_V23);
	//ssl_env_set_certificate_file("certs/ca.crt");
	//ssl_env_set_private_key("certs/ca.key");
	//ssl_env_set_private_pass("123456");
	//INetIoDesc* pIoDesc = GetIoDesc();
	//pIoDesc->SetSSL(true);

	IEntity* pEntity = GetCore()->LookupEntity("SoloGmcc");
	m_pMainEntity = dynamic_cast<SoloGmcc*>(pEntity);
	if (NULL == m_pMainEntity)
	{
		FS_LOG_FATAL("not found main service");
		return false;
	}

	pEntity = GetCore()->LookupEntity("EntRpc");
	m_pRpcService = dynamic_cast<EntRpc*>(pEntity);
	if (NULL == m_pRpcService)
	{
		FS_LOG_FATAL("not found rpc service.");
		return false;
	}


	if (EntNetBase::Startup())
	{
		if (!m_pService->Listen())
		{
			FS_LOG_ERROR("listen service failed");
			return false;
		}
		OnReqProfile();

//#define TEST
#ifdef TEST
		PushData data;
		data.msgType = EM_PUSH_NOTIFICATION;
		data.recvierPlatform = EM_CLIENT_ALL;
		data.title = "test push";
		data.content = "xxxxxxxx dddss dddd";
		OnReqPushNotification(data);
		//HttpAction* actoin = new HttpAction();
		//IHttpRequest* request = (IHttpRequest*)actoin->Request();
		//request->SetURI("https://www.baidu.com:443/");
		//m_pService->Send(NULL_NID, std::shared_ptr<INetAction>(actoin));

#endif
		if (LoadConfig())
		{
			g_pCore->AddExecute(this);
			return true;
		}
		else
		{
			FS_LOG_FATAL("load config failed");
		}
	}
	FS_LOG_FATAL("start server fatal");
	return false;
}


bool EntHttp::Stop()
{
	return EntNetBase::Stop();
}

void EntHttp::Execute(float seconds)
{
	EntNetBase::Execute(seconds);

	std::list<ResendAction> resentList;
	std::map<INetAction*, ResendAction>::iterator itr = m_cacheActions.begin();
	for (std::map<INetAction*, ResendAction>::iterator next = itr; itr != m_cacheActions.end(); itr = next)
	{
		++next;
		itr->second.elapsed += seconds;
		if (itr->second.elapsed >= HTTP_CACHE_RESEND_TIMER)
		{
			resentList.push_back( itr->second );
			m_cacheActions.erase(itr);
		}
	}

	for (std::list<ResendAction>::iterator itr = resentList.begin(); itr != resentList.end(); ++itr)
	{
		RepRequest(itr->action);
		//if (!RepRequest(itr->action))
		//{
		//	itr->elapsed = 0.0;
		//	m_cacheActions[itr->action.get()] = *itr;
		//}
	}
}

int EntHttp::GetNetKey() const
{
	return EM_NET_KEY_HTTP;
}

const char* EntHttp::GetPushUrl() const
{
	return m_strUrlPush.c_str();
}

void EntHttp::SetPushUrl(const char* url)
{
	Assert(!StringUtil::CharIsNull(url));
	m_strUrlPush = url;
}

const char* EntHttp::GetPushKey() const
{
	return m_strPushKey.c_str();
}

void EntHttp::SetPushKey(const char* key)
{
	Assert(!StringUtil::CharIsNull(key));
	m_strPushKey = key;
}

const char* EntHttp::GetPushSecret() const
{
	return m_strPushSecret.c_str();
}

void EntHttp::SetPushSecret(const char* secret)
{
	Assert(!StringUtil::CharIsNull(secret));
	m_strPushSecret = secret;
}

const char* EntHttp::GetPushSound() const
{
	return m_strDefaultSound.c_str();
}



void EntHttp::SetPushSound(const char* sound)
{
	Assert(!StringUtil::CharIsNull(sound));
	m_strDefaultSound = sound;
}

int EntHttp::GetPushBuilderId() const
{
	return m_nDefaultBuilderId;
}

void EntHttp::SetPushBuilderId(int nBuilderId)
{
	m_nDefaultBuilderId = nBuilderId;
}

int EntHttp::GetPushTimeLive() const
{
	return m_nTimeToLive;
}

void EntHttp::SetPushTimeLive(int nValue)
{
	m_nTimeToLive = nValue;
}

bool EntHttp::IsProduction() const
{
	return m_bEnvProduction;
}

void EntHttp::SetProduction(bool bProduction)
{
	m_bEnvProduction = bProduction;
}

void EntHttp::SetReqResueNum(int nNum)
{
	m_nReqReuseNum = nNum;
	if (NULL != m_pIoDesc)
	{
		HttpIoDesc* pDesc = dynamic_cast<HttpIoDesc*>(m_pIoDesc);
		if (NULL != pDesc)
		{
			pDesc->SetReqReuseNum(nNum);
		}
	}
}

int EntHttp::GetReqResueNum()
{
	return m_nReqReuseNum;
}

void EntHttp::SetReqRetries(int nNum)
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

int EntHttp::GetReqRetries()
{
	return m_nReqRetries;
}

void EntHttp::AddReqDefHeader(const char* pszKey, const char* pszValue)
{
	if (StringUtil::CharIsNull(pszKey) || StringUtil::CharIsNull(pszValue))
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

void EntHttp::RemoveReqDefHeader(const char* pszKey)
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

void EntHttp::ClearReqDefHeaders()
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

bool EntHttp::LoadConfig()
{
	// 读取游戏配置
	const char *path = g_pCore->GetResourcePath();
	std::string strConfig(path);
	strConfig.append(PATH_CONFIG_NAME);
	
	Json::Value root;
	Json::Reader reader;

	std::ifstream ifs;
	ifs.open(strConfig);
	bool bOK = reader.parse(ifs, root);
	if (!bOK)
	{
		return false;
	}

	if (!root.isArray())
	{
		FS_LOG_FATAL_FORMAT("config struct error(path:%s)", strConfig.c_str());
		return false;
	}

	m_mapConfigs.clear();
	Json::Value::iterator itr = root.begin();
	for (; itr != root.end(); ++itr)
	{
		Json::Value& info = *itr;
		if (!info.isObject())
		{
			FS_LOG_FATAL_FORMAT("config data error(path:%s)", strConfig.c_str());
			return false;
		}

		int32_t nCata = info["type"].asInt();
		int32_t nCfgId = info["cfgid"].asInt();
		int64_t nKey = ((int64_t)nCata << 32) | nCfgId;
		config_info& cfg = m_mapConfigs[nKey];
		cfg.catalog = nCata;
		cfg.cfgid = nCfgId;
		cfg.name = info["name"].asString();
		cfg.key = info["key"].asString();
		cfg.path = info["path"].asString();
		cfg.tplpath = info["template"].asString();

		// 判断文件路径是否存在
		std::string strTpl = g_pCore->GetResourcePath();
		strTpl += cfg.tplpath;
		if (access(strTpl.c_str(), 0) != 0)
		{
			FS_LOG_FATAL_FORMAT("tpl not found(file:%s)", strTpl.c_str());
			return false;
		}
		std::string cfgPath = g_pCore->GetResourcePath();
		cfgPath += cfg.path;
		size_t nPos = cfgPath.find_last_of('/');
		if (nPos == std::string::npos)
		{
			nPos = cfgPath.find_last_of('\\');
		}
		if (nPos != std::string::npos)
		{
			cfgPath = cfgPath.substr(0, nPos);
		}
		if (access(cfgPath.c_str(), 0) != 0)
		{
			FS_LOG_FATAL_FORMAT("config path not found(path:%s)", cfgPath.c_str());
			return false;
		}
	}

	return true;
}


const config_info* EntHttp::GetConfigData(int32_t nCata, int32_t nCfgId, bool bAdaCataLog /*= true*/) const
{
	int64_t nKey = ((int64_t)nCata << 32) | nCfgId;

	std::map<int64_t, config_info>::const_iterator itr = m_mapConfigs.find(nKey);
	if (itr != m_mapConfigs.end())
	{
		return &itr->second;
	}
	if (bAdaCataLog)
	{
		nKey = (int64_t)nCata << 32;
		itr = m_mapConfigs.find(nKey);
		if (itr != m_mapConfigs.end())
		{
			return &itr->second;
		}
	}

	return NULL;
}

bool EntHttp::ParseConfigTplData(int32_t nCata, const Json::Value& root, std::map<int64_t, tpl_info>& tpldatas, int64_t version) const
{
	int32_t nCfgId = 0;
	if (root.isObject())
	{
		nCfgId = root["type"].asInt();
	}
	const config_info* pcfg = GetConfigData(nCata, nCfgId);
	if (NULL == pcfg)
	{
		FS_LOG_FATAL_FORMAT("not found tpl config(catalog:%d cfgid:%d)", nCata, nCfgId);
		return false;
	}

	int64_t nKey = ((int64_t)pcfg->catalog << 32) | pcfg->cfgid;
	tpl_info& tplInfo = tpldatas[nKey];
	tplInfo.cfg = pcfg;
	tplInfo.file_version = version;

	tplInfo.data["file_version"] = version;
	cpptempl::auto_data& p = tplInfo.data["configs"];
	return JsonToTplData(root, p);
}

bool EntHttp::JsonToTplData(const Json::Value& root, cpptempl::auto_data& data) const
{
	switch (root.type())
	{
	case Json::intValue:
	case Json::uintValue:
		data = root.asInt();
		break;
	case Json::realValue:
		data = root.asDouble();
		break;
	case Json::stringValue:
		data = root.asString();
		break;
	case Json::booleanValue:
		data = root.asBool();
		break;
	case Json::arrayValue:
		{
			Json::Value::const_iterator itr = root.begin();
			for (; itr != root.end(); ++itr)
			{
				cpptempl::auto_data& child = data.push_back( cpptempl::auto_data() );
				JsonToTplData(*itr, child);
			}
		}
		break;
	case Json::objectValue:
		{
			Json::Value::const_iterator itr = root.begin();
			for (; itr != root.end(); ++itr)
			{
				cpptempl::auto_data& child = data[ itr.memberName() ];
				JsonToTplData(*itr, child);
			}
		}
		break;
	default:
		break;
	}

	return true;
}

void EntHttp::HandleConfigTplData(const std::map<int64_t, tpl_info>& tpldatas) const
{
	std::string bak_path = g_pCore->GetResourcePath();
	bak_path.append(PATH_CONFIG_BAK);

	std::vector<ConfigLoadInfo> loadInfos;
	std::map<int64_t, tpl_info>::const_iterator itr = tpldatas.begin();
	for ( ; itr != tpldatas.end(); ++itr )
	{
		std::string tpl = g_pCore->GetResourcePath();
		tpl.append(itr->second.cfg->tplpath);
		
		std::ifstream in( tpl.c_str() );
		if (!in.is_open())
		{
			FS_LOG_FATAL_FORMAT("open file '%s' failed.", tpl.c_str());
			continue;
		}

		std::string strTplText((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		in.close();

		std::string ret = cpptempl::parse(strTplText, itr->second.data);

		std::string path = g_pCore->GetResourcePath();
		path.append(itr->second.cfg->path);

		std::ofstream out(path.c_str());
		if (!out.is_open())
		{
			FS_LOG_FATAL_FORMAT("open file '%s' failed.", path.c_str());
			continue;
		}
		out.write(ret.c_str(), ret.size());
		out.flush();
		out.close();

		ConfigLoadInfo info;
		info.key = itr->second.cfg->key;
		info.path = itr->second.cfg->path;
		info.catalog = itr->second.cfg->catalog;
		info.cfgid = itr->second.cfg->cfgid;
		loadInfos.push_back(info);
	}

	if (loadInfos.size() > 0)
	{
		m_pMainEntity->SendLoadConfigInfo(loadInfos);
	}
}

void EntHttp::OnInit()
{
	EntNetBase::OnInit();

	BindAction("HTTP", ACTION_BINDER(EntHttp::OnAction, this), 0);
	BindAction("HTTP", ACTION_BINDER(EntHttp::OnReply, this), 0);
}

INetIoDesc* EntHttp::GetIoDesc()
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
			.SetReqReuseNum(m_nReqReuseNum);
		pIoDesc->SetReqDefHeader("Accept-Encoding", "gzip, deflate, br");
		pIoDesc->SetReqDefHeader("Accept", "*/*");
	}

	return m_pIoDesc;
}

void EntHttp::OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
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


		FS_LOG_DEBUG_FORMAT("uri:%s\r\ntext:%s", req->GetURI(), req->GetText());

		const std::map<std::string, std::string>& headers = req->GetHeaders();
		FS_LOG_DEBUG_FORMAT("headers:(%d)", headers.size());
		for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
		{
			FS_LOG_DEBUG_FORMAT("\t%s:%s", itr->first.c_str(), itr->second.c_str());
		}

		res->AddHeader("Content-Type", "text/html");
		char* szRes = "<html><head><title>http test</title></head><body>sfdxdsfsdfsdfsdf</body></html>";

		res->SetText(szRes, strlen(szRes)).SetReason("OK").SetResult(200);
		action->SetState(EM_ACTION_REPLY);
#endif
	}
}

void EntHttp::OnReply(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	if ( action->IsResult() )
	{
		bool bHandled_ = false;
		int nProtocol = action->GetTag();
		switch ( nProtocol )
		{
		case PROTOCOL_HTTP_PROFILE:
			bHandled_ = OnRspProfile(action);
			break;
		case PROTOCOL_HTTP_NOTICE:
			bHandled_ = OnRspNotice(action);
			break;
		case PROTOCOL_HTTP_GIFT:
			bHandled_ = OnRspExchGift(action);
			break;
		case PROTOCOL_HTTP_PUSH_NOTIFICATION:
			bHandled_ = OnRspPushNotification(action);
			break;
		case PROTOCOL_HTTP_CONFIG_ACTIVITY:
			bHandled_ = OnRspConfig(action);
			break;
		case PROTOCOL_HTTP_MALL_ITEMS:
			bHandled_ = OnRspMallItems(action);
			break;
		case PROTOCOL_HTTP_MAIL_RESULT:
			bHandled_ = OnRspConfirmMail(action);
			break;
		case PROTOCOL_HTTP_GMCMD_RESULT:
			bHandled_ = OnRspConfirmCmd(action);
			break;
		case PROTOCOL_HTTP_PRODUCTS:
			bHandled_ = OnRspProducts(action);
			break;
		case PROTOCOL_HTTP_ORDER_PREPARE:
			bHandled_ = OnRspPreOrder(action);
			break;
		case PROTOCOL_HTTP_ORDER_STATE:
			bHandled_ = OnRspOrdeState(action);
			break;
		case PROTOCOL_HTTP_REBATE_INFO:
			bHandled_ = OnRspRebateInfo(action);
			break;
		case PROTOCOL_HTTP_REBATE_PICK:
			bHandled_ = OnRspRebateResult(action);
			break;
		default:
#ifdef TEST
			IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
			IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
			char szMsg[102400] = { 0 };
			FS_LOG_DEBUG_FORMAT("uri:%s", req->GetURI());
			FS_LOG_DEBUG_FORMAT("response:%s status:%d reason:%s", res->GetText(), res->GetResult(), res->GetReason() ? res->GetReason() : "unknow");

			const std::map<std::string, std::string>& headers = res->GetHeaders();
			FS_LOG_DEBUG_FORMAT("headers:(%d)", headers.size());
			for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
			{
				FS_LOG_DEBUG_FORMAT("\t%s:%s", itr->first.c_str(), itr->second.c_str());
			}
#else
			FS_LOG_WARN_FORMAT("protocol[id:%d] not handler", nProtocol);
#endif
			bHandled_ = true;
			break;
		}

		if (bHandled_)
		{
			// 从缓存中移除
			m_cacheActions.erase(action.get());
		}
		else
		{
			// 添加到缓存 定时发送
			FS_LOG_INFO_FORMAT("add req action to resent queue(tag:%d)", nProtocol);
			ResendAction& resent = m_cacheActions[action.get()];
			resent.action = action;
			resent.elapsed = 0;
		}
	}

#ifdef _DEBUG
	DumpAction(action);
#endif
}

bool EntHttp::OnRspProfile(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();
		Json::Value& data_ = *pdata_;

		SrvInfo _info;
		//_info.Identity = data_["id"].asInt64();
		_info.GameId = data_["gameId"].asInt64();
		//_info.ChannelId = data_["channelId"].asInt64();
		//_info.ProductionId = data_["envId"].asInt64();
		//_info.ServerId = data_["gameServerId"].asInt64();
		_info.AreaId = data_["serverId"].asInt64();
		_info.DeployId = data_["groupId"].asInt64();

		if ( _info.AreaId == 0
			|| _info.GameId == 0 )
		{
			FS_LOG_FATAL_FORMAT("init profile data error(raw data:%s)", res_->GetText());
			return true;
		}

		m_strGameid = StringUtil::Int64AsString(_info.GameId);
		m_strAreaId = StringUtil::Int64AsString(_info.AreaId);

		m_mapConfigIds.clear();
		Json::Value& configs_ = data_["configs"];
		if (!configs_.isArray() )
		{
			FS_LOG_FATAL_FORMAT("configs list not found(raw data:%s)", res_->GetText());
			return true;
		}

		Json::ValueIterator itr = configs_.begin();
		for (; itr != configs_.end(); ++itr)
		{
			Json::Value& item_ = *itr;

			int nId_ = item_["id"].asInt();
			std::string& code_ = item_["code"].asString();
			m_mapConfigIds[nId_] = code_;
		}

		// 第一次成功后读取所有通知
		//this->OnQueryAllData();
		FS_LOG_INFO("init profile succeed!!!");
		printf("init profile succeed!!!");
		m_pMainEntity->OnProfileInited(_info);

		EXCEPTION_END();
	}

	// 心跳执行， 总是返回true即可
	return true;
}

bool EntHttp::OnRspNotice(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;
		if (!data_.isArray())
		{
			FS_LOG_FATAL("parse notice result failed, object not is array");
			return true;
		}

		Json::ValueIterator itr = data_.begin();
		for (; itr != data_.end(); ++itr)
		{
			Json::Value& notice = *itr;
			NoticeData data;
			data.Name = notice["title"].asString();
			data.Content = notice["content"].asString();
			data.NoticeId = notice["id"].asInt64();
			data.CreateTime = notice["startAt"].asInt64() / 1000;		// 毫秒转秒
			data.Interval = notice["interval"].asInt64();
			data.Loop = notice["times"].asInt();
			data.Type = notice["type"].asInt();
			data.EndTime = notice["endAt"].asInt64() / 1000;		// 毫秒转秒

			// 超过时间输出日志就不发送了
			int64_t tNow = util_get_utc_time();
			if ( data.EndTime <= tNow )
			{
				FS_LOG_WARN_FORMAT("notice had expired(id:%d content:%s start:%lld duration:%lld)", 
						data.NoticeId, data.Name.c_str(), data.CreateTime, data.EndTime);
				continue;
			}

			m_pMainEntity->SendNotice(data);
		}

		EXCEPTION_END();
	}
	else if (nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		if ( nCode_ == EM_HTTP_CODE_TIMEOUT )
		{
			return false;
		}
	}

	return true;
}

bool EntHttp::OnRspExchGift(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;

		const std::shared_ptr<HttpData>& _http_data = req_->GetAttachData();
		assert(_http_data.get());
		if (!_http_data.get())
		{
			FS_LOG_FATAL("exch gift data error, don't save http data.");
			return true;
		}

		ExchGiftResult rsp;
		std::string content = data_.asString();

		CVarList args;
		StringUtil::SplitString(args, content, ";");
		const char* pszCapitals = args.StringVal(0);
		const char* pszItems = args.StringVal(1);
		const char* pszKey = _http_data->GetCString("code");
		const char* pszRoleId = _http_data->GetCString("roleId");
		rsp.capitals = NULL == pszCapitals ? "" : pszCapitals;
		rsp.items = NULL == pszItems ? "" : pszItems;
		rsp.key = NULL == pszKey ? "" : pszKey;
		rsp.roleId = NULL == pszRoleId ? "" : pszRoleId;
		rsp.result = EM_GIFT_CODE_SUCCESS;
		m_pMainEntity->SendExchGiftItems(rsp);

		EXCEPTION_END();
	}
	else if ( NULL != req_ )
	{
		EXCEPTION_BEGIN();

		const std::shared_ptr<HttpData>& _http_data = req_->GetAttachData();
		assert(_http_data.get());
		if (!_http_data.get())
		{
			FS_LOG_FATAL("exch gift data error, don't save http data.");
			return true;
		}

		const char* pszAccid = _http_data->GetCString("accId");
		const char* pszRoleid = _http_data->GetCString("roleId");
		const char* pszKey = _http_data->GetCString("code");
		if (NULL != pszRoleid && NULL != pszKey)
		{
			ExchGiftResult rsp;
			rsp.roleId = pszRoleid;
			rsp.key = pszKey;
			rsp.result = GetGiftErrorCode(root_["code"].asString().c_str());
			m_pMainEntity->SendExchGiftItems(rsp);
		}
					
		EXCEPTION_END();
	}

	// 失败后直接返回不重发
	return true;
}

bool EntHttp::OnRspProducts(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;
		if (!data_.isArray())
		{
			FS_LOG_FATAL_FORMAT("parse product list failed(reason:goods not is json array, data:%s)", res_->GetText());
			return true;
		}

		std::vector<Product> products_;
		Json::Value::iterator itr = data_.begin();
		for (; itr != data_.end(); ++itr)
		{
			Json::Value& goods_ = *itr;
			Json::Value& code_ = goods_["code"];
			Json::Value& price_ = goods_["price"];
			Json::Value& items_ = goods_["items"];
			if (code_.isNull() || price_.isNull() || !items_.isArray())
			{
				FS_LOG_FATAL_FORMAT("product data error, please check goods list config[data:%s]", res_->GetText());
				return true;
			}

			size_t nIndex = products_.size();
			products_.resize(nIndex + 1);
			Product& product_ = products_[nIndex];

			product_.ProductId = code_.asString();
			product_.Price = price_.asFloat();
			product_.Rule = EnumIndex<EmProductRule>(goods_["buyRule"].asString().c_str());
			product_.State = EnumIndex<EmProductState>(goods_["state"].asString().c_str());

			product_.Times = goods_["buyTimes"].asInt();

			// 读取物品
			if (!ParseGoodsItem(items_, product_.Items))
			{
				FS_LOG_FATAL_FORMAT("parse product items failed[data:%s]", res_->GetText());
				return true;
			}

			// 读取首充礼包
			Json::Value& gifts_ = goods_["firstGiftItems"];
			if (gifts_.isArray())
			{
				if (!ParseGoodsItem(gifts_, product_.Gifts))
				{
					FS_LOG_FATAL_FORMAT("parse product first gifts failed[data:%s]", res_->GetText());
					return true;
				}
			}

			// 读取购买赠送
			Json::Value& normals_ = goods_["normalGiftItems"];
			if (normals_.isArray())
			{
				if (!ParseGoodsItem(normals_, product_.Rewards))
				{
					FS_LOG_FATAL_FORMAT("parse product normal gifts failed[data:%s]", res_->GetText());
					return true;
				}
			}

			// 读取达到次数限制后的额外礼包
			Json::Value& extras_ = goods_["secondGiftItems"];
			if (extras_.isArray())
			{
				if (!ParseGoodsItem(extras_, product_.Extra))
				{
					FS_LOG_FATAL_FORMAT("parse product extra gifts failed[data:%s]", res_->GetText());
					return true;
				}
			}
		}

		m_pMainEntity->SendProducts(products_);

		EXCEPTION_END();
	}
	else if ( nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		if ( EM_HTTP_CODE_TIMEOUT == nCode_ )
		{
			return false;
		}
	}

	return true;
}

bool EntHttp::OnRspPreOrder(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == req_)
	{
		return true;
	}

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;

	ProductOderInfo order;
	// 获取发送信息
	order.RoleId = req_->GetHeader("roleId");
	order.ProductId = req_->GetHeader("productId");
	order.State = EM_ORDER_STATE_UNKOWN;
	order.PayType = EM_BILL_TYPE_MAX_END;


	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;

		order.RoleId = data_["roleId"].asString();
		order.ProductId = data_["goodsCode"].asString();
		order.State = EnumIndex<EmOrderState>(data_["state"].asString().c_str());
		order.PayType = EnumIndex<EmBillType>(data_["type"].asString().c_str());
		order.OrderId = data_["orderId"].asString();
		order.Extra = data_["extra"].asString();
		order.Price = data_["price"].asFloat();
		EXCEPTION_END();
	}
	
	// 超时或错误直接放弃, 有订单号当作成功
	m_pMainEntity->SendProductOrder(order);
	return true;
}

bool EntHttp::OnRspOrdeState(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		return false;
	}
	return true;
}

bool EntHttp::OnRspRebateInfo(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == req_)
	{
		return true;
	}

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;

	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;
		std::string rid_ = data_["roleId"].asString();
		int nType_ = data_["type"].asInt();
		int nAmount_ = data_["amount"].asInt();

		m_pMainEntity->SendPayedRebateInfo(rid_, nType_, nAmount_);
		EXCEPTION_END();
	}

	// 超时或错误直接放弃
	return true;
}

bool EntHttp::OnRspRebateResult(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());
	if (NULL == req_)
	{
		return true;
	}

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;

	int nRebateResult_ = EM_REBATE_RESULT_ERROR;
	std::string rid_;
	int nRebate_ = 0;

	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;
		rid_ = data_["roleId"].asString();
		nRebate_ = data_["amount"].asInt();
		bool bResult_ = data_["result"].asBool();
		nRebateResult_ = bResult_ ? EM_REBATE_RESULT_SUCCEED : EM_REBATE_RESULT_PICKED;

		EXCEPTION_END();
	}
	else
	{
		FS_LOG_ERROR("request pick rebate occur error.");
	}

	if (rid_.empty())
	{
		std::shared_ptr<HttpData> _http_data = req_->GetAttachData();
		const char* pid_ = _http_data->GetCString("roleId");
		if (NULL != pid_) rid_ = pid_;
	}

	if (!rid_.empty())
	{
		m_pMainEntity->SendPayedRebateResult(rid_, nRebateResult_, nRebate_);
	}
	else
	{
		FS_LOG_ERROR("request pick rebate request http data error.");
	}

	// 超时或错误直接放弃
	return true;
}

bool EntHttp::OnRspPushNotification(std::shared_ptr<INetAction>& action)
{
	// 通知结果处理
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
	assert(req != NULL);
	assert(res != NULL);
	if (NULL == req || res == NULL)
	{
		FS_LOG_FATAL("http data error.");
		return true;
	}

	int nResult = res->GetResult();
	FS_LOG_INFO_FORMAT("on push notification result:%d", nResult);
	if (nResult == EM_HTTP_CODE_OK)
	{
		FS_LOG_DEBUG("on push notification success");
		// PENDING: 暂时什么也不用做
	}
	else if ( nResult == 429 )
	{
		// 过多的请求 请求超出了频率限制。相应的描述信息会解释具体的原因。
		FS_LOG_INFO("too more push request");
		FS_LOG_ERROR_FORMAT("push notification failed, result:%d content:%s", nResult, res->GetText());
		// PENDING: 暂时什么也不用做
	}
	else
	{
		FS_LOG_ERROR_FORMAT("push notification failed, result:%d content:%s", nResult, res->GetText());
		if (EM_HTTP_CODE_TIMEOUT == nResult)
		{
			// 添加到缓存 定时发送
			return false;
		}
	}

	return true;
}

bool EntHttp::OnRspConfig(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();

		Json::Value& data_ = *pdata_;

		if (!data_.isArray())
		{
			FS_LOG_FATAL_FORMAT("config data error, obj not is object(data:%s)", res_->GetText());
			return true;
		}

		const std::shared_ptr<HttpData>& attr_ = req_->GetAttachData();
		int nCate_ = attr_->GetInt("catelog");
		int64_t version = 0;// data["version"].asInt64();
		std::map<int64_t, tpl_info> tpldatas;

		if (!ParseConfigTplData(nCate_, data_, tpldatas, version))
		{
			FS_LOG_FATAL_FORMAT("parse config tpl data failed(context:%s)", res_->GetText());
			return true;
		}

		// 处理模板数据
		HandleConfigTplData(tpldatas);

		EXCEPTION_END();
	}
	else if ( nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		if (EM_HTTP_CODE_TIMEOUT == nCode_ )
		{
			return false;
		}
	}

	return true;
}

bool EntHttp::OnRspMallItems(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_SUCCEED)
	{
		assert(NULL != pdata_);
		EXCEPTION_BEGIN();
		Json::Value& data_ = *pdata_;

		if (!data_.isObject())
		{
			FS_LOG_FATAL_FORMAT("mall items data error, obj not is object(data:%s)", res_->GetText());
			return true;
		}

		// 用空的替代， 商城列表未用到, 只在活动表中加了一行管理时间的数据
		std::wstring name_;// = StringUtil::StringAsWideStr(root["Name"].asString().c_str());
		std::wstring desc_;// = StringUtil::StringAsWideStr(root["Desc"].asString().c_str());
		int64 startTime = 0;
		int64 endTime = 0;
		//std::string strStartTime = root["StartTime"].asString();
		//std::string strEndTime = root["EndTime"].asString();
		//util_convert_string_to_time(strStartTime.c_str(), &startTime);
		//util_convert_string_to_time(strEndTime.c_str(), &endTime);

		// 7天内显示为新品
		const time_t T_SECONDS_OF_DAY = 24 * 60 * 60;
		std::vector<ShopItem> shopItems;
		std::vector<ShopTag> catalogs;
		// 读取分类
		Json::Value& tags_ = data_["types"];
		if (!tags_.isArray())
		{
			FS_LOG_ERROR_FORMAT("mall types data error(data:%s)", res_->GetText());
			return true;
		}
		for (Json::Value::iterator itr = tags_.begin(); itr != tags_.end(); ++itr)
		{
			Json::Value& item = *itr;
			if (!item.isObject())
			{
				FS_LOG_FATAL_FORMAT("mall types data error, item not is object (data:%s)", res_->GetText());
				return true;
			}
			ShopTag tag_;
			tag_.id = item["id"].asInt();
			tag_.name = StringUtil::StringAsWideStr(item["name"].asString().c_str());
			catalogs.push_back(tag_);
		}

		Json::Value& shops_ = data_["shops"];
		if (!shops_.isArray())
		{
			FS_LOG_ERROR_FORMAT("mall item data error(data:%s)", res_->GetText());
			return true;
		}
		for (Json::Value::iterator itr = shops_.begin(); itr != shops_.end(); ++itr)
		{
			Json::Value& item = *itr;
			if (!item.isObject())
			{
				FS_LOG_FATAL_FORMAT("activity item list data error, item not is object (data:%s)", res_->GetText());
				return true;
			}

			ShopItem sItem;

			sItem.id = item["code"].asString();
			sItem.name = StringUtil::StringAsWideStr(item["name"].asString().c_str());
			sItem.desc = StringUtil::StringAsWideStr(item["description"].asString().c_str());
			sItem.goods = item["items"].asString();
			sItem.price = item["price"].asInt();
			sItem.color = item["color"].asInt();
			sItem.count = item["count"].asInt();
			sItem.level = item["level"].asInt();
			sItem.vip = item["vip"].asInt();
			sItem.purchaseNum = item["number"].asInt();
			sItem.refreshType = item["refresh"].asInt();
			sItem.discount = item["discount"].asInt();
			sItem.catalog = item["typeId"].asInt();

			sItem.onShowTime = item["showStartAt"].asInt64() / 1000;
			sItem.offShowTime = item["showEndAt"].asInt64() / 1000;
			sItem.onSellTime = item["sellStartAt"].asInt64() / 1000;
			sItem.offSellTime = item["sellEndAt"].asInt64() / 1000;

			if (sItem.id.empty() || sItem.name.empty() || sItem.goods.empty() || sItem.price <= 0 )
			{
				FS_LOG_WARN_FORMAT("activity item list content invalid, plase check again(content:%s)?", item.toStyledString().c_str());
				continue;
			}

			int nNewProduct = item["newProduct"].asInt();
			if (nNewProduct > 0)
			{
				sItem.newGoodTime = sItem.onSellTime + T_SECONDS_OF_DAY * nNewProduct;
			}
			else
			{
				sItem.newGoodTime = 0;
			}
			if (sItem.discount > 0)
			{
				int nRealPrice = (int)(sItem.price * sItem.discount / 100);
				FS_LOG_INFO_FORMAT("activity item occur discount(id:%s orig_price:%d real_price:%d discount:%d)",
					sItem.id.c_str(), sItem.price, nRealPrice, sItem.discount);
				sItem.price = nRealPrice;
			}

			shopItems.push_back(sItem);
		}

		if (shopItems.size() == 0)
		{
			FS_LOG_WARN("activity item list is empty, are you sure?");
		}
		m_pMainEntity->ShopPushItem(name_, desc_, startTime, endTime, catalogs, shopItems);

		EXCEPTION_END();
	}
	else if (nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		if (EM_HTTP_CODE_TIMEOUT == nCode_)
		{
			return false;
		}
	}

	return true;
}

bool EntHttp::OnRspConfirmMail(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		return false;
	}
	return true;
}

bool EntHttp::OnRspConfirmCmd(std::shared_ptr<INetAction>& action)
{
	IHttpRequest* req_ = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res_ = dynamic_cast<IHttpResponse*>(action->Response());

	Json::Reader reader_;
	Json::Value root_;
	Json::Value* pdata_ = NULL;
	int nCode_ = -1;
	int nResult_ = HandleCommonRsp(req_, res_, reader_, root_, &pdata_, nCode_, MY_FUNCTION_NAME);
	if (nResult_ == EM_RESULT_DATA_HTTP_ERR)
	{
		return false;
	}
	return true;
}

/* 用mall代替
void EntHttp::OnSetShopItemList(Json::Value& root)
{
	FS_LOG_INFO("on set activity item list");
	Json::Value& data = root["Content"];
	if (!data.isArray())
	{
		FS_LOG_FATAL_FORMAT("activity item list data error, Content not is array(data:%s)", root.toStyledString().c_str());
		return;
	}

	EXCEPTION_BEGIN();
	std::wstring name = StringUtil::StringAsWideStr(root["Name"].asString().c_str());
	std::wstring desc = StringUtil::StringAsWideStr(root["Desc"].asString().c_str());
	int64 startTime = 0;
	int64 endTime = 0;
	std::string strStartTime = root["StartTime"].asString();
	std::string strEndTime = root["EndTime"].asString();
	util_convert_string_to_time(strStartTime.c_str(), &startTime);
	util_convert_string_to_time(strEndTime.c_str(), &endTime);

	// 7天内显示为新品
	const time_t T_SECONDS_OF_DAY = 24 * 60 * 60;
	std::vector<ShopItem> shopItems;
	std::map<std::wstring, int> catalogs;
	for (Json::Value::iterator itr = data.begin(); itr != data.end(); ++itr)
	{
		Json::Value& item = *itr;
		if (!item.isObject())
		{
			FS_LOG_FATAL_FORMAT("activity item list data error, item not is object (data:%s)", root.toStyledString().c_str());
			return;
		}

		ShopItem sItem;

		sItem.id = item["ID"].asString();
		sItem.name = StringUtil::StringAsWideStr(item["Name"].asString().c_str());
		sItem.desc = StringUtil::StringAsWideStr(item["Desc"].asString().c_str());
		sItem.goods = item["Goods"].asString();
		sItem.price = item["Price"].asInt();
		sItem.color = item["Color"].asInt();
		sItem.count = item["Count"].asInt();
		sItem.level = item["Level"].asInt();
		sItem.vip = item["VIP"].asInt();
		sItem.purchaseNum = item["PurchaseNum"].asInt();
		sItem.refreshType = item["Refresh"].asInt();
		sItem.discount = item["Discount"].asInt();

		// 分类处理
		std::wstring strCatalog = StringUtil::StringAsWideStr(item["Catalog"].asString().c_str());
		std::map<std::wstring, int>::iterator mit = catalogs.find(strCatalog);
		if (mit == catalogs.end())
		{
			sItem.catalog = (int32_t)catalogs.size() + 1;
			catalogs.insert(std::make_pair(strCatalog, sItem.catalog));
		}
		else
		{
			sItem.catalog = mit->second;
		}

		sItem.onShowTime = 0;
		sItem.offShowTime = 0;
		sItem.onSellTime = 0;
		sItem.offSellTime = 0;

		std::string strOnShowTime = item["OnShowTime"].asString();
		std::string strOffShowTime = item["OffShowTime"].asString();
		std::string strOnSellTime = item["OnSellTime"].asString();
		std::string strOffSellTime = item["OffSellTime"].asString();
		util_convert_string_to_time(strOnShowTime.c_str(), &sItem.onShowTime);
		util_convert_string_to_time(strOffShowTime.c_str(), &sItem.offShowTime);
		util_convert_string_to_time(strOnSellTime.c_str(), &sItem.onSellTime);
		util_convert_string_to_time(strOffSellTime.c_str(), &sItem.offSellTime);

		if (sItem.id.empty() || sItem.name.empty() || sItem.goods.empty() || sItem.price <= 0 ||
			0 == sItem.onShowTime || 0 == sItem.offShowTime || 0 == sItem.onSellTime || 0 == sItem.offSellTime)
		{
			FS_LOG_WARN_FORMAT("activity item list content invalid, plase check again(content:%s)?", item.toStyledString().c_str());
			continue;
		}

		int nNewProduct = item["NewProduct"].asInt();
		if (nNewProduct > 0)
		{
			sItem.newGoodTime = sItem.onSellTime + T_SECONDS_OF_DAY * nNewProduct;
		}
		else
		{
			sItem.newGoodTime = 0;
		}
		if (sItem.discount > 0)
		{
			int nRealPrice = (int)(sItem.price * sItem.discount / 100);
			FS_LOG_INFO_FORMAT("activity item occur discount(id:%s orig_price:%d real_price:%d discount:%d)",
				sItem.id.c_str(), sItem.price, nRealPrice, sItem.discount);
			sItem.price = nRealPrice;
		}

		shopItems.push_back(sItem);
	}
	if (shopItems.size() == 0)
	{
		FS_LOG_WARN("activity item list is empty, are you sure?");
	}

	m_pMainEntity->ShopPushItem(name, desc, startTime, endTime, catalogs, shopItems);

	EXCEPTION_END();
}
*/

void EntHttp::OnSetLevelingConfig(Json::Value& root)
{
	FS_LOG_INFO("on set leveling config");
	Json::Value& actList = root["Content"];
	if (!actList.isArray())
	{
		FS_LOG_FATAL_FORMAT("leveling list data error, Content not is object(data:%s)", actList.toStyledString().c_str());
		return;
	}

	EXCEPTION_BEGIN();
	ActivityInfo info;
	info.actType = EM_ACT_TYPE_PLAYER_LEVEL_MATCH;
	info.name = StringUtil::StringAsWideStr(root["Name"].asString().c_str());
	info.desc = StringUtil::StringAsWideStr(root["Desc"].asString().c_str());
	std::string strStartTime = root["StartTime"].asString();
	std::string strEndTime = root["EndTime"].asString();
	util_convert_string_to_time(strStartTime.c_str(), &info.startTime);
	util_convert_string_to_time(strEndTime.c_str(), &info.endTime);

	for (Json::Value::iterator itr = actList.begin(); itr != actList.end(); ++itr)
	{
		Json::Value& item = *itr;
		if (!item.isObject())
		{
			FS_LOG_FATAL_FORMAT("leveling list data error, item not is object (data:%s)", root.toStyledString().c_str());
			return;
		}

		ActivityItem sItem;

		sItem.value = item["ID"].asInt();
		sItem.name = StringUtil::StringAsWideStr(item["Name"].asString().c_str());
		sItem.goods = item["Goods"].asString();
		sItem.count = item["Count"].asInt();

		if (sItem.value <= 0 || sItem.name.empty() || sItem.goods.empty())
		{
			FS_LOG_WARN_FORMAT("leveling list content invalid, plase check again(content:%s)?", item.toStyledString().c_str());
			continue;
		}

		info.items.push_back(sItem);
	}
	if (info.items.size() == 0)
	{
		FS_LOG_WARN("leveling item list is empty, are you sure?");
	}

	m_pMainEntity->SetActivityConfig(info);

	EXCEPTION_END();
}

void EntHttp::OnReqProfile()
{
	FS_LOG_INFO("on http request profile");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	InitRequest(m_urlProfile, action, PROTOCOL_HTTP_PROFILE, TIMEOUT_OF_PROFILE_REQUEST);
	if (NULL != m_pService)
	{
		m_pService->Send(NULL_NID, action);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqExchGift(const ExchGiftInfo& info)
{
	FS_LOG_INFO_FORMAT("on http request gift(accid:%s roleid:%s key:%s)", 
							info.account.c_str(), info.roleid.c_str(), info.key.c_str());

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlGift, action, PROTOCOL_HTTP_GIFT, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	HttpData* _pHttpData = new HttpData(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
	std::shared_ptr<HttpData> _http_data( _pHttpData );
	_http_data->AddValue("code", info.key.c_str());
	_http_data->AddValue("accId", info.account.c_str());
	_http_data->AddValue("roleId", info.roleid.c_str());

	std::string _http_text;
	_http_data->toString(_http_text);
	request->SetText(_http_text.c_str(), _http_text.size());
	request->AttachHttpData(_http_data);

	if (NULL != m_pService)
	{
		m_pService->Send(NULL_NID, std::shared_ptr<INetAction>(action));
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqNotify(const NotifyInfo& notify)
{
	RequestNotifyData( &notify );
}

void EntHttp::OnReqAllData()
{
	// 请求通知
	RequestNotifyData( NULL );
	// 请求商城配置
	OnReqMallItems();
	// 请求商品列表
	OnReqProducts();

	// 请求活动配置
	for (auto itr = m_mapConfigIds.begin(); itr != m_mapConfigIds.end(); ++itr)
	{
		OnReqConfig( itr->first );
	}
}

void EntHttp::OnReqProducts()
{
	FS_LOG_INFO("on http request all products");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlGoods, action, PROTOCOL_HTTP_PRODUCTS, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	HttpData _http_data(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
	_http_data.AddValue("gameId", m_strGameid.c_str());
	std::string _http_text;
	_http_data.toString(_http_text);
	request->SetText(_http_text.c_str(), _http_text.size());

	if (NULL != m_pService)
	{
		this->m_pService->Send(NULL_NID, action);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnNotifyOrder(const OrderNotify& notify)
{
	FS_LOG_INFO("on http request notify order state");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlOrderState, action, PROTOCOL_HTTP_ORDER_STATE, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	try
	{
		// PATCH /api/v1/order/{id}/state/{state}
		char szUrl[512] = { 0 };
		SPRINTF_S(szUrl, m_urlOrderState.url.c_str(), notify.OrderId.c_str(), EnumString<EmOrderState>(notify.State));
		request->SetURI(szUrl);
	}
	catch ( ... )
	{
		FS_LOG_ERROR("order state url format error, should like '/api/v1/order/%%s/state/%%s'");
		return;
	}

	if (NULL != m_pService)
	{
		this->m_pService->Send(NULL_NID, action);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqPreOrder(const ProductOrder& order)
{
	FS_LOG_INFO("on http request prepare order");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlPreOrder, action, PROTOCOL_HTTP_ORDER_PREPARE, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	request->AddHeader("Content-Type", "application/json");

	// 保存角色和产品id处理超时
	request->AddHeader("roleId", order.RoleId.c_str());
	request->AddHeader("productId", order.ProductId.c_str());

	Json::Value _root;
	// 填充数据
	_root["clientToken"] = order.Token;
	_root["goodsCode"] = order.ProductId;
	_root["roleId"] = order.RoleId;
	_root["roleName"] = order.RoleName;
	_root["serverId"] = m_strAreaId;

	std::string _http_text = _root.toStyledString();
	request->SetText(_http_text.c_str(), _http_text.size());

	if (NULL != m_pService)
	{
		this->m_pService->Send(NULL_NID, action);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::RequestNotifyData(const NotifyInfo* pNotify)
{
	if (NULL == pNotify)
	{
		FS_LOG_INFO("on http request all notify");
	}
	else
	{
		FS_LOG_INFO_FORMAT("on http request notify(ids:%s)", pNotify->Content.c_str());
	}

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlNotice, action, PROTOCOL_HTTP_NOTICE, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	if (NULL != pNotify)
	{
		HttpData _http_data(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
		_http_data.AddValue("id", pNotify->Content.c_str());
		std::string _http_text;
		_http_data.toString(_http_text);
		request->SetText(_http_text.c_str(), _http_text.size());
	}

	if (NULL != m_pService)
	{
		this->m_pService->Send(NULL_NID, action);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqPushNotification(const PushData& data)
{
	FS_LOG_DEBUG("on http push notification");

	std::shared_ptr<INetAction> actoin = std::make_shared<HttpAction>();
	IHttpRequest* request = dynamic_cast<IHttpRequest*>(actoin->Request());
	assert(NULL != request);

	actoin->SetTag(PROTOCOL_HTTP_PUSH_NOTIFICATION);
	request->SetURI(m_strUrlPush.c_str());
	request->SetMethod(EM_HTTP_METHOD_POST);
	request->SetReqTimeout(TIMEOUT_OF_HTTP_REQUEST);

	request->AddHeader("Accept", "application/json");
	request->AddHeader("Content-Type", "application/json");
	if (m_strPushAuthString.empty())
	{
		std::string _baseText = m_strPushKey;
		_baseText.append(":");
		_baseText.append(m_strPushSecret);
		
		m_strPushAuthString = "Basic ";
		std::string authString;
		authString.resize(_baseText.size() * 2 + 1);
		base64_encode((unsigned char*)&authString[0],
									(const unsigned char*)_baseText.c_str(), _baseText.size());
		m_strPushAuthString.append(authString);

	}
	request->AddHeader("Authorization", m_strPushAuthString.c_str());

	Json::Value _root;
	FillPushData(_root, data);
	std::string _http_text = _root.toStyledString();

	request->SetText(_http_text.c_str(), _http_text.size());
	if (NULL != m_pService)
	{
		this->m_pService->Send(NULL_NID, actoin);
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqConfig( int nType )
{
	FS_LOG_DEBUG("on http request mall item list");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlAcitvity, action, PROTOCOL_HTTP_CONFIG_ACTIVITY, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	if (NULL != m_pService)
	{
		// 重构url
		char szUrl[512] = { 0 };
		try
		{
			std::string& strCate_ = m_mapConfigIds[nType];
			if (strCate_.empty())
			{
				FS_LOG_FATAL_FORMAT("cate not found(cate:%d).", nType);
				return;
			}
			SPRINTF_S(szUrl, m_urlAcitvity.url.c_str(), strCate_.c_str() );
			request->SetURI(szUrl);
		}
		catch (...)
		{
			FS_LOG_FATAL_FORMAT("url format error(uri:%s).", request->GetURI());
			return;
		}

		std::shared_ptr<HttpData> data = std::make_shared<HttpData>();
		data->AddValue("catelog", nType);
		request->AttachHttpData(data);

		if (this->m_pService->Send(NULL_NID, action))
		{
			ResendAction& resent = m_cacheActions[action.get()];
			resent.action = action;
			resent.elapsed = 0;
		}
		else
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqMallItems()
{
	FS_LOG_DEBUG("on http request mall item list");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlMall, action, PROTOCOL_HTTP_MALL_ITEMS, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	if (NULL != m_pService)
	{
		if (this->m_pService->Send(NULL_NID, action))
		{
			ResendAction& resent = m_cacheActions[action.get()];
			resent.action = action;
			resent.elapsed = 0;
		}
		else
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqConfirmMail(int64_t nMailId, int32_t nResult, bool bIsGlobal, const std::map<std::string, int32_t>& resultMap)
{
	FS_LOG_DEBUG("OnReqConfirmMail");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlMailConfirm, action, PROTOCOL_HTTP_MAIL_RESULT, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);
	request->AddHeader("Content-Type", "application/json");

	Json::Value _root;
	// 填充数据
	_root["entityId"] = nMailId;
	_root["result"] = nResult;
	_root["global"] = bIsGlobal;

	if (!bIsGlobal)
	{
		assert(resultMap.size() > 0);
		Json::Value& roleMap = _root["detail"];
		for (std::map<std::string, int32_t>::const_iterator itr = resultMap.begin();
			itr != resultMap.end(); ++itr)
		{
			Json::Value roleResult;
			Json::Value& rRole = roleMap.append(roleResult);
			rRole["roleId"] = itr->first;
			rRole["result"] = itr->second;
		}
	}
	std::string _http_text = _root.toStyledString();
	request->SetText(_http_text.c_str(), _http_text.size());
	if (NULL != m_pService)
	{
		if (this->m_pService->Send(NULL_NID, action))
		{
			ResendAction& resent = m_cacheActions[action.get()];
			resent.action = action;
			resent.elapsed = 0;
		}
		else
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqConfirmGm(int64_t nCmdId, int32_t nResult, const std::string& roleid, const std::string& message)
{
	FS_LOG_DEBUG("OnReqConfirmGm");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlCmdConfirm, action, PROTOCOL_HTTP_GMCMD_RESULT, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);
	request->AddHeader("Content-Type", "application/json");

	Json::Value _root;
	// 填充数据
	_root["entityId"] = nCmdId;
	_root["result"] = nResult;
	_root["message"] = message;

	Json::Value& roleMap = _root["detail"];
	Json::Value roleResult;
	Json::Value& rRole = roleMap.append(roleResult);
	rRole["roleId"] = roleid;
	rRole["result"] = nResult;

	std::string _http_text = _root.toStyledString();
	request->SetText(_http_text.c_str(), _http_text.size());
	if (NULL != m_pService)
	{
		if (this->m_pService->Send(NULL_NID, action))
		{
			ResendAction& resent = m_cacheActions[action.get()];
			resent.action = action;
			resent.elapsed = 0;
		}
		else
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqPayedRebate(const std::string& uid, const std::string& rid)
{
	FS_LOG_DEBUG("OnReqPayedRebate");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlRebateInfo, action, PROTOCOL_HTTP_REBATE_INFO, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);

	HttpData _http_data(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
	_http_data.AddValue("accountId", uid.c_str());
	_http_data.AddValue("roleId", rid.c_str());

	std::string _http_text;
	_http_data.toString(_http_text);
	request->SetText(_http_text.c_str(), _http_text.size());
	if (NULL != m_pService)
	{
		if (!this->m_pService->Send(NULL_NID, action))
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::OnReqPayedRebatePick(const std::string& uid, const std::string& rid, const std::wstring& name)
{
	FS_LOG_DEBUG("OnReqPayedRebate");

	std::shared_ptr<INetAction> action = std::make_shared<HttpAction>();
	IHttpRequest* request = InitRequest(m_urlRebatePick, action, PROTOCOL_HTTP_REBATE_PICK, TIMEOUT_OF_HTTP_REQUEST);
	assert(NULL != request);
	request->AddHeader("Content-Type", "application/json");

	HttpData* pd_ = new HttpData(std::shared_ptr<IHttpFormater>(new HttpDataFormater()));
	std::shared_ptr<HttpData> _http_data(pd_);
	_http_data->AddValue("roleId", rid.c_str());
	request->AttachHttpData(_http_data);

	Json::Value _root;
	// 填充数据
	_root["accountId"] = uid;
	_root["roleId"] = rid;
	_root["roleName"] = StringUtil::WideStrAsUTF8String(name.c_str());

	std::string _http_text = _root.toStyledString();
	request->SetText(_http_text.c_str(), _http_text.size());
	if (NULL != m_pService)
	{
		if (!this->m_pService->Send(NULL_NID, action))
		{
			FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", request->GetURI());
			DumpAction(action);

			// 按失败返回给游戏服务器
			if (NULL != m_pMainEntity)
			{
				m_pMainEntity->SendPayedRebateResult(rid, EM_REBATE_RESULT_ERROR, 0);
			}
		}
	}
	else
	{
		FS_LOG_ERROR("http server not start");
	}
}

void EntHttp::FillPushData(Json::Value& root, const PushData& data) const
{
	// 推送平台
	if (data.recvierPlatform == EM_CLIENT_ALL)
	{
		root["platform"] = "all";
	}
	else
	{
		Json::Value& _platform = root["platform"];
		if ( (data.recvierPlatform & EM_CLIENT_IOS) == EM_CLIENT_IOS)
		{
			_platform.append("ios");
		}
		if ( (data.recvierPlatform & EM_CLIENT_ANDROID) == EM_CLIENT_ANDROID)
		{
			_platform.append("android");
		}
		if ( (data.recvierPlatform & EM_CLIENT_WINPHONE) == EM_CLIENT_WINPHONE)
		{
			_platform.append("winphone");
		}
	}

	// 推送目标
	if (data.recivers.size() == 0)
	{
		// 广播
		root["audience"] = "all";
	}
	else
	{
		Json::Value& _audience = root["audience"];
		for (std::map<int, std::vector<std::string> >::const_iterator itr = data.recivers.begin();
								itr != data.recivers.end(); ++itr )
		{
			Json::Value* _pRecivers = NULL;
			switch ( itr->first )
			{
			case EM_PUSH_RECVIER_ALIAS:
				_pRecivers = &_audience["alias"];
			break;
			case EM_PUSH_RECVIER_TAG:
				_pRecivers = &_audience["tag"];
				break;
			case EM_PUSH_RECVIER_REGISTRATIONID:
				_pRecivers = &_audience["registration_id"];
				break;
			default:
				break;
			}

			if (NULL == _pRecivers)
			{
				FS_LOG_WARN_FORMAT("push recvier type not exists(type:%d).", itr->first);
				continue;
			}

			for (std::vector<std::string>::const_iterator it = itr->second.begin(); 
							it != itr->second.end(); ++it)
			{
				_pRecivers->append(*it);
			}
		}
	}

	// 通知内容
	if ((data.msgType & EM_PUSH_NOTIFICATION) == EM_PUSH_NOTIFICATION)
	{
		Json::Value& _notification = root["notification"];
		if ((data.recvierPlatform & EM_CLIENT_IOS) == EM_CLIENT_IOS)
		{
			Json::Value& _ios = _notification["ios"];
			_ios["alert"] = data.content;
			if (!data.sound.empty())
			{
				_ios["sound"] = data.sound;
			}
			else if (!m_strDefaultSound.empty())
			{
				_ios["sound"] = m_strDefaultSound;
			}

			if (data.badge > 0)
			{
				char szBadge[32] = { 0 };
				SPRINTF_S(szBadge, "+%d", data.badge);
				_ios["badge"] = szBadge;
			}

			if (data.content_available == 1)
			{
				_ios["content-available"] = true;
			}

			// 扩展信息
			if (data.extra.size() > 0)
			{
				Json::Value& _extra = _ios["extras"];
				for (std::map<std::string, std::string>::const_iterator itr = data.extra.begin();
					itr != data.extra.end(); ++itr)
				{
					_extra[itr->first] = itr->second;
				}
			}
		}
		if ((data.recvierPlatform & EM_CLIENT_ANDROID) == EM_CLIENT_ANDROID)
		{
			Json::Value& _android = _notification["android"];
			_android["alert"] = data.content;
			_android["title"] = data.title;
			if (data.builderid > 0)
			{
				_android["builder_id"] = data.builderid;
			}
			else if (m_nDefaultBuilderId > 0)
			{
				_android["builder_id"] = m_nDefaultBuilderId;
			}

			// 扩展信息
			if (data.extra.size() > 0)
			{
				Json::Value& _extra = _android["extras"];
				for (std::map<std::string, std::string>::const_iterator itr = data.extra.begin();
					itr != data.extra.end(); ++itr)
				{
					_extra[itr->first] = itr->second;
				}
			}
		}

		if ((data.recvierPlatform & EM_CLIENT_WINPHONE) == EM_CLIENT_WINPHONE)
		{
			Json::Value& _winphone = _notification["winphone"];
			_winphone["alert"] = data.content;
			_winphone["title"] = data.title;
			//_winphone["_open_page"] = open_url;
			// 扩展信息
			if (data.extra.size() > 0)
			{
				Json::Value& _extra = _winphone["extras"];
				for (std::map<std::string, std::string>::const_iterator itr = data.extra.begin();
					itr != data.extra.end(); ++itr)
				{
					_extra[itr->first] = itr->second;
				}
			}
		}

	}

	// 消息内容
	if ((data.msgType & EM_PUSH_CUSTOM) == EM_PUSH_CUSTOM)
	{
		Json::Value& _message = root["message"];
		_message["msg_content"] = data.content;
		_message["content_type"] = "text";
		_message["title"] = data.title;

		// 扩展信息
		if ( data.extra.size() > 0 )
		{
			Json::Value& _extra = _message["extras"];
			for (std::map<std::string, std::string>::const_iterator itr = data.extra.begin();
						itr != data.extra.end(); ++itr )
			{
				_extra[itr->first] = itr->second;
			}
		}
	}

	// options
	Json::Value& _options = root["options"];
	_options["apns_production"] = m_bEnvProduction;
	if (m_nTimeToLive > 0)
	{
		_options["time_to_live"] = m_nTimeToLive;
	}
}

void EntHttp::DumpAction( std::shared_ptr<INetAction>& action )
{
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
	assert(req != NULL);
	if ( NULL == req )
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
	FS_LOG_INFO_FORMAT("REQ INFO:::url:%s query:%s param:%s method:%d body:%s", req->GetURI(), req->GetQuery(), req->GetText(), req->GetMethod(), req->GetText() );
	if ( NULL != res )
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

bool EntHttp::RepRequest( std::shared_ptr<INetAction>& action )
{
	IHttpRequest* req = dynamic_cast<IHttpRequest*>(action->Request());
	IHttpResponse* res = dynamic_cast<IHttpResponse*>(action->Response());
	assert(req != NULL);
	if (NULL == req)
	{
		return false;
	}

	if (NULL != res)
	{
		res->RemoveAllHeaders();
		res->SetText("", 0);
	}

	int nIndex = action->GetIndex();
	if ( nIndex < TIMEOUT_OF_RETRIES_NUM )
	{
		action->SetIndex(++nIndex);
		FS_LOG_WARN_FORMAT("rep request(url:%s retries:%d))", req->GetURI(), nIndex);
		if (NULL != m_pService)
		{
			action->SetState(EM_ACTION_REQUEST);
			if (this->m_pService->Send(NULL_NID, action))
			{
				// 插入到缓存
				ResendAction& resent = m_cacheActions[action.get()];
				resent.action = action;
				resent.elapsed = 0;
			}
			else
			{
				FS_LOG_FATAL_FORMAT("request http failed(uri:%s).", req->GetURI());
				DumpAction(action);
			}
		}
		else
		{
			FS_LOG_ERROR("http server not start");
		}

		return true;
	}
	else
	{
		FS_LOG_FATAL_FORMAT("request http retries failed end(uri:%s).", req->GetURI());
		DumpAction(action);
	}
	
	FS_LOG_ERROR_FORMAT("rep request failed(url:%s retries:%d)", req->GetURI(), nIndex);
	return false;
}

bool EntHttp::ParseGoodsItem(const Json::Value& items, std::vector<GoodsItem>& goods)
{
	Assert(items.isArray());
	// 读取物品
	Json::Value::const_iterator itemItr_ = items.begin();
	for (; itemItr_ != items.end(); ++itemItr_)
	{
		const Json::Value& item_ = *itemItr_;
		const Json::Value& id_ = item_["code"];
		if (id_.isNull())
		{
			FS_LOG_FATAL("parse goods item failed, please check goods list");
			return false;
		}

		GoodsItem goodsItem_;
		goodsItem_.ItemId = id_.asString();
		goodsItem_.Number = item_["number"].asInt();
		goods.push_back(goodsItem_);
	}

	return true;
}

int EntHttp::HandleCommonRsp(IHttpRequest* req, IHttpResponse* res, Json::Reader& reader, Json::Value& root, Json::Value** ppdata, int& nHttpCode, const char* pszFuncName)
{
	assert( req != NULL );
	assert( res != NULL );
	if (NULL == req || res == NULL)
	{
		FS_LOG_FATAL_FORMAT("[%s]http response data error.", pszFuncName);
		nHttpCode = -1;
		return EM_RESULT_DATA_HTTP_ERR;
	}

	nHttpCode = res->GetResult();
	FS_LOG_INFO_FORMAT("[%s][url:%s]http response result:%d", pszFuncName, req->GetURI(), nHttpCode);
	if (nHttpCode == EM_HTTP_CODE_OK)
	{
		EXCEPTION_BEGIN();

		std::string strMsgBody_ = StringUtil::Utf8StringAsString(res->GetText());
		bool parsedOk = reader.parse(strMsgBody_, root);
		if (!parsedOk)
		{
			FS_LOG_FATAL_FORMAT("[%s] parse external web data failed(data:%s)", pszFuncName, strMsgBody_.c_str());
			return EM_RESULT_DATA_FAILED;
		}
		FS_LOG_INFO_FORMAT("[%s]response data:%s", pszFuncName, res->GetText());

		std::string strCode_ = root["code"].asString();
		if (strcmp(strCode_.c_str(), ERROR_CODE_SUCCESS) != 0)
		{
			FS_LOG_FATAL_FORMAT("request failed(error code:%s msg:%s)", strCode_.c_str(), root["message"].asString().c_str());
			return EM_RESULT_DATA_FAILED;
		}
		
		if (NULL != ppdata)
		{
			*ppdata = &root["obj"];
		}
		EXCEPTION_END();
	}
	else
	{
		return EM_RESULT_DATA_HTTP_ERR;
	}

	return EM_RESULT_DATA_SUCCEED;
}

bool EntHttp::ConfigRequestUrl()
{
#define INIT_REQUEST_URL(req, key)	\
	{ \
		const char* pszUrl = GetValue(key, NULL);	\
		if ( !ParseRequest(req, pszUrl) ) \
		{	\
			FS_LOG_FATAL_FORMAT("url data error(key:%s url:%s)", key, pszUrl ? pszUrl : "(null)");	 \
			return false;	\
		}	\
	}

	INIT_REQUEST_URL(m_urlProfile, "profileUrl");
	INIT_REQUEST_URL(m_urlNotice, "noticeUrl");
	INIT_REQUEST_URL(m_urlGift, "giftUrl");
	INIT_REQUEST_URL(m_urlAcitvity, "activityUrl");
	INIT_REQUEST_URL(m_urlMall, "mallUrl");
	INIT_REQUEST_URL(m_urlMailConfirm, "mailConfirmUrl");
	INIT_REQUEST_URL(m_urlCmdConfirm, "cmdConfirmUrl");
	INIT_REQUEST_URL(m_urlGoods, "goodsUrl");
	INIT_REQUEST_URL(m_urlPreOrder, "preOrderUrl");
	INIT_REQUEST_URL(m_urlOrderState, "orderStateUrl");
	INIT_REQUEST_URL(m_urlRebateInfo, "rebateInfoUrl");
	INIT_REQUEST_URL(m_urlRebatePick, "rebatePickUrl");
	return true;
}

bool EntHttp::ParseRequest(RequestInfo& req, const char* pszUrl)
{
	if ( StringUtil::CharIsNull(pszUrl))
	{
		return false;
	}

	const static char szFlags_[] = "/{method=";
	std::string url_ = pszUrl;
	std::string::size_type nIndex_ = url_.rfind(szFlags_);
	if (nIndex_ == std::string::npos)
	{
		req.url = url_;
		req.method = EM_HTTP_METHOD_GET;
	}
	else
	{
		req.url = url_.substr(0, nIndex_);
		const char* pszMethod = url_.c_str() + nIndex_ + strlen(szFlags_);
		if (strnicmp(pszMethod, "post}", 5) == 0)
		{
			req.method = EM_HTTP_METHOD_POST;
		}
		else
		{
			req.method = EM_HTTP_METHOD_GET;
		}
	}

	return true;
}

IHttpRequest* EntHttp::InitRequest(const RequestInfo& req, std::shared_ptr<INetAction>& action, int nTag, int nTimeOut)
{
	IHttpRequest* request = (IHttpRequest*)action->Request();
	request->SetURI(req.url.c_str());
	request->SetMethod((EM_HTTP_METHOD)req.method);
	request->SetReqTimeout(nTimeOut);
	if (req.method == EM_HTTP_METHOD_POST)
	{
		request->AddHeader("Content-Type", "application/x-www-form-urlencoded");
	}

	request->AddHeader("Accept", "application/json");
	action->SetTag(nTag);
	return request;
}
