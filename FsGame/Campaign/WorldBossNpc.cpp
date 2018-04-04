//--------------------------------------------------------------------
// 文件名:      WorldBossNpc.cpp
// 内  容:      世界Boss模块
// 说  明:		
// 创建日期:    2015年6月23日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "WorldBossNpc.h"

#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/CoolDownDefine.h"
#include "FsGame/Define/SceneListDefine.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/WorldBossNpcDefine.h"
#include "FsGame/NpcBaseModule/ai/AIDefine.h"
//#include "FsGame/Define/ToolBoxSysInfoDefine.h"

#include "FsGame/CommonModule/PubModule.h"
#include "FsGame/SocialSystemModule/ChatModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/SystemFunctionModule/ChannelModule.h"
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/CommonModule/LandPosModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/CommonModule/PropRefreshModule.h"
#include "FsGame/SystemFunctionModule/RankListModule.h"
#include "FsGame/Interface/FightInterface.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/CommonModule/ActionMutex.h"

#include <time.h>
#include <algorithm>
#include "utils/util.h"
#include "utils/XmlFile.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "../CommonModule/AsynCtrlModule.h"
#include "../CommonModule/EnvirValueModule.h"
#include "SystemFunctionModule/ResetTimerModule.h"
#include "Define/ResetTimerDefine.h"
#include "Define/Fields.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "ItemModule/ToolItem/ToolItemModule.h"

// boss 临时存储属性名字
static const char* WORLD_BOSS_ACTIVED                 = "world_boss_actived";

// 下午1点对应秒
static const int MIDDAY_SECOND                        = 13 * 60 * 60;
// 12点半对应秒
static const int MIDDAY_SECOND_AM                      = 12 * 60 * 60 + 30 * 60;

// 指针初始化
WorldBossNpc*       WorldBossNpc::m_pWorldBossNpc            = NULL;    
ChannelModule*      WorldBossNpc::m_pChannelModule           = NULL;
RewardModule*       WorldBossNpc::m_pRewardModule            = NULL;
DropModule*         WorldBossNpc::m_pDropModule              = NULL;
LandPosModule*      WorldBossNpc::m_pLandPosModule           = NULL;
CapitalModule*      WorldBossNpc::m_pCapitalModule           = NULL;

// PUB域名字
std::wstring WorldBossNpc::m_domainName; 

// GM设置时间
GMTimeInfo WorldBossNpc::GMTime;

TimeInterval_t WorldBossNpc::m_NoticeTimeCount;
TimeInterval_t WorldBossNpc::m_ActiveTimeCount;

std::vector<int> WorldBossNpc::m_SceneScopeVec;
WorldBossNpc::ConstConfig WorldBossNpc::m_kConstConfig;
WorldBossNpc::BossAwardVec WorldBossNpc::m_vecBossAward;
WorldBossNpc::WorldBossActiveVec WorldBossNpc::m_vecActiveInfo;


// 重加载活动配置文件
int nx_reload_world_boss_active_config(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == pKernel)
	{
		return 0;
	}
	WorldBossNpc::m_pWorldBossNpc->LoadActiveInfoResource(pKernel);

	return 0;
}

// 重加载奖励配置文件
int nx_reload_world_boss_award_config(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == pKernel)
	{
		return 0;
	}
	WorldBossNpc::m_pWorldBossNpc->LoadAwardResource(pKernel);

	return 0;
}

// 重加载BOSS成长配置文件
int nx_reload_world_boss_grow_up_config(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == pKernel)
	{
		return 0;
	}
	WorldBossNpc::m_pWorldBossNpc->LoadActiveRuleResource(pKernel);

	return 0;
}

// 重置世界BOSS活动GM
int nx_worldboss_start(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == pKernel)
	{
		return 0;
	}

	// 宏定义  检查LUA脚本参数个数
	CHECK_ARG_NUM(state, nx_worldboss_start, 2);

	CHECK_ARG_OBJECT(state, nx_worldboss_start, 1);

	CHECK_ARG_INT(state, nx_worldboss_start, 2);

	int iKillTimes = pKernel->LuaToInt(state, 2);

	CVarList CMDMsg;
	CMDMsg << COMMAND_WORLD_BOSS_ACTIVE 
		   << SS_WORLD_BOSS_ACTIVE_RESTART 
		   << iKillTimes;

	pKernel->CommandToAllScene(CMDMsg);

	return 0;
}

// 重置世界BOSS活动GM
int nx_worldboss_end(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == pKernel)
	{
		return 0;
	}

	CVarList CMDMsg;
	CMDMsg << COMMAND_WORLD_BOSS_ACTIVE 
		   << SS_WORLD_BOSS_ACTIVE_END;

	pKernel->CommandToAllScene(CMDMsg);

	return 0;
}

/*!
* @brief	初始化
* @param	核心指针
* @return	bool
*/
bool WorldBossNpc::Init(IKernel* pKernel)
{
	m_pWorldBossNpc      = this;
	m_pChannelModule     = dynamic_cast<ChannelModule*>(pKernel->GetLogicModule("ChannelModule"));
	m_pRewardModule      = dynamic_cast<RewardModule*>(pKernel->GetLogicModule("RewardModule"));
	m_pDropModule        = dynamic_cast<DropModule*>(pKernel->GetLogicModule("DropModule"));
	m_pLandPosModule     = dynamic_cast<LandPosModule*>(pKernel->GetLogicModule("LandPosModule"));
	m_pCapitalModule     = dynamic_cast<CapitalModule*>(pKernel->GetLogicModule("CapitalModule"));

	Assert(m_pWorldBossNpc 
		&& m_pChannelModule 
		&& m_pRewardModule 
		&& m_pDropModule 
		&& m_pLandPosModule
		&& m_pCapitalModule);

	// 加载配置
	LoadAwardResource(pKernel);
	LoadActiveInfoResource(pKernel);
	LoadActiveRuleResource(pKernel);
	LoadWorldBossGrowUpConfig(pKernel);

	// 场景创建回调
	pKernel->AddEventCallback("scene", "OnCreate", WorldBossNpc::OnCreateScene);

	// 玩家上线
	pKernel->AddEventCallback("player", "OnRecover", WorldBossNpc::OnRecover);

	// 添加玩家准备就绪回调
	pKernel->AddEventCallback("player", "OnReady", WorldBossNpc::OnPlayerReady);

	// 添加玩家准备就绪回调
	pKernel->AddEventCallback("player", "OnContinue", WorldBossNpc::OnPlayerContinue);

	// Boss死亡监听
	pKernel->AddIntCommandHook("NormalNpc", COMMAND_BEKILL, WorldBossNpc::OnBossBeKilled);

	// 客户端消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_WORLD_BOSS_ACTIVE, OnCustomMessage);

	// 玩家上线
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CONTINUE_ON_READY, WorldBossNpc::OnCustomReady);

	// 注册进入场景之后事件回调
	pKernel->AddEventCallback("player",  "OnAfterEntryScene",  WorldBossNpc::OnPlayerEntryScence);

	// GM命令（重开活动）
	pKernel->AddIntCommandHook("scene", COMMAND_WORLD_BOSS_ACTIVE, WorldBossNpc::OnCommandMessage, 100);

	// 注册BOSS被攻击回调
	pKernel->AddIntCommandHook("WorldBossNpc", COMMAND_BEDAMAGE, WorldBossNpc::OnCommandBossBeDamaged);

	// 心跳定义
	DECL_HEARTBEAT(WorldBossNpc::HB_CheckWorldBossActive);
	DECL_HEARTBEAT(WorldBossNpc::HB_CheckNotice);
	DECL_HEARTBEAT(WorldBossNpc::HB_SendCustomMsg);
	DECL_HEARTBEAT(WorldBossNpc::HB_ShowAttackRank);
	DECL_HEARTBEAT(WorldBossNpc::HB_StartPlayCG);
	DECL_HEARTBEAT(WorldBossNpc::HB_BossBorn);
	DECL_HEARTBEAT(WorldBossNpc::HB_LeaveScene);

	// LUA定义
	DECL_LUA_EXT(nx_reload_world_boss_active_config);
	DECL_LUA_EXT(nx_reload_world_boss_award_config);
	DECL_LUA_EXT(nx_worldboss_start);
	DECL_LUA_EXT(nx_worldboss_end);

	RELOAD_CONFIG_REG("WorldBossConfig", WorldBossNpc::ReloadConfig);
	return true;
}

/*!
* @brief	销毁  
* @param	核心指针
* @return	bool
*/
bool WorldBossNpc::Shut(IKernel* pKernel)
{
	m_SceneScopeVec.clear();
	m_vecBossAward.clear();
	m_vecActiveInfo.clear();

	return true;
}

/*!
* @brief	时间转换成秒
* @param	时间字符串
* @param	数据引用
* @return	void
*/
int WorldBossNpc::ParseTime(const char* timesStr)
{
	// 判空
	if (StringUtil::CharIsNull(timesStr))
	{
		return 0;
	}

	return util_analyzing_time_str(timesStr);
}

/*!
* @brief	世界BOSS转换
* @param	配置字符串
* @param	数据引用
* @return	void
*/
void WorldBossNpc::ParseBossID(const char* strBossIDs, std::map<int, std::string>& mapBossIDs)
{
	// 判空
	if (StringUtil::CharIsNull(strBossIDs))
	{
		return;
	}

	CVarList BossIDList;
	util_split_string(BossIDList, strBossIDs, ",");

	int iCount = (int)BossIDList.GetCount();
	LoopBeginCheck(m);
	for (int i = 0; i < iCount; i++)
	{
		LoopDoCheck(m);
		const char* bossStr = BossIDList.StringVal(i);
		if (StringUtil::CharIsNull(bossStr))
		{
			continue;
		}
		CVarList _BossIDList;
		util_split_string(_BossIDList, bossStr, ":");

		if (_BossIDList.GetCount() == 2)
		{
			mapBossIDs.insert(std::make_pair(_BossIDList.IntVal(0), _BossIDList.StringVal(1)));
		}
	}
}

