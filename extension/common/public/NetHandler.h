//--------------------------------------------------------------------
// 文件名:		NetHandler.h
// 内  容:		网络服务处理器
// 说  明:		
// 创建日期:		2016年5月31日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_HANDLER_H__
#define __NET_HANDLER_H__
#include "base/INetService.h"
#include "action/IActionManager.h"
#include "protocols/SysAction.h"

class NetHandler : public INetIoHandler, public ActionManager
{
	SERID m_serid;
public:
	NetHandler(const SERID& serid, IActionHandleLinkManager& dataManager)
		: ActionManager(dataManager)
		, m_serid(serid)
	{

	}

	//--------------------------------------------------------------------
	// INetIoHandler
	//--------------------------------------------------------------------
	virtual void OnConnect( INetConnection* conn )
	{
		assert(NULL != conn);
		SYSActionEnter* pEnter = new SYSActionEnter();
		pEnter->SetPeerHost(conn->RemoteIP());
		pEnter->SetPeerPort(conn->RemotePort());
		pEnter->SetAccept(conn->IsAccept());
		pEnter->SetUD(conn->GetUD());
		TriggerAction(m_serid, conn->GetConnId(), std::shared_ptr<INetAction>( pEnter ));
	}

	virtual void OnClose( INetConnection* conn, EMNetReason reason, int nErrCode )
	{
		assert(NULL != conn);
		SYSActionExit* pExit = new SYSActionExit();
		pExit->SetPeerHost(conn->RemoteIP());
		pExit->SetPeerPort(conn->RemotePort());
		pExit->SetAccept(conn->IsAccept());
		pExit->SetUD(conn->GetUD());
		TriggerAction(m_serid, conn->GetConnId(), std::shared_ptr<INetAction>(pExit));
	}

	virtual bool OnAction(INetConnection* conn, std::shared_ptr<INetAction>& action)
	{
		assert(NULL != conn);
		return TriggerAction(m_serid, conn->GetConnId(), action);
	}

	virtual bool OnUnRegistAction(INetConnection* conn, std::shared_ptr<INetAction>& action)
	{
		assert(NULL != conn);
		return TriggerAction( m_serid, conn->GetConnId(), action );
	}

	virtual bool OnReply(INetConnection* conn, std::shared_ptr<INetAction>& action)
	{
		assert(NULL != conn);
		return TriggerAction( m_serid, conn->GetConnId(), action );
	}

	virtual bool OnTimeout(INetConnection* conn, std::shared_ptr<INetAction>& action)
	{
		NID nid = NULL_NID;
		if (NULL != conn)
		{
			nid = conn->GetConnId();
		}
		return TriggerAction(m_serid, nid, action);
	}

	virtual void OnRawMessage( INetConnection* conn, const char* szMsg, int nLen )
	{
		assert(NULL != conn);
		SYSActionRawData* pAction = new SYSActionRawData();
		BinaryMessage* pData = dynamic_cast<BinaryMessage*>(pAction->Request());
		pData->SetMsgBody( szMsg, nLen );
		TriggerAction(m_serid, conn->GetConnId(), std::shared_ptr<INetAction>(pAction));
	}

	virtual EMNetErrorDealWith OnErrorMessage(INetConnection* conn, const char* szMsg, int nLen)
	{
		assert(NULL != conn);
		SYSActionErrData* pAction = new SYSActionErrData();
		TriggerAction(m_serid, conn->GetConnId(), std::shared_ptr<INetAction>(pAction));
		return EM_NET_ERRROR_DEAL_WITH_BREAK;
	}
};
#endif // END __NET_HANDLER_H__