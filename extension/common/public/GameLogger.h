//--------------------------------------------------------------------
// 文件名:		Logger.h
// 内  容:		游戏日志接口
// 说  明:		
// 创建日期:		2016年9月9日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __GAME_LOGGER_H__
#define __GAME_LOGGER_H__

#include "public/ICore.h"
#include "FsLogger.h"
#include "public/Inlines.h"

extern ICore* g_pCore;

class GameLogger : public ILogger
{
public:

	virtual void LogFatal(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		WriteLog(EM_LOGGER_LEVEL_FATAL, fileName, funcName, nLine, msg);
	}

	virtual void LogError(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		WriteLog(EM_LOGGER_LEVEL_ERROR, fileName, funcName, nLine, msg);
	}

	virtual void LogWarn(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		WriteLog(EM_LOGGER_LEVEL_WARN, fileName, funcName, nLine, msg);
	}

	virtual void LogInfo(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		WriteLog(EM_LOGGER_LEVEL_INFO, fileName, funcName, nLine, msg);
	}

	virtual void LogDebug(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_BLUE);
		WriteLog(EM_LOGGER_LEVEL_DEBUG, fileName, funcName, nLine, msg);
	}

	virtual void LogTrace(const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		SetTextColor(FOREGROUND_GREEN);
		WriteLog(EM_LOGGER_LEVEL_TRACE, fileName, funcName, nLine, msg);
	}

private:
	void WriteLog(EMLoggerLevel eLevel, const char* fileName, const char* funcName, int nLine, const char* msg)
	{
		if (eLevel <= this->m_eLogLevel)
		{
			char szLog[FORMAT_LOG_MAX_SIZE] = { 0 };
			if (NULL == fileName) fileName = "";
			if (NULL == funcName) funcName = "";
			if (NULL == msg) msg = "";

			//std::string date;
			//util_convert_date_to_string2(date);
			//SPRINTF_S(szLog, "[%s][%s][%s:%d][%s]%s", GetLogName(eLevel), date.c_str(), fileName, nLine, funcName, msg);
			SPRINTF_S(szLog, "[%s][%d][%s]%s", GetLogName(eLevel), nLine, funcName, msg);
			g_pCore->TraceLog(szLog);
		}
	}

	const char* GetLogName(EMLoggerLevel eLevel) const
	{
		static const char* szLogNames[EM_LOGGER_LEVEL_MAX] = {
			"OFF",
			"FATAL",
			"ERROR",
			"WARN",
			"INFO",
			"DEBUG",
			"TRACE"
		};

		if (eLevel >= 0 && eLevel < EM_LOGGER_LEVEL_MAX)
		{
			return szLogNames[eLevel];
		}

		return "UNKOWN";
	}

	void SetTextColor( int nIndex )
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, nIndex);
	}
};


#endif // END __GAME_LOGGER_H__