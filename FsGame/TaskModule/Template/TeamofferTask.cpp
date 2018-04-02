// -------------------------------------------
// 文件名称： TeamofferTask.cpp
// 文件说明： 缉盗任务
// 创建日期： 2018/04/02
// 创 建 人：  tzt
// -------------------------------------------

#include "TeamofferTask.h"
#include "../Data/TaskLoader.h"
#include "..\TaskUtil.h"
#include "FsGame\Define\GameDefine.h"
#include "../TaskManager.h"
#include "Define/CommandDefine.h"
#include "SystemFunctionModule/ResetTimerModule.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"

TeamofferTask::TeamofferTask()
{
	m_task_type = TASK_TYPE_TEAMOFFER;
}

// 初始化
bool TeamofferTask::Init(IKernel *pKernel)
{
	// 功能开启
	pKernel->AddIntCommandHook("player", COMMAND_MSG_ACTIVATE_FUNCTION, TeamofferTask::OnCommandActiveFunc);

	DECL_RESET_TIMER(RESET_DAILY_TASK, TeamofferTask::ResetTask);
	return true;
}

// 开启
bool TeamofferTask::Start(IKernel *pKernel, const PERSISTID &self)
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

	// 已开启
	if (!ActivateFunctionModule::m_pInstance->CheckActivateFunction(pKernel, self, AFM_TEAMOFFER_TASK))
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

	return true;
}

// 注册定时器
void TeamofferTask::RegistResetTimer(IKernel *pKernel, const PERSISTID &self)
{
	// 定时器注册
	REGIST_RESET_TIMER(pKernel, self, RESET_TEAMOFFER_TASK);
}

// 任务提交后
void TeamofferTask::OnSubmit(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{

}

// 功能开启 
int TeamofferTask::OnCommandActiveFunc(IKernel *pKernel, const PERSISTID &self,
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int func_type = args.IntVal(1);
	if (func_type != AFM_TEAMOFFER_TASK)
	{
		return 0;
	}

	// 模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_TEAMOFFER);
	if (NULL == pTemplate)
	{
		return 0;
	}

	// 开启
	pTemplate->Start(pKernel, self);

	return 0;
}

// 缉盗任务定时器
int TeamofferTask::ResetTask(IKernel *pKernel, const PERSISTID &self, int slice)
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
	const TaskConfig *task_config = TaskLoader::instance().GetTaskConfig(TASK_TYPE_TEAMOFFER);
	if (NULL == task_config)
	{
		return 0;
	}

	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_TEAMOFFER);
	if (NULL == pTemplate)
	{
		return 0;
	}

	const int exist_row = pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, TASK_TYPE_TEAMOFFER);

	// 未开启，进行一次开启检测
	if (exist_row < 0)
	{
		pTemplate->Start(pKernel, self);
	}
	else // 已开启，重置次数
	{
		pConfRec->SetInt(exist_row, COLUMN_TASK_CONFIG_REC_COUNT, 0);
	}

	return 0;
}

// 接取后置任务
void TeamofferTask::AcceptPostTask(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{

}
