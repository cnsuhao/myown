// -------------------------------------------
// 文件名称： TaskData_Simple.h
// 文件说明： 单一附加条件任务数据
// 创建日期： 2018/02/28
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskData_Simple_H_
#define _TaskData_Simple_H_

#include "TaskBaseData.h"

/*
** 单一附加条件任务信息类
*/
class TaskData_Simple : public TaskBaseData
{
public:
	std::string add_info;	// 单一条件
	
public:
	TaskData_Simple() : TaskBaseData()
	{
		
	}
};

#endif
