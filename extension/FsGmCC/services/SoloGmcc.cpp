#include "EntNetManager.h"
#include "SoloGmcc.h"
#include <stdio.h>
#include "public/Module.h"
#include "FsLogger.h"
#include "EntHttp.h"
#include "ServerManager.h"
#include "NetWorldGmcc.h"
#include "EntRpc.h"
#include "PayedOrder.h"
#include "NetWorldPush.h"
#include "NetWorldActivity.h"
#include "NetWorkPayed.h"

DECLARE_ENTITY("FsGmcc", SoloGmcc, EntNetBase)
DECLARE_PROPERTY(const char*, SoloGmcc, GameName, GetGameName, SetGameName)
DECLARE_PROPERTY(int, SoloGmcc, Heartbeat, GetHeartbeat, SetHeartbeat )

#ifdef _DEBUG
#define GAME_READY_CHECK_INTERVAL	10			// 10秒请求一次状态
#else
#define GAME_READY_CHECK_INTERVAL	30			// 30秒请求一次状态
#endif
#define GAME_ACTION_RESEND_TIMER	60			// 1分钟重发一次

extern ICore* g_pCore;

SoloGmcc::SoloGmcc()
	: m_pHttpService( NULL )
	, m_pRpcService( NULL )
	, m_pWorldSrv( NULL )
	, m_bGameReady( false )
	, m_bSelfReady( false )
	, m_tcheckTime( 0 )
	, m_nHeatbeat( 65 )
	, m_nSeq( 0 )
{
}

bool SoloGmcc::Init(const IVarList& args)
{
	FS_LOG_INFO( "SoloCharge::Init(const IVarList& args)" );
	m_pNetConnFactory = NetCreator::CreateConnectionFactory(false);
	m_pActionPacker = &m_packer;
	return EntNetBase::Init(args);
}

bool SoloGmcc::Shut()
{
	FS_LOG_INFO("SoloCharge::Shut");
	if (EntNetBase::Shut())
	{
		if (NULL != m_pNetConnFactory)
		{
			NetCreator::FreeConnectionFactory(m_pNetConnFactory);
			m_pNetConnFactory = NULL;
		}
		m_pActionPacker = NULL;

		CVarList res;
		GetCore()->RunScript("main.lua", "on_cleanup", res);
		return true;
	}
	return false;
}

bool SoloGmcc::Startup()
{
	IEntity* pEntity = GetCore()->LookupEntity("EntHttp");
	m_pHttpService = dynamic_cast<EntHttp*>(pEntity);

	pEntity = GetCore()->LookupEntity("EntRpc");
	m_pRpcService = dynamic_cast<EntRpc*>(pEntity);
	if (NULL == m_pHttpService)
	{
		FS_LOG_FATAL("not found http service.");
		return false;
	}

	if (NULL == m_pRpcService)
	{
		FS_LOG_FATAL("not found rpc service.");
		return false;
	}

	m_tcheckTime = time(NULL);
	std::string rpcName = StringUtil::StringAsUtf8String(m_strGameName.c_str());
	m_pRpcService->OnSetGameName(rpcName.c_str());
	m_pRpcService->OnSetServerHeartbeat(m_nHeatbeat);
	if (EntNetBase::Startup())
	{
		if (!m_pService->Listen())
		{
			FS_LOG_FATAL("listen service failed.");
			return false;
		}
		return true;
	}

	FS_LOG_FATAL("start service fatal.");
	return false;
}

bool SoloGmcc::Stop()
{
	m_pWorldSrv = NULL;
	m_pHttpService = NULL;
	m_pRpcService = NULL;
	ServerManager::GetInstance()->RemoveAll();
	g_pCore->SetQuit(true);
	return EntNetBase::Stop();
}

