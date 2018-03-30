//--------------------------------------------------------------------
// 文件名:		Portable.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_PORTABLE_H
#define _PUBLIC_PORTABLE_H

#include "Macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinPortable.h"
	#include "../system/WinFileSys.h"
	#include "../system/WinString.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxPortable.h"
	#include "../system/LinuxFileSys.h"
	#include "../system/LinuxString.h"
#endif // FX_SYSTEM_LINUX

#endif // _PUBLIC_PORTABLE_H
