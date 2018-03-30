// -------------------------------------------
// 文件名称： DailyTask.h
// 文件说明： 日常任务
// 创建日期： 2018/02/28
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _DailyTask_H_
#define _DailyTask_H_

#include "TaskTemplate.h"

class DailyTask : public TaskTemplate
{

public:
	DailyTask();

    // 初始化
    virtual bool Init(IKernel *pKernel);

	// 开启
	virtual bool Start(IKernel *pKernel, const PERSISTID &self);

	// 可否接取任务
	virtual bool CanAccept(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 任务提交后
	virtual void OnSubmit(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 注册定时器
	virtual void RegistResetTimer(IKernel *pKernel, const PERSISTID &self);

private:
	// 功能开启 
	static int OnCommandActiveFunc(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 日常任务定时器
	static int ResetTask(IKernel *pKernel, const PERSISTID &self, int slice);

private:
	// 接取后置任务
	virtual void AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
		const int task_id);
};

#endif