void SoloGmcc::Execute(float seconds)
{
	EntNetBase::Execute(seconds);
	GetCore()->Execute();

	if (m_bSelfReady)
	{
		if (NULL == m_pWorldSrv)
		{
			return;
		}

		if (!m_bGameReady && NULL != m_pWorldSrv)
		{
			time_t tNow = time(NULL);
			if (tNow - m_tcheckTime >= GAME_READY_CHECK_INTERVAL)
			{
				CheckGameReady();
				m_tcheckTime = tNow;
			}
		}
		else
		{
			// 重发请求
			std::map<uint64_t, PendingInfo>::iterator itr = m_mapPendings.begin();
			for (std::map<uint64_t, PendingInfo>::iterator next = itr; itr != m_mapPendings.end(); itr = next)
			{
				++next;
				itr->second.elapsed += seconds;
				if (itr->second.elapsed >= GAME_ACTION_RESEND_TIMER)
				{
					itr->second.elapsed = 0;
					if (!this->m_pService->Send(m_pWorldSrv->ConnId, itr->second.action))
					{
						FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
					}
				}
			}
		}
	}
	else
	{
		// 初始化profile
		time_t tNow = time(NULL);
		if (tNow - m_tcheckTime >= GAME_READY_CHECK_INTERVAL)
		{
			if (NULL != m_pHttpService)
			{
				FS_LOG_INFO("request http profile again...");
				m_pHttpService->OnReqProfile();
			}
			else
			{
				FS_LOG_WARN("http service not start....");
			}
			m_tcheckTime = tNow;
		}
	}
}

int SoloGmcc::GetNetKey() const
{
	return EM_NET_KEY_TCP;
}

void SoloGmcc::SetGameName(const char* pszName)
{
	if (pszName != m_strGameName.c_str())
	{
		m_strGameName = pszName;
	}

	if (m_pRpcService != NULL)
	{
		std::string rpcName = StringUtil::StringAsUtf8String( pszName );
		m_pRpcService->OnSetGameName(rpcName.c_str());
	}
}

const char* SoloGmcc::GetGameName() const
{
	return m_strGameName.c_str();
}

void SoloGmcc::SetHeartbeat(int nHeartbeat)
{
	m_nHeatbeat = nHeartbeat;
	if (m_pRpcService != NULL)
	{
		m_pRpcService->OnSetServerHeartbeat(nHeartbeat);
	}
}

int SoloGmcc::GetHeartbeat()
{
	return m_nHeatbeat;
}

void SoloGmcc::OnInit()
{
	EntNetBase::OnInit();
#ifdef TEST
	BindAction("TEXT", ACTION_BINDER(SoloGmcc::OnAction, this), 0);
#else
	BindAction("unknown", ACTION_BINDER(SoloGmcc::OnAction, this), 0);
	BindAction("register", ACTION_BINDER(SoloGmcc::OnActionServerRegister, this), 0);
	BindAction("keep", ACTION_BINDER(SoloGmcc::OnActionKeep, this), 0);

	BindAction(PROTOCOL_ID_GM_REPLY, ACTION_BINDER(SoloGmcc::OnGmResult, this), 0);
	BindAction(PROTOCOL_ID_MAIL_RESULT, ACTION_BINDER(SoloGmcc::OnMailResult, this), 0);
	BindAction(PROTOCOL_ID_NOTICE_RESULT, ACTION_BINDER(SoloGmcc::OnNoticeResult, this), 0);
	//BindAction(PROTOCOL_ID_GIFT_RESULT, ACTION_BINDER(SoloGmcc::OnExchGift, this), 0);

	BindAction(PROTOCOL_ID_CONFIRM_ACK, ACTION_BINDER(SoloGmcc::OnGameAck, this), 0);
	BindAction(PROTOCOL_ID_GAME_READY, ACTION_BINDER(SoloGmcc::OnGameReady, this), 0);
	BindAction(PROTOCOL_ID_GAME_REQ_SRV_INFO, ACTION_BINDER(SoloGmcc::OnGameReqSrvInfo, this), 0);
	BindAction(PROTOCOL_ID_GIFT_EXCH, ACTION_BINDER(SoloGmcc::OnExchGift, this), 0);
	//BindAction(PROTOCOL_ID_NOTICE_SYNC, ACTION_BINDER(SoloGmcc::OnSyncNotice, this), 0);
	BindAction(PROTOCOL_ID_ROLE_PROP_DATA, ACTION_BINDER(SoloGmcc::OnQueryPropData, this), 0);
	BindAction(PROTOCOL_ID_ROLE_RECORD_DATA, ACTION_BINDER(SoloGmcc::OnQueryRecordData, this), 0);
	BindAction(PROTOCOL_ID_PUSH_NOTIFICATION, ACTION_BINDER(SoloGmcc::OnPushNotification, this), 0);

	BindAction(PROTOCOL_ID_ORDER_REQ, ACTION_BINDER(SoloGmcc::OnClientOrder, this), 0);
	BindAction(PROTOCOL_ID_ORDER_NOTIFY, ACTION_BINDER(SoloGmcc::OnOrderNotify, this), 0);
	BindAction(PROTOCOL_ID_REBATE_QUERY, ACTION_BINDER(SoloGmcc::OnGameQueryRebate, this), 0);
	BindAction(PROTOCOL_ID_REBATE_PICK, ACTION_BINDER(SoloGmcc::OnGamePickedRebate, this), 0);
	BindAction(PROTOCOL_ID_REPORT_MEMBER_INFO, ACTION_BINDER(SoloGmcc::OnReportOnlineCount, this), 0);

	BindAction(PROTOCOL_ID_SHOP_ITEM_LIST, ACTION_BINDER(SoloGmcc::OnShopItemList, this), 0);
#endif
}

