//--------------------------------------------------------------------
// 文件名:		IIntCreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_IINTCREATOR_H
#define _PUBLIC_IINTCREATOR_H

#include "Macros.h"

// IIntCreator
// 功能接口创建器

class IInterface;

class IIntCreator
{
public:
	IIntCreator(IIntCreator* pNext) { m_pNext = pNext; }
	
	virtual ~IIntCreator() = 0;

	// 返回名字空间
	virtual const char* GetSpace() const = 0;
	
	// 返回名称
	virtual const char* GetName() const = 0;
	
	// 创建
	virtual IInterface* Create() = 0;
	
	// 删除
	virtual void Destroy(IInterface* p) = 0;
	
	// 获得下一个
	IIntCreator* GetNext() const { return m_pNext; }

private:
	IIntCreator();

private:
	IIntCreator* m_pNext;
};

inline IIntCreator::~IIntCreator() {}

#endif // _PUBLIC_IINTCREATOR_H

