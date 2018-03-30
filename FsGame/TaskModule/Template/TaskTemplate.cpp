// -------------------------------------------
// 文件名称： TaskTemplate.cpp
// 文件说明： 任务类型模板
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#include "TaskTemplate.h"
#include "../Data/TaskLoader.h"
#include "FsGame\Define\GameDefine.h"
#include "..\TaskUtil.h"
#include "..\TaskManager.h"
#include "Define\CommandDefine.h"
#include "Define\ServerCustomDefine.h"
#include "SystemFunctionModule/RewardModule.h"

TaskTemplate::TaskTemplate() : m_task_type(TASK_TYPE_NONE)
{

}

// 取得任务类型
const TaskTypes TaskTemplate::GetTaskType()
{
	return m_task_type;
}

// 可否接取任务
bool TaskTemplate::CanAccept(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
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

	// 任务进度表已达上限
	IRecord *pProgRec = pSelf->GetRecord(FIELD_RECORD_TASK_PROGRESS_REC);// TODO
	if (NULL == pProgRec)
	{
		return false;
	}

	if (pProgRec->GetRows() == pProgRec->GetRowMax())
	{
		return false;
	}

	// 不满足接受等级
	const int level = pSelf->QueryInt(FIELD_PROP_LEVEL);
	if (level < pData->accept_min_level 
		&& level > pData->accept_max_level)
	{
		return false;
	}

	// 已拥有该任务
	const int task_status = TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id);
	if (TASK_STATUS_DOING == task_status  
		|| TASK_STATUS_DONE == task_status)
	{
		return false;
	}

	// 前置任务未完成
	if (!TaskUtilS::Instance()->IsPrevTaskSubmit(pKernel, self, task_id))
	{
		return false;
	}


	return true;
}

// 接取任务
bool TaskTemplate::DoAccept(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
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

	// 任务规则
	TaskBaseRule *pRule = TaskManager::m_pThis->GetRule(TaskRules(pData->rule));
	if (NULL == pRule)
	{
		return false;
	}

	// 接取成功
	if (pRule->ExecAccept(pKernel, self, task_id))
	{
		// 通知任务已接取
		CVarList s2s_msg;
		s2s_msg << COMMAND_TASK_ACTION_MSG
				<< TASK_ACTION_ACCEPT
				<< task_id;
		pKernel->Command(self, self, s2s_msg);

		return true;
	}

	return false;
}

// 可否更新任务
bool TaskTemplate::CanUpdate(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 任务不在进行中
	const int task_status = TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id);
	if (TASK_STATUS_DOING != task_status)
	{
		return false;
	}

	return true;
}

// 更新任务
bool TaskTemplate::DoUpdate(IKernel *pKernel, const PERSISTID &self, 
	const int task_id, const IVarList &args)
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

	// 取得任务规则
	TaskBaseRule *pRule = TaskManager::m_pThis->GetRule(TaskRules(pData->rule));
	if (NULL == pRule)
	{
		return false;
	}

	return pRule->ExecUpdate(pKernel, self, task_id, args);
}

// 任务完成后
void TaskTemplate::OnComplete(IKernel *pKernel, const PERSISTID &self, 
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

	// 取得任务规则
	TaskBaseRule *pRule = TaskManager::m_pThis->GetRule(TaskRules(pData->rule));
	if (NULL == pRule)
	{
		return;
	}

	// 清理分组
	TaskUtilS::Instance()->CleanGroupClone(pKernel, self, task_id);

	// 任务完成回调
	pRule->OnComplete(pKernel, self, task_id);

	// 记录任务完成日志
	// TODO

	// 记录已完成次数
	TaskUtilS::Instance()->RecordTaskNum(pKernel, self, pData->type);

	// 可否自动提交
	if (!pData->IsAutoSubmit())
	{
		return;
	}

	// 提交任务
	if (CanSubmit(pKernel, self, task_id))
	{
		DoSubmit(pKernel, self, task_id);
	}

	return;
}

// 注册定时器
void TaskTemplate::RegistResetTimer(IKernel *pKernel, const PERSISTID &self)
{

}

// 接取任务后处理
void TaskTemplate::OnAccept(IKernel *pKernel, const PERSISTID &self,
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

	// 主线任务设置
	if (pData->type == TASK_TYPE_MAIN)
	{
		pSelf->SetInt(FIELD_PROP_CUR_MAIN_TASK, task_id);
	}

	// 自动寻路
	if (pData->IsAutoPathFinding())
	{
		CVarList s2c_msg;
		s2c_msg << SERVER_CUSTOMMSG_TASK_MSG
				<< S2C_TASK_SUMMSG_FINDPATH
				<< task_id;
		pKernel->Custom(self, s2c_msg);
	}

	// 任务自检测
	TaskUtilS::Instance()->SelfCheck(pKernel, self, task_id);

	// 创建任务分组副本
	TaskUtilS::Instance()->CreateGroupClone(pKernel, self, task_id);

	// 任务切场景处理
	TaskUtilS::Instance()->AutoSwitchScene(pKernel, self, task_id);
}

// 可否提交任务
bool TaskTemplate::CanSubmit(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 任务不可提交
	const int task_status = TaskUtilS::Instance()->QueryTaskStatus(pKernel, self, task_id);
	if (TASK_STATUS_DONE != task_status)
	{
		return false;
	}

	return true;
}

// 提交任务
bool TaskTemplate::DoSubmit(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
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

	// 任务数据
	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 领取奖励
	if (pData->IsHaveReward())
	{
		if (RewardModule::m_pRewardInstance->RewardPlayerById(pKernel, self, pData->reward_id))
		{
			// 是否需要客户端打开奖励提示
			if (pData->IsShowReward())
			{
				// 通知客户端显示奖励
				CVarList s2c_msg;
				s2c_msg << SERVER_CUSTOMMSG_TASK_MSG
						<< S2C_TASK_SUMMSG_SHOW_REWARD
						<< pData->reward_id
						<< 1.0f;
				pKernel->Custom(self, s2c_msg);
			}
		}
		else // 领取失败
		{
			// 消息通知
			// TODO
			// 失败日志记录
			// TODO

			return false;
		}		
	}

	// 记录任务提交成功的日志
	// TODO

	// 删除任务
	const int exist_row = pPrgRec->FindInt(COLUMN_TASK_PROGRESS_REC_ID, task_id);
	if (exist_row >= 0)
	{
		pPrgRec->RemoveRow(exist_row);

		// 记录任务删除的日志
		// TODO
	}	

	// 通知任务已提交
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_ACTION_MSG
			<< TASK_STATUS_SUBMITTED
			<< task_id;
	pKernel->Command(self, self, s2s_msg);

	return true;
}

