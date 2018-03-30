#include "EntRpc.h"
#include <stdio.h>
#include "public/Module.h"
#include "FsLogger.h"
#include "EntNetManager.h"
#include "RpcHandler.h"
#include "EntHttp.h"
#include "SoloGmcc.h"
#include "utils/string_util.h"
#include "PayedOrder.h"
#include "ProtocolsID.h"
#include "../app/FsAppHelper.h"

EntRpc::EntRpc() 
	: m_pGmcc( NULL )
	, m_pHttp( NULL )
{
	m_pThread = new CThread(EntRpc::RpcThread, NULL, NULL, this, 0, 0);
}

EntRpc::~EntRpc()
{
	if (!m_pThread->GetQuit())
	{
		m_pThread->Stop();
	}

	delete m_pThread;
}

DECLARE_ENTITY("FsGmcc", EntRpc, EntNetBase)

bool EntRpc::Init(const IVarList& args)
{
	FS_LOG_INFO("EntRpc::Init(const IVarList& args)");
	boost::shared_ptr<RpcHandler> rpcHandler(new RpcHandler());
	m_rpcHandler.swap(rpcHandler);
	return true;
}

bool EntRpc::Shut()
{
	FS_LOG_INFO("EntRpc::Shut()");
	return true;
}

bool EntRpc::Startup()
{
	IEntity* pEntity = GetCore()->LookupEntity("EntHttp");
	m_pHttp = dynamic_cast<EntHttp*>(pEntity);

	pEntity = GetCore()->LookupEntity("SoloGmcc");
	m_pGmcc = dynamic_cast<SoloGmcc*>(pEntity);
	if (NULL == m_pHttp)
	{
		FS_LOG_FATAL("not found http service.");
		return false;
	}

	if (NULL == m_pGmcc)
	{
		FS_LOG_FATAL("not found rpc service.");
		return false;
	}

	GetCore()->AddExecute(this);
	m_rpcHandler->SetRpcServer(this);
	m_rpcHandler->SetGameInfo(&m_gameInfo);

	return true;
}

bool EntRpc::Stop()
{
	StopRpc();

	m_rpcHandler->SetRpcServer(NULL);
	m_rpcHandler->SetGameInfo(NULL);
	GetCore()->RemoveExecute(this);
	m_pHttp = NULL;
	m_pGmcc = NULL;

	return EntNetBase::Stop();
}

void EntRpc::Execute(float seconds)
{
	EntNetBase::Execute(seconds);
	m_mainQueue.RunQueue();
}

int EntRpc::GetNetKey() const
{
	return EM_NET_KEY_ECHO;
}

GameInfo& EntRpc::GetGameInfo()
{
	return m_gameInfo;
}

const GameInfo& EntRpc::GetGameInfo() const
{
	return m_gameInfo;
}

bool EntRpc::StartRpc()
{
	if ( m_rpcServer.IsRunning() )
	{
		return true;
	}

	m_rpcServer.Start(m_rpcHandler, m_nPort);

	// async	
	return m_pThread->Start();
}

bool EntRpc::StopRpc()
{
	if (m_rpcServer.IsRunning())
	{
		m_rpcServer.Stop();
	}

	return m_pThread->Stop();
}

void EntRpc::OnRpcCommand(const CmdInfo& cmd)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc command(cmd:%s)", cmd.command.c_str());
		if (NULL != m_pGmcc)
		{
			m_pGmcc->SendGameCommand(cmd);
		}
	});
}

void EntRpc::OnRpcNotify(const NotifyInfo& notify)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc notify(type:%d)", notify.NotifyType);
		FS_LOG_DEBUG_FORMAT("on rpc notify(content:%s)", notify.Content.c_str());
		switch (notify.NotifyType)
		{
		case EM_NOTIFY_NOTICE:
			if (notify.OpType == EM_NOTIFY_OP_DEL)
			{
				if (NULL != m_pGmcc)
				{
					int64_t nNoticeId = StringUtil::StringAsInt64(notify.Content.c_str());
					m_pGmcc->DeleteNotice(nNoticeId);
				}
			}
			else
			{
				if (NULL != m_pHttp)
				{
					m_pHttp->OnReqNotify(notify);
				}
			}
			break;
		case EM_NOTIFY_PAYED:
			if (NULL != m_pHttp) {
				// 
				FS_LOG_FATAL("don't support payed notify, please use new interface.");
			}
			break;
		case EM_NOTIFY_CONFIG:
			//if (NULL != m_pHttp) {
			//	try
			//	{
			//		int64_t nType_ = StringUtil::StringAsInt64(notify.Content.c_str());
			//		if (nType_ == EM_CONFIG_CATALOG_ACTIVITY)
			//		{
			//			m_pHttp->OnReqConfig();
			//		}
			//		else if (nType_ == EM_CONFIG_CATALOG_MALL)
			//		{
			//			m_pHttp->OnReqMallItems();
			//		}
			//		else
			//		{
			//			FS_LOG_FATAL_FORMAT("don't support config type:%d.", nType_);
			//		}
			//	}
			//	catch (...)
			//	{
			//		FS_LOG_FATAL_FORMAT("rpc config parse exception(content:%s).", notify.Content.c_str());
			//	}
			//}
			FS_LOG_FATAL("don't support config notify, please use new interface.");
			break;
		default:
			break;
		}
	});
}

