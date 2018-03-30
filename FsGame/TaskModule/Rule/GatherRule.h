// -------------------------------------------
// 文件名称： GatherRule.h
// 文件说明： 采集规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _GatherRule_H_
#define _GatherRule_H_

#include "TaskBaseRule.h"

class GatherRule : public TaskBaseRule
{

public:
	// 执行更新任务
	virtual bool ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
		const int task_id, const IVarList &args);
};

#endif
