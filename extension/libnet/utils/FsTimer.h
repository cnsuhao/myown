/*@FileName:	FsTimer.h
* @Author:		李海罗
* @Function:	时间函数
* @Modify:		2012-11-11
*/
#ifndef __FAST_FRAMEWORK_TIMER_INCLUDE__
#define __FAST_FRAMEWORK_TIMER_INCLUDE__

#if defined( _WIN32 ) || defined(_WIN64)
#include <windows.h>
#endif
#include <time.h>
#include <sys/timeb.h>

#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
struct timeval
{
	long tv_sec;
	long tv_usec;
};
#endif

int gettimeofday( struct timeval* tv )
{
	union {
		long long ns100;
		FILETIME ft;
	} now;
	GetSystemTimeAsFileTime(&now.ft);
	tv->tv_usec = (long)((now.ns100 / 10LL) % 1000000LL);
	tv->tv_sec = (long)((now.ns100 - 116444736000000000LL) / 10000000LL);

	return (0);
}

//获取1970年至今UTC的微妙数
time_t get_utc_times()
{
	timeval tv;
	gettimeofday(&tv);
	return ((time_t)tv.tv_sec*(time_t)1000000 + tv.tv_usec);
}

#endif	// END __FAST_FRAMEWORK_TIMER_INCLUDE__