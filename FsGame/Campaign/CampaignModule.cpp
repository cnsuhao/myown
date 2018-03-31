//--------------------------------------------------------------------
// 文件名:      CampaignModule.h
// 内  容:      活动模块
// 说  明:
// 创建日期:    2017年2月15日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#include "CampaignModule.h"
#include "CampaignBase.h"
#include "Define\PubDefine.h"
#include "Define\CommandDefine.h"
#include "CommonModule\ReLoadConfigModule.h"
#include "CommonModule\LuaExtModule.h"
#include "CampaignAsuraBattle.h"

CampaignModule* CampaignModule::m_pCampaignModule = NULL;

std::wstring CampaignModule::m_domainName = L"";

// 设置活动状态
int nx_change_campaign_state(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);			    

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_change_campaign_state, 2);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_change_campaign_state, 1);
	CHECK_ARG_INT(state, nx_change_campaign_state, 2);

	// 获取参数
	int nCampaignType = pKernel->LuaToInt(state, 1);
	int nCampaignState = pKernel->LuaToInt(state, 2);
	
	// 同步到公共区,活动结束
	CampaignModule::m_pCampaignModule->SendMsgToPubServer(pKernel, CVarList() << SP_DOMAIN_MSG_CAMPAIGN_GM_CHANGE_STATE << nCampaignType << nCampaignState);
	return 1;
}

// 初始化
bool CampaignModule::Init(IKernel* pKernel)
{
	m_pCampaignModule = this;

	pKernel->AddEventCallback("scene", "OnPublicMessage", OnPublicMessage);

	m_vecCampaignLogic.resize(MAX_CAMPAIGN_TYPE_NUM);
	LoopBeginCheck(a);
	for (int i = 0; i < MAX_CAMPAIGN_TYPE_NUM;++i)
	{
		LoopDoCheck(a);
		CampaignBase* pCampaign = CreateCampaign((CampaignType)i);
		if (NULL != pCampaign)
		{
			pCampaign->Init(pKernel);
			pCampaign->LoadResource(pKernel);
		}
		m_vecCampaignLogic[i] = pCampaign;
	}

	RELOAD_CONFIG_REG("CampaignConfig", CampaignModule::ReloadConfig);

	// gm命令
	DECL_LUA_EXT(nx_change_campaign_state);
	return true;
}

// 释放
bool CampaignModule::Shut(IKernel* pKernel)
{
	return true;
}

// 发消息给Pub服务器
bool CampaignModule::SendMsgToPubServer(IKernel* pKernel, const IVarList &msg)
{
	CVarList pubmsg;
	pubmsg << PUBSPACE_DOMAIN << CampaignModule::GetDomainName(pKernel) << msg;

	pKernel->SendPublicMessage(pubmsg);

	return true;
}

// 查询活动的当前状态数据
void CampaignModule::QueryCampaignState(IKernel* pKernel, int& nOutState, int64_t& nOutEndTime, int nGameType)
{
	nOutState = CAMPAIGN_CLOSE_STATE;
	nOutEndTime = 0;
	IRecord* pCampaignRec = FindPubRec(pKernel, PUB_CAMPAIGN_MAIN_REC);
	if (NULL == pCampaignRec)
	{
		return;
	}

	int nRowIndex = pCampaignRec->FindInt(CMR_PUB_COL_ACTIVITY_TYPE, nGameType);
	if (-1 == nRowIndex)
	{
		return;
	}

	nOutState = pCampaignRec->QueryInt(nRowIndex, CMR_PUB_COL_STATE);
	nOutEndTime = pCampaignRec->QueryInt64(nRowIndex, CMR_PUB_COL_END_TIME);
}

// 查询公共区的表
IRecord* CampaignModule::FindPubRec(IKernel * pKernel, const char* strRecName)
{
	// 公共数据
	IPubData* pPubData = FindPubData(pKernel);
	if (pPubData == NULL)
	{
		return NULL;
	}

	IRecord* pFindRec = pPubData->GetRecord(strRecName);
	return pFindRec;
}

IPubData* CampaignModule::FindPubData(IKernel * pKernel)
{
	// 公共空间
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return NULL;
	}

	// 公共数据
	std::wstring wsDomainName = GetDomainName(pKernel);
	return pPubSpace->GetPubData(wsDomainName.c_str());
}

// 响应公共区消息			 	 
int CampaignModule::OnPublicMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetType(0) != VTYPE_WIDESTR || args.GetType(1) != VTYPE_INT)
	{
		return 0;
	}

	int nIndex = 0;
	const wchar_t* strPubData = args.WideStrVal(nIndex++);
	int main_msg_id = args.IntVal(nIndex++);
	if (main_msg_id != PS_DOMAIN_MSG_CAMPAIGN_STATE)
	{
		return 0;
	}

	
	int nCurState = args.IntVal(nIndex++);
	int nGameType = args.IntVal(nIndex++);
	if (nGameType <= CT_NONE_TYPE || nGameType >= MAX_CAMPAIGN_TYPE_NUM)
	{
		return 0;
	}

	m_pCampaignModule->m_vecCampaignLogic[nGameType]->OnChangeCampainState(pKernel, nCurState);

	int nSceneNum = args.IntVal(nIndex++);
	LoopBeginCheck(q);
	for (int i = 0; i < nSceneNum;++i)
	{
		LoopDoCheck(q);
		int nSceneId = args.IntVal(nIndex++);

		pKernel->CommandToScene(nSceneId, CVarList() << COMMAND_CAMPAIGN_MSG << CAMPAIGN_STATE_CHANGE_MSG << nGameType << nCurState);
	}
	return 0;
}

CampaignBase* CampaignModule::CreateCampaign(CampaignType nType)
{
	CampaignBase* pCampaign = NULL;
	switch (nType)
	{
	case CT_ASURA_BATTLE:
		pCampaign = NEW CampaignAsuraBattle;
		break;
	}

	return pCampaign;
}

void CampaignModule::ReloadConfig(IKernel* pKernel)
{
	int nSize = m_pCampaignModule->m_vecCampaignLogic.size();
	LoopBeginCheck(r);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(r);
		CampaignBase* pCampaign = m_pCampaignModule->m_vecCampaignLogic[i];
		if (NULL != pCampaign)
		{
			pCampaign->LoadResource(pKernel);
		}
	}
}

// 获取空间名
const std::wstring & CampaignModule::GetDomainName(IKernel * pKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Campaign_%d", server_id);
		m_domainName = wstr;
	}
	return m_domainName;
}