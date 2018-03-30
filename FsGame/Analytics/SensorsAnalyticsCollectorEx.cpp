#include "SensorsAnalyticsCollectorEx.h"
#include "public/Debug.h"
#include "utils/util_func.h"
#include "GameAnalyticsEventNames.h"
#include "GameAnalyticsProperties.h"
#include "utils/string_util.h"
#include "utils/util_ini.h"
#include "Define/GameDefine.h"

SensorsAnalyticsCollectorEx::SensorsAnalyticsCollectorEx(IAnalytisLogger& logger, 
												int64_t nDeployId,			// 布署id
												int64_t nGameId,
												int64_t nSrvId,
												int64_t nDistrictId,
												int64_t nChannelId,				// 游戏归类渠道： 联运或专用渠道
												int64_t nProductionId,			// 生产id: 正式环境还是开发环境 ...
												const char* gameVer, 
												const char* gameName,
												const char* strDistrictlName )
	: m_nDeployId(nDeployId)
	, m_nGameId(nGameId)
	, m_nSrvId(nSrvId)
	, m_nDistrictId( nDistrictId )
	, m_nChannelId( nChannelId )
	, m_nProductionId( nProductionId )
	, m_anaLogger(logger)
	
{
	if (NULL != gameVer)
	{
		m_strGameVer = gameVer;
	}

	if (NULL != gameName)
	{
		m_strGameName = gameName;
	}

	if (NULL != strDistrictlName)
	{
		m_strChannelName = strDistrictlName;
	}
}

void SensorsAnalyticsCollectorEx::FillSensorsTrackHeader(Json::Value& root,
										const char* type,
										const char* evt,
										const char* roleid,
										int level )
{
	Assert(NULL != roleid);
	Assert(NULL != type);
	Assert(NULL != evt);

	root[SENSORS_INNER_DISTINCT_ID] = roleid;
	root[SENSORS_INNER_TYPE] = type;
	root[SENSORS_INNER_EVENT] = evt;

	// 时间处理
	int64_t tNow = util_get_utc_ms_time();
	
		// 判断玩家是否在缓存中
	auto it = m_strRoleCache.find(roleid);
	if (it != m_strRoleCache.end())
	{
		if (tNow <= it->second)
		{
#ifdef FSGAME_EXPORTS
			if (it->second - tNow >= 5000)
			{
				::extend_warning(LOG_WARNING, "role event too much role=%s",roleid);
			}
#endif
			it->second += 1;
			// 增加时间到下一ms
			tNow = it->second;
		}
		else
		{
			it->second = tNow;
		}
		
	}
	else
	{
		m_strRoleCache[roleid] = tNow;
	}
	// 使用毫秒计时器
	root[SENSORS_INNER_TIME] = tNow;

	if (!m_strGameName.empty())
	{
		root[SENSORS_INNER_PROJECT] = m_strGameName;
	}

	// 填充属性
	Json::Value& properties = root[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_DEPLOY_ID] = m_nDeployId;
	properties[GAME_ANALYTICS_PROPERTIES_GAME_ID] = m_nGameId;
	properties[GAME_ANALYTICS_PROPERTIES_SERVER_ID] = m_nSrvId;
	properties[GAME_ANALYTICS_PROPERTIES_DISTRICT_ID] = m_nDistrictId;
	properties[GAME_ANALYTICS_PROPERTIES_GAME_VER] = m_strGameVer;
	properties[GAME_ANALYTICS_PROPERTIES_CHANNEL] = m_nChannelId;
	properties[GAME_ANALYTICS_PROPERTIES_PRODUCTION_ID] = m_nProductionId;
	//properties[GAME_ANALYTICS_PROPERTIES_DISTRICT_NAME] = m_strChannelName;
	// 日期
	// std::string date;
	// util_convert_date_to_string2(date);
	// properties[GAME_ANALYTICS_PROPERTIES_DATE] = date;
	
	// 角色id
	//properties[GAME_ANALYTICS_PROPERTIES_UID] = uid;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_ID] = roleid;
	if (level > 0)
	{
		properties[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	}
}

void SensorsAnalyticsCollectorEx::FillModuleInfo(Json::Value& root, int module, int func, const char* extra, const KVArgs* args/* = NULL*/) const
{
	//const char* pModuleName = GetModuleName(module);
	//const char* pFuncName = GetFuncName(func);

	//InsertString(root, GAME_ANALYTICS_PROPERTIES_GAME_MODULE, pModuleName);
	//InsertString(root, GAME_ANALYTICS_PROPERTIES_GAME_ACTION, pFuncName);
	if (module == 0)
	{
		module = func;
	}
	const char* actoinName = GetActionName(module);
	if (actoinName != NULL){
		//root[GAME_ANALYTICS_PROPERTIES_GAME_MODULE] = module;
		//root[GAME_ANALYTICS_PROPERTIES_GAME_ACTION] = actoinName;
		root[actoinName] = func;
	}
	InsertString(root, GAME_ANALYTICS_PROPERTIES_EXTRA_PARAM, extra);
	FillCustomArgs(root, args);
}

void SensorsAnalyticsCollectorEx::FillSensorsProfileHeader(Json::Value& root, const char* roleid, const char* type)
{
	Assert(NULL != roleid);
	Assert(NULL != type);

	root[SENSORS_INNER_DISTINCT_ID] = roleid;
	root[SENSORS_INNER_TYPE] = type;
	root[SENSORS_INNER_TIME] = util_get_utc_ms_time();

	// 达纳数据要求加上分区信息
	Json::Value& properties = root[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_DEPLOY_ID] = m_nDeployId;
	properties[GAME_ANALYTICS_PROPERTIES_GAME_ID] = m_nGameId;
	properties[GAME_ANALYTICS_PROPERTIES_SERVER_ID] = m_nSrvId;
	properties[GAME_ANALYTICS_PROPERTIES_DISTRICT_ID] = m_nDistrictId;
	properties[GAME_ANALYTICS_PROPERTIES_CHANNEL] = m_nChannelId;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_ID] = roleid;
}


