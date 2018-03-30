//--------------------------------------------------------------------
// 文件名:		PubLogicDll.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_PUBLOGICDLL_H
#define _SERVER_PUBLOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "IPubKernel.h"

// 定义一个公共数据逻辑DLL
#define DECLARE_PUBLIC_LOGIC_DLL(a) \
ICore* g_pCore = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxPublicLogic_GetVersion() { return PUBLIC_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
IPubCallee* __cdecl FxPublicLogic_CreateCallee() { return NEW a; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxPublicLogic_DeleteCallee(IPubCallee* p) { delete p; } \
ASSERTION_FAIL_DEFINED;

#endif // _SERVER_PUBLOGICDLL_H

