// -------------------------------------------
// 文件名称： TaskBaseRule.cpp
// 文件说明： 任务基础规则
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#include "TaskBaseRule.h"
#include "../Data/TaskLoader.h"
#include "FsGame\Define\GameDefine.h"
#include "..\TaskUtil.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"
#include "Define/CommandDefine.h"

// 初始化
bool TaskBaseRule::Init(IKernel *pKernel)
{
	return true;
}

// 执行接取任务
bool TaskBaseRule::ExecAccept(IKernel *pKernel, const PERSISTID &self, 
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

	CVarList row_value;
	row_value << task_id
			  << TASK_STATUS_DOING
			  << StringUtil::IntAsString(0);

	if (pPrgRec->AddRowValue(-1, row_value) < 0)
	{
		extend_warning(LOG_WARNING, "[%s][%d] add row failed:[%d]", __FILE__, __LINE__, task_id);
		return false;
	}

	return true;
}

// 执行更新任务
bool TaskBaseRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, const int task_id, 
	const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 验证是否可增加进度
	// TODO

	// 任务进度计算
	std::string new_progress = "";
	bool is_complete = false;
	if (!Calculate(pKernel, self, task_id, new_progress, args, is_complete))
	{
		return false;
	}

	// 修改进度
	return SetProgress(pKernel, self, task_id, new_progress.c_str(), is_complete);
}

// 任务自检测
void TaskBaseRule::ExecCheck(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{
	return;
}

// 任务完成后
void TaskBaseRule::OnComplete(IKernel *pKernel, const PERSISTID &self, 
	const int task_id)
{

	// 重置特定任务修改的属性
	// TODO
	return;
}

// 任务进度计算
bool TaskBaseRule::Calculate(IKernel *pKernel, const PERSISTID &self, 
	const int task_id, std::string &new_progress, const IVarList &args, bool& is_complete)
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

	const int exist_row = pPrgRec->FindInt(COLUMN_TASK_PROGRESS_REC_ID, task_id);
	if (exist_row < 0)
	{
		return false;
	}

	const TaskBaseData *pData = TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 当前进度
	int cur_progress = StringUtil::StringAsInt(pPrgRec->QueryString(exist_row, COLUMN_TASK_PROGRESS_REC_PROGRESS));
	int incc_value = args.IntVal(0);// 增值
	incc_value = incc_value > 0 ? incc_value : 1;
	
	// 递增
	if (pData->calculate_mode == TASK_CALCULATE_MODE_ADD)
	{
		cur_progress += incc_value;
	}
	else if (pData->calculate_mode == TASK_CALCULATE_MODE_RESET)
	{
		cur_progress = incc_value;
	}
	
	if (cur_progress >= pData->count)
	{
		cur_progress = pData->count;
		is_complete = true;
	}

	new_progress = StringUtil::IntAsString(cur_progress);
	return true;
}

// 修改进度
bool TaskBaseRule::SetProgress(IKernel *pKernel, const PERSISTID &self, 
	const int task_id, const char *new_progress, bool is_completed /*= true*/)
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

	const int exist_row = pPrgRec->FindInt(COLUMN_TASK_PROGRESS_REC_ID, task_id);
	if (exist_row < 0)
	{
		return false;
	}
	
	// 任务当前进度
	pPrgRec->SetString(exist_row, COLUMN_TASK_PROGRESS_REC_PROGRESS, new_progress);

	// 已完成
	if (is_completed)
	{
		// 设置任务进度为可提交
		pPrgRec->SetInt(exist_row, COLUMN_TASK_PROGRESS_REC_STATUS, TASK_STATUS_DONE);

		// 任务已完成
		CVarList s2s_msg;
		s2s_msg << COMMAND_TASK_ACTION_MSG
				<< TASK_ACTION_COMPLETE
				<< task_id;
		pKernel->Command(self, self, s2s_msg);
	}

	return true;
}
