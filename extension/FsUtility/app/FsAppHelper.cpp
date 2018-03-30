//--------------------------------------------------------------------
// 文件名:		FsAppHelper.h
// 内  容:		应用程序辅助类
// 说  明:		
// 创建日期:		2013年5月18日
// 创建人:		李海罗
//--------------------------------------------------------------------
#include "stdafx.h"
#include "FsAppHelper.h"
#include "CrashHandler.h"
#include <string>

#ifdef __cplusplus
	extern "C"
	{
		typedef void(*CrashingHook)();
		typedef void(*CrashedHook)();

		typedef bool(*ThreadHook)(bool bTerminate);
	}
#endif

CrashingHook g_cbCrashingHook = NULL;
CrashedHook g_cbCrashedHook = NULL;
ThreadHook g_cbThreadHook = NULL;
std::string g_strCrashName;

void SetProcessExceptionHandlers()
{
	CCrashHandler::SetProcessExceptionHandlers();
}

void UnSetProcessExceptionHandlers()
{
	
}

void EventThreadStart()
{
	CCrashHandler::SetThreadExceptionHandlers();
	if (NULL != g_cbThreadHook)
	{
		g_cbThreadHook( true );
	}
}

void EventThreadStop()
{
	if (NULL != g_cbThreadHook)
	{
		g_cbThreadHook(false);
	}
}

ThreadHook SetThreadHook(ThreadHook cbHook)
{
	ThreadHook cbOrgHook = g_cbThreadHook;
	g_cbThreadHook = cbHook;
	return cbOrgHook;
}

void DisableUnHandledExceptionFilter()
{
	CCrashHandler::DisableUnHandledExceptionFilter();
}
void EnableUnHandlerExceptionFliter()
{
	CCrashHandler::EnableUnHandlerExceptionFliter();
}

void SetCrashFile(const char* pszFileName)
{
	if (NULL != pszFileName)
	{
		g_strCrashName = pszFileName;
	}
}
const char* GetCrashFile()
{
	return g_strCrashName.c_str();
}

CrashingHook SetCrahingHook(CrashingHook cbHook)
{
	CrashingHook cbOrigHook = g_cbCrashingHook;
	g_cbCrashingHook = cbHook;
	return cbOrigHook;
}

CrashedHook SetCrashedHook(CrashedHook cbHook)
{
	CrashedHook cbOrigHook = g_cbCrashedHook;
	g_cbCrashedHook = cbHook;
	return cbOrigHook;
}


#ifdef __cplusplus
namespace FsFramework
{
	namespace app
	{
		extern "C"
		{
#endif 
			void SetProcessExceptionHandlers()
			{
				::SetProcessExceptionHandlers();
			}

			void UnSetProcessExceptionHandlers()
			{
				::UnSetProcessExceptionHandlers();
			}

			void EventThreadStart()
			{
				::EventThreadStart();
			}

			void EventThreadStop()
			{
				::EventThreadStop();
			}

			ThreadHook SetThreadHook(ThreadHook cbHook)
			{
				return (ThreadHook)::SetThreadHook((::ThreadHook)cbHook);
			}

			void DisableUnHandledExceptionFilter()
			{
				::DisableUnHandledExceptionFilter();
			}

			void EnableUnHandlerExceptionFliter()
			{
				::EnableUnHandlerExceptionFliter();
			}

			void SetCrashFile(const char* pszFileName)
			{
				::SetCrashFile(pszFileName);
			}

			const char* GetCrashFile()
			{
				return ::GetCrashFile();
			}

			CrashingHook SetCrahingHook(CrashingHook cbHook)
			{
				return (CrashingHook)::SetCrahingHook((::CrashedHook)cbHook);
			}

			CrashedHook SetCrashedHook(CrashedHook cbHook)
			{
				return (CrashedHook)::SetCrashedHook((::CrashedHook)cbHook);
			}

#ifdef __cplusplus
		}
	}
}
#endif 