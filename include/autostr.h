//-------------------------------------------------------------------
// 文 件 名：autostr.h
// 内    容：
// 说    明：
// 创建日期：2013年10月21日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __AUTOSTR_H__
#define __AUTOSTR_H__

#include "../../public/VarType.h"
#include "../../public/debug.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

template<typename TYPE>
class TAutostrCharTraits
{
};

template<>
class TAutostrCharTraits<char>
{
public:
	static unsigned int Length(const char* s)
	{
		return (unsigned int)strlen(s);
	}

	static bool Equal(const char* s1,const char* s2)
	{
		return strcmp(s1,s2)==0;
	}

	static void Copy(char* dst, const char* src, unsigned int nChrCnt)
	{
		memcpy(dst, src, nChrCnt);
	}

	static int Compare(const char* s1, const char* s2)
	{
		return strcmp(s1, s2);
	}

	static void Put(char* s, char c)
	{
		*s = c;
	}

	static unsigned int GetHashValue(const char* s)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		while (*s)
		{
			hash = hash * seed + (*s++);
		}

		return (hash & 0x7FFFFFFF);
	}

	static const char* EmptyValue()
	{
		return "";
	}

	static unsigned int Find(const char* s, const char* sub)
	{
		const char* p = strstr(s, sub);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}
	static unsigned int FindChr(const char* s, char c)
	{
		const char* p = strchr(s, c);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}
	static unsigned int RFindChr(const char* s, char c)
	{
		const char* p = strrchr(s, c);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}

	static int ToInt(const char* s)
	{
		return atoi(s);
	}

	static __int64 ToInt64(const char* s)
	{
		return _atoi64(s);
	}

	static double ToDouble(const char* s)
	{
		return atof(s);
	}

	static unsigned int NPos()
	{
		return unsigned int(-1);
	}

	static const char* GetFormater(int type)
	{
		switch (type)
		{
		case VTYPE_INT: return "%d";
		case VTYPE_INT64: return "%lld";
		case VTYPE_FLOAT:
		case VTYPE_DOUBLE: return "%f";
		case VTYPE_STRING: return "%s";
		case VTYPE_POINTER: return "%p";
		}
		return 0;
	}

	static unsigned int Sprintf(char* buf, int chrCnt, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		int n = _vsnprintf_s(buf, chrCnt, chrCnt - 1, fmt, args);
		va_end(args);

		if (n > 0 && n < chrCnt)
		{
			// 正常
			buf[n] = 0;
			return n;
		}
		else if (n >= chrCnt)
		{
			buf[chrCnt-1] = 0;
			return chrCnt-1;
		}

		// 发生错误
		buf[0] = 0;
		return 0;
	}
};


template<>
class TAutostrCharTraits<wchar_t>
{
public:
	static unsigned int Length(const wchar_t* s)
	{
		return (unsigned int)wcslen(s);
	}

	static bool Equal(const wchar_t* s1,const wchar_t* s2)
	{
		return wcscmp(s1,s2)==0;
	}

	static void Copy(wchar_t* dst, const wchar_t* src, unsigned int nChrCnt)
	{
		memcpy(dst, src, sizeof(wchar_t)*nChrCnt);
	}

	static int Compare(const wchar_t* s1, const wchar_t* s2)
	{
		return wcscmp(s1, s2);
	}

	static void Put(wchar_t* s, wchar_t c)
	{
		*s = c;
	}

	static unsigned int GetHashValue(const wchar_t* ws)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		while(*ws)
		{
			hash = hash * seed + (*ws++);
		}

		return (hash & 0x7FFFFFFF);
	}

	static const wchar_t* EmptyValue()
	{
		return L"";
	}

	static unsigned int Find(const wchar_t* s, const wchar_t* sub)
	{
		const wchar_t* p = wcsstr(s, sub);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}
	static unsigned int FindChr(const wchar_t* s, wchar_t c)
	{
		const wchar_t* p = wcschr(s, c);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}
	static unsigned int RFindChr(const wchar_t* s, wchar_t c)
	{
		const wchar_t* p = wcsrchr(s, c);

		if (p)
		{
			return unsigned int(p-s);
		}

		return NPos();
	}

	static int ToInt(const wchar_t* ws)
	{
		return _wtoi(ws);
	}

	static __int64 ToInt64(const wchar_t* ws)
	{
		return _wtoi64(ws);
	}

	static double ToDouble(const wchar_t* ws)
	{
		return _wtof(ws);
	}

	static unsigned int NPos()
	{
		return unsigned int (-1);
	}

	static const wchar_t* GetFormater(int type)
	{
		switch (type)
		{
		case VTYPE_INT: return L"%d";
		case VTYPE_INT64: return L"%lld";
		case VTYPE_FLOAT:
		case VTYPE_DOUBLE: return L"%f";
		case VTYPE_STRING: return L"%s";
		case VTYPE_POINTER: return L"%p";
		}
		return 0;
	}

	static unsigned int Sprintf(wchar_t* buf, int chrCnt, const wchar_t* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		int n = _vsnwprintf_s(buf, chrCnt, chrCnt - 1, fmt, args);
		va_end(args);

		if (n > 0 && n < chrCnt)
		{
			// 正常
			buf[n] = 0;
			return n;
		}
		else if (n >= chrCnt)
		{
			buf[chrCnt-1] = 0;
			return chrCnt-1;
		}

		// 发生错误
		buf[0] = 0;
		return 0;
	}
};

