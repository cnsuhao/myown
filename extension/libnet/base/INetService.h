//--------------------------------------------------------------------
// 文件名:		INetService.h
// 内  容:		网络服务接口
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __INET_SERVICE_HANDLER_H__
#define __INET_SERVICE_HANDLER_H__
#include "INetConnection.h"
#include <functional>
#include <memory>

class INetIoDesc
{
public:
	virtual ~INetIoDesc() = 0;
	virtual int GetIoKey() const = 0;

	virtual size_t GetReadBufSize() const = 0;
	virtual size_t GetSendBufSize() const = 0;

	virtual size_t GetMaxAccept() const = 0;

	virtual const char*		ListenIP() const = 0;
	virtual unsigned short	ListenPort() const = 0;


	virtual bool IsSSL() const = 0;
	virtual void SetSSL(bool bSSL) = 0;
};

inline INetIoDesc::~INetIoDesc(){}


enum EMNetErrorDealWith
{
	EM_NET_ERRROR_DEAL_WITH_BREAK,		// 中断网络
	EM_NET_ERRROR_DEAL_WITH_CONTINUE,	// 丢弃当前缓冲区数据 继续
};

// INetIoHandler由NetService线程调用
class INetIoHandler
{
public:
	virtual ~INetIoHandler() = 0;
	virtual void OnConnect(INetConnection* conn) = 0;
	virtual void OnClose( INetConnection* conn, EMNetReason reason, int nErrCode ) = 0;

	virtual bool OnAction(INetConnection* conn, std::shared_ptr<INetAction>& action) = 0;
	virtual bool OnUnRegistAction(INetConnection* conn, std::shared_ptr<INetAction>& action) = 0;
	virtual bool OnReply(INetConnection* conn, std::shared_ptr<INetAction>& action) = 0;
	virtual bool OnTimeout(INetConnection* conn, std::shared_ptr<INetAction>& action) = 0;

	virtual void OnRawMessage(INetConnection* conn, const char* szMsg, int nLen) = 0;
	virtual EMNetErrorDealWith OnErrorMessage(INetConnection* conn, const char* szMsg, int nLen) = 0;
};
inline INetIoHandler::~INetIoHandler(){}

class INetFilter
{
public:
	virtual ~INetFilter() = 0;
	virtual bool AllowConnect(const char* szRemoteIp, int nRemotePort) = 0;
};
inline INetFilter::~INetFilter(){}

class INetService
{
public:
	virtual ~INetService() = 0;
	virtual int GetKey() const = 0;
	virtual const INetIoDesc* GetDesc() const = 0;
	virtual INetIoHandler* GetHandler() = 0;
	virtual INetActionPacker* GetPacker() = 0;
	virtual INetFilter* GetFilter() = 0;
	virtual INetConnectionFactory* GetFactory() = 0;

	virtual bool Listen() = 0;
	virtual bool IsExists(const NID& id) const = 0;
	virtual size_t GetConnCount() const = 0;
	virtual void Close(const NID& id) = 0;

	virtual bool Connect(const char* ip, unsigned short port, int nTimout, int ud ) = 0;
	virtual bool Send(const NID& id, const char* szMsg, size_t nLen) = 0;
	virtual bool Send(const NID& id, std::shared_ptr<INetAction>& action) = 0;

	virtual bool Startup() = 0;
	virtual bool Stop( int nWaitMillsec ) = 0;
	virtual bool IsRunning() const = 0;

	virtual void Run() = 0;
	virtual void RunOnce() = 0;
	virtual void Post(std::function<void()> p) = 0;
};
inline INetService::~INetService(){}

class INetServiceCreator
{
public:
	virtual ~INetServiceCreator() = 0;
	virtual INetService* Create(INetIoDesc* pDesc,
					INetIoHandler* pHandler,
					INetConnectionFactory* pFactory,
					INetActionPacker* packer,
					INetFilter* pFilter) = 0;
	virtual void Free(INetService* service) = 0;
};
inline INetServiceCreator::~INetServiceCreator(){}

class INetManager
{
public:
	virtual ~INetManager() = 0;

	virtual bool AddCreator(int nKey, INetServiceCreator* creator) = 0;
	virtual bool RemoveCreator(int nKey) = 0;
	virtual bool SetDefaultCreator(INetServiceCreator* creator) = 0;
	virtual INetServiceCreator* LookupCreator(int nKey) = 0;

	virtual bool StartServer(INetIoDesc* pDesc,
							INetIoHandler* pHandler,
							INetConnectionFactory* pFactory,
							INetActionPacker* packer = NULL,
							INetFilter* pFilter = NULL
							) = 0;
	virtual bool StopServer(int nKey) = 0;
	virtual void RemoveServer(int nKey) = 0;

	virtual INetService* Lookup(int nKey) = 0;

	virtual bool IsRunning(int nKey) = 0;
};
inline INetManager::~INetManager(){}
#endif // END __INET_SERVICE_HANDLER_H__