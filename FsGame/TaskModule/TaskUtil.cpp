// -------------------------------------------
// 文件名称： TaskUtil.h
// 文件说明： 任务工具类
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#include "TaskUtil.h"
#include "utils\extend_func.h"
#include "TaskManager.h"
#include "FsGame/Define/GameDefine.h"
#include "Data\TaskLoader.h"
#include "utils\string_util.h"
#include "utils\util_func.h"
#include "Define\CommandDefine.h"
#include "CommonModule\AsynCtrlModule.h"
#include "CommonModule\ActionMutex.h"
#include "utils\custom_func.h"
#include "NpcBaseModule\NpcCreatorModule.h"
#include "CommonModule\CommRuleModule.h"
#include "Data\TaskData_Simple.h"

// 初始化
bool TaskUtil::Init(IKernel *pKernel)
{
	DECL_HEARTBEAT(TaskUtil::HB_DelayUpdateKillNpc);
	return true;
}

// 查询任务状态
const TaskStatus TaskUtil::QueryTaskStatus(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return TASK_STATUS_MAX;
	}

	// 进度表
	IRecord *pPrgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);
	if (NULL == pPrgRec)
	{
		return TASK_STATUS_MAX;
	}

	// 提交表
	IRecord *pSubmitRec = pSelf->GetRecord(FIELD_RECORD_TASK_SUBMIT_REC);
	if (NULL == pSubmitRec)
	{
		return TASK_STATUS_MAX;
	}

	// 查找当前任务
	const int prog_row = pPrgRec->FindInt(COLUMN_TASK_PROGRESS_REC_ID, task_id);
	if (prog_row >= 0)
	{
		return (TaskStatus)pPrgRec->QueryInt(prog_row, COLUMN_TASK_PROGRESS_REC_STATUS);
	}

	// 不存在，则检查完成表
	if (pSubmitRec->FindInt(COLUMN_TASK_SUBMIT_REC_ID, task_id) >= 0)
	{
		// 已提交
		return TASK_STATUS_SUBMITTED;
	}

	return TASK_STATUS_MAX;
}

// 查询当前任务次数
const int TaskUtil::QueryTaskNum(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 配置表
	IRecord *pConfRec = pSelf->GetRecord(FIELD_RECORD_TASK_CONFIG_REC);
	if (NULL == pConfRec)
	{
		return 0;
	}

	const int exist_row = pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, task_type);
	if (exist_row < 0)
	{
		return 0;
	}

	return pConfRec->QueryInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT);
}

// 查询任务总次数
const int TaskUtil::QueryTaskTotalNum(const int task_type)
{
	const TaskConfig *pConfig = TaskLoader::instance().GetTaskConfig(task_type);
	if (NULL == pConfig)
	{
		return 0;
	}

	return pConfig->limit_num;
}

// 前置任务是否已提交
bool TaskUtil::IsPrevTaskSubmit(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 提交表
	IRecord *pSubmitRec = pSelf->GetRecord(FIELD_RECORD_TASK_SUBMIT_REC);
	if (NULL == pSubmitRec)
	{
		return false;
	}

	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	if (pData->prev_task_list.empty())
	{
		return true;
	}

	// 前置任务
	const int prev_task_id = pData->prev_task_list[0];

	return pSubmitRec->FindInt(COLUMN_TASK_SUBMIT_REC_ID, prev_task_id) >= 0;
}

// 任务是否已开启
bool TaskUtil::IsStart(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 配置表
	IRecord *pConfRec = pSelf->GetRecord(FIELD_RECORD_TASK_CONFIG_REC);
	if (NULL == pConfRec)
	{
		return false;
	}

	return pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, task_type) >= 0;
}

