//--------------------------------------------------------------------
// 文件名:		HttpsLogicDll.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _HTTPS_SERVER_LOGICDLL_H
#define _HTTPS_SERVER_LOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "ICreator.h"
#include "IModuleCreator.h"

// 定义一个游戏逻辑DLL
#define DECLARE_HTTPS_LOGIC_DLL \
ICore* g_pCore = NULL; \
ICreator* g_pCreator = NULL; \
IModuleCreator* g_pModuleCreator = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxGameLogic_GetVersion() { return HTTPS_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
ICreator* __cdecl FxGameLogic_GetCreator() { return g_pCreator; } \
extern "C" FX_DLL_EXPORT \
IModuleCreator* __cdecl FxGameLogic_GetModuleCreator() { return g_pModuleCreator; } \
ASSERTION_FAIL_DEFINED;

#endif // _HTTPS_SERVER_LOGICDLL_H
