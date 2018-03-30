//----------------------------------------------------------
// 文件名:      PassiveSkillUpgrade.h
// 内  容:      被动技能养成系统
// 说  明:		
// 创建日期:    2015年6月24日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------

#include "PassiveSkillSystem.h"
#include "..\Define\ClientCustomDefine.h"
#include "utils\XmlFile.h"
#include "utils\extend_func.h"
#include "..\Define\LogDefine.h"
#include "Impl\SkillDataQueryModule.h"
#include "..\CommonModule\LogModule.h"
#include "utils\string_util.h"
#include "..\SystemFunctionModule\CapitalModule.h"
#include "..\CommonModule\ContainerModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "..\SocialSystemModule\SystemMailModule.h"
#endif
#include "..\Define\CommandDefine.h"
#include "..\CommonModule\LuaExtModule.h"
#include "utils\custom_func.h"
//#include "..\Define\SkillUpgradeSysInfoDefine.h"
#include "..\CommonModule\PropRefreshModule.h"
#include "..\Define\ModifyPackDefine.h"
#include "..\Interface\FightInterface.h"
#include "FsGame\CommonModule\SwitchManagerModule.h"
#include "..\Define\Skilldefine.h"
#include "..\Define\ServerCustomDefine.h"
#include "PassiveSkillModifyPack.h"
//#include "SkillUpgradeModule.h"
#include "FsGame/Define/Fields.h"
#include "FsGame/CommonModule\FunctionEventModule.h"
//#include "Define\BattleAbilityDefine.h"

SkillDataQueryModule*	PassiveSkillSystem::m_pSkillDataQueryModule	= NULL;
ContainerModule*		PassiveSkillSystem::m_pContainerModule			= NULL;
CapitalModule*			PassiveSkillSystem::m_pCapitalModule			= NULL;

#ifndef FSROOMLOGIC_EXPORTS
SystemMailModule*		PassiveSkillSystem::m_pSystemMailModule		= NULL;
#endif // FSROOMLOGIC_EXPORTS

int nx_passive_skill_level_up(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_passive_skill_level_up, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_passive_skill_level_up, 1);
	CHECK_ARG_INT(state, nx_passive_skill_level_up, 2);
	CHECK_ARG_INT(state, nx_passive_skill_level_up, 3);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);
	int nSkill = pKernel->LuaToInt(state, 2);
	int nSkillLv = pKernel->LuaToInt(state, 3);

	PassiveSkillSystemSingleton::Instance()->UpdatePassiveSkill(pKernel, self, nSkill, nSkillLv, PSS_BY_SOUL);
	//PassiveSkillSystem::OnCustomOnPassiveSkillUpgrade(pKernel, self, PERSISTID(), CVarList() << CLIENT_CUSTOMMSG_PASSIVE_SKILL_UPGRADE << nSkill);
	return 0;
}

int nx_passive_skill_reset(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_passive_skill_reset, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_passive_skill_reset, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	PassiveSkillSystem::OnCustomResetPassiveSkill(pKernel, self, PERSISTID(), CVarList() << CLIENT_CUSTOMMSG_RESET_PASSIVE_SKILL);
	return 0;
}

int nx_learn_all_passive_skill(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, learn_all_passive_skill, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, learn_all_passive_skill, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	PassiveSkillSystemSingleton::Instance()->LearnAllPassiveSkill(pKernel, self);
	return 0;
}

// 初始化
bool PassiveSkillSystem::Init(IKernel* pKernel)
{
	pKernel->AddEventCallback("player", "OnRecover", PassiveSkillSystem::OnPlayerRecover, 100);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_PASSIVE_SKILL_UPGRADE, PassiveSkillSystem::OnCustomOnPassiveSkillUpgrade);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_RESET_PASSIVE_SKILL, PassiveSkillSystem::OnCustomResetPassiveSkill);

	m_pSkillDataQueryModule = (SkillDataQueryModule*)pKernel->GetLogicModule("SkillDataQueryModule");
	m_pContainerModule		= (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pCapitalModule		= (CapitalModule*)pKernel->GetLogicModule("CapitalModule");
	
	Assert(NULL != m_pSkillDataQueryModule && NULL != m_pContainerModule && NULL != m_pCapitalModule);

#ifndef FSROOMLOGIC_EXPORTS
	m_pSystemMailModule		= (SystemMailModule*)pKernel->GetLogicModule("SystemMailModule");
	Assert(NULL != m_pSystemMailModule);
#endif // FSROOMLOGIC_EXPORTS

	// 被动技能属性包逻辑
	if(!PassiveSkillModifyPackSingleton::Instance()->Init(pKernel))
	{
		return false;
	}

	// gm命令
	DECL_LUA_EXT(nx_passive_skill_level_up);
	DECL_LUA_EXT(nx_passive_skill_reset);
	DECL_LUA_EXT(nx_learn_all_passive_skill);
	return true;
}

