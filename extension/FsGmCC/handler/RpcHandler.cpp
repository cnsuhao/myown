#include "RpcHandler.h"
#include "MailData.h"
#include "CmdInfo.h"
#include "NoticeInfo.h"
#include "utils/string_util.h"
#include "PayedOrder.h"
#include "ProtocolsID.h"


void RpcHandler::getAreaInfo(::AreaInfo& _return)
{
	try
	{
		if (NULL != m_pGameInfo)
		{
			_return.areaId = m_pGameInfo->GetAreaId();
			_return.currentCnt = m_pGameInfo->GetOnlineCount();
			_return.name = m_pGameInfo->GetName();

			std::vector<int> vecMemberIds;
			int nCount = m_pGameInfo->GetMemberIds(vecMemberIds);
			for (std::vector<int>::iterator itr = vecMemberIds.begin(); itr != vecMemberIds.end(); ++itr)
			{
				GameServerInfo serverInfo;

				serverInfo.serverId = *itr;
				serverInfo.currentCnt = 0;
				serverInfo.name = m_pGameInfo->GetMemberName(*itr);
				serverInfo.status = (Status::type)m_pGameInfo->GetMemberState(*itr);
				EmMemberWorkState state = m_pGameInfo->GetMemberWorkState(*itr);
				if (state == EM_MEMBER_WORK_STATE_FULL)
				{
					state = EM_MEMBER_WORK_STATE_BUSY;
				}
				serverInfo.operationStatus = (OperationStatus::type)state;
				_return.serverList.push_back(serverInfo);
			}
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}

bool RpcHandler::closeServer(const int32_t serviceId)
{
	return false;
}

bool RpcHandler::modifyGameStatus(const ::GameStatus::type status)
{
	return false;
}

bool RpcHandler::modifyGameStep(const ::GameStep::type step)
{
	return false;
}

bool RpcHandler::addMail(const ::MailInfo& mailInfo)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			// 如果是通知则为支付信息
			if (mailInfo.type == MailType::NOTICE)
			{
				// TODO: ERROR
				printf("[%s %s] add mail type error!!!", __FILE__, __FUNCTION__);
				return false;
			}
			else
			{
				MailData data;
				data.MailId = mailInfo.id;
				data.MailType = mailInfo.type;
				data.CreateTime = mailInfo.createTime / 1000;		// 毫秒转秒
				data.Status = 0;
				data.Period = 0;
				data.Title = mailInfo.title;
				data.Content = mailInfo.content;
				data.ReciverIds = mailInfo.recieverId;
				data.senderId = mailInfo.senderId;
				data.HasAttach = mailInfo.isAttach;
				data.AttachInfo = mailInfo.attachInfo;
				data.GlobalMail = data.ReciverIds.size() == 0;

				m_pRpcServer->OnRpcMail(data);
			}
			return true;
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
	return false;
}

void RpcHandler::queryRoleInfo(::RoleInfo& _return, const int64_t roleId)
{
	
}

bool RpcHandler::sendCommand(const ::CommandInfo& commandInfo)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			CmdInfo cmd;
			cmd.roleId = commandInfo.roleId;
			cmd.command = commandInfo.command;
			cmd.operateRoleId = commandInfo.operateRoleId;
			cmd.cmdArgs = commandInfo.cmdArgs;
			cmd.seq = commandInfo.cmdId;

			m_pRpcServer->OnRpcCommand(cmd);
			return true;
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
	return false;
}

void RpcHandler::syncNoticeStatus(::HandleResult& _return, const int64_t noticeId)
{
	
}

bool RpcHandler::sendSysNotice(const ::SystemNotice& notice)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			NotifyInfo info;
			info.NotifyType = notice.resourceType;
			info.OpType = notice.operateType;
			info.AccId = StringUtil::Int64AsString(notice.accId);
			info.RoleId = notice.roleId;
			info.Content = notice.content;
			m_pRpcServer->OnRpcNotify(info);
			return true;
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
	return false;
}

void RpcHandler::notifyOrder(const ::Order& order)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			Payment payment_;
			payment_.RoleId = order.roleId;
			payment_.OrderId = order.orderId;
			payment_.ProductId = order.goodsCode;
			payment_.Amount = (float)order.price;
			payment_.State = EnumIndex<EmOrderState>( order.state.c_str() );
			payment_.PayType = EnumIndex<EmBillType>(order.type.c_str());
			payment_.OrderTime = order.createAt / 1000;			// 转成s
			payment_.PayedTime = order.payAt / 1000;			// 转成s

			m_pRpcServer->OnRpcPayment(payment_);
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}

void RpcHandler::notifyGoods()
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			m_pRpcServer->OnRpcGoods();
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}

void RpcHandler::notifyConfig(const ::ConfigType::type type, const int64_t id)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			m_pRpcServer->OnRpcConfig((int)type, id );
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}

void RpcHandler::notifyShop(const std::string& id)
{
	try
	{
		if (NULL != m_pRpcServer)
		{
			m_pRpcServer->OnRpcMallItems();
		}
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}


void RpcHandler::OnPropData(const CmdPropData& data)
{
	try
	{
		m_quePropAck.EnQueue(data);
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}

void RpcHandler::OnRecordData(const CmdRecordData& data)
{
	try
	{
		m_queRecordAck.EnQueue(data);
	}
	catch (...)
	{
		printf("[%s %s] exception!!!", __FILE__, __FUNCTION__);
	}
}