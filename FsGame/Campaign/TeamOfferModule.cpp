//--------------------------------------------------------------------
// 文件名:		TeamOfferModule.cpp
// 内  容:		组队悬赏模块
// 说  明:		
// 创建日期:		2017年01月13日
// 创建人:		tongzt
// 修改人:		 
//--------------------------------------------------------------------

#include "TeamOfferModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/CommonModule/ActionMutex.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/NpcBaseModule/NpcCreatorModule.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif

#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include <algorithm>
#include "TaskModule/TaskUtil.h"
#include "CommonModule/EnvirValueModule.h"
#include "SocialSystemModule/TeamModule.h"
#include "CommonModule/CommRuleModule.h"
#include "TaskModule/Data/TaskLoader.h"
#include "SystemFunctionModule/RewardModule.h"


TeamOfferModule *TeamOfferModule::m_pTeamOfferModule = NULL;
std::wstring TeamOfferModule::m_domainName = L"";

// 重载资源
inline int nx_reload_teamoffer_res(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);

	TeamOfferModule::m_pTeamOfferModule->LoadRes(pKernel);

	return 1;
}

// 发起组队悬赏
inline int nx_launch_teamoffer(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_launch_teamoffer, 1);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_launch_teamoffer, 1);
	PERSISTID player = pKernel->LuaToObject(state, 1);

	TeamOfferModule::m_pTeamOfferModule->Launch(pKernel, player);

	return 1;
}

// 进入组队悬赏副本
inline int nx_entry_group(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_entry_group, 1);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_entry_group, 1);
	PERSISTID player = pKernel->LuaToObject(state, 1);
	IGameObj *pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return 0;
	}

	TeamOfferModule::m_pTeamOfferModule->EntryGroup(pKernel, player, pPlayer->QueryInt(FIELD_PROP_TEAM_ID));

	return 1;
}


// 初始化
bool TeamOfferModule::Init(IKernel* pKernel)
{
	m_pTeamOfferModule = this;
	Assert(m_pTeamOfferModule != NULL);

	// 客户端消息
	pKernel->AddIntCustomHook(CLASS_NAME_PLAYER, CLIENT_CUSTOMMSG_TEAMOFFER, TeamOfferModule::OnCustomMsg);

	// 玩家内部消息
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_TEAMOFFER_MSG, TeamOfferModule::OnCommandMsg);

	// 刷怪器内部消息
	pKernel->AddIntCommandHook(CLASS_NAME_RANDOM_NPC_CREATOR, COMMAND_TEAMOFFER_MSG, TeamOfferModule::OnCreatorCommandMsg);

	// 上线
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnRecover", TeamOfferModule::OnPlayerRecover, -1);

	// 离开场景
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnLeaveScene", TeamOfferModule::OnPlayerLeaveScene);

	// 进入场景
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnAfterEntryScene", TeamOfferModule::OnPlayerEntry);

	// 客户端准备就绪
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnReady", TeamOfferModule::OnPlayerReady);

	// 客户端断线重连OnReady消息
	pKernel->AddIntCustomHook(CLASS_NAME_PLAYER, CLIENT_CUSTOMMSG_CONTINUE_ON_READY, TeamOfferModule::OnPlayerReady);

	// 玩家离线回调
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnStore", TeamOfferModule::OnStore);

	// 接收公共区消息
	pKernel->AddEventCallback(CLASS_NAME_SCENE, "OnPublicMessage", TeamOfferModule::OnPublicMessage);

	// 悬赏怪物死亡
	pKernel->AddIntCommandHook(CLASS_NAME_TEAM_OFFER_NPC, COMMAND_BEKILL, TeamOfferModule::OnOfferNpcBeKilled);

	// 任务进度回调
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_TASK_ACTION_MSG, TeamOfferModule::OnCommandTaskProc);

	if (!LoadRes(pKernel))
	{
		Assert(false);
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("TeamOfferModule", TeamOfferModule::ReloadTeamofferConfig);
#endif

	DECL_HEARTBEAT(TeamOfferModule::HB_TeamOfferEnd);
	DECL_HEARTBEAT(TeamOfferModule::HB_Contrl);

	DECL_CRITICAL(TeamOfferModule::C_OnTeamChange);

	//TeamModule::AddCheckTeamRule(COMMAND_TEAM_CHECK_TEAMOFFER, TeamOfferModule::CheckAddTeamoffer);

	DECL_LUA_EXT(nx_reload_teamoffer_res);
	DECL_LUA_EXT(nx_launch_teamoffer);
	DECL_LUA_EXT(nx_entry_group);

	return true;
}

// 关闭
bool TeamOfferModule::Shut(IKernel* pKernel)
{
	m_CaptainBoxVec.clear();
	m_OfferAwardRuleVec.clear();

	return true;
}

// 加载资源
bool TeamOfferModule::LoadRes(IKernel* pKernel)
{
	if (!LoadCaptainBox(pKernel))
	{
		return false;
	}

	if (!LoadAwardRule(pKernel))
	{
		return false;
	}

	return true;
}

// 加载队长宝箱配置
bool TeamOfferModule::LoadCaptainBox(IKernel* pKernel)
{
	m_CaptainBoxVec.clear();

	std::string file_path = pKernel->GetResourcePath();
	file_path += TEAM_OFFER_CAPTAIN_BOX_FILE;

	// xml读取
	CXmlFile xml(file_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = file_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	// 解析
	LoopBeginCheck(b);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(b);
		const char *sec = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec))
		{
			continue;
		}

		// 奖励编号
		const int offer_num = xml.ReadInteger(sec, "TeamofferNum", 0);

		CaptainBox box(offer_num);
		box.reward_id = xml.ReadInteger(sec, "RewardID", 0);

		m_CaptainBoxVec.push_back(box);
	}

	return true;
}

// 加载奖励规则
bool TeamOfferModule::LoadAwardRule(IKernel* pKernel)
{
	m_OfferAwardRuleVec.clear();

	std::string file_path = pKernel->GetResourcePath();
	file_path += TEAM_OFFER_AWARD_RULE_FILE;

	// xml读取
	CXmlFile xml(file_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = file_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	// 解析
	LoopBeginCheck(b);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(b);
		const char *sec = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec))
		{
			continue;
		}

		OfferAwardRule award_rule;

		award_rule.lower_num = xml.ReadInteger(sec, "LowerNum", 0);
		award_rule.lower_level = xml.ReadInteger(sec, "LowerLevel", 0);

		int upper_num = xml.ReadInteger(sec, "UpperNum", 0);
		upper_num = upper_num <= 0 ? INT_MAX : upper_num;
		award_rule.upper_num = max(award_rule.lower_num, upper_num);

		int upper_level = xml.ReadInteger(sec, "UpperLevel", 0);
		upper_level = upper_level <= 0 ? INT_MAX : upper_level;
		award_rule.upper_level = max(award_rule.lower_level, upper_level);

		award_rule.reward_id = xml.ReadInteger(sec, "RewardID", 0);

		m_OfferAwardRuleVec.push_back(award_rule);
	}

	return true;
}

