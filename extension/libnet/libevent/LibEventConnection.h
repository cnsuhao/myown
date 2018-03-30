//--------------------------------------------------------------------
// 文件名:		LibEventConnection.h
// 内  容:		libevent连接类
// 说  明:		
// 创建日期:		2016年6月7日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_LIB_EVENT_CONNECTION_H__
#define __NET_LIB_EVENT_CONNECTION_H__

#include "base/NetConnection.h"
#include <event2/event.h>
#include <event2/bufferevent.h>

class LibEventConnection : public NetConnection
{
	struct bufferevent* m_pbufev;
	struct event*		m_pNotify;
	
public:
	LibEventConnection(size_t nSndBuf, size_t nReadBuf);

	bool Initilize(struct bufferevent* buf,
						NETFD fd,
						struct event* pSendNotify,
						bufferevent_data_cb readcb,
						bufferevent_data_cb writecb,
						bufferevent_event_cb eventcb);

	virtual void OnSend(const char* data, size_t nLen);
	virtual void OnClosed(EMNetReason reason, int nErrCode);
	virtual bool Close(EMNetReason reason /* = EM_NET_REASON_PEER_DISCONNECT */);
protected:
	virtual void PostSend();

	size_t m_nLastSendSize;
};

/*
class LibEventSocket : public SocketDesc
{
	char* m_pb;
	size_t m_nMaxSize;
	size_t m_nValidSize;
	void ReleaseBuf()
	{
		if (NULL != m_pb)
		{
			delete[] m_pb;
		}
		m_pb = NULL;
		m_nMaxSize = 0;
		m_nValidSize = 0;
	}
public:
	LibEventSocket(struct event_base* base, 
					NETFD fd, 
					const char* ip, 
					unsigned short port) : SocketDesc(fd, ip, port)
					, m_pb( NULL )
					, m_nMaxSize( 0 )
					, m_nValidSize( 0 )
	{
		m_pbufev = bufferevent_socket_new(base, fd, 0);
		evutil_make_socket_nonblocking(fd);
	}

	void InitBuffer(size_t nSize)
	{
		ReleaseBuf();
		m_pb = new char[nSize];
		m_nMaxSize = nSize;
		m_nValidSize = 0;
	}

	char* GetBufer()
	{
		return m_pb;
	}

	size_t GetLeft() const
	{
		return m_nValidSize < m_nMaxSize ? m_nMaxSize - m_nValidSize : 0;
	}

	size_t GetValidSize() const
	{
		return m_nValidSize;
	}

	void OnWriteBuf(size_t nSize)
	{
		assert(nSize + m_nValidSize <= m_nMaxSize);
		m_nValidSize += nSize;
		if (m_nValidSize > m_nMaxSize)
		{
			m_nValidSize = m_nMaxSize;
		}
	}

	void OnReadBuf( size_t nSize )
	{
		if (NULL != m_pb)
		{
			if (nSize >= m_nValidSize)
			{
				m_nValidSize = 0;
			}
			else
			{
				size_t nLeft = m_nValidSize - nSize;
				memmove(m_pb, m_pb + nSize, nLeft);
				m_nValidSize = nLeft;
			}
		}
	}

	~LibEventSocket()
	{
		SetDead();
		Close(EM_NET_REASON_SERVER_DISCONNECT);
		ReleaseBuf();
	}

	virtual bool Close( EMNetReason reason )
	{
		if (IsConnected())
		{
			evutil_closesocket(m_nFD);
			m_nFD = 0;
			m_eReason = reason;
		}

		return true;
	}

	virtual void SetDead()
	{
		if (NULL != m_pbufev)
		{
			bufferevent_disable(m_pbufev, EV_READ | EV_WRITE);
			bufferevent_free(m_pbufev);
			m_pbufev = NULL;
		}
	}

	virtual bool Send(const char* pdata, size_t nLen)
	{
		if (NULL != m_pbufev)
		{
			return bufferevent_write(m_pbufev, pdata, nLen) == 0;
		}

		return false;
	}

	bool Start( bufferevent_data_cb readcb, bufferevent_data_cb writecb,
		bufferevent_event_cb eventcb )
	{
		if (NULL != m_pbufev && IsConnected())
		{
			bufferevent_setcb(m_pbufev, readcb, writecb, eventcb, this);
			bufferevent_enable(m_pbufev, EV_READ | EV_WRITE);

			return true;
		}
		return false;
	}
};
*/
#endif // END __NET_LIB_EVENT_CONNECTION_H__