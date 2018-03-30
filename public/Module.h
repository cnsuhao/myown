//--------------------------------------------------------------------
// 文件名:		Module.h
// 内  容:		组件模块必须包含的头文件
// 说  明:		
// 创建日期:	2007年2月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_MODULE_H
#define _PUBLIC_MODULE_H

#include "Macros.h"
#include "IVar.h"
#include "IVarList.h"
#include "ICore.h"
#include "IEntity.h"
#include "IInterface.h"
#include "IEntCreator.h"
#include "IIntCreator.h"
#include "IEntInfo.h"
#include "IEntScript.h"
#include "IFuncCreator.h"
#include "ILogic.h"
#include "ILogicInfo.h"
#include "ILogicCreator.h"
#include "VarType.h"
#include "VarTraits.h"
#include "VarGetter.h"
#include "VarSetter.h"
#include "CoreMem.h"
#include <stdlib.h>
#include <stdio.h>

// 实体方法的异常捕获
#define __INNER_TRY    
#define __INNER_CATCH  if (0)
/*
#ifdef _DEBUG
	#define __INNER_TRY    
	#define __INNER_CATCH  if (0)
#else
	#define __INNER_TRY    try
	#define __INNER_CATCH  catch (...)
#endif	// _DEBUG
*/

// 核心接口指针
extern ICore* g_pCore;
// 实体创建器链表
extern IEntCreator* g_pEntCreator;
// 功能接口创建器链表
extern IIntCreator* g_pIntCreator;
// 逻辑创建器链表
extern ILogicCreator* g_pLogicCreator;
// 脚本扩展函数创建器链表
extern IFuncCreator* g_pFuncCreator;

// 定义组件模块
// space: 组件的缺省名字空间（为空则使用模块的文件名）
#define DECLARE_MODULE(space) \
ICore* g_pCore = NULL; \
IEntCreator* g_pEntCreator = NULL; \
IIntCreator* g_pIntCreator = NULL; \
ILogicCreator* g_pLogicCreator = NULL; \
IFuncCreator* g_pFuncCreator = NULL; \
extern "C" FX_DLL_EXPORT \
const char* FxModule_GetType() { return "FxModule"; } \
extern "C" FX_DLL_EXPORT \
int FxModule_GetVersion() { return FX_MODULE_CORE_VERSION; } \
extern "C" FX_DLL_EXPORT \
const char* FxModule_GetSpace() { return space; } \
extern "C" FX_DLL_EXPORT \
void FxModule_Init(ICore* pCore) { g_pCore = pCore; } \
extern "C" FX_DLL_EXPORT \
IEntCreator* FxModule_GetEntCreator() { return g_pEntCreator; } \
extern "C" FX_DLL_EXPORT \
IIntCreator* FxModule_GetIntCreator() { return g_pIntCreator; } \
extern "C" FX_DLL_EXPORT \
ILogicCreator* FxModule_GetLogicCreator() { return g_pLogicCreator; } \
extern "C" FX_DLL_EXPORT \
IFuncCreator* FxModule_GetFuncCreator() { return g_pFuncCreator; } \
ASSERTION_FAIL_DEFINED;

