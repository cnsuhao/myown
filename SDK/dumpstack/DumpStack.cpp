//--------------------------------------------------------------------
// 文件名:		DumpStack.cpp
// 内  容:		输出堆栈
// 说  明:		
// 创建日期:	2013年11月15日
// 创建人:		liugang
//    :	   
//--------------------------------------------------------------------
#include "DumpStack.h"
#include "../../public/Portable.h"
#include "../../public/CoreLog.h"
#include "../../public/Inlines.h"

CDumpStack::CDumpStack(const char* strPath): StackWalker(RetrieveNone)
{
	m_pFile = NULL;

	if ((NULL == strPath) || (strlen(strPath) == 0))
	{
		m_pFile = Port_FileOpen(".\\dump.log", "wb");
	}
	else
	{
		m_pFile = Port_FileOpen(strPath, "rb");
	}
}

CDumpStack::~CDumpStack()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
	}
}

void CDumpStack::OnOutput(LPCSTR szText)
{
	if (NULL == m_pFile)
	{
		return;
	}

	fprintf(m_pFile, "%s \n", szText);
}

CFastDumpStack* CFastDumpStack::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = NEW CFastDumpStack();
	}

	return m_pInstance;
}

// 释放
void CFastDumpStack::ClearInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CFastDumpStack* CFastDumpStack::m_pInstance = NULL;

CFastDumpStack::CFastDumpStack(): StackWalker(RetrieveNone)
{
	m_pInstance = NULL;
}

CFastDumpStack::~CFastDumpStack()
{

}

void CFastDumpStack::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
{
}

void CFastDumpStack::OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, 
		DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
{

}

void CFastDumpStack::OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
{
	switch(eType)
	{
	case firstEntry:
		CORE_TRACE("[STACK] BEGIN");
		break;
	case nextEntry:
		if (entry.offset != 0)
		{
			char buf[256];
			SafeSprintf(buf, sizeof(buf),
				"[STACK] %s:%d [%s] %s(0x%0x)", entry.lineFileName, entry.lineNumber,
				entry.moduleName, entry.name, entry.offset);
			CORE_TRACE(buf);
		}
		break;
	case lastEntry:
		if (entry.offset != 0)
		{
			char buf[256];
			SafeSprintf(buf, sizeof(buf),
				"[STACK] %s:%d [%s] %s(0x%0x)", entry.lineFileName, entry.lineNumber,
				entry.moduleName, entry.name, entry.offset);
			CORE_TRACE("[STACK] EDN");
		}
		break;
	default:
		break;
	}
}

void CFastDumpStack::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
{
}

void CFastDumpStack::OnOutput(LPCSTR szText)
{
}
