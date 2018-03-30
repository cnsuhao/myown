
/********************************************************************
    file base:	LoopCheck.h	
    created:	2015/07/13
    author:		tangyz
    company:    Suzhou Snail Digital Technology Co., Ltd.

	purpose:	
*********************************************************************/
#ifndef _SERVER_LOOP_CHECK_H
#define _SERVER_LOOP_CHECK_H

#include "../sdk/dumpstack/DumpStack.h"
#include "../public/Inlines.h"
#include "../public/CoreLog.h"

extern int64_t g_nMaxCirculateCount;

void SetMaxCirculateCount(int count);

#define  LoopBeginCheck(name) \
	     int nCheck##name##Count = 0;

#define  LoopDoCheck(name) \
		nCheck##name##Count++;\
		if((g_nMaxCirculateCount > 0) && (nCheck##name##Count >= g_nMaxCirculateCount))\
		{\
            char szLoopCheckbuf[256]; \
            SPRINTF_S(szLoopCheckbuf, "LoopDoCheck: %s %s %d %d", __FILE__, __FUNCTION__, __LINE__, nCheck##name##Count); \
            CORE_TRACE(szLoopCheckbuf); \
			CFastDumpStack::Instance()->ShowCallstack();\
			break;\
		}
#endif // _SERVER_LOOP_CHECK_H

