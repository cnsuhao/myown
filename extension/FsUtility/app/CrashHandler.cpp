#include "stdafx.h"
#include "CrashHandler.h"
#include "FsAppHelper.h"

#ifndef _AddressOfReturnAddress

// Taken from: http://msdn.microsoft.com/en-us/library/s975zw7k(VS.71).aspx
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// _ReturnAddress and _AddressOfReturnAddress should be prototyped before use 
EXTERNC void * _AddressOfReturnAddress(void);
EXTERNC void * _ReturnAddress(void);

#endif 

CCrashHandler::CCrashHandler()
{	
}

CCrashHandler::~CCrashHandler()
{    
}

void CCrashHandler::SetProcessExceptionHandlers()
{
	// Install top-level SEH handler
	SetUnhandledExceptionFilter(SehHandler);    

	// Catch pure virtual function calls.
	// Because there is one _purecall_handler for the whole process, 
	// calling this function immediately impacts all threads. The last 
	// caller on any thread sets the handler. 
	// http://msdn.microsoft.com/en-us/library/t296ys27.aspx
	_set_purecall_handler(PureCallHandler);    

	// Catch new operator memory allocation exceptions
	_set_new_handler(NewHandler);

	// Catch invalid parameter exceptions.
	_set_invalid_parameter_handler(InvalidParameterHandler); 

	// Set up C++ signal handlers

	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	// Catch an abnormal program termination
	signal(SIGABRT, SigabrtHandler);  

	// Catch illegal instruction handler
	signal(SIGINT, SigintHandler);     

	// Catch a termination request
	signal(SIGTERM, SigtermHandler);          

}

void CCrashHandler::SetThreadExceptionHandlers()
{

	// Catch terminate() calls. 
	// In a multithreaded environment, terminate functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// terminate function. Thus, each thread is in charge of its own termination handling.
	// http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
	set_terminate(TerminateHandler);       

	// Catch unexpected() calls.
	// In a multithreaded environment, unexpected functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// unexpected function. Thus, each thread is in charge of its own unexpected handling.
	// http://msdn.microsoft.com/en-us/library/h46t5b69.aspx  
	set_unexpected(UnexpectedHandler);    

	// Catch a floating point error
	typedef void (*sigh)(int);
	signal(SIGFPE, (sigh)SigfpeHandler);     

	// Catch an illegal instruction
	signal(SIGILL, SigillHandler);     

	// Catch illegal storage access errors
	signal(SIGSEGV, SigsegvHandler);   

}

// The following code gets exception pointers using a workaround found in CRT code.
void CCrashHandler::GetExceptionPointers(DWORD dwExceptionCode, 
										 EXCEPTION_POINTERS** ppExceptionPointers)
{
	// The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

	__asm {
		mov dword ptr [ContextRecord.Eax], eax
			mov dword ptr [ContextRecord.Ecx], ecx
			mov dword ptr [ContextRecord.Edx], edx
			mov dword ptr [ContextRecord.Ebx], ebx
			mov dword ptr [ContextRecord.Esi], esi
			mov dword ptr [ContextRecord.Edi], edi
			mov word ptr [ContextRecord.SegSs], ss
			mov word ptr [ContextRecord.SegCs], cs
			mov word ptr [ContextRecord.SegDs], ds
			mov word ptr [ContextRecord.SegEs], es
			mov word ptr [ContextRecord.SegFs], fs
			mov word ptr [ContextRecord.SegGs], gs
			pushfd
			pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);


#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();

	///

	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;  
}

