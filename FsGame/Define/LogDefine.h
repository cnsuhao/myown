//--------------------------------------------------------------------
// 文件名:		LogDefine.h
// 内  容:		Log类型定义
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:        ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __LogDefine_H__
#define __LogDefine_H__
#include "FunctionEventDefine.h"

// Log等级
enum LogLevelEnum
{
	LOG_TRACE_PERFORMANCE,
	LOG_TRACE = 1,			/**< 程序调试级别日志 */
	LOG_INFO,				/**< 系统跟踪日志 */
	LOG_WARNING,			/**< 警告日志 */
	LOG_ERROR,				/**< 错误日志 */
	LOG_CRITICAL			/**< 严重错误(崩溃级别，慎用) */
};

// $@ 17 = 玩家升级日志类型 17000-17999
enum LOG_ROLE_UPGRADE_TYPE
{
	LOG_ROLE_UPGRADE_MIN = 17000,		// $ 17000 = 玩家升级日志类型最小值
	LOG_ROLE_UPGRADE_PLAYER,			// $ 17001 = 玩家升级
	LOG_ROLE_UPGRADE_SKILL,				// $ 17002 = 技能升级
	LOG_ROLE_UPGRADE_PASSIVE_SKILL,		// $ 17003 = 被动技能升级
	LOG_ROLE_UPGRADE_WING_LEVEL,		// $ 17004 = 玩家翅膀等级提升
	LOG_ROLE_UPGRADE_WING_STEP,			// $ 17005 = 玩家翅膀阶级提升

	LOG_ROLE_UPGRADE_MAX = 17999,		// $ 17999 = 玩家升级日志类型最大值
};

// $@ 20 = 好友日志记录 20000-20999
enum LOG_FRIEND_TYPE
{
	LOG_FRIEND_MIN = 20000,			// $ 20000 = 好友相关日志起始
	LOG_FRIEND_ADD,					// $ 20001 = 添加好友
	LOG_FRIEND_DELETE,				// $ 20002 = 删除好友
	LOG_FRIEND_GIVE,				// $ 20003 = 赠送好友
	LOG_FRIEND_GET,					// $ 20004 = 接受好友赠送

	LOG_FRIEND_MAX = 20999,			// $ 20999 = 好友相关日志结束
};

// $@ 31 = 时装日志 21000-21999
enum LOG_FASHION_TYPE
{
	LOG_FASHION_UNLOCK_EQUIP = 21000,	// $ 21000 = 装备解锁时装


	LOG_FASHION_MAX = 21999,	// $ 21999 = 时装日志类型最大值    
};


// 异动类型
enum LOG_ACT_EVENT_TYPE
{
	LOG_ACT_EVENT_GAIN = 0,			// 异动类型-增加
	LOG_ACT_EVENT_LOSE = 1,			// 异动类型-减少
};

// 异动状态
enum LOG_ACT_STATE_TYPE
{
	LOG_ACT_STATE_FAIL = 0,			// 异动状态-失败
	LOG_ACT_STATE_SUCCESS = 1,			// 异动状态-成功
};
/****************************************加载到数据库-end-**************************************************************************************/



#endif // __LogDefine_H__
