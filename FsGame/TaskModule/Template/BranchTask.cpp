// -------------------------------------------
// 文件名称： BranchTask.cpp
// 文件说明： 支线任务
// 创建日期： 2018/02/28
// 创 建 人：  tzt
// -------------------------------------------

#include "BranchTask.h"
#include "../Data/TaskLoader.h"
#include "..\TaskUtil.h"
#include "FsGame\Define\GameDefine.h"
#include "../TaskManager.h"

BranchTask::BranchTask()
{
	m_task_type = TASK_TYPE_BRANCH;
}

// 初始化
bool BranchTask::Init(IKernel *pKernel)
{
	// 客户端准备就绪
	pKernel->AddEventCallback("player", "OnReady", BranchTask::OnReady, -10);
	return true;
}

// 开启
bool BranchTask::Start(IKernel *pKernel, const PERSISTID &self)
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

	if (pConfRec->FindInt(COLUMN_TASK_CONFIG_REC_TYPE, m_task_type) < 0)
	{
		// 加入配置表
		CVarList row_value;
		row_value << TASK_TYPE_BRANCH
				  << 0;
		pConfRec->AddRowValue(-1, row_value);
	}
	
	return true;
}

// 任务提交后
void BranchTask::OnSubmit(IKernel *pKernel, const PERSISTID &self,
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

// 客户端准备就绪
int BranchTask::OnReady(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 支线模板
	TaskTemplate *pTemplate = TaskManager::m_pThis->GetTemplate(TASK_TYPE_BRANCH);
	if (NULL == pTemplate)
	{
		return 0;
	}

	// 开启支线
	pTemplate->Start(pKernel, self);

	return 0;
}

// 接取后置任务
void BranchTask::AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	// 后续任务处理
	TaskUtilS::Instance()->AcceptPostTask(pKernel, self, task_id);
}
