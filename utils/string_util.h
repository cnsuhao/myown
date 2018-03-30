//--------------------------------------------------------------------
// 文件名:		Code\FxGameLogic\utils\StringUtil.h
// 内  容:		字符串操作类
// 说  明:		
// 创建日期:	
// 创建人:		
//    :	   
//--------------------------------------------------------------------
#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include "public/Macros.h"
#include "public/IVarList.h"
#include <string>
#include <map>

// 字符串处理

class StringUtil
{
public:

    //字符串是否为空
    static bool	CharIsNull(const char* strChar);
	static bool	CharIsNull(const wchar_t* strChar);
	//物品是是空
	static bool ItemIsNull(const char* strConfig);

	//比较
	static bool CharIsEqual(const char* srcChar, const char* dstChar);
	static bool CharIsEqual(const wchar_t* srcChar, const wchar_t* dstChar);

	// 字符串转换成宽字符串
	static std::wstring StringAsWideStr(const char* s);
	// 字符串转换成utf8
	static std::string StringAsUtf8String(const char* s);
	// utf8转字符串
	static std::string Utf8StringAsString(const char* s);
	// 获得宽字符串转换成字符串后的长度
	static std::string WideStrAsString(const wchar_t* ws);
    // 获得宽字符串转换成UTF8字符串
    static std::string WideStrAsUTF8String(const wchar_t* ws);
	// 获得UTF8字符串换成宽字符串转
	static std::wstring UTF8StringAsWideStr(const char* s);

	//把一个字符串转换成const char*,如果其串为空则返回def_value
	static const char* ParseStrAsStr(const char* str, const char* def_value);
	//把一个字符串转换成long,如果其串为空则返回def_value
	static long ParseStrAsLong(const char* str, long def_value);
	//把一个字符串转换成float,如果其串为空则返回def_value
	static float ParseStrAsFloat(const char* str, float def_value);
	//把一个字符串转换成bool,如果其串为空则返回def_value
	static bool ParseStrAsBool(const char* str, bool def_value);

	// 整数转换为字符串
	static std::string IntAsString(int value);
    static std::string UIntAsString(unsigned int value);
    static std::string Int64AsString(int64_t value);

	// 浮点数转换为字符串
	static std::string FloatAsString(float value, int decimals = 0);
    // 双精度数转换成字符串
    static std::string DoubleAsString(double value, int decimals = 0);

	// 字符串转换为整数
	static int StringAsInt(const char* value, int def_value = 0);
	// 字符串转换为64整数
	static int64_t StringAsInt64(const char* value);
	// 宽字符串转换为整数
	static int64_t WideStrAsInt64(const wchar_t* value);
	// 字符串转换为浮点数
	static float StringAsFloat(const char* value, float def_value = 0.0f);
	// 字符串转换为整数
	static int StringAsInt(const std::string& value, int def_value = 0);
	// 字符串转换为浮点数
	static float StringAsFloat(const std::string& value, float def_value = 0.0f);
	// 整数转换为宽字符串
	static std::wstring IntAsWideStr(int value);
	// 浮点数转换为宽字符串
	static std::wstring FloatAsWideStr(float value, int decimals = 0);
	// 宽字符串转换为整数
	static int WideStrAsInt(const wchar_t* value);
	// 宽字符串转换为浮点数
	static float WideStrAsFloat(const wchar_t* value);
	// 宽字符串转换为整数
	static int WideStrAsInt(const std::wstring& value);
	// 宽字符串转换为浮点数
	static float WideStrAsFloat(const std::wstring& value);

	// 转换为大写的字符串
	static std::string ToUpper(const char* str);

	// 转换为小写的字符串
	static std::string ToLower(const char* str);

	// 取子串，pos从0开始
	static std::string Substr(const char* str, size_t pos, size_t len);

	//把str字符串里的pat都换成replace
	static void StrReplace(std::string& str,const std::string& pat,const std::string& replace);

	//去除str字符串中前后的指定字符
    static std::string Trim(const std::string& str, const char ch = ' ');

	// 用指定的字符分割字符串
	static size_t SplitString(const char* str, char splitter, char* buffer, 
		size_t buffer_size, const char* result[], size_t result_max);

	// 用指定的字符串整串来分割字符串
    static void SplitString(IVarList & result, const std::string& str,
        const std::string& delims, unsigned int max_splits = 0);

	// 用指定的字符串中的任意字符来分割字符串
    static void SplitStringOf(IVarList & result, const std::string& str,
        const std::string& delims = "/\\", unsigned int max_splits = 0);

	// 取用指定字符串分隔的子串分解到Vector里
	static size_t ParseToVector(const char* str, char splitter, IVarList& result);
	// [add by lihl] 取用指定的字符串分隔的字串分解到Map里<splitter1为第一次分隔，splitter2为第一次分隔的子串再次分隔符)
	static size_t ParseToMap(const char* str, char splitter1, char splitter2, std::map<std::string, std::string>& result);


	// 宽字符版本的函数，同上
	static std::wstring ToUpperW(const wchar_t* str);
	static std::wstring ToLowerW(const wchar_t* str);
	static std::wstring SubstrW(const wchar_t* str, size_t pos, size_t len)	;

	static void StrReplaceW(std::wstring& str,const std::wstring& pat, const std::wstring& replace);

    static std::wstring TrimW(const std::wstring& str, const wchar_t ch = ' ');

	static size_t SplitStringW(const wchar_t* str, wchar_t splitter, 
		wchar_t* buffer, size_t buffer_size, const wchar_t* result[], size_t result_max);

	// 用指定的字符串整串来分割字符串
    static void SplitStringW(IVarList & result, const std::wstring& str,
        const std::wstring& delims, unsigned int max_splits = 0);

	// 用指定的字符串中的任意字符来分割字符串
    static void SplitStringOfW(IVarList & result, const std::wstring& str,
        const std::wstring& delims = L"/\\", unsigned int max_splits = 0);

	static size_t ParseToVectorW(const wchar_t* str, wchar_t splitter, IVarList& result);

	// varlist转成utf8字符串
	static std::string ConvertUtf8String(const IVarList& data, size_t nIndex);

	// 判断字符串是否是字母和数字的组成
	static bool StringIsAlnum(const char* pszStr);
};

#endif// _STRINGUTIL_H