/*!
 * @brief	增加或更新被动技能 宠物被动技能用到
 * @param	self 对象号(可能是宠物也可能是玩家)
 * @param	nSkillId 技能id
 * @param	nSkillLevel 技能等级
 * @param	nSkillSrc  技能来源
 * @return	bool
 */
bool PassiveSkillSystem::UpdatePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId, int nSkillLevel, int nSkillSrc)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return false;
	}
	// 是否有这个被动技能
	int nRows = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, nSkillId);
	// 没有的话
	if (-1 == nRows)
	{
		// 增加新的被动技能
		pPassiveSkillRec->AddRowValue(nRows, CVarList() << nSkillId << nSkillLevel << nSkillSrc);
	}
	else
	{
		// 刷新被动技能等级
		pPassiveSkillRec->SetInt(nRows, PS_COL_SKILL_LEVEL, nSkillLevel);
	}

	const PassiveSkillLevelInfo* pSkillInfo = m_pSkillDataQueryModule->GetPassiveSkillLevelInfo(nSkillId, nSkillLevel);
	if (NULL == pSkillInfo)
	{
		return false;
	}

	OnPassiveSkillChange(pKernel, self, nSkillId, pSkillInfo);
	return true;
}

/*!
 * @brief	移除被动技能
 * @param	self 对象号(可能是宠物也可能是玩家)
 * @param	nSkillId技能id
 * @return	bool
 */
bool PassiveSkillSystem::RemovePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return false;
	}
	// 是否有这个被动技能
	int nRows = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, nSkillId);
	// 没有的话
	if (-1 == nRows)
	{
		return false;
	}
	int nSkillLevel = pPassiveSkillRec->QueryInt(nRows, PS_COL_SKILL_LEVEL);
	int nSkillSrc = pPassiveSkillRec->QueryInt(nRows, PS_COL_SKILL_SRC);
	// 合理保护
	if (TYPE_PLAYER  == pSelfObj->GetClassType() && PSS_BY_STUDY == nSkillSrc)
	{
		return false;
	}
	pPassiveSkillRec->RemoveRow(nRows);
	OnRemovePassiveSkill(pKernel, self, nSkillId, nSkillLevel);
	return true;
}
// 查找被动技能等级,没找到返回0
int PassiveSkillSystem::QueryPassiveSkillLevel(IKernel* pKernel, const PERSISTID &self, int nSkillId)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return 0;
	}
	// 没找到技能
	int nRows = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, nSkillId);
	if (-1 == nRows)
	{ 
		return 0;
	}

	return pPassiveSkillRec->QueryInt(nRows, PS_COL_SKILL_LEVEL);
}

// 响应被动技能升级消息(记日志)
int PassiveSkillSystem::OnCustomOnPassiveSkillUpgrade(IKernel* pKernel, const PERSISTID &self,
	const PERSISTID & sender, const IVarList & args)
{
	if (args.GetCount() != 2 || args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_INT)
	{
		return 0;
	}

#ifndef FSROOMLOGIC_EXPORTS
	//被动技能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_PASSIVE_SKILL, self))
	{
		return 0;
	}
#endif // FSROOMLOGIC_EXPORTS

	// 升级的技能id
	int nSkillId = args.IntVal(1);
	
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	int nSelfJob = pSelfObj->QueryInt("Job");
	const PassiveSkill* pSkillData = m_pSkillDataQueryModule->GetPassiveSkillConfig(nSkillId);
	// 合理保护
	if (NULL == pSkillData || pSkillData->nCareer != nSelfJob)
	{
		return 0;
	}

	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return 0;
	}

	// 是否学过的被动技能
	int nRows = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, nSkillId);

	// 请求升的技能级别
	int nRequestLevel = nRows != -1 ? pPassiveSkillRec->QueryInt(nRows, PS_COL_SKILL_LEVEL) + 1 : 1;

	// 获取当前技能的最高等级