/*!
* @brief	星期转换
* @param	星期字符串
* @param	数据引用
* @return	void
*/
void WorldBossNpc::ParseWeek(const char* weeksStr, std::vector<int>& vecWeeks)
{
	// 判断是否为空
	if (StringUtil::CharIsNull(weeksStr))
	{
		return;
	}

	CVarList WeekList;
	util_split_string(WeekList, weeksStr,",");

	int iCount = (int)WeekList.GetCount();
	LoopBeginCheck(m);
	for (int i = 0; i < iCount; i++)
	{
		LoopDoCheck(m);
		vecWeeks.push_back(WeekList.IntVal(i));
	}
}

// 内部命令回调
int WorldBossNpc::OnCommandMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 保护
	if (NULL == pKernel || !pKernel->Exists(self))
	{
		return 0;
	}

	int iSubCMD = args.IntVal(1);
	switch(iSubCMD)
	{
	case SS_WORLD_BOSS_ACTIVE_RESTART:
		{
			OnCommandGMRestartActive(pKernel, self, args);
		}
		break;

	case SS_WORLD_BOSS_ACTIVE_UPDATE_DATA:
		{
			m_ActiveTimeCount.m_BeginTime = args.IntVal(2);
			m_ActiveTimeCount.m_EndTime = args.IntVal(3);
		}
		break;

	case SS_WORLD_BOSS_ACTIVE_END:
		{
			OnCommandGMEndActive(pKernel, self, args);
		}
		break;

	default:
		break;
	}
	return 0;
}

// 活动重新开启
int WorldBossNpc::OnCommandGMRestartActive(IKernel* pKernel, 
										   const PERSISTID& self, 
										   const IVarList& args)
{
	// 找到需要开启活动心跳的场景
	int iSceneID = pKernel->GetSceneId();
	std::vector<int>& vecSceneScope = m_SceneScopeVec;

	std::vector<int>::iterator it = find(vecSceneScope.begin(), 
		vecSceneScope.end(), iSceneID);
	if (it != vecSceneScope.end())
	{
		PERSISTID CurrScene = pKernel->GetScene();
		IGameObj* pScene = pKernel->GetGameObj(CurrScene);
		if (NULL == pScene)
		{
			return NULL;
		}

		// 清除活动相关数据
		DELETE_HEART_BEAT(pKernel, CurrScene, "WorldBossNpc::HB_CheckNotice" );
		DELETE_HEART_BEAT(pKernel, CurrScene, "WorldBossNpc::HB_CheckWorldBossActive" );

		if (pScene->FindData(WORLD_BOSS_ACTIVED))
		{
			PERSISTID WorldBoss = pScene->QueryDataObject(WORLD_BOSS_ACTIVED);
			if (pKernel->Exists(WorldBoss))
			{
				// 销毁BOSS对象
				pKernel->Destroy(CurrScene, WorldBoss);
			}
		}

		m_pWorldBossNpc->ClearActiveData(pKernel);

		// 设置GM标识
		pScene->SetDataInt("WorldBossGM", 1);

		// 设置死亡次数
		if (args.IntVal(2) > 0)
		{
			CVarList PubMsg;
			PubMsg << SP_WORLD_BOSS_MSG_STATUS 
				   << SP_WORLD_BOSS_BE_KILL_TIME_SET_GM 
				   << pKernel->GetSceneId() 
				   << args.IntVal(2);
			SendMsgToPubServer(pKernel, PubMsg);
		}

		// 重新设置数据(时间)
		int daySecond = util_convert_zerotime_to_sec();

		// 公告时间
		GMTime.m_NoticeBeginTime = daySecond;
		GMTime.m_NoticeEndTime = daySecond + 60 * 1 - 10;

		// 活动时间
		GMTime.m_ActiveBeginTime = daySecond + 60 * 1;
		GMTime.m_ActiveEndTime = GMTime.m_ActiveBeginTime + 60 * 10;

		// 定时检查世界BOSS活动时间
		ADD_HEART_BEAT(pKernel, self, "WorldBossNpc::HB_CheckWorldBossActive", 3 * 1000);
	}

	return 0;
}

// 活动结束
int WorldBossNpc::OnCommandGMEndActive(IKernel* pKernel, 
										   const PERSISTID& self, 
										   const IVarList& args)
{
	// 重新设置数据(时间)
	int daySecond = util_convert_zerotime_to_sec();

	// 活动时间
	GMTime.m_ActiveBeginTime = daySecond + 1;
	GMTime.m_ActiveEndTime = GMTime.m_ActiveBeginTime + 10;

	return 0;
}

/*!
* @brief	监听场景创建 
* @param	pKernel
* @param	self
* @param	sender
* @param	args
* @return	int
*/
int WorldBossNpc::OnCreateScene(IKernel* pKernel, 
								const PERSISTID& self, 
								const PERSISTID& sender, 
								const IVarList& args)
{
	// 保护
	if (!pKernel->Exists(sender))
	{
		return 0;
	}

	// 场景是否存在
	IGameObj* pScene = pKernel->GetGameObj(self);
	if (NULL == pScene)
	{
		return 0;
	}

	// 需要开启活动心跳场景
	int iCurrSceneID = pKernel->GetSceneId();
	std::vector<int>::iterator it = find(m_SceneScopeVec.begin(), 
		m_SceneScopeVec.end(), iCurrSceneID);
	if (it == m_SceneScopeVec.end())  																											  
	{
		return 0;
	}

	m_pWorldBossNpc->AddSceneData(pKernel, self);

	ADD_HEART_BEAT(pKernel, self, "WorldBossNpc::HB_CheckWorldBossActive", TIMER_WORLDBOSS_ACTIVE * 1000);
	return 0;
}

/*!
* @brief	监听活动状态心跳 
* @param	pKernel
* @param	self
* @param	slice
* @return	int
*/
int WorldBossNpc::HB_CheckWorldBossActive(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 场景是否存在
	IGameObj* pScene = pKernel->GetGameObj(self);
	if (NULL == pScene)
	{
		return 0;
	}

	int nSceneId = pKernel->GetSceneId();

	// 统计当天活动类型
	time_t tNow = ::time(NULL);
	tm* pNow = ::localtime(&tNow);
	if (NULL == pNow)
	{
		return 0;
	}

	int iCurrSec = pNow->tm_hour * 3600 + pNow->tm_min * 60 + pNow->tm_sec;
	//UpdateTodayActive(pKernel, self, iCurrSec);

	std::vector<WorldBossActive_t>& vecActiveInfo = m_vecActiveInfo;
	int iSize = (int)vecActiveInfo.size();
	LoopBeginCheck(q);
	for (int i = 0; i < iSize; ++i)
	{
		LoopDoCheck(q);
		// 场景活动配置信息
		WorldBossActive_t* pActiveInfo = &vecActiveInfo[i];
		if (NULL == pActiveInfo)
		{
			continue;
		}

		// 判断场景
		if (pKernel->GetSceneId() != pActiveInfo->m_SceneID)
		{
			continue;
		}

		// 查询是否有GM设置(默认为0--没有开启GM)
		int iGMVal = 0;
		if (pScene->FindData(FLAG_WORLD_BOSS_GM))
		{
			iGMVal = pScene->QueryDataInt(FLAG_WORLD_BOSS_GM);
		}

		// 开关
		bool bSwitch = true;

		// 功能开关
		if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WORLD_BOSS))
		{
			bSwitch = false;
		}	

		int iState = STATE_WBOSS_ACTIVE_CLOSE;
		if (iGMVal == 1)
		{
// 			if (GMTime.m_DoRule != pActiveInfo->m_DoRule)
// 			{
// 				continue;
// 			}

			// 开关开
			if (bSwitch)
			{
				iState = m_pWorldBossNpc->GetGMWorldBossActiveState(pKernel, self, pActiveInfo, iCurrSec);
			}

			m_NoticeTimeCount.m_BeginTime = GMTime.m_NoticeBeginTime;
			m_NoticeTimeCount.m_EndTime = GMTime.m_NoticeEndTime;
			m_ActiveTimeCount.m_BeginTime = GMTime.m_ActiveBeginTime;
			m_ActiveTimeCount.m_EndTime = GMTime.m_ActiveEndTime;
		}
		else
		{
			// 查找星期几
			std::vector<int>& vecWeeks = pActiveInfo->m_Weeks;
			std::vector<int>::iterator it = find(vecWeeks.begin(), vecWeeks.end(), pNow->tm_wday);
			if (it == vecWeeks.end())
			{
				continue;
			}

			// 开关开
			if (bSwitch)
			{
				iState = m_pWorldBossNpc->GetWorldBossActiveState(pKernel, self, pActiveInfo, iCurrSec);
			}

			m_NoticeTimeCount.m_BeginTime = pActiveInfo->m_NoticeBeginTime;
			m_NoticeTimeCount.m_EndTime = pActiveInfo->m_NoticeEndTime;
			m_ActiveTimeCount.m_BeginTime = pActiveInfo->m_ActiveBeginTime;
			m_ActiveTimeCount.m_EndTime = pActiveInfo->m_ActiveEndTime;
		}

		// 更新member数据
		m_pWorldBossNpc->SyncMemberData(pKernel);

		// 活动开启，设置配置ID
		if (iState == STATE_WBOSS_ACTIVE_OPEN || iState == STATE_WBOSS_ACTIVE_NOTICE)
		{
			if (pScene->FindData(WORLD_BOSS_ACTIVE_CFG_ID))
			{
				pScene->SetDataInt(WORLD_BOSS_ACTIVE_CFG_ID, pActiveInfo->m_ID);
			}
		}

		// 获取活动状态是否已经执行
		int iVisit = pScene->QueryInt(FLAG_WORLD_BOSS_VISIT);;
		switch(iState)
		{
		case STATE_WBOSS_ACTIVE_NOTICE:
			{
				if (iVisit != FLAG_WORLD_BOSS_VISIT_NOTICED)
				{
					m_pWorldBossNpc->ActiveNotice(pKernel, self);
				}
				break;
			}
		case STATE_WBOSS_ACTIVE_OPEN:
			{
				if (/*iVisit < FLAG_WORLD_BOSS_VISIT_ACTIVED*/iVisit == FLAG_WORLD_BOSS_VISIT_NOTICED)
				{
					m_pWorldBossNpc->ActiveOpening(pKernel, self, pActiveInfo, pNow, iState, i);
				}
				break;
			}
		case STATE_WBOSS_ACTIVE_CLOSE: 
			{
				if (pScene->QueryDataInt(WORLD_BOSS_ACTIVE_CFG_ID) == pActiveInfo->m_ID 
					&& iVisit != FLAG_WORLD_BOSS_VISIT_CLOSED 
					&& iVisit == FLAG_WORLD_BOSS_VISIT_BORNED)
				{
					m_pWorldBossNpc->ActiveClosed(pKernel, self, iState);
				}
				break; 
			}
		default:
			break;
		}
	}

	return 0;
}

