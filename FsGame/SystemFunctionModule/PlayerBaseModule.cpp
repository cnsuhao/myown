//--------------------------------------------------------------------
// 文件名:		PlayerBaseModule.cpp
// 内  容:		玩家基础数据
// 说  明:		
// 创建日期:	2014年10月17日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------
#include "PlayerBaseModule.h"

#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/SnsDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/SnsDataModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/SystemFunctionModule/ToolBoxModule.h"
//#include "FsGame/SceneBaseModule/SceneInfoModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "utils/util_func.h"
#include <time.h>
#include "FsGame/Define/PlayerBaseDefine.h"
#include "utils/string_util.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "utils/custom_func.h"
#include "FsGame/CommonModule/EnvirValueModule.h"
#include "extension/FsGmCC/protocols/ProtocolsID.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/SystemFunctionModule/ResetTimerModule.h"
//#include "FsGame/Define/ExtraServerDefine.h"
#endif
#include <algorithm>
#include "CreateRoleModule.h"
//#include "ItemModule/FashionModule.h"
#include "ItemModule/ToolItem/ToolItemModule.h"
#include "Define/ExtraServerDefine.h"

// 踢下线间隔时间
const int FORCE_OFFLINE_TIME = 500;

// 设置玩家位置
inline int nx_reset_player_position(void *state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == PlayerBaseModule::m_pPlayerBaseModule)
	{
		return 0;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_reset_player_position, 2);

	// 检查参数类型
	CHECK_ARG_INT(state, nx_reset_player_position, 1);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_reset_player_position, 2);
	

	// 获取参数
	int sceneID = pKernel->LuaToInt(state, 1);
	const wchar_t* strName = pKernel->LuaToWideStr(state, 2);
	
//	bool ret = PlayerBaseModule::m_pPlayerBaseModule->GMResetPlayerPosition(pKernel, strName, sceneID);
//	pKernel->LuaPushBool(state, ret);
	return 0;
}

#ifndef FSROOMLOGIC_EXPORTS
// 设置玩家签名
inline int nx_exchange_gift(void *state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == PlayerBaseModule::m_pPlayerBaseModule)
	{
		return 0;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_exchange_gift, 2);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_exchange_gift, 1);
	CHECK_ARG_STRING(state, nx_exchange_gift, 2);

	// 获取参数
	PERSISTID player = pKernel->LuaToObject(state, 1);
	const char* strKey = pKernel->LuaToString(state, 2);
	if (StringUtil::CharIsNull(strKey))
	{
		return 0;
	}

	// 发送请求兑换礼包请求
	CVarList msg;
	msg << PROTOCOL_ID_GIFT_EXCH << strKey << pKernel->QueryString(player, FIELD_PROP_ACCOUNT) << pKernel->QueryString(player, FIELD_PROP_UID);

	pKernel->SendExtraMessage(ESI_GMCC_SERVER, msg);

	//pKernel->LuaPushBool(state, true);

	return 1;
}
#endif

//判断函数地址表
std::vector<PLAYER_VERSION_FUNC> PlayerBaseModule::m_funList;

PlayerBaseModule * PlayerBaseModule::m_pPlayerBaseModule =NULL;

int OnExtraMessage( IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& msg )
{
#ifdef _DEBUG
	size_t nCount = msg.GetCount();
	int nIndex = msg.IntVal(0);
	printf("OnExtraMessage:%d\n", nIndex );
	for( size_t i = 1; i < nCount ; ++i)
	{
		if ( msg.GetType(i) == VTYPE_INT )
		{
			printf("第%d个参数:%d(int)\n", (int)i, msg.IntVal(i) );
		}
		else if ( msg.GetType(i) == VTYPE_STRING )
		{
			printf("第%d个参数:%s(string)\n", (int)i, msg.StringVal(i) );
		}
		else
		{
			printf("第%d个参数:(other)\n", (int)i );
		}
	}
#endif
	return 0;
}

// 初始化
bool PlayerBaseModule::Init(IKernel* pKernel)
{
	//初始化随机种子
	srand((int)util_get_ticks());
	m_pPlayerBaseModule = this;

	Assert( m_pPlayerBaseModule != NULL );

	// 版本号升级执行函数
	m_funList.resize(PLAYER_VERSION_MAX, NULL);

// 	RegisterVersionFun(PLAYER_PACK_UPDATE, UpdatePack);     // 更新玩家背包
//     RegisterVersionFun(PLAYER_PACK_ITEM_ASSIGN, AssignPackItem);



	pKernel->AddClassCallback("player", "OnCreateClass", OnCreateClass);
	pKernel->AddEventCallback("player", "OnEntry", PlayerBaseModule::OnEntry, -1);
	pKernel->AddEventCallback("player", "OnEntryGame", PlayerBaseModule::OnEntryGame, -1);
	pKernel->AddEventCallback("player", "OnContinue", PlayerBaseModule::OnContinue, -1);
	pKernel->AddEventCallback("player", "OnRecover", PlayerBaseModule::OnRecover, 1000);
	pKernel->AddEventCallback("player", "OnStore", PlayerBaseModule::OnStore);
	pKernel->AddEventCallback("player", "OnDisconnect", PlayerBaseModule::OnDisconnect, 1000);
	pKernel->AddEventCallback("player", "OnSelect", PlayerBaseModule::OnPlayerSelect); 
	pKernel->AddEventCallback("npc", "OnSelect", PlayerBaseModule::OnPlayerSelect);

	pKernel->AddEventCallback("player", "OnStore", PlayerBaseModule::OnStore2, 50000);
	pKernel->AddEventCallback("player", "OnLeave", PlayerBaseModule::OnLeave, 50000);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_GET_SERVERTIME, PlayerBaseModule::OnCustomGetServerTime);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CHECKED_SELECTROLE, PlayerBaseModule::OnCheckedSelectRole);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_TOKEN, PlayerBaseModule::GetToken);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_SET_VISUAL_PLAYERS, PlayerBaseModule::OnCustomSetVisualPlayers);
    //客户端转到后台
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_BACKGROUND, PlayerBaseModule::OnClientBackgroundTask);

	//pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_RECORD_PLAY_CG_SCENE, PlayerBaseModule::OnCustomRecordPlayCGScene);
	// 获取玩家属性数据
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_GET_PLAYER_PROP_DATA, PlayerBaseModule::OnCustomGetPlayerPropData);

    // 玩家被伤害
	pKernel->AddIntCommandHook("player", COMMAND_BEDAMAGE, PlayerBaseModule::OnCommandBeDamage);

	// 冻结玩家
