//--------------------------------------------------------------------
// 文件名:      JewelModule.cpp
// 内  容:      宝石模块
// 说  明:		
// 创建日期:    2014年12月5日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------


#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/GameDefine.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "CommonModule/FunctionEventModule.h"
#include "JewelModule.h"
#include "EquipDefine.h"
#include "SystemFunctionModule/CapitalModule.h"


#define TEXT_INLAY_CHARGE        "MountCost"        // 镶嵌费用
#define TEXT_DISMANTLE_CHARGE    "ExtirpateCost"    // 拆除费用

const char* const JEWEL_USE_POS_XML = "ini/Item/Jewel/JewelUsePos.xml";	// 使用部位
const char* const JEWEL_XML = "ini/Item/Jewel/Jewel.xml";	// 关联表

JewelModule*		JewelModule::m_pInstance = NULL;
ContainerModule*	JewelModule::m_pContainerModule = NULL;
CapitalModule*		JewelModule::m_pCapitalModule = NULL;
AsynCtrlModule *	JewelModule::m_pAsynCtrlModule = NULL;

//初始化
bool JewelModule::Init(IKernel* pKernel)
{
    m_pInstance = this;

    m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pCapitalModule = (CapitalModule*)pKernel->GetLogicModule("CapitalModule");
	m_pAsynCtrlModule = (AsynCtrlModule *)pKernel->GetLogicModule("AsynCtrlModule");

    Assert(m_pContainerModule && m_pAsynCtrlModule  && m_pCapitalModule);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_JEWEL, JewelModule::OnCustomJewel);

	if (!LoadResource(pKernel))
	{
		return false;
	}

	if (!LoadJewelIndexCfg(pKernel))
	{
		return false;
	}

	RELOAD_CONFIG_REG("JewelModule", JewelModule::ReloadJewelConfig);
    return true;
}

// 加载资源
bool JewelModule::LoadResource(IKernel* pKernel)
{
	m_JewelUsePosVec.clear();
	// 配置文件路径
	std::string file_path = pKernel->GetResourcePath();
	file_path += JEWEL_USE_POS_XML;

	// 加载配置文件
	CXmlFile xml(file_path.c_str());

	if (!xml.LoadFromFile())
	{
		std::string err_msg = file_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		// ID
		int jewel_type = StringUtil::StringAsInt(secName);

		JewelUsePos use_pos(jewel_type);
		xml.ReadList<int>(secName, "UseEquipPos", ",", &use_pos.equip_pos_list);
		m_JewelUsePosVec.push_back(use_pos);
	}

	return true;
}

// 加载宝石索引映射配置
bool JewelModule::LoadJewelIndexCfg(IKernel* pKernel)
{
	m_JewelIndexMap.clear();
	// 配置文件路径
	std::string file_path = pKernel->GetResourcePath();
	file_path += JEWEL_XML;

	// 加载配置文件
	CXmlFile xml(file_path.c_str());

	if (!xml.LoadFromFile())
	{
		std::string err_msg = file_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		// Index
		const int index = xml.ReadInteger(secName, "Index", 0);

		m_JewelIndexMap.insert(JewelIndexMap::value_type(index, secName));
	}

	return true;
}

//释放
bool JewelModule::Shut(IKernel* pKernel)
{
	m_JewelUsePosVec.clear();
	m_JewelIndexMap.clear();
    return true;
}

//宝石相关消息
int JewelModule::OnCustomJewel(IKernel* pKernel, const PERSISTID& self,
							   const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (m_pAsynCtrlModule->IsWaiting(pKernel, self))
	{
		return 0;
	}

	if (pSelfObj->QueryInt("Dead") > 0)
	{
		return 0;
	}

	int iType = args.IntVal(1);

	if (iType == C2S_JEWEL_SUBMSG_INLAY)
	{
		return m_pInstance->ExecuteJewelInlay(pKernel, self, args);
	}
	else if (iType == C2S_JEWEL_SUBMSG_DISMANTLE)
	{
		return m_pInstance->DismantleJewel(pKernel, self, args);
	}
	else if (iType == C2S_JEWEL_SUBMSG_COMPOSE)
	{
		return m_pInstance->ComposeJewel(pKernel, self, args);
	}

	return 0;
}

