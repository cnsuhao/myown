//--------------------------------------------------------------------
// 文件名:		FsThreadQueue.h
// 内  容:		线程安全队列
// 说  明:		
// 创建日期:		2013年5月10日
// 修改日期:		2016年9月14日
// 创建人:		李海罗
//--------------------------------------------------------------------
#ifndef __FS_THREAD_QUEUE_H__
#define __FS_THREAD_QUEUE_H__
#include "utils/FsLockedQueue.h"

class FsThreadQueue
{
private:
	typedef std::function<void()> Action;
	FsLockedQueue<Action>	m_queueActions;

public:
	void RunInQueueThread(const Action& action)
	{
		m_queueActions.EnQueue( action );
	}

	void RunQueue()
	{
		Action action;
		while ( m_queueActions.DeQueue(action) )
		{
			action();
		}
	}
};

#endif // END __FS_THREAD_QUEUE_H__