//	pKernel->AddIntCommandHook("player", COMMAND_BLOCK_PLAYER, PlayerBaseModule::OnCommandBlockPlayer);
	// 获取玩家属性数据
	pKernel->AddIntCommandHook("player", COMMAND_CLIENT_QUERY_PROP_DATA, PlayerBaseModule::OnCommandClientQueryPropData);

	bool bOK = pKernel->AddEventCallback("scene", "OnExtraMessage", OnExtraMessage );

	DECL_HEARTBEAT(PlayerBaseModule::CB_ForceOffline);

#ifndef FSROOMLOGIC_EXPORTS
	DECL_RESET_TIMER(RESET_ROLE_LOGIN_TIME, PlayerBaseModule::ResetLoginTimeTimer);

	DECL_LUA_EXT(nx_exchange_gift);
#endif

    // GM命令重置玩家位置到皇城
	DECL_LUA_EXT(nx_reset_player_position);

	// TODO: 可查询属性字段暂时直接写入
// 	m_propPubset.insert( FIELD_PROP_BATTLE_ABILITY );
// 	m_propPubset.insert(FIELD_PROP_TEAM_ID);
// 	m_propPubset.insert(FIELD_PROP_TEAM_CAPTAIN);
// 	m_propPubset.insert(FIELD_PROP_GUILD_NAME);
// 	m_propPubset.insert(FIELD_PROP_GUILD_POSITION);
//	m_propPubset.insert(FIELD_PROP_NATION);
	m_propPubset.insert(FIELD_PROP_NAME);
	return true;
}

// 释放
bool PlayerBaseModule::Shut(IKernel* pKernel)
{
	return true;
}

//属性
int PlayerBaseModule::OnCreateClass(IKernel* pKernel, int index)
{
#ifndef FSROOMLOGIC_EXPORTS
	createclass_from_xml(pKernel, index, "struct\\player\\player.xml");
#else
	createclass_from_xml(pKernel, index, "room_struct\\player\\player.xml");
#endif // FSROOMLOGIC_EXPORTS
	return 0;
}

int PlayerBaseModule::OnEntry(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	PERSISTID scene = pKernel->GetScene();
	if (!pKernel->Exists(scene))
	{
		return 0;
	}

	// 场景对象
	IGameObj *pScene = pKernel->GetGameObj(scene);
	if (NULL == pScene)
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	//设置可视范围
	float fVisualRange = 50.0f;

	if (pScene->FindAttr("VisualRange"))
	{
		float visual_range = pScene->QueryFloat("VisualRange");

		fVisualRange = (visual_range < 50.0f) ? visual_range : 50.0f;
	}

	if (pPlayer->FindAttr("VisualRange"))
	{
		pPlayer->SetFloat("VisualRange", fVisualRange);	
	}

// 	CVarList extraMsg;
// 	extraMsg << "extra" << "player entry" << 10 << 10.5;
// 	pKernel->SendExtraMessage( 8, extraMsg );

	return 0;
}

//玩家上线
int PlayerBaseModule::OnRecover(IKernel* pKernel, const PERSISTID& self, 
								const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	//重置HP时间
	int64_t resetTime = static_cast<int64_t>(EnvirValueModule::EnvirQueryInt(ENV_VALUE_RESET_MAX_HP_TIME));
	int64_t logoutTime = pSelf->QueryInt64(FIELD_PROP_ROLE_LOGOUT_TIME);
    
	//血量数据
	int64_t maxHP = pSelf->QueryInt64("MaxHP");
	int64_t iLastHP = pSelf->QueryInt64("TempHP");

	//时间判断是否重置HP
	if( ::time(NULL) - logoutTime >= resetTime )
	{	
		pSelf->SetInt64("HP", maxHP);
	}
	else
	{
		pSelf->SetInt64("HP", iLastHP > maxHP ? maxHP : iLastHP);
	}

	float fWalkStep = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_PLAYER_WALK_STEP_DEFAULT);
	pSelf->SetFloat("WalkStep", fWalkStep);
	pSelf->SetFloat("CollideRadius", 1.0f);// 增加设置玩家碰撞半径
	pSelf->SetInt("MapFree", 1);// 飞行时不受场景限制

	// 判断是不是同一天
	int64_t nLastLoginTime_ = pSelf->QueryInt64(FIELD_PROP_ROLE_LOGIN_TIME);
	// 判断当前是否已经记录过了
	int64_t tNow_ = util_get_utc_time();
	int nLoginDays_ = pSelf->QueryInt(FIELD_PROP_LOGIN_DAYS);
	// 上线登记过了
	if (util_is_sameday(nLastLoginTime_, tNow_))
	{
		if (nLoginDays_ <= 0)
		{
			pSelf->SetInt(FIELD_PROP_LOGIN_DAYS, 1);
		}
	}
	else
	{
		pSelf->SetInt(FIELD_PROP_LOGIN_DAYS, nLoginDays_ + 1);
	}

	pSelf->SetInt64("RoleLoginTime", tNow_);
	//预设置玩家同屏显示人数，客户端自定义消息再次设置
	pSelf->SetInt("VisualPlayers", 5);
	pSelf->SetInt("MaxVisuals", 5);

	// 设置服务器ID