void SoloGmcc::OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	if (NULL == m_pWorldSrv)
	{
		m_pWorldSrv = new ServerInfo;
		m_pWorldSrv->ConnId = sessid;
	}
	EntNetBase::OnConnection(serid, sessid, pAction);

	m_tcheckTime = time(NULL);
	// 连接成功取服务器当前通知
	//if (NULL != m_pHttpService)
	//{
	//	m_pHttpService->OnQueryAllData();
	//}
	if (NULL != m_pRpcService)
	{
		m_pRpcService->OnReportState(EM_MEMBER_STATE_OPENING);
	}
}

void SoloGmcc::OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	if (NULL != m_pWorldSrv && m_pWorldSrv->ConnId == sessid)
	{
		delete m_pWorldSrv;
		m_pWorldSrv = NULL;

		if (NULL != m_pRpcService)
		{
			m_pRpcService->OnReportState(EM_MEMBER_STATE_DEAD);
		}

		m_bGameReady = false;
	}

	ServerManager::GetInstance()->RemoveByNID(sessid);
	EntNetBase::OnClose(serid, sessid, pAction);
}

void SoloGmcc::OnActionServerRegister(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetWorldAction_ServerRegister* pAction = dynamic_cast<NetWorldAction_ServerRegister*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'register' action");
		return;
	}

	NetWorldMessage_ServerRgister* pMsg = dynamic_cast<NetWorldMessage_ServerRgister*>(pAction->Request());
	if (NULL == pMsg)
	{
		FS_LOG_ERROR("action 'register' not has message");
		return;
	}
	FS_LOG_INFO("register server ............................");
	// 注册服务****(测试未调用)
	ServerInfo srvInfo;
	srvInfo.ConnId = sid;
	srvInfo.ServerId = pMsg->value_of_3;
	srvInfo.ServerType = pMsg->value_of_2;
	ServerManager::GetInstance()->PushServer(srvInfo);

	if (NULL == m_pWorldSrv)
	{
		m_pWorldSrv = new ServerInfo;
	}
	m_pWorldSrv->ConnId = sid;

	if (NULL != m_pRpcService)
	{
		m_pRpcService->OnReportState( EM_MEMBER_STATE_OPENED );
	}
}

void SoloGmcc::OnActionKeep(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	//NetWorldAction_Keep* pAction = dynamic_cast<NetWorldAction_Keep*>(action.get());
	//if (NULL == pAction)
	//{
	//	WriteMessage("recv empty 'keep' action");
	//	return;
	//}

	//char szMsg[1024] = { 0 };
	//sprintf(szMsg, "connect info:%s\nprotocol:%s", pAction->coninfo.c_str(), pAction->protocol.c_str());
	//WriteMessage(szMsg);
	if (m_bReplyKeep)
	{
		if (NULL != m_pService)
		{
			m_pService->Send(sid, action);
		}
	}
}

void SoloGmcc::OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
#ifdef TEST
	TextAction* txtAction = dynamic_cast<TextAction*>(action.get());
	if (NULL == txtAction)
	{
		WriteMessage("recv empty action");
		return;
	}

	NetTextMessage* txtMessage = dynamic_cast<NetTextMessage*>(txtAction->Request());
	if (NULL == txtMessage)
	{
		WriteMessage("recv empty message");
		return;
	}

	WriteMessage(txtMessage->GetMsgBody());
	// reply
	m_pService->Send(sid, action);
