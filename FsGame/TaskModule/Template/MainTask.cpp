// -------------------------------------------
// 文件名称： MainTask.cpp
// 文件说明： 主线任务
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#include "MainTask.h"
#include "../Data/TaskLoader.h"
#include "..\TaskUtil.h"
#include "FsGame\Define\GameDefine.h"
#include "../TaskManager.h"

MainTask::MainTask()
{
	m_task_type = TASK_TYPE_MAIN;
}

// 初始化
bool MainTask::Init(IKernel *pKernel)
{
	// 客户端准备就绪
	pKernel->AddEventCallback("player", "OnReady", MainTask::OnReady, -10);
	return true;
}

// 开启
bool MainTask::Start(IKernel *pKernel, const PERSISTID &self)
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

	// 接取第一个主线任务
	int cur_main_task = pSelf->QueryInt(FIELD_PROP_CUR_MAIN_TASK);
	if (cur_main_task != 0)
	{
		return false;
	}

	// 任务配置
	const TaskConfig *task_config = TaskLoader::instance().GetTaskConfig(TASK_TYPE_MAIN);
	if (NULL == task_config)
	{
		return false;
	}

	if (pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, m_task_type) < 0)
	{
		// 加入配置表
		CVarList row_value;
		row_value << TASK_TYPE_MAIN
				  << 0;
		pConfRec->AddRowValue(-1, row_value);
	}	

	// 接取任务
	if (CanAccept(pKernel, self, task_config->first_task))
	{
		return DoAccept(pKernel, self, task_config->first_task);
	}

	return false;
}

// 可否接取任务
bool MainTask::CanAccept(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	// 主线任务接取条件
	// 已完成过
	if (TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id) == TASK_STATUS_SUBMITTED)
	{
		return false;
	}
	
	return TaskTemplate::CanAccept(pKernel, self, task_id);
}

// 任务提交后
void MainTask::OnSubmit(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 提交表
	IRecord *pSubmitRec = pSelf->GetRecord(FIELD_RECORD_TASK_SUBMIT_REC);
	if (NULL == pSubmitRec)
	{
		return;
	}

	// 记录到提交表中
	pSubmitRec->AddRowValue(-1, CVarList() << task_id);

	// 后续任务处理
	AcceptPostTask(pKernel, self, task_id);
}

// 客户端准备就绪
int MainTask::OnReady(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 主线模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_MAIN);
	if (NULL == pTemplate)
	{
		return 0;
	}

	// 开启主线
	pTemplate->Start(pKernel, self);

	return 0;
}

// 接取后置任务
void MainTask::AcceptPostTask(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	// 后续任务处理
	TaskUtilS::Instance()->AcceptPostTask(pKernel, self, task_id);
}