void EntRpc::OnRpcMail(const MailData& data)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc mail(title:%s)", data.Title.c_str());
		if (NULL != m_pGmcc)
		{
			m_pGmcc->SendMail(data);
		}
	});
}

void EntRpc::OnRpcPropQuery(const CmdPropQuery& query)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc prop query(role id:%s)", query.roleId.c_str());

		bool bOk = false;
		if (NULL != m_pGmcc)
		{
			bOk = m_pGmcc->QueryPlayerProps(query);
		}

		if (!bOk)
		{
			CmdPropData data;
			data.queryId = query.queryId;
			data.result = DATA_QUERY_SERVER_DISCONNECT;
			data.roleId = query.roleId;
			OnPropData(data);
		}
	});
}

void EntRpc::OnRpcRecordQuery(const CmdRecordQuery& query)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc record query(roleid:%s name:%s)", query.roleId.c_str(), query.recordName.c_str());

		bool bOk = false;
		if (NULL != m_pGmcc)
		{
			bOk = m_pGmcc->QueryPlayerRecordData(query);
		}

		if (!bOk)
		{
			CmdRecordData data;
			data.queryId = query.queryId;
			data.result = DATA_QUERY_SERVER_DISCONNECT;
			data.roleId = query.roleId;
			data.offset = query.offset;
			data.count = query.count;
			data.cols = 0;
			OnRecordData(data);
		}
	});
}

void EntRpc::OnRpcPayment(const Payment& payment)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO_FORMAT("on rpc payment(role:%s order:%s product:%s)",
					payment.RoleId.c_str(), payment.OrderId.c_str(), payment.ProductId.c_str());

		if (NULL != m_pGmcc)
		{
			m_pGmcc->SendPayment(payment);
		}
		else
		{
			FS_LOG_ERROR_FORMAT("gmcc server not initied on rpc payment(role:%s order:%s product:%s)",
				payment.RoleId.c_str(), payment.OrderId.c_str(), payment.ProductId.c_str());
		}
	});
}

void EntRpc::OnRpcGoods()
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO("on rpc update goods");

		if (NULL != m_pHttp)
		{
			m_pHttp->OnReqProducts();
		}
		else
		{
			FS_LOG_ERROR( "http not initied on rpc update goods" );
		}
	});
}

void EntRpc::OnRpcMallItems()
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO("on rpc update goods");

		if (NULL != m_pHttp)
		{
			m_pHttp->OnReqMallItems();
		}
		else
		{
			FS_LOG_ERROR("http not initied on rpc mall items");
		}
	});
}

void EntRpc::OnRpcConfig(int nType, int nConfigId)
{
	m_mainQueue.RunInQueueThread([=]()->void{
		FS_LOG_INFO("on rpc update goods");

		if (NULL != m_pHttp)
		{
			m_pHttp->OnReqConfig( nType );
		}
		else
		{
			FS_LOG_ERROR("http not initied on rpc update goods");
		}
	});
}


void EntRpc::OnPropData(const CmdPropData& data)
{
	m_rpcHandler->OnPropData(data);
}

void EntRpc::OnRecordData(const CmdRecordData& data)
{
	m_rpcHandler->OnRecordData(data);
}

void EntRpc::OnSetServerHeartbeat(int nHeatBeat)
{
	m_gameInfo.SetCheckVal(nHeatBeat);
}

void EntRpc::OnSetGameName(const char* pszName)
{
	m_gameInfo.SetName(pszName);
}

void EntRpc::OnSetGameAreaId(long long nAreaId)
{
	m_gameInfo.SetAreaId(nAreaId);
}

void EntRpc::OnReportState(EmMemberState state)
{
	m_gameInfo.SetState(state);
}

void EntRpc::OnReportOnline(int nCount)
{
	m_gameInfo.SetOnlineCount(nCount);
}

void EntRpc::OnReportMemberInfo(const MemberInfo& info)
{
	m_gameInfo.SetMember( info.name.c_str(), info.memberId, info.state, info.workState );
}

void EntRpc::OnReportDeadAll()
{
	m_gameInfo.DeadAll();
}

void EntRpc::RpcThread(void* p)
{
	FsFramework::app::EventThreadStart();

	EntRpc* pThis = (EntRpc*)p;
	pThis->m_rpcServer.Run();

	FsFramework::app::EventThreadStop();
}