// 任务是否有次数
bool TaskUtil::IsHaveNumOfType(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 配置表
	IRecord *pConfRec = pSelf->GetRecord(FIELD_RECORD_TASK_CONFIG_REC);
	if (NULL == pConfRec)
	{
		return false;
	}

	const TaskConfig *pConfig = TaskLoader::instance().GetTaskConfig(task_type);
	if (NULL == pConfig)
	{
		return false;
	}

	const int exist_row = pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, task_type);
	if (exist_row < 0)
	{
		return false;
	}

	// 当前次数
	const int cur_count = pConfRec->QueryInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT);

	return cur_count < pConfig->limit_num;
}

// 当前是否有指定类型的任务
bool TaskUtil::IsHaveOfType(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 进度表
	IRecord *pPrgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);
	if (NULL == pPrgRec)
	{
		return false;
	}

	LoopBeginCheck(a);
	for (int row = 0; row < pPrgRec->GetRows(); ++row)
	{
		LoopDoCheck(a);
		const int task_id = pPrgRec->QueryInt(row, COLUMN_TASK_PROGRESS_REC_ID);
		const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
		if (NULL == pData)
		{
			continue;
		}

		if (pData->type == task_type)
		{
			return true;
		}
	}

	return false;
}

// 当前是否有指定规则的任务
bool TaskUtil::IsHaveOfRule(IKernel *pKernel, const PERSISTID &self, 
	const int task_rule)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 进度表
	IRecord *pPrgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);
	if (NULL == pPrgRec)
	{
		return false;
	}

	LoopBeginCheck(a);
	for (int row = 0; row < pPrgRec->GetRows(); ++row)
	{
		LoopDoCheck(a);
		const int task_id = pPrgRec->QueryInt(row, COLUMN_TASK_PROGRESS_REC_ID);
		const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
		if (NULL == pData)
		{
			continue;
		}

		if (pData->rule == task_rule)
		{
			return true;
		}
	}

	return false;
}

// 随机指定任务类型的任务
const int TaskUtil::RandomTask(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 玩家等级
	const int self_level = pSelf->QueryInt(FIELD_PROP_LEVEL);

	// 取得该类型的任务表
	const TaskBaseDataS *pTaskTable = TaskLoader::instance().GetTaskTableByType(task_type);
	if (NULL == pTaskTable)
	{
		return 0;
	}

	TaskBaseDataS::const_iterator start_it = pTaskTable->begin();
	TaskBaseDataS::const_iterator end_it = pTaskTable->end();

	NumList task_list;
	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		if (self_level < start_it->second->accept_min_level 
			|| self_level > start_it->second->accept_max_level)
		{
			continue;
		}

		task_list.push_back(start_it->first);
	}

	if (task_list.empty())
	{
		return 0;
	}

	const int idx = util_random_int((int)task_list.size());
	return task_list[idx];
}

// 记录任务完成次数
void TaskUtil::RecordTaskNum(IKernel *pKernel, const PERSISTID &self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 配置表
	IRecord *pConfRec = pSelf->GetRecord(FIELD_RECORD_TASK_CONFIG_REC);
	if (NULL == pConfRec)
	{
		return;
	}

	const int exist_row = pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, task_type);
	if (exist_row < 0)
	{
		return;
	}

	// 当前次数
	int cur_count = pConfRec->QueryInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT);
	++cur_count;
	pConfRec->SetInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT, cur_count);
}

// 接取任务
bool TaskUtil::AcceptTask(IKernel *pKernel, const PERSISTID &self, const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pData->type));
	if (NULL == pTemplate)
	{
		return false;
	}

	if (pTemplate->CanAccept(pKernel, self, task_id))
	{
		return pTemplate->DoAccept(pKernel, self, task_id);
	}

	return false;
}

