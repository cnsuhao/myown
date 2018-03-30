//--------------------------------------------------------------------
// 文件名:      BattleAbilityModule.h
// 内  容:      战斗力统计
// 说  明:
// 创建日期:    2017/04/12
// 创建人:      刘明飞
//    :       
//--------------------------------------------------------------------

#include "BattleAbilityModule.h"
#include "FsGame/Define/BattleAbilityDefine.h"
#include "Define/GameDefine.h"
#include "Define/CommandDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include <algorithm>
#include "Define/Fields.h"
#include "SkillModule/SkillToSprite.h"
#include "utils/exptree.h"
#include "SkillModule/PassiveSkillSystem.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "Define/InternalFields.h"
#include "Define/ModifyPackDefine.h"

BattleAbilityModule* BattleAbilityModule::m_pInstance = NULL;

//#define BATTLE_ABILITY_RATE_CONFIG_PATH		"ini/rule/battle_ability/role_battle_ability_rate.xml"
#define BATTLE_ABILITY_FORMULA_CONFIG_PATH	"ini/SystemFunction/BattleAbility/RoleBattleAbilityFormula.xml"
#define BATTLE_ABILITY_VALUE_CONFIG_PATH	"ini/SystemFunction/BattleAbility/RoleBattleAbilityValue.xml"


//初始化
bool BattleAbilityModule::Init(IKernel* pKernel)
{
	m_pInstance = this;

	pKernel->AddEventCallback("player", "OnRecover", BattleAbilityModule::OnPlayerRecover, 1000);
	pKernel->AddEventCallback("player", "OnContinue", BattleAbilityModule::OnPlayerRecover, 1000);
	pKernel->AddIntCommandHook("player", COMMAND_MSG_UPDATE_BATTLE_ABILITY, BattleAbilityModule::OnCommandUpdateBattleAbility);

	if (!LoadConfig(pKernel))
	{
		return false;
	}

	RELOAD_CONFIG_REG("BattleAbilityModule", BattleAbilityModule::ReloadConfig);
	return true;
}

//释放
bool BattleAbilityModule::Shut(IKernel* pKernel)
{
	return true;
}

// 读取配置
bool BattleAbilityModule::LoadConfig(IKernel* pKernel)
{
	if (!LoadValueBAConfig(pKernel))
	{
		return false;
	}

// 	if (!LoadRateBAConfig(pKernel))
// 	{
// 		return false;
// 	}

	if (!LoadFormulaBAConfig(pKernel))
	{
		return false;
	}
	return true;
}

// 玩家上线
int BattleAbilityModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	m_pInstance->UpdatePropertyBattleAbility(pKernel, pSelfObj);
	m_pInstance->UpdateSkillBattleAbility(pKernel, pSelfObj);
	m_pInstance->UpdatePassiveSkillBattleAbility(pKernel, pSelfObj);

	m_pInstance->StatisticsTotalBattleAbility(pKernel, self, pSelfObj);
	return 0;
}

// 响应更新战斗力
int BattleAbilityModule::OnCommandUpdateBattleAbility(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nOnline = pSelfObj->QueryInt(FIELD_PROP_ONLINE);
	if (nOnline <= 0)
	{
		return 0;
	}

	int nUpdateType = args.IntVal(1);

	switch (nUpdateType)
	{
	case BA_PROPERTY_TYPE:
		m_pInstance->UpdatePropertyBattleAbility(pKernel, pSelfObj);
		break;
	case BA_SKILL_TYPE:
		m_pInstance->UpdateSkillBattleAbility(pKernel, pSelfObj);
		break;
	case BA_PASSIVE_SKILL_TYPE:
		m_pInstance->UpdatePassiveSkillBattleAbility(pKernel, pSelfObj);
		break;
	default:
		break;
	}

	m_pInstance->StatisticsTotalBattleAbility(pKernel, self, pSelfObj);
	return 0;
}

void BattleAbilityModule::ReloadConfig(IKernel* pKernel)
{

}

