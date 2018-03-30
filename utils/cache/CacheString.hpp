//--------------------------------------------------------------------
// 文件名:		CacheString.h
// 内  容:		cache string,buffer,常用字符串函数，安全使用
// 说  明:		
// 创建日期:	2017年06月02日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#ifndef __CACHE_STRING_H__
#define	__CACHE_STRING_H__
#include "ICacheString.h"
#include <cassert>
#include <stdarg.h>
#include <stdio.h>
//namespace cache{

// string empty
static const char	   C_CACHE_STRIMG_EMPTY[] = "";

// interface: cache string.
template< unsigned int buff_capacity_ = 1024 >
struct CacheString : public	ICacheString
{
protected:
	unsigned int buff_size_;
	char  buffer_[buff_capacity_];
public:
	CacheString() :buff_size_(0)
	{
		if (buff_capacity_ > 1)
		{
			buff_size_ = buff_capacity_ - 1;
		}
		memset(buffer_, 0, buff_capacity_);
	}
	~CacheString(){}
public:
	/// \brief 返回字符串内容
	virtual const char*  GetString() const { return buffer_; }
	virtual char*  GetBuffer(){ return buffer_; }
public:
	/// \brief 字符串赋值
	/// \param str 字符串
	virtual bool SetString(const char* str)
	{
		unsigned int copy_len = 0;
		if (buff_size_ <= 0 || str == NULL)
		{
			assert(0);
			return false;
		}
		copy_len = (unsigned int)strlen(str);
		if (copy_len <= 0)
		{
			memset(buffer_, 0, 1);
			return true;
		}
		if (copy_len >= buff_size_)
		{
			assert(0);
			copy_len = buff_size_;
			memcpy(buffer_, str, copy_len);
			return false;
		}
		memcpy(buffer_, str, copy_len);
		memset(buffer_ + copy_len, 0, 1);
		return true;
	}
	/// \brief 字符串赋值
	/// \param ref 字符串
	virtual bool SetStringEx(const ICacheString& ref)
	{
		if (ref.IsEmpty())
		{
			return true;
		}
		return SetString(ref.GetString()); 
	}
	/// \brief 字符串赋值,格式化
	/// \param format 格式化符号
	virtual bool SetFormat(const char* format, ...)
	{
		if (format == NULL || buff_size_ <= 0)
		{
			assert(0);
			return false;
		}
		va_list args;
		va_start(args, format);
#	if ( defined _WIN32 || defined _WIN64 )
		_vsnprintf_s(buffer_, buff_size_, buff_size_ - 1, format, args);
# else
		vsnprintf(buffer_, buff_size_, format, args);
#	endif
		va_end(args);
		memset(buffer_ + GetSize(), 0, 1);
		return true;
	}
public:
	/// \brief 字符追加
	/// \param str 字符串
	virtual bool AppendString(const char* str)
	{
		unsigned int copy_len = 0;
		unsigned int old_len = GetSize();
		if (buff_size_ <= 0 || str == NULL)

		{
			assert(0);
			return false;
		}
		if (old_len >= buff_size_)
		{
			assert(0);
			return false;
		}
		copy_len = (unsigned int)strlen(str);
		if (copy_len <= 0)
		{
			return false;
		}
		if (old_len + copy_len >= buff_size_)
		{
			assert(0);
			copy_len = buff_size_ - old_len;
			memcpy(buffer_ + old_len, str, copy_len);
			return false;
		}
		memcpy(buffer_ + old_len, str, copy_len);
		memset(buffer_ + old_len + copy_len, 0, 1);
		return true;
	}
	/// \brief 字符追加
	/// \param p_buff 字符串
	virtual bool AppendStringEx(const ICacheString& p_buff)
	{
		if (p_buff.IsEmpty())
		{
			return true;
		}
		return AppendString(p_buff.GetString());
	}
	/// \brief 字符追加,格式化
	/// \param format 格式化符号
	virtual bool AppendFormat(const char* format, ...)
	{
		unsigned int old_len = GetSize();
		if (format == NULL || buff_size_ <= 0)
		{
			assert(0);
			return false;
		}
		if (old_len >= buff_size_)
		{
			assert(0);
			return false;
		}
		unsigned int copy_len = buff_size_ - old_len;
		char* p_buffer = buffer_ + old_len;
		va_list args;
		va_start(args, format);
#	if ( defined _WIN32 || defined _WIN64 )
		_vsnprintf_s(p_buffer, copy_len, copy_len-1, format, args);
# else
		vsnprintf(p_buffer, copy_len, format, args);
#	endif
		va_end(args);
		memset(buffer_ + GetSize(), 0, 1);
		return true;
	}
	/// \brief 字符追加 回车符
	virtual bool AppendLF()
	{
		if (buff_size_ <= 0)
		{
			assert(0);
			return false;
		}
		unsigned int old_len = GetSize();
		if (old_len >= buff_size_)
		{
			return false;
		}
		memset(buffer_ + old_len, '\n', 1);
		memset(buffer_ + old_len + 1, 0, 1);
		return true;
	}
public:
	/// \brief 字符分割字符串
	/// \param str 字符串
	/// \param split_pos 分割符
	virtual bool AppendSplitString(const char* split_pos, const char* str)
	{
		if (split_pos == NULL || str == NULL)
		{
			assert(0);
			return false;
		}
		if (!IsEmpty())
		{
			AppendString(split_pos);
		}
		return AppendString(str);
	}
	/// \brief 字符分割字符串
	/// \param str 字符串
	/// \param split_pos 分割符
	virtual bool AppendSplitFormat(const char* split_pos, const char* format, ...)
	{
		if (split_pos == NULL || format == NULL)
		{
			assert(0);
			return false;
		}
		if (!IsEmpty())
		{
			AppendString(split_pos);
		}
		unsigned int old_len = GetSize();
		if (buff_size_ <= 0)
		{
			assert(0);
			return false;
		}
		if (old_len >= buff_size_)
		{
			assert(0);
			return false;
		}
		unsigned int copy_len = buff_size_ - old_len;
		char* p_buffer = buffer_ + old_len;
		va_list args;
		va_start(args, format);
#	if ( defined _WIN32 || defined _WIN64 )
		_vsnprintf_s(p_buffer, copy_len, copy_len - 1, format, args);
# else
		vsnprintf(p_buffer, copy_len, format, args);
#	endif
		va_end(args);
		memset(buffer_ + GetSize(), 0, 1);
		return true;
	}
public:
	/// \brief 比较，字符串是否相同
	/// \param str 字符串
	virtual bool IsEqual(const char* str) const
	{
		if (str == NULL)
		{
			return false;
		}
		return (strcmp(buffer_, str) == 0) ? true : false;
	}
public:
	/// \brief 字符串转换 大写
	virtual void toUpper()
	{
		unsigned int n_size = GetSize();
		if (n_size > 0)
		{
			for (unsigned int index = 0; index < n_size; index++)
			{
				buffer_[index] = toupper(buffer_[index]);
			}
		}
	}
	/// \brief 字符串转换 小写
	virtual void toLower()
	{
		unsigned int n_size = GetSize();
		if (n_size >0)
		{
			for (unsigned int index = 0; index < n_size; index++)
			{
				buffer_[index] = tolower(buffer_[index]);
			}
		}
	}
	/// \brief 字符串转换 反转
	virtual bool toReversal()
	{
		unsigned int len = GetSize();
		if (len <= 0)
		{
			return false;
		}
		char    *start = buffer_;
		char    *end = buffer_ + len - 1;
		char    ch;
		while (start < end)
		{
			ch = *start;
			*start++ = *end;
			*end-- = ch;
		}
		return true;
	}
public:
	/// \brief 字符串是否为空
	virtual bool IsEmpty() const { return (strcmp(buffer_, C_CACHE_STRIMG_EMPTY) == 0) ? true : false; }
	/// \brief 字符串长度
	virtual unsigned int GetSize() const { return (unsigned int)strlen(buffer_); }
	/// \brief 字符串容量
	virtual unsigned int GetCapacity() const { return buff_size_; }
public:
	/// \brief 字符串清空
	virtual void Clear() { if (!IsEmpty()){ memset(buffer_, 0, buff_capacity_); } }
};


