//--------------------------------------------------------------------
// 文件名:      GmccModule.cpp
// 内  容:      gmcc消息处理模块
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:        
//    :        
//--------------------------------------------------------------------

#include "FsGame/CommonModule/GmccModule.h"
#include "FsGame/Define/ModuleCmdDefine.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/TipsDefine.h"
#include "FsGame/Define/GmccDefine.h"
#include "public/Converts.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "GMModule.h"
#include "Define/ExtraServerDefine.h"
#include "extension/FsGmCC/protocols/ProtocolsID.h"
#include "extension/FsGmCC/model/ServerInfo.h"
#include "InterActiveModule/SystemMailModule.h"
#include "InterActiveModule/ChatModule.h"
#include "system\winportable.h"
#include <time.h>
#include "GeneralModule/RewardModule.h"
#include "Define/PubDefine.h"
#include "server/RoomKnlConst.h"
#include "Define/SceneListDefine.h"
#include "LoginExtModule.h"
#include "TradeModule/ShopModule.h"
#include "GeneralModule/CapitalModule.h"
#include "LuaExtModule.h"
#include "Define/DynamicActDefine.h"
#include "InterActiveModule/RedPacketModule.h"
#include "EnvirValueModule.h"
#include "PlayerBaseModule/PlayerBaseModule.h"
#include "ReLoadConfigModule.h"
#include "LuaScriptModule.h"
#include "FsGame/Middle/MiddleModule.h"

std::wstring GmccModule::m_domainName = L"";
GmccModule * GmccModule::m_pGmccModule = NULL;
GMModule*	GmccModule::m_pGmModule = NULL;
bool		GmccModule::ms_bGameReady = false;
bool		GmccModule::ms_bRecivedSrvInfo = false;
int64_t		GmccModule::ms_nLastSyncTimestamp = 0;

// 充值
inline int nx_recharge(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_recharge, 3);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_recharge, 1);
	CHECK_ARG_FLOAT(state, nx_recharge, 2);
	CHECK_ARG_STRING(state, nx_recharge, 3);
	PERSISTID player = pKernel->LuaToObject(state, 1);
	float fAmout = pKernel->LuaToFloat(state, 2);
	const char* pszOrderId = pKernel->LuaToString(state, 3);

	GmccModule::m_pGmccModule->OnRechargeSucceed(pKernel, player, pszOrderId, fAmout, true);
	pKernel->LuaPushBool(state, true);

	return  1;
}

// 支付订单
inline int nx_pay_item(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_pay_item, 6);


	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_pay_item, 1);
	CHECK_ARG_FLOAT(state, nx_pay_item, 2);
	CHECK_ARG_STRING(state, nx_pay_item, 3);
	CHECK_ARG_STRING(state, nx_pay_item, 4);
	CHECK_ARG_STRING(state, nx_pay_item, 5);
	CHECK_ARG_STRING(state, nx_pay_item, 6);

	PERSISTID player = pKernel->LuaToObject(state, 1);
	float fAmout = pKernel->LuaToFloat(state, 2);
	const char* pszOrderId = pKernel->LuaToString(state, 3);			// 订单id
	const char* pszProductId = pKernel->LuaToString(state, 4);			// 商品
	const char* pszItems = pKernel->LuaToString(state, 5);				// 获得物品
	const char* pszRewards = pKernel->LuaToString(state, 6);			// 首充奖励

	if (NULL == pszOrderId || NULL == pszProductId || NULL == pszItems || NULL == pszRewards)
	{
		pKernel->LuaErrorHandler(state, "nx_pay_item argument has null value");
		return 1; 
	}

	IRecord* pPayedRec = pKernel->GetRecord(player, FIELD_RECORD_PAYED_ORDER);
	if (NULL == pPayedRec)
	{
		pKernel->LuaErrorHandler(state, "nx_pay_item not found record 'FIELD_RECORD_PAYED_INFO'");
		return 1;
	}

	IRecord* pPayedProducts = pKernel->GetRecord(player, FIELD_RECORD_PAYED_PRODUCTS);
	time_t tNow = util_get_utc_time();
	GmccModule::PayedOrderData data;
	data.OrderId = pszOrderId;
	data.ProductId = pszProductId;
	data.Items = pszItems;
	data.Rewards = pszRewards;
	data.Extras = pszItems;
	data.Amount = fAmout;
	data.ProductRule = EM_PRODUCT_RULE_NORMAL;
	data.MaxPurchase = 0;
	data.IsRealPay = false;
	data.nOrderTime = tNow;
	data.nPayedTime = tNow;
	bool bRet = GmccModule::m_pGmccModule->HandleOrderSucceed(pKernel, player, pPayedRec, pPayedProducts, data);
	pKernel->LuaPushBool(state, bRet);

	return  1;
}


bool GmccModule::Init(IKernel * pKernel)
{
	m_pGmccModule = this;
	m_pGmModule = dynamic_cast<GMModule*>(pKernel->GetLogicModule("GMModule"));

	Assert(m_pGmModule && m_pGmccModule);

	pKernel->AddEventCallback("player", "OnReady", GmccModule::OnPlayerReady);
	pKernel->AddEventCallback("player", "OnContinue", GmccModule::OnPlayerContinue);

	//客户端消息
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_GMCC_MSG, OnCustom);
	pKernel->AddEventCallback("scene", "OnExtraMessage", OnGmccMessage);
	pKernel->AddIntCommandHook("player", COMMAND_GMCC, OnGmccCommand);
	pKernel->AddIntCommandHook("scene", COMMAND_GMCC, OnGmccSceneCommand);

	// 公共服消息
	pKernel->AddEventCallback("scene", "OnPublicMessage", OnPublicMessage);

	DECL_HEARTBEAT(GmccModule::HB_PayedWait);
	DECL_HEARTBEAT(GmccModule::HB_ReportMemberInfo);
	DECL_HEARTBEAT(GmccModule::HB_RequestSrvInfo);

	// gm
	DECL_LUA_EXT(nx_recharge);
	DECL_LUA_EXT(nx_pay_item);

	return true;
}

bool GmccModule::Shut(IKernel * pKernel)
{
	return true;
}
void GmccModule::OnModuleCommand(IKernel* pKernel, int cmd_id, const IVarList& args)
{
	switch (cmd_id)
	{
	case E_MM_CMD_READY_PUB:
		{
			ms_bGameReady = true;
			OnGameReady(pKernel);
			// SystemMailModule::SendGlobalMail(pKernel, L"", L"广播邮件测试", L"这是广播看见了伯红烧豆腐 有", NULL, 0);
		}
		break;
	default:
		break;
	}
}
int GmccModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& player, const PERSISTID& sender, const IVarList& msg)
{
	bool bFirst = msg.IntVal(0) == 1;
	if (bFirst)
	{
		// 重连会处理充值返利
		OnPlayerContinue(pKernel, player, sender, msg);

		// 首次上线查询充值支付
		m_pGmccModule->OnPayedRecvItems(pKernel, player);

		// 收取全局邮件
		SystemMailModule::RecvGlobalMail(pKernel, player);
	}
	else
	{
		// 充值返利处理
		m_pGmccModule->QueryPayedRebate(pKernel, player);
	}

	return 0;
}

int GmccModule::OnPlayerContinue(IKernel* pKernel, const PERSISTID& player, const PERSISTID& sender, const IVarList& msg)
{
	// 充值返利处理
	m_pGmccModule->QueryPayedRebate(pKernel, player);

	const std::wstring wsName = GetDomainName(pKernel);

	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace)
	{
		return 0;
	}

	IPubData * pPubData = pPubSpace->GetPubData(wsName.c_str());
	if (pPubData == NULL)
	{
		return 0;
	}

	IRecord* pNoticeRec = pPubData->GetRecord(PUB_GMCC_NOTICE_REC);
	if (NULL == pNoticeRec)
	{
		return 0;
	}

	int nRows = pNoticeRec->GetRows();

	CVarList senddata;
	senddata << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_ADD_NOTICE << nRows;
	LoopBeginCheck(t);

	static int64_t s_LastCheckNoticeTime = 0;
	const int64_t NOTICE_CHECK_INTERVAL = 3;
	int64_t nNow = util_get_utc_time();
	if (nNow - s_LastCheckNoticeTime >= NOTICE_CHECK_INTERVAL)
	{
		int nCount = 0;
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(t);
			int64_t nNoticeId = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_ID);
			// 删除过期通知
			int64_t nStart = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_CREATE_TIME);
			int64_t nEnd = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_END_TIME);
			if (nEnd <= nNow)
			{
				CVarList msg;
				msg << PUBSPACE_DOMAIN << wsName.c_str() << SP_DOMAIN_MSG_GMCC_DELETE_NOTICE << nNoticeId;
				pKernel->SendPublicMessage(msg);
			}
			else
			{
				const wchar_t* strContent = pNoticeRec->QueryWideStr(i, PUB_COL_GNR_NOTICE_CONTENT);
				int nLoop = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_LOOP);
				int nInterval = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_INTERVAL);
				int nType = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_TYPE);

				// 计算公告开始剩余时间
				int32_t nLeft = 0;
				if (nStart > nNow)
				{
					nLeft = (int32_t)(nStart - nNow);
				}
				++nCount;
				senddata << nNoticeId << strContent << nLoop << nInterval << nType << nLeft << nEnd;
			}
		}
		// 重置条数
		senddata.SetInt(2, nCount);
		s_LastCheckNoticeTime = nNow;
	}
	else
	{
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(t);
			int64_t nNoticeId = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_ID);
			const wchar_t* strContent = pNoticeRec->QueryWideStr(i, PUB_COL_GNR_NOTICE_CONTENT);
			int nLoop = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_LOOP);
			int nInterval = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_INTERVAL);
			int64_t nStart = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_CREATE_TIME);
			int64_t nEnd = pNoticeRec->QueryInt64(i, PUB_COL_GNR_NOTICE_END_TIME);
			int nType = pNoticeRec->QueryInt(i, PUB_COL_GNR_NOTICE_TYPE);

			// 计算公告开始剩余时间
			int32_t nLeft = 0;
			if (nStart > nNow)
			{
				nLeft = (int32_t)(nStart - nNow);
			}
			senddata << nNoticeId << strContent << nLoop << nInterval << nType << nLeft << nEnd;
		}
	}
	pKernel->Custom(player, senddata);

	return 0;
}

int GmccModule::OnPublicMessage(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args)
{
	if (args.GetCount()<2)
	{
		return 0;
	}
	// 消息格式
	// string "domain", wstring L"DomainGmcc_serverid", int msgid, string RoleId, string Account, string Order, int nResult

	const char* spaceName = args.StringVal(0);
	const wchar_t* logicName = args.WideStrVal(1);

	// 不是指定的消息，不需要处理
	if (0 != ::strcmp(spaceName, PUBSPACE_DOMAIN)
		|| 0 != ::wcscmp(logicName, GetDomainName(pKernel).c_str()))
	{
		return 0;
	}

	int msgId = args.IntVal(2);
	if (msgId == PS_DOMAIN_MSG_GMCC_PAYED_PUSH_RESULT)
	{
		Assert(args.GetCount() == 6);
		if (args.GetCount() < 6)
		{
			::extend_warning(LOG_ERROR, "PS_DOMAIN_MSG_PAYED_PUSH_RESULT args count error.");
			return 0;
		}

		int nResult = args.IntVal(3);
		const char* pszRoleId = args.StringVal(4);
		const char* pszOrder = args.StringVal(5);
		m_pGmccModule->OnPayedPushResult(pKernel, pszRoleId, pszOrder, nResult);
	}
	else if (msgId == PS_DOMAIN_MSG_ADD_GLOBAL_MAIL_RESULT)
	{
		// 添加全局邮件结果 格式：string "domain", wstring L"DomainGmcc_serverid", int msgid, int64 nIdent, int nResult
		
		int nResult = args.IntVal(3);
		if (nResult == EM_GLOBAL_MAIL_ADD_SUCCESS)
		{
			// 广播命令提示在线玩家收取邮件
			CVarList var;
			var << COMMAND_GMCC << GMCC_CMD_RECV_GLOBAL_MAIL;
			pKernel->CommandByWorld( var );
		}
		else
		{
			int64_t nIdent = args.Int64Val(4);
			// 输出错误日志
			::extend_warning(LOG_ERROR, "add global mail failed(ident:%lld result:%d)", nIdent, nResult);
		}
	}

	return 0;
}

