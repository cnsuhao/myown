#include "GuildLoadResource.h"
#include "utils/extend_func.h"
#include "server/LoopCheck.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
const char * GUILD_SKILL = "ini/SocialSystem/Guild/GuildSkillConfig.xml";
const char * GUILD_SKILL_LEVEL_UP = "ini/SocialSystem/Guild/GuildSkillLevelUpCost.xml";
const char * GUILD_SKILL_STUDY = "ini/SocialSystem/Guild/GuildSkillStudy.xml";

bool LoadGuildUpLevelConfig(const char *resourcePath, GuildUpLevelConfigVector &configVec)
{
    configVec.clear();

    std::string pathname = std::string(resourcePath) + "ini/SocialSystem/Guild/GuildMemberConfig.xml";
    TiXmlDocument xmlDocument(pathname.c_str());
    if (!xmlDocument.LoadFile())
    {
		extend_warning(LOG_ERROR, "LoadGuildUpLevelConfig: load %s ERROR !!", pathname.c_str());
        return false;
    }

    TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

    TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bl);
    for (int i = 0; groupEle != NULL; i++)
    {
		LoopDoCheck(bl);
        GuildUpLevelConfig_t oneData;
        oneData.m_Level            = convert_int(groupEle->Attribute("Level"), 1);
       
        oneData.m_MaxMember        = convert_int(groupEle->Attribute("MaxMember"), 0);
        oneData.m_DeputyCaptainNum = convert_int(groupEle->Attribute("DeputyCaptainNum"), 0);
        oneData.m_GeneralNum       = convert_int(groupEle->Attribute("GeneralNum"), 0);
		oneData.m_Elder			   = convert_int(groupEle->Attribute("ElderNum"), 0);
      
        configVec.push_back(oneData);

        groupEle = groupEle->NextSiblingElement("Property");
    }

    return true;
}

bool LoadGuildDonateConfig(const char *resourcePath, GuildDonateConfigVector &configVec)
{
    configVec.clear();

    std::string pathname = std::string(resourcePath) + "ini/SocialSystem/Guild/GuildDonateConfig.xml";
    TiXmlDocument xmlDocument(pathname.c_str());
    if (!xmlDocument.LoadFile())
    {
		extend_warning(LOG_ERROR, "LoadGuildDonateConfig: load %s ERROR !!", pathname.c_str());
        return false;
    }

    TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

    TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bm);
    for (int i = 0; groupEle != NULL; i++)
    {
		LoopDoCheck(bm);
        GuildDonateConfig_t oneData;
        oneData.m_Id                = convert_int(groupEle->Attribute("ID"), 0);
        Assert(oneData.m_Id == i);
        oneData.m_CapitalNumber     = convert_int(groupEle->Attribute("CapitalNumber"), 0);
        oneData.m_RewardPlayerDonate= convert_int(groupEle->Attribute("RewardPlayerDonate"), 0);
        oneData.m_GuildDonate			= convert_int(groupEle->Attribute("GuildDonate"), 0);
        configVec.push_back(oneData);


        groupEle = groupEle->NextSiblingElement("Property");
    }

    return true;
}

// 加载公会职务配置信息
bool LoadGuildPositionConfig(const char *resourcePath, GuildPositionConfigVector &configVec)
{
    configVec.clear();

	std::string pathname = std::string(resourcePath) + "ini/SocialSystem/Guild/GuildPositionConfig.xml";

	CXmlFile ini(pathname.c_str());
	std::string log;
	if (!ini.LoadFromFile(log))
	{
		::extend_warning(
			LOG_ERROR,
			"[StaticDataQueryModule::LoadStaticData] %s",
			log.c_str());
		return false;
	}
	int sect_num = ini.GetSectionCount();
	for (int i = 0; i < sect_num; i++)
	{
		int cols = ini.GetSectionItemCount(i);
		GuildPositionConfig_t info;
		for (int j = 0; j < cols; j++)
		{
			const char* key = ini.GetSectionItemKey(i, j);
			const char* val = ini.GetSectionItemValue(i, j);

			if (strcmp(key, "Position") == 0)
			{
				info.m_Position = StringUtil::StringAsInt(val);
				continue;
			}



			int value = StringUtil::StringAsInt(val);
			if (value == 0){
				continue;
			}
			
			std::string keyName = key;
			keyName = keyName.erase(0, 1);
			int privType = StringUtil::StringAsInt(keyName.c_str());
			info.m_privList |= 1 << privType;
		}

		configVec.push_back(info);
	}

    return true;
}

