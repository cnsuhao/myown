//--------------------------------------------------------------------
// 文件名:		util_func.h
// 内  容:		工具函数
// 说  明:		
// 创建日期:	2008年10月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#ifndef _GAME_UTIL_FUNC_H
#define _GAME_UTIL_FUNC_H

#include "../../public/IVarList.h"
#include "extend_func.h"

inline unsigned int SetbitofUint(unsigned int src, unsigned int bitnum) 
{
	unsigned int m = 0x01;
	return  src | (m << bitnum);
}
inline unsigned int ResetbitofUint(unsigned int src, unsigned int bitnum)
{
	unsigned int m = 0x01;
	return src & (~(m << bitnum));
}
inline unsigned int GetbitofUint(unsigned int src, unsigned int bitnum)
{
	unsigned int m = 0x01;
	return  (src >> bitnum ) & m;
}

bool date_from_tm(unsigned int year, unsigned int month, unsigned int day,	
				  unsigned int hour, unsigned int minute, unsigned int second, double& date);

bool decode_date(double date, unsigned int& wYear, unsigned int& wMonth,
				 unsigned int& wDay, unsigned int& wHour,
				 unsigned int& wMinute, unsigned int& wSecond);


// 返回0-(scale-1)的随机整数
int util_random_int(int scale);

// 返回0-scale)的随机浮点数
float util_random_float(float scale);

// 调整两个数的大小后返回
void util_sort_int(int& minvalue, int& maxvalue);
void util_sort_float(float& minvalue, float& maxvalue);

// 去除字符串前后空格
std::string util_trim_string(std::string str, const char delims = ' ');

// 去除宽字符串前后空格
std::wstring util_trim_wstring(std::wstring str, const wchar_t delims = L' ');

// 分割字符串,带圆括号的
bool util_split_string_with_parentheses(IVarList& result, const std::string& str, char delims);

// 以分割串delims整串匹配来分割字符串
void util_split_string(IVarList& result,
					   const std::string& str,
					   const std::string& delims,
					   unsigned int max_splits = 0);


// 分割字符串表达式(exp=value,exp1=value1)
bool util_split_express_string(IVarList& result,
					   const std::string& str,
					   const char* exp_equa = "=",
					   const char* exp_split = ",");


// 以分割串delims中的任意字符来分割字符串
void util_split_string_of(IVarList& result,
						  const std::string& str,
						  const std::string& delims = "/\\",
						  unsigned int max_splits = 0);

// 以分割串delims整串匹配来分割字符串
void util_split_wstring(IVarList& result, 
						const std::wstring& str,
						const std::wstring& delims, 
						unsigned int max_splits = 0);

// 以分割串delims中的任意字符来分割字符串
void util_split_wstring_of(IVarList& result, 
						   const std::wstring& str,
						   const std::wstring& delims = L"/\\", 
						   unsigned int max_splits = 0);

// 字符串转换成宽字符串
std::wstring util_string_as_widestr(const char* s);

// 整数换成字符串
fast_string util_int_as_string(int value);

// 双精度数转换成字符串
std::string util_double_as_string(double value, int decimals = 0);

// 双精度转换成宽字符串
std::wstring util_double_as_widestr(double value, int decimals = 0);

// 整型数转换成字符串
fast_string util_int64_as_string(__int64 value);

// 整型数转换成宽字符串
std::wstring util_int64_as_widestr(__int64 value);

// 宽字符串转换成字符串
fast_string util_widestr_as_string(const wchar_t * ws);

// 宽字符转换成UTF-8
fast_string util_widestr_as_utf8(const wchar_t* ws);

// UTF-8转换为宽字符
std::wstring util_utf8_as_widestr(const char* s);

std::wstring util_int_as_widestr(int value);

// 字符串转换成int64
__int64 util_string_as_int64(const std::string& val);
// 宽字符串转换成int64
__int64 util_widestr_as_int64(const std::wstring& val);

// 字符串替换
std::string util_replace_string(const std::string& srcString, const std::string& oldStr, const std::string& newStr);
// 宽字符串替换
std::wstring util_replace_wstring(const std::wstring& srcString, const std::wstring& oldStr, const std::wstring& newStr);

// 字符串替换 检查准备替换的字符串的后一位是否为字母或下划线,是的话禁止替换,只在战斗公式中使用,防止公式错乱
std::string util_replace_string_with_check(const std::string& srcString, const std::string& oldStr, const std::string& newStr);

// 取当前时间
double util_get_now();

// 取日期
double util_get_date();

