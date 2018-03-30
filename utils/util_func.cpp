//--------------------------------------------------------------------
// 文件名:		util_func.cpp
// 内  容:		工具函数
// 说  明:		
// 创建日期:	2008年10月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------
#include "../../public/Converts.h"
#include "../../public/Inlines.h"
#include "../../public/AutoMem.h"
#include "../../public/Portable.h"
#include "util_func.h"
#include <string>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include "util.h"

#define RAND_MAX1 (RAND_MAX + 1)
#define FLOAT_EPSILON 1.192092896e-07F

#ifdef OPEN_COST_TIME
std::vector<std::string> CostTime::m_vecCostTime;
#endif

const int days_in_months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 返回0-(scale-1)的随机整数
int util_random_int(int scale)
{
    if ( scale <= 1 )
    {
        return 0;
    }

	std::uniform_int_distribution<int> dis(0, scale-1);
	return dis(randomSeed);
}

// 返回0-scale)的随机浮点数
float util_random_float(float scale)
{
	if (scale < 0.0f)
	{
		return 0.0f;
	}

	std::uniform_real_distribution<float> dis(0.0, scale);
	return dis(randomSeed);
}

// 调整两个数的大小后返回
void util_sort_int(int& minvalue, int& maxvalue)
{
	if (minvalue > maxvalue)
	{
		int tmpvalue = minvalue;
		minvalue = maxvalue;
		maxvalue = tmpvalue;
	}
}

void util_sort_float(float& minvalue, float& maxvalue)
{
	if (minvalue > maxvalue)
	{
		float tmpvalue = minvalue;
		minvalue = maxvalue;
		maxvalue = tmpvalue;
	}
}

// 分割字符串
void util_split_string(IVarList& result, const std::string& str,
					   const std::string& delims, unsigned int max_splits)
{
	if (str.empty())
	{
		return;
	}

	unsigned int num_splits = 0;

	size_t start, pos;
	start = 0;
	do
	{
		pos = str.find(delims, start);
		if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
		{
			result << util_trim_string(str.substr(start)).c_str();
			break;
		}
		else
		{
			result << util_trim_string(str.substr(start, pos - start)).c_str();
			start = pos + delims.size();
		}

		num_splits++;

	}
	while (pos != std::string::npos);
	return;
}
bool util_split_express_string(IVarList& result, const std::string& str, const char* exp_equa, const char* exp_split)
{
	CVarList parse_exp;
	util_split_string(parse_exp, str, exp_split);
	if (parse_exp.GetCount() <= 0)
	{
		return false;
	}
	CVarList parse_pair;
	const char* string_exp = NULL;
	const char* string_value = NULL;
	for (int index = 0; index < (int)parse_exp.GetCount(); index++)
	{
		parse_pair.Clear();
		util_split_string(parse_pair, parse_exp.StringVal(index), exp_equa);
		if (parse_pair.GetCount() != 2)
		{
			continue;
		}
		string_exp = parse_pair.StringVal(0);
		string_value = parse_pair.StringVal(1);
		if (string_exp == NULL || strcmp(string_exp, "") == 0 || string_value == NULL || strcmp(string_value, "") == 0)
		{
			continue;
		}
		result.AddString(string_exp);
		result.AddString(string_value);
	}
	return true;
}

void util_split_string_of(IVarList& result, const std::string& str,
						  const std::string& delims, unsigned int max_splits)
{
	if (str.empty())
	{
		return;
	}

	unsigned int num_splits = 0;

	size_t start, pos;
	start = 0;
	do
	{
		pos = str.find_first_of(delims, start);
		if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
		{
			result << util_trim_string(str.substr(start)).c_str();
			break;
		}
		else
		{
			result << util_trim_string(str.substr(start, pos - start)).c_str();
			start = pos + 1;
		}

		num_splits++;

	}
	while (pos != std::string::npos);
	return;
}
// 分割字符串,带圆括号的
bool util_split_string_with_parentheses(IVarList& result, const std::string& str, char delims)
{
	if (0 == str.compare(""))
	{
		return false;
	}

	int cur_level = 0;

	int str_len = (int)str.length();
	const char* p = str.c_str();

	std::string tmpstr = "";
	for (int i=0; i<str_len; ++i)
	{
		switch (p[i])
		{
		case '(':
			++cur_level;
			break;
		case ')':
			--cur_level;
			break;
		default:
			if (delims == p[i])
			{
				if (cur_level == 0)
				{
					result << tmpstr;
					tmpstr.clear();
					continue;
				}
			}

			break;
		} // end of switch (...

		if (cur_level < 0)
		{ // 括号不配对
			return false;
		}

		tmpstr += p[i];
	} // end of for ...

	if (cur_level != 0)
	{
		return false;
	}

	result << tmpstr;

	return true;
}

// 分割字符串
void util_split_wstring(IVarList& result, const std::wstring& str,
						const std::wstring& delims, unsigned int max_splits)
{
	if (0 == str.compare(L""))
	{
		return;
	}

	unsigned int num_splits = 0;

	size_t start, pos;
	start = 0;
	do
	{
		pos = str.find(delims, start);
		if (pos == start)
		{
			start = pos + delims.size();
		}
		else if (pos == std::wstring::npos || (max_splits > 0 && num_splits == max_splits))
		{
			result << util_trim_wstring(str.substr(start)).c_str();
			break;
		}
		else
		{
			result << util_trim_wstring(str.substr(start, pos - start)).c_str();
			start = pos + delims.size();
		}

		num_splits++;

	}
	while (pos != std::wstring::npos);
	return;
}