/*!
* @brief	创建BOSS  
* @param	引擎指针
* @param	当前场景
* @param	引用返回对象
* @return	函数返回int
*/
PERSISTID WorldBossNpc::CreateWorldBoss(IKernel* pKernel, 
										const WorldBossActive_t* pActiveInfo, 
										const int& iCurrSceneID, 
										std::string& strBossPos, 
										int iActiveTime, 
										int iWeek)
{
	// 获取BOSS信息,创建BOSS
	IGameObj* pScene = pKernel->GetGameObj(pKernel->GetScene());
	if (NULL == pScene)
	{
		return PERSISTID();
	}

	if (NULL == pActiveInfo)
	{
		return PERSISTID();
	}

	// 获取BOSSID
	const std::string& strBossCfgID = pActiveInfo->m_strBossID;
	pScene->SetDataString("BOSS_CONFIG_ID", strBossCfgID.c_str());

	// BOSS坐标
	float fY = pKernel->GetMapHeight(pActiveInfo->m_BossPos.fPosX, pActiveInfo->m_BossPos.fPosZ);

	// 创建BOSS	
	CVarList BossParam;
	BossParam << CREATE_TYPE_PROPERTY_VALUE 
			  << "GroupID"
			  << -1;
	PERSISTID WorldBoss = pKernel->CreateObjectArgs("", strBossCfgID.c_str(), 0, pActiveInfo->m_BossPos.fPosX, fY, pActiveInfo->m_BossPos.fPosZ, pActiveInfo->m_BossPos.fOrient, BossParam);
	IGameObj* pWorldBossObj = pKernel->GetGameObj(WorldBoss);
	if (NULL == pWorldBossObj)
	{
		return PERSISTID();
	}

	// 获取增加的属性包id
	int nCurBossPropertyPakId = m_pWorldBossNpc->QueryCurPackageId(pKernel);
	if (nCurBossPropertyPakId > 0)
	{
		// 给BOSS加属性
		MapPropValue mapPropValue;
		CVarList vPropNameList;
		CVarList vPropValueList;
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_NPC_BASE_PACK,
			StringUtil::IntAsString(nCurBossPropertyPakId), vPropNameList, vPropValueList, VTYPE_INT64);

		LoopBeginCheck(n);
		for (size_t i = 0; i < vPropNameList.GetCount(); ++i)
		{
			LoopDoCheck(n);
			// 获取计算公式数据
			float fVal = (float)vPropValueList.IntVal(i);
			if (FloatEqual(fVal, 0.0f))
			{
				continue;
			}
			PropRefreshModule::m_pInstance->CountRefreshData(pKernel, WorldBoss,
				vPropNameList.StringVal(i), fVal, EModify_ADD_VALUE, mapPropValue);
		}
		PropRefreshModule::m_pInstance->RefreshData(pKernel, WorldBoss, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	}
	
	int64_t nMaxHP = pWorldBossObj->QueryInt64(FIELD_PROP_MAX_HP);
	pWorldBossObj->SetInt64(FIELD_PROP_HP, nMaxHP);
	
	// 添加可视优先级
	pKernel->AddVisualPriorityByScene(WorldBoss, true);

	// 记录boss刷新时间
	pScene->SetInt64(FIELD_PROP_WORLD_BOSS_CREATE_TIME, ::util_get_utc_time());

	// 设置BOSS LifeTime
	IGameObj* pWorldBoss = pKernel->GetGameObj(WorldBoss);
	if (NULL == pWorldBoss)
	{
		return PERSISTID();
	}
	pWorldBoss->SetInt("LifeTime", (iActiveTime + 3) * 1000);
	return WorldBoss;
}


/*!
* @brief	获取活动状态
* @param	引擎指针
* @param	当前场景
* @param	返回活动状态引用
* @return	函数返回bool
*/
int WorldBossNpc::GetWorldBossActiveState(IKernel* pKernel, 
										  const PERSISTID& self, 
										  const WorldBossActive_t* pActiveInfo, 
										  int iCurrSec)
{
	// 默认值
	int iState = STATE_WBOSS_ACTIVE_CLOSE;

	// 发布公告
	if (pActiveInfo->m_NoticeBeginTime <= iCurrSec && iCurrSec < pActiveInfo->m_NoticeEndTime)
	{
		iState = STATE_WBOSS_ACTIVE_NOTICE;
	}
	// 开启活动
	else if (pActiveInfo->m_ActiveBeginTime <= iCurrSec && iCurrSec <= pActiveInfo->m_ActiveEndTime)
	{
		iState = STATE_WBOSS_ACTIVE_OPEN;
	}
	// 活动关闭
	else
	{
		iState = STATE_WBOSS_ACTIVE_CLOSE;
	}

	return iState;
}

/*!
* @brief	获取GM活动状态
* @param	引擎指针
* @param	当前场景
* @param	返回活动状态引用
* @return	函数返回bool
*/
int WorldBossNpc::GetGMWorldBossActiveState(IKernel* pKernel, 
											const PERSISTID& self, 
											const WorldBossActive_t* pActiveInfo, 
											int iCurrSec)
{
	// 默认值
	int iState = STATE_WBOSS_ACTIVE_CLOSE;

	// 发布公告
	if (GMTime.m_NoticeBeginTime <= iCurrSec && iCurrSec < GMTime.m_NoticeEndTime)  
	{
		iState = STATE_WBOSS_ACTIVE_NOTICE;
	}
	// 开启活动
	else if (GMTime.m_ActiveBeginTime <= iCurrSec && iCurrSec <= GMTime.m_ActiveEndTime)  
	{
		iState = STATE_WBOSS_ACTIVE_OPEN;
	}
	// 活动关闭
	else
	{
		iState = STATE_WBOSS_ACTIVE_CLOSE;
	}
	return iState;
}

/*!
* @brief	获取活动配置信息指针
* @param    场景ID
* @return	WorldBossActive_t对象指针
*/
WorldBossActive_t* WorldBossNpc::GetActiveInfoCfg(IKernel* pKernel, const int iID)
{
	std::vector<WorldBossActive_t>& vecActiveInfo = m_vecActiveInfo;
	int iSize = (int)vecActiveInfo.size();

	LoopBeginCheck(q);
	for (int i = 0; i < iSize; ++i)
	{
		LoopDoCheck(q);
		if (iID == vecActiveInfo[i].m_ID)
		{
			return &vecActiveInfo[i];
		}
	}

	return NULL;
}

// 获取当前场景活动信息
WorldBossActive_t* WorldBossNpc::GetActiveInfoCfgBySceneId(IKernel* pKernel, const int nSceneId)
{
	std::vector<WorldBossActive_t>& vecActiveInfo = m_vecActiveInfo;
	int iSize = (int)vecActiveInfo.size();

	LoopBeginCheck(q);
	for (int i = 0; i < iSize; ++i)
	{
		LoopDoCheck(q);
		if (nSceneId == vecActiveInfo[i].m_SceneID)
		{
			return &vecActiveInfo[i];
		}
	}

	return NULL;
}

/*!
* @brief	获取活动奖励配置信息指针
* @param    击杀次数
* @return	WorldBossAward_t对象指针
*/
WorldBossAward_t* WorldBossNpc::GetBossAward(IKernel* pKernel, const char* strBossCfgID)
{
	// 保护
	if (m_vecBossAward.empty() || StringUtil::CharIsNull(strBossCfgID))
	{
		return NULL;
	}

	std::vector<WorldBossAward_t>& vecAward = m_vecBossAward;

	// 遍历查找符合条件的奖励
	struct BeKillTimesFinder
	{
		BeKillTimesFinder(const std::string& strBossID): _strBossID(strBossID){};
		bool operator() (const WorldBossAward_t& award)
		{
			return (strcmp(award.m_BossCfgID.c_str(), _strBossID.c_str()) == 0);
		}

		std::string _strBossID;
	};
	std::vector<WorldBossAward_t>::iterator it = find_if(vecAward.begin(), 
		vecAward.end(), BeKillTimesFinder(strBossCfgID));

	if (it != vecAward.end())
	{
		WorldBossAward_t& award = *it;
		return &award;
	}

	return  NULL;
}