// 读取属性值战力数据
bool BattleAbilityModule::LoadValueBAConfig(IKernel* pKernel)
{
	std::string strFileName = pKernel->GetResourcePath();
	strFileName += BATTLE_ABILITY_VALUE_CONFIG_PATH;
	CXmlFile xml(strFileName.c_str());
	if (!xml.LoadFromFile())
	{
		::extend_warning(LOG_ERROR, "[BattleAbilityModule::LoadValueBAConfig] not found file:%s.", strFileName.c_str());
		return false;
	}

	m_vecValueBAData.clear();

	CVarList vSectionList;
	int nSectionCount = xml.GetSectionCount();
	m_vecValueBAData.reserve(nSectionCount);

	LoopBeginCheck(a);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(a);
		const char* strSection = xml.GetSectionByIndex(i);

		ValueBAData data;
		data.strProName = strSection;
		LoopBeginCheck(b);
		for (int j = PLAYER_JOB_WARRIOR; j < PLAYER_JOB_MAX; ++j)
		{
			LoopDoCheck(b);
			char strKey[32] = { 0 };
			SPRINTF_S(strKey, "Rate%d", j);
			data.arrRate[j] = xml.ReadFloat(strSection, strKey, 0.0f);
		}

		m_vecValueBAData.push_back(data);
	}
	return true;
}

// 读取百分比战力数据
// bool BattleAbilityModule::LoadRateBAConfig(IKernel* pKernel)
// {
// 	std::string strFileName = pKernel->GetResourcePath();
// 	strFileName += BATTLE_ABILITY_RATE_CONFIG_PATH;
// 	CXmlFile xml(strFileName.c_str());
// 	if (!xml.LoadFromFile())
// 	{
// 		::extend_warning(LOG_ERROR, "[BattleAbilityModule::LoadRateBAConfig] not found file:%s.", strFileName.c_str());
// 		return false;
// 	}
// 
// 	m_vecRateBAData.clear();
// 	int nSectionCount = xml.GetSectionCount();
// 	int nKeyNum = xml.GetItemCount("1");
// 
// 	m_vecRateBAData.reserve(nKeyNum);
// 	LoopBeginCheck(c);
// 	for (int i = 0; i < nSectionCount; ++i)
// 	{
// 		LoopDoCheck(c);
// 		const char* strSection = xml.GetSectionByIndex(i);
// 
// 		CVarList itemlist;
// 		xml.GetItemList(strSection, itemlist);
// 		int nCount = (int)itemlist.GetCount();
// 		LoopBeginCheck(d)
// 		for (int j = 0; j < nCount;++j)
// 		{
// 			LoopDoCheck(d);
// 			const char* strProName = itemlist.StringVal(j);
// 			float fRate = xml.ReadFloat(strSection, strProName, 0.0f);
// 
// 			ParseRateBAData(strProName, fRate);
// 		}
// 	}
// 	return true;
// }

// 读取公式战力数据
bool BattleAbilityModule::LoadFormulaBAConfig(IKernel* pKernel)
{
	std::string strFileName = pKernel->GetResourcePath();
	strFileName += BATTLE_ABILITY_FORMULA_CONFIG_PATH;
	CXmlFile xml(strFileName.c_str());
	if (!xml.LoadFromFile())
	{
		::extend_warning(LOG_ERROR, "[BattleAbilityModule::LoadFormulaBAConfig] not found file:%s.", strFileName.c_str());
		return false;
	}

	m_mapFormulaBAData.clear();

	CVarList vSectionList;
	int nSectionCount = xml.GetSectionCount();

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* strSection = xml.GetSectionByIndex(i);

		FormulaBADataMap::iterator iter = m_mapFormulaBAData.find(strSection);
		if (iter == m_mapFormulaBAData.end())
		{
			FormulaBAData data;
			data.strFormulaId = strSection;
			data.nCalType = xml.ReadInteger(strSection, "CalType", 0);
			data.strFormula = xml.ReadString(strSection, "CalFormula", "");

			m_mapFormulaBAData.insert(std::make_pair(strSection, data));
		}
	}
	return true;
}

// 更新属性战斗力
void BattleAbilityModule::UpdatePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return;
	}

	// 统计值属性战力
	float fTotalBA = 0;
	int nSize = (int)m_vecValueBAData.size();
	LoopBeginCheck(a);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(a);
		float fProBA = CalValuePropertyBattleAbility(pKernel, pSelfObj, m_vecValueBAData[i]);
		fTotalBA += fProBA;
	}

	// 统计百分比属性战力
