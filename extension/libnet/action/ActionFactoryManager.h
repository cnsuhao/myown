//--------------------------------------------------------------------
// 文件名:		ActionBaseFactoryManager.h
// 内  容:		动作基本抽象创建工厂
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_FACTORY_MANAGER_H__
#define __I_ACTION_FACTORY_MANAGER_H__

#include "IActionFactory.h"
#include <map>
#include <vector>
#include <string>
#include <list>

class ActionBaseFactoryManager : public IActionAbstractFactory
{
	typedef std::map<std::string, IActionFactory*> KeyFactorySet;
	typedef std::map<int, IActionFactory*> IDFactorySet;

	KeyFactorySet m_keyFactorys;
	IDFactorySet m_idFactorys;
public:
	~ActionBaseFactoryManager()
	{
		for (KeyFactorySet::iterator itr = m_keyFactorys.begin();
					itr != m_keyFactorys.end(); 
					++itr )
		{
			delete itr->second;
		}

		for (IDFactorySet::iterator itr = m_idFactorys.begin();
			itr != m_idFactorys.end();
			++itr)
		{
			delete itr->second;
		}
	}

	bool RegisterFactory(const char* szActionKey, IActionFactory* pFactory)
	{
		KeyFactorySet::_Pairib pb = m_keyFactorys.insert(std::make_pair(std::string(szActionKey), pFactory));
		return pb.second;
	}

	IActionFactory* RomoveFactory( const char* szActionKey )
	{
		IActionFactory* pFactory = NULL;
		KeyFactorySet::iterator itr = m_keyFactorys.find(szActionKey);
		if (itr != m_keyFactorys.end())
		{
			pFactory =  itr->second;
			m_keyFactorys.erase(itr);
		}

		return pFactory;
	}

	IActionFactory* FindFactory(const char* szActionKey) const
	{
		KeyFactorySet::const_iterator itr = m_keyFactorys.find(szActionKey);
		if (itr != m_keyFactorys.end())
		{
			return itr->second;
		}
		return NULL;
	}

	std::list<IActionFactory*> RemoveAllKeyFactorys()
	{
		std::list<IActionFactory*> keyFactorys;

		for (KeyFactorySet::iterator itr = m_keyFactorys.begin();
			itr != m_keyFactorys.end();
			++itr)
		{
			keyFactorys.push_back( itr->second );
		}
		m_keyFactorys.clear();

		return keyFactorys;
	}

	bool RegisterFactory(int nActionId, IActionFactory* pFactory)
	{
		IDFactorySet::_Pairib pb = m_idFactorys.insert(std::make_pair(nActionId, pFactory));
		return pb.second;

	}

	IActionFactory* RomoveFactory(int nActionId)
	{
		IActionFactory* pFactory = NULL;
		IDFactorySet::iterator itr = m_idFactorys.find(nActionId);
		if (itr != m_idFactorys.end())
		{
			pFactory = itr->second;
			m_idFactorys.erase(itr);
		}

		return pFactory;

	}

	IActionFactory* FindFactory( int nActionId ) const
	{
		IDFactorySet::const_iterator itr = m_idFactorys.find(nActionId);
		if (itr != m_idFactorys.end())
		{
			return itr->second;
		}
		return NULL;
	}

	std::list<IActionFactory*> RemoveAllIDFactorys()
	{
		std::list<IActionFactory*> idFactorys;

		for (IDFactorySet::iterator itr = m_idFactorys.begin();
			itr != m_idFactorys.end();
			++itr)
		{
			idFactorys.push_back(itr->second);
		}
		m_idFactorys.clear();

		return idFactorys;
	}


	virtual IAction* CreateAction(int nActionID)
	{
		IActionFactory* pFactory = FindFactory(nActionID);
		if (NULL != pFactory)
		{
			return pFactory->CreateAction();
		}

		return NULL;
	}

	virtual IAction* CreateAction(const char* szActionKey)
	{
		IActionFactory* pFactory = FindFactory( szActionKey );
		if (NULL != pFactory)
		{
			return pFactory->CreateAction();
		}

		return NULL;
	}

	virtual void FreeAction( IAction* action )
	{
		IActionFactory* pFactory = FindFactory( action->ActionId() );
		if (NULL != pFactory)
		{
			pFactory->FreeAction( action );
		}
		else
		{
			IActionFactory* pFactory = FindFactory( action->ActionKey() );
			if (NULL != pFactory)
			{
				pFactory->FreeAction(action);
			}
			else
			{
				delete action;
			}
		}
	}
};


class ActionSeqFactoryManager : public IActionAbstractFactory
{
	typedef std::vector<IActionFactory*> FactorySet;

	FactorySet m_idFactorys;
public:
	~ActionSeqFactoryManager()
	{
		for (FactorySet::iterator itr = m_idFactorys.begin();
			itr != m_idFactorys.end();
			++itr)
		{
			delete *itr;
		}
	}

	void InitSeqSize( size_t nMaxActionId )
	{
		m_idFactorys.resize(nMaxActionId, NULL);
	}

	bool RegisterFactory(int nActionId, IActionFactory* pFactory)
	{
		if ((size_t)nActionId >= m_idFactorys.size)
		{
			return false;
		}

		if (NULL != m_idFactorys[nActionId])
		{
			return false;
		}

		m_idFactorys[nActionId] = pFactory;
		return true;

	}

	IActionFactory* RomoveFactory(int nActionId)
	{
		if ((size_t)nActionId >= m_idFactorys.size)
		{
			return NULL;
		}

		m_idFactorys[nActionId] = NULL;
		return	m_idFactorys[nActionId];
	}

	IActionFactory* FindFactory(int nActionId) const
	{
		if ((size_t)nActionId >= m_idFactorys.size)
		{
			return NULL;
		}

		return	m_idFactorys[nActionId];
	}

	std::list<IActionFactory*> RemoveAllFactorys()
	{
		std::list<IActionFactory*> factorys;

		for ( size_t i = 0; i < m_idFactorys.size(); ++i )
		{
			if (NULL != m_idFactorys[i])
			{
				factorys.push_back(m_idFactorys[i]);
				m_idFactorys[i] = NULL;
			}
		}

		return factorys;
	}


	virtual IAction* CreateAction( int nActionID )
	{
		IActionFactory* pFactory = FindFactory(nActionID);
		if (NULL != pFactory)
		{
			return pFactory->CreateAction();
		}

		return NULL;
	}

	virtual IAction* CreateAction(const char* szActionKey)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void FreeAction(IAction* action)
	{
		IActionFactory* pFactory = FindFactory(action->ActionId());
		if (NULL != pFactory)
		{
			pFactory->FreeAction(action);
		}
		else
		{
			delete action;
		}
	}
};


#endif // END __I_ACTION_FACTORY_H__