// 接取后置任务
void TaskUtil::AcceptPostTask(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return;
	}

	// 取得后续任务列表
	const NumList &post_task_list = pData->post_task_list;

	// 接受后续任务
	LoopBeginCheck(a);
	for (int i = 0; i < (int)post_task_list.size(); ++i)
	{
		LoopDoCheck(a);
		// 任务数据
		const int post_task_id = post_task_list[i];
		const TaskBaseData *pPostData = TaskLoader::instance().GetTaskBase(post_task_id);
		if (NULL == pPostData)
		{
			continue;
		}

		TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pPostData->type));
		if (NULL == pTemplate)
		{
			continue;
		}

		if (pTemplate->CanAccept(pKernel, self, post_task_id))
		{
			pTemplate->DoAccept(pKernel, self, post_task_id);
		}
	}
}

// 清除指定类型的任务
bool TaskUtil::CleanTaskByType(IKernel* pKernel, const PERSISTID& self, 
	const int task_type)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 进度表
	IRecord *pPrgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);
	if (NULL == pPrgRec)
	{
		return false;
	}

	LoopBeginCheck(a);
	for (int row = pPrgRec->GetRows() - 1; row >= 0; --row)
	{
		LoopDoCheck(a);
		const int task_id = pPrgRec->QueryInt(row, COLUMN_TASK_PROGRESS_REC_ID);
		const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
		if (NULL == pData)
		{
			continue;
		}

		if (pData->type == task_type)
		{
			pPrgRec->RemoveRow(row);
		}
	}

	return true;
}

// 当前主线任务安全检查
void TaskUtil::MainTaskSafeCheck(IKernel *pKernel, const PERSISTID &self)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 主线模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_MAIN);
	if (NULL == pTemplate)
	{
		return;
	}

	// 当前主线
	const int cur_main_task = pSelf->QueryInt(FIELD_PROP_CUR_MAIN_TASK);

	// 当前主线的状态
	const int task_status = QueryTaskStatus(pKernel, self, cur_main_task);

	// 无此任务，需要重新接取
	if (TASK_STATUS_MAX == task_status)
	{
		// 重新接受该任务
		if (pTemplate->CanAccept(pKernel, self, cur_main_task))
		{
			pTemplate->DoAccept(pKernel, self, cur_main_task);
			extend_warning(LOG_WARNING, "[%s][%d] Accept Main Task:[%d]", __FILE__, __LINE__, cur_main_task);
			return;
		}
	}	
	else if (TASK_STATUS_SUBMITTED == task_status)// 该任务已提交，检查后续
	{
		AcceptPostTask(pKernel, self, cur_main_task);
	}	
}

// 任务规则自检测
void TaskUtil::SelfCheck(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return;
	}

	// 任务规则
	TaskBaseRule *pRule = TaskManager::m_pThis->GetRule(TaskRules(pData->rule));
	if (NULL == pRule)
	{
		return;
	}

	pRule->ExecCheck(pKernel, self, task_id);
}

// 任务自动切场景
void TaskUtil::AutoSwitchScene(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return;
	}

	// 无需切换场景
	if (!pData->IsSwitchScene() 
		|| pData->scene_list.empty())
	{
		return;
	}

	// 任务状态
	const int task_status = QueryTaskStatus(pKernel, self, task_id);
	if (TASK_STATUS_DOING != task_status)
	{
		return;
	}

	// 取第一个场景
	const int dest_scene = pData->scene_list[0];

	// 无需切换坐标
	if (pData->IsSwitchPosition() 
		&& !pData->posi_list.empty())
	{
		// 取第一个坐标
		const Position& position = pData->posi_list[0];

		// 同场景切换
		if (pKernel->GetSceneId() == dest_scene)
		{
			pKernel->MoveTo(self, position.x, 
				pKernel->GetMapHeight(position.x, position.z), position.z, position.o);
		}
		else
		{
			AsynCtrlModule::m_pAsynCtrlModule->SwitchLocate(pKernel, self,
				dest_scene, position.x, position.y, position.z, position.o);
		}		
	}
	else
	{
		AsynCtrlModule::m_pAsynCtrlModule->SwitchBorn(pKernel, self, dest_scene);
	}
}

