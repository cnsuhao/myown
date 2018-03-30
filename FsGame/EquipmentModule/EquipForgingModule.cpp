//--------------------------------------------------------------------
// 文件名:      EquipForging.cpp
// 内  容:      装备锻造
// 说  明:
// 创建日期:    2017年04月19日
// 创建人:       tongzt
// 修改人:		
//    :       
//--------------------------------------------------------------------
#include "EquipForgingModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/CommonModule/FunctionEventModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif

#include "utils/extend_func.h"
#include "utils/XmlFile.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/util_func.h"
#include "SystemFunctionModule/RewardModule.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "CommonModule/CommRuleModule.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"


EquipForgingModule *    EquipForgingModule::m_pInstance = NULL;

inline int nx_reload_forging_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 如果指针存在，表明模块已创建
	if (NULL == EquipForgingModule::m_pInstance)
	{
	    return 1;
	}

    bool ret =  EquipForgingModule::m_pInstance->LoadResource(pKernel);
    
	return ret ? 0 : 1;
}

inline int nx_forge_equip(void* state)
{
    // 获得核心指针
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 如果指针存在，表明模块已创建
    if (NULL == EquipForgingModule::m_pInstance)
    {
        return 1;
    }
    
    // 检查参数数量
    CHECK_ARG_NUM(state, nx_forge_equip, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_forge_equip, 1);
    CHECK_ARG_STRING(state, nx_forge_equip, 2);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
    const char *drawing = pKernel->LuaToString(state, 2);

    EquipForgingModule::m_pInstance->Forging(pKernel, player, drawing);

	return 0;
}

//初始化
bool EquipForgingModule::Init(IKernel* pKernel)
{
    m_pInstance = this;

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_FORGING, OnCustomEquipmentForging);

	Assert(m_pInstance != NULL);

    DECL_LUA_EXT(nx_reload_forging_config);
    DECL_LUA_EXT(nx_forge_equip);

	if (!LoadResource(pKernel))
	{
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("EquipForgingModule", EquipForgingModule::ReloadForgingConfig);
#endif
    
    return true;
}

//释放
bool EquipForgingModule::Shut(IKernel* pKernel)
{
    m_ForgingRuleVec.clear();
    return true;
}


//加载资源
bool EquipForgingModule::LoadResource(IKernel* pKernel)
{
    return LoadForgingRule(pKernel);
}

// 锻造
void EquipForgingModule::Forging(IKernel *pKernel, const PERSISTID &self, 
	const char *drawing)
{
	// 检查是否合法
	if (StringUtil::CharIsNull(drawing))
	{
		return;
	}

	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (pSelf == NULL)
	{
		return;
	}

	if (!ActivateFunctionModule::m_pInstance->CheckActivateFunction(pKernel, self, AFM_FORGING))
	{
		return;
	}

	// 取得图纸规则
	const ForgingRule *rule = GetForgingRule(drawing);
	if (NULL == rule)
	{
		return;
	}

	// 产出
	if (rule->output_equip.empty())// 异常
	{
		return;
	}

	// 产出品质
	const int output_color = RandomOutputColor(rule->color_weight_list, rule->sum_weight);
	if (!CommRuleModule::m_pThis->IsValidColor(output_color))
	{
		return;
	}

	// 货币是否足够
	if (!CommRuleModule::CanDecCapitals(pKernel, self, rule->capitals))
	{
		return;
	}

	// 材料是否足够
	if (!CommRuleModule::CanDecItems(pKernel, self, rule->items))
	{
		return;
	}

	// 扣除物品
	if (!CommRuleModule::ConsumeItems(pKernel, self, rule->items, FUNCTION_EVENT_ID_FORGING))
	{
		return;
	}

	// 扣除货币
	if (!CommRuleModule::ConsumeCapitals(pKernel, self, rule->capitals, FUNCTION_EVENT_ID_FORGING))
	{
		return;
	}

	// 通过奖励发放
	RewardModule::AwardEx award;
	award.name = "equip_forging_gain";	// 邮件名
	award.srcFunctionId = FUNCTION_EVENT_ID_FORGING;

	RewardModule::AwardData item;
	item.strType = rule->output_equip;
	item.nAddVal = output_color;
	award.items.push_back(item);

	RewardModule::m_pRewardInstance->RewardPlayer(pKernel, self, &award, CVarList());

	CVarList s2c_msg;
	s2c_msg << SERVER_CUSTOMMSG_FORGING
			<< S2C_FORGING_SUBMSG_SUCCESS;
	pKernel->Custom(self, s2c_msg);

	//LogModule::m_pLogModule->OnForgeEquipment(pKernel, self, drawing, output_id, output_color);
}

