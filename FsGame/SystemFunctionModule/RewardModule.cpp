//---------------------------------------------------------
//文件名:       RewardModule.h
//内  容:       发放奖励给玩家
//说  明:       
//          
//创建日期:      2015年03月30日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#include "RewardModule.h"

#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/PubDefine.h"


#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/SocialSystemModule/SystemMailModule.h"
#endif
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/CommonModule/LogModule.h"
//#include "FsGame/ItemModule/EquipmentModule.h"
#include "FsGame/ItemModule/ToolItem/ToolItemModule.h"

#include "utils/util_func.h"
#include "public/VarList.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "FsGame/CommonModule/LuaExtModule.h"


#include <vector>
#include "utils/custom_func.h"
#include "../Define/ToolBoxSysInfoDefine.h"
#include "CommonModule/FunctionEventModule.h"
//#include "Define/GuildDefine.h"

// 发送奖励:
int nx_reward(void* state)
{
//     IKernel* pKernel = LuaExtModule::GetKernel(state);
// 
//     if (NULL == RewardModule::m_pRewardInstance)
//     {
//         return 0;
//     }
// 
//     int param_cnt = pKernel->LuaGetArgCount(state);
//     
//     // 奖励内容
// 	RewardModule::AwardEx award;
// 	award.srcFunctionId = FUNCTION_EVENT_ID_GM_COMMAND;
//     award.name = "player_greeting";
//     award.exp = 10;
// 	award.capitals["CapitalCopper"] = 99;
// 	award.capitals["CapitalGold"] = 99;
// 	award.capitals["CapitalSmelt"] = 99;
// 	award.capitals["CapitalPublicOffice"] = 99;
// 
// 	RewardModule::ParseAddOneItem(award.items, "Equip10710032", 3);
// 	RewardModule::ParseAddOneItem(award.items, "BianStone2082014", 2);
//     
//     if (param_cnt == 1)
//     {
//         // 检查参数类型
//         CHECK_ARG_OBJECT(state, nx_reward, 1);
// 
//         PERSISTID player = pKernel->LuaToObject(state, 1);    
//         RewardModule::m_pRewardInstance->RewardPlayer(pKernel, player, &award);
//     }
//     else if (param_cnt == 2)
//     {
//         // 检查参数类型
//         CHECK_ARG_OBJECT(state, nx_reward, 1);
//         CHECK_ARG_WIDESTR(state, nx_reward, 2);
// 
//         const wchar_t * role_name = pKernel->LuaToWideStr(state, 2);
//         RewardModule::m_pRewardInstance->RewardByRoleName(pKernel, role_name, &award);
//     }
    
    return 1;
}

RewardModule* RewardModule::m_pRewardInstance = NULL;
ContainerModule* RewardModule::m_pContainerModule = NULL;
CapitalModule* RewardModule::m_pCapitalModule = NULL;
#ifndef FSROOMLOGIC_EXPORTS
SystemMailModule* RewardModule::m_pMailModule = NULL;
#endif
LevelModule* RewardModule::m_pLevelModule = NULL;

bool RewardModule::Init(IKernel* pKernel)
{
	m_pRewardInstance = this;
	
    m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
    m_pCapitalModule   = (CapitalModule*)pKernel->GetLogicModule("CapitalModule");
	m_pLevelModule      = (LevelModule*)pKernel->GetLogicModule("LevelModule");
#ifndef FSROOMLOGIC_EXPORTS
    m_pMailModule      = (SystemMailModule*)pKernel->GetLogicModule("SystemMailModule");
	Assert(m_pContainerModule != NULL && m_pCapitalModule != NULL
		&& m_pMailModule != NULL && m_pLevelModule != NULL);
#else
	Assert(m_pContainerModule != NULL && m_pCapitalModule != NULL
		&& m_pLevelModule != NULL);
#endif
    

	LoadAllRewardConfig(pKernel);

    DECL_LUA_EXT(nx_reward);
	return true;
}

bool RewardModule::Shut(IKernel* pKernel)
{
	return true;
}

