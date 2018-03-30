//--------------------------------------------------------------------
// 文件名:      LogModule.cpp
// 内  容:      日志模块
// 说  明:
// 创建日期:    2014年10月20日
// 整理日期:    2015年09月10日
// 创建人:       
// 整理人:       
//    :       
//--------------------------------------------------------------------
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/CapitalDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/Define/ItemTypeDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"
#include "utils/XmlFile.h"
#include "public/VarList.h"
#include <ctime>
//#include "../Define/OffLineDefine.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "Analytics/GameAnalyticsDef.h"
#include "Analytics/GameAnalyticsEventNames.h"
#include "Analytics/GameAnalyticsProperties.h"
#ifdef ANALY_BY_USER
#include "Analytics/SensorsAnalyticsCollector.h"
#else
#include "Analytics/SensorsAnalyticsCollectorEx.h"
#endif
#include "utils/IniFile.h"
//#include "SocialSystemModule/ChatModule.h"
#include "Define/ContainerDefine.h"
#include "ContainerModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "ItemModule/ToolItem/ToolItemModule.h"
#endif
//#include "Define/AuctionDefine.h"
#include "Define/SnsDefine.h"

//#include "FsGame/Middle/MiddleModule.h"

// 统计数据场景id
#define STATICS_SCENE_ID 2

const static int COMMENT_MAX_LEN = 255;

// 货币日志属性
static const char* GA_CAPITAL_PROPERTIES[CAPITAL_MAX_TYPE] = 
{
	GAME_ANALYTICS_PROPERTIES_COPPER,
	GAME_ANALYTICS_PROPERTIES_SILVER,
// 	GAME_ANALYTICS_PROPERTIES_SMELT,
// 	GAME_ANALYTICS_PROPERTIES_EXPLOIT,
// 	GAME_ANALYTICS_PROPERTIES_SOUL,
// 	GAME_ANALYTICS_PROPERTIES_HONOR,
// 	GAME_ANALYTICS_PROPERTIES_CHIVALRY,
// 	GAME_ANALYTICS_PROPERTIES_TRADE_SILVER,
// 	GAME_ANALYTICS_PROPERTIES_CAMPVALUE,
};

LogModule* LogModule::m_pLogModule = NULL;
const char *LOG4CPLUS = "log4plush.properties";
const char* GAME_INI_PATH = "game.ini";

int64_t LogModule::ms_nNextReportCount = 0;
const int HB_REPORT_COUNT_INTERVAL = 10000;					// 10s
const int64_t REPORT_COUNT_INTERVAL = 60 * 5;				// 间隔5分钟

bool LogModule::Init(IKernel* pKernel)
{
	m_pLogModule = this;

	log4cplus::initialize();
	const char *path = pKernel->GetResourcePath();
	log4cplus::tstring pathname(path);
	pathname.append( LOG4CPLUS );
	log4cplus::PropertyConfigurator::doConfigure( pathname );
	// 神策日志
	log4cplus::Logger _anaLogger = log4cplus::Logger::getInstance("ANA");

	// 读取游戏配置
	std::string strGameIni(path);
	strGameIni.append(GAME_INI_PATH);
	CIniFile iniFile(strGameIni.c_str());
	if (!iniFile.LoadFromFile())
	{
		Assert(false);
		::extend_warning(LOG_ERROR, "open game.ini(path:%s) failed", strGameIni.c_str());
		return false;
	}
	int nDeployId = iniFile.ReadInteger("game_info", "game_deploy_id", 0);
	int nGameId = iniFile.ReadInteger("game_info", "game_id", 0);
	const char* pszGameName = iniFile.ReadString("game_info", "game_name", "");
	const char* pszGameVer = iniFile.ReadString("game_info", "game_ver", "1.0");
	const char* pszDistrictName = iniFile.ReadString("game_info", "district_name", "");
	int pszChannelId = iniFile.ReadInteger("game_info", "channelid", 0);
	int production = iniFile.ReadInteger("game_info", "production", 0);
	int nLogLevel = iniFile.ReadInteger("game_info", "log_level", 0);
	if (nLogLevel > 0)
	{
		// 设置日志等级
		extend_log_level(pKernel, (LogLevelEnum)nLogLevel, "");
	}

#ifndef FSROOMLOGIC_EXPORTS
	m_pAnaCollector = new GameAnalytics(_anaLogger, 
					nDeployId,
					nGameId,
					pKernel->GetServerId(),
					pKernel->GetDistrictId(),
					pszChannelId,
					production,
					StringUtil::StringAsUtf8String(pszGameVer).c_str(),
					StringUtil::StringAsUtf8String(pszGameName).c_str(),
					StringUtil::StringAsUtf8String(pszDistrictName).c_str());
#else
	m_pAnaCollector = new GameAnalytics(_anaLogger,
		nDeployId,
		nGameId,
		pKernel->GetPlatformId(),
		pKernel->GetRoomId(),
		pszChannelId,
		production,
		StringUtil::StringAsUtf8String(pszGameVer).c_str(),
		StringUtil::StringAsUtf8String(pszGameName).c_str(),
		StringUtil::StringAsUtf8String(pszDistrictName).c_str());
#endif
	m_pAnaCollector->InitAction();
	Assert(m_pLogModule);

	m_pAnaCollector->OnServerStart();

	pKernel->AddEventCallback("scene", "OnCreate", LogModule::OnCreate);

	// 聊天日志
	m_pChatLogger = log4cplus::Logger::getInstance("CHAT");

	
	DECL_HEARTBEAT(LogModule::H_RecOnlineCount);
	return true;
}

bool LogModule::Shut(IKernel* pKernel)
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnServerClose();
		m_pAnaCollector->GetLogger().shutdown();
		delete m_pAnaCollector;
		m_pAnaCollector = NULL;
	}
	return true;
}

void LogModule::SetSrvInfo(IKernel* pKernel, const SrvInfo& info)
{
	Assert(NULL != m_pAnaCollector);
	if (NULL != m_pLogModule)
	{
		m_pAnaCollector->SetChannelId(info.ChannelId);
		m_pAnaCollector->SetDistrictId(info.AreaId);
		m_pAnaCollector->SetGameId(info.GameId);
		m_pAnaCollector->SetProductionId(info.ProductionId);
		m_pAnaCollector->SetServerId(info.ServerId);
		m_pAnaCollector->SetDeployId(info.DeployId);
	}
	else
	{
		::extend_warning(LOG_ERROR, "init logmodule info failed. not init analysis instance");
	}
}