// 宝石镶嵌
int JewelModule::ExecuteJewelInlay(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 只按等级来
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

 	int srcviewid = VIEWPORT_ITEM_BOX;	// 移出的容器
	int srcpos = args.IntVal(2);		// 移出的物品位置
	int equipindex = args.IntVal(3);	// 移入的装备格子的索引(1-10)
	int jewelindex = args.IntVal(4);	// 移入的宝石格子的索引(1-4)
		
	// 为了和宝石列对应上
	jewelindex += EQUIP_STRHENTHEN_JEWEL1-1;

	// 只能从道具栏往宝石栏移
	if (equipindex < EQUIP_POS_STR_MIN || 
		equipindex > EQUIP_POS_STR_MAX ||
		jewelindex < EQUIP_STRHENTHEN_JEWEL1 ||
		jewelindex > EQUIP_STRHENTHEN_JEWEL4)
	{
		return 0;
	}

	// 先拆下
	if (!Dismantle(pKernel, self, args.IntVal(3), args.IntVal(4)))
	{
		return 0;
	}
	
	// 获得客户端视窗对应服务器的容器编号
	PERSISTID srcbox = pKernel->GetViewportContainer(self, srcviewid);
	PERSISTID equbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(srcbox) || !pKernel->Exists(equbox))
	{
		return 0;
	}

	PERSISTID equip = pKernel->GetItem(equbox, equipindex);
	if (!pKernel->Exists(equip))
	{
		return 0;
	}

	// 判断位置是否正确
	if (srcpos <= 0 || srcpos > pKernel->GetCapacity(srcbox))
	{
		return 0;
	}

	// 判断是否可以操作容器
	if (!m_pContainerModule->CanOperateContainer(pKernel, self, srcbox, CONTAINER_OPTYPE_REMOVE, srcpos))
	{
		return 0;
	}

	// 获得对应的物品
	PERSISTID srcitem = pKernel->GetItem(srcbox, srcpos);
	IGameObj* pSrcItemObj = pKernel->GetGameObj(srcitem);
	if (pSrcItemObj == NULL)
	{ 
		return 0;
	}

	int amount = pSrcItemObj->QueryInt(FIELD_PROP_AMOUNT);
	if (amount < 1)
	{
		return 0;
	}

	// 检查是否是宝石
	const char * script = pKernel->GetScript(srcitem);
	if (::strcmp(script, CLASS_NAME_JEWEL) != 0)
	{
		return 0;
	}

	const int jewel_type = pKernel->QueryInt(srcitem, FIELD_PROP_ITEM_TYPE);
	if (!CanUseJewel(jewel_type, equipindex))
	{
		// 无法镶嵌此类型宝石
		/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, 
			STR_JEWEL_SYSINFO_JEWEL_POS_ERROR, CVarList());*/
		return 0;
	}
	
	// 检查宝石镶嵌的等级限制
	int level_required = pSrcItemObj->QueryInt(FIELD_PROP_UP_LEVEL);
	int player_level = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	if (level_required > player_level)
	{
       /* CVarList info_args;
        info_args << level_required;
	    ::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_UP_LOW_LEVEL, info_args);*/

	    return 0;
	}

	// 强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}

	int equiprow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, equipindex);
	if (equiprow == -1)
	{
		return 0;
	}

	// 旧的宝石的类型
	int oldJewelType = 0;
	std::string strOldJewel = pStrengthenRec->QueryString(equiprow, jewelindex);

	int nBindState = FunctionEventModule::GetItemBindState(FUNCTION_EVENT_ID_JEWEL_INLAY);
	if (!StringUtil::ItemIsNull(strOldJewel.c_str()))
	{
		const char * old_jewel_type = pKernel->GetConfigProperty(strOldJewel.c_str(), FIELD_PROP_ITEM_TYPE);
		if (!StringUtil::ItemIsNull(old_jewel_type))
		{
			oldJewelType = ::atoi(old_jewel_type);
		}        

		// 包裹是否满
		if (m_pContainerModule->TryAddItem(pKernel, srcbox, strOldJewel.c_str(), 1, nBindState) < 1)
		{
			// 包裹栏满
			//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_TOOLFULL, CVarList());

			return 0;
		}
	}

	// 宝石类型
	int newJewelType = pSrcItemObj->QueryInt(FIELD_PROP_ITEM_TYPE);

	if (/*oldJewelType != 0 && 
		oldJewelType != newJewelType &&*/
		JewelHaveSame(pKernel, self, equiprow, jewelindex, newJewelType))
	{
		//有相同类型宝石
		//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_SAME_TYPE, CVarList());

		return 0;
	}
	
    // 宝石镶嵌扣钱
    int inlayFees = pSrcItemObj->QueryInt(FIELD_PROP_MOUNT_COST);
    __int64 currentCapital = m_pCapitalModule->GetCapital(pKernel, self, CAPITAL_COPPER);
    if (inlayFees > currentCapital)
    {
        // 资金不足
        //::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_UP_NOTCAPITAL, CVarList());
        return 0;        
    }

	int decCapitalRet = m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_COPPER, inlayFees, FUNCTION_EVENT_ID_JEWEL_INLAY);
    if (decCapitalRet != DC_SUCCESS)
    {
        return 0;
    }

	// 镶嵌宝石ID
	std::string strAddJewel = pKernel->GetConfig(srcitem);
	
	// 获取宝石等级
	int nJewelLevel = pSrcItemObj->QueryInt(FIELD_PROP_LEVEL);
	int nColor = pSrcItemObj->QueryInt(FIELD_PROP_COLOR_LEVEL);

	// 先删除一个宝石
	pSrcItemObj->SetInt(FIELD_PROP_AMOUNT, amount - 1);

	// 添加镶嵌宝石ID
	pStrengthenRec->SetString(equiprow, jewelindex, strAddJewel.c_str());

	// 是否有宝石，有则是替换，创建一个放包裹里
	if (!StringUtil::ItemIsNull(strOldJewel.c_str()))
	{
		//包裹创建宝石
		m_pContainerModule->PlaceItem(pKernel, srcbox, 
			strOldJewel.c_str(), 1, FUNCTION_EVENT_ID_JEWEL_INLAY, false, nBindState);
	}

	// 更新宝石属性
	pKernel->Command(self, self, 
		CVarList() << COMMAND_EQUIPMENT_JEWEL_PKG 
				   << equipindex 
				   << EQUIP_STRPROP_OPTION_ADD 
				   << strAddJewel.c_str());

	//// 装备镶嵌宝石日志
	//EquipLog jewelLog;
	//jewelLog.operateType    = LOG_EQUIP_JEWEL_INLAY;
	//jewelLog.equipPos       = equipindex;
	//jewelLog.equipSlot      = jewelindex;
	//jewelLog.sourceItemID   = "";
	//jewelLog.targetItemID   = strAddJewel.c_str();
	//jewelLog.deductItemIDs  = strAddJewel.c_str();
	//jewelLog.deductItemNums = "1";
	//jewelLog.capitalType    = CAPITAL_COPPER;
	//jewelLog.capitalNum     = inlayFees;        
	//LogModule::m_pLogModule->SaveEquipLog(pKernel, self, jewelLog);


	//LogModule::m_pLogModule->OnJewelInlay(pKernel, self, strOldJewel.c_str(), strAddJewel.c_str(), nColor, inlayFees, newJewelType);

	return 0;
}

