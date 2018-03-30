//--------------------------------------------------------------------
// 文件名:      EquipStrengthenModule.cpp
// 内  容:      装备强化
// 说  明:
// 创建日期:    2014年11月1日
// 创建人:        
// 修改人:		
//    :       
//--------------------------------------------------------------------
#include "EquipStrengthenModule.h"

#include "utils/extend_func.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/util_func.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "FsGame/CommonModule/FunctionEventModule.h"
#include "Define/GameDefine.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "EquipmentModule.h"
#include "CommonModule/ContainerModule.h"
#include "CommonModule/CommRuleModule.h"
#include "CommonModule/EnvirValueModule.h"

EquipStrengthenModule * EquipStrengthenModule::m_pInstance = NULL;

const char * EQUIP_STRENGTHEN_CONSUME	= "ini/Item/Stengthen/EquipStrengthen.xml";
const char * EQUIP_STRENGTHEN_SUIT = "ini/Item/Stengthen/EquipStrengthenSuit.xml";
const char * EQUIP_STRENGTHEN_LIMIT = "ini/Item/Stengthen/EquipStrengthenLimit.xml";
const char * EQUIP_STRENGTHEN_APPERANCE = "ini/Item/Stengthen/EquipStrengthenApper.xml";

inline int nx_reload_strengthen_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 如果指针存在，表明模块已创建
	if (NULL != EquipStrengthenModule::m_pInstance)
	{
		EquipStrengthenModule::m_pInstance->LoadConsumeRes(pKernel);
	}

	return 0;
}

//初始化
bool EquipStrengthenModule::Init(IKernel* pKernel)
{
    m_pInstance = this;

	//强化消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_STRENGTHEN, OnCustomEquipStrengthen);

	Assert(m_pInstance != NULL);

	if (!LoadRes(pKernel))
	{
		Assert(false);
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("EquipStrengthenModule", EquipStrengthenModule::ReloadStrengthenConfig);
#endif

	DECL_LUA_EXT(nx_reload_strengthen_config);

    return true;
}

//释放
bool EquipStrengthenModule::Shut(IKernel* pKernel)
{
	ReleaseResource(pKernel);
    return true;
}

// 加载资源
bool EquipStrengthenModule::LoadRes(IKernel* pKernel)
{
	if (!LoadConsumeRes(pKernel)
		|| !LoadStrSuitRes(pKernel)
		|| !LoadStrLimitRes(pKernel)
		|| !LoadStrApperRes(pKernel))
	{
		return false;
	}

	return true;
}

//加载消耗资源
bool EquipStrengthenModule::LoadConsumeRes(IKernel* pKernel)
{
	m_StrenthenConsumeVec.clear();

	std::string path = pKernel->GetResourcePath();

	std::string file = path + EQUIP_STRENGTHEN_CONSUME;

	CXmlFile xml(file.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(pKernel, "Load xml file Error :");
		extend_warning(pKernel, file.c_str());

		return false;
	}

	size_t sec_count = xml.GetSectionCount();

    LoopBeginCheck(a)
	for (size_t i = 0; i < sec_count; i++)
	{
	    LoopDoCheck(a)
	    
		const char * secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		const int str_level = ::atoi(secName);
		StrenthenConsume cfg(str_level);

		CommRuleModule::ParseConsumeVec(xml.ReadString(secName, "Capitals", ""), cfg.capitals);
		CommRuleModule::ParseConsumeVec(xml.ReadString(secName, "Items", ""), cfg.items);

		m_StrenthenConsumeVec.push_back(cfg);
	}

	return true;
}

// 加载强化限制配置
bool EquipStrengthenModule::LoadStrLimitRes(IKernel* pKernel)
{
	m_StrengthenLimitVec.clear();

	std::string path = pKernel->GetResourcePath();

	std::string file = path + EQUIP_STRENGTHEN_LIMIT;

	CXmlFile xml(file.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(pKernel, "Load xml file Error :");
		extend_warning(pKernel, file.c_str());

		return false;
	}

	LoopBeginCheck(a);
	for (size_t i = 0; i < (int)xml.GetSectionCount(); i++)
	{
		LoopDoCheck(a);

		const char * secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}
		const int level = StringUtil::StringAsInt(secName);
		StrengthenLimit cfg(level);
		xml.ReadList<int>(secName, "LimitArgs", ",", &cfg.color_to_limit);
		m_StrengthenLimitVec.push_back(cfg);
	}

	return true;
}