template<typename TYPE>
struct TAutoStrAllocator
{
	static TYPE* Alloc(int count)
	{
		return NEW TYPE[count];
	}
	static void Free(const TYPE* p, int)
	{
		delete[] p;
	}
};

template<typename TYPE, unsigned int SIZE>
class TAutoStr
{
public:
	typedef TAutoStr<TYPE,SIZE> self_type;
	typedef TAutostrCharTraits<TYPE> TRAITS;
	typedef TAutoStrAllocator<TYPE> ALLOCATOR;

public:
	TAutoStr()
	{
		m_pData = m_pArray;
		m_pEnd	= m_pData + SIZE;
		m_bExtenMem = false;
		Clear();
	}
	TAutoStr(const TYPE* str)
	{
		m_pData = &m_pArray[0];
		m_pEnd	= m_pData + SIZE;
		unsigned int nChrCnt = TRAITS::Length(str);
		Assign(str, nChrCnt);
	}
	TAutoStr(const TYPE* str, unsigned int nChrCnt)
	{
		m_pData = m_pArray;
		m_pEnd	= m_pData + SIZE;
		Assign(str, nChrCnt);
	}

	TAutoStr(const self_type& other)
	{
		m_pData = m_pArray;
		m_pEnd	= m_pData + SIZE;
		Assign(other.c_str(), other.Length());
	}

	/*template<unsigned int SIZE2>
	TAutoStr(const TAutoStr<TYPE,SIZE2>& other)
	{
		m_pData = m_pArray;
		m_pEnd	= m_pData + SIZE;
		Assign(other.c_str(), other.Length());
	}*/
	~TAutoStr()
	{
		unsigned int caps = Capacity();

		if (caps > SIZE && !m_bExtenMem)
		{
			ALLOCATOR::Free(m_pData, caps);
		}
	}

public:
	unsigned int Length() const
	{
		return unsigned int(m_pCurrent - m_pData);
	}
	bool Empty() const 
	{
		return m_pCurrent == m_pData;
	}
	unsigned int RemainLen() const
	{
		return m_pEnd - m_pCurrent;
	}
	unsigned int Capacity() const
	{
		return unsigned int(m_pEnd - m_pData);
	}
	const TYPE* c_str() const
	{
		return m_pData;
	}
	const TYPE* back() const 
	{
		return m_pCurrent-1;
	}
	TYPE* Data()
	{
		return m_pData;
	}
	void Clear()
	{
		m_pCurrent	= m_pData;
		*m_pCurrent = 0;
	}
	void ClearData()
	{
		m_pCurrent	= m_pData;
		*m_pCurrent = 0;
	}
	void RemoveLastChr()
	{
		if (m_pCurrent > m_pData)
		{
			--m_pCurrent;
			*m_pCurrent = 0;
		}
	}
	void UseMemory(void* pmem, int len)
	{
		unsigned int caps = Capacity();

		if (caps > SIZE && !m_bExtenMem)
		{
			ALLOCATOR::Free(m_pData, caps);
		}

		int size = len/sizeof(TYPE);
		m_pData	= (TYPE*)pmem;
		m_pCurrent = m_pData;
		m_pEnd	= m_pData+size;
		*m_pCurrent = 0;
		m_bExtenMem = true;
	}

	// 从nBeginPos开始 向后查找字符c出现的位置
	unsigned int Find(TYPE c, unsigned int nBeginPos = 0) const
	{
		TYPE* p = m_pData+nBeginPos;

		while(p && p < m_pCurrent)
		{
			if (*p == c)
			{
				return p-m_pData;
			}

			++p;
		}

		return TRAITS::NPos();
	}

