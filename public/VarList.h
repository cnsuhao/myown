//--------------------------------------------------------------------
// 文件名:		VarList.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月6日
// 创建人:		 
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VARLIST_H
#define _PUBLIC_VARLIST_H

#include "CoreLog.h"
#include "IVarList.h"
#include "VarType.h"
#include "IVar.h"
#include "Inlines.h"

#define ALING_VARLIST 4
#define ROUND_UP (((bytes) + (size_t)ALING_VARLIST - 1) & ~((size_t)ALING_VARLIST - 1))

#define VARLIST_MAX_MEMBER 10 * 1024*1024


// 参数集

class TVarListAlloc
{
public:
	TVarListAlloc() {}
	~TVarListAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TVarListAlloc& src) {}
};

template<size_t DATA_SIZE, size_t BUFFER_SIZE, typename ALLOC = TVarListAlloc>
class TVarList: public IVarList
{
private:
	typedef TVarList<DATA_SIZE, BUFFER_SIZE, ALLOC> self_type;

	struct var_data_t
	{
		int nType;
		union
		{
			bool boolValue;
			int intValue;
			int64_t int64Value;
			float floatValue;
			double doubleValue;
			size_t stringValue;
			size_t widestrValue;
			void* pointerValue;
			size_t userdataValue;
			struct
			{
				unsigned int objectIdent;
				unsigned int objectSerial;
			};
		};
	};

public:
	TVarList()
	{
		Assert(DATA_SIZE > 0);
		Assert(BUFFER_SIZE > 0);

		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
		m_nWarningCount = 0;

	}

	TVarList(const self_type& src)
	{
		Assert(DATA_SIZE > 0);
		Assert(BUFFER_SIZE > 0);

		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
		m_nWarningCount = 0;

		InnerAppend(src, 0, src.GetCount());
	}

	virtual ~TVarList()
	{
		if (m_nDataSize > DATA_SIZE)
		{
			m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
		}

		if (m_nBufferSize > BUFFER_SIZE)
		{
			m_Alloc.Free(m_pBuffer, m_nBufferSize);
		}
	}

	self_type& operator=(const self_type& src)
	{
		if (m_nDataSize > DATA_SIZE)
		{
			m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
		}

		if (m_nBufferSize > BUFFER_SIZE)
		{
			m_Alloc.Free(m_pBuffer, m_nBufferSize);
		}

		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
		InnerAppend(src, 0, src.GetCount());
		m_nWarningCount = 0;
		return *this;
	}

	// 合并
	virtual bool Concat(const IVarList& src)
	{
		return InnerAppend(src, 0, src.GetCount());
	}

	// 添加
	virtual bool Append(const IVarList& src, size_t start, size_t count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		size_t end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}

