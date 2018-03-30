//--------------------------------------------------------------------
// 文件名:		CacheTimer.h
// 内  容:		cache timer 简易计时器，性能分析，及其计时器应用
// 说  明:		
// 创建日期:	2017年06月03日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------

#ifndef __CACHE_TIMER_HPP__
#define	__CACHE_TIMER_HPP__

//#include <windows.h>
#include "CacheString.hpp"

namespace cache{

// work time param
struct CacheWorkParam 
{
public:
	unsigned int  count_;	//次数
	unsigned long time_;	//时间
public:
	CacheWorkParam() :count_(0),time_(0){}
public:
	//for init.
	void Init(unsigned int count, unsigned long time)
	{
		count_ = count;
		time_ = time;
	}
	void Set(const CacheWorkParam& ref)
	{
		count_ = ref.count_;
		time_ = ref.time_;
	}
	//for clear
	void Clear()
	{
		count_ = 0;
		time_  = 0;
	}
	// for valid
	bool IsValid() const
	{
		if (count_>0 || time_>0)
		{
			return true;
		}
		return false;
	}
};
// work timer
struct CacheWorkTimer
{
private:
	bool is_expired_;
	unsigned long start_time_;	//开始时间
private:
	CacheWorkParam limit_param_;
	CacheWorkParam work_prev_;
	CacheWorkParam work_param_;
public:
	CacheWorkTimer() :is_expired_(false), start_time_(0){}
public:
	/// \brief 初始化运行时间
	/// \param work_count 运行次数，默认0，0表示不受限
	/// \param work_time 最大运行时间,单位毫秒，默认0，0表示不受限
	void Init(unsigned int work_count = 0, unsigned long work_time = 0)
	{
		is_expired_ = false;
		start_time_ = GetCurTicket();

		limit_param_.Init(work_count, work_time);
		work_prev_.Clear();
		work_param_.Clear();
	}
	/// \brief Valid
	bool IsValid() const { return limit_param_.IsValid(); }
public:
	/// \brief 重置
	void ReInit()
	{
		is_expired_ = false;
		start_time_ = GetCurTicket();
		work_prev_.Clear();
		work_param_.Clear();
	}
	/// \brief 更新次数
	void Update()
	{
		work_prev_.Set(work_param_);
		work_param_.count_ += 1;
		if (limit_param_.count_>0 && work_param_.count_ > limit_param_.count_)
		{
			is_expired_ = true;
		}
		work_param_.time_ = GetCurTicket() - start_time_;
		if (limit_param_.time_ > 0 && work_param_.time_ > limit_param_.time_)
		{
			is_expired_ = true;
		}
	}
	/// \brief 检查是否过期
	/// \return  true:过期, false：未过期。
	bool IsExpired() const
	{
		return is_expired_;
	}
	/// \brief 返回工作参数
	const CacheWorkParam& GetWorkPrev() const
	{
		return work_prev_;
	}
	const CacheWorkParam& GetWorkParam() const
	{
		return work_param_;
	}
private:
	/// \brief 返回时间
	unsigned long GetCurTicket()
	{ 
		return ::GetTickCount(); 
	}
};



}
#endif