void LogModule::OnClientInfo(IKernel* pKernel, const PERSISTID& player, const char* ip,
							const char* platform,
							const char* clientver,
							const char* manufacturer,
							const char* deviceid,
							const char* model,
							const char* os,
							const char* osver,
							bool bwifi )
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* rid = pPlayer->QueryString( FIELD_PROP_UID );
		//int nLevel = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		//int64_t nExp = pPlayer->QueryInt64(FIELD_PROP_PLAYER_EXP);
		bool bFirst = pPlayer->QueryInt( FIELD_PROP_BORN_STATE) != EM_PLAYER_STATE_NORMAL;
		if (bFirst)
		{
			pPlayer->SetInt(FIELD_PROP_BORN_STATE, EM_PLAYER_STATE_NORMAL);
		}
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnClientInfo(rid, bFirst, clientver, ip, platform, bwifi, manufacturer, deviceid, model, os, osver, &args);
	}
}

#ifdef ANALY_BY_USER
// 注册用户
void LogModule::OnNewUser(	const char* uid,		// 用户唯一id (require)
							const char* anid,		// 用户原匿名id(option)
							const char* nick,		// 昵称/用户名(require)
							int sex,				// 性别(option)
							__int64 bron,			// 出生日期(option)
							const char* ip,			// 终端id(option)
							const char* platform,	// 注册平台(option)
							bool wifi,				// 是否使用wifi(option)
							const char* manufacturer,	// 制造商(option)
							const char* devid,		// 终端设备唯一标识(option)
							const char* model,		// 终端类型(option)
							const char* os,			// 终端系统(option)
							const char* os_ver		// 终端系统版本(option)
							)
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnNewUser(uid, anid, nick, sex, bron, ip, platform, wifi, manufacturer, devid, model, os, os_ver);
	}
}

void LogModule::OnDelUser(const char* uid)
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnDelUser(uid);
	}
}
#endif
// 用户登陆
void LogModule::OnUserLogin(const char* uid,			// 用户唯一id (require)
							const char* nick,		// 昵称/用户名(require)
							const char* ip,			// 终端ip(option)
							const char* platform,	// 平台(option)
							const char* manufacturer,	// 制造商(option)
							const char* devid,		// 终端设备唯一标识(option)
							const char* model,		// 终端类型(option)
							const char* os,			// 终端系统(option)
							const char* os_ver,		// 终端系统版本(option)
							bool bReset /*= false*/	)
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnUserLogin(uid, nick, ip, platform, manufacturer, devid, model, os, os_ver, bReset);
	}
}

void LogModule::OnUserLogout(const char* uid, int64_t timestamp, bool bReset/* = false*/ )
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnUserLogout(uid, timestamp, bReset);
	}
}

// 创建角色
void LogModule::OnCreateRole(IKernel* pKernel, const PERSISTID& player)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString( FIELD_PROP_UID );
		const wchar_t* pwRoleName = pPlayer->QueryWideStr(FIELD_PROP_NAME);
		const std::string& roleName = StringUtil::WideStrAsUTF8String(pwRoleName);
		int job = pPlayer->QueryInt(FIELD_PROP_JOB);
		int sex = pPlayer->QueryInt(FIELD_PROP_SEX);
		int nation = 0/*pPlayer->QueryInt(FIELD_PROP_NATION)*/;
		const char* ip = pPlayer->QueryString(FIELD_PROP_ADDRESS);

		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnNewRole(rid, uid, roleName.c_str(), job, sex, nation, ip, &args);

		//int nLevel = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		//int nVip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		// 刚创建设置关键初始化属性变化
		//for (int i = CAPITAL_MIN_TYPE; i < CAPITAL_MAX_TYPE; ++i)
		//{
		//	int64_t gold = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, i);
		//	if (0 != gold)
		//	{
		//		m_pAnaCollector->OnGainCoin(rid, nLevel, nVip, i, gold, gold, 0, 0, LOG_GAIN_INIT_ROLE, NULL);
		//	}
		//}
	}
}
// 角色进入
void LogModule::OnRoleEntry(IKernel* pKernel, const PERSISTID& player, bool bReset/* = false*/)
{
#ifdef TEST_LOG_PERFOMANCE
	int64_t tNow = util_get_utc_ms_time();
	for (int i = 0; i < 10000; ++i)
	{
#endif
		if (NULL != m_pAnaCollector)
		{
			IGameObj* pPlayer = pKernel->GetGameObj(player);
			if (NULL == pPlayer)return;

			// 非正常上线
			if (pPlayer->QueryInt(FIELD_PROP_ONLINE) == OFFLINE)
			{
				return;
			}

			const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
			const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
			int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
			int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
			const char* ip = pPlayer->QueryString(FIELD_PROP_ADDRESS);
			int64_t gold = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_GOLD);
			//int64_t silver = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_TRADE_SILVER);
			int64_t copper = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_COPPER);

			// 添加在线人数统计
			int nCurOnlineCount = 0;
#ifndef FSROOMLOGIC_EXPORTS
			nCurOnlineCount = pKernel->GetOnlineCount();
#endif
			KVArgs args;
			args[GAME_ANALYTICS_PROPERTIES_ONLINE_COUNT] = nCurOnlineCount;
			//args[GAME_ANALYTICS_PROPERTIES_TRADE_SILVER] = silver;
			args[GAME_ANALYTICS_PROPERTIES_COPPER] = copper;

			m_pAnaCollector->OnRoleEntry(rid, uid, level, vip, gold, ip, &args, bReset);
		}
#ifdef TEST_LOG_PERFOMANCE
	}
	int64_t tEnd = util_get_utc_ms_time();
	int64_t tCost = tEnd - tNow;
	printf("log 1000 cost:%lldms\n", tCost);
#endif
}
// 角色退出
void LogModule::OnRoleExit(IKernel* pKernel, const PERSISTID& player, bool bReset/* = false*/)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		const char* ip = pPlayer->QueryString(FIELD_PROP_ADDRESS);
		int64_t gold = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_GOLD);