void util_split_wstring_of(IVarList& result, const std::wstring& str,
						   const std::wstring& delims, unsigned int max_splits)
{
	if (0 == str.compare(L""))
	{
		return;
	}

	unsigned int num_splits = 0;

	size_t start, pos;
	start = 0;
	do
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			start = pos + 1;
		}
		else if (pos == std::wstring::npos || (max_splits > 0 && num_splits == max_splits))
		{
			result << util_trim_wstring(str.substr(start)).c_str();
			break;
		}
		else
		{
			result << util_trim_wstring(str.substr(start, pos - start)).c_str();
			start = pos + 1;
		}

		num_splits++;

	}
	while (pos != std::wstring::npos);
	return;
}


// 字符串转换成宽字符串
std::wstring util_string_as_widestr(const char* s)
{
	size_t size = ::GetToWideStrLen(s);

	TAutoMem<wchar_t, 256> auto_buf(size);

	wchar_t* buffer = auto_buf.GetBuffer();

	::ToWideStr(s, buffer, size * sizeof(wchar_t));

	return std::wstring(buffer);
}

// 整数换成字符串
fast_string util_int_as_string(int value)
{
	char buf[32];

	SafeSprintf(buf, sizeof(buf), "%d", value);

	return fast_string(buf);
}

// 双精度数转换成字符串
std::string util_double_as_string(double value, int decimals)
{
	char str[64];

	if (0 == decimals)
	{
		SPRINTF_S(str, "%f", value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			::SPRINTF_S(str, "%.01f", value);
			break;
		case 2:
			::SPRINTF_S(str, "%.02f", value);
			break;
		case 3:
			::SPRINTF_S(str, "%.03f", value);
			break;
		case 4:
			::SPRINTF_S(str, "%.04f", value);
			break;
		case 5:
			::SPRINTF_S(str, "%.05f", value);
			break;
		default:
			::SPRINTF_S(str, "%f", value);
			break;
		}
	}

	return std::string(str);
}

// 双精度转换成宽字符串
std::wstring util_double_as_widestr(double value, int decimals)
{
	wchar_t buf[64];

	if (0 == decimals)
	{
		::swprintf(buf, L"%f", value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			::swprintf(buf, L"%.01f", value);
			break;
		case 2:
			::swprintf(buf, L"%.02f", value);
			break;
		case 3:
			::swprintf(buf, L"%.03f", value);
			break;
		case 4:
			::swprintf(buf, L"%.04f", value);
			break;
		case 5:
			::swprintf(buf, L"%.05f", value);
			break;
		default:
			::swprintf(buf, L"%f", value);
			break;
		}
	}

	return std::wstring(buf);
}

// 整型数转换成字符串
fast_string util_int64_as_string(__int64 value)
{
	char buf[64];

	::SPRINTF_S(buf, "%I64d", value);

	return fast_string(buf);
}

// 整型数转换成宽字符串
std::wstring util_int64_as_widestr(__int64 value)
{
	wchar_t buf[64];

	::swprintf(buf, L"%I64d", value);

	return std::wstring(buf);
}

// 整型数转换成宽字符串
std::wstring util_int_as_widestr(int value)
{
	wchar_t buf[64];

	::swprintf(buf, L"%d", value);

	return std::wstring(buf);
}
// 宽字符串转换成字符串
fast_string util_widestr_as_string(const wchar_t* ws)
{
	size_t size = GetToStringLen(ws);
	TAutoMem<char, 256> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();

	ToString(ws, buffer, size);

	return fast_string(buffer);
}

// 宽字符转换成UTF-8
fast_string util_widestr_as_utf8(const wchar_t* ws)
{
	if (ws == NULL)
	{
		return "";
	}

	size_t size = ::Port_GetWideStrToUTF8Len(ws);

	if (size < sizeof(char))
	{
		return "";
	}

	TAutoMem<char, 512> buf(size+1);
	//static char szStaticBuf[512];

	::Port_WideStrToUTF8(ws, buf.GetBuffer(), size);

	return buf.GetBuffer();
}

// UTF-8转换为宽字符
std::wstring util_utf8_as_widestr(const char* s)
{
	if (s == NULL)
	{
		return L"";
	}

	size_t size = ::Port_GetUTF8ToWideStrLen(s);

	if (size <= 0)
	{
		return L"";
	}

	TAutoMem<wchar_t, 512> wsBuffer(size);

	Port_UTF8ToWideStr(s, wsBuffer.GetBuffer(), size * sizeof(wchar_t));

	return wsBuffer.GetBuffer();
}

// 字符串转换成int64
__int64 util_string_as_int64(const std::string& val)
{
	__int64 val_int64;
	sscanf(val.c_str(), "%I64d", &val_int64);
	return val_int64;
}

// 宽字符串转换成int64
__int64 util_widestr_as_int64(const std::wstring& val)
{
	__int64 val_int64;
	swscanf(val.c_str(), L"%I64d", &val_int64);
	return val_int64;
}