	// 从nBeginPos开始 向后查找str首次出现的位置
	unsigned int Find(const TYPE* str, unsigned int nBeginPos = 0) const
	{
		if (m_pCurrent > m_pData)
		{
			return TRAITS::Find(m_pData+nBeginPos, str);
		}

		return TRAITS::NPos();
	}

	// 从nBeginPos开始 向前查找字符c出现的位置
	unsigned int RFind(TYPE c, unsigned int nBeginPos) const
	{
		TYPE* p = m_pData+nBeginPos-1;

		while(p && p >= m_pData)
		{
			if (*p == c)
			{
				return (unsigned int)(p-m_pData);
			}

			--p;
		}

		return TRAITS::NPos();
	}

	self_type Sub(unsigned int nBeginPos, unsigned int nChrCnt) const
	{
		self_type str;
		if (Length() >= (nBeginPos+nChrCnt))
		{
			str.Assign(m_pData+nBeginPos, nChrCnt);
		}

		return str;
	}

	// 删除最后nChrCnt个字符
	void RmLastChr(unsigned int nChrCnt = 1)
	{
		if ((m_pCurrent-nChrCnt) >= m_pData)
		{
			m_pCurrent -= nChrCnt;
			*m_pCurrent = 0;
		}
	}

public:
	//----------------------------------
	// 字符串扩展
	//----------------------------------
	self_type& AppendChr(TYPE value)
	{
		Expand(1);
		*m_pCurrent = value;
		++m_pCurrent;
		*m_pCurrent = 0;
		return *this;
	}
	self_type& AppendInt(int value)
	{
		const unsigned int need_max = 12;
		Expand(need_max);
		m_pCurrent += TRAITS::Sprintf(m_pCurrent, need_max, TRAITS::GetFormater(VTYPE_INT), value);
		*m_pCurrent = 0;
		return *this;
	}
	self_type& AppendInt64(__int64 value)
	{
		const unsigned int need_max = 20;
		Expand(need_max);
		m_pCurrent += TRAITS::Sprintf(m_pCurrent, need_max, TRAITS::GetFormater(VTYPE_INT64), value);
		*m_pCurrent = 0;
		return *this;
	}
	self_type& AppendFloat(float value)
	{
		const unsigned int need_max = 12;
		Expand(need_max);
		m_pCurrent += TRAITS::Sprintf(m_pCurrent, need_max, TRAITS::GetFormater(VTYPE_FLOAT), value);
		*m_pCurrent = 0;
		return *this;
	}
	self_type& AppendDouble(double value)
	{
		const unsigned int need_max = 12;
		Expand(need_max);
		m_pCurrent += TRAITS::Sprintf(m_pCurrent, need_max, TRAITS::GetFormater(VTYPE_DOUBLE), value);
		*m_pCurrent = 0;
		return *this;
	}
	self_type& AppendString(const TYPE* value, unsigned int chrCount = 0)
	{
		if (chrCount == 0)
		{
			chrCount = TRAITS::Length(value);
		}

		Expand(chrCount);
		TRAITS::Copy(m_pCurrent,value,chrCount);
		m_pCurrent += chrCount;
		*m_pCurrent = 0;
		return *this;
	}

	//----------------------------------
	// 操作符 << 重载
	//----------------------------------
	self_type& operator << (TYPE value)
	{
		return AppendChr(value);
	}
	self_type& operator << (int value)
	{
		return AppendInt(value);
	}
	self_type& operator << (__int64 value)
	{
		return AppendInt64(value);
	}
	self_type& operator << (float value)
	{
		return AppendFloat(value);
	}
	self_type& operator << (double value)
	{
		return AppendDouble(value);
	}
	self_type& operator << (const TYPE* value)
	{
		return AppendString(value);
	}
	template<unsigned int SIZE2>
	self_type& operator <<(const TAutoStr<TYPE,SIZE2>& other)
	{
		return AppendString(other.c_str(), other.Length());
	}

	//----------------------------------
	// 操作符 + 重载
	//----------------------------------
	self_type operator + (TYPE value)
	{
		self_type temp(*this);
		return temp.AppendChr(value);
	}
	self_type operator + (int value)
	{
		self_type temp(*this);
		return temp.AppendInt(value);
	}
	self_type operator + (__int64 value)
	{
		self_type temp(*this);
		return temp.AppendInt64(value);
	}
	self_type operator + (float value)
	{
		self_type temp(*this);
		return temp.AppendFloat(value);
	}
	self_type operator + (double value)
	{
		self_type temp(*this);
		return temp.AppendDouble(value);
	}
	self_type operator + (const TYPE* value)
	{
		self_type temp(*this);
		return temp.AppendString(value);
	}
	template<unsigned int SIZE2>
	self_type operator + (const TAutoStr<TYPE,SIZE2>& other)
	{
		self_type temp(*this);
		return temp.AppendString(other.c_str(), other.Length());
	}

