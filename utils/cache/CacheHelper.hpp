//--------------------------------------------------------------------
// 文件名:		CacheHelper.h
// 内  容:		Cache类助手
// 说  明:		
// 创建日期:	2017年06月03日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------

#ifndef __CACHE_HELPER_HPP__
#define	__CACHE_HELPER_HPP__

#include "CacheTimer.hpp"
#include "FsGame/Define/LogDefine.h"
#include "utils/extend_func.h"

#ifdef _DEBUG

// work timer for func
#define LOGIC_WORKTIME_BEGIN(name)  \
	cache::CacheWorkTimer t_##name##_func; \
	t_##name##_func.Init(cache::GetWorkParam_Logic().count_, cache::GetWorkParam_Logic().time_);
#define LOGIC_WORKTIME_CHECK(name,is_after_reset) \
	cache::WorkTimer_Update(t_##name##_func, #name, __FILE__, __LINE__, is_after_reset);


// work timer for loops
#define LOOPS_WORKTIME_BEGIN()  \
	cache::GetWorkTime_Loops().ReInit();
#define LOOPS_WORKTIME_CHECK(name) \
	cache::WorkTimer_Update(cache::GetWorkTime_Loops(), #name, __FILE__, __LINE__, false);

#else
// work timer for func
#define LOGIC_WORKTIME_BEGIN(name)
#define LOGIC_WORKTIME_CHECK(name,is_after_reset)


// work timer for loops
#define LOOPS_WORKTIME_BEGIN()
#define LOOPS_WORKTIME_CHECK(name)

#endif

namespace cache{

// func.wormparam
static CacheWorkParam& GetWorkParam_Logic()
{
	static CacheWorkParam work_param_logic_;
	if (!work_param_logic_.IsValid())
	{
		work_param_logic_.Init(100, 17);
	}
	return work_param_logic_;
}
// loops.worktime
static CacheWorkTimer& GetWorkTime_Loops()
{
	static CacheWorkTimer work_time_loops_;
	if (!work_time_loops_.IsValid())
	{
		work_time_loops_.Init(100, 17);
	}
	return work_time_loops_;
}
//for work timer.
static void WorkTimer_Update(CacheWorkTimer& p_timer,const char* title,const char* strfile, unsigned int n_line, bool is_reinit = true)
{
	p_timer.Update();
	if (p_timer.IsExpired())
	{
		::extend_warning(::LOG_WARNING, "[%s] count=%d,time=%d,%s[%d]", title, p_timer.GetWorkParam().count_, p_timer.GetWorkParam().time_, strfile, n_line);
	}
	if (is_reinit)
	{
		p_timer.ReInit();
	}
	
}

}
#endif