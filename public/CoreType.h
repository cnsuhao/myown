//--------------------------------------------------------------------
// 文件名:		CoreType.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年6月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORETYPE_H
#define _PUBLIC_CORETYPE_H

#include "Macros.h"
#include "CoreMem.h"
#include "FastStr.h"
#include "Var.h"
#include "VarList.h"

typedef TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> core_string;
typedef TFastStr<wchar_t, 16, TCharTraits<wchar_t>, TCoreAlloc> core_wstring;
typedef TFastStr<char, 128, TCharTraits<char>, TCoreAlloc> core_long_string;
typedef TFastStr<wchar_t, 128, TCharTraits<wchar_t>, TCoreAlloc> core_long_wstring;
typedef TVar<4, TCoreAlloc> core_var;
typedef TVarList<8, 128, TCoreAlloc> core_var_list;

#endif // _PUBLIC_CORETYPE_H
