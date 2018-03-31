//--------------------------------------------------------------------
// 文件名:      CampaignAsuraBattle.h
// 内  容:      修罗战场活动
// 说  明:
// 创建日期:    2017年10月19日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------
#include "CampaignAsuraBattle.h"
#include "Define\CampaignDefine.h"
#include "Define\CommandDefine.h"
#include "Define\ClientCustomDefine.h"
#include "utils\custom_func.h"
#include "CampaignModule.h"
#include "CommonModule\LandPosModule.h"
#include "CommonModule\SwitchManagerModule.h"
#include "CommonModule\ActionMutex.h"
#include "Define\Fields.h"
#include "CommonModule\AsynCtrlModule.h"
#include "utils\XmlFile.h"
#include "utils\extend_func.h"
#include "utils\string_util.h"
#include "utils\util_func.h"
#include "NpcBaseModule\AI\AIDefine.h"
#include "Interface\FightInterface.h"
#include "SystemFunctionModule\CapitalModule.h"
#include <algorithm>
#include "Define\ServerCustomDefine.h"
#include "SystemFunctionModule\RewardModule.h"
//#include "CommonModule\WorldLevelAddExp.h"

CampaignModule*	CampaignAsuraBattle::m_pCampaignModule = NULL;

CampaignAsuraBattle*	CampaignAsuraBattle::m_pCampaignAsuraBattle = NULL;
LandPosModule*			CampaignAsuraBattle::m_pLandPosModule = NULL;

static const char* ASURA_BATTLE_RANK_REWARD_TITLE = "asura_battle_rank_reward_title"; // 名次奖励标题
static const char* ASURA_BATTLE_RESULT_WIN_REWARD_TITLE = "asura_battle_result_win_reward_title"; // 胜利奖励标题
static const char* ASURA_BATTLE_RESULT_LOSE_REWARD_TITLE = "asura_battle_result_lose_reward_title"; // 失败奖励标题

static const float NOT_VALID_POS = 1000.0f;


bool CampaignAsuraBattle::Init(IKernel* pKernel)
{
	m_pCampaignAsuraBattle = this;
	m_pCampaignModule = (CampaignModule*)pKernel->GetLogicModule("CampaignModule");
	m_pLandPosModule = (LandPosModule*)pKernel->GetLogicModule("LandPosModule");

	Assert(NULL != m_pCampaignAsuraBattle && NULL != m_pCampaignModule && NULL != m_pLandPosModule);

	pKernel->AddIntCommandHook("player", COMMAND_BEKILL, CampaignAsuraBattle::OnCommandBeKill);
	pKernel->AddIntCommandHook("scene", COMMAND_CAMPAIGN_MSG, CampaignAsuraBattle::OnCommandCampaignMsg);
	pKernel->AddIntCommandHook("AsuraBossNpc", COMMAND_BEDAMAGE, CampaignAsuraBattle::OnCommandBossBeDamage);
	pKernel->AddIntCommandHook("AsuraBossNpc", COMMAND_BEKILL, CampaignAsuraBattle::OnCommandBossBeKill);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_ASURA_BATTLE, CampaignAsuraBattle::OnCustomAsuraBattle);

	pKernel->AddEventCallback("player", "OnReady", CampaignAsuraBattle::OnPlayerReady);
	pKernel->AddEventCallback("player", "OnLeaveScene", CampaignAsuraBattle::OnPlayerLeaveScene);

	DECL_HEARTBEAT(CampaignAsuraBattle::HB_CheckBroadcastInfo);
	DECL_HEARTBEAT(CampaignAsuraBattle::HB_SendReward);
	DECL_HEARTBEAT(CampaignAsuraBattle::HB_LeaveScene);

	return true;
}

void CampaignAsuraBattle::OnChangeCampainState(IKernel* pKernel, int nCurState)
{
	if (ASURA_BATTLE_READY == nCurState)
	{
		::CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_101, CVarList());
	}
	else if (ASURA_BATTLE_START == nCurState)
	{
		CVarList msg;
		msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_START;
		pKernel->CustomByWorld(msg);
	}
}

bool CampaignAsuraBattle::LoadResource(IKernel* pKernel)
{
	if (!LoadMaterialConfig(pKernel))
	{
		return false;
	}

	if (!LoadCampBuffConfig(pKernel))
	{
		return false;
	}

	if (!LoadConstConfig(pKernel))
	{
		return false;
	}

	if (!LoadShopBuffConfig(pKernel))
	{																								   
		return false;
	}

	if (!LoadRankRewardConfig(pKernel))
	{
		return false;
	}

	if (!LoadBossScoreRule(pKernel))
	{
		return false;
	}
	return true;
}

// 活动是否开启
bool CampaignAsuraBattle::IsCampaignStart(IKernel* pKernel)
{
	int nCurState = ASURA_BATTLE_CLOSE;
	int64_t nEndTime = 0;
	m_pCampaignModule->QueryCampaignState(pKernel, nCurState, nEndTime, CT_ASURA_BATTLE);

	return nCurState == ASURA_BATTLE_START;
}

// 响应客户端消息
int CampaignAsuraBattle::OnCustomAsuraBattle(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self) || args.GetCount() < 2 || args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_INT)
	{
		return 0;
	}
// 	CS_ASURA_BATTLE_ENTER,					// 进入修罗战场
// 		CS_ASURA_BATTLE_COLLECTION,				// 采集战资	 object 对象id
// 		CS_ASURA_BATTLE_BUY_BUFF,				// 购买buff	 int buffid
// 		CS_ASURA_BATTLE_EXIT,					// 退出修罗战场

	int nSubMsg = args.IntVal(1);
	switch (nSubMsg)
	{
	case CS_ASURA_BATTLE_ENTER:
		m_pCampaignAsuraBattle->OnCustomEnterScene(pKernel, self);
		break;
	case CS_ASURA_BATTLE_COLLECTION:
		m_pCampaignAsuraBattle->OnCustomCollectMaterial(pKernel, self, args);
		break;
	case CS_ASURA_BATTLE_BUY_BUFF:
		m_pCampaignAsuraBattle->OnCustomBuyBuff(pKernel, self, args);
		break;
	case CS_ASURA_BATTLE_EXIT:
		m_pCampaignAsuraBattle->OnCustomExitScene(pKernel, self);
		break;
	case CS_ASURA_BATTLE_SUBMIT:
		m_pCampaignAsuraBattle->OnCustomSubmitMaterial(pKernel, self, args);
		break;
	case CS_ASURA_BATTLE_QUERY_RANK_LIST:
		m_pCampaignAsuraBattle->OnCustomQueryRankList(pKernel, self, args);
		break;
	}
	return 0;
}

// 响应玩家进入场景
int CampaignAsuraBattle::OnPlayerReady(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	if (!IsInAsuraBattleScene(pKernel))
	{
		return 0;
	}

	if (!IsCampaignStart(pKernel))
	{
		m_pLandPosModule->PlayerReturnLandPosi(pKernel, self);
	}
	else
	{
		IGameObj* pSelfObj = pKernel->GetGameObj(self);
		if (NULL == pSelfObj)
		{
			return 0;
		}
		// 进入场景,增加阵营buff
		int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
		int nCurCollectState = m_pCampaignAsuraBattle->QueryCampCollectState(pKernel, nCamp);
		const CampBuff* pCampBuffData = m_pCampaignAsuraBattle->QueryCollectData(nCurCollectState);
		if (NULL != pCampBuffData)
		{
			const char* strBuff = pCampBuffData->strBuffId.c_str();
			if (!StringUtil::CharIsNull(strBuff))
			{
				FightInterfaceInstance->AddBuffer(pKernel, self, self, strBuff);
			}
		}

		// 初始化默认玩家积分
		IRecord* pScoreRec = m_pCampaignAsuraBattle->GetCampScoreRec(pKernel, nCamp);
		if (NULL == pScoreRec)
		{
			return 0;
		}

		const wchar_t* wsName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
		int nRowIndex = pScoreRec->FindWideStr(CAMP_PLAYER_COL_NAME, wsName);
		if (-1 == nRowIndex)
		{
			// 清空遗留的数据
			m_pCampaignAsuraBattle->ClearPlayerFightData(pKernel, self);

			CVarList value;
			value << wsName << 0 << 0 << 0 << 0;
			pScoreRec->AddRowValue(-1, value);
			m_pCampaignAsuraBattle->UpdateFightData(pKernel, self, m_pCampaignAsuraBattle->m_kConstData.nPlayerInitScore);
		}
	}
	return 0;
}

// 响应玩家离开场景
int CampaignAsuraBattle::OnPlayerLeaveScene(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	if (!IsInAsuraBattleScene(pKernel))
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!IsCampaignStart(pKernel))
	{
		m_pCampaignAsuraBattle->ClearPlayerFightData(pKernel, self);
	}
	else
	{
		int nCurScore = m_pCampaignAsuraBattle->QueryPlayerFightData(pKernel, self, COLUMN_ASURA_FIGHT_REC_SCORE);
		int nDecScore = (int)((float)nCurScore * m_pCampaignAsuraBattle->m_kConstData.fLeaveDecScoreRate);
		
		m_pCampaignAsuraBattle->UpdateFightData(pKernel, self, -nDecScore);

		pSelfObj->SetInt(FIELD_PROP_ASURA_COLLECT_SCORE, 0);
	}
	
	return 0;
}

