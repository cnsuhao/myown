//--------------------------------------------------------------------
// 文件名:		util_config_template.h
// 内  容:		
// 说  明:		
// 创建日期:	2016年4月28日
// 创建人:		lihl
//    :	   
//--------------------------------------------------------------------

#include "util_config.h"

#ifndef PROPLIST
	#error("please define PROPLIST")
#endif

#ifndef PROPLIST
#	define PROPLIST
#endif

#ifdef CONFIG_CLASS_NAME
#	ifdef DECLARE_CONFIG_PROP_FIELD
#		undef DECLARE_CONFIG_PROP_FIELD
#	endif
#	ifdef DECLARE_CONFIG_PROP_FIELD_EX
#		undef DECLARE_CONFIG_PROP_FIELD_EX
#	endif
#	define DECLARE_CONFIG_PROP_FIELD(NAME, DESC, TP,...)
#	define DECLARE_CONFIG_PROP_FIELD_EX(NAME, DESC, DEFVALUE, TP,...) init_config_prop(NAME, DEFVALUE)
public:
CONFIG_CLASS_NAME()
{
	PROPLIST
};
#endif

#ifdef DECLARE_CONFIG_PROP_FIELD
#	undef DECLARE_CONFIG_PROP_FIELD
#endif
#ifdef DECLARE_CONFIG_PROP_FIELD_EX
#	undef DECLARE_CONFIG_PROP_FIELD_EX
#endif
#define DECLARE_CONFIG_PROP_FIELD(NAME, DESC, TP,...) declare_conifg_prop_ex(TP, NAME, ##__VA_ARGS__)
#define DECLARE_CONFIG_PROP_FIELD_EX(NAME, DESC, DEFVALUE, TP,...) declare_conifg_prop_ex(TP, NAME, ##__VA_ARGS__)
PROPLIST

#ifdef DECLARE_CONFIG_PROP_FIELD
#	undef DECLARE_CONFIG_PROP_FIELD
#endif
#ifdef DECLARE_CONFIG_PROP_FIELD_EX
#	undef DECLARE_CONFIG_PROP_FIELD_EX
#endif
#define DECLARE_CONFIG_PROP_FIELD(NAME, DESC, TP,...) CONFIG_SET_PROP_ITEM(NAME)
#define DECLARE_CONFIG_PROP_FIELD_EX(NAME, DESC, DEFVALUE, TP,...) CONFIG_SET_PROP_ITEM(NAME)
DECLARE_SET_FIELDS( PROPLIST )

#ifdef DECLARE_CONFIG_PROP_FIELD
#	undef DECLARE_CONFIG_PROP_FIELD
#endif
#ifdef DECLARE_CONFIG_PROP_FIELD_EX
#	undef DECLARE_CONFIG_PROP_FIELD_EX
#endif
#ifdef CONFIG_CLASS_NAME
#	undef CONFIG_CLASS_NAME
#endif
#undef PROPLIST