// 加载公会商店配置信息
bool LoadGuildShopConfig(const char *resourcePath, GuildShopConfigVector &configVec)
{
    configVec.clear();

	std::string pathname = std::string(resourcePath) + "ini/SocialSystem/Guild/GuildShopConfig.xml";
    TiXmlDocument xmlDocument(pathname.c_str());
    if (!xmlDocument.LoadFile())
    {
		extend_warning(LOG_ERROR, "LoadGuildShopConfig: load %s ERROR !!", pathname.c_str());
        return false;
    }

    TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

    TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bo);
    for (int i = 0; groupEle != NULL; i++)
    {
		LoopDoCheck(bo);
        GuildShopConfigItem_t oneData;
		oneData.m_Index			= convert_int(groupEle->Attribute("ID"), 0);
        oneData.m_ItemId        = groupEle->Attribute("ItemID");
        oneData.m_MemberLimit   = convert_int(groupEle->Attribute("MemberLimit"), 0);
        oneData.m_GuildTotal    = convert_int(groupEle->Attribute("GuildTotal"), 0);
        oneData.m_UnlockLevel   = convert_int(groupEle->Attribute("UnlockLevel"), 1);
		oneData.m_addGuildCapital = convert_int(groupEle->Attribute("AddGuildCapital"), 0);
        const char* decCap   = convert_string(groupEle->Attribute("DecCapital"), "");

		CVarList ret;
		StringUtil::SplitString(ret, decCap, ",");
		for (size_t i = 0; i < ret.GetCount(); i++)
		{
			CVarList ret2;
			StringUtil::SplitString(ret2,ret.StringVal(i), ":");
			Assert (ret2.GetCount() == 2);
			std::pair<int, int> cap;
			cap.first = ret2.IntVal(0);
			cap.second = ret2.IntVal(1);
			oneData.m_decCapital.push_back(cap);

		}



        configVec.push_back(oneData);
        groupEle = groupEle->NextSiblingElement("Property");
    }

    return true;
}

// 加载公会商店重置配置
bool LoadGuildTimer(const char *resourcePath, GuildTimerCallbackVector &configVec)
{
    configVec.clear();

    std::string pathname = std::string(resourcePath) + "ini/SocialSystem/Guild/GuildTimer.xml";
    TiXmlDocument xmlDocument(pathname.c_str());
    if (!xmlDocument.LoadFile())
    {
		extend_warning(LOG_ERROR, "LoadGuildTimer: load %s ERROR !!", pathname.c_str());
        return false;
    }

    TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

    TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bp);
    for (int i = 0; groupEle != NULL; i++)
    {
		LoopDoCheck(bp);
        GuildTimerCallback_t oneData;
        oneData.m_TimerName = groupEle->Attribute("TimerName");
        oneData.m_CallbackName = groupEle->Attribute("CallbackName");
        
        oneData.m_Hour      = convert_int(groupEle->Attribute("ResetHour"), 0);
        oneData.m_Minute    = convert_int(groupEle->Attribute("ResetMinute"), 0);
        oneData.m_CallbackFunc = NULL;

        configVec.push_back(oneData);
        groupEle = groupEle->NextSiblingElement("Property");
    }

    return true;
}

