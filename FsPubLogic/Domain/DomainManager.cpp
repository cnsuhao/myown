//--------------------------------------------------------------------
// 文件名:		DomainManager.cpp
// 内  容:		公共数据服务器逻辑模块:全局公共数据的管理
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "DomainManager.h"
#include "DomainRankList.h"
#include "EnvirValue.h"
#include "Domain_Task.h"
#include "Domain_SceneList.h"
#include "Domain_Look.h"
#include "Domain_Friend.h"
#include "Domain_Team.h"
#include "Domain_Boss.h"
#include "Domain_WorldBoss.h"
#include "Domain_Teamoffer.h"

DomainManager* DomainManager::m_pInstance = NULL;

DomainManager::DomainManager()
{
}

DomainManager::~DomainManager()
{
	MapData::iterator it = m_mapDataList.begin();

    LoopBeginCheck(a);
	for (; it != m_mapDataList.end(); ++it)
	{
        LoopDoCheck(a);

		delete it->second;
		it->second = 0;
	}

	m_mapDataList.clear();
}

IPubLogic* DomainManager::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = NEW DomainManager;
	}

	return m_pInstance;
}

/// \brief 公共数据服务器启动
/// \param pPubKernel 核心指针，下同
int DomainManager::OnPublicStart(IPubKernel* pPubKernel)
{
	IDomainData* pDomainData = NULL;
	// 添加一个数据空间
	pDomainData = NEW Domain_SceneList();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

    pDomainData = NEW DomainRankList();
    m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Task();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Look();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Friend();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Team();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Boss();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_WorldBoss();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	pDomainData = NEW Domain_Teamoffer();
	m_mapDataList[pDomainData->GetDomainName(pPubKernel)] = pDomainData;

	EnvirValue::Init(pPubKernel);
	return 1;
}

/// \brief 公共数据服务器关闭
/// \param pPubKernel 核心指针，下同
int DomainManager::OnServerClose(IPubKernel* pPubKernel)
{
	// 保存整个公共数据的space
	pPubKernel->SavePubSpace(GetName());

	return 1;
}

/// \brief 公共数据项加载完成
/// \param data_name 公共数据项名
int DomainManager::OnPubDataLoaded(IPubKernel* pPubKernel, const wchar_t * data_name)
{
	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (pPubSpace == NULL)
	{
		Assert(false);

		return 1;
	}

	MapData::iterator it = m_mapDataList.find(data_name);
	if (it != m_mapDataList.end())
	{
		it->second->OnLoad(pPubKernel, pPubSpace);
	}

	return 1;
}

/// \brief 公共数据空间中的所有项加载完成
/// \param pPubKernel 核心指针，下同
int DomainManager::OnPubSpaceLoaded(IPubKernel* pPubKernel)
{
	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (pPubSpace == NULL)
	{
		Assert(false);

		return 1;
	}

	MapData::iterator it = m_mapDataList.begin();

    LoopBeginCheck(b);
	for (; it != m_mapDataList.end(); ++it)
	{
        LoopDoCheck(b);

		IDomainData * pDomainData = it->second;
		if (!pPubSpace->FindPubData(pDomainData->GetDomainName(pPubKernel).c_str()))
		{
			bool res = pPubSpace->AddPubData(pDomainData->GetDomainName(pPubKernel).c_str(), pDomainData->GetSave());
			if (res)
			{
				pDomainData->OnCreate(pPubKernel, pPubSpace);
			}
		}
	}

	return 1;
}

/// \brief 接收到场景服务器的消息
/// \param source_id 发消息的服务器标识
/// \param msg 消息内容
int DomainManager::OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
								   int scene_id, const IVarList& msg)
{
	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (pPubSpace == NULL)
	{
		Assert(false);

		return 1;
	}

	const wchar_t * data_name = msg.WideStrVal(1);
	MapData::iterator it = m_mapDataList.find(data_name);
	if (it != m_mapDataList.end())
	{
		it->second->OnMessage(pPubKernel, pPubSpace, source_id, scene_id, msg);
	}

	return 1;
}

// 更新配置
bool DomainManager::OnUpdateConfig(IPubKernel* pPubKernel, int source_id, 
								 int scene_id, const IVarList& msg)
{
	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (NULL == pPubSpace)
	{
		Assert(false);

		return false;
	}

	const wchar_t * data_name = msg.WideStrVal(1);
	MapData::iterator it = m_mapDataList.find(data_name);
	if (it != m_mapDataList.end())
	{
		return it->second->OnUpdateConfig(pPubKernel, pPubSpace, source_id, scene_id, msg);
	}

	return false;
}

/// \brief 获得指定名称的DomainData
/// \param data_name 名称
/// \param msg 消息内容
IDomainData * DomainManager::FindDomainData(const wchar_t * data_name)
{
	MapData::iterator it = m_mapDataList.find(data_name);
	if (it != m_mapDataList.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}
bool DomainManager::OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args)
{
	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (pPubSpace == NULL)
	{
		Assert(false);

		return 1;
	}

	MapData::iterator it = m_mapDataList.begin();

	LoopBeginCheck(b);
	for (; it != m_mapDataList.end(); ++it)
	{
		LoopDoCheck(b);

		it->second->OnPublicCommand(pPubKernel, cmd_id, args);
	}
	return true;
}