// 取得队长宝箱
const int TeamOfferModule::GetCaptainBox(const int offer_num)
{

	//  查找
	CaptainBox temp(offer_num);
	CaptainBoxVec::const_iterator find_it = find(
		m_CaptainBoxVec.begin(),
		m_CaptainBoxVec.end(),
		temp);

	if (find_it != m_CaptainBoxVec.end())
	{
		return find_it->reward_id;
	}

	return 0;
}

// 取得奖励规则
const OfferAwardRule * TeamOfferModule::GetAwardRule(const int offer_num, const int level)
{
	struct AwardRuleFinder 
	{
		AwardRuleFinder(int _num, int _level) : 
			offer_num(_num), 
			level(_level)
		{
		}
		int offer_num;
		int level;

		bool operator () (const OfferAwardRule& rule) const
		{
			return (offer_num >= rule.lower_num && offer_num <= rule.upper_num)
				&& (level >= rule.lower_level && level <= rule.upper_level);
		}
	};

	OfferAwardRuleVec::const_iterator find_it = find_if(
		m_OfferAwardRuleVec.begin(),
		m_OfferAwardRuleVec.end(),
		AwardRuleFinder(offer_num, level));
	if (find_it != m_OfferAwardRuleVec.end())
	{
		return &(*find_it);
	}

	return NULL;
}

// 可否接取悬赏任务
bool TeamOfferModule::CanAcceptOfferTask(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 未开启
	if (!TaskUtilS::Instance()->IsStart(pKernel, self, TASK_TYPE_TEAMOFFER))
	{		
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_151, CVarList());
		return false;
	}

	// 已有悬赏任务
	if (TaskUtilS::Instance()->IsHaveOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_153, CVarList());
		return false;
	}

	// 次数不足
	if (!TaskUtilS::Instance()->IsHaveNumOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_152, CVarList());
		return false;
	}

	return true;
}

// 队员可否进行悬赏活动
bool TeamOfferModule::CanTeammemberDo(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 队伍成员
	CVarList team_mem_list;
	TeamModule::m_pTeamModule->GetTeamMemberList(pKernel, self, team_mem_list);
	const int team_mem_num = (int)team_mem_list.GetCount();	

	// 队伍人数不足
	if (team_mem_num < EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_LIMIT_PEOPLE))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_154, CVarList());
		return false;
	}

	// 队员条件是否满足
	bool is_suff = true;
	LoopBeginCheck(b);
	for (int j = 0; j < team_mem_num; ++j)
	{
		LoopDoCheck(b);
		const wchar_t *mem_name = team_mem_list.WideStrVal(j);
		if (wcscmp(pSelfObj->GetName(), mem_name) == 0)
		{
			continue;
		}

		// 在线
		if (pKernel->GetPlayerScene(mem_name) > 0)
		{
			PERSISTID member = pKernel->FindPlayer(mem_name);
			IGameObj *pMember = pKernel->GetGameObj(member);
			if (NULL == pMember)
			{
				// 队员{@0:name}不在悬赏场景
				is_suff = false;
				CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_155, CVarList() << mem_name);
				continue;
			}
			else
			{
				// 功能互斥
				if (ActionMutexModule::IsHaveAction(pKernel, member, false))
				{
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_156, CVarList() << mem_name);
					return false;
				}

				// 等级不足
				if (!TaskUtilS::Instance()->IsStart(pKernel, member, TASK_TYPE_TEAMOFFER))
				{
					is_suff = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_157, CVarList() << mem_name);
					continue;
				}

				// 次数不足
				if (!TaskUtilS::Instance()->IsHaveNumOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
				{
					is_suff = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_158, CVarList() << mem_name);
					continue;
				}
			}
		}
		else
		{	// 离线
			is_suff = false;
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_159, CVarList() << mem_name);
		}
	}

	return is_suff;
}

// 队员可否进入悬赏分组
bool TeamOfferModule::CanTeammemberEntry(IKernel* pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 队伍成员
	CVarList team_mem_list;
	TeamModule::m_pTeamModule->GetTeamMemberList(pKernel, self, team_mem_list);
	const int team_mem_num = (int)team_mem_list.GetCount();

	// 队伍人数不足
	if (team_mem_num < EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_LIMIT_PEOPLE))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_154, CVarList());
		return false;
	}

	// 队员条件是否满足
	bool can_entry = true;
	LoopBeginCheck(b);
	for (int j = 0; j < team_mem_num; ++j)
	{
		LoopDoCheck(b);
		const wchar_t *mem_name = team_mem_list.WideStrVal(j);
		if (wcscmp(pSelfObj->GetName(), mem_name) == 0)
		{
			continue;
		}

		// 在线
		if (pKernel->GetPlayerScene(mem_name) > 0)
		{
			PERSISTID member = pKernel->FindPlayer(mem_name);
			IGameObj *pMember = pKernel->GetGameObj(member);
			if (NULL == pMember)
			{
				// 队员{@0:name}不在悬赏场景
				can_entry = false;
				CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_155, CVarList() << mem_name);
				continue;
			}
			else
			{
				// 功能互斥
				if (ActionMutexModule::IsHaveAction(pKernel, member, false))
				{
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_156, CVarList() << mem_name);
					return false;
				}

				// 等级不足
				if (!TaskUtilS::Instance()->IsStart(pKernel, member, TASK_TYPE_TEAMOFFER))
				{
					can_entry = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_157, CVarList() << mem_name);
					continue;
				}

				// 次数不足
				if (!TaskUtilS::Instance()->IsHaveNumOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
				{
					can_entry = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_158, CVarList() << mem_name);
					continue;
				}

				// 无匹配的悬赏任务
				if (TaskUtilS::Instance()->QueryTaskStatus(pKernel, member, task_id) != TASK_STATUS_DOING)
				{
					can_entry = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_160, CVarList() << mem_name);
					continue;
				}

				// 在副本中
				if (pMember->QueryInt(FIELD_PROP_GROUP_ID) > 0)
				{
					can_entry = false;
					CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_161, CVarList() << mem_name);
					continue;
				}
			}
		}
		else
		{	// 离线
			can_entry = false;
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_159, CVarList() << mem_name);
		}
	}

	return can_entry;
}