//摘下宝石
int JewelModule::DismantleJewel(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	int equipindex = args.IntVal(2);	//移出的容器(1-10)
	int jewelindex = args.IntVal(3);	//移出的物品(1-4)

	Dismantle(pKernel, self, equipindex, jewelindex);

	return 0;
}

// 合成宝石
int JewelModule::ComposeJewel(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 背包
	PERSISTID powerbox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(powerbox))
	{
		return 0;
	}

	// 目标宝石索引
	int args_index = 2;
	const int target_jewel_idx = args.IntVal(args_index++);
	const int target_jewel_num = args.IntVal(args_index++);

	// 目标宝石信息
	const char *target_id = GetJewelID(target_jewel_idx);
	if (StringUtil::CharIsNull(target_id))
	{
		return 0;
	}

	// 检查背包是否已满
	const int bind_state = FunctionEventModule::GetItemBindState(FUNCTION_EVENT_ID_JEWEL_COMPOSE);
	if (m_pContainerModule->TryAddItem(pKernel, powerbox, target_id, target_jewel_num, bind_state) < target_jewel_num)
	{
		/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
			STR_JEWEL_SYSINFO_TOOLFULL, CVarList());*/
		return 0;
	}

	// 宝石等级不能高于限制等级
	const int required_level = atoi(pKernel->GetConfigProperty(target_id, FIELD_PROP_UP_LEVEL));
	if (required_level > pSelfObj->QueryInt(FIELD_PROP_LEVEL))
	{
		//// 角色等级不足，宝石合成失败，要求等级{@1}
		//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, 
		//	STR_JEWEL_SYSINFO_UP_LOW_LEVEL, CVarList() << required_level);

		//return 0;
	}

	// 目标等级、合成费用、合成点数
	const int target_level = StringUtil::StringAsInt(pKernel->GetConfigProperty(target_id, FIELD_PROP_LEVEL));
	int compose_fee = StringUtil::StringAsInt(pKernel->GetConfigProperty(target_id, FIELD_PROP_UP_MONEY)) * target_jewel_num;
	const int compose_point = StringUtil::StringAsInt(pKernel->GetConfigProperty(target_id, FIELD_PROP_POINT_VALUE)) * target_jewel_num;

	// 已选材料数
	const int select_num = args.IntVal(args_index++);
	const int select_size = select_num * 2;
	CVarList select_list;
	select_list.Append(args, args_index, select_size);

	// 已选材料统计、验证
	JewelComposeConsume select_materia;
	if (!CountMateria(pKernel, select_list, target_level, select_materia))
	{
		return 0;
	}

	// 已选材料是否足够
	JewelMap::const_iterator start_it = select_materia.jewel_map.begin();
	JewelMap::const_iterator end_it = select_materia.jewel_map.end();
	std::string log_item_id = "";
	std::string log_itme_count = "";

	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		if (m_pContainerModule->GetItemCount(pKernel, powerbox, start_it->first.c_str()) < start_it->second)
		{
			// 材料不够
			/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, 
				STR_JEWEL_SYSINFO_UP_NOTMATERIAL, CVarList());*/

			return 0;
		}

		// 日志用
		if (!log_item_id.empty())
		{
			log_item_id.append(",");
		}
		log_item_id.append(target_id);
		
		if (!log_itme_count.empty())
		{
			log_itme_count.append(",");
		}
		log_itme_count.append(StringUtil::IntAsString(start_it->second));
	}

	// 需购买材料数
	args_index += select_size;
	const int buy_num = args.IntVal(args_index++);
	const int buy_size = buy_num * 2;
	CVarList buy_list;
	buy_list.Append(args, args_index, buy_size);

	// 需购买材料统计、验证
	JewelComposeConsume buy_materia;
	buy_materia.buy_capital = CAPITAL_GOLD;
	if (!CountMateria(pKernel, buy_list, target_level, buy_materia, true))
	{
		return 0;
	}

	// 材料总点数、合成总花费
	const int material_point = select_materia.point_values + buy_materia.point_values;
	compose_fee -= (select_materia.compose_fee + buy_materia.compose_fee);
	if (compose_fee < 0)
	{
		return 0;
	}

	// 检查商会的物品数量是否足够
