//--------------------------------------------------------------------
// 文件名:		IActionManager.h
// 内  容:		动作管理器
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_MANAGER_H__
#define __I_ACTION_MANAGER_H__

#include "utils/FsLockedQueue.h"
#include "utils/Binder.h"
#include "base/INetType.h"
#include "IAction.h"
#include "IActionLinkManager.h"
#include "IActionBinder.h"

#define ACTION_BINDER FS_BINDER_3
#define ASYNC_ACTION_COMPLETED_BINDER FS_BINDER_3

typedef std::function<void(const SERID&, const SESSID&, std::shared_ptr<INetAction>&)> ACTION_ASYNC_COMPLETED_HANDLE;
class IActionManager : public IActionBinder
{
public:
	virtual ~IActionManager() = 0;

	virtual bool TriggerAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action) = 0;
	virtual void SetUnBindAction(ACTION_EVENT handler) = 0;
	virtual void SetAsyncCompletedHandle(ACTION_ASYNC_COMPLETED_HANDLE handle) = 0;
};
inline IActionManager::~IActionManager() {}

class ActionManager : public IActionManager
{
protected:
	struct AsyncActionData
	{
		SERID						From;
		SESSID						FromSession;
		std::shared_ptr<INetAction>	RealAction;
		IActionHandlerLink*			Handlers;
	};

	IActionHandleLinkManager&		m_linkManager;

	FsLockedQueue<AsyncActionData*>	m_asyncActions;

	ACTION_ASYNC_COMPLETED_HANDLE	m_asyncCompletedHandle;
	ACTION_EVENT					m_unbindHandle;
public:
	ActionManager(IActionHandleLinkManager& linkManager)
		: m_linkManager(linkManager)
	{

	}

	~ActionManager()
	{
		Clear();
	}

	virtual void SetAsyncCompletedHandle(ACTION_ASYNC_COMPLETED_HANDLE handle)
	{
		m_asyncCompletedHandle = handle;
	}

	virtual void SetUnBindAction(ACTION_EVENT handler)
	{
		m_unbindHandle = handler;
	}

	virtual bool TriggerAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action)
	{
		IActionHandlerLink* pLink = NULL;
		if (action->ActionId() != 0)
		{
			pLink = m_linkManager.Lookup(action->ActionId());
		}
		if (NULL == pLink)
		{
			const char* szKey = action->ActionKey();
			if (NULL != szKey && szKey[0] != 0)
			{
				pLink = m_linkManager.Lookup(szKey);
			}
		}

		if ( action->IsAsync() )
		{
			AsyncActionData* ad = new AsyncActionData();
			ad->Handlers = pLink;
			ad->RealAction = action;
			ad->From = from;
			ad->FromSession = sid;

			m_asyncActions.EnQueue(ad);
			return false;
		}
		else
		{
			OnSyncAction(from, sid, action, pLink);
			return true;
		}
	}
	


	virtual bool BindAction(int nActionId, ACTION_EVENT action, int nPriority)
	{
		if ( 0 == nActionId )
		{
			return false;
		}

		IActionHandlerLink* pLink = m_linkManager.Get( nActionId );
		if (NULL == pLink )
		{
			return false;
		}

		Bind( pLink, action, nPriority );
		return true;
	}
	virtual bool BindAction(const char* szActionKey, ACTION_EVENT action, int nPriority)
	{
		if (szActionKey == NULL || szActionKey[0] == 0) 
			return false;
		IActionHandlerLink* pLink = m_linkManager.Get( szActionKey );
		if (NULL == pLink )
		{
			return false;
		}

		Bind(pLink, action, nPriority);
		return true;
	}

	virtual void Process()
	{
		AsyncActionData* pd = NULL;
		while (m_asyncActions.DeQueue(pd))
		{
			if (NULL != pd && NULL != pd->RealAction.get())
			{
				std::shared_ptr<INetAction>& pAction = pd->RealAction;
				IActionHandlerLink* pLink = pd->Handlers;
				OnAsyncAction(pd->From, pd->FromSession, pAction, pLink);
			}

			if (NULL != pd)
			{
				delete pd;
				pd = NULL;
			}
		}
	}

	virtual void Clear()
	{
		m_linkManager.Clear();
		AsyncActionData* pd = NULL;
		while (m_asyncActions.DeQueue(pd))
		{
			if (NULL != pd)
			{
				delete pd;
				pd = NULL;
			}
		}
	}

protected:
	virtual void Bind(IActionHandlerLink* pLink, ACTION_EVENT action, int nPriority)
	{
		IActionHandler* pHandler = new ActionHandler(action);
		pHandler->SetPriority(nPriority);
		pLink->PushHandler(pHandler);
	}

	virtual void OnAsyncAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& pAction, IActionHandlerLink* pLink)
	{
		if (NULL != pLink && pLink->GetSize() > 0)
		{
			pLink->OnAction( from, sid, pAction );
			if (m_asyncCompletedHandle)
			{
				m_asyncCompletedHandle(from, sid, pAction);
			}
		}
		else
		{
			if (m_unbindHandle)
			{
				m_unbindHandle(from, sid, pAction);
			}
		}
	}

	virtual void OnSyncAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& pAction, IActionHandlerLink* pLink)
	{
		if (NULL != pLink && pLink->GetSize() > 0)
		{
			pLink->OnAction(from, sid, pAction);
		}
		else
		{
			if (m_unbindHandle)
			{
				m_unbindHandle(from, sid, pAction);
			}
		}
	}
};

#endif // END __I_ACTION_MANAGER_H__