// -------------------------------------------
// 文件名称： EntrySceneRule.cpp
// 文件说明： 进入场景规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#include "../Data/TaskLoader.h"
#include "FsGame\Define\GameDefine.h"
#include "..\TaskUtil.h"
#include "utils/string_util.h"
#include "Define/CommandDefine.h"
#include "EntrySceneRule.h"

// 执行更新任务
bool EntrySceneRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, const int task_id,
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
void EntrySceneRule::ExecCheck(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_ENTRY_SCENE
			<< 1;
	pKernel->Command(self, self, s2s_msg);
}