// 自动播放主线任务cg
void TaskUtil::AutoPlayTaskCG(IKernel *pKernel, const PERSISTID &self)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 当前主线任务
	const int cur_main_task = pSelf->QueryInt(FIELD_PROP_CUR_MAIN_TASK);

	// 不在进行中
	if (QueryTaskStatus(pKernel, self, cur_main_task) != TASK_STATUS_DOING)
	{
		return;
	}

	const TaskData_Simple *pData = (TaskData_Simple *)TaskLoader::instance().GetTaskBase(cur_main_task);
	if (NULL == pData)
	{
		return;
	}

	if (pData->rule != TASK_RULE_PLAY_CG)
	{
		return;
	}

	if (!IsTargetScene(pKernel, pData->scene_list))
	{
		return;
	}

	// 播放CG
	CommRuleModule::m_pThis->PlayCG(pKernel, self, pData->add_info.c_str());
}

// 创建任务副本
bool TaskUtil::CreateGroupClone(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 是否有分组副本
	if (!pData->IsGrouping())
	{
		return false;
	}

	// 任务是否进行中
	if (QueryTaskStatus(pKernel, self, task_id) != TASK_STATUS_DOING)
	{
		return false;
	}

	// 是否有互斥
	if (ActionMutexModule::IsHaveAction(pKernel, self))
	{
		return false;
	}

	// 新分组
	const int new_group_id = pKernel->NewGroupId();

	// 使用刷怪器创建副本怪（刷怪器属性IsDestroy必须保证为0）
	const PERSISTID creator = NpcCreatorModule::m_pNpcCreatorModule->CreateTaskrCreator(
		pKernel, pKernel->GetScene(), task_id, new_group_id, pSelf->QueryInt(FIELD_PROP_LEVEL));
	IGameObj *pCreator = pKernel->GetGameObj(creator);
	if (NULL == pCreator)
	{
		return false;
	}

	// 保证刷怪器不自动清除怪物
	pCreator->SetInt(FIELD_PROP_IS_DESTROY, 0);

	// 分组副本创建成功	
	//ADD_DATA_OBJECT(pSelf, "TaskCreator");
	//pSelf->SetDataObject("TaskCreator", creator);
	ADD_DATA_INT(pSelf, "CloneTask");
	pSelf->SetDataInt("CloneTask", task_id);

	// 玩家进入分组
	CommRuleModule::m_pThis->ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_ENTRY, new_group_id);
	// 宠物分组同步
	// TODO

	// 添加互斥
	ActionMutexModule::NoteAction(pKernel, self, ACTION_IN_TASK_GROUP);

	
	return true;
}

// 清理任务副本（任务完成，离开场景，下线时处理）
void TaskUtil::CleanGroupClone(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 不在分组副本中
	if (!ActionMutexModule::IsThisAction(pKernel, self, ACTION_IN_TASK_GROUP))
	{
		return;
	}

	// 是否任务副本
	if (!pSelf->FindData("CloneTask"))
	{
		return;
	}

	// 副本任务id
	const int clone_task_id = pSelf->QueryDataInt("CloneTask");

	// 目标任务不属于当前副本任务
	if (clone_task_id != task_id)
	{
		return;
	}

	// 不在副本中
	const int cur_group_id = pSelf->QueryInt(FIELD_PROP_GROUP_ID);
	if (cur_group_id <= 0)
	{
		return;
	}

	// 离开分组
	CommRuleModule::m_pThis->ChangeGroup(pKernel, self, GROUP_CHANGE_TYPE_QUIT);

	// 宠物同步分组
	// TODO

	// 清除副本任务id
	REMOVE_DATA_TYPE(pSelf, "CloneTask");

	// 解除互斥
	ActionMutexModule::CleanAction(pKernel, self);

	// 清除分组
	pKernel->ClearGroup(cur_group_id);
}