// 锻造设置
bool EquipForgingModule::LoadForgingRule(IKernel* pKernel)
{
    m_ForgingRuleVec.clear();
    
    std::string the_config_file = pKernel->GetResourcePath();
	the_config_file.append(EQUIP_FORGING_CONFIG);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(a)
    for (int i = 0; i < sec_count; ++i)
    {
        LoopDoCheck(a)
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);
		if (StringUtil::CharIsNull(sec))
		{
			continue;
		}

		const char *drawing = xml.ReadString(sec, "Drawing", "");
		if (StringUtil::CharIsNull(drawing))
		{
			continue;
		}

        ForgingRule the_rule(drawing);

		the_rule.output_equip = xml.ReadString(sec, "OutputEquip", "");
		String_Vec range_str_list;
		xml.ReadList<std::string>(sec, "ColorWeight", ",", &range_str_list);

		int sum_weight = 0;
		LoopBeginCheck(b);
		for (int idx = 0; idx < (int)range_str_list.size(); ++idx)
		{
			LoopDoCheck(b);
			OutputColorWeight range_cfg;
			CVarList result;
			util_split_string(result, range_str_list[idx], ":");
			range_cfg.output_color = result.IntVal(0);
			int weight = result.IntVal(1);
			range_cfg.lower_weight = sum_weight + 1;
			sum_weight += weight;
			range_cfg.upper_weight = sum_weight;
			the_rule.color_weight_list.push_back(range_cfg);
		}

		the_rule.sum_weight = sum_weight;
        
		CommRuleModule::ParseConsumeVec(xml.ReadString(sec, "Capitals", ""), the_rule.capitals);
		CommRuleModule::ParseConsumeVec(xml.ReadString(sec, "Items", ""), the_rule.items);
        
		m_ForgingRuleVec.push_back(the_rule);
    }

    return true;
}

// 重新加载锻造装备配置
void EquipForgingModule::ReloadForgingConfig(IKernel* pKernel)
{
	EquipForgingModule::m_pInstance->LoadResource(pKernel);
}

// 取得锻造规则
const ForgingRule* EquipForgingModule::GetForgingRule(const char *drawing)
{
	if (StringUtil::CharIsNull(drawing))
	{
		return NULL;
	}

	ForgingRule rule(drawing);
	ForgingRuleVec::const_iterator find_it = find(
		m_ForgingRuleVec.begin(),
		m_ForgingRuleVec.end(),
		rule);
	if (find_it != m_ForgingRuleVec.end())
	{
		return &(*find_it);
	}

	return NULL;
}

// 随机产出品质
const int EquipForgingModule::RandomOutputColor(const OutputColorWeightVec& output_list, const int sum_weight)
{
	const int rdx = util_random_int(sum_weight) + 1;
	OutputColorWeightVec::const_iterator find_it = find_if(
		output_list.begin(),
		output_list.end(),
		OutputColorFinder(rdx));
	if (find_it == output_list.end())
	{
		return -1;
	}

	return find_it->output_color;
}

// 锻造消息
int EquipForgingModule::OnCustomEquipmentForging(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	if (args.GetCount() < 2)
	{
		return 0;
	}

	int sub_msg = args.IntVal(1);

	switch (sub_msg)
	{
	case C2S_FORGING_SUBMSG_EXECUTE:
		{
			// 锻造装备
			m_pInstance->Forging(pKernel, self, args.StringVal(2));
		}
		break;
	default:
		break;
	}

	return 0;
}