#include "SensorsAnalyticsCollector.h"
#include "utils/util_func.h"
#include "GameAnalyticsEventNames.h"
#include "GameAnalyticsProperties.h"
#include "utils/string_util.h"

SensorsAnalyticsCollector::SensorsAnalyticsCollector(IAnalytisLogger& logger, 
			int nGameId, int nSrvId, int nChannelId,
			const char* gameVer, 
			const char* gameName,
			const char* strChannelName )
	: m_anaLogger( logger )
	, m_nGameId(nGameId)
	, m_nSrvId(nSrvId)
	, m_nChannelId( nChannelId )
{
	if (NULL != gameVer)
	{
		m_strGameVer = gameVer;
	}

	if (NULL != gameName)
	{
		m_strGameName = gameName;
	}

	if (NULL != strChannelName)
	{
		m_strChannelName = strChannelName;
	}
}

void SensorsAnalyticsCollector::FillSensorsTrackHeader(Json::Value& root,
				const char* distinctid,
				const char* type,
				const char* evt,
				const char* roleid,
				int level,
				const char* original_id/* = NULL*/) const
{
	Assert(NULL != distinctid);
	Assert(NULL != type);
	Assert(NULL != evt);

	root[SENSORS_INNER_DISTINCT_ID] = distinctid;
	root[SENSORS_INNER_TYPE] = type;
	root[SENSORS_INNER_EVENT] = evt;
	root[SENSORS_INNER_TIME] = util_get_utc_time();

	if (!m_strGameName.empty())
	{
		root[SENSORS_INNER_PROJECT] = m_strGameName;
	}
	if ( NULL != original_id )
	{
		root[SENSORS_INNER_ORIGINAL_ID] = original_id;
	}

	// 填充属性
	Json::Value& properties = root[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_GAME_ID] = m_nGameId;
	properties[GAME_ANALYTICS_PROPERTIES_SERVER_ID] = m_nSrvId;
	properties[GAME_ANALYTICS_PROPERTIES_SERVER_ID] = m_nChannelId;
	properties[GAME_ANALYTICS_PROPERTIES_GAME_VER] = m_strGameVer;
	properties[GAME_ANALYTICS_PROPERTIES_DISTRICT_NAME] = m_strChannelName;
	// 日期
	std::string date;
	util_convert_date_to_string2(date);
	properties[GAME_ANALYTICS_PROPERTIES_DATE] = date;
	// 角色id
	if (NULL != roleid)
	{
		properties[GAME_ANALYTICS_PROPERTIES_UID] = distinctid;
		properties[GAME_ANALYTICS_PROPERTIES_ROLE_ID] = roleid;
		properties[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	}
}

void SensorsAnalyticsCollector::FillModuleInfo(Json::Value& root, int module, int func, const char* extra, const KVArgs* args/* = NULL*/) const
{
	//const char* pModuleName = GetModuleName(module);
	//const char* pFuncName = GetFuncName(func);

	//InsertString(root, GAME_ANALYTICS_PROPERTIES_GAME_MODULE, pModuleName);
	//InsertString(root, GAME_ANALYTICS_PROPERTIES_GAME_ACTION, pFuncName);
	root[GAME_ANALYTICS_PROPERTIES_GAME_MODULE] = module;
	root[GAME_ANALYTICS_PROPERTIES_GAME_ACTION] = func;
	InsertString(root, GAME_ANALYTICS_PROPERTIES_EXTRA_PARAM, extra);

	if (NULL != args)
	{
		for (auto itr = args->begin(); itr != args->end(); ++itr)
		{
			root[itr.memberName()] = *itr;
			//switch (itr->second.GetType())
			//{
			//case VTYPE_BOOL:
			//	root[itr->first.c_str()] = itr->second.BoolVal();
			//	break;
			//case VTYPE_INT:
			//	root[itr->first.c_str()] = itr->second.IntVal();
			//	break;
			//case VTYPE_INT64:
			//	root[itr->first.c_str()] = itr->second.Int64Val();
			//	break;
			//case VTYPE_FLOAT:
			//	root[itr->first.c_str()] = itr->second.FloatVal();
			//	break;
			//case VTYPE_DOUBLE:
			//	root[itr->first.c_str()] = itr->second.DoubleVal();
			//	break;
			//case VTYPE_STRING:
			//	InsertString(root, itr->first.c_str(), itr->second.StringVal());
			//	break;
			//case VTYPE_WIDESTR:
			//	InsertString(root, itr->first.c_str(), StringUtil::WideStrAsUTF8String(itr->second.WideStrVal()).c_str());
			//	break;
			//default:
			//	break;
			//}
		}
	}
}

void SensorsAnalyticsCollector::FillSensorsProfileHeader(Json::Value& root, const char* distinctid, const char* type)
{
	Assert(NULL != distinctid);
	Assert(NULL != type);

	root[SENSORS_INNER_DISTINCT_ID] = distinctid;
	root[SENSORS_INNER_TYPE] = type;
	root[SENSORS_INNER_TIME] = util_get_utc_time();

	if (!m_strGameName.empty())
	{
		root[SENSORS_INNER_PROJECT] = m_strGameName;
	}
}

//const char* SensorsAnalyticsCollector::GetModuleName(int nModule) const
//{
//	static char szName[32] = { 0 };
//	sprintf_s(szName, "%d", nModule);
//	return szName;
//}
//const char* SensorsAnalyticsCollector::GetFuncName(int nFuncName) const
//{
//	static char szName[32] = { 0 };
//	sprintf_s(szName, "%d", nFuncName);
//	return szName;
//}

void SensorsAnalyticsCollector::OnServerStart()
{
	// 写入一个空行标志游戏开始
	ANALYTICS_LOG(m_anaLogger, "");

	Json::Value sensordata;
	std::stringstream ss;
	ss << m_nGameId;
	FillSensorsTrackHeader(sensordata, ss.str().c_str(), SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GAME_START);
	WriteAnalyticsLog(sensordata);
}

void SensorsAnalyticsCollector::OnServerClose()
{
	Json::Value sensordata;

	std::stringstream ss;
	ss << m_nGameId;

	FillSensorsTrackHeader(sensordata, ss.str().c_str(), SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GAME_END);
	WriteAnalyticsLog(sensordata);
}

void SensorsAnalyticsCollector::OnClientInfo(const char* uid, /* 用户唯一id (require) */ 
									const char* roleid, /* 角色id(require) */ 
									bool bfirst,				// 是否是首次
									const char* clientver,
									const char* ip, /* 终端id(option) */
									const char* platform, /* 注册平台(option) */ 
									bool wifi, /* 是否使用wifi(option) */ 
									const char* manufacturer, /* 制造商(option) */
									const char* devid, /* 终端设备唯一标识(option) */ 
									const char* model, /* 终端类型(option) */ 
									const char* os, /* 终端系统(option) */
									const char* os_ver, /* 终端系统版本(option) */
									const char* channel	)// 渠道(option)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SET_CLIENT, roleid, 0);

	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_DEVICE_ID, devid);
	InsertString(properties, SENSORS_PROPERTY_IP, ip);
	properties[SENSORS_PROPERTY_WIFI] = wifi;
	InsertString(properties, SENSORS_PROPERTY_MANUFACTURER, manufacturer);
	InsertString(properties, SENSORS_PROPERTY_MODEL, model);
	InsertString(properties, SENSORS_PROPERTY_OS, os);
	InsertString(properties, SENSORS_PROPERTY_OS_VER, os_ver);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_CLIENT_VER, clientver);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_CHANNEL, channel);
	WriteAnalyticsLog(trackdata);

	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_PLATFORM, platform);
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_DEVICE_ID, devid);
	InsertString(profileset, SENSORS_PROPERTY_IP, ip);
	InsertString(profileset, SENSORS_PROPERTY_MANUFACTURER, manufacturer);
	InsertString(profileset, SENSORS_PROPERTY_MODEL, model);
	InsertString(profileset, SENSORS_PROPERTY_OS, os);
	InsertString(profileset, SENSORS_PROPERTY_OS_VER, os_ver);
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_CLIENT_VER, clientver);
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_CHANNEL, channel);
	profileset[SENSORS_PROPERTY_WIFI] = wifi;

	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnNewUser(const char* uid,
											const char* anid,
											const char* nick, 
											int sex, 
											__int64 bron )
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_NEW_USER, NULL, 0, anid);

	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_NICK, nick);
	properties[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
	properties[GAME_ANALYTICS_PROPERTIES_BORN] = bron;
	if ( !m_strGameVer.empty() )
		properties[SENSORS_PROPERTY_APP_VER] = m_strGameVer;

	WriteAnalyticsLog( trackdata );

	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET_ONCE);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	profileset[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
	InsertString(profileset, GAME_ANALYTICS_PROPERTIES_NICK, nick);
	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnDelUser(const char* uid)
{
	Json::Value trackdata;
	FillSensorsTrackHeader( trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_DEL_USER );

	WriteAnalyticsLog( trackdata );

	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_DELETE);
	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnUserLogin( const char* uid )
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SIGNIN);

	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnUserLogout(const char* uid, int64_t onlinetime)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_SIGNOUT);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ONLINETIME] = onlinetime;
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnNewRole(const char* roleid, 
								const char* uid, 
								const char* roleName, 
								int job,
								int sex, 
								int flag,
								const KVArgs* args
								)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_NEW_ROLE, roleid, 1);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_SEX] = sex;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_FLAG] = flag;

	WriteAnalyticsLog( trackdata );

	// profile(sex, name, level, gold, vip... )
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SEX, sex);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_ROLE_ID, roleid);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
	FillSensorsProfileArgs(profileset, job, args);

	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnDelRole(const char* roleid, int job, int level, const char* uid)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_DEL_ROLE, roleid, level);
	WriteAnalyticsLog( trackdata );

	// profile(sex, name, level, gold, vip... )
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_UNSET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	bool bDel = true;
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SEX, bDel);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, bDel);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_LEVEL, bDel);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_VIP, bDel);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SILVER, bDel);
	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnRoleEntry(const char* roleid,			// 角色id
											const char* roleName,		// 角色名
											int job,
											int level,					// 角色等级
											int vip,					// 角色vip
											int64_t gold,				// 当前所拥有的金币
											const char* uid				// 角色所属账号
										)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_ROLE_ENTRY, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
	properties[GAME_ANALYTICS_PROPERTIES_LEVEL] = level;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	WriteAnalyticsLog(trackdata);

		// profile(sex, name, level, gold, vip... )
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SILVER, gold);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_VIP, vip);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_LEVEL, level);

	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnRoleExit(const char* roleid, 		// 角色id
											const char* roleName,		// 角色名
											int job,
											int level,					// 角色等级
											int vip,					// 角色vip
											int64_t gold,				// 当前所拥有的金币
											int64_t onlinetime,			// 在线时长
											const char* uid				// 角色所属账号
											)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_ROLE_EXIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ROLE_NAME, roleName);
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SILVER] = gold;
	properties[GAME_ANALYTICS_PROPERTIES_ONLINETIME] = onlinetime;
	WriteAnalyticsLog( trackdata );

	// profile(sex, name, level, gold, vip... )
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SILVER, gold);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_VIP, vip);
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_LEVEL, level);

	WriteAnalyticsLog(profiledata);
}