// 检查GMCC客服的参数
bool GmccModule::CheckCustom(IKernel * pKernel, const PERSISTID & player, const PERSISTID & sender, const IVarList & msg)
{
	// 判断玩家的合法性
	if (!pKernel || !pKernel->Exists(player))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::CheckCustom] %s is NULL", "pKernel");
		return false;
	}

	int count = (int)msg.GetCount();
	if (count < 2 || msg.GetType(1) != VTYPE_INT)
	{
		return false;
	}

	// 检查GMCC的cmd类型
	const int cmd = msg.IntVal(1);
	if ( cmd == GMCC_C2S_ACCUSE )
	{ 
		if (count != 6 
			|| msg.GetType(2) != VTYPE_WIDESTR		// const std::wstring & name
			|| msg.GetType(3) != VTYPE_INT			// const int why
			|| msg.GetType(4) != VTYPE_WIDESTR		// const std::wstring & Reason
			|| msg.GetType(5) != VTYPE_WIDESTR )	// const std::wstring & BaseInfo
		{
			return false; 
		}
	}
	else if( cmd == GMCC_C2S_BUG || cmd == GMCC_C2S_EXP )
	{	
		if (count != 6 
			|| msg.GetType(2) != VTYPE_WIDESTR		// const wchar_t* index_type
			|| msg.GetType(3) != VTYPE_WIDESTR		// const wchar_t* ipt_1
			|| msg.GetType(4) != VTYPE_WIDESTR		// const wchar_t* ipt_2
			|| msg.GetType(5) != VTYPE_WIDESTR )	// const wchar_t* desc
		{
			return false; 
		}	
	}
	else if (cmd == GMCC_C2S_EXCH_GIFT)
	{
		if (count != 3
			|| msg.GetType(2) != VTYPE_STRING)		// const string* exch_key
		{
			return false;
		}
	}
	else if (cmd == GMCC_C2S_PICK_PAYED_REBATE)
	{
		return count >= 2;
	}
	else
	{
		if (count != 3 || msg.GetType(2) != VTYPE_WIDESTR)
		{
			return false; 
		}	
	}
	return true;
}

void GmccModule::OnPayedPushResult(IKernel* pKernel, const char* pszRoleId, const char* pszOrder, int nResult)
{
	Assert(NULL != pszRoleId);
	Assert(NULL != pszOrder);
	if (StringUtil::CharIsNull(pszRoleId) || StringUtil::CharIsNull(pszOrder))
	{
		::extend_warning(LOG_ERROR, "OnPayedPushResult args invalid.");
		return;
	}

	// 回应gmcc服务器
	OnOrderStateChanged(pKernel, pszRoleId, pszOrder, EM_ORDER_STATE_ARRIVED);
	
	// 如果成功 通知玩家领取
	if (nResult == PAYED_RESULT_SUCCED)
	{
		const wchar_t* pwszRoleName = pKernel->SeekRoleName(pszRoleId);
		if (StringUtil::CharIsNull(pwszRoleName))
		{
			::extend_warning(LOG_ERROR, "payed failed, role '%s' not exists(order:%s).", pszRoleId, pszOrder);
		}
		else
		{
			CVarList var;
			var << COMMAND_GMCC << GMCC_CMD_EXEC_PAYED << pszOrder;
			pKernel->CommandByName(pwszRoleName, var);
		}
	}
}

void GmccModule::OnPayedCompleted(IKernel* pKernel, const PERSISTID& player, const char* pszOrder)
{
	if ( OnPayedRecvItems(pKernel, player, pszOrder) <= 0)
	{
		// 未成功  等一会查询，  公共服务器数据还没有同步过来
		if (pKernel->FindHeartBeat(player, "GmccModule::HB_PayedWait") == 0)
		{
			// 15秒还未收到 刚不再处理，上线后再处理
			pKernel->AddCountBeat(player, "GmccModule::HB_PayedWait", 3000, 5);
		}
	}
}

int GmccModule::HB_PayedWait(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 刷新所有列表
	if (m_pGmccModule->OnPayedRecvItems(pKernel, self) > 0)
	{
		pKernel->RemoveHeartBeat(self, "GmccModule::HB_PayedWait");
	}
	return 0;
}

bool GmccModule::SendPayedItem(IKernel* pKernel, const PERSISTID& player, IRecord* pPubPayedRec, int nRowId, IRecord* pPayedRec, IRecord* pPayedProducts)
{
	Assert(pPubPayedRec);
	Assert(pPayedRec);
	Assert(nRowId > -1);

	// 将数据插入到玩家表中
	const char* pszRoleId = pKernel->QueryString(player, FIELD_PROP_UID);
	Assert(pszRoleId);

	PayedOrderData data;
	data.OrderId = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_ORDER);
	Assert(!StringUtil::CharIsNull(data.OrderId.c_str()));
	data.ProductId = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_PRODUCT_ID);
	data.Items = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_ITEMS);
	data.Rewards = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_REWARDS);
	data.GiftRewords = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_GIFTS);
	data.Extras = pPubPayedRec->QueryString(nRowId, PUB_COL_GNR_PAYED_EXTRAS);
	data.Amount = pPubPayedRec->QueryFloat(nRowId, PUB_COL_GNR_PAYED_AMOUNT);
	data.IsRealPay = pPubPayedRec->QueryInt(nRowId, PUB_COL_GNR_PAYED_BILL_TYPE) == EM_BILL_TYPE_REAL;
	data.ProductRule = pPubPayedRec->QueryInt(nRowId, PUB_COL_GNR_PAYED_PRODUCT_RULE);
	data.MaxPurchase = pPubPayedRec->QueryInt(nRowId, PUB_COL_GNR_PAYED_PRODUCT_NUMBER);
	data.nOrderTime = pPubPayedRec->QueryInt64(nRowId, PUB_COL_GNR_PAYED_ORDER_TIME);
	data.nPayedTime = pPubPayedRec->QueryInt64(nRowId, PUB_COL_GNR_PAYED_PAYED_TIME);
	Assert(data.Amount > 0);


	int nRowIndex = pPayedRec->FindString( COLUMN_PAYED_ORDER_ORDER_ID, data.OrderId.c_str() );
	if (nRowIndex >= 0)
	{
		::extend_warning(LOG_WARNING, "order had received, don't get again[roleid:%s order:%s item:%s rewards:%s amount:%f]",
									pszRoleId, data.OrderId.c_str(), data.Items.c_str(), data.Rewards.c_str(), data.Amount);
		return true;
	}

	// 输出一个跟踪日志
	::extend_warning(LOG_INFO, "payed[roleid:%s order:%s product:%s item:%s rewards:%s amount:%f]", 
							pszRoleId, data.OrderId.c_str(), data.ProductId.c_str(), data.Items.c_str(), data.Rewards.c_str(), data.Amount);
	
	return HandleOrderSucceed( pKernel, player, pPayedRec, pPayedProducts, data );
}

bool GmccModule::HandlePayment(IKernel* pKernel, const char* pszRoleId, 
												 const char* pszOrderId, 
												 const char* pszProductId, 
												 float fAmount, int nPayType,
												 int64_t nCreatedTime, 
												 int64_t nPayedTime)
{
	Assert(!StringUtil::CharIsNull(pszRoleId));
	Assert(!StringUtil::CharIsNull(pszOrderId));
	Assert(!StringUtil::CharIsNull(pszProductId));
	Assert(fAmount > 0);

	extend_warning(LOG_INFO, "[GmccModule::HandlePayment]payed recv payed data from gmcc[roleid:%s order:%s product_id:%s fAmount:%f pay_type:%d]",
									pszRoleId, pszOrderId, pszProductId, fAmount, nPayType);
	Assert(!StringUtil::CharIsNull(pszRoleId));
	Assert(!StringUtil::CharIsNull(pszOrderId));
	Assert(!StringUtil::CharIsNull(pszProductId));
	Assert(fAmount > 0);

	// 验证参数
	if (StringUtil::CharIsNull(pszRoleId) || StringUtil::CharIsNull(pszOrderId) || StringUtil::CharIsNull(pszProductId))
	{
		extend_warning(LOG_INFO, "[GmccModule::HandlePayment]push payment data error");
		return false;
	}

	// 先记录到公共服务器
	CVarList msg;
	std::wstring wstr = GetDomainName(pKernel);
	msg << PUBSPACE_DOMAIN << wstr.c_str() << SP_DOMAIN_MSG_GMCC_PAYED_PUSH
		<< pszRoleId << pszOrderId << pszProductId << fAmount << nPayType << nCreatedTime << nPayedTime;
	return pKernel->SendPublicMessage(msg);
}

bool GmccModule::HandleOrderSucceed(	IKernel* pKernel, const PERSISTID& player, 
										IRecord* pPayedRec,
										IRecord* pPayedProducts,
										const PayedOrderData& data	)
{
	int nRows = pPayedRec->GetRows();
	int nMax = pPayedRec->GetRowMax();
	if (nRows > nMax)
	{
		// 删除最前一条记录
		pPayedRec->RemoveRow(0);
	}

	// 添加记录
	int nFindIndex_ = -1;
	int nPurchased_ = 0;
	if (NULL != pPayedProducts)
	{
		nFindIndex_ = pPayedProducts->FindString(COLUMN_PAYED_PRODUCTS_PRODUCT_ID, data.ProductId.c_str());
		if (nFindIndex_ == -1)
		{
			CVarList values;
			values << data.ProductId.c_str() << 0 << 0 << (int64_t)0;
			nFindIndex_ = pPayedProducts->AddRowValue(-1, values);
		}
		else
		{
			nPurchased_ = pPayedProducts->QueryInt(nFindIndex_, COLUMN_PAYED_PRODUCTS_PURCHASE_TIMES);
		}
	}

	const char* pszItems_ = data.Items.c_str();
	bool bInDurationMode_ = false;
	bool bOverload_ = false;
	// 根据规则计算出最终的物品
	if (data.ProductRule != EM_PRODUCT_RULE_NORMAL && data.MaxPurchase > 0)
	{
		bInDurationMode_ = IsInPurchaseDuration(data.ProductRule, data.nOrderTime);
		if (bInDurationMode_ && nPurchased_ >= data.MaxPurchase)
		{
			pszItems_ = data.Extras.c_str();
			bOverload_ = true;
		}
	}

	// 发送邮件通知
	const wchar_t* pRoleName = pKernel->QueryWideStr(player, FIELD_PROP_NAME);
	std::string strProductNo = "{#";
	strProductNo.append(data.ProductId);
	strProductNo.append("}");
	std::string strDate;
	CVarList params;
	try
	{
		char szbuf[64] = { 0 };
		time_t time_ = (time_t)data.nPayedTime;
		util_convert_time_to_string(&time_, szbuf, 64);
		strDate = szbuf;
	}
	catch (...)
	{
		::extend_warning(LOG_ERROR, "convert payed time error(time:%lld)", data.nPayedTime);
	}
	params << strDate << strProductNo;
	SystemMailModule::SendMailToPlayer(pKernel, MAIL_NAME_PAYED, pRoleName, params, std::string(), FUNCTION_EVENT_ID_PAYED);

	// 在时间范围内且未超出购买次数
	if ( !bOverload_ )
	{
		// 月卡/终身卡处理
		if (stricmp(EnvirValueModule::EnvirQueryString(ENV_VALUE_MONTHOD_CARD_NO), data.ProductId.c_str()) == 0)
		{
			// 忽略物品项
			ShopModule::AddWelfareCard(pKernel, player, EM_CARD_MONTH);
			pszItems_ = "";
		}
		else if (stricmp(EnvirValueModule::EnvirQueryString(ENV_VALUE_FORERVER_CARD_NO), data.ProductId.c_str()) == 0)
		{
			// 第二次购买用额外物品代替
			bool bOK = ShopModule::AddWelfareCard(pKernel, player, EM_CARD_FORERVER);
			if (bOK)
			{
				pszItems_ = "";
			}
		}
	}

	if ( !StringUtil::CharIsNull(pszItems_) )
	{
		// 发放奖励
		RewardModule::AwardEx award;
		award.name = MAIL_NAME_PAYED_ITEM;
		award.srcFunctionId = FUNCTION_EVENT_ID_PAYED;

		CVarList mail_param;
		RewardModule::ParseAwardData(pszItems_, award);
		bool bOK = RewardModule::m_pRewardInstance->RewardPlayer(pKernel, player, &award, mail_param);
		Assert(bOK);
	}

	// 添加支付记录
	CVarList row;
	row << data.OrderId.c_str() << data.ProductId.c_str() << pszItems_ << data.Rewards.c_str() << data.GiftRewords.c_str()
				<< data.Amount << data.nOrderTime << data.nPayedTime ;
	pPayedRec->AddRowValue(-1, row);

	// 奖励物品
	if (!StringUtil::CharIsNull(data.Rewards.c_str()))
	{
		RewardModule::AwardEx award;
		award.name = MAIL_NAME_PAYED_REWORD;
		award.srcFunctionId = FUNCTION_EVENT_ID_PAYED_PRODUCTION_REWORD;

		CVarList mail_param;
		RewardModule::ParseAwardData(data.Rewards.c_str(), award);
		RewardModule::m_pRewardInstance->RewardPlayer(pKernel, player, &award, mail_param);
	}

	// 奖励物品首充
	if (!StringUtil::CharIsNull(data.GiftRewords.c_str()) && nFindIndex_ >= 0)
	{
		int nTotalPurchased_ = pPayedProducts->QueryInt(nFindIndex_, COLUMN_PAYED_PRODUCTS_TATAL_TIMES);
		if (nTotalPurchased_ == 0)
		{
			RewardModule::AwardEx award;
			award.name = MAIL_NAME_PAYED_FIRST_PRODUCT;
			award.srcFunctionId = FUNCTION_EVENT_ID_PAYED_PRODUCTION_FIRST_REWORD;

			CVarList mail_param;
			RewardModule::ParseAwardData(data.GiftRewords.c_str(), award);
			RewardModule::m_pRewardInstance->RewardPlayer(pKernel, player, &award, mail_param);
		}
	}

	OnRechargeSucceed(pKernel, player, data.OrderId.c_str(), data.Amount, data.IsRealPay);

	// 增加记录
	if (nFindIndex_ > -1)
	{
		int nTotalPurchased_ = pPayedProducts->QueryInt(nFindIndex_, COLUMN_PAYED_PRODUCTS_TATAL_TIMES);
		pPayedProducts->SetInt(nFindIndex_, COLUMN_PAYED_PRODUCTS_TATAL_TIMES, nTotalPurchased_ + 1);
		if (bInDurationMode_)
		{
			pPayedProducts->SetInt(nFindIndex_, COLUMN_PAYED_PRODUCTS_PURCHASE_TIMES, nPurchased_ + 1);
			pPayedProducts->SetInt64(nFindIndex_, COLUMN_PAYED_PRODUCTS_ORDER_TIME, data.nOrderTime);
		}
	}

	// 通知客户端收取成功
	ShopModule::OnReciveProduct( pKernel, player, data.OrderId.c_str(), data.ProductId.c_str() );
	return true;
}

