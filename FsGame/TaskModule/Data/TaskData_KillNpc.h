// -------------------------------------------
// 文件名称： TaskData_KillNpc.h
// 文件说明： 击杀Npc任务数据
// 创建日期： 2018/02/24
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskData_KillNpc_H_
#define _TaskData_KillNpc_H_

#include "TaskBaseData.h"

/*
** 击杀Npc任务信息类
*/
class TaskData_KillNpc : public TaskBaseData
{
public:
	CoupleElementList npc_list;	// 击杀怪物列表
	int kill_way;				// 击杀怪物计算方式（0击杀所有 1击杀任一组 2击杀指定数量）
	
public:
	TaskData_KillNpc() : TaskBaseData(), kill_way(0)
	{
		npc_list.clear();
	}
};

#endif