// 	nSize = (int)m_vecRateBAData.size();
// 	LoopBeginCheck(b);
// 	for (int i = 0; i < nSize; ++i)
// 	{
// 		LoopDoCheck(b);
// 		float fProBA = CalRatePropertyBattleAbility(pKernel, pSelfObj, m_vecRateBAData[i]);
// 		fTotalBA += fProBA;
// 	}

	// 去掉buff或临时属性增加的战力
	IRecord* pPropModifyRec = pSelfObj->GetRecord(CONST_PROP_MODIFY_REC);
	if (NULL == pPropModifyRec)
	{
		return;
	}
	int nRows = pPropModifyRec->GetRows();
	float fBuffBA = 0.0f;
	LoopBeginCheck(w);
	for (int i = 0; i < nRows;++i)
	{
		LoopDoCheck(w);
		const char* strPropName = pPropModifyRec->QueryString(i, PMPS_PROP_NAME);
		float fPropVal = pPropModifyRec->QueryFloat(i, PMPS_PROP_VALUE);
		float fBARate = QueryPropertyBARate(pSelfObj, strPropName);

		fBuffBA += fPropVal * fBARate;
	}

	fTotalBA -= fBuffBA;

	RefreshOneTypeBAValue(pSelfObj, BA_PROPERTY_TYPE, fTotalBA);
}

// 更新主动技能战斗力
void BattleAbilityModule::UpdateSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj)
{
	 if (NULL == pSelfObj)
	 {
		 return;
	 }

	 PERSISTID skillcontainer = SKillToSpriteSingleton::Instance()->GetSkillContainer(pKernel, pSelfObj->GetObjectId());
	 if (!pKernel->Exists(skillcontainer))
	 {
		 return;
	 }

	 float fTotalBA = 0.0f;
	 // 获得第一个子对象
	 unsigned int it;
	 PERSISTID skill = pKernel->GetFirst(skillcontainer, it);
	 LoopBeginCheck(c)
	 while (pKernel->Exists(skill))
	 {
		 LoopDoCheck(c)
		 float fBAValue = CalSkillBattleAbility(pKernel, pSelfObj, skill);
		 fTotalBA += fBAValue;

		 skill = pKernel->GetNext(skillcontainer, it);
	 }

	 RefreshOneTypeBAValue(pSelfObj, BA_SKILL_TYPE, fTotalBA);
}

// 更新被动技能战斗力
void BattleAbilityModule::UpdatePassiveSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return;
	}

	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return;
	}

	float fTotalBA = 0.0f;
	int nRows = pPassiveSkillRec->GetRows();
	LoopBeginCheck(w);
	for (int i = 0; i < nRows;++i)
	{
		LoopDoCheck(w);
		int nSkillId = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_ID);

		float fBAValue = CalPassiveSkillBattleAbility(pKernel, pSelfObj, nSkillId);
		fTotalBA += fBAValue;
	}

	RefreshOneTypeBAValue(pSelfObj, BA_PASSIVE_SKILL_TYPE, fTotalBA);
}

// 计算某一个玩家属性增加的战力
float BattleAbilityModule::CalValuePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const ValueBAData& data)
{
	if (NULL == pSelfObj)
	{
		return 0.0f;
	}

	int nJob = pSelfObj->QueryInt(FIELD_PROP_JOB);
	if (nJob <= PLAYER_JOB_MIN || nJob >= PLAYER_JOB_MAX)
	{
		return 0.0f;
	}

	float fBattleAbility = 0.0f;
	do 
	{
		const char* strProName = data.strProName.c_str();
		if (!pSelfObj->FindAttr(strProName))
		{
			break;
		}

		float fProValue = QueryProperty(pSelfObj, strProName);
		fBattleAbility = fProValue * data.arrRate[nJob];
	} while (0);
	
	return fBattleAbility;
}