// 	JewelMap::const_iterator buy_start_it = buy_materia.jewel_map.begin();
// 	JewelMap::const_iterator buy_end_it = buy_materia.jewel_map.end();
// 	for (; buy_start_it != buy_end_it; ++buy_start_it)
// 	{
// 		// 商会物品数量是否足够
// 		if (ShopModule::FindCommerceItemRemain(pKernel, self, buy_start_it->first.c_str()) < buy_start_it->second)
// 		{
// 			return 0;
// 		}
// 	}

	// 材料不足
	if (material_point < compose_point)
	{
		// 合成材料不够
		/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
			STR_JEWEL_SYSINFO_UP_NOTMATERIAL, CVarList());*/

		return 0;
	}

	// 大于属于异常
	if (material_point > compose_point)
	{
		return 0;
	}

	// 合成费用不足
	if (!m_pCapitalModule->CanDecCapital(pKernel, self, CAPITAL_COPPER, compose_fee))
	{
		// 合成铜钱不够
		/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, 
			STR_JEWEL_SYSINFO_UP_NOTCAPITAL, CVarList());*/
		return 0;
	}

	// 购买花费
	if (buy_materia.buy_cost > 0 
		&& !m_pCapitalModule->CanDecCapital(pKernel, self, buy_materia.buy_capital, buy_materia.buy_cost))
	{
		// 购买宝石银子不足
		/*::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
			STR_JEWEL_SYSINFO_116, CVarList());*/
		return 0;
	}
	
	log_item_id.append("|");
	log_itme_count.append("|");
	// 执行购买操作、失败则返回