// 	int nMaxLevel = (int)pSkillData->vUpgradeInfoList.size();
// 	if (nRequestLevel > nMaxLevel)
// 	{
// 		::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_UPGRADE_FAILED_MAX_LEVEL, CVarList());
// 		return 0;
// 	}
	// 获取当前技能等级的配置
	const PassiveSkillLevelInfo* pSkillInfo = m_pSkillDataQueryModule->GetPassiveSkillLevelInfo(nSkillId, nRequestLevel);
	if (NULL == pSkillInfo)
	{
		return 0;
	}

	// 检查被动技能升级的前置条件
	if (!PassiveSkillSystemSingleton::Instance()->CheckUpgradeTerm(pKernel, pSelfObj, pSkillData, pSkillInfo, nRequestLevel))
	{
		return 0;
	}
	
	// 检查是否满足消耗条件
	if (!PassiveSkillSystemSingleton::Instance()->UpgradeCostPlayerAssets(pKernel, pSelfObj, pSkillInfo))
	{
		return 0;
	}
	if (-1 == nRows)
	{
		// 增加新的被动技能
		pPassiveSkillRec->AddRowValue(nRows, CVarList() << nSkillId << nRequestLevel << PSS_BY_STUDY);
	}
	else
	{
		// 提升被动技能等级
		pPassiveSkillRec->SetInt(nRows, PS_COL_SKILL_LEVEL, nRequestLevel);
	}
	
#ifndef FSROOMLOGIC_EXPORTS
	// 记录被动技能升级日志
	RoleUpgradeLog log;

	log.type           = LOG_ROLE_UPGRADE_PASSIVE_SKILL;
	log.levelBefore    = nRequestLevel - 1;
	log.levelAfter     = nRequestLevel;
	std::string strSkilId	   = StringUtil::IntAsString(pSkillData->nSkillId);
	log.skillID		   = strSkilId.c_str();
	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, self, log);

	// 通知成就模块更新被动技能升级成就
// 	AchievementModule::m_pAchievementModule->UpdateSelfCheckAchievement(pKernel, self, 
// 		SUBTYPE_GROW_SKILL_PASSIVE_LEVEL, CVarList());

#endif

	// 响应被动技能等级升级的变化
	PassiveSkillSystemSingleton::Instance()->OnPassiveSkillChange(pKernel, self, pSkillData->nSkillId, pSkillInfo);

	// 战斗力更新
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PASSIVE_SKILL_TYPE);

	return 0;
}

// 响应被动技能重置消息(记日志)
int PassiveSkillSystem::OnCustomResetPassiveSkill(IKernel* pKernel, const PERSISTID &self,
	const PERSISTID & sender, const IVarList & args)
{
	if (args.GetCount() != 1 || args.GetType(0) != VTYPE_INT)
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

#ifndef FSROOMLOGIC_EXPORTS
	//被动技能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_PASSIVE_SKILL, self))
	{
		return 0;
	}
#endif // FSROOMLOGIC_EXPORTS

	// 检查是否需要重置被动技能
// 	bool bCheckIsCanReset = PassiveSkillSystemSingleton::Instance()->IsHavePassiveSkill(pKernel, self);
// 	if (!bCheckIsCanReset)
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_RESET_FAILED_NONE_PS, CVarList());
// 		return false;
// 	}

	// 检查技能重置是否满足条件
	if (!PassiveSkillSystemSingleton::Instance()->ResetCostItem(pKernel, self))
	{
		return 0;
	}

	// 执行被动技能重置操作
	PassiveSkillSystemSingleton::Instance()->ExecuteResetPassiveSkill(pKernel, self);

	// 战斗力更新
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PASSIVE_SKILL_TYPE);
	//::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_RESET_SUCCESS, CVarList() << kTotalMoney.nMoneyNum << nTotalPoint);
	return 0;
}

//回调函数 玩家加载数据完成
int PassiveSkillSystem::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 战场服不能再次加载属性 否则属性叠加
#ifndef FSROOMLOGIC_EXPORTS 
	PassiveSkillSystemSingleton::Instance()->AddAllPassiveSkillPro(pKernel, pSelfObj);
