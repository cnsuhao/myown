//--------------------------------------------------------------------
// 文件名:		fm_math.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月22日
// 创建人:		张俊胜
//    :	   
// 备注内容:

//--------------------------------------------------------------------


#ifndef _FM_MATH_H
#define _FM_MATH_H

// #include "fm_vector.h"
// #include "fm_quaternion.h"
// #include "fm_matrix.h"
#include "fm_fixedpoint.h"
#include "vector.h"
//#include "KeMathBase.h"

//value                               2.1      2.6      -2.1     -2.6
//floor  :不大于自变量的最大整数      2        2        -3       -3
//ceil   :不小于自变量的最大整数      3        3        -2       -2
//round  :四舍五入到最邻近的整数      2        3        -2       -3
inline int64_t number_floor(double value)
{
	return (int64_t)floor(value);
}
#define express_floor(p)  number_floor((double)(p))
inline int64_t number_ceil(double value) 
{
	return (int64_t)ceil(value);
}
#define express_ceil(p)  number_ceil((double)(p))
inline int64_t number_round(double value)
{
	return (int64_t)round(value);
}
#define express_round(p)  number_round((double)(p))

//percent: %100+%n, percent(0~100)
inline double safe_percent_inc(int percent, bool need_lq_200 = true)
{
	if (percent<0)
	{
		return (double)1.0;
	}
	if (need_lq_200 && percent>100)
	{
		return (double)2.0;
	}
	return (double)((double)(100 + percent) / 100);
}
//percent: %n , percent(0~100)
inline double safe_percent(int percent, bool need_lq_100 = true)
{
	if (percent<0)
	{
		return (double)0.0;
	}
	if (need_lq_100 && percent > 100)
	{
		return (double)1.0;
	}
	return (double)((double)percent / 100);
}

#endif