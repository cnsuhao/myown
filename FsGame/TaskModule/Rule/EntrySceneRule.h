// -------------------------------------------
// 文件名称： EntrySceneRule.h
// 文件说明： 进入场景规则
// 创建日期： 2018/03/01
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _EntrySceneRule_H_
#define _EntrySceneRule_H_

#include "TaskBaseRule.h"

class EntrySceneRule : public TaskBaseRule
{

public:
	// 执行更新任务
	virtual bool ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
		const int task_id, const IVarList &args);

	// 任务自检测
	virtual void ExecCheck(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

};

#endif
