//--------------------------------------------------------------------
// 文件名:      LevelModule.cpp
// 内  容:      经验与等级相关模块
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/PlayerBaseDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/RankingDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/SystemFunctionModule/RankListModule.h"
#include "ReLoadConfigModule.h"
#endif
#include "Define/Fields.h"
#include "EnvirValueModule.h"
//#include "Define/BattleAbilityDefine.h"
#include "utils/custom_func.h"
#include "Define/ToolBoxSysInfoDefine.h"
//#include "SkillModule/CriminalSystem.h"
//#include "Define/PKModelDefine.h"

LevelModule* LevelModule::m_pLevelModule = NULL;

int nx_reload_levelup_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 如果指针存在，表明模块已创建
	if (NULL != LevelModule::m_pLevelModule)
	{
		LevelModule::m_pLevelModule->LoadResource(pKernel);

//		WorldAverageLevelAddExpSingleton::Instance()->LoadResource(pKernel);
	}

	return 0;
}

//gm 命令: /add_exp 100
int nx_add_exp(void* state)
{
    // 获得核心指针
    IKernel* pKernel = LuaExtModule::GetKernel(state);
    // 如果指针存在，表明模块已创建
    if (NULL == LevelModule::m_pLevelModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_add_exp, 3);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_add_exp, 1);
    CHECK_ARG_INT(state, nx_add_exp, 2);
    CHECK_ARG_INT(state, nx_add_exp, 3);

    // 获取参数
    PERSISTID self = pKernel->LuaToObject(state, 1);
    int exp_type = pKernel->LuaToInt(state, 2);
    int exp_amount = pKernel->LuaToInt(state, 3);

    bool add_ret = LevelModule::m_pLevelModule->AddExp(pKernel, self, exp_type, exp_amount);
    if (!add_ret)
    {
        return 0;
    }
    
    return 1;
}

bool LevelModule::Init(IKernel* pKernel)
{
	m_pLevelModule = this;	
	m_nMaxLevel = 0;

	pKernel->AddEventCallback("player", "OnRecover", LevelModule::OnRecover, -2);

	DECL_CRITICAL(LevelModule::C_OnLevelChanged);

	//加载配置文件
	LoadResource(pKernel);
	DECL_LUA_EXT(nx_reload_levelup_config);
    DECL_LUA_EXT(nx_add_exp);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("LevelConfig", LevelModule::ReloadConfig);
#endif

//	WorldAverageLevelAddExpSingleton::Instance()->Init(pKernel);
	return true;
}

bool LevelModule::Shut(IKernel* pKernel)
{
	return true;
}

// 添加角色的经验
bool LevelModule::AddExp(IKernel* pKernel,
						 const PERSISTID& self,
						 int nSrcFrom,
						 int64_t exp)
{
	if ( exp <= 0 )
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	//// 非任务奖励时，死亡不进行经验增加
	////if ( pSelfObj->QueryInt("Dead") > 0 )
	////{
	////	/*if (nSrcFrom != FUNCTION_EVENT_ID_TASK)
	////	{
	////		return false;
	////	}*/

	////	
	////}

	// 预创建账号不给经验
	if (pSelfObj->QueryInt("PrepareRole") == ROLE_FLAG_ROBOT)
	{
		return true;
	}

	// 如果玩家已经达到上限了
	if (pSelfObj->QueryInt("Level") >= m_pLevelModule->GetPlayerMaxLevel(pKernel))
	{
		return false;
	}

	// 计算经验值 处理玩家升级
	//玩家当前经验
	int64_t curExp = pSelfObj->QueryInt64("PlayerExp");

// 	int nAddExp = WorldAverageLevelAddExpSingleton::Instance()->GetCurrentAddExp(pKernel, pSelfObj, (int)exp, nSrcFrom);
// 
// 	// 看看是否需要红名惩罚衰减
// 	float fRate = 1.0f;
// 	if (CriminalSystemSingleton::Instance()->QueryCiminalRate(pKernel, fRate, self, nSrcFrom, CP_EXP_RATE))
// 	{
// 		nAddExp = (int)((float)nAddExp * fRate);
// 	}

	int64_t nNewExp = curExp + nSrcFrom;
	pSelfObj->SetInt64("PlayerExp", nNewExp);

#ifndef FSROOMLOGIC_EXPORTS
	// 日志记录
	LogModule::m_pLogModule->OnIncExp(pKernel, self, (int)exp, (int)nNewExp, (int)curExp, nSrcFrom);
#endif

	// 打怪的经验提示不需要显示在系统提示中
	CVarList tip_args;
	tip_args << nSrcFrom;
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7016, tip_args);

	//升级需求经验
	int curLevel = pSelfObj->QueryInt("Level");
	int64_t upgradeExp = GetLevelUpgradeExp(pKernel, self, curLevel);

	//计算加了经验值后的新的级别
	int newLevel = curLevel;
	LoopBeginCheck(b);
	while (nNewExp >= upgradeExp && upgradeExp > 0)
	{
		LoopDoCheck(b);
		newLevel++;
		nNewExp -= upgradeExp;

		//这块放循环里面处理避免一次升多级只刷一次属性的情况，默认是只可能一次升一级
		if (newLevel > curLevel)
		{
			LevelUp(pKernel, self, newLevel, nNewExp);
		}

		// 重置升级所需经验值
		upgradeExp = GetLevelUpgradeExp(pKernel, self, newLevel);

	}
	return true;
}

