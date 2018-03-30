#include "LibEventConnection.h"

LibEventConnection::LibEventConnection(size_t nSndBuf, size_t nReadBuf)
	: NetConnection( nSndBuf, nReadBuf )
	, m_pbufev( NULL )
	, m_pNotify( NULL )
	, m_nLastSendSize( 0 )
{

}

bool LibEventConnection::Initilize(struct bufferevent* buf, NETFD fd, struct event* pSendNotify, 
								bufferevent_data_cb readcb, 
								bufferevent_data_cb writecb, 
								bufferevent_event_cb eventcb)
{
	m_pbufev = buf;
	this->SetFD(fd);

	evutil_make_socket_nonblocking(fd);

	m_pNotify = pSendNotify;
	bufferevent_setcb(m_pbufev, readcb, writecb, eventcb, this);
	bufferevent_enable(m_pbufev, EV_READ | EV_WRITE);
	return true;
}

void LibEventConnection::OnClosed(EMNetReason reason, int nErrCode)
{
	if (NULL != m_pbufev)
	{
		bufferevent_setcb(m_pbufev, NULL, NULL, NULL, NULL);
		bufferevent_disable(m_pbufev, EV_READ | EV_WRITE);
		bufferevent_free(m_pbufev);
		m_pbufev = NULL;
	}

	if (NULL != m_nFD)
	{
		evutil_closesocket(m_nFD);
	}
	NetConnection::OnClosed(reason, nErrCode);
}

bool LibEventConnection::Close(EMNetReason reason /* = EM_NET_REASON_PEER_DISCONNECT */)
{
	if (m_eState == EM_CONN_STATE_CONNECTED || m_eState == EM_CONN_STATE_CONNECTING)
	{
		m_eState = EM_CONN_STATE_CLOSING;
		m_bIsPassived = false;

		// 关闭sockket, 让其回调OnClosed
		if (NULL != m_nFD)
		{
			evutil_closesocket(m_nFD);
			m_nFD = NULL;
		}
		return true;
	}
	return false;
}

void LibEventConnection::OnSend(const char* data, size_t nLen)
{
	nLen = m_nLastSendSize;
	m_nLastSendSize = 0;
	NetConnection::OnSend(m_dbSend.GetBuffer(), nLen);
}

void LibEventConnection::PostSend()
{
	if (IsConnected() && !m_bIsSending)
	{
		size_t nSize = m_dbSend.GetValidSize();
		if (nSize > 0)
		{
			m_bIsSending = true;
			char* pb = m_dbSend.GetBuffer() + m_dbSend.GetReadPos();
			if (bufferevent_write(m_pbufev, pb, nSize) == 0)
			{
				m_nLastSendSize = nSize;
				event_active(m_pNotify, 0, 0);
				//OnSend(pb, nSize);
			}
		}
	}
}