//		int64_t silver = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_TRADE_SILVER);
		int64_t copper = CapitalModule::m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_COPPER);

		int64_t nowTime = util_get_utc_time();
		int64_t loginTime = pPlayer->QueryInt64( FIELD_PROP_ROLE_LOGIN_TIME );
		int64_t onlinetime = nowTime - loginTime;
		int64_t totalonline = pPlayer->QueryInt(FIELD_PROP_TOTALSEC);
		// 在线时长在下线存储才会记录，要加上当前在线时长
		totalonline += onlinetime;
		//pPlayer->SetInt64(FIELD_PROP_ON_LINE_TIME, totalonline);

		// 添加在线人数统计
		int nCurOnlineCount = 0;
#ifndef FSROOMLOGIC_EXPORTS
		nCurOnlineCount = pKernel->GetOnlineCount();
#endif
		
		KVArgs args;
		args[GAME_ANALYTICS_PROPERTIES_ONLINE_COUNT] = nCurOnlineCount;
		//args[GAME_ANALYTICS_PROPERTIES_TRADE_SILVER] = silver;
		args[GAME_ANALYTICS_PROPERTIES_COPPER] = copper;

		m_pAnaCollector->OnRoleExit(rid, uid, level, vip, gold, onlinetime, totalonline, ip, &args, bReset);
	}
}

void LogModule::OnPaying(IKernel* pKernel, const PERSISTID& player, 
						const char* orderid, const char* itemid, int count, float price, bool first)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

		const char* osType = pPlayer->QueryString(FIELD_PROP_CLIENT_TYPE);
		m_pAnaCollector->OnPaying(rid, uid, level, vip, orderid, itemid, count, price, first, osType);
	}
}

void LogModule::OnPayed(IKernel* pKernel, const PERSISTID& player, 
			const char* orderid, float amount, int result, bool first, bool bIsReal,
			const char* platform, const char* os)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)return;

	const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
	if (NULL != m_pAnaCollector)
	{
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		float totalamount = pPlayer->QueryFloat(FIELD_PROP_TOTAL_PAY);
		float realamount = pPlayer->QueryFloat(FIELD_PROP_TOTAL_PAY_REAL);
		int paytimes = pPlayer->QueryInt(FIELD_PROP_PAY_TIMES);

		OnPayed(rid, uid, level, vip, orderid, amount, totalamount, realamount, paytimes, result, first, bIsReal, platform, os);
	}

	IGameObj* pDcObj = pKernel->GetGameObj(player);
	Assert(pDcObj != NULL);
	const wchar_t* name = pDcObj->QueryWideStr(FIELD_PROP_NAME);
	const std::string& strTime = m_pLogModule->GetLogTime();

	// 保存充值日志
	CVarList logInfo;
	logInfo << uid << name << strTime << (first ? 1 : 0) << 0 << orderid << StringUtil::FloatAsString(amount) << "";
	pKernel->CustomLog("traceorder_log", logInfo);
}

void LogModule::OnPayed( const char* roleid, const char* uid, int level, int vip, 
	const char* orderid, float amount, float totalamount, float realamount, int paytimes, int result, bool first, bool bIsReal,
							const char* platform, const char* os)
{
	if (NULL != m_pAnaCollector)
	{
		m_pAnaCollector->OnPayCompleted(roleid, uid, level, vip, orderid, amount, totalamount, realamount, paytimes, result, first, bIsReal, platform, os);
	}
}

void LogModule::OnIncExp(IKernel* pKernel, const PERSISTID& player, 
							int value,					// 增加值(require)
							int newValue,				// 增加后的值
							int origValue,				// 原始值
							int source					// 来源
							)
{
	// 只记录指定功能获得的经验
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnIncExp(rid, level, value, newValue, origValue, source, &args);
	}
}

void LogModule::OnLevelup(IKernel* pKernel, const PERSISTID& player, int level, int original)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnLevelup(rid, level, original, &args);
	}
}

void LogModule::OnGainCoin(IKernel* pKernel, const PERSISTID& player, 
						int coid, /* 游戏币id(金/银/铜/声望....)(require), 游戏中定义 */ 
						int64_t value, /* 增加值(require) */ 
						int64_t newvalue, /* 当前新值(require) */ 
						int64_t original, /* 原始值(require) */ 
						int module, /* 功能模块(option), 游戏中定义(在什么模块) */ 
						int func, /* 功能事件(option), 游戏中定义(做什么事获得) */ 
						const char* extra /*= NULL/* 额外说明(option), 对event进一步说明 */)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnGainCoin(rid, level, vip, coid, value, newvalue, original, module, func, extra, &args);
	}
}

void LogModule::OnCostCoin(IKernel* pKernel, const PERSISTID& player, 
				int coid, /* 游戏币id(金/银/铜/声望....)(require), 游戏中定义 */ 
				int64_t value, /* 消耗值(require) */
				int64_t newvalue, /* 当前新值 */
				int64_t original, /* 原始值(require) */ 
				int module, /* 功能模块(option), 游戏中定义(在什么模块) */ 
				int func, /* 功能事件(option), 游戏中定义(做什么事获得) */ 
				const char* extra /*= NULL/* 额外说明(option), 对event进一步说明 */)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCostCoin(rid, level, vip, coid, value, newvalue, original, module, func, extra, &args);
	}
}

void LogModule::OnGainItem(IKernel* pKernel, const PERSISTID& player, 
							const char* itemid, /* 物品id(require) */ 
							int color,				// 物品品质
							int num, /* 获得数值(require) */ 
							int count, /* 当前拥有数 */ 
							int orig, /* 原来拥有数 */ 
							int module, /* 功能模块(option), 游戏中定义(在什么模块) */ 
							int func, /* 功能事件(option), 游戏中定义(做什么事获得) */ 
							const char* extra /*= NULL/* 额外说明(option), 对event进一步说明 */)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		if (color >= 0)
		{
			args[GAME_ANALYTICS_PROPERTIES_ITEM_COLOR] = color;
		}
		m_pAnaCollector->OnGainItem(rid, level, vip, itemid, num, count, orig, module, func, extra, &args);
	}
}