// 加载强化套装配置
bool EquipStrengthenModule::LoadStrSuitRes(IKernel* pKernel)
{
	m_StrengthenSuitList.clear();

	std::string path = pKernel->GetResourcePath();

	std::string file = path + EQUIP_STRENGTHEN_SUIT;

	CXmlFile xml(file.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(pKernel, "Load xml file Error :");
		extend_warning(pKernel, file.c_str());

		return false;
	}

	LoopBeginCheck(aa)
	for (size_t i = 0; i < (int)xml.GetSectionCount(); i++)
	{
		LoopDoCheck(aa)

		const char * secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		StrengthenSuit suit;
		suit.lower_str_lvl = xml.ReadInteger(secName, "StrLowerLvl", 0);
		suit.upper_str_lvl = xml.ReadInteger(secName, "StrUpperLvl", 0);
		suit.active_str_pkg = xml.ReadInteger(secName, "StrActivePkg", 0);

		m_StrengthenSuitList.push_back(suit);
	}

	return true;
}

// 加载强化外观配置
bool EquipStrengthenModule::LoadStrApperRes(IKernel* pKernel)
{
	m_StrengthenApperanceVec.clear();

	std::string path = pKernel->GetResourcePath();

	std::string file = path + EQUIP_STRENGTHEN_APPERANCE;

	CXmlFile xml(file.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(pKernel, "Load xml file Error :");
		extend_warning(pKernel, file.c_str());

		return false;
	}

	LoopBeginCheck(aa)
	for (size_t i = 0; i < (int)xml.GetSectionCount(); i++)
	{
		LoopDoCheck(aa)

		const char * secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		StrengthenApperance cfg;
		cfg.equip_id = xml.ReadString(secName, "EquipId", "");
		cfg.lower_lvl = xml.ReadInteger(secName, "StrLowerLvl", 0);
		cfg.uppper_lvl = xml.ReadInteger(secName, "StrUpperLvl", 0);
		cfg.str_apperance = xml.ReadInteger(secName, "StrApperance", 0);

		m_StrengthenApperanceVec.push_back(cfg);
	}

	return true;
}

// 释放配置文件
bool EquipStrengthenModule::ReleaseResource(IKernel* pKernel)
{
	m_StrengthenLimitVec.clear();
	m_StrengthenSuitList.clear();
	m_StrenthenConsumeVec.clear();
	m_StrengthenApperanceVec.clear();

	return true;
}

//player强化消息回调
int EquipStrengthenModule::OnCustomEquipStrengthen(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    if(m_pInstance == NULL)
    {
        return 0;
    }

	////装备强化开关
	//if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_ENHANCE_EQUIP, self))
	//{
	//	return 0;
	//}

	size_t nSize = args.GetCount();
	if (nSize < 3)
	{
		return 0;
	}


    //二级消息号
    int subMsg = args.IntVal(1);

    //装备栏ID
    int iEquipIndex = args.IntVal(2);
    if (iEquipIndex < 1 || iEquipIndex >= EQUIP_MAX_PLACEHOLDER)
    {
        return 0;
    }

    switch(subMsg)
    {
    case EQUIP_STRENGTHEN_ONCE:
        {
            m_pInstance->ExecuteStrengthenEquip(pKernel, self, args);

			// 强化套装检查
			m_pInstance->SuitCheck(pKernel, self);
        }
        break;
    case EQUIP_STRENGTHEN_AUTO:
        {
            m_pInstance->AutoStrengthenEquip(pKernel, self, args);

			// 强化套装检查
			m_pInstance->SuitCheck(pKernel, self);
        }
        break;
    default:
        break;
    }

    return 0;
}