// normal cache buff
template< unsigned int buff_capacity_ = 1024 >
struct CacheBuff : public	ICacheBuff
{
private:
	unsigned int real_size_;
	unsigned int buff_size_;
	char  buffer_[buff_capacity_];   // buffer
public:
	CacheBuff() :real_size_(0)
	{
		if (buff_capacity_ > 1)
		{
			buff_size_ = buff_capacity_ - 1;
		}
		memset(buffer_, 0, buff_capacity_);
	}
	~CacheBuff(){}
public:
	/// \brief 设置缓冲区
	/// \param p_buff 字符串
	/// \param buff_len 长度
	virtual bool SetBuff(const char* p_buff, unsigned int buff_len)
	{
		if (real_size_ > 0)
		{
			Clear();
		}
		return AppendBuff(p_buff, buff_len);
	}
	/// \brief 追加缓冲
	/// \param p_buff 字符串
	/// \param buff_len 长度
	virtual bool AppendBuff(const char* p_buff, unsigned int buff_len)
	{
		if (buff_len + real_size_ > buff_size_)
		{
			assert(0);
			unsigned int copy_len = 0;
			if (real_size_<buff_size_)
			{
				copy_len = buff_size_ - real_size_;
			}
			if (copy_len >0)
			{
				memcpy(buffer_ + real_size_, p_buff, copy_len);
				real_size_ = buff_size_;
			}
			return false;
		}
		memcpy(buffer_ + real_size_, p_buff, buff_len);
		real_size_ += buff_len;
		return true;
	}
public:
	/// \brief 缓冲区清空
	virtual void Clear()
	{
		real_size_ = 0;
		memset(buffer_, 0, buff_capacity_);
	}
	/// \brief 是否为空
	virtual bool IsEmpty() const
	{
		if (real_size_ <= 0)
		{
			return true;
		}
		return false;
	}
public:
	/// \brief 返回缓冲区
	virtual const char*  GetBuff() const { return buffer_; }
	// \brief 返回缓冲区长度
	virtual unsigned int GetBuffSize() const { return real_size_; }
	// \brief 返回缓冲区容量
	virtual unsigned int GetBufftCapacity() const { return buff_size_; }
public:
	/// \brief 返回缓冲区,可读写
	virtual char*  GetBuffRaw() { return buffer_; }
	// \brief 返回缓冲区长度,可读写
	virtual unsigned int& GetBuffRawLen()
	{
		real_size_ = buff_size_;
		return real_size_;
	}
};

//}
#endif