//--------------------------------------------------------------------
// 文件名:      CampaignModule.h
// 内  容:      活动模块
// 说  明:
// 创建日期:    2017年2月15日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#ifndef _CAMPAIGN_MODULE_H_
#define _CAMPAIGN_MODULE_H_

#include "Fsgame/Define/header.h"
#include "Define/CampaignDefine.h"
#include <vector>

class CampaignBase;

class CampaignModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

	// 发消息给Pub服务器
	bool SendMsgToPubServer(IKernel* pKernel, const IVarList &msg);

	// 查询活动的当前状态数据
	void QueryCampaignState(IKernel* pKernel, int& nOutState, int64_t& nOutEndTime, int nGameType);

	// 查询公共区的表
	IRecord* FindPubRec(IKernel * pKernel, const char* strRecName);

	IPubData* FindPubData(IKernel * pKernel);
private:
	typedef std::vector<CampaignBase*> CampaignVec;

	// 响应公共区消息
	static int OnPublicMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static CampaignBase* CreateCampaign(CampaignType nType);

	static void ReloadConfig(IKernel* pKernel);

	static const std::wstring & GetDomainName(IKernel * pKernel);
public:
    // 本模块指针
	static CampaignModule* m_pCampaignModule;
private:
	CampaignVec		m_vecCampaignLogic;		// 所有活动功能

	// 公共数据名称
	static std::wstring m_domainName;
};

#endif