void SensorsAnalyticsCollector::OnPaying(const char* uid,			// 用户id
									const char* roleid,			// 角色id(require)
									int job,
									int level,					// 角色等级
									int vip,					// 角色vip等级
									const char* orderid, 
									const char* itemid, 
									int count, 
									float price,				// 产品价格(require)
									bool first,					// 是否首充
									const char* os				// 客户端os(android, ios, pc...)
									)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PAY_START, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ORDER_ID, orderid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ITEM_ID, itemid);
	properties[GAME_ANALYTICS_PROPERTIES_PAY_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_ITEM_PRICE] = price;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_FIRST] = first;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_OS, os);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnPayCompleted(const char* uid,				// 用户id
												const char* roleid,			// 角色id(require)
												int job,					// 角色职业
												int level,					// 角色等级
												int vip,					// 角色vip等级
												const char* orderid,			// 订单id(require)
												float amount,					// 实际支付金额(require)
												int result,						// 支付结果(require) 0:成功 1:失败 2:取消
												bool first,						// 是否首充
												const char* platform,			// 支付平台(require)(xy, xiaomi,baidu....)
												const char* os,					// 客户端os
												const char* reason				// 非成功原因(option)
											)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PAY_END, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_ORDER_ID, orderid);
	properties[GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT] = amount;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_PAY_FIRST] = first;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_PLATFORM, platform);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PAY_REASON, reason);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_OS, os);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnIncExp(const char* uid,			// 用户id
										const char* roleid,		// 角色id(require)
										int job,
										int level,				// 角色等级
										int value,				// 增加值(require)
										int module,				// 功能模块(option), 游戏中定义(在什么模块)
										int func,				// 功能事件(option), 游戏中定义(做什么事获得)
										const char* extra		// 额外说明(option), 对event进一步说明
										)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_INC_EXP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_INC_EXP] = value;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnLevelup(const char* uid, const char* roleid, int job, int level, int original)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_LEVELUP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_PRE_LEVEL] = original;
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	WriteAnalyticsLog( trackdata );

	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_LEVEL, level);
	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnGainCoin(const char* uid,
												const char* roleid,			// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int coid,
												int64_t value,
												int64_t newvalue,
												int64_t original,
												int module, 
												int func, 
												const char* extra)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_INC_COIN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ID] = coid;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_NEW_VALUE] = newvalue;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ORIGINAL] = original;

	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );

	// profile
	if (coid == CAPITAL_SILVER)
	{
		Json::Value profiledata;
		FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
		Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
		FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SILVER, newvalue);
		WriteAnalyticsLog(profiledata);
	}
}