bool RewardModule::TryRewardPlayer(IKernel *pKernel, const PERSISTID &player, int nAwardId, int operation_on_full)
{
	// 直接发邮件
	if (REWARD_MAIL == operation_on_full)
	{
		return true;
	}

	const AwardEx *award = m_pRewardInstance->QueryAwardById(nAwardId);
	if (NULL == award)
	{
		return false;
	}
	// 看看背包能否放下
	int nBindState = FunctionEventModule::GetItemBindState(award->srcFunctionId);
	// 背包放不下,直接失败
	if (!m_pRewardInstance->CanPutInToolBox(pKernel, player, award->items, nBindState) && operation_on_full == REWARD_FAILED_ON_FULL)
	{
		return false;
	}

	return true;
}

// 发奖励发给玩家
// 当 @operation_on_full = REWARD_MAIL_ON_FULL 的时候, 会通过使用预定义的邮件发给玩家.
// 其中, @award.name 是邮件模板的名字, 如果邮件模板中有参数，则通过@mail_param 传送
bool RewardModule::RewardPlayerById(IKernel *pKernel, const PERSISTID &player, 
	int nRewardId, int operation_on_full, const IVarList &mail_param)
{
	// 直接发邮件
	IGameObj* pDcObj = pKernel->GetGameObj(player);
	const AwardEx* award = m_pRewardInstance->QueryAwardById(nRewardId);
	if (NULL == pDcObj || NULL == award)
	{
		return false;
	}

	return RewardPlayer(pKernel, player, award, mail_param, operation_on_full);
}


// 直接奖励给角色
// 当 @operation_on_full = REWARD_MAIL_ON_FULL 的时候, 会通过使用预定义的邮件发给玩家.
// 其中, @award.name 是邮件模板的名字, 如果邮件模板中有参数，则通过@mail_param 传送
// bool RewardModule::RewardByRoleName(IKernel *pKernel, const wchar_t *role_name, int nAwardId, const IVarList &mail_param)
// {   
// 	// 直接邮件发送
// 	if (operation_on_full == REWARD_MAIL)
// 	{
// 		// 邮件发送
// 		return InnerRewardViaMail(pKernel, role_name, award, mail_param);
// 	}
// 
// 	// 查找玩家
// 	PERSISTID player = pKernel->FindPlayer(role_name);
// 	if (pKernel->Exists(player))
// 	{
// 		// 玩家在当前场景, 直接奖励给玩家
// 		RewardPlayer(pKernel, player, award, mail_param, operation_on_full);
// 	}
// 	else
// 	{
// 		// 玩家不再当前场景, 通过邮件发送奖励
// 		InnerRewardViaMail(pKernel, role_name, award, mail_param);
// 	}
// 	
// 	return true;
// }

// 通过邮件发奖励给玩家
bool RewardModule::RewardViaMail(IKernel *pKernel, const wchar_t *role_name, int nAwardId, const IVarList &mail_param, const wchar_t *sender_name /*= L""*/)
{
	const AwardEx* pAwardData = m_pRewardInstance->QueryAwardById(nAwardId);
	if (NULL == pAwardData)
	{
		return false;
	}

	return m_pRewardInstance->InnerRewardViaMail(pKernel, role_name, pAwardData, mail_param, sender_name);
}

// 将奖励物品串解析为奖励数据
// 格式：  "CapitalCopper:10,CapitalGold:10,CapitalSmelt:10,CapitalPublicOffice:10,CapitalBattleSoul:10,CapitalBlackShop:10,CapitalGuild:10"
bool RewardModule::GenerateAwardData(const std::string& strAward, RewardModule::AwardEx& award)
{
	// 安全检测
	if (strAward.empty())
	{
		return false;
	}

	// 分隔字符串
	CVarList result;
	util_split_string(result, strAward, ",");

	// 分隔后的字符串个数
	int iCount = (int)result.GetCount();
	if (iCount == 0)
	{
		return false;
	}

	// 循环保护
	LoopBeginCheck(fh);
	for (int i = 0; i < iCount; ++i)
	{
		LoopDoCheck(fh);

		// 取分隔后的子串
		std::string strTmp = result.StringVal(i);

		// 再次进行分隔 物品名称 数量
		CVarList info;
		util_split_string(info, strTmp, ":");
		if (info.GetCount() < 2)
		{
			continue;
		}

		// 名字
		std::string name = info.StringVal(0);

		int nCapitalType = m_pCapitalModule->GetCapitalType(name);
		// 说明是货币
		if (nCapitalType >= CAPITAL_MIN_TYPE)
		{
			ParseAddOneAwardData(strTmp.c_str(), CAPITAL_AWARD, award);
		}
		else
		{
			if (StringUtil::CharIsEqual(name.c_str(), STR_LEVEL_EXP))
			{
				// 经验
				ParseAddOneAwardData(strTmp.c_str(), EXP_AWARD, award);
			}
			else
			{
				// 物品
				ParseAddOneAwardData(strTmp.c_str(), ITEM_AWARD, award);
			}
		}
	}

	return true;
}