// 字符串替换
std::string  util_replace_string(const std::string& srcStr, const std::string& oldStr, const std::string& newStr)
{
	std::string destStr = srcStr;
	size_t pos = 0;

	while (pos < destStr.size())
	{
		pos = destStr.find(oldStr, pos);
		if (pos == std::string::npos)
		{
			break;
		}

		destStr.replace(pos, oldStr.size(), newStr);
		pos += newStr.size();
	}

	return destStr;
}

// 宽字符串替换
std::wstring util_replace_wstring(const std::wstring& srcStr, const std::wstring& oldStr, const std::wstring& newStr)
{
	std::wstring destStr = srcStr;
	size_t pos = 0;

	while (pos < destStr.size())
	{
		pos = destStr.find(oldStr, pos);
		if (pos == std::wstring::npos)
		{
			break;
		}

		destStr.replace(pos, oldStr.size(), newStr);
		pos += newStr.size();
	}

	return destStr;
}

// 字符串替换 检查准备替换的字符串的后一位是否为字母或下划线,是的话禁止替换,只在战斗公式中使用
std::string util_replace_string_with_check(const std::string& srcString, const std::string& oldStr, const std::string& newStr)
{
	std::string destStr = srcString;
	size_t pos = 0;
	size_t check_pos = 0;

	while (pos < destStr.size())
	{
		pos = destStr.find(oldStr, pos);
		if (pos == std::wstring::npos) 
		{
			break;
		}
		check_pos = pos + oldStr.size();
		// 找到准备替换的后一位字符
		std::string tmp = destStr.substr(check_pos, 1);
		const char* find = tmp.c_str();
		if (strlen(find) > 0)
		{
			char check = find[0];
			if ((check >= 'a' && check <= 'z') || (check >= 'A' && check <= 'Z') || check == '_')
			{
				break;
			}
		}
		
		destStr.replace(pos, oldStr.size(), newStr);
		pos += newStr.size();
	}

	return destStr;
}

// One-based array of days in year at month start
static const int MONTH_DAYS[13] =
{
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

static const double MIN_DATE = -657434.0;  // about year 100
static const double MAX_DATE = 2958465.0;  // about year 9999
static const double HALF_SECOND = 1.0 / 172800.0;

bool date_from_tm(unsigned int wYear, unsigned int wMonth, unsigned int wDay,
				  unsigned int wHour, unsigned int wMinute,
				  unsigned int wSecond, double& dtDest)
{
	// Validate year and month (ignore day of week and milliseconds)
	if ((wYear > 9999) || (wMonth < 1) || (wMonth > 12))
	{
		return false;
	}

	//  Check for leap year and set the number of days in the month
	bool bLeapYear = ((wYear & 3) == 0) &&
		((wYear % 100) != 0 || (wYear % 400) == 0);

	int nDaysInMonth = MONTH_DAYS[wMonth] - MONTH_DAYS[wMonth - 1] +
		((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

	// Finish validating the date
	if ((wDay < 1) || (wDay > unsigned int(nDaysInMonth)) ||
		(wHour > 23) || (wMinute > 59) || (wSecond > 59))
	{
		return false;
	}

	// Cache the date in days and time in fractional days
	long    nDate;
	double  dblTime;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate = wYear * 365L + wYear / 4 - wYear / 100 + wYear / 400 +
		MONTH_DAYS[wMonth - 1] + wDay;

	//  If leap year and it's before March, subtract 1:
	if ((wMonth <= 2) && bLeapYear)
	{
		--nDate;
	}

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((long)wHour * 3600L) + ((long)wMinute * 60L) +
		((long)wSecond)) / 86400.0;

	dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

	return true;
}

bool decode_date(double date, unsigned int& wYear, unsigned int& wMonth,
				 unsigned int& wDay, unsigned int& wHour,
				 unsigned int& wMinute, unsigned int& wSecond)
{
	// The legal range does not actually span year 0 to 9999.
	if ((date > MAX_DATE) || date < (MIN_DATE))
	{
		return false;
	}

	long nDays;             // Number of days since Dec. 30, 1899
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
	long nMinutesInDay;     // Minutes in day

	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
	//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)

	bool bLeap4 = true;     // TRUE if 4 year block includes leap year

	double dblDate = date; // tempory serial date

	// If a valid date, then this conversion should not overflow
	nDays = (long)dblDate;

	// Round to the second
	dblDate += ((date > 0.0) ? HALF_SECOND : -HALF_SECOND);

	nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

	dblDate = ::fabs(dblDate);
	nSecsInDay = (long)((dblDate - ::floor(dblDate)) * 86400.);

	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
		{
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		}
		else
		{
			bLeap4 = false;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
		{
			n4Day = (n4Day - 1) % 365;
		}
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	wYear = WORD(n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr);

	// Handle leap year: before, on, and after Feb. 29.
	if ((n4Yr == 0) && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29*/
			wMonth = 2;
			wDay = 29;

			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
		{
			--n4Day;
		}
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time*/
	for (wMonth = (n4Day >> 5); n4Day > MONTH_DAYS[wMonth]; ++wMonth)
	{
		wDay = (int)(n4Day - MONTH_DAYS[wMonth]);
	}

DoTime:
	if (nSecsInDay == 0)
	{
		wHour = 0;
		wMinute = 0;
		wSecond = 0;
	}
	else
	{
		wSecond = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		wMinute = (int)nMinutesInDay % 60;
		wHour = (int)nMinutesInDay / 60;
	}

	return true;
}

// 高精度计数微秒
__int64  __fastcall  usTimeCounter()
{
	// return usTimeCounter();

	static  LARGE_INTEGER  t1;

	QueryPerformanceCounter(&t1);

	static  LARGE_INTEGER  t2;

	QueryPerformanceFrequency(&t2);

	return t1.QuadPart / t2.QuadPart * 1000000 + t1.QuadPart % t2.QuadPart * 1000000 / t2.QuadPart;
	//return t1.QuadPart * 1000000 / t2.QuadPart;
}

// 取当前时间
double util_get_now()
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	DATE dt;

	date_from_tm(tm.wYear, tm.wMonth, tm.wDay,
		tm.wHour, tm.wMinute, tm.wSecond, dt);

	return dt;
}

// 取日期
double util_get_date()
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	DATE dt;

	date_from_tm(tm.wYear, tm.wMonth, tm.wDay, 0, 0, 0, dt);

	return dt;
}

// 取一天内的时间（0.0f - 1.0f）
double util_get_time()
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	double dblTime = (((long)tm.wHour * 3600L) + ((long)tm.wMinute * 60L) +
		((long)tm.wSecond)) / 86400.0;

	return dblTime;
}

// 取时间差（天数）
double util_get_time_diff(double date1, double date2)
{
	return (date1 - date2);
}

// 取年月日
void util_get_year_month_day(int& year, int& month, int& day)
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	year = tm.wYear;
	month = tm.wMonth;
	day = tm.wDay;
}

