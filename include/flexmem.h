//-------------------------------------------------------------------
// 文 件 名：flexmem.h
// 内    容：
// 说    明：
// 创建日期：2013年9月25日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __FLEXMEM_H__
#define __FLEXMEM_H__

#include <stdlib.h>

class FlexMem
{
public:
	FlexMem()
	{
		m_nSize = 0;
		m_pData = 0;
	}

	FlexMem(int size)
	{
		m_nSize = size;
		m_pData = NEW char[size];
	}

	~FlexMem()
	{
		if (m_pData)
		{
			delete[] m_pData;
		}

		m_nSize = 0;
	}

public:
	char* GetBuffer()
	{
		return m_pData;
	}

	bool Resize(int byte_size)
	{
		if (byte_size > m_nSize)
		{
			char* p = NEW char[byte_size];

			if (m_pData)
			{
				// memcpy(p, m_pData, m_nSize);
				delete[] m_pData;
			}
			
			m_pData = p;
			m_nSize = byte_size;
		}

		return true;
	}

	int MaxSize()
	{
		return m_nSize;
	}

private:
	int m_nSize;
	char* m_pData;
};

#endif // __FLEXMEM_H__