// 计算某一个玩家百分比属性增加的战力
float BattleAbilityModule::CalRatePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const RateBAData& data)
{
	if (NULL == pSelfObj)
	{
		return 0.0f;
	}

	int nLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	--nLevel; // 数组的索引从0计数
	int nSize = (int)data.vecLevelRate.size();
	if (nLevel < 0 || nLevel >= nSize)
	{
		return 0.0f;
	}

	float fBattleAbility = 0.0f;
	do
	{
		const char* strProName = data.strProName.c_str();
		if (!pSelfObj->FindAttr(strProName))
		{
			break;
		}

		float fProValue = QueryProperty(pSelfObj, strProName);
		fBattleAbility = fProValue * data.vecLevelRate[nLevel];
	} while (0);

	return fBattleAbility;
}

// 计算某个主动技能增加的战力
float BattleAbilityModule::CalSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skill)
{
	if (!pKernel->Exists(skill) || NULL == pSelfObj)
	{
		return 0.0f;
	}

	float fBattleAbility = 0.0f;
	do 
	{
		const char* strSkillId = pKernel->GetConfig(skill);
		FormulaBADataMap::iterator iter = m_mapFormulaBAData.find(strSkillId);
		// 此技能不需要统计战力
		if (iter == m_mapFormulaBAData.end())
		{
			break;
		}

		fBattleAbility = CalFormulaBattleAbility(pKernel, pSelfObj, skill, iter->second);
	} while (0);

	return fBattleAbility;
}

// 计算某个被动技能增加的战力
float BattleAbilityModule::CalPassiveSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, int nPassiveSkillId)
{
	if (NULL == pSelfObj || 0 == nPassiveSkillId)
	{
		return 0.0f;
	}

	std::string strSkillId = StringUtil::IntAsString(nPassiveSkillId);
	FormulaBADataMap::iterator iter = m_mapFormulaBAData.find(strSkillId);
	// 此技能不需要统计战力
	if (iter == m_mapFormulaBAData.end())
	{
		return 0.0f;
	}

	return CalFormulaBattleAbility(pKernel, pSelfObj, PERSISTID(), iter->second);;
}

// 统计总战斗力
void BattleAbilityModule::StatisticsTotalBattleAbility(IKernel* pKernel, const PERSISTID& self, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return;
	}

	IRecord* pBattleAbilityRec = pSelfObj->GetRecord(BATTLLE_ABILITY_REC);
	if (NULL == pBattleAbilityRec)
	{
		return;
	}

	float fTotalBA = 0.0f;
	int nRows = pBattleAbilityRec->GetRows();
	LoopBeginCheck(r);
	for (int i = 0; i < nRows;++i)
	{
		LoopDoCheck(r);
		float fBAValue = pBattleAbilityRec->QueryFloat(i, BAR_COL_VALUE);

		fTotalBA += fBAValue;
	}

	int nTotalBA = (int)fTotalBA;
	pSelfObj->SetInt(FIELD_PROP_BATTLE_ABILITY, nTotalBA);

	int nHighestBA = pSelfObj->QueryInt(FIELD_PROP_HIGHEST_BATTLE_ABILITY);
	if (nTotalBA > nHighestBA)
	{
		pSelfObj->SetInt(FIELD_PROP_HIGHEST_BATTLE_ABILITY, nTotalBA);
	}
	//MiddleModule::Instance()->OnAchievementCondition(pKernel, self, EAchievementDstCond_RoleFight, nTotalBA);
}

// 增加一条百分比配置
// void BattleAbilityModule::ParseRateBAData(const char* strProName, float fRate)
// {
// 	if (StringUtil::CharIsNull(strProName) || fRate <= 0.0f)
// 	{
// 		return;
// 	}
// 
// 	RateBAData data;
// 	data.strProName = strProName;
// 	RateBADataVec::iterator iter = find(m_vecRateBAData.begin(), m_vecRateBAData.end(), data);
// 	if (iter != m_vecRateBAData.end())
// 	{
// 		iter->vecLevelRate.push_back(fRate);
// 	}
// 	else
// 	{
// 		data.vecLevelRate.reserve(100);
// 		data.vecLevelRate.push_back(fRate);
// 
// 		m_vecRateBAData.push_back(data);
// 	}
// }	

