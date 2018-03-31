//--------------------------------------------------------------------
// 文件名:		GameDefine.h
// 内  容:		通用常用定义模块
// 说  明:		一定常用字符等宏定义
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __GameDefine_h__ 
#define __GameDefine_h__

#include "utils/util_macros.h"
#include "Classes.h"
#include "Fields.h"
#include "InternalFields.h"

// 当创建机器人的时候需要此宏
//#define CREATE_ROBOT

//每次发送的条数  
#ifdef _DEBUG

#define PER_SEND_COUNT  2 //不能为0

#else

#define PER_SEND_COUNT  30 //不能为0

#endif

#define RUN_SPEED  PI2

#define SAFE_DELETE(p) if(NULL != p) {delete(p);(p) = NULL;}

#define SAFE_ARRAY_DELETE(p) if(NULL != p) {delete[](p);(p) = NULL;}

#define _STR(s) #s
#define STR(s) _STR(s)

#define TRY_MODIFY_MOVE_TIME 100
#define TRY_MODIFY_MOVE_TIME_MAX 10000

//指定次数前重试，之后偏移动态找路
#define TRY_RETRY_PREPATH_MAX 3

#define DISTANCE_ADJUST_MAX 15.0f

#define OUTDOOR_SCENE_MAX  500    //野外最大场景编号-500



// 默认MemberId
const int MEMBER_ID_DEFAULT = 0;

// 是否为内部测试状态
#define INTERNAL_TEST_STATE

// 游戏对象类型占用格子数
enum OBJECT_TYPES_PATH_GRID_ENUM
{
	TYPE_PLAYER_PATH_GRID = 1,	        // 玩家
	TYPE_PLAYER_DEAD_PATH_GRID = 0,	    // 死亡状态下占用的格子数
	TYPE_PLAYER_PATH_GRID_OFF = 0,	    // 玩家不占格
	TYPE_PLAYER_PATH_GRID_ON = 1,	    // 玩家占格
};

// 货币种类
enum CAPITAL_TYPE
{
	CAPITAL_MIN_TYPE = 0,

	CAPITAL_COPPER = 0,			// $ 0 = 铜币
	CAPITAL_GOLD = 1,			// $ 1 = 元宝
	CAPITAL_TSILVER = 2,		// $ 2 = 交易银元（拍卖行）
	CAPITAL_BIND_GOLD = 3,		// $ 3 = 绑定元宝
	CAPITAL_SMELT = 4,			// $ 4 = 熔炼值
	CAPITAL_AREAN = 5,			// $ 4 = 竞技积分

	CAPITAL_MAX_TYPE
};

// 穿透
enum
{
	PATH_THROUGH_OFF = 0,				// 关
	PATH_THROUGH_ON = 1,				// 开
};

// 玩家状态
enum EmPlayerBronState
{
	EM_PLAYER_STATE_BRON,			// 出生状态
	EM_PLAYER_STATE_INIT,			// 初始化成功（未设置clientInfo)
	EM_PLAYER_STATE_NORMAL,			// 正常状诚
};

// 消息类型
enum CustomDisplayType
{
	CUSTOM_ADD_HP = 0,				// 加HP	int hp
	CUSTOM_DEC_HP_WITH_EFFECT,		// 减HP+减HP的效果 int hp 
	CUSTOM_PLAY_CG,					// 播放CG string cg float x z x z
}; 

// 可否移动
enum CantMoveType
{
	MOVE_OK = 0, // 可以移动
	MOVE_NO = 1, // 大于 0 不可以移动
};

// 星期
enum WeekDay
{
	EWEEK_SUNDAY,
	EWEEK_MONDAY,
	EWEEK_TUESDAY,
	EWEEK_WEDESDAY,
	EWEEK_THURSDAY,
	EWEEK_FRIDAY,
	EWEEK_SATURDAY,
};

// 能量类型
enum EnergyType
{
	ENERGY_NONE = -1,
	ENERGY_HP,
	ENERGY_MP,	

	ENERGY_MAX
};

// 是否显示提醒定义
enum
{
	NEW_NOTIFY_HIDE = 0,
	NEW_NOTIFY_SHOW = 1,
};

// 最大对象附加显示数量 MaxVisuals = VisualPlayers + MAX_VISUALS_ADD
const int MAX_VISUALS_ADD = 10;


void OnConfigError( bool bError, const char* msg );
#endif

#define ADD_COUNT_BEAT(pKernel, self, sFunc, nTime, nCount)\
if(!pKernel->FindHeartBeat(self, sFunc))\
{\
	pKernel->AddCountBeat(self,sFunc,nTime,nCount);\
}

//添加心跳
#define ADD_HEART_BEAT(pKernel, self, sFunc, nTime)\
if(!pKernel->FindHeartBeat(self, sFunc))\
{\
	pKernel->AddHeartBeat(self,sFunc,nTime);\
}

//删除心跳
#define DELETE_HEART_BEAT(pKernel, self, sFunc)\
if(pKernel->FindHeartBeat(self, sFunc))\
{\
	pKernel->RemoveHeartBeat(self,sFunc);\
}

//添加回调
#define ADD_CRITICAL(pKernel, self, property, sFunc)\
if(!pKernel->FindCritical( self, property, sFunc))\
{ \
	pKernel->AddCritical( self, property, sFunc );\
}

//删除属性回调
#define DELETE_CRITICAL(pKernel, self, property, sFunc)\
	if(pKernel->FindCritical( self, property, sFunc))\
{ \
	pKernel->RemoveCriticalFunc( self, property, sFunc );\
}

//添加临时属性
#define ADD_DATA(pKernel, self, property, propertyType)\
	if(!pKernel->FindData(self, property))\
{ \
	pKernel->AddData(self, property, propertyType);\
}

//删除临时属性
#define REMOVE_DATA(pKernel, self, property)\
	if(pKernel->FindData(self, property))\
{ \
	pKernel->RemoveData(self, property);\
}


//添加回调
#define ADD_RECHOOK(pKernel, self, rec, sFunc)\
	if(!pKernel->FindRecHook( self, rec, sFunc))\
{ \
	pKernel->AddRecHook( self, rec, sFunc );\
}

//添加临时属性
#define ADD_DATA_INT(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataInt(property, 0);\
}
#define ADD_DATA_INT64(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataInt64(property, 0);\
}
#define ADD_DATA_FLOAT(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataFloat(property, 0.0f);\
}
#define ADD_DATA_DOUBLE(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataDouble(property, 0.0f);\
}
#define ADD_DATA_STRING(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataString(property, "");\
}
#define ADD_DATA_WIDESTR(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataWideStr(property, L"");\
}
#define ADD_DATA_OBJECT(pSelfObj, property)\
	if(pSelfObj != NULL && !pSelfObj->FindData(property))\
{ \
	pSelfObj->AddDataObject(property, PERSISTID());\
}

//删除临时属性
#define REMOVE_DATA_TYPE(pSelfObj, property)\
if(pSelfObj != NULL && pSelfObj->FindData(property))\
{ \
	pSelfObj->RemoveData(property);\
}