#endif // FSROOMLOGIC_EXPORTS
	return 0;
}
// 检查技能的升级条件
bool PassiveSkillSystem::CheckUpgradeTerm(IKernel* pKernel, IGameObj* pSelfObj, const PassiveSkill* pMainInfo, const PassiveSkillLevelInfo* pSkillInfo, const int nRequestSkillLevel)
{
	if (NULL == pSelfObj || NULL == pSkillInfo || NULL == pMainInfo)
	{
		return false;
	}

	bool bLevelUp = true;
	do 
	{
		// 找到被动技能表
		IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
		if (NULL == pPassiveSkillRec)
		{
			bLevelUp = false;
			break;
		}

		// 找到其前置技能
		int nFindRow = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, pSkillInfo->nPreSkillId);
		if (-1 == nFindRow)
		{
			bLevelUp = false;
			break;
		}

		// 统计相同类型的消耗的技能点
		int nRows = pPassiveSkillRec->GetRows();
		int nTotalCost = 0;
		LoopBeginCheck(a);
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(a);
			int nSkillId = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_ID);
			int nSkillLevel = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_LEVEL);
			int nSkillSrc = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_SRC);
			if (nSkillSrc != PSS_BY_STUDY)
			{
				continue;
			}
			const PassiveSkill* pSkillData = m_pSkillDataQueryModule->GetPassiveSkillConfig(nSkillId);
			if (NULL == pSkillData)
			{
				continue;;
			}
			if (pSkillData->nType != pMainInfo->nType)
			{
				continue;
			}

			int nCostSkillPoint = StatOneSkillCostPoint(nSkillId, nSkillLevel, pSkillData);
			nTotalCost += nCostSkillPoint;
		}
	} while (0);
	
	return bLevelUp;
}

// 扣除玩家财产(钱,材料,技能点)
bool PassiveSkillSystem::UpgradeCostPlayerAssets(IKernel* pKernel, IGameObj* pSelfObj, const PassiveSkillLevelInfo* pSkillInfo)
{
	if (NULL == pSelfObj || NULL == pSkillInfo)
	{
		return false;
	}

	// 材料名为空,说明升级不需要材料
	bool bNeedItem = !pSkillInfo->strItemId.empty();

	// 工具容器是否为空
	PERSISTID toolBox = pKernel->GetChild(pSelfObj->GetObjectId(), ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolBox))
	{
		return false;
	}

	// 检查钱、技能点、材料够不够
// 	if(!m_pCapitalModule->CanDecCapital(pKernel, pSelfObj->GetObjectId(), pSkillInfo->nMoneyType, pSkillInfo->nMoneyCost))
// 	{
// 		::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_UPGRADE_FAILED_GOLD, CVarList());
// 		return false;
// 	}
// 
// 	int nCurSkillPoint = pSelfObj->QueryInt(SKILL_POINT_NAME);
// 	if(nCurSkillPoint < pSkillInfo->nSkillPoint)
// 	{
// 		::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_UPGRADE_FAILED_SKILL_POINT, CVarList());
// 		return false;
// 	}
// 	if(bNeedItem && !m_pContainerModule->TryRemoveItems(pKernel, toolBox, pSkillInfo->strItemId.c_str(), pSkillInfo->nItemCost))
// 	{
// 		::CustomSysInfo(pKernel, pSelfObj->GetObjectId(), TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_UPGRADE_FAILED_ITEM, CVarList());
// 		return false;
// 	}

	// 开始扣钱、技能点、道具
	m_pCapitalModule->DecCapital(pKernel, pSelfObj->GetObjectId(), pSkillInfo->nMoneyType, pSkillInfo->nMoneyCost, FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL);
	
//	pSelfObj->SetInt(SKILL_POINT_NAME, nCurSkillPoint - pSkillInfo->nSkillPoint);
	
	// 不需要扣道具直接返回
	if (!bNeedItem)
	{
		return true;
	}

	//有道具，扣道具
	if (FunctionEventModule::GetItemBindPriorUse(FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL))
	{
		m_pContainerModule->RemoveItemsBindPrior(pKernel, toolBox, pSkillInfo->strItemId.c_str(), pSkillInfo->nItemCost, FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL, true);
	}
	else
	{
		m_pContainerModule->RemoveItems(pKernel, toolBox, pSkillInfo->strItemId.c_str(), pSkillInfo->nItemCost, FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL, true);
	}
	return true;
}