// 查询属性值
float BattleAbilityModule::QueryProperty(IGameObj* pSelfObj, const char* strProName)
{
	float fValue = 0.0f;
	int nValType = pSelfObj->GetAttrType(strProName);
	switch (nValType)
	{
	case VTYPE_INT:
		fValue = (float)pSelfObj->QueryInt(strProName);
		break;
	case VTYPE_INT64:
		fValue = (float)pSelfObj->QueryInt64(strProName);
		break;
	case VTYPE_FLOAT:
		fValue = (float)pSelfObj->QueryFloat(strProName);
		break;
	}

	return fValue;
}

// 更新某个类型的战力值
void BattleAbilityModule::RefreshOneTypeBAValue(IGameObj* pSelfObj, int nBAType, float fBAValue)
{
	if (NULL == pSelfObj || nBAType < BA_PROPERTY_TYPE || nBAType >= MAX_BATTLE_ABILITY_TYPE)
	{
		return;
	}

	IRecord* pBattleAbilityRec = pSelfObj->GetRecord(BATTLLE_ABILITY_REC);
	if (NULL == pBattleAbilityRec)
	{
		return;
	}

	int nRowIndex = pBattleAbilityRec->FindInt(BAR_COL_TYPE, nBAType);
	if (-1 == nRowIndex)
	{
		pBattleAbilityRec->AddRowValue(-1, CVarList() << nBAType << fBAValue);
	}
	else
	{
		pBattleAbilityRec->SetFloat(nRowIndex, BAR_COL_VALUE, fBAValue);
	}
}

// 计算公式战斗力
float BattleAbilityModule::CalFormulaBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skill, const FormulaBAData& data)
{
	if (NULL == pSelfObj)
	{
		return 0.0f;
	}

	int nCalType = data.nCalType;

	float fBAValue = 0.0f;
	ExpTree exp;
	if (BFC_SKILL_CAL == nCalType)
	{
		fBAValue = exp.CalculateEvent(pKernel, PERSISTID(), skill, PERSISTID(), PERSISTID(), data.strFormula.c_str());
	}
	else if (BFC_PASSIVE_SKILL_CAL == nCalType)
	{
		int nPassiveSkillId = StringUtil::StringAsInt(data.strFormulaId.c_str());
		int nSkillLevel = PassiveSkillSystemSingleton::Instance()->QueryPassiveSkillLevel(pKernel, pSelfObj->GetObjectId(), nPassiveSkillId);

		float fArgs[MAX_PARAM] = { 0.0f };
		fArgs[0] = (float)nSkillLevel;
		if (nSkillLevel > 0)
		{
			fBAValue = exp.CalculateParam(data.strFormula.c_str(), fArgs);
		}
	}

	return fBAValue;
}

// 查询某个属性的战力系数
float BattleAbilityModule::QueryPropertyBARate(IGameObj* pSelfObj, const char* strProName)
{
	int nJob = pSelfObj->QueryInt(FIELD_PROP_JOB);
	if (nJob <= PLAYER_JOB_MIN || nJob >= PLAYER_JOB_MAX)
	{
		return 0.0f;
	}
	float fRate = 0.0f;
	int nSize = (int)m_vecValueBAData.size();
	LoopBeginCheck(s);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(s);
		const ValueBAData& data = m_vecValueBAData[i];
		if (strcmp(data.strProName.c_str(), strProName) == 0)
		{
			fRate = data.arrRate[nJob];
			break;
		}
	}

	// 说明已经找到
// 	if (fRate > 0.0f)
// 	{
// 		return fRate;
// 	}
// 
// 	int nLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
// 	nSize = (int)m_vecRateBAData.size();
// 	LoopBeginCheck(g);
// 	for (int i = 0; i < nSize; ++i)
// 	{
// 		LoopDoCheck(g);
// 		const RateBAData& data = m_vecRateBAData[i];
// 		if (strcmp(data.strProName.c_str(), strProName) == 0)
// 		{
// 			--nLevel;
// 			if (nLevel >= 0 && nLevel < (int)data.vecLevelRate.size())
// 			{													 
// 				fRate = data.vecLevelRate[nLevel];
// 				break;
// 			}
// 		}
// 	}

	return fRate;
}