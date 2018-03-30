//--------------------------------------------------------------------
// 文件名:		Code\FxGameLogic\utils\StringUtil.cpp
// 内  容:		字符串操作类
// 说  明:		
// 创建日期:	
// 创建人:		
//    :	   
//--------------------------------------------------------------------

#include "string_util.h"
#include "public/IVarList.h"
#include "public/Converts.h"
#include "public/AutoMem.h"
#include "public/Inlines.h"
#include "public/VarList.h"


//字符串是否为空
bool StringUtil::CharIsNull(const char* strChar)
{
    if (!strChar || strChar[0] == '\0')
    {
        return true;
    }

    return false;
}
bool StringUtil::CharIsNull(const wchar_t* strChar)
{
	if (!strChar || strChar[0] == L'\0')
	{
		return true;
	}

	return false;
}

//物品是是空
bool StringUtil::ItemIsNull(const char* strConfig)
{
	if (!strConfig || strConfig[0] == '\0' || stricmp(strConfig, "0") == 0)
	{
		return true;
	}

	return false;
}
bool StringUtil::CharIsEqual(const char* srcChar, const char* dstChar)
{
	if (srcChar == NULL || dstChar == NULL)
	{
		return false;
	}
	if (strcmp(srcChar, dstChar) == 0)
	{
		return true;
	}
	return false;
}
bool StringUtil::CharIsEqual(const wchar_t* srcChar, const wchar_t* dstChar)
{
	if (srcChar == NULL || dstChar == NULL)
	{
		return false;
	}
	if (wcscmp(srcChar, dstChar) == 0)
	{
		return true;
	}
	return false;
}
// 转换到宽字符串
std::wstring StringUtil::StringAsWideStr(const char* s)
{
	size_t size = ::GetToWideStrLen(s);

	TAutoMem<wchar_t, 256> auto_buf(size);

	wchar_t* buffer = auto_buf.GetBuffer();

	::ToWideStr(s, buffer, size * sizeof(wchar_t));

	return std::wstring(buffer);
}

// 字符串转换成utf8
std::string StringUtil::StringAsUtf8String( const char* s )
{
	std::wstring wideStr = StringAsWideStr(s);
	return WideStrAsUTF8String(wideStr.c_str());
}

// utf8转字符串
std::string StringUtil::Utf8StringAsString(const char* s)
{
	std::wstring wideStr = UTF8StringAsWideStr(s);
	return WideStrAsString(wideStr.c_str());
}

// 转换到字符串
std::string StringUtil::WideStrAsString(const wchar_t* ws)
{
	size_t size = ::GetToStringLen(ws);

	TAutoMem<char, 256> auto_buf(size);

	char* buffer = auto_buf.GetBuffer();

	::ToString(ws, buffer, size);

	return std::string(buffer);
}
// 获得宽字符串转换成UTF8字符串
std::string StringUtil::WideStrAsUTF8String(const wchar_t* ws)
{
	size_t size = Port_GetWideStrToUTF8Len(ws);
	TAutoMem<char, 256> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();
	Port_WideStrToUTF8(ws, buffer, size);
	return std::string(buffer);
}
// 获得UTF8字符串换成宽字符串转
std::wstring StringUtil::UTF8StringAsWideStr(const char* s)
{
	size_t size = Port_GetUTF8ToWideStrLen(s);
	TAutoMem<wchar_t, 256> auto_buf(size);
	wchar_t* buffer = auto_buf.GetBuffer();
	Port_UTF8ToWideStr(s, buffer, size * sizeof(wchar_t));
	return std::wstring(buffer);
}

// 把一个字符串转换成const char*,如果其串为空则返回def_value
const char* StringUtil::ParseStrAsStr(const char* str, const char* def_value)
{
	if (NULL == str)
	{
		return def_value;
	}

	return str;
}

// 把一个字符串转换成long,如果其串为空则返回def_value
long StringUtil::ParseStrAsLong(const char* str, long def_value)
{
	if (NULL == str)
	{
		return def_value;
	}

	return StringAsInt(str, def_value);
}