bool GmccModule::IsInPurchaseDuration(int nRule, int64_t tTime)
{
	switch ( nRule )
	{
	case EM_PRODUCT_RULE_DAILY:
		return util_is_sameday(tTime, util_get_utc_time());
		break;
	case EM_PRODUCT_RULE_ONCE:
		return true;
	case EM_PRODUCT_RULE_WEEK:
		return util_is_sameweek(tTime, util_get_utc_time());
		break;
	case EM_PRODUCT_RULE_MONTH:
		return util_is_samemonth(tTime, util_get_utc_time());
		break;
	case EM_PRODUCT_RULE_YEAR:
		return util_is_sameyear(tTime, util_get_utc_time());
		break;
	default:
		break;
	}

	return false;
}

void GmccModule::OnRechargeSucceed(IKernel* pKernel, const PERSISTID& player, const char* pszOrderId, float fAmout, bool bIsReal)
{
	float fPayed = pKernel->QueryFloat(player, FIELD_PROP_TOTAL_PAY);
	bool bFirst = fPayed < 0.01; // 首充
	fPayed += fAmout;
	pKernel->SetFloat(player, FIELD_PROP_TOTAL_PAY, fPayed);
	pKernel->IncInt(player, FIELD_PROP_PAY_TIMES, 1);
	// 增加VIP经验
	pKernel->IncInt(player, FIELD_PROP_VIP_EXP, (int)fAmout);

	// 增加真实充值金额
	if (bIsReal)
	{
		pKernel->IncFloat(player, FIELD_PROP_TOTAL_PAY_REAL, fAmout);
	}

	LogModule::m_pLogModule->OnPayed(pKernel, player, pszOrderId, fAmout, 0, bFirst, bIsReal, NULL, NULL);

	// 首充处理
	if (bFirst)
	{
		ShopModule::OnFirstPayed(pKernel, player, fAmout);
	}
	// 称号：首冲，累计金额，累计次数
	MiddleModule::Instance()->OnTitleCondition(pKernel, player, ETitleCond_PayedFirst, (int)fAmout);
	MiddleModule::Instance()->OnTitleCondition(pKernel, player, ETitleCond_PayedSum, (int)fPayed);
	MiddleModule::Instance()->OnTitleCondition(pKernel, player, ETitleCond_PayedCount, pKernel->QueryInt(player, FIELD_PROP_PAY_TIMES));

	// 统计充值数
	pKernel->Command(player, player, CVarList() << COMMAND_DYNAMIC_ACT << DYNAMIC_ACT_SUB_MSG_FILL_SILVER << (int)fPayed << (int)fAmout);
	// 响应充值发红包
	if (RedPacketModule::m_pRedPacketModule != NULL)
	{
		RedPacketModule::m_pRedPacketModule->OnSendChargeSystemPacket(pKernel, player, (int)fAmout);
	}
}

void GmccModule::OnPropQuery(IKernel* pKernel, IGameObj* pPlayer, const IVarList& keys, size_t nOffset, int64_t nQueryId)
{
	if (NULL == pPlayer) return;
	CVarList ret;
	ret << PROTOCOL_ID_ROLE_PROP_DATA << nQueryId;
	const char* pszRoleId = pPlayer->QueryString(FIELD_PROP_UID);
	if (StringUtil::CharIsNull(pszRoleId))
	{
		ret << DATA_QUERY_ROLEID_ERR << "";
	}
	else
	{
		size_t nCount = keys.GetCount();
		if (nCount <= nOffset)
		{
			ret << DATA_QUERY_KEY_ERROR << pszRoleId;
		}
		else
		{
			LoadPropsValue(pPlayer, ret, keys, nOffset);
		}
	}

	pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
}

void GmccModule::OnRecordQuery(IKernel* pKernel, IGameObj* pPlayer, const char* pRecName, int nOffset, int nCount, int64_t nQueryId)
{
	if (NULL == pPlayer) return;
	if (NULL == pRecName) return;

	CVarList ret;
	ret << PROTOCOL_ID_ROLE_RECORD_DATA << nQueryId;
	const char* pszRoleId = pPlayer->QueryString(FIELD_PROP_UID);
	if (StringUtil::CharIsNull(pszRoleId))
	{
		ret << "" << pRecName << DATA_QUERY_ROLEID_ERR << nOffset << nCount << 0;
	}
	else
	{
		ret << pszRoleId << pRecName;
		IRecord* pRecord = pPlayer->GetRecord(pRecName);
		if ( NULL == pRecord )
		{
			ret << DATA_QUERY_KEY_ERROR << nOffset << nCount << 0;
		}
		else
		{
			ret << DATA_QUERY_SUCCESS << nOffset << nCount << pRecord->GetCols();
			LoadRecordData(pPlayer, pRecord, nOffset, nCount, ret );
		}
	}

	pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
}

void GmccModule::OnSyncGmccSrvInfo(IKernel* pKernel, const IVarList& msg, int nOffset)
{
	assert((int)msg.GetCount() >= nOffset + 6);
	if ((int)msg.GetCount() < nOffset + 6)
	{
		::extend_warning(LOG_ERROR, "cmd sync gmcc srv info param error.");
		return;
	}

	SrvInfo _info;
	_info.Identity = msg.Int64Val(nOffset++);				// nOffset + 0
	_info.GameId = msg.Int64Val(nOffset++);					// nOffset + 1
	_info.ChannelId = msg.Int64Val(nOffset++);				// nOffset + 2
	_info.ProductionId = msg.Int64Val(nOffset++);			// nOffset + 3
	_info.ServerId = msg.Int64Val(nOffset++);				// nOffset + 4
	_info.AreaId = msg.Int64Val(nOffset++);					// nOffset + 5
	_info.DeployId = msg.Int64Val(nOffset++);				// nOffset + 6

	LogModule::m_pLogModule->SetSrvInfo(pKernel, _info);
}

int GmccModule::HB_ReportMemberInfo(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 上报服务器状态
	int nCurOnlineCount = pKernel->GetOnlineCount();
	int nMemberId = pKernel->GetMemberId();
	int nWorkState = 0;

	CVarList ret;
	ret << PROTOCOL_ID_REPORT_MEMBER_INFO << nCurOnlineCount << nMemberId << nWorkState;
	pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
	return 0;
}

int GmccModule::HB_RequestSrvInfo(IKernel* pKernel, const PERSISTID& self, int slice)
{
	if (ms_bRecivedSrvInfo)
	{
		// 启动请求服务信息心跳
		if (pKernel->FindHeartBeat(pKernel->GetScene(), "GmccModule::HB_RequestSrvInfo"))
		{
			// 1分钟
			pKernel->RemoveHeartBeat(pKernel->GetScene(), "GmccModule::HB_RequestSrvInfo");
		}
	}
	else
	{
		// 先发送准备好了消息
		CVarList args;
		args << PROTOCOL_ID_GAME_READY;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);

		// 再求请服务器信息
		CVarList ret;
		ret << PROTOCOL_ID_GAME_REQ_SRV_INFO;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
	}
	return 0;
}

int GmccModule::OnPayedRecvItems(IKernel* pKernel, const PERSISTID& player, const char* pszOrder /*= NULL*/)
{
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace)
	{
		return 0;
	}
	IPubData * pPubData = pPubSpace->GetPubData(GetDomainName(pKernel).c_str());
	if (pPubData == NULL)
	{
		return 0;
	}

	IRecord* pPubPayedRec = GetPubRecord(pKernel, PUB_GMCC_PAYED_REC);
	if (NULL == pPubPayedRec)
	{
		return 0;
	}

	IRecord* pPayedRec = pKernel->GetRecord(player, FIELD_RECORD_PAYED_ORDER);
	if (NULL == pPayedRec)
	{
		return 0;
	}

	const char* pszRoleId = pKernel->QueryString(player, FIELD_PROP_UID);
	if (StringUtil::CharIsNull(pszRoleId))
	{
		return 0;
	}	

	IRecord* pPayedProducts_ = pKernel->GetRecord(player, FIELD_RECORD_PAYED_PRODUCTS);
	int nCount = 0;
	const std::wstring& pubName = GetDomainName(pKernel);
	int nRows = pPubPayedRec->GetRows();
	if (StringUtil::CharIsNull(pszOrder))
	{
		// 处理所有的订单
		LoopBeginCheck(a);
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(a);
			const char* pszRecRoleId = pPubPayedRec->QueryString(i, PUB_COL_GNR_PAYED_ROLE_ID);
			if (strcmp(pszRecRoleId, pszRoleId) == 0)
			{
				if (SendPayedItem(pKernel, player, pPubPayedRec, i, pPayedRec, pPayedProducts_))
				{
					++nCount;

					const char* pszRecOrderId = pPubPayedRec->QueryString(i, PUB_COL_GNR_PAYED_ORDER);
					// 删除订单
					CVarList var;
					var << PUBSPACE_DOMAIN << pubName.c_str() << SP_DOMAIN_MSG_GMCC_PAYED_PICKED << pszRoleId << pszRecOrderId;
					pKernel->SendPublicMessage(var);

					// 通知Gmcc状态改变
					OnOrderStateChanged(pKernel, pszRoleId, pszRecOrderId, EM_ORDER_STATE_TAKE);
				}
			}
		}
	}
	else
	{
		// 处理指定订单
		LoopBeginCheck(a);
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(a);
			const char* pszRecRoleId = pPubPayedRec->QueryString(i, PUB_COL_GNR_PAYED_ROLE_ID);
			const char* pszRecOrderId = pPubPayedRec->QueryString(i, PUB_COL_GNR_PAYED_ORDER);
			if (strcmp(pszRecRoleId, pszRoleId) == 0 && strcmp(pszOrder, pszRecOrderId) == 0 )
			{
				if (SendPayedItem(pKernel, player, pPubPayedRec, i, pPayedRec, pPayedProducts_))
				{
					++nCount;
					// 删除订单
					CVarList var;
					var << PUBSPACE_DOMAIN << pubName.c_str() << SP_DOMAIN_MSG_GMCC_PAYED_PICKED << pszRoleId << pszRecOrderId;
					pKernel->SendPublicMessage(var);

					// 通知Gmcc状态改变
					OnOrderStateChanged(pKernel, pszRoleId, pszRecOrderId, EM_ORDER_STATE_TAKE);
				}
				break;
			}
		}
	}

	return nCount;
}

