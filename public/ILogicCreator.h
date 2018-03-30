//--------------------------------------------------------------------
// 文件名:		ILogicCreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_ILOGICCREATOR_H
#define _PUBLIC_ILOGICCREATOR_H

#include "Macros.h"

// ILogicCreator
// 逻辑创建器

class ILogic;

// 逻辑方法描述
struct ILogicCallback
{
	const char* m_strName;
	void* m_pMidFunc;
	bool m_bReturnTable;
	ILogicCallback* m_pNext;
};

class ILogicCreator
{
public:
	ILogicCreator(ILogicCreator* pNext)
	{  
		m_pNext = pNext; 
		m_pCallback = NULL;
	}

	virtual ~ILogicCreator() = 0;

	// 返回名字空间
	virtual const char* GetSpace() const = 0;

	// 返回名称
	virtual const char* GetName() const = 0;

	// 创建
	virtual ILogic* Create() = 0;
	
	// 删除
	virtual void Destroy(ILogic* p) = 0;
	
	// 获得下一个
	ILogicCreator* GetNext() const { return m_pNext; }

	// 获得方法链表
	ILogicCallback* GetCallbackLink() const { return m_pCallback; }

	// 设置方法链表
	void SetCallbackLink(ILogicCallback* value) { m_pCallback = value; }

private:
	ILogicCreator();

private:
	ILogicCreator* m_pNext;
	ILogicCallback* m_pCallback;
};

inline ILogicCreator::~ILogicCreator() {}

#endif // _PUBLIC_ILOGICCREATOR_H