// 是否可发起组队悬赏
bool TeamOfferModule::CanLaunch(IKernel* pKernel, const PERSISTID &self, 
	bool check_entry_npc/* = true*/)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return false;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return false;
	}

	// 功能互斥
	if (ActionMutexModule::IsHaveAction(pKernel, self))
	{
		return false;
	}

	// 可否接取悬赏任务
	if (!CanAcceptOfferTask(pKernel, self))
	{
		return false;
	}

	// 非队长无法操作
	if (!TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_162, CVarList());
		return false;
	}	

	// 是否在入口Npc附近
	bool is_around = false;
	const char *entry_npc = EnvirValueModule::EnvirQueryString(ENV_VALUE_TEAMOFFER_ENTRY_NPC);
	if (StringUtil::CharIsNull(entry_npc))
	{
		is_around = true;
	}
	else
	{
		CVarList npc_list;
		pKernel->GetAroundList(self, 15.0f, TYPE_NPC, 0, npc_list);

		LoopBeginCheck(a);
		for (int i = 0; i < (int)npc_list.GetCount(); ++i)
		{
			LoopDoCheck(a);
			IGameObj *pNpc = pKernel->GetGameObj(npc_list.ObjectVal(i));
			if (NULL == pNpc)
			{
				continue;
			}

			if (StringUtil::CharIsEqual(entry_npc, pNpc->GetConfig()))
			{
				is_around = true;
				break;
			}
		}
	}

	// 距离悬赏引导Npc太远
	if (!is_around && check_entry_npc)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_163, CVarList());
		return false;
	}

	// 是否有进行中的悬赏
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, pSelfObj->QueryInt(FIELD_PROP_TEAM_ID));
	if (exist_row >= 0)
	{
		if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) == TEAMOFFER_STATE_DOING)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_164, CVarList());
			return false;
		}
	}
	
	// 队员条件是否满足
	if (!CanTeammemberDo(pKernel, self))
	{
		return false;
	}

	return true;
}

// 悬赏场景验证
bool TeamOfferModule::IsVaildOfferScene(IKernel* pKernel, const PERSISTID &self, 
	const int task_id, const int scene_id)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 取得数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 场景是否一致
	return TaskUtilS::Instance()->IsTargetScene(pKernel, pData->scene_list, scene_id);
}

// 计算队伍平均等级
const int TeamOfferModule::GetAverageLvl(IKernel* pKernel, const PERSISTID &self)
{

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 保护
	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	if (team_id <= 0)
	{
		return 0;
	}

	IRecord *pTeamRec = pSelfObj->GetRecord(TEAM_REC_NAME);
	if (NULL == pTeamRec)
	{
		return 0;
	}

	// 遍历
	const int rows = pTeamRec->GetRows();
	int sum_level = 0;

	LoopBeginCheck(a);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(a);
		sum_level += pTeamRec->QueryInt(i, TEAM_REC_COL_BGLEVEL);
	}

	return (int)(sum_level / rows);
}

// 玩家准备就绪
void TeamOfferModule::PlayerReady(IKernel* pKernel, const PERSISTID &self, 
	const IVarList& args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	// 无队伍
	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	if (team_id <= 0)
	{
		return;
	}

	// 无悬赏
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	// 不在悬赏场景
	const int task_id = pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_TASK_ID);
	if (!IsVaildOfferScene(pKernel, self, task_id, pKernel->GetSceneId()))
	{
		return;
	}

	// 不在悬赏副本
	const int group_id = pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_GROUP_ID);
	if (pSelfObj->QueryInt(FIELD_PROP_GROUP_ID) != group_id)
	{
		return;
	}
	
	// 当前状态
	const int state = pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE);	
	if (state == TEAMOFFER_STATE_DOING)	// 进行阶段
	{
		// 通知客户端倒计时
		const int offer_time = EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_TIME) * 60 * 1000;
		const int64_t state_time = pRec->QueryInt64(exist_row, TEAMOFFER_REC_COL_STATE_TIME);
		const int64_t now_time = util_get_time_64();
		const int64_t diff_time = offer_time - (now_time - state_time);

		// 有倒计时
		if (diff_time > 0)
		{
			CVarList s2c_msg;
			s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
					<< S2C_TEAMOFFER_SUB_MSG_COUNTDOWN
					<< (int)diff_time;
			pKernel->Custom(self, s2c_msg);
		}
	}	
}

// 玩家上线
void TeamOfferModule::PlayerOnline(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 属性回调
	ADD_CRITICAL(pKernel, self, FIELD_PROP_TEAM_ID, "TeamOfferModule::C_OnTeamChange");

	// 检查
	IPubData *pPubData = TeamOfferModule::m_pTeamOfferModule->GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	// 无队伍
	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	if (team_id <= 0)
	{
		// 清除悬赏任务
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);
		return;
	}

	// 无悬赏
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		// 清除悬赏任务
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);
	}	
}

// 发起悬赏
void TeamOfferModule::Launch(IKernel* pKernel, const PERSISTID &self,
	bool check_entry_npc/* = true*/)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 条件验证
	if (!CanLaunch(pKernel, self, check_entry_npc))
	{
		return;
	}

	// 队伍id
	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);

	// 悬赏任务id
	const int task_id = TaskUtilS::Instance()->RandomTask(pKernel, self, TASK_TYPE_TEAMOFFER);

	// 发送发起消息到公共区间
	CVarList s2p_msg;
	s2p_msg << S2P_TEAMOFFER_SUB_MSG_LAUNCH
			<< pSelfObj->GetName()
			<< team_id
			<< task_id
			<< -1
			<< TEAMOFFER_STATE_NONE
			<< PERSISTID();
	SendS2PMsg(pKernel, s2p_msg);
}

// 队长发起任务同步
bool TeamOfferModule::SyncTask(IKernel* pKernel, const PERSISTID &self, 
	const IVarList &args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 队伍id、悬赏id
	const int team_id = args.IntVal(2);
	const int task_id = args.IntVal(3);

	if (pSelfObj->QueryInt(FIELD_PROP_TEAM_ID) != team_id)
	{
		return false;
	}

	if (!TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
	{
		return false;
	}

	// 接受任务
	if (!TaskUtilS::Instance()->AcceptTask(pKernel, self, task_id))
	{
		return false;
	}

	// 队伍成员
	CVarList mem_list;
	TeamModule::m_pTeamModule->GetTeamMemberList(pKernel, self, mem_list);

	// 通知队员同步任务
	CVarList s2s_msg;
	s2s_msg << COMMAND_TEAMOFFER_MSG
			<< S2S_TEAMOFFER_SUB_MSG_MEMBER_SYNC_TASK
			<< team_id
			<< task_id;
	
	LoopBeginCheck(a);
	for (int i = 0; i < (int)mem_list.GetCount(); ++i)
	{
		LoopDoCheck(a);
		const wchar_t *name = mem_list.WideStrVal(i);
		if (StringUtil::CharIsNull(name))
		{
			continue;
		}

		if (wcscmp(name, pSelfObj->GetName()) == 0)
		{
			continue;
		}

		pKernel->CommandByName(name, s2s_msg);
	}

	return true;
}

// 队员同步任务
bool TeamOfferModule::MemberSyncTask(IKernel* pKernel, const PERSISTID &self, const IVarList &args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 队伍id、悬赏id
	const int team_id = args.IntVal(2);
	const int task_id = args.IntVal(3);

	if (pSelfObj->QueryInt(FIELD_PROP_TEAM_ID) != team_id)
	{
		return false;
	}

	if (TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
	{
		return false;
	}

	// 接受任务
	return TaskUtilS::Instance()->AcceptTask(pKernel, self, task_id);
}

// 删除悬赏任务
void TeamOfferModule::DeleteTask(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);
		return;
	}

	// 已结束
	if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) == TEAMOFFER_STATE_END)
	{
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);
	}
}