// 取一天内的时间（0.0f - 1.0f）
double util_get_time();

// 取时间差（天数）
double util_get_time_diff(double date1, double date2);

// 取年月日
void util_get_year_month_day(int& year, int& month, int& day);

// 取时分秒
void util_get_hour_minute_second(int& hour, int& minute, int& second);

// 取星期（0为星期天） utcx
int util_get_day_of_week();

// 取星期（0为星期天）和时分秒
int util_get_day_of_week(int& hour, int& minute, int& second);
int util_get_day_of_week_ex(time_t now, int& hour, int& minute, int& second);

// 取距離周幾某時某分某秒的時間(0為星期天)( 當前時間為0 取當前時間)
time_t util_get_first_day_of_week(time_t now, int wday, int hour, int minute, int sec);
// 取当天过了多少秒了
int util_get_day_elapsed_seconds();
// 计算秒数
int util_calc_seconds( int hour, int minutes , int seconds);


// 取日期(一个月的第几天)
int util_get_day_of_month();

// 判断是否为闰年
bool util_is_leap_year(const int year);

// 获取当年的天数
int util_get_days_of_year(const int year);

// 转换整数日期到浮点日期
bool util_encode_date(int year, int month, int day, double& date);

// 转换整数时间浮点时间
bool util_encode_time(int hour, int minute, int second, double& time);

// 转换浮点日期到整数日期
bool util_decode_date(double date, int& year, int& month, int& day);

// 转换浮点时间到整数时间
bool util_decode_time(double date, int& hour, int& minute, int& second);

// 获得当前时间,精确到s  (utc_0)
time_t util_get_utc_time();

// 获得当前时间,精确到s  (utc_x)
time_t util_get_utc_time_x();

// 时间转换，utc_0 = > utc_x
time_t util_convert_time_utc_0_x(time_t utc_time);

// 时间转换，utc_x = > utc_0
time_t util_convert_time_utc_x_0(time_t utc_time);

// 获得当前时间,精确到ms
int64_t util_get_utc_ms_time();

// tm时区按照标准时区计算，该结构空间由调用者分配，转换为utc秒数
time_t util_make_utc_time(struct tm*  tmtime);

// 将time_t转换成struct tm结构，时区按照标准时区计算，该结构空间由调用者分配
void util_get_utc_time(time_t* ttime, struct tm* tmtime);

// 将time_t转换成struct tm结构，并且加上本机的时区，该结构空间由调用者分配
void util_get_local_time(time_t* ttime, struct tm* tmtime);

// 高精度计数微秒
__int64  __fastcall  usTimeCounter();

// 获得系统Ticks
unsigned long util_get_ticks();

// 获得64位的当前时间,精确到ms
__time64_t util_get_time_64();

// 获得64位的指定时间,精确到ms
__time64_t util_get_time_64(int year, int month, int day, int hour, int minute, int second, int Millisecond = 0);

//解析时间
bool util_decode_time64(__int64 nTime, int& nYear, int& nMonth, int& nDay,
						int& nHour, int& nMin, int& nSec);
//年/月/日转成utc时间
__time64_t util_encode_time64(int year, int month, int day, int hour, int minute, int second);


// 将time_t转换成struct tm结构，并且加上本机的时区，该结构空间由调用者分配
void util_get_local_time(time_t* ttime, struct tm* tmtime);

// 将time_t转换成yy-mm-dd hour:min:sec格式 utc0=>utcx
void util_convert_time_to_string(time_t* ttime, char* strtime, size_t len);

// 将time_t转换成yy-mm-dd hour:min:sec格式 utcx=>utcx
void util_convert_time_to_string_xx(time_t* ttime, char* strtime, size_t len);

//将一个字符串转换成日期，必须是标准的yy-mm-dd hour:min:sec格式  utcx=>utc0
void util_convert_string_to_time(const char* strtime, time_t* ttime,bool is_only_date=false);

//将一个字符串转换成日期，必须是标准的yy-mm-dd hour:min:sec格式  utcx=>utcx
void util_convert_string_to_time_xx(const char* strtime, time_t* ttime, bool is_only_date = false);

// 获取字符串表示的当前日期，格式为 YYYY-MM-DD
void util_convert_date_to_string(char* strtime, int str_size);
// 获取字符串表示的当前日期，格式为 YYYY-MM-DD
void util_convert_date_to_string(time_t* ttime, char* strtime, int str_size);