bool LoadGuildSkill(const char*resourcePath, GuildSkillConfigMap&configVec)
{

	configVec.clear();
	std::string pathname = std::string(resourcePath) + GUILD_SKILL;
	TiXmlDocument xmlDocument(pathname.c_str());
	if (!xmlDocument.LoadFile())
	{
		extend_warning(LOG_ERROR, "LoadGuildUpLevelConfig: load %s ERROR !!", pathname.c_str());
		return false;
	}

	TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

	TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bl);
	for (int i = 0; groupEle != NULL; i++)
	{
		LoopDoCheck(bl);
		GuildSkillConfig oneData;
		int m_skillID = convert_int(groupEle->Attribute("SkillID"),0);
		oneData.m_unlockShuYuanLevel = convert_int(groupEle->Attribute("UnlockShuYuanLevel"), 0);
		oneData.m_initLevel = convert_int(groupEle->Attribute("InitLevel"), 0);
		oneData.m_autoUnlock = convert_int(groupEle->Attribute("AutoUnlock"), 0)>0;
		oneData.m_skillType = static_cast<GUILD_SKILL_TYPE>(convert_int(groupEle->Attribute("SkillType"), 0));
		oneData.m_unlockCost= convert_int(groupEle->Attribute("UnlockCost"), 0);
		oneData.m_startStudyCost = convert_int(groupEle->Attribute("StartStudyCost"),0);
		configVec[m_skillID] = oneData;
		groupEle = groupEle->NextSiblingElement("Property");
	}

	return true;
}

bool LoadGuildSkillLevelUpConst(const char*resourcePath, GuildSkillLevelUpSpendMap&configVec)
{
	configVec.clear();

	std::string pathname = std::string(resourcePath) + GUILD_SKILL_LEVEL_UP;
	TiXmlDocument xmlDocument(pathname.c_str());
	if (!xmlDocument.LoadFile())
	{
		extend_warning(LOG_ERROR, "LoadGuildUpLevelConfig: load %s ERROR !!", pathname.c_str());
		return false;
	}

	TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

	TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bl);
	for (int i = 0; groupEle != NULL; i++)
	{
		LoopDoCheck(bl);
		int skillID = convert_int(groupEle->Attribute("SkillType"), 0);
		int level = convert_int(groupEle->Attribute("Level"), 0);		
		
		GuildSkillLevelUpSpend& oneData = configVec[skillID][level];

		// 货币消耗
		const char* costStr = convert_string(groupEle->Attribute("CurrencyCost"), 0);
		CVarList ret;
		StringUtil::SplitString(ret, costStr, ";");
		for (size_t i = 0; i < ret.GetCount(); i++)
		{
			CVarList ret2;
			StringUtil::SplitString(ret2, ret.StringVal(i), ",");
			if (ret2.GetCount() == 2)
			{
				oneData.m_vCurrency[ ret2.IntVal(0) ] = ret2.IntVal(1);
			}
		}
		// 物品消耗
		const char* itemStr = convert_string(groupEle->Attribute("ConsumeItem"), 0);
		CVarList retList;
		StringUtil::SplitString(retList, itemStr, ";");
		for (size_t i = 0; i < retList.GetCount(); i++)
		{
			CVarList ret2;
			StringUtil::SplitString(ret2, retList.StringVal(i), ":");
			if (ret2.GetCount() == 2)
			{
				oneData.m_vItems[ret2.StringVal(0)] = ret2.IntVal(1);
			}
		}

		groupEle = groupEle->NextSiblingElement("Property");
	}

	return true;
}

bool LoadGuildSkillLvUpExp(const char*resourcePath, GuildSkillLvExp&configVec)
{
	configVec.clear();

	std::string pathname = std::string(resourcePath) + GUILD_SKILL_STUDY;
	TiXmlDocument xmlDocument(pathname.c_str());
	if (!xmlDocument.LoadFile())
	{
		extend_warning(LOG_ERROR, "LoadGuildUpLevelConfig: load %s ERROR !!", pathname.c_str());
		return false;
	}

	TiXmlElement* objectEle = xmlDocument.RootElement();
	if (objectEle == NULL)
	{
		return false;
	}

	TiXmlElement* groupEle = objectEle->FirstChildElement("Property");
	// 循环保护
	LoopBeginCheck(bl);
	for (int i = 0; groupEle != NULL; i++)
	{
		LoopDoCheck(bl);
		GuildSkillLevelUpSpend oneData;
		GuildSkillLv info;
		int skillID = convert_int(groupEle->Attribute("SkillType"), 0);
		int level = convert_int(groupEle->Attribute("Level"), 0);
		info.m_shuYuanLv = convert_int(groupEle->Attribute("ShuYuanLevel"), 0);
		info.m_exp = convert_int(groupEle->Attribute("Exp"), 0);
		configVec[skillID][level] = info;
		groupEle = groupEle->NextSiblingElement("Property");
	}

	return true;
}