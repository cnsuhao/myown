//--------------------------------------------------------------------
// 文件名:      DomainCampaign.h
// 内  容:      活动流程
// 说  明:		
// 创建日期:    2017年2月13日
// 创建人:      刘明飞
// 修改人:        
// 版权所有:      
//--------------------------------------------------------------------

#ifndef __DOMAIN_CAMPAIGN_H__
#define __DOMAIN_CAMPAIGN_H__

#include "../IDomainData.h"
#include <vector>
#include <string>

class DomainCampaignBase;																																					 

class DomainCampaign : public IDomainData
{
public:
	DomainCampaign() : IDomainData(L"")
	{
		m_bGmControl = false;
	}
	
public:
	virtual const std::wstring & GetDomainName(IPubKernel * pPubKernel);

	virtual bool GetSave();
	virtual int OnCreate(IPubKernel * pPubKernel, IPubSpace * pPubSpace); 
	virtual int OnLoad(IPubKernel * pPubKernel, IPubSpace * pPubSpace);

	///  \brief 接收到场景服务器的消息
	///  \param source_id 发消息的服务器标识
	///  \param msg 消息内容
	virtual int OnMessage(IPubKernel * pPubKernel,
						  IPubSpace * pPubSpace,
						  int source_id,
						  int scene_id,
						  const IVarList & msg);
private:
	enum
	{
		CHECK_STATE_INTERVAL = 5000,		// 检测状态频率 
	};

	// 状态数据
	struct StateData 
	{
		int			nState;				// 状态值
		int			nStartTime;			// 状态开始时间
		int			nEndTime;			// 状态结束时间
	};

	typedef std::vector<StateData>	StateVec;	

	// 活动数据
	struct CampaignData 
	{
		int					nCampaignId;		// 活动id
		int					nGameType;			// 玩法类型
		std::vector<int>	vecSceneId;			// 活动场景
		std::vector<int>	vecWeek;			// 活动日期
		StateVec			vecStateData;		// 状态数据
	};

	typedef std::vector<CampaignData>	CampaignDataVec;

	// 读取配置
	bool LoadResource(IPubKernel* pPubKernel);

	// 检测活动状态心跳
	static int CheckCampaignStateTimer(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time);

	// 判断当前的活动状态
	int JudgeCampaignState(int64_t& nEndTime, const CampaignData& camdata);

	// 查询活动数据
	const CampaignData* QueryCampaignData(int nCampaignId);

	// 解析星期和场景配置
	bool ParseXmlSection(std::vector<int>& vecOutData, const char* strData);

	// 解析活动状态配置
	bool ParseCampaignState(StateVec& vecOutData, const char* strState, int nState);

	// 响应活动状态改变
	bool OnCampaignEnd(IPubKernel* pPubKernel, IPubData* pPubData, int nCampaignType);

	// 响应gm命令控制活动状态改变
	bool OnGMControlCampaignState(IPubKernel* pPubKernel, IPubData* pPubData, int nCampaignId, int nCampaignState);

	// 向活动场景发改变活动状态消息
	void SendStateChangeToCampaignScene(IPubKernel* pPubKernel, const CampaignData* pData, int nCampaignState);

	typedef std::vector<DomainCampaignBase*> DomainCampaignVec;
private:
	CampaignDataVec			m_vecCampaignData;		// 活动配置
	bool					m_bGmControl;			// 是否用gm命令控制
	DomainCampaignVec		m_vecDomainCampaign;	// 所有活动类

	static DomainCampaign * m_pDomainCampaign;
};

#endif