// 更新任务
void TaskUtil::UpdateTask(IKernel *pKernel, const PERSISTID &self, 
	const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 进度表
	IRecord *pPrgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);
	if (NULL == pPrgRec)
	{
		return;
	}

	// 方式
	const int opt_way = args.IntVal(0);
	
	// 按任务规则更新
	if (TASK_OPT_WAY_BY_RULE == opt_way)
	{
		// 更新规则
		const int target_rule = args.IntVal(1);

		// 取参数
		CVarList var;
		var.Append(args, 2, args.GetCount() - 2);

		LoopBeginCheck(a);
		for (int i = 0; i < pPrgRec->GetRows(); ++i)
		{
			LoopDoCheck(a);
			// 任务数据
			const int task_id = pPrgRec->QueryInt(i, COLUMN_TASK_PROGRESS_REC_ID);
			const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
			if (NULL == pData)
			{
				continue;
			}

			if (pData->rule != target_rule)
			{
				continue;
			}

			// 模板
			TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pData->type));
			if (NULL == pTemplate)
			{
				continue;
			}

			if (pTemplate->CanUpdate(pKernel, self, task_id))
			{
				pTemplate->DoUpdate(pKernel, self, task_id, var);
			}
		}
	}

	// 按任务id更新
	if (TASK_OPT_WAY_BY_ID == opt_way)
	{
		// 更新id
		const int task_id = args.IntVal(1);

		// 取参数
		CVarList var;
		var.Append(args, 2, args.GetCount() - 2);

		// 任务数据
		const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
		if (NULL == pData)
		{
			return;
		}

		// 模板
		TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pData->type));
		if (NULL == pTemplate)
		{
			return;
		}

		if (pTemplate->CanUpdate(pKernel, self, task_id))
		{
			pTemplate->DoUpdate(pKernel, self, task_id, var);
		}
	}

	// 按任务类型更新
	// TODO
}

// 提交任务
void TaskUtil::SubmitTask(IKernel *pKernel, const PERSISTID &self, 
	const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	const int task_id = args.IntVal(0);
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return;
	}

	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TaskTypes(pData->type));
	if (NULL == pTemplate)
	{
		return;
	}

	// 提交
	if (pTemplate->CanSubmit(pKernel, self, task_id))
	{
		pTemplate->DoSubmit(pKernel, self, task_id);
	}
}

// 设置自动寻路临时状态
void TaskUtil::SetAutoPathFind(IKernel *pKernel, const PERSISTID &self, 
	const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 获取自动寻路状态
	const int auto_pathfind = args.IntVal(0);

	ADD_DATA_INT(pSelf, "AutoWalk");
	pSelf->SetDataInt("AutoWalk", auto_pathfind);

	// 传送门先触发onSpring
	// 自动寻路停止，玩家在传送门中。主动触发onSpring
	if (pSelf->FindData("InDoorNPC") 
		&& auto_pathfind == AUTO_PATHFINDING_NO)
	{
		// 重新触发传送门
		PERSISTID door_npc = pSelf->QueryDataObject("InDoorNPC");
		if (pKernel->Exists(door_npc))
		{
			pKernel->Command(self, door_npc, CVarList() << COMMAND_NPC_AUTO_TRANSFER);
		}
	}
}

// 更新杀怪任务
void TaskUtil::UpdateKillNpc(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &npc)
{
	// 被杀者和凶手
	IGameObj *pNpc = pKernel->GetGameObj(npc);
	if (NULL == pNpc)
	{
		return;
	}

	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}


	// 取NpcID
	const char *npc_id = pNpc->GetConfig();
	if (StringUtil::CharIsNull(npc_id))
	{
		return;
	}

	// 有死亡时间
	const int dead_time = pNpc->QueryInt(FIELD_PROP_NPC_DEAD_TIME);
	if (dead_time > 0)
	{
		// 需延迟Npc
		ADD_DATA_STRING(pSelf, "TempNpcID");
		pSelf->SetDataString("TempNpcID", npc_id);

		// 延迟心跳
		ADD_COUNT_BEAT(pKernel, self, "TaskUtil::HB_DelayUpdateKillNpc", dead_time, 1);

		return;
	}

	// 击杀Npc任务处理
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_KILL_NPC
			<< npc_id
			<< 1;

	pKernel->Command(self, self, s2s_msg);

}