// 响应活动消息
int CampaignAsuraBattle::OnCommandCampaignMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	int nSubMsg = args.IntVal(1);
	if (CAMPAIGN_STATE_CHANGE_MSG == nSubMsg)
	{
		m_pCampaignAsuraBattle->OnCommandCampaignStateChange(pKernel, self, sender, args);
	}
	return 0;
}

// 响应BOSS被攻击
int CampaignAsuraBattle::OnCommandBossBeDamage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	const char* strScirpt = pSelfObj->GetScript();
	if (strcmp(strScirpt, CLASS_NAME_ASURA_BOSS_NPC) != 0)
	{
		return 0;
	}

	IRecord* pBeDamageRec = pSelfObj->GetRecord(FIELD_RECORD_ASURABOSS_ATTACK_LIST_REC);
	if (NULL == pBeDamageRec)
	{
		return 0;
	}

	PERSISTID realattacker = get_pet_master(pKernel, sender);
	IGameObj* pAttacker = pKernel->GetGameObj(realattacker);
	if (NULL == pAttacker)
	{
		return 0;
	}

	int nDamageVal = args.IntVal(1);
	const wchar_t* wsPlayerName = pAttacker->QueryWideStr(FIELD_PROP_NAME);
	int nCamp = pAttacker->QueryInt(FIELD_PROP_CAMP);

	int nRowIndex = pBeDamageRec->FindWideStr(COLUMN_ASURABOSS_ATTACK_LIST_REC_NAME, wsPlayerName);
	if (-1 == nRowIndex)
	{
		pBeDamageRec->AddRowValue(-1, CVarList() << wsPlayerName << nCamp << nDamageVal);
	}
	else
	{
		int nDamVal = pBeDamageRec->QueryInt(nRowIndex, COLUMN_ASURABOSS_ATTACK_LIST_REC_DAMVAL);
		nDamVal += nDamageVal;
		pBeDamageRec->SetInt(nRowIndex, COLUMN_ASURABOSS_ATTACK_LIST_REC_DAMVAL, nDamVal);
	}
	return 0;
}

// 响应BOSS被杀死
int CampaignAsuraBattle::OnCommandBossBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
																																															  
	const char* strScirpt = pSelfObj->GetScript();
	if (strcmp(strScirpt, CLASS_NAME_ASURA_BOSS_NPC) != 0)
	{
		return 0;
	}

	IRecord* pBeDamageRec = pSelfObj->GetRecord(FIELD_RECORD_ASURABOSS_ATTACK_LIST_REC);
	if (NULL == pBeDamageRec)
	{
		return 0;
	}

	// 移除优先显示
	pKernel->RemoveVisualPriorityByScene(self);

	// 判断胜负 统计参与人数
	int nJHTotalDamVal = 0;
	int nCTTotalDamVal = 0;
	int nJHPlayerNum = 0;
	int nCTPlayerNum = 0;
	m_pCampaignAsuraBattle->StatDamageBossData(pKernel, self, nJHTotalDamVal, nCTTotalDamVal, nJHPlayerNum, nCTPlayerNum);

	int nWinCamp = nCTTotalDamVal >= nJHTotalDamVal ? CAMP_RED : CAMP_BLUE;		

	// 为胜利阵营玩家增加积分
	int nPlayerNum = nWinCamp == CAMP_RED ? nCTPlayerNum : nJHPlayerNum;
	int nAddScore = m_pCampaignAsuraBattle->QueryKillBossAddScore(nPlayerNum);

	int nRows = pBeDamageRec->GetRows();
	LoopBeginCheck(o);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(o);
		const wchar_t* wsName = pBeDamageRec->QueryWideStr(i, COLUMN_ASURABOSS_ATTACK_LIST_REC_NAME);
		int nCamp = pBeDamageRec->QueryInt(i, COLUMN_ASURABOSS_ATTACK_LIST_REC_CAMP);
		if (nCamp != nWinCamp)
		{
			continue;
		}
		PERSISTID player = pKernel->FindPlayer(wsName);
		m_pCampaignAsuraBattle->UpdateFightData(pKernel, player, nAddScore);
	}

	m_pCampaignAsuraBattle->SyncBossFightData(pKernel);

	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (pSceneObj != NULL)
	{
		pSceneObj->SetObject(FIELD_PROP_ASURA_BOSS, PERSISTID());
	}
	// boss被杀通告
	CustomSysInfoByScene(pKernel, pKernel->GetSceneId(), SYSTEM_INFO_ID_102, CVarList() << GetCampTxtId(nWinCamp) << nAddScore);
	return 0;
}

// 响应活动状态改变
int CampaignAsuraBattle::OnCommandCampaignStateChange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	int nGameType = args.IntVal(2);
	int nCurState = args.IntVal(3);

	if (nGameType != CT_ASURA_BATTLE)
	{
		return 0;
	}

	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_CAMPAIGN_ASURA_BATTLE))
	{
		return 0;
	}

	int nSceneId = pKernel->GetSceneId();
	PERSISTID scene = pKernel->GetScene();
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return 0;
	}

	// 活动状态改变
	if (ASURA_BATTLE_READY == nCurState)
	{
		ADD_COUNT_BEAT(pKernel, scene, "CampaignAsuraBattle::HB_GenerateMaterial", m_kConstData.nGenrateInterval, 1);
		ADD_HEART_BEAT(pKernel, scene, "CampaignAsuraBattle::HB_CheckBroadcastInfo", CHECK_BROADCAST_INTERVAL);
		IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
		if (NULL == pCampFightRec)
		{
			return 0;
		}
		pCampFightRec->AddRowValue(-1, CVarList() << CAMP_BLUE << 0 << 0 << 0);
		pCampFightRec->AddRowValue(-1, CVarList() << CAMP_RED << 0 << 0 << 0);
	}
	else if (ASURA_BATTLE_START == nCurState)
	{
		// 查询公共区的表
		IRecord* pMainRec = m_pCampaignModule->FindPubRec(pKernel, PUB_CAMPAIGN_MAIN_REC);
		if (NULL == pMainRec)
		{
			return 0; 
		}
		int nRowIndex = pMainRec->FindInt(CMR_PUB_COL_ACTIVITY_TYPE, CT_ASURA_BATTLE);
		if (-1 != nRowIndex)
		{
			int nActivityId = pMainRec->QueryInt(nRowIndex, CMR_PUB_COL_ACTIVITY_ID);
			pKernel->SetInt(scene, FIELD_PROP_ASURA_BATTLE_ID, nActivityId);
		}
	}
	else if (ASURA_BATTLE_CLOSE == nCurState)
	{
		CampaignOver(pKernel);
	}

	return 0;
}

// 响应进入修罗战场
void CampaignAsuraBattle::OnCustomEnterScene(IKernel* pKernel, const PERSISTID& self)
{
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_CAMPAIGN_ASURA_BATTLE, self))
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 是否互斥
	if (ActionMutexModule::IsHaveAction(pKernel, self))
	{
		return;
	}

	// 活动未开启
	if (!IsCampaignStart(pKernel))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_103, CVarList());
		return;
	}

	int nPlayerLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	if (nPlayerLevel < m_kConstData.nMinLevel)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_104, CVarList());
		return;
	}

	// 开始进入场景
	// 记录下玩家此时的位置
	len_t pos_x = pKernel->GetPosiX(self);
	len_t pos_z = pKernel->GetPosiZ(self);
	len_t pos_y = pKernel->GetMapHeight(pos_x, pos_z);
	int sceneId = pKernel->GetSceneId();
	m_pLandPosModule->SetPlayerLandPosi(pKernel, self, sceneId, pos_x, pos_y, pos_z);

	int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	const Pos& pos = nCamp == CAMP_BLUE ? m_kConstData.kJHBornPos : m_kConstData.kCTBornPos;

	AsynCtrlModule::m_pAsynCtrlModule->SwitchLocate(pKernel, self, m_kConstData.nSceneId, pos.fPosX, 0.0f, pos.fPosZ, pos.fOrient);
}

// 响应采集战资
void CampaignAsuraBattle::OnCustomCollectMaterial(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	if (args.GetCount() > 3 || args.GetType(1) != VTYPE_INT || args.GetType(2) != VTYPE_OBJECT)
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	float fSelfPosX = pSelfObj->GetPosiX();
	float fSelfPosZ = pSelfObj->GetPosiZ();
	float fSelfPosY = pKernel->GetMapHeight(fSelfPosX, fSelfPosZ);

	int nCollectState = QueryCampCollectState(pKernel, nCamp);
	int nAddScore = 0;
	int nSize = (int)m_vecMaterialGenRule.size();
	LoopBeginCheck(t);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(t);
		const MaterialGenRule& data = m_vecMaterialGenRule[i];
		float nDistance2 = util_dot_distance2(fSelfPosX, fSelfPosZ, data.kCenterPos.fPosX, data.kCenterPos.fPosZ);
		if (nDistance2 < data.fRadius2)
		{
			// 备战期积分与其他阶段积分不同	
			if (nCollectState == CAMP_COLLECT_FIGHT)
			{
				nAddScore = nCamp == CAMP_RED ? data.nCTFightScore : data.nJHFightScore;						  
			}
			else
			{
				nAddScore = nCamp == CAMP_RED ? data.nCTStartScore : data.nJHStartScore;
			}
			break;
		}
	}

	pSelfObj->SetInt(FIELD_PROP_ASURA_COLLECT_SCORE, nAddScore);
}