bool SensorsAnalyticsCollectorEx::InitAction()
{
	// 功能来源
	LogActionType lgFunction;
	lgFunction.m_actionName = "function_id";
	lgFunction.m_actionMin = FUNCTION_EVENT_ID_SYS - 1;
	lgFunction.m_actionMax = FUNCTION_EVENT_ID_END;
	m_action.push_back(lgFunction);

	// 任务类型
// 	LogActionType lgTaskType;
// 	lgTaskType.m_actionName = "task_log_type";
// 	lgTaskType.m_actionMin = LOG_TASK_STATE_MIN;
// 	lgTaskType.m_actionMax = LOG_TASK_STATE_MAX;
// 	m_action.push_back(lgTaskType);
// 
// 	// 游戏功能
// 	LogActionType lgGameType;
// 	lgGameType.m_actionName = "game_func_log_type";
// 	lgGameType.m_actionMin = LOG_GAME_ACTION_MIN;
// 	lgGameType.m_actionMax = LOG_GAME_ACTION_MAX;
// 	m_action.push_back(lgGameType);

	// 玩家升级
	LogActionType lgUpgradeType;
	lgUpgradeType.m_actionName = "role_upgrade_type";
	lgUpgradeType.m_actionMin = LOG_ROLE_UPGRADE_MIN;
	lgUpgradeType.m_actionMax = LOG_ROLE_UPGRADE_MAX;
	m_action.push_back(lgUpgradeType);

// 	// 惩罚日志类型
// 	LogActionType lgPunishType;
// 	lgPunishType.m_actionName = "punish_log_type";
// 	lgPunishType.m_actionMin = LOG_PUNISH_MIN;
// 	lgPunishType.m_actionMax = LOG_PUNISH_MAX;
// 	m_action.push_back(lgPunishType);
// 
// 	// 玩家日志
// 	LogActionType lgPlayerType;
// 	lgPlayerType.m_actionName = "player_log_type";
// 	lgPlayerType.m_actionMin = LOG_PLAYER_MIN;
// 	lgPlayerType.m_actionMax = LOG_PLAYER_MAX;
// 	m_action.push_back(lgPlayerType);
// 
// 	// 好友日志
// 	LogActionType lgFridType;
// 	lgFridType.m_actionName = "friend_log_type";
// 	lgFridType.m_actionMin = LOG_FRIEND_MIN;
// 	lgFridType.m_actionMax = LOG_FRIEND_MAX;
// 	m_action.push_back(lgFridType);
// 
// 	// 装备记录
// 	LogActionType lgEquipType;
// 	lgEquipType.m_actionName = "equip_log_type";
// 	lgEquipType.m_actionMin = LOG_EQUIP_MIN;
// 	lgEquipType.m_actionMax = LOG_EQUIP_MAX;
// 	m_action.push_back(lgEquipType);
// 
// 	// 公会日志类型
// 	LogActionType lgGuildType;
// 	lgGuildType.m_actionName = "guild_log_type";
// 	lgGuildType.m_actionMin = LOG_GUILD_MIN;
// 	lgGuildType.m_actionMax = LOG_GUILD_MAX;
// 	m_action.push_back(lgGuildType);
// 
// 	// 游戏玩法
// 	LogActionType lgGameLogType;
// 	lgGameLogType.m_actionName = "game_log_state";
// 	lgGameLogType.m_actionMin = LOG_GAME_ACTION_STATE_MIN;
// 	lgGameLogType.m_actionMax = LOG_GAME_ACTION_STATE_MAX;
// 	m_action.push_back(lgGameLogType);
// 
// 	// 坐骑日志
// 	LogActionType lgRideType;
// 	lgRideType.m_actionName = "pet_ride_log_type";
// 	lgRideType.m_actionMin = LOG_PET_RIDE_MIN;
// 	lgRideType.m_actionMax = LOG_PET_RIDE_MAX;
// 	m_action.push_back(lgRideType);
// 
// 	// 国家系统日志
// 	LogActionType lgNationType;
// 	lgNationType.m_actionName = "nation_log_type";
// 	lgNationType.m_actionMin = LOG_NATION_MIN;
// 	lgNationType.m_actionMax = LOG_NATION_MAX;
// 	m_action.push_back(lgNationType);
// 
// 	// 称号日志
// 	LogActionType lgTitleType;
// 	lgTitleType.m_actionName = "title_log_type";
// 	lgTitleType.m_actionMin = LOG_TITLE_MIN;
// 	lgTitleType.m_actionMax = LOG_TITLE_MAX;
// 	m_action.push_back(lgTitleType);
// 
// 	// 经脉系统日志
// 	LogActionType lgMeridianType;
// 	lgMeridianType.m_actionName = "meridian_log_type";
// 	lgMeridianType.m_actionMin = LOG_MERIDIAN_LEVEL_UP;
// 	lgMeridianType.m_actionMax = LOG_MERIDIAN_MAX;
// 	m_action.push_back(lgMeridianType);
	return true;
}