// 定义功能接口
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 接口类名
#define DECLARE_FUNCTION_INTERFACE(space, cls) \
class cls##IntCreator: public IIntCreator \
{ public: \
cls##IntCreator(IIntCreator*& pCreator) \
: IIntCreator(pCreator) { pCreator = this; } \
virtual ~cls##IntCreator() {} \
virtual const char* GetSpace() const { return space; } \
virtual const char* GetName() const { return #cls; } \
virtual IInterface* Create() { return CORE_NEW(cls); } \
virtual void Destroy(IInterface* p) { CORE_DELETE((cls*)p); } }; \
cls##IntCreator cls##_IntCreator(g_pIntCreator);

#define INNER_ENTITY_CREATOR(cls) \
cls##EntCreator& cls##_EntCreator(IEntCreator*& pCreator) \
{ \
	static cls##EntCreator s_init(pCreator); \
	return s_init; \
} \
struct cls##EntCreator_Init \
{ \
	cls##EntCreator_Init(IEntCreator*& pCreator) \
	{ \
		cls##_EntCreator(pCreator); \
	} \
} cls##EntCreator_init_single(g_pEntCreator);

// 定义只能用于继承的抽象实体类
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 实体类名
// parent: 父类名
#define DECLARE_ABSTRACT_ENTITY(space, cls, parent) \
class cls##EntCreator: public IEntCreator \
{ public: \
cls##EntCreator(IEntCreator*& pCreator) \
: IEntCreator(pCreator) { pCreator = this; } \
virtual ~cls##EntCreator() {} \
virtual bool IsAbstract() const { return true; } \
virtual const char* GetParent() const \
{ parent::__validate_parent_exists(); return #parent; } \
virtual const char* GetSpace() const { return space; } \
virtual const char* GetName() const { return #cls; } \
virtual IEntity* Create() { return NULL; } \
virtual void Destroy(IEntity* p) {} }; \
INNER_ENTITY_CREATOR(cls);

// 定义实体类
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 实体类名
// parent: 父类名
#define DECLARE_ENTITY(space, cls, parent) \
class cls##EntCreator: public IEntCreator \
{ public: \
cls##EntCreator(IEntCreator*& pCreator) \
: IEntCreator(pCreator) { pCreator = this; } \
virtual ~cls##EntCreator() {} \
virtual const char* GetParent() const \
{ parent::__validate_parent_exists(); return #parent; } \
virtual const char* GetSpace() const { return space; } \
virtual const char* GetName() const { return #cls; } \
virtual IEntity* Create() { return CORE_NEW(cls); } \
virtual void Destroy(IEntity* p) { CORE_DELETE((cls*)p); } }; \
INNER_ENTITY_CREATOR(cls);

// 定义只读实体属性
// type: 属性类型
// cls: 实体类名
// prop: 属性名
// get_func: 读取属性的类成员函数
#define DECLARE_PROPERTY_GET(type, cls, prop, get_func) \
bool cls##prop##_getter(IEntity* ent, IVar& v) \
{ \
	TVarSetter<type>::Set(v, ((cls*)ent)->get_func()); \
	return true; \
} \
struct cls##prop##_prop_struct: public IEntProperty \
{ \
	cls##prop##_prop_struct() \
	{ \
		IEntCreator& creator = cls##_EntCreator(g_pEntCreator); \
		m_strName = #prop; \
		m_nType = TVarTraits<type>::Type; \
		m_pGetFunc = (void*)(cls##prop##_getter); \
		m_pSetFunc = NULL; \
		m_pNext = creator.GetPropertyLink(); \
		creator.SetPropertyLink(this); \
	} \
}; \
cls##prop##_prop_struct g_##cls##prop##_prop_struct;
		
// 定义可读可写实体属性
// type: 属性类型
// cls: 实体类名
// prop: 属性名
// get_func: 读取属性的类成员函数
// set_func: 设置属性的类成员函数
#define DECLARE_PROPERTY(type, cls, prop, get_func, set_func) \
bool cls##prop##_getter(IEntity* ent, IVar& v) \
{ \
	TVarSetter<type>::Set(v, ((cls*)ent)->get_func()); \
	return true; \
} \
bool cls##prop##_setter(IEntity* ent, const IVar& v) \
{ \
	((cls*)ent)->set_func(TVarGetter<type>::Get(v)); \
	return true; \
} \
struct cls##prop##_prop_struct: public IEntProperty \
{ \
	cls##prop##_prop_struct() \
	{ \
		IEntCreator& creator = cls##_EntCreator(g_pEntCreator); \
		m_strName = #prop; \
		m_nType = TVarTraits<type>::Type; \
		m_pGetFunc = (void*)(cls##prop##_getter); \
		m_pSetFunc = (void*)(cls##prop##_setter); \
		m_pNext = creator.GetPropertyLink(); \
		creator.SetPropertyLink(this); \
	} \
}; \
cls##prop##_prop_struct g_##cls##prop##_prop_struct;

// 获得实体方法的参数
#define INNER_GET_ARG(type, args, index) TVarGetter<type>::Get(args, index)

// 实体方法接口函数
#define INNER_ENTITY_METHOD(cls, func, ret_table) \
struct cls##func##_func_struct: public IEntMethod \
{ \
    cls##func##_func_struct() \
    { \
		IEntCreator& creator = cls##_EntCreator(g_pEntCreator); \
		m_strName = #func; \
		m_pMidFunc = (void*)(cls##func##_mid); \
		m_bReturnTable = ret_table; \
		m_pNext = creator.GetMethodLink(); \
		creator.SetMethodLink(this); \
    } \
}; \
cls##func##_func_struct g_##cls##func##_func_struct;

// 定义实体方法
// cls: 实体类名
// func: 方法函数名
#define DECLARE_METHOD_N(cls, func) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		((cls*)ent)->func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

// 返回表的实体方法
// cls: 实体类名
// func: 方法函数名
#define DECLARE_METHOD_T(cls, func) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		((cls*)ent)->func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, true)

// tr: 返回值类型
// cls: 实体类名
// func: 方法函数名
#define DECLARE_METHOD_0(tr, cls, func) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func(); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

// tr: 返回值类型
// cls: 实体类名
// func: 方法函数名
// t0: 参数类型
#define DECLARE_METHOD_1(tr, cls, func, t0) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_2(tr, cls, func, t0, t1) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_3(tr, cls, func, t0, t1, t2) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_4(tr, cls, func, t0, t1, t2, t3) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_5(tr, cls, func, t0, t1, t2, t3, t4) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_6(tr, cls, func, t0, t1, t2, t3, t4, t5) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_7(tr, cls, func, t0, t1, t2, t3, t4, t5, t6) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_8(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_9(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7), \
			INNER_GET_ARG(t8, args, 8)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

// cls: 实体类名
// func: 方法函数名
#define DECLARE_METHOD_VOID_0(cls, func) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func(); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

// cls: 实体类名
// func: 方法函数名
// t0: 参数类型
#define DECLARE_METHOD_VOID_1(cls, func, t0) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_2(cls, func, t0, t1) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_3(cls, func, t0, t1, t2) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_4(cls, func, t0, t1, t2, t3) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_5(cls, func, t0, t1, t2, t3, t4) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_6(cls, func, t0, t1, t2, t3, t4, t5) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_7(cls, func, t0, t1, t2, t3, t4, t5, t6) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_8(cls, func, t0, t1, t2, t3, t4, t5, t6, t7) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_METHOD_VOID_9(cls, func, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool cls##func##_mid(IEntity* ent, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	__INNER_TRY \
	{ \
		((cls*)ent)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7), \
			INNER_GET_ARG(t8, args, 8)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_ENTITY_METHOD(cls, func, false)

#define DECLARE_EVENT(p)

// 逻辑模块接口

#define INNER_LOGIC_CREATOR(cls) \
cls##LogicCreator& cls##_LogicCreator(ILogicCreator*& pCreator) \
{ \
	static cls##LogicCreator s_init(pCreator); \
	return s_init; \
} \
struct cls##LogicCreator_Init \
{ \
	cls##LogicCreator_Init(ILogicCreator*& pCreator) \
	{ \
		cls##_LogicCreator(pCreator); \
	} \
} cls##LogicCreator_init_single(g_pLogicCreator);

// 定义逻辑类
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 逻辑类名
#define DECLARE_LOGIC(space, cls) \
class cls##LogicCreator: public ILogicCreator \
{ public: \
cls##LogicCreator(ILogicCreator*& pCreator) \
: ILogicCreator(pCreator) { pCreator = this; } \
virtual ~cls##LogicCreator() {} \
virtual const char* GetSpace() const { return space; } \
virtual const char* GetName() const { return #cls; } \
virtual ILogic* Create() { return CORE_NEW(cls); } \
virtual void Destroy(ILogic* p) { CORE_DELETE((cls*)p); } }; \
INNER_LOGIC_CREATOR(cls);

// 逻辑方法接口函数
#define INNER_LOGIC_CALLBACK(cls, func, ret_table) \
struct cls##func##_func_struct: public ILogicCallback \
{ \
    cls##func##_func_struct() \
    { \
		ILogicCreator& creator = cls##_LogicCreator(g_pLogicCreator); \
		m_strName = #func; \
		m_pMidFunc = (void*)(cls##func##_mid); \
		m_bReturnTable = ret_table; \
		m_pNext = creator.GetCallbackLink(); \
		creator.SetCallbackLink(this); \
    } \
}; \
cls##func##_func_struct g_##cls##func##_func_struct;

// 定义逻辑方法
// cls: 逻辑类名
// func: 方法函数名
#define DECLARE_CALLBACK_N(cls, func) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		((cls*)logic)->func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

// 返回表的逻辑方法
// cls: 逻辑类名
// func: 方法函数名
#define DECLARE_CALLBACK_T(cls, func) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		((cls*)logic)->func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, true)

// tr: 返回值类型
// cls: 逻辑类名
// func: 方法函数名
#define DECLARE_CALLBACK_0(tr, cls, func) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func(); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

// tr: 返回值类型
// cls: 逻辑类名
// func: 方法函数名
// t0: 参数类型
#define DECLARE_CALLBACK_1(tr, cls, func, t0) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_2(tr, cls, func, t0, t1) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_3(tr, cls, func, t0, t1, t2) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_4(tr, cls, func, t0, t1, t2, t3) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_5(tr, cls, func, t0, t1, t2, t3, t4) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_6(tr, cls, func, t0, t1, t2, t3, t4, t5) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_7(tr, cls, func, t0, t1, t2, t3, t4, t5, t6) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_8(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

#define DECLARE_CALLBACK_9(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool cls##func##_mid(ILogic* logic, const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	__INNER_TRY \
	{ \
		res << ((cls*)logic)->func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7), \
			INNER_GET_ARG(t8, args, 8)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_LOGIC_CALLBACK(cls, func, false)

// 脚本扩展函数接口

#define INNER_FUNC_CREATOR(func) \
func##FuncCreator& func##_FuncCreator(IFuncCreator*& pCreator) \
{ \
	static func##FuncCreator s_init(pCreator); \
	return s_init; \
} \
struct func##FuncCreator_Init \
{ \
	func##FuncCreator_Init(IFuncCreator*& pCreator) \
	{ \
		func##_FuncCreator(pCreator); \
	} \
} func##FuncCreator_init_single(g_pFuncCreator);

// 定义逻辑类
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 逻辑类名
#define INNER_EXT_FUNCTION(func, ret_table) \
class func##FuncCreator: public IFuncCreator \
{ public: \
func##FuncCreator(IFuncCreator*& pCreator) \
: IFuncCreator(pCreator) { \
	m_strName = #func; \
	m_pMidFunc = (void*)(func##_extfunc_mid); \
	m_bReturnTable = ret_table; \
	pCreator = this; } \
virtual ~func##FuncCreator() {} }; \
INNER_FUNC_CREATOR(func);

// 定义脚本扩展函数
// func: 方法函数名
#define DECLARE_FUNCTION_N(func) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

// 返回表的脚本扩展函数
// func: 方法函数名
#define DECLARE_FUNCTION_T(func) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	__INNER_TRY \
	{ \
		func(args, res); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, true)

// tr: 返回值类型
// func: 方法函数名
#define DECLARE_FUNCTION_0(tr, func) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	__INNER_TRY \
	{ \
		res << func(); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

// tr: 返回值类型
// func: 方法函数名
// t0: 参数类型
#define DECLARE_FUNCTION_1(tr, func, t0) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_2(tr, func, t0, t1) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_3(tr, func, t0, t1, t2) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_4(tr, func, t0, t1, t2, t3) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_5(tr, func, t0, t1, t2, t3, t4) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_6(tr, func, t0, t1, t2, t3, t4, t5) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_7(tr, func, t0, t1, t2, t3, t4, t5, t6) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_8(tr, func, t0, t1, t2, t3, t4, t5, t6, t7) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#define DECLARE_FUNCTION_9(tr, func, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool func##_extfunc_mid(const IVarList& args, IVarList& res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	__INNER_TRY \
	{ \
		res << func( \
			INNER_GET_ARG(t0, args, 0), \
			INNER_GET_ARG(t1, args, 1), \
			INNER_GET_ARG(t2, args, 2), \
			INNER_GET_ARG(t3, args, 3), \
			INNER_GET_ARG(t4, args, 4), \
			INNER_GET_ARG(t5, args, 5), \
			INNER_GET_ARG(t6, args, 6), \
			INNER_GET_ARG(t7, args, 7), \
			INNER_GET_ARG(t8, args, 8)); \
	} __INNER_CATCH { return false; } \
	return true; \
} \
INNER_EXT_FUNCTION(func, false)

#endif // _PUBLIC_MODULE_H