// 队长发起进入副本
void TeamOfferModule::EntryGroup(IKernel* pKernel, const PERSISTID &self, 
	const int team_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	if (!TeamModule::m_pTeamModule->IsInTeam(pKernel, self))
	{
		return;
	}

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	if (pSelfObj->QueryInt(FIELD_PROP_TEAM_ID) != team_id)
	{
		return;
	}

	const int cur_group_id = pSelfObj->QueryInt(FIELD_PROP_GROUP_ID);

	// 异常
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);

		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_165, CVarList());
		return;
	}

	CVarList row_value;
	pRec->QueryRowValue(exist_row, row_value);

	const int task_id = row_value.IntVal(TEAMOFFER_REC_COL_TASK_ID);
	const int group_id = row_value.IntVal(TEAMOFFER_REC_COL_GROUP_ID);
	const int state = row_value.IntVal(TEAMOFFER_REC_COL_STATE);
	const int64_t state_time = row_value.Int64Val(TEAMOFFER_REC_COL_STATE_TIME);
	PERSISTID creator = row_value.ObjectVal(TEAMOFFER_REC_COL_CREATOR);

	// 已在悬赏分组中
	if (group_id > 0 && cur_group_id == group_id)
	{
		return;
	}

	// 功能互斥
	if (ActionMutexModule::IsHaveAction(pKernel, self))
	{
		return;
	}

	// 已在副本中
	if (cur_group_id > 0)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_166, CVarList());
		return;
	}

	// 是否有悬赏任务
	if (!TaskUtilS::Instance()->IsHaveOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_167, CVarList());
		return;
	}

	// 任务不一致
	if (TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id) == TASK_STATUS_MAX)
	{
		TaskUtilS::Instance()->AcceptTask(pKernel, self, task_id);
		return;
	}

	// 场景不对
	if (!IsVaildOfferScene(pKernel, self, task_id, pKernel->GetSceneId()))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_168, CVarList());
		return;
	}

	// 队长第一次操作
	if (TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self) && group_id < 0)
	{		
		// 未开始，进行第一次发起分组
		if (TEAMOFFER_STATE_NONE == state)
		{			
			// 队员条件是否满足
			if (!CanTeammemberEntry(pKernel, self, task_id))
			{
				return;
			}

			// 计算平均等级
			int average_lvl = GetAverageLvl(pKernel, self);
			average_lvl = average_lvl == 0 ? pSelfObj->QueryInt(FIELD_PROP_LEVEL) : average_lvl;

			const int new_group_id = pKernel->NewGroupId();

			// 创建刷怪器
			const PERSISTID creator = NpcCreatorModule::m_pNpcCreatorModule->CreateTaskrCreator(
				pKernel, pKernel->GetScene(), task_id, new_group_id, average_lvl);

			IGameObj *pCreator = pKernel->GetGameObj(creator);
			if (NULL == pCreator)
			{
				return;
			}

			// 队伍id
			ADD_DATA_INT(pCreator, "OfferTeam");
			pCreator->SetDataInt("OfferTeam", team_id);

			// 发送副本开始通知
			CVarList s2p_msg;
			s2p_msg << S2P_TEAMOFFER_SUB_MSG_START
					<< team_id
					<< task_id
					<< new_group_id
					<< creator;
			SendS2PMsg(pKernel, s2p_msg);
						
			// 进入分组
			CommRuleModule::ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_ENTRY, new_group_id);

			// 通知客户端倒计时
			const int offer_time = EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_TIME) * 60 * 1000;
			CVarList s2c_msg;
			s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
					<< S2C_TEAMOFFER_SUB_MSG_COUNTDOWN
					<< offer_time;
			pKernel->Custom(self, s2c_msg);

			// 加入互斥
			ActionMutexModule::NoteAction(pKernel, self, ACTION_TEAMOFFER);

			// 控制心跳
			ADD_HEART_BEAT(pKernel, creator, "TeamOfferModule::HB_Contrl", 1000);
			ADD_COUNT_BEAT(pKernel, creator, "TeamOfferModule::HB_TeamOfferEnd", offer_time, 1);
		}
	}
	else
	{
		// 副本未开始
		if (TEAMOFFER_STATE_NONE == state)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_169, CVarList());
			return;
		}
		// 进行中，可进入分组
		else if (TEAMOFFER_STATE_DOING == state)
		{
			if (group_id > 0)
			{
				CommRuleModule::ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_ENTRY, group_id);

				// 通知客户端倒计时
				const int offer_time = EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_TIME) * 60 * 1000;
				const int64_t now_time = util_get_time_64();
				const int64_t diff_time = offer_time - (now_time - state_time);

				// 有倒计时
				if (diff_time > 0)
				{
					CVarList s2c_msg;
					s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
							<< S2C_TEAMOFFER_SUB_MSG_COUNTDOWN
							<< (int)diff_time;
					pKernel->Custom(self, s2c_msg);
				}
				
				// 加入互斥
				ActionMutexModule::NoteAction(pKernel, self, ACTION_TEAMOFFER);
			}
		}
		// 副本结算中
		else if (TEAMOFFER_STATE_END == state)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_170, CVarList());
			return;
		}
	}	
}