#ifdef FSROOMLOGIC_EXPORTS

	int serverId = pKernel->GetRoomId();

#else

	int serverId = pKernel->GetServerId();

#endif
	

	pSelf->SetInt("ServerID", serverId);
	
#ifndef FSROOMLOGIC_EXPORTS
	// 注册玩家0点更新登录时间
	REGIST_RESET_TIMER(pKernel, self, RESET_ROLE_LOGIN_TIME);

	// 玩家上线
	LogModule::m_pLogModule->OnRoleEntry(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
	return 0;
}

//玩家断线重连
int PlayerBaseModule::OnContinue(IKernel* pKernel, const PERSISTID& self, 
								const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 更新渠道uid
	PlayerBaseModule::m_pPlayerBaseModule->UpdateAccountUID(pKernel, self);
	return 0;
}

//玩家断线重连，返回的是缓存时间10秒(这个回调只能有一个)
int PlayerBaseModule::OnDisconnect(IKernel* pKernel, const PERSISTID& self, 
								   const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(self);
	if (!pPlayerObj)
	{
		return 0;
	}

#ifndef FSROOMLOGIC_EXPORTS
   if (pPlayerObj->FindData("CB_DisnectProtect"))
   {
	   int nProTime = pPlayerObj->QueryDataInt("CB_DisnectProtect");
	   return nProTime;
   }
#endif

	if (pPlayerObj->FindData("OnNormalExitGame") &&
		pPlayerObj->QueryDataInt("OnNormalExitGame") == 1)
	{
		return 0;
	}

	//普通秘境断线保护300s
	int nScene = pKernel->GetSceneId();
	const char* name = pKernel->GetSceneScript(nScene);
	if (strcmp(name, "SecretScene") == 0)
	{
		return 300;
	}

	//断线重连60s保护
	return 60;
}

//玩家下线
int PlayerBaseModule::OnStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	// 记录玩家登出时间
	pPlayer->SetInt64(FIELD_PROP_ROLE_LOGOUT_TIME, ::time(NULL));
	
	const int type = args.IntVal(0);
	if (STORE_EXIT == type)
	{
		//use_role_rec	
		if (pKernel->Type(self) == TYPE_PLAYER)
		{
			time_t start = pPlayer->QueryInt64("RoleLoginTime");
			if (start <= 0)
			{
				return 0;
			}
			time_t now = ::time(NULL);
			if (now <= 0)
			{
				return 0;
			}
			const char* address = pPlayer->QueryString("Token");
			const char* ClientType = pPlayer->QueryString("ClientType");

#ifndef FSROOMLOGIC_EXPORTS
			//保存角色登录日志modify by SunJian(2015-04-23)
			RoleLog log;
			log.loginTime        = start;
			log.logoutTime       = now;
			log.ip               = pPlayer->QueryString("Address");
			log.mac              = address;
			log.osType           = ClientType;
			LogModule::m_pLogModule->SaveRoleLog(pKernel, self, log);

			// 玩家下线
			LogModule::m_pLogModule->OnRoleExit(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
		}

	}

	return 0;
}

int PlayerBaseModule::OnStore2(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	const int type = args.IntVal(0);
	if (STORE_EXIT != type)
	{
		return 0;
	}

	IGameObj* pPlayerObj = pKernel->GetGameObj(self);
	if (!pPlayerObj)
	{
		return 0;
	}

	if (pPlayerObj->FindData("TempStoreTime"))
	{
		CVarList msg;
		msg << (int)SERVER_CUSTOMMSG_SWITCH_ROLE << (int)EMSG_SWITCH_STATUS_1;
		pKernel->Custom(self, msg);

		int64_t t = util_get_time_64() - pPlayerObj->QueryDataInt64("TempStoreTime");
		if (t < 1000)
		{
			return 0;
		}
		char strtime[256] = { 0 };
		time_t ttime = time(NULL);
		util_convert_time_to_string(&ttime, strtime, sizeof(strtime));

		::extend_warning(LOG_ERROR, "[ERROR][%s]: LeaveGame to OnStore End:[%d]", strtime, t);
	}

	return 0;
}