void LogModule::OnCostItem(IKernel* pKernel, const PERSISTID& player, 
							const char* itemid, /* 物品id(require) */ 
							int color,				// 物品品质
							int num, /* 获得数值(require) */ 
							int count, /* 当前拥有数 */ 
							int orig, /* 原来拥有数 */
							int module, /* 功能模块(option), 游戏中定义(在什么模块) */ 
							int func, /* 功能事件(option), 游戏中定义(做什么事获得) */ 
							const char* extra /*= NULL/* 额外说明(option), 对event进一步说明 */)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		if (color >= 0)
		{
			args[GAME_ANALYTICS_PROPERTIES_ITEM_COLOR] = color;
		}
		m_pAnaCollector->OnCostItem(rid, level, vip, itemid, num, count, orig, module, func, extra, &args);
	}
}

void LogModule::OnSkipUpStartScene(IKernel* pKernel, const PERSISTID& player)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)return;

	const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
	int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
	int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

	KVArgs args;
	FillCommonArgs(pKernel, player, args);
	m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_SKIP_UP_START_SCENE, 0, 0, &args);
}

void LogModule::OnVisitFunction(IKernel* pKernel, const PERSISTID& player,
						int module, /* 功能模块(require), 游戏中定义(在什么模块) */ 
						int func, /* 功能名(require) 游戏中定义(建议跟上面的func一致) */
						bool result, /* 结果(require) true:成功 false: 失败 */ 
						int value, /* 额外说明(option), 对func进一步说明 */ 
						const char* extra /* 额外说明(option), 对func进一步说明 */)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		//const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
		KVArgs args;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnVisitFunction(rid, level, vip, module, func, result, value, extra, &args);
	}
}


void LogModule::OnFriendEvent(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int opType)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

		KVArgs args;
		//args[GAME_EXTRA_PROPERTIES_FRIEND_SELF_NAME] = StringUtil::WideStrAsUTF8String(pPlayer->QueryWideStr(FIELD_PROP_NAME));
		args[GAME_EXTRA_PROPERTIES_FRIEND_NAME] = StringUtil::WideStrAsUTF8String(pFriendName);
		//args[GAME_EXTRA_PROPERTIES_FRIEND_RID] = pFriendId;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_FRIEND_OP, 0, opType, &args);
	}
}

void LogModule::OnFriendSendGift(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nGift, int nIntLevel, int nIncIntimacy)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

		KVArgs args;
		//args[GAME_EXTRA_PROPERTIES_FRIEND_SELF_NAME] = StringUtil::WideStrAsUTF8String(pPlayer->QueryWideStr(FIELD_PROP_NAME));
		args[GAME_EXTRA_PROPERTIES_FRIEND_NAME] = StringUtil::WideStrAsUTF8String(pFriendName);
		//args[GAME_EXTRA_PROPERTIES_FRIEND_RID] = pFriendId;

		args[GAME_EXTRA_PROPERTIES_FRIEND_GIFT_NUM] = nGift;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INTLEVEL] = nIntLevel;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INC_INTIMACY] = nIncIntimacy;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_SND_GIFT, 0, 0, &args);
	}
}

void LogModule::OnFriendRecvGift(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nGift, int nIntLevel, int nIncIntimacy)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		//const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);


		KVArgs args;
		//args[GAME_EXTRA_PROPERTIES_FRIEND_SELF_NAME] = StringUtil::WideStrAsUTF8String(pPlayer->QueryWideStr(FIELD_PROP_NAME));
		args[GAME_EXTRA_PROPERTIES_FRIEND_NAME] = StringUtil::WideStrAsUTF8String(pFriendName);
		//args[GAME_EXTRA_PROPERTIES_FRIEND_RID] = pFriendId;
		args[GAME_EXTRA_PROPERTIES_FRIEND_GIFT_NUM] = nGift;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INTLEVEL] = nIntLevel;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INC_INTIMACY] = nIncIntimacy;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_RCV_GIFT, 0, 0, &args);
	}
}

void LogModule::OnFriendTeamFight(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nIntLevel, int nIncIntimacy)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		//const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

		KVArgs args;
		//args[GAME_EXTRA_PROPERTIES_FRIEND_SELF_NAME] = StringUtil::WideStrAsUTF8String(pPlayer->QueryWideStr(FIELD_PROP_NAME));
		args[GAME_EXTRA_PROPERTIES_FRIEND_NAME] = StringUtil::WideStrAsUTF8String(pFriendName);
		//args[GAME_EXTRA_PROPERTIES_FRIEND_RID] = pFriendId;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INTLEVEL] = nIntLevel;
		args[GAME_EXTRA_PROPERTIES_FRIEND_INC_INTIMACY] = nIncIntimacy;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_TEAM_FIGHT, 0, 0, &args);
	}
}

void LogModule::OnFriendCharmChanged(IKernel* pKernel, const PERSISTID& player, int nCharm, int nTotalCharm, int nReason)
{
	if (NULL != m_pAnaCollector)
	{
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)return;

		//const char* uid = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
		int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
		int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

		KVArgs args;
		// 设置属性值
		args[GAME_ANALYTICS_PROPERTIES_ROLE_CHARMS] = nTotalCharm;
		m_pAnaCollector->SetProfile(rid, args);

		// 写入事件
		args[GAME_ANALYTICS_PROPERTIES_COMM_C_VALUE] = nCharm;
		FillCommonArgs(pKernel, player, args);
		m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_CAHRMS_CHANGED, 0, nReason, &args);

	}
}

void LogModule::OnSworn(IKernel*pKernel, const PERSISTID& player, const wchar_t* playerName, int isSameSex, int isAdd)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)return;

	const char* rid = pPlayer->QueryString(FIELD_PROP_UID);
	int level = pPlayer->QueryInt(FIELD_PROP_LEVEL);
	int vip = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

	KVArgs args;
	args[GAME_EXTRA_PROPERTIES_FRIEND_NAME] = StringUtil::WideStrAsUTF8String(playerName);
	args[GAME_EXTRA_PROPERTIES_FRIEND_IS_SAME_SEX] = isSameSex;
	args[GAME_EXTRA_PROPERTIES_FRIEND_IS_SWORN_ADD] = isAdd;
	FillCommonArgs(pKernel, player, args);
	m_pAnaCollector->OnCustomEvent(rid, level, vip, GAME_EXTRA_EVENT_SWORN, 0, 0, &args);

}