// 取时分秒
void util_get_hour_minute_second(int& hour, int& minute, int& second)
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	hour = tm.wHour;
	minute = tm.wMinute;
	second = tm.wSecond;
}

// 取星期（0为星期天）
int util_get_day_of_week()
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	return tm.wDayOfWeek;
}

// 取星期（0为星期天）和时分秒
int util_get_day_of_week(int& hour, int& minute, int& second)
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	hour = tm.wHour;
	minute = tm.wMinute;
	second = tm.wSecond;

	return tm.wDayOfWeek;
}

int util_get_day_of_week_ex(time_t now, int& hour, int& minute, int& second)
{
	struct tm* tmp = localtime(&now);

	hour = tmp->tm_hour;
	minute = tmp->tm_min;
	second = tmp->tm_sec;

	return tmp->tm_wday;
}


// 取距離周幾某時某分某秒的時間(0為星期天)( 當前時間為0 取當前時間)
time_t util_get_first_day_of_week(time_t now, int wday, int hour, int minute, int sec)
{
	int _hour = 0;
	int _minute = 0;
	int _seconds = 0;
	int _wday = 0;
	if (now == 0)
	{
		_wday = util_get_day_of_week(_hour, _minute, _seconds);
		now = util_get_utc_time();
	}
	else
	{
		_wday = util_get_day_of_week_ex(now, _hour, _minute, _seconds);
	}

	// 默认七天
	int nLeft = 604800;
	const int nScondsNow = _hour * 3600 + _minute * 60 + _seconds;
	const int nScondsNext = hour * 3600 + minute * 60 + sec;
	const int SECONDS_OF_DAY = 3600 * 24;
	// 计算天数差
	if (_wday == wday)
	{
		if (nScondsNext > nScondsNow)
		{
			nLeft = nScondsNext - nScondsNow;
		}
		else
		{
			nLeft = 7 * SECONDS_OF_DAY + nScondsNext - nScondsNow;
		}
	}
	else
	{
		int nDay = 1;
		if (wday > _wday)
		{
			nDay = wday - _wday;
		}
		else
		{
			nDay = wday - _wday + 7;
		}
		nLeft = nDay * SECONDS_OF_DAY + nScondsNext - nScondsNow;
	}
	return now + nLeft;
}


int util_get_day_elapsed_seconds()
{
	// 取星期（0为星期天）和时分秒
	int hour_ = 0, minute_ = 0, second_ = 0;
	int week_ = util_get_day_of_week(hour_, minute_, second_);
	
	return util_calc_seconds(hour_ , minute_, second_);
}

int util_calc_seconds(int hour, int minutes, int seconds)
{
	return hour * 3600 + minutes * 60 + seconds;
}

// 取日期(一个月的第几天)
int util_get_day_of_month()
{
	SYSTEMTIME tm;

	::GetLocalTime(&tm);

	return tm.wDay;
}

// 判断是否为闰年
bool util_is_leap_year(const int year) 
{
	if (year % 4 != 0) 
		return false;
	if (year % 400 == 0) 
		return true;
	if (year % 100 == 0) 
		return false;
	return true;
}

// 获取当前月的天数
int util_get_days_of_month(const int year, const int month) 
{
	int days = days_in_months[month];
	// 润年的2月29天
	if (month == 1 && util_is_leap_year(year)) 
		days += 1;
	return days;
}

// 获取当年的天数
int util_get_days_of_year(const int year)
{
	if (util_is_leap_year(year))
	{
		return 366;
	}
	else
	{
		return 365;
	}
}

// 转换整数日期到浮点日期
bool util_encode_date(int year, int month, int day, double& date)
{
	DATE dt;

	if (! date_from_tm(year, month, day, 0, 0, 0, dt))
	{
		return false;
	}

	date = dt;

	return true;
}

