//--------------------------------------------------------------------
// 文件名:		IModuleCreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IMODULECREATOR_H
#define _SERVER_IMODULECREATOR_H

#include "../public/Macros.h"
#include "ILogicModule.h"

// 逻辑模块创建器接口

class IModuleCreator
{
public:
	IModuleCreator(IModuleCreator* pNext) { m_pNext = pNext; }
	virtual ~IModuleCreator() = 0;

	// 逻辑模块名称
	virtual const char* Name() = 0;
	// 创建逻辑模块
	virtual ILogicModule* Create() = 0;
	// 删除逻辑模块
	virtual void Destroy(ILogicModule* pModule) = 0;
	
	IModuleCreator* Next() { return m_pNext; }
	
private:
	IModuleCreator();
	IModuleCreator(const IModuleCreator &);
	IModuleCreator & operator=(const IModuleCreator &);

private:
	IModuleCreator* m_pNext;
};

inline IModuleCreator::~IModuleCreator() {}

// 逻辑模块创建器链表
extern IModuleCreator* g_pModuleCreator;

// 定义逻辑模块创建器
#define LOGIC_MODULE_CREATOR(a) \
class a##ModuleCreator: public IModuleCreator { public: \
a##ModuleCreator(IModuleCreator*& pCreator): IModuleCreator(pCreator) { pCreator = this; } \
virtual ~a##ModuleCreator() {} \
virtual const char* Name() { return #a; } \
virtual ILogicModule* Create() { return NEW a; } \
virtual void Destroy(ILogicModule* pModule) { ::delete pModule; } }; \
a##ModuleCreator a##_ModuleCreator(g_pModuleCreator);

#endif // _SERVER_IMODULECREATOR_H

