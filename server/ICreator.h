//--------------------------------------------------------------------
// 文件名:		ICreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2003年12月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ICREATOR_H
#define _SERVER_ICREATOR_H

#include "../public/Macros.h"
#include "ICallee.h"

// 逻辑接口创建器

class ICreator
{
public:
	ICreator(ICreator* pNext) { m_pNext = pNext; }
	virtual ~ICreator() = 0;

	virtual const char* Name() = 0;
	virtual int Type() = 0;
	virtual ICallee* Create() = 0;
	virtual void Destroy(ICallee* pCallee) = 0;
	
	ICreator* Next() { return m_pNext; }
	
private:
	ICreator();
	ICreator(const ICreator &);
	ICreator & operator=(const ICreator &);

private:
	ICreator* m_pNext;
};

inline ICreator::~ICreator() {}

// 逻辑接口创建器链表
extern ICreator* g_pCreator;

#define CALLEE_CREATOR(a, p, t) \
class a##Creator: public ICreator { public: \
a##Creator(ICreator*& pCreator): ICreator(pCreator) { pCreator = this; } \
virtual ~a##Creator() {} \
virtual const char* Name() { return p; } \
virtual int Type() { return t; } \
virtual ICallee* Create() { return NEW a; } \
virtual void Destroy(ICallee* pCallee) { ::delete pCallee; } }; \
a##Creator a##_Creator(g_pCreator);

#define SCRIPT_CREATOR(a, t) CALLEE_CREATOR(a, #a, t)

#endif // _SERVER_ICREATOR_H

