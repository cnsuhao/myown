//--------------------------------------------------------------------
// 文件名:		util.cpp
// 内  容:		常用工具
// 说  明:		
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------
#include "util.h"
#include "util_func.h"
#include <string>
#include <time.h>
#include "string_util.h"
#include "../public/IVarList.h"

// 返回[0,int max]范围随机整数
int util_range_random_int()
{
	std::uniform_int_distribution<int> dis;
	return dis(randomSeed);
}

// 返回[0, max]范围随机整数
int util_range_random_int( int max )
{
	if ( max < 0 )
	{
		return 0;
	}
	std::uniform_int_distribution<int> dis(0, max);
	return dis(randomSeed);
}

// 返回[min, max]范围随机整数
int util_range_random_int( int min, int max )
{
	if ( min > max )
	{
		int temp;
		temp = min;
		min = max;
		max = temp;
	}

	if ( min < 0 )
	{
		min = 0;
	}

	if ( max <= 0 )
	{
		return 0;
	}

	std::uniform_int_distribution<int> dis(min, max);
	return dis(randomSeed);
	
}

// 返回[0.0f, 32767.0f]范围随机浮点数
float util_range_random_float()
{
	return float( rand() );
}

// 返回[0.0f, max)范围随机浮点数
float util_range_random_float( float max )
{

	if ( max < 0.0f )
	{
		return 0.0f;
	}

	std::uniform_real_distribution<float> dis(0.0, max);
	return dis(randomSeed);

	
}

// 返回[min, max)范围随机浮点数
float util_range_random_float( float min, float max )
{
	if ( min > max )
	{
		float temp;
		temp = min;
		min = max;
		max = temp;
	}

	if ( min < 0.0f )
	{
		min = 0.0f;
	}


	if ( max <= 0.0f )
	{
		return 0.0f;
	}
	std::uniform_real_distribution<float> dis(min, max);
	return dis(randomSeed);
}

// 随机生成一定范围成的不重复的整数[0, 32767]
void util_different_Random_int( const int min, const int max, const unsigned int num, std::vector<int> & iVecOut )
{
	iVecOut.clear();
	typedef std::vector<int>::iterator iVecOutIterator;
	LoopBeginCheck(a)
	while ( ( ( int )iVecOut.size() < num) ) 
	{
		LoopDoCheck(a)
		int number = util_range_random_int( min, max );
		iVecOutIterator it;
		LoopBeginCheck(b)
		for( it = iVecOut.begin(); it !=iVecOut.end() ; ++it )
		{
			LoopDoCheck(b)
			if( *it == number )
			{
				break;
			}
		}

		if( it ==iVecOut.end() )
		{
			iVecOut.push_back(number);
		}
	}
}

// 权重累加和
int util_power_sum_int( std::vector<element> & iVecIn )
{
	typedef std::vector<element>::iterator iVecInIterator;
	int sum = 0;
	LoopBeginCheck(c)
	for( iVecInIterator it = iVecIn.begin(); it !=iVecIn.end() ; ++it )
	{
		LoopDoCheck(c)
		sum += (*it).iPower;
	}

	return sum;
}

// 将圆周弧度角值转换为0-pi*2之间的值
float util_to_normalize_circular( float angle )
{
	float value = ::fmod( angle, PI2 );
	if ( value < 0 )
	{
		value = value + PI2;
	}

	return value;
}

// 将圆周角度值转换为0-360度之间的值
float util_to_normalize_angle( float angle )
{
	float value = ::fmod( angle, ANGLE360 );
	if ( value < 0 )
	{
		value = value + ANGLE360;
	}

	return value;
}

//角度转弧度
float util_angle_to_circular( float angle )
{
	return util_to_normalize_angle( angle ) / ANGLE360 * PI2;
}

//弧度转角度
float util_circular_to_angle( float angle )
{
	return util_to_normalize_circular( angle ) / PI2 * ANGLE360;
}

//根据权重值取得随机值
bool util_random_element_int( std::vector<element> & iVecIn, element& Element )
{
	int iSumPower = util_power_sum_int( iVecIn );
	typedef std::vector<element>::iterator iVecInIterator;
	int iRandPower = util_range_random_int( iSumPower );
	int iTempSumPower = 0;
	LoopBeginCheck(d)
	for( iVecInIterator it = iVecIn.begin(); it !=iVecIn.end() ; ++it )
	{
		LoopDoCheck(d)
		iTempSumPower += (*it).iPower;
		if( iTempSumPower >= iRandPower )
		{
			Element = *it;

			return true;
		}
	}

	return false;
}

// 不重复插入
bool util_different_insert_int( const int num, std::vector<int> & iVecOut )
{
	typedef std::vector<int>::iterator iVecOutIterator;
	iVecOutIterator it;
	LoopBeginCheck(e)
	for( it = iVecOut.begin(); it !=iVecOut.end() ; ++it )
	{
		LoopDoCheck(e)
		if( *it == num )
		{
			break;
		}
	}

	if( it == iVecOut.end() )
	{
		iVecOut.push_back(num);

		return true;
	}

	return false;
}