void GmccModule::OnCommandRebateInfo(IKernel* pKernel, const PERSISTID& player, const IVarList& args, int nOffset)
{
	int nType_ = args.IntVal(nOffset++);
	int nRebate_ = args.IntVal(nOffset++);
	if (nRebate_ > 0)
	{
		pKernel->SetInt(player, FIELD_PROP_PAYED_REBATE, nRebate_);
		pKernel->SetInt(player, FIELD_PROP_PAYED_REBATE_TYPE, nType_);

		// 发送通知
		CVarList retMsg;
		retMsg << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_PAYED_REBATE_NOTIFY << nRebate_ << nType_;
		pKernel->Custom(player, retMsg);
	}
	else
	{
		PlayerBaseModule::AddPlayerGlobalFlag(pKernel, player, EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED);
	}
}

void GmccModule::OnCommandRebateResult(IKernel* pKernel, const PERSISTID& player, const IVarList& args, int nOffset)
{
	int nResult = args.IntVal(nOffset++);
	int nRebate = args.IntVal(nOffset++);

	if (nResult == EM_REBATE_RESULT_SUCCEED)
	{
		Assert(nRebate > 0);
		bool bOk = CapitalModule::m_pCapitalModule->IncCapital(pKernel, player, CAPITAL_SILVER, nRebate, FUNCTION_EVENT_ID_TEST_PAYED_REBATE);
		if (bOk)
		{
			// 修改标志 并返回成功消息
			pKernel->SetInt(player, FIELD_PROP_PAYED_REBATE, nRebate);
			PlayerBaseModule::AddPlayerGlobalFlag(pKernel, player, EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED);
		}
		else
		{
			// 输出一个错误，结果无法返回，计费状态已改变了
			::extend_warning(LOG_ERROR, "[GmccModule::OnCommandRebateResult] inc silver failed.");
		}
	}
	else if (nResult == EM_REBATE_RESULT_PICKED)
	{
		pKernel->SetInt(player, FIELD_PROP_PAYED_REBATE, 0);
		PlayerBaseModule::AddPlayerGlobalFlag(pKernel, player, EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED);
	}

	CVarList retMsg;
	retMsg << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_PICK_PAYED_REBATE << nResult;
	pKernel->Custom(player, retMsg);
}

void GmccModule::QueryPayedRebate( IKernel* pKernel, const PERSISTID& player )
{
	// 是否领取过了
	bool bPicked_ = PlayerBaseModule::TestPlayerGlobalFlag( pKernel, player, EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED);
	if (bPicked_)
	{
		// 领取过了什么也不处理
		return;
	}

	IGameObj* pPlayer_ = pKernel->GetGameObj(player);
	if (NULL == pPlayer_)
	{
		return;
	}

	// 是否查询过了 查询过了, 返利值肯定不为0
	int nRebate_ = pPlayer_->QueryInt( FIELD_PROP_PAYED_REBATE );
	if (nRebate_ > 0)
	{
		int nType_ = pPlayer_->QueryInt(FIELD_PROP_PAYED_REBATE_TYPE);
		CVarList retMsg;
		retMsg << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_PAYED_REBATE_NOTIFY << nRebate_ << nType_;
		pKernel->Custom(player, retMsg);
	}
	else
	{
		const char* pszRoleId_ = pPlayer_->QueryString(FIELD_PROP_UID);
		const char* pszUid_ = pPlayer_->QueryString(FIELD_PROP_ACCOUNT);

		// 发消息到web查询返利
		CVarList args;
		args << PROTOCOL_ID_REBATE_QUERY
			<< pszUid_
			<< pszRoleId_;

		pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);
	}
}

// 接收到客户端消息 添加整数编号的对象客户端消息处理函数
int GmccModule::OnCustom(IKernel* pKernel, const PERSISTID& player, const PERSISTID& sender, const IVarList& msg)
{
	// 判断玩家的合法性
	if (!pKernel || !pKernel->Exists(player))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL", "pKernel");
		return 0;
	}
	// 检查参数判断
	if (!m_pGmccModule->CheckCustom(pKernel, player, sender, msg))
	{
		return 0;
	}

	// 检查GMCC的cmd类型
	const int cmd = msg.IntVal(1);
	if ( cmd == GMCC_C2S_ACCUSE )
	{ 
		// 客户端举报玩家
		const wchar_t* accusedname = msg.WideStrVal(2);					// 被举报的名字
		if (StringUtil::CharIsNull(accusedname))
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL or Empty !", "name");
			return 0;
		}
		int why = msg.IntVal(3);
		const wchar_t* reas = msg.WideStrVal(4);
		const wchar_t* baseInfo = msg.WideStrVal(5);
		if ( !reas || !baseInfo)
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL!", "reason|baseInfo");
			return 0;
		}

		//GmccModule::SendAccuseInfoToPlayer(pKernel, player, accusedname, why, reas, baseInfo);
	}
	else if( cmd == GMCC_C2S_BUG || cmd == GMCC_C2S_EXP )
	{	
		// 客户端提交BUG或游戏提交
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL or Empty!", "Account|Name");
			return 0;
		}
		const char* account= pPlayer->QueryString("Account");	//玩家账号
		const wchar_t* name = pPlayer->QueryWideStr("Name");	//玩家姓名
		if (StringUtil::CharIsNull(account) || StringUtil::CharIsNull(name))
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL or Empty!", "Account|Name");
			return 0;
		}

		int level = pPlayer->QueryInt("Level");					//等级
		int jobid = pPlayer->QueryInt("Job");					//职业

		const wchar_t* index_type = msg.WideStrVal(2);   
		const wchar_t* ipt_1 = msg.WideStrVal(3); 	
		const wchar_t* ipt_2 = msg.WideStrVal(4);
		const wchar_t* desc  = msg.WideStrVal(5);
		if (!index_type || !ipt_1 || !ipt_2 || !desc)
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL !", " index_type|ipt_1|ipt_2|desc");
			return 0;
		}

		char strtime[256] = {0};
		time_t ttime = time(NULL);
		util_convert_time_to_string(&ttime, strtime, sizeof(strtime));

		int nation = pPlayer->QueryInt("Nation");
		std::wstring roleCamp = util_int_as_widestr(nation);	// 阵营 -> 国家
		std::wstring wstrJobID = util_int64_as_widestr(jobid);	// 职业

		int reort_type = cmd - 2;								// 类型 0 bug 1 advise
		const char* default_string = "";						// 保留字段

		// 游戏服务器向GMCC服务器通报bug和建议收集信息
		CVarList var;
		var << 0x4033 << account << name << level << roleCamp 
			<< wstrJobID << reort_type << index_type << ipt_1 
			<< ipt_2 << desc << strtime << default_string; 

		//pKernel->SendGmccCustom(var);
	}
	else if (cmd == GMCC_C2S_EXCH_GIFT)
	{
		const char* pszKey = msg.StringVal(2);
		if (!pszKey)
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL !", "key");
			return 0;
		}
		m_pGmccModule->OnClientExchGift(pKernel, player, pszKey);
	}
	else if (cmd == GMCC_C2S_PICK_PAYED_REBATE)
	{
		m_pGmccModule->OnClientPickPayedRebate(pKernel, player);
	}
	else
	{
		const wchar_t* info = msg.WideStrVal(2);
		if (!info)
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnCustom] %s is NULL !", "info");
			return 0;
		}
		if (pKernel->Type(player) == TYPE_PLAYER)
		{
			//pKernel->SendToGmcc(player, info);
		}
	}

	return 0;
}

// gmcc服务器->游戏服务器: 发送的控制消息
int GmccModule::OnGmccMessage(IKernel* pKernel, const PERSISTID& scene, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnGmccMessage] %s is NULL", "pKernel");
		return 0;
	}

	// 服务器id
	int index = args.IntVal(0);
	if ( index != ESI_GMCC_SERVER )
	{
		return 0;
	}

	//查看2级消息头
	int msgid = args.IntVal(1);
	::extend_warning(LOG_INFO, "[GmccModule::OnGmccMessage] recv gmcc msg id:%d", msgid);
	switch (msgid)
	{
	case PROTOCOL_ID_GM_EXEC:
		m_pGmccModule->OnDoGmccCommand(pKernel, args);
		break;
	case PROTOCOL_ID_MAIL_POST:
		m_pGmccModule->OnDoGmccMail(pKernel, args);
		break;
	case PROTOCOL_ID_NOTICE_POST:
		m_pGmccModule->OnDoGmccNotice(pKernel, args);
		break;
	case PROTOCOL_ID_NOTICE_DELETE:
		m_pGmccModule->OnDoGmccNoticeDel(pKernel, args);
		break;
	case PROTOCOL_ID_GIFT_RESULT:
		m_pGmccModule->OnDoGmccExchResult(pKernel, args);
		break;
	case PROTOCOL_ID_PRODUCTS_PSH:
		m_pGmccModule->OnDoGmccProductsPush(pKernel, args);
		break;
	case PROTOCOL_ID_PAYMENT_PSH:
		m_pGmccModule->OnDoGamccPaymentPush(pKernel, args);
		break;
	case PROTOCOL_ID_ORDER_INFO:
		m_pGmccModule->OnDoGamccOrderResult(pKernel, args);
		break;
	case PROTOCOL_ID_ROLE_PROP_QUERY:
		m_pGmccModule->OnDoGmccPropQuery(pKernel, args);
		break;
	case PROTOCOL_ID_ROLE_RECORD_QUERY:
		m_pGmccModule->OnDoGmccRecordQuery(pKernel, args);
		break;
	case RPOTOCOL_ID_GAME_INIT_INFO:
		m_pGmccModule->OnDoGmccInitSrvInfo(pKernel, args);
		break;
	case PROTOCOL_ID_GAME_CHECK_READY:
		if (ms_bGameReady)
		{
			m_pGmccModule->OnGameReady(pKernel);
		}
		break;
	case PROTOCOL_ID_PUSH_SHOP_ITEM:
		m_pGmccModule->OnDoGmccPushShopItems(pKernel, args);
		break;
	case PROTOCOL_ID_DEL_SHOP_ITEM:
		m_pGmccModule->OnDoGmccDelShopItems(pKernel, args);
		break;
	case PROTOCOL_ID_QUERY_SHOP_ITEM:
		m_pGmccModule->OnDoGmccQueryShopItems(pKernel, args);
		break;
	case PROTOCOL_ID_SET_ACTIVITY_CONFIG:
		m_pGmccModule->OnDoGmccSetGenActivityConfig(pKernel, args);
		break;
	case PROTOCOL_ID_LOAD_CONFIG:
		m_pGmccModule->OnDoGmccLoadConfig(pKernel, args);
		break;
	case PROTOCOL_ID_REBATE_INFO:
		m_pGmccModule->OnDoGmccRebateInfo(pKernel, args);
		break;
	case PROTOCOL_ID_REBATE_RESULT:
		m_pGmccModule->OnDoGmccRebateResult(pKernel, args);
		break;
	default:
		break;
	}

	return 0;
}

