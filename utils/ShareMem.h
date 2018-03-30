//--------------------------------------------------------------------
// 文件名:		ShareMem.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年8月23日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_SHAREMEM_H
#define _UTILS_SHAREMEM_H

#include "../public/Macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinShareMem.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxShareMem.h"
#endif // FX_SYSTEM_LINUX

/*
#include <windows.h>
#include <string.h>

// 进程间的共享内存

class CShareMem
{
public:
	static bool Exists(const char* name)
	{
		HANDLE hFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);

		if (NULL == hFile)
		{
			return false;
		}

		CloseHandle(hFile);

		return true;
	}

public:
	CShareMem()
	{
		m_pName = NULL;
		m_nSize = 0;
		m_hFile = NULL;
		m_pMem = NULL;
	}

	~CShareMem()
	{
		Destroy();
	}

	const char* GetName() const
	{
		if (NULL == m_pName)
		{
			return "";
		}
		
		return m_pName;
	}

	size_t GetSize() const
	{
		return m_nSize;
	}

	void* GetMem() const
	{
		return m_pMem;
	}

	bool IsValid() const
	{
		return (m_hFile != NULL) && (m_pMem != NULL);
	}
	
	bool Create(const char* name, size_t size, bool* exists = NULL)
	{
		Assert(name != NULL);
		Assert(size > 0);

		size_t name_size = strlen(name) + 1;
		
		char* pName = NEW char[name_size];
		
		memcpy(pName, name, name_size);
		
		if (m_pName)
		{
			delete[] m_pName;
		}
		
		m_pName = pName;
		m_nSize = size;

		if (exists)
		{
			*exists = false;
		}

		m_hFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);

		if (NULL == m_hFile)
		{
			m_hFile = CreateFileMapping(INVALID_HANDLE_VALUE,
				NULL, PAGE_READWRITE, 0, DWORD(m_nSize), name);

			if (NULL == m_hFile)
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

		m_pMem = MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, m_nSize);

		if (NULL == m_pMem)
		{
			CloseHandle(m_hFile);
			m_hFile = NULL;
			return false;
		}
		
		return true;
	}

	bool Destroy()
	{
		if (m_pMem != NULL)
		{
			UnmapViewOfFile(m_pMem);
			m_pMem = NULL;
		}

		if (!CloseHandle(m_hFile))
		{
			return false;
		}

		m_hFile = NULL;
		
		if (m_pName)
		{
			delete[] m_pName;
			m_pName = NULL;
		}
		
		return true;
	}

private:
	CShareMem(const CShareMem&);
	CShareMem& operator=(const CShareMem&);

private:
	char* m_pName;
	size_t m_nSize;
	HANDLE m_hFile;
	void* m_pMem;
};
*/

#endif // _UTILS_SHAREMEM_H

