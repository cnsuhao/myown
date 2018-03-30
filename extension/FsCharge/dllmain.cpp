// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <vld.h>
#ifdef USE_LOG4CPLUS
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "FsLogger.h"

const char* LOG_PROPERITIES = "../res/game_charge.properties";
log4cplus::Logger g_logger;
#else
#include "GameLogger.h"
#endif
#include "../app/FsAppHelper.h"

FsLogger* g_pLogger = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		FsFramework::app::SetCrashFile("game_charge.dmp");
#ifdef USE_LOG4CPLUS
		log4cplus::initialize();
		log4cplus::tstring pathname(LOG_PROPERITIES);
		log4cplus::PropertyConfigurator::doConfigure(pathname);
		g_logger = log4cplus::Logger::getInstance("MAIN");
		g_pLogger = &g_logger;
#else
		g_pLogger = new GameLogger();
#endif
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#ifdef USE_LOG4CPLUS
		log4cplus::Logger::shutdown();
		//log4cplus::threadCleanup();
		if (NULL != g_pLogger)
		{
			//delete g_pLogger;
			g_pLogger = NULL;
		}
#endif
		break;
	}
	return TRUE;
}

