#ifndef __FS_I_LOGGER_H__
#define __FS_I_LOGGER_H__

enum EMLoggerLevel
{
	EM_LOGGER_LEVEL_OFF,
	EM_LOGGER_LEVEL_FATAL,
	EM_LOGGER_LEVEL_ERROR,
	EM_LOGGER_LEVEL_WARN,
	EM_LOGGER_LEVEL_INFO,
	EM_LOGGER_LEVEL_DEBUG,
	EM_LOGGER_LEVEL_TRACE,
	EM_LOGGER_LEVEL_MAX
};

class ILogger
{
public:
	virtual ~ILogger();
	virtual void LogFatal(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;
	virtual void LogError(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;
	virtual void LogWarn(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;
	virtual void LogInfo(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;
	virtual void LogDebug(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;
	virtual void LogTrace(const char* fileName, const char* funcName, int nLine, const char* msg) = 0;

	void SetLogLevel(EMLoggerLevel eLevel)
	{
		m_eLogLevel = eLevel;
	}
	EMLoggerLevel GetLogLevel() const
	{
		return m_eLogLevel;
	}
protected:
	ILogger() : m_eLogLevel(EM_LOGGER_LEVEL_MAX)
	{
	}
	EMLoggerLevel m_eLogLevel;
};
inline ILogger::~ILogger(){}

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4786 )
// Warning about: <type1> needs to have dll-interface to be used by clients of class <type2>
#  pragma warning( disable : 4251 )

#  define FS_INLINES_ARE_EXPORTED

#  if _MSC_VER >= 1400
#    define FS_WORKING_LOCALE
#    define FS_HAVE_FUNCTION_MACRO
#    define FS_HAVE_FUNCSIG_MACRO
#    define FS_HAVE_C99_VARIADIC_MACROS
#    define FS_ATTRIBUTE_NORETURN __declspec(noreturn)
#  endif
#  if _MSC_VER >= 1700
#    define FS_HAVE_CXX11_ATOMICS
#    define FS_WITH_CXX11_THREADS
#  endif
#endif

#if defined (__GNUC__)
#  undef FS_INLINES_ARE_EXPORTED
#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#    define FS_HAVE_PRETTY_FUNCTION_MACRO
#    define FS_HAVE_FUNC_SYMBOL
#  endif
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#    if defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#      define FS_HAVE___SYNC_SUB_AND_FETCH
#      define FS_HAVE___SYNC_ADD_AND_FETCH
#    endif
#  endif
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#    if defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#      define FS_HAVE___ATOMIC_ADD_FETCH
#      define FS_HAVE___ATOMIC_SUB_FETCH
#    endif
// This has worked for some versions of MinGW with GCC 4.7+ but it
// appears to be broken again in 4.8.x. Thus, we disable this for GCC
// completely forever.
//
//#    define FS_INLINES_ARE_EXPORTED
#  endif
#  define FS_HAVE_FUNCTION_MACRO
#  define FS_HAVE_GNU_VARIADIC_MACROS
#  define FS_HAVE_C99_VARIADIC_MACROS
#  if defined (__MINGW32__)
#    define FS_WORKING_C_LOCALE
#  endif
#endif

#if defined (__BORLANDC__) && __BORLANDC__ >= 0x0650
#  define FS_HAVE_FUNCTION_MACRO
#  define FS_HAVE_C99_VARIADIC_MACROS
#endif // __BORLANDC__


#undef FS_MACRO_FUNCTION
#define FS_MACRO_FUNCTION() 0
#if ! defined (FS_DISABLE_FUNCTION_MACRO)
#  if defined (FS_HAVE_FUNCTION_MACRO)
#    undef FS_MACRO_FUNCTION
#    define FS_MACRO_FUNCTION() __FUNCTION__
#  elif defined (FS_HAVE_PRETTY_FUNCTION_MACRO)
#    undef FS_MACRO_FUNCTION
#    define FS_MACRO_FUNCTION() __PRETTY_FUNCTION__
#  elif defined (FS_HAVE_FUNCSIG_MACRO)
#    undef FS_MACRO_FUNCTION
#    define FS_MACRO_FUNCTION() __FUNCSIG__
#  elif defined (FS_HAVE_FUNC_SYMBOL)
#    undef FS_MACRO_FUNCTION
#    define FS_MACRO_FUNCTION() __func__
#  endif
#endif

#ifdef USE_LOG4CPLUS
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#define FsLogger log4cplus::Logger
#define FS_LOG_FATAL( s ) LOG4CPLUS_FATAL( *g_pLogger, s )
#define FS_LOG_ERROR( s ) LOG4CPLUS_ERROR( *g_pLogger, s )
#define FS_LOG_WARN( s ) LOG4CPLUS_WARN( *g_pLogger, s )
#define FS_LOG_INFO( s ) LOG4CPLUS_INFO( *g_pLogger, s )
#define FS_LOG_DEBUG( s ) LOG4CPLUS_DEBUG( *g_pLogger, s )
#define FS_LOG_TRACE( s ) LOG4CPLUS_TRACE( *g_pLogger, s )
#else
#define FsLogger ILogger
#define FS_LOG_FATAL( s ) g_pLogger->LogFatal( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#define FS_LOG_ERROR( s ) g_pLogger->LogError( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#define FS_LOG_WARN( s ) g_pLogger->LogWarn( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#define FS_LOG_INFO( s ) g_pLogger->LogInfo( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#define FS_LOG_DEBUG( s ) g_pLogger->LogDebug( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#define FS_LOG_TRACE( s ) g_pLogger->LogTrace( __FILE__, FS_MACRO_FUNCTION(), __LINE__, s )
#endif

extern FsLogger* g_pLogger;
#define FORMAT_LOG_MAX_SIZE 4096
#include "public/Inlines.h"
#define __XX_LOGGER_MACRO_CONCAT( logger, format, ... )	\
											do	\
											{	\
												char __szLogMsg[FORMAT_LOG_MAX_SIZE] = {0};	\
												SPRINTF_S(__szLogMsg, format, ##__VA_ARGS__);	\
												logger( __szLogMsg );	\
											} while (0)

#define FS_LOG_FATAL_FORMAT( format, ... )	__XX_LOGGER_MACRO_CONCAT(FS_LOG_FATAL, format, ##__VA_ARGS__)
#define FS_LOG_ERROR_FORMAT( format, ... ) __XX_LOGGER_MACRO_CONCAT(FS_LOG_ERROR, format, ##__VA_ARGS__)
#define FS_LOG_WARN_FORMAT( format, ... ) __XX_LOGGER_MACRO_CONCAT(FS_LOG_WARN, format, ##__VA_ARGS__)
#define FS_LOG_INFO_FORMAT( format, ... ) __XX_LOGGER_MACRO_CONCAT(FS_LOG_INFO, format, ##__VA_ARGS__)
#define FS_LOG_DEBUG_FORMAT( format, ... ) __XX_LOGGER_MACRO_CONCAT(FS_LOG_DEBUG, format, ##__VA_ARGS__)
#define FS_LOG_TRACE_FORMAT( format, ... ) __XX_LOGGER_MACRO_CONCAT(FS_LOG_TRACE, format, ##__VA_ARGS__)


#endif // END __FS_I_LOGGER_H__