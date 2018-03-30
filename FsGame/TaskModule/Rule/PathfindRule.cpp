// -------------------------------------------
// 文件名称： PathfindRule.cpp
// 文件说明： 寻路规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#include "../Data/TaskLoader.h"
#include "FsGame\Define\GameDefine.h"
#include "..\TaskUtil.h"
#include "utils/string_util.h"
#include "Define/CommandDefine.h"
#include "PathfindRule.h"

// 执行更新任务
bool PathfindRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, const int task_id,
	const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
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
	if (!TaskUtilS::Instance()->IsTargetScene(pKernel, pData->scene_list))
	{
		return false;
	}

	// 坐标是否匹配
	Position position(pSelf->GetPosiX(), pSelf->GetPosiY(), pSelf->GetPosiZ(), pSelf->GetOrient());
	if (!TaskUtilS::Instance()->IsTargetPosition(pKernel, pData->posi_list, position))
	{
		return false;
	}

	// 任务进度计算
	std::string new_progress = "";
	bool is_complete = false;
	if (!Calculate(pKernel, self, task_id, new_progress, CVarList() << 1, is_complete))
	{
		return false;
	}

	// 修改进度
	return SetProgress(pKernel, self, task_id, new_progress.c_str(), is_complete);
}

// 任务自检测
void PathfindRule::ExecCheck(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 检查寻路任务（暂时不做检查）
	/*CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
	<< S2S_TASK_SUBMSG_UPDATE
	<< TASK_RULE_PATHFINDING
	<< 1;
	pKernel->Command(self, self, s2s_msg);*/
}