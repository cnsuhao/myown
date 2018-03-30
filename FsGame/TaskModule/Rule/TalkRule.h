// -------------------------------------------
// 文件名称： TalkRule.h
// 文件说明： 对话规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TalkRule_H_
#define _TalkRule_H_

#include "TaskBaseRule.h"

class TalkRule : public TaskBaseRule
{

public:
	// 执行更新任务
	virtual bool ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
		const int task_id, const IVarList &args);
};

#endif