#else 
	NetWorldAction_Unknown* pAction = dynamic_cast<NetWorldAction_Unknown*>(action.get());
	if (NULL == pAction)
	{
		FS_LOG_ERROR("recv empty 'unknown' action");
		return;
	}

	IVarList& args = pAction->cargs;
	FS_LOG_WARN_FORMAT("param count:%d", args.GetCount());
	for (size_t i = 0; i < args.GetCount(); ++i)
	{
		switch (args.GetType(i))
		{
		case	VTYPE_BOOL:		// 布尔
			FS_LOG_WARN_FORMAT( "index:%d bool value:%s", i, args.BoolVal(i) ? true : false);
			break;
		case	VTYPE_INT:		// 32位整数
			FS_LOG_WARN_FORMAT( "index:%d int value:%d", i, args.IntVal(i));
			break;
		case	VTYPE_INT64:	// 64位整数
			FS_LOG_WARN_FORMAT( "index:%d int64 value:%lld", i, args.Int64Val(i));
			break;
		case	VTYPE_FLOAT:	// 单精度浮点数
			FS_LOG_WARN_FORMAT( "index:%d float value:%f", i, args.FloatVal(i));
			break;
		case	VTYPE_DOUBLE:	// 双精度浮点数
			FS_LOG_WARN_FORMAT( "index:%d double value:%llf", i, args.DoubleVal(i));
			break;
		case	VTYPE_STRING:	// 字符串
			FS_LOG_WARN_FORMAT( "index:%d string value:%s", i, args.StringVal(i));
			break;
		case	VTYPE_WIDESTR:	// 宽字符串
			FS_LOG_WARN_FORMAT( "index:%d wide value:%s", i, (char*)args.WideStrVal(i));
			break;
		case	VTYPE_OBJECT:	// 对象号
			FS_LOG_WARN_FORMAT( "index:%d object value:", i);
			break;
		case	VTYPE_POINTER:	// 指针
			FS_LOG_WARN_FORMAT( "index:%d pointer value:", i);
			break;
		case	VTYPE_USERDATA:	// 用户数据
			FS_LOG_WARN_FORMAT( "index:%d user value:", i);
			break;
		case	VTYPE_TABLE:	// 表
			FS_LOG_WARN_FORMAT( "index:%d table value:", i);
			break;
		default:
			FS_LOG_WARN_FORMAT( "index:%d value:unkown type", i);
			break;
		}
	}

	/*
	NetExtraAction_Notice_Post* p = new NetExtraAction_Notice_Post;
	//p->protocol = "post";
	//p->coninfo = "112233";
	NetExtra_Notice_Post* post = (NetExtra_Notice_Post*)p->Request();
	post->data.NoticeId = 2;
	post->data.NoticeType = 3;
	post->data.CreateTime = 112222;
	post->data.ExtP1 = 1;
	post->data.ExtP2 = 1;
	post->data.Duration = 1000;
	post->data.Content = "aaafbcfe";
	std::shared_ptr<INetAction> a(p);
	m_pService->Send(sid, a);
	*/
#endif
}

void SoloGmcc::OnGameAck(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	NetExtraGameAck* pGameAck = dynamic_cast<NetExtraGameAck*>(action->Request());
	assert(NULL != pGameAck);
	if (NULL == pGameAck)
	{
		FS_LOG_FATAL("request type error.");
		return;
	}
	m_mapPendings.erase((uint64_t)pGameAck->seq);
	FS_LOG_INFO_FORMAT("game confirm action ack(seq:%lld)", (uint64_t)pGameAck->seq);
}

void SoloGmcc::OnGameReady(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server ready, start send gmcc config info");
	printf("on game ready, start send gmcc config info\n");
	OnInnerGameReady();
}

void SoloGmcc::OnGameReqSrvInfo(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	printf("on game request gmcc server config info!\n");
	if ( m_bSelfReady )
	{
		SendSrvInfo();
	}
}

void SoloGmcc::OnExchGift(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server exch gift");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Exch_Gift* pExchGift = dynamic_cast<NetExtra_Exch_Gift*>(action->Request());
		assert(NULL != pExchGift);
		if (NULL == pExchGift)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}
		m_pHttpService->OnReqExchGift( pExchGift->info );
	}
}