// 响应提交战资
void CampaignAsuraBattle::OnCustomSubmitMaterial(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	int nAddScore = pSelfObj->QueryInt(FIELD_PROP_ASURA_COLLECT_SCORE);
	int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);

	UpdateFightData(pKernel, self, nAddScore);
	UpdateCampFightRec(pKernel, nCamp, COLUMN_CAMP_FIGHT_REC_NUM, 1);
	pSelfObj->SetInt(FIELD_PROP_ASURA_COLLECT_SCORE, 0);

	CVarList msg;
	msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_SUBMIT_MATERIAL << nAddScore;
	pKernel->Custom(self, msg);
}

// 响应查询排行榜
void CampaignAsuraBattle::OnCustomQueryRankList(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	const wchar_t* wsName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);

	int nSelfRank = QueryPlayerRank(pKernel, wsName, nCamp);
	CVarList msg;
	msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_FIGHTINT_RANK_INFO << nSelfRank;

	CVarList jh_arg;
	CVarList ct_arg;
	GenerateFightRankNum(pKernel, jh_arg, CAMP_BLUE);
	GenerateFightRankNum(pKernel, ct_arg, CAMP_RED);

	msg << jh_arg << ct_arg;
	pKernel->Custom(self, msg);
}

// 响应购买buff
void CampaignAsuraBattle::OnCustomBuyBuff(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	if (args.GetCount() > 3 || args.GetType(1) != VTYPE_INT || args.GetType(2) != VTYPE_STRING)
	{
		return;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{																										 
		return;
	}

	const char* strBuffId = args.StringVal(2);
	const ShopBuff* pBuffData = QueryShopBuffData(strBuffId);
	if (NULL == pBuffData)
	{
		return;
	}

	CVarList buff;
	if (FightInterfaceInstance->FindBuffer(pKernel, self, strBuffId, buff))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_105, CVarList());
		return;
	}

	if (!CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, pBuffData->nCapitalType, pBuffData->nCapitalCost))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_106, CVarList());
		return;
	}

	if (CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, pBuffData->nCapitalType, pBuffData->nCapitalCost, FUNCTION_EVENT_ID_ASURA_BATTLE) != DC_SUCCESS)
	{
		return;
	}

	FightInterfaceInstance->AddBuffer(pKernel, self, self, pBuffData->strBuffId.c_str());

	CVarList msg;
	msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_BUY_BUFF_SUC;
	pKernel->Custom(self, msg);
}

// 响应退出修罗战场
void CampaignAsuraBattle::OnCustomExitScene(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	m_pLandPosModule->PlayerReturnLandPosi(pKernel, self);
}

// 读取采集物品配置
bool CampaignAsuraBattle::LoadMaterialConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/BattleMaterialConfig.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecMaterialGenRule.clear();
	m_vecMaterialGenRule.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		MaterialGenRule data;

		data.nJHStartScore	= xmlfile.ReadInteger(section, "JHStartScore", 0);
		data.nCTStartScore	= xmlfile.ReadInteger(section, "CTStartScore", 0);
		data.nJHFightScore = xmlfile.ReadInteger(section, "JHFightScore", 0);
		data.nCTFightScore = xmlfile.ReadInteger(section, "CTFightScore", 0);

		float fRadius	= xmlfile.ReadFloat(section, "Radius", 0.0f);
		data.fRadius2 = fRadius * fRadius;

		const char* strPos = xmlfile.ReadString(section, "Pos", "");
		ParsePos(data.kCenterPos, strPos);

		m_vecMaterialGenRule.push_back(data);
	}
	return true;
}

// 响应目标被杀
int CampaignAsuraBattle::OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (NULL == pSelfObj || NULL == pSenderObj || NULL == m_pCampaignAsuraBattle || pSelfObj->GetClassType() != TYPE_PLAYER || pSenderObj->GetClassType() != TYPE_PLAYER)
	{
		return 0;
	}
	if (!m_pCampaignAsuraBattle->IsInAsuraBattleScene(pKernel) || !m_pCampaignAsuraBattle->IsCampaignStart(pKernel))
	{
		return 0;
	}

	const ConstData& kConstData = m_pCampaignAsuraBattle->m_kConstData;
	// 提示购买buff
	int nDeadNum = pSelfObj->QueryInt(FIELD_PROP_ASURA_DEAD_NUM);
	++nDeadNum;
	pSelfObj->SetInt(FIELD_PROP_ASURA_DEAD_NUM, nDeadNum);
	if (nDeadNum == kConstData.nDeadBuyTipNum)
	{
		CVarList msg;
		msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_BUY_BUFF_TIP;
		pKernel->Custom(self, msg);

		pSelfObj->SetInt(FIELD_PROP_ASURA_DEAD_NUM, 0);
	}
	// 结算死亡目标的积分
	int nOldScore = m_pCampaignAsuraBattle->QueryPlayerFightData(pKernel, self, COLUMN_ASURA_FIGHT_REC_SCORE);
	int nDecScore = 0;
	nDecScore = __max((int)((float)nOldScore * kConstData.fKillScoreRate), 1);
	m_pCampaignAsuraBattle->UpdateFightData(pKernel, self, -nDecScore, CAMP_PLAYER_COL_DEAD, 1);
	
	// 结算杀人及助攻者的积分

	// 总积分是否要增加爵位差值运算
	int nAddScore = nDecScore + kConstData.nKillBaseScore;

	// 计算加减分的比率
	float fPeerageDiffRate = kConstData.fPeerageAddRate;
	fPeerageDiffRate = __max(fPeerageDiffRate, kConstData.fMinPeerageAddRate);
	fPeerageDiffRate = __min(fPeerageDiffRate, kConstData.fMaxPeerageAddRate);
	float fPeerageAddRate = 1 + fPeerageDiffRate;

	nAddScore = (int)((float)nAddScore * fPeerageAddRate);

	const wchar_t* wsSelfName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);

	CVarList attackerlist;
	// 有助攻的玩家
	if (FightInterfaceInstance->GetAttackerList(pKernel, attackerlist, self, sender))
	{
		// 增加杀人者积分
		int nKillAddScore = (int)((float)nAddScore * kConstData.fKillAddRate);
		nKillAddScore = m_pCampaignAsuraBattle->ComputeAddScore(pKernel, sender, nKillAddScore);
		m_pCampaignAsuraBattle->UpdateFightData(pKernel, sender, nKillAddScore, CAMP_PLAYER_COL_KILL, 1);

		// 增加积分提示
		::CustomSysInfo(pKernel, sender, SYSTEM_INFO_ID_107, CVarList() << wsSelfName << nKillAddScore);

		// 增加助攻玩家积分
		int nATNum = attackerlist.GetCount();
		if (nATNum <= 0)
		{
			return 0;
		}
		int nATAddScore = __max((int)((float)nAddScore * kConstData.fATAddRate / (float)nATNum), 1);
		LoopBeginCheck(s);
		for (int i = 0; i < nATNum;++i)
		{
			LoopDoCheck(s);
			PERSISTID player = attackerlist.ObjectVal(i);
			int nRealScore = m_pCampaignAsuraBattle->ComputeAddScore(pKernel, player, nATAddScore);
			m_pCampaignAsuraBattle->UpdateFightData(pKernel, player, nRealScore, CAMP_PLAYER_COL_ASSIST, 1);

			// 增加积分提示
			::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_108, CVarList() << wsSelfName << nRealScore);
		}
	}
	else // 自己单独杀的
	{
		int nRealScore = m_pCampaignAsuraBattle->ComputeAddScore(pKernel, sender, nAddScore);
		m_pCampaignAsuraBattle->UpdateFightData(pKernel, sender, nRealScore, CAMP_PLAYER_COL_KILL, 1);

		// 增加积分提示
		::CustomSysInfo(pKernel, sender, SYSTEM_INFO_ID_107, CVarList() << wsSelfName << nRealScore);
	}

	return 0;
}

// 读取buff配置
bool CampaignAsuraBattle::LoadCampBuffConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/active_Camp_buff.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecCampBuff.clear();
	m_vecCampBuff.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		CampBuff data;

		data.nCollectState	= StringUtil::StringAsInt(section);
		data.nMinCollectNum = xmlfile.ReadInteger(section, "MinCollectNum", 0);
		data.nMaxCollectNum = xmlfile.ReadInteger(section, "MaxCollectNum", 0);
		data.strBuffId		= xmlfile.ReadString(section, "BuffId", "");

		m_vecCampBuff.push_back(data);
	}
	return true;
}