void SensorsAnalyticsCollector::OnCostCoin(const char* uid,
											const char* roleid,			// 角色id(require)
											int job,
											int level,				// 角色当前等级(require)
											int vip,				// vip等级(require)
											int coid, 
											int64_t value,
											int64_t newvalue,
											int64_t original,
											int module, 
											int func, 
											const char* extra)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_DEC_COIN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ID] = coid;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_NEW_VALUE] = newvalue;
	properties[GAME_ANALYTICS_PROPERTIES_COIN_ORIGINAL] = original;

	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );

	// profile
	if (coid == CAPITAL_SILVER)
	{
		Json::Value profiledata;
		FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
		Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
		FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_SILVER, newvalue);
		WriteAnalyticsLog(profiledata);
	}
}

void SensorsAnalyticsCollector::OnGainItem(const char* uid,
												const char* roleid,			// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												const char* itemid, 
												const char* itemName, 
												int num,				// 获得数值(require)
												int count,				// 当前拥有数
												int orig,				// 原来拥有数
												int module, 
												int func, 
												const char* extra)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GAIN_ITEM, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_ID, itemid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_NAME, itemName);
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_NUM] = num;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_ORIG_COUNT] = orig;

	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnCostItem(const char* uid,
												const char* roleid,			// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												const char* itemid, 
												const char* itemName, 
												int num,				// 获得数值(require)
												int count,				// 当前拥有数
												int orig,				// 原来拥有数
												int module,
												int func, 
												const char* extra)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COST_ITEM, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_ID, itemid);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_ITEM_NAME, itemName);
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_COUNT] = count;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_NUM] = num;
	properties[GAME_ANALYTICS_PROPERTIES_ITEM_ORIG_COUNT] = orig;

	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnVipLevelup(const char* uid,
												const char* roleid,			// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int value, 
												int original, 
												int module, 
												int func, 
												const char* extra)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_VIPUP, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = value;
	properties[GAME_ANALYTICS_PROPERTIES_PRE_VIP] = original;

	FillModuleInfo(properties, module, func, extra);
	WriteAnalyticsLog( trackdata );

	// profile
	Json::Value profiledata;
	FillSensorsProfileHeader(profiledata, uid, SENSORS_EVENT_TYPE_PROFILE_SET);
	Json::Value& profileset = profiledata[SENSORS_INNER_PROPERTYIES];
	FillSensorsProfileProperty(profileset, job, GAME_ANALYTICS_PROPERTIES_VIP, value);
	WriteAnalyticsLog(profiledata);
}

