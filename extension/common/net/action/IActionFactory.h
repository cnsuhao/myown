//--------------------------------------------------------------------
// 文件名:		IActionFactory.h
// 内  容:		动作创建工厂
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_FACTORY_H__
#define __I_ACTION_FACTORY_H__

#include "IAction.h"
#include <functional>
#include <stack>

class IActionFactory
{
public:
	virtual ~IActionFactory() = 0;

	virtual IAction* CreateAction() = 0;
	virtual void  FreeAction(IAction* action) = 0;
};
inline IActionFactory::~IActionFactory() {}

class IActionAbstractFactory
{
public:
	virtual ~IActionAbstractFactory() = 0;

	virtual IAction* CreateAction( int nActionID ) = 0;
	virtual IAction* CreateAction( const char* szActionKey ) = 0;

	virtual void FreeAction( IAction* action ) = 0;
};
inline IActionAbstractFactory::~IActionAbstractFactory(){}


class ActionCustomFactory : IActionFactory
{
protected:
	typedef std::function<IAction*()> FuncCreator;
	typedef std::function<void(IAction*)> FuncDestory;

	FuncCreator	m_funcCreator;
	FuncDestory	m_funcDestory;

public:
	ActionCustomFactory(FuncCreator creator, FuncDestory destory)
		: m_funcCreator( creator )
		, m_funcDestory( destory )
	{

	}

	void SetFuncCreator(FuncCreator creator)
	{
		m_funcCreator = creator;
	}

	void SetFuncDestory(FuncDestory destory)
	{
		m_funcDestory = destory;
	}

	virtual IAction* CreateAction()
	{
		return m_funcCreator();
	}

	virtual void FreeAction(IAction* action)
	{
		m_funcDestory(action);
	}
};

class ActionPoolsFactory : public ActionCustomFactory
{
	typedef std::function<void(IAction*)> FuncRelease;
	std::stack<IAction*> m_pools;

	FuncRelease m_funcRelease;
	size_t m_nPoolSize;
public:
	ActionPoolsFactory(FuncCreator creator, FuncDestory destory, FuncRelease release, size_t nPoolSize = 1024 )
		: ActionCustomFactory( creator, destory )
		, m_funcRelease(release)
		, m_nPoolSize( nPoolSize )
	{

	}

	ActionPoolsFactory(FuncCreator creator, FuncDestory destory, size_t nPoolSize = 1024)
		: ActionCustomFactory(creator, destory)
		, m_nPoolSize(nPoolSize)
	{

	}

	~ActionPoolsFactory()
	{
		while ( m_pools.size() > 0 )
		{
			IAction* pAction = m_pools.top();
			m_pools.pop();

			if ( m_funcRelease )
				m_funcRelease(pAction);
			m_funcDestory(pAction);
		}
	}

	void SetFuncRelease(FuncRelease release)
	{
		m_funcRelease = release;
	}

	virtual IAction* CreateAction()
	{
		if (m_pools.size() > 0)
		{
			IAction* pAction = m_pools.top();
			m_pools.pop();
			return pAction;
		}

		return ActionCustomFactory::CreateAction();
	}

	virtual void FreeAction( IAction* action )
	{
		if (m_pools.size() >= m_nPoolSize)
		{
			ActionCustomFactory::FreeAction(action);
		}
		else
		{
			if (m_funcRelease)
			{
				m_funcRelease(action);
			}

			m_pools.push(action);
		}
	}
};

template<typename T>
class TActionPoolsFactory : public IActionFactory
{
	std::stack<T*> m_pools;
	size_t m_nPoolSize;

public:
	TActionPoolsFactory( size_t nPoolSize = 1024 )
		: m_nPoolSize(nPoolSize)
	{
	}
	~TActionPoolsFactory()
	{
		while (m_pools.size() > 0)
		{
			T* pAction = m_pools.top();
			m_pools.pop();
			delete pAction;
		}
	}

	virtual IAction* CreateAction()
	{
		if (m_pools.size() > 0)
		{
			T* pAction = m_pools.top();
			m_pools.pop();
			pAction = new (pAction)T();
			return pAction;
		}
		return new T();
	}

	virtual void FreeAction(IAction* action)
	{
		if (m_pools.size() >= m_nPoolSize)
		{
			delete action;
		}
		else
		{
			T* p = dynamic_cast<T*>(action);
			if ( NULL != p )
			{
				p->~T();
				m_pools.push(p);
			}
			else
			{
				delete action;
			}
		}
	}
};

#endif // END __I_ACTION_FACTORY_H__