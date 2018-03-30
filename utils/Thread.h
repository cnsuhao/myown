//--------------------------------------------------------------------
// 文件名:		Thread.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年10月31日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#ifndef _UTILS_THREAD_H
#define _UTILS_THREAD_H

#include "../public/Macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinThread.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxThread.h"
#endif // FX_SYSTEM_LINUX

/*
#include <windows.h>
#include <process.h>

// CThread
// 线程

class CThread
{
private:
	typedef void (__cdecl* THREAD_FUNC)(void*);
	
	// 线程函数
	static void __cdecl WorkerProc(void* lpParameter)
	{
		CThread* pthis = (CThread*)lpParameter;
		
		THREAD_FUNC func = pthis->m_ThreadFunc;
		void* context = pthis->m_pContext;
		int sleep_ms = pthis->m_nSleep;
		
		while (!pthis->m_bQuit)
		{
			func(context);
			
			Sleep(sleep_ms);
		}
	}
	
public:
	CThread(THREAD_FUNC func, void* context, int sleep_ms, int stack_size)
	{
		m_ThreadFunc = func;
		m_pContext = context;
		m_nSleep = sleep_ms;		
		m_nStackSize = stack_size;
		m_bQuit = false;
		m_hThread = NULL;
	}
	
	~CThread()
	{
	}
	
	// 启动线程
	bool Start()
	{
		m_bQuit = false;
	
		m_hThread = (HANDLE)_beginthread(WorkerProc, m_nStackSize, this);
			
		return true;
	}
	
	// 停止线程
	bool Stop()
	{
		m_bQuit = true;
	
		if (m_hThread != NULL)
		{
			WaitThreadExit(m_hThread);
			m_hThread = NULL;
		}
	
		return true;
	}
	
private:
	CThread();
	CThread(const CThread&);
	CThread& operator=(const CThread&);
	
	// 等待线程结束
	bool WaitThreadExit(HANDLE handle)
	{
		DWORD exit_code;

		if (GetExitCodeThread(handle, &exit_code) == FALSE)
		{
			return false;
		}

		while (exit_code == STILL_ACTIVE)
		{
			Sleep(0);

			if (GetExitCodeThread(handle, &exit_code) == FALSE)
			{
				return false;
			}
		}

		return true;
	}

private:
	THREAD_FUNC m_ThreadFunc;
	void* m_pContext;
	int m_nSleep;
	int m_nStackSize;
	bool m_bQuit;
	HANDLE m_hThread;
};
*/

#endif // _UTILS_THREAD_H

