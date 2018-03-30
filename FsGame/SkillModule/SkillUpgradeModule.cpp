//---------------------------------------------------------
//文件名:       SkillUpgradeModule.h
//内  容:       技能和技能升级模块
//说  明:  
//          
//创建日期:      2016年4月5日
//创建人:         刘明飞
//修改人:
//   :         
//---------------------------------------------------------

#include "SkillUpgradeModule.h"
#include "SkillToSprite.h"
#include "FsGame/Define/Skilldefine.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/XmlFile.h"
#include "utils/custom_func.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
//#include "FsGame/Define/SkillUpgradeSysInfoDefine.h"
#include "FsGame/SkillModule/SkillToSprite.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "Skillmodule.h"
#include "PassiveSkillSystem.h"
#include "utils/string_util.h"
#include "../Define/FightPropertyDefine.h"
#include "../Define/StaticDataDefine.h"
#include "../CommonModule/EnvirValueModule.h"
#include "../Interface/FightInterface.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "Define/GameDefine.h"
//#include "Define/BattleAbilityDefine.h"
#define SKILL_UPGRADE_CONFIG_FILE    "ini\\Skill\\SkillUpgradeConfig.xml"
#define SKILL_LEVEL_COST_CONFIG_FILE "ini\\Skill\\SkillLevelCostConfig.xml"
#define NORMAL_SKILL_CONFIG_FILE     "ini\\Skill\\NormalSkillConfig.xml"

SkillUpgradeModule * SkillUpgradeModule::m_pSkillUpgradeModule = NULL;
CapitalModule * SkillUpgradeModule::m_pCapitalModule = NULL;

inline int nx_reload_skill_upgrade_config(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL != SkillUpgradeModule::m_pSkillUpgradeModule)
    {
        SkillUpgradeModule::m_pSkillUpgradeModule->LoadResource(pKernel);
    }

    return 0;
}

// 升级某个技能
int nx_upgrade_skill(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_upgrade_skill, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_upgrade_skill, 1);
	CHECK_ARG_STRING(state, nx_upgrade_skill, 2);
	CHECK_ARG_INT(state, nx_upgrade_skill, 3);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);
	const char* strSkill = pKernel->LuaToString(state, 2);
	int		nType = pKernel->LuaToInt(state, 3);

	SkillUpgradeModule::m_pSkillUpgradeModule->OnUpgradeSkill(pKernel, self, nType, CVarList() << CLIENT_CUSTOMMSG_SKILL_UPGRADE << C2S_ESKILL_UPGRADE_SKILL << strSkill);
	return 1;
}

bool SkillUpgradeModule::Init(IKernel* pKernel)
{
    m_pSkillUpgradeModule = this;
    m_pCapitalModule = (CapitalModule *)pKernel->GetLogicModule("CapitalModule");

    Assert(m_pCapitalModule != NULL);

    pKernel->AddEventCallback("player", "OnRecover", SkillUpgradeModule::OnPlayerRecover);

    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_SKILL_UPGRADE, SkillUpgradeModule::OnSkillSetupMsg);

    LoadResource(pKernel);

    // 定义属性变化回调
    DECL_CRITICAL(SkillUpgradeModule::OnPlayerLevelUp);

    DECL_LUA_EXT(nx_reload_skill_upgrade_config);
	DECL_LUA_EXT(nx_upgrade_skill);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("SkillUpgradeConfig", SkillUpgradeModule::ReloadConfig);
#endif
    return true;
}

bool SkillUpgradeModule::Shut(IKernel *pKernel)
{
    return true;
}

// 玩家数据恢复
int SkillUpgradeModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& player,
                           const PERSISTID& sender, const IVarList& args)
{
    // 属性变化的回调
    pKernel->AddCritical(player, "Level", "SkillUpgradeModule::OnPlayerLevelUp");
    
    // 为玩家分配默认技能(如果玩家没有获得技能的话)

    if (!pKernel->Exists(player))
    {
        return 0;
    }
    
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 设置普通技能
    m_pSkillUpgradeModule->SetPlayerNormalSkill(pKernel, player);

	// 技能学习
	m_pSkillUpgradeModule->LearnPlayerSkill(pKernel, player);
    return 0;
}

