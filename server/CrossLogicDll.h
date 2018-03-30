//--------------------------------------------------------------------
// 文件名:		CrossLogicDll.h
// 内  容:		跨服逻辑功能库定义
// 说  明:		
// 创建日期:	2012年11月01日
// 创建人:		刘刚
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_CROSSLOGICDLL_H
#define _SERVER_CROSSLOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "ICrossKernel.h"

// 定义一个跨域数据逻辑DLL
#define DECLARE_CROSS_LOGIC_DLL(a) \
ICore* g_pCore = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxCrossLogic_GetVersion() { return CROSS_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
ICrossCallee* __cdecl FxCrossLogic_CreateCallee() { return NEW a; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxCrossLogic_DeleteCallee(ICrossCallee* p) { delete p; } \
ASSERTION_FAIL_DEFINED;

#endif // _SERVER_CROSSLOGICDLL_H