// 	JewelMap::const_iterator buy_start_it1 = buy_materia.jewel_map.begin();
// 	JewelMap::const_iterator buy_end_it1 = buy_materia.jewel_map.end();
// 	for (; buy_start_it1 != buy_end_it1; ++buy_start_it1)
// 	{
// 		// 去商会扣钱、扣物品数量
// 		if (ShopModule::BuyCommerceItem(pKernel, self, 
// 			buy_start_it1->first.c_str(), buy_start_it1->second) != EM_COMMERCE_RESULT_SUCCEED)
// 		{
// 			return 0;
// 		}
// 	}

	// 扣除已有宝石
	if (!DecJewels(pKernel, self, select_materia.jewel_map, FUNCTION_EVENT_ID_JEWEL_COMPOSE))
	{
		return 0;
	}
	
	// 宝石合成扣钱
	if (DC_SUCCESS != m_pCapitalModule->DecCapital(pKernel, self,
		CAPITAL_COPPER, compose_fee, FUNCTION_EVENT_ID_JEWEL_COMPOSE))
	{
		return 0;
	}

	// 进行合成
	m_pContainerModule->PlaceItem(pKernel, powerbox, 
		target_id, target_jewel_num, FUNCTION_EVENT_ID_JEWEL_COMPOSE, true, bind_state);

	CVarList s2c_msg;
	s2c_msg << SERVER_CUSTOMMSG_JEWEL
			<< S2C_JEWEL_SUBMSG_COMPOSE_SUCCESS
			<< target_id
			<< target_jewel_num;
	pKernel->Custom(self, s2c_msg);

	// 宝石合成日志
	std::string new_item = target_id;
	new_item.append(",");
	new_item.append(StringUtil::IntAsString(target_jewel_num));

	//EquipLog jewel_Log;
	//jewel_Log.operateType = LOG_EQUIP_JEWEL_COMPOSE;
	//jewel_Log.targetItemID = new_item;
	//jewel_Log.deductItemIDs = log_item_id;
	//jewel_Log.deductItemNums = log_itme_count;
	//jewel_Log.capitalType = CAPITAL_COPPER;
	//jewel_Log.capitalNum = compose_fee;
	//LogModule::m_pLogModule->SaveEquipLog(pKernel, self, jewel_Log);
	//
	////MiddleModule::Instance()->OnAchievementCondition(pKernel, self, EAchievementDstCond_OwnJewelLevel, target_level);

	//LogModule::m_pLogModule->OnJewelUp(pKernel, self, 
	//	log_item_id.c_str(), log_itme_count.c_str(), new_item.c_str(),
	//	atoi(pKernel->GetConfigProperty(target_id, FIELD_PROP_ITEM_TYPE)));
	return 0;
}

