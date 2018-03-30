// -------------------------------------------
// 文件名称： TaskTemplate.h
// 文件说明： 任务类型模板
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskTemplate_H_
#define _TaskTemplate_H_

#include "Fsgame/Define/header.h"
#include "../TaskDefine.h"

class TaskTemplate
{
protected:
	TaskTypes m_task_type;

public:
	TaskTemplate();

	// 取得任务类型
	const TaskTypes GetTaskType();

public:
    // 初始化
    virtual bool Init(IKernel *pKernel) = 0;

	// 开启
	virtual bool Start(IKernel *pKernel, const PERSISTID &self) = 0;

	// 可否接取任务
	virtual bool CanAccept(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 接取任务
	virtual bool DoAccept(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 可否更新任务
	virtual bool CanUpdate(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 更新任务
	virtual bool DoUpdate(IKernel *pKernel, const PERSISTID &self,
		const int task_id, const IVarList &args);

	// 任务完成后
	void OnComplete(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 任务提交后
	virtual void OnSubmit(IKernel *pKernel, const PERSISTID &self, 
		const int task_id) = 0;

	// 注册定时器
	virtual void RegistResetTimer(IKernel *pKernel, const PERSISTID &self);

public:
	// 接取任务后处理（子类暂不需重写）
	virtual void OnAccept(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 可否提交任务（子类暂不需重写）
	virtual bool CanSubmit(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 提交任务（子类暂不需重写）
	virtual bool DoSubmit(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

private:
	// 接取后置任务
	virtual void AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
		const int task_id) = 0;
};

#endif
