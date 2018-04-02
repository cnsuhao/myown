//--------------------------------------------------------------------
// 文件名:      WingModule.h
// 内  容:      翅膀模块
// 说  明:
// 创建日期:    2018年3月15日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "WingModule.h"
#include "FsGame/Define/WingDefine.h"
#include "Define/ClientCustomDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "Define/Fields.h"
#include "utils/custom_func.h"
#include "CommonModule/ContainerModule.h"
#include "Define/ContainerDefine.h"
#include "CommonModule/LogModule.h"
#include "Define/CommandDefine.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "Define/ResetTimerDefine.h"
#include "CommonModule/LuaExtModule.h"
#include "CommonModule/SwitchManagerModule.h"
#include "CapitalModule.h"
#include "CommonModule/FunctionEventModule.h"
#include "utils/exptree.h"
#include "utils/util_func.h"
#include "Define/ServerCustomDefine.h"
#include "CommonModule/EnvirValueModule.h"
#include "ResetTimerModule.h"
#include "ActivateFunctionModule.h"
#include "CommonModule/PropRefreshModule.h"
#include "Define/StaticDataDefine.h"
#include "CommonModule/CommRuleModule.h"

WingModule* WingModule::m_pWingModule = NULL;
CapitalModule* WingModule::m_pCapitalModule = NULL;
ContainerModule* WingModule::m_pContainerModule = NULL;

//初始化
bool WingModule::Init(IKernel* pKernel)
{
	m_pWingModule = this;

	m_pCapitalModule = (CapitalModule*)pKernel->GetLogicModule("CapitalModule");
	m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	Assert(NULL != m_pWingModule && NULL != m_pCapitalModule && NULL != m_pContainerModule);

	pKernel->AddEventCallback("player", "OnRecover", WingModule::OnPlayerRecover);
	pKernel->AddEventCallback("player", "OnReady", WingModule::OnPlayerReady);
	pKernel->AddEventCallback("player", "OnContinue", WingModule::OnPlayerReady);

	//升级强化等级
	pKernel->AddIntCommandHook("player", COMMAND_WING_LEVEL_UP, WingModule::OnWingLevelUp);

	pKernel->AddIntCommandHook("player", COMMAND_MSG_ACTIVATE_FUNCTION, OnCommandActivateWing);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_WING, WingModule::OnCustomWingMsg);

	if (!LoadResource(pKernel))
	{
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("WingConfig", WingModule::ReloadConfig);
#endif
	// 定义定时器
	DECL_RESET_TIMER(RESET_WING_STEP_BLESS, WingModule::ResetStepBlessVal);
	return true;
}

// 查询玩家的属性包
int WingModule::QueryWingLvPropId(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nWingLv = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL);
	
	const LevelData* pData = QueryLevelData(nWingLv);
	if (NULL == pData)
	{
		return 0;
	}
	return pData->nPropertyPak;
}

// 查询当前翅膀的默认外观
int WingModule::GetWingDefaultModel(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	// 当前的阶级和翅膀等级
	int nCurStep = pSelfObj->QueryInt(FIELD_PROP_WING_STEP);
	const StepData* pCurData = QueryStepData(nCurStep);
	if (NULL == pCurData)
	{
		return 0;
	}

	return pCurData->nWingModel;
}

// 客户端相关消息
int WingModule::OnCustomWingMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self) || args.GetCount() < 2 || args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_INT)
	{
		return 0;
	}

	int nSubMsg = args.IntVal(1);
	switch (nSubMsg)
	{
	case WING_C2S_LEVEL_UP:
		m_pWingModule->OnCustomLevelUp(pKernel, self, args);
		break;
	case WING_C2S_STEP_UP:
		m_pWingModule->OnCustomStepUp(pKernel, self, args);
		break;
	case WING_C2S_WEAR:
		m_pWingModule->OnCustomWear(pKernel, self, args);
		break;
	case WING_C2S_HIDE:
		m_pWingModule->OnCustomHide(pKernel, self, args);
		break;
	}
	return 0;
}

//回调函数 玩家加载数据完成
int WingModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	m_pWingModule->UpdateWingModifyProp(pKernel, self);

	// 定时器注册
	REGIST_RESET_TIMER(pKernel, self, RESET_WING_STEP_BLESS);
	return 0;
}

int WingModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	return 0;
}

// 响应翅膀功能激活
int WingModule::OnCommandActivateWing(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nFuncType = args.IntVal(1);
	if (nFuncType == AFM_WING_FUNCTION)
	{
		int nWingLv = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL);
		// 已经激活
		if (nWingLv > 0)
		{
			return 0;
		}
		// 设置初始等级
		pSelfObj->SetInt(FIELD_PROP_WING_LEVEL, 1);
		pSelfObj->SetInt(FIELD_PROP_WING_STEP, 1);

		const StepData* pCurData = m_pWingModule->QueryStepData(1);
		if (NULL != pCurData)
		{
			pSelfObj->SetInt(FIELD_PROP_WING, pCurData->nWingModel);
		}
	}
	return 0;
}

// 响应翅膀升级
int WingModule::OnWingLevelUp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& item, const IVarList& args)
{
	m_pWingModule->UpdateWingModifyProp(pKernel, self);
	return 0;
}

// 重置升阶祝福值
int WingModule::ResetStepBlessVal(IKernel *pKernel, const PERSISTID &self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	pSelfObj->SetInt(FIELD_PROP_WING_STEP_BLESS, 0);
	return 0;
}

// 响应翅膀升级
void WingModule::OnCustomLevelUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//翅膀功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WING, self))
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 检查被动技能升级的前置条件
	if (!CheckUpgradeTerm(pKernel, pSelfObj))
	{
		return;
	}

	int nRequestLevel = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL) + 1;
	// 检查是否满足消耗条件
	if (!LevelCostPlayerAssets(pKernel, self, nRequestLevel))
	{
		return;
	}
	
	// 更新翅膀等级
	pSelfObj->SetInt(FIELD_PROP_WING_LEVEL, nRequestLevel);
	
	// 检查是否大于当前翅膀的最大等级
	int nWingLv = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL);
	int nWingStep = pSelfObj->QueryInt(FIELD_PROP_WING_STEP);

	const StepData* pStepData = QueryStepData(nWingStep);
	if (NULL == pStepData)
	{
		return;
	}

	// 升满级了,奖励祝福值
	if (nWingLv == pStepData->nMaxWingLv)
	{
		int nBlessVal = pSelfObj->QueryInt(FIELD_PROP_WING_STEP_BLESS);
		nBlessVal = __min(nBlessVal + pStepData->nMaxLevelAddBless, MAX_BLESS_VALUE);
		pSelfObj->SetInt(FIELD_PROP_WING_STEP_BLESS, nBlessVal);
	}

	// 记录翅膀升级日志
	RoleUpgradeLog log;

	log.type = LOG_ROLE_UPGRADE_WING_LEVEL;
	log.levelBefore = nRequestLevel - 1;
	log.levelAfter = nRequestLevel;
	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, self, log);

	pKernel->Command(self, self, CVarList() << COMMAND_WING_LEVEL_UP << nRequestLevel);
	CVarList msg;
	msg << SERVER_CUSTOMMSG_WING << WING_S2C_LEVEL_UP_SUC;
	pKernel->Custom(self, msg);
}