int PlayerBaseModule::OnLeave(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(self);
	if (!pPlayerObj)
	{
		return 0;
	}

	if (pPlayerObj->FindData("TempStoreTime"))
	{
		CVarList msg;
		msg << (int)SERVER_CUSTOMMSG_SWITCH_ROLE << (int)EMSG_SWITCH_STATUS_2;
		pKernel->Custom(self, msg);

		int64_t t = util_get_time_64() - pPlayerObj->QueryDataInt64("TempStoreTime");
		if (t < 3000)
		{
			return 0;
		}
		char strtime[256] = { 0 };
		time_t ttime = time(NULL);
		util_convert_time_to_string(&ttime, strtime, sizeof(strtime));

		::extend_warning(LOG_ERROR, "[ERROR][%s]: LeaveGame to OnLeave End:[%d]", strtime, t);
	}

	return 0;
}

// 玩家进入游戏
int PlayerBaseModule::OnEntryGame(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
	
	// 更新渠道uid
	PlayerBaseModule::m_pPlayerBaseModule->UpdateAccountUID(pKernel, self);

	return 0;
}

// 更新渠道uid
void PlayerBaseModule::UpdateAccountUID(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	if (pKernel->Type(self) != TYPE_PLAYER 
		|| pSelfObj->QueryInt("Online") != ONLINE)
	{
		return;
	}

	// 解析UID
	// $#648_47579443
	if (StringUtil::CharIsNull(pSelfObj->QueryString("AccountUID")))
	{
		std::string account = pSelfObj->QueryString("Account"); 
		if (!account.empty())
		{
			size_t pos = account.rfind("_");
			if (pos != std::string::npos)
			{
				// 取uid起始位置和长度
				size_t startPos = pos + strlen("_");
				int accountSize = static_cast<int>(account.size() - startPos);

				// 截取uid
				std::string uid = account.substr(startPos, accountSize);
				pSelfObj->SetString("AccountUID", uid.c_str());
			}
		}
	}
}
int PlayerBaseModule::OnPlayerSelect(IKernel* pKernel, const PERSISTID& self,
									 const PERSISTID& sender, const IVarList& args)
{
	// 玩家对象
	IGameObj *pObj = pKernel->GetGameObj(sender);
	if (NULL == pObj)
	{
		return 0;
	}

	int function_id = args.IntVal(0);

	if (function_id == 0)
	{
		return 0;
	}

	if(function_id > 10)
	{
		return 0;
	}
	else if(function_id == 1)
	{
		PERSISTID cur_co = pObj->QueryObject("LastObject");

		if(cur_co != self)
		{
			pObj->SetObject("LastObject",self);
		}
	}

	return 0;
}

//服务器时间点
int PlayerBaseModule::OnCustomGetServerTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	CVarList arg_list;
	arg_list << SERVER_CUSTOMMSG_SERVER_TIME << time(NULL) << ::util_get_time_64();
	pKernel->Custom( self, arg_list );
	return 0;
}

// （小退）返回角色选择
int PlayerBaseModule::OnCheckedSelectRole(IKernel* pKernel, const PERSISTID& self,
										  const PERSISTID& player, const IVarList& args)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

    //如果延迟下线（跨服战）
    if (pPlayer->FindData("CB_DisnectProtect"))
    {
        if(pPlayer->QueryDataInt("CB_DisnectProtect") > 0)
        {
            //::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, CROSSBATTLE_MATCHBATTLE_NO_EXIT_ROLE, CVarList());
            return 0;
        }
    }

	if (!pPlayer->FindData("OnNormalExitGame"))
	{
		pPlayer->AddDataInt("OnNormalExitGame", 1);
	}
	else
	{
		pPlayer->SetDataInt("OnNormalExitGame", 1);
	}
	ADD_DATA_INT64(pPlayer, "TempStoreTime");
	pPlayer->SetDataInt64("TempStoreTime", util_get_time_64());

	// 记录切换角色消息时间 [3/25/2017 lihailuo]
	::extend_warning( LOG_INFO, "player '%s(%s)' check back select role[timestamp:%lld]",
						pPlayer->QueryString(FIELD_PROP_UID), 
						StringUtil::WideStrAsString(pPlayer->QueryWideStr(FIELD_PROP_NAME)),
						util_get_time_64() );

	CVarList msg;
	msg << (int)SERVER_CUSTOMMSG_SWITCH_ROLE << (int)EMSG_SWITCH_ROLE_REPLY;
	if (pKernel->PlayerLeaveGame(self))
	{
		msg  << (int)EM_SWITCH_ROLE_SUCCEED;
	}
	else
	{
		msg << (int)EM_SWITCH_ROLE_FAILED;
	}
	pKernel->Custom(self, msg);
#endif
	return 0;
}

