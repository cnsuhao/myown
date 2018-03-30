//--------------------------------------------------------------------
// 文件名:		Inlines.h
// 内  容:		常用内联函数
// 说  明:		
// 创建日期:	2007年2月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_INLINES_H
#define _PUBLIC_INLINES_H

#include "Macros.h"
#include <string.h>
#include <wchar.h>
#include <float.h>

#include "../server/LoopCheck.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/WinString.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/LinuxString.h"
#endif // FX_SYSTEM_LINUX

#define SafeSprintf Port_SafeSprintf
#define SafeSprintList Port_SafeSprintList
#define SafeSwprintf Port_SafeSwprintf
#define SafeSwprintList Port_SafeSwprintList


// 安全格式化字符串
#define SPRINTF_S(buf, str, ...) SafeSprintf(buf, sizeof(buf), str, __VA_ARGS__)

#define SWPRINTF_S(buf, str, ...) SafeSwprintf(buf, sizeof(buf), str, __VA_ARGS__)


// 取哈希值，忽略大小写
inline unsigned int GetHashValue(const char* name)
{
	static unsigned char convert_to_lower[256] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
		0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
		0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
		0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
		0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
		0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,	
	};
	
	Assert(name != NULL);

	unsigned int h = 0;
	
	for (; *name; name++)
	{
		h = h * 131 + convert_to_lower[(unsigned char)(*name)];
	}
	
	return h;
}

// 取哈希值
inline unsigned int GetHashValueCase(const char* name)
{
	Assert(name != NULL);

	unsigned int h = 0;
	
	for (; *name; name++)
	{
		h = h * 131 + *name;
	}
	
	return h;
}

// 取哈希值，忽略大小写
inline unsigned int GetHashValueW(const wchar_t* name)
{
	static unsigned short convert_to_lower[256] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
		0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
		0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
		0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
		0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
		0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,	
	};
	
	Assert(name != NULL);

	unsigned int h = 0;
	
	for (; *name; name++)
	{
		unsigned int val = (unsigned int)(*name);
		
		if (val < 256)
		{
			h = h * 131 + convert_to_lower[val];
		}
		else
		{
			h = h * 131 + val;
		}
	}
	
	return h;
}

// 取哈希值
inline unsigned int GetHashValueCaseW(const wchar_t* name)
{
	Assert(name != NULL);

	unsigned int h = 0;
	
	for (; *name; name++)
	{
		h = h * 131 + *name;
	}
	
	return h;
}

// 字符串是否为空
inline bool StringEmpty(const char* s)
{
	return (s[0] == 0);
}

// 宽字符串是否为空
inline bool WideStrEmpty(const wchar_t* s)
{
	return (s[0] == 0);
}

// 拷贝字符串
inline void CopyString(char* buf, size_t byte_size, const char* str)
{
	const size_t SIZE1 = strlen(str) + 1;

	if (SIZE1 <= byte_size)
	{
		memcpy(buf, str, SIZE1);
	}
	else
	{
		memcpy(buf, str, byte_size - 1);
		buf[byte_size - 1] = 0;
	}
}

// 拷贝宽字符串
inline void CopyWideStr(wchar_t* buf, size_t byte_size, const wchar_t* str)
{
	const size_t SIZE1 = (wcslen(str) + 1) * sizeof(wchar_t);

	if (SIZE1 <= byte_size)
	{
		memcpy(buf, str, SIZE1);
	}
	else
	{
		const size_t SIZE2 = byte_size / sizeof(wchar_t) - 1;
		memcpy(buf, str, SIZE2 * sizeof(wchar_t));
		buf[SIZE2] = 0;
	}
}

// 消除空格
inline size_t TrimStringLen(const char* str, size_t len, char* buf, 
	size_t maxlen)
{
	const char* beg = str;
	const char* end = str + len - 1;
	
	for (; beg <= end; beg++)
	{
		if ((*beg != ' ') && (*beg != '\t'))
		{
			break;
		}
	}
	
	for (; end >= beg; end--)
	{
		if ((*end != ' ') && (*end != '\t'))
		{
			break;
		}
	}
	
	size_t size = end - beg + 1;
	
	if (size >= maxlen)
	{
		size = maxlen - 1;
	}
	
	memcpy(buf, beg, size);
	
	buf[size] = 0;
	
	return size;
}

// 消除空格
inline size_t TrimString(const char* str, char* buf, size_t maxlen)
{
	return TrimStringLen(str, strlen(str), buf, maxlen);
}

// 消除空格
inline size_t TrimWideStrLen(const wchar_t* str, size_t len, wchar_t* buf, 
	size_t maxlen)
{
	const wchar_t* beg = str;
	const wchar_t* end = str + len - 1;
	
	for (; beg <= end; beg++)
	{
		if ((*beg != L' ') && (*beg != L'\t'))
		{
			break;
		}
	}
	
	for (; end >= beg; end--)
	{
		if ((*end != L' ') && (*end != L'\t'))
		{
			break;
		}
	}
	
	size_t size = end - beg + 1;
	
	const size_t MAX_LEN = maxlen / sizeof(wchar_t);
	
	if (size >= MAX_LEN)
	{
		size = MAX_LEN - 1;
	}
	
	memcpy(buf, beg, size * sizeof(wchar_t));
	
	buf[size] = 0;
	
	return size;
}

// 消除空格
inline size_t TrimWideStr(const wchar_t* str, wchar_t* buf, size_t maxlen)
{
	return TrimWideStrLen(str, wcslen(str), buf, maxlen);
}

// 单精度浮点数相等
inline bool FloatEqual(float v1, float v2)
{
	return (v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON));
}

// 双精度浮点数相等
inline bool DoubleEqual(double v1, double v2)
{
	return (v1 <= (v2 + DBL_EPSILON)) && (v1 >= (v2 - DBL_EPSILON));
}

// 交换变量
template<typename TYPE>
void SwapValue(TYPE& v1, TYPE& v2)
{
	TYPE t(v1);

	v1 = v2;
	v2 = t;
}

#endif // _PUBLIC_INLINES_H