// 响应技能升级后的变化
void PassiveSkillSystem::OnPassiveSkillChange(IKernel* pKernel, const PERSISTID &self, int nSkillId, const PassiveSkillLevelInfo* pSkillInfo)
{
	if (NULL == pSkillInfo)
	{
		return;
	}
	const PSPropInfoVec& propvec = pSkillInfo->vPropInfoList;
	int nSize = (int)propvec.size();
	CVarList playerprops;
	LoopBeginCheck(d)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(d)
		const PassiveSkillPropInfo& data = propvec[i];
		// 统计所有改变人物的属性
		if (PS_ADD_PLAYER_PROPERTY == data.nOptType)
		{
			playerprops << data.strPropName;
			playerprops << data.uPropAdd.fPropAdd;
		}
		else if (PS_ADD_SKILL_PROPERTY == data.nOptType) // 改变技能属性
		{
			PERSISTID skill = FightInterfaceInstance->FindSkill(pKernel, self, data.strSkillID.c_str());
			m_pSkillDataQueryModule->OnSkillPropChange(pKernel, skill, self);
		}
		else if (PS_CHANGE_SKILL_CONFIG == data.nOptType)// 影响技能配置
		{
			// 再加送新的属性
			CVarList msg;
			msg << COMMAND_PASSIVE_SKILL_AFFECT_SKILL_CONFIG << 1 << data.strSkillID.c_str() << data.strPropName.c_str();
			pKernel->Command(self, self, msg);
		}
		else if (PS_ADD_NEW_BUFF == data.nOptType) // 增加新buff
		{
			FightInterfaceInstance->AddBuffer(pKernel, self, self, data.strSkillID.c_str());
		}
	}
	
	// 有影响人物属性改变的
	if (playerprops.GetCount() > 0)
	{
		// 先清除属性
		pKernel->Command(self, self, CVarList() << COMMAND_PASSIVE_SKILL_REMOVE_PROP << nSkillId);	
		// 再加送新的属性
		CVarList msg;
		msg << COMMAND_PASSIVE_SKILL_ADD_PROP << nSkillId << (int)playerprops.GetCount() / 2 << playerprops;
		pKernel->Command(self, self, msg);
	}
}

// 归还某个被动的钱、技能点、材料
void PassiveSkillSystem::GiveBackOneSkillAssets(IKernel* pKernel, int& outSkillPoint, int nSkillId, int nSkillLevel)
{
	const PassiveSkill* pSkill = m_pSkillDataQueryModule->GetPassiveSkillConfig(nSkillId);
	if (NULL == pSkill)
	{
		return;
	}

	int nTotalSkillPoint = StatOneSkillCostPoint(nSkillId, nSkillLevel, pSkill);

	// 记录技能点
	outSkillPoint += nTotalSkillPoint;
}

// 读取技能点配置
bool PassiveSkillSystem::LoadSkillPointAddConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Skill/passive_skill_point_config.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	m_vecSkillPointRec.clear();

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_vecSkillPointRec.reserve(iSectionCount);

	LoopBeginCheck(f)
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(f)
		const char* section = xmlfile.GetSectionByIndex(i);

		SkillPoint data;
		data.nMeridianLevel	= xmlfile.ReadInteger(section, "Level", 1);
		data.nAddSkillPoint	= xmlfile.ReadInteger(section, "SkillPointAdd", 1);
			
		m_vecSkillPointRec.push_back(data);
	}
	return true;
}

// 查询对应等级增加的技能点
int PassiveSkillSystem::QueryAddSkillPoint(int nPlayerLevel)
{
	int nAddPoint = 0;
	int nSize = (int)m_vecSkillPointRec.size();
	LoopBeginCheck(h)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(h)
		const SkillPoint& data = m_vecSkillPointRec[i];
		if (data.nMeridianLevel == nPlayerLevel)
		{
			nAddPoint = data.nAddSkillPoint;
			break;
		}
	}

	return nAddPoint;
}

// 技能重置物品检查及扣除
bool PassiveSkillSystem::ResetCostItem(IKernel* pKernel, const PERSISTID &self)
{
	// 工具容器是否为空
	PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolBox))
	{
		return false;
	}

// 	if(!m_pContainerModule->TryRemoveItems(pKernel, toolBox, m_kResetCost.strItemId.c_str(), m_kResetCost.nItemNum))
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_PS_RESET_FAILED_ITEM, CVarList());
// 		return false;
// 	}

	//有道具，扣道具