bool RewardModule::RewardPlayer(IKernel *pKernel, const PERSISTID &player, const AwardEx *award, const IVarList &mail_param, int operation_on_full /*= REWARD_MAIL_ON_FULL*/)
{
	// 直接发邮件
	IGameObj* pDcObj = pKernel->GetGameObj(player);
	Assert(pDcObj != NULL);

	if (REWARD_MAIL == operation_on_full)
	{
		// 通过邮件发给玩家
		return m_pRewardInstance->InnerRewardViaMail(pKernel, pDcObj->QueryWideStr("Name"), award, mail_param);
	}

	// 看看背包能否放下
	int nBindState = FunctionEventModule::GetItemBindState(award->srcFunctionId);
	if (!m_pRewardInstance->CanPutInToolBox(pKernel, player, award->items, nBindState))
	{
		// 背包放不下,直接失败
		if (operation_on_full == REWARD_FAILED_ON_FULL)
		{
			// 背包空间不足
			::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_7004, CVarList());
			return false;
		}
		// lihl: 无法放入背包 给出系统提示
		// 背包空间不足
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_7010, CVarList());

		// 背包无法放下, 只能通过邮件发给玩家
		m_pRewardInstance->InnerRewardViaMail(pKernel, pDcObj->QueryWideStr("Name"), award, mail_param);
		// 把经验加给玩家
		if (!award->exps.empty())
		{
			m_pRewardInstance->AddExp(pKernel, player, award->exps, award->srcFunctionId);
		}
	}
	else
	{
		// 直接奖励给玩家
		m_pRewardInstance->InnerRewardDirectly(pKernel, player, award);
	}

	return true;
}

// 通过邮件发送奖励
bool RewardModule::InnerRewardViaMail(IKernel *pKernel, const wchar_t *role_name, const AwardEx* award, const IVarList &mail_param, const wchar_t *sender_name)
{
#ifndef FSROOMLOGIC_EXPORTS
    // 根据奖励的名字，获取邮件模板
    const ConfigMailItem& mail_template = m_pMailModule->GetConfigMail(pKernel, award->name);

    // 邮件中的附件
    bool has_attachment = false;
    std::string attachment; // 附件包括资金和道具

    // 资金
    if (award->capitals.size() > 0)
    {
        has_attachment = true;
		attachment.append(award->strCapitals);
		attachment.append(";");
    }
	else
	{
		attachment.append(";");
	}
    
    if (award->items.size() > 0)
    {
        has_attachment = true;
		attachment.append(award->strItems);
		attachment.append(";");
    }
    
    if (!has_attachment)
    {
        attachment = mail_template.items;
    }

    // 邮件的内容
    std::wstring mail_content = mail_template.mail_content;
    if (mail_param.GetCount() > 0)
    {
        mail_content = SystemMailModule::ComposeMailContent(pKernel, mail_template.mail_content.c_str(), 
            mail_param);
    }
    if (StringUtil::CharIsNull(sender_name))
    {
        sender_name = mail_template.sender.c_str();
    }

    m_pMailModule->SendMailToPlayer(pKernel, sender_name,
									role_name,
									mail_template.mail_title.c_str(),
									mail_content.c_str(),
									attachment, (EmFunctionEventId)award->srcFunctionId);

	// 邮件日志记录
	MailLog mailLotLog;
	mailLotLog.name = role_name;
	mailLotLog.templateType = award->name;
	mailLotLog.itemParams = award->strItems;
	mailLotLog.capitalParams = award->strCapitals;

	LogModule::m_pLogModule->SaveMailAwardLog(pKernel, mailLotLog);
#endif
   return true;
}

// 直接奖励给玩家
bool RewardModule::InnerRewardDirectly(IKernel *pKernel, const PERSISTID &player, const AwardEx *award)
{
    // 把道具放入背包中
	int nBindState = FunctionEventModule::GetItemBindState(award->srcFunctionId);
	PutInToolBox( pKernel, player, award->items, award->srcFunctionId, nBindState);

    // 把资金加给玩家
    AddCapital(pKernel, player, award->capitals, award->srcFunctionId);
    
    // 把经验加给玩家
	AddExp(pKernel, player, award->exps, award->srcFunctionId);
    return true;
}