// 队员跟随进入
void TeamOfferModule::FollowEntry(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	if (!TeamModule::m_pTeamModule->IsInTeam(pKernel, self))
	{
		return;
	}

	// 队长是否在同场景
	const wchar_t *captain_name = pSelfObj->QueryWideStr(FIELD_PROP_TEAM_CAPTAIN);
	if (StringUtil::CharIsNull(captain_name))
	{
		return;
	}

	PERSISTID captain = pKernel->FindPlayer(captain_name);
	IGameObj *pCaptain = pKernel->GetGameObj(captain);
	if (NULL == pCaptain)
	{
		return;
	}

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	// 异常
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, pSelfObj->QueryInt(FIELD_PROP_TEAM_ID));
	if (exist_row < 0)
	{
		TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);
		return;
	}

	CVarList row_value;
	pRec->QueryRowValue(exist_row, row_value);

	const int task_id = row_value.IntVal(TEAMOFFER_REC_COL_TASK_ID);
	const int group_id = row_value.IntVal(TEAMOFFER_REC_COL_GROUP_ID);
	const int state = row_value.IntVal(TEAMOFFER_REC_COL_STATE);
	const int64_t state_time = row_value.Int64Val(TEAMOFFER_REC_COL_STATE_TIME);
	PERSISTID creator = row_value.ObjectVal(TEAMOFFER_REC_COL_CREATOR);

	// 场景不对
	if (!IsVaildOfferScene(pKernel, self, task_id, pKernel->GetSceneId()))
	{
		return;
	}

	// 队长未在分组
	if (pCaptain->QueryInt(FIELD_PROP_GROUP_ID) != group_id)
	{
		return;
	}

	// 已在悬赏分组中
	const int cur_group_id = pSelfObj->QueryInt(FIELD_PROP_GROUP_ID);	
	if (group_id > 0 && cur_group_id == group_id)
	{
		return;
	}

	// 功能互斥
	if (ActionMutexModule::IsHaveAction(pKernel, self))
	{
		return;
	}

	// 已在副本中
	if (cur_group_id > 0)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_166, CVarList());
		return;
	}

	// 任务不一致
	if (TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id) == TASK_STATUS_MAX)
	{
		TaskUtilS::Instance()->AcceptTask(pKernel, self, task_id);
		return;
	}

	// 是否有悬赏任务
	if (!TaskUtilS::Instance()->IsHaveOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_167, CVarList());
		return;
	}

	if (TEAMOFFER_STATE_DOING == state)
	{
		if (group_id > 0)
		{
			CommRuleModule::ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_ENTRY, group_id);

			// 通知客户端倒计时
			const int offer_time = EnvirValueModule::EnvirQueryInt(ENV_VALUE_TEAMOFFER_TIME) * 60 * 1000;
			const int64_t now_time = util_get_time_64();
			const int64_t diff_time = offer_time - (now_time - state_time);

			// 有倒计时
			if (diff_time > 0)
			{
				CVarList s2c_msg;
				s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
						<< S2C_TEAMOFFER_SUB_MSG_COUNTDOWN
						<< (int)diff_time;
				pKernel->Custom(self, s2c_msg);
			}

			// 加入互斥
			ActionMutexModule::NoteAction(pKernel, self, ACTION_TEAMOFFER);
		}
	}
	// 副本结算中
	else if (TEAMOFFER_STATE_END == state)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_170, CVarList());
		return;
	}	
}

// 发送消息到公共区间
void TeamOfferModule::SendS2PMsg(IKernel* pKernel, const IVarList& s2p_sub_msg)
{
	// 发送发起消息到公共区间
	CVarList s2p_msg;
	s2p_msg << PUBSPACE_DOMAIN
			<< TeamOfferModule::GetDomainName(pKernel)
			<< SP_DOMAIN_MSG_TEAMOFFER;
	s2p_msg.Concat(s2p_sub_msg);
	pKernel->SendPublicMessage(s2p_msg);
}

// 取得公共区数据
IPubData* TeamOfferModule::GetPubData(IKernel* pKernel)
{
	IPubSpace* pDomainSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pDomainSpace == NULL)
	{
		return NULL;
	}

	IPubData *pPubData = pDomainSpace->GetPubData(GetDomainName(pKernel).c_str());
	
	return pPubData;
}

// 退出分组
void TeamOfferModule::LeaveGroup(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 离开队伍
	CVarList s2s_msg;
	s2s_msg << COMMAND_TEAM_MSG
			<< COMMAND_TEAM_LEAVE_MSG;
	pKernel->Command(self, self, s2s_msg);
}

// 组队悬赏结果处理
void TeamOfferModule::OnResult(IKernel* pKernel, const IVarList& args)
{
	PERSISTID creator = args.ObjectVal(6);
	if (!pKernel->Exists(creator))
	{
		return;
	}

	CVarList sub_msg;
	sub_msg.Append(args, 4, args.GetCount() - 4);

	// 通知刷怪器处理悬赏结果
	CVarList s2s_msg;
	s2s_msg << COMMAND_TEAMOFFER_MSG
			<< S2S_TEAMOFFER_SUB_MSG_RESULT;
	s2s_msg.Concat(sub_msg);

	pKernel->Command(pKernel->GetScene(), creator, s2s_msg);
}