void SensorsAnalyticsCollectorEx::OnServerStart()
{
	// 写入一个空行标志游戏开始
	//ANALYTICS_LOG(m_anaLogger, "{}\r\n");
}

void SensorsAnalyticsCollectorEx::OnServerClose()
{

}

void SensorsAnalyticsCollectorEx::OnClientInfo(
									const char* roleid,			// 角色id(require)
									bool bfirst,				// 是否是首次
									const char* clientver,
									const char* ip,				// 终端id(option)
									const char* platform,		// 注册平台(option)
									bool wifi,					// 是否使用wifi(option)
									const char* manufacturer,	// 制造商(option)
									const char* devid,			// 终端设备唯一标识(option)
									const char* model,			// 终端类型(option)
									const char* os,				// 终端系统(option)
									const char* os_ver,			// 终端系统版本(option)
									const KVArgs * args)
								
{
	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	if (bfirst)
	{
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_DEVICE_ID, devid);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_MANUFACTURER, manufacturer);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_MODEL, model);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_OS, os);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_OS_VER, os_ver);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_CLIENT_VER, clientver);
		profileset[GAME_ANALYTICS_PROPERTIES_WIFI] = wifi;
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_REGISTER_IP, ip);
	}
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_LOGIN_IP, ip);
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SET_CLIENT, roleid, 0);

	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_DEVICE_ID, devid);
	InsertString(properties, SENSORS_PROPERTY_IP, ip);
	properties[SENSORS_PROPERTY_WIFI] = wifi;
	properties[GAME_ANALYTICS_PROPERTIES_IS_REGISTER] = bfirst;
	InsertString(properties, SENSORS_PROPERTY_MANUFACTURER, manufacturer);
	InsertString(properties, SENSORS_PROPERTY_MODEL, model);
	InsertString(properties, SENSORS_PROPERTY_OS, os);
	InsertString(properties, SENSORS_PROPERTY_OS_VER, os_ver);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_CLIENT_VER, clientver);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );

}