// 获取角色升级所需经验
int64_t LevelModule::GetLevelUpgradeExp(IKernel* pKernel,
									const PERSISTID& self,
									int level)
{
	// 保护
	if (!pKernel->Exists(self))
	{
		return 0;
	}
	// 检查等级数值是否合法
	if (level <= 0 ||
		level > m_pLevelModule->GetPlayerMaxLevel(pKernel))
	{
		extend_warning(LOG_WARNING, "[%s][%d] invalid level", __FILE__, __LINE__);
		return 0;
	}

	return m_pLevelModule->m_vecExpToUpgrade[level];
}

// 读取配置信息
bool LevelModule::LoadResource(IKernel* pKernel)
{
	m_vecExpToUpgrade.clear();

	//初始化各级别升级所需经验
	std::string pathName = pKernel->GetResourcePath();
	//配置文件路径
	pathName += "ini/SystemFunction/LevelExp/levelexp.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string err_msg = pathName;
		err_msg.append(" does not exist.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	//最大等级
	m_nMaxLevel = xmlfile.ReadInteger("MaxLevel", "Exp", 80);

	//各级所需经验
	m_vecExpToUpgrade.resize(m_nMaxLevel + 1);

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	LoopBeginCheck(a);
	for (int i = 0; i < iSectionCount; ++i)
	{
		LoopDoCheck(a);
		const char* sectionName = xmlfile.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sectionName))
		{
			continue;
		}

		int iLevel = ::atoi(sectionName);
		if (iLevel <= 0)
		{
			continue;
		}

		int64_t iLevelExpValue = xmlfile.ReadInt64(sectionName, "Exp", 0);
		if (iLevel <= 0 || iLevel > m_nMaxLevel)
		{
			continue;
		}
		m_vecExpToUpgrade[iLevel] = iLevelExpValue;
	}	
	return true;
}

// 升级
bool LevelModule::LevelUp(IKernel* pKernel, const PERSISTID& self, int newlevel, int64_t curExp)
{
	// 保护
	if (!pKernel->Exists(self))
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	int oldlevel = pSelfObj->QueryInt("Level");
	if (oldlevel == newlevel)
	{
		return false;
	}

	//设置当前经验
	pSelfObj->SetInt64("PlayerExp", curExp);

	int64_t upgradeExp = GetLevelUpgradeExp(pKernel, self, newlevel);
	//升级后下级需要的经验
	pSelfObj->SetInt64("PlayerUpgradeExp", upgradeExp);

	// 超过最高等级
	if ( newlevel > m_pLevelModule->GetPlayerMaxLevel(pKernel) )
	{
		newlevel = m_pLevelModule->GetPlayerMaxLevel(pKernel);
	} 

	pSelfObj->SetInt("Level", newlevel);
#ifndef FSROOMLOGIC_EXPORTS
	m_pLevelModule->SaveLevelUpInfo(pKernel, self, oldlevel, newlevel);
#endif
	return true;
}

//刷新升级所需经验
int LevelModule::OnRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 保护
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 等级回调
	ADD_CRITICAL(pKernel, self, "Level", "LevelModule::C_OnLevelChanged");

	// 新建角色
	int nLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	if (nLevel <= 0)
	{
		nLevel = 1;
		pSelfObj->SetInt(FIELD_PROP_LEVEL, nLevel);
	}
	// 刷新升级所需经验
	int64_t iUpgradeExp = m_pLevelModule->GetLevelUpgradeExp(pKernel, self, nLevel);
	pSelfObj->SetInt64("PlayerUpgradeExp", iUpgradeExp);

//	OnCommandComputeKillNpcExpRate(pKernel, self, self, args);
	return 0;
}