// 摘下
bool JewelModule::Dismantle(IKernel* pKernel, const PERSISTID& self, 
	const int equip_idx, const int jewel_idx)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	int equipindex = equip_idx;	//移出的容器(1-10)
	int jewelindex = jewel_idx;	//移出的物品(1-4)

	//为了和宝石列对应上
	jewelindex += EQUIP_STRHENTHEN_JEWEL1 - 1;

	//只能从宝石栏往道具栏移
	if (equipindex < EQUIP_POS_STR_MIN ||
		equipindex > EQUIP_POS_STR_MAX ||
		jewelindex < EQUIP_STRHENTHEN_JEWEL1 ||
		jewelindex > EQUIP_STRHENTHEN_JEWEL4)
	{
		return false;
	}

	//获得客户端视窗对应服务器的容器编号
	PERSISTID equbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	PERSISTID toolbox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(equbox) || !pKernel->Exists(toolbox))
	{
		return false;
	}

	PERSISTID equip = pKernel->GetItem(equbox, equipindex);

	//强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return false;
	}

	//查找对应装备行
	int equiprow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, equipindex);
	if (equiprow == -1)
	{
		return false;
	}

	//宝石是否存在
	std::string strJewelItem = pStrengthenRec->QueryString(equiprow, jewelindex);
	if (StringUtil::ItemIsNull(strJewelItem.c_str()))
	{
		return true;
	}

	//包裹是否满
	int nBindState = FunctionEventModule::GetItemBindState(FUNCTION_EVENT_ID_JEWEL_DISMANTLE);
	if (m_pContainerModule->TryAddItem(pKernel, toolbox, strJewelItem.c_str(), 1, nBindState) < 1)
	{
		//包裹栏满
		//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_TOOLFULL, CVarList());

		return false;
	}

	// 宝石拆除扣钱
	int dismantleFees = atoi(pKernel->GetConfigProperty(strJewelItem.c_str(), FIELD_PROP_EXTIRPATE_COST));
	__int64 currentCapital = m_pCapitalModule->GetCapital(pKernel, self, CAPITAL_COPPER);
	if (dismantleFees > currentCapital)
	{
		// 资金不足
		//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_JEWEL_SYSINFO_UP_NOTCAPITAL, CVarList());
		return false;
	}

	int decCapitalRet = m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_COPPER, dismantleFees, FUNCTION_EVENT_ID_JEWEL_DISMANTLE);
	if (decCapitalRet != DC_SUCCESS)
	{
		return false;
	}

	//拆掉宝石
	pStrengthenRec->SetString(equiprow, jewelindex, "");

	//包裹创建宝石
	m_pContainerModule->PlaceItem(pKernel, toolbox, strJewelItem.c_str(), 1, FUNCTION_EVENT_ID_JEWEL_DISMANTLE, false, nBindState);

	//更新宝石属性
	pKernel->Command(self, self, CVarList() << COMMAND_EQUIPMENT_JEWEL_PKG << equipindex << EQUIP_STRPROP_OPTION_REMOVE << strJewelItem.c_str());

	// 摘除装备宝石日志modify by   (2015-04-23)
	/*EquipLog jewelLog;
	jewelLog.operateType = LOG_EQUIP_JEWEL_DISMANTLE;
	jewelLog.equipPos = equipindex;
	jewelLog.equipSlot = jewelindex;
	jewelLog.sourceItemID = strJewelItem.c_str();
	jewelLog.targetItemID = "";
	jewelLog.deductItemIDs = "";
	jewelLog.deductItemNums = "";
	jewelLog.capitalType = CAPITAL_COPPER;
	jewelLog.capitalNum = dismantleFees;
	LogModule::m_pLogModule->SaveEquipLog(pKernel, self, jewelLog);

	LogModule::m_pLogModule->OnJewelDismantle(pKernel, self, strJewelItem.c_str(), dismantleFees, atoi(pKernel->GetConfigProperty(strJewelItem.c_str(), "ColorLevel")));*/

	return true;
}

