//--------------------------------------------------------------------
// 文件名:		PubLogicDll.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_SNS_LOGICDLL_H
#define _SERVER_SNS_LOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "ISnsKernel.h"
#include "ISnsCreator.h"
#include "IModuleCreator.h"

// 定义一个公共数据逻辑DLL
#define DECLARE_SNS_LOGIC_DLL \
ICore* g_pCore = NULL; \
ISnsCreator* g_pSnsCreator = NULL; \
IModuleCreator* g_pModuleCreator = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxSnsLogic_GetVersion() { return SNS_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
ISnsCreator* __cdecl FxSnsLogic_GetCreator() { return g_pSnsCreator; } \
extern "C" FX_DLL_EXPORT \
IModuleCreator* __cdecl FxSnsLogic_GetModuleCreator() { return g_pModuleCreator; } \
ASSERTION_FAIL_DEFINED;

#endif // _SERVER_PUBLOGICDLL_H