// 处理来自客户端的消息
int SkillUpgradeModule::OnSkillSetupMsg(IKernel *pKernel, const PERSISTID &player,
                           const PERSISTID & sender, const IVarList & args)
{
    int sub_msg_id = args.IntVal(1);

    switch (sub_msg_id)
    {
    case C2S_ESKILL_UPGRADE_SKILL:
        m_pSkillUpgradeModule->OnUpgradeSkill(pKernel, player, UPGRADE_ONE_LEVEL, args);
		break;
	case C2S_ESKILL_UPGRADE_MULTI:
		m_pSkillUpgradeModule->OnUpgradeSkill(pKernel, player, UPGRADE_MULTI_LEVEL, args);
		break;

    default:
       break;
    }

    return 0;
}

// 玩家升级技能的请求
int SkillUpgradeModule::OnUpgradeSkill(IKernel *pKernel, const PERSISTID &player, int nUpgradeType, const IVarList & args)
{
    /*
        收到的消息
            [msg_id][sub_msg_id][skill_id]

        回应的消息

            成功:
                [msg_id][sub_msg_id][1][citta_row_id_in_record][citta_id][citta_level]
            失败:
                [msg_id][sub_msg_id][0][citta_id]
            
     */
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }

#ifndef FSROOMLOGIC_EXPORTS
	//技能升级功能开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_SKILL_UP, player))
	{
		return 0;
	}
#endif

    const char *skill_id = args.StringVal(2);
	if (StringUtil::CharIsNull(skill_id))
	{
		return 0;
	}

	// 处理技能升级
	int nOldSkillLevel = 0;
	int nNewSkillLevel = 0;
	if(!m_pSkillUpgradeModule->UpgradeSkillLevel(pKernel, player, skill_id, nUpgradeType, nOldSkillLevel, nNewSkillLevel))
	{
		return 0;
	}

	// 记录技能升级日志modify by   (2015-04-23)
	RoleUpgradeLog log;
	log.type           = LOG_ROLE_UPGRADE_SKILL;
	log.levelBefore    = nOldSkillLevel;
	log.levelAfter     = nNewSkillLevel;
	log.skillID        = skill_id;
#ifndef FSROOMLOGIC_EXPORTS
	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, player, log);
#endif

	// 战斗力更新
	//pKernel->Command(player, player, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_SKILL_TYPE);
    return 0;
}

// 玩家角色升级
int SkillUpgradeModule::OnPlayerLevelUp(IKernel* pKernel, const PERSISTID& player,
                                        const char* property, const IVar& old)
{
    m_pSkillUpgradeModule->LearnPlayerSkill(pKernel, player);
	m_pSkillUpgradeModule->SetPlayerNormalSkill(pKernel, player);

	// 战斗力更新
	//pKernel->Command(player, player, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_SKILL_TYPE);
    return 0;
}