void LogModule::OnForbidSpeak(IKernel* pKernel, const PERSISTID& player, int64_t nInterval, int64_t nExpiredTime)
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	LogBaseInfo baseInfo;
	if (!GetPlayerBaseInfo(pKernel, player, baseInfo))
	{
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_FORBID_EXPIRED_TIME] = nExpiredTime;
	m_pAnaCollector->SetProfile(baseInfo.strUid.c_str(), args);

	args[GAME_EXTRA_DURATION_TIME] = nInterval;
	FillCommonArgs(pKernel, player, args);
	m_pAnaCollector->OnCustomEvent(baseInfo.strUid.c_str(), 0, 0, GAME_EXTRA_EVENT_FORBID_SPEAK, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::OnAllowSpeak(IKernel* pKernel, const PERSISTID& player )
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	LogBaseInfo baseInfo;
	if (!GetPlayerBaseInfo(pKernel, player, baseInfo))
	{
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_FORBID_EXPIRED_TIME] = 0;
	m_pAnaCollector->SetProfile(baseInfo.strUid.c_str(), args);
	FillCommonArgs(pKernel, player, args);
	m_pAnaCollector->OnCustomEvent(baseInfo.strUid.c_str(), 0, 0, GAME_EXTRA_EVENT_ALLOW_SPEAK, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::OnBlockPlayer(IKernel* pKernel, const PERSISTID& player, int64_t nInterval, int64_t nExpiredTime)
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	LogBaseInfo baseInfo;
	if (!GetPlayerBaseInfo(pKernel, player, baseInfo))
	{
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_BLOCK_EXPIRED_TIME] = nExpiredTime;
	m_pAnaCollector->SetProfile(baseInfo.strUid.c_str(), args);

	args[GAME_EXTRA_DURATION_TIME] = nInterval;
	FillCommonArgs(pKernel, player, args);

	m_pAnaCollector->OnCustomEvent(baseInfo.strUid.c_str(), 0, 0, GAME_EXTRA_EVENT_BLOCK_PLAYER, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::OnBlockPlayer(IKernel* pKernel, const char* pszRoleId, int64_t nInterval, int64_t nExpiredTime)
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_BLOCK_EXPIRED_TIME] = nExpiredTime;
	m_pAnaCollector->SetProfile(pszRoleId, args);

	args[GAME_EXTRA_DURATION_TIME] = nInterval;

	m_pAnaCollector->OnCustomEvent(pszRoleId, 0, 0, GAME_EXTRA_EVENT_BLOCK_PLAYER, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::OnResumePlayer(IKernel* pKernel, const PERSISTID& player)
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	LogBaseInfo baseInfo;
	if (!GetPlayerBaseInfo(pKernel, player, baseInfo))
	{
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_BLOCK_EXPIRED_TIME] = 0;
	m_pAnaCollector->SetProfile(baseInfo.strUid.c_str(), args);
	FillCommonArgs(pKernel, player, args);

	m_pAnaCollector->OnCustomEvent(baseInfo.strUid.c_str(), 0, 0, GAME_EXTRA_EVENT_RESUME_PLAYER, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::OnResumePlayer(IKernel* pKernel, const char* pszRoleId)
{
	if (NULL == m_pAnaCollector)
	{
		Assert(false);
		return;
	}

	KVArgs args;
	args[GAME_EXTRA_BLOCK_EXPIRED_TIME] = 0;
	m_pAnaCollector->SetProfile(pszRoleId, args);

	m_pAnaCollector->OnCustomEvent(pszRoleId, 0, 0, GAME_EXTRA_EVENT_RESUME_PLAYER, 0, FUNCTION_EVENT_ID_GMCC_WEB, &args);
}

void LogModule::FillCommonArgs(IKernel*pKernel, const PERSISTID&player, KVArgs& args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(player);
	if (pSelfObj == NULL){
		return;
	}

	args[GAME_ANALYTICS_PROPERTIES_UID] = pSelfObj->QueryString(FIELD_PROP_ACCOUNT);
}

// 获取玩家的基本数据
bool LogModule::GetPlayerBaseInfo(IKernel *pKernel, const PERSISTID& player, LogBaseInfo& outBaseInfo)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return false;
	}
	
	outBaseInfo.strAccount	= pPlayer->QueryString(FIELD_PROP_ACCOUNT);
	outBaseInfo.strUid		= pPlayer->QueryString(FIELD_PROP_UID);
	outBaseInfo.nLevel		= pPlayer->QueryInt(FIELD_PROP_LEVEL);
	outBaseInfo.nVipLv		= pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
	//outBaseInfo.nJob		= pPlayer->QueryInt(FIELD_PROP_JOB);

	return true;
}

/*!
* @brief	玩家登陆日志
* @param    核心指针
* @param    玩家对象
* @param    角色结构
* @return	结果bool
*/
bool LogModule::SaveRoleLog(IKernel* pKernel, const PERSISTID& player, const RoleLog &log)
{
	//1、基础容错
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//4、玩家账号和名字
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveRoleLog] account is NULL or empty! ip -> %s ", log.ip.c_str());
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveRoleLog] name is NULL or empty! ip -> %s ", log.ip.c_str());
		return false;
	}

	//5、获取登录时间，下线时间
	std::string strTimeIn = m_pLogModule->GetLogTime(log.loginTime);
	std::string strTimeOut = m_pLogModule->GetLogTime(log.logoutTime);

	//6、保存玩家登录日志
	CVarList logInfo;
	logInfo << account << name << log.ip << strTimeIn.c_str() << strTimeOut.c_str() << log.mac << log.osType << strComment.c_str();

	return pKernel->CustomLog("role_log", logInfo);
}

