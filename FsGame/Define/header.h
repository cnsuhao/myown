//--------------------------------------------------------------------
// 文件名:		header.h
// 内  容:		游戏必要头文件
// 说  明:		
// 创建日期:	2015年11月24日
// 创建人:		     
// 版权所有:	 
//--------------------------------------------------------------------

#ifndef _GAME_HEADER_H
#define _GAME_HEADER_H

#ifdef FSROOMLOGIC_EXPORTS
#include "server/IPubData.h"
#include "server/IRoomKernel.h"
#include "server/IRoomLogicModule.h"
#define ILogicModule IRoomLogicModule
#else
#include "server/IKernel.h"
#include "server/ILogicModule.h"
#endif
#include "server/ILogicModule_t.h"




#endif // _GAME_HEADER_H