void SensorsAnalyticsCollectorEx::OnNewUser(const char* uid, 
											const char* anid,
											const char* nick, 
											int sex, 
											__int64 bron )
{
	Assert(false);
}

void SensorsAnalyticsCollectorEx::OnDelUser(const char* uid)
{
	Assert(false);
}

void SensorsAnalyticsCollectorEx::OnUserLogin(const char* uid,			// 用户唯一id (require)
													const char* nick,		// 昵称/用户名(require)
													const char* ip,			// 终端ip(option)
													const char* platform,	// 平台(option)
													const char* manufacturer,	// 制造商(option)
													const char* devid,		// 终端设备唯一标识(option)
													const char* model,		// 终端类型(option)
													const char* os,			// 终端系统(option)
													const char* os_ver,		// 终端系统版本(option)
													bool bReset/* = false*/)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SIGNIN, uid, 0);
	trackdata.removeMember(GAME_ANALYTICS_PROPERTIES_ROLE_ID);
	trackdata[SENSORS_INNER_DISTINCT_ID] = uid;
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_LOGIN_IP, ip);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_DEVICE_ID, devid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_OS, os);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_OS_VER, os_ver);

	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnUserLogout(const char* uid, int64_t nTimestamp/* = 0*/, bool bReset/* = false*/)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SIGNOUT, uid, 0);
	trackdata.removeMember( GAME_ANALYTICS_PROPERTIES_ROLE_ID );
	// 填充属性
	trackdata[SENSORS_INNER_DISTINCT_ID] = uid;
	if (nTimestamp > 0)
	{
		trackdata[SENSORS_INNER_TIME] = nTimestamp;
	}

	
	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnNewRole(const char* roleid,
	const char* uid,
	const char* roleName,
	int job,
	int sex,
	int flag,
	const char* ip,
	const KVArgs* args
	)
{
	// profile
	{
		Json::Value profiledata;
		FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
		Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
		profileset[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
		profileset[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
		profileset[GAME_ANALYTICS_PROPERTIES_ROLE_FLAG] = flag;
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_UID, uid);
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_GAME_VER, m_strGameVer.c_str());
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_GAME_NAME, m_strGameName.c_str());
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_REGISTER_IP, ip);
		profileset[GAME_ANALYTICS_PROPERTIES_GAME_ID] = m_nGameId;
		profileset[GAME_ANALYTICS_PROPERTIES_SERVER_ID] = m_nSrvId;
		profileset[GAME_ANALYTICS_PROPERTIES_DISTRICT_ID] = m_nDistrictId;
		profileset[GAME_ANALYTICS_PROPERTIES_CHANNEL] = m_nChannelId;
		profileset[GAME_ANALYTICS_PROPERTIES_PRODUCTION_ID] = m_nProductionId;
		// 创日期
		std::string date;
		util_convert_date_to_string2(date);
		profileset[GAME_ANALYTICS_PROPERTIES_CTIME] = date;
		FillSensorsProfileArgs(profileset, job, args);
		WriteAnalyticsLog(profiledata);
	}

	{
		Json::Value trackdata;
		FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_NEW_ROLE, roleid, 1);
		// 填充属性
		Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
		properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
		properties[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
		properties[GAME_ANALYTICS_PROPERTIES_ROLE_FLAG] = flag;
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_REGISTER_IP, ip);
		FillCustomArgs(properties, args);
		WriteAnalyticsLog(trackdata);
	}

	// 记录用户新建角色
	{
		Json::Value trackdata;
		FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_U_NEW_ROLE, roleid, 1);
		// 填充属性
		trackdata[SENSORS_INNER_DISTINCT_ID] = uid;
		Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_REGISTER_IP, ip);
		properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
		properties[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
		properties[GAME_ANALYTICS_PROPERTIES_ROLE_FLAG] = flag;
		WriteAnalyticsLog(trackdata);
	}
}

void SensorsAnalyticsCollectorEx::OnDelRole(const char* roleid, int level, const char* uid)
{
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_DELETE);
	WriteAnalyticsLog(profiledata);	
	
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_DEL_ROLE, roleid, level);
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
	WriteAnalyticsLog( trackdata );

}

