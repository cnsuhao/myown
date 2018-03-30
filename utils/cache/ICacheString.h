//--------------------------------------------------------------------
// 文件名:		ICacheString.h
// 内  容:		cache string,buffer,常用字符串函数，安全使用
// 说  明:		
// 创建日期:	2017年06月02日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#ifndef __I_CACHE_STRING_H__
#define	__I_CACHE_STRING_H__

//namespace cache{

// interface: cache string.
struct ICacheString
{
public:
	~ICacheString(){}
public:
	/// \brief 返回字符串内容
	virtual const char*  GetString() const = 0;
	virtual char*  GetBuffer() = 0;
public:
	/// \brief 字符串赋值
	/// \param str 字符串
	virtual bool SetString( const char* str ) = 0;
	/// \brief 字符串赋值
	/// \param ref 字符串
	virtual bool SetStringEx(const ICacheString& ref) = 0;
	/// \brief 字符串赋值,格式化
	/// \param format 格式化符号
	virtual bool SetFormat(const char* format, ...) = 0;
public:
	/// \brief 字符追加
	/// \param str 字符串
	virtual bool AppendString(const char* str) = 0;
	/// \brief 字符追加
	/// \param p_buff 字符串
	virtual bool AppendStringEx(const ICacheString& p_buff) = 0;
	/// \brief 字符追加,格式化
	/// \param format 格式化符号
	virtual bool AppendFormat(const char* format, ...) = 0;
	/// \brief 字符追加 回车符
	virtual bool AppendLF() = 0;
public:
	/// \brief 字符分割字符串
	/// \param str 字符串
	/// \param split_pos 分割符
	virtual bool AppendSplitString(const char* split_pos,const char* str) = 0;
	/// \brief 字符分割字符串
	/// \param str 字符串
	/// \param split_pos 分割符
	virtual bool AppendSplitFormat(const char* split_pos,const char* format, ...) = 0;
public:
	/// \brief 比较，字符串是否相同
	/// \param str 字符串
	virtual bool IsEqual( const char* str ) const = 0;
public:
	/// \brief 字符串转换 大写
	virtual void toUpper() = 0;
	/// \brief 字符串转换 小写
	virtual void toLower() = 0;
	/// \brief 字符串转换 反转
	virtual bool toReversal() = 0;
public:
	/// \brief 字符串是否为空
	virtual bool IsEmpty() const = 0;
	/// \brief 字符串是否为空
	/// \brief 字符串长度
	virtual unsigned int GetSize() const = 0;
	/// \brief 字符串容量
	virtual unsigned int GetCapacity() const = 0;
public:
	/// \brief 字符串清空
	virtual void Clear() = 0;
};

// interface: cache buff.
struct ICacheBuff
{
public:
	~ICacheBuff(){}
public:
	/// \brief 设置缓冲区
	/// \param p_buff 字符串
	/// \param buff_len 长度
	virtual bool SetBuff(const char* p_buff,unsigned int buff_len) = 0;
	/// \brief 追加缓冲
	/// \param p_buff 字符串
	/// \param buff_len 长度
	virtual bool AppendBuff(const char* p_buff, unsigned int buff_len) = 0;
public:
	/// \brief 缓冲区清空
	virtual void Clear() = 0;
	/// \brief 是否为空
	virtual bool IsEmpty() const = 0;
public:
	/// \brief 返回缓冲区
	virtual const char*  GetBuff() const = 0;
	// \brief 返回缓冲区长度
	virtual unsigned int GetBuffSize() const = 0;
	// \brief 返回缓冲区容量
	virtual unsigned int GetBufftCapacity() const = 0;
public:
	/// \brief 返回缓冲区,可读写
	virtual char*  GetBuffRaw() = 0;
	// \brief 返回缓冲区长度,可读写
	virtual unsigned int& GetBuffRawLen() = 0;
};

//}
#endif