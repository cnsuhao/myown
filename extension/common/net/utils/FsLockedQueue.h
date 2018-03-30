/*@FileName:	FsLockedQueue.h
* @Author:		李海罗
* @Function:	加锁队列
* @Modify:		2012-11-11
*/
#ifndef __FAST_FRAMEWORK_LOCKED_QUEUE_INCLUDE__
#define __FAST_FRAMEWORK_LOCKED_QUEUE_INCLUDE__

#include <queue>
#include "FsSpinLock.h"

template<typename T, typename LOCKER = SpinLock, typename _Container=std::deque<T> >
class FsLockedQueue : protected std::queue<T, _Container>
{
protected:
	LOCKER m_queue_locker;
public:
	void EnQueue(const T& data)
	{
		LockGuard<LOCKER> guard(m_queue_locker);
		this->push(data);
	}

	bool DeQueue(T& data)
	{
		bool bOK = false;
		LockGuard<LOCKER> guard(m_queue_locker);
		if ( size() > 0)
		{
			bOK = true;
			data = this->front();
			this->pop();
		}

		return bOK;
	}

	size_t Length() const
	{
		LockGuard<LOCKER> guard(m_queue_locker);
		return size();
	}

	bool IsEmpty() const
	{
		return Length() == 0;
	}
};

#endif	// END __FAST_FRAMEWORK_LOCKED_QUEUE_INCLUDE__