// 重新加载装备强化配置
void EquipStrengthenModule::ReloadStrengthenConfig(IKernel* pKernel)
{
	EquipStrengthenModule::m_pInstance->LoadRes(pKernel);
}

// 强化套装检查
void EquipStrengthenModule::SuitCheck(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 取得强化最低等级
	const int min_str_lvl = GetMinStrenLvl(pKernel, self);
	if (min_str_lvl <= 0)
	{
		return;
	}

	// 取得对应属性包
	const int active_pkg = GetSuitActivePkg(min_str_lvl);
	if (active_pkg == 0)
	{
		return;
	}

	// 当前属性包
	const int cur_active_pkg = pSelfObj->QueryInt(FIELD_PROP_STREN_SUIT_PKG);
	if (active_pkg == cur_active_pkg)
	{
		return;
	}

	// 更新属性包
	pSelfObj->SetInt(FIELD_PROP_STREN_SUIT_PKG, active_pkg);

	CVarList cmd_msg;
	cmd_msg << COMMAND_EQUIP_STR_SUIT_PKG
		    << cur_active_pkg;
	pKernel->Command(self, self, cmd_msg);
}

// 取得最高强化等级
const int EquipStrengthenModule::QueryMaxStrenLvl(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equipbox))
	{
		return 0;
	}

	//强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}

	int max_str_lvl = 0;

	//遍历装备栏
	LoopBeginCheck(bb);
	for (int pos = EQUIP_POS_STR_MIN; pos <= EQUIP_POS_STR_MAX; ++pos)
	{
		LoopDoCheck(bb);

		// 根据@pos找到此装备在强化表中的位置
		int row = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, pos);
		if (row < 0)
		{
			continue;
		}

		// 强化等级
		const int str_lvl = pStrengthenRec->QueryInt(row, EQUIP_STRHENTHEN_LEVEL);

		// 取灵魂中最低的等级
		if (max_str_lvl < str_lvl)
		{
			max_str_lvl = str_lvl;
		}
	}

	return max_str_lvl;
}

// 取得强化消耗
bool EquipStrengthenModule::GetStrConsume(const int str_level, Consume_Vec& capitals, Consume_Vec& items)
{
	StrenthenConsume cfg(str_level);
	StrenthenConsumeVec::const_iterator find_it = find(
		m_StrenthenConsumeVec.begin(),
		m_StrenthenConsumeVec.end(),
		cfg);
	if (find_it == m_StrenthenConsumeVec.end())
	{
		return false;
	}

	capitals = find_it->capitals;
	items = find_it->items;
	return true;
}

// 取得强化限制等级
const int EquipStrengthenModule::GetLimitStrLevel(IKernel* pKernel, const PERSISTID& self, 
	int equip_index)
{
	if (equip_index < EQUIP_POS_STR_MIN 
		|| equip_index > EQUIP_POS_STR_MAX)
	{
		return 0;
	}

	if (!pKernel->Exists(self))
	{
		return 0;
	}

	PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equipbox))
	{
		return 0;
	}

	PERSISTID equip = pKernel->GetItem(equipbox, equip_index);
	IGameObj* pEquipObj = pKernel->GetGameObj(equip);
	if (pEquipObj == NULL)
	{
		return 0;
	}

	int equip_level = pEquipObj->QueryInt(FIELD_PROP_LIMIT_LEVEL);
	int equip_color = pEquipObj->QueryInt(FIELD_PROP_COLOR_LEVEL);


	if (!CommRuleModule::m_pThis->IsValidColor(equip_color))
	{
		return 0;
	}

	StrengthenLimit temp(equip_level);
	StrengthenLimitVec::const_iterator find_it = find(
		m_StrengthenLimitVec.begin(),
		m_StrengthenLimitVec.end(),
		temp);
	if (find_it == m_StrengthenLimitVec.end())
	{
		return 0;
	}

	// 取对应值，无值取最后位
	int idx = (int)find_it->color_to_limit.size() - 1;
	idx = min(equip_color, idx);
	
	return find_it->color_to_limit[idx];
}

