//--------------------------------------------------------------------
// 文件名:		LinuxString.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUXSTRING_H
#define _SYSTEM_LINUXSTRING_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>

// 字符串转换为64位整数
inline int64_t Port_StringToInt64(const char* str)
{
	return atoll(str);
}

// 64位整数转换为字符串
inline void Port_Int64ToString(int64_t val, char* buf, size_t byte_size)
{
	snprintf(buf, byte_size - 1, "%lld", val);
	buf[byte_size - 1] = 0;
}

// 宽字符串转换为64位整数
inline int64_t Port_WideStrToInt64(const wchar_t* str)
{
	return wcstoll(str, NULL, 10);
}

// 64位整数转换为宽字符串
inline void Port_Int64ToWideStr(int64_t val, wchar_t* buf, size_t byte_size)
{
	size_t len = byte_size / sizeof(wchar_t);
	swprintf(buf, len - 1, L"%lld", val);
	buf[len - 1] = 0;
}

// 字符串转换为大写
inline char* Port_StringUpper(char* str)
{
	char* p = str;
	
	while (*p)
	{
		*p = toupper(*p);
		p++;
	}

	return str;
}

// 字符串转换为小写
inline char* Port_StringLower(char* str)
{
	char* p = str;
	
	while (*p)
	{
		*p = tolower(*p);
		p++;
	}

	return str;
}

// 宽字符串转换为大写
inline wchar_t* Port_WideStrUpper(wchar_t* str)
{
	wchar_t* p = str;
	
	while (*p)
	{
		*p = towupper(*p);
		p++;
	}

	return str;
}

// 宽字符串转换为小写
inline wchar_t* Port_WideStrLower(wchar_t* str)
{
	wchar_t* p = str;
	
	while (*p)
	{
		*p = towlower(*p);
		p++;
	}

	return str;
}

// 组织字符串
inline void Port_SafeSprintf(char* buf, size_t byte_size, const char* info, ...)
{
	va_list args;
	
	va_start(args, info);
	
	vsnprintf(buf, byte_size - 1, info, args);

	buf[byte_size - 1] = 0;

	va_end(args);
}

// 组织字符串
inline void Port_SafeSprintList(char* buf, size_t byte_size, const char* info, 
	va_list args)
{
	vsnprintf(buf, byte_size - 1, info, args);
	
	buf[byte_size - 1] = 0;
}

// 组织宽字符串
inline void Port_SafeSwprintf(wchar_t* buf, size_t byte_size, 
	const wchar_t* info, ...)
{
	size_t size = byte_size / sizeof(wchar_t);
	
	va_list args;
	
	va_start(args, info);
	
	vswprintf(buf, size - 1, info, args);

	buf[size - 1] = 0;

	va_end(args);
}

// 组织宽字符串
inline void Port_SafeSwprintList(wchar_t* buf, size_t byte_size, 
	const wchar_t* info, va_list args)
{
	size_t size = byte_size / sizeof(wchar_t);
	
	vswprintf(buf, size - 1, info, args);
	
	buf[size - 1] = 0;
}

#endif // _SYSTEM_LINUXSTRING_H
