//--------------------------------------------------------------------
// 文件名:		LoadArchive.h
// 内  容:		
// 说  明:		
// 最后修改:	2008年9月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_LOADARCHIVE_H
#define _UTILS_LOADARCHIVE_H

#include "../public/Macros.h"
#include "../public/PersistId.h"
#include "../public/ExceptMsg.h"
#include <string.h>

// 序列化读取

class CLoadArchive
{
public:
	CLoadArchive(const void* pdata, size_t len)
	{
		Assert(pdata != NULL);
	
		m_pData = (char*)pdata;
		m_nMaxLen = len;
		m_nPosi = 0;
	}
	
	~CLoadArchive()
	{
	}

	// 移动指针
	void Seek(size_t len)
	{
		Assert(len <= m_nMaxLen);

		m_nPosi = len;
	}

	// 获得数据指针
	const void* GetData() const
	{
		return m_pData;
	}
	
	// 获得最大数据长度
	size_t GetMaxLen() const
	{
		return m_nMaxLen;
	}
	
	// 获得当前数据长度
	size_t GetLength() const
	{
		return m_nPosi;
	}
	
	// 获得剩余数据长度
	size_t GetRemain() const
	{
		return m_nMaxLen - m_nPosi;
	}

	// 读取数据
	void Read(void* buf, size_t len)
	{
		Assert(buf != NULL);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		memcpy(buf, m_pData + m_nPosi, len);
	
		m_nPosi += len;
	}

	// 读取数据
	bool ReadBool()
	{
		bool data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(bool*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	unsigned char ReadByte()
	{
		unsigned char data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(unsigned char*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	unsigned short ReadWord()
	{
		unsigned short data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(unsigned short*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	int ReadInt()
	{
		int data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(int*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	int64_t ReadInt64()
	{
		int64_t data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(int64_t*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	float ReadFloat()
	{
		float data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(float*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	double ReadDouble()
	{
		double data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(double*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	const char* ReadString()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		len = *(unsigned int*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(len);
		
		const char* p = m_pData + m_nPosi;
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len - 1] != 0)
		{
			throw CExceptMsg("String no end symbol");
		}
		
		return p;
	}
	
	const wchar_t* ReadWideStr()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		len = *(unsigned int*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(len);
		
		const wchar_t* p = (const wchar_t*)(m_pData + m_nPosi);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len / sizeof(wchar_t) - 1] != 0)
		{
			throw CExceptMsg("Wide string no end symbol");
		}
		
		return p;
	}
	
	const unsigned short* ReadUCS2()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		len = *(unsigned int*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(len);
		
		const unsigned short* p = (const unsigned short*)(m_pData + m_nPosi);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len / sizeof(unsigned short) - 1] != 0)
		{
			throw CExceptMsg("UCS2 string no end symbol");
		}
		
		return p;
	}
	
	PERSISTID ReadObject()
	{
		PERSISTID data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		data = *(PERSISTID*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	void* ReadBinary(size_t& size)
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		len = *(unsigned int*)(m_pData + m_nPosi);
	
		m_nPosi += sizeof(len);
		
		void* p = m_pData + m_nPosi;
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		size = len;
		
		return p;
	}
	
protected:
	CLoadArchive();
	CLoadArchive(const CLoadArchive&);
	CLoadArchive& operator=(const CLoadArchive&);

private:
	char* m_pData;
	size_t m_nMaxLen;
	size_t m_nPosi;
};

#endif // _UTILS_LOADARCHIVE_H
