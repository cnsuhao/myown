// -------------------------------------------
// 文件名称： KillNpcRule.h
// 文件说明： 击杀Npc规则
// 创建日期： 2018/02/27
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _KillNpcRule_H_
#define _KillNpcRule_H_

#include "TaskBaseRule.h"

class KillNpcRule : public TaskBaseRule
{

public:
	// 执行接取任务
	virtual bool ExecAccept(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 执行更新任务
	virtual bool ExecUpdate(IKernel *pKernel, const PERSISTID &self, 
		const int task_id, const IVarList &args);

private:
	// 任务进度计算
	virtual bool Calculate(IKernel *pKernel, const PERSISTID &self,
		const int task_id, std::string &new_progress, const IVarList &args, bool& is_complete);
};

#endif