/*!
* @brief	监听BOSS死亡
* @param    引擎指针
* @param    BOSS对象
* @param    最后一击玩家对象
* @param    携带参数
* @return	函数执行返回int
*/
int WorldBossNpc::OnBossBeKilled(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 取出前场景
	PERSISTID CurrScene = pKernel->GetScene();
	IGameObj* pScene = pKernel->GetGameObj(CurrScene);
	if (NULL == pScene)
	{
		return 0;
	}

	//判断是否有活动场景标识
	if (!pScene->FindAttr(FLAG_WORLD_BOSS_VISIT))
	{	
		return 0;
	}

	// 判断是否活动期间内击杀BOSS
	int iVisit = pScene->QueryInt(FLAG_WORLD_BOSS_VISIT);
	if (FLAG_WORLD_BOSS_VISIT_BORNED != iVisit)
	{
		return 0;
	}

	// 更新场景精英怪状态
	IRecord* pEliteNpcRec = pScene->GetRecord(WORLD_BOSS_ELITE_NPC_REC);
	if (NULL != pEliteNpcRec)
	{
		int nRowIndex = pEliteNpcRec->FindObject(WBEN_COL_NPC_OBJECT, self);
		if (-1 != nRowIndex)
		{
			pEliteNpcRec->SetObject(nRowIndex, WBEN_COL_NPC_OBJECT, PERSISTID());
			int nNpcIndex = pEliteNpcRec->QueryInt(nRowIndex, WBEN_COL_NPC_INDEX);
			CVarList senddata;
			senddata << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE << SC_WORLD_BOSS_ELITE_NPC_DEAD << nNpcIndex;
			pKernel->CustomByScene(pKernel->GetSceneId(), senddata);
		}
	}

	//判断是否活动场景里创建的BOSS
	PERSISTID QueryBoss =  pScene->QueryDataObject(WORLD_BOSS_ACTIVED);
	if (QueryBoss != self)
	{
		return 0;
	}

	// 保存活动状态
	m_pWorldBossNpc->OnSaveActiveStatusToPUB(pKernel, pKernel->GetSceneId(), 
		STATE_WBOSS_ACTIVE_CLOSE, pScene->QueryDataInt(WORLD_BOSS_ACTIVE_CFG_ID));

	// 设置结束标识
	//pScene->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_CLOSED);

	// 伤害数据拷贝
	m_pWorldBossNpc->CopyAndSortHurtRecordFromAI(pKernel, self, sender);

	// 最后一击名字
	const wchar_t* wsName = pKernel->QueryWideStr(sender, "Name"); 
	if (!StringUtil::CharIsNull(wsName))
	{
		// 设置最后一击名字
		pScene->SetDataWideStr("Last_Harm", wsName);
		
		// 最后一击奖励
		m_pWorldBossNpc->ActiveLastDamageAward(pKernel, self, sender);
	}

	// 排名奖励
	m_pWorldBossNpc->ActiveRankAward(pKernel, CurrScene, SUC_KILL_WORLD_BOSS);

	// boss成长级别变化
	m_pWorldBossNpc->UpdateGrowUpLevel(pKernel, true);

	// 重置BOSS临时对象，销毁用
	pScene->SetDataObject(WORLD_BOSS_ACTIVED, PERSISTID());

	// 通知活动结束
	m_pWorldBossNpc->NoticeActiveFinish(pKernel, CurrScene, true);
	return 0;
}


/*!
* @brief	BOSS死亡时候把伤害统计数据拷贝过来 
* @param    引擎指针
* @param    BOSS对象
* @param    sender
* @return	函数执行返回bool
*/
bool WorldBossNpc::CopyAndSortHurtRecordFromAI(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender)
{
	// 保护判断
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 取出BOSS身上的伤害统计表格数据
	IRecord* pAIRecord = pKernel->GetRecord(self, WORLDBOSS_ATTACK_LIST_REC);
	if (NULL == pAIRecord)
	{
		return false;
	}

	int iAICount = pAIRecord->GetRows();

	CopyAIHurtVector tempDatas;
	tempDatas.reserve(iAICount);

	LoopBeginCheck(u); 
	for (int i = 0; i < iAICount; ++i)
	{
		LoopDoCheck(u);
		int iDamage = pAIRecord->QueryInt(i, WORLDBOSS_ATTACK_LIST_REC_DAMAGE);
		if (iDamage > 0)
		{
			CopyAIHurt_t tmp;
			tmp.m_player_name = pAIRecord->QueryWideStr(i, WORLDBOSS_ATTACK_LIST_REC_PLAYER_NAME);
			tmp.m_damage = iDamage;
			tempDatas.push_back(tmp);
		}		

	}
	// 排序
	sort(tempDatas.begin(), tempDatas.end(), std::less<CopyAIHurt_t>());

	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return false;
	}
	IRecord* pSceneDamRec = pSceneObj->GetRecord(WORLD_BOSS_DAMAGE_REC);
	if (NULL == pSceneDamRec)
	{
		return false;
	}

	pSceneDamRec->ClearRow();
	// 添加数据
	int len = (int)tempDatas.size();
	LoopBeginCheck(v);
	for (int i = 0; i < len; ++i)
	{
		LoopDoCheck(v);
		const CopyAIHurt_t& tmp = tempDatas[i];
		pSceneDamRec->AddRowValue(-1, CVarList() << i + 1 << tmp.m_player_name << tmp.m_damage);
	}

	const wchar_t* wsKillerName = L"";
	if (pKernel->Exists(sender))
	{
		wsKillerName = pKernel->QueryWideStr(sender, FIELD_PROP_NAME);
	}
	
	CVarList PubMsg;
	PubMsg << SP_WORLD_BOSS_MSG_STATUS
		<< SP_UPDATE_BOSS_REC_INFO
		<< pKernel->GetSceneId()
		<< wsKillerName
		<< STATE_WBOSS_ACTIVE_CLOSE;

	SendMsgToPubServer(pKernel, PubMsg);
	return true;
}


/*!
* @brief	处理客户端消息
* @param    引擎指针
* @param    玩家对象
* @param    场景
* @param    携带参数
* @return	函数执行返回int
*/
int WorldBossNpc::OnCustomMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 保护判断
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WORLD_BOSS, self))
	{
		return 0;
	}

	// 子消息ID
	int iSubCommand = args.IntVal(1);
	switch(iSubCommand)
	{
		// 传送玩家到指定场景
	case CS_WORLD_BOSS_ACTIVE_TRANSMIT:
		{
			// 根据玩家国籍找出活动场景
			int iScene = args.IntVal(2);
			if (!m_pWorldBossNpc->IsWorldBossScene(iScene))
			{
				return 0;
			}
			WorldBossActive_t* pActiveInfo = m_pWorldBossNpc->GetActiveInfoCfgBySceneId(pKernel, iScene);
			if (NULL == pActiveInfo)
			{ 
				return 0;
			}

			// 目标场景最大人数保护
// 			if (SceneInfoModule::IsMaxPeopNum(pKernel, pActiveInfo->m_SceneID))
// 			{
// 				//目标场人数已达最大数，请稍后再试
// 				::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, 
// 					GUIDE_BATTLE_MODULE_CUSTOM_string_3, CVarList());
// 
// 				return 0;
// 			}

			// 当前是否有场景正在活动
			int nStatus = m_pWorldBossNpc->GetActiveStatusFromPUB(pKernel, iScene);
			if (nStatus != STATE_WBOSS_ACTIVE_OPEN)
			{
				// 系统提示活动关闭不能进入
				CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_52, CVarList());
				return 0;
			}

			// 判断等级条件
			int iLevel = pSelf->QueryInt("Level");
			if (iLevel < pActiveInfo->m_PlayerLevel)
			{
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_51, CVarList() << pActiveInfo->m_PlayerLevel);
				return 0;
			}
			
			// 传送
			float fPosY = pKernel->GetMapHeight(pActiveInfo->m_PlayerPos.fPosX, pActiveInfo->m_PlayerPos.fPosZ);
			if (NULL != m_pLandPosModule)
			{
				m_pLandPosModule->SetPlayerLandPosi(pKernel, self, pKernel->GetSceneId(), pSelf->GetPosiX(), pSelf->GetPosiY(), pSelf->GetPosiZ());
			}

			AsynCtrlModule::m_pAsynCtrlModule->SwitchLocate(pKernel, self, iScene, pActiveInfo->m_PlayerPos.fPosX, fPosY, pActiveInfo->m_PlayerPos.fPosZ, pActiveInfo->m_PlayerPos.fOrient);
		}
		break;

		// 返回上一个场景
	case CS_WORLD_BOSS_ACTIVE_BACK_TO_LAST_SCENE:
		{
			if (NULL != m_pLandPosModule)
			{
				m_pLandPosModule->PlayerReturnLandPosi(pKernel, self);
			}
		}

		break;
	case CS_WORLD_BOSS_ACTIVE_QUERY_ACTIVE_STATUS:
		{
			m_pWorldBossNpc->OnQueryWorldBossState(pKernel, self);
		}
		break;
	case CS_WORLD_BOSS_ACTIVE_QUERY_BASE_INFO:
		{
			m_pWorldBossNpc->OnQueryBaseInfo(pKernel, self);
		}
		break;
	}

	return 0;
}

/*!
* @brief	玩家上线检查是否有世界BOSS活动开启 
* @param    引擎指针
* @param    玩家对象
* @param    sender
* @param    携带参数
* @return	函数执行返回int
*/
int WorldBossNpc::OnPlayerReady(IKernel* pKernel, const PERSISTID& self,const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	bool first = args.BoolVal(0);

	if (first)
	{
		m_pWorldBossNpc->OnQueryWorldBossState(pKernel, self);
	}
	OnPlayerContinue(pKernel, self, sender, args);
	return 0;
}

// 玩家断线重连
int WorldBossNpc::OnPlayerContinue(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	return 0;
}

/*!
* @brief	玩家断线重连检查是否有世界BOSS活动开启 
* @param    引擎指针
* @param    玩家对象
* @param    sender
* @param    携带参数
* @return	函数执行返回int
*/
int WorldBossNpc::OnCustomReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	m_pWorldBossNpc->OnQueryWorldBossState(pKernel, self);

	return 0;
}

/*!
* @brief	玩家数据恢复
* @param    引擎指针
* @param    玩家对象
* @param    sender
* @param    args
* @return   返回int
*/
int WorldBossNpc::OnRecover(IKernel* pKernel, const PERSISTID& self,
							const PERSISTID& sender, const IVarList& args) 
{
	return 0;
}

// 查询世界boss活动状态
void WorldBossNpc::OnQueryWorldBossState(IKernel* pKernel, const PERSISTID& self)
{
	// 取出玩家对应的活动场景
	int nActiveID = QueryCurActivity(pKernel);
	WorldBossActive_t* pActiveInfo = m_pWorldBossNpc->GetActiveInfoCfg(pKernel, nActiveID);
	if (NULL == pActiveInfo)
	{
		return;
	}
	// 活动剩余时间
	int iActiveTime = GetRemainTime(pActiveInfo);

	// 发送消息给客户端，通知活动开启
	CVarList TransMsg;
	TransMsg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE
		<< SC_WORLD_BOSS_ACTIVE_OPEN
		<< iActiveTime;

	pKernel->Custom(self, TransMsg);
}

