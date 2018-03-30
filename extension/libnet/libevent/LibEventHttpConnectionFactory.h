//--------------------------------------------------------------------
// 文件名:		LibEventHttpConnectionFactory.h
// 内  容:		网络连接创建工厂
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_LIBEVENT_HTTP_CONNECTION_CREATOR_H__
#define __NET_LIBEVENT_HTTP_CONNECTION_CREATOR_H__

#include "LibEventHttpConnection.h"

class LibEventHttpConnectionFactory : public INetConnectionFactory
{
public:

	virtual INetConnection* CreateConnection(size_t nSndBuf, size_t nRecvBuf, bool bIsAccept)
	{
		LibEventHttpConnection* conn = new LibEventHttpConnection();
		conn->SetAccept(bIsAccept);
		return conn;
	}

	virtual void FreeConnection(INetConnection* conn)
	{
		if (NULL != conn)
		{
			delete conn;
		}
	}
};

#endif // END LibEventHttpConnectionFactory