// 转换整数时间浮点时间
bool util_encode_time(int hour, int minute, int second, double& time)
{
	if ((hour < 0) || (hour > 23)
		|| (minute < 0) || (minute > 59)
		|| (second < 0) || (second > 59))
	{
		return false;
	}

	time = (((long)hour * 3600L) + ((long)minute * 60L) +
		((long)second)) / 86400.0;

	return true;
}

// 转换浮点日期到整数日期
bool util_decode_date(double date, int& year, int& month, int& day)
{
	unsigned int wYear, wMonth, wDay, wHour, wMinute, wSecond;

	if (!decode_date(date, wYear, wMonth, wDay, wHour, wMinute, wSecond))
	{
		return false;
	}

	year = wYear;
	month = wMonth;
	day = wDay;

	return true;
}

// 转换浮点时间到整数时间
bool util_decode_time(double date, int& hour, int& minute, int& second)
{
	unsigned int wYear, wMonth, wDay, wHour, wMinute, wSecond;
	
	if (!decode_date(date, wYear, wMonth, wDay, wHour, wMinute, wSecond))
	{
		return false;
	}
	
	hour = wHour;
	minute = wMinute;
	second = wSecond;
	
	return true;
}

// 获得系统Ticks
unsigned long util_get_ticks()
{
	return ::GetTickCount();
}

// 获得64位的当前时间,精确到ms，注意获取到的是UTC时间
__time64_t util_get_time_64()
{
	SYSTEMTIME stm;
	FILETIME ftm;
	::GetSystemTime(&stm);
	::SystemTimeToFileTime(&stm, &ftm);

	__time64_t now = ftm.dwHighDateTime;
	now = ((now << 32) | ftm.dwLowDateTime) / 10000;

	return now;
}

// 获得64位的指定时间,精确到ms，注意获取到的是UTC时间，传入参数是当地时间
__time64_t util_get_time_64(int year, int month, int day, int hour, int minute, int second, int Millisecond /* = 0 */)
{
	SYSTEMTIME stm;
	stm.wYear			= year;
	stm.wMonth			= month;
	stm.wDay			= day;
	stm.wDayOfWeek		= 0;	// 一周中的第几天
	stm.wHour			= hour;
	stm.wMinute			= minute;
	stm.wSecond			= second;
	stm.wMilliseconds	= Millisecond;

	FILETIME ftm;
	::SystemTimeToFileTime(&stm, &ftm);

	FILETIME utcftm;
	::LocalFileTimeToFileTime(&ftm, &utcftm);

	__time64_t now = utcftm.dwHighDateTime;
	now = ((now << 32) | utcftm.dwLowDateTime) / 10000;

	return now;
}

// 判断是否同一天
bool util_is_sameday(time_t time1, time_t time2)
{
	return (util_subtract_days(time1, time2) == 0);
}

// 判断是否昨天
bool util_is_yestoday(time_t today, time_t yestoday)
{
	return (util_subtract_days(today, yestoday) == 1);
}

// 判断是否明天
bool util_is_tomorrow(time_t today, time_t tomorrow)
{
	return (util_subtract_days(today, tomorrow) == -1);
}

bool util_is_sameweek(time_t time1, time_t time2)
{
	if (time1 > time2)
	{
		time_t temp = time1;
		time1 = time2;
		time2 = temp;
	}

	time_t diff = time2 - time1;
	const time_t wdaysSec = 3600 * 24 * 7;
	if (diff >= wdaysSec)
	{
		// 超过7天肯定不在同一周
		return false;
	}

	struct tm* tm1 = localtime(&time1);
	struct tm* tm2 = localtime(&time2);
	return tm1->tm_wday <= tm2->tm_wday;
}

bool util_is_samemonth(time_t time1, time_t time2)
{
	struct tm* tm1 = localtime(&time1);
	struct tm* tm2 = localtime(&time2);
	return tm1->tm_year == tm2->tm_year && tm1->tm_mon == tm2->tm_mon;
}

bool util_is_sameyear(time_t time1, time_t time2)
{
	struct tm* tm1 = localtime(&time1);
	struct tm* tm2 = localtime(&time2);
	return tm1->tm_year == tm2->tm_year;
}

// 计算相隔天数，time1-time2
int util_subtract_days(time_t time1, time_t time2)
{
	const int INVALID_DAY_CNT = 2147483648;
	const int LOCAL_TIME_BEGIN_YEAR = 1900; // 本地时间起始年份

	if (time1 == time2)
	{
		return 0;
	}
	if (time1 == 0 || time2 == 0)
	{
		return INVALID_DAY_CNT;
	}
	tm t1;
	if (::localtime_s(&t1, &time1) != 0)
	{
		return INVALID_DAY_CNT;
	}

	tm t2;
	if (::localtime_s(&t2, &time2) != 0)
	{
		return INVALID_DAY_CNT;
	}

	if (t1.tm_year == t2.tm_year)
	{
		return (t1.tm_yday - t2.tm_yday);
	}
	else if (t1.tm_year < t2.tm_year)
	{
		int nDays = t1.tm_yday - t2.tm_yday;

		for (int i = t1.tm_year; i < t2.tm_year; ++i)
		{
			nDays -= util_get_days_of_year(i + LOCAL_TIME_BEGIN_YEAR);
		}

		return nDays;
	}
	else
	{
		int nDays = t1.tm_yday - t2.tm_yday;

		for (int i = t2.tm_year; i < t1.tm_year; ++i)
		{
			nDays += util_get_days_of_year(i + LOCAL_TIME_BEGIN_YEAR);
		}

		return nDays;
	}
}

