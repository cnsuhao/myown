//--------------------------------------------------------------------
// 文件名:		NetServiceCreator.h
// 内  容:		网络服务创建器
// 说  明:		
// 创建日期:		2016年5月20日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_SERVICE_CREATOR_H__
#define __NET_SERVICE_CREATOR_H__

#include "NetService.h"
#include "NetCreator.h"

class NetTcpCreator : public INetServiceCreator
{
public:
	virtual INetService* Create(INetIoDesc* pDesc,
						INetIoHandler* pHandler,
						INetConnectionFactory* pFactory,
						INetActionPacker* packer,
						INetFilter* pFilter)
	{
		INetService* pService = NetCreator::Create(pDesc, false);
		if (NULL != pService)
		{
			NetService* pNetService = dynamic_cast<NetService*>(pService);
			if (NULL != pNetService)
			{
				pNetService->SetHandler(pHandler)
					.SetFactory(pFactory)
					.SetPacker(packer)
					.SetFilter(pFilter);
			}
			else
			{
				NetCreator::Free(pService);
				pService = NULL;
			}
		}

		return pService;
	}

	virtual void Free(INetService* pService)
	{
		NetCreator::Free(pService);
	}
};

class NetHttpCreator : public INetServiceCreator
{
public:
	virtual INetService* Create(INetIoDesc* pDesc,
						INetIoHandler* pHandler,
						INetConnectionFactory* pFactory,
						INetActionPacker* packer,
						INetFilter* pFilter)
	{
		INetService* pService = NetCreator::Create(pDesc, true);
		if (NULL != pService)
		{
			NetService* pNetService = dynamic_cast<NetService*>(pService);
			if (NULL != pNetService)
			{
				pNetService->SetHandler(pHandler)
					.SetFactory(pFactory)
					.SetPacker(packer)
					.SetFilter(pFilter);
			}
			else
			{
				NetCreator::Free(pService);
				pService = NULL;
			}
		}

		return pService;
	}

	virtual void Free(INetService* pService)
	{
		NetCreator::Free(pService);
	}
};

#endif // END __NET_SERVICE_CREATOR_H__