void SoloGmcc::OnSyncNotice(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server request sync notice");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		m_pHttpService->OnReqAllData();
	}
}

void SoloGmcc::OnQueryPropData(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server return prop data");

	assert(NULL != m_pRpcService);
	if (NULL != m_pRpcService)
	{
		NetExtra_Role_Prop_Data* pData = dynamic_cast<NetExtra_Role_Prop_Data*>(action->Request());
		assert(NULL != pData);
		if (NULL == pData)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}
		m_pRpcService->OnPropData(pData->data);
	}
}

void SoloGmcc::OnQueryRecordData(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server return record data");

	assert(NULL != m_pRpcService);
	if (NULL != m_pRpcService)
	{
		NetExtra_Role_Record_Data* pData = dynamic_cast<NetExtra_Role_Record_Data*>(action->Request());
		assert(NULL != pData);
		if (NULL == pData)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}
		m_pRpcService->OnRecordData(pData->data);
	}
}

void SoloGmcc::OnPushNotification(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server push notification");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_PushRequest* pReq = dynamic_cast<NetExtra_PushRequest*>(action->Request());
		assert(NULL != pReq);
		if (NULL == pReq)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}
		m_pHttpService->OnReqPushNotification(pReq->data);
	}
}

void SoloGmcc::OnShopItemList(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	// TODO: 商品列表返回
}

void SoloGmcc::OnOrderNotify(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server changed order state");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Product_Order_Notify* pNotify_ = dynamic_cast<NetExtra_Product_Order_Notify*>(action->Request());
		assert(NULL != pNotify_);
		if (NULL == pNotify_)
		{
			FS_LOG_FATAL("product order type error.");
			return;
		}
		m_pHttpService->OnNotifyOrder(pNotify_->data);
	}
}

void SoloGmcc::OnClientOrder(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game client request product order");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Product_Order* pOrder_ = dynamic_cast<NetExtra_Product_Order*>(action->Request());
		assert(NULL != pOrder_);
		if (NULL == pOrder_)
		{
			FS_LOG_FATAL("product order type error.");
			return;
		}
		m_pHttpService->OnReqPreOrder(pOrder_->data);
	}
}

void SoloGmcc::OnGameQueryRebate(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO( "on game server query rebate" );

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Payed_Rebate_Query* pReq = dynamic_cast<NetExtra_Payed_Rebate_Query*>(action->Request());
		assert(NULL != pReq);
		if (NULL == pReq)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}

		m_pHttpService->OnReqPayedRebate(pReq->uid, pReq->rid);
	}
}

void SoloGmcc::OnGamePickedRebate(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server notify picked rebate");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Payed_Rebate_Pick* pReq = dynamic_cast<NetExtra_Payed_Rebate_Pick*>(action->Request());
		assert(NULL != pReq);
		if (NULL == pReq)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}

		m_pHttpService->OnReqPayedRebatePick(pReq->uid, pReq->rid, pReq->name);
	}
}

void SoloGmcc::OnReportOnlineCount(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	//FS_LOG_DEBUG("on game server report online count");

	assert(NULL != m_pRpcService);
	if (NULL != m_pRpcService)
	{
		NetExtra_MemberInfo* pMemberInfo = dynamic_cast<NetExtra_MemberInfo*>(action->Request());
		assert(NULL != pMemberInfo);
		if (NULL == pMemberInfo)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}

		m_pRpcService->OnReportOnline(pMemberInfo->online_count);

		MemberInfo info;
		info.memberId = pMemberInfo->memberId;
		info.workState = pMemberInfo->workState;
		info.state = EM_MEMBER_STATE_OPENED;
		m_pRpcService->OnReportMemberInfo( info );
	}
}

void SoloGmcc::OnGmResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server reply exec gm result");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtraGM_Reply* pGmReply = dynamic_cast<NetExtraGM_Reply*>( action->Request() );
		//assert(NULL != pGmReply);
		if (NULL == pGmReply)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}

		m_pHttpService->OnReqConfirmGm(pGmReply->cmdid, pGmReply->result, pGmReply->roleid, pGmReply->message);
		//m_pRpcService->OnGmResult( pGmReply->cmdid, pGmReply->result );
	}
}