int JewelModule::GetExecuteJewelNum(IKernel*pKernel, const PERSISTID& self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return 0;
	}

	//强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return 0;
	}
	int count = 0;
	for (int row = 0; row < pStrengthenRec->GetRows(); row++)
	{
		for (int m = EQUIP_STRHENTHEN_JEWEL1; m <= EQUIP_STRHENTHEN_JEWEL4; m++)
		{
			std::string strJewelItem = pStrengthenRec->QueryString(row, m);
			if (!StringUtil::ItemIsNull(strJewelItem.c_str())){
				++count;
			}

		}

	}
	return count;
}

//是否有相似宝石
bool JewelModule::JewelHaveSame(IKernel* pKernel, const PERSISTID& self, int equiprow, int jewelindex, int newType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	const char * jewelitem = NULL;
	const char * jeweltype = NULL;
	int nItemType = 0;

	PERSISTID equbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equbox))
	{
		return true;
	}

	//强化表
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return true;
	}

	if (equiprow < 0 || equiprow >= pStrengthenRec->GetRows())
	{
		return true;
	}

	LoopBeginCheck(d);
	for (int i = EQUIP_STRHENTHEN_JEWEL1; i <= EQUIP_STRHENTHEN_JEWEL4; ++i)
	{
		LoopDoCheck(d);
	    
		if (jewelindex == i)
		{
			continue;
		}

		jewelitem = pStrengthenRec->QueryString(equiprow, i);
		if (StringUtil::ItemIsNull(jewelitem))
		{
			continue;
		}

		jeweltype = pKernel->GetConfigProperty(jewelitem, "ItemType");
		if (StringUtil::ItemIsNull(jeweltype))
		{
			continue;
		}

		nItemType = ::atoi(jeweltype);

		if (newType == nItemType)
		{
			return true;
		}
	}

	return false;
}

// 指定部位是否可以使用指定类型宝石
bool JewelModule::CanUseJewel(const int jewel_type, const int equip_pos)
{
	JewelUsePos temp(jewel_type);
	JewelUsePosVec::const_iterator find_it = find(
		m_JewelUsePosVec.begin(),
		m_JewelUsePosVec.end(),
		temp);
	if (m_JewelUsePosVec.end() == find_it)
	{
		return false;
	}

	std::vector<int>::const_iterator find_it2 = find(
		find_it->equip_pos_list.begin(), 
		find_it->equip_pos_list.end(), 
		equip_pos);

	if (find_it->equip_pos_list.end() == find_it2)
	{
		return false;
	}

	return true;
}

// 根据索引号取宝石ID
const char* JewelModule::GetJewelID(const int index)
{
	JewelIndexMap::const_iterator find_it = m_JewelIndexMap.find(index);
	if (find_it != m_JewelIndexMap.end())
	{
		return find_it->second.c_str();
	}

	return "";
}