// 	if (FunctionEventModule::GetItemBindPriorUse(FUNCTION_EVENT_ID_RESET_PASSIVE_SKILL))
// 	{
// 		m_pContainerModule->RemoveItemsBindPrior(pKernel, toolBox, m_kResetCost.strItemId.c_str(), m_kResetCost.nItemNum, FUNCTION_EVENT_ID_RESET_PASSIVE_SKILL, true);
// 	}
// 	else
// 	{
// 		m_pContainerModule->RemoveItems(pKernel, toolBox, m_kResetCost.strItemId.c_str(), m_kResetCost.nItemNum, FUNCTION_EVENT_ID_RESET_PASSIVE_SKILL, true);
// 	}
	return true;
}

// 删除被动技能的处理
bool PassiveSkillSystem::OnRemovePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId, int nSkillLevel)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}
	const PassiveSkillLevelInfo* pSkillInfo = m_pSkillDataQueryModule->GetPassiveSkillLevelInfo(nSkillId, nSkillLevel);
	if (NULL == pSkillInfo)
	{
		return false;
	}
 
	const PSPropInfoVec& propvec = pSkillInfo->vPropInfoList;
	int nSize = (int)propvec.size();
	CVarList prop;
	// 找下是否有改变人物的属性
	LoopBeginCheck(i)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(i)
		const PassiveSkillPropInfo& data = propvec[i];

		// 统计所有改变人物的属性
		if (PS_ADD_PLAYER_PROPERTY == data.nOptType)
		{
			// 清除属性
			pKernel->Command(self, self, CVarList() << COMMAND_PASSIVE_SKILL_REMOVE_PROP << nSkillId);
		}
		else if (PS_ADD_SKILL_PROPERTY == data.nOptType) // 改变技能属性
		{
			PERSISTID skill = FightInterfaceInstance->FindSkill(pKernel, self, data.strSkillID.c_str());
			m_pSkillDataQueryModule->OnSkillPropChange(pKernel, skill, self);
		}
		else if (PS_CHANGE_SKILL_CONFIG == data.nOptType)// 影响技能配置
		{
			// 再加送新的属性
			CVarList msg;
			msg << COMMAND_PASSIVE_SKILL_AFFECT_SKILL_CONFIG << 0 << data.strSkillID.c_str() << data.strPropName.c_str();
			pKernel->Command(self, self, msg);
		}
		else if (PS_ADD_NEW_BUFF == data.nOptType) // 删除buff
		{
			FightInterfaceInstance->RemoveBuffer(pKernel, self, data.strSkillID.c_str());
		}
	}
	return true;
}

// 添加被动技能影响的属性
void PassiveSkillSystem::AddAllPassiveSkillPro(IKernel* pKernel, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return;
	}
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return;
	}
	int nRows = pPassiveSkillRec->GetRows();
	LoopBeginCheck(c)
	for (int i = 0;i < nRows;++i)
	{
		LoopDoCheck(c)
		int nSkillId = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_ID);
		int nSkillLevel = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_LEVEL);

		// 获取当前技能等级的配置
		const PassiveSkillLevelInfo* pSkillInfo = m_pSkillDataQueryModule->GetPassiveSkillLevelInfo(nSkillId, nSkillLevel);
		if (NULL == pSkillInfo)
		{
			continue;
		}
		PassiveSkillSystemSingleton::Instance()->OnPassiveSkillChange(pKernel, pSelfObj->GetObjectId(), nSkillId, pSkillInfo);
	}
}

// 执行被动技能重置
void PassiveSkillSystem::ExecuteResetPassiveSkill(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
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

	int nRecCount = pPassiveSkillRec->GetRows();

	// 记录返还的技能点
	int nTotalPoint = 0;

	// 记录当前所有被动技能的状态
	std::string strInfo = "";

	// 统计返还的玩家资产
	LoopBeginCheck(b)
	for (int i = 0;i < nRecCount;++i)
	{
		LoopDoCheck(b)
		int nSkillId	= pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_ID);
		int nSkillLevel = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_LEVEL);
		int nSkillSrc = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_SRC);
		// 忽略其他途径获得的被动技能
		if (PSS_BY_STUDY != nSkillSrc)
		{
			continue;
		}
		// 此技能没学
		if (0 == nSkillLevel)
		{
			continue;
		}
		// 返还一个技能的消耗资产
		PassiveSkillSystemSingleton::Instance()->GiveBackOneSkillAssets(pKernel, nTotalPoint, nSkillId, nSkillLevel);

		// 记录当前技能状态
		strInfo.append(StringUtil::IntAsString(nSkillId));
		strInfo.append(",");
		strInfo.append(StringUtil::IntAsString(nSkillLevel));
		strInfo.append(";");

		// 清空技能等级
		pPassiveSkillRec->SetInt(i, PS_COL_SKILL_LEVEL, 0);

		// 被动技能移除处理
		PassiveSkillSystemSingleton::Instance()->OnRemovePassiveSkill(pKernel, self, nSkillId, nSkillLevel);
	}

