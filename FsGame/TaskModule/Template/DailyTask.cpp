// -------------------------------------------
// 文件名称： DailyTask.cpp
// 文件说明： 日常任务
// 创建日期： 2018/02/28
// 创 建 人：  tzt
// -------------------------------------------

#include "DailyTask.h"
#include "../Data/TaskLoader.h"
#include "..\TaskUtil.h"
#include "FsGame\Define\GameDefine.h"
#include "../TaskManager.h"
#include "Define/CommandDefine.h"
#include "SystemFunctionModule/ResetTimerModule.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"

DailyTask::DailyTask()
{
	m_task_type = TASK_TYPE_DAILY;
}

// 初始化
bool DailyTask::Init(IKernel *pKernel)
{
	// 功能开启
	pKernel->AddIntCommandHook("player", COMMAND_MSG_ACTIVATE_FUNCTION, DailyTask::OnCommandActiveFunc);

	DECL_RESET_TIMER(RESET_DAILY_TASK, DailyTask::ResetTask);
	return true;
}

// 开启
bool DailyTask::Start(IKernel *pKernel, const PERSISTID &self)
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

	// 任务配置
	const TaskConfig *task_config = TaskLoader::instance().GetTaskConfig(m_task_type);
	if (NULL == task_config)
	{
		return false;
	}

	// 功能未开启
	if (!ActivateFunctionModule::m_pInstance->CheckActivateFunction(pKernel, self, AFM_DAILY_TASK))
	{
		return false;
	}

	// 已开启
	if (pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, m_task_type) >= 0)
	{
		return false;
	}

	// 加入配置表
	CVarList row_value;
	row_value << m_task_type
			  << 0;
	pConfRec->AddRowValue(-1, row_value);

	// 随机任务
	const int task_id = TaskUtilS::Instance()->RandomTask(pKernel, self, m_task_type);

	// 接取任务
	if (CanAccept(pKernel, self, task_id))
	{
		return DoAccept(pKernel, self, task_id);
	}

	return false;
}

// 可否接取任务
bool DailyTask::CanAccept(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	// 日常任务接取条件
	// 是否开启
	if (!TaskUtilS::Instance()->IsStart(pKernel, self, m_task_type))
	{
		return false;
	}

	// 次数是否上限
	if (!TaskUtilS::Instance()->IsHaveNumOfType(pKernel, self, m_task_type))
	{
		return false;
	}

	// 是否有同类型任务
	if (TaskUtilS::Instance()->IsHaveOfType(pKernel, self, m_task_type))
	{
		return false;
	}
	
	return TaskTemplate::CanAccept(pKernel, self, task_id);
}

// 任务提交后
void DailyTask::OnSubmit(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 后续任务处理
	AcceptPostTask(pKernel, self, task_id);
}

// 注册定时器
void DailyTask::RegistResetTimer(IKernel *pKernel, const PERSISTID &self)
{
	// 定时器注册
	REGIST_RESET_TIMER(pKernel, self, RESET_DAILY_TASK);
}

// 功能开启 
int DailyTask::OnCommandActiveFunc(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int func_type = args.IntVal(1);
	if (func_type != AFM_DAILY_TASK)
	{
		return 0;
	}

	// 日常模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_DAILY);
	if (NULL == pTemplate)
	{
		return 0;
	}

	// 开启日常
	pTemplate->Start(pKernel, self);

	return 0;
}

// 日常任务定时器
int DailyTask::ResetTask(IKernel *pKernel, const PERSISTID &self, int slice)
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

	// 任务配置
	const TaskConfig *task_config = TaskLoader::instance().GetTaskConfig(TASK_TYPE_DAILY);
	if (NULL == task_config)
	{
		return 0;
	}

	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_DAILY);
	if (NULL == pTemplate)
	{
		return 0;
	}

	const int exist_row = pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, TASK_TYPE_DAILY);

	// 未开启，进行一次开启检测
	if (exist_row < 0)
	{
		pTemplate->Start(pKernel, self);
	}
	else // 已开启，重置次数
	{
		pConfRec->SetInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT, 0);

		// 随机任务
		const int task_id = TaskUtilS::Instance()->RandomTask(pKernel, self, TASK_TYPE_DAILY);

		// 接取任务
		if (pTemplate->CanAccept(pKernel, self, task_id))
		{
			pTemplate->DoAccept(pKernel, self, task_id);
		}
	}

	return 0;
}

// 接取后置任务
void DailyTask::AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 后续任务处理
	const int next_task_id = TaskUtilS::Instance()->RandomTask(pKernel, self, m_task_type);
	if (CanAccept(pKernel, self, next_task_id))
	{
		DoAccept(pKernel, self, next_task_id);
	}
}
