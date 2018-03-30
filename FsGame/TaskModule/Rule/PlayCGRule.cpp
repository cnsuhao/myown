// -------------------------------------------
// 文件名称： PlayCGRule.cpp
// 文件说明： 播放cg规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#include "../Data/TaskLoader.h"
#include "PlayCGRule.h"
#include "Define/ServerCustomDefine.h"
#include "../Data/TaskData_Simple.h"
#include "CommonModule/CommRuleModule.h"
#include "utils/string_util.h"
#include "../TaskUtil.h"

// 执行更新任务
bool PlayCGRule::ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
	const int task_id, const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// CG
	const char *cg_id = args.StringVal(0);
	if (StringUtil::CharIsNull(cg_id))
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
	if (!StringUtil::CharIsEqual(pData->add_info.c_str(), cg_id))
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
void PlayCGRule::ExecCheck(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	const TaskData_Simple *pData = (TaskData_Simple *)TaskLoader::instance().GetTaskBase(task_id);
	if (NULL == pData)
	{
		return;
	}

	// 播放CG
	CommRuleModule::m_pThis->PlayCG(pKernel, self, pData->add_info.c_str());
}