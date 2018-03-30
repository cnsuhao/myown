//--------------------------------------------------------------------
// 文件名:		MiniStr.h
// 内  容:		内存占用优化字符串
// 说  明:		
// 创建日期:	2007年8月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _PUBLIC_MINISTR_H
#define _PUBLIC_MINISTR_H

#include "../public/Macros.h"
#include "../public/CharTraits.h"

// TMiniStr

template<typename TYPE, typename TRAITS = TCharTraits<TYPE> >
class TMiniStr
{
private:
	typedef TMiniStr<TYPE, TRAITS> self_type;

public:
	TMiniStr()
	{
		m_pData = NULL;
	}

	TMiniStr(const self_type& src)
	{
		if (NULL == src.m_pData)
		{
		 	m_pData = NULL;
		}
		else
		{
			const size_t SIZE1 = TRAITS::Length(src.m_pData);

			m_pData = NEW TYPE[SIZE1 + 1];

			TRAITS::Copy(m_pData, src.m_pData, SIZE1 + 1);
		}
	}

	TMiniStr(const TYPE* src)
	{
		const size_t SIZE1 = TRAITS::Length(src);

		if (SIZE1 == 0)
		{
			m_pData = NULL;
		}
		else
		{
			m_pData = NEW TYPE[SIZE1 + 1];

			TRAITS::Copy(m_pData, src, SIZE1 + 1);
		}
	}
	
	TMiniStr(const TYPE* s1, const TYPE* s2)
	{
		const size_t SIZE1 = TRAITS::Length(s1);
		const size_t SIZE2 = TRAITS::Length(s2);
		
		if ((SIZE1 + SIZE2) == 0)
		{
			m_pData = NULL;
		}
		else
		{
			m_pData = NEW TYPE[SIZE1 + SIZE2 + 1];
		
			TRAITS::Copy(m_pData, s1, SIZE1);
			TRAITS::Copy(m_pData + SIZE1, s2, SIZE2 + 1);
		}
	}
	
	~TMiniStr()
	{
		if (m_pData)
		{
			delete[] m_pData;
		}
	}

	self_type& operator=(const self_type& src)
	{
		return inner_assign(src.c_str());
	}

	self_type& operator=(const TYPE* src)
	{
		return inner_assign(src);
	}

	self_type& operator+=(const self_type& src)
	{
		return inner_append(src);
	}

	self_type& operator+=(const TYPE* src)
	{
		return inner_append(src);
	}
	
	void swap(self_type& src)
	{
		TYPE* temp_pdata = src.m_pData;
		
		src.m_pData = m_pData;
		
		m_pData = temp_pdata;
	}

	bool empty() const
	{
		return (NULL == m_pData);
	}
	
	size_t length() const
	{
		if (NULL == m_pData)
		{
			return 0;
		}
		
		return TRAITS::Length(m_pData);
	}

	const TYPE* c_str() const
	{
		if (NULL == m_pData)
		{
			return TRAITS::EmptyValue();
		}
		
		return m_pData;
	}

	int compare(const self_type& src) const
	{
		return TRAITS::Compare(c_str(), src.c_str());
	}

	int compare(const TYPE* src) const
	{
		return TRAITS::Compare(c_str(), src);
	}

	self_type& append(const self_type& src)
	{
		return inner_append(src.c_str());
	}

	self_type& append(const TYPE* src)
	{
		return inner_append(src);
	}

	size_t get_memory_usage() const
	{
		size_t size = sizeof(self_type);
		
		if (m_pData)
		{
			size += TRAITS::Length(m_pData) * sizeof(TYPE);
		}	
		
		return size;
	}

private:
	self_type& inner_assign(const TYPE* s)
	{
		self_type temp(s);
		swap(temp);
		return *this;
	}
	
	self_type& inner_append(const TYPE* s)
	{
		self_type temp(c_str(), s);
		swap(temp);
		return *this;
	}

private:
	TYPE* m_pData;
};

template<typename TYPE, typename TRAITS>
inline bool operator==(const TMiniStr<TYPE, TRAITS>& s1,
					   const TMiniStr<TYPE, TRAITS>& s2)
{
	return (s1.compare(s2) == 0);
}

template<typename TYPE, typename TRAITS>
inline bool operator==(const TMiniStr<TYPE, TRAITS>& s1,
					   const TYPE* s2)
{
	return (s1.compare(s2) == 0);
}

template<typename TYPE, typename TRAITS>
inline bool operator==(const TYPE* s1,
					   const TMiniStr<TYPE, TRAITS>& s2)
{
	return (s2.compare(s1) == 0);
}

template<typename TYPE, typename TRAITS>
inline bool operator!=(const TMiniStr<TYPE, TRAITS>& s1,
					   const TMiniStr<TYPE, TRAITS>& s2)
{
	return (s1.compare(s2) != 0);
}

template<typename TYPE, typename TRAITS>
inline bool operator!=(const TMiniStr<TYPE, TRAITS>& s1,
					   const TYPE* s2)
{
	return (s1.compare(s2) != 0);
}

template<typename TYPE, typename TRAITS>
inline bool operator!=(const TYPE* s1,
					   const TMiniStr<TYPE, TRAITS>& s2)
{
	return (s2.compare(s1) != 0);
}

template<typename TYPE, typename TRAITS>
inline TMiniStr<TYPE, TRAITS> 
operator+(const TMiniStr<TYPE, TRAITS>& s1,
		  const TMiniStr<TYPE, TRAITS>& s2)
{
	return TMiniStr<TYPE, TRAITS>(s1).append(s2);
}

template<typename TYPE, typename TRAITS>
inline TMiniStr<TYPE, TRAITS> 
operator+(const TMiniStr<TYPE, TRAITS>& s1,
		  const TYPE* s2)
{
	return TMiniStr<TYPE, TRAITS>(s1).append(s2);
}

template<typename TYPE, typename TRAITS>
inline TMiniStr<TYPE, TRAITS> 
operator+(const TYPE* s1,
		  const TMiniStr<TYPE, TRAITS>& s2)
{
	return TMiniStr<TYPE, TRAITS>(s1).append(s2);
}

typedef TMiniStr<char> mini_string;
typedef TMiniStr<wchar_t> mini_wstring;

#endif // _PUBLIC_MINISTR_H
*/