//获取设备号
int PlayerBaseModule::GetToken(IKernel* pKernel,const PERSISTID& self,const PERSISTID& sender,const IVarList& args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int nIndex = 1;
	const char* clientType = args.StringVal(nIndex++);
	const char* token = args.StringVal(nIndex++);
	const wchar_t* pServerName = args.WideStrVal(nIndex++);
	const char* AppPackage = args.StringVal(nIndex++);

	const char* platform = args.StringVal(nIndex++);
	bool bwifi = args.IntVal(nIndex++) != 0;
	const char* manufacturer = args.StringVal(nIndex++);
	const char* devid = args.StringVal(nIndex++);
	const char* model = args.StringVal(nIndex++);
	const char* os = args.StringVal(nIndex++);
	const char* os_ver = args.StringVal(nIndex++);
	const char* client_ver = args.StringVal(nIndex++);

    const char* account = pPlayer->QueryString("Account");
	if (StringUtil::CharIsNull(clientType)
		|| StringUtil::CharIsNull(token)
		|| StringUtil::CharIsNull(account)
		|| StringUtil::CharIsNull(pServerName)
		|| StringUtil::CharIsNull(AppPackage))
	{
		return 0;
	}

    pPlayer->SetString("ClientType", clientType);
    pPlayer->SetString("Token", token);
	pPlayer->SetWideStr("ServerName", pServerName);
	pPlayer->SetString("AppPackage", AppPackage);

#ifndef FSROOMLOGIC_EXPORTS
	const char* uid = pPlayer->QueryString(FIELD_PROP_UID);
	const char* ip = pPlayer->QueryString(FIELD_PROP_ADDRESS);

	if (!pPlayer->FindData("TempLoginPlatform"))
	{
		pPlayer->AddDataString("TempLoginPlatform", platform);
	}
	else
	{
		pPlayer->SetDataString("TempLoginPlatform", platform);
	}

	if (!pPlayer->FindData("TempLoginModel"))
	{
		pPlayer->AddDataString("TempLoginModel", model);
	}
	else
	{
		pPlayer->SetDataString("TempLoginModel", model);
	}

	if (!pPlayer->FindData("TempLoginManufacturer"))
	{
		pPlayer->AddDataString("TempLoginManufacturer", manufacturer);
	}
	else
	{
		pPlayer->SetDataString("TempLoginManufacturer", manufacturer);
	}

	if (!pPlayer->FindData("TempLoginDeviceId"))
	{
		pPlayer->AddDataString("TempLoginDeviceId", devid);
	}
	else
	{
		pPlayer->SetDataString("TempLoginDeviceId", devid);
	}
	LogModule::m_pLogModule->OnClientInfo(pKernel, self, ip, platform, client_ver, manufacturer, devid, model, os, os_ver, bwifi);

	CVarList msg_list;
	msg_list << "Token" << token 
		<< "ClientType" << clientType 
		<< "Account" << account 
		<< "ServerName" << pServerName
		<< "AppPackage" << AppPackage;
    SnsDataModule::SavePlayerAttr(pKernel, self, msg_list);

	//通知Pub上线
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_DOMAIN
// 		<< SnsPushModule::m_pSnsPushModule->GetDomainName(pKernel).c_str()
// 		<< SP_DOMAIN_MSG_MSG_PUSH_INFO
// 		<< account
// 		<< pKernel->SeekRoleUid(pPlayer->QueryWideStr("Name"))
// 		<< pPlayer->QueryInt("Nation")
// 		<< clientType
// 		<< token
// 		<< pServerName
// 		<< AppPackage;
// 	pKernel->SendPublicMessage(pubMsg);
#endif // FSROOMLOGIC_EXPORTS

	return 0;
}

//设置同屏显示人数
//msg格式：
//       [msgid][visual_player_num]
int PlayerBaseModule::OnCustomSetVisualPlayers( IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args )
{
#ifndef FSROOMLOGIC_EXPORTS
	if (args.GetCount() < 2 ||
		args.GetType(1) != VTYPE_INT)
	{
		//客户端上发的消息，参数错误
		return 0;
	}

	// 如果玩家不存在
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int visual_player_num = args.IntVal(1);

	//同屏显示上线50，下线10
	if(visual_player_num < 10)
	{
		visual_player_num = 10;
	}

	if(visual_player_num > 50)
	{
		visual_player_num = 50;
	}

	pPlayer->SetInt("VisualPlayers", visual_player_num);

	pPlayer->SetInt("MaxVisuals", visual_player_num + MAX_VISUALS_ADD);

	// 阵营战重置阵营可视视野
	//BattleHelperS::Instance()->ResetCampVisual(pKernel, self);

#endif // FSROOMLOGIC_EXPORTS

	return 0;
}

// 响应被伤害事件
int PlayerBaseModule::OnCommandBeDamage(IKernel* pKernel, const PERSISTID& self,
							 const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (NULL == pSelfObj || NULL == pSenderObj)
	{
		return 0;
	}

	// 必须为在线玩家
	if (TYPE_PLAYER == pSelfObj->GetClassType() && 1 == pSelfObj->QueryInt("Online"))
	{
		// 默认为攻击者
		PERSISTID prior = sender;
		
		// 技能npc,设置为施法者,宠物,设置为主人
		const char* strScript = pSenderObj->GetScript();
		if (0 == strcmp(strScript, "TrapNpc") || 0 == strcmp(strScript, "PetNpc"))
		{
			prior = pSenderObj->QueryObject("Master");
		}

		// 优先显示攻击者
		if (pKernel->Exists(prior))
		{
			pKernel->AddVisualPriority(self, sender, true);
		}
	}
	return 0;
}

