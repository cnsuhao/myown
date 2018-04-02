// -------------------------------------------
// 文件名称：	 TaskManager.h
// 文件说明： 任务管理模块
// 创建日期： 2018/02/24
// 创 建 人：  tzt
// -------------------------------------------

#include "TaskManager.h"
#include "Data/TaskLoader.h"
#include "CommonModule\ReLoadConfigModule.h"
#include "utils\extend_func.h"
#include "Template\MainTask.h"
#include "Define\CommandDefine.h"
#include "TaskUtil.h"
#include "FsGame\Define\GameDefine.h"
#include "Rule/KillNpcRule.h"
#include "Define/ClientCustomDefine.h"
#include "Template/DailyTask.h"
#include "Template/BranchTask.h"
#include "Rule/TalkRule.h"
#include "Rule/GatherRule.h"
#include "Rule/PathfindRule.h"
#include "Rule/EntrySceneRule.h"
#include "Template/TeamofferTask.h"

std::wstring TaskManager::m_domainName = L"";
TaskManager *TaskManager::m_pThis = NULL;

// 初始化
bool TaskManager::Init(IKernel *pKernel)
{
	m_pThis = this;
	// 客户端准备就绪
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnReady", TaskManager::OnReady);
	// 离开场景
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnLeaveScene", TaskManager::OnLeaveScence);
	// 上线
	pKernel->AddEventCallback(CLASS_NAME_PLAYER, "OnRecover", TaskManager::OnPlayerRecover);
	// 任务动作
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_TASK_ACTION_MSG, TaskManager::OnCommandTaskAction, INT_MIN);
	// 任务COMMAND消息
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_TASK_COMMAND, TaskManager::OnCommandTask);
	// 任务客户端消息
	pKernel->AddIntCustomHook(CLASS_NAME_PLAYER, CLIENT_CUSTOMMSG_TASK_MSG, TaskManager::OnCustomTask);
	// 玩家移动
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_PLAYER_MOTION, TaskManager::OnCommandMotion);
	// 玩家升级
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_LEVELUP_CHANGE, TaskManager::OnCommandLevelUp);
	// 击杀Npc
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_KILL_NPC_WITH_RULE, TaskManager::OnKillNpc);
	// 击杀玩家
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_KILL_PLAYER_WITH_RULE, TaskManager::OnKillPlayer);
	// CG播放结束
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_PLAY_CG_DONE, TaskManager::OnCommandCGDone);

	// 初始化任务模板
	if (!InitTemplates(pKernel))
	{
		Assert(false);
		return false;
	}

	// 初始化任务规则
	if (!InitRules(pKernel))
	{
		Assert(false);
		return false;
	}

	if (!LoadRes(pKernel))
	{
		Assert(false);
		return false;
	}

	// 工具类初始化
	if (!TaskUtilS::Instance()->Init(pKernel))
	{
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("AllTask", TaskManager::ReloadTask_All);
	RELOAD_CONFIG_REG("MainTask", TaskManager::ReloadTask_Main);
	RELOAD_CONFIG_REG("BranchTask", TaskManager::ReloadTask_Branch);
	RELOAD_CONFIG_REG("DailyTask", TaskManager::ReloadTask_Daily);
	RELOAD_CONFIG_REG("TeamofferTask", TaskManager::ReloadTask_Teamoffer);
#endif

	return true;
}

// 关闭
bool TaskManager::Shut(IKernel *pKernel)
{	
	return TaskLoader::instance().ReleaseRes();
}

// 加载资源文件
bool TaskManager::LoadRes(IKernel *pKernel)
{
	return TaskLoader::instance().LoadRes(pKernel);
}

// 注册任务模板
bool TaskManager::RegistTemplate(IKernel *pKernel, TaskTemplate *pTemplate)
{
	if (NULL == pTemplate)
	{
		return false;
	}

	// 非法类型
	if (pTemplate->GetTaskType() <= TASK_TYPE_NONE
		|| pTemplate->GetTaskType() >= TASK_TYPE_TOTAL)
	{
		delete pTemplate;
		extend_warning(pKernel, "Register Task Template Failed");
		return false;
	}
	const int t = pTemplate->GetTaskType();
	// 注册
	if (NULL == m_pTemplates[pTemplate->GetTaskType()])
	{
		if (pTemplate->Init(pKernel))
		{
			m_pTemplates[pTemplate->GetTaskType()] = pTemplate;
			return true;
		}

		return false;
	}

	return false;
}

