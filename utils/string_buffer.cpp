
//--------------------------------------------------------------------
// 文件名:      \utils\string_buffer.cpp
// 内  容:      
// 说  明:      
// 创建日期:    2014年9月17日
// 创建人:      longch
//    :       
//--------------------------------------------------------------------

#include "utils/string_buffer.h"


const size_t StringBuffer::STRING_BUFFER_INVALID_INDEX = (size_t)-1;
StringBuffer::StringBuffer()
{
    m_bInitialize = false;
    m_nCapacity = 0;
    m_nSize = 0;
    m_nGrowSize = 0;
    m_nLastStringPos = 0;
    m_nBufferPos = 0;
    m_pBuffer = NULL;
}

StringBuffer::~StringBuffer()
{
    if (m_pBuffer)
    {
        delete[] m_pBuffer;
    }
}

bool StringBuffer::Init(size_t init_size, size_t grow_size)
{
    m_pBuffer = new char[init_size];
    if (NULL == m_pBuffer)
    {
        return false;
    }

    memset(m_pBuffer, 0, init_size);
    m_nCapacity = init_size;
    m_nGrowSize = grow_size;
    m_bInitialize = true;
    return true;
}

const char* StringBuffer::Get(size_t offset) const
{
    if (offset < m_nBufferPos)
    {
        return m_pBuffer+offset;
    }
    return "";
}

size_t StringBuffer::Add(const char* ss)
{
    if (NULL == ss || '\0' == *ss)
    {
        return STRING_BUFFER_INVALID_INDEX;
    }
    size_t len = strlen(ss);
    char* p = GetBuffer(len);
    if (NULL == p)
    {
        return STRING_BUFFER_INVALID_INDEX;
    }
    strcpy(p, ss);
    ++m_nSize;
    return m_nLastStringPos;
}

char* StringBuffer::GetBuffer(size_t offset)
{
    size_t len = offset + 1;
    if (m_nBufferPos + len > m_nCapacity)
    {
        size_t new_capacity = m_nCapacity + m_nGrowSize;
        char* new_buff = new char[new_capacity];
        if (NULL == new_buff)
        {
            return NULL;
        }

        memset(new_buff, 0, new_capacity);
        memcpy(new_buff, m_pBuffer, m_nCapacity);
        delete[] m_pBuffer;
        m_pBuffer = new_buff;
        m_nCapacity = new_capacity;
    }

    char* p = m_pBuffer + m_nBufferPos;
    m_nLastStringPos = m_nBufferPos;
    m_nBufferPos += len;

    return p;
}


//////////////////////////////////////////////////////////////////////////

StringArray::StringArray()
{
    m_bInitialize = false;
    m_nCapacity = 0;
    m_nCount = 0;
    m_nGrowSize = 0;
    m_pArray = NULL;
    m_pStringBufferRef = NULL;
}

StringArray::StringArray(const StringArray& src)
{
    if (src.m_bInitialize)
    {
        m_bInitialize = src.m_bInitialize;
        m_nCapacity = src.m_nCapacity;
        m_nCount = src.m_nCount;
        m_nGrowSize = src.m_nGrowSize;
        m_pArray = new ArrayIndexNode[m_nCapacity];
        memcpy(m_pArray, src.m_pArray, m_nCapacity*sizeof(ArrayIndexNode));
        m_pStringBufferRef = src.m_pStringBufferRef;
    }
    else
    {
        m_bInitialize = false;
        m_nCapacity = 0;
        m_nCount = 0;
        m_nGrowSize = 0;
        m_pArray = NULL;
        m_pStringBufferRef = NULL;
    }
}

StringArray& StringArray::operator=(const StringArray& src)
{
    if (&src != this && src.m_bInitialize)
    {
        if (m_bInitialize)
        {
            if (m_pArray)
            {
                delete[] m_pArray;
            }
        }

        m_bInitialize = src.m_bInitialize;
        m_nCapacity = src.m_nCapacity;
        m_nCount = src.m_nCount;
        m_nGrowSize = src.m_nGrowSize;
        m_pArray = new ArrayIndexNode[m_nCapacity];
        memcpy(m_pArray, src.m_pArray, m_nCapacity*sizeof(ArrayIndexNode));
        m_pStringBufferRef = src.m_pStringBufferRef;
    }
    return *this;
}

StringArray::~StringArray()
{
    if (m_pArray)
    {
        delete[] m_pArray;
    }
}

bool StringArray::Init(size_t init_size, size_t grow_size, StringBuffer* string_buffer)
{
    if (NULL == string_buffer)
    {
        return false;
    }
    m_pArray = new ArrayIndexNode[init_size];
    if (NULL == m_pArray)
    {
        return false;
    }

    memset(m_pArray, 0, init_size*sizeof(ArrayIndexNode));
    m_nCapacity = init_size;
    m_nGrowSize = grow_size;
    m_pStringBufferRef = string_buffer;
    m_bInitialize = true;
    return true;
}

void StringArray::AddVarList(const IVarList& value)
{
    size_t count = value.GetCount();
    for (size_t t=0; t<count; ++t)
    {
        const char* ss = value.StringVal(t);
        Add(ss);
    }
}

void StringArray::ConvertToVarList(IVarList& value) const
{
    for (size_t t=0; t<m_nCount; ++t)
    {
        value << StringVal(t);
    }
}

// Add调用前，需要调用Init并返回了true之后，才能保证Add调用是安全的
const char* StringArray::Add(const char* value)
{
    if (NULL == m_pStringBufferRef)
    {
        return NULL;
    }
    size_t offset = m_pStringBufferRef->Add(value);
    ArrayIndexNode* pp = GetNode();
    if (NULL == pp)
    {
        return NULL;
    }
    pp->offset = offset;
    ++m_nCount;
    return m_pStringBufferRef->Get(offset);
}

bool StringArray::Find(const char* value)
{
    if (!value || !*value || NULL == m_pStringBufferRef)
    {
        return false;
    }

    for (size_t i=0; i<m_nCount; i++)
    {
        const char* ss = m_pStringBufferRef->Get(m_pArray[i].offset);
        if (strcmp(ss, value) == 0)
        {
            return true;
        }
    }
    return false;
}

const char* StringArray::StringVal(size_t index) const
{
    if (index < m_nCount && m_pStringBufferRef)
    {
        return m_pStringBufferRef->Get(m_pArray[index].offset);
    }
    return "";
}

StringArray::ArrayIndexNode* StringArray::GetNode()
{
    if (m_nCount >= m_nCapacity)
    {
        size_t new_capacity = m_nCapacity + m_nGrowSize;
        ArrayIndexNode* new_buff = new ArrayIndexNode[new_capacity];
        if (NULL == new_buff)
        {
            return NULL;
        }

        memcpy(new_buff, m_pArray, m_nCapacity*sizeof(ArrayIndexNode));
        delete[] m_pArray;
        m_pArray = new_buff;
        m_nCapacity = new_capacity;
    }

    return m_pArray + m_nCount;
}