// 0点更新登录时间
int PlayerBaseModule::ResetLoginTimeTimer(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

#ifndef FSROOMLOGIC_EXPORTS
	// 判断登陆时间是否同一天
	int64_t nLastLoginTime_ = pPlayer->QueryInt64(FIELD_PROP_ROLE_LOGIN_TIME);
	// 判断当前是否已经记录过了
	int64_t tNow_ = util_get_utc_time();
	// 上线登记过了
	if (!util_is_sameday(nLastLoginTime_, tNow_))
	{
		// add login days
		int nLoginDays_ = pPlayer->QueryInt(FIELD_PROP_LOGIN_DAYS);
		pPlayer->SetInt(FIELD_PROP_LOGIN_DAYS, nLoginDays_ + 1);


		// 定时更新角色登录日志(0点跨天)
		RoleLog log;
		log.loginTime = pPlayer->QueryInt64(FIELD_PROP_ROLE_LOGIN_TIME);
		log.logoutTime = ::time(NULL);
		log.ip = pPlayer->QueryString("Address");
		log.mac = pPlayer->QueryString("Token");
		log.osType = pPlayer->QueryString("ClientType");
		LogModule::m_pLogModule->SaveRoleLog(pKernel, self, log);


		// 重新设置登录时间
		pPlayer->SetInt64(FIELD_PROP_ROLE_LOGIN_TIME, ::time(NULL));
		// 角色退出
		LogModule::m_pLogModule->OnRoleExit(pKernel, self, true);
		// 用户退出
		const char* pszAccId = pPlayer->QueryString(FIELD_PROP_ACCOUNT);
		LogModule::m_pLogModule->OnUserLogout(pszAccId, 0, true);

		const char* pszPlatform = "";
		const char* pszModel = "";
		const char* pszManu = "";
		const char* pszDeviceId = "";

		if (pPlayer->FindData("TempLoginPlatform"))
		{
			pszPlatform = pPlayer->QueryDataString("TempLoginPlatform");
		}

		if (pPlayer->FindData("TempLoginModel"))
		{
			pszModel = pPlayer->QueryDataString("TempLoginModel");
		}

		if (pPlayer->FindData("TempLoginManufacturer"))
		{
			pszManu = pPlayer->QueryDataString("TempLoginManufacturer");
		}

		if (pPlayer->FindData("TempLoginDeviceId"))
		{
			pszDeviceId = pPlayer->QueryDataString("TempLoginDeviceId");
		}

		// 用户进入
		LogModule::m_pLogModule->OnUserLogin(pszAccId, pszAccId, log.ip.c_str(), pszPlatform, pszManu, pszDeviceId, pszModel, log.osType.c_str(), "", true);
		// 角色进入
		LogModule::m_pLogModule->OnRoleEntry(pKernel, self, true);
	}

//	ShopModule::m_pShopModule->OnResetPayedProducts(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
	return 0;
}

// 
// int PlayerBaseModule::OnCustomCheckStrength(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
// {
// 
// 	if (!pKernel->Exists(self))
// 	{
// 		return 0;
// 	}
// 
// 	if (args.GetCount() < 1)
// 	{
// 		return 0;
// 	}
// 
// 	IGameObj* player = pKernel->GetGameObj(self);
// 	if (NULL == player)
// 	{
// 		return 0;
// 	}
// 
// 	//获得装备战力
// 	const int equipability = player->QueryInt("EquipBattleAbility");
// 	//获得宠物加到玩家身上战力
// 	//const int petability = player->QueryInt("PetBattleAbility");
// 	//获得坐骑战力
// 	const int rideability = player->QueryInt("RideBattleAbility");
// 	//获得技能战力
// 	const int skillability = player->QueryInt("SkillBattleAbility");
// 	//获得军阶战力
// 	const int officelevability = player->QueryInt("OfficeLevBattleAbility");
// 
// 	//获得宝石,器魂战斗力,强化战斗力
// 	int jewelability = 0;
// 	int soulability = 0;
// 	int strengthability = 0;
// 
// 	IRecord* battle_ability_record = player->GetRecord(EQUIP_BATTLE_ABILITY_REC);
// 	if (battle_ability_record == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	int row_count = battle_ability_record->GetRows();
// 
// 	LoopBeginCheck(a);
// 	for (int row = 0; row < row_count; ++row)
// 	{
// 		LoopDoCheck(a);
// 		int equip_ability = battle_ability_record->QueryInt(row, EQUIP_BA_EQUIPMENT);
// 		if (equip_ability == -1)
// 		{
// 			// -1 表示此栏位没有装备
// 			continue;
// 		}
// 
// 		// 查看这一行是否存放的是武器的战斗力, 灵魂神装的战斗力也记录在这一行
// 		int equip_index = battle_ability_record->QueryInt(row, EQUIP_BA_EQUIP_POS);
// 		if (equip_index == EQUIP_POS_WEAPON)
// 		{
// 			soulability += battle_ability_record->QueryInt(row, EQUIP_BA_SOUL_CLOTH);
// 		}
// 
// 		// 累加器魂战斗力
// 		soulability += battle_ability_record->QueryInt(row, EQUIP_BA_SOUL);
// 
// 		// 累加强化战斗力
// 		strengthability += battle_ability_record->QueryInt(row, EQUIP_BA_STRHENTHEN);
// 
// 		// 累加宝石战斗力
// 		LoopBeginCheck(b);
// 		for (int col = EQUIP_BA_JEWEL1; col <= EQUIP_BA_JEWEL4; ++col)
// 		{
// 			LoopDoCheck(b);
// 			jewelability += battle_ability_record->QueryInt(row, col);
// 		}
// 	}
// 
// 	CVarList msg;
// 	msg << SERVER_CUSTOMMSG_CHECK_STRENGTH
// 		<< equipability
// 		<< rideability
// 		<< skillability
// 		<< officelevability
// 		<< jewelability
// 		<< soulability
// 		<< strengthability;
// 
// 	pKernel->Custom(self, msg);
// 
// 	return 0;
// }