// 处理技能升级的逻辑
bool SkillUpgradeModule::UpgradeSkillLevel(IKernel * pKernel, const PERSISTID & player, const char * skill_id, int nUpgradeType, int &nOldSkillLevel, int& nNewSkillLevel) 
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if(StringUtil::CharIsNull(skill_id) || NULL == pPlayerObj)
	{
		return false;
	} 

	// 技能的定义
	const SkillBaseDef *pSkillDef = GetSkillDef(skill_id);
	if (pSkillDef == NULL)
	{
		return false;
	}

	PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, player, skill_id);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSkillObj)
	{
		return false;
	}


	// 默认为只升一级
	nOldSkillLevel = pSkillObj->QueryInt("Level");
	nNewSkillLevel = nOldSkillLevel + 1;

	const SkillLevelCostDef * level_cost = GetSkillLevelCost(pSkillDef->seq_no, nNewSkillLevel);
	if (level_cost == NULL)
	{
		// 找不到此等级的价格
		return false;
	}

	int nCostMoney = 0;
	// 检查金币是否足够
	if (!m_pCapitalModule->CanDecCapital(pKernel, player, level_cost->capital_type, level_cost->capital_amount))
	{
		//CVarList unlock_tip_args;
		//::CustomSysInfo(pKernel, player, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_CITTA_UPGRADE_FAILED_GOLD, unlock_tip_args);

		return false;
	}

	// 当前的铜钱
	__int64 currentCapital = m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_COPPER);

	// 玩家等级
	int player_level = pPlayerObj->QueryInt("Level");

	// 一键升级的最高等级
	int nMaxUpgradeLevel = 1;// EnvirValueModule::EnvirQueryInt(FU_SKILL_LEVEL);

	// 计算最多能升级的等级数
	int nUpgradeLevel = 0;
	if(player_level >= nOldSkillLevel + nMaxUpgradeLevel)
	{
		nUpgradeLevel = nMaxUpgradeLevel;
	}	
	else
	{
		nUpgradeLevel = player_level - nOldSkillLevel;
	}

	// 一键升级的方式
	if (UPGRADE_MULTI_LEVEL  == nUpgradeType)
	{
		LoopBeginCheck(p);
		for (int i = nOldSkillLevel + 1;i <= nOldSkillLevel + nUpgradeLevel;++i)
		{
			LoopDoCheck(p);
			level_cost = GetSkillLevelCost(pSkillDef->seq_no, i);
			if (level_cost == NULL)
			{
				// 找不到此等级的价格
				continue;
			}
			nNewSkillLevel = i;
			nCostMoney += level_cost->capital_amount;
			// 超过玩家铜钱最大值
			if (nCostMoney > currentCapital)
			{
				// 恢复到上一级别的消耗
				--nNewSkillLevel;
				nCostMoney -= level_cost->capital_amount;
				break;
			}
		}
	}
	else
	{
		nCostMoney = level_cost->capital_amount;
	}

	// 技能等级不能高于限制
	if (nNewSkillLevel > pSkillDef->max_level || nNewSkillLevel > player_level)
	{
		return false;
	}

	// 扣铜钱
	m_pCapitalModule->DecCapital(pKernel, player, level_cost->capital_type, nCostMoney, FUNCTION_EVENT_ID_UPDATE_SKILL);

	// 提升技能等级
	pSkillObj->SetInt("Level", nNewSkillLevel);

	// 增加对应任务的升级次数
// 	CVarList update_msg;
// 	update_msg << COMMAND_QUEST_COMMAND
// 						  << COMMAND_QUEST_SUB_UPDATE
// 						  << CRITERIA_TYPE_SKILL_UPGRADE
// 						  << (nNewSkillLevel - nOldSkillLevel);
// 	pKernel->Command(player, player, update_msg);
// 
// 	// 更新日常活跃
// 	CVarList s2s_msg;
// 	s2s_msg << COMMAND_DAILY_ACTIVITY
// 			<< DAILY_ACTIVITY_TYPE_UPGRADE_SKILL
// 			<< (nNewSkillLevel - nOldSkillLevel);
// 	pKernel->Command(player, player, s2s_msg);

	return true;
}

// 获取技能配置
const SkillUpgradeModule::SkillBaseDef* SkillUpgradeModule::GetSkillDef(const char *skill_id)
{
	SkillMap::iterator it = m_mapSkillData.find(skill_id);
	if (it != m_mapSkillData.end())
	{
		return &(it->second);
	}
	else
	{
		return NULL;
	}
}

// 根据玩家的等级和职业，获取玩家可用的普通技能																			  
int SkillUpgradeModule::GetUsableNormalSkill(IKernel *pKernel, const PERSISTID &player, std::vector<std::string> *out_skill)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return 0;
	}

	int player_career = pPlayerObj->QueryInt("Job");
	int player_level = pPlayerObj->QueryInt("Level");

	NormalSkillMap::const_iterator it = m_mapNormalSkill.begin();
	NormalSkillMap::const_iterator it_end = m_mapNormalSkill.end();

	LoopBeginCheck(c);
	for (; it != it_end; ++it)
	{
		LoopDoCheck(c);
		const NormalSkillDef &skill_def = it->second;
		if (skill_def.level_to_unlock > player_level)
		{
			continue;
		}

		if (player_career == skill_def.career_id || PUBLIC_CAREER_SKILL == skill_def.career_id)
		{
			out_skill->push_back(skill_def.id);
		}
	}

	return 0;
}

