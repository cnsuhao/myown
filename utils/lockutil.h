//-------------------------------------------------------------------- 
// 文件名:		LockUtil.h 
// 内  容:		
// 说  明:		
// 创建日期:	2002年7月9日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_LOCKUTIL_H
#define _UTILS_LOCKUTIL_H

#include "../public/Macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinLock.h"
	#include "../system/WinLockVal.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxLock.h"
	#include "../system/LinuxLockVal.h"
#endif // FX_SYSTEM_LINUX

/*
#include <windows.h>

// 共享资源互斥访问锁

class CLockUtil
{
public:
	CLockUtil() 
	{ 
		InitializeCriticalSection(&m_csHandle); 
	}

	~CLockUtil() 
	{ 
		DeleteCriticalSection(&m_csHandle); 
	}

	// 锁定
	void Lock() 
	{ 
		EnterCriticalSection(&m_csHandle); 
	}

	// 释放
	void Unlock() 
	{	
		LeaveCriticalSection(&m_csHandle); 
	}

private:
	CRITICAL_SECTION m_csHandle;
};
*/

// 用结构析构函数自动加解锁

class CAutoLock
{
public:
	explicit CAutoLock(CLockUtil& lock): m_Lock(lock)
	{
		m_Lock.Lock();
	}
	
	~CAutoLock() 
	{ 
		m_Lock.Unlock(); 
	}

private:
	CAutoLock();
	CAutoLock(const CAutoLock&);
	CAutoLock& operator=(const CAutoLock&);

private:
	CLockUtil& m_Lock;
};

/*
// CLockVal
// 带锁的整数

class CLockVal
{
public:
	CLockVal()
	{
		m_nVal = 0;
	}

	int Inc()
	{
		return InterlockedIncrement(&m_nVal);
	}

	int Dec()
	{
		return InterlockedDecrement(&m_nVal);
	}

	int Set(int val)
	{
		return InterlockedExchange(&m_nVal, val);
	}

	int Get() const
	{
		return m_nVal;
	}

public:
	LONG m_nVal;
};
*/

#endif // _UTILS_LOCKUTIL_H
