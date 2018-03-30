// -------------------------------------------
// 文件名称： TaskBaseRule.h
// 文件说明： 任务基础规则
// 创建日期： 2018/02/26
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskBaseRule_H_
#define _TaskBaseRule_H_

#include "Fsgame/Define/header.h"
#include "../TaskDefine.h"

class TaskBaseRule
{

public:
    // 初始化
    virtual bool Init(IKernel *pKernel);

	// 执行接取任务
	virtual bool ExecAccept(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 执行更新任务
	virtual bool ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
		const int task_id, const IVarList &args);

	// 任务自检测
	virtual void ExecCheck(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 任务完成后
	virtual void OnComplete(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

public:
	// 任务进度计算
	virtual bool Calculate(IKernel *pKernel, const PERSISTID &self,
		const int task_id, std::string &new_progress, const IVarList &args, bool& is_complete);

	// 修改进度
	virtual bool SetProgress(IKernel *pKernel, const PERSISTID &self,
		const int task_id, const char *new_progress, bool is_completed = true);
};

#endif
