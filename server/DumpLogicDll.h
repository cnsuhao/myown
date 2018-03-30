//--------------------------------------------------------------------
// 文件名:		DumpLogicDll.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_DUMPLOGICDLL_H
#define _SERVER_DUMPLOGICDLL_H

#include "../public/Macros.h"
#include "../public/ICore.h"
#include "IDumpKernel.h"

// 定义一个数据转储逻辑DLL
#define DECLARE_DUMP_LOGIC_DLL(a) \
ICore* g_pCore = NULL; \
extern "C" FX_DLL_EXPORT \
const char* __cdecl FxModule_GetType() { return "FxGameLogic"; } \
extern "C" FX_DLL_EXPORT \
void __cdecl FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
int __cdecl FxDumpLogic_GetVersion() { return DUMP_LOGIC_MODULE_VERSION; } \
extern "C" FX_DLL_EXPORT \
IDumpCallee* __cdecl FxDumpLogic_GetCallee() { static a s_##a; return &s_##a; } \
ASSERTION_FAIL_DEFINED;

#endif // _SERVER_DUMPLOGICDLL_H