// GMCC服务器->游戏服务器: 给指定名字的场景发送消息
int GmccModule::OnGmccCommand(IKernel* pKernel, const PERSISTID& player, const PERSISTID& sender, const IVarList& msg)
{
	// 判断玩家的合法性
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnGmccCommand] %s is NULL", "pKernel");
		return 0;
	}

	int cmd = msg.IntVal(1);
	switch (cmd)
	{
	case GMCC_CMD_EXEC_GM_COMMAND:
		{
			// 设置执行目标
			if (!pKernel->FindData(player, "GMPropTarget"))
			{
				pKernel->AddData(player, "GMPropTarget", VTYPE_OBJECT);
			}
			pKernel->SetDataObject(player, "GMPropTarget", player);

			int64_t nCmdId_ = msg.Int64Val(2);
			const char* pszCommand = msg.StringVal(3);
			const char* pszRoleId_ = pKernel->QueryString(player, FIELD_PROP_UID);
			std::string strMsg_;
			int nResult = m_pGmccModule->HandleCommand( pKernel, player, pszCommand, msg, 4, strMsg_ );
			CVarList retMsg;
			retMsg << PROTOCOL_ID_GM_REPLY << nCmdId_ << pszRoleId_ << nResult << strMsg_.c_str();
			pKernel->SendExtraMessage(ESI_GMCC_SERVER, retMsg);
		}
		break;
	case GMCC_CMD_EXEC_PAYED:
		{
			const char* pszOrder = msg.StringVal(2);
			m_pGmccModule->OnPayedCompleted(pKernel, player, pszOrder);
		}
		break;
	case GMCC_CMD_EXEC_PROP_QUERY:
	{
		IGameObj* pGameObj = pKernel->GetGameObj(player);
		int64_t nQueryId = msg.Int64Val(2);
		m_pGmccModule->OnPropQuery(pKernel, pGameObj, msg, 3, nQueryId);
	}
	break;
	case GMCC_CMD_EXEC_RECORD_QUERY:
		{
			IGameObj* pGameObj = pKernel->GetGameObj(player);
			int64_t nQueryId = msg.Int64Val(2);
			const char* pszRecName = msg.StringVal(3);
			int nOffset = msg.IntVal(4);
			int nCount = msg.IntVal(5);
			m_pGmccModule->OnRecordQuery(pKernel, pGameObj, pszRecName, nOffset, nCount, nQueryId);
		}
		break;
		//case GMCC_CMD_SYNC_SEV_INFO:
		//{
		//	m_pGmccModule->OnSyncGmccSrvInfo(pKernel, msg, 2);
		//}
		//break;
	case GMCC_CMD_RECV_GLOBAL_MAIL:
		SystemMailModule::RecvGlobalMail( pKernel, player );
		break;
	case GMCC_CMD_REBATE_INFO:
		m_pGmccModule->OnCommandRebateInfo(pKernel, player, msg, 2);
		break;
	case GMCC_CMD_REBATE_RESULT:
		m_pGmccModule->OnCommandRebateResult(pKernel, player, msg, 2);
		break;
	default:
		break;
	}
	return 0;
}

int GmccModule::OnGmccSceneCommand(IKernel* pKernel, const PERSISTID& player, const PERSISTID& sender, const IVarList& msg)
{
	int cmd = msg.IntVal(1);
	if (cmd == GMCC_CMD_SYNC_SEV_INFO)
	{
		int64_t tNow = util_get_utc_time();
		if (!ms_bRecivedSrvInfo || tNow > ms_nLastSyncTimestamp + 5)
		{
			m_pGmccModule->OnSyncGmccSrvInfo(pKernel, msg, 2);

			// 通知计费登陆服务器
			{
				LoginExtModule::OnLoginReady(pKernel);
			}

			// 开启上报状态心跳(1分钟)
			if (!pKernel->FindHeartBeat(player, "GmccModule::HB_ReportMemberInfo"))
			{
				pKernel->AddHeartBeat(player, "GmccModule::HB_ReportMemberInfo", 60000);
			}
			ms_bRecivedSrvInfo = true;
			ms_nLastSyncTimestamp = tNow;
		}
	}
	return 0;
}

bool GmccModule::OnForbidSpeak(IKernel* pKernel, const PERSISTID& player, const IVarList &msg, int nOffset)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return false;
	}

	int nSilenceTime = StringUtil::StringAsInt( msg.StringVal(nOffset) );
	const wchar_t* name = pPlayer->QueryWideStr(FIELD_PROP_NAME);
	const char* account = pPlayer->QueryString(FIELD_PROP_ACCOUNT);

	//禁言的结果
	bool nRes = ChatModule::m_pChatModule->SetSilence(pKernel, player, nSilenceTime);

	//禁言成功  提示玩家
	if (nRes)
	{
		CVarList msgList;
		msgList << SERVER_CUSTOMMSG_SYSINFO << TIPSTYPE_NORMAL_CHAT_MESSAGE << "sys_item_0083" << account << nSilenceTime;
		pKernel->CustomByName(name, msgList);

		// 添加惩罚日志记录:这里是GMCC给指定玩家添加了禁言
		PunishLog log;
		log.gmAccount = account;
		log.type = LOG_PUNISH_GMCC_SLIENCE;
		log.minutes = nSilenceTime;
		LogModule::m_pLogModule->SavePunishLog(pKernel, player, log);

		int64_t nInterval = nSilenceTime * 60;
		int64_t nExpired = util_get_utc_time() + nInterval;
		LogModule::m_pLogModule->OnForbidSpeak(pKernel, player, nInterval, nExpired);
	}

	return nRes;
}

// 响应玩家解除禁言
bool GmccModule::OnAllowSpeak(IKernel* pKernel, const PERSISTID& player, const IVarList &msg, int nOffset)
{
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return false;
	}

	const wchar_t * name = pPlayer->QueryWideStr(FIELD_PROP_NAME);
	const char * account = pPlayer->QueryString(FIELD_PROP_ACCOUNT);

	// 禁言中,删除禁言心跳
	if (pKernel->FindHeartBeat(player, "ChatModule::H_Silence"))
	{
		pKernel->RemoveHeartBeat(player, "ChatModule::H_Silence");
	}

	pPlayer->SetInt("SilenceTick", 0);
	LogModule::m_pLogModule->OnAllowSpeak(pKernel, player);

	return true;
}

bool GmccModule::OnGrantAuth(IKernel* pKernel, const PERSISTID& player, const IVarList &msg, int nOffset)
{
	int nLevel = StringUtil::StringAsInt(msg.StringVal(nOffset));
	pKernel->SetInt(player, FIELD_PROP_GAMEMASTER, nLevel);

	::extend_warning(LOG_INFO, "[GmccModule::OnGrantAuth]grant %s game master level %d",
							pKernel->QueryString(player, FIELD_PROP_UID), nLevel);

	return true;
}

void GmccModule::OnDoGmccCommand(IKernel* pKernel, const IVarList& args)
{
// 	assert(args.GetCount() >= 5);
// 	if (args.GetCount() < 5)
// 	{
// 		// param error
// 		return;
// 	}

	size_t nIndex = 2;
	const char* pszOpRoleId = args.StringVal(nIndex++);
	const char* pszRoleId = args.StringVal(nIndex++);

	// 处理非在线玩家命令
	const char* pszCommand = args.StringVal(nIndex++);
	int64_t nCmdId = args.Int64Val(nIndex++);

	CVarList retMsg;
	retMsg << PROTOCOL_ID_GM_REPLY << nCmdId << pszRoleId;
	int nResult = EM_GMCC_RESULT_SUCCEED;
	std::string strMsg_;
	do 
	{
		if (StringUtil::CharIsNull(pszRoleId))
		{
			// 执行不需要角色的命令
			if (m_pGmccModule->IsCommandRule(pKernel, pszCommand, "run_no_player"))
			{
				PERSISTID player;
				nResult = m_pGmccModule->HandleCommand(pKernel, player, pszCommand, args, nIndex, strMsg_);
			}
			else
			{
				// 通知运营平台玩家不在线
				::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccCommand]lack role id, don't operator by cmd '%s'", pszCommand);
				nResult = EM_GMCC_RESULT_NO_PLAYER;
				strMsg_ = "{\"error\":\"command need role id\"}";
			}
			break;
		}

		extend_warning(LOG_INFO, "[GmccModule::OnDoGmccCommand]execute cmd '%s'(id:%lld)", pszCommand, nCmdId);
		const wchar_t* pwszName = pKernel->SeekRoleName(pszRoleId);
		if ( StringUtil::CharIsNull(pwszName) )
		{
			::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccCommand]not found roleid('%s')", pszRoleId);
			nResult = EM_GMCC_RESULT_NO_PLAYER;
			strMsg_ = "{\"error\":\"role id not found\"}";
			break;
		}

		// 踢人封号
		if (strcmp(pszCommand, GMCC_EXEC_KICK_OUT_IN_TIMER) == 0 || strcmp(pszCommand, GMCC_EXEC_LOCK_PLAYER) == 0)
		{
			int64_t nBlockTime = StringUtil::StringAsInt64(args.StringVal(nIndex));
			int64_t nExpired = util_get_utc_time() + nBlockTime;
			if (PlayerBaseModule::BlockPlayer(pKernel, pwszName, nExpired))
			{
				nResult = EM_GMCC_RESULT_SUCCEED;
				strMsg_ = "{\"content\":\"success\"}";
			}
			else
			{
				nResult = EM_GMCC_RESULT_ERROR;
				strMsg_ = "{\"error\":\"execute block player function failed\"}";
			}
			break;
		}
		// 解封
		else if (strcmp(pszCommand, GMCC_EXEC_UNLOCK_PLAYER) == 0)
		{
			if (PlayerBaseModule::BlockPlayer(pKernel, pwszName, 0))
			{
				nResult = EM_GMCC_RESULT_SUCCEED;
				strMsg_ = "{\"content\":\"success\"}";
			}
			else
			{
				nResult = EM_GMCC_RESULT_ERROR;
				strMsg_ = "{\"error\":\"execute block player function failed\"}";
			}
			break;
		}

		// 玩家必须在线的命令处理
		int nSceneId = pKernel->GetPlayerScene(pwszName);
		if (nSceneId <= 0)
		{
			// 执行不需要在线的命令
			if (m_pGmccModule->IsCommandRule(pKernel, pszCommand, "run_offline"))
			{
				PERSISTID player;
				nResult = m_pGmccModule->HandleCommand(pKernel, player, pszCommand, args, nIndex, strMsg_);
			}
			else
			{
				// 通知运营平台玩家不在线
				::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccCommand]'%s' offline, don't operator by cmd '%s'",
					StringUtil::WideStrAsString(pwszName).c_str(), pszCommand);
				nResult = EM_GMCC_RESULT_PLAYER_OFFLINE;
				strMsg_ = "{\"error\":\"player not on line\"}";
			}
		}
		else
		{
			CVarList var;
			var << COMMAND_GMCC << GMCC_CMD_EXEC_GM_COMMAND << nCmdId << pszCommand;
			var.Append(args, nIndex, args.GetCount() - nIndex);
			pKernel->CommandByName(pwszName, var);

			// 正在执行中 先不返回
			// retMsg << EM_GMCC_RESULT_SUCCEED;
			return;
		}
	} while ( false );

	retMsg << nResult << strMsg_.c_str();
	pKernel->SendExtraMessage(ESI_GMCC_SERVER, retMsg);
}

void GmccModule::OnDoGmccMail(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 13);
	if (args.GetCount() < 13)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccMail]args count error");
		// param error
		return;
	}

	size_t nIndex = 2;
	bool bGlobal = args.BoolVal(nIndex++);				// nIndex + 0
	int64_t nMailId = args.Int64Val(nIndex++);			// nIndex + 1
	int nMailType = args.IntVal(nIndex++);				// nIndex + 2
	int64_t nCreateTime = args.Int64Val(nIndex++);		// nIndex + 3
	int nStatus = args.IntVal(nIndex++);				// nIndex + 4
	int nPeriod = args.IntVal(nIndex++);				// nIndex + 5
	const char* pszTitle = args.StringVal(nIndex++);		// nIndex + 6
	const char* pszContent = args.StringVal(nIndex++);		// nIndex + 7
	const char* pszsenderId = args.StringVal(nIndex++);		// nIndex + 8

	int nRecvCount = args.IntVal(nIndex++);					// nIndex + 9
	CVarList recvers;
	LoopBeginCheck(a);
	for (int i = 0; i < nRecvCount; ++i)
	{
		LoopDoCheck(a);
		const char* pszReciverId = args.StringVal(nIndex++);	// nIndex + 10 + i
		if (!StringUtil::CharIsNull(pszReciverId))
		{
			recvers << pszReciverId;
		}
	}

	::extend_warning(LOG_INFO, "[GmccModule::OnDoGmccMail] recv mail:%lld is global:%s", nMailId, bGlobal ? "true" : "false");
	bool bHasAttach = args.BoolVal(nIndex++);				// nIndex + 10 + nRecvCount
	const char* pszAttachInfo = args.StringVal(nIndex++);	// nIndex + 11 + nRecvCount

	const wchar_t* pwszSender = L"";
	if (!StringUtil::CharIsNull(pszsenderId))
	{
		pwszSender = pKernel->SeekRoleName(pszsenderId);
	}
	if (NULL == pwszSender) pwszSender = L"";

	Assert(NULL != pszTitle);
	Assert(NULL != pszContent);

	std::wstring strTitle;
	std::wstring strContent;
	if (NULL != pszTitle)
	{
		strTitle = StringUtil::UTF8StringAsWideStr(pszTitle);
	}
	if (NULL != pszContent)
	{
		strContent = StringUtil::UTF8StringAsWideStr(pszContent);
	}

	std::string strAttach;
	if (bHasAttach && NULL != pszAttachInfo )
	{
		strAttach = pszAttachInfo;
	}

	CVarList retMsg;
	retMsg << PROTOCOL_ID_MAIL_RESULT << nMailId;
	if (bGlobal)
	{
		if (SystemMailModule::SendGlobalMail(pKernel, pwszSender, strTitle.c_str(), strContent.c_str(), strAttach.c_str(), FUNCTION_EVENT_ID_BS_SEND_GLOBAL_MAIL, nMailId))
		{
			retMsg << EM_GMCC_RESULT_SUCCEED;
		}
		else
		{
			retMsg << EM_GMCC_RESULT_ERROR;
		}
		// 全局邮件标志
		retMsg << 1;
	}
	else
	{
		// 全局邮件标志
		retMsg << EM_GMCC_RESULT_SUCCEED << 0;
		Assert(recvers.GetCount() > 0);
		size_t nCount = recvers.GetCount();
		if (nCount == 0)
		{
			::extend_warning(LOG_ERROR, "gmcc mail recv error.");
		}

		LoopBeginCheck(a);
		int nCountPos = (int)retMsg.GetCount();
		retMsg << nCount;
		for ( size_t i = 0; i < nCount; ++i)
		{
			LoopDoCheck(a);
			const char* pszRecv = recvers.StringVal(i);
			const wchar_t* pwszRecv = pKernel->SeekRoleName(pszRecv);
			retMsg << pszRecv;
			if ( StringUtil::CharIsNull(pwszRecv))
			{
				::extend_warning(LOG_ERROR, "gmcc mail receiver not found(role id:%s).", pszRecv);
				retMsg << EM_GMCC_RESULT_NO_PLAYER;
			}
			else
			{
				if (SystemMailModule::SendMailToPlayer(pKernel, pwszSender, pwszRecv, strTitle.c_str(), strContent.c_str(), strAttach, FUNCTION_EVENT_ID_GMCC_WEB))
				{
					retMsg << EM_GMCC_RESULT_SUCCEED;
				}
				else
				{
					retMsg << EM_GMCC_RESULT_NO_PLAYER;
				}
			}
		}
	}

	pKernel->SendExtraMessage(ESI_GMCC_SERVER, retMsg);
}