// 取参考最小值
void util_get_min_float( float fDisMin, std::vector<float> & iVecOut )
{
	typedef std::vector<float>::iterator iVecOutIterator;
	iVecOutIterator it;
	LoopBeginCheck(f)
	for( it = iVecOut.begin(); it !=iVecOut.end() ; ++it )
	{
		LoopDoCheck(f)
		if( *it < fDisMin )
		{
			fDisMin = *it;
		}
	}
}

//是否在列表里
bool util_find_in_list_int( std::vector<int> & vtList, int iIndex )
{
	LoopBeginCheck(g)
	for ( std::vector<int>::reverse_iterator it = vtList.rbegin(); it != vtList.rend(); ++it )
	{
		LoopDoCheck(g)
		if ( *it == iIndex )
		{
			return true;
		}
	}

	return false;
}

// 不重复插入std::vector< std::vector<int> >
void util_different_insert_vector_int( std::vector<int> & vtList, std::vector< std::vector<int> > & iVecOut )
{
	typedef std::vector< std::vector<int> >::iterator iVecOutIterator;
	iVecOutIterator it;
	LoopBeginCheck(h)
	for( it = iVecOut.begin(); it !=iVecOut.end() ; ++it )
	{
		LoopDoCheck(h)
		if( *it == vtList )
		{
			break;
		}
	}

	if( it == iVecOut.end() )
	{
		iVecOut.push_back(vtList);
	}
}

//获取当天零点时间戳(北京时间)
int64_t util_get_current_zero_unix_timestamp(const int iTimezone)
{
	time_t iCurTime = ::time(NULL);
	return iCurTime - (iCurTime % UTIL_ONE_DAY_S  + iTimezone * UTIL_ONE_HOUR_S) % UTIL_ONE_DAY_S;
}

//获取指定时间戳零点时间戳(北京时间)
int64_t util_get_current_zero_unix_timestamp(const int64_t iTimestamp, const int iTimezone)
{
	return iTimestamp - (iTimestamp % UTIL_ONE_DAY_S  + iTimezone * UTIL_ONE_HOUR_S) % UTIL_ONE_DAY_S;
}

// 不重复插入
bool util_different_insert_string( const std::string& str, std::vector<std::string> & iVecOut )
{
	typedef std::vector<std::string>::iterator iVecOutIterator;
	iVecOutIterator it;
	LoopBeginCheck(i)
	for( it = iVecOut.begin(); it !=iVecOut.end() ; ++it )
	{
		LoopDoCheck(i)
		if( *it == str )
		{
			break;
		}
	}

	if( it == iVecOut.end() )
	{
		iVecOut.push_back(str);

		return true;
	}

	return false;
}

//是否在列表里
bool util_find_in_list_string( std::vector<std::string> & vtList, const std::string& str )
{
	LoopBeginCheck(j)
	for ( std::vector<std::string>::reverse_iterator it = vtList.rbegin(); it != vtList.rend(); ++it )
	{
		LoopDoCheck(j)
		if ( *it == str )
		{
			return true;
		}
	}

	return false;
}

//生成边界保护值[left, right]
int util_my_rand(int left, int right)
{
	if (left >= right)
	{
		return left;
	}
	else 
	{
		int64_t time = ::time(NULL);
		return left + time % (right - left);
	}
}

//随机安全上限[minProtect, limit] 取最大可用平均值
int util_average_rand(const int curNum, const int n, const int minProtect)
{
	if (n <= 1)
	{
		return curNum;
	}

	int limit = (curNum -(n - 1) * minProtect) / (n - 1);
	
	return util_my_rand((int)(minProtect), (int)(limit));
}

//参数转换为分隔字符(VTYPE_OBJECT除外)
std::wstring util_args_to_wstring(const IVarList& args)
{
	std::wstring tempStr = L"";
	size_t iCount = args.GetCount();
	LoopBeginCheck(k);
	for (size_t i = 0; i < iCount; ++i)
	{
		LoopDoCheck(k);
		int iType = args.GetType(i);
		if (VTYPE_INT == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + StringUtil::IntAsWideStr(args.IntVal(i));
		}
		else if (VTYPE_FLOAT == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + StringUtil::FloatAsWideStr(args.FloatVal(i));
		}
		else if (VTYPE_DOUBLE == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + StringUtil::StringAsWideStr(StringUtil::DoubleAsString(args.DoubleVal(i)).c_str());
		}
		else if (VTYPE_INT64 == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + StringUtil::StringAsWideStr(StringUtil::Int64AsString(args.Int64Val(i)).c_str());
		}
		else if (VTYPE_STRING == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + StringUtil::StringAsWideStr(args.StringVal(i));
		}
		else if (VTYPE_WIDESTR == iType)
		{
			if (!tempStr.empty())
			{
				tempStr = tempStr + L":";
			}
			tempStr = tempStr + args.WideStrVal(i);
		}
	}

	return tempStr;
}