// 读取常量配置
bool CampaignAsuraBattle::LoadConstConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/AsuraBattleConstConfig.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	const char* strSectionName = xmlfile.GetSectionByIndex(0);

	m_kConstData.nSceneId = xmlfile.ReadInteger(strSectionName, "SceneID", 0);
	m_kConstData.nMinLevel = xmlfile.ReadInteger(strSectionName, "MinLevel", 0);
	m_kConstData.fKillScoreRate = xmlfile.ReadFloat(strSectionName, "KillScoreRate", 0.0f);
	m_kConstData.fKillAddRate = xmlfile.ReadFloat(strSectionName, "KillAddRate", 0.0f);
	m_kConstData.fATAddRate = xmlfile.ReadFloat(strSectionName, "ATAddRate", 0.0f);
	m_kConstData.fPeerageAddRate = xmlfile.ReadFloat(strSectionName, "PeerageAddRate", 0.0f);
	m_kConstData.nBroadcastCondition = xmlfile.ReadInteger(strSectionName, "BroadcastCondition", 0);
	m_kConstData.nPlayerInitScore = xmlfile.ReadInteger(strSectionName, "PlayerInitScore", 0);
	m_kConstData.nGenrateInterval = xmlfile.ReadInteger(strSectionName, "GenrateInterval", 0) * 1000;
	m_kConstData.nKillBaseScore = xmlfile.ReadInteger(strSectionName, "KillBaseScore", 0);
	m_kConstData.fMaxPeerageAddRate = xmlfile.ReadFloat(strSectionName, "MaxPeerageAddRate", 0.0f);
	m_kConstData.fMinPeerageAddRate = xmlfile.ReadFloat(strSectionName, "MinPeerageAddRate", 0.0f);
	m_kConstData.fLeaveDecScoreRate = xmlfile.ReadFloat(strSectionName, "LeaveDecScoreRate", 0.0f);
	m_kConstData.strWinReward = xmlfile.ReadString(strSectionName, "WinReward", "");
	m_kConstData.strLoseReward = xmlfile.ReadString(strSectionName, "LoseReward", "");
	m_kConstData.nDelaySendRewardTime = xmlfile.ReadInteger(strSectionName, "DelaySendRewardTime", 0) * 1000;
	m_kConstData.nDelayExitTime = xmlfile.ReadInteger(strSectionName, "DelayExitTime", 0) * 1000;
	m_kConstData.nSettleRankNum = xmlfile.ReadInteger(strSectionName, "SettleRankNum", 0);
	m_kConstData.nScoreFirstTipInterval = xmlfile.ReadInteger(strSectionName, "ScoreFirstTipInterval", 0) * 1000;
	m_kConstData.nDeadBuyTipNum = xmlfile.ReadInteger(strSectionName, "DeadBuyTipNum", 0);
	m_kConstData.nFightRankNum = xmlfile.ReadInteger(strSectionName, "FightRankNum", 0);
	m_kConstData.nBossBornTime = xmlfile.ReadInteger(strSectionName, "BossBornTime", 0) * 60;
	m_kConstData.nBossTipTime = xmlfile.ReadInteger(strSectionName, "BossTipTime", 0) * 60;
	m_kConstData.strBossId = xmlfile.ReadString(strSectionName, "BossId", "");

	const char* strPos = xmlfile.ReadString(strSectionName, "JHBornPos", "");
	ParsePos(m_kConstData.kJHBornPos, strPos);

	strPos = xmlfile.ReadString(strSectionName, "CTBornPos", "");
	ParsePos(m_kConstData.kCTBornPos, strPos);

	strPos = xmlfile.ReadString(strSectionName, "BossBornPos", "");
	ParsePos(m_kConstData.kBossBornPos, strPos);

	const char* strBattleTip = xmlfile.ReadString(strSectionName, "BattleTimeTip", "");
	ParseTimeTipData(strBattleTip);

	return true;
}

// 读取商店buff配置
bool CampaignAsuraBattle::LoadShopBuffConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/AsuraShopBuff.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecShopBuff.clear();
	m_vecShopBuff.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		ShopBuff data;

		data.strBuffId = xmlfile.ReadString(section, "BuffId", "");
		data.nCapitalCost = xmlfile.ReadInteger(section, "CapitalNum", 0);
		data.nCapitalType = xmlfile.ReadInteger(section, "CapitalType", 0);

		m_vecShopBuff.push_back(data);
	}
	return true;
}

// 读取boss积分规则
bool CampaignAsuraBattle::LoadBossScoreRule(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/AsuraBossScore.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecBossScoreRule.clear();
	m_vecBossScoreRule.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		BossScoreRule data;
		data.nMinNum = xmlfile.ReadInteger(section, "MinNum", 0);
		data.nMaxNum = xmlfile.ReadInteger(section, "MaxNum", 0);
		data.nAddScore = xmlfile.ReadInteger(section, "AddScore", 0);

		m_vecBossScoreRule.push_back(data);
	}
	return true;
}

// 是否在修罗战场中
bool CampaignAsuraBattle::IsInAsuraBattleScene(IKernel* pKernel)
{
	int nSceneId = pKernel->GetSceneId();

	if (NULL == m_pCampaignAsuraBattle)
	{
		return false;
	}
	return nSceneId == m_pCampaignAsuraBattle->m_kConstData.nSceneId;
}

// 采集物品生成检测心跳
// int CampaignAsuraBattle::HB_GenerateMaterial(IKernel* pKernel, const PERSISTID& self, int slice)
// {
// 	IGameObj* pSceneObj = pKernel->GetGameObj(self);
// 	if (NULL == pSceneObj || NULL == m_pCampaignAsuraBattle)
// 	{
// 		return 0;
// 	}
// 
// 	int nPosNum = (int)m_pCampaignAsuraBattle->m_vecMaterialGenRule.size();
// 	LoopBeginCheck(k);
// 	for (int i = 0; i < nPosNum; ++i)
// 	{
// 		LoopDoCheck(k);
// 		m_pCampaignAsuraBattle->GenerateMaterialAtPos(pKernel, i);
// 	}
// 
// 	return 0;
// }

// 发放结算奖励
int CampaignAsuraBattle::HB_SendReward(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj || NULL == m_pCampaignAsuraBattle)
	{
		return 0;
	}

	// 发放奖励及数据同步
	m_pCampaignAsuraBattle->SendBattleRewardByCamp(pKernel, CAMP_BLUE);
	m_pCampaignAsuraBattle->SendBattleRewardByCamp(pKernel, CAMP_RED);

	// 增加延迟离开场景心跳
	ADD_COUNT_BEAT(pKernel, self, "CampaignAsuraBattle::HB_LeaveScene", m_pCampaignAsuraBattle->m_kConstData.nDelayExitTime, 1);
	return 0;
}

// 活动结束,离开场景
int CampaignAsuraBattle::HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 将场景内玩家踢出
	CVarList playerlist;
	pKernel->GetChildList(pKernel->GetScene(), TYPE_PLAYER, playerlist);
	int nCount = (int)playerlist.GetCount();
	LoopBeginCheck(c);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(c);
		PERSISTID player = playerlist.ObjectVal(i);
		m_pLandPosModule->PlayerReturnLandPosi(pKernel, player);
	}

	m_pCampaignAsuraBattle->ClearBattleData(pKernel);
	return 0;
}

// 更新玩家自己的积分
void CampaignAsuraBattle::UpdateFightData(IKernel* pKernel, const PERSISTID& self, int nChangeScore, int nColType, int nChangeFightData)
{
	if (!IsCampaignStart(pKernel))
	{
		return;
	}

	IGameObj* pSceneObj = pKernel->GetSceneObj();
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || NULL == pSceneObj)
	{
		return;
	}

	// 更新自己的战斗数据
	int nPlayerColType = GetPlayerColType(pKernel, nColType);
	UpdatePlayerData(pKernel, self, nChangeScore, nPlayerColType, nChangeFightData);

	// 更新排行榜数据
	const wchar_t* wsName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
	int nCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	IRecord* pScoreRec = GetCampScoreRec(pKernel, nCamp);
	if (NULL == pScoreRec)
	{
		return;
	}

	int nRowIndex = pScoreRec->FindWideStr(CAMP_PLAYER_COL_NAME, wsName);
	if (-1 == nRowIndex)
	{
		return;
	}

	int nCurScore = pScoreRec->QueryInt(nRowIndex, CAMP_PLAYER_COL_SCORE);
	nCurScore = __max(nCurScore + nChangeScore, 0);

	pScoreRec->SetInt(nRowIndex, CAMP_PLAYER_COL_SCORE, nCurScore);

	// 更新阵营总成绩
	UpdateCampFightRec(pKernel, nCamp, COLUMN_CAMP_FIGHT_REC_SCORE, nChangeScore);

	// 更新排行榜战斗数据
	if (-1 == nColType)
	{
		return;
	}

	int nCurVal = pScoreRec->QueryInt(nRowIndex, nColType);
	nCurVal += nChangeFightData;
	pScoreRec->SetInt(nRowIndex, nColType, nCurVal);
}

