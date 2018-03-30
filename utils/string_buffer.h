
//--------------------------------------------------------------------
// 文件名:      \utils\string_buffer.h
// 内  容:      
// 说  明:      StringBuffer的作用是申请一块连续的较大内存，用于对加载后
//              不作修改但频繁查询的字符串存储。可以避免使用std::string造成的
//              内存浪费现象。
//              StringArray用于替换CVarList存储字符串使用。StringArray中
//              有一个StringBuffer指针和一个动态数组，当天数组保存字符串在
//              StringBuffer中的偏移位置，当需要访问字符串时，先在数组中找到
//              偏移地址，再交给StringBuffer去获取对应的字符串值。
//
//              经测试，对于配置加载中需要用到std::string和CVarList的情况下，
//              改用StringBuffer和StringArray可以提升70%以上的存储空间。测试
//              数据来自技能配置数据加载的前后对比分析。
// 创建日期:    2014年9月17日
// 创建人:      longch
//    :       
//--------------------------------------------------------------------


#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include "public/VarList.h"

class StringBuffer
{
public:
    static const size_t STRING_BUFFER_INVALID_INDEX;

    StringBuffer();
    ~StringBuffer();

    bool Init(size_t init_size, size_t grow_size);

    size_t GetSize() const
    {
        return m_nSize;
    }
    size_t GetCapacity() const
    {
        return m_nCapacity;
    }
    bool IsInit() const
    {
        return m_bInitialize;
    }
    size_t GetLastStringPos() const
    {
        return m_nLastStringPos;
    }

    const char* Get(size_t offset) const;
    
    size_t Add(const char* ss);

    size_t GetMemoryUsage() const
    {
        size_t size = sizeof(*this) + m_nCapacity;
        return size;
    }

    size_t GetTotalStringLength() const
    {
        return m_nBufferPos;
    }
private:
    char* GetBuffer(size_t offset);

private:
    StringBuffer(const StringBuffer& ss);
    StringBuffer operator=(const StringBuffer& ss);
public:
    bool m_bInitialize;
    size_t m_nCapacity;
    size_t m_nSize;
    size_t m_nGrowSize;

    size_t m_nLastStringPos;
    size_t m_nBufferPos;
    char* m_pBuffer;
}; // end of class StringBuffer


class StringAtom
{
public:
    StringAtom(const char* ss = NULL)
    {
        m_str = ss;
    }

    StringAtom(const StringAtom& ss)
    {
        m_str = ss.m_str;
    }

    size_t length() const
    {
        if (NULL == m_str)
        {
            return 0;
        }
        return strlen(m_str);
    }
    size_t size() const
    {
        return length();
    }
    size_t capacity() const
    {
        return length();
    }
    const char* c_str() const
    {
        return m_str;
    }

private:
    const char* m_str;
};

class StringArray
{
public:
    StringArray();
    StringArray(const StringArray& src);
    StringArray& operator=(const StringArray& src);
    ~StringArray();

    bool Init(size_t init_size, size_t grow_size, StringBuffer* string_buffer);

    size_t GetCount() const
    {
        return m_nCount;
    }
    size_t GetCapacity() const
    {
        return m_nCapacity;
    }
    bool IsInitialize() const
    {
        return m_bInitialize;
    }

    // StringArray与CVarList相互转换
    void AddVarList(const IVarList& value);
    void ConvertToVarList(IVarList& value) const;

    // Add调用前，需要调用Init并返回了true之后，才能保证Add调用是安全的
    const char* Add(const char* value);

    bool Find(const char* value);

    const char* StringVal(size_t index) const;

    StringArray& operator<<(const char* value) 
    { 
        Add(value); 
        return *this; 
    }

    size_t GetMemoryUsage() const
    {
        size_t size = sizeof(*this) + m_nCapacity;
        return size;
    }

private:

    struct ArrayIndexNode 
    {
        size_t offset;// 保存StringBuffer的偏移值，不要保存实际指针，防止内存重新申请后，地址无效
        ArrayIndexNode() : offset(-1) {}
        bool IsValid() const {return (size_t)StringBuffer::STRING_BUFFER_INVALID_INDEX != offset;}
    };

    ArrayIndexNode* GetNode();

private:

    bool m_bInitialize;
    size_t m_nCapacity;
    size_t m_nCount;
    size_t m_nGrowSize;

    ArrayIndexNode* m_pArray;

    StringBuffer* m_pStringBufferRef;
}; // end of class StringArray


#endif // __STRING_BUFFER_H__