// 把一个字符串转换成float,如果其串为空则返回def_value
float StringUtil::ParseStrAsFloat(const char* str, float def_value)
{
	if (NULL == str)
	{
		return def_value;
	}

	return StringAsFloat(str);
}

//把一个字符串转换成bool,如果其串为空则返回def_value
bool StringUtil::ParseStrAsBool(const char* str, bool def_value)
{
	if (NULL == str)
	{
		return def_value;
	}

	return (stricmp(str, "true") == 0);
}


// 整数转换为字符串
std::string StringUtil::IntAsString(int value)
{
	char str[32];

	SPRINTF_S(str, "%d", value);

	return std::string(str);
}

std::string StringUtil::UIntAsString(unsigned int value)
{
    char str[32];

    SPRINTF_S(str, "%u", value);

    return std::string(str);
}

std::string StringUtil::Int64AsString(int64_t value)
{
    char str[32];
    Port_Int64ToString(value, str, sizeof(str));

    return std::string(str);
}

// 浮点数转换为字符串
std::string StringUtil::FloatAsString(float value, int decimals)
{
	char str[64];

	if (0 == decimals)
	{
		SafeSprintf(str, sizeof(str), "%f", (double)value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			SafeSprintf(str, sizeof(str), "%.01f", (double)value);
			break;
		case 2:
			SafeSprintf(str, sizeof(str), "%.02f", (double)value);
			break;
		case 3:
			SafeSprintf(str, sizeof(str), "%.03f", (double)value);
			break;
		case 4:
			SafeSprintf(str, sizeof(str), "%.04f", (double)value);
			break;
		case 5:
			SafeSprintf(str, sizeof(str), "%.05f", (double)value);
			break;
		default:
			SafeSprintf(str, sizeof(str), "%f", (double)value);
			break;
		}
	}

	int len = (int)strlen(str);

	for(int i = len - 1; i >= 0 && str[i]!= '.'; i--)
	{
		if (str[i] == '0')
		{
			str[i] = 0;
		}
		else
		{
			if (str[i] == '.')
			{
				str[i] = 0;
			}

			break;
		}
	}

	return std::string(str);
}

// 双精度数转换成字符串
std::string StringUtil::DoubleAsString(double value, int decimals)
{
    char str[64];

    if (0 == decimals)
    {
        SafeSprintf(str, 64, "%f", value);
    }
    else
    {
        switch (decimals)
        {
        case 1:
            SafeSprintf(str, 64, "%.01f", value);
            break;
        case 2:
            SafeSprintf(str, 64,  "%.02f", value);
            break;
        case 3:
            SafeSprintf(str, 64,  "%.03f", value);
            break;
        case 4:
            SafeSprintf(str, 64,  "%.04f", value);
            break;
        case 5:
            SafeSprintf(str, 64,  "%.05f", value);
            break;
        default:
            SafeSprintf(str, 64,  "%f", value);
            break;
        }
    }

    return std::string(str);
}


// 字符串转换为整数
int StringUtil::StringAsInt(const char* value, int def_value)
{
    if (!value || !*value)
    {
        return def_value;
    }
	return atoi(value);
}

// 字符串转换为64整数
int64_t StringUtil::StringAsInt64(const char* value)
{
	return Port_StringToInt64(value);
}

// 字符串转换为浮点数
float StringUtil::StringAsFloat(const char* value, float def_value)
{
    if (!value || !*value)
    {
        return def_value;
    }
	return (float)atof(value);
}

// 字符串转换为整数
int StringUtil::StringAsInt(const std::string& value, int def_value)	
{
	return StringAsInt(value.c_str(), def_value);
}

// 字符串转换为浮点数
float StringUtil::StringAsFloat(const std::string& value, float def_value)
{
    if (value.empty())
    {
        return def_value;
    }
	return (float)atof(value.c_str());
}

// 整数转换为宽字符串
std::wstring StringUtil::IntAsWideStr(int value)
{
	wchar_t buf[32];

	Port_SafeSwprintf(buf, 32,L"%d", value);

	return std::wstring(buf);
}