		return	InnerAppend(src, start, end);
	}

	// 清空
	virtual void Clear()
	{
		m_nDataUsed = 0;
		m_nBufferUsed = 0;
	}

	// 是否为空
	virtual bool IsEmpty() const
	{
		return (0 == m_nDataUsed);
	}

	// 数据数量
	virtual size_t GetCount() const 
	{
		return m_nDataUsed;
	}

	// 数据类型
	virtual int GetType(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		return m_pData[index].nType;
	}

	// 添加数据
	virtual bool AddBool(bool value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_BOOL;
		p->boolValue = value;

		return true;
	}

	virtual bool AddInt(int value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_INT;
		p->intValue = value;

		return true;
	}

	virtual bool AddInt64(int64_t value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_INT64;
		p->int64Value = value;

		return true;
	}

	virtual bool AddFloat(float value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_FLOAT;
		p->floatValue = value;

		return true;
	}

	virtual bool AddDouble(double value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_DOUBLE;
		p->doubleValue = value;

		return true;
	}

	virtual bool AddString(const char* value)
	{
		Assert(value != NULL);
		if (GetMemoryUsage() + strlen(value) + 1 + sizeof(var_data_t) > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d] ", __FUNCTION__, GetMemoryUsage() + strlen(value) + 1 + sizeof(var_data_t), VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_STRING;
		p->stringValue = m_nBufferUsed;

		const size_t value_size = strlen(value) + 1;
		char* v = AddBuffer(value_size);
		if (NULL == v)
		{
			return false;
		}
		memcpy(v, value, value_size);

		return true;
	}

	virtual bool AddWideStr(const wchar_t* value)
	{
		Assert(value != NULL);
		if (GetMemoryUsage() +(wcslen(value) + 1) * sizeof(wchar_t) + sizeof(var_data_t) > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, GetMemoryUsage() +(wcslen(value) + 1) * sizeof(wchar_t) + sizeof(var_data_t), VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_WIDESTR;
		p->widestrValue = m_nBufferUsed;

		const size_t value_size = (wcslen(value) + 1) * sizeof(wchar_t);
		char* v = AddBuffer(value_size);
		if (NULL == v)
		{
			return false;
		}
		memcpy(v, value, value_size);

		return true;
	}

	virtual bool AddObject(const PERSISTID& value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_OBJECT;
		p->objectIdent = value.nIdent;
		p->objectSerial = value.nSerial;

		return true;
	}

	virtual bool AddPointer(void* value)
	{
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_POINTER;
		p->pointerValue = value;

		return true;
	}

	virtual bool AddUserData(const void* pdata, size_t size)
	{
		if (GetMemoryUsage() + size + sizeof(var_data_t) > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, GetMemoryUsage() + size + sizeof(var_data_t), VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}
		var_data_t* p = AddVarData();
		if (NULL == p)
		{
			return false;
		}
		p->nType = VTYPE_USERDATA;
		p->userdataValue = m_nBufferUsed;

		const size_t value_size = IVar::GetRawUserDataSize(size);
		char* v = AddBuffer(value_size);
		if (NULL == v)
		{
			return false;
		}
		IVar::InitRawUserData(v, pdata, size);

		return true;
	}

	virtual bool AddRawUserData(void* value)
	{
		return AddUserData(IVar::GetUserDataPointer(value), 
			IVar::GetUserDataSize(value));
	}

	// 获得数据
	virtual bool BoolVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return false;
		}


		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return m_pData[index].boolValue;
		case VTYPE_INT:
			return (m_pData[index].intValue != 0);
		case VTYPE_INT64:
			return (m_pData[index].int64Value != 0);
        case VTYPE_STRING:
            return (*(m_pBuffer + m_pData[index].stringValue) != 0);
		default:
			break;
		}

		return false;
	}

	virtual int IntVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return int(m_pData[index].boolValue);
		case VTYPE_INT:
			return m_pData[index].intValue;
		case VTYPE_INT64:
			return int(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return int(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return int(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return atoi(m_pBuffer + m_pData[index].stringValue);
		default:
			break;
		}

		return 0;
	}

	virtual int64_t Int64Val(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return int64_t(m_pData[index].boolValue);
		case VTYPE_INT:
			return int64_t(m_pData[index].intValue);
		case VTYPE_INT64:
			return m_pData[index].int64Value;
		case VTYPE_FLOAT:
			return int64_t(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return int64_t(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return _atoi64(m_pBuffer + m_pData[index].stringValue);
		default:
			break;
		}

		return 0;
	}

	virtual float FloatVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0.0f;
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return float(m_pData[index].boolValue);
		case VTYPE_INT:
			return float(m_pData[index].intValue);
		case VTYPE_INT64:
			return float(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return m_pData[index].floatValue;
		case VTYPE_DOUBLE:
			return float(m_pData[index].doubleValue);
        case VTYPE_STRING:
            return float(atof(m_pBuffer + m_pData[index].stringValue));
		default:
			break;
		}

		return 0.0f;
	}

	virtual double DoubleVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0.0;
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return double(m_pData[index].boolValue);
		case VTYPE_INT:
			return double(m_pData[index].intValue);
		case VTYPE_INT64:
			return double(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return double(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return m_pData[index].doubleValue;
        case VTYPE_STRING:
            return atof(m_pBuffer + m_pData[index].stringValue);
		default:
			break;
		}

		return 0.0;
	}

	virtual const char* StringVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return "";
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_STRING:
			return m_pBuffer + m_pData[index].stringValue;
		default:
			break;
		}

		return "";
	}

	virtual const wchar_t* WideStrVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return L"";
		}



		switch (m_pData[index].nType)
		{
		case VTYPE_WIDESTR:
			return (wchar_t*)(m_pBuffer + m_pData[index].widestrValue);
		default:
			break;
		}

		return L"";
	}

	virtual PERSISTID ObjectVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return PERSISTID();
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_OBJECT:
			return PERSISTID(m_pData[index].objectIdent, 
				m_pData[index].objectSerial);
		default:
			break;
		}

		return PERSISTID();
	}

	virtual void* PointerVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return NULL;
		}

		if (m_pData[index].nType != VTYPE_POINTER)
		{
			return NULL;
		}

		return m_pData[index].pointerValue;
	}

	virtual const void* UserDataVal(size_t index, size_t& size) const
	{
		if (index >= m_nDataUsed)
		{
			size = 0;
			return NULL;
		}

		if (m_pData[index].nType != VTYPE_USERDATA)
		{
			size = 0;
			return NULL;
		}

		char* p = m_pBuffer + m_pData[index].userdataValue;

		size = IVar::GetUserDataSize(p);

		return IVar::GetUserDataPointer(p);
	}

	virtual void* RawUserDataVal(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return NULL;
		}


		if (m_pData[index].nType != VTYPE_USERDATA)
		{
			return NULL;
		}

		return m_pBuffer + m_pData[index].userdataValue;
	}

	//设置数据
	virtual bool SetInt(size_t index, int value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_INT)
			return false;

		m_pData[index].intValue = value;
		return true;
	}

	virtual bool SetInt64(size_t index, int64_t value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_INT64)
			return false;

		m_pData[index].int64Value = value;
		return true;
	}

	virtual bool SetFloat(size_t index, float value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_FLOAT)
			return false;

		m_pData[index].floatValue = value;
		return true;
	}

	virtual bool SetDouble(size_t index, double value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_DOUBLE)
			return false;

		m_pData[index].doubleValue = value;
		return true;
	}

	virtual bool SetString(size_t index, const char* value)
	{
		Assert(value != NULL);

		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_STRING)
			return false;

		char *ps = m_pBuffer + m_pData[index].stringValue;
		const size_t SIZE1 = strlen(value) + 1;
		
		//小于等于以前的,放回原地
		if(SIZE1 <= strlen(ps) + 1)
		{
			strcpy(ps, value);
			return true;
		}

		int len = static_cast<int> (GetMemoryUsage() - strlen(ps) - 1 + SIZE1);
		if (len > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, len, VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}

		m_pData[index].stringValue = m_nBufferUsed;

		const size_t value_size = strlen(value) + 1;
		char* v = AddBuffer(value_size);
		if (NULL == v)
		{
			return false;
		}
		memcpy(v, value, value_size);

		return true;
	}

	virtual bool SetWideStr(size_t index, const wchar_t* value)
	{
		Assert(value != NULL);

		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_WIDESTR)
			return false;

		wchar_t *ps = (wchar_t*)(m_pBuffer + m_pData[index].widestrValue);
		const size_t SIZE1 = wcslen(value) + 1;

		//小于等于以前的,放回原地
		if(SIZE1 <= wcslen(ps) + 1)
		{
			memcpy(ps, value, SIZE1 * sizeof(wchar_t));
			return true;
		}

		int len = static_cast<int>(GetMemoryUsage() + (SIZE1 - wcslen(ps) - 1) * sizeof(wchar_t)) ;
		if (len > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, len, VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}
		m_pData[index].widestrValue = m_nBufferUsed;

		const size_t value_size = (wcslen(value) + 1) * sizeof(wchar_t);
		char* v = AddBuffer(value_size);
		if (NULL == v)
		{
			return false;
		}
		memcpy(v, value, value_size);

		return true;
	}

	virtual bool SetObject(size_t index, const PERSISTID& value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_OBJECT)
			return false;

		m_pData[index].objectIdent = value.nIdent;
		m_pData[index].objectSerial = value.nSerial;
		return true;
	}

	// Add By liulk 2010-10-14
	virtual bool SetPointer(size_t index, void* value)
	{
		if (index >= m_nDataUsed)
			return false;
		if (m_pData[index].nType != VTYPE_POINTER)
			return false;

		m_pData[index].pointerValue = value;
		return true;
	}
	// 获得内存占用
	virtual size_t GetMemoryUsage() const
	{
		size_t size = sizeof(self_type);
		
		if (m_nDataSize > DATA_SIZE)
		{
			size += sizeof(var_data_t) * m_nDataSize;
		}

		if (m_nBufferSize > BUFFER_SIZE)
		{
			size += m_nBufferSize;
		}
		
		return size;
	}
	
