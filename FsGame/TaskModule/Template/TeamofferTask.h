// -------------------------------------------
// 文件名称： TeamofferTask.h
// 文件说明： 缉盗任务
// 创建日期： 2018/04/02
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _Teamoffer_Task_H_
#define _Teamoffer_Task_H_

#include "TaskTemplate.h"

class TeamofferTask : public TaskTemplate
{

public:
	TeamofferTask();

    // 初始化
    virtual bool Init(IKernel *pKernel);

	// 开启
	virtual bool Start(IKernel *pKernel, const PERSISTID &self);

	// 注册定时器
	virtual void RegistResetTimer(IKernel *pKernel, const PERSISTID &self);

	// 任务提交后
	virtual void OnSubmit(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

private:
	// 功能开启 
	static int OnCommandActiveFunc(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 缉盗任务定时器
	static int ResetTask(IKernel *pKernel, const PERSISTID &self, int slice);

private:
	// 接取后置任务
	virtual void AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
		const int task_id);
};

#endif