// 获取技能消耗配置
const SkillUpgradeModule::SkillLevelCostDef* SkillUpgradeModule::GetSkillLevelCost(int skill_seqno, int level)
{
	char the_key[256] = {0};
	SPRINTF_S(the_key, "%d_%d", skill_seqno, level);

	SkillLevelCostMap::iterator it = m_mapSkillLevelCost.find(the_key);
	if (it == m_mapSkillLevelCost.end())
	{
		return NULL;
	}
	else
	{
		return &(it->second);
	}
}

// 设置玩家的普通技能
int SkillUpgradeModule::SetPlayerNormalSkill(IKernel *pKernel, const PERSISTID &player)
{
	PERSISTID skill_contaienr = pKernel->GetChild(player, SKILL_CONTAINER_NAME);
	if (!pKernel->Exists(skill_contaienr))
	{
		return 0;
	}

	// 玩家的普通技能
	std::vector<std::string> normal_skill;
	m_pSkillUpgradeModule->GetUsableNormalSkill(pKernel, player, &normal_skill);

	std::vector<std::string>::iterator it = normal_skill.begin();
	std::vector<std::string>::iterator it_end = normal_skill.end();

	LoopBeginCheck(d);
	for (; it!=it_end; ++it)
	{
		LoopDoCheck(d);
		std::string &skill_config_id = *it;
		FightInterfaceInstance->UpdateSkillData(pKernel, player, skill_config_id.c_str(), 1);
	}

	return 0;
}

// 玩家技能学习
int SkillUpgradeModule::LearnPlayerSkill(IKernel *pKernel, const PERSISTID &player)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 获取当前玩家等级
	int nCurLevel = pSelfObj->QueryInt("Level");

	// 玩家职业
	int nJob = pSelfObj->QueryInt("Job");
	
	SkillMap::iterator iter = m_mapSkillData.begin();
	LoopBeginCheck(rt);
	for (;iter != m_mapSkillData.end();++iter)
	{
		LoopDoCheck(rt);
		const SkillBaseDef& data = iter->second;
		// 只学本职业的技能
		if(data.career_id != nJob)
		{
			continue;;
		}
		PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, player, data.id.c_str());
		if (!skill.IsNull())
		{
			// 此技能玩家已经拥有
			continue;
		}

		// 默认为系统创建的玩家增加技能
		if (nCurLevel >= data.level_to_unlock)
		{
			// 添加玩家技能
			FightInterfaceInstance->UpdateSkillData(pKernel, player, data.id.c_str(), 1);

			// 通知客户端
			CVarList msg_to_client;
			msg_to_client << SERVER_CUSTOMMSG_SKILL_UPGRADE;
			msg_to_client << S2C_ESKILL_SETUP_SKILL_UNLOCKED;
			msg_to_client << data.id.c_str();

			pKernel->Custom(player, msg_to_client);
		}
	}

	return 0;
}

// 载入配置资源
bool SkillUpgradeModule::LoadResource(IKernel* pKernel)
{
	m_mapSkillLevelCost.clear();
	m_mapSkillData.clear();
	m_mapNormalSkill.clear();

	if (!LoadSkill(pKernel))
	{
		return false;
	}

	if (!LoadSkillLevelCost(pKernel))
	{
		return false;
	}

	if (!LoadNormalSkill(pKernel))
	{
		return false;
	}

	return true;
}