/*!
* @brief	记录游戏玩法日志
* @param    核心指针
* @param    玩家对象
* @param    玩法结构
* @return	结果bool
*/
bool LogModule::SaveGameActionLog(IKernel* pKernel, const PERSISTID& player, const GamePlayerActionLog &log)
{
	//1、基本容错判断
	if (NULL == pKernel)
	{
		return false;
	}

	//2、活动计算时玩家已离线只有昵称记录
	const char* account = "";
	const wchar_t* name = L"";
	//int nation = 0;
	if (pKernel->Exists(player) && pKernel->Type(player) == TYPE_PLAYER)
	{
		IGameObj *playerObj = pKernel->GetGameObj(player);
		if (NULL == playerObj)
		{
			return false;
		}

		//3、玩家账号和名字
		account = playerObj->QueryString("Account");
		if (StringUtil::CharIsNull(account))
		{
			::extend_warning(LOG_ERROR, "[LogModule::SaveGameActionLog] account is NULL or empty! logType -> %d ", log.actionType);
			return false;
		}
		name = playerObj->QueryWideStr("Name");
		if (StringUtil::CharIsNull(name))
		{
			::extend_warning(LOG_ERROR, "[LogModule::SaveGameActionLog] name is NULL or empty! logType -> %d ", log.actionType);
			return false;
		}

		//nation = playerObj->QueryInt("Nation");

		// Analytics
		if (NULL != m_pAnaCollector)
		{
			const char* rid = playerObj->QueryString(FIELD_PROP_UID);
			int level = playerObj->QueryInt(FIELD_PROP_LEVEL);
			int vip = playerObj->QueryInt(FIELD_PROP_VIP_LEVEL);
			int job = playerObj->QueryInt(FIELD_PROP_JOB);
			KVArgs args;
			FillCommonArgs(pKernel, player, args);
			m_pAnaCollector->OnVisitFunction(rid, level, vip, 0, log.actionType, true, log.actionState, NULL, &args);
			//m_pAnaCollector->OnCustomEvent(account, rid, job, level, vip, "game_custom", log.actionType, log.actionState, NULL);
		}
	}
	else
	{
		name = log.name.c_str();
	}

	//4、获取日志时间和游戏玩法ID
	std::string strTime = m_pLogModule->GetLogTime();

	//5、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1);
	}

	//6、获取真实场景ID
	int sceneID = pKernel->GetSceneId();
	if (pKernel->GetSceneClass() == 2)
	{
		sceneID = pKernel->GetPrototypeSceneId(sceneID);
	}

	//7、组装参数
	CVarList logInfo;
	logInfo << account << name << strTime.c_str() << log.actionType << sceneID << log.actionState << strComment.c_str();

	//8、保存游戏玩法日志
	return pKernel->CustomLog("game_player_action_log", logInfo);
}


/*!
* @brief	任务日志
* @param    核心指针
* @param    玩家对象
* @param    任务结构
* @return	结果bool
*/
bool LogModule::SaveTaskLog(IKernel* pKernel, const PERSISTID& player, const TaskLog &log)
{
	//1、基本容错
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//4、玩家账号和名字
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveTaskLog] account is NULL or empty! taskID -> %d ", log.taskID);
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveTaskLog] name is NULL or empty! taskID -> %d ", log.taskID);
		return false;
	}

	//5、获取当前日志时间和任务ID
	std::string strTime = m_pLogModule->GetLogTime();
	std::string strTaskID = StringUtil::IntAsString(log.taskID);
	

	//6、组装任务日志参数
	CVarList logInfo;
	logInfo << account << name << strTime.c_str() << strTaskID.c_str() << log.type << log.state << strComment.c_str();

	//7、保存活动日志
	return pKernel->CustomLog("task_log", logInfo);
}


/*!
* @brief	金钱日志
* @param    核心指针
* @param    玩家对象
* @param    金钱结构
* @return	结果bool
*/
bool LogModule::SaveMoneyLog(IKernel* pKernel, const PERSISTID& player, const MoneyLog &log)
{
	//1、基本容错
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、玩家账号和名字
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveMoneyLog] account is NULL or empty! eventID -> %d ", log.eventID);
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveMoneyLog] name is NULL or empty! eventID -> %d ", log.eventID);
		return false;
	}

    //4、获取玩家Uid
    const char* uid = pKernel->SeekRoleUid(name);
	if( StringUtil::CharIsNull(uid) )
	{
		return false;
	}

    //5、时间
	std::string strTime = m_pLogModule->GetLogTime();

	//6、获取真实场景ID
	int sceneID = pKernel->GetSceneId();
	if(pKernel->GetSceneClass() == 2)
	{
		sceneID = pKernel->GetPrototypeSceneId(sceneID);
	}
	
	//7、判断公会名字
	const wchar_t* guildName = playerObj->QueryWideStr("GuildName");
	if ( StringUtil::CharIsNull(guildName) )
	{
		guildName = L"";
	}

	//9、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//10、组装金钱异动日志
	CVarList logInfo;
	logInfo << strTime.c_str() << -1 << -1 << account << name << uid 
		    << log.eventID << log.eventType  << log.eventSerial << log.capitalType
		    << log.before << log.money << log.after << log.senderName << log.senderUid 
			<< sceneID << "" << guildName << log.state << strComment.c_str();

	// ANALYTICES:
	if (NULL != m_pAnaCollector)
	{
		// 邮件转发的不记录到神策
		if (log.eventID != FUNCTION_EVENT_ID_ITEM_TO_MAIL && log.eventID != FUNCTION_EVENT_ID_SYSTEM_MAIL)
		{
			if (log.eventType == LOG_ACT_EVENT_GAIN)
			{
				OnGainCoin(pKernel, player, log.capitalType, log.money, log.after, log.before, 0, log.eventID, NULL);
			}
			else
			{
				OnCostCoin(pKernel, player, log.capitalType, log.money, log.after, log.before, 0, log.eventID, NULL);
			}
		}
	}

	//11、保存金钱异动日志
	return pKernel->CustomLog("money_log", logInfo);
}


