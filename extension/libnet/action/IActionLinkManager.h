//--------------------------------------------------------------------
// 文件名:		IActionLinkManager.h
// 内  容:		动作处理集管理器
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_HANDLE_LINK_MANAGER_H__
#define __I_ACTION_HANDLE_LINK_MANAGER_H__

#include "IActionHandler.h"
#include <map>
#include <vector>
#include <string>
#include <list>

class IActionHandleLinkManager
{
public:
	virtual ~IActionHandleLinkManager() = 0;
	virtual IActionHandlerLink* Lookup(int nActionID) const = 0;
	virtual IActionHandlerLink* Lookup(const char* szActionKey) const = 0;

	virtual IActionHandlerLink* Get( int nActionID ) = 0;
	virtual IActionHandlerLink* Get( const char* szActionKey ) = 0;

	virtual void Clear() = 0;
};
inline IActionHandleLinkManager::~IActionHandleLinkManager(){};

class ActionHandleLinkManager : public IActionHandleLinkManager
{
	typedef std::map<std::string, IActionHandlerLink*>	KeyActionSet;
	typedef std::map<int, IActionHandlerLink*>			IDActionSet;

	KeyActionSet m_keySet;
	IDActionSet m_idSet;
public:

	~ActionHandleLinkManager()
	{
		Clear();
	}

	virtual IActionHandlerLink* Lookup(int nActionID) const
	{
		IDActionSet::const_iterator itr = m_idSet.find(nActionID);
		if (itr != m_idSet.end())
		{
			return itr->second;
		}
		return NULL;
	}

	virtual IActionHandlerLink* Lookup(const char* szActionKey) const
	{
		KeyActionSet::const_iterator itr = m_keySet.find( szActionKey );
		if (itr != m_keySet.end())
		{
			return itr->second;
		}
		return NULL;
	}

	virtual IActionHandlerLink* Get(int nActionID)
	{
		IActionHandlerLink* pLink = Lookup(nActionID);
		if (NULL == pLink)
		{
			pLink = CreateLink();
			m_idSet[nActionID] = pLink;
		}

		return pLink;
	}

	virtual IActionHandlerLink* Get(const char* szActionKey)
	{
		IActionHandlerLink* pLink = Lookup(szActionKey);
		if (NULL == pLink)
		{
			pLink = CreateLink();
			m_keySet[szActionKey] = pLink;
		}

		return pLink;
	}

	virtual void Clear()
	{
		for (KeyActionSet::iterator itr = m_keySet.begin();
			itr != m_keySet.end();
			++itr)
		{
			IActionHandlerLink* pLink = itr->second;
			FreeLink(pLink);
		}
		m_keySet.clear();

		for (IDActionSet::iterator itr = m_idSet.begin();
			itr != m_idSet.end();
			++itr)
		{
			IActionHandlerLink* pLink = itr->second;
			FreeLink(pLink);
		}
		m_idSet.clear();
	}

protected:
	virtual IActionHandlerLink* CreateLink()
	{
		return new ActionHandlerLink();
	}

	virtual void FreeLink(IActionHandlerLink* handlerLink)
	{
		if (NULL != handlerLink)
		{
			delete handlerLink;
		}
	}
};


class ActionDataSeqManager : public IActionHandleLinkManager
{
	typedef std::vector<IActionHandlerLink*> ActionDataSet;
	mutable ActionDataSet m_dataSet;
public:
	ActionDataSeqManager(size_t nMaxActionId)
	{
		for (size_t i = 0; i < nMaxActionId; ++i)
		{
			IActionHandlerLink* pLink = CreateLink();
			m_dataSet.push_back(pLink);
		}
	}

	~ActionDataSeqManager()
	{
		Clear();
	}

	virtual IActionHandlerLink* Lookup(int nActionID) const
	{
		if ((size_t)nActionID >= m_dataSet.size())
		{
			return NULL;
		}

		return m_dataSet[nActionID];
	}

	virtual IActionHandlerLink* Lookup(const char* szActionKey) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual IActionHandlerLink* Get(int nActionID)
	{
		return Lookup(nActionID);
	}

	virtual IActionHandlerLink* Get(const char* szActionKey)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void Clear()
	{
		for (ActionDataSet::iterator itr = m_dataSet.begin();
			itr != m_dataSet.end();
			++itr)
		{
			IActionHandlerLink* pLink = *itr;
			FreeLink(pLink);
		}
		m_dataSet.clear();
	}

protected:
	virtual IActionHandlerLink* CreateLink()
	{
		return new ActionHandlerLink();
	}

	virtual void FreeLink(IActionHandlerLink* handlerLink)
	{
		if (NULL != handlerLink)
		{
			delete handlerLink;
		}
	}
};


#endif // END __I_ACTION_HANDLE_LINK_MANAGER_H__