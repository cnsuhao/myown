//--------------------------------------------------------------------
// 文件名:		TickTimer.h
// 内  容:		
// 说  明:		
// 创建日期:	2002年8月9日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#ifndef _UTILS_TICKTIMER_H
#define _UTILS_TICKTIMER_H

#include "../public/Portable.h"

// 低精度计时器

class CTickTimer
{
public:
	CTickTimer()
	{
		m_nLastMilliseconds = 0;
	}
	
	// 初始化
	void Initialize()
	{
		m_nLastMilliseconds = Port_GetTickCount();
	}
	
	// 返回逝去的毫秒数
	int GetElapseMillisec(int expect = 0)
	{
		unsigned int cur = Port_GetTickCount();
		unsigned int dw = cur - m_nLastMilliseconds;
			
		if (dw >= (unsigned int)expect)
		{
			m_nLastMilliseconds = cur;
		}
			
		return dw;
	}

private:
	unsigned int m_nLastMilliseconds;
};

#endif // _UTILS_TICKTIMER_H