void SensorsAnalyticsCollectorEx::OnRoleEntry(const char* roleid,		// 角色id
												const char* uid,			// 角色所属uid(require)
												int level,					// 角色等级
												int vip,					// 角色vip
												int64_t gold,				// 当前所拥有的金币
												const char* ip,				// ip
												const KVArgs* args,			// 参数列表(属性名键值对)
												bool bReset/* = false*/)
{
	// profile
	{
		Json::Value profiledata;
		FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
		Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
		InsertString(profileset, GAME_ANALYTICS_PROPERTIES_UID, uid);
		profileset[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
		profileset[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
		profileset[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
		// 登陆时间
		std::string date;
		util_convert_date_to_string2(date);
		profileset[GAME_ANALYTICS_PROPERTIES_LTIME] = date;
		profileset[GAME_ANALYTICS_PROPERTIES_ONLINE] = true;
		if ( NULL != ip && ip[0] != '\0' )
		{
			profileset[GAME_ANALYTICS_PROPERTIES_LOGIN_IP] = ip;
		}
		WriteAnalyticsLog(profiledata);
	}

	{
		Json::Value trackdata;
		FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_ROLE_ENTRY, roleid, level);
		// 填充属性
		Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_LOGIN_IP, ip);
		properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
		properties[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
		FillCustomArgs(properties, args);
		WriteAnalyticsLog(trackdata);
	}

	{
		Json::Value trackdata;
		FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_U_ROLE_ENTRY, roleid, level);
		// 填充属性
		trackdata[SENSORS_INNER_DISTINCT_ID] = uid;
		Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_LOGIN_IP, ip);
		InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
		properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
		properties[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
		FillCustomArgs(properties, args);
		WriteAnalyticsLog(trackdata);
	}
}

void SensorsAnalyticsCollectorEx::OnRoleExit(const char* roleid, 		// 角色id
											const char* uid,			// 角色所属uid(require)
											int level,					// 角色等级
											int vip,					// 角色vip
											int64_t gold,				// 当前所拥有的金币
											int64_t onlinetime,			// 在线时长
											int64_t totaltime,			// 总在线时长
											const char* ip,				// ip
											const KVArgs* args,			// 参数列表(属性名键值对)
											bool bReset/* = false*/)
{
	// ?? 在线时长字段表示意思冲突(profile和track)
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_UID, uid);
	profileset[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
	profileset[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	profileset[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	profileset[GAME_ANALYTICS_PROPERTIES_ONLINETIME] = totaltime;
	// 退出时间
	std::string date;
	util_convert_date_to_string2(date);
	profileset[GAME_ANALYTICS_PROPERTIES_QTIME] = date;
	profileset[GAME_ANALYTICS_PROPERTIES_ONLINE] = false;
	if (NULL != ip && ip[0] != '\0')
	{
		profileset[GAME_ANALYTICS_PROPERTIES_LOGIN_IP] = ip;
	}
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_ROLE_EXIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_LOGIN_IP, ip);
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
	properties[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	properties[GAME_ANALYTICS_PROPERTIES_ONLINETIME] = onlinetime;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );

	if (!bReset)
	{
		auto it = m_strRoleCache.find(uid);
		if (it != m_strRoleCache.end())
		{
			m_strRoleCache.erase(it);
		}
	}
}


void SensorsAnalyticsCollectorEx::OnPaying(
									const char* roleid,			// 角色id(require)
									const char* uid,			// 角色所属uid(require)
									int level,					// 角色等级
									int vip,					// 角色vip等级
									const char* orderid, 
									const char* itemid, 
									int count, 
									float price,				// 产品价格(require)
									bool first,					// 是否首充
									const char* platform		// 客户端os(android, ios, pc...)
									)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PAY_START, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ORDER_ID, orderid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ITEM_ID, itemid);
	properties[GAME_ANALYTICS_PROPERTIES_PAY_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_ITEM_PRICE] = price;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_FIRST] = first;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnPayCompleted(
												const char* roleid,				// 角色id(require)
												const char* uid,				// 角色所属uid(require)
												int level,						// 角色等级
												int vip,						// 角色vip等级
												const char* orderid,			// 订单id(require)
												float amount,					// 实际支付金额(require)
												float totalamount,				// 总支付金额
												float realamount,				// 真实支付金额
												int paytimes,					// 总充值次数
												int result,						// 支付结果(require) 0:成功 1:失败 2:取消
												bool first,						// 是否首充
												bool bIsReal,					// 是否虚拟币充值
												const char* platform,			// 支付平台(require)(xy, xiaomi,baidu....)
												const char* os					// 客户端os
											)
{
	// 累加支付金额
	Json::Value paydata;
	FillSensorsProfileHeader(paydata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& payset = paydata[SENSORS_INNER_PROPERTYIES];
	payset[GAME_ANALYTICS_PROPERTIES_PAY_TOTAL_AMOUNT] = totalamount;
	payset[GAME_ANALYTICS_PROPERTIES_PAY_REAL_AMOUNT] = realamount;
	payset[GAME_ANALYTICS_PROPERTIES_PAY_VIRTUAL_AMOUNT] = totalamount-realamount;
	payset[GAME_ANALYTICS_PROPERTIES_PAY_TIMES] = paytimes;
	payset[GAME_ANALYTICS_PROPERTIES_PAY_LAST_TIME] = util_get_utc_time();
	WriteAnalyticsLog(paydata);	
	
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PAY_END, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_UID, uid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ORDER_ID, orderid);
	properties[GAME_ANALYTICS_PROPERTIES_PAY_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_FIRST] = first;
	if (bIsReal)
	{
		properties[GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT_1] = 0;
		properties[GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT] = amount;
	}
	else
	{
		properties[GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT_1] = amount;
		properties[GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT] = 0;
	}
	properties[GAME_ANALYTICS_PROPERTIES_PAY_TIMES] = paytimes;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_PLATFORM, platform);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_OS, os);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnIncExp(
										const char* roleid,		// 角色id(require)
										int level,				// 角色等级
										int value,					// 增加值(require)
										int newValue,				// 增加后的值
										int origValue,				// 原始值
										int source	,				// 来源
										const KVArgs * args
										)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_INC_EXP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_INC_EXP] = value;
	properties[GAME_ANALYTICS_PROPERTIES_ORIG_EXP] = origValue;
	properties[GAME_ANALYTICS_PROPERTIES_EXP] = newValue;
	FillCustomArgs(properties, args);
	FillModuleInfo(properties, 0, source, NULL);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnLevelup(const char* roleid, int level, int original, const KVArgs * args)
{
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	profileset[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	WriteAnalyticsLog(profiledata);	
	
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_LEVELUP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_PRE_LEVEL] = original;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

// 经脉等级增加
void SensorsAnalyticsCollectorEx::OnMeridianLevelup(const char* roleid, int level, int vip, int value, int original,const KVArgs * args)
{
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	profileset[GAME_ANALYTICS_PROPERTIES_MERIDIAN_LEVEL] = value;
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_MERIDIAN_UP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_MERIDIAN_LEVEL] = value;
	properties[GAME_ANALYTICS_PROPERTIES_MERIDIAN_PRE_LEVEL] = original;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnGainCoin(
												const char* roleid,			// 角色id(require)
												int level,					// 角色当前等级(require)
												int vip,					// vip等级(require)
												int coid,
												int64_t value,
												int64_t newvalue,
												int64_t original,
												int module, 
												int func, 
												const char* extra,
												const KVArgs * args)
{
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	// profile
	switch (coid)
	{
	case CAPITAL_GOLD:
		profileset[GAME_ANALYTICS_PROPERTIES_SILVER]=  newvalue;
		break;
	case  CAPITAL_COPPER:
		profileset[GAME_ANALYTICS_PROPERTIES_COPPER] = newvalue;
		break;
	case  CAPITAL_BIND_GOLD:
		// TODO
		break;
	case  CAPITAL_TSILVER:
		// TODO
		break;
	case  CAPITAL_SMELT:
		// TODO
		break;
	default:
		Assert(false);
	}
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_INC_COIN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ID] = coid;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_NEW_VALUE] = newvalue;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ORIGINAL] = original;
	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnCostCoin(
											const char* roleid,			// 角色id(require)
											int level,				// 角色当前等级(require)
											int vip,				// vip等级(require)
											int coid, 
											int64_t value,
											int64_t newvalue,
											int64_t original,
											int module, 
											int func, 
											const char* extra,
											const KVArgs * args)
{
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	// profile
	switch (coid)
	{
	case CAPITAL_GOLD:
		profileset[GAME_ANALYTICS_PROPERTIES_SILVER] = newvalue;
		break;
	case  CAPITAL_COPPER:
		profileset[GAME_ANALYTICS_PROPERTIES_COPPER] = newvalue;
		break;
	case CAPITAL_TSILVER:
		// TODO
		break;
	case CAPITAL_BIND_GOLD:
		// TODO
		break;
	case CAPITAL_SMELT:
		// TODO
		break;
// 	case CAPITAL_BATTLE_SOUL:
// 		profileset[GAME_ANALYTICS_PROPERTIES_SOUL] = newvalue;
// 		break;
// 	case CAPITAL_SMELT:
// 		profileset[GAME_ANALYTICS_PROPERTIES_SMELT] = newvalue;
// 		break;
// 	case  CAPITAL_EXPLOIT:
// 		profileset[GAME_ANALYTICS_PROPERTIES_EXPLOIT] = newvalue;
// 		break;
// 	case  CAPITAL_HONOR:
// 		profileset[GAME_ANALYTICS_PROPERTIES_HONOR] = newvalue;
// 		break;
// 	case  CAPITAL_CHIVALRY:
// 		profileset[GAME_ANALYTICS_PROPERTIES_CHIVALRY] = newvalue;
// 		break;
// 	case  CAPITAL_TRADE_SILVER:
// 		profileset[GAME_ANALYTICS_PROPERTIES_TRADE_SILVER] = newvalue;
// 		break;
// 	case  CAPITAL_CAMPVALUE:
// 		profileset[GAME_ANALYTICS_PROPERTIES_CAMPVALUE] = newvalue;
// 		break;
// 	case  CAPITAL_WISH_SCORE:
// 		profileset[GAME_ANALYTICS_PROPERTIES_WISHSCORE] = newvalue;
// 		break;
// 	case CAPITAL_GUILD_CURRENCY:
// 		profileset[GAME_ANALYTICS_PROPERTIES_GUILD_CURRENCY] = newvalue;
// 		break;
	default:
		Assert(false);
	}
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_DEC_COIN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ID] = coid;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_NEW_VALUE] = newvalue;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ORIGINAL] = original;

	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog(trackdata);

}