// 浮点数转换为宽字符串
std::wstring StringUtil::FloatAsWideStr(float value, int decimals)
{
	wchar_t buf[32];

	if (0 == decimals)
	{
		Port_SafeSwprintf(buf, 32,L"%f", (double)value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			Port_SafeSwprintf(buf, 32,L"%.01f", (double)value);
			break;
		case 2:
			Port_SafeSwprintf(buf, 32,L"%.02f", (double)value);
			break;
		case 3:
			Port_SafeSwprintf(buf, 32,L"%.03f", (double)value);
			break;
		case 4:
			Port_SafeSwprintf(buf, 32,L"%.04f", (double)value);
			break;
		case 5:
			Port_SafeSwprintf(buf, 32,L"%.05f", (double)value);
			break;
		default:
			Port_SafeSwprintf(buf, 32,L"%f", (double)value);
			break;
		}
	}

	return std::wstring(buf);
}

// 宽字符串转换为整数
int StringUtil::WideStrAsInt(const wchar_t* value)
{
	char buf[32];

	ToString(value, buf, sizeof(buf));

	return atoi(buf);
}

// 宽字符串转换为整数
int64_t StringUtil::WideStrAsInt64(const wchar_t* value)
{
	return Port_WideStrToInt64(value);
}


// 宽字符串转换为浮点数
float StringUtil::WideStrAsFloat(const wchar_t* value)
{
	char buf[32];

	ToString(value, buf, sizeof(buf));

	return (float)atof(buf);
}

// 宽字符串转换为整数
int StringUtil::WideStrAsInt(const std::wstring& value)
{
	return WideStrAsInt(value.c_str());
}

// 宽字符串转换为浮点数
float StringUtil::WideStrAsFloat(const std::wstring& value)
{
	return WideStrAsFloat(value.c_str());
}

// 转换为大写的字符串
std::string StringUtil::ToUpper(const char* str)
{
	TAutoMem<char, 256> auto_mem(strlen(str) + 1);
	char* data = auto_mem.GetBuffer();

	strcpy(data, str);

	return Port_StringUpper(data);
}

// 转换为小写的字符串
std::string StringUtil::ToLower(const char* str)
{
	//Assert(str != NULL);

	TAutoMem<char, 256> auto_mem(strlen(str) + 1);

	char* data = auto_mem.GetBuffer();

	strcpy(data, str);

	return Port_StringLower(data);
}

// 取子串，pos从0开始
std::string StringUtil::Substr(const char* str, size_t pos, size_t len)
{
	Assert(str != NULL);

	if (len == 0)
	{
		return std::string();
	}

	size_t i = pos + len;

	if (i > strlen(str))
	{
		return std::string();
	}

	std::string s(str);

	return s.substr(pos, len);
}

// 取用指定字符串分隔的子串分解到Vector里
size_t StringUtil::ParseToVector(const char* str, char splitter, IVarList& result)
{
	result.Clear();

	if (str[0] == 0)
	{
		return 0;
	}

	size_t size = strlen(str) + 1;

	TAutoMem<char, 256> auto_buf(size);

	char* buffer = auto_buf.GetBuffer();

	memcpy(buffer, str, size);

	const char* res = buffer;

	char* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result.AddString(res);

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result.AddString(res);
	}

	return result.GetCount();
}

// 取用指定的字符串分隔的字串分解到Map里<splitter1为第一次分隔，splitter2为第一次分隔的子串再次分隔符)
size_t StringUtil::ParseToMap(const char* str, char splitter1, char splitter2, std::map<std::string, std::string>& result)
{
	result.clear();
	CVarList arr;
	if ( ParseToVector(str, splitter1, arr) > 0 )
	{
		for ( size_t i = 0 ; i < arr.GetCount(); ++i )
		{
			const char* pkv = arr.StringVal(i);
			if ( pkv[0] == '\0') continue;

			CVarList kvList;
			if ( ParseToVector(pkv, splitter2, kvList) < 2 ) continue;

			const char* k = kvList.StringVal(0);
			const char* v = kvList.StringVal(1);
			if ( k[0] == 0 ) continue;

			result.insert( std::make_pair(k, v) ); 
		}
	}
	return result.size();
}


