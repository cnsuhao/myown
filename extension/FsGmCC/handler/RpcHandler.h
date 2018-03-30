//--------------------------------------------------------------------
// 文件名:		RpcHandler.h
// 内  容:		Rpc回调处理
// 说  明:		
// 创建日期:		2016年9月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __RPC_HANDLER_H__
#define __RPC_HANDLER_H__

#include "gen-cpp/GMCCServer.h"
#include "CmdInfo.h"
#include "NoticeInfo.h"
#include "MailData.h"
#include "PlayerData.h"
#include "utils/FsLockedQueue.h"
#include "GameInfo.h"
#include "PayedOrder.h"

class IRpcServer
{
public:
	virtual ~IRpcServer() = 0;
	virtual void OnRpcCommand(const CmdInfo& cmd) = 0;
	virtual void OnRpcNotify(const NotifyInfo& notify) = 0;
	virtual void OnRpcMail(const MailData& data) = 0;
	virtual void OnRpcPropQuery(const CmdPropQuery& query) = 0;
	virtual void OnRpcRecordQuery(const CmdRecordQuery& query) = 0;
	virtual void OnRpcPayment(const Payment& payment) = 0;
	virtual void OnRpcGoods() = 0;
	virtual void OnRpcMallItems() = 0;
	virtual void OnRpcConfig( int nType, int nConfigId ) = 0;
};

inline IRpcServer::~IRpcServer(){}

class RpcHandler : public GMCCServerIf
{
	IRpcServer* m_pRpcServer;
	GameInfo*	m_pGameInfo;
public:
	RpcHandler() : m_pRpcServer(NULL), m_pGameInfo(NULL)
	{}
	void SetRpcServer(IRpcServer* pServer)
	{
		m_pRpcServer = pServer;
	}
	void SetGameInfo(GameInfo* pGameInfo)
	{
		m_pGameInfo = pGameInfo;
	}

	virtual void getAreaInfo(::AreaInfo& _return) override;

	virtual bool closeServer(const int32_t serviceId);

	virtual bool modifyGameStatus(const ::GameStatus::type status);

	virtual bool modifyGameStep(const ::GameStep::type step);

	virtual bool addMail(const ::MailInfo& mailInfo);

	virtual void queryRoleInfo(::RoleInfo& _return, const int64_t roleId);

	virtual bool sendCommand(const ::CommandInfo& commandInfo);

	virtual void syncNoticeStatus(::HandleResult& _return, const int64_t noticeId);

	virtual bool sendSysNotice(const ::SystemNotice& systemMessage);

	virtual void notifyOrder(const  ::Order& order);

	virtual void notifyGoods();

	virtual void notifyConfig(const ::ConfigType::type type, const int64_t id) override;
	virtual void notifyShop(const std::string& id) override;

	//----------------------------------------------------------------
	// 返回数据
	//----------------------------------------------------------------
	void OnPropData(const CmdPropData& data);
	void OnRecordData(const CmdRecordData& data);

protected:
	FsLockedQueue<CmdPropData>		m_quePropAck;
	FsLockedQueue<CmdRecordData>	m_queRecordAck;
};

#endif 