bool util_decode_time64(time_t nTime, int& nYear, int& nMonth, int& nDay, int& nHour, int& nMin, int& nSec)
{
	if (nTime <= 0)
		nTime = util_get_time_64();

	struct tm* tm = localtime(&nTime);

	nYear = tm->tm_year + 1900;
	nMonth = tm->tm_mon + 1;
	nDay = tm->tm_mday;
	nHour = tm->tm_hour;
	nMin = tm->tm_min;
	nSec = tm->tm_sec;
	return true;
}

// 获得64位的指定时间,精确到s，注意获取到的是UTC时间，传入参数是当地时间
__time64_t util_encode_time64(int year, int month, int day, int hour, int minute, int second)
{
	struct tm tm = {0};
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;

	return mktime(&tm);
}

void util_get_utc_time(time_t* ttime, struct tm* tmtime)
{
	memcpy(tmtime, gmtime(ttime), sizeof(struct tm));
}

time_t util_get_utc_time()
{
	return time(NULL);
}
time_t util_get_utc_time_x()
{
	return util_convert_time_utc_0_x(util_get_utc_time());
}
time_t util_convert_time_utc_0_x(time_t utc_time)
{
	if (utc_time<=0)
	{
		return 0;
	}
	return utc_time - ::timezone;
}
time_t util_convert_time_utc_x_0(time_t utc_time)
{
	if (utc_time <= 0)
	{
		return 0;
	}
	return utc_time + ::timezone;
}

int64_t util_get_utc_ms_time()
{
	struct timeb tb;
	ftime(&tb);

	int64_t nMilliTime = tb.time * 1000 + tb.millitm;
	return nMilliTime;
}

time_t util_make_utc_time(struct tm* tmtime)
{
	return mktime(tmtime);
}

void util_get_local_time(time_t* ttime, struct tm* tmtime)
{
	tm* time = localtime(ttime);
	if (NULL == time)
	{
		ZeroMemory(tmtime, sizeof(struct tm));
		return;
	}
    memcpy(tmtime, localtime(ttime), sizeof(struct tm));
}

void util_convert_time_to_string(time_t* ttime, char* strtime, size_t len)
{
	struct tm OutTm;
	memset(&OutTm, 0, sizeof(OutTm));

	util_get_local_time(ttime, &OutTm);

	SafeSprintf(strtime, len, "%d-%02d-%02d %02d:%02d:%02d", OutTm.tm_year + 1900, OutTm.tm_mon + 1, OutTm.tm_mday,
		OutTm.tm_hour, OutTm.tm_min, OutTm.tm_sec);
}
void util_convert_time_to_string_xx(time_t* ttime, char* strtime, size_t len)
{
	struct tm OutTm;
	memset(&OutTm, 0, sizeof(OutTm));

	util_get_utc_time(ttime, &OutTm);

	SafeSprintf(strtime, len, "%d-%02d-%02d %02d:%02d:%02d", OutTm.tm_year + 1900, OutTm.tm_mon + 1, OutTm.tm_mday,
		OutTm.tm_hour, OutTm.tm_min, OutTm.tm_sec);
}