// This method creates minidump of the process
void CCrashHandler::CreateMiniDump(EXCEPTION_POINTERS* pExcPtrs)
{   
    HMODULE hDbgHelp = NULL;
    HANDLE hFile = NULL;
    MINIDUMP_EXCEPTION_INFORMATION mei;
    MINIDUMP_CALLBACK_INFORMATION mci;
    
    // Load dbghelp.dll
    hDbgHelp = LoadLibrary(_T("dbghelp.dll"));
    if(hDbgHelp==NULL)
    {
        // Error - couldn't load dbghelp.dll
		return;
    }

	const char* pszDumpName = _T("crashdump.dmp");
	const char* pszFile = FsFramework::app::GetCrashFile();
	if (NULL != pszFile)
	{
		pszDumpName = pszFile;
	}

    // Create the minidump file
    hFile = CreateFile(
		pszDumpName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile==INVALID_HANDLE_VALUE)
    {
        // Couldn't create file
		return;
    }
   
    // Write minidump to the file
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = pExcPtrs;
    mei.ClientPointers = FALSE;
    mci.CallbackRoutine = NULL;
    mci.CallbackParam = NULL;

    typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
        HANDLE hProcess, 
        DWORD ProcessId, 
        HANDLE hFile, 
        MINIDUMP_TYPE DumpType, 
        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
        CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam, 
        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

    LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
        (LPMINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(!pfnMiniDumpWriteDump)
    {    
        // Bad MiniDumpWriteDump function
        return;
    }

    HANDLE hProcess = GetCurrentProcess();
	DWORD dwProcessId = GetCurrentProcessId();

    BOOL bWriteDump = pfnMiniDumpWriteDump(
        hProcess,
        dwProcessId,
        hFile,
        MiniDumpNormal,
        &mei,
        NULL,
        &mci);

    if(!bWriteDump)
    {    
        // Error writing dump.
        return;
    }

    // Close file
    CloseHandle(hFile);

    // Unload dbghelp.dll
    FreeLibrary(hDbgHelp);
}

// Structured exception handler
LONG WINAPI CCrashHandler::SehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{ 
	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
	// Unreacheable code  
	return EXCEPTION_EXECUTE_HANDLER;
}

// CRT terminate() call handler
void __cdecl CCrashHandler::TerminateHandler()
{
	// Abnormal program termination (terminate() function was called)
	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
}

// CRT unexpected() call handler
void __cdecl CCrashHandler::UnexpectedHandler()
{
	// Unexpected error (unexpected() function was called)

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    	    
}

// CRT Pure virtual method call handler
void __cdecl CCrashHandler::PureCallHandler()
{
	// Pure virtual function call

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	 
}


// CRT invalid parameter handler
void __cdecl CCrashHandler::InvalidParameterHandler(
	const wchar_t* expression, 
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved)
{
	pReserved;

	// Invalid parameter exception

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	   
}

// CRT new operator fault handler
int __cdecl CCrashHandler::NewHandler(size_t)
{
	// 'new' operator memory allocation exception

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);
	
	// Unreacheable code
	return 0;
}

// CRT SIGABRT signal handler
void CCrashHandler::SigabrtHandler(int)
{
	// Caught SIGABRT C++ signal

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);   
	
}

// CRT SIGFPE signal handler
void CCrashHandler::SigfpeHandler(int /*code*/, int subcode)
{
	// Floating point exception (SIGFPE)

	EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
	
	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
}

// CRT sigill signal handler
void CCrashHandler::SigillHandler(int)
{
	// Illegal instruction (SIGILL)

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
}

// CRT sigint signal handler
void CCrashHandler::SigintHandler(int)
{
	// Interruption (SIGINT)

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
}

// CRT SIGSEGV signal handler
void CCrashHandler::SigsegvHandler(int)
{
	// Invalid storage access (SIGSEGV)

	PEXCEPTION_POINTERS pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
}

// CRT SIGTERM signal handler
void CCrashHandler::SigtermHandler(int)
{
	// Termination request (SIGTERM)

	// Retrieve exception information
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	// crashing callback
	OnCrashing();

	// Write minidump file
	CreateMiniDump(pExceptionPtrs);

	// crashedcallback
	OnCrashed();

	// Terminate process
	TerminateProcess(GetCurrentProcess(), 1);    
	
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
	LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}

BOOL CCrashHandler::DisableUnHandledExceptionFilter()
{
#ifndef _WIN64
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;
	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += sizeof(DWORD)+1; // add 5 for 5 op-codes for jmp far  
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
	//DWORD dwRelativeAddr = dwNewEntryAddr;

	newJump[0] = 0xE9;  // JMP absolute  
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(DWORD));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(dwNewEntryAddr)+1, &bytesWritten);
	return bRet;
#else
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;
	unsigned char newJump[100];
	size_t dwOrgEntryAddr = (size_t)pOrgEntry;
	dwOrgEntryAddr += sizeof(size_t)+2; // add 5 for 5 op-codes for jmp far  
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	size_t dwNewEntryAddr = (size_t)pNewFunc;
	size_t dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
	//DWORD dwRelativeAddr = dwNewEntryAddr;

	newJump[0] = 0x48;  // long JMP absolute  
	newJump[1] = 0xb8;
	memcpy(&newJump[2], &dwNewEntryAddr, sizeof(size_t));
	newJump[10] = 0x50;   // long JMP absolute  
	newJump[11] = 0xc3;
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(size_t)+4, &bytesWritten);
	return bRet;
#endif
}

void CCrashHandler::EnableUnHandlerExceptionFliter()
{

}

void CCrashHandler::OnCrashing()
{
	FsFramework::app::CrashingHook l_crashingHook = FsFramework::app::SetCrahingHook(NULL);
	FsFramework::app::SetCrahingHook(l_crashingHook);
	if (NULL != l_crashingHook)
	{
		l_crashingHook();
	}
}

void CCrashHandler::OnCrashed()
{
	FsFramework::app::CrashedHook l_crashedHook = FsFramework::app::SetCrashedHook(NULL);
	FsFramework::app::SetCrashedHook(l_crashedHook);
	if (NULL != l_crashedHook)
	{
		l_crashedHook();
	}
}