// 响应翅膀升阶
void WingModule::OnCustomStepUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//翅膀功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WING, self))
	{
		return;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 工具容器是否为空
	PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolBox))
	{
		return;
	}
	// 当前的阶级和翅膀等级
	int nCurStep = pSelfObj->QueryInt(FIELD_PROP_WING_STEP);
	int nWingLevel = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL);

	// 升级的阶级
	int nRequestStep = nCurStep + 1;
	const StepData* pCurData = QueryStepData(nCurStep);
	const StepData* pReData = QueryStepData(nRequestStep);
	// 阶级升满了, pReData为空
	if (NULL == pReData || NULL == pCurData)
	{
		return;
	}

	// 检查翅膀等级
	if (nWingLevel < pCurData->nMaxWingLv)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19305, CVarList());
		return;
	}

	// 检查材料够不够
	if (!CommRuleModule::CanDecItems(pKernel, self, pReData->vecConsumeItem))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19304, CVarList());
		return;
	}

	//有道具，扣道具
	CommRuleModule::ConsumeItems(pKernel, self, pReData->vecConsumeItem, FUNCTION_EVENT_ID_WING);
	// 获取当前祝福值
	int nBlessVal = pSelfObj->QueryInt(FIELD_PROP_WING_STEP_BLESS);
	// 计算当前的概率
	ExpTree exp;
	int nProbability = (int)exp.CalculateEvent(pKernel, self, PERSISTID(), PERSISTID(), PERSISTID(), pReData->strProbability.c_str());
	int nRanNum = util_random_int(MAX_BLESS_VALUE);
																							
	CVarList msg;
	msg << SERVER_CUSTOMMSG_WING << WING_S2C_STEP_UP_RESULT;
	// 祝福值大于升阶阀值, 升阶成功
	if (nBlessVal >= pCurData->nStepUpBlessVal && nRanNum < nProbability)
	{
		pSelfObj->SetInt(FIELD_PROP_WING_STEP_BLESS, 0);
		pSelfObj->SetInt(FIELD_PROP_WING_STEP, nRequestStep);
		// 显示默认外观时更新高阶的翅膀外观
		int nCurWingModel = pSelfObj->QueryInt(FIELD_PROP_WING);
		if (nCurWingModel == pCurData->nWingModel)
		{
			pSelfObj->SetInt(FIELD_PROP_WING, pReData->nWingModel);
		}

		msg << WING_STEP_UP_SUC << 0;
	}
	else
	{
		int nAddBlessVal = pReData->nMinBless + util_random_int(pReData->nMaxBless - pReData->nMinBless);
		int nOldBlessVal = pSelfObj->QueryInt(FIELD_PROP_WING_STEP_BLESS);
		int nCurBlessVal = __min(nAddBlessVal + nOldBlessVal, MAX_BLESS_VALUE);
		pSelfObj->SetInt(FIELD_PROP_WING_STEP_BLESS, nCurBlessVal);

		msg << WING_STEP_UP_FAILED << nCurBlessVal - nOldBlessVal;
	}

	pKernel->Custom(self, msg);

	// 记录翅膀升阶日志
	RoleUpgradeLog log;

// 	log.type = LOG_ROLE_UPGRADE_WING_STEP;
// 	log.levelBefore = nRequestLevel - 1;
// 	log.levelAfter = nRequestLevel;
// 	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, self, log);
}

// 响应开启翅膀
void WingModule::OnCustomWear(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//翅膀功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WING, self))
	{
		return;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

// 	int nStep = pSelfObj->QueryInt(FIELD_PROP_WING_STEP);
// 	const StepData* pStepData = QueryStepData(nStep);
// 	if (NULL == pStepData)
// 	{
// 		return;
// 	}

	// 设置翅膀外观
	pSelfObj->SetInt(FIELD_PROP_IS_WEAR_WING, WEAR_WING);

	// 开始起飞
}

// 响应隐藏翅膀
void WingModule::OnCustomHide(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//翅膀功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_WING, self))
	{
		return;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	pSelfObj->SetInt(FIELD_PROP_IS_WEAR_WING, NO_WEAR_WING);

	// 落地
}

// 读取配置
bool WingModule::LoadResource(IKernel* pKernel)
{
	// 读取翅膀升级配置
	if (!LoadLevelResource(pKernel))
	{
		return false;
	}

	// 读取翅膀升阶配置
	if (!LoadStepResource(pKernel))
	{
		return false;
	}
	return true;
}

// 读取升级配置
bool WingModule::LoadLevelResource(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/SystemFunction/Wing/WingLevelConfig.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecLevelData.clear();
	m_vecLevelData.reserve(nSectionCount);

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);
		int nSection = StringUtil::StringAsInt(section);

		LevelData data;
		const char* strCapital = xmlfile.ReadString(section, "ConsumeCapital", "");
		CommRuleModule::ParseConsumeVec(strCapital, data.vecConsumeCapital);

		const char* strItems = xmlfile.ReadString(section, "ConsumeItem", "");
		CommRuleModule::ParseConsumeVec(strItems, data.vecConsumeItem);

		data.nPropertyPak = xmlfile.ReadInteger(section, "PropertyPak", 0);
		m_vecLevelData.push_back(data);
	}

	return true;
}

