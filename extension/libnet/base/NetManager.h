//--------------------------------------------------------------------
// 文件名:		NetManager.h
// 内  容:		网络服务管理器
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_MANAGER_H__
#define __NET_MANAGER_H__
#include "INetService.h"
#include <map>

class NetManager : public INetManager
{
	INetServiceCreator* m_pDefaultCteator;

	std::map<int, INetServiceCreator*> m_mapNetCreators;
	std::map<int, INetService*> m_mapNetServices;

public:
	NetManager() : m_pDefaultCteator(NULL)
	{

	}
	virtual ~NetManager()
	{}

	virtual bool AddCreator(int nKey, INetServiceCreator* creator)
	{
		std::map<int, INetServiceCreator*>::_Pairib&& pb = m_mapNetCreators.insert(std::make_pair(nKey, creator));
		return pb.second;
	}
	virtual bool RemoveCreator(int nKey)
	{
		std::map<int, INetServiceCreator*>::iterator itr = m_mapNetCreators.find(nKey);
		if (itr != m_mapNetCreators.end())
		{
			m_mapNetCreators.erase(itr);
			return true;
		}

		return false;
	}
	virtual bool SetDefaultCreator(INetServiceCreator* creator)
	{
		m_pDefaultCteator = creator;
		return true;
	}
	virtual INetServiceCreator* LookupCreator(int nKey)
	{
		std::map<int, INetServiceCreator*>::iterator itr = m_mapNetCreators.find(nKey);
		if (itr != m_mapNetCreators.end())
		{
			return itr->second;
		}

		return NULL;
	}

	virtual bool StartServer( INetIoDesc* pDesc,
					INetIoHandler* pHandler,
					INetConnectionFactory* pFactory,
					INetActionPacker* packer = NULL,
					INetFilter* pFilter = NULL
					)
	{
		INetServiceCreator* pCreator = LookupCreator(pDesc->GetIoKey());
		if (NULL == pCreator)
		{
			pCreator = m_pDefaultCteator;

			if (NULL == pCreator)
			{
				return false;
			}
		}

		INetService* pService = Lookup(pDesc->GetIoKey());
		if (NULL != pService)
		{
			return false;
		}

		pService = pCreator->Create(pDesc, pHandler, pFactory, packer, pFilter);
		if (NULL == pService)
		{
			return false;
		}

		std::map<int, INetService*>::_Pairib&& pb = m_mapNetServices.insert(std::make_pair(pDesc->GetIoKey(), pService));
		if (!pService->Startup())
		{
			m_mapNetServices.erase(pb.first);
			pService->Stop(0);
			pCreator->Free(pService);
			return false;
		}

		return true;
	}
	virtual bool StopServer(int nKey)
	{
		INetService* pService = Lookup(nKey);
		if (NULL == pService)
		{
			return false;
		}

		return pService->Stop(-1);
	}
	virtual void RemoveServer(int nKey)
	{
		std::map<int, INetService*>::iterator& itr = m_mapNetServices.find(nKey);
		if (itr != m_mapNetServices.end() )
		{
			INetService* pService = itr->second;
			if (NULL != pService)
			{
				if (pService->IsRunning())
				{
					pService->Stop(-1);
				}
				INetServiceCreator* pCreator = LookupCreator(nKey);
				if (NULL != pCreator)
				{
					pCreator->Free(pService);
				}
			}

			m_mapNetServices.erase(itr);
		}
	}

	virtual INetService* Lookup(int nKey)
	{
		std::map<int, INetService*>::iterator& itr = m_mapNetServices.find(nKey);
		if (itr != m_mapNetServices.end())
		{
			return itr->second;
		}

		return NULL;
	}

	virtual bool IsRunning(int nKey)
	{
		INetService* pService = Lookup(nKey);
		if (NULL == pService)
		{
			return false;
		}

		return pService->IsRunning();
	}
};
#endif // END __NET_MANAGER_H__