#ifndef FSROOMLOGIC_EXPORTS
	strInfo = strInfo + std::string("Add SkillPoint:") + StringUtil::IntAsString(nTotalPoint);

	// 记录日志
// 	LogModule::m_pLogModule->SavePlayerLog(pKernel, self, LOG_PLAYER_RESET_PS,
// 		util_string_as_widestr(strInfo.c_str()).c_str());
#endif // FSROOMLOGIC_EXPORTS

	// 返还技能点
	int nOldSkillPoint = pSelfObj->QueryInt(SKILL_POINT_NAME);

	// 技能点上限保护
// 	int nMaxSkillPoint = PassiveSkillSystemSingleton::Instance()->ComputeCurLevelMaxSkillPoint(pKernel, pSelfObj);
// 	int nGiveBackPoint = __min(nOldSkillPoint + nTotalPoint, nMaxSkillPoint);

	//pSelfObj->SetInt(SKILL_POINT_NAME, nGiveBackPoint);
}

// 检查是否有学习的被动技能
bool PassiveSkillSystem::IsHavePassiveSkill(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return 0;
	}
	int nRecCount = pPassiveSkillRec->GetRows();

	// 检查是否需要重置被动技能
	bool bCheckIsCanReset = false;
	LoopBeginCheck(a)
	for (int i = 0;i < nRecCount;++i)
	{
		LoopDoCheck(a)
			int nSkillSrc = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_SRC);
		// 忽略宠物加的被动技能
		if (PSS_BY_PET == nSkillSrc)
		{
			continue;
		}
		int nSkillLevel = pPassiveSkillRec->QueryInt(i, PS_COL_SKILL_LEVEL);
		if (nSkillLevel > 0)
		{
			bCheckIsCanReset = true;
			break;
		}
	}

	return bCheckIsCanReset;
}

// 统计某个技能消耗的技能点
int PassiveSkillSystem::StatOneSkillCostPoint(int nSkillId, int nSkillLevel, const PassiveSkill* pMainInfo)
{
	if (NULL == pMainInfo || nSkillId <= 0 || nSkillLevel <= 0)
	{
		return 0;
	}
	// 合理保护
	const PSLevelInfoVec& vPSLevelInfo = pMainInfo->vUpgradeInfoList;
	int nSize = (int)vPSLevelInfo.size();
	if (nSize < nSkillLevel)
	{
		return 0;
	}
	int nTotalCost = 0;
	LoopBeginCheck(r);
	for (int i = 0;i < nSkillLevel;++i)
	{
		LoopDoCheck(r);
		const PassiveSkillLevelInfo& kInfo = vPSLevelInfo[i];
		nTotalCost += kInfo.nSkillPoint;
	}

	return nTotalCost;
}

// 学习所有被动技能(测试、调试使用)
void PassiveSkillSystem::LearnAllPassiveSkill(IKernel* pKernel, const PERSISTID &self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return;
	}
	pPassiveSkillRec->ClearRow();
	// 查询所有能学的被动技能
	CVarList skills;
	m_pSkillDataQueryModule->QueryPassiveSkillIdByJob(pKernel, skills, self);

	// 学习被动技能
	int nCount = (int)skills.GetCount();
	LoopBeginCheck(e)
	for (int i = 0;i < nCount;++i)
	{
		LoopDoCheck(e)
		int nSkillId = skills.IntVal(i);
		// 增加新的被动技能
		pPassiveSkillRec->AddRowValue(-1, CVarList() << nSkillId << 1 << PSS_BY_STUDY);

		// 获取当前技能等级的配置
		const PassiveSkillLevelInfo* pSkillInfo = m_pSkillDataQueryModule->GetPassiveSkillLevelInfo(nSkillId, 1);
		if (NULL == pSkillInfo)
		{
			return;
		}
		
		// 响应被动技能变化
		OnPassiveSkillChange(pKernel, self, nSkillId, pSkillInfo);
	}

	// 战斗力更新
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
// 	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PASSIVE_SKILL_TYPE);
}