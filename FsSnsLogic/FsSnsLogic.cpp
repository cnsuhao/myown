//--------------------------------------------------------------------
// 文件名:		FsSnsLogic.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "../server/ISnsCallee.h"
#include "../server/ISnsKernel.h"
#include "../server/SnsLogicDll.h"
#include "../public/Inlines.h"
#include <Windows.h>
#include "SnsManager.h"

int64_t g_nMaxCirculateCount = 10000;

void SetMaxCirculateCount(int count)
{
	if (g_nMaxCirculateCount != count)
	{
		g_nMaxCirculateCount = count;
	}
}

DECLARE_SNS_LOGIC_DLL
SNS_SCRIPT_CREATOR(SnsManager);

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{ 
	switch (fdwReason) 
	{ 
	case DLL_PROCESS_ATTACH: 
		break; 
	case DLL_THREAD_ATTACH: 
		break; 
	case DLL_THREAD_DETACH: 
		break; 
	case DLL_PROCESS_DETACH: 
		break; 
	default: 
		break; 
	} 

	return TRUE; 
} 