// 宽串版本，取用指定字符串分隔的子串分解到Vector里
size_t StringUtil::ParseToVectorW(const wchar_t* str, wchar_t splitter, IVarList& result)
{
	result.Clear();

	if (str[0] == 0)
	{
		return 0;
	}

	size_t size = wcslen(str) + 1;

	TAutoMem<wchar_t, 256> auto_buf(size);

	wchar_t* buffer = auto_buf.GetBuffer();

	memcpy(buffer, str, size * sizeof(wchar_t));

	const wchar_t* res = buffer;

	wchar_t* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result.AddWideStr(res);

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result.AddWideStr(res);
	}

	return result.GetCount();
}

// 把str字符串里的pat都换成replace
void StringUtil::StrReplace(std::string& str, const std::string& pat, const std::string& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);

		if (pos == std::wstring::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);

		pos += replace.size();
	}
}

// 去除str字符串中前后的指定字符
std::string StringUtil::Trim(const std::string& str, const char ch)
{
	if (str.empty())
	{
		return "";
	}

	if (str[0] != ch && str[str.size()-1] != ch)
	{
		return str;
	}

	size_t pos_begin = str.find_first_not_of(ch, 0);
	size_t pos_end = str.find_last_not_of(ch, str.size());

	if (pos_begin == std::string::npos || pos_end == std::string::npos)
	{
		//这表示这个字符串里的字符全都是ch组成，把它设置为空串
		return "";
	}

	return str.substr(pos_begin, pos_end - pos_begin + 1);
}

// 用指定的字符分割字符串
size_t StringUtil::SplitString(const char* str, char splitter, char* buffer, 
							   size_t buffer_size, const char* result[], size_t result_max)
{
	Assert(str != NULL);
	Assert(buffer != NULL);

	if (str[0] == 0)
	{
		return 0;
	}

	CopyString(buffer, buffer_size, str);

	size_t count = 0;
	const char* res = buffer;

	char* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result[count++] = res;

			if (count == result_max)
			{
				return count;
			}

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result[count++] = res;
	}

	return count;
}

void StringUtil::SplitString(IVarList & result, const std::string& str,
                        const std::string& delims, unsigned int max_splits)
{
    if (0 == str.compare(""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find(delims, start);
        if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << Trim(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << Trim(str.substr(start, pos - start)).c_str();
            start = pos + delims.size();
        }

        num_splits++;

    } while (pos != std::string::npos);
    return;
}

void StringUtil::SplitStringOf(IVarList & result, const std::string& str,
                        const std::string& delims, unsigned int max_splits)
{
    if (0 == str.compare(""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find_first_of(delims, start);
        if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << Trim(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << Trim(str.substr(start, pos - start)).c_str();
            start = pos + 1;
        }

        num_splits++;

    } while (pos != std::string::npos);
    return;
}

// 宽字符版本的函数，同上
std::wstring StringUtil::ToUpperW(const wchar_t* str)
{
	Assert(str != NULL);

	TAutoMem<wchar_t, 256> auto_mem(wcslen(str) + 1);

	wchar_t* data = auto_mem.GetBuffer();

	wcscpy(data, str);

	return Port_WideStrUpper(data);
}

std::wstring StringUtil::ToLowerW(const wchar_t* str)
{
	Assert(str != NULL);

	TAutoMem<wchar_t, 256> auto_mem(wcslen(str) + 1);

	wchar_t* data = auto_mem.GetBuffer();

	wcscpy(data, str);

	return Port_WideStrLower(data);
}

std::wstring StringUtil::SubstrW(const wchar_t* str, size_t pos, size_t len)
{
	Assert(str != NULL);

	if (len == 0)
	{
		return std::wstring();
	}

	size_t i = pos + len;

	if (i > wcslen(str))
	{
		return std::wstring();
	}

	std::wstring s(str);

	return s.substr(pos, len);
}

void StringUtil::StrReplaceW(std::wstring& str, const std::wstring& pat, const std::wstring& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);

		if (pos == std::wstring::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);

		pos += replace.size();
	}
}