void SensorsAnalyticsCollectorEx::OnGainItem(
												const char* roleid,			// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												const char* itemid, 
												int num,				// 获得数值(require)
												int count,				// 当前拥有数
												int orig,				// 原来拥有数
												int module, 
												int func, 
												const char* extra,
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GAIN_ITEM, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_ID, itemid);
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_NUM] = num;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_ORIG_COUNT] = orig;
	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnCostItem(
												const char* roleid,			// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												const char* itemid, 
												int num,				// 获得数值(require)
												int count,				// 当前拥有数
												int orig,				// 原来拥有数
												int module,
												int func, 
												const char* extra,
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COST_ITEM, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_ID, itemid);
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_NUM] = num;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_ORIG_COUNT] = orig;
	FillCustomArgs(properties, args);
	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnVipLevelup(
												const char* roleid,			// 角色id(require)
												int level,				// 角色当前等级(require)
												int value, 
												int original, 
												int module, 
												int func, 
												const char* extra,
												const KVArgs * args)
{
	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	profileset[GAME_ANALYTICS_PROPERTIES_VIP] = value;
	WriteAnalyticsLog(profiledata);

	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_VIPUP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = value;
	properties[GAME_ANALYTICS_PROPERTIES_PRE_VIP] = original;

	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnPlayerPropertyChanged(
											const char* roleid,			// 角色id(require)
											int level,				// 角色当前等级(require)
											int vip,				// vip等级(require)
											const char* propery, 
											float value, 
											float original, 
											int module, 
											int func, 
											const char* extra,
											const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PROPERTY_CHANGED, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY, propery);
	properties[GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_PRE_VALUE] = original;

	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnPlayerTransaction(const char* transid, 
									int transtype, 
									const char* sender, 
									const char* reciver, 
									const char* senderItem, 
									int senderCount, 
									const char* reciverItem, 
									int reciverCount,
									const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, transid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TRANSZCTION);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_TRANS_TYPE] = transtype;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SENDER, sender);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_RECIVER, reciver);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SENDER_ITEM, senderItem);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_RECIVER_ITEM, reciverItem);
	properties[GAME_ANALYTICS_PROPERTIES_SENDER_COUNT] = senderCount;
	properties[GAME_ANALYTICS_PROPERTIES_RECIVER_COUNT] = reciverCount;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );

}