// 获取字符串表示的当前日期，格式为 YYYY-MM-DD hh:mm:ss
void util_convert_date_to_string2(std::string &out_str);
// 获取字符串表示的当前日期，格式为 YYYY-MM-DD hh:00:00
void util_convert_date_to_string2_sp(time_t* ttime, char* strtime, int str_size);

// 获得日志格式时间字符串
void util_get_log_time_dump(char* strtime, time_t slice = 0);

// 获得字串表示的秒数 格式为 hour:min:sec
int util_convert_string_to_sec(const char * strtime);

// 判断是否同一天
bool util_is_sameday(time_t time1, time_t time2);

// 判断是否昨天
bool util_is_yestoday(time_t today, time_t yestoday);

// 判断是否明天
bool util_is_tomorrow(time_t today, time_t tomorrow);

// 判断是否同一周
bool util_is_sameweek(time_t time1, time_t time2);
// 判断是否同一月
bool util_is_samemonth(time_t time1, time_t time2);
// 判断是否同一年
bool util_is_sameyear(time_t time1, time_t time2);

// 计算相隔天数，time1-time2
int util_subtract_days(time_t time1, time_t time2);

// 获得当天的时间总共秒数
int util_convert_zerotime_to_sec();

// 生成64位唯一整型id
uint64_t util_gen_unique_id();

// 求两点的距离
float util_dot_distance(float x_1, float y_1, float x2, float y2);

// 求两点的距离的平方
float util_dot_distance2(float x_1, float y_1, float x2, float y2);

// 求三维空间的两点的距离
float util_dot_distance(float x_1, float y_1, float z1, float x2, float y2, float z2);

// 求三维空间的两点的距离的平方
float util_dot_distance2(float x_1, float y_1, float z1, float x2, float y2, float z2);

// 求两点所构成矢量的弧度值(返回0~2*pi的值，以y的正方向为0度)
float util_dot_angle(float x_1, float y_1, float x2, float y2);

// 将-pi~pi的弧度值转换为0~2*pi的弧度值
float util_normalize_angle(float angle);

// 求angle1和angle2的角度差，返回0~pi的弧度值
float util_get_angle_diff(const float & angle1, const float & angle2);

// 比较两个浮点数是否可认为相等
bool util_float_equal(float f1, float f2);

bool util_float_equal_zero(float f);

bool util_double_equal_zero(double f);

// 通过场景ConfigID得到寻路中使用的场景Resource
void util_get_scene_resource_by_configid(std::string& strRet, const std::string& config_id);

// 判断一个字符串的合法性
#define IsInvalidString(szChar)     (NULL == (szChar) || '\0' == *(szChar))

// 快速计算根号分之1
inline float Invfast_sqrt(float x)
{
	float xhalf = 0.5f * x;

	int i = *(int *)&x;             // 浮点数按BIT强行赋给长整形

	i = 0x5f375a86 - (i >> 1);      // Quake-III Arena (雷神之锤3)的底层代码

	x = *(float *)&i;               // convert bits back to float

	x = x * (1.5f - xhalf * x * x); // 牛顿叠代法Newton step, repeating increases accuracy

	return x;
}

// fast_sqrt
inline float fast_sqrt(const float x)
{
	return x * Invfast_sqrt(x);
}

// fast fabs
inline float fast_fabs(const float num)
{
	unsigned int ret = *((unsigned int*)&num);

	ret = ret << 1;
	ret = ret >> 1;

	return *((float*)&ret);
}

#if 0
inline int fast_abs(const int num)
{
	int ret = num;
	int tmp = num;

	tmp = tmp >> 31;
	ret = ret ^ tmp;
	ret = ret - tmp;

	return ret;
}
#endif


// 取时分秒毫秒
void util_get_hour_minute_second_milliseconds(int& hour, int& minute, int& second, int& nMilliseconds);

// 生成唯一的名字id
std::string util_gen_unique_name();

// 生成64位唯一整型id
uint64_t util_gen_unique_id();

// 4个int压缩成一个int (-128  -- 127 之前的数值)
int util_compress_int4(int iUpHigh = 0, int iUpLow = 0, int iDownHigh = 0, int iDownLow = 0);

// 2个int压缩成一个int （-32758 -- 32767 之间的数值）
int util_compress_int2(int iHighValue = 0, int iLowValue = 0);
//获得下一个整点时间秒数
int64_t util_get_next_hour_sec();

// 时间测试代码，验证：整型时间，字符串时间，时区的影响。
bool util_time_test_utc_0_X();

#endif // _GAME_UTIL_FUNC_H
 