int PlayerBaseModule::OnClientBackgroundTask(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
    CVarList msg;
    msg << SERVER_CUSTOMMSG_BACKGROUND;
    pKernel->Custom(self, msg);

    return 0;
}

// 冻结角色
int PlayerBaseModule::OnCommandBlockPlayer(IKernel* pKernel, const PERSISTID& self,
										const PERSISTID& sender, const IVarList& args)
{
	// 安全验证
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 取时间
	int64_t time = args.Int64Val(1);

	// 冻结角色
	BlockPlayer(pKernel, self, time);

	return 0;
}

int PlayerBaseModule::OnCommandClientQueryPropData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pPlayer_ = pKernel->GetGameObj(self);
	if (NULL == pPlayer_)
	{
		return 0;
	}

	// 通过命令转发 格式: 发送者 客户端定义标识 属性列表....
	Assert(args.GetCount() >= 4);
	const wchar_t* pwszSender_ = args.WideStrVal(1);
	int nud_ = args.IntVal(2);

	m_pPlayerBaseModule->HandlerPlayerPropQuery(pKernel, pwszSender_, *pPlayer_, args, 3, nud_);
	return 0;
}

// 冻结在线玩家角色
bool PlayerBaseModule::BlockPlayer(IKernel* pKernel, const PERSISTID& self, int64_t time)
{
	// 获取角色对象
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 记录封存时间点
	if (pSelfObj->FindData("BlockPlayerTime"))
	{
		pSelfObj->SetDataInt64("BlockPlayerTime", time);
	}
	else
	{
		pSelfObj->AddDataInt64("BlockPlayerTime", time);
	}

	// 通知客户端不要断线重连
	//pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_CONNECT_CLOSE << STR_SYSINFO_HEART_BEAT_ERROR);

	// 添加下线心跳
	ADD_COUNT_BEAT(pKernel, self, "PlayerBaseModule::CB_ForceOffline", FORCE_OFFLINE_TIME, 1);

	return true;
}

// 踢下线
int PlayerBaseModule::CB_ForceOffline(IKernel* pKernel, const PERSISTID& self, int time)
{
	// 获取角色对象
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 获取封存时间点
	int64_t blockTime = 0;
	if (pSelfObj->FindData("BlockPlayerTime"))
	{
		blockTime = pSelfObj->QueryDataInt64("BlockPlayerTime");
	}

	// 时间检测
	time_t tNow = ::time(NULL);
	if (blockTime <= 0)
	{
		blockTime = tNow;
	}

	tm structTime;
	util_get_local_time(&blockTime, &structTime);

#ifndef FSROOMLOGIC_EXPORTS
	// 强制下线
	bool bOk = pKernel->BlockPlayer(pSelfObj->QueryWideStr("Name"), 
		structTime.tm_year + 1900, 
		structTime.tm_mon + 1, 
		structTime.tm_mday, 
		structTime.tm_hour, 
		structTime.tm_min, 
		structTime.tm_sec);

	if (!bOk)
	{
		return 0;
	}

	if (time > tNow)
	{
		LogModule::m_pLogModule->OnBlockPlayer(pKernel, self, time - tNow, time);
	}
	else
	{
		LogModule::m_pLogModule->OnResumePlayer(pKernel, self);
	}

	// 设置可以清除当前角色数据
	if (!pSelfObj->FindData("OnNormalExitGame"))
	{
		pSelfObj->AddDataInt("OnNormalExitGame", 1);
	}
	else
	{
		pSelfObj->SetDataInt("OnNormalExitGame", 1);
	}
	pKernel->BreakPlayer(self);


#endif

	return 0;
}

// 冻结解封角色 time=0 解封
bool PlayerBaseModule::BlockPlayer(IKernel* pKernel, const wchar_t* playerName, int64_t time)
{
	// 安全检测
	if (StringUtil::CharIsNull(playerName))
	{
		return false;
	}

	// 玩家是否在线
	if (pKernel->GetPlayerScene(playerName) <= 0)
	{
		// 时间检测
		time_t tNow = ::time(NULL);
		if (time <= 0)
		{
			time = tNow;
		}

		// 玩家不在线
		tm structTime;
		util_get_local_time(&time, &structTime);

#ifndef FSROOMLOGIC_EXPORTS
		// 设置冻结时间
		bool bOK = pKernel->BlockPlayer(playerName, 
			structTime.tm_year + 1900, 
			structTime.tm_mon + 1, 
			structTime.tm_mday, 
			structTime.tm_hour, 
			structTime.tm_min, 
			structTime.tm_sec);
		if (bOK)
		{
			const char* pszRoleId = pKernel->SeekRoleUid(playerName);
			if ( time > tNow )
			{
				LogModule::m_pLogModule->OnBlockPlayer(pKernel, pszRoleId, time - tNow, time);
			}
			else
			{
				LogModule::m_pLogModule->OnResumePlayer(pKernel, pszRoleId );
			}
		}
#endif
	}
// 	else
// 	{
// 		// 玩家在线
// 		pKernel->CommandByName(playerName, CVarList() << COMMAND_BLOCK_PLAYER << time);
// 	}

	return true;
}

bool PlayerBaseModule::IsRobot(IKernel *pKernel, const PERSISTID &player)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 排除机器人
	if (pKernel->Find(player, "PrepareRole"))
	{
		int role_flag = pKernel->QueryInt(player, "PrepareRole");
		if (role_flag == ROLE_FLAG_ROBOT)
		{
			return true;
		}
	}
#endif
	return false;
}