void SensorsAnalyticsCollectorEx::OnGuideEvent(
										const char* roleid,			// 角色id(require)
										int level,				// 角色当前等级(require)
										int vip,				// vip等级(require)
										int guideid, int step, const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GUIDE, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_GUIDE_ID] = guideid;
	properties[GAME_ANALYTICS_PROPERTIES_GUIDE_STEP] = step;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnTaskStart(
												const char* roleid,		// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int taskid,				// 接取的任务id(require)
												int type,				// 任务类型(option) 游戏中定义
												int opt_type,			// 任务操作方式(option) 游戏中定义
												const int state,		// 任务状态	
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TASK_ACCEPT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_ID] = taskid;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_OPT_WAY] = opt_type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_STATE] = state;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnTaskCompleted(
												const char* roleid,		// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int taskid,				// 接取的任务id(require)
												int type,				// 任务类型(option) 游戏中定义
												int opt_type,			// 任务操作方式(option) 游戏中定义
												const int state,		// 任务状态	
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TASK_COMPLETE, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_ID] = taskid;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_OPT_WAY] = opt_type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_STATE] = state;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

// 交任务
void SensorsAnalyticsCollectorEx::OnTaskSubmit(
	const char* roleid, /* 角色id(require) */
	int level, /* 角色当前等级(require) */
	int vip, /* vip等级(require) */
	int taskid, /* 接取的任务id(require) */
	int type, /* 任务类型(option) 游戏中定义 */
	int opt_type, /* 任务操作方式(option) 游戏中定义 */
	const int state,		// 任务状态	
	const KVArgs * args
	)

