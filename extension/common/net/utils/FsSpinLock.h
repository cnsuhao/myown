/*@FileName:	FsSpinLock.h
* @Author:		Àîº£ÂÞ
* @Function:	×ÔÐýËø
* @Modify:		2012-11-11
*/
#ifndef __FAST_FRAMEWORK_SPIN_LOCK_INCLUDE__
#define __FAST_FRAMEWORK_SPIN_LOCK_INCLUDE__

#include <atomic>
#include <thread>
#include <mutex>

class SpinLock
{
	std::atomic_flag m_lock_queue;
public:
private:
	SpinLock(const SpinLock&) = delete;
	SpinLock& operator =(const SpinLock&) = delete;
public:
	SpinLock()
	{
		m_lock_queue.clear();
	}

	~SpinLock()
	{
		UnLock();
	}

	void Lock()
	{
		while (m_lock_queue.test_and_set())
			std::this_thread::yield();
			//std::this_thread::sleep_for( std::chrono::milliseconds(1) );
	}

	void UnLock()
	{
		m_lock_queue.clear();
	}

	bool TryLock()
	{
		return !m_lock_queue.test_and_set();
	}
};

class FsLocker
{
	std::mutex m_mutex;
public:
	void Lock()
	{
		m_mutex.lock();
	}

	void UnLock()
	{
		m_mutex.unlock();
	}

	bool TryLock()
	{
		return m_mutex.try_lock();
	}
};


template<typename T>
class LockGuard
{
	T& m_impLocker;
public:
	LockGuard(T& locker) : m_impLocker( locker )
	{
		m_impLocker.Lock();
	}

	~LockGuard()
	{
		m_impLocker.UnLock();
	}
};

#endif	// END __FAST_FRAMEWORK_SPIN_LOCK_INCLUDE__