private:
	var_data_t* AddVarData()
	{
		if (m_nDataUsed >= m_nDataSize)
		{

			size_t new_size = m_nDataSize * 2;
			if (new_size * sizeof(var_data_t) + m_nBufferSize > VARLIST_MAX_MEMBER *5 )		//5 倍的max_memery
			{
				char buf[128];
				SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, new_size * sizeof(var_data_t) + m_nBufferSize, VARLIST_MAX_MEMBER*5 );
				CORE_TRACE(buf);
				return NULL;
			}

			if (new_size * sizeof(var_data_t) + m_nBufferSize > VARLIST_MAX_MEMBER)
			{
				if (m_nWarningCount == 0)
				{
					char buf[128];
					SPRINTF_S(buf, "Warning, %s varlist buf is [%d] out of [%d]", __FUNCTION__, new_size * sizeof(var_data_t) + m_nBufferSize, VARLIST_MAX_MEMBER );
					CORE_TRACE(buf);
				}
				m_nWarningCount = 1;
			}

			var_data_t* p = (var_data_t*)m_Alloc.Alloc(
				new_size * sizeof(var_data_t));

			memcpy(p, m_pData, m_nDataUsed * sizeof(var_data_t));

			if (m_nDataSize > DATA_SIZE)
			{
				m_Alloc.Free(m_pData, m_nDataSize * sizeof(var_data_t));
			}

			m_pData = p;
			m_nDataSize = new_size;
		}

		return m_pData + m_nDataUsed++;
	}

	char* AddBuffer(size_t need_size)
	{
		size_t new_used = m_nBufferUsed + need_size;

		if (new_used > m_nBufferSize)
		{
			size_t new_size = m_nBufferSize * 2;

			if (m_nDataSize * sizeof(var_data_t) + new_used > VARLIST_MAX_MEMBER *5 )		//5 倍的max_memery
			{
				char buf[128];
				SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, m_nDataSize * sizeof(var_data_t) + new_used, VARLIST_MAX_MEMBER*5 );
				CORE_TRACE(buf);
				return NULL;
			}

			if (m_nDataSize * sizeof(var_data_t) + new_used > VARLIST_MAX_MEMBER)
			{
				if (m_nWarningCount == 0)
				{
					char buf[128];
					SPRINTF_S(buf, "Warning, %s varlist buf is [%d] out of [%d]", __FUNCTION__, m_nDataSize * sizeof(var_data_t) + new_used, VARLIST_MAX_MEMBER );
					CORE_TRACE(buf);
				}
				m_nWarningCount = 1;
			}
			if (new_used > new_size)
			{
				new_size = new_used * 2;
			}

			char* p = (char*)m_Alloc.Alloc(new_size);

			memcpy(p, m_pBuffer, m_nBufferUsed);

			if (m_nBufferSize > BUFFER_SIZE)
			{
				m_Alloc.Free(m_pBuffer, m_nBufferSize);
			}

			m_pBuffer = p;
			m_nBufferSize = new_size;
		}

		char* ret = m_pBuffer + m_nBufferUsed;

		m_nBufferUsed = new_used;

		return ret;
	}

	bool InnerAppend(const IVarList& src, size_t start, size_t end)
	{
		if (GetMemoryUsage() + src.GetMemoryUsage() > VARLIST_MAX_MEMBER * 5)
		{
			char buf[128];
			SPRINTF_S(buf, "Error, %s varlist buf is [%d] out of [%d]", __FUNCTION__, GetMemoryUsage() + src.GetMemoryUsage(), VARLIST_MAX_MEMBER*5 );
			CORE_TRACE(buf);
			return false;
		}

		for (size_t i = start; i < end; ++i)
		{
			switch (src.GetType(i))
			{
			case VTYPE_BOOL:
				AddBool(src.BoolVal(i));
				break;
			case VTYPE_INT:
				AddInt(src.IntVal(i));
				break;
			case VTYPE_INT64:
				AddInt64(src.Int64Val(i));
				break;
			case VTYPE_FLOAT:
				AddFloat(src.FloatVal(i));
				break;
			case VTYPE_DOUBLE:
				AddDouble(src.DoubleVal(i));
				break;
			case VTYPE_STRING:
				AddString(src.StringVal(i));
				break;
			case VTYPE_WIDESTR:
				AddWideStr(src.WideStrVal(i));
				break;
			case VTYPE_OBJECT:
				AddObject(src.ObjectVal(i));
				break;
			case VTYPE_POINTER:
				AddPointer(src.PointerVal(i));
				break;
			case VTYPE_USERDATA:
				{
					size_t size;
					const void* pdata = src.UserDataVal(i, size);

					AddUserData(pdata, size);
				}
				break;
			default:
				Assert(0);
				break;
			}
		}
		return true;
	}