// 回复世界boss基本数据查询
int WorldBossNpc::OnQueryBaseInfo(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IPubData* pWBPubData = GetPubData(pKernel);
	if (NULL == pWBPubData)
	{
		return 0;
	}
	IRecord* pKillerRec = pWBPubData->GetRecord(PUB_KILL_WORLD_BOSS_PLAYER_NAME_REC);
	if (NULL == pKillerRec)
	{
		return 0;
	}
	int nRows = pKillerRec->GetRows();

	CVarList msg;
	msg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE
		<< SC_WORLD_BOSS_ACTIVE_BASE_INFO
		<< nRows;

	LoopBeginCheck(a);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(a);
		const wchar_t* wsName = pKillerRec->QueryWideStr(i, KWBP_PUB_COL_KILLER_NAME);
		int nSceneId = pKillerRec->QueryInt(i, KWBP_PUB_COL_SCENE_ID);
		int nBossState = pKillerRec->QueryInt(i, KWBP_PUB_COL_BOSS_STATE);
		msg << nSceneId << wsName << nBossState;
	}

	pKernel->Custom(self, msg);
	return 0;
}

/*!
* @brief	根据玩家查找是否有年兽BOSS活动开启
* @param    引擎指针
* @param    玩家对象
* @return	函数执行返回bool
*/
int WorldBossNpc::QueryBossActiveType()
{
	return 1;// m_pWorldBossNpc->GetModel();
}

/*!
* @brief	根据玩家国籍找出活动场景 
* @param    引擎指针
* @param    玩家对象
* @return	函数执行返回int
*/
int WorldBossNpc::GetActiveSceneByPlayer(IKernel* pKernel, const PERSISTID& self)
{
	// 保护
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 目前活动时只有一个场景,双方阵营的人都可以进
	// 玩家国籍
	int iActiveSceneID = 0;
	std::vector<int>& vecScope = m_SceneScopeVec;
	int iSize = (int)vecScope.size();
	LoopBeginCheck(ab);
	for (int i = 0; i < iSize; ++i)
	{
		LoopDoCheck(ab);
		iActiveSceneID = vecScope[i];
// 		int iEqualNation = GetNationTypeBySceneId(iActiveSceneID);
// 		if (iEqualNation == iNation)
// 		{
// 			return iActiveSceneID;
// 		}
	}

	return iActiveSceneID;
}


/*!
* @brief	广播消息，30秒一次
* @param	[in] pKernel 引擎指针
* @param	[in] self 场景
* @param	[in] slice
* @return	int 逻辑执行结果
*/
int WorldBossNpc::HB_CheckNotice(IKernel* pKernel, const PERSISTID& self, int slice)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_53, CVarList());
	return 0;
}


/*!
* @brief	活动结束清除数据
* @param	[in] pKernel 引擎指针
* @return	bool 逻辑执行结果 
*/
bool WorldBossNpc::ClearActiveData(IKernel* pKernel)
{
	// 清空伤害统计表数据
	PERSISTID CurrScene = pKernel->GetScene();
	IGameObj* pScene = pKernel->GetGameObj(CurrScene);
	if (NULL == pScene)
	{
		return 0;
	}

	// 重置GM属性(0:正常  1:GM),默认不开启GM
	pScene->SetDataInt(FLAG_WORLD_BOSS_GM, 0);

	// 重置最后一击名字（默认""）
	pScene->SetDataWideStr("Last_Harm", L"");

	// 重置Boss配置ID(默认"")
	pScene->SetDataString("BOSS_CONFIG_ID", "");
	return true;
}

/*!
* @brief	最后一刀奖 
* @param	[in] pKernel 引擎指针
* @param	[in] killer 最后一击玩家
* @param	[in] wsName  玩家名字
* @return	int 逻辑执行结果
*/
int WorldBossNpc::ActiveLastDamageAward(IKernel* pKernel, 
										const PERSISTID& boss, 
										const PERSISTID& killer)
{
	// 判断场景
	IGameObj* pScene = pKernel->GetGameObj(pKernel->GetScene());
	if (NULL == pScene)
	{
		return 0;
	}

	//奖励物品
	const char* cBossID = pScene->QueryDataString("BOSS_CONFIG_ID");
	if (StringUtil::CharIsNull(cBossID))
	{
		return 0;
	}

	WorldBossAward_t* pBossAward =  GetBossAward(pKernel, cBossID);
	if (NULL == pBossAward)
	{
		extend_warning(LOG_ERROR, "[WorldBossNpc::ActiveLastDamageAward] not find last damage award!");
		return 0;
	}

	// 玩家名字
	const wchar_t* killerName = pKernel->QueryWideStr(killer, "Name");
	if (StringUtil::CharIsNull(killerName))
	{
		extend_warning(LOG_ERROR, "[WorldBossNpc::ActiveLastDamageAward] not find palyer name!");
		return 0;
	}

	// 邮件发放奖励
	bool result = m_pRewardModule->RewardViaMail(pKernel, killerName, pBossAward->m_nLastHurtAward, CVarList());
	if (result)
	{
		// BOSS名字
		const char* npcName = pKernel->QueryString(boss, "Config");
		if (StringUtil::CharIsNull(npcName))
		{
			extend_warning(LOG_ERROR, "[WorldBossNpc::ActiveLastDamageAward] not find boss configID!");
			npcName = "";
		}

		CVarList LastKillMsg;
		LastKillMsg << killerName 
					<< npcName 
					<< pBossAward->m_nLastHurtAward;

		CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_54, LastKillMsg);
	}
	return 0;
}

/*!
* @brief	排名奖
* @param	[in] pKernel 引擎指针
* @param	[in] CurrScene 场景
* @param	[in] boss 
* @return	int 逻辑执行结果
*/
int WorldBossNpc::ActiveRankAward(IKernel* pKernel, const PERSISTID& CurrScene, int nResult)
{
	// 保护
	IGameObj* pScene = pKernel->GetGameObj(CurrScene);
	if (NULL == pScene)
	{
		return 0;
	}

	//奖励物品
	const char* cBossID = pScene->QueryDataString("BOSS_CONFIG_ID");
	WorldBossAward_t* pBossAward = GetBossAward(pKernel, cBossID);
	if (NULL == pBossAward)
	{
		return 0;
	}

	// 伤害表格数据
	IRecord* pWBossDamRecord = pScene->GetRecord(WORLD_BOSS_DAMAGE_REC);
	if (NULL == pWBossDamRecord) 
	{
		return 0;
	}

	int iRowCount = pWBossDamRecord->GetRows();
	std::vector<RankAward_t>& vecRankAward = pBossAward->m_vecRankAward;// nResult == SUC_KILL_WORLD_BOSS ? : pBossAward->m_FailRankAward;
	int iRankVecCount = (int)vecRankAward.size();
	if (iRowCount < 1 || iRankVecCount < 1)
	{
		return 0;
	}
	LoopBeginCheck(y);
	for (int k = 0; k < iRankVecCount; ++k)
	{
		LoopDoCheck(y);
		RankAward_t& tRankAward = vecRankAward[k];
		LoopBeginCheck(z);
		for (int i = 0; i < iRowCount; ++i)
		{
			LoopDoCheck(z);
			int iRank = pWBossDamRecord->QueryInt(i, WBD_COL_PLAYER_RANK);
			if (iRank >= tRankAward.m_MinRank && iRank <= tRankAward.m_MaxRank)
			{
				const wchar_t* play_name = pWBossDamRecord->QueryWideStr(i, WBD_COL_PLAYER_NAME);
				if (StringUtil::CharIsNull(play_name))
				{
					continue;
				}

				int nRewardId = nResult == SUC_KILL_WORLD_BOSS ? tRankAward.m_nWinRewardId : tRankAward.m_nFailRewardId;
				// 玩家对象是否在当前场景存在
				PERSISTID& Attacker = pKernel->FindPlayer(play_name);
				if (pKernel->Exists(Attacker))
				{

					// 结算界面排行奖品信息
					if (!pKernel->FindData(Attacker, "TempRankAward"))
					{
						ADD_DATA(pKernel, Attacker, "TempRankAward", VTYPE_INT);
						pKernel->SetDataInt(Attacker, "TempRankAward", nRewardId);
					}
// 
					// 玩法日志(记录玩家排行名次)
					GamePlayerActionLog log;
					log.actionType = LOG_GAME_ACTION_WORLDBOSS;
					log.actionState = iRank;
					LogModule::m_pLogModule->SaveGameActionLog(pKernel, Attacker, log);
				}

				m_pRewardModule->RewardViaMail(pKernel, play_name, nRewardId, CVarList() << iRank << cBossID);
			}
		}
	}

	return 0;
}

/*!
* @brief	物品掉落
* @param	[*] pKernel 引擎指针
* @param	[Object] Attacker 攻击者（玩家）
* @param	[Object] boss 
* @param	[string] strItemList 奖励信息
* @return	int 逻辑执行结果
*/
int WorldBossNpc::DropItems(IKernel* pKernel,const PERSISTID& Attacker, const PERSISTID& boss, const std::string& strItemList)
{
	// 保护
	if (strItemList.empty())
	{
		return 0;
	}

	// 掉落组装参数定义
	CVarList DropInfoList;

	// 掉落位置的对象
	DropInfoList << Attacker;

	// 掉落件数
	CVarList ItemList;
	util_split_string(ItemList, strItemList, ",");
	int iItemCount = (int)ItemList.GetCount();
	DropInfoList << iItemCount;

	// 具体掉落物品信息
	LoopBeginCheck(cc);
	for (int i = 0; i < iItemCount; ++i)
	{
		LoopDoCheck(cc);
		const char* strItem = ItemList.StringVal(i);
		if (StringUtil::CharIsNull(strItem))
		{
			continue;
		}

		CVarList ItemInfo;
		util_split_string(ItemInfo, strItem, ":");
		DropInfoList << ItemInfo.StringVal(0) 
			<< ItemInfo.IntVal(1);
	}

	m_pDropModule->PlayerDropItems(pKernel, Attacker, DropInfoList);

	return 0;
}