// 初始化任务模板
bool TaskManager::InitTemplates(IKernel *pKernel)
{
	LoopBeginCheck(a);
	for (int i = TASK_TYPE_NONE; i < TASK_TYPE_TOTAL; ++i)
	{
		LoopDoCheck(a);
		m_pTemplates[i] = NULL;
	}

	// 主线任务
	if (!RegistTemplate(pKernel, NEW MainTask()))
	{
		return false;
	}

	// 支线任务
	if (!RegistTemplate(pKernel, NEW BranchTask()))
	{
		return false;
	}

	// 日常任务
	if (!RegistTemplate(pKernel, NEW DailyTask()))
	{
		return false;
	}

	// 缉盗任务
	if (!RegistTemplate(pKernel, NEW TeamofferTask()))
	{
		return false;
	}

	return true;
}

// 创建规则类
TaskBaseRule * TaskManager::CreateRule(const TaskRules rule)
{
	if (rule <= TASK_RULE_NONE 
		|| rule >= TASK_RULE_TOTAL)
	{
		return NULL;
	}

	switch (rule)
	{
	case TASK_RULE_KILL_NPC: // 击杀Npc		
		return NEW KillNpcRule;

	case TASK_RULE_DIALOGUE: // 对话	
		return NEW TalkRule;

	case TASK_RULE_PATHFINDING: // 寻路
		return NEW PathfindRule;

	case TASK_RULE_ENTRY_SCENE: // 进入场景
		return NEW EntrySceneRule;

	case TASK_RULE_GATHER:	 // 物品采集
	case TASK_RULE_USE_ITEM: // 物品使用
		return NEW GatherRule;

	default:
		return NEW TaskBaseRule;
	}
}

// 初始化任务规则
bool TaskManager::InitRules(IKernel *pKernel)
{
	for (int i = TASK_RULE_NONE + 1; i < TASK_RULE_TOTAL; ++i)
	{
		TaskBaseRule *pRule = CreateRule(TaskRules(i));
		if (NULL == pRule)
		{
			extend_warning(pKernel, "Create Task Rule Failed");
			return false;
		}

		if (!pRule->Init(pKernel))
		{
			delete pRule;
			return false;
		}

		m_pRules[i] = pRule;
	}

	return true;
}

// 取得任务模板
TaskTemplate* TaskManager::GetTemplate(const TaskTypes task_type)
{
	if (task_type > TASK_TYPE_NONE 
		&& task_type < TASK_TYPE_TOTAL)
	{
		return m_pTemplates[task_type];
	}

	return NULL;
}

// 取得任务规则
TaskBaseRule* TaskManager::GetRule(const TaskRules task_rule)
{
	if (task_rule > TASK_RULE_NONE
		&& task_rule < TASK_RULE_TOTAL)
	{
		return m_pRules[task_rule];
	}

	return NULL;
}

// 为所有任务注册定时器
void TaskManager::RegistResetTimer(IKernel *pKernel, const PERSISTID &self)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	LoopBeginCheck(a);
	for (int type = TASK_TYPE_NONE + 1; type < TASK_TYPE_TOTAL; ++type)
	{
		LoopDoCheck(a);
		TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(type));
		if (NULL == pTemplate)
		{
			continue;
		}

		pTemplate->RegistResetTimer(pKernel, self);
	}
}

// 获取任务domain
const std::wstring& TaskManager::GetDomainName(IKernel * pKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Task_%d", server_id);

		m_domainName = wstr;
	}

	return m_domainName;
}

// 任务动作
int TaskManager::OnCommandTaskAction(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int sub_msg = args.IntVal(1);
	const int task_id = args.IntVal(2);

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return 0;
	}

	// 任务模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pData->type));
	if (NULL == pTemplate)
	{
		return 0;
	}

	// 接取
	if (TASK_ACTION_ACCEPT == sub_msg)
	{
		pTemplate->OnAccept(pKernel, self, task_id);
	}

	// 完成
	if (TASK_ACTION_COMPLETE == sub_msg)
	{
		pTemplate->OnComplete(pKernel, self, task_id);
	}

	// 提交
	if (TASK_ACTION_SUBMIT == sub_msg)
	{
		pTemplate->OnSubmit(pKernel, self, task_id);
	}

	return 0;
}

// 客户端准备就绪
int TaskManager::OnReady(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 安全检查
	TaskUtilS::Instance()->MainTaskSafeCheck(pKernel, self);

	// 进入场景任务检查
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_ENTRY_SCENE
			<< 1;
	pKernel->Command(self, self, s2s_msg);

	// 自动播放cg检查
	TaskUtilS::Instance()->AutoPlayTaskCG(pKernel, self);

	return 0;
}