// 执行强化操作
int EquipStrengthenModule::ExecuteStrengthenEquip(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 如果玩家不存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int iEquipIndex = args.IntVal(2);

	PERSISTID equip_box = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equip_box))
	{
		return 0;
	}

	// 取得选中的装备
	PERSISTID equip = pKernel->GetItem(equip_box, iEquipIndex);
	if (!pKernel->Exists(equip))
	{
		// 提示没有装备
		return 0;
	}

	// 取出强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}

	int iStrengthenRow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, iEquipIndex);
	if (iStrengthenRow == -1)
	{
		// 没有找到对应装备
		return 0;
	}

	// 强化等级
	int iEquipStrLv = pStrengthenRec->QueryInt(iStrengthenRow, EQUIP_STRHENTHEN_LEVEL);
	if (iEquipStrLv >= EnvirValueModule::EnvirQueryInt(ENV_VALUE_STRENGTHEN_MAX_LEVEL))
	{
		return 0;
	}

	// 装备的强化上限由装备等级和装备品质决定
	const int iLimitLv = GetLimitStrLevel(pKernel, self, iEquipIndex);
	if (iLimitLv == 0)
	{
		return 0;
	}
	
	if (iEquipStrLv >= iLimitLv)
	{
		// 强化已达到上限，无需再强化
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5101, CVarList());
		return 0;
	}

	// 获取当前等级的金钱和材料消耗
	Consume_Vec capitals;
	Consume_Vec items;	
	if (!GetStrConsume(iEquipStrLv + 1, capitals, items))
	{
		return 0;
	}

	// 材料是否够
	if (!CommRuleModule::CanDecItems(pKernel, self, items))
	{
		//材料不够
		return 0;
	}

	// 钱是否够
	if (!CommRuleModule::CanDecCapitals(pKernel, self, capitals))
	{
		//金钱不够
		return 0;
	}

	// 扣材料
	if (!CommRuleModule::ConsumeItems(pKernel, self, items, FUNCTION_EVENT_ID_STRENGTHE))
	{
		return 0;
	}

	// 扣钱
	if (!CommRuleModule::ConsumeCapitals(pKernel, self, capitals, FUNCTION_EVENT_ID_STRENGTHE))
	{
		//扣金钱失败
		return 0;
	}

    // 强化后的等级不能高于限制等级
	int iFinalStrLevel = iEquipStrLv + 1;
    if (iFinalStrLevel > iLimitLv)
    {
        iFinalStrLevel = iLimitLv;
    }

	pStrengthenRec->SetInt(iStrengthenRow, EQUIP_STRHENTHEN_LEVEL, iFinalStrLevel);

	// 强化成功
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_STRENTHEN << 1);

	// 强化等级变化
	pKernel->Command(self, self, CVarList() << COMMAND_EQUIP_STRENGTHEN_CHANGE_LEVEL << iEquipIndex << iEquipStrLv);

	//更新战斗力
	pKernel->Command(equip, self, CVarList() << COMMAND_EQUIP_STRENGTHEN_PKG << iEquipStrLv << iFinalStrLevel);

	//const char* itemId = pKernel->GetConfig(equip);

	//// 装备强化日志 modify by   (2015-04-23)
	//EquipLog strengLog;
	//strengLog.operateType    = LOG_EQUIP_STRENGTHEN;
	//strengLog.equipPos       = iEquipIndex;
	//strengLog.equipSlot      = EQUIP_STRHENTHEN_LEVEL;
 //   strengLog.beforeGrade    = iEquipStrLv;
	//strengLog.afterGrade     = iFinalStrLevel;
	//strengLog.sourceItemID = itemId;
 //   strengLog.deductItemIDs  = m_strStrMaterial.c_str();
	//strengLog.deductItemNums = util_int_as_string(iMaterialAmount).c_str();
	//strengLog.capitalType    = m_nStrCapitalType;
	//strengLog.capitalNum     = iCapitalValue;        
	//LogModule::m_pLogModule->SaveEquipLog(pKernel, self, strengLog);

	//LogModule::m_pLogModule->OnEquipStrengthen(pKernel, self, itemId, pKernel->QueryInt(equip, "ColorLevel"), iEquipStrLv, iFinalStrLevel, iCapitalValue, iEquipIndex);
	
	return 0;
}

