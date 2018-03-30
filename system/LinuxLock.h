//-------------------------------------------------------------------- 
// 文件名:		LinuxLock.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUXLOCK_H
#define _SYSTEM_LINUXLOCK_H

#include <pthread.h>

// 共享资源互斥访问锁

class CLockUtil
{
public:
	CLockUtil() 
	{ 
		pthread_mutex_init(&m_Handle, NULL); 
	}

	~CLockUtil() 
	{ 
		pthread_mutex_destroy(&m_Handle); 
	}

	// 锁定
	void Lock() 
	{ 
		pthread_mutex_lock(&m_Handle); 
	}

	// 释放
	void Unlock() 
	{	
		pthread_mutex_unlock(&m_Handle); 
	}

private:
	pthread_mutex_t m_Handle;
};

#endif // _SYSTEM_LINUXLOCK_H
