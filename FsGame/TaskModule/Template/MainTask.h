// -------------------------------------------
// 文件名称： MainTask.h
// 文件说明： 主线任务
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _MainTask_H_
#define _MainTask_H_

#include "TaskTemplate.h"

class MainTask : public TaskTemplate
{

public:
	MainTask();

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

private:
	// 客户端准备就绪
	static int OnReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

private:
	// 接取后置任务
	virtual void AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
		const int task_id);
};

#endif