// 根据阵营获取积分表
IRecord* CampaignAsuraBattle::GetCampScoreRec(IKernel* pKernel, int nCamp)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj || nCamp <= CAMP_NONE || nCamp >= CAMP_TYPE_TOTAL)
	{
		return NULL;
	}

	const char* strRecName = nCamp == CAMP_BLUE ? FIELD_RECORD_BLUE_SCORE_REC : FIELD_RECORD_RED_SCORE_REC;
	return pSceneObj->GetRecord(strRecName);
}

// 更新阵营数据表
void CampaignAsuraBattle::UpdateCampFightRec(IKernel* pKernel, int nCamp, int nColType, int nChangeVal)
{
	if (!IsCampaignStart(pKernel))
	{
		return;
	}
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj || nColType != COLUMN_CAMP_FIGHT_REC_NUM && nColType != COLUMN_CAMP_FIGHT_REC_SCORE)
	{
		return;
	}
	IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
	if (NULL == pCampFightRec)
	{
		return;
	}
	int nRowIndex = pCampFightRec->FindInt(COLUMN_CAMP_FIGHT_REC_CAMP, nCamp);
	if (-1 == nRowIndex)
	{
		return;
	}
	int nOldVal = pCampFightRec->QueryInt(nRowIndex, nColType);
	int nCurVal = __max(nOldVal + nChangeVal, 0);
	pCampFightRec->SetInt(nRowIndex, nColType, nCurVal);

	// 采集总数变化
	if (nColType == COLUMN_CAMP_FIGHT_REC_NUM)
	{
		int nState = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_STATE);
		// 到达武装阶段了,就不用改状态了 
		if (nState < CAMP_COLLECT_FIGHT)
		{
			int nQueryState = QueryCollectState(nCurVal);
			if (nState < nQueryState)
			{
				pCampFightRec->SetInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_STATE, nQueryState);
				OnActiveCampCollectState(pKernel, nCamp, nQueryState);
			}
		}
	}
}

// 解析出生点坐标
bool CampaignAsuraBattle::ParsePos(Pos& outPos, const char* strPos)
{
	if (StringUtil::CharIsNull(strPos))
	{
		return false;
	}

	CVarList pos;
	StringUtil::SplitString(pos, strPos, ",");
	if (pos.GetCount() != 3)
	{
		return false;
	}
	outPos.fPosX = pos.FloatVal(0);
	outPos.fPosZ = pos.FloatVal(1);
	outPos.fOrient = pos.FloatVal(2);

	return true;
}

// 获取当前采集阶段
int CampaignAsuraBattle::QueryCollectState(int nCollectNum)
{
	int nState = CAMP_COLLECT_START;
	int nSize = (int)m_vecCampBuff.size();
	LoopBeginCheck(d);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(d);
		const CampBuff& data = m_vecCampBuff[i];
		if (nCollectNum >= data.nMinCollectNum && nCollectNum <= data.nMaxCollectNum)
		{
			nState = data.nCollectState;
			break;
		}
	}

	return nState;
}

// 响应阵营激活采集阶段
void CampaignAsuraBattle::OnActiveCampCollectState(IKernel* pKernel, int nCamp, int nCollectState)
{
	const CampBuff* pBuffData = QueryCollectData(nCollectState);
	if (NULL == pBuffData)
	{
		return;
	}
	const char* strAddBuff = pBuffData->strBuffId.c_str();
	if (StringUtil::CharIsNull(strAddBuff))
	{
		return;
	}
	// 为场景中的玩家加buff
	CVarList playerlist;
	pKernel->GetChildList(pKernel->GetScene(), TYPE_PLAYER, playerlist);
	int nCount = (int)playerlist.GetCount();
	LoopBeginCheck(c);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(c);
		PERSISTID player = playerlist.ObjectVal(i);
		IGameObj* pPlayerObj = pKernel->GetGameObj(player);
		if (NULL == pPlayerObj)
		{
			continue;
		}	   
		int nPlayerCamp = pPlayerObj->QueryInt(FIELD_PROP_CAMP);
		if (nPlayerCamp == nCamp)
		{
			FightInterfaceInstance->AddBuffer(pKernel, player, player, strAddBuff);
		}
	}

	if (nCollectState > CAMP_COLLECT_START)
	{
		char strTxtId[64] = { 0 };
		SPRINTF_S(strTxtId, "%s%d", "AsuraCollect_", nCollectState);
		::CustomSysInfoByScene(pKernel, pKernel->GetSceneId(), SYSTEM_INFO_ID_109, CVarList() << GetCampTxtId(nCamp) << strTxtId);
	}
}

// 查询当前的采集状态
int CampaignAsuraBattle::QueryCampCollectState(IKernel* pKernel, int nCamp)
{
	int nCollectState = CAMP_COLLECT_START;
	do 
	{
		IGameObj* pSceneObj = pKernel->GetSceneObj();
		if (NULL == pSceneObj)
		{
			break;
		}
		IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
		if (NULL == pCampFightRec)
		{
			break;
		}
		int nRowIndex = pCampFightRec->FindInt(COLUMN_CAMP_FIGHT_REC_CAMP, nCamp);
		if (-1 == nRowIndex)
		{
			break;
		}

		nCollectState = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_STATE);																    
	} while (0);
	
	return nCollectState;
}

// 计算增加的积分
int CampaignAsuraBattle::ComputeAddScore(IKernel* pKernel, const PERSISTID& self, int nScore)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || nScore <= 0)
	{
		return 0;
	}

	float fAsuraAddRate = pSelfObj->QueryFloat(FIELD_PROP_ASURA_ADD_RATE);
	int nAddScore = int((float)nScore * (1 + fAsuraAddRate));

	return nAddScore;
}

// 检测广播消息通告
int CampaignAsuraBattle::HB_CheckBroadcastInfo(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 1.检测通告剩余时间
	m_pCampaignAsuraBattle->BroadcastTimeLeftTip(pKernel);

	// 2.通用战斗数据广播
	m_pCampaignAsuraBattle->SyncCampFightInfo(pKernel);

	// 3.检测通告积分第一
	m_pCampaignAsuraBattle->BroadcastFirstPlayerTip(pKernel, slice);

	// 4.检测战场mvp
	m_pCampaignAsuraBattle->SetBattleMVP(pKernel);

	// 5.检测boss刷新
	m_pCampaignAsuraBattle->CheckBossBorn(pKernel);

	// 6.同步boss战斗数据
	m_pCampaignAsuraBattle->SyncBossFightData(pKernel);
	return 0;
}

// 根据buffid查询商店buff数据
const CampaignAsuraBattle::ShopBuff* CampaignAsuraBattle::QueryShopBuffData(const char* strBuffId)
{
	if (StringUtil::CharIsNull(strBuffId))
	{
		return NULL;
	}

	const ShopBuff* pFindData = NULL;
	int nSize = (int)m_vecShopBuff.size();
	LoopBeginCheck(e);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(e);
		const ShopBuff& data = m_vecShopBuff[i];
		if (strcmp(strBuffId, data.strBuffId.c_str()) == 0)
		{
			pFindData = &m_vecShopBuff[i];
			break;
		}
	}

	return pFindData;
}

// 提示结束的时间点配置解析
bool CampaignAsuraBattle::ParseTimeTipData(const char* strInfo)
{
	if (StringUtil::CharIsNull(strInfo))
	{
		return false;
	}

	CVarList args;
	StringUtil::SplitString(args, strInfo, ",");
	int nCount = args.GetCount();
	if (nCount <= 0)
	{
		return false;
	}
	m_vecTimeLeftTip.resize(nCount);
	LoopBeginCheck(r);
	for (int i = 0; i < nCount;++i)
	{
		LoopDoCheck(r);
		m_vecTimeLeftTip[i] = args.IntVal(i) * 60;
	}

	return true;
}

// 剩余时间通告
void CampaignAsuraBattle::BroadcastTimeLeftTip(IKernel* pKernel)
{
	int nCurState = ASURA_BATTLE_CLOSE;
	int64_t nEndTime = 0;
	m_pCampaignModule->QueryCampaignState(pKernel, nCurState, nEndTime, CT_ASURA_BATTLE);
	if (nCurState != ASURA_BATTLE_START)
	{
		return;
	}

	int nSceneId = pKernel->GetSceneId();
	// 当前的时间点
	int nCurTime = util_convert_zerotime_to_sec();
	int nLeftTiem = (int)nEndTime - nCurTime; // 还有多久结束

	int nSize = (int)m_vecTimeLeftTip.size();
	LoopBeginCheck(q);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(q);
		int nTipTime = m_vecTimeLeftTip[i];
		if (nLeftTiem > nTipTime && nLeftTiem < nTipTime + CHECK_BROADCAST_INTERVAL / 1000)
		{
			::CustomSysInfoByScene(pKernel, nSceneId, SYSTEM_INFO_ID_110, CVarList() << nTipTime / 60);
			break;
		}
	}
}

