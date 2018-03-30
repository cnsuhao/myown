//--------------------------------------------------------------------
// 文件名:		IEntity.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_IENTITY_H
#define _PUBLIC_IENTITY_H

#include "Macros.h"
#include "PersistId.h"
#include "ICore.h"
#include "IVarTable.h"

// 实体接口

class IEntInfo;
class IEntScript;
//class IVar;

class IEntity
{
public:
	// 内部用于验证父类的有效性，不可调用
	static void __validate_parent_exists() {}

public:
	IEntity()
	{
		m_pCore = NULL;
		m_pCustoms = NULL;
		//m_pVar = NULL;
		m_pEntInfo = NULL;
		m_pEntScript = NULL;
		m_nMsgHooks = 0;
		m_bDeleted = false;
		m_bCanDelByScript = false;
	}

	virtual ~IEntity() = 0;

	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args) = 0;
	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut() = 0;
	
	// 每帧执行逻辑（如果已添加到运行队列）
	virtual void Execute(float seconds) {}
	// 每帧执行显示（如果已添加到运行队列）
	virtual void Display(float offset_seconds) {}

	// 窗口消息处理
	virtual bool MsgProc(void* hwnd, unsigned int msg, unsigned int param1, 
		unsigned int param2, int& result) { return false; }

	// 释放自身
	virtual void Release()
	{
		m_pCore->DeleteEntity(GetID());
	}

	// 获得内存占用
	virtual size_t GetMemoryUsage() { return 0; }
	
	// 获得核心接口
	ICore* GetCore() const
	{
		return m_pCore;
	}

	// 获得自定义属性表
	IVarTable* GetCustoms() const
	{
		return m_pCustoms;
	}
	//IVar* GetVar() const
	//{
	//	return m_pVar;
	//}
	
	// 获得实体类信息
	IEntInfo* GetEntInfo() const
	{
		return m_pEntInfo;
	}

	// 获得绑定的脚本
	IEntScript* GetScript() const
	{
		return m_pEntScript;
	}
	
	// 获得对象ID
	PERSISTID GetID() const
	{
		return m_PersistID;
	}

	// 是否已删除
	bool GetDeleted() const
	{
		return m_bDeleted;
	}

	// 是否可以被脚本删除
	bool GetCanDelByScript() const
	{
		return m_bCanDelByScript;
	}

private:
	IEntity(const IEntity&);
	IEntity& operator=(const IEntity&);
	
	void SetCore(ICore* value)
	{
		m_pCore = value;
	}
	
	void SetCustoms(IVarTable* value)
	{
		m_pCustoms = value;
	}

	//void SetVar(IVar* value)
	//{
	//	m_pVar = value;
	//}
	
	void SetEntInfo(IEntInfo* value)
	{
		m_pEntInfo = value;
	}

	void SetScript(IEntScript* value)
	{
		m_pEntScript = value;
	}
	
	void SetID(const PERSISTID& value)
	{
		m_PersistID = value;
	}

	void SetDeleted(bool value)
	{
		m_bDeleted = value;
	}

	void SetCanDelByScript(bool value)
	{
		m_bCanDelByScript = value;
	}

	void IncMsgHooks()
	{
		++m_nMsgHooks;
	}

	void DecMsgHooks()
	{
		--m_nMsgHooks;
	}

	int GetMsgHooks() const
	{
		return m_nMsgHooks;
	}
	
private:
	ICore* m_pCore;
	IVarTable* m_pCustoms;
	//IVar* m_pVar;
	IEntInfo* m_pEntInfo;
	IEntScript* m_pEntScript;
	int m_nMsgHooks;
	bool m_bDeleted;
	bool m_bCanDelByScript;
	PERSISTID m_PersistID;

	friend class CEntFactory;
};

inline IEntity::~IEntity() {}

#endif // _PUBLIC_IENTITY_H