void SoloGmcc::OnMailResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_INFO("on game server reply mail result");

	assert(NULL != m_pHttpService);
	if (NULL != m_pHttpService)
	{
		NetExtra_Mail_Result* pMailResult = dynamic_cast<NetExtra_Mail_Result*>(action->Request());
		//assert(NULL != pMailResult);
		if (NULL == pMailResult)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}

		m_pHttpService->OnReqConfirmMail(pMailResult->MailId, pMailResult->Result, pMailResult->IsGlobal, pMailResult->Roles);
	}
}

void SoloGmcc::OnNoticeResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	FS_LOG_DEBUG("on game server reply notice result");

	assert(NULL != m_pRpcService);
	if (NULL != m_pRpcService)
	{
		NetExtra_Notice_Result* pNoticeResult = dynamic_cast<NetExtra_Notice_Result*>(action->Request());
		//assert(NULL != pNoticeResult);
		if (NULL == pNoticeResult)
		{
			FS_LOG_FATAL("request type error.");
			return;
		}
		//m_pRpcService->OnNoticeResult(pNoticeResult->NoticeId, pNoticeResult->Result);
	}
}


void SoloGmcc::OnProfileInited(const SrvInfo& info)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	printf("on profile ready\n");
	m_bSelfReady = true;
	m_srvInfo = info;
	if (m_bGameReady)
	{
		SendSrvInfo();
	}

	assert(NULL != m_pRpcService);
	if (NULL != m_pRpcService)
	{
		m_pRpcService->OnSetGameAreaId(info.AreaId);
	}
}

void SoloGmcc::SendGameCommand(const CmdInfo& cmd)
{
	FS_LOG_DEBUG("on external web send command to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_GM_Exec);
		NetExtraGM_Exec* pExec = dynamic_cast<NetExtraGM_Exec*>( action->Request() );
		pExec->cmd = cmd;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on sent command(cmd:%s) to game server", cmd.command.c_str());
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT( "world server had disconected." );
	}
}

void SoloGmcc::SendMail(const MailData& mail)
{
	FS_LOG_DEBUG("on external web send mail to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Mail_Post);
		NetExtra_Mail_Post* pMail = dynamic_cast<NetExtra_Mail_Post*>(action->Request());
		pMail->data = mail;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on sent mail(title:%s global:%s) to game server", mail.Title.c_str(), mail.GlobalMail ? "true" : "false");
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
}

void SoloGmcc::SendNotice(const NoticeData& notice)
{
	FS_LOG_INFO_FORMAT("on external web send notice to game server(id:%lld).", notice.NoticeId);
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Notice_Post);
		NetExtra_Notice_Post* pNotice = dynamic_cast<NetExtra_Notice_Post*>(action->Request());
		pNotice->data = notice;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on sent notice(title:%s id:%lld) to game server", notice.Name.c_str(), notice.NoticeId);
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
}

void SoloGmcc::DeleteNotice(int64_t nNoticeId)
{
	FS_LOG_DEBUG("on external web send delete notice to game server.");

	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Notice_Delete);
		NetExtra_Notice_Delete* pNotice = dynamic_cast<NetExtra_Notice_Delete*>(action->Request());
		pNotice->NoticeId = nNoticeId;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on delete notice(id:%lld) to game server", nNoticeId);
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}

}

void SoloGmcc::SendExchGiftItems(const ExchGiftResult& exchResult)
{
	FS_LOG_INFO("on external web send excharge gift result and items to game server.");

	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	assert(!exchResult.roleId.empty());
	assert(!exchResult.key.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Gift_Result);
		NetExtra_Gift_Result* pGift = dynamic_cast<NetExtra_Gift_Result*>(action->Request());
		pGift->data = exchResult;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on sent gift(roleid:%s key:%s) to game server", exchResult.roleId.c_str(), exchResult.key.c_str());
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected. send gift item failed(roleid:%s key:%s) to game server", exchResult.roleId.c_str(), exchResult.key.c_str());
	}
}

void SoloGmcc::SendPayment(const Payment& payment)
{
	FS_LOG_DEBUG("on external web send payment to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	assert(!payment.RoleId.empty());
	assert(!payment.OrderId.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Payment);
		NetExtra_PaymentInfo* payedInfo_ = dynamic_cast<NetExtra_PaymentInfo*>(action->Request());
		payedInfo_->data = payment;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on send payment info(roleid:%s order:%s product:%s) to game server",
				payment.RoleId.c_str(), payment.OrderId.c_str(), payment.ProductId.c_str());
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected, send payment info failed(roleid:%s order:%s product:%s) to game server",
								payment.RoleId.c_str(), payment.OrderId.c_str(), payment.ProductId.c_str());
	}
}