//等级改变回调
int LevelModule::C_OnLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old)
{
	// 保护
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int old_level = old.IntVal();
	int new_level = pSelfObj->QueryInt("Level");

	if (old_level <= 0 || new_level <= 0)
	{
		return 0;
	}

	// 当前最高等级
	int maxLevel = m_pLevelModule->GetPlayerMaxLevel(pKernel);

	// 记录升级信息
	//int saveLevel = new_level > maxLevel ? maxLevel : new_level;
	//if( old_level != saveLevel )
	//{
	//	m_pLevelModule->SaveLevelUpInfo(pKernel, self, old_level, saveLevel);
	//}
	//
	// 超过最高等级
	if (new_level > maxLevel)
	{
		pSelfObj->SetInt("Level", maxLevel);
		return 0;
	}

	LoopBeginCheck(c);
	for (int curLevel = old_level; curLevel < new_level; ++curLevel)
	{
		LoopDoCheck(c);
		m_pLevelModule->RefreshLevelUp(pKernel, self, curLevel, curLevel + 1);
	}

	//通知客户端播放特效
//	pKernel->CustomByKen(self, CVarList() << SERVER_CUSTOMMSG_EFFECT << self << LEVEL_UP);

	CVarList msg;
	msg << COMMAND_LEVELUP_CHANGE << old_level;
	pKernel->Command(self, self, msg);

    //保存信息 防止玩家登陆数据丢失
    pKernel->Command(self, self, CVarList() << COMMAND_SAVE_ROLE_INFO);

	// 战斗力更新
//	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
	return 0;
}

// 刷新等级变化后的属性
bool LevelModule::RefreshLevelUp(IKernel* pKernel,
								 const PERSISTID& obj,
								 const int old_level,
								 int curLevel)
{
	// 保护
	if (!pKernel->Exists(obj))
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(obj);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 刷新角色经验值
	int64_t levelUpgradeExp = pSelfObj->QueryInt64("PlayerUpgradeExp");
	int64_t curLevelUpgradeExp = GetLevelUpgradeExp(pKernel, obj, curLevel);
	if (levelUpgradeExp < curLevelUpgradeExp)
	{
		pSelfObj->SetInt64("PlayerUpgradeExp", curLevelUpgradeExp);
	}
 
	// 升级后满血
	pSelfObj->SetInt64("HP", pSelfObj->QueryInt64("MaxHP"));
	return true;
}

// 尝试增加经验值
bool LevelModule::TryAddExp( IKernel* pKernel, const PERSISTID& self, int64_t exp )
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 等级限制
	int iSelfLevel = pSelfObj->QueryInt("Level");	       // 玩家当前等级
	int iMaxLevel = m_pLevelModule->GetPlayerMaxLevel(pKernel);	// 最大等级

	// 已到最大等级，不能加经验
	if ( iSelfLevel >= iMaxLevel )
	{
		return false;
	}

	// 要添加的经验值是否有效
	int64_t iPlayerExp = pSelfObj->QueryInt64("PlayerExp");
	int64_t iNewExp = exp + iPlayerExp;

	//升级需求经验
	int64_t iUpgradeExp = m_pLevelModule->GetLevelUpgradeExp(pKernel, self, iSelfLevel);
	int iAddLevel = 0;	// 升级数
	int iNewLevel = iSelfLevel;	// 新等级

	//计算加了经验值后的新的级别
	LoopBeginCheck(d);
	while (iNewExp >= iUpgradeExp && iUpgradeExp > 0)
	{
		LoopDoCheck(d);
		++iAddLevel;
		iNewExp -= iUpgradeExp;
		
		// 重置升级所需经验值
		iNewLevel += iAddLevel;	// 新等级
		iUpgradeExp = m_pLevelModule->GetLevelUpgradeExp(pKernel, self, iNewLevel);
	}

	// 新等级超过最大等级或达到最大等级并且升级后的经验值大于0，表示该经验值不可添加
	if ( iNewLevel > iMaxLevel ) // 升级时候的超出部分经验会被记录
	{
		return false;
	}

	return true;
}

// 取得角色等级上限值
int LevelModule::GetPlayerMaxLevel(IKernel* pKernel)
{
	if( pKernel == NULL )
	{
		return 0;
	}

	return m_nMaxLevel;
}

// 玩家升级信息记录
void LevelModule::SaveLevelUpInfo(IKernel* pKernel, const PERSISTID& self, const int oldLevel, const int newLevel)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if( pSelfObj == NULL )
	{
		return;
	}

#ifndef FSROOMLOGIC_EXPORTS
	//保存人物升级日志
	RoleUpgradeLog log;
	log.type           = LOG_ROLE_UPGRADE_PLAYER;
	log.levelBefore    = oldLevel;
	log.levelAfter     = newLevel;
	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, self, log);
#endif // FSROOMLOGIC_EXPORTS
}

void LevelModule::ReloadConfig(IKernel* pKernel)
{
	m_pLevelModule->LoadResource(pKernel);
}