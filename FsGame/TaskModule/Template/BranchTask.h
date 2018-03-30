// -------------------------------------------
// 文件名称： BranchTask.h
// 文件说明： 支线任务
// 创建日期： 2018/02/28
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _BranchTask_H_
#define _BranchTask_H_

#include "TaskTemplate.h"

class BranchTask : public TaskTemplate
{

public:
	BranchTask();

    // 初始化
    virtual bool Init(IKernel *pKernel);

	// 开启
	virtual bool Start(IKernel *pKernel, const PERSISTID &self);

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