// 通告当前阵营积分第一
void CampaignAsuraBattle::BroadcastFirstPlayerTip(IKernel* pKernel, int nSlice)
{
	static int nTipTimeLast = 0;
	nTipTimeLast += nSlice;
	if (nTipTimeLast >= m_kConstData.nScoreFirstTipInterval)
	{
		const wchar_t* wsJhFirstName = L"";
		const wchar_t* wsCTFirstName = L"";
		if (!m_vecJHSortData.empty())
		{
			wsJhFirstName = m_vecJHSortData[0].wsPlayerName.c_str();
		}

		if (!m_vecCTSortData.empty())
		{
			wsCTFirstName = m_vecCTSortData[0].wsPlayerName.c_str();
		}

		// 必须双方都有第一才显示
		if (StringUtil::CharIsNull(wsCTFirstName) || StringUtil::CharIsNull(wsJhFirstName))
		{
			return;
		}
		int nSceneId = pKernel->GetSceneId();
		std::string strJHTxt = GetCampTxtId(CAMP_BLUE);
		std::string strCTTxt = GetCampTxtId(CAMP_RED);

		::CustomSysInfoByScene(pKernel, nSceneId, SYSTEM_INFO_ID_111, CVarList() << strJHTxt.c_str() << wsJhFirstName << strCTTxt.c_str() << wsCTFirstName);
		nTipTimeLast = 0;
	}
}

// 设置当前战场的mvp
void CampaignAsuraBattle::SetBattleMVP(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}

	const wchar_t* wsJHMVPName = QueryMVPPlayer(pKernel, CAMP_BLUE);
	const wchar_t* wsCTMVPName = QueryMVPPlayer(pKernel, CAMP_RED);

	pSceneObj->SetWideStr(FIELD_PROP_BLUE_MVP, wsCTMVPName);
	pSceneObj->SetWideStr(FIELD_PROP_RED_MVP, wsJHMVPName);
}

// 检测boss刷新
void CampaignAsuraBattle::CheckBossBorn(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}

	int nCurState = ASURA_BATTLE_CLOSE;
	int64_t nEndTime = 0;
	m_pCampaignModule->QueryCampaignState(pKernel, nCurState, nEndTime, CT_ASURA_BATTLE);
	if (nCurState != ASURA_BATTLE_START)
	{
		return;
	}

	int nCurTime = util_convert_zerotime_to_sec();
	int nBattleLeftTime = (int)nEndTime - nCurTime;

	if (nBattleLeftTime >= m_kConstData.nBossTipTime && nBattleLeftTime < m_kConstData.nBossTipTime + CHECK_BROADCAST_INTERVAL / 1000)
	{
		// boss出生提示公告
		CustomSysInfoByScene(pKernel, pKernel->GetSceneId(), SYSTEM_INFO_ID_112, CVarList());
	}
	// nEndTime == 0 gm命令调试时用到
	else if (nEndTime == 0 || nBattleLeftTime >= m_kConstData.nBossBornTime && nBattleLeftTime < m_kConstData.nBossBornTime + CHECK_BROADCAST_INTERVAL / 1000)
	{
		PERSISTID boss = pSceneObj->QueryObject(FIELD_PROP_ASURA_BOSS);
		if (pKernel->Exists(boss))
		{
			return;
		}
		int nWorldLevel = 1;// WorldAverageLevelAddExpSingleton::Instance()->QueryWorldLevel(pKernel);
		float fPosY = pKernel->GetMapHeight(m_kConstData.kBossBornPos.fPosX, m_kConstData.kBossBornPos.fPosZ);
		boss = pKernel->CreateObjectArgs("", m_kConstData.strBossId.c_str(), 0, m_kConstData.kBossBornPos.fPosX, fPosY
									, m_kConstData.kBossBornPos.fPosZ, m_kConstData.kBossBornPos.fOrient
									, CVarList() << CREATE_TYPE_PROPERTY_VALUE << FIELD_PROP_GROUP_ID << -1 << FIELD_PROP_NPC_PACKAGE_LEVEL << nWorldLevel);

		pSceneObj->SetObject(FIELD_PROP_ASURA_BOSS, boss);

		// 添加可视优先级
		pKernel->AddVisualPriorityByScene(boss, true);

		// boss出生公告
		CustomSysInfoByScene(pKernel, pKernel->GetSceneId(), SYSTEM_INFO_ID_113, CVarList());
	}
}

// 同步boss战斗数据
void CampaignAsuraBattle::SyncBossFightData(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}

	PERSISTID boss = pSceneObj->QueryObject(FIELD_PROP_ASURA_BOSS);
	IGameObj* pBossObj = pKernel->GetGameObj(boss);
	if (NULL == pBossObj)
	{
		return;
	}

	int nJHTotalDamVal = 0;
	int nCTTotalDamVal = 0;
	int nJHPlayerNum = 0;
	int nCTPlayerNum = 0;
	StatDamageBossData(pKernel, boss, nJHTotalDamVal, nCTTotalDamVal, nJHPlayerNum, nCTPlayerNum);

	int64_t nCurBossHP = pBossObj->QueryInt64(FIELD_PROP_HP);
	int64_t nMaxBossHP = pBossObj->QueryInt64(FIELD_PROP_MAX_HP);
	if (nMaxBossHP <= 0)
	{
		return;
	}

	float fBossHPRate = (float)nCurBossHP / (float)nMaxBossHP * 100.0f;

	int nSceneId = pKernel->GetSceneId();
	CVarList msg;
	msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_FIGHT_BOSS_DATA << nJHTotalDamVal << nCTTotalDamVal << (int)fBossHPRate;
	pKernel->CustomByScene(nSceneId, msg);
}

// 必要战斗数据同步(积分榜重新排序)
void CampaignAsuraBattle::SyncCampFightInfo(IKernel* pKernel)
{
	ExecuteRecordDataSort(pKernel, m_vecJHSortData, CAMP_BLUE);
	ExecuteRecordDataSort(pKernel, m_vecCTSortData, CAMP_RED);

	CVarList jhInfo;
	CVarList ctInfo;
	GenerateCampFightInfo(pKernel, jhInfo, CAMP_BLUE);
	GenerateCampFightInfo(pKernel, ctInfo, CAMP_RED);

	// 向场景内的玩家同步数据
	CVarList playerlist;
	pKernel->GetChildList(pKernel->GetScene(), TYPE_PLAYER, playerlist);
	int nCount = (int)playerlist.GetCount();
	LoopBeginCheck(c);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(c);
		PERSISTID player = playerlist.ObjectVal(i);
		IGameObj* pPlayerObj = pKernel->GetGameObj(player);
		if (NULL == pPlayerObj)
		{
			continue;
		}
		const wchar_t* wsName = pPlayerObj->QueryWideStr(FIELD_PROP_NAME);
		int nCamp = pPlayerObj->QueryInt(FIELD_PROP_CAMP);

		int nRank = QueryPlayerRank(pKernel, wsName, nCamp);

		CVarList msg;
		msg << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_FIGHT_INFO;

		msg << nRank << jhInfo << ctInfo;
		pKernel->Custom(player, msg);
	}
}

// 获取阵营当前在场景中排名第一的玩家的位置
bool CampaignAsuraBattle::FindFirstPlayerPos(IKernel* pKernel, float& fOutPosX, float& fOurPosZ, int nCamp)
{
	const SortDataVec& sortlist = nCamp == CAMP_RED ? m_vecCTSortData : m_vecJHSortData;
	if (sortlist.empty())
	{
		return false;
	}

	bool bFindPlayer = false;
	do 
	{
		int nSize = (int)sortlist.size();
		LoopBeginCheck(y);
		for (int i = 0; i < nSize;++i)
		{
			LoopDoCheck(y);
			const wchar_t* wsName = sortlist[i].wsPlayerName.c_str();
			PERSISTID player = pKernel->FindPlayer(wsName);
			IGameObj* pPlayerObj = pKernel->GetGameObj(player);
			if (pPlayerObj != NULL)
			{
				fOutPosX = pPlayerObj->GetPosiX();
				fOurPosZ = pPlayerObj->GetPosiZ();
				bFindPlayer = true;
				break;
			}
		}
	} while (0);

	return bFindPlayer;
}

// 生成同步的阵营战斗数据
void CampaignAsuraBattle::GenerateCampFightInfo(IKernel* pKernel, IVarList& outFightInfo, int nCamp)
{
	/*!
	* @brief	阵营战斗数据
	* @param	int			排名
	以下循环2次
	* @param	int			阵营类型
	* @param	int			总积分
	* @param	int			备战状态
	* @param	int			采集进度
	* @param	float x,z	积分第一玩家的位置
	*/
	//	SC_ASURA_BATTLE_FIGHT_INFO,

	outFightInfo.Clear();
	outFightInfo << nCamp;

	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}
	IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
	if (NULL == pCampFightRec)
	{
		return;
	}
	int nRowIndex = pCampFightRec->FindInt(COLUMN_CAMP_FIGHT_REC_CAMP, nCamp);
	if (-1 == nRowIndex)
	{
		return;
	}
	int nScore = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_SCORE);
	int nCollectNum = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_NUM);

	// 计算采集进度
	int nCollectState = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_STATE);
	const CampBuff* pBuffData = QueryCollectData(CAMP_COLLECT_FIGHT);
	if (NULL == pBuffData || pBuffData->nMinCollectNum <= 0)
	{
		return;
	}
	float fCollectRate = (float)nCollectNum / (float)pBuffData->nMinCollectNum * 100.0f;
	int nCollectRate = __min((int)fCollectRate, 100);

	outFightInfo << nScore << nCollectState << nCollectRate;
	// 查找积分第一玩家的位置
	float fPosX = 0.0f;
	float fPosZ = 0.0f;
	if (FindFirstPlayerPos(pKernel, fPosX, fPosZ, nCamp))
	{
		outFightInfo << fPosX << fPosZ;
	}
	else
	{
		outFightInfo << NOT_VALID_POS << NOT_VALID_POS;
	}
}

