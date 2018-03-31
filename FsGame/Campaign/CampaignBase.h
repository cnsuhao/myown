//--------------------------------------------------------------------
// 文件名:      CampaignBase.h
// 内  容:      活动基类
// 说  明:
// 创建日期:    2017年2月15日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#ifndef _CAMPAIGN_BASE_H_
#define _CAMPAIGN_BASE_H_

#include "Fsgame/Define/header.h"

class CampaignBase
{
public:
	virtual bool Init(IKernel* pKernel) = 0;

	virtual void OnChangeCampainState(IKernel* pKernel, int nCurState) = 0;

	virtual bool LoadResource(IKernel* pKernel)
	{
		return true;
	}
};

#endif
