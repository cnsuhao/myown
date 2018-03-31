//--------------------------------------------------------------------
// 文件名:      DomainCampaignBase.h
// 内  容:      公共区活动基类
// 说  明:
// 创建日期:    2017年4月18日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#ifndef _DOMAIN_CAMPAIGN_BASE_H_
#define _DOMAIN_CAMPAIGN_BASE_H_

#include "../IDomainData.h"
#include "FsGame/Define/CampaignDefine.h"

class DomainCampaignBase
{
public:
	virtual bool OnLoad(IPubKernel* pPubKernel, IPubData* pPubData) = 0;

	virtual bool OnMessage(IPubKernel* pPubKernel, IPubData* pPubData, int source_id, int scene_id, const IVarList& msg) = 0;

	virtual bool OnCampaignTimer(IPubKernel* pPubKernel, IPubData* pPubData);

	static DomainCampaignBase* CreateCampaign(CampaignType nType);
};

#endif