/*!
* @brief	结算界面
* @param	[*] pKernel 引擎指针
* @param	[Object] CurrScene 场景
* @param	[int] slice
* @return	int 逻辑执行结果
*/
int WorldBossNpc::HB_SendCustomMsg(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 保护
	IGameObj* pScene = pKernel->GetGameObj(self);
	if (NULL == pScene)
	{
		return 0;
	}

	/*!
	* @brief	结算界面
	* @param	wstring	击杀boss玩家名字
	* @param	int			我的排名
	* @param	int			我的伤害
	* @param	int		前n名
	以下循环n次
	* @param	wstring 玩家名字
	* @param	int		玩家名次
	* @param	int		玩家阵营
	* @param	int		玩家伤害值
	*/
//	SC_WORLD_BOSS_ACTIVE_ACTOR_AWARD,

	// 最后一击名字
	const wchar_t* wstrLastHarm = pScene->QueryDataWideStr("Last_Harm");
	if (StringUtil::CharIsNull(wstrLastHarm))
	{
		wstrLastHarm = LAST_KILL_BOSS_NAME;
	}

	// 伤害表格数据
	IRecord* pDamRec = pScene->GetRecord(WORLD_BOSS_DAMAGE_REC);
	if (NULL == pDamRec)
	{
		return 0;
	}

	CVarList ShowMsg;
	int iRowCount = pDamRec->GetRows();
	// 向玩家推送结算界面数据
	LoopBeginCheck(ee);
	for (int i = 0; i < iRowCount; ++i)
	{
		LoopDoCheck(ee);
		const wchar_t* player_name = pDamRec->QueryWideStr(i, WBD_COL_PLAYER_NAME);
		if (StringUtil::CharIsNull(player_name))
		{
			continue;
		}

		// 玩家不在当前场景不推送结算界面
		PERSISTID& Attacker = pKernel->FindPlayer(player_name);
		IGameObj* pAttacker = pKernel->GetGameObj(Attacker);
		if (NULL == pAttacker)
		{
			continue;
		}

		int iHurt = pDamRec->QueryInt(i, WBD_COL_PLAYER_TOTAL_DAMAGE);

		// 通知成就模块更新世界boss相关成就[2015.08.03 tongzt]
		int iRank = i + 1;	// 排名

		ShowMsg.Clear();
		ShowMsg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE
			<< SC_WORLD_BOSS_ACTIVE_ACTOR_AWARD
//			<< wstrLastHarm
			<< iRank
			<< iHurt;
/*			<< ranklist;*/

		// 查询临时属性
		if (!pKernel->FindData(Attacker, "TempRankAward"))
		{ 
			continue;
		}
		const char* strRankAward = pAttacker->QueryDataString("TempRankAward");
		if (StringUtil::CharIsNull(strRankAward))
		{
			continue;
		}

		ShowMsg << strRankAward;

		SafeCustom(pKernel, Attacker, ShowMsg);

		REMOVE_DATA(pKernel, Attacker, "TempRankAward");
	}

	// 清理数据和删除BOSS存活标识
	m_pWorldBossNpc->ClearActiveData(pKernel);

	// 离开当前场景
	ADD_COUNT_BEAT(pKernel, self, "WorldBossNpc::HB_LeaveScene", m_kConstConfig.nLeaveSceneDelay, 1);
	return 0;
}

/*!
* @brief	通知玩家活动结束
* @param	[*] pKernel 引擎指针
* @param	[PERSISTID] 场景
* @return	int 执行返回
*/
int WorldBossNpc::NoticeActiveFinish(IKernel* pKernel, const PERSISTID& self, bool bossBeKilled)
{
	// BOSS未被打死,广播活动结束
	if (!bossBeKilled)
	{
		CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_52, CVarList());
	}

	CVarList FinishMsg;
	FinishMsg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE 
		      << SC_WORLD_BOSS_ACTIVE_CLOSE;

	pKernel->CustomByWorld(FinishMsg);

	// 清除场景内的npc
	ClearSceneNpc(pKernel);

	// 删除实时伤害显示心跳
	DELETE_HEART_BEAT(pKernel, pKernel->GetScene(), "WorldBossNpc::HB_ShowAttackRank");

	ADD_COUNT_BEAT(pKernel, pKernel->GetScene(), "WorldBossNpc::HB_SendCustomMsg", 1500, 1);
	return 0;
}

/*!
* @brief	算出活动剩余时间
* @param	场景ID
* @return	int 
*/
int WorldBossNpc::GetRemainTime(const WorldBossActive_t* pActiveInfo)
{
	if (NULL == pActiveInfo)
	{
		return 0;
	}

	int daySecond = util_convert_zerotime_to_sec();

	int iTime = 0;

	// 活动还未开始
	if (m_ActiveTimeCount.m_BeginTime >= daySecond)
	{
		iTime = m_ActiveTimeCount.m_EndTime - m_ActiveTimeCount.m_BeginTime;
	}
	// 活动已经开始
	else if (m_ActiveTimeCount.m_BeginTime < daySecond && m_ActiveTimeCount.m_EndTime > daySecond)
	{
		iTime = m_ActiveTimeCount.m_EndTime - daySecond;
	}
	// 活动结束
	else
	{
		iTime = 0;
	}

	return iTime;
}

// 查询当前的活动
int WorldBossNpc::QueryCurActivity(IKernel* pKernel)
{
	IPubData* pWorldBossPubData = GetPubData(pKernel);
	if (NULL == pWorldBossPubData)
	{
		return 0;
	}

	int nCurActivityId = WBAT_NONE_WORLD_BOSS;
	// 统一时间段只能有一种活动在进行中
	IRecord* pRecord = pWorldBossPubData->GetRecord(WORLD_BOSS_ACTIVE_STATUS_REC);
	if (NULL == pRecord)
	{
		return false;
	}
	int nRows = pRecord->GetRows();
	LoopBeginCheck(t);
	for (int i = 0; i < nRows;++i)
	{
		LoopDoCheck(t);
		int nState = pRecord->QueryInt(i, WORLD_BOSS_STATUS_REC_COL_STATUS);
		if (nState == STATE_WBOSS_ACTIVE_OPEN)
		{
			nCurActivityId = pRecord->QueryInt(i, WORLD_BOSS_STATUS_REC_COL_ID);
			break;
		}
	}
	
	return nCurActivityId;
}

// 清除场景内的所有npc
void WorldBossNpc::ClearSceneNpc(IKernel* pKernel)
{
	CVarList Result;
	pKernel->GetChildList(pKernel->GetScene(), TYPE_NPC, Result);

	int iCount = (int)Result.GetCount();
	if (iCount == 0)
	{
		return;
	}

	LoopBeginCheck(u);
	for (int i = 0; i < iCount; ++i)
	{
		LoopDoCheck(u);
		PERSISTID npc = Result.ObjectVal(i);
		pKernel->DestroySelf(npc);
	}
}

void WorldBossNpc::ReloadConfig(IKernel* pKernel)
{
	// 加载配置
	LoadAwardResource(pKernel);
	LoadActiveInfoResource(pKernel);
	LoadActiveRuleResource(pKernel);
	m_pWorldBossNpc->LoadWorldBossGrowUpConfig(pKernel);
}

// 判定世界boss场景是否可进入
bool WorldBossNpc::IsSceneEnter(IKernel* pKernel, const PERSISTID& self, int nSceneId)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}
	WorldBossActive_t* pActiveInfo = m_pWorldBossNpc->GetActiveInfoCfgBySceneId(pKernel, nSceneId);
	if (NULL == pActiveInfo) // 不是世界boss场景 不需要验证
	{
		return true;
	}
	bool bEnter = true;
	do 
	{
		// 当前是否有场景正在活动
		int nStatus = m_pWorldBossNpc->GetActiveStatusFromPUB(pKernel, nSceneId);
		if (nStatus != STATE_WBOSS_ACTIVE_OPEN)
		{
			// 系统提示活动关闭不能进入
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_52, CVarList());

			bEnter = false;
			break;
		}

		// 判断等级条件
		int iLevel = pSelf->QueryInt("Level");
		if (iLevel < pActiveInfo->m_PlayerLevel)
		{
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_51, CVarList() << pActiveInfo->m_PlayerLevel);
			bEnter = false;
		}
	} while (0);

	return bEnter;
}

/*!
* @brief	进入场景之后
* @param	pKernel 引擎指针
* @param	玩家
* @param	
* @param    参数
* @return	int 
*/
int WorldBossNpc::OnPlayerEntryScence(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 玩家对象
	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	// 玩家正在进入的场景
	int iEntrySceneID = pKernel->GetSceneId();

	std::vector<int>::const_iterator it_beg = m_SceneScopeVec.begin();
	std::vector<int>::const_iterator it_end = m_SceneScopeVec.end();
	std::vector<int>::const_iterator it = find(it_beg, it_end, iEntrySceneID);
	if (it == it_end)
	{
		return 0;
	}

	int iState = m_pWorldBossNpc->GetActiveStatusFromPUB(pKernel, iEntrySceneID);
	if (iState == STATE_WBOSS_ACTIVE_CLOSE)
	{
		// 玩家进入活动场景,活动结束拉回上一个场景
		m_pLandPosModule->PlayerReturnLandPosi(pKernel, self);
	}

	return 0;
}

// 开始播放CG
int WorldBossNpc::HB_StartPlayCG(IKernel* pKernel, const PERSISTID& self, int slice)
{
	int nSceneId = pKernel->GetSceneId();

	pKernel->CustomByScene(nSceneId, CVarList() << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE << SC_WORLD_BOSS_START_PLAY_CG);
	return 0;
}

// 离开场景心跳
int WorldBossNpc::HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 保护
	IGameObj* pScene = pKernel->GetGameObj(self);
	if (NULL == pScene)
	{
		return 0;
	}

	// 通知所有在场景的玩家离开
	CVarList Result;
	pKernel->GetChildList(self, TYPE_PLAYER, Result);

	int iCount = (int)Result.GetCount();
	if (iCount == 0)
	{
		return 0;
	}

	LoopBeginCheck(u);
	// 返回进来时的场景
	for (int i = 0; i < iCount; ++i)
	{
		LoopDoCheck(u);
		PERSISTID _player = Result.ObjectVal(i);
		if (pKernel->Exists(_player))
		{
			m_pLandPosModule->PlayerReturnLandPosi(pKernel, _player);
		}
	}
	return 0;
}