void SoloGmcc::SendProductOrder(const ProductOderInfo& order)
{
	FS_LOG_DEBUG("on external web send product order info to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	assert(!order.RoleId.empty());
	assert(!order.ProductId.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Product_Order_Info);
		NetExtra_Product_Order_Info* productOrder_ = dynamic_cast<NetExtra_Product_Order_Info*>(action->Request());
		productOrder_->data = order;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on sent product order (roleid:%s order:%s product:%s) to game server",
									order.RoleId.c_str(), order.OrderId.c_str(), order.ProductId.c_str());
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected, send product order failed(roleid:%s order:%s product:%s) to game server",
							order.RoleId.c_str(), order.OrderId.c_str(), order.ProductId.c_str());
	}
}

void SoloGmcc::SendProducts(const std::vector<Product>& products)
{
	FS_LOG_DEBUG("on external web send product list to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Products_Push);
		NetExtra_Products_Push* goodsList_ = dynamic_cast<NetExtra_Products_Push*>(action->Request());
		goodsList_->data = products;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO("on sent goods list to game server");
		}
	}
	else
	{
		FS_LOG_FATAL("world server had disconected, send goods list to game server");
	}
}

void SoloGmcc::SendPayedRebateInfo(const std::string rid, int nType, int nRebate)
{
	FS_LOG_DEBUG("on external web send rebate info to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_PayedRebateInfo);
		NetExtra_Payed_Rebate_Info* rebate_ = dynamic_cast<NetExtra_Payed_Rebate_Info*>(action->Request());
		rebate_->rid = rid;
		rebate_->type = nType;
		rebate_->rebate = nRebate;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO("on sent rebate to game server");
		}
	}
	else
	{
		FS_LOG_FATAL("world server had disconected, send rebate to game server");
	}
}

void SoloGmcc::SendPayedRebateResult( const std::string rid, int nResult, int nRebate )
{
	FS_LOG_DEBUG("on external web send rebate result to game server.");
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_PayedRebateResult);
		NetExtra_Payed_Rebate_Result* rebate_ = dynamic_cast<NetExtra_Payed_Rebate_Result*>(action->Request());
		rebate_->rid = rid;
		rebate_->result = nResult;
		rebate_->amount = nRebate;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO("on sent rebate result to game server");
		}
	}
	else
	{
		FS_LOG_FATAL("world server had disconected, send rebate to game server");
	}
}

bool SoloGmcc::QueryPlayerProps(const CmdPropQuery& query)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return false;
	}

	assert(!query.roleId.empty());
	assert(! query.props.size() );

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Role_Prop_Query);
		NetExtra_Role_Prop_Query* pq = dynamic_cast<NetExtra_Role_Prop_Query*>(action->Request());
		pq->data = query;
		FS_LOG_INFO_FORMAT("on sent prop query(roleid:%s) to game server", query.roleId.c_str());
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return true;
		}		
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
	return false;
}

bool SoloGmcc::QueryPlayerRecordData(const CmdRecordQuery& query)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return false;
	}

	assert(!query.roleId.empty());
	assert(!query.recordName.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_Role_Record_Query);
		NetExtra_Role_Record_Query* pq = dynamic_cast<NetExtra_Role_Record_Query*>(action->Request());
		pq->data = query;
		FS_LOG_INFO_FORMAT("on sent record query(roleid:%s rec name:%s) to game server", query.roleId.c_str(), query.recordName.c_str());
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return true;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
	return false;
}

bool SoloGmcc::ShopPushItem(const std::wstring& name, const std::wstring& desc, int64 startTime, int64 endTime, 
									const std::vector<ShopTag>& catalogs, const std::vector<ShopItem>& items)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return false;
	}

	assert(!items.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_ShopPushItem());
		NetExtra_ShopItemList* pq = dynamic_cast<NetExtra_ShopItemList*>(action->Request());
		pq->name = name;
		pq->desc = desc;
		pq->startTime = startTime;
		pq->endTime = endTime;
		pq->catalogs = catalogs;
		pq->items = items;
		FS_LOG_INFO_FORMAT("on sent shop item to game server(count:%d)", items.size());
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return true;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected. plus item send failed");
	}
	return false;
}