// 增加一项奖励 nAwardType (对应AwardType)
void RewardModule::ParseAddOneAwardData(const char* strAward, int nAwardType, RewardModule::AwardEx& award)
{
	// 格式: type:num
	CVarList single_item;
	util_split_string(single_item, strAward, ":");
	if (single_item.GetCount() != 2)
	{
		return;
	}

	AwardData data;
	data.strType = single_item.StringVal(0);
	data.nAddVal = single_item.IntVal(1);

	if (nAwardType == CAPITAL_AWARD)
	{
		award.capitals.push_back(data);
	}
	else if (nAwardType == ITEM_AWARD)
	{
		award.items.push_back(data);
	}
	else
	{
		award.exps.push_back(data);
	}
}

bool RewardModule::ItemsToString(const AwardVec& items_list, std::string& strItems, const char* pszSp1/* = ","*/, const char* pszSp2/* = ":"*/)
{
	for (AwardVec::const_iterator itr = items_list.begin(); itr != items_list.end(); ++itr )
	{
		if (strItems.size() > 0)
		{
			strItems.append(pszSp1);
		}
		strItems.append(itr->strType);
		strItems.append(pszSp2);
		strItems.append( StringUtil::IntAsString(itr->nAddVal) );
	}
	return true;
}

const RewardModule::AwardEx* RewardModule::QueryAwardById(int nAwardId)
{
	AwardMap::iterator iter = m_mapAllAwardData.find(nAwardId);
	if (iter == m_mapAllAwardData.end())
	{
		return NULL;
	}

	return iter->second;
}

// 经验可以直接加给玩家
bool RewardModule::AddExp(IKernel *pKernel, const PERSISTID &player, const AwardVec& exp_list, int exp_from)
{
	int nSize = (int)exp_list.size();
	LoopBeginCheck(u);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(u);
		const AwardData& exp = exp_list[i];
		if (StringUtil::CharIsEqual(exp.strType.c_str(), STR_LEVEL_EXP))
		{
			m_pLevelModule->AddExp(pKernel, player, exp_from, exp.nAddVal);
		}
	}
    return true;
}

// 资金可以直接加给玩家, @capital 的格式：capital_name:数量,capital_name:数量,
bool RewardModule::AddCapital(IKernel *pKernel, const PERSISTID &player,
					const AwardVec &capital_list, EmFunctionEventId capitalEventID)
{    
	if (capital_list.empty())
    {
        return true;
    }
        
    LoopBeginCheck(a)
	for (AwardVec::const_iterator itr = capital_list.begin(); itr != capital_list.end(); ++itr)
    {
        LoopDoCheck(a)

		int capital_type = m_pCapitalModule->GetCapitalType( itr->strType.c_str() );
		m_pCapitalModule->IncCapital(pKernel, player, capital_type, itr->nAddVal, capitalEventID);
    }
    
    return true;
}

// 增加帮会贡献最大值只用来记录
// bool RewardModule::AddGuildDevote(IKernel *pKernel, const PERSISTID &player, int64_t devoteValue)
// {
// 	IGameObj* pSelfObj = pKernel->GetGameObj(player);
// 	if (pSelfObj == NULL)
// 	{
// 		return false;
// 	}
// 
// 	// 帮会名称
// 	const wchar_t* strGuildName = pSelfObj->QueryWideStr("GuildName");
// 	if (StringUtil::CharIsNull(strGuildName))
// 	{
// 		return false;
// 	}
// #ifndef FSROOMLOGIC_EXPORTS
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD 
// 		<< strGuildName 
// 		<< SP_GUILD_ADD_GUID_DEVOTE_MAX
// 		<< pSelfObj->QueryWideStr("Name") 
// 		<< (int)devoteValue;
// 	pKernel->SendPublicMessage(pubMsg);
// #endif
// 	return true;
// }

