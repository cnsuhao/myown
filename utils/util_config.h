//--------------------------------------------------------------------
// 文件名:		util_config.h
// 内  容:		
// 说  明:		
// 创建日期:	2016年3月18日
// 创建人:		lihl
//    :	   
//--------------------------------------------------------------------

#ifndef __UTIL_CONFIG_H__
#define __UTIL_CONFIG_H__
#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "extend_func.h"
#include "Singleton.h"

#include <vector>
#include <map>
#include "string_util.h"

template<typename KEY_TYPE>
class ConfigData
{
protected:
	std::map<std::string, std::string> m_mapUnKownFields;
public: 
	virtual ~ConfigData()
	{
	}

	typename typedef KEY_TYPE key_type;
	typename typedef ConfigData<key_type> self_type;

	virtual const key_type& GetID() const =  0;
	virtual void PostLoad()
	{
		// don't any thing
	}
	virtual bool LoadString( const char* kv )
	{
		if ( NULL == kv ) return false;
		std::map<std::string, std::string> kves;
		if ( 0 == StringUtil::ParseToMap( kv, ';', ':', kves) )
		{
			return false;
		}

		for (std::map<std::string, std::string>::iterator itr = kves.begin();
			itr != kves.end(); ++itr )
		{
			SetPropValue( itr->first.c_str(), itr->second.c_str() );
		}
		
		return true;	
	}

	bool SetPropValue( const char* k, const char* v )
	{
		if ( InnerSetProp(k, v) )
		{
			return true;
		}

		if ( NULL != k && v != NULL )
		{
			return m_mapUnKownFields.insert( std::make_pair(std::string(k), std::string(v)) ).second;
		}
		return false;
	}

protected:
	virtual bool InnerSetProp( const char* k, const char* v )
	{
		return false;
	}
};





