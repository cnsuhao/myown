//--------------------------------------------------------------------
// 文件名:		NetCreator.h
// 内  容:		网络创建器
// 说  明:		
// 创建日期:		2016年5月20日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_CREATOR_H__
#define __NET_CREATOR_H__

#include "INetService.h"

class NetCreator
{
public:
	static INetService* Create( INetIoDesc* pDesc, bool bHttp = false );
	static void Free(INetService* service);

	static INetConnectionFactory* CreateConnectionFactory(bool bHttp = false);
	static void FreeConnectionFactory(INetConnectionFactory* factory);
};

#endif // END __NET_CREATOR_H__