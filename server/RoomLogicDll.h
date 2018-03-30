//--------------------------------------------------------------------
// 文件名:		RoomLogicDll.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年11月5日
// 创建人:		丁有进
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ROOM_LOGICDLL_H
#define _SERVER_ROOM_LOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "IRoomKernel.h"
#include "IRoomCreator.h"
#include "IRoomModuleCreator.h"

// 定义一个游戏逻辑DLL
#define DECLARE_ROOM_LOGIC_DLL \
ICore* g_pCore = NULL; \
IRoomCreator* g_pRoomCreator = NULL; \
IRoomModuleCreator* g_pRoomModuleCreator = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxGameLogic_GetVersion() { return GAME_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
IRoomCreator* __cdecl FxGameLogic_GetCreator() { return g_pRoomCreator; } \
extern "C" FX_DLL_EXPORT \
IRoomModuleCreator* __cdecl FxGameLogic_GetModuleCreator() { return g_pRoomModuleCreator; } \
ASSERTION_FAIL_DEFINED;

#endif // _SERVER_LOGICDLL_H