// 载入技能配置
bool SkillUpgradeModule::LoadSkill(IKernel* pKernel)
{
    std::string res_path = pKernel->GetResourcePath();
    std::string skill_config_path = res_path;
    skill_config_path.append(SKILL_UPGRADE_CONFIG_FILE);

    CXmlFile xml(skill_config_path.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = skill_config_path;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();
	LoopBeginCheck(l);
    for (int i=0; i<sec_count; i++)
    {
		LoopDoCheck(l);
        const char *sec = sec_list.StringVal(i);

        SkillBaseDef skill_def;
        skill_def.id = sec;
        skill_def.career_id = xml.ReadInteger(sec, "CareerID", 0);
        skill_def.seq_no = xml.ReadInteger(sec, "SeqNo", -1);
        skill_def.level_to_unlock = xml.ReadInteger(sec, "LevelToUnlock", 0);
        skill_def.max_level = xml.ReadInteger(sec, "MaxLevel", 15);

		if (m_mapSkillData.find(skill_def.id) == m_mapSkillData.end())
		{
			m_mapSkillData.insert(make_pair(skill_def.id, skill_def));
		}
    }
    return true;
}

// 载入技能等级价格
bool SkillUpgradeModule::LoadSkillLevelCost(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	std::string skill_config_path = res_path;
	skill_config_path.append(SKILL_LEVEL_COST_CONFIG_FILE);

	CXmlFile xml(skill_config_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = skill_config_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	CVarList sec_list;
	xml.GetSectionList(sec_list);

	int sec_count = (int)sec_list.GetCount();
	LoopBeginCheck(m);
	for (int i=0; i<sec_count; i++)
	{
		LoopDoCheck(m);
		const char *sec = sec_list.StringVal(i);

		SkillLevelCostDef level_cost_def;
		level_cost_def.skill_seq_no = xml.ReadInteger(sec, "SkillSeqNo", -1);
		level_cost_def.skill_level = xml.ReadInteger(sec, "Level", -1);
		level_cost_def.capital_type = xml.ReadInteger(sec, "CapitalType", -1);
		level_cost_def.capital_amount = xml.ReadInteger(sec, "CapitalAmount", -1);

		if (level_cost_def.skill_seq_no == -1 || level_cost_def.skill_level == -1
			|| level_cost_def.capital_type == -1 || level_cost_def.capital_amount == -1)
		{
			std::string err_msg = skill_config_path;
			err_msg.append(" invalid config.");
			::extend_warning(LOG_ERROR, err_msg.c_str());

			continue;
		}

		char the_key[256] = {0};
		SPRINTF_S(the_key, "%d_%d", level_cost_def.skill_seq_no, level_cost_def.skill_level);

		SkillLevelCostMap::iterator it = m_mapSkillLevelCost.find(the_key);
		if (it == m_mapSkillLevelCost.end())
		{
			m_mapSkillLevelCost.insert(SkillLevelCostMap::value_type(the_key, level_cost_def));
		}
	}
	return true;
}

//　载入普通技能
bool SkillUpgradeModule::LoadNormalSkill(IKernel* pKernel)
{
	 /*
        <Property ID="10110a" CareerID="1" CittaLevelToUnlock="0" /> 
    */

    std::string res_path = pKernel->GetResourcePath();
    std::string skill_config_path = res_path;
    skill_config_path.append(NORMAL_SKILL_CONFIG_FILE);

    CXmlFile xml(skill_config_path.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = skill_config_path;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();
	LoopBeginCheck(o)
    for (int i=0; i<sec_count; i++)
    {
		LoopDoCheck(o)
        const char *sec = sec_list.StringVal(i);

        NormalSkillDef skill_def;
        skill_def.id = sec;
        skill_def.career_id = xml.ReadInteger(sec, "CareerID", 0);
        skill_def.level_to_unlock = xml.ReadInteger(sec, "LevelToUnlock", 0);

		NormalSkillMap::iterator it = m_mapNormalSkill.find(skill_def.id);
		NormalSkillMap::iterator it_end = m_mapNormalSkill.end();

		if (it == it_end)
		{
			m_mapNormalSkill.insert(NormalSkillMap::value_type(skill_def.id, skill_def));
		}
    }

    return true;
}

void SkillUpgradeModule::ReloadConfig(IKernel* pKernel)
{
	m_pSkillUpgradeModule->LoadResource(pKernel);
}