// 取得指定部位的强化等级
const int EquipStrengthenModule::GetStrLvlByPos(IKernel* pKernel, const PERSISTID& self, 
	int target_pos)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 取出强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}

	int iStrengthenRow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, target_pos);
	if (iStrengthenRow == -1)
	{
		return 0;
	}

	// 当前强化等级
	return pStrengthenRec->QueryInt(iStrengthenRow, EQUIP_STRHENTHEN_LEVEL);	
}

// 取得强化外观
const int EquipStrengthenModule::GetStrApperance(const char *equip_id, const int str_lvl)
{
	StrengthenApperanceVec::const_iterator find_it = find_if(
		m_StrengthenApperanceVec.begin(),
		m_StrengthenApperanceVec.end(),
		StrengthenApperanceFinder(str_lvl, equip_id));
	if (find_it == m_StrengthenApperanceVec.end())
	{
		return 0;
	}
	return find_it->str_apperance;
}

// 取得最低强化等级
const int EquipStrengthenModule::GetMinStrenLvl(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equipbox))
	{
		return 0;
	}

	//强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}

	int min_str_lvl = 0;
	bool is_first = true;

	//遍历装备栏
	LoopBeginCheck(bb)
	for (int pos = EQUIP_POS_STR_MIN; pos <= EQUIP_POS_STR_MAX; ++pos)
	{
		LoopDoCheck(bb)

		// 根据@pos找到此装备在强化表中的位置
		int row = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, pos);
		if (row < 0)
		{
			continue;
		}

		// 强化等级
		const int str_lvl = pStrengthenRec->QueryInt(row, EQUIP_STRHENTHEN_LEVEL);
		if (is_first)
		{
			min_str_lvl = str_lvl;
			is_first = false;
		}

		// 取最低的等级
		if (min_str_lvl > str_lvl)
		{
			min_str_lvl = str_lvl;
		}
	}

	return min_str_lvl;
}

// 取得套装激发的属性包
const int EquipStrengthenModule::GetSuitActivePkg(const int min_str_lvl)
{
	StrengthenSuitList::const_iterator find_it = std::find_if(
		m_StrengthenSuitList.begin(), 
		m_StrengthenSuitList.end(), 
		StrengthenSuitFinder(min_str_lvl));

	if (find_it == m_StrengthenSuitList.end())
	{
		return 0;
	}

	return find_it->active_str_pkg;
}

