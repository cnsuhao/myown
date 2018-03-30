//--------------------------------------------------------------------
// 文件名:		INetConnection.h
// 内  容:		网络连接对象定义
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
// 修改日期：		2016年06月07日
// 修改描述：		规范连接会话接口只处理二进制数据 所有数据操作都应该异步post到服务线程处理, 不能直接处理内部句柄
//--------------------------------------------------------------------
#ifndef __INET_CONNECTION_H__
#define __INET_CONNECTION_H__

#include "INetType.h"
#include "action/INetAction.h"

enum EMNetReason
{
	EM_NET_REASON_SELF_DISCONNECT,			// 自己主动断开
	EM_NET_REASON_PEER_DISCONNECT,			// 对方主动断开
	EM_NET_REASON_SOCKET_ERRROR,			// 发生错误断开
	EM_NET_REASON_SND_BUFFER_NO_ENGTH,		// 发送缓冲区过小
	EM_NET_REASON_RCV_BUFFER_NO_ENGTH,		// 接收缓冲区过小
	EM_NET_REASON_PARAM_ERRROR,				// 服务器参数错误
	EM_NET_REASON_LOGIC_ERRROR,				// 逻辑错误
	EM_NET_REASON_CONNECT_FAILED,			// 连接失败
};

enum EMConnState
{
	EM_CONN_STATE_NOT_CONNECTED,			// 未连接
	EM_CONN_STATE_CONNECTING,				// 连接中
	EM_CONN_STATE_CONNECTED,				// 连接成功
	EM_CONN_STATE_CLOSING,					// 正在关闭中
	EM_CONN_STATE_CLOSED,					// 已关闭
};

class INetService;
class INetConnection
{
public:
	virtual ~INetConnection() = 0;
	// 连接id
	virtual NID GetConnId() const = 0;
	// 连接句柄
	virtual NETFD FD() const = 0;

	// 主动关闭连接
	virtual bool Close(EMNetReason reason = EM_NET_REASON_PEER_DISCONNECT) = 0;
	// 是否死连接
	virtual bool IsClosed() const = 0;
	// 是否活动连接
	virtual bool IsConnected() const = 0;
	// 设置当前状态
	virtual void SetState(EMConnState state) = 0;
	// 获取当前状态
	virtual EMConnState GetState() const = 0;
	// 是否接受连接
	virtual bool IsAccept() const = 0;
	// 是否被动关闭
	virtual bool IsPassiveClosed() const = 0;
	// 是否连接成功过
	virtual bool IsSucceed() const = 0;

	// 对端ip
	virtual const char* RemoteIP() const = 0;
	// 对端端口
	virtual unsigned short RemotePort() const = 0;
	// 断开原因
	virtual EMNetReason DiedReason() const = 0;
	// 关联的服务
	virtual INetService* GetService() const = 0; 
	
	// 应用层自定义数据
	virtual int GetUD() const = 0;
	virtual void  SetUD(int ud) = 0;

	// 发送数据
	virtual bool Send(const char* data, size_t nLen) = 0;
};
inline INetConnection::~INetConnection(){}

class INetConnectionFactory
{
public:
	virtual ~INetConnectionFactory() = 0;
	virtual INetConnection* CreateConnection( size_t nSndBuf, size_t nRecvBuf, bool bIsAccept ) = 0;
	virtual void FreeConnection(INetConnection* conn) = 0;
};
inline INetConnectionFactory::~INetConnectionFactory(){}

#endif // END __INET_CONNECTION_H__