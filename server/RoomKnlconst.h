//--------------------------------------------------------------------
// 文件名:		KnlConst.h
// 内  容:		与核心部分相关的定义
// 说  明:		
// 创建日期:	2006年1月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ROOM_KNLCONST_H
#define _SERVER_ROOM_KNLCONST_H

#include "KnlConst.h"
#include "../public/PersistId.h"

class IKernel;
class IRoomCallee;
class IVarList;
class IVar;

// LUA脚本扩展函数
typedef int (__cdecl* ROOM_LUA_EXT_FUNC)(void* state);
// 心跳函数
typedef int (__cdecl* ROOM_HEARTBEAT_FUNC)(IKernel* pKernel, const PERSISTID& self,
	int time);
// 属性回调函数
typedef int (__cdecl* ROOM_CRITICAL_FUNC)(IKernel* pKernel, const PERSISTID& self,
	const char* property, const IVar& old);
// 表格钩子函数
typedef int (__cdecl* ROOM_RECHOOK_FUNC)(IKernel* pKernel, const PERSISTID& self, 
	const char* record, int op_type, int row, int col);
// 事务脚本扩展函数
typedef int (__cdecl* ROOM_TASK_FUNC)(void* context);
// 逻辑类创建回调函数
typedef int (__cdecl* ROOM_LOGIC_CLASS_FUNC)(IKernel* pKernel, int index);
// 逻辑事件回调函数
typedef int (__cdecl* ROOM_LOGIC_EVENT_FUNC)(IKernel* pKernel, 
	const PERSISTID& self, const PERSISTID& sender, const IVarList& msg);

// 定义LUA脚本扩展函数
#define DECL_LUA_EXT(p) pKernel->DeclareLuaExt(#p, p)
// 定义心跳函数
#define DECL_HEARTBEAT(p) pKernel->DeclareHeartBeat(#p, p)
// 定义属性回调函数
#define DECL_CRITICAL(p) pKernel->DeclareCritical(#p, p)
// 定义表格操作回调函数
#define DECL_RECHOOK(p) pKernel->DeclareRecHook(#p, p)

// 定义事务脚本扩展函数
//#define DECL_TASKFUNC_0(res, p) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList())
//#define DECL_TASKFUNC_1(res, p, a0) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0)
//#define DECL_TASKFUNC_2(res, p, a0, a1) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1)
//#define DECL_TASKFUNC_3(res, p, a0, a1, a2) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2)
//#define DECL_TASKFUNC_4(res, p, a0, a1, a2, a3) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3)
//#define DECL_TASKFUNC_5(res, p, a0, a1, a2, a3, a4) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3 << a4)
//#define DECL_TASKFUNC_6(res, p, a0, a1, a2, a3, a4, a5) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3 << a4 << a5)
//#define DECL_TASKFUNC_7(res, p, a0, a1, a2, a3, a4, a5, a6) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3 << a4 << a5 << a6)
//#define DECL_TASKFUNC_8(res, p, a0, a1, a2, a3, a4, a5, a6, a7) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7)
//#define DECL_TASKFUNC_9(res, p, a0, a1, a2, a3, a4, a5, a6, a7, a8) pKernel->DeclareTaskFunc( \
//	#p, p, res, CVarList() << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8)

//// 游戏对象类型
//enum OBJECT_TYPES_ENUM
//{
//	TYPE_SCENE = 1,		// 场景
//	TYPE_PLAYER = 2,	// 玩家
//	TYPE_NPC = 4,		// NPC
//	TYPE_ITEM = 8,		// 物品
//	TYPE_HELPER = 16,	// 辅助对象
//	TYPE_WEAKBOX = 32,	// 弱关联容器
//};
//
//// 游戏对象的碰撞外形分类
//enum SHAPE_TYPES_ENUM
//{
//	SHAPE_CYLINDER = 0,	// 圆柱体
//	SHAPE_SPHERE = 1,	// 球体
//	SHAPE_POLYGON = 2,	// 多边形柱体
//};
//
//// OnStore回调的类型（type）参数的取值
//enum STORE_TYPES_ENUM
//{
//	STORE_EXIT,		// 玩家离开游戏
//	STORE_TIMING,	// 定时保存
//	STORE_SWITCH,	// 切换场景前保存
//	STORE_MANUAL,	// 人工保存（调用SavePlayerData）
//	STORE_EDIT,		// 修改不在线角色数据后的保存
//	STORE_RECREATE,	// 恢复角色数据后的保存
//};
//
//// 对象的移动模式
//enum MOTION_MODE_ENUM
//{
//	MMODE_STOP,		// 停止
//	MMODE_MOTION,	// 地表移动
//	MMODE_JUMP,		// 跳跃
//	MMODE_JUMPTO,	// 改变跳跃的目标方向
//	MMODE_FLY,		// 空中移动
//	MMODE_SWIM,		// 水中移动
//	MMODE_DRIFT,	// 水面移动
//	MMODE_CLIMB,	// 爬墙
//	MMODE_SLIDE,	// 在不可行走范围内滑行
//	MMODE_SINK,		// 下沉
//	MMODE_LOCATE,	// 强制定位
//};
//
//// OnMotion回调的移动结果（result)参数的取值，未标注的是内部使用值
//enum MOTION_RESULT_ENUM
//{
//	MRESULT_STATIC,
//	MRESULT_CONTINUE,
//	MRESULT_SUCCEED,		// 成功到达
//	MRESULT_COLLIDE_SCENE,	// 碰到场景
//	MRESULT_COLLIDE_OBJECT,	// 碰到对象
//};
//
//// 表格操作的类型（用于表钩子RecHook）
//enum RECORD_OPERATE_TYPES_ENUM
//{
//	RECOP_INIT,			// AddRecHook时触发的钩子
//	RECOP_ADD_ROW,		// 添加行
//	RECOP_REMOVE_ROW,	// 删除行
//	RECOP_CLEAR_ROW,	// 清空
//	RECOP_GRID_CHANGE,	// 表元数据改变
//	RECOP_SET_ROW,		// 某一行的数据改变
//};
//
//// 跨服角色存档类型
//enum CROSS_SAVE_TYPES_ENUM
//{
//	CROSS_SAVE_IN  = 200, // 回传角色存档（战场服的操作类型）
//	CROSS_SAVE_OUT = 300, // 传出角色存档（普通服的操作类型）
//};
//
//// 跨服角色状态
//enum CROSS_STATUS_ENUM
//{
//	CS_INVALID = 0,	  // 无效
//	CS_WAITING = 100, // 等待转入
//	CS_NORMAL  = 101, // 已转入(转入服务器的正常状态)
//	CS_BACKING = 102, // 正在回传中
//	CS_GOING   = 103, // 正在转出
//	CS_CROSSED = 104, // 已转出
//	// normal server: CS_GOING -> CS_CROSSED -> CS_INVALID
//	// fight server : CS_WAITING -> CS_NORMAL -> CS_BACKING -> CS_INVALID
//};
//
//enum LIMIT_TIME_TYPE_ENUM
//{
//	JIANGHU_LIMIT_TIME = 2,	//江湖明俊
//	ZHIZUN_LIMIT_TIME  =3,	//至尊明俊
//};
//
//enum RESEND_TO_PLAYER_ENUM
//{
//	RTP_NULL,
//	RTP_SEND_LETTER,
//	RTP_QUERY_LETTER,
//	RTP_RECV_LETTER,
//	RTP_RECV_LETTER_FAIL,
//	RTP_LOOK_LETTER,
//	RTP_CLEAN_LETTER,
//	RTP_REJECT_LETTER,
//};

#endif // _SERVER_KNLCONST_H


