// -------------------------------------------
// 文件名称： UpgradeLevelRule.h
// 文件说明： 升级规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _UpgradeLevelRule_H_
#define _UpgradeLevelRule_H_

#include "TaskBaseRule.h"

class UpgradeLevelRule : public TaskBaseRule
{

public:
	// 任务自检测
	virtual void ExecCheck(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

};

#endif
