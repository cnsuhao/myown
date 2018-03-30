//--------------------------------------------------------------------
// 文件名:		RpcServer.h
// 内  容:		Rpc服务
// 说  明:		
// 创建日期:		2016年9月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __RPC_SERVER_H__
#define __RPC_SERVER_H__
#include "thrift/server/TThreadPoolServer.h"
#include "RpcHandler.h"
#include <mutex>
#include <condition_variable>

class RpcServer
{
	apache::thrift::server::TThreadPoolServer* m_pServer;
	std::atomic_bool m_bExitThread;
	bool m_bRunning;
public:
	RpcServer();
	void Start(const boost::shared_ptr<RpcHandler>& handler, int nPort);
	void Stop();
	void Run();
	bool IsRunning();
};

#endif 