bool SoloGmcc::ShopRemoveItem(const std::vector<std::string>& removes)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return false;
	}

	assert(!removes.empty());

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_ShopRemoveItem());
		NetExtra_ShopRemoveList* pq = dynamic_cast<NetExtra_ShopRemoveList*>(action->Request());
		pq->removes = removes;
		FS_LOG_INFO_FORMAT("on sent shop removes item to game server(count:%d)", removes.size());
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return true;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected. remove plus item failed");
	}
	return false;
}

bool SoloGmcc::ShopQueryItem()
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return false;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_ShopQueryItem());
		FS_LOG_INFO("on sent shop query item to game server");
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return true;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
	return false;
}

void SoloGmcc::SetActivityConfig(ActivityInfo data)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_ActiityConfig());
		NetExtra_ActiityConfig* pq = dynamic_cast<NetExtra_ActiityConfig*>(action->Request());
		pq->data = data;
		FS_LOG_INFO_FORMAT("on sent activity item to game server(type:%d count:%d)", data.actType, data.items.size());
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected, send activity config failed.");
	}
}

void SoloGmcc::SendLoadConfigInfo(const std::vector<ConfigLoadInfo>& cfgInfo)
{
	if (NULL == m_pService)
	{
		FS_LOG_FATAL_FORMAT("service don't start.");
		return;
	}

	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_LoadConfig());
		NetExtra_LoadConfig* pq = dynamic_cast<NetExtra_LoadConfig*>(action->Request());
		pq->data = cfgInfo;
		pq->seq = (int64_t)m_nSeq++;

		PendingInfo& pending = m_mapPendings[(uint64_t)pq->seq];
		pending.action = action;
		pending.elapsed = 0;
		FS_LOG_INFO_FORMAT("on sent gmcc config to game server(count:%d seq:%lld)", cfgInfo.size(), (uint64_t)pq->seq);
		FS_LOG_INFO_FORMAT("action requst seq:%lld", (uint64_t)pq->seq);
		if (this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			return;
		}
		FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected, send gmcc config failed.");
	}
}

void SoloGmcc::OnInnerGameReady()
{
	if (!m_bGameReady)
	{
		if (!m_pRpcService->StartRpc())
		{
			FS_LOG_FATAL("start rpc server failed.");
			return;
		}
		else
		{
			FS_LOG_INFO_FORMAT("start rpc server succeed.");
		}

		m_bGameReady = true;

		if (m_bSelfReady)
		{
			SendSrvInfo();
		}
	}
}

void SoloGmcc::SendSrvInfo()
{
	FS_LOG_INFO("send server info to game server.");
	if (NULL != m_pWorldSrv)
	{
		std::shared_ptr<INetAction> action(new NetExtraAction_GmccInfo);
		NetExtraGmccInfo* pReq = dynamic_cast<NetExtraGmccInfo*>(action->Request());
		pReq->Info = m_srvInfo;
		if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
		{
			FS_LOG_WARN_FORMAT("send msg to game server failed, net disconnected or send buffer to lower.");
		}
		else
		{
			FS_LOG_INFO_FORMAT("on send srv info to game server success!!");

			assert(NULL != m_pHttpService);
			if (NULL != m_pHttpService)
			{
				// 第一次连接成功后主动去请求所有配置
				m_pHttpService->OnReqAllData();
			}
			else
			{
				FS_LOG_FATAL_FORMAT("http service not start.");
			}
		}
	}
	else
	{
		FS_LOG_FATAL_FORMAT("world server had disconected.");
	}
}

void SoloGmcc::CheckGameReady()
{
	if (!m_bGameReady)
	{
		if (NULL != m_pWorldSrv)
		{
			std::shared_ptr<INetAction> action(new NetExtraAction_GameCheck);
			if (!this->m_pService->Send(m_pWorldSrv->ConnId, action))
			{
				FS_LOG_WARN_FORMAT("send check command to game server failed, net disconnected or send buffer to lower.");
			}
			else
			{
				FS_LOG_INFO_FORMAT("send check command to game server.");
			}
		}
		else
		{
			FS_LOG_FATAL_FORMAT("world server had disconected.");
		}
	}
}
