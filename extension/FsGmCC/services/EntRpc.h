//--------------------------------------------------------------------
// 文件名:		EntRpc.h
// 内  容:		RPC服務實現
// 说  明:		
// 创建日期:		2016年9月14日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __ENT_RPC_SERVICE_H__
#define __ENT_RPC_SERVICE_H__

#include <list>
#include "EntNetBase.h"
#include "NetTextAction.h"
#include "CmdInfo.h"
#include "NoticeInfo.h"
#include "MailData.h"
#include "RpcServer.h"
#include "FsThreadQueue.h"
#include "RpcHandler.h"
#include "PlayerData.h"
#include "GameInfo.h"
#include "FsDelayQueue.h"

class SoloGmcc;
class EntHttp;
class EntRpc : public EntNetBase, public IRpcServer
{
public:
	EntRpc();
	~EntRpc();
	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;

	// 启动服务
	bool Startup();
	// 停止服务
	bool Stop();
	virtual void Execute(float seconds);
	virtual int GetNetKey() const;

	GameInfo& GetGameInfo();
	const GameInfo& GetGameInfo() const;

	// 启动rpc服务
	bool StartRpc();
	bool StopRpc();

public:
	void OnRpcCommand( const CmdInfo& cmd );
	void OnRpcNotify(const NotifyInfo& notify);
	void OnRpcMail(const MailData& data);
	void OnRpcPropQuery(const CmdPropQuery& query);
	void OnRpcRecordQuery(const CmdRecordQuery& query);

	void OnRpcPayment(const Payment& payment);
	void OnRpcGoods();

	virtual void OnRpcMallItems() override;
	virtual void OnRpcConfig(int nType, int nConfigId) override;


	void OnPropData(const CmdPropData& data);
	void OnRecordData(const CmdRecordData& data);

	// 设置心跳
	void OnSetServerHeartbeat(int nHeatBeat);
	// 设置游戏名
	void OnSetGameName(const char* pszName);
	// 游戏区id
	void OnSetGameAreaId(long long nAreaId);
	// 上报服务器状态
	void OnReportState(EmMemberState state);
	// 上报在线人数
	void OnReportOnline( int nCount );
	// 上报服务器成员信息
	void OnReportMemberInfo(const MemberInfo& info);
	// 上报服务器断开信息
	void OnReportDeadAll();
private:
	static void RpcThread(void* p);
protected:
	// gmcc 主接口
	SoloGmcc* m_pGmcc;
	// gmcc http接口
	EntHttp* m_pHttp;
	// rpc服务
	RpcServer						m_rpcServer;
	boost::shared_ptr<RpcHandler>	m_rpcHandler;
	// 主线程队列
	//FsThreadQueue					m_mainQueue;
	FsDelayQueue					m_mainQueue;

	// 游戏服务器数据
	GameInfo						m_gameInfo;

	CThread*						m_pThread;
};

#endif // END __ENT_RPC_SERVICE_H__