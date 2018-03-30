//--------------------------------------------------------------------
// 文件名:		ILogic.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_ILOGIC_H
#define _PUBLIC_ILOGIC_H

#include "Macros.h"
#include "ILogicInfo.h"
#include "ILogicCreator.h"

// ILogic
// 实体接口

class IEntity;

class ILogic
{
public:
	ILogic()
	{
		m_pEntity = NULL;
		m_pLogicInfo = NULL;
	}

	virtual ~ILogic() = 0;

	// 初始化
	virtual bool Init(const IVarList& args) { return true; }
	// 关闭
	virtual bool Shut() { return true; }
	
	// 释放自身
	virtual void Release()
	{
		m_pLogicInfo->GetCreator()->Destroy(this);
	}
	
	// 获得绑定对象
	IEntity* GetEntity() const
	{
		return m_pEntity;
	}
	
	// 获得逻辑类信息
	ILogicInfo* GetLogicInfo() const
	{
		return m_pLogicInfo;
	}

private:
	ILogic(const ILogic&);
	ILogic& operator=(const ILogic&);
	
	void SetEntity(IEntity* value)
	{
		m_pEntity = value;
	}
	
	void SetLogicInfo(ILogicInfo* value)
	{
		m_pLogicInfo = value;
	}

private:
	IEntity* m_pEntity;
	ILogicInfo* m_pLogicInfo;

	friend class CCore;
};

inline ILogic::~ILogic() {}

#endif // _PUBLIC_ILOGIC_H