// 处理组队悬赏结果
void TeamOfferModule::ProcResult(IKernel* pKernel, const PERSISTID &creator,
	const IVarList& args)
{
	IGameObj *pCreator = pKernel->GetGameObj(creator);
	if (NULL == pCreator)
	{
		return;
	}

	const int result = args.IntVal(2);
	const int team_id = args.IntVal(3);
	const int group_id = pCreator->QueryInt(FIELD_PROP_GROUP_ID);

	CVarList mem_list;
	mem_list.Append(args, 6, args.GetCount() - 6);

	// 副本已无人
	if (mem_list.IsEmpty())
	{
		// 清理分组
		pKernel->ClearGroup(group_id);

		// 停止悬赏
		CVarList s2p_msg;
		s2p_msg << S2P_TEAMOFFER_SUB_MSG_STOP
				<< team_id
				<< 0
				<< group_id;
		SendS2PMsg(pKernel, s2p_msg);

		return;
	}

	// 失败
	if (RESULTS_FAILED == result)
	{		
		CVarList s2c_msg;
		s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
				<< S2C_TEAMOFFER_SUB_MSG_RESULT
				<< result;

		LoopBeginCheck(a);
		for (int i = 0; i < (int)mem_list.GetCount(); ++i)
		{
			LoopDoCheck(a);
			const wchar_t *name = mem_list.WideStrVal(i);
			if (StringUtil::CharIsNull(name))
			{
				continue;
			}

			PERSISTID member = pKernel->FindPlayer(name);
			IGameObj *pMember = pKernel->GetGameObj(member);
			if (NULL == pMember)
			{
				continue;
			}

			// 未开启
			if (!TaskUtilS::Instance()->IsStart(pKernel, member, TASK_TYPE_TEAMOFFER))
			{
				continue;
			}

			// 是否匹配
			if (pMember->QueryInt(FIELD_PROP_TEAM_ID) != team_id)
			{
				continue;
			}

			if (pMember->QueryInt(FIELD_PROP_GROUP_ID) != group_id)
			{
				continue;
			}
			
			// 失败界面
			pKernel->Custom(member, s2c_msg);

			// 清除互斥
			ActionMutexModule::CleanAction(pKernel, member);

			// 切出分组
			CommRuleModule::ChangeGroup(pKernel, member, GROUP_CHANGE_TYPE_QUIT, -1);

			// 玩法日志-组队悬赏
			//GamePlayerActionLog log;
			//log.actionType = LOG_GAME_ACTION_TEAMOFFER;
			//log.actionState = LOG_GAME_ACTION_FAIL;
			//LogModule::m_pLogModule->SaveGameActionLog(pKernel, member, log);

			//// 记录j结束日志
			//TeamOfferLog end_log;
			//end_log.offer_level = pCreator->QueryDataInt("CreatorLevel");
			//end_log.offer_num = task_conf.IntVal(QUEST_CONFIG_REC_COMPLETE_COUNT);
			//end_log.opt_type = LOG_GAME_ACTION_TEAMOFFER;
			//end_log.offer_result = result;
			//LogModule::m_pLogModule->OnTeamOfferEnd(pKernel, member, end_log);
		}

		// 清理分组
		pKernel->ClearGroup(group_id);	

		// 重置悬赏
		CVarList s2p_msg;
		s2p_msg << S2P_TEAMOFFER_SUB_MSG_RESET
				<< team_id;
		SendS2PMsg(pKernel, s2p_msg);
	}
	else // 处理悬赏成功
	{		
		// 通关时间
		const int pass_time = args.IntVal(5);

		// 当前队伍人数
		int team_num = (int)mem_list.GetCount();
		std::map<std::wstring, IGameObj*> playerGroup;
		LoopBeginCheck(b);
		for (int i = 0; i < team_num; ++i)
		{
			LoopDoCheck(b);
			const wchar_t *name = mem_list.WideStrVal(i);
			if (StringUtil::CharIsNull(name))
			{
				continue;
			}

			PERSISTID member = pKernel->FindPlayer(name);
			IGameObj *pMember = pKernel->GetGameObj(member);
			if (NULL == pMember)
			{		
				// 删除悬赏任务	
				pKernel->CommandByName(name, CVarList() << COMMAND_TEAMOFFER_MSG << S2S_TEAMOFFER_SUB_MSG_DELETE_TASK);
				continue;
			}

			// 未开启
			if (!TaskUtilS::Instance()->IsStart(pKernel, member, TASK_TYPE_TEAMOFFER))
			{
				continue;
			}

			// 队伍中
			if (pMember->QueryInt(FIELD_PROP_TEAM_ID) != team_id)
			{
				continue;
			}

			// 删除
			TaskUtilS::Instance()->CleanTaskByType(pKernel, member, TASK_TYPE_TEAMOFFER);

			// 不在副本中
			if (pMember->QueryInt(FIELD_PROP_GROUP_ID) != group_id)
			{
				continue;
			}
			playerGroup[name] = pMember;

			// 任务次数递增
			TaskUtilS::Instance()->RecordTaskNum(pKernel, member, TASK_TYPE_TEAMOFFER);

			// 奖励领取
			RewardTeamOffer(pKernel, member, pass_time);

			// 清除互斥
			ActionMutexModule::CleanAction(pKernel, member);

			// 清除战斗状态
			pMember->SetInt(FIELD_PROP_FIGHT_STATE, 0);

			// 切出分组
			CommRuleModule::ChangeGroup(pKernel, member, GROUP_CHANGE_TYPE_QUIT, -1);

			// 玩法日志-组队悬赏
			//GamePlayerActionLog log;
			//log.actionType = LOG_GAME_ACTION_TEAMOFFER;
			//log.actionState = LOG_GAME_ACTION_SUCCESS;
			//LogModule::m_pLogModule->SaveGameActionLog(pKernel, member, log);

			//// 记录结束日志
			//TeamOfferLog end_log;
			//end_log.offer_level = pCreator->QueryDataInt("CreatorLevel");
			//end_log.offer_num = task_conf.IntVal(QUEST_CONFIG_REC_COMPLETE_COUNT);
			//end_log.pass_time = pass_time;
			//end_log.opt_type = LOG_GAME_ACTION_TEAMOFFER;
			//end_log.offer_result = result;
			//LogModule::m_pLogModule->OnTeamOfferEnd(pKernel, member, end_log);
		}

		// 清理分组
		pKernel->ClearGroup(group_id);

		// 结束已完成悬赏
		CVarList s2p_msg;
		s2p_msg << S2P_TEAMOFFER_SUB_MSG_STOP
				<< team_id
				<< 0
				<< group_id;
		SendS2PMsg(pKernel, s2p_msg);

		// 通知队长自动发起下一环
		CVarList s2s_msg;
		s2s_msg << COMMAND_TEAMOFFER_MSG
				<< S2S_TEAMOFFER_SUB_MSG_CONTINUE;

		LoopBeginCheck(c);
		for (int i = 0; i < (int)mem_list.GetCount(); ++i)
		{
			LoopDoCheck(c);
			const wchar_t *name = mem_list.WideStrVal(i);
			if (StringUtil::CharIsNull(name))
			{
				continue;
			}

			pKernel->CommandByName(name, s2s_msg);
		}
	}			
}

// 加入队伍
void TeamOfferModule::AddTeam(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	CVarList row_value;
	pRec->QueryRowValue(exist_row, row_value);

	const int task_id = row_value.IntVal(TEAMOFFER_REC_COL_TASK_ID);
	const int group_id = row_value.IntVal(TEAMOFFER_REC_COL_GROUP_ID);

	// 条件验证
	if (!CanAcceptOfferTask(pKernel, self))
	{
		return;
	}

	// 接取悬赏任务
	if (!TaskUtilS::Instance()->AcceptTask(pKernel, self, task_id))
	{
		return;
	}
}

// 退出队伍
void TeamOfferModule::QuitTeam(IKernel* pKernel, const PERSISTID &self, 
	const int old_team_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 清除悬赏任务
	TaskUtilS::Instance()->CleanTaskByType(pKernel, self, TASK_TYPE_TEAMOFFER);

	IPubData *pPubData = GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, old_team_id);
	if (exist_row < 0)
	{
		return;
	}

	CVarList row_value;
	pRec->QueryRowValue(exist_row, row_value);

	const int group_id = row_value.IntVal(TEAMOFFER_REC_COL_GROUP_ID);
	const int state = row_value.IntVal(TEAMOFFER_REC_COL_STATE);	

	// 离开分组
	if (pSelfObj->QueryInt(FIELD_PROP_GROUP_ID) == group_id)
	{
		CommRuleModule::ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_QUIT, -1);
	}

	// 属于组队悬赏
	if (ActionMutexModule::IsThisAction(pKernel, self, ACTION_TEAMOFFER))
	{
		ActionMutexModule::CleanAction(pKernel, self);
	}

	// 未开始时，验证队伍是否解散
	if (state == TEAMOFFER_STATE_NONE)
	{
		// 组队悬赏有效验证
		CVarList s2p_msg;
		s2p_msg << S2P_TEAMOFFER_SUB_MSG_CHECK
				<< old_team_id;
		m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);
	}
	
	// 您已退出组队悬赏
	CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_171, CVarList());
}