/*!
* @brief	物品日志表
* @param    核心指针
* @param    玩家对象
* @param    物品结构
* @return	结果bool
*/
bool LogModule::SaveItemLog(IKernel* pKernel, const PERSISTID& player, const ItemLog &log)
{
	//1、基本容错
	if ( NULL == pKernel || !pKernel->Exists(player)  || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、判断账号和玩家名字
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveItemLog] account is NULL or empty! eventID -> %d ", log.eventID);
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveItemLog] name is NULL or empty! eventID -> %d ", log.eventID);
		return false;
	}

	//4、获取玩家Uid
	const char* uid = pKernel->SeekRoleUid(name);
	if( StringUtil::CharIsNull(uid) )
	{
		return false;
	}

	//5、时间
	std::string strTime = m_pLogModule->GetLogTime();

	//6、获取真实场景ID
	int sceneID = pKernel->GetSceneId();
	if(pKernel->GetSceneClass() == 2)
	{
		sceneID = pKernel->GetPrototypeSceneId(sceneID);
	}

	//7、判断公会名字
	const wchar_t* guildName = playerObj->QueryWideStr("GuildName");
	if ( StringUtil::CharIsNull(guildName) )
	{
		guildName = L"";
	}

	//8、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//9、如果是装备itemNum为品质
	int num = log.itemNum;
	int colorLevel = log.colorLevel;
	if(colorLevel < 0)
	{
#ifndef FSROOMLOGIC_EXPORTS
		if (ToolItemModule::IsCanWearItem(pKernel, log.itemID.c_str()))
		{
			num = 1;
			colorLevel = log.itemNum;
		}
		else
#endif
		{
			num = log.itemNum;
			const char* colorProperty = pKernel->GetConfigProperty(log.itemID.c_str(), "ColorLevel");  //配置的颜色品质
			colorLevel = StringUtil::StringAsInt(colorProperty);
		}
	}

	int nAfter = log.after;
	int nBefore = log.before;
	if (nAfter == 0 && nBefore == 0)
	{
		// 取背包当前数据计算数量
		PERSISTID toolBox = pKernel->GetChild(player, ITEM_BOX_NAME_WSTR);
		if ( pKernel->Exists(toolBox) )
		{
			nAfter = ContainerModule::GetItemCount(pKernel, toolBox, log.itemID.c_str());
			if (log.eventType == LOG_ACT_EVENT_GAIN)
			{
				nBefore = nAfter - num;
			}
			else
			{
				nBefore = nAfter + num;
			}
		}
	}
	
	//10、组装物品异动参数
	CVarList logInfo;
	logInfo << strTime.c_str() << -1 << -1 << account << name << uid 
	     	<< log.eventID << log.eventType << log.eventSerial
		    << log.itemID << log.itemUid << log.itemLifeTime 
			<< nBefore << num << nAfter << colorLevel 
			<< log.senderName << log.senderUid
			<< sceneID << "" << guildName << log.state << strComment.c_str();

	// ANALYTICES:
	if (NULL != m_pAnaCollector)
	{
		// 邮件转发的不记录到神策
		if (log.eventID != FUNCTION_EVENT_ID_ITEM_TO_MAIL && log.eventID != FUNCTION_EVENT_ID_SYSTEM_MAIL
			&& log.eventID != FUNCTION_EVENT_ID_GM_COMMAND 
			&& log.eventID != FUNCTION_EVENT_ID_SYS )
		{
			int level = playerObj->QueryInt(FIELD_PROP_LEVEL);
			int vip = playerObj->QueryInt(FIELD_PROP_VIP_LEVEL);
			if (log.eventType == LOG_ACT_EVENT_GAIN)
			{
				OnGainItem(pKernel, player, log.itemID.c_str(), colorLevel, num, nAfter, nBefore, 0, log.eventID, NULL);
			}
			else
			{
				OnCostItem(pKernel, player, log.itemID.c_str(), colorLevel, num, nAfter, nBefore, 0, log.eventID, NULL);
			}
		}
	}

	//11、保存物品异动日志
	return pKernel->CustomLog("item_log", logInfo);
}


/*!
* @brief	升级日志(人物、技能、军阶)
* @param    核心指针
* @param    玩家对象
* @param    升级日志结构
* @return	结果bool
*/
bool LogModule::SaveRoleUpgradeLog(IKernel* pKernel, const PERSISTID& player, const RoleUpgradeLog &log)
{
	// 1、基础容错
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、判断账号和玩家名字
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveRoleUpgradeLog] account is NULL or empty! type -> %d ", log.type);
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveRoleUpgradeLog] name is NULL or empty! type -> %d ", log.type);
		return false;
	}

	//4、日志时间
	std::string strTime = m_pLogModule->GetLogTime();

	//5、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//6、日志参数
	CVarList logInfo;
	logInfo << account << name << strTime.c_str() << log.type << log.levelBefore << log.levelAfter << log.skillID << strComment.c_str();

	// ANALYTICS:
	if (NULL != m_pAnaCollector)
	{
		switch ( log.type )
		{
		case LOG_ROLE_UPGRADE_PLAYER:
			this->OnLevelup(pKernel, player, log.levelAfter, log.levelBefore);
			break;
		default:
			break;
		}
	}

	//7、保存升级日志
	return pKernel->CustomLog("role_upgrade_log", logInfo);
}

/*!
* @brief	BOSS日志记录
* @param    核心指针
* @param    玩家对象
* @param    BOSS日志结构
* @return	结果bool
*/
bool LogModule::SaveBossLog(IKernel* pKernel, const PERSISTID& player, const BossLog &log)
{
	//1、基础容错
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	//称号
	//MiddleModule::Instance()->OnTitleConditionStr(pKernel, player, ETitleCond_KillBossID, log.bossID.c_str());

	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、账号和名字合法性检查
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveBossLog] account is NULL or empty! bossID -> %d ", log.bossID);
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveBossLog] name is NULL or empty! bossID -> %d ", log.bossID);
		return false;
	}

	//4、日志时间
	std::string strTime = m_pLogModule->GetLogTime();

	//5、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//6、获取真实场景ID
	int sceneID = pKernel->GetSceneId();
	if(pKernel->GetSceneClass() == 2)
	{
		sceneID = pKernel->GetPrototypeSceneId(sceneID);
	}

	//7、保存Boss日志
	CVarList logInfo;
	logInfo << account << name << strTime.c_str() << sceneID << log.bossID << strComment.c_str();

	return pKernel->CustomLog("boss_log", logInfo);
}

/*!
* @brief	场景日志
* @param    核心指针
* @param    玩家对象
* @param    场景日志结构
* @return	结果bool
*/
bool LogModule::SaveSceneLog(IKernel* pKernel, const PERSISTID& player, const SceneLog &log)
{
	//1、基础容错
	if ( NULL == pKernel || !pKernel->Exists(player)  || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、账号和名字合法性检查
	const char* account = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(account))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveSceneLog] account is NULL or empty! ");
		return false;
	}
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveSceneLog] name is NULL or empty! ");
		return false;
	}

	//4、备注字符长度检查
	std::wstring strComment = log.comment;
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	//5、获取真实场景ID
	int sceneID = pKernel->GetSceneId();
	if(pKernel->GetSceneClass() == 2)
	{
		sceneID = pKernel->GetPrototypeSceneId(sceneID);
	}

	//6、获取登录时间，下线时间
	std::string strTimeIn = m_pLogModule->GetLogTime(log.loginTime);
	std::string strTimeOut = m_pLogModule->GetLogTime(log.logoutTime);
	int64_t totalTime = log.logoutTime - log.loginTime;

	//7、保存Boss日志
	CVarList logInfo;
	logInfo << account << name << sceneID << strTimeIn.c_str() << strTimeOut.c_str() << totalTime << strComment.c_str();

	return pKernel->CustomLog("scene_log", logInfo);
}