	//----------------------------------
	// 操作符 = 重载
	//----------------------------------
	self_type& operator =(const TYPE* str)
	{
		unsigned int nChrCnt = TRAITS::Length(str);
		Assign(str, nChrCnt);
		return *this;
	}

	self_type& operator =(const self_type& other)
	{
		Assign(other.c_str(), other.Length());
		return *this;
	}

	/*template<unsigned int SIZE2>
	self_type& operator =(const TAutoStr<TYPE,SIZE2>& other)
	{
		Assign(other.c_str(), other.Length());
		return *this;
	}*/

	bool operator==(const TYPE* str) const
	{
		return TRAITS::Equal(m_pData, str);
	}

	bool operator==(const self_type& other) const
	{
		return TRAITS::Equal(other.c_str(), m_pData);
	}

	/*template<unsigned int SIZE2>
	bool operator =(const TAutoStr<TYPE,SIZE2>& other) const
	{
		return TRAITS::Equal(m_pData, other.c_str());
	}*/

	//----------------------------------
	// 操作符 += 重载
	//----------------------------------
	self_type& operator += (TYPE value)
	{
		return AppendChr(value);
	}
	self_type& operator += (int value)
	{
		return AppendInt(value);
	}
	self_type& operator += (__int64 value)
	{
		return AppendInt64(value);
	}
	self_type& operator += (float value)
	{
		return AppendFloat(value);
	}
	self_type& operator += (double value)
	{
		return AppendDouble(value);
	}
	self_type& operator += (const TYPE* value)
	{
		return AppendString(value);
	}
	template<unsigned int SIZE2>
	self_type& operator +=(const TAutoStr<TYPE,SIZE2>& other)
	{
		return AppendString(other.c_str(), other.Length());
	}

	TYPE& operator[](unsigned int index)
	{
		Assert(index<Length());
		return *(m_pData+index);
	}

private:
	// 赋值
	void Assign(const TYPE* str, unsigned int chrCount = 0)
	{
		unsigned int caps = Capacity();
		if (chrCount >= caps)
		{
			if (caps > SIZE && !m_bExtenMem)
			{
				ALLOCATOR::Free(m_pData, caps);
			}

			m_pData = ALLOCATOR::Alloc(chrCount+1);
			m_pEnd = m_pData+chrCount;
			m_bExtenMem = false;
		}

		TRAITS::Copy(m_pData, str, chrCount);
		m_pCurrent	= m_pData+chrCount;
		*m_pCurrent = 0;
	}

	void Expand(unsigned int chrCount)
	{
		// 剩余长度足够
		if (unsigned int(m_pEnd-m_pCurrent) > chrCount)
		{
			return;
		}

		// 计算新内存长度
		unsigned int caps		= Capacity();
		unsigned int new_caps	= caps + chrCount;
		if (new_caps < caps*2 && new_caps < 0x100000)
		{
			new_caps = caps*2;
		}

		// 复制到新内存
		TYPE* p = ALLOCATOR::Alloc(new_caps);
		unsigned int len = Length();
		TRAITS::Copy(p, m_pData, len);

		// 回收旧内存
		if (caps > SIZE && !m_bExtenMem)
		{
			ALLOCATOR::Free(m_pData, caps);
		}

		m_bExtenMem = false;

		// 设置游标
		m_pData		= p;
		m_pEnd		= m_pData+new_caps;
		m_pCurrent	= m_pData + len;
		*m_pCurrent = 0;
	}

private:
	TYPE m_pArray[SIZE];
	TYPE* m_pData;
	TYPE* m_pCurrent;
	TYPE* m_pEnd;
	bool  m_bExtenMem;
};

typedef TAutoStr<char,16> small_str;
typedef TAutoStr<wchar_t,16> small_wstr;
typedef TAutoStr<char,128> normal_str;
typedef TAutoStr<wchar_t,128> normal_wstr;
typedef TAutoStr<char,1024> large_str;
typedef TAutoStr<wchar_t,1024> large_wstr;

typedef TAutostrCharTraits<char> straits;
typedef TAutostrCharTraits<wchar_t> wtraits;



#include <Windows.h>
#include "flexmem.h"
#include "../../public/VarList.h"