// 继续发起下一个悬赏
void TeamOfferModule::ContinueNext(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 非队长
	if (!TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
	{
		return;
	}

	// 未开启
	if (!TaskUtilS::Instance()->IsStart(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		return;
	}

	// 悬赏完成次数
	if (!TaskUtilS::Instance()->IsHaveNumOfType(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		return;
	}

	// 是否结束一环
	const int offer_num = TaskUtilS::Instance()->QueryTaskNum(pKernel, self, TASK_TYPE_TEAMOFFER);
	if (offer_num % 10 == 0)
	{
		// 本环悬赏已结束，请重新接取下一环悬赏
		CVarList s2c_msg;
		s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
				<< S2C_TEAMOFFER_SUB_MSG_CONTINUE_NEXT
				<< offer_num;
		pKernel->Custom(self, s2c_msg);

		return;
	}

	// 悬赏发起
	m_pTeamOfferModule->Launch(pKernel, self, false);
}

// 奖励处理
void TeamOfferModule::RewardTeamOffer(IKernel* pKernel, const PERSISTID &self, 
	const int pass_time)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 未开启
	if (!TaskUtilS::Instance()->IsStart(pKernel, self, TASK_TYPE_TEAMOFFER))
	{
		return;
	}
	
	// 悬赏完成次数
	const int offer_num = TaskUtilS::Instance()->QueryTaskNum(pKernel, self, TASK_TYPE_TEAMOFFER);

	// 玩家等级
	const int self_level = pSelfObj->QueryInt(FIELD_PROP_LEVEL);

	CVarList s2c_msg;
	s2c_msg << SERVER_CUSTOMMSG_TEAMOFFER
			<< S2C_TEAMOFFER_SUB_MSG_RESULT
			<< RESULTS_SUCCESS
			<< offer_num
			<< TaskUtilS::Instance()->QueryTaskTotalNum(TASK_TYPE_TEAMOFFER)
			<< pass_time;

	// 根据次数和等级取奖励
	const OfferAwardRule *award_rule = GetAwardRule(offer_num, self_level);
	if (NULL != award_rule)
	{					
		// 领取个人奖励
		RewardModule::m_pRewardInstance->RewardPlayerById(pKernel, self, award_rule->reward_id);

		s2c_msg << award_rule->reward_id;
	}

	// 队长宝箱
	if (TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
	{
		const int reward_id = GetCaptainBox(offer_num);
		if (reward_id != 0)
		{			
			// 领取宝箱
			RewardModule::m_pRewardInstance->RewardPlayerById(pKernel, self, reward_id);
		}
		//pKernel->Command(self,self, CVarList() << COMMAND_FRIEND_MSG << SS_FRIEND_TEAM_ADD_INTIMACY << TEAM_ADD_INTIMACY_OFFER);
	}

	pKernel->Custom(self, s2c_msg);
}

// 重新加载组队悬赏活动配置
void TeamOfferModule::ReloadTeamofferConfig(IKernel* pKernel)
{
	TeamOfferModule::m_pTeamOfferModule->LoadRes(pKernel);
}

// 客户端消息
int TeamOfferModule::OnCustomMsg(IKernel* pKernel, const PERSISTID &self, 
							  const PERSISTID & sender, const IVarList & args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 子消息
	const int sub_msg = args.IntVal(1);

	switch (sub_msg)
	{
	case C2S_TEAMOFFER_SUB_MSG_LAUNCH:	// 发起一次组队悬赏
		{
			m_pTeamOfferModule->Launch(pKernel, self);
		}
		break;
	case C2S_TEAMOFFER_SUB_MSG_ENTRY_GROUP:	// 进入悬赏分组
		{
			// 队长进分组
			if (TeamModule::m_pTeamModule->IsTeamCaptain(pKernel, self))
			{
				m_pTeamOfferModule->EntryGroup(pKernel, self, pSelfObj->QueryInt(FIELD_PROP_TEAM_ID));
			}
			else
			{
				m_pTeamOfferModule->FollowEntry(pKernel, self);
			}			
		}
		break;
	case C2S_TEAMOFFER_SUB_MSG_LEAVE:	// 退出组队悬赏
		{
			m_pTeamOfferModule->LeaveGroup(pKernel, self);
		}
		break;
	default:
		break;
	}
	return 0;
}

// 玩家内部消息
int TeamOfferModule::OnCommandMsg(IKernel *pKernel, const PERSISTID &self,
									const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 提取参数
	int sub_msg = args.IntVal(1);
	
	switch (sub_msg)
	{
	case S2S_TEAMOFFER_SUB_MSG_SYNC_TASK:	// 通知队长发起同步悬赏任务
		{
			// 同步发起失败
			if (!m_pTeamOfferModule->SyncTask(pKernel, self, args))
			{
				CVarList s2p_msg;
				s2p_msg << S2P_TEAMOFFER_SUB_MSG_STOP
						<< args.IntVal(2)
						<< args.IntVal(3)
						<< -1;
				m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);
			}
		}		
		break;
	case S2S_TEAMOFFER_SUB_MSG_ENTRY:	// 进入组队悬赏分组副本
		{
			m_pTeamOfferModule->EntryGroup(pKernel, self, args.IntVal(2));
		}		
		break;
	case S2S_TEAMOFFER_SUB_MSG_CONTINUE:	// 通知队队长继续下一环
		{
			m_pTeamOfferModule->ContinueNext(pKernel, self);			
		}		
		break;
	case S2S_TEAMOFFER_SUB_MSG_MEMBER_SYNC_TASK:	// 通知队员同步悬赏任务
		{
			m_pTeamOfferModule->MemberSyncTask(pKernel, self, args);
		}
		break;
	case S2S_TEAMOFFER_SUB_MSG_DELETE_TASK:	// 通知成员删除旧的悬赏任务
		{
			m_pTeamOfferModule->DeleteTask(pKernel, self);
		}
		break;
	default:
		break;
	}

	return 0;
}

// 刷怪器内部消息
int TeamOfferModule::OnCreatorCommandMsg(IKernel *pKernel, const PERSISTID &creator,
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(creator))
	{
		return 0;
	}

	// 提取参数
	int sub_msg = args.IntVal(1);
	
	switch (sub_msg)
	{
	case S2S_TEAMOFFER_SUB_MSG_RESULT:	// 组队悬赏结果
		{
			m_pTeamOfferModule->ProcResult(pKernel, creator, args);
		}		
		break;
	default:
		break;
	}

	return 0;
}

// 任务进度
int TeamOfferModule::OnCommandTaskProc(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 提交操作
	const int sub_msg = args.IntVal(1);
	const int task_id = args.IntVal(2);
	if (TASK_ACTION_SUBMIT != sub_msg)
	{
		return 0;
	}

	IPubData *pPubData = m_pTeamOfferModule->GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return 0;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return 0;
	}

	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return 0;
	}

	// 不在进行中
	if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) != TEAMOFFER_STATE_DOING)
	{
		return 0;
	}	

	// 取得数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return 0;
	}

	if (pData->type != TASK_TYPE_TEAMOFFER)
	{
		return 0;
	}

	if (task_id != pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_TASK_ID))
	{
		return 0;
	}

	// 组队悬赏成功
	CVarList s2p_msg;
	s2p_msg << S2P_TEAMOFFER_SUB_MSG_RESULT
			<< RESULTS_SUCCESS
			<< team_id;
	m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);

	return 0;
}