/*!
* @brief	记录玩家日志
* @param    核心指针
* @param    玩家对象
* @param    日志类型
* @param    扩展参数
* @return	结果bool
*/
bool LogModule::SavePlayerLog(IKernel* pKernel, const PERSISTID& player,
							  const int logType, const wchar_t* comment)
{
	//1、基础容错和参数类型判断
	if ( NULL == pKernel || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、参数判断
	std::wstring strComment = L"";
	if( !StringUtil::CharIsNull(comment) )
	{
		if( std::wstring(comment).length() >= COMMENT_MAX_LEN )
		{
			strComment = StringUtil::SubstrW(comment, 0, COMMENT_MAX_LEN - 1 );
		}
		else
		{
			strComment = comment;
		}
	}

	//4、 名字合法性
	const wchar_t* name = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SavePlayerLog without Param:name] name is NULL or empty! ");
		return false;
	}

	//5、备注
	const wchar_t* content = L"SavePlayerLog";

	//6、保存玩家日志
	return pKernel->SaveLog(name, logType, content, strComment.c_str());
}


/*!
* @brief	GM日志
* @param    核心指针
* @param    内容
* @param    扩展参数
* @return	结果bool
*/
bool LogModule::SaveGMLog(IKernel* pKernel, const PERSISTID& player, const wchar_t* GMContent, const wchar_t* comment)
{
	//1、基础容错和参数合法性检查
	if ( NULL == pKernel || !pKernel->Exists(player) || StringUtil::CharIsNull(GMContent) || StringUtil::CharIsNull(comment) )
	{
		return false;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return false;
	}

	//3、账号
	const char* acc = playerObj->QueryString("Account");
	if (StringUtil::CharIsNull(acc))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveGMLog] account is NULL or empty! ");
		return false;
	}
	std::wstring account = std::wstring(L"*") + ::util_string_as_widestr(acc);

	// 保存GM命令日志
	return pKernel->GmLog(account.c_str(), 0, GMContent, comment);
}

/*!
* @brief	邮件奖励日志
* @param    核心指针
* @param    邮件奖励日志结构
* @return	结果bool
*/
bool LogModule::SaveMailAwardLog( IKernel* pKernel, const MailLog &log )
{
	const wchar_t* name = log.name.c_str();
	if (StringUtil::CharIsNull(name))
	{
		::extend_warning(LOG_ERROR, "[LogModule::SaveMailAwardLog] name is NULL or empty! logType -> %s ", log.templateType.c_str());
		return false;
	}

	// 2、日志时间
	std::string strTime = m_pLogModule->GetLogTime();

	// 3、备注字符长度检查
	std::wstring strComment = log.comment;
	std::string strItemParams = log.itemParams;
	
	if (log.comment.length() >= COMMENT_MAX_LEN)
	{
		strComment = StringUtil::SubstrW(log.comment.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	if (log.itemParams.length() >= COMMENT_MAX_LEN)
	{
		strItemParams = StringUtil::Substr(log.itemParams.c_str(), 0, COMMENT_MAX_LEN - 1 );
	}

	// 4、保存邮件奖励日志
	CVarList logInfo;
	logInfo << "" << name << strTime.c_str() 
		    << log.templateType.c_str() << log.capitalParams.c_str() 
			<< strItemParams.c_str() << strComment.c_str();
	return pKernel->CustomLog("mail_log", logInfo);
}

//场景创建
int LogModule::OnCreate(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (pKernel->GetSceneId() != STATICS_SCENE_ID)
	{
		return 0;
	}
	
	// 10s一次心跳 保证偏差值在0-10之间
	ADD_HEART_BEAT(pKernel, self, "LogModule::H_RecOnlineCount", 10000);
	return 0;
}

// 记录在线玩家数
int LogModule::H_RecOnlineCount(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	if (NULL == m_pLogModule->m_pAnaCollector)
	{
		return 0;
	}

	// 默认值为0， 开启第一次执行报告操作
	time_t tNow = util_get_utc_time();
	if (tNow >= ms_nNextReportCount)
	{
		int nCurOnlineCount = 0;
#ifndef FSROOMLOGIC_EXPORTS
		nCurOnlineCount = pKernel->GetOnlineCount();
#endif
		char szid[32] = { 0 };
#ifndef FSROOMLOGIC_EXPORTS
		SPRINTF_S(szid, "%d", pKernel->GetServerId());
#else
		SPRINTF_S(szid, "%d", pKernel->GetRoomId());
#endif

		KVArgs args;
		args[GAME_ANALYTICS_PROPERTIES_ONLINE_COUNT] = nCurOnlineCount;
		m_pLogModule->m_pAnaCollector->OnCustomEvent(szid, 0, 0, GAME_ANALYTICS_EVENT_ONLINE_COUNT, 0, 0, &args);

		// 获得下一个时间
		//int hour = 0, minute = 0, seconds = 0;
		//// 保证跳过整点值
		//int nWeek = util_get_day_of_week_ex(tNow + HB_REPORT_COUNT_INTERVAL, hour, minute, seconds);
		//seconds += minute * 60 + hour * 3600;
		//seconds %= REPORT_COUNT_INTERVAL;
		time_t seconds = tNow % REPORT_COUNT_INTERVAL;
		ms_nNextReportCount = tNow - seconds + REPORT_COUNT_INTERVAL;
	}
	return 0;
}

// 获取标准格式的日志时间
std::string LogModule::GetLogTime(time_t slice)
{
	if (slice <= 0)
	{
		slice = time(NULL);
	}

	tm* t = ::localtime(&slice);
	if (NULL == t)
	{
		return std::string("");
	}
	char buf[32] = "\0";
	SPRINTF_S(buf, "%04d%02d%02d %02d:%02d:%02d", 
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return buf;
}