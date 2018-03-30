//------------------------------------------------------------------
// 文 件 名:    ActionMutexDefine.h
// 内    容:    行为定义
// 说    明:    
// 创 建 人:      
// 创建日期:
//    :        
//-------------------------------------------------------------------
#ifndef _ACTION_DEFINE_H__
#define _ACTION_DEFINE_H__
#include <vector>

//玩法ID
enum
{
	ACTION_NONE = 0,

	ACTION_IN_TASK_GROUP,		// 任务分组中
	ACTION_SECRET,				// 秘境
	ACTION_ARENA,				// 竞技场	
	ACTION_GUILD_STATION,		// 帮会驻地
	ACTION_REVENGE_ARENA,		// 生死擂
	ACTION_ESCORT,				// 押囚
	ACTION_TRANSFORM,			// 变身
	ACTION_CONVOY,				// 护送
	ACTION_TEAM_SECRET,			// 组队秘境
	ACTION_PLUNDER_BATTLE,		// 掠夺战
	ACTION_TEAMOFFER,			// 组队悬赏
	ACTION_TOWER,				// 试炼塔
	ACTION_INFORMAL_PVP,		// 切磋
	ACTION_KIDNAP,				// 绑票
	ACTION_TREASURY_FRONTIER,	// 夜袭宝库
	ACTION_IN_JAIL,				// 坐牢
	ACTION_POWER_BATTLE,		// 势力战
	ACTION_FRO_TOWER,			// 边境塔防
	ACTION_TEACHING,			// 正在传功
	ACTION_MAX,
};


#endif // _ACTION_DEFINE_H__