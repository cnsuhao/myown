#include "NetConnection.h"
#include "INetService.h"

NetConnection::NetConnection( size_t nSndBuf, size_t nReadBuf )
	: m_pService(NULL)
	, m_nId(0)
	, m_nFD(0)
	, m_ud(NULL)
	, m_nPort(0)
	, m_eReason(EM_NET_REASON_SELF_DISCONNECT)
	, m_bIsAccept(false)
	, m_dbSend(nSndBuf)
	, m_dbRecv(nReadBuf)
	, m_eState(EM_CONN_STATE_NOT_CONNECTED)
	, m_bIsSending( false )
	, m_bIsPassived( true )
	, m_bSucceed( false )
{

}

bool NetConnection::Send(const char* szMsg, size_t nLen)
{
	if (IsClosed())
	{
		return false;
	}

	if (m_dbSend.GetLeftSpace() >= nLen)
	{
		m_dbSend.Write( szMsg, nLen );
	}
	else
	{
		return false;
	}

	PostSend();
	return true;
}

void NetConnection::Reset()
{
	m_pService = NULL;
	m_nId = 0;

	m_nFD = 0;			// 句柄
	m_ud = NULL;		// 用户数据

	m_strHost.clear();	// ip
	m_nPort = 0;		// 端口

	m_dbSend.Clear();		// 发送缓冲区
	m_dbRecv.Clear();		// 接收缓冲区

	// 断开原因
	m_eReason = EM_NET_REASON_SELF_DISCONNECT;
	m_eState = EM_CONN_STATE_NOT_CONNECTED;
	m_bIsAccept = true;
	m_bIsSending = false;
}

void NetConnection::OnSend(const char* data, size_t nLen)
{
	m_dbSend.Read(NULL, nLen);
	m_dbSend.Rewind();
	m_bIsSending = false;
	if (m_dbSend.GetValidSize() > 0)
	{
		PostSend();
	}
}

bool NetConnection::OnRecv( const char* data, size_t nLen )
{
	m_dbRecv.Write( data, nLen );
	// 解包处理
	size_t nPos = 0;
	size_t nSize = m_dbRecv.GetValidSize();
	char* pb = m_dbRecv.GetBuffer() + m_dbRecv.GetReadPos();
	INetActionPacker* packer = m_pService->GetPacker();
	INetIoHandler* pHandler = m_pService->GetHandler();
	while (nPos < nSize)
	{
		std::shared_ptr<INetAction> pAction = NULL;
		size_t nDataLen = nSize - nPos;
		FsRefStream refStream(pb + nPos, nDataLen);
		refStream.SeekWritePos((int32)nDataLen);
		size_t nLen = 0;
		EMPackResult result = EM_PACK_RAW_MSG;
		if (NULL != packer)
		{
			result = packer->UnPack(this, refStream, pAction, &nLen);
		}
		switch (result)
		{
		case EM_PACK_SUCCESS:
			nPos += nLen;
			if (pAction->IsRequest())
			{
				pHandler->OnAction(this, pAction);
			}
			else if ( pAction->IsResult() )
			{
				pHandler->OnReply(this, pAction);
			}
			else
			{
				assert(false);
			}
			break;
		case EM_PACK_UNREGISTER:
			nPos += nLen;
			pHandler->OnUnRegistAction(this, pAction);
			break;
		case EM_PACK_BROKEN:
			nPos += nLen;
			goto END;
			break;
		case EM_PACK_EMPTY:
			nPos += nLen;
			if ( NULL != pAction.get() )
			{
				pHandler->OnAction(this, pAction);
			}
			break;
		case EM_PACK_RAW_MSG:
			pHandler->OnRawMessage(this, pb + nPos, (int32)(nSize - nPos));
			nPos = nSize;
			break;
		default:
			EMNetErrorDealWith dealWith = pHandler->OnErrorMessage(this, pb + nPos, (int32)(nSize - nPos));
			if (EM_NET_ERRROR_DEAL_WITH_BREAK == dealWith)
			{
				this->Close(EM_NET_REASON_LOGIC_ERRROR);
				return false;
			}
			else
			{
				nPos = nSize;
			}
			break;
		}
	}

END:
	m_dbRecv.Read(NULL, nPos);
	m_dbRecv.Rewind();
	return true;
}

void NetConnection::OnConnected()
{
	assert(m_eState == EM_CONN_STATE_CONNECTING);
	assert(NULL != m_pService);
	SetState(EM_CONN_STATE_CONNECTED);
	INetIoHandler* pHandler = m_pService->GetHandler();
	if (NULL != pHandler)
	{
		pHandler->OnConnect(this);
	}
}

void NetConnection::OnClosed(EMNetReason reason, int nErrorCode)
{
	if (m_eState != EM_CONN_STATE_CLOSED)
	{
		m_eState = EM_CONN_STATE_CLOSED;
		m_eReason = reason;
		m_nFD = 0;

		// 如果是连接的未成功不回调关闭通知
		if (IsAccept() && !IsSucceed())
		{
			return;
		}

		assert(NULL != m_pService);
		INetIoHandler* pHandler = m_pService->GetHandler();
		if (NULL != pHandler)
		{
			pHandler->OnClose(this, reason, nErrorCode);
		}
	}
}

bool NetConnection::Close(EMNetReason reason /* = EM_NET_REASON_PEER_DISCONNECT */)
{
	// 未关闭则执行
	if (m_eState == EM_CONN_STATE_CONNECTED || m_eState == EM_CONN_STATE_CONNECTING)
	{
		m_eState = EM_CONN_STATE_CLOSING;
		m_bIsPassived = false;
		OnClosed(reason, 0);
		return true;
	}
	return false;
}
