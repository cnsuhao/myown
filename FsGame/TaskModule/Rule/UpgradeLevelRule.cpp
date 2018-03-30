// -------------------------------------------
// 文件名称： UpgradeLevelRule.cpp
// 文件说明： 升级规则
// 创建日期： 2018/03/02
// 创 建 人：  tzt
// -------------------------------------------

#include "FsGame/Define/GameDefine.h"
#include "Define/CommandDefine.h"
#include "UpgradeLevelRule.h"

// 任务自检测
void UpgradeLevelRule::ExecCheck(IKernel *pKernel, const PERSISTID &self,
	const int task_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}
	
	// 检查升级任务
	CVarList s2s_msg;
	s2s_msg << COMMAND_TASK_COMMAND
			<< S2S_TASK_SUBMSG_UPDATE
			<< TASK_OPT_WAY_BY_RULE
			<< TASK_RULE_UPGRADE_LEVEL
			<< pSelf->QueryInt(FIELD_PROP_LEVEL);
	pKernel->Command(self, self, s2s_msg);
}