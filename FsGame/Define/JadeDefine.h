//--------------------------------------------------------------------
// 文件名:		JadeDefine.h
// 内  容:		玉珏
// 说  明:		
//
// 创建日期:    2018年03月23日
// 创建人:      刘明飞      
//--------------------------------------------------------------------
#ifndef __JADE_DEFINE_H__
#define __JADE_DEFINE_H__

#define JADE_BOX_WSTR L"JadeBox"
#define JADE_BOX_STR  "JadeBox"
#define MAX_JADE_BOX_CAPACITY	10		// 玉珏容器最大容量

// 玉珏等级属性包标识
#define JADE_PROPPAK_ID "JadeProp"

// 二级消息
enum JadeC2SMsg
{
	JADE_C2S_LEVEL_UP,		// 玉珏升级 string 玉珏id
	JADE_C2S_STEP_UP,		// 玉珏升阶	string 玉珏id
	JADE_C2S_COMPOSE,		// 玉珏合成	string 玉珏id
	JADE_C2S_WEAR,			// 穿戴玉珏	string 玉珏id
};


enum JadeS2CMsg
{
	JADE_S2C_LEVEL_UP_SUC,	// 玉珏成功
	JADE_S2C_STEP_UP_SUC,// 玉珏升阶成功
};


#endif	// __Jade_DEFINE_H__