// 读取升阶配置
bool WingModule::LoadStepResource(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/SystemFunction/Wing/WingStepConfig.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecStepData.clear();
	m_vecStepData.reserve(nSectionCount);

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);
		int nSection = StringUtil::StringAsInt(section);

		StepData data;
		const char* strCostItem = xmlfile.ReadString(section, "CostItem", "");
		CommRuleModule::ParseConsumeVec(strCostItem, data.vecConsumeItem);

		data.strProbability = xmlfile.ReadString(section, "Probability", "");
		data.nMinBless = xmlfile.ReadInteger(section, "MinBless", 0);
		data.nMaxBless = xmlfile.ReadInteger(section, "MaxBless", 0);
		data.nMaxWingLv = xmlfile.ReadInteger(section, "MaxWingLevel", 0);
		data.nStepUpBlessVal = xmlfile.ReadInteger(section, "StepUpBlessVal", 0);
		data.nWingModel = xmlfile.ReadInteger(section, "WingModel", 0);
		data.nMaxLevelAddBless = xmlfile.ReadInteger(section, "MaxLevelAddBless", 0);

		m_vecStepData.push_back(data);
	}

	return true;
}

// 检查升级翅膀的条件
bool WingModule::CheckUpgradeTerm(IKernel* pKernel, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 检查是否大于当前翅膀的最大等级
	int nWingLv = pSelfObj->QueryInt(FIELD_PROP_WING_LEVEL);
	int nWingStep = pSelfObj->QueryInt(FIELD_PROP_WING_STEP);

	const StepData* pStepData = QueryStepData(nWingStep);
	if (NULL == pStepData)
	{
		return false;
	}
	
	// 翅膀级别已到最大级别
	if (nWingLv >= pStepData->nMaxWingLv)
	{
		::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), SYSTEM_INFO_ID_19301, CVarList());
		return false;
	}

	return true;
}

// 消耗翅膀升级的物品和金钱
bool WingModule::LevelCostPlayerAssets(IKernel* pKernel, const PERSISTID& self, int nLevel)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	const LevelData* pData = QueryLevelData(nLevel);
	// 工具容器是否为空
	PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolBox) || NULL == pData)
	{
		return false;
	}

	// 检查钱、材料够不够
	if (!CommRuleModule::CanDecCapitals(pKernel, self, pData->vecConsumeCapital))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19302, CVarList());
		return false;
	}
	
	if (!CommRuleModule::CanDecItems(pKernel, self, pData->vecConsumeItem))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19303, CVarList());
		return false;
	}

	// 开始扣钱、道具
	CommRuleModule::ConsumeCapitals(pKernel, self, pData->vecConsumeCapital, FUNCTION_EVENT_ID_WING);

	//有道具，扣道具
	CommRuleModule::ConsumeItems(pKernel, self, pData->vecConsumeItem, FUNCTION_EVENT_ID_WING);
	return true;
}

// 更新玩家翅膀属性
bool WingModule::UpdateWingModifyProp(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	int nMeridianLevelPropId = WingModule::m_pWingModule->QueryWingLvPropId(pKernel, self);
	PropRefreshModule::UpdateModifyProp(pKernel, self, STATIC_DATA_WING_ATTRIBUTE_PACK, WING_PROPPAK_ID, nMeridianLevelPropId);
	return true;
}

// 查询升阶数据
const WingModule::StepData* WingModule::QueryStepData(int nStep)
{
	// 索引从0 计数
	int nIndex = --nStep;
	int nSize = m_vecStepData.size();
	if (nIndex < 0 || nIndex >= nSize)
	{
		return NULL;
	}

	return &m_vecStepData[nIndex];
}

// 查询升级数据
const WingModule::LevelData* WingModule::QueryLevelData(int nLevel)
{
	// 索引从0 计数
	int nIndex = --nLevel;
	int nSize = m_vecLevelData.size();
	if (nIndex < 0 || nIndex >= nSize)
	{
		return NULL;
	}

	return &m_vecLevelData[nIndex];
}

void WingModule::ReloadConfig(IKernel* pKernel)
{
	m_pWingModule->LoadResource(pKernel);
}