// 开始刷Boss
int WorldBossNpc::HB_BossBorn(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSceneObj = pKernel->GetGameObj(self);
	if (NULL == pSceneObj)
	{
		return 0;
	}
	// 获取当前的时间
	time_t tNow = ::time(NULL);
	tm* pNow = ::localtime(&tNow);
	if(NULL == pNow)
	{
		return 0;
	}

	// 获取当前配置索引
	if (!pSceneObj->FindData(FLAG_WORLD_BOSS_CONFIG_DATA_INDEX))
	{
		return 0;
	}

	int nConfigIndex = pSceneObj->QueryDataInt(FLAG_WORLD_BOSS_CONFIG_DATA_INDEX);

	// 找到配置数据
	std::vector<WorldBossActive_t>& vecActiveInfo = m_vecActiveInfo;
	if (nConfigIndex < 0 || nConfigIndex >= (int)vecActiveInfo.size())
	{
		return 0;
	}

	WorldBossActive_t* pActiveInfo = &vecActiveInfo[nConfigIndex];
	if (NULL == pActiveInfo)
	{
		return 0;
	}

	int iActiveTime = GetRemainTime(pActiveInfo);

	// 创建BOSS
	std::string strBossPos;
	PERSISTID WorldBoss = CreateWorldBoss(pKernel, pActiveInfo, 
		pKernel->GetSceneId(), strBossPos, iActiveTime, pNow->tm_wday);
	if(WorldBoss.IsNull()) 
	{
		return 0;
	}

	// 设置活动标识
	pSceneObj->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_BORNED);

	// 场景上临时存储BOSS对象ID
	if (pSceneObj->FindData(WORLD_BOSS_ACTIVED))
	{
		pSceneObj->SetDataObject(WORLD_BOSS_ACTIVED, WorldBoss);
	}

	return 0;
}

/*!
* @brief	监听世界BOSS被伤害
* @param	引擎指针
* @param	self
* @param	sender
* @param	args
* @return	int 
*/
int WorldBossNpc::OnCommandBossBeDamaged(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// BOSS
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 宠物的话,找到它的主人
	PERSISTID damager = get_pet_master(pKernel, sender);

	// 攻击者
	IGameObj* pSender = pKernel->GetGameObj(damager);
	if (NULL == pSender)
	{
		return 0;
	}

	// 攻击者名字
	const wchar_t* wsPlayerName = pSender->QueryWideStr(FIELD_PROP_NAME);
	if (StringUtil::CharIsNull(wsPlayerName))
	{
		return 0;
	}

	//攻击者对BOSS伤害输出
	int damage = args.IntVal(1);

	IRecord* pAttackerList = pSelf->GetRecord(WORLDBOSS_ATTACK_LIST_REC);
	if(NULL == pAttackerList)
	{
		return 0;
	}

	//将攻击对象加入到表里
	int row = pAttackerList->FindWideStr(WORLDBOSS_ATTACK_LIST_REC_PLAYER_NAME, wsPlayerName);
	if (row == -1)
	{
		CVarList values;
		values	<< wsPlayerName 
				<< damage;
		pAttackerList->AddRowValue(-1, values);

	}
	else
	{
		int sum = pAttackerList->QueryInt(row, WORLDBOSS_ATTACK_LIST_REC_DAMAGE);
		sum = sum + damage;
		pAttackerList->SetInt(row, WORLDBOSS_ATTACK_LIST_REC_DAMAGE, sum);
	}

	return 0;
}


/////////////////////////////////公共服务数据逻辑/////////////////////////////////////////

/*!
* @brief	获取空间数据域指针
* @param	引擎指针
* @return	int 
*/
IPubData* WorldBossNpc::GetPubData(IKernel* pKernel)
{
	IPubSpace* pDomainSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pDomainSpace)
	{
		return NULL;
	}

	return pDomainSpace->GetPubData(GetDomainName(pKernel).c_str());
}


/*!
* @brief	获取空间名
* @param	引擎指针
* @return	int 
*/
const std::wstring& WorldBossNpc::GetDomainName(IKernel* pKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_WorldBoss_%d", server_id);

		m_domainName = wstr;
	}

	return m_domainName;
}

/*!
* @brief	去公共服务器查找活动状态
* @param	引擎指针
* @param	场景ID
* @return	int 
*/
int WorldBossNpc::GetActiveStatusFromPUB(IKernel* pKernel, int iSceneID)
{
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return 0;
	}

	// 公共数据指针
	IPubData* pPAData = pPubSpace->GetPubData(WorldBossNpc::m_pWorldBossNpc->GetDomainName(pKernel).c_str());
	if (NULL == pPAData)
	{
		return 0;
	}

	// 备份表
	IRecord* pRecord = pPAData->GetRecord(WORLD_BOSS_ACTIVE_STATUS_REC);
	if (NULL == pRecord)
	{
		return 0;
	}

	// 查找对应的场景活动状态
	const int iRow = pRecord->FindInt(WORLD_BOSS_STATUS_REC_COL_SCENEID, iSceneID);
	if (iRow != -1)
	{
		return pRecord->QueryInt(iRow, WORLD_BOSS_STATUS_REC_COL_STATUS);
	}

	return 0;
}

int WorldBossNpc::GetActiveCfgIDFromPUB(IKernel* pKernel, int iSceneID)
{
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return 0;
	}

	// 公共数据指针
	IPubData* pPAData = pPubSpace->GetPubData(WorldBossNpc::m_pWorldBossNpc->GetDomainName(pKernel).c_str());
	if (NULL == pPAData)
	{
		return 0;
	}

	// 备份表
	IRecord* pRecord = pPAData->GetRecord(WORLD_BOSS_ACTIVE_STATUS_REC);
	if (NULL == pRecord)
	{
		return 0;
	}

	// 查找对应的场景活动状态
	const int iRow = pRecord->FindInt(WORLD_BOSS_STATUS_REC_COL_SCENEID, iSceneID);
	if (iRow != -1)
	{
		return pRecord->QueryInt(iRow, WORLD_BOSS_STATUS_REC_COL_ID);
	}

	return 0;
}

// 验证是否为世界boss场景
bool WorldBossNpc::IsWorldBossScene(int iSceneID)
{
	if (NULL == m_pWorldBossNpc)
	{
		return false;
	}
	std::vector<int>::iterator iter = find(m_pWorldBossNpc->m_SceneScopeVec.begin(), m_pWorldBossNpc->m_SceneScopeVec.end(), iSceneID);
	return iter != m_pWorldBossNpc->m_SceneScopeVec.end();
}

/*!
* @brief	发消息给Pub服务器
* @param	引擎指针
* @return	int 
*/
bool WorldBossNpc::SendMsgToPubServer(IKernel* pKernel, const IVarList& msg)
{
	CVarList PubMsg;
	PubMsg << PUBSPACE_DOMAIN 
		<< GetDomainName(pKernel).c_str()
		<< msg;

	return pKernel->SendPublicMessage(PubMsg);
}

/*!
* @brief	设置活动状态
* @param	引擎指针
* @return	int 
*/
int WorldBossNpc::OnSaveActiveStatusToPUB(IKernel* pKernel, const int iSceneID, const int iStatus, const int iID)
{
	CVarList PubMsg;
	PubMsg << SP_WORLD_BOSS_MSG_STATUS
		<< SP_WORLD_BOSS_ACTIVE_STATUS_SET
		<< iSceneID 
		<< iStatus
		<< iID;

	return SendMsgToPubServer(pKernel, PubMsg);
}

/*!
* @brief	添加临时属性
* @param	引擎指针
* @param	Scene
* @return	void 
*/
void WorldBossNpc::AddSceneData(IKernel* pKernel, const PERSISTID& Scene)
{
	IGameObj* pScene = pKernel->GetGameObj(Scene);
	if (NULL == pScene)
	{
		return;
	}
	// 设置GM属性(0:正常  1:GM),默认不开启GM
	ADD_DATA(pKernel, Scene, FLAG_WORLD_BOSS_GM, VTYPE_INT);
	pScene->SetDataInt(FLAG_WORLD_BOSS_GM, 0);

	// 状态标识,默认关闭
	//ADD_DATA(pKernel, Scene, FLAG_WORLD_BOSS_VISIT, VTYPE_INT);
	pScene->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_CLOSED);

	// 设置活动配置ID(默认-1)
	ADD_DATA(pKernel, Scene, WORLD_BOSS_ACTIVE_CFG_ID, VTYPE_INT);
	pScene->SetDataInt(WORLD_BOSS_ACTIVE_CFG_ID, -1);

	// 最后一击名字（默认""）
	ADD_DATA(pKernel, Scene, "Last_Harm", VTYPE_WIDESTR);
	pScene->SetDataWideStr("Last_Harm", L"");

	// Boss配置ID(默认"")
	ADD_DATA(pKernel, Scene, "BOSS_CONFIG_ID", VTYPE_STRING);
	pScene->SetDataString("BOSS_CONFIG_ID", "");

	// 添加BOSS临时对象，销毁用
	ADD_DATA(pKernel, Scene, WORLD_BOSS_ACTIVED, VTYPE_OBJECT);  
	pScene->SetDataObject(WORLD_BOSS_ACTIVED, PERSISTID());
}