//一键强化
int EquipStrengthenModule::AutoStrengthenEquip(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
    // 如果玩家不存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    //要强化的装备索引
    int iEquipIndex = args.IntVal(2);

    //取得选中的装备
    PERSISTID equip_box = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
    if (!pKernel->Exists(equip_box))
    {
        return 0;
    }
    PERSISTID equip = pKernel->GetItem(equip_box, iEquipIndex);
    if (!pKernel->Exists(equip))
    {
        // 提示没有装备
        return 0;
    }

    //取出强化表
    IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
    if (NULL == pStrengthenRec)
    {
        return 0;
    }
    int iStrengthenRow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, iEquipIndex);
    if (iStrengthenRow == -1)
    {
        // 没有找到对应装备
        return 0;
    }

	// 强化等级
	int iEquipStrLv = pStrengthenRec->QueryInt(iStrengthenRow, EQUIP_STRHENTHEN_LEVEL);
	if (iEquipStrLv >= EnvirValueModule::EnvirQueryInt(ENV_VALUE_STRENGTHEN_MAX_LEVEL))
	{
		return 0;
	}

	// 装备的强化上限由装备等级和装备品质决定
	const int iLimitLv = GetLimitStrLevel(pKernel, self, iEquipIndex);
	if (iLimitLv == 0)
	{
		return 0;
	}

    if (iEquipStrLv >= iLimitLv)
    {
        // 强化已达到上限，无需再强化
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5101, CVarList());
        return 0;
    }

    //背包
    PERSISTID tool_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
    if (!pKernel->Exists(tool_box))
    {
        return 0;
    }

    //新等级
    int newEquipStrLv = iEquipStrLv;

    //强化成功次数
    int autoStrengthenNum = 0;

    //这次一键强化总消耗的钱和物品数量
    int totalCapitalConsume = 0;
    int totalMaterialConsume = 0;

    //强化到5次
    LoopBeginCheck(as)
    for(int i = 0; i < iLimitLv; ++i)
    {
        LoopDoCheck(as)
        if (newEquipStrLv >= iLimitLv)
        {
            break;
        }

		if (i >= EnvirValueModule::EnvirQueryInt(ENV_VALUE_STRENGTHEN_AUTO_COUNT))
        {
            break;
        }

		// 获取当前等级的金钱和材料消耗
		Consume_Vec capitals;
		Consume_Vec items;
		if (!GetStrConsume(iEquipStrLv + 1, capitals, items))
		{
			break;
		}

		// 材料是否够
		if (!CommRuleModule::CanDecItems(pKernel, self, items))
		{
			//材料不够
			break;
		}

		// 钱是否够
		if (!CommRuleModule::CanDecCapitals(pKernel, self, capitals))
		{
			//金钱不够
			break;
		}

		// 扣材料
		if (!CommRuleModule::ConsumeItems(pKernel, self, items, FUNCTION_EVENT_ID_STRENGTHE))
		{
			break;
		}

		// 扣钱
		if (!CommRuleModule::ConsumeCapitals(pKernel, self, capitals, FUNCTION_EVENT_ID_STRENGTHE))
		{
			//扣金钱失败
			break;
		}

        autoStrengthenNum ++;

        //当前等级增加
		const int old_lvl = newEquipStrLv;
        newEquipStrLv += 1;
        if (newEquipStrLv > iLimitLv)
        {
            newEquipStrLv = iLimitLv;
        }

		pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_STRENTHEN << 1);
    }

    //强化没有成功
    if(newEquipStrLv == iEquipStrLv)
    {
        //这里就用返回提示了，因为前面提示过了
        return 0;
    }

    //设置强化等级
    pStrengthenRec->SetInt(iStrengthenRow, EQUIP_STRHENTHEN_LEVEL, newEquipStrLv);

	// 强化等级变化
	pKernel->Command(self, self, CVarList() << COMMAND_EQUIP_STRENGTHEN_CHANGE_LEVEL << iEquipIndex << iEquipStrLv);

    //更新战斗力
	pKernel->Command(equip, self, CVarList() << COMMAND_EQUIP_STRENGTHEN_PKG << iEquipStrLv << newEquipStrLv);

    // 一键装备强化日志 modify by   (2015-04-23)
  /*  EquipLog strengLog;
    strengLog.operateType    = LOG_EQUIP_STRENGTHEN_AUTO;
    strengLog.equipPos       = iEquipIndex;
    strengLog.equipSlot      = EQUIP_STRHENTHEN_LEVEL;
    strengLog.beforeGrade    = iEquipStrLv;
    strengLog.afterGrade     = newEquipStrLv;
    strengLog.sourceItemID   = pKernel->GetConfig(equip);
    strengLog.deductItemIDs  = m_strStrMaterial.c_str();
    strengLog.deductItemNums = util_int_as_string(totalMaterialConsume).c_str();
    strengLog.capitalType    = m_nStrCapitalType;
    strengLog.capitalNum     = totalCapitalConsume;        
    LogModule::m_pLogModule->SaveEquipLog(pKernel, self, strengLog);

	LogModule::m_pLogModule->OnEquipStrengthen(pKernel, self, pKernel->GetConfig(equip), pKernel->QueryInt(equip, "ColorLevel"), iEquipStrLv, newEquipStrLv, totalCapitalConsume, iEquipIndex);*/

    return 0;
}