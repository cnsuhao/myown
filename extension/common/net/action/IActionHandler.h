//--------------------------------------------------------------------
// 文件名:		IActionHandler.h
// 内  容:		动作处理器
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_HANDLER_H__
#define __I_ACTION_HANDLER_H__
#include "base/INetType.h"
#include "INetAction.h"
#include <functional>
#include <memory>

typedef std::function<void(const SERID&, const SESSID&, std::shared_ptr<INetAction>&)> ACTION_EVENT;
class IActionHandler
{
public:
	IActionHandler() : m_nPriority(0)
	{
	}
	virtual ~IActionHandler() = 0;

	virtual void OnAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action) = 0;
	virtual int GetPriority() const
	{
		return m_nPriority;
	}
	virtual void SetPriority(int nPriority)
	{
		m_nPriority = nPriority;
	}

protected:
	int m_nPriority;
};
inline IActionHandler::~IActionHandler() {}

class ActionHandler : public IActionHandler
{
protected:
	ACTION_EVENT m_innerAction;

public:
	ActionHandler(ACTION_EVENT action) : m_innerAction(action)
	{

	}

	virtual void OnAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action)
	{
		m_innerAction( from, sid, action );
	}
};

class IActionHandlerLink
{
public:
	virtual ~IActionHandlerLink() = 0;
	virtual	void PushHandler(IActionHandler* handler) = 0;
	virtual	void RemoveHandler(IActionHandler* handler) = 0;
	virtual	void OnAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action)  const = 0;
	virtual size_t GetSize() const = 0;
};
inline IActionHandlerLink::~IActionHandlerLink(){}

class ActionHandlerLink : public IActionHandlerLink
{
public: 
	typedef std::list<IActionHandler*> LIST;
protected:
	LIST m_listHandlers;

public:
	~ActionHandlerLink()
	{
		for (LIST::iterator itr = m_listHandlers.begin(); itr != m_listHandlers.end(); ++itr)
		{
			delete *itr;
		}
		m_listHandlers.clear();
	}
	const LIST& GetLinkList() const
	{
		return m_listHandlers;
	}

	void PushHandler( IActionHandler* handler )
	{
		m_listHandlers.push_back(handler);
		m_listHandlers.sort([](IActionHandler* a, IActionHandler* b) -> bool {
			return	a->GetPriority() < b->GetPriority();
		});
	}

	void RemoveHandler(IActionHandler* handler)
	{
		m_listHandlers.remove(handler);
	}

	void OnAction(const SERID& from, const SESSID& sid, std::shared_ptr<INetAction>& action) const
	{
		LIST::const_iterator itr = m_listHandlers.begin();
		for (LIST::const_iterator next = itr; itr != m_listHandlers.end(); itr = next)
		{
			++next;
			(*itr)->OnAction( from, sid, action );
		}
	}

	virtual size_t GetSize() const
	{
		return m_listHandlers.size();
	}
};

#endif // END __I_ACTION_HANDLER_H__