// 生成同步的排行榜数据
void CampaignAsuraBattle::GenerateFightRankNum(IKernel* pKernel, IVarList& outFightInfo, int nCamp)
{
	const SortDataVec& vecSortData = nCamp == CAMP_RED ? m_vecCTSortData : m_vecJHSortData;	  
	outFightInfo.Clear();

	int nTotalKilNum = 0;
	CVarList ranklist;

	int nSize = (int)vecSortData.size();
	int nRankNum = m_kConstData.nFightRankNum > nSize ? nSize : m_kConstData.nFightRankNum;
	LoopBeginCheck(r);
	for (int i = 0; i < nRankNum; ++i)
	{
		LoopDoCheck(r);
		const SortData& data = vecSortData[i];
		ranklist << i + 1 << data.wsPlayerName << data.nSortVal << data.nKillNum << data.nDeadNum << data.nAssistNum;

		nTotalKilNum += data.nKillNum;
	}

	outFightInfo << nTotalKilNum << nRankNum << ranklist;
}

// 活动结束
void CampaignAsuraBattle::CampaignOver(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}

	// 1.判定胜负
	int nWinCamp = GenerateResult(pKernel);

	// 2.增加发放奖励心跳,结算奖励
	ADD_COUNT_BEAT(pKernel, pSceneObj->GetObjectId(), "CampaignAsuraBattle::HB_SendReward", m_kConstData.nDelaySendRewardTime, 1);

	// 3.通告
	if (nWinCamp == BATTLE_DRAW_RESULT)
	{
		CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_114, CVarList());
	}
	else
	{
		CustomSysInfoByScene(pKernel, 0, SYSTEM_INFO_ID_115, CVarList() << GetCampTxtId(nWinCamp));
	}
 	

	// 移除同步活动数据心跳
	DELETE_HEART_BEAT(pKernel, pSceneObj->GetObjectId(), "CampaignAsuraBattle::HB_CheckBroadcastInfo");

 }

// 胜负判定
int CampaignAsuraBattle::GenerateResult(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return 0;
	}
	int nJHTotalScore = 0;
	int nCTTotalScore = 0;
	if (!GetCampFightScore(pKernel, nJHTotalScore, nCTTotalScore))
	{
		return 0;
	}

	int nWinCamp = 0;
	if (nJHTotalScore == nCTTotalScore)
	{
		nWinCamp = BATTLE_DRAW_RESULT;
	}
	else
	{
		nWinCamp = nJHTotalScore > nCTTotalScore ? CAMP_BLUE : CAMP_RED;
	}	

	pSceneObj->SetInt(FIELD_PROP_WIN_CAMP, nWinCamp);
	return nWinCamp;
}

// 读取名次奖励
bool CampaignAsuraBattle::LoadRankRewardConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Campaign/AsuraBattle/AsuraBattleReward.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecRankReward.clear();
	m_vecRankReward.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		RankReward data;

		data.nHightRank = xmlfile.ReadInteger(section, "HighRank", 0);
		data.nLowRank = xmlfile.ReadInteger(section, "LowRank", 0);
		data.strReward = xmlfile.ReadString(section, "RankReward", "");

		m_vecRankReward.push_back(data);
	}
	return true;
}

// 按照阵营发放结算奖励
void CampaignAsuraBattle::SendBattleRewardByCamp(IKernel* pKernel, int nCamp)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}
	SortDataVec& vecSort = nCamp == CAMP_BLUE ? m_vecJHSortData : m_vecCTSortData;
	m_pCampaignAsuraBattle->ExecuteRecordDataSort(pKernel, m_vecJHSortData, nCamp);

	// 胜负奖励
	int nWinCamp = pSceneObj->QueryInt(FIELD_PROP_WIN_CAMP);
	bool bIsWin = nWinCamp == nCamp || nWinCamp == BATTLE_DRAW_RESULT;
	const char* strReward = bIsWin ? m_kConstData.strWinReward.c_str() : m_kConstData.strLoseReward.c_str();

	// 结算消息
	/*!
	* @brief	活动结算
	* @param	int			江湖积分
	* @param	int			朝廷积分
	* @param	int			自己的名次
	* @param	int			自己的积分
	* @param	string		名次奖励
	* @param	string		胜负奖励
	* @param	int			前n名
	以下循环n次
	* @param	int			名次
	* @param	wstring		玩家名
	* @param	int			积分
	*/
//	SC_ASURA_BATTLE_SETTLE_INFO,
	int nJHTotalScore = 0;
	int nCTTotalScore = 0;
	if (!GetCampFightScore(pKernel, nJHTotalScore, nCTTotalScore))
	{
		return;
	}
	CVarList msghead;
	msghead << SERVER_CUSTOMMSG_ASURA_BATTLE << SC_ASURA_BATTLE_SETTLE_INFO << nJHTotalScore << nCTTotalScore;

	// 排行榜前n名数据
	int nSize = (int)vecSort.size();
	CVarList ranklist;
	int nRankNum = nSize > m_kConstData.nSettleRankNum ? m_kConstData.nSettleRankNum : nSize;
	ranklist << nRankNum;
	LoopBeginCheck(y);
	for (int i = 0; i < nRankNum;++i)
	{
		LoopDoCheck(y);
		const wchar_t* wsName = vecSort[i].wsPlayerName.c_str();
		int nRank = i + 1;
		int nScore = vecSort[i].nSortVal;

		ranklist << nRank << wsName << nScore;
	}


	// 发放排名奖励、胜负奖励及消息同步
	LoopBeginCheck(s);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(s);
		CVarList msg;
		const wchar_t* wsName = vecSort[i].wsPlayerName.c_str();
		int nRank = i + 1;
		int nScore = vecSort[i].nSortVal;

		const char* strRankReward = GetRankReward(nRank); 

		// 通告邮件发放排名奖励
// 		RewardModule::AwardEx rank_award;
// 		rank_award.name = ASURA_BATTLE_RANK_REWARD_TITLE;
// 		rank_award.srcFunctionId = FUNCTION_EVENT_ID_ASURA_BATTLE;
// 		RewardModule::ParseAwardData(strRankReward, rank_award);
// 		RewardModule::m_pRewardInstance->RewardByRoleName(pKernel, wsName, &rank_award, CVarList() << nRank);
// 
// 		// 通告邮件发放胜负奖励
// 		RewardModule::AwardEx award;
// 		award.name = bIsWin ? ASURA_BATTLE_RESULT_WIN_REWARD_TITLE : ASURA_BATTLE_RESULT_LOSE_REWARD_TITLE;
// 		award.srcFunctionId = FUNCTION_EVENT_ID_ASURA_BATTLE;
// 		RewardModule::ParseAwardData(strReward, award);
// 		RewardModule::m_pRewardInstance->RewardByRoleName(pKernel, wsName, &award);

		PERSISTID player = pKernel->FindPlayer(wsName);
		if (!pKernel->Exists(player))
		{
			continue;
		}
		// 同步结算消息
		msg << msghead << nRank << nScore << strRankReward << strReward << ranklist;
		pKernel->Custom(player, msg);
	}
}

// 获取双方阵营的总积分
bool CampaignAsuraBattle::GetCampFightScore(IKernel* pKernel, int& nOutJHScore, int& nOurCTScore)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return false;
	}
	IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
	if (NULL == pCampFightRec)
	{
		return false;
	}

	int nRowIndex = pCampFightRec->FindInt(COLUMN_CAMP_FIGHT_REC_CAMP, CAMP_BLUE);
	int nJHTotalScore = 0;
	int nCTTotalScore = 0;
	if (nRowIndex != -1)
	{
		nOutJHScore = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_SCORE);
	}
	nRowIndex = pCampFightRec->FindInt(COLUMN_CAMP_FIGHT_REC_CAMP, CAMP_RED);
	if (nRowIndex != -1)
	{
		nOurCTScore = pCampFightRec->QueryInt(nRowIndex, COLUMN_CAMP_FIGHT_REC_SCORE);
	}
	return true;
}