// 转换CoupleElementList为string
bool TaskUtil::ConvertCEL2Str(const CoupleElementList& src_list, std::string& target_str, 
	bool zero_value /*= false*/, const char *del1 /*= ":"*/, const char *del2 /*= ","*/)
{
	target_str = "";

	CoupleElementList::const_iterator start_it = src_list.begin();
	CoupleElementList::const_iterator end_it = src_list.end();

	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		if (!target_str.empty())
		{
			target_str.append(del2);
		}

		target_str.append(start_it->id);
		target_str.append(del1);

		const int value = zero_value ? 0 : start_it->num;
		target_str.append(StringUtil::IntAsString(value));
	}

	return true;
}

// 转换string为CoupleElementList
bool TaskUtil::ConvertStr2CEL(const std::string& src_str, CoupleElementList& target_list, 
	const char *del1 /*= ":"*/, const char *del2 /*= ","*/)
{
	target_list.clear();

	CVarList element_list;
	util_split_string(element_list, src_str, del2);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)element_list.GetCount(); ++i)
	{
		LoopDoCheck(a);
		CVarList result;
		util_split_string(result, element_list.StringVal(i), del1);

		if (result.GetCount() < 2)
		{
			continue;
		}

		CoupleElement element(result.StringVal(0));
		element.num = result.IntVal(1);
		target_list.push_back(element);
	}

	return true;
}

// 任务场景验证
bool TaskUtil::IsTargetScene(IKernel *pKernel, const NumList& scene_list, 
	const int cur_scene /*= 0*/)
{
	if (scene_list.empty())
	{
		return true;
	}

	// 需验证场景
	int target_scene = cur_scene;
	if (0 == target_scene)
	{
		target_scene = pKernel->GetSceneId();
	}

	NumList::const_iterator find_it = find(
		scene_list.begin(),
		scene_list.end(),
		target_scene);

	// 不匹配
	if (find_it == scene_list.end())
	{
		return false;
	}

	return true;
}

// 坐标验证
bool TaskUtil::IsTargetPosition(IKernel *pKernel, const PositionList& position_list, 
	const Position& position, const float distance/* = COMMON_VERIFY_DISTANCE*/)
{
	// 不需要判断位置
	if (position_list.empty())
	{
		return true;
	}
		
	bool verify = false;

	// 需要判断位置
	LoopBeginCheck(a);
	for (int i = 0; i < (int)position_list.size(); ++i)
	{
		LoopDoCheck(a);
		const Position& temp = position_list[i];
		if (::util_dot_distance(position.x, position.z, temp.x, temp.z) <= COMMON_VERIFY_DISTANCE)
		{
			verify = true;
			break;
		}
	}

	return verify;
}

// 延迟更新击杀Npc任务心跳
int TaskUtil::HB_DelayUpdateKillNpc(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 无记录
	if (!pSelfObj->FindData("TempNpcID"))
	{
		return 0;
	}

	// 延迟处理的Npc
	const char *npc_id = pSelfObj->QueryDataString("TempNpcID");
	if (StringUtil::CharIsNull(npc_id))
	{
		return 0;
	}

	// 击杀Npc类任务处理
	CVarList args_list;
	args_list << COMMAND_TASK_COMMAND
			  << S2S_TASK_SUBMSG_UPDATE
			  << TASK_RULE_KILL_NPC
			  << npc_id
			  << 1;

	pKernel->Command(self, self, args_list);

	return 0;
}
