//--------------------------------------------------------------------
// 文件名:		WingDefine.h
// 内  容:		翅膀
// 说  明:		
//
// 创建日期:    2018年03月15日
// 创建人:      刘明飞      
//--------------------------------------------------------------------
#ifndef __WING_DEFINE_H__
#define __WING_DEFINE_H__

// 翅膀等级属性包标识
#define WING_PROPPAK_ID "WingProp"

// 二级消息
enum WingC2SMsg
{
	WING_C2S_LEVEL_UP,		// 翅膀升级
	WING_C2S_STEP_UP,		// 翅膀升阶
	WING_C2S_WEAR,			// 穿戴翅膀
	WING_C2S_HIDE,			// 隐藏翅膀
	WING_C2S_CHANGE_MODEL,	// 改变外观 int nWingModel 
};


enum WingS2CMsg
{
	WING_S2C_LEVEL_UP_SUC,	// 翅膀成功
	WING_S2C_STEP_UP_RESULT,// 翅膀升阶结果 int 升阶结果 int 增加的祝福值(失败时有用)
};

// 升阶结果
enum 
{
	WING_STEP_UP_FAILED,	// 升阶失败 加祝福值	
	WING_STEP_UP_SUC,		// 升阶成功
};

// 翅膀外观
enum 
{
	NO_WEAR_WING,			
	WEAR_WING,				
};


#endif	// __WING_DEFINE_H__