void RewardModule::ShowItem(IKernel*pKernel, const PERSISTID &player, const std::string& awardStr)
{

	if (StringUtil::CharIsNull(awardStr.c_str()))
	{
		return;
	}
	
	std::map<std::string, int> itemList;
	// 给客户端发送的参数
	CVarList argsItems;
	CVarList res1;
	StringUtil::SplitString(res1, awardStr, ",");
	for (size_t i = 0; i < res1.GetCount();i++)
	{
		CVarList res2;
		StringUtil::SplitString(res2, res1.StringVal(i), ":");

		const char* itemID = res2.StringVal(0);
		int itemNum = res2.IntVal(1);
		if (StringUtil::CharIsNull(itemID) ||itemNum == 0){
			continue;
		}
		if (!ToolItemModule::IsCanWearItem(pKernel, itemID))
		{
			itemList[itemID] += itemNum;
		}
		else
		{
			
			argsItems << itemID << itemNum;
		}
	}

	for (auto it : itemList){
		
		argsItems << it.first << it.second;
	};

	if (argsItems.GetCount() == 0)
	{
		return;
	}

	CVarList args;
	args << SERVER_CUSTOMMSG_GET_ITEM
		<< S2C_SHOW_ITEM_LIST
		<<(int) argsItems.GetCount()
		<< argsItems;
	pKernel->Custom(player, args);
}

// 读取全部奖励配置
bool RewardModule::LoadAllRewardConfig(IKernel *pKernel)
{
	std::string strAwardPath = pKernel->GetResourcePath();
	strAwardPath += "ini/SystemFunction/Reward/";

	std::string strAwardFilepath = strAwardPath + "PathConfig.xml";
	CXmlFile xml(strAwardFilepath.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[RewardModule::LoadRewardConfig PathConfig.xml] failed");
		return false;
	}

	// 读取路径配置
	int nSecCount = (int)xml.GetSectionCount();
	LoopBeginCheck(c);
	for (int i = 0; i < nSecCount;++i)
	{
		LoopDoCheck(c);
		const char *section = xml.GetSectionByIndex(i);

		std::string strXmlName = xml.ReadString(section, "Path", "");
		std::string strPath = strAwardPath + strXmlName;

		LoadRewardConfig(pKernel, strPath.c_str());
	}

	return true;
}

// 读取配置
bool RewardModule::LoadRewardConfig(IKernel *pKernel, const char* strPath)
{
	CXmlFile xml(strPath);

	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_ERROR, "[RewardModule::LoadRewardConfig %s] failed", strPath);
		return false;
	}

	// xml读取
	LoopBeginCheck(b);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(b);
		const char *section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		int id = StringUtil::StringAsInt(section);

		// 一条奖励信息
		AwardEx* pData = NEW AwardEx; 
		if (NULL == pData)
		{
			return false;
		}

		pData->name = xml.ReadString(section, "MailName", "");

		pData->strCapitals = xml.ReadString(section, "Capitals", "");
		ParseAddAwardData(pData->strCapitals.c_str(), pData->capitals);

		pData->strItems = xml.ReadString(section, "Items", "");
		ParseAddAwardData(pData->strItems.c_str(), pData->items);

		const char* strExp = xml.ReadString(section, "AddExp", "");
		ParseAddAwardData(strExp, pData->exps);

		pData->srcFunctionId = (EmFunctionEventId)xml.ReadInteger(section, "FunctionEventId", -1);

		AwardMap::iterator iter = m_mapAllAwardData.find(id);
		if (iter != m_mapAllAwardData.end())
		{
			extend_warning(LOG_WARNING, "LoadRewardConfig %s, exist repeat id %d", strPath, id);
			continue;
		}

		m_mapAllAwardData.insert(std::make_pair(id, pData));
	}

	return true;
}

// 是否可以保存到背包中
bool RewardModule::CanPutInToolBox(IKernel *pKernel, const PERSISTID &player, const AwardVec &items_list, int nBindState)
{      
	if (items_list.size() == 0)
    {
        return true;
    }
        
    // 获取背包
    PERSISTID tool_box = pKernel->GetChild(player, ITEM_BOX_NAME_WSTR);

    if (!pKernel->Exists(tool_box))
    {
        return false;
    }
    
    VectorBoxTestItem try_items;
    
    int index = -1;
    
    LoopBeginCheck(a)
	for (AwardVec::const_iterator itr = items_list.begin(); itr != items_list.end(); ++itr)
    {
        LoopDoCheck(a)
        
        SBoxTestItem temp_item;
		temp_item.config_id = itr->strType;
		temp_item.amount = itr->nAddVal;
		temp_item.bind_status = nBindState;
        try_items.push_back(temp_item);
    }
    
	if (m_pContainerModule->TryAddItems(pKernel, tool_box, &try_items))
	{
		return true;
	}

	// 放入临时背包
// 	if (bUseTempBag)
// 	{
// 		// 获取背包
// 		PERSISTID temp_box = pKernel->GetChild(player, TEMPORARY_BAG_BOX_NAME_WSTR);
// 		if (!pKernel->Exists(temp_box))
// 		{
// 			return false;
// 		}
// 
// 		if (m_pContainerModule->TryAddItems(pKernel, temp_box, &try_items))
// 		{
// 			return true;
// 		}
// 	}

	return false;
}