// 任务command消息
int TaskManager::OnCommandTask(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int sub_msg = args.IntVal(1);

	// 取参数
	CVarList var;
	var.Append(args, 2, args.GetCount() - 2);
	
	switch (sub_msg)
	{
	case S2S_TASK_SUBMSG_UPDATE: // 更新任务
		TaskUtilS::Instance()->UpdateTask(pKernel, self, var);
		break;
	default:
		break;
	}

	
	return 0;
}

// 客户端任务消息
int TaskManager::OnCustomTask(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int sub_msg = args.IntVal(1);

	// 参数
	CVarList var;
	var.Append(args, 2, args.GetCount() - 2);

	switch (sub_msg)
	{
	case C2S_TASK_SUBMSG_SUBMIT: // 提交任务
		TaskUtilS::Instance()->SubmitTask(pKernel, self, var);
		break;

	case C2S_TASK_SUBMSG_DIALOG_END: // 对话完成
		TaskUtilS::Instance()->UpdateTask(pKernel, self, 
			CVarList() << TASK_OPT_WAY_BY_ID << var);
		break;

	case C2S_TASK_SUBMSG_SET_PATHFINDING: // 通知任务自动寻路状态
		TaskUtilS::Instance()->SetAutoPathFind(pKernel, self, var);
		break;

	case C2S_TASK_SUBMSG_GATHER: // 采集完成
		TaskUtilS::Instance()->UpdateTask(pKernel, self, 
			CVarList() << TASK_OPT_WAY_BY_RULE << TASK_RULE_GATHER << var);
		break;

	case C2S_TASK_SUBMSG_USE_ITEM: // 物品使用完成
		TaskUtilS::Instance()->UpdateTask(pKernel, self,
			CVarList() << TASK_OPT_WAY_BY_ID << var);
		break;

	case C2S_TASK_SUBMSG_SWITCH_GROUP: // 请求开始分组副本
		TaskUtilS::Instance()->CreateGroupClone(pKernel, self, var.IntVal(0));
		break;

	case C2S_TASK_SUBMSG_DETAIL_INFO: // 请求某种任务信息
		// TODO
		break;
	default:
		break;
	}

	return 0;
}

// 玩家从数据库恢复数据完成（玩家上线）
int TaskManager::OnPlayerRecover(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 注册定时器
	TaskManager::m_pThis->RegistResetTimer(pKernel, self);

	return 0;
}

// 玩家移动
int TaskManager::OnCommandMotion(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 检查寻路任务
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_PATHFINDING
			<< 1;
	pKernel->Command(self, self, s2s_msg);

	return 0;
}

// 等级提升
int TaskManager::OnCommandLevelUp(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 检查升级任务
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_UPGRADE_LEVEL
			<< pSelf->QueryInt(FIELD_PROP_LEVEL);
	pKernel->Command(self, self, s2s_msg);

	return 0;
}

// 玩家离开场景
int TaskManager::OnLeaveScence(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 任务副本清除
	if (pSelf->FindData("CloneTask"))
	{
		TaskUtilS::Instance()->CleanGroupClone(pKernel, self, pSelf->QueryDataInt("CloneTask"));
	}

	return 0;
}

// NPC被杀时回调
int TaskManager::OnKillNpc(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	TaskUtilS::Instance()->UpdateKillNpc(pKernel, self, sender);

	return 0;
}

// 玩家被击杀回调
int TaskManager::OnKillPlayer(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 击杀玩家任务处理
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_KILL_PLAYER
			<< 1;
	pKernel->Command(self, self, s2s_msg);

	return 0;
}

// CG播放结束消息
int TaskManager::OnCommandCGDone(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	TaskUtilS::Instance()->UpdateTask(pKernel, self,
		CVarList() << TASK_OPT_WAY_BY_RULE << TASK_RULE_PLAY_CG << args.StringVal(1));

	return 0;
}

// 重新加载所有任务
void TaskManager::ReloadTask_All(IKernel* pKernel)
{
	TaskLoader::instance().ReloadRes(pKernel);
}

// 重新加载主线任务
void TaskManager::ReloadTask_Main(IKernel* pKernel)
{
	TaskLoader::instance().ReloadRes(pKernel, TASK_TYPE_MAIN);
}

// 重新加载支线任务
void TaskManager::ReloadTask_Branch(IKernel* pKernel)
{
	TaskLoader::instance().ReloadRes(pKernel, TASK_TYPE_BRANCH);
}

// 重新加载日常任务
void TaskManager::ReloadTask_Daily(IKernel* pKernel)
{
	TaskLoader::instance().ReloadRes(pKernel, TASK_TYPE_DAILY);
}

// 重新加载缉盗任务
void TaskManager::ReloadTask_Teamoffer(IKernel* pKernel)
{
	TaskLoader::instance().ReloadRes(pKernel, TASK_TYPE_TEAMOFFER);
}
