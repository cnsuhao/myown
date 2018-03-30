// -------------------------------------------
// 文件名称： GatherRule.cpp
// 文件说明： 采集规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#include "GatherRule.h"
#include "../Data/TaskLoader.h"
#include "utils/string_util.h"
#include "../TaskUtil.h"
#include "../Data/TaskData_Simple.h"


// 执行更新任务
bool GatherRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, const int task_id,
	const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 目标物品
	const char *item_id = args.StringVal(0);
	if (StringUtil::CharIsNull(item_id))
	{
		return false;
	}

	// 取得数据
	const TaskData_Simple *pData = (TaskData_Simple*)TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 不匹配
	if (!StringUtil::CharIsEqual(pData->add_info.c_str(), item_id))
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
