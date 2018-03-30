// -------------------------------------------
// 文件名称： TaskBaseData.h
// 文件说明： 任务基础数据
// 创建日期： 2018/02/24
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskBaseData_H_
#define _TaskBaseData_H_

#include "TaskStructDefine.h"

/*
** 任务基础信息类
*/
class TaskBaseData
{
public:
	int task_id;			// 任务编号
	int type;				// 类型
	int rule;				// 种类
	int accept_min_level;	// 接受最小等级
	int accept_max_level;	// 接受最大等级
	int count;				// 任务总进度
	int calculate_mode;		// 统计方式
	NumList prev_task_list;	// 前置任务表
	NumList post_task_list;	// 后续任务表
	int task_star;			// 任务星级

	int is_group;			// 是否有分组副本（0无分组副本 1有分组副本）
	int auto_pathfinding;	// 接取任务后是否自动寻路
	int is_auto_submit;		// 完成任务后是否自动提交
	int is_switch_scene;	// 接取任务后是否切换场景
	int is_switch_position;	// 接取任务后是否切换坐标
	int is_show_reward;		// 提交任务后是否打开奖励界面

	NumList	scene_list;		// 场景列表
	PositionList posi_list;	// 坐标列表
	int reward_id;			// 奖励ID

public:
	TaskBaseData() :
		task_id(0),
		type(0),
		rule(0),
		accept_min_level(0),
		accept_max_level(0),
		count(0),
		calculate_mode(0), 
		task_star(0),
		is_group(0),
		auto_pathfinding(0),
		is_auto_submit(0),
		is_switch_scene(0),
		is_switch_position(0),
		is_show_reward(0),
		reward_id(0)
	{
		prev_task_list.clear();
		post_task_list.clear();
		scene_list.clear();
		posi_list.clear();
	}


public:
	// 是否有分组副本
	bool IsGrouping() const
	{
		return is_group > 0 ? true : false;
	}

	// 接取任务后是否自动寻路
	bool IsAutoPathFinding() const
	{
		return auto_pathfinding > 0 ? true : false;
	}

	// 完成任务后是否自动提交
	bool IsAutoSubmit() const
	{
		return is_auto_submit > 0 ? true : false;
	}

	// 接取任务后是否切换场景
	bool IsSwitchScene() const
	{
		return is_switch_scene > 0 ? true : false;
	}

	// 接取任务后是否切换坐标
	bool IsSwitchPosition() const
	{
		return is_switch_position > 0 ? true : false;
	}

	// 提交任务后是否打开奖励界面
	bool IsShowReward() const
	{
		return is_show_reward > 0 ? true : false;
	}

	// 是否有奖励
	bool IsHaveReward() const
	{
		return reward_id > 0;
	}
};

#endif