void SensorsAnalyticsCollector::OnPlayerPropertyChanged(const char* uid,
											const char* roleid,			// 角色id(require)
											int job,
											int level,				// 角色当前等级(require)
											int vip,				// vip等级(require)
											const char* propery, 
											float value, 
											float original, 
											int module, 
											int func, 
											const char* extra )
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_PROPERTY_CHANGED, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY, propery);
	properties[GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_VALUE] = value;
	properties[GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_PRE_VALUE] = original;

	FillModuleInfo(properties, module, func, extra);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnPlayerTransaction(const char* transid, 
									int transtype, 
									const char* sender, 
									const char* reciver, 
									const char* senderItem, 
									int senderCount, 
									const char* reciverItem, 
									int reciverCount)
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

	WriteAnalyticsLog( trackdata );

}

void SensorsAnalyticsCollector::OnGuideEvent(const char* uid,
										const char* roleid,			// 角色id(require)
										int job,
										int level,				// 角色当前等级(require)
										int vip,				// vip等级(require)
										int guideid, int step)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_GUIDE, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_GUIDE_ID] = guideid;
	properties[GAME_ANALYTICS_PROPERTIES_GUIDE_STEP] = step;

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnTaskStart(const char* uid,
												const char* roleid,		// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int taskid,				// 接取的任务id(require)
												const char* taskName,	// 任务名
												int type				// 任务类型(option) 游戏中定义
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TASK_ACCEPT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_ID] = taskid;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_TASK_NAME, taskName);
	properties[GAME_ANALYTICS_PROPERTIES_TASK_TYPE] = type;

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnTaskCompleted(const char* uid,
												const char* roleid,		// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int taskid,				// 接取的任务id(require)
												const char* taskName,	// 任务名
												int type				// 任务类型(option) 游戏中定义
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_TASK_SUBMIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_TASK_ID] = taskid;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_TASK_NAME, taskName);
	properties[GAME_ANALYTICS_PROPERTIES_TASK_TYPE] = type;

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnSceneOpen(const char* uid,
												const char* roleid,		// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int sid, 
												const char* sname,		// 副本名
												int chapter, 
												int type )
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_OPEN, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SCENE_NAME, sname);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnSceneEntry(const char* uid,
													const char* roleid,		// 角色id(require)
													int job,
													int level,				// 角色当前等级(require)
													int vip,				// vip等级(require)
													int sid,				// 副本id(require)
													const char* sname,			// 副本名
													int chapter, 
													int type, 
													int result, 
													const char* reason)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_ENTRY, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_RESULT] = result;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SCENE_NAME, sname);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SCENE_REASON, reason);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnSceneExit(const char* uid,
												const char* roleid,		// 角色id(require)
												int job,
												int level,				// 角色当前等级(require)
												int vip,				// vip等级(require)
												int sid,				// 副本id(require)
												const char* sname,		// 副本名
												int chapter,			// 副本章节(option)
												int type,				// 副本类型(option) 游戏中定义
												time_t staytime,		// 驻留时间
												int result,				// 战斗结果(require)
												const char* reason		// 结果描述(option)
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_COPY_SCENE_EXIT, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_ID] = sid;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER] = chapter;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_TYPE] = type;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_SCENE_STAY_TIME] = staytime;
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SCENE_NAME, sname);
	InsertString(properties, GAME_ANALYTICS_PROPERTIES_SCENE_REASON, reason);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnVisitFunction(const char* uid,		// 用户id(require)
												const char* roleid,	// 角色id(require)
												int job,
												int level,				// 角色等级(require)
												int vip,				// vip等级(require)
												int module,				// 功能模块(require), 游戏中定义(在什么模块)
												int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
												bool result,			// 结果(require) true:成功 false: 失败
												int value,				// 额外说明(option), 对func进一步说明
												const char* extra		// 额外说明(option), 对func进一步说明
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, GAME_ANALYTICS_EVENT_VISIT_FUNCTION, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;
	properties[GAME_ANALYTICS_PROPERTIES_VISIT_RESULT] = result;
	properties[GAME_ANALYTICS_PROPERTIES_VISIT_VALUE] = value;

	FillModuleInfo(properties, module, func, extra);

	WriteAnalyticsLog( trackdata );
}

void SensorsAnalyticsCollector::OnCustomEvent(	const char* uid,		// 用户id(require)
												const char* roleid,		// 角色id(option)
												int job,				
												int level,				// 角色等级(require)
												int vip,				// vip等级(require)
												const char* evtName,		// 事件名(require)
												int module,				// 功能模块(require), 游戏中定义(在什么模块)
												int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
												const KVArgs* args		// 参数列表(属性名键值对)
												)
{
	Json::Value trackdata;
	FillSensorsTrackHeader(trackdata, uid, SENSORS_EVENT_TYPE_TRACK, evtName, roleid, level);
	// 填充属性
	Json::Value& properties = trackdata[SENSORS_INNER_PROPERTYIES];
	properties[GAME_ANALYTICS_PROPERTIES_ROLE_JOB] = job;
	properties[GAME_ANALYTICS_PROPERTIES_VIP] = vip;

	FillModuleInfo(properties, module, func, NULL, args);

	WriteAnalyticsLog( trackdata );

}