typedef void (*FUNC_CONFIG_ERROR)( bool bError, const char* pMsg );
#define LOG_CONFIG_ERROR( onError, isError, format, ... ) \
	do{\
		if ( NULL != onError ) \
		{ \
			char szBuffer[1024] = {0};\
			sprintf(szBuffer, format, ##__VA_ARGS__);	\
			onError( isError, szBuffer);	\
		}\
	}while(0)

// DataType should inherit IConfigData
template<typename DataType>
class Configure
{
private:
	typename typedef DataType::key_type key_type;
	typedef std::map<key_type, size_t>	INDEX_TYPE;
	typedef std::vector<DataType*>					VALUE_TYPE;

	static INDEX_TYPE ms_mapIndexes;
	static VALUE_TYPE ms_vecValues;

public:
	static bool LoadXml( const char* szXmlPath, FUNC_CONFIG_ERROR onError )
	{
		Release();

		try
		{
			char* pfilebuff = GetFileBuff(szXmlPath);

			if (NULL == pfilebuff)
			{
				LOG_CONFIG_ERROR(onError, true, "not found config '%s'", szXmlPath);
				return false;
			}

			xml_document<> doc;

			doc.parse<rapidxml::parse_default>(pfilebuff);

			xml_node<>* pNodeRoot = doc.first_node("Object");
			if (NULL == pNodeRoot)
			{
				LOG_CONFIG_ERROR(onError, true, "xml '%s' not found element[Object]", szXmlPath);
				return false;
			}
			xml_node<>* pNode = pNodeRoot->first_node("Property");
			if (NULL == pNode)
			{
				LOG_CONFIG_ERROR(onError, true, "xml '%s' not found element[Property]", szXmlPath);
				return false;
			}

			LoopBeginCheck(check_kk);
			int nLine = 1;
			while (pNode != NULL)
			{
				LoopDoCheck(check_kk);
				xml_attribute<>* pAttr = pNode->first_attribute();
				if ( NULL == pAttr )
				{
					LOG_CONFIG_ERROR(onError, false, "xml '%s' has empty element at line %d", szXmlPath, nLine);
					continue;
				}

				DataType* pd = new DataType();
				while( NULL != pAttr )
				{
					try
					{
						if (!pd->SetPropValue( pAttr->name(), pAttr->value() ) )
						{
							LOG_CONFIG_ERROR(onError, false, "parse xml '%s' not find field '%s' at class %s", 
								szXmlPath, pAttr->name(), typeid(DataType).name());
						}
					}
					catch(...)
					{
						delete pd;
						LOG_CONFIG_ERROR(onError, true, "xml '%s' parse attribute '%s' error at line %d", szXmlPath, pAttr->name(), nLine);
						return false;
					}

					pAttr = pAttr->next_attribute();
				}
				
				try
				{
					pd->PostLoad();
				}
				catch(...)
				{
					delete pd;
					LOG_CONFIG_ERROR(onError, true, "xml '%s' PostLoad error", szXmlPath);
					return false;
				}

				INDEX_TYPE::_Pairib result = ms_mapIndexes.insert( std::make_pair(pd->GetID(), (unsigned int)ms_vecValues.size()));
				if ( result.second )
				{
					ms_vecValues.push_back( pd );
				}
				else
				{
					LOG_CONFIG_ERROR(onError, false, "can't add same id:%s in config:%s at line:%d",
							ToString(pd->GetID()).c_str(),
							szXmlPath, nLine);
					delete pd;
				}
				pNode = pNode->next_sibling("Property");
				++nLine;
			}
		}
		catch(parse_error& e)
		{
			LOG_CONFIG_ERROR(onError, true, "parse xml '%s' error[%s]", szXmlPath, e.what());
			return false;
		}

		return true;
	}

	static void Release()
	{
		for( size_t i = 0; i < ms_vecValues.size(); ++i )
		{
			delete ms_vecValues[i];
		}

		ms_vecValues.clear();
		ms_mapIndexes.clear();
	}

	static DataType* Find(const key_type& key)
	{
		INDEX_TYPE::iterator itr = ms_mapIndexes.find( key );
		if ( itr != ms_mapIndexes.end() )
		{
			return ms_vecValues[itr->second];
		}
		return NULL;
	}

	typedef bool (*FUNC_CONFIG_COMPARE)( DataType* data );
	static DataType* FindEx( FUNC_CONFIG_COMPARE compare )
	{
		VALUE_TYPE::iterator result = std::find_if( ms_vecValues.begin(), ms_vecValues.end(),compare );
		if ( ms_vecValues.end() != result )
		{
			return *result;
		}

		return NULL;
	}

	static size_t GetRowCount()
	{
		return ms_vecValues.size();
	}

	static DataType* GetRow(size_t row)
	{
		if (row == -1 || row >= ms_vecValues.size())
			return NULL;
		return ms_vecValues[row];
	}

	private:
		template<typename T>
		static std::string ToString( const T& d )
		{
			return std::string("");
		}

		template<>
		static std::string ToString<size_t>( const size_t& d )
		{
			char s[16] = {0};
			sprintf(s, "%ld", (int)d);
			return std::string(s);
		}

		template<>
		static std::string ToString<std::string>( const std::string& d )
		{
			return d;
		}
};

template<typename DataType>
typename Configure<DataType>::INDEX_TYPE Configure<DataType>::ms_mapIndexes;
template<typename DataType>
typename Configure<DataType>::VALUE_TYPE Configure<DataType>::ms_vecValues;



#define INT_CAST(VALUE) atoi(VALUE)
#define FLOAT_CAST(VALUE) (float)atof(VALUE)
#define DOUBLE_CAST(VALUE) atof(VALUE)
#define STRING_CAST(VALUE) (VALUE)

#define CONFIG_SET_PROP_ITEM(NAME) \
	if ( strcmp(k, #NAME) == 0) \
	{ \
		__Set##NAME(v);return true;\
	}\

#define DECLARE_CONFIG_PROP_X(TYPE, NAME, CAST) \
	protected:\
	TYPE m_prop##NAME;\
	void __Set##NAME( const char* value )\
	{\
		m_prop##NAME = (TYPE)CAST(value); \
	};\
	public:\
	const TYPE& Get##NAME() const\
	{\
		return m_prop##NAME;\
	};\
	void Set##NAME( const TYPE& value)\
	{\
		m_prop##NAME = value;\
	};
#define DECLARE_CONFIG_PROP_INT_X(TYPE, NAME) DECLARE_CONFIG_PROP_X( TYPE, NAME, INT_CAST)
#define DECLARE_CONFIG_PROP_INT8(NAME) DECLARE_CONFIG_PROP_INT_X( char, NAME )
#define DECLARE_CONFIG_PROP_UIN8(NAME) DECLARE_CONFIG_PROP_INT_X( (unsigned char), NAME )
#define DECLARE_CONFIG_PROP_INT16(NAME) DECLARE_CONFIG_PROP_INT_X( short, NAME )
#define DECLARE_CONFIG_PROP_UINT16(NAME) DECLARE_CONFIG_PROP_INT_X( (unsigned short), NAME )
#define DECLARE_CONFIG_PROP_INT32(NAME) DECLARE_CONFIG_PROP_INT_X( int, NAME )
#define DECLARE_CONFIG_PROP_UINT32(NAME) DECLARE_CONFIG_PROP_INT_X( unsigned int, NAME )
#define DECLARE_CONFIG_PROP_SIZE_T(NAME) DECLARE_CONFIG_PROP_INT_X( size_t, NAME )
#define DECLARE_CONFIG_PROP_INT64(NAME) DECLARE_CONFIG_PROP_INT_X( long long, NAME )
#define DECLARE_CONFIG_PROP_UINT64(NAME) DECLARE_CONFIG_PROP_INT_X( unsigned long long, NAME )
#define DECLARE_CONFIG_PROP_FLOAT(NAME) DECLARE_CONFIG_PROP_X( float, NAME, FLOAT_CAST )
#define DECLARE_CONFIG_PROP_DOUBLE(NAME) DECLARE_CONFIG_PROP_X( double, NAME, FLOAT_CAST)
#define DECLARE_CONFIG_PROP_STRING(NAME) DECLARE_CONFIG_PROP_X( std::string, NAME, STRING_CAST )

#define DECLARE_CONFIG_PROP_ARRAY_X(TYPE, NAME, CAST) 	\
	protected:\
	std::vector<TYPE> m_prop##NAME;\
	void __Set##NAME( const char* value )\
	{\
		m_prop##NAME.clear();\
		CVarList arr; \
		if ( StringUtil::ParseToVector(value, ',', arr) > 0 ) \
		{\
			for( size_t i = 0; i < arr.GetCount(); ++i ) {m_prop##NAME.push_back( CAST(arr.StringVal(i)) );}\
		}\
	};\
	public:\
	const std::vector<TYPE>& Get##NAME() const\
	{\
		return m_prop##NAME;\
	}\
	std::vector<TYPE>& Get##NAME()\
	{\
		return m_prop##NAME;\
	}
#define DECLARE_CONFIG_PROP_ARRAY_INT(NAME) DECLARE_CONFIG_PROP_ARRAY_X( int, NAME, INT_CAST)
#define DECLARE_CONFIG_PROP_ARRAY_FLOAT(NAME) DECLARE_CONFIG_PROP_ARRAY_X( float, NAME, FLOAT_CAST )
#define DECLARE_CONFIG_PROP_ARRAY_DOUBLE(NAME) DECLARE_CONFIG_PROP_ARRAY_X( double, NAME, FLOAT_CAST)
#define DECLARE_CONFIG_PROP_ARRAY_STRING(NAME) DECLARE_CONFIG_PROP_ARRAY_X( std::string, NAME, STRING_CAST )

#define DECLARE_CONFIG_PROP_MAP(NAME, KTYPE, VTYPE, KCAST, VCAST) 	\
	protected:\
	std::map<KTYPE,VTYPE> m_prop##NAME;\
	void __Set##NAME( const char* value )\
	{\
		m_prop##NAME.clear(); \
		std::map<std::string, std::string> kves; \
		if ( 0 == StringUtil::ParseToMap( value, ';', ':', kves) ) \
		{ \
			return; \
		} \
			for (std::map<std::string, std::string>::iterator itr = kves.begin(); \
			itr != kves.end(); ++itr ) \
		{ \
			m_prop##NAME.insert( std::make_pair<KTYPE, VTYPE>( KCAST(itr->first.c_str()), VCAST(itr->second.c_str()) ) ); \
		} \
	};\
	public:\
		const std::map<KTYPE,VTYPE>& Get##NAME() const\
		{\
			return m_prop##NAME;\
		}\
		std::map<KTYPE,VTYPE>& Get##NAME()\
		{\
			return m_prop##NAME;\
		}

#define DECLARE_CONFIG_PROP_DATA( NAME, TYPE ) 	\
	protected:\
	TYPE m_prop##NAME;\
	void __Set##NAME( const char* value )\
	{\
		m_prop##NAME.LoadString(value); \
	};\
		public:\
	const TYPE& Get##NAME() const\
	{\
		return m_prop##NAME;\
	}\
	TYPE& Get##NAME()\
	{\
	return m_prop##NAME;\
	}

// LOADER => bool (*Loader)(const char* data, TYPE& ret)
#define DECLARE_CONFIG_PROP_CUSTOM( NAME, TYPE,LOADER) 	\
	protected:\
	TYPE m_prop##NAME;\
	void __Set##NAME( const char* value )\
	{\
		LOADER(value, m_prop##NAME); \
	};\
	public:\
	const TYPE& Get##NAME() const\
	{\
		return m_prop##NAME;\
	}\
	TYPE& Get##NAME()\
	{\
		return m_prop##NAME;\
	}

#define DECLARE_CONFIG_OBJECT_BEGIN_T( NAME, KEY_TYPE ) \
class NAME : public ConfigData<KEY_TYPE> \
{	

#define DECLARE_PROP_FIELDS( PROPS ) \
public:	\
	PROPS

#define CONSTRUCT( NAME, INIT_FIELDS_LIST ) \
public:	\
	NAME()	\
	{\
		INIT_FIELDS_LIST	\
	};


#define INIT_SET_FIELDS( NAME, VALUE ) Set##NAME(VALUE);

#define DECLARE_SET_FIELDS( SETS ) \
protected:	\
	virtual bool InnerSetProp( const char* k, const char* v ) \
	{ \
		SETS;\
		return false; \
	} 

#define DECLARE_CONFIG_OBJECT_BEGIN( NAME ) DECLARE_CONFIG_OBJECT_BEGIN_T( NAME, size_t )

#define DECLARE_CONFIG_OBJECT_END() };


#define declare_config_prop_int8(NAME,...) DECLARE_CONFIG_PROP_INT8(NAME)
#define declare_config_prop_uint8(NAME,...) DECLARE_CONFIG_PROP_UIN8(NAME)
#define declare_config_prop_int16(NAME,...) DECLARE_CONFIG_PROP_INT16(NAME)
#define declare_config_prop_uint16(NAME,...) DECLARE_CONFIG_PROP_UINT16(NAME)
#define declare_config_prop_int32(NAME,...) DECLARE_CONFIG_PROP_INT32(NAME)
#define declare_config_prop_uint32(NAME,...) DECLARE_CONFIG_PROP_UINT32(NAME)
#define declare_config_prop_size_t(NAME,...) DECLARE_CONFIG_PROP_SIZE_T(NAME)
#define declare_config_prop_int64(NAME,...) DECLARE_CONFIG_PROP_INT64(NAME)
#define declare_config_prop_uint64(NAME,...) DECLARE_CONFIG_PROP_UINT64(NAME)
#define declare_config_prop_f32(NAME,...) DECLARE_CONFIG_PROP_FLOAT(NAME)
#define declare_config_prop_f64(NAME,...) DECLARE_CONFIG_PROP_DOUBLE(NAME)
#define declare_config_prop_string(NAME,...) DECLARE_CONFIG_PROP_STRING(NAME)

#define declare_config_prop_array_int32(NAME,...) DECLARE_CONFIG_PROP_ARRAY_INT(NAME)
#define declare_config_prop_array_f32(NAME,...) DECLARE_CONFIG_PROP_ARRAY_FLOAT(NAME)
#define declare_config_prop_array_f64(NAME,...) DECLARE_CONFIG_PROP_ARRAY_DOUBLE(NAME)
#define declare_config_prop_array_string(NAME,...) DECLARE_CONFIG_PROP_ARRAY_STRING(NAME)
#define declare_config_prop_map(NAME, KTYPE, VTYPE, KCAST, VCAST,...) DECLARE_CONFIG_PROP_MAP(NAME, KTYPE, VTYPE, KCAST, VCAST)
#define declare_config_prop_data( NAME, TYPE, ... ) DECLARE_CONFIG_PROP_DATA( NAME, TYPE )
#define declare_config_prop_custom( NAME, TYPE,LOADER,...) DECLARE_CONFIG_PROP_CUSTOM( NAME, TYPE,LOADER)

#define EXPAND(...) __VA_ARGS__
#define declare_config_prop_array(NAME, TP, ...) EXPAND(declare_config_prop_array_##TP(NAME,__VA_ARGS__))
#define declare_conifg_prop_ex(TP,NAME,...) EXPAND(declare_config_prop_##TP(NAME, __VA_ARGS__))
#define init_config_prop(NAME, VALUE) Set##NAME(VALUE);

// 定义宏
#define DECLARE_CONFIG_PROP_FIELD(NAME, DESC, TP,...) declare_conifg_prop_ex(TP, NAME, ##__VA_ARGS__)
#define DECLARE_CONFIG_PROP_FIELD_EX(NAME, DESC, DEFVALUE, TP, ...) declare_conifg_prop_ex(TP, NAME, ##__VA_ARGS__)
#define MACRO_NOTES //


DECLARE_CONFIG_OBJECT_BEGIN(ConfigTest)

DECLARE_PROP_FIELDS(
	DECLARE_CONFIG_PROP_SIZE_T(ID)
	DECLARE_CONFIG_PROP_STRING(Name)
	DECLARE_CONFIG_PROP_ARRAY_INT(List)
	DECLARE_CONFIG_PROP_MAP(Map, std::string, int, STRING_CAST, INT_CAST ) )

DECLARE_SET_FIELDS(
	CONFIG_SET_PROP_ITEM(ID)
	CONFIG_SET_PROP_ITEM(Name)
	CONFIG_SET_PROP_ITEM(List)
	CONFIG_SET_PROP_ITEM(Map) )

DECLARE_CONFIG_OBJECT_END()

//class ConfigTest : public ConfigData
//{
//	DECLARE_CONFIG_PROP_UINT32(ID )
//	DECLARE_CONFIG_PROP_STRING(Name)
//	DECLARE_CONFIG_PROP_ARRAY_INT(List)
//	DECLARE_CONFIG_PROP_MAP(Map, std::string, int, STRING_CAST, INT_CAST )
//
//public:
//	virtual void SetProp( const char* k, const char * v)
//	{
//		DECLARE_CONFIG_SET_PROP_ITEM(ID)
//		DECLARE_CONFIG_SET_PROP_ITEM(Name)
//		DECLARE_CONFIG_SET_PROP_ITEM(List)
//		DECLARE_CONFIG_SET_PROP_ITEM(Map)
//	}
//};


#endif // __UTIL_CONFIG_H__
