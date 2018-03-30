//--------------------------------------------------------------------
// 文件名:      ResetTimerDefine.h
// 内  容:      定时器定义
// 说  明:
// 创建日期:    2016年03月28日
// 创建人:        tongzt
// 修改人:        
//--------------------------------------------------------------------

#ifndef  _RESET_TIMER_DEFINE_H
#define _RESET_TIMER_DEFINE_H

#include <map>

// 心跳时长,刷新间隔最低只能指定大于次时间的长度
#define RESET_HEART_BEAT_CYCLE  1

/*
 * 定时器功能类型
 */
enum ResetTimerTypes
{
	RESET_TYPES_NONE = 0,
	RESET_DAILY_TASK,										// 重置日常任务
	RESET_DDY_TOPLIMIT,										// 0点重置当天累计增加资金记录值
	RESET_ROLE_LOGIN_TIME,									// 0点更新登录时间
	RESET_WING_STEP_BLESS,									// 重置翅膀升阶祝福值

	RESET_FRIEND_WEEK_FLOWER_NUM,							// 重置好友周常收花
	RESET_DAILY_FRIEND,										// 每日重置好友功能

	RESET_SHOP_GOLD,										// 元宝商店刷新
	RESET_SHOP_GOLD_TIMES,									// 元宝商店刷新次数刷新

	RESET_RIDE,												// 坐骑
	RESET_GUILD_DAILY_RESET,								// 组织每日重置

	RESET_TYPES_TOTAL,
};

// 定时器类型
enum TimerTypes
{
	TIMER_TYPE_DAY = 0,		// 日刷新
	TIMER_TYPE_WEEK,		// 周刷新
};

// 刷新数据
struct ResetData
{
	int reset_type;				// 刷新类型
	std::string reset_timer;	// 刷新时间
	int reset_weekday;			// 刷新weekday

	ResetData() :
		reset_type(0), 
		reset_weekday(-1)
		{
		}
};

// 刷新函数格式定义
typedef int (*ResetTimer)(IKernel* pKernel, const PERSISTID& self, int slice);

typedef std::map<int, ResetTimer> ResetFuncMap;	// 函数名字-指针映射
typedef std::map<int, ResetData>  ResetdDataMap;	// 刷新类型-刷新数据

#endif //_RESET_TIMER_DEFINE_H