// 更新玩家自己的数据
void CampaignAsuraBattle::UpdatePlayerData(IKernel* pKernel, const PERSISTID& self, int nChangeScore, int nColType, int nChangeVal)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IRecord* pFightDataRec = pSelfObj->GetRecord(FIELD_RECORD_ASURA_FIGHT_REC);
	if (NULL == pFightDataRec)
	{
		return;
	}

	if (pFightDataRec->GetRows() == 0)
	{
		pFightDataRec->AddRowValue(-1, CVarList() << 0 << 0 << 0 << 0);
	}

	int nCurScore = pFightDataRec->QueryInt(0, COLUMN_ASURA_FIGHT_REC_SCORE);	
	nCurScore = __max(nCurScore + nChangeScore, 0);
	pFightDataRec->SetInt(0, COLUMN_ASURA_FIGHT_REC_SCORE, nCurScore);
	pSelfObj->SetInt(FIELD_PROP_ASURA_SCORE, nCurScore);
	if (-1 == nColType)
	{
		return;
	}

	int nCurVal = pFightDataRec->QueryInt(0, nColType);
	nCurVal = __max(nCurVal + nChangeVal, 0);
	pFightDataRec->SetInt(0, nColType, nCurVal);
}

// 获取玩家数据列
int CampaignAsuraBattle::GetPlayerColType(IKernel* pKernel, int nCampColType)
{								
	int nPlayerColType = -1;
	switch (nCampColType)
	{
	case CAMP_PLAYER_COL_KILL:
		nPlayerColType = COLUMN_ASURA_FIGHT_REC_KILL;
		break;
	case CAMP_PLAYER_COL_DEAD:
		nPlayerColType = COLUMN_ASURA_FIGHT_REC_DEAD;
		break;
	case CAMP_PLAYER_COL_ASSIST:
		nPlayerColType = COLUMN_ASURA_FIGHT_REC_ASSIST;
		break;
	}

	return nPlayerColType;
}

// 清空玩家战斗数据
void CampaignAsuraBattle::ClearPlayerFightData(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IRecord* pFightDataRec = pSelfObj->GetRecord(FIELD_RECORD_ASURA_FIGHT_REC);
	if (NULL == pFightDataRec)
	{
		return;
	}

	pFightDataRec->ClearRow();
	pSelfObj->SetInt(FIELD_PROP_ASURA_SCORE, 0);
	pSelfObj->SetInt(FIELD_PROP_ASURA_COLLECT_SCORE, 0);
}

// 查询玩家战斗数据
int CampaignAsuraBattle::QueryPlayerFightData(IKernel* pKernel, const PERSISTID& self, int nColType)
{
	int nQueryVal = 0;
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord* pFightDataRec = pSelfObj->GetRecord(FIELD_RECORD_ASURA_FIGHT_REC);
	if (NULL == pFightDataRec || pFightDataRec->GetRows() == 0)
	{
		return 0;
	}

	nQueryVal = pFightDataRec->QueryInt(0, nColType);
	return nQueryVal;
}

// 统计boss伤害数据
bool CampaignAsuraBattle::StatDamageBossData(IKernel* pKernel, const PERSISTID& self, int& nJHTotalDamVal, int& nCTTotalDamVal, int& nJHPlayerNum, int& nCTPlayerNum)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	IRecord* pBeDamageRec = pSelfObj->GetRecord(FIELD_RECORD_ASURABOSS_ATTACK_LIST_REC);
	if (NULL == pBeDamageRec)
	{
		return false;
	}
	nJHTotalDamVal = 0;
	nCTTotalDamVal = 0;
	nJHPlayerNum = 0;
	nCTPlayerNum = 0;

	int nRows = pBeDamageRec->GetRows();
	LoopBeginCheck(u);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(u);
		int nCamp = pBeDamageRec->QueryInt(i, COLUMN_ASURABOSS_ATTACK_LIST_REC_CAMP);
		int nDamVal = pBeDamageRec->QueryInt(i, COLUMN_ASURABOSS_ATTACK_LIST_REC_DAMVAL);

		if (CAMP_RED == nCamp)
		{
			nCTTotalDamVal += nDamVal;
			++nCTPlayerNum;
		}
		else
		{
			nJHTotalDamVal += nDamVal;
			++nJHPlayerNum;
		}
	}

	return true;
}

// 对统计表的数据进行排序
bool CampaignAsuraBattle::ExecuteRecordDataSort(IKernel* pKernel, SortDataVec& outVecSort, int nCamp)
{
	outVecSort.clear();
	IRecord* pRec = GetCampScoreRec(pKernel, nCamp);
	if (NULL == pRec)
	{
		return false;
	}
	int nRows = pRec->GetRows();
	if (nRows <= 0)
	{
		return false;
	}
	outVecSort.reserve(nRows);
	LoopBeginCheck(q);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(q);
		SortData data;
		data.wsPlayerName = pRec->QueryWideStr(i, CAMP_PLAYER_COL_NAME);
		data.nSortVal = pRec->QueryInt(i, CAMP_PLAYER_COL_SCORE);
		data.nKillNum = pRec->QueryInt(i, CAMP_PLAYER_COL_KILL);
		data.nDeadNum = pRec->QueryInt(i, CAMP_PLAYER_COL_DEAD);
		data.nAssistNum = pRec->QueryInt(i, CAMP_PLAYER_COL_ASSIST);

		outVecSort.push_back(data);
	}

	sort(outVecSort.begin(), outVecSort.end(), std::less<SortData>());
	return !outVecSort.empty();
}

// 根据采集状态,查找buff
const CampaignAsuraBattle::CampBuff* CampaignAsuraBattle::QueryCollectData(int nCollectState)
{
	const CampBuff* pFindData = NULL;
	int nSize = (int)m_vecCampBuff.size();
	LoopBeginCheck(f);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(f);
		const CampBuff& data = m_vecCampBuff[i];
		if (data.nCollectState == nCollectState)
		{
			pFindData = &m_vecCampBuff[i];
			break;
		}
	}

	return pFindData;
}

// 查询玩家当前的积分排名
int CampaignAsuraBattle::QueryPlayerRank(IKernel* pKernel, const wchar_t* wsName, int nCamp)
{
	const SortDataVec& sortlist = nCamp == CAMP_RED ? m_vecCTSortData : m_vecJHSortData;
	if (sortlist.empty())
	{
		return 0;
	}

	int nRank = 0;
	int nSize = (int)sortlist.size();
	LoopBeginCheck(y);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(y);
		const wchar_t* wsTempName = sortlist[i].wsPlayerName.c_str();
		if (wcscmp(wsName, wsTempName) == 0)
		{
			nRank = i + 1;
			break;
		}
	}

	return nRank;
}


// 获取名次的奖励数据
const char* CampaignAsuraBattle::GetRankReward(int nRank)
{
	const char* strReward = "";
	int nSize = (int)m_vecRankReward.size();
	LoopBeginCheck(u);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(u);
		const RankReward& data = m_vecRankReward[i];
		if (nRank >= data.nHightRank && nRank <= data.nLowRank)
		{
			strReward = data.strReward.c_str();
			break;
		}
	}

	return strReward;
}

// 清除战场数据
void CampaignAsuraBattle::ClearBattleData(IKernel* pKernel)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return;
	}
	IRecord* pCampFightRec = pSceneObj->GetRecord(FIELD_RECORD_CAMP_FIGHT_REC);
	if (pCampFightRec != NULL)
	{
		pCampFightRec->ClearRow();
	}
	IRecord* pJHScoreRec = pSceneObj->GetRecord(FIELD_RECORD_BLUE_SCORE_REC);
	if (NULL != pJHScoreRec)
	{
		pJHScoreRec->ClearRow();
	}
	IRecord* pCTScoreRec = pSceneObj->GetRecord(FIELD_RECORD_RED_SCORE_REC);
	if (NULL != pCTScoreRec)
	{
		pCTScoreRec->ClearRow();
	}

	PERSISTID boss = pSceneObj->QueryObject(FIELD_PROP_ASURA_BOSS);
	if (pKernel->Exists(boss))
	{
		pKernel->DestroySelf(boss);
	}

	pSceneObj->SetObject(FIELD_PROP_ASURA_BOSS, PERSISTID());
}

// 查询某个阵营的mvp
const wchar_t* CampaignAsuraBattle::QueryMVPPlayer(IKernel* pKernel, int nCamp)
{
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return L"";
	}

	const wchar_t* wsFindName = L"";
	const SortDataVec& vecSortData = nCamp == CAMP_RED ? m_vecCTSortData : m_vecJHSortData;
	int nSize = (int)vecSortData.size();
	LoopBeginCheck(i);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(i);
		const wchar_t* wsName = vecSortData[i].wsPlayerName.c_str();
		PERSISTID player = pKernel->FindPlayer(wsName);
		if (pKernel->Exists(player))
		{
			wsFindName = wsName;
			break;
		}
	}

	return wsFindName;
}

// 查询击杀boss增加的积分
int CampaignAsuraBattle::QueryKillBossAddScore(int nPlayerNum)
{
	int nAddScore = 0;
	int nSize = (int)m_vecBossScoreRule.size();
	LoopBeginCheck(q);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(q);
		const BossScoreRule& data = m_vecBossScoreRule[i];
		if (nPlayerNum >= data.nMinNum && nPlayerNum <= data.nMaxNum)
		{
			nAddScore = data.nAddScore;
			break;
		}
	}

	return nAddScore;
}