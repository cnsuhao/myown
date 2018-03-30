//--------------------------------------------------------------------
// 文件名:		util.h
// 内  容:		常用工具
// 说  明:		
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef _GAME_UTIL_H
#define _GAME_UTIL_H

#include "server/IKernel.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <time.h>
#define UTIL_RAND_MAX RAND_MAX
#define UTIL_FLOAT_EPSILON 0.000001

#define UTIL_ONE_DAY_S 86400
#define UTIL_ONE_HOUR_S 3600

#define ZERO         (1.0e-7f)
#define PI           (3.14159265f)
#define PI2          (6.2831853f)
#define PI_2         (1.570796325f)
#define PI_3         (1.0471975512f)
#define PI_4         (0.7853981625f)
#define PI_6         (0.5235987756f)

#define ANGLE0      (0.0f)
#define ANGLE360    (360.0f)
#define ANGLE_90    (90.0f)
#define ANGLE_60    (60.0f)
#define ANGLE_45    (45.0f)
#define ANGLE_30    (30.0f)

#ifndef INT_MAX

#define INT_MAX (2147483647)

#endif

struct element
{
	element()
	{
		iOrder = 0;
		pName = NULL;
		iPower = 0;
	}
	element( int Order, const char* Name, int Power )
	{
		iOrder = Order;
		pName = Name;
		iPower = Power;
	}
	int iOrder;
	const char* pName;
	int iPower;
};

// 返回[0, 32767]范围随机整数
int util_range_random_int();

// 返回[0, max]范围随机整数
int util_range_random_int( int max );

// 返回[min, max]范围随机整数
int util_range_random_int( int min, int max );

// 返回[0.0f, 32767.0f]范围随机浮点数
float util_range_random_float();

// 返回[0.0f, max]范围随机浮点数
float util_range_random_float( float max );

// 返回[min, max]范围随机浮点数
float util_range_random_float( float min, float max );

// 随机生成一定范围成的不重复的整数[0, 32767]
void util_different_Random_int( const int min, const int max, const unsigned int num, std::vector<int> & iVecOut );

// 权重累加和
int util_power_sum_int( const std::vector<int> & iVecIn );

// 将圆周弧度角值转换为0-pi*2之间的值
float util_to_normalize_circular( float angle );

// 将圆周角度值转换为0-360度之间的值
float util_to_normalize_angle( float angle );

//角度转弧度
float util_angle_to_circular( float angle );

//弧度转角度
float util_circular_to_angle( float angle );

//根据权重值取得随机值
bool util_random_element_int( std::vector<element> & iVecIn, element& Element );

// 不重复插入
bool util_different_insert_int( const int num, std::vector<int> & iVecOut );

// 取参考最小值
void util_get_min_float( float fDisMin, std::vector<float> & iVecOut );

//是否在列表里
bool util_find_in_list_int( std::vector<int> & vtList, int iIndex );

// 不重复插入std::vector< std::vector<int> >
void util_different_insert_vector_int( std::vector<int> & vtList, std::vector< std::vector<int> > & iVecOut );

//获取当天零点时间戳(北京时间)
int64_t util_get_current_zero_unix_timestamp(const int iTimezone = 8);

//获取指定时间戳零点时间戳(北京时间)
int64_t util_get_current_zero_unix_timestamp(const int64_t iTimestamp, const int iTimezone = 8);

// 不重复插入
bool util_different_insert_string( const std::string& str, std::vector<std::string> & iVecOut );

//是否在列表里
bool util_find_in_list_string( std::vector<std::string> & vtList, const std::string& str );

//生成边界保护值[left, right]
int util_my_rand(int left, int right);

//随机安全上限[minProtect, limit] 取最大可用平均值
int util_average_rand(const int curNum, const int n, const int minProtect = 1);

//参数转换为分隔字符
std::wstring util_args_to_wstring(const IVarList& args);

static std::default_random_engine randomSeed(static_cast<unsigned int>(::time(NULL)));
#endif// _GAME_UTIL_H

