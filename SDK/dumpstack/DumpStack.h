//--------------------------------------------------------------------
// 文件名:		DumpStack.h
// 内  容:		输出堆栈
// 说  明:		
// 创建日期:	2013年11月15日
// 创建人:		liugang
//    :	   
//--------------------------------------------------------------------

#ifndef _DumpStack_H_
#define _DumpStack_H_
#include <stdio.h>
#include "StackWalker.h"

// CDumpStack
class CDumpStack : public StackWalker
{
public:
	explicit CDumpStack(const char* strPath);
	~CDumpStack();

	CDumpStack(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}

	virtual void OnOutput(LPCSTR szText);

private:
	FILE* m_pFile;
};

// CFastDumpStack
class CFastDumpStack : public StackWalker
{
public:
	static CFastDumpStack* Instance();
	static void ClearInstance();

	virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
	virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, 
		DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
	virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
	virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
	virtual void OnOutput(LPCSTR szText);

private:
	CFastDumpStack();
	~CFastDumpStack();
	CFastDumpStack(const CFastDumpStack&);
	CFastDumpStack& operator=(const CFastDumpStack&);

	static CFastDumpStack* m_pInstance;
};

#endif //_DumpStack_H_
