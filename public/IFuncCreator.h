//--------------------------------------------------------------------
// 文件名:		IFuncCreator.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_IFUNCCREATOR_H
#define _PUBLIC_IFUNCCREATOR_H

#include "Macros.h"

// IFuncCreator
// 脚本扩展函数创建器

class IFuncCreator
{
public:
	IFuncCreator(IFuncCreator* pNext) { m_pNext = pNext; }
	
	virtual ~IFuncCreator() = 0;

	const char* m_strName;
	void* m_pMidFunc;
	bool m_bReturnTable;
	
	// 获得下一个
	IFuncCreator* GetNext() const { return m_pNext; }

private:
	IFuncCreator();

private:
	IFuncCreator* m_pNext;
};

inline IFuncCreator::~IFuncCreator() {}

#endif // _PUBLIC_IFUNCCREATOR_H

