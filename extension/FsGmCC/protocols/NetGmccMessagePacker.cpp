//--------------------------------------------------------------------
// 文件名:		NetGmccMessagePacker.h
// 内  容:		
// 说  明:		
// 创建日期:	2016年9月17日
// 创建人:		李海罗
// 版权所有:	苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#include "NetGmccMessagePacker.h"
#include "ProtocolsID.h"
#include "NetWorldGmcc.h"
#include "NetWorldPush.h"
#include "NetWorldActivity.h"
#include "NetWorkPayed.h"

INetVarListActionProtocol* NetGmccMessagePacker::CreateAction(int nKey) const
{
	INetVarListActionProtocol* pAction = NULL;

	switch (nKey)
	{
	case PROTOCOL_ID_GIFT_EXCH:
		pAction = new NetExtraAction_Exch_Gift();
		break;
	case PROTOCOL_ID_NOTICE_SYNC:
		pAction = new NetExtraAction_Notice_Sync();
		break;
	case  PROTOCOL_ID_ROLE_PROP_DATA:
		pAction = new NetExtraAction_Role_Prop_Data();
		break;
	case  PROTOCOL_ID_ROLE_RECORD_DATA:
		pAction = new NetExtraAction_Role_Record_Data();
		break;
	case PROTOCOL_ID_PUSH_NOTIFICATION:
		pAction = new NetExtraAction_PushNotification();
		break;
	case PROTOCOL_ID_GAME_READY:
		pAction = new NetExtraAction_GameReady();
		break;
	case PROTOCOL_ID_GM_REPLY:
		pAction = new NetExtraAction_GM_Reply();
		break;
	case PROTOCOL_ID_MAIL_RESULT:
		pAction = new NetExtraAction_Mail_Result();
		break;
	case PROTOCOL_ID_NOTICE_RESULT:
		pAction = new NetExtraAction_Notice_Result();
		break;
	case PROTOCOL_ID_SHOP_ITEM_LIST:
		pAction = new NetExtraAction_ShopItemList();
		break;
	case PROTOCOL_ID_REPORT_MEMBER_INFO:
		pAction = new NetExtraAction_ReportOnlineCount();
		break;
	case PROTOCOL_ID_GAME_REQ_SRV_INFO:
		pAction = new NetExtraAction_GameReqSrvInfo();
		break;
	case PROTOCOL_ID_CONFIRM_ACK:
		pAction = new NetExtraAction_GameAck();
		break;
	case PROTOCOL_ID_ORDER_REQ:
		pAction = new NetExtraAction_Product_Order();
		break;
	case PROTOCOL_ID_ORDER_NOTIFY:
		pAction = new NetExtraAction_Product_Order_Notify();
		break;
	case PROTOCOL_ID_REBATE_QUERY:
		pAction = new NetExtraAction_PayedRebateQuery();
		break;
	case PROTOCOL_ID_REBATE_PICK:
		pAction = new NetExtraAction_PayedRebatePick();
		break;
	default:
		break;
	}

	return pAction;
}