// 去除str字符串中前后的指定字符
std::wstring StringUtil::TrimW(const std::wstring& str, const wchar_t ch)
{
	if (str.empty())
	{
		return L"";
	}

	if (str[0] != ch && str[str.size() - 1] != ch)
	{
		return str;
	}

	size_t pos_begin = str.find_first_not_of(ch, 0);
	size_t pos_end = str.find_last_not_of(ch, str.size());

	if (pos_begin == std::wstring::npos || pos_end == std::wstring::npos)
	{
		//这表示这个字符串里的字符全都是ch组成，把它设置为空串
		return L"";
	}

	return str.substr(pos_begin, pos_end - pos_begin);
}

// 用指定的字符分割宽字符串
size_t StringUtil::SplitStringW(const wchar_t* str, wchar_t splitter, 
								wchar_t* buffer, size_t buffer_size, const wchar_t* result[], size_t result_max)
{
	Assert(str != NULL);
	Assert(buffer != NULL);

	if (str[0] == 0)
	{
		return 0;
	}

	CopyWideStr(buffer, buffer_size, str);

	size_t count = 0;
	const wchar_t* res = buffer;

	wchar_t* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result[count++] = res;

			if (count == result_max)
			{
				return count;
			}

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result[count++] = res;
	}

	return count;
}

// 分割字符串
void StringUtil::SplitStringW(IVarList & result, const std::wstring& str,
                        const std::wstring& delims, unsigned int max_splits)
{
    if (0 == str.compare(L""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find(delims, start);
        if (pos == start)
        {
            start = pos + delims.size();
        }
        else if (pos == std::wstring::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << TrimW(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << TrimW(str.substr(start, pos - start)).c_str();
            start = pos + delims.size();
        }

        num_splits++;

    } while (pos != std::wstring::npos);
    return;
}


// 分割字符串
void StringUtil::SplitStringOfW(IVarList & result, const std::wstring& str,
                        const std::wstring& delims, unsigned int max_splits)
{
    if (0 == str.compare(L""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find_first_of(delims, start);
        if (pos == start)
        {
            start = pos + 1;
        }
        else if (pos == std::wstring::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << TrimW(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << TrimW(str.substr(start, pos - start)).c_str();
            start = pos + 1;
        }

        num_splits++;

    } while (pos != std::wstring::npos);
    return;
}

std::string StringUtil::ConvertUtf8String(const IVarList& data, size_t nIndex)
{
	try
	{
		char szBuf[32] = { 0 };
		switch (data.GetType(nIndex))
		{
		case VTYPE_BOOL:
			sprintf(szBuf, "%d", data.BoolVal(nIndex) ? 1 : 0);
			break;
		case VTYPE_INT:
			sprintf(szBuf, "%d", data.IntVal(nIndex));
			break;
		case VTYPE_INT64:
			sprintf(szBuf, "%lld", data.Int64Val(nIndex));
			break;
		case VTYPE_FLOAT:
			sprintf(szBuf, "%f", data.FloatVal(nIndex));
			break;
		case VTYPE_DOUBLE:
			sprintf(szBuf, "%llf", data.DoubleVal(nIndex));
			break;
		case VTYPE_STRING:
			return StringUtil::StringAsUtf8String(data.StringVal(nIndex));
			break;
		case VTYPE_WIDESTR:
			return StringUtil::WideStrAsUTF8String( data.WideStrVal(nIndex) );
			break;
		case VTYPE_OBJECT:
			sprintf(szBuf, "%lld", data.ObjectVal(nIndex).nData64);
			break;
		default:
			break;
		}
		return std::string(szBuf);
	}
	catch (...)
	{
	}
	return std::string();
}

bool StringUtil::StringIsAlnum(const char* pszStr)
{
	size_t nLen = strlen(pszStr);
	for (size_t i = 0; i < nLen; ++i)
	{
		if (!isalnum(pszStr[i]))
		{
			return false;
		}
	}

	return true;
}
