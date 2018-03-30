#ifndef __ENUM_UTILS_DEFINE__
#define __ENUM_UTILS_DEFINE__
#include <stdio.h>
#include <string.h>

#define ENUM_ITEM_STR(Value) #Value,
#define ENUM_ITEM_STR_EX(Value) ENUM_ITEM_STR(Value)

//
// 枚举处理
//
template<typename Type>
inline const char* const EnumString(int nIndex)
{
	return "";
}

template<typename Type>
inline int EnumIndex( const char* szName )
{
	return -1;
}

#define ENUM_TO_STRING( TYPE, COUNT, VALUES ) \
	template<>\
	inline const char* const EnumString<TYPE>( int nIndex ) \
	{	\
		static char* szNames[COUNT+1] = { VALUES "" }; \
		if (nIndex < 0 || nIndex >= COUNT) \
		{ \
			return ""; \
		} \
		return szNames[nIndex]; \
	};

#define STRING_TO_ENUM( TYPE, COUNT, VALUES ) \
	template<>\
	inline int EnumIndex<TYPE>( const char* szName ) \
	{	\
		static char* szNames[COUNT+1] = { VALUES "" }; \
		if ( NULL == szName ) \
		{ \
			return -1; \
		} \
		for (int i = 0; i < COUNT; ++i)	\
		{	\
			if ( NULL  != szNames[i] && strcmp( szName , szNames[i] ) == 0 ) \
			{ \
				return i; \
			} \
		}\
		return -1; \
	};	

#define ENUM_TO_STRING_EX( EnumName, COUNT, VALUES ) \
	ENUM_TO_STRING(EnumName, COUNT, VALUES) \
	STRING_TO_ENUM(EnumName, COUNT, VALUES)

// 标志处理
template<typename FlagEnumType>
class EnumFlag
{
public:
	explicit		EnumFlag(unsigned int data = 0):data_(data){}

	template<FlagEnumType flag>inline	void	SetFlag()			{data_|=(1<<flag);}
	template<FlagEnumType flag>inline	void	SetFlag(bool bset)  {bset?SetFlag<flag>():ResetFlag<flag>();}
	template<FlagEnumType flag>inline	void	ResetFlag()			{data_&=~(1<<flag);}
	template<FlagEnumType flag>inline	bool	TestFlag()const		{return (0!=(data_&(1<<flag)));}

	inline	void	SetFlag(FlagEnumType flag)      {data_|=(1<<flag);}
	inline	void	ResetFlag(FlagEnumType flag)    {data_&=~(1<<flag);}
	inline	bool	TestFlag(FlagEnumType flag)const{return (0!=(data_&(1<<flag)));}

	inline	int		GetData()const                  {return data_;}
	inline	void	SetData(unsigned int data)               {data_ = data;}
	inline	void	ResetData()                     {data_ = 0;}

private:
	unsigned int	data_;
};
#endif