//将一个字符串转换成日期，必须是标准的yy-mm-dd hour:min格式
void util_convert_string_to_time(const char* strtime, time_t* ttime, bool is_only_date)
{
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

	if (strtime != NULL && strlen(strtime) > 6)
	{
		if (is_only_date)
		{
			sscanf(strtime, "%d-%02d-%02d", &year, &month, &day);
		}
		else
		{
			sscanf(strtime, "%d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &min, &sec);
		}
	}
	tm tmtemp;
	tmtemp.tm_year = year - 1900;
	tmtemp.tm_mon = month - 1;
	tmtemp.tm_mday = day;
	tmtemp.tm_hour = hour;
	tmtemp.tm_min = min;
	tmtemp.tm_sec = sec;
	tmtemp.tm_isdst = 0;
	tmtemp.tm_wday = 0;
	tmtemp.tm_yday = 0;
	*ttime = mktime(&tmtemp);
}
void util_convert_string_to_time_xx(const char* strtime, time_t* ttime, bool is_only_date)
{
	util_convert_string_to_time(strtime, ttime, is_only_date);

	*ttime = util_convert_time_utc_0_x(*ttime);
}

// 获取字符串表示的当前日期，格式为 YYYY-MM-DD
void util_convert_date_to_string(char* strtime, int str_size)
{
    int year, month, month_day;

    util_get_year_month_day(year, month, month_day);
    
    SafeSprintf(strtime, str_size-1, "%d-%02d-%02d", year, month, month_day);
}

// 获取字符串表示的当前日期，格式为 YYYY-MM-DD
void util_convert_date_to_string(time_t* ttime, char* strtime, int str_size)
{
	struct tm OutTm;
	memset(&OutTm, 0, sizeof(OutTm));

	util_get_local_time(ttime, &OutTm);

	SafeSprintf(strtime, str_size - 1, "%d-%02d-%02d", 
		OutTm.tm_year + 1900, OutTm.tm_mon + 1, OutTm.tm_mday );
}


// 获取字符串表示的当前日期，格式为 YYYY-MM-DD hh:mm:ss
void util_convert_date_to_string2(std::string &out_str)
{
    time_t slice = time(NULL);
    struct tm tmtime;
    memset(&tmtime, 0, sizeof(tmtime));
    memcpy(&tmtime, localtime(&slice), sizeof(struct tm));

    char strtime[128] = {0};
    SafeSprintf(strtime, sizeof(strtime), "%d-%02d-%02d %02d:%02d:%02d", tmtime.tm_year + 1900, tmtime.tm_mon + 1, tmtime.tm_mday,
        tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec);

    out_str = strtime;
}

// 获取字符串表示的当前日期，格式为 YYYY-MM-DD hh:00:00
void util_convert_date_to_string2_sp(time_t* ttime, char* strtime, int str_size)
{
	struct tm OutTm;
	memset(&OutTm, 0, sizeof(OutTm));

	util_get_local_time(ttime, &OutTm);

	SafeSprintf(strtime, str_size-1, "%d-%02d-%02d %02d:%02d:%02d", 
		OutTm.tm_year + 1900, OutTm.tm_mon + 1, OutTm.tm_mday,
		OutTm.tm_hour, 0, 0);
}


// 获得日志格式时间字符串
void util_get_log_time_dump(char* strtime, time_t slice /* = 0 */)
{
    time_t _slice = slice;
    if (_slice <= 0)
    {
        _slice = time(NULL);
    }
    struct tm tmtime;
    memset(&tmtime, 0, sizeof(tmtime));
    memcpy(&tmtime, localtime(&_slice), sizeof(struct tm));

    SPRINTF_S(strtime, "'%04d-%02d-%02d %02d:%02d:%02d'", tmtime.tm_year + 1900, tmtime.tm_mon + 1, tmtime.tm_mday,
        tmtime.tm_hour, tmtime.tm_min, tmtime.tm_sec);
}

//字串转换秒数
int util_convert_string_to_sec(const char * strtime)
{
	int hour = 0, min = 0, sec = 0;
	if (strtime != NULL && strlen(strtime) > 3)
	{
		sscanf(strtime, "%d:%d:%d", &hour, &min, &sec);
	}

	int convtime = hour * 60 * 60 + min * 60 + sec;
	return convtime;
}

//当前到时间点转为秒数
int util_convert_zerotime_to_sec()
{
	int hour = 0, min = 0, sec = 0;
	util_get_hour_minute_second(hour, min, sec);
	int convtime = hour * 60 * 60 + min * 60 + sec;
	return convtime;
}

// 去除字符串前后空格
std::string util_trim_string(std::string str, const char delims )
{
	if (str.empty())
	{
		return str;
	}

	size_t pos = str.find_first_not_of(delims);
	if (pos == str.npos)
	{
		return "";
	}

	str = str.substr(pos, str.size() - pos);
	pos = str.find_last_not_of(delims);
	return str.substr(0, pos + 1);
}

// 去除宽字符串前后空格
std::wstring util_trim_wstring(std::wstring str, const wchar_t delims)
{
	size_t pos = str.find_first_not_of(delims);
	if (pos == str.npos)
	{
		return L"";
	}

	str = str.substr(pos, str.size() - pos);
	pos = str.find_last_not_of(delims);
	return str.substr(0, pos + 1);
}

// 通过场景ConfigID得到寻路中使用的场景Resource
void util_get_scene_resource_by_configid(std::string& strRet, const std::string& config_id)
{
	CVarList strSplit;
	std::string strTmp;

	// 按照 "\" 分割
	util_split_string(strSplit, config_id, "\\");

	if (strSplit.GetCount() == 0)
	{
		strRet = "";
		return;
	}

	// 目标字符串在最后分割的字符串中
	strTmp = strSplit.StringVal(strSplit.GetCount() - 1);

	// 按照 "_" 分割
	strSplit.Clear();
	util_split_string(strSplit, strTmp, "_");

	if (strSplit.GetCount() == 0)
	{
		strRet = "";
		return;
	}

	// 目标字符串为第一个
	strRet = strSplit.StringVal(0);

	return;
}

// 生成64位唯一整型id
uint64_t util_gen_unique_id()
{
	GUID id;

	if (CoCreateGuid(&id) != S_OK)
	{
		return 0;
	}

	uint64_t res = (uint64_t)id.Data1 << 40 | (uint64_t)id.Data2 << 24 | (uint64_t)id.Data3 << 8 | (uint64_t)id.Data4[0] << 7
		| (uint64_t)id.Data4[1] << 6 | (uint64_t)id.Data4[2] << 5 | (uint64_t)id.Data4[3] << 4 | (uint64_t)id.Data4[4] << 3
		| (uint64_t)id.Data4[5] << 2 | (uint64_t)id.Data4[6] << 1 | (uint64_t)id.Data4[7];


	return res;
}

// 求两点的距离
float util_dot_distance(float x_1, float y_1, float x2, float y2)
{
	float sx = x_1 - x2;
	float sy = y_1 - y2;

	return ::sqrt(sx * sx + sy * sy);
}

// 求两点的距离的平方
float util_dot_distance2(float x_1, float y_1, float x2, float y2)
{
	float sx = x_1 - x2;
	float sy = y_1 - y2;

	return (sx * sx + sy * sy);
}

// 求三维空间的两点的距离
float util_dot_distance(float x_1, float y_1, float z1, float x2, float y2, float z2)
{
	float sx = x_1 - x2;
	float sy = y_1 - y2;
	float sz = z1 - z2;

	return ::sqrt(sx * sx + sy * sy + sz * sz);
}

// 求三维空间的两点的距离的平方
float util_dot_distance2(float x_1, float y_1, float z1, float x2, float y2, float z2)
{
	float sx = x_1 - x2;
	float sy = y_1 - y2;
	float sz = z1 - z2;

	return (sx * sx + sy * sy + sz * sz);
}
// 求两点的角度
float util_dot_angle(float x1, float y1, float x2, float y2)
{
	float sx = x2 - x1;
	float sy = y2 - y1;

	float dist = ::sqrt(sx * sx + sy * sy);

	float a = 0.0f;

	if(!util_float_equal(dist, 0.0f))
	{
		a = ::acos(sy / dist);
	}

	if (sx < 0)
	{
		a = -a;
	}

	return ::util_normalize_angle(a);
}

// 将角度值转换为0-pi*2之间的值
float util_normalize_angle(float angle)
{
	float value = ::fmod(angle, PI2);

	if (value < 0)
	{
		value = value + PI2;
	}

	return value;
}
float util_get_angle_diff(const float & angle1, const float & angle2)
{
	const float norm_angle1 = util_normalize_angle(angle1);
	const float norm_angle2 = util_normalize_angle(angle2);

	float angle_diff = fabs(norm_angle1 - norm_angle2);
	if (angle_diff > PI)
	{
		angle_diff = PI2 - angle_diff;
	}

	return angle_diff;
}
// 判断两个浮点数是否可认为相等
bool util_float_equal(float f1, float f2)
{
	if (memcmp(&f1, &f2, sizeof(f1)) == 0)
	{
		return true;
	}

	return (f1 < (f2 + FLOAT_EPSILON)) && (f1 > (f2 - FLOAT_EPSILON));
}

bool util_float_equal_zero(float f)
{
	return (f >= -FLT_EPSILON && f <= FLT_EPSILON);
}
bool util_double_equal_zero(double f)
{
	return (f >= -FLT_EPSILON && f <= FLT_EPSILON);
}

// 取时分秒毫秒
void util_get_hour_minute_second_milliseconds(int& hour, int& minute, int& second, int& nMilliseconds)
{
	port_date_time_t tm;

	Port_GetLocalTime(&tm);

	hour = tm.nHour;
	minute = tm.nMinute;
	second = tm.nSecond;
	nMilliseconds = tm.nMilliseconds;
}

// 生成唯一的名字id
std::string util_gen_unique_name()
{
	GUID id;

	if (CoCreateGuid(&id) != S_OK)
	{
		return std::string();
	}

	char buf[64];

	SPRINTF_S(buf, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		id.Data1, id.Data2, id.Data3,
		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	return std::string(buf);
}

// 4个int压缩成一个int (-128  -- 127 之前的数值)
int util_compress_int4(int iUpHigh, int iUpLow, int iDownHigh, int iDownLow)
{
	//缩小4倍放至高16位
	int HighValue = ((iUpHigh << 24) & 0xFF000000) | ((iUpLow << 16) & 0x00FF0000);
	HighValue &= 0xFFFF0000;

	//低位设置(坐标缩小4倍)保存传送
	int LowValue = ((iDownHigh << 8) & 0x0000FF00) | (iDownLow & 0x000000FF);
	LowValue &= 0x0000FFFF;
	HighValue |= LowValue;

	return HighValue;	
}

// 2个int压缩成一个int （-32768 -- 32767 之间的数值）
int util_compress_int2(int iHighValue, int iLowValue)
{
	int Value = ((iHighValue << 16) & 0xFFFF0000) | (iLowValue & 0x0000FFFF);
	return Value;
}
//获得下一个整点时间秒数
int64_t util_get_next_hour_sec()
{
	int64_t timeNow = ::time(NULL);

	struct tm tm_time;
	util_get_local_time(&timeNow, &tm_time);
	int min = tm_time.tm_min;
	int sec = tm_time.tm_sec;
	int diffValue = (59 - min) * 60 + (60 - sec);
	return timeNow + diffValue;

}
bool util_time_test_utc_0_X()
{
	//fore test
	char cur_time_s[32];   //utc0
	char cur_time_s_x[32]; //utcX<=> local time
	// 返回 utc0时间（LU)
	time_t cur_time = util_get_utc_time();
	// 返回 utcX时间 （扩展）
	time_t cur_time_x = util_get_utc_time_x();

	//当前小时。
	time_t cur_hour = (cur_time % 86400) / 3600;
	time_t cur_hour_x = (cur_time_x % 86400) / 3600;

	time_t cur_time_1 = 0;
	time_t cur_time_1_x = 0;

	// 系统，LU定义的utc0(int) <=>utcx(string)
	util_convert_time_to_string(&cur_time, cur_time_s, 32);
	util_convert_string_to_time(cur_time_s, &cur_time_1);

	Assert(cur_time == cur_time_1);

	// 扩展，utcX(int)<=>utcX(string)
	util_convert_time_to_string_xx(&cur_time_x, cur_time_s_x, 32);
	util_convert_string_to_time_xx(cur_time_s_x, &cur_time_1_x);
	
	Assert(cur_time_x == cur_time_1_x);

	return true;
}