// 放入背包中
bool RewardModule::PutInToolBox(IKernel *pKernel, const PERSISTID &player, const AwardVec& item_list, EmFunctionEventId itemEventID, int nBindState)
{
	if (item_list.size() == 0)
    {
        return true;
    }

    VectorBoxTestItem try_items; 
    LoopBeginCheck(a)
	for (AwardVec::const_iterator itr = item_list.begin(); itr != item_list.end(); ++itr)
    {
        LoopDoCheck(a)
        
        SBoxTestItem temp_item;
		temp_item.config_id = itr->strType;
		temp_item.amount = itr->nAddVal;
		temp_item.bind_status = nBindState;

        try_items.push_back(temp_item);
    }

// 	if (bUseTempBag)
// 	{
// 		return ContainerModule::PlaceItemsToBagOrTemp(pKernel, player, &try_items, itemEventID, true);
// 	}

	// 获取背包
	PERSISTID tool_box = pKernel->GetChild(player, ITEM_BOX_NAME_WSTR);

	if (!pKernel->Exists(tool_box))
	{
		return false;
	}
	return m_pContainerModule->PlaceItems(pKernel, tool_box, &try_items, itemEventID, true); 
}

// 将奖励物品串解析为奖励数据
// 格式：  "CapitalCopper:10,CapitalGold:10,CapitalSmelt:10,CapitalPublicOffice:10,CapitalBattleSoul:10,CapitalBlackShop:10,CapitalGuild:10"
void RewardModule::ParseAddAwardData(const char* strAwardData, AwardVec& vecAward)
{
	// name:amount,...name:amount
	CVarList temp_award_list;
	util_split_string(temp_award_list, strAwardData, ",");

	int temp_count = (int)(temp_award_list.GetCount());

	vecAward.reserve(temp_count);
	LoopBeginCheck(a)
	for (int i = 0; i < temp_count; ++i)
	{
		LoopDoCheck(a)

		const char *temp_capital = temp_award_list.StringVal(i);
		CVarList single_award;
		util_split_string(single_award, temp_capital, ":");
		if (single_award.GetCount() != 2)
		{
			continue;
		}

		AwardData data;
		data.strType = single_award.StringVal(0);
		data.nAddVal = single_award.IntVal(1);

		vecAward.push_back(data);
	}
}

// 将物品串中其他职业的物品排除
// 格式：  "ConfigID:数量,ConfigID:数量"
// bool RewardModule::FilterAwardByJob(IKernel* pKernel, const PERSISTID& self, AwardVec& items)
// {
// 	
// 	// 玩家对象
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	if (pSelfObj == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//玩家职业
// 	int job = pSelfObj->QueryInt("Job");
// 	return FilterAwardByJob(pKernel, job, items);
// }
// 
// bool RewardModule::FilterAwardByJob(IKernel* pKernel, int job, AwardVec& items)
// {
// 	// 安全检测
// 	if (EquipmentModule::m_pEquipmentModule == NULL)
// 	{
// 		return false;
// 	}
// 
// 	// 循环保护
// 	LoopBeginCheck(fh);
// 	for (AwardVec::iterator itr = items.begin(), next = itr; itr != items.end(); itr = next)
// 	{
// 		++next;
// 		LoopDoCheck(fh);
// 		// 名字
// 		const char* pszItemId = itr->strType.c_str();
// 		std::string itemScript = pKernel->GetConfigProperty(pszItemId, "Script");
// 
// 		//是装备
// 		if (itemScript == "Equipment")
// 		{
// 			// 检测是否是此职业的装备
// 			int jobLimit = StringUtil::StringAsInt(pKernel->GetConfigProperty(pszItemId, "ClassLimit"));
// 			if (!EquipmentModule::m_pEquipmentModule->EquipIsSameJob(jobLimit, job))
// 			{
// 				next = items.erase(itr);
// 			}
// 		}
// 	}
// 
// 	return true;
// }