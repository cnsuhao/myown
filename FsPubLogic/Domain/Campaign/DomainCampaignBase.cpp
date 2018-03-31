//--------------------------------------------------------------------
// 文件名:      DomainCampaignBase.h
// 内  容:      公共区活动基类
// 说  明:
// 创建日期:    2017年4月18日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#include "DomainCampaignBase.h"

bool DomainCampaignBase::OnCampaignTimer(IPubKernel* pPubKernel, IPubData* pPubData)
{
	return true;
}

DomainCampaignBase* DomainCampaignBase::CreateCampaign(CampaignType nType)
{
	DomainCampaignBase* pCampaign = NULL;
// 	switch (nType)
// 	{
// 
// 	}

	return pCampaign;
}
