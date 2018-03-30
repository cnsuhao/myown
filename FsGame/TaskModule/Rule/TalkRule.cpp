// -------------------------------------------
// 文件名称： TalkRule.cpp
// 文件说明： 对话规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#include "TalkRule.h"
#include "../Data/TaskLoader.h"
#include "utils/string_util.h"
#include "../TaskUtil.h"
#include "../Data/TaskData_Simple.h"


// 执行更新任务
bool TalkRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, const int task_id,
	const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 对话Npc
	const char *target_npc = args.StringVal(0);
	if (StringUtil::CharIsNull(target_npc))
	{
		return false;
	}

	// 取得击杀npc数据
	const TaskData_Simple *pData = (TaskData_Simple*)TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return false;
	}

	// 不匹配
	if (!StringUtil::CharIsEqual(pData->add_info.c_str(), target_npc))
	{
		return false;
	}

	// 场景是否一致
	if (!TaskUtilS::Instance()->IsTargetScene(pKernel, pData->scene_list))
	{
		return false;
	}

	// 查找范围内Npc
	CVarList around_npc_list;
	pKernel->GetAroundList(self, TALK_VERIFY_DISTANCE, TYPE_NPC, 0, around_npc_list);

	bool verify = false;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)around_npc_list.GetCount(); ++i)
	{
		LoopDoCheck(a);
		PERSISTID npc = around_npc_list.ObjectVal(i);
		IGameObj *pNpc = pKernel->GetGameObj(npc);
		if (NULL == pNpc)
		{
			continue;
		}

		if (!StringUtil::CharIsEqual(pNpc->GetConfig(), target_npc))
		{
			continue;
		}

		// 玩家与npc的距离超过安全距离
		if (pKernel->Distance2D(self, npc) > TALK_VERIFY_DISTANCE)
		{
			continue;
		}

		verify = true;
		break;
	}

	// 验证未通过
	if (!verify)
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