private:
	ALLOC m_Alloc;
	var_data_t m_DataStack[DATA_SIZE];
	var_data_t* m_pData;
	size_t m_nDataSize;
	size_t m_nDataUsed;
	char m_BufferStack[BUFFER_SIZE];
	char* m_pBuffer;
	size_t m_nBufferSize;
	size_t m_nBufferUsed;
	char	m_nWarningCount;
};

typedef TVarList<8, 128> CVarList;

/*
class CVarList: public IVarList
{
private:
	enum { STACK_SIZE = 8 };
	enum { BUFFER_SIZE = 64 };
	
	struct var_data_t
	{
		int nType;
		union
		{
			bool boolValue;
			int intValue;
			int64_t int64Value;
			float floatValue;
			double doubleValue;
			char* stringValue;
			wchar_t* widestrValue;
			void* pointerValue;
			char* userdataValue;
			struct
			{
				unsigned int objectIdent;
				unsigned int objectSerial;
			};
		};
	};

public:
	CVarList()
	{
		m_pData = m_Stack;
		m_nCapacity = STACK_SIZE;
		m_nSize = 0;
		m_nBufUsed = 0;
	}

	CVarList(const CVarList& src)
	{
		m_pData = m_Stack;
		m_nCapacity = STACK_SIZE;
		m_nSize = 0;
		m_nBufUsed = 0;
		InnerAppend(src, 0, src.GetCount());
	}

	virtual ~CVarList()
	{
		Delete(m_pData, m_nSize);

		if (m_nCapacity > STACK_SIZE)
		{
			delete[] m_pData;
		}
	}

	CVarList& operator=(const CVarList& src)
	{
		Delete(m_pData, m_nSize);

		if (m_nCapacity > STACK_SIZE)
		{
			delete[] m_pData;
		}
		
		m_pData = m_Stack;
		m_nCapacity = STACK_SIZE;
		m_nSize = 0;
		m_nBufUsed = 0;
		InnerAppend(src, 0, src.GetCount());
		
		return *this;
	}

	// 合并
	virtual bool Concat(const IVarList& src)
	{
		InnerAppend(src, 0, src.GetCount());

		return true;
	}
	
	// 添加
	virtual bool Append(const IVarList& src, size_t start, size_t count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		size_t end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}
		
		InnerAppend(src, start, end);

		return true;
	}

	// 清空
	virtual void Clear()
	{
		Delete(m_pData, m_nSize);
		m_nSize = 0;
		m_nBufUsed = 0;
	}

	// 是否为空
	virtual bool IsEmpty() const
	{
		return (0 == m_nSize);
	}

	// 数据数量
	virtual size_t GetCount() const 
	{
		return m_nSize;
	}
	
	// 数据类型
	virtual int GetType(size_t index) const
	{
		if (index >= m_nSize)
		{
			return 0;
		}

		return m_pData[index].nType;
	}
	
	// 添加数据
	virtual bool AddBool(bool value)
	{
		var_data_t* p = AddVarData();

		p->nType = VTYPE_BOOL;
		p->boolValue = value;
		
		return true;
	}
	
	virtual bool AddInt(int value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_INT;
		p->intValue = value;
		
		return true;
	}
	
	virtual bool AddInt64(int64_t value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_INT64;
		p->int64Value = value;

		return true;
	}
	
	virtual bool AddFloat(float value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_FLOAT;
		p->floatValue = value;

		return true;
	}
	
	virtual bool AddDouble(double value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_DOUBLE;
		p->doubleValue = value;

		return true;
	}
	
	virtual bool AddString(const char* value)
	{
		Assert(value != NULL);
		
		const size_t SIZE1 = strlen(value) + 1;
		char* s;
		
		if ((m_nBufUsed + SIZE1) <= BUFFER_SIZE)
		{
			s = m_Buffer + m_nBufUsed;
			m_nBufUsed += SIZE1; 
		}
		else
		{
			s = NEW char[SIZE1];
		}
		
		memcpy(s, value, SIZE1);
		
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_STRING;
		p->stringValue = s;

		return true;
	}
	
	virtual bool AddWideStr(const wchar_t* value)
	{
		Assert(value != NULL);
		
		const size_t SIZE1 = wcslen(value) + 1;
		wchar_t* s;
		
		if ((m_nBufUsed + SIZE1 * sizeof(wchar_t)) <= BUFFER_SIZE)
		{
			s = (wchar_t*)(m_Buffer + m_nBufUsed);
			m_nBufUsed += SIZE1 * sizeof(wchar_t);
		}
		else
		{
			s = NEW wchar_t[SIZE1];
		}
		
		memcpy(s, value, SIZE1 * sizeof(wchar_t));
		
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_WIDESTR;
		p->widestrValue = s;

		return true;
	}
	
	virtual bool AddObject(const PERSISTID& value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_OBJECT;
		p->objectIdent = value.nIdent;
		p->objectSerial = value.nSerial;
		
		return true;
	}
	
	virtual bool AddPointer(void* value)
	{
		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_POINTER;
		p->pointerValue = value;

		return true;
	}
	
	virtual bool AddUserData(const void* pdata, size_t size)
	{
		const size_t SIZE1 = IVar::GetRawUserDataSize(size);
		char* v;
		
		if ((m_nBufUsed + SIZE1) <= BUFFER_SIZE)
		{
			v = m_Buffer + m_nBufUsed;
			m_nBufUsed += SIZE1;
		}
		else
		{
			v = NEW char[SIZE1];
		}
		
		IVar::InitRawUserData(v, pdata, size);

		var_data_t* p = AddVarData();
		
		p->nType = VTYPE_USERDATA;
		p->userdataValue = v;

		return true;
	}
	
	virtual bool AddRawUserData(void* value)
	{
		return AddUserData(IVar::GetUserDataPointer(value), 
			IVar::GetUserDataSize(value));
	}
	
	// 获得数据
	virtual bool BoolVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return false;
		}

		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return m_pData[index].boolValue;
		case VTYPE_INT:
			return (m_pData[index].intValue != 0);
		case VTYPE_INT64:
			return (m_pData[index].int64Value != 0);
		default:
			break;
		}
		
		return false;
	}
	
	virtual int IntVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return 0;
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return int(m_pData[index].boolValue);
		case VTYPE_INT:
			return m_pData[index].intValue;
		case VTYPE_INT64:
			return int(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return int(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return int(m_pData[index].doubleValue);
		default:
			break;
		}
		
		return 0;
	}
	
	virtual int64_t Int64Val(size_t index) const
	{
		if (index >= m_nSize)
		{
			return 0;
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return int64_t(m_pData[index].boolValue);
		case VTYPE_INT:
			return int64_t(m_pData[index].intValue);
		case VTYPE_INT64:
			return m_pData[index].int64Value;
		case VTYPE_FLOAT:
			return int64_t(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return int64_t(m_pData[index].doubleValue);
		default:
			break;
		}
		
		return 0;
	}
	
	virtual float FloatVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return 0.0f;
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return float(m_pData[index].boolValue);
		case VTYPE_INT:
			return float(m_pData[index].intValue);
		case VTYPE_INT64:
			return float(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return m_pData[index].floatValue;
		case VTYPE_DOUBLE:
			return float(m_pData[index].doubleValue);
		default:
			break;
		}
		
		return 0.0f;
	}
	
	virtual double DoubleVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return 0.0;
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_BOOL:
			return double(m_pData[index].boolValue);
		case VTYPE_INT:
			return double(m_pData[index].intValue);
		case VTYPE_INT64:
			return double(m_pData[index].int64Value);
		case VTYPE_FLOAT:
			return double(m_pData[index].floatValue);
		case VTYPE_DOUBLE:
			return m_pData[index].doubleValue;
		default:
			break;
		}
		
		return 0.0;
	}
	
	virtual const char* StringVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return "";
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_STRING:
			return m_pData[index].stringValue;
		default:
			break;
		}
		
		return "";
	}
	
	virtual const wchar_t* WideStrVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return L"";
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_WIDESTR:
			return m_pData[index].widestrValue;
		default:
			break;
		}
		
		return L"";
	}
	
	virtual PERSISTID ObjectVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return PERSISTID();
		}
		
		switch (m_pData[index].nType)
		{
		case VTYPE_OBJECT:
			return PERSISTID(m_pData[index].objectIdent, 
				m_pData[index].objectSerial);
		default:
			break;
		}
		
		return PERSISTID();
	}

	virtual void* PointerVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return NULL;
		}
		
		if (m_pData[index].nType != VTYPE_POINTER)
		{
			return NULL;
		}

		return m_pData[index].pointerValue;
	}

	virtual const void* UserDataVal(size_t index, size_t& size) const
	{
		if (index >= m_nSize)
		{
			size = 0;
			return NULL;
		}

		if (m_pData[index].nType != VTYPE_USERDATA)
		{
			size = 0;
			return NULL;
		}
		
		char* p = m_pData[index].userdataValue;
		
		size = IVar::GetUserDataSize(p);
		
		return IVar::GetUserDataPointer(p);
	}
	
	virtual void* RawUserDataVal(size_t index) const
	{
		if (index >= m_nSize)
		{
			return NULL;
		}
		
		if (m_pData[index].nType != VTYPE_USERDATA)
		{
			return NULL;
		}
		
		return m_pData[index].userdataValue;
	}
	
	// 获得内存占用
	size_t GetMemoryUsage() const
	{
		size_t size = sizeof(CVarList);
		
		if (m_nCapacity > STACK_SIZE)
		{
			size += sizeof(var_data_t) * m_nCapacity;
		}
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			switch (m_pData[i].nType)
			{
			case VTYPE_STRING:
				if (NeedDelete(m_pData[i].stringValue))
				{
					size += strlen(m_pData[i].stringValue) + 1;
				}
				break;
			case VTYPE_WIDESTR:
				if (NeedDelete(m_pData[i].widestrValue))
				{
					size += (wcslen(m_pData[i].widestrValue) + 1) 
						* sizeof(wchar_t);
				}
				break;
			case VTYPE_USERDATA:
				if (NeedDelete(m_pData[i].userdataValue))
				{
					size += IVar::GetUserDataSize(m_pData[i].userdataValue);
				}
				break;
			default:
				break;
			}
		}
		
		return size;
	}
	
private:
	bool NeedDelete(void* p) const
	{
		size_t diff = (char*)p - m_Buffer;
		
		return (diff >= BUFFER_SIZE);
	}
	
	void Delete(var_data_t* pData, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			switch (pData[i].nType)
			{
			case VTYPE_STRING:
				if (NeedDelete(pData[i].stringValue))
				{
					delete[] pData[i].stringValue;
				}
				break;
			case VTYPE_WIDESTR:
				if (NeedDelete(pData[i].widestrValue))
				{
					delete[] pData[i].widestrValue;
				}
				break;
			case VTYPE_USERDATA:
				if (NeedDelete(pData[i].userdataValue))
				{
					delete[] pData[i].userdataValue;
				}
				break;
			default:
				break;
			}
		}
	}

	var_data_t* AddVarData()
	{
		if (m_nSize >= m_nCapacity)
		{
			size_t new_cap = m_nCapacity * 2;
			var_data_t* p = NEW var_data_t[new_cap];

			memcpy(p, m_pData, m_nSize * sizeof(var_data_t));

			if (m_nCapacity > STACK_SIZE)
			{
				delete[] m_pData;
			}

			m_pData = p;
			m_nCapacity = new_cap;
		}
		
		return m_pData + m_nSize++;
	}

	void InnerAppend(const IVarList& src, size_t start, size_t end)
	{
		for (size_t i = start; i < end; ++i)
		{
			switch (src.GetType(i))
			{
			case VTYPE_BOOL:
				AddBool(src.BoolVal(i));
				break;
			case VTYPE_INT:
				AddInt(src.IntVal(i));
				break;
			case VTYPE_INT64:
				AddInt64(src.Int64Val(i));
				break;
			case VTYPE_FLOAT:
				AddFloat(src.FloatVal(i));
				break;
			case VTYPE_DOUBLE:
				AddDouble(src.DoubleVal(i));
				break;
			case VTYPE_STRING:
				AddString(src.StringVal(i));
				break;
			case VTYPE_WIDESTR:
				AddWideStr(src.WideStrVal(i));
				break;
			case VTYPE_OBJECT:
				AddObject(src.ObjectVal(i));
				break;
			case VTYPE_POINTER:
				AddPointer(src.PointerVal(i));
				break;
			case VTYPE_USERDATA:
				{
					size_t size;
					const void* pdata = src.UserDataVal(i, size);
					
					AddUserData(pdata, size);
				}
				break;
			default:
				Assert(0);
				break;
			}
		}
	}
	
private:
	var_data_t m_Stack[STACK_SIZE];
	char m_Buffer[BUFFER_SIZE];
	var_data_t* m_pData;
	size_t m_nCapacity;
	size_t m_nSize;
	size_t m_nBufUsed;
};
*/

#endif // _PUBLIC_VARLIST_H

