//--------------------------------------------------------------------
// 文件名:	GameAnalyticsDef.h
// 内  容:	游戏数据分析收集器所需宏定义
// 说  明:	
// 创建日期:	2016年08月24日
// 创建人:	李海罗
// 整理人:            
//--------------------------------------------------------------------
#ifndef __GAME_ANALYTICS_DEFINED_H__
#define __GAME_ANALYTICS_DEFINED_H__

// 分析功能模块
enum EGameModule
{
	EGAME_MODULE_BASE = 1,			// 基础模块
	EGAME_MODULE_BAG,				// 背包模块
	EGAME_MODULE_EQUIP,				// 装备模块

	EGAME_MODULE_TASK,				// 任务模块
	EGAME_MODULE_ACHIEVEMENT,		// 成就系统
	EGAME_MODULE_GUILD,				// 公会模块

	EGAME_MODULE_FIGHT,				// 战斗模块
	EGAME_MODULE_SKILL,				// 技能模块
	EGAME_MODULE_RIDE,				// 坐骑模块

	EGAME_MODULE_ACTIVITY,			// 活动模块
	EGAME_MODULE_SOCIAL,			// 社交模块
};

// 分析游戏动作
enum EGameAction
{
	//// 基础( 1-199 )
	//EM_ACTION_RND_NATION = 1,				// 随机选择国家
	EM_ACTION_ROLE_CREATE,					// 创角
	//EM_ACTION_GM_SUPPLY,					// GM补给
	//EM_ACTION_PAY,							// 充值
	//EM_ACTION_SHARE,						// 好友圈分享
	//EM_ACTION_VIP_LEVELUP,					// vip升级

	//// 背包模块(200-399)
	//EM_ACTION_SALE_ITEM,					// 出售物品
	//EM_ACTION_BUY_ITEM,						// 购买物品
	//EM_ACTION_BUY_UNTREAD,					// 购买退回
	//EM_ACTION_USE_ITEM,						// 使用物品


	//// 任务(-99)
	//EM_ACTION_TASK_ACCEPT,						// 接收任务 主要参数: 任务id 任务类型
	//EM_ACTION_TASK_CANCEL,						// 取消任务 主要参数: 任务id 任务类型
	//EM_ACTION_TASK_GIVE,						// 提交任务 主要参数: 任务id
	//EM_ACTION_TASK_AWARD,						// 领取任务奖励 主要参数: 任务id 是否成功
	//EM_ACTION_TASK_CLEAR,						// 清除任务 主要参数: 任务id 是否成功
	//EM_ACTION_TASK_REFRESH,						// 刷新任务
	//// TEST
	//EM_ACTION_TASK_FAILED_AWARD,				// 领取奖励失败
	//EM_ACTION_TASK_ACTIVITY_CANCEL,				// 放弃日常活动任务

	////// 成就(100-199)
	//EM_ACTION_ACHIEVMENT_COMPLETED,				// 完成成就 主要参数: 成就id
	//EM_ACTION_ACHIEVMENT_AWARD,					// 领取成就 主要参数: 成就id


	//// 公会
	//EM_ACTION_GUILD_CREATE,						// 创建工会
	//EM_ACTION_GUILD_UNTREAT,					// 创建工会退回
	//EM_ACTION_GUILD_ACTIVITY,					// 工会活动


	//// 战斗
	//EM_ACTION_FIGHT_KILL_PLAYER,			// 杀人
	//EM_ACTION_FIGHT_KILL_NPC,				// 杀怪
	//EM_ACTION_FIGHT_ENTRY_SCENE,			// 进入场景/副本
	//EM_ACTION_FIGHT_SECRET_PASS,			// 秘境通关
	//EM_ACTION_FIGHT_SECRET_STAR,			// 秘境3星奖励

	//// 技能
	//EM_ACTION_SKILL_LEVELUP,				// 技能升级

	//// 坐骑
};

#endif