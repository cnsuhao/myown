//----------------------------------------------------------
// 文件名:      DeclareProperty.h
// 内  容:      属性接口声明
// 说  明:      通过宏定义，自动添加属性的Get及Set接口，加强属性的封装调用
// 创建人:      hehui
// 创建日期:    2012年12月24日
//    :       
//----------------------------------------------------------
#ifndef FSGAME_INTERFACE_DECLARE_PROPERTY_H_
#define FSGAME_INTERFACE_DECLARE_PROPERTY_H_

#include "Fsgame/Define/header.h"
#include "utils/extend_func.h"


/** INT型属性接口 */
#define DECLARE_PROPERTY_INT_VARIANT(name, get_func, set_func) \
    public: \
    bool get_func(IGameObj* pGameObj, int& value) { \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name))                 \
            return false;                              \
        value = pGameObj->QueryInt(#name);              \
        return true;                                   \
    } \
    bool set_func(IGameObj* pGameObj, const int value) {  \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name))                 \
            return false;                                  \
        PERSISTID npc = pGameObj->GetObjectId();\
        ::extend_warning(LOG_TRACE, "%d-%d [PropertySet]:%s = %d", \
                         npc.nIdent, npc.nSerial, #name, value);\
        pGameObj->SetInt(#name, value);                 \
        return true; \
    }

// 默认参数
#define DECLARE_PROPERTY_INT(name) DECLARE_PROPERTY_INT_VARIANT(name, Get##name, Set##name)
///////////////////////////////////////////////////////////////////////////

/** float型属性接口 */
#define DECLARE_PROPERTY_FLOAT_VARIANT(name, get_func, set_func) \
    public: \
    bool get_func(IGameObj* pGameObj, float& value) { \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name))                       \
            return false;                                         \
        value = pGameObj->QueryFloat(#name);                 \
        return true; \
    } \
    bool set_func(IGameObj* pGameObj, const float value) { \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name)) \
            return false; \
        PERSISTID npc = pGameObj->GetObjectId();\
        ::extend_warning(LOG_TRACE, "%d-%d [PropertySet]:%s = %f", \
                         npc.nIdent, npc.nSerial, #name, value);\
        pGameObj->SetFloat(#name, value); \
        return true; \
    }

// 默认参数
#define DECLARE_PROPERTY_FLOAT(name) DECLARE_PROPERTY_FLOAT_VARIANT(name, Get##name, Set##name)
///////////////////////////////////////////////////////////////////////////

/** char型属性接口 */
#define DECLARE_PROPERTY_STRING_VARIANT(name, get_func, set_func) \
    public: \
    bool get_func(IGameObj* pGameObj, std::string& value) { \
		if (NULL == pGameObj)							\
			return false;								\
        if (!pGameObj->FindAttr(#name))                       \
            return false;                                         \
        value = pGameObj->QueryString(#name);                 \
        return true; \
    } \
    bool set_func(IGameObj* pGameObj, const char* value) { \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name)) \
            return false; \
        PERSISTID npc = pGameObj->GetObjectId();\
        ::extend_warning(LOG_TRACE, "%d-%d [PropertySet]:%s = %s", \
                         npc.nIdent, npc.nSerial, #name, value);\
        pGameObj->SetString(#name, value); \
        return true; \
    }

// 默认参数
#define DECLARE_PROPERTY_STRING(name) DECLARE_PROPERTY_STRING_VARIANT(name, Get##name, Set##name)
///////////////////////////////////////////////////////////////////////////

/** object类型属性接口 */
#define DECLARE_PROPERTY_OBJECT_VARIANT(name, get_func, set_func) \
    public: \
    bool get_func(IGameObj* pGameObj, PERSISTID& value) { \
		if (NULL == pGameObj)							\
			return false;								\
        if (!pGameObj->FindAttr(#name))                       \
            return false;                                         \
        value = pGameObj->QueryObject(#name);                 \
        return true; \
    } \
    bool set_func(IGameObj* pGameObj, const PERSISTID& value) { \
		if (NULL == pGameObj)						\
			return false;							\
        if (!pGameObj->FindAttr(#name)) \
            return false; \
        PERSISTID npc = pGameObj->GetObjectId();\
        ::extend_warning(LOG_TRACE, "%d-%d [PropertySet]:%s = %d-%d", \
                         npc.nIdent, npc.nSerial, #name, value.nIdent, value.nSerial);\
        pGameObj->SetObject(#name, value); \
        return true; \
    }

// 默认参数
#define DECLARE_PROPERTY_OBJECT(name) DECLARE_PROPERTY_OBJECT_VARIANT(name, Get##name, Set##name)
///////////////////////////////////////////////////////////////////////////

/** INT64型属性接口 */
#define DECLARE_PROPERTY_INT64_VARIANT(name, get_func, set_func) \
	public: \
	bool get_func(IGameObj* pGameObj, int64_t& value) { \
	if (NULL == pGameObj)						\
		return false;							\
	if (!pGameObj->FindAttr(#name))                 \
	return false;                              \
	value = pGameObj->QueryInt64(#name);              \
	return true;                                   \
	} \
	bool set_func(IGameObj* pGameObj, const int64_t value) {  \
	if (NULL == pGameObj)						\
	return false;								\
	if (!pGameObj->FindAttr(#name))                 \
	return false;                                  \
	PERSISTID npc = pGameObj->GetObjectId();\
	::extend_warning(LOG_TRACE, "%d-%d [PropertySet]:%s = %d", \
	npc.nIdent, npc.nSerial, #name, value);\
	pGameObj->SetInt64(#name, value);                 \
	return true; \
	}

// 默认参数
#define DECLARE_PROPERTY_INT64(name) DECLARE_PROPERTY_INT64_VARIANT(name, Get##name, Set##name)

#endif // FSGAME_INTERFACE_DECLARE_PROPERTY_H_
