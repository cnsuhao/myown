//--------------------------------------------------------------------
// 文件名:		Mutex.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年8月23日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_MUTEX_H
#define _UTILS_MUTEX_H

#include "../public/Macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinMutex.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxMutex.h"
#endif // FX_SYSTEM_LINUX

/*
#include <windows.h>
#include <string.h>

// 进程互斥锁

class CWinMutex
{
public:
	static bool Exists(const char* name)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

		if (NULL == mutex)
		{
			return false;
		}

		CloseHandle(mutex);

		return true;
	}

public:
	CWinMutex()
	{
		m_pName = NULL;
		m_hMutex = NULL;
	}

	~CWinMutex()
	{
		Destroy();
	}

	// 获得名字
	const char* GetName() const
	{
		if (NULL == m_pName)
		{
			return "";
		}
		
		return m_pName;
	}
	
	// 是否有效
	bool IsValid() const
	{
		return (m_hMutex != NULL);
	}
	
	// 创建或获得锁
	bool Create(const char* name, bool* exists = NULL)
	{
		Assert(name != NULL);

		size_t name_size = strlen(name) + 1;
		
		char* pName = NEW char[name_size];
		
		memcpy(pName, name, name_size);
		
		if (m_pName)
		{
			delete[] m_pName;
		}
		
		m_pName = pName;
		
		if (exists)
		{
			*exists = false;
		}

		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

		if (NULL == m_hMutex)
		{
			m_hMutex = CreateMutex(NULL, FALSE, name);

			if (NULL == m_hMutex)
			{
				return false;
			}

			if (exists)
			{
				if (GetLastError() == ERROR_ALREADY_EXISTS)
				{
					*exists = true;
				}
			}
		}
		else
		{
			if (exists)
			{
				*exists = true;
			}
		}

		return true;
	}

	// 删除锁
	bool Destroy()
	{
		if (!CloseHandle(m_hMutex))
		{
			return false;
		}

		m_hMutex = NULL;
		
		if (m_pName)
		{
			delete[] m_pName;
			m_pName = NULL;
		}
		
		return true;
	}

	// 加锁
	void Lock()
	{
		Assert(m_hMutex != NULL);

		WaitForSingleObject(m_hMutex, INFINITE);
	}

	// 解锁
	void Unlock()
	{
		Assert(m_hMutex != NULL);

		ReleaseMutex(m_hMutex);
	}

private:
	CWinMutex(const CWinMutex&);
	CWinMutex& operator=(const CWinMutex&);

private:
	char* m_pName;
	HANDLE m_hMutex;
};
*/

// 自动加解锁

class CAutoMutex
{
public:
	explicit CAutoMutex(CMutex& mutex): m_Mutex(mutex)
	{
		Assert(mutex.IsValid());

		m_Mutex.Lock();
	}

	~CAutoMutex()
	{
		m_Mutex.Unlock();
	}

private:
	CAutoMutex();
	CAutoMutex(const CAutoMutex&);
	CAutoMutex& operator=(const CAutoMutex&);

private:
	CMutex& m_Mutex;
};

#endif // _UTILS_MUTEX_H