int PlayerBaseModule::OnCustomGetPlayerPropData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetCount() < 2)
	{
		return 0;
	}

	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	// 玩家对象
	const wchar_t* pwszSender_ = pPlayer->QueryWideStr( FIELD_PROP_NAME);
	int nud_ = args.IntVal(1);
	const wchar_t* pwszTarget_ = args.WideStrVal(2);

	if (StringUtil::CharIsNull(pwszSender_) || StringUtil::CharIsNull(pwszTarget_))
	{
		// 参数错误
		return 0;
	}

	if ( wcscmp(pwszSender_, pwszTarget_) == 0 )
	{
		m_pPlayerBaseModule->HandlerPlayerPropQuery(pKernel, pwszSender_, *pPlayer, args, 3, nud_);
	}
	else
	{
		// 玩家是否在当前场景
		PERSISTID& p_ = pKernel->FindPlayer(pwszTarget_);
		IGameObj* pTarget_ = pKernel->GetGameObj(p_);
		if (NULL != pTarget_)
		{
			m_pPlayerBaseModule->HandlerPlayerPropQuery(pKernel, pwszSender_, *pTarget_, args, 3, nud_);
		}
		else
		{
			// 玩家是否在线
			int nScene_ = pKernel->GetPlayerScene(pwszTarget_);
			if (nScene_ > 0)
			{
				// 通过命令转发 格式: 发送者 客户端定义标识 属性列表....
				CVarList cmd_;
				cmd_ << COMMAND_CLIENT_QUERY_PROP_DATA << pwszSender_ << nud_;
				cmd_.Append(args, 3, args.GetCount() - 3);
				pKernel->CommandByName(pwszTarget_, cmd_);
			}
			else
			{
				// 直接返回失败
				CVarList retMsg_;
				retMsg_ << SERVER_CUSTOMMSG_GET_PLAYER_PROP_RSP << EM_PROP_QUERY_RESULT_OFFLINE << nud_ << pwszTarget_;
			}
		}
	}
	return 0;
}

//注册玩家版本号函数
bool PlayerBaseModule::RegisterVersionFun(PlayerVersion version, PLAYER_VERSION_FUNC fun)
{
	if(version < 0 || version >= PLAYER_VERSION_MAX)
	{
		return false;
	}
	if(fun == NULL)
	{
		return false;
	}
	m_funList[version] = fun;

	return true;
}

void PlayerBaseModule::HandlerPlayerPropQuery(IKernel* pKernel, const wchar_t* pszSender, IGameObj& player, const IVarList& query, int nOffset, int nUID)
{
	CVarList msg;
	msg << SERVER_CUSTOMMSG_GET_PLAYER_PROP_RSP << EM_PROP_QUERY_RESULT_SUCCEED << nUID << player.QueryWideStr(FIELD_PROP_NAME);

	int nCount = query.GetCount();
	for (int i = nOffset; i < nCount; ++i)
	{
		const char* pszProp = query.StringVal(i);
		// 先查是否在公开列表中(TODO: 以后通过配置定义)
		if (m_propPubset.find(pszProp) == m_propPubset.end())
		{
			continue;
		}

		if ( player.FindAttr(pszProp) )
		{
			int nType = player.GetAttrType(pszProp);
			msg << pszProp << nType;
			switch (nType)
			{
			case VTYPE_INT:		// 32位整数
			case VTYPE_BOOL:	// 布尔
				msg << player.QueryInt(pszProp);
				break;
			case VTYPE_INT64:	// 64位整数
				msg << player.QueryInt64(pszProp);
				break;
			case VTYPE_FLOAT:	// 单精度浮点数
				msg << player.QueryFloat(pszProp);
				break;
			case VTYPE_DOUBLE:	// 双精度浮点数
				msg << player.QueryDouble(pszProp);
				break;
			case VTYPE_STRING:	// 字符串
				msg << player.QueryString(pszProp);
				break;
			case VTYPE_WIDESTR:	// 宽字符串
				msg << player.QueryWideStr(pszProp);
				break;
			case VTYPE_OBJECT:	// 对象号
				msg << player.QueryObject(pszProp);
				break;
			default:
				break;
			}
		}
	}

	pKernel->CustomByName(pszSender, msg);
}

// 处理玩家版本号升级逻辑
bool PlayerBaseModule::HandlePlayerVersion(IKernel *pKernel, const PERSISTID &self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if( pSelfObj == NULL )
	{
		return false;
	}

	// 玩家当前版本号
	PlayerVersion version = (PlayerVersion)pSelfObj->QueryInt("PlayerVersion");

	if( version < 0 || version >= PLAYER_VERSION_MAX )
	{
		return true;
	}
	if( m_funList.size() == 0 )
	{
		return true;
	}

	// 版本号临时数据
	int tempVersion = version;

	bool isUpVersion = false;

	LoopBeginCheck(a);
    for ( int i = 0; i < PLAYER_VERSION_MAX; ++i )
	{
		LoopDoCheck(a);
		// 小于当前版本的不处理
		if( i <= version )
		{
			continue;
		}

		bool result = m_funList[i](pKernel, self, args);
		if( result )
		{
			tempVersion = i;

			isUpVersion = true;
		}
		else
		{
			break;
		}
	}
    
	// 设置新的版本号
	if( isUpVersion )
	{
		pSelfObj->SetInt("PlayerVersion", tempVersion);
	}

	return true;
}