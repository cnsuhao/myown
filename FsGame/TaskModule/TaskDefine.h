 // -----------------------------------------
// 文件名称： TaskDefine.h
// 文件说明： 任务定义
// 创建日期： 2018/02/23
// 创 建 人：  tzt
// -----------------------------------------

#ifndef _TaskDefine_H_
#define _TaskDefine_H_

/////////////////////////////////////常量定义/////////////////////////////////////       
#define PATHFIND_VERIFY_DISTANCE 5.0f		// 寻路用验证距离  
#define COMMON_VERIFY_DISTANCE 8.0f			// 通用验证距离  
#define TALK_VERIFY_DISTANCE 20.0f			// 对话用验证距离  
#define FULL_STAR 5						// 满星级
#define DOMAIN_TASK_RESET_TIME 5		// 任务域区间重置时间

// 任务类型
enum TaskTypes
{
	TASK_TYPE_NONE	= 0,	// 无类型

	TASK_TYPE_MAIN,			// 主线任务
	TASK_TYPE_BRANCH,		// 支线任务
	TASK_TYPE_DAILY,		// 日常任务				

	TASK_TYPE_TOTAL,		// 总类型
};

// 任务规则
enum TaskRules
{
	TASK_RULE_NONE = 0,				// 无类型

	TASK_RULE_KILL_NPC = 1,			// 击杀怪物
	TASK_RULE_KILL_PLAYER = 2,		// 击杀玩家
	TASK_RULE_DIALOGUE = 3,			// 对话
	TASK_RULE_PATHFINDING = 4,		// 寻路
	TASK_RULE_USE_ITEM = 5,			// 使用物品
	TASK_RULE_GATHER = 6,			// 采集
	TASK_RULE_UPGRADE_LEVEL = 7,	// 升级
	TASK_RULE_ENTRY_SCENE = 8,		// 进入场景
	TASK_RULE_PLAY_CG = 9,			// 播放CG

	TASK_RULE_TOTAL,				// 类型总数
};

// 任务状态
enum TaskStatus
{
	TASK_STATUS_DOING = 0,		// 进行中
	TASK_STATUS_DONE,			// 完成，可提交
	TASK_STATUS_SUBMITTED,		// 已提交

	TASK_STATUS_MAX,
};

// 任务动作
enum TaskActions
{
	TASK_ACTION_ACCEPT = 0,		// 接取
	TASK_ACTION_COMPLETE,		// 完成
	TASK_ACTION_SUBMIT,			// 提交
};

// 任务操作方式
enum TaskOptWay
{
	TASK_OPT_WAY_BY_ID = 0,		// 根据任务id操作
	TASK_OPT_WAY_BY_TYPE,		// 根据任务类型操作
	TASK_OPT_WAY_BY_RULE,		// 根据任务规则操作
};

// 任务进度统计方式
enum TaskCalculateMode
{
	TASK_CALCULATE_MODE_ADD = 0,	// 累计
	TASK_CALCULATE_MODE_RESET,		// 重置
};

// 击杀Npc统计方式
enum KillNpcStatWay
{
	KILL_NPC_STAT_WAY_ALL = 0,		// 击杀列表所有
};

// 自动寻路状态
enum AutoPathFinding
{
	AUTO_PATHFINDING_NO = 0,	// 不在自动寻路
	AUTO_PATHFINDING_YES,		// 自动寻路中
};

///////////////////////////////////二级消息定义///////////////////////////////////////
// 服务器-服务器子消息
enum S2S_Task_SubMsg
{
	/*!
	* @brief	接取任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
	S2S_TASK_SUBMSG_ACCEPT,

	/*!
	* @brief	更新任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args
	*/
	S2S_TASK_SUBMSG_UPDATE,

	/*!
	* @brief	清除任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
	S2S_TASK_SUBMSG_CLEAR,
    
	/*!
	* @brief	提交任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
	S2S_TASK_SUBMSG_SUBMIT,

};

// 服务器-客户端子消息
enum S2C_Task_SumMsg
{
	/*!
	* @brief	通知客户端打开任务奖励提示界面
	* @param int 奖励ID
	* @param float 倍率
	*/
    S2C_TASK_SUMMSG_SHOW_REWARD = 1,

	/*!
	* @brief	通知客户端进行寻路
	* @param int 任务ID
	*/
	S2C_TASK_SUMMSG_FINDPATH,

	/*!
	* @brief	推送某种类型任务详细信息
	* @param int 任务类型
	* @param int 已用次数
	* @param int 总次数
	*/
	S2C_TASK_SUMMSG_DETAIL_INFO,
	
};

// 客户端-服务器子消息
enum C2S_Task_SubMsg
{
	/*!
	* @brief	向服务器请求接受任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
    C2S_TASK_SUBMSG_ACCEPT = 1,

	/*!
	* @brief	向服务器请求更新任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
	C2S_TASK_SUBMSG_UPDATE,

	/*!
	* @brief	向服务器请求提交领奖
	* @param int 任务ID
	*/
	C2S_TASK_SUBMSG_SUBMIT,

	/*!
	* @brief	向服务器请求清除任务
	* @param int 操作方式TaskOptWay
	* @param 可变参args(参照TaskOptWay填参)
	*/
	C2S_TASK_SUBMSG_CLEAR,

	/*!
	* @brief	通知服务器剧情对话完成
	* @param int 任务ID
	* @param string 对话NpcID
	*/
	C2S_TASK_SUBMSG_DIALOG_END,

	/*!
	* @brief	通知服务器采集物品
	* @param string 物品ID
	*/
	C2S_TASK_SUBMSG_GATHER,

	/*!
	* @brief	通知服务器使用物品
	* @param int 任务ID
	* @param string 物品ID
	*/
	C2S_TASK_SUBMSG_USE_ITEM,

	/*!
	* @brief	请求分组副本
	* @param int 任务ID
	*/
	C2S_TASK_SUBMSG_SWITCH_GROUP,

	/*!
	* @brief	通知任务自动寻路状态
	* @param int 自动寻路状态(AutoPathFinding)
	*/
	C2S_TASK_SUBMSG_SET_PATHFINDING,

	/*!
	* @brief	请求某类型任务详细信息
	* @param int 任务类型
	*/
	C2S_TASK_SUBMSG_DETAIL_INFO,


	C2S_TASK_SUBMSG_TOTAL,
};	

#endif