void GmccModule::OnDoGmccNotice(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 9);
	if (args.GetCount() < 9)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	std::wstring& strTitle = StringUtil::StringAsWideStr( args.StringVal(nIndex++) );		// nIndex + 0
	std::wstring& strContent = StringUtil::StringAsWideStr(args.StringVal(nIndex++));		// nIndex + 1
	int64_t nNoticeId = args.Int64Val(nIndex++);				// nIndex + 2
	int64_t nCreateTime = args.Int64Val(nIndex++);				// nIndex + 3
	int64_t nEndTime = args.Int64Val(nIndex++);					// nIndex + 4
	int32_t nInterval = (int)args.Int64Val(nIndex++);			// nIndex + 5
	int32_t nLoop = args.IntVal(nIndex++);						// nIndex + 6
	int32_t nType = args.IntVal(nIndex++);						// nIndex + 7
 	// 0或-1无限循环 -1方便处理
	if (nLoop == 0)
	{
		nLoop = -1;
	}

	/*!
	* @brief	增加gmcc公告
	* @param	int			通告个数n
	以下循环n次
	* @param	int64		通告id
	* @param	string		通告内容
	* @param	int			是否循环播放
	* @param	int			播放间隔时间
	*/

	// 发送公告
	CVarList msg;
	// 计算公告开始剩余时间
	int64_t tNow = util_get_utc_time();
	int32_t nLeft = 0;
	if (nCreateTime > tNow)
	{
		nLeft = (int32_t)(nCreateTime - tNow);
	}
	msg << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_ADD_NOTICE << 1 << nNoticeId << strContent << nLoop << nInterval << nType << nLeft << nEndTime;
	pKernel->CustomByWorld(msg);

	// 记录公告数据 
	msg.Clear();
	std::wstring wstr = GetDomainName(pKernel);
	msg << PUBSPACE_DOMAIN << wstr.c_str() << SP_DOMAIN_MSG_GMCC_ADD_NOTICE << nNoticeId << strTitle << strContent << nCreateTime << nEndTime << (int)nInterval << nLoop << nType;
	pKernel->SendPublicMessage(msg);
}

void GmccModule::OnDoGmccNoticeDel(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 3);
	if (args.GetCount() < 3)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	int64_t nNoticeId = args.Int64Val(nIndex++);				// nIndex + 1

	CVarList msg;
	msg << SERVER_CUSTOMMSG_GMCC << GMCC_S2C_DELETE_NOTICE << nNoticeId;
	pKernel->CustomByWorld(msg);

	// 记录公告数据 
	msg.Clear();
	std::wstring wstr = GetDomainName(pKernel);
	msg << PUBSPACE_DOMAIN << wstr.c_str() << SP_DOMAIN_MSG_GMCC_DELETE_NOTICE << nNoticeId;
	pKernel->SendPublicMessage(msg);
}

void GmccModule::OnDoGmccExchResult(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 7);
	if (args.GetCount() < 7)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	const char* pszRoleId = args.StringVal(nIndex++);		// nIndex + 0
	const char* pszKey = args.StringVal(nIndex++);			// nIndex + 1
	int nResult = args.IntVal(nIndex++);					// nIndex + 2
	const char* pszCapitals = args.StringVal(nIndex++);		// nIndex + 3
	const char* pszItems = args.StringVal(nIndex++);		// nIndex + 4

	Assert(NULL != pszRoleId);
	if (NULL == pszRoleId)
	{
		::extend_warning(LOG_ERROR, "exchange gift result role id is null."); 
		return;
	}
	const wchar_t* pwszRoleName = pKernel->SeekRoleName(pszRoleId);
	if (StringUtil::CharIsNull(pwszRoleName))
	{
		::extend_warning(LOG_ERROR, "exchange gift result role id not found(role id:%s).", pszRoleId);
		return;
	}

	if (nResult == EM_GIFT_CODE_SUCCESS)
	{
		std::string attachData;
		if (NULL != pszCapitals)
		{
			attachData.append(pszCapitals);
		}
		attachData.append(";");
		if (NULL != pszItems)
		{
			attachData.append(pszItems);
		}

		// 根据奖励的名字，获取邮件模板
		const ConfigMailItem& mail_template = SystemMailModule::GetConfigMail(pKernel, "mail_exch_gift_result");
		SystemMailModule::SendMailToPlayer(pKernel, mail_template.sender.c_str(),
			pwszRoleName,
			mail_template.mail_title.c_str(),
			mail_template.mail_content.c_str(),
			attachData, FUNCTION_EVENT_ID_SYSTEM_GIF);

		// 成功提示
		CVarList msgList;
		msgList << SERVER_CUSTOMMSG_SYSINFO << TIPSTYPE_SHOW_WINDOW_MESSAGE << TIP_EXCH_GIFT_SUCCEED;
		pKernel->CustomByName(pwszRoleName, msgList);
	}
	else
	{
		CVarList msgList;
		msgList << SERVER_CUSTOMMSG_SYSINFO << TIPSTYPE_SHOW_WINDOW_MESSAGE;
		// 失败提示
		switch (nResult)
		{
		case EM_GIFT_CODE_INVLID:		// 未找到
			msgList << TIP_EXCH_GIFT_INVALID;
			break;
		case EM_GIFT_CODE_TAKED:		// 已领取
			msgList << TIP_EXCH_GIFT_TAKED;
			break;
		case EM_GIFT_CODE_EXPIRED:		// 已过期
			msgList << TIP_EXCH_GIFT_EXPIRED;
			break;
		case EM_GIFT_CODE_LIMIT:			// 礼包账取次数受限制
			msgList << TIP_EXCH_GIFT_LIMIT;
			break;
		default:
			msgList << TIP_EXCH_GIFT_UNKOWN;
			break;
		}
		pKernel->CustomByName(pwszRoleName, msgList);
	}
}

std::string ConcatPayedRewards(const IVarList& args, size_t& nIndex, int nCount)
{
	// 拼接物品和奖励
	//std::stringstream items;
	//std::stringstream captials;
	//LoopBeginCheck(a);
	//for (int i = 0; i < nCount; ++i)
	//{
	//	LoopDoCheck(a);
	//	const char* pszItem = args.StringVal(nIndex++);
	//	int nNumber = args.IntVal(nIndex++);
	//	assert(!StringUtil::CharIsNull(pszItem));
	//	assert(nNumber > 0);
	//	// 是否是货币
	//	if (CapitalModule::m_pCapitalModule->IsCapitalDefined(pszItem))
	//	{
	//		captials << pszItem << ":" << nNumber << ",";
	//	}
	//	else
	//	{
	//		items << pszItem << ":" << nNumber << ",";
	//	}
	//}

	//std::string rewards = captials.str();
	//if (rewards.size() > 0)
	//{
	//	// 截断最后一个字符串
	//	rewards[rewards.size() - 1] = 0;
	//}
	//rewards.append(";");
	//rewards.append(items.str());

	//size_t nSize = rewards.size();
	//if (rewards[nSize - 1] == ',')
	//{
	//	rewards[nSize - 1] = '\0';
	//}

	std::stringstream rewards;
	for (int i = 0; i < nCount; ++i)
	{
		const char* pszItem = args.StringVal(nIndex++);
		int nNumber = args.IntVal(nIndex++);
		assert(!StringUtil::CharIsNull(pszItem));
		assert(nNumber > 0);

		if ( i > 0 )
		{
			rewards << ",";
		}

		rewards << pszItem << ":" << nNumber;
	}

	return rewards.str();
}

void GmccModule::OnDoGmccProductsPush(IKernel* pKernel, const IVarList& args)
{
	// 消息转发给公共服务器 格式: domain domain_name modify_msg mode data(gmcc发过来的完全转发， 注意gmcc与public表数据完全一致)
	CVarList msg;
	std::wstring wstr = GetDomainName(pKernel);
	msg << PUBSPACE_DOMAIN << wstr.c_str() << SP_DOMAIN_MSG_GMCC_PRODUCT_MONDIFY << EM_GMCC_MONDIFY_PUSH;
	int nCount_ = args.GetCount();
	if ((nCount_ - 3) % 9 != 0)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccProductsPush]gmcc push goods data error.");
		return;
	}

	// *第三位是产品数，依次为: 数量 (id 价格 规则 次数 状态 物品列表 赠送奖励 首购 超过购买次数后的替换物品)...
	msg.Append(args, 2, nCount_ - 2);
	pKernel->SendPublicMessage(msg);
}

void GmccModule::OnDoGamccPaymentPush(IKernel* pKernel, const IVarList& args)
{
	// 消息转发给公共服务器 并通知  玩家领取
	assert(args.GetCount() >= 10 );
	if (args.GetCount() < 10)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGamccPaymentPush]gmcc push payment data param count error.");
		return;
	}

	size_t nIndex_ = 2;
	const char* pszRoleId_ = args.StringVal(nIndex_++);				// nIndex + 0
	const char* pszOrder_ = args.StringVal(nIndex_++);				// nIndex + 1
	const char* pszProductId_ = args.StringVal(nIndex_++);			// nIndex + 2
	float fAmount_ = args.FloatVal(nIndex_++);						// nIndex + 3
	int nState_ = args.IntVal(nIndex_++);							// nIndex + 4
	int nBillType_ = args.IntVal(nIndex_++);						// nIndex + 5
	int64_t nOrderTime_ = args.Int64Val(nIndex_++);					// nIndex + 6
	int64_t nPayedTime_ = args.Int64Val(nIndex_++);					// nIndex + 7

	if (!HandlePayment(pKernel, pszRoleId_, pszOrder_, pszProductId_, fAmount_, nBillType_, nOrderTime_, nPayedTime_))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGamccPaymentPush]gmcc push payment data error.");
	}
}

