//--------------------------------------------------------------------
// 文件名:		IModuleCreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IROOM_MODULE_CREATOR_H
#define _SERVER_IROOM_MODULE_CREATOR_H

#include "../public/Macros.h"
#include "IRoomLogicModule.h"

// 逻辑模块创建器接口

class IRoomModuleCreator
{
public:
	IRoomModuleCreator(IRoomModuleCreator* pNext) { m_pNext = pNext; }
	virtual ~IRoomModuleCreator() = 0;

	// 逻辑模块名称
	virtual const char* Name() = 0;
	// 创建逻辑模块
	virtual IRoomLogicModule* Create() = 0;
	// 删除逻辑模块
	virtual void Destroy(IRoomLogicModule* pModule) = 0;
	
	IRoomModuleCreator* Next() { return m_pNext; }
	
private:
	IRoomModuleCreator();
	IRoomModuleCreator(const IRoomModuleCreator &);
	IRoomModuleCreator & operator=(const IRoomModuleCreator &);

private:
	IRoomModuleCreator* m_pNext;
};

inline IRoomModuleCreator::~IRoomModuleCreator() {}

// 逻辑模块创建器链表
extern IRoomModuleCreator* g_pRoomModuleCreator;

// 定义逻辑模块创建器
#define ROOM_LOGIC_MODULE_CREATOR(a) \
class a##ModuleCreator: public IRoomModuleCreator { public: \
a##ModuleCreator(IRoomModuleCreator*& pCreator): IRoomModuleCreator(pCreator) { pCreator = this; } \
virtual ~a##ModuleCreator() {} \
virtual const char* Name() { return #a; } \
virtual IRoomLogicModule* Create() { return NEW a; } \
virtual void Destroy(IRoomLogicModule* pModule) { ::delete pModule; } }; \
a##ModuleCreator a##_ModuleCreator(g_pRoomModuleCreator);

#endif // _SERVER_IMODULECREATOR_H