/*!
* @brief	member数据同步
* @param	引擎指针
* @return	void 
*/
void WorldBossNpc::SyncMemberData(IKernel* pKernel)
{
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace != NULL)
	{
		wchar_t pubDataName[256];
		SWPRINTF_S(pubDataName, L"Domain_SceneList%d", pKernel->GetServerId());
		IPubData* pPubData = pPubSpace->GetPubData(pubDataName);
		if (pPubData != NULL)
		{
			IRecord* pRecord = pPubData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
			if (pRecord != NULL)
			{
				// 分发命令
				CVarList UpdateMsg;
				UpdateMsg << COMMAND_WORLD_BOSS_ACTIVE
						  << SS_WORLD_BOSS_ACTIVE_UPDATE_DATA
						  << m_ActiveTimeCount.m_BeginTime
						  << m_ActiveTimeCount.m_EndTime;

				int iRows = pRecord->GetRows();
				LoopBeginCheck(ax);
				for(int i = 0; i < iRows; ++i)
				{
					LoopDoCheck(ax);

					pKernel->CommandToScene(pRecord->QueryInt(i, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id), UpdateMsg);
				}
			}
			else
			{
				extend_warning(LOG_ERROR, "[WorldBossNpc::HB_CheckWorldBossActive] pRecord is null");
			}	
		}
		else
		{
			extend_warning(LOG_ERROR, "[WorldBossNpc::HB_CheckWorldBossActive] pPubData is null");
		}
	}
	else
	{
		extend_warning(LOG_ERROR, "[WorldBossNpc::HB_CheckWorldBossActive] pPubSpace is null");
	}
}

/*!
* @brief	活动广播
* @param	引擎指针
* @param	Scene
* @return	void 
*/
void WorldBossNpc::ActiveNotice(IKernel* pKernel, const PERSISTID& Scene)
{
	IGameObj* pScene = pKernel->GetGameObj(Scene);
	if (NULL == pScene)
	{
		return;
	}
	// 设置公告标识
	pScene->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_NOTICED);

	// 只在一个场景增加活动开始提示心跳
	if (pKernel->GetSceneId() != m_nCustomSceneId)
	{
		return;
	}

	// 计算心跳次数
	int iHBCount = 0;
	int iDistTime = 0;

	int daySecond = util_convert_zerotime_to_sec();
	if (m_NoticeTimeCount.m_BeginTime >= daySecond)
	{
		iDistTime = m_NoticeTimeCount.m_EndTime - m_NoticeTimeCount.m_BeginTime;
	}
	else if (m_NoticeTimeCount.m_BeginTime < daySecond && m_NoticeTimeCount.m_EndTime > daySecond)
	{
		iDistTime = m_NoticeTimeCount.m_EndTime - daySecond;
	}
	iHBCount = iDistTime > 0 ? (iDistTime / 15) : 0;

	// 活动公告
	if (iHBCount > 0)
	{
		ADD_COUNT_BEAT(pKernel, Scene, "WorldBossNpc::HB_CheckNotice", 15000, iHBCount);
	}
}

/*!
* @brief	活动开始
* @param	引擎指针
* @param	Scene
* @param	pActiveInfo
* @param	pNow
* @param	iState
* @return	void 
*/
void WorldBossNpc::ActiveOpening(IKernel* pKernel, const PERSISTID& Scene, const WorldBossActive_t* pActiveInfo, const tm* pNow, int iState, int nConfigIndex)
{
	// 保护
	IGameObj* pScene = pKernel->GetGameObj(Scene);
	if (NULL == pScene)
	{
		return;
	}

	// 设置活动标识
	pScene->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_ACTIVED);

	if(!pScene->FindData(FLAG_WORLD_BOSS_CONFIG_DATA_INDEX))
	{
		pScene->AddDataInt(FLAG_WORLD_BOSS_CONFIG_DATA_INDEX, nConfigIndex);
	}
	else
	{
		pScene->SetDataInt(FLAG_WORLD_BOSS_CONFIG_DATA_INDEX, nConfigIndex);
	}

	// 删除公告心跳
	DELETE_HEART_BEAT(pKernel, Scene, "WorldBossNpc::HB_CheckNotice");

	// 活动状态保存到PUB服务
	m_pWorldBossNpc->OnSaveActiveStatusToPUB(pKernel, pKernel->GetSceneId(), 
		iState, pScene->QueryDataInt(WORLD_BOSS_ACTIVE_CFG_ID));

	// 活动剩余时间
	int iActiveTime = GetRemainTime(pActiveInfo);

	// 增加刷boss心跳
	ADD_COUNT_BEAT(pKernel, Scene, "WorldBossNpc::HB_BossBorn", m_kConstConfig.nReadyTime, 1);

	// 增加播放CG的心跳
	ADD_COUNT_BEAT(pKernel, Scene, "WorldBossNpc::HB_StartPlayCG", m_kConstConfig.nBornCGTime, 1);

	ADD_HEART_BEAT(pKernel, Scene, "WorldBossNpc::HB_ShowAttackRank", 5000);

	CVarList PubMsg;
	PubMsg << SP_WORLD_BOSS_MSG_STATUS
		<< SP_UPDATE_BOSS_REC_INFO
		<< pKernel->GetSceneId()
		<< L""
		<< STATE_WBOSS_ACTIVE_OPEN;
	 
	SendMsgToPubServer(pKernel, PubMsg);

	if (pKernel->GetSceneId() != m_nCustomSceneId)
	{
		return;
	} 
	// 发送消息给客户端，通知活动开启
	CVarList TransMsg;
	TransMsg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE 
			 << SC_WORLD_BOSS_ACTIVE_OPEN 
			 << iActiveTime;
	pKernel->CustomByWorld(TransMsg);

	// 发送消息给客户端，显示活动进入窗口
	TransMsg.Clear();
	TransMsg << SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE
			 << SC_WORLD_BOSS_ACTIVE_SHOW_ENTRY_PANEL;
	pKernel->CustomByWorld(TransMsg);
}

/*!
* @brief	活动结束
* @param	引擎指针
* @param	Scene
* @param	iState
* @return	void 
*/
void WorldBossNpc::ActiveClosed(IKernel* pKernel, const PERSISTID& Scene, int iState)
{
	IGameObj* pScene = pKernel->GetGameObj(Scene);
	if (NULL == pScene)
	{
		return;
	}

	// 设置结束标识
	pScene->SetInt(FLAG_WORLD_BOSS_VISIT, FLAG_WORLD_BOSS_VISIT_CLOSED);

	// 活动状态保存到PUB服务
	m_pWorldBossNpc->OnSaveActiveStatusToPUB(pKernel, pKernel->GetSceneId(), 
		iState, pScene->QueryDataInt(WORLD_BOSS_ACTIVE_CFG_ID));

	// BOSS对象
	PERSISTID WorldBoss = pScene->QueryDataObject(WORLD_BOSS_ACTIVED);
	// WorldBoss为Null 说明boss已经被杀死结算过了
	bool bKilled = WorldBoss.IsNull();
	if (!bKilled)
	{
		// 伤害数据拷贝
		m_pWorldBossNpc->CopyAndSortHurtRecordFromAI(pKernel, WorldBoss, PERSISTID());

		// 排名奖励
		m_pWorldBossNpc->ActiveRankAward(pKernel, Scene, FAIL_KILL_WORLD_BOSS);

		// 销毁BOSS对象
		pKernel->Destroy(Scene, WorldBoss);

		// boss太难了,级别降低
		UpdateGrowUpLevel(pKernel, false);

		// 通知活动结束
		m_pWorldBossNpc->NoticeActiveFinish(pKernel, Scene, bKilled);
	}
}

// 获取世界boss的属性包 
int	WorldBossNpc::QueryCurPackageId(IKernel* pKernel)
{
	IPubData* pWBPubData = GetPubData(pKernel);
	if (NULL == pWBPubData)
	{
		return 0;
	}
	IRecord* pGrowUpRec = pWBPubData->GetRecord(PUB_WORLD_BOSS_GROW_UP_REC);
	if (NULL == pGrowUpRec)
	{
		return 0;
	}
	int nSceneId = pKernel->GetSceneId();

	int nCurLevel = 0;
	int nRowIndex = pGrowUpRec->FindInt(WORLD_BOSS_GROW_UP_REC_COL_SCENEID, nSceneId);
	if (nRowIndex != -1)
	{
		nCurLevel = pGrowUpRec->QueryInt(nRowIndex, WORLD_BOSS_GROW_UP_REC_COL_LEVEL);
	}

	const WorldBossGrowUp* pGrowData = GetGrowUpData(nSceneId);
	if (NULL == pGrowData)
	{
		return 0;
	}

	// 不能超过最大等级
	nCurLevel = __min(nCurLevel, pGrowData->nMaxLevel);

	return pGrowData->nBasePackageId + nCurLevel;
}

// 获取成长配置
const WorldBossGrowUp* WorldBossNpc::GetGrowUpData(int nSceneId)
{
	WorldBossGrowUp* pFind = NULL;
	int nSize = m_vecWorldBossGrowUp.size();
	LoopBeginCheck(q);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(q);
		WorldBossGrowUp* pData = &m_vecWorldBossGrowUp[i];
		if (pData != NULL && pData->nSceneId == nSceneId)
		{
			pFind = pData;
			break;
		}
	}

	return pFind;
}

// 更新boss等级
void WorldBossNpc::UpdateGrowUpLevel(IKernel* pKernel, bool bBeKilled)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}

	// 获取成长的配置
	int nSceneId = pKernel->GetSceneId();
	const WorldBossGrowUp* pGrowData = GetGrowUpData(nSceneId);
	if (NULL == pGrowData)
	{
		return;
	}

	int nChangeLv = 0;
	if (bBeKilled)
	{
		int64_t nBornTime = pSceneObj->QueryInt64(FIELD_PROP_WORLD_BOSS_CREATE_TIME);
		int64_t nDurTime = ::util_get_utc_time() - nBornTime;
		if (nDurTime < pGrowData->nKillDurationTime)
		{
			nChangeLv = pGrowData->nIncrement;
		}
	}
	else
	{
		nChangeLv = pGrowData->nIncrement * -1;
	}
	
	// 无变化
	if (0 == nChangeLv)
	{
		return;
	}

	CVarList PubMsg;
	PubMsg << SP_WORLD_BOSS_MSG_STATUS
		<< SP_WORLD_BOSS_BE_KILL_TIME_SET
		<< pKernel->GetSceneId()
		<< nChangeLv
		<< pGrowData->nMaxLevel;

	SendMsgToPubServer(pKernel, PubMsg);
}