{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TASK_SUBMIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_ID] = taskid;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_OPT_WAY] = opt_type;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_STATE] = state;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog(trackdata);
}

void SensorsAnalyticsCollectorEx::OnSceneOpen(
												const char* roleid,		// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int sid, 
												int chapter, 
												int type ,
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_OPEN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnSceneEntry(
													const char* roleid,		// 角色id(require)
													int level,				// 角色当前等级(require)
													int vip,				// vip等级(require)
													int sid,				// 副本id(require)
													int chapter, 
													int type,
													const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_ENTRY, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnSceneExit(
												const char* roleid,		// 角色id(require)
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int sid,				// 副本id(require)
												int chapter,			// 副本章节(option)
												int type,				// 副本类型(option) 游戏中定义
												time_t staytime,		// 驻留时间
												int diedNum,			// 死亡次数
												int result,				// 战斗结果(require)
												const KVArgs * args)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_EXIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_STAY_TIME] = staytime;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_DIED_NUM] = diedNum;
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnVisitFunction(
												const char* roleid,	// 角色id(require)
												int level,				// 角色等级(require)
												int vip,				// vip等级(require)
												int module,				// 功能模块(require), 游戏中定义(在什么模块)
												int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
												bool result,			// 结果(require) true:成功 false: 失败
												int value,				// 额外说明(option), 对func进一步说明
												const char* extra,		// 额外说明(option), 对func进一步说明
												const KVArgs * args
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_VISIT_FUNCTION, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_VISIT_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_VISIT_VALUE] = value;
	FillModuleInfo(properties, module, func, extra);
	FillCustomArgs(properties, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::OnCustomEvent(
												const char* roleid,		// 角色id(option)
												int level,				// 角色等级(require)
												int vip,				// vip等级(require)
												const char* evtName,	// 事件名(require)
												int module,				// 功能模块(require), 游戏中定义(在什么模块)
												int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
												const KVArgs* args		// 参数列表(属性名键值对)
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, SENSORS_EVENT_TYPE_TRACK, evtName, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	if (vip > 0)
	{
		properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	}
	FillModuleInfo(properties, module, func, NULL, args);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollectorEx::SetProfile(const char* roleid, const KVArgs& args)
{
	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, roleid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	
	FillCustomArgs(profileset, &args);
	WriteAnalyticsLog(profiledata);
}