// 统计合成材料
bool JewelModule::CountMateria(IKernel* pKernel, const IVarList &res, 
	const int target_level, JewelComposeConsume &compose_material, bool buy/* = false*/)
{
	LoopBeginCheck(a);
	for (int i = 0; i < (int)res.GetCount(); ++i)
	{
		LoopDoCheck(a);
		const int idx = res.IntVal(i++);
		const int num = res.IntVal(i);

		const char *id = GetJewelID(idx);
		if (StringUtil::CharIsNull(id))
		{
			return false;
		}

		const int lvl = StringUtil::StringAsInt(pKernel->GetConfigProperty(id, FIELD_PROP_LEVEL));
		if (lvl >= target_level)
		{
			return false;
		}

		const int compose_fee = StringUtil::StringAsInt(pKernel->GetConfigProperty(id, FIELD_PROP_UP_MONEY)) * num;
		const int compose_point = StringUtil::StringAsInt(pKernel->GetConfigProperty(id, FIELD_PROP_POINT_VALUE)) * num;

		compose_material.compose_fee += compose_fee;
		compose_material.point_values += compose_point;

		// 统计购买花费
// 		if (buy)
// 		{
// 			const int price = ShopModule::FindCommerceItemPrice(pKernel, id) * num;
// 			if (price <= 0)
// 			{
// 				return false;
// 			}
// 
// 			compose_material.buy_cost += price;
// 		}

		JewelMap::iterator it = compose_material.jewel_map.find(id);
		if (it == compose_material.jewel_map.end())
		{
			compose_material.jewel_map.insert(JewelMap::value_type(id, num));
		}
		else
		{
			it->second += num;
		}		
	}

	return true;
}

// 扣除物品
bool JewelModule::DecJewels(IKernel *pKernel, const PERSISTID &self,
	const JewelMap &jewels, const int log_id)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box))
	{
		return false;
	}

	JewelMap::const_iterator start_it = jewels.begin();
	JewelMap::const_iterator end_it = jewels.end();

	bool remove_done = true;
	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		const char *jewel_id = start_it->first.c_str();
		const int jewel_num = start_it->second;

		// 扣除
		int remove_count = 0;
		if (FunctionEventModule::GetItemBindPriorUse(log_id))
		{
			remove_count = ContainerModule::RemoveItemsBindPrior(pKernel, item_box,
				jewel_id, jewel_num, (EmFunctionEventId)log_id, true);
		}
		else
		{
			remove_count = ContainerModule::m_pContainerModule->RemoveItems(pKernel, item_box,
				jewel_id, jewel_num, (EmFunctionEventId)log_id, true);
		}

		if (remove_count != jewel_num)
		{
			remove_done = false;
			break;
		}
	}

	return remove_done;
}

// 获取玩家身上等于某个等级的宝石数量(运营活动用)
int JewelModule::GetJewelsByLevel(IKernel* pKernel, const PERSISTID& self, const int level)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int nCount = 0;

    LoopBeginCheck(f)
	do 
	{
	    LoopDoCheck(f)
		// 装备栏
		PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
		if (!pKernel->Exists(equipbox))
		{
			break;
		}

		// 强化表
		IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
		if (NULL == pStrengthenRec)
		{
			break;
		}

		const char * jewelitem = NULL;
		int nRows = pStrengthenRec->GetRows();
		LoopBeginCheck(g)
		for (int i = 0; i < nRows; ++i)	// 遍历装备格子
		{
		    LoopDoCheck(g)
		    
		    LoopBeginCheck(h)
			for (int j = EQUIP_STRHENTHEN_JEWEL1; j <= EQUIP_STRHENTHEN_JEWEL4; ++j)	// 遍历宝石位置
			{
			    LoopDoCheck(h)
			    
				// 获取宝石对象
				jewelitem = pStrengthenRec->QueryString(i, j);
				if (StringUtil::ItemIsNull(jewelitem))
				{
					continue;
				}

				// 获取符合等级要求的宝石
				int nJewelLevel = atoi(pKernel->GetConfigProperty(jewelitem, "Level"));
				if (nJewelLevel >= level)
				{
					nCount++;
				}
			}
		}

	} while (0);

	return nCount;
}

// 重新加载宝石配置
void JewelModule::ReloadJewelConfig(IKernel* pKernel)
{
	m_pInstance->LoadResource(pKernel);

	m_pInstance->LoadJewelIndexCfg(pKernel);
}