// 组队悬赏Npc被杀回调
int TeamOfferModule::OnOfferNpcBeKilled(IKernel* pKernel, const PERSISTID& offer_npc,
									  const PERSISTID& killer, const IVarList& args)
{
	IGameObj *pOfferNpc = pKernel->GetGameObj(offer_npc);
	if (NULL == pOfferNpc)
	{
		return 0;
	}

	// 记录杀死NPC信息
	NpcCreatorModule::m_pNpcCreatorModule->RecordKillNpc(pKernel, offer_npc);

	return 0;
}

// 玩家上线
int TeamOfferModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
								  const PERSISTID& sender, const IVarList& args)
{
	m_pTeamOfferModule->PlayerOnline(pKernel, self);

	return 0;
}

// 玩家离开场景
int TeamOfferModule::OnPlayerLeaveScene(IKernel *pKernel, const PERSISTID &self, 
									 const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!TeamModule::m_pTeamModule->IsInTeam(pKernel, self))
	{
		return 0;
	}

	IPubData *pPubData = m_pTeamOfferModule->GetPubData(pKernel);
	if (NULL == pPubData)
	{
		return 0;
	}

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return 0;
	}

	const int cur_group_id = pSelfObj->QueryInt(FIELD_PROP_GROUP_ID);
	const int team_id = pSelfObj->QueryInt(FIELD_PROP_TEAM_ID);

	// 异常
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return 0;
	}

	const int group_id = pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_GROUP_ID);

	// 已在悬赏分组中
	if (group_id > 0 && cur_group_id == group_id)
	{
		// 属于组队悬赏
		if (ActionMutexModule::IsThisAction(pKernel, self, ACTION_TEAMOFFER))
		{
			ActionMutexModule::CleanAction(pKernel, self);
		}

		CommRuleModule::ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_QUIT, -1);
	}

	return 0;
}

// 进入场景
int TeamOfferModule::OnPlayerEntry(IKernel * pKernel, const PERSISTID & self, 
								const PERSISTID & sender, const IVarList & args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	return 0;
}

// 客户端准备就绪
int TeamOfferModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, 
								const PERSISTID& sender, const IVarList& args)
{
	m_pTeamOfferModule->PlayerReady(pKernel, self, args);

	return 0;
}

// 下线
int TeamOfferModule::OnStore(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int reason = args.IntVal(0);
	if (reason != STORE_EXIT)
	{
		return 0;
	}

	return 0;
}

// 队伍变化回调
int TeamOfferModule::C_OnTeamChange(IKernel* pKernel, const PERSISTID& self, 
	const char *property, const IVar& old)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 离队
	if (pSelfObj->QueryInt(property) <= 0)
	{
		m_pTeamOfferModule->QuitTeam(pKernel, self, old.IntVal());
	}
	else // 加入队伍
	{
		m_pTeamOfferModule->AddTeam(pKernel, self);
	}

	return 0;
}

// 组队悬赏结束心跳
int TeamOfferModule::HB_TeamOfferEnd(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	IGameObj *pCreator = pKernel->GetGameObj(creator);
	if (NULL == pCreator)
	{
		return 0;
	}

	// 组队悬赏结果（悬赏时间结束，失败）
	CVarList s2p_msg;
	s2p_msg << S2P_TEAMOFFER_SUB_MSG_RESULT
			<< RESULTS_FAILED
			<< pCreator->QueryDataInt("OfferTeam");
	m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);

	return 0;
}

// 组队悬赏副本控制心跳
int TeamOfferModule::HB_Contrl(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	IGameObj *pCreator = pKernel->GetGameObj(creator);
	if (NULL == pCreator)
	{
		return 0;
	}

	// 组队悬赏有效验证
	CVarList player_list;
	pKernel->GroupChildList(pCreator->QueryInt(FIELD_PROP_GROUP_ID), TYPE_PLAYER, player_list);
	if (player_list.IsEmpty())
	{
		// 组队悬赏结果（失败）
		CVarList s2p_msg;
		s2p_msg << S2P_TEAMOFFER_SUB_MSG_RESULT
				<< RESULTS_FAILED
				<< pCreator->QueryDataInt("OfferTeam");
		m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);
	}

	return 0;
}

// 处理公共数据服务器下传的消息
int TeamOfferModule::OnPublicMessage(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	int sub_msg_id = args.IntVal(2);
	if (sub_msg_id != PS_DOMAIN_MSG_TEAMOFFER)
	{
		return 0;
	}

	// 操作
	const int opt_type = args.IntVal(3);

	switch (opt_type)
	{
	case P2S_TEAMOFFER_SUB_MSG_SYNC_TASK:	// 通知队伍成员同步悬赏任务
		{
			const wchar_t *capt_name = args.WideStrVal(4);
			if (!StringUtil::CharIsNull(capt_name))
			{
				if (pKernel->GetPlayerScene(capt_name) < 0)
				{
					CVarList s2p_msg;
					s2p_msg << S2P_TEAMOFFER_SUB_MSG_STOP
							<< args.IntVal(5)
							<< args.IntVal(6)
							<< -1;
					m_pTeamOfferModule->SendS2PMsg(pKernel, s2p_msg);
				}
				else
				{
					CVarList s2s_msg;
					s2s_msg << COMMAND_TEAMOFFER_MSG
							<< S2S_TEAMOFFER_SUB_MSG_SYNC_TASK
							<< args.IntVal(5)
							<< args.IntVal(6);
					pKernel->CommandByName(capt_name, s2s_msg);
				}				
			}
		}
		break;
	case P2S_TEAMOFFER_SUB_MSG_ENTRY_GROUP:	// 通知队伍进入分组副本
		{
			CVarList mem_list;
			mem_list.Append(args, 5, args.GetCount() - 5);
			CVarList s2s_msg;
			s2s_msg << COMMAND_TEAMOFFER_MSG
					<< S2S_TEAMOFFER_SUB_MSG_ENTRY
					<< args.IntVal(4);

			LoopBeginCheck(a);
			for (int i = 0; i < (int)mem_list.GetCount(); ++i)
			{
				LoopDoCheck(a);
				pKernel->CommandByName(mem_list.WideStrVal(i), s2s_msg);
			}			
		}
		break;
	case P2S_TEAMOFFER_SUB_MSG_RESULT:	// 组队悬赏结果
		m_pTeamOfferModule->OnResult(pKernel, args);
		break;
	default:
		break;
	}

	return 0;
}

const std::wstring & TeamOfferModule::GetDomainName(IKernel * pKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Teamoffer_%d", server_id);
		m_domainName = wstr;
	}
	return m_domainName;
}