class StringUtil
{
public:
	typedef TAutostrCharTraits<char> StrTraits;
	typedef TAutostrCharTraits<wchar_t> WstrTraits;
public:
	static const wchar_t* StringToWide(const char* str, int cp = CP_ACP)
	{
		int size = MultiByteToWideChar(cp, 0, str, -1, NULL, 0);

		static FlexMem sMem(128);
		sMem.Resize(size*sizeof(wchar_t));

		wchar_t* wsBuff = (wchar_t*)sMem.GetBuffer();

		int res = MultiByteToWideChar(cp, 0, str, -1, wsBuff, int(size));
		if (res == 0)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				wsBuff[size - 1] = 0;
			}
			else
			{
				wsBuff[0] = 0;
			}
		}

		return wsBuff;
	}

	static const char* WideToString(const wchar_t* wstr, int cp = CP_ACP)
	{
		int len = WideCharToMultiByte(cp, 0, wstr, -1, NULL, 0, NULL, NULL);

		static FlexMem sMem(128);
		sMem.Resize(len);
		char* sBuff = sMem.GetBuffer();

		int res = WideCharToMultiByte(cp, 0, wstr, -1, sBuff, len, NULL, NULL);

		if (0 == res)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				sBuff[len - 1] = 0;
			}
			else
			{
				sBuff[0] = 0;
			}
		}

		return sBuff;	
	}

	static const char* StringToUtf8(const char* str)
	{
		return WideToString(StringToWide(str, CP_ACP), CP_UTF8);
	}
	static const char* Utf8ToString(const char* str)
	{
		return WideToString(StringToWide(str, CP_UTF8), CP_ACP);
	}

	static const char* WideToUtf8(const wchar_t* wstr)
	{
		return WideToString(wstr, CP_UTF8);
	}
	static const wchar_t* Utf8ToWide(const char* str)
	{
		return StringToWide(str, CP_UTF8);
	}

	static void SplitString(IVarList& var, const char* str, const char* spliter)
	{
		unsigned int spliter_len = (unsigned int)strlen(spliter);
		TAutoStr<char, 128> strTemp;
		const char* s = str;
		const char* p = strstr(s, spliter);
		while (p)
		{
			strTemp.ClearData();

			unsigned int len = unsigned int(p-s);
			if (len > 0)
			{
				strTemp.AppendString(s, len);
				var << strTemp.c_str();
			}
			else
			{
				var << "";
			}

			s = p+spliter_len;
			p = strstr(s, spliter);
		}

		if (*s)
		{
			strTemp.ClearData();
			strTemp.AppendString(s);
			var << strTemp.c_str();
		}
	}

	static void SplitString(IVarList& var, const char* str, const char spliter = ',')
	{
		TAutoStr<char, 128> strTemp;
		const char* s = str;
		const char* p = strchr(s, spliter);
		while (p)
		{
			strTemp.ClearData();

			unsigned int len = unsigned int(p-s);
			if (len > 0)
			{
				strTemp.AppendString(s, len);
				var << strTemp.c_str();
			}
			else
			{
				var << "";
			}

			s = p+1;
			p = strchr(s, spliter);
		}

		if (*s)
		{
			strTemp.ClearData();
			strTemp.AppendString(s);
			var << strTemp.c_str();
		}
	}

	static void SplitWidestr(IVarList& var, const wchar_t* ws, const wchar_t* wspliter)
	{
		unsigned int spliter_len = (unsigned int)wcslen(wspliter);
		TAutoStr<wchar_t, 128> strTemp;
		const wchar_t* s = ws;
		const wchar_t* p = wcsstr(s, wspliter);
		while (p)
		{
			strTemp.ClearData();

			unsigned int len = unsigned int(p-s);
			if (len > 0)
			{
				strTemp.AppendString(s, len);
				var << strTemp.c_str();
			}
			else
			{
				var << L"";
			}

			s = p+spliter_len;
			p = wcsstr(s, wspliter);
		}

		if (*s)
		{
			strTemp.ClearData();
			strTemp.AppendString(s);
			var << strTemp.c_str();
		}
	}

	static void SplitWidestr(IVarList& var, const wchar_t* ws, const wchar_t wspliter = L',')
	{
		TAutoStr<wchar_t, 128> strTemp;
		const wchar_t* s = ws;
		const wchar_t* p = wcschr(s, wspliter);
		while (p)
		{
			strTemp.ClearData();

			unsigned int len = unsigned int(p-s);
			if (len > 0)
			{
				strTemp.AppendString(s, len);
				var << strTemp.c_str();
			}
			else
			{
				var << L"";
			}

			s = p+1;
			p = wcschr(s, wspliter);
		}

		if (*s)
		{
			strTemp.ClearData();
			strTemp.AppendString(s);
			var << strTemp.c_str();
		}
	}
};

#endif // __AUTOSTR_H__