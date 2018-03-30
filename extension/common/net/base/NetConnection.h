//--------------------------------------------------------------------
// 文件名:		NetConnection.h
// 内  容:		网络连接定义
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__

#include "INetConnection.h"
#include "public/FastStr.h"
#include <map>
#include <string>
#include "utils/FsSpinLock.h"
#include "DataBuffer.h"

// 保证只在主服务线程读线
class NetConnection : public INetConnection
{
protected:
	INetService*	m_pService;
	NID				m_nId;

	NETFD			m_nFD;		// 句柄
	int				m_ud;		// 用户数据

	TFastStr<char, 16>	m_strHost;	// ip
	unsigned short		m_nPort;	// 端口

	DataBuffer		m_dbSend;		// 发送缓冲区
	DataBuffer		m_dbRecv;		// 接收缓冲区

	EMConnState m_eState;	// 连接状态
	EMNetReason m_eReason;	// 断开原因

	bool		m_bIsAccept;		// 是否是服务器连接
	bool		m_bIsPassived;		// 被动关闭
	bool		m_bSucceed;			// 是否成功连接过

	bool		m_bIsSending;
public:
	NetConnection(size_t nSndBuf, size_t nReadBuf);

	NetConnection& SetConnId(NID id)
	{
		m_nId = id;
		return *this;
	}
	virtual NID GetConnId() const
	{
		return m_nId;
	}

	NetConnection& SetFD(const NETFD& fd)
	{
		m_nFD = fd;
		return *this;
	}
	virtual NETFD FD() const
	{
		return m_nFD;
	}


	NetConnection& SetAccept( bool bAccept )
	{
		m_bIsAccept = bAccept;
		return *this;
	}
	virtual bool IsAccept() const
	{
		return m_bIsAccept;
	}
	// 是否被动关闭
	virtual bool IsPassiveClosed() const
	{
		return m_bIsPassived;
	}
	// 是否死连接
	virtual bool IsClosed() const
	{
		return m_eState == EM_CONN_STATE_CLOSED;
	}
	// 是否活动连接
	virtual bool IsConnected() const
	{
		return m_eState == EM_CONN_STATE_CONNECTED;
	}
	// 设置状态
	virtual void SetState(EMConnState state)
	{
		m_eState = state;
		if (m_eState == EM_CONN_STATE_CONNECTED)
		{
			m_bSucceed = true;
		}
	}
	// 获取状态
	virtual EMConnState GetState() const
	{
		return m_eState;
	}
	// 是否连接成功过
	virtual bool IsSucceed() const
	{
		return m_bSucceed;
	}

	NetConnection& SetRemotIP( const char* ip )
	{
		m_strHost = NULL == ip ? "" : ip;
		return *this;
	}
	virtual const char* RemoteIP() const
	{
		return m_strHost.c_str();
	}

	NetConnection& SetRemotePort(unsigned short nPort)
	{
		m_nPort = nPort;
		return *this;
	}
	virtual unsigned short RemotePort() const
	{
		return m_nPort;
	}

	virtual EMNetReason DiedReason() const
	{
		return m_eReason;
	}

	virtual INetService* GetService() const
	{
		return m_pService;
	}

	virtual NetConnection& SetService(INetService* pService)
	{
		m_pService = pService;
		return *this;
	}

	// 应用层自定义数据
	virtual int GetUD() const
	{
		return m_ud;
	}
	virtual void  SetUD(int ud)
	{
		m_ud = ud;
	}

	virtual bool Send(const char* szMsg, size_t nLen);

	virtual void Reset();

	// 发送回调
	virtual void OnSend(const char* data, size_t nLen);
	// 收到数据
	virtual bool OnRecv(const char* data, size_t nLen);

	// 连接成功回调
	virtual void OnConnected();
	// 已经关闭回调
	virtual void OnClosed( EMNetReason reason, int nErrorCode );

	// 发起关闭
	virtual bool Close(EMNetReason reason = EM_NET_REASON_PEER_DISCONNECT);

	DataBuffer& GetRecvBufer()
	{
		return m_dbRecv;
	}

	DataBuffer& GetSendBuffer()
	{
		return m_dbSend;
	}

protected:
	virtual void PostSend() = 0;
	//{
	//	char*	pb = NULL;
	//	size_t	nSize = 0;
	//	bool bExp = false;
	//	if ( m_bIsSending.compare_exchange_strong(bExp, true) )
	//	{
	//		LockGuard<SpinLock> guard(m_senderLocker);
	//		pb = m_dbSend.GetBuffer() + m_dbSend.GetReadPos();
	//		nSize = m_dbSend.GetValidSize();
	//		if (nSize <= 0)
	//			m_bIsSending.exchange(false);
	//	}

	//	if ( pb != NULL && nSize > 0 )
	//	{
	//		RealSend( pb, nSize );
	//	}
	//}

	//virtual bool RealSend(const char* data, size_t nLen)
	//{
	//	OnSend( data, nLen );
	//	return true;
	//}
};

#endif // END __NET_CONNECTION_H__