void GmccModule::OnDoGamccOrderResult(IKernel* pKernel, const IVarList& args)
{
	// 判断是否已支付， 已支付转走成功消息 否则通吞客户端支付
	// 消息转发给公共服务器 并通知  玩家领取
	assert(args.GetCount() >= 9);
	if (args.GetCount() < 9)
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGamccOrderResult]gmcc order result data error.");
		return;
	}

	size_t nIndex_ = 2;
	const char* pszRoleId_ = args.StringVal(nIndex_++);				// nIndex + 0
	const char* pszProductId_ = args.StringVal(nIndex_++);			// nIndex + 1
	const char* pszOrder_ = args.StringVal(nIndex_++);				// nIndex + 2
	int nState_ = args.IntVal(nIndex_++);							// nIndex + 3
	int nBillType_ = args.IntVal(nIndex_++);						// nIndex + 4
	const char* pszExtra_ = args.StringVal(nIndex_++);				// nIndex + 5
	float fPrice_ = args.FloatVal(nIndex_++);						// nIndex + 6

	Assert(!StringUtil::CharIsNull(pszRoleId_));
	//Assert(!StringUtil::CharIsNull(pszOrder_));
	Assert(!StringUtil::CharIsNull(pszProductId_));

	extend_warning(LOG_INFO, "[GmccModule::OnDoGamccOrderResult]push order info[roleid:%s order:%s product_id:%s state:%d type:%d extra:%s]",
									pszRoleId_, pszOrder_, pszProductId_, nState_, nBillType_, pszExtra_);

	EmPayOrderResult eResult = EM_PAY_ORDER_RESULT_SUCEED;
	switch (nState_)
	{
	case EM_ORDER_STATE_PAYED:
		{
			// 支付完成处理
			int64_t tNow_ = util_get_utc_time();
			if (!HandlePayment(pKernel, pszRoleId_, pszOrder_, pszProductId_, fPrice_, nBillType_, tNow_, tNow_))
			{
				::extend_warning(LOG_ERROR, "[GmccModule::OnDoGamccOrderResult] payment error.");
			}
			eResult = EM_PAY_ORDER_RESULT_PAYED;
		}
		break;
	case EM_ORDER_STATE_UNKOWN:
		eResult = EM_PAY_ORDER_RESULT_ORDING;
		break;
	case EM_ORDER_STATE_ARRIVED:
	case EM_ORDER_STATE_TAKE:
	case EM_ORDER_STATE_FINISH:
		eResult = EM_PAY_ORDER_RESULT_PAYED;
		break;
	}
	
	// 下单处理 返回给客户端
	ShopModule::OnOrderInfo(pKernel, pszRoleId_, pszOrder_, pszProductId_, pszExtra_, eResult, fPrice_);
}

void GmccModule::OnDoGmccPropQuery(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 4);
	if (args.GetCount() < 4)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	int64_t nQueryId = args.Int64Val(nIndex++);				// nIndex + 0
	const char* pszRoleId = args.StringVal(nIndex++);		// nIndex + 1
	Assert(!StringUtil::CharIsNull(pszRoleId));
	const wchar_t* pwszRoleName = pKernel->SeekRoleName(pszRoleId);
	if (StringUtil::CharIsNull(pwszRoleName))
	{
		CVarList ret;
		ret << PROTOCOL_ID_ROLE_PROP_DATA << nQueryId << DATA_QUERY_ROLEID_ERR << pszRoleId;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
		return;
	}

	int nSceneId = pKernel->GetPlayerScene(pwszRoleName);
	if (nSceneId <= 0)
	{
		CVarList ret;
		ret << PROTOCOL_ID_ROLE_PROP_DATA << nQueryId << DATA_QUERY_ROLEIN_OFFLINE << pszRoleId;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
		return;
	}

	const PERSISTID player = pKernel->FindPlayer(pwszRoleName);
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL != pPlayer)
	{
		OnPropQuery(pKernel, pPlayer, args, nIndex, nQueryId);
	}
	else
	{
		CVarList cmd;
		cmd << COMMAND_GMCC << GMCC_CMD_EXEC_PROP_QUERY << nQueryId;
		cmd.Append(args, nIndex, args.GetCount() - nIndex);
		pKernel->CommandByName(pwszRoleName, cmd);
	}
}

void GmccModule::OnDoGmccRecordQuery(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 7);
	if (args.GetCount() < 7)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	int64_t nQueryId = args.Int64Val(nIndex++);				// nIndex + 0
	const char* pszRoleId = args.StringVal(nIndex++);		// nIndex + 1
	const char* pszRcordName = args.StringVal(nIndex++);	// nIndex + 2
	int nOffset = args.IntVal(nIndex++);					// nIndex + 3
	int nCount = args.IntVal(nIndex++);						// nIndex + 4
	Assert(!StringUtil::CharIsNull(pszRoleId));
	const wchar_t* pwszRoleName = pKernel->SeekRoleName(pszRoleId);
	if (StringUtil::CharIsNull(pwszRoleName))
	{
		CVarList ret;
		ret << PROTOCOL_ID_ROLE_RECORD_DATA << nQueryId << pszRoleId << pszRcordName << DATA_QUERY_ROLEID_ERR
			<< nOffset << nCount << 0;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
		return;
	}

	int nSceneId = pKernel->GetPlayerScene(pwszRoleName);
	if (nSceneId <= 0)
	{
		CVarList ret;
		ret << PROTOCOL_ID_ROLE_RECORD_DATA << nQueryId << pszRoleId << pszRcordName << DATA_QUERY_ROLEIN_OFFLINE
			<< nOffset << nCount << 0;
		pKernel->SendExtraMessage(ESI_GMCC_SERVER, ret);
		return;
	}

	const PERSISTID player = pKernel->FindPlayer(pwszRoleName);
	IGameObj* pPlayer = pKernel->GetGameObj(player);
	if (NULL != pPlayer)
	{
		OnRecordQuery(pKernel, pPlayer,  pszRcordName, nOffset, nCount, nQueryId);
	}
	else
	{
		CVarList cmd;
		cmd << COMMAND_GMCC << GMCC_CMD_EXEC_RECORD_QUERY << nQueryId << pszRcordName << nOffset << nCount;
		pKernel->CommandByName(pwszRoleName, cmd);
	}
}

//#define TEST_NOTIFY_PUSH
#ifdef TEST_NOTIFY_PUSH
#include "PushDataModule.h"
// 测试通知
#endif

void GmccModule::OnDoGmccInitSrvInfo(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 8);
	if (args.GetCount() < 8)
	{
		// param error
		return;
	}

#ifdef _DEBUG
	SrvInfo _info;
	size_t nIndex = 2;
	_info.Identity = args.Int64Val(nIndex++);				// nIndex + 0
	_info.GameId = args.Int64Val(nIndex++);					// nIndex + 1
	_info.ChannelId = args.Int64Val(nIndex++);				// nIndex + 2
	_info.ProductionId = args.Int64Val(nIndex++);			// nIndex + 3
	_info.ServerId = args.Int64Val(nIndex++);				// nIndex + 4
	_info.AreaId = args.Int64Val(nIndex++);					// nIndex + 5
	_info.DeployId = args.Int64Val(nIndex++);
#endif
	// 修改日志服务信息
	{
		CVarList cmd;
		cmd << COMMAND_GMCC << GMCC_CMD_SYNC_SEV_INFO;
		cmd.Append(args, 2, args.GetCount() - 2);
		BroadcastMemberMessage(pKernel, cmd);
	}

#ifdef TEST_NOTIFY_PUSH
	// 测试通知
	PushDataModule::PushNotification(pKernel, "test title", "this is content");
#endif
}

void GmccModule::OnDoGmccRebateInfo(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 4);
	if (args.GetCount() < 4)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	const char* rid_ = args.StringVal(nIndex++);
	int nRebate_ = args.IntVal(nIndex++);
	int nType_ = args.IntVal(nIndex++);

	const wchar_t* pszName_ = pKernel->SeekRoleName(rid_);
	if (StringUtil::CharIsNull(pszName_))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccRebateInfo] role id not found[rid:%s]", rid_);
		return;
	}

	// 通知玩家处理
	CVarList cmd;
	cmd << COMMAND_GMCC << GMCC_CMD_REBATE_INFO << nType_ << nRebate_;
	pKernel->CommandByName(pszName_, cmd);
}

void GmccModule::OnDoGmccRebateResult(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 5);
	if (args.GetCount() < 5)
	{
		// param error
		return;
	}

	size_t nIndex = 2;
	const char* rid_ = args.StringVal(nIndex++);
	int nResult_ = args.IntVal(nIndex++);
	int nRebate_ = args.IntVal(nIndex++);

	const wchar_t* pszName_ = pKernel->SeekRoleName(rid_);
	if (StringUtil::CharIsNull(pszName_))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccRebateResult] role id not found[rid:%s]", rid_);
		return;
	}

	// 通知玩家处理
	CVarList cmd;
	cmd << COMMAND_GMCC << GMCC_CMD_REBATE_RESULT << nResult_ << nRebate_;
	pKernel->CommandByName(pszName_, cmd);
}

void GmccModule::OnDoGmccPushShopItems(IKernel* pKernel, const IVarList& args)
{
	extend_warning(LOG_INFO, "[GmccModule::OnDoGmccPushShopItems] args:%d", args.GetCount() - 1);
	ShopModule::AddPlusShopItem(pKernel, args, 2);
}

void GmccModule::OnDoGmccDelShopItems(IKernel* pKernel, const IVarList& args)
{
	extend_warning(LOG_INFO, "[GmccModule::OnDoGmccDelShopItems] args:%d", args.GetCount() - 1);
	ShopModule::RemovePlusShopItem(pKernel, args, 2);
}

void GmccModule::OnDoGmccQueryShopItems(IKernel* pKernel, const IVarList& args)
{
	CVarList msg;
	msg << PROTOCOL_ID_SHOP_ITEM_LIST << 0; // 第2位为物品数
	
	int nCount = ShopModule::GetPlusShopList(pKernel, msg);
	// 设置为实际的数量
	msg.SetInt(1, nCount);
	if (!pKernel->SendExtraMessage(ESI_GMCC_SERVER, msg))
	{
		extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccQueryShopItems] return plus shop list failed.");
	}
}

void GmccModule::OnDoGmccLoadConfig(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 4);
	if (args.GetCount() < 4)
	{
		extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccSetGenActivityConfig]args count error");
		// param error
		return;
	}

	size_t _nIndex = 2;
	int64_t nSeq = args.Int64Val(_nIndex++);
	int32_t nCount = args.IntVal(_nIndex++);

	for (int32_t i = 0; i < nCount; ++i )
	{
		const char* pszKey = args.StringVal(_nIndex++);
		// 跳过path,catalog,cfgid
		_nIndex += 3;
		extend_warning(LOG_INFO, "[GmccModule::OnDoGmccSetGenActivityConfig]reload config '%s'", pszKey);
		ReLoadConfigModule::m_pReLoadConfigModule->SendReloadConfigCommand(pKernel, pszKey);
	}
	
	ConfirmGmccAck(pKernel, nSeq);
}

void GmccModule::OnDoGmccSetGenActivityConfig(IKernel* pKernel, const IVarList& args)
{
	assert(args.GetCount() >= 3);
	if (args.GetCount() < 3)
	{
		// param error
		return;
	}

	size_t _nIndex = 2;

	const wchar_t* _pszName = args.WideStrVal(_nIndex++);
	const wchar_t* _pszDesc = args.WideStrVal(_nIndex++);
	int _nType = args.IntVal(_nIndex++);
	int64_t _startTime = args.Int64Val(_nIndex++);
	int64_t _endTime = args.Int64Val(_nIndex++);

	// 验证数据长度 ActivityItem(gmcc\ShopItem.h)
	Assert((args.GetCount() - _nIndex - 1) % 4 == 0);
	if (_nType == EM_ACT_TYPE_PLAYER_LEVEL_MATCH)
	{
		// 设置冲级配置
		ShopModule::SetPlushActivityConfig(pKernel, (EmPlusActivityType)_nType, _pszName, _pszDesc, _startTime, _endTime, args, _nIndex);
	}
	else
	{
		Assert(false);
		extend_warning(LOG_ERROR, "[GmccModule::OnDoGmccSetGenActivityConfig]don't support config type:%d.", _nType);
	}
}

void GmccModule::OnClientExchGift(IKernel* pKernel, const PERSISTID& player, const char* exchKey)
{
	Assert(NULL != exchKey);
	const char* pAccId = pKernel->QueryString(player, FIELD_PROP_ACCOUNT);
	const char* pRoleId = pKernel->QueryString(player, FIELD_PROP_UID);
	if (StringUtil::CharIsNull(pAccId) || StringUtil::CharIsNull(pRoleId))
	{
		extend_warning(LOG_ERROR, "[GmccModule::OnClientExchGift]accid or roleid is null.");
		return;
	}

	do 
	{
		// 判断key是否合法(季爷描述: 字母数字组合 区分大小写 ~~ 最长 32 )
		if ( strlen(exchKey) > 32 )
		{
			break;
		}

		if ( !StringUtil::StringIsAlnum(exchKey))
		{
			break;
		}

		CVarList args;
		args << PROTOCOL_ID_GIFT_EXCH << exchKey << pAccId << pRoleId;
		if (!pKernel->SendExtraMessage(ESI_GMCC_SERVER, args))
		{
			CVarList param;
			::CustomSysInfo(pKernel, player, TIPSTYPE_NORMAL_CHAT_MESSAGE, TIP_EXCH_GIFT_UNKOWN, param);
		}

		return;
	} while (false);
	
	extend_warning(LOG_TRACE, "[GmccModule::OnClientExchGift]key invalid:%s.", exchKey);
	CVarList msgList;
	msgList << SERVER_CUSTOMMSG_SYSINFO << TIPSTYPE_SHOW_WINDOW_MESSAGE << TIP_EXCH_GIFT_INVALID;
	pKernel->Custom(player, msgList);
}

void GmccModule::OnClientPickPayedRebate(IKernel* pKernel, const PERSISTID& player)
{
	// 是否领取过了
	bool bPicked_ = PlayerBaseModule::TestPlayerGlobalFlag( pKernel, player, EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED);
	if (bPicked_)
	{
		// 错误消息 领取过了什么也不处理
		return;
	}

	IGameObj* pPlayer_ = pKernel->GetGameObj(player);
	if (NULL == pPlayer_)
	{
		return;
	}	
	
	// 根据返利字段返返回元宝
	int nSliver_ = pPlayer_->QueryInt(FIELD_PROP_PAYED_REBATE);
	if (nSliver_ <= 0)
	{
		// 错误消息
		::extend_warning(LOG_WARNING, "[GmccModule::OnClientPickPayedRebate] rebate lower zero[uid:%s]", 
							pPlayer_->QueryString(FIELD_PROP_UID));
		return;
	}

	// 通知计费领取
	const char* pszRoleId_ = pPlayer_->QueryString(FIELD_PROP_UID);
	const char* pszUid_ = pPlayer_->QueryString(FIELD_PROP_ACCOUNT);
	const wchar_t* pwszName_ = pPlayer_->QueryWideStr(FIELD_PROP_NAME);
	// 发消息到web查询返利
	CVarList args;
	args << PROTOCOL_ID_REBATE_PICK
		<< pszUid_
		<< pszRoleId_
		<< pwszName_;

	pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);
}

void GmccModule::OnGameReady(IKernel* pKernel)
{
	int member_id = pKernel->GetMemberId();
	if (member_id != 0)
	{
		//by kevin 按照原意，只通知master，是否要开放，还需相关人员解释，并开放。 2017-08-07 20:54
		return;
	}
	CVarList args;
	args << PROTOCOL_ID_GAME_READY;
	pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);

	// 启动请求服务信息心跳
	if (!ms_bRecivedSrvInfo)
	{
		if (!pKernel->FindHeartBeat(pKernel->GetScene(), "GmccModule::HB_RequestSrvInfo"))
		{
			// 1分钟
			pKernel->AddHeartBeat(pKernel->GetScene(), "GmccModule::HB_RequestSrvInfo", 60000);
		}
	}
}

void GmccModule::ConfirmGmccAck(IKernel* pKernel, int64_t nSeq)
{
	CVarList args;
	args << PROTOCOL_ID_CONFIRM_ACK << nSeq;
	pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);
}

int GmccModule::HandleCommand(IKernel* pKernel, const PERSISTID& player, 
					const char* cmd, const IVarList& args, int offset, std::string& outMsg)
{
	int nResult = EM_GMCC_RESULT_SUCCEED;
	outMsg = "{\"content\":\"success\"}";
	if (strncmp(cmd, GMCC_GM_COMMAND_PRI, strlen(GMCC_GM_COMMAND_PRI)) == 0)
	{
		// 去除gm前缀
		if (!m_pGmModule->RunGmCommand(pKernel, player, cmd + strlen(GMCC_GM_COMMAND_PRI), args, offset, args.GetCount() - offset, outMsg))
		{
			nResult = EM_GMCC_RESULT_ERROR;
		}
	}
	else
	{
		if (strcmp(cmd, GMCC_EXEC_FORBID_SPEAK) == 0)
		{
			if (!m_pGmccModule->OnForbidSpeak(pKernel, player, args, offset))
			{
				outMsg = "{\"error\":\"execute failed\"}";
				nResult = EM_GMCC_RESULT_ERROR;
			}
		}
		else if (strcmp(cmd, GMCC_EXEC_ALLOW_SPEAK) == 0)
		{
			if (!m_pGmccModule->OnAllowSpeak(pKernel, player, args, offset))
			{
				outMsg = "{\"error\":\"execute failed\"}";
				nResult = EM_GMCC_RESULT_ERROR;
			}
		}
		else if (strcmp(cmd, GMCC_EXEC_GRANT_AUTH) == 0)
		{
			if (!m_pGmccModule->OnGrantAuth(pKernel, player, args, offset))
			{
				outMsg = "{\"error\":\"execute failed\"}";
				nResult = EM_GMCC_RESULT_ERROR;
			}
		}
		else if (strcmp(cmd, GMCC_EXEC_RELOAD_LUA) == 0)
		{
			if ( !pKernel->LuaLoadScript(args.StringVal(offset)) )
			{
				outMsg = "{\"error\":\"execute failed\"}";
				nResult = EM_GMCC_RESULT_ERROR;
			}
		}
		else
		{
			// 命令错误
			outMsg = "{\"error\":\"command not found\"}";
			nResult = EM_GMCC_RESULT_ERROR;
		}
	}

	return nResult;
}

bool GmccModule::IsCommandRule(IKernel* pKernel, const char* cmd, const char* rule)
{
	if (LuaScriptModule::m_pLuaScriptModule)
	{
		if (strncmp(cmd, GMCC_GM_COMMAND_PRI, strlen(GMCC_GM_COMMAND_PRI)) == 0)
		{
			// 去除gm前缀
			cmd = cmd + strlen(GMCC_GM_COMMAND_PRI);
		}

		CVarList results;
		CVarList luaargs;
		luaargs << cmd;
		LuaScriptModule::m_pLuaScriptModule->RunLuaScript(pKernel, "gm_x_rule.lua", rule, luaargs, 1, &results);
		return strcmp(results.StringVal(0), "ok") == 0;
	}

	return false;
}

const std::wstring & GmccModule::GetDomainName(IKernel * pKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];

		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"DomainGmcc_%d", server_id);
		m_domainName = wstr;
	}

	return m_domainName;
}

IPubData* GmccModule::GetPubData(IKernel* pKernel)
{
	IPubSpace* pPubSpace_ = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace_)
	{
		return NULL;
	}
	IPubData* pPubData_ = pPubSpace_->GetPubData(GetDomainName(pKernel).c_str());
	return pPubData_;
}

IRecord* GmccModule::GetPubRecord(IKernel* pKernel, const char* pszRecName)
{
	IPubData * pPubData_ = GetPubData(pKernel);
	if (pPubData_ == NULL)
	{
		return NULL;
	}

	IRecord* pRecord_ = pPubData_->GetRecord(pszRecName);
	return pRecord_;
}

void GmccModule::LoadPropsValue(IGameObj* pObject, IVarList& values, const IVarList& keys, size_t nOffset)
{
	Assert(NULL != pObject);
	size_t nCount = keys.GetCount();
	while (nOffset < nCount)
	{
		const char* pszKey = keys.StringVal(nOffset++);
		if (StringUtil::CharIsNull(pszKey))
		{
			continue;
		}

		int nType = pObject->GetAttrType(pszKey);
		switch ( nType )
		{
		case VTYPE_BOOL:
			values.AddInt(pObject->QueryInt(pszKey));
			break;
		case VTYPE_INT:
			values.AddInt(pObject->QueryInt(pszKey));
			break;
		case VTYPE_INT64:
			values.AddInt64(pObject->QueryInt64(pszKey));
			break;
		case VTYPE_FLOAT:
			values.AddFloat(pObject->QueryFloat(pszKey));
			break;
		case VTYPE_DOUBLE:
			values.AddDouble(pObject->QueryDouble(pszKey));
			break;
		case VTYPE_STRING:
			values.AddString(pObject->QueryString(pszKey));
			break;
		case VTYPE_WIDESTR:
			values.AddWideStr(pObject->QueryWideStr(pszKey));
			break;
		case VTYPE_OBJECT:
			values.AddInt64(pObject->QueryObject(pszKey).nData64);
			break;
		default:
			break;
		}
	}
}

void GmccModule::LoadRecordData(IGameObj* pObject, IRecord* pRec, int nOffset, int nCount, IVarList& retData)
{
	Assert(NULL != pObject);
	Assert(NULL != pRec);
	int nEnd = nOffset + nCount;
	if (nEnd > pRec->GetRows())
	{
		nEnd = pRec->GetRows();
	}
	int nCols = pRec->GetCols();
	LoopBeginCheck(a);
	for ( int i = nOffset; i < nEnd; ++i )
	{
		LoopDoCheck(a);

		LoopBeginCheck(b);
		for (int j = 0; j < nCols; ++j)
		{
			LoopDoCheck(b);
			int nType = pRec->GetColType(j);
			switch (nType)
			{
			case VTYPE_BOOL:
				retData.AddInt(pRec->QueryInt(i, j));
				break;
			case VTYPE_INT:
				retData.AddInt(pRec->QueryInt(i, j));
				break;
			case VTYPE_INT64:
				retData.AddInt64(pRec->QueryInt64(i, j));
				break;
			case VTYPE_FLOAT:
				retData.AddFloat(pRec->QueryFloat(i, j));
				break;
			case VTYPE_DOUBLE:
				retData.AddDouble(pRec->QueryDouble(i, j));
				break;
			case VTYPE_STRING:
				retData.AddString(pRec->QueryString(i, j));
				break;
			case VTYPE_WIDESTR:
				retData.AddWideStr(pRec->QueryWideStr(i, j));
				break;
			case VTYPE_OBJECT:
				retData.AddInt64(pRec->QueryObject(i, j).nData64);
				break;
			default:
				Assert(false);
				retData.AddInt(pRec->QueryInt(i, j));
				break;
			}
		}
	}
}

int GmccModule::BroadcastMemberMessage(IKernel* pKernel, const IVarList& args)
{
	// 公共空间
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return 0;
	}

	// 公共数据
	wchar_t pubDataName[256];
	SWPRINTF_S(pubDataName, L"Domain_SceneList%d", pKernel->GetServerId());
	IPubData* pPubData = pPubSpace->GetPubData(pubDataName);
	if (pPubData == NULL)
	{
		return 0;
	}

	// 上架表
	IRecord* pRecord = pPubData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
	if (pRecord == NULL)
	{
		return 0;
	}
	int rows = pRecord->GetRows();
	LoopBeginCheck(a);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(a);
		pKernel->CommandToScene(pRecord->QueryInt(i, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id), args);
	}

	return rows;
}

bool GmccModule::OnClientOrder(IKernel * pKernel, const char* pszRoleId, const wchar_t* pwszName, 
								const char* pszProductId, const char* pszToken)
{
	Assert(!StringUtil::CharIsNull(pszRoleId));
	Assert(!StringUtil::CharIsNull(pszProductId));
	Assert(!StringUtil::CharIsNull(pszToken));

	if (StringUtil::CharIsNull(pszRoleId) || StringUtil::CharIsNull(pszProductId) || StringUtil::CharIsNull(pszToken))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnClientOrder] param is emty");
		return false;
	}

	// 客户端下单处理
	CVarList args;
	args << PROTOCOL_ID_ORDER_REQ
		<< pszRoleId
		<< pwszName
		<< pszToken
		<< pszProductId;

	return pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);
}

bool GmccModule::OnOrderStateChanged(IKernel* pKernel, const char* pszRoleId, const char* pszOrder, int nState)
{
	Assert(!StringUtil::CharIsNull(pszRoleId));
	Assert(!StringUtil::CharIsNull(pszOrder));

	if (StringUtil::CharIsNull(pszRoleId) || StringUtil::CharIsNull(pszOrder))
	{
		::extend_warning(LOG_ERROR, "[GmccModule::OnOrderStateChanged] param is emty");
		return false;
	}

	// 通知扩展服务器订单状态改变
	CVarList args;
	args << PROTOCOL_ID_ORDER_NOTIFY
		<< pszRoleId
		<< pszOrder
		<< nState;

	return pKernel->SendExtraMessage(ESI_GMCC_SERVER, args);
}
