//--------------------------------------------------------------------
// 文件名:      EquipSmeltModule.cpp
// 内  容:      装备熔炼
// 说  明:
// 创建日期:    2014年11月27日
// 创建人:        
// 修改人:		
//    :       
//--------------------------------------------------------------------

#include "EquipSmeltModule.h"
#include <vector>
#include <map>
#include <sstream>

#include "utils/extend_func.h"
#include "utils/XmlFile.h"

#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/ItemModule/ItemBaseModule.h"
#include "FsGame/ItemModule/ToolItem/ToolItemModule.h"
#include "FsGame/CommonModule/EnvirValueModule.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "EquipSmeltDefine.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "EquipDefine.h"
#include "utils/exptree.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"

EquipSmeltModule *	EquipSmeltModule::m_pEquipSmeltModule = NULL;


bool EquipSmeltModule::Init(IKernel* pKernel)
{
	m_pEquipSmeltModule	= this;

	Assert(m_pEquipSmeltModule != NULL);

	
	//强化消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_SMELT, OnCustomEquipSmelt);

	// 上线
	pKernel->AddEventCallback("player", "OnRecover", EquipSmeltModule::OnPlayerRecover, 1);

	return true;
}


//清理
bool EquipSmeltModule::Shut(IKernel* pKernel)
{
    return true;
}

// 解析并销毁原料, 返回产出熔炼值
const int EquipSmeltModule::ConsumeRawMaterial(IKernel* pKernel, const PERSISTID& self,
	const IVarList &uid_list)
{
	PERSISTID power_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(power_box))
	{
		return 0;
	}

	// 解析出的装备
	CVarList items;

	int uid_count = (int)uid_list.GetCount();

	if (uid_count > EnvirValueModule::EnvirQueryInt(ENV_VALUE_SMELT_MAX_NUM))
	{
		// 超出一次熔炼限制
		return 0;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < uid_count; ++i)
	{
		LoopDoCheck(a);
		const char * uid = uid_list.StringVal(i);
		if (StringUtil::CharIsNull(uid))
		{
			// 无效UID
			continue;
		}

		// 无效装备
		PERSISTID temp_item = ContainerModule::m_pContainerModule->FindItemByUid(pKernel, power_box, uid);
		if (!pKernel->Exists(temp_item))
		{			
			continue;
		}

		// 重复物品
		if (HasItem(items, temp_item))
		{			
			continue;
		}

		// 必须是装备
		const char * script = pKernel->GetScript(temp_item);
		if (StringUtil::CharIsNull(script))
		{
			continue;
		}

		// 非装备不能熔炼
		if (!StringUtil::CharIsEqual(script, CLASS_NAME_EQUIPMENT))
		{
			continue;
		}

		items << temp_item;
	}

	// 日志记录物品
	std::string log_comment = "";
	float total_smelt_value = 0.0f;

	// 消耗原料
	int item_count = (int)items.GetCount();
	LoopBeginCheck(b);
	for (int i = 0; i < item_count; ++i)
	{
		LoopDoCheck(b);
		PERSISTID temp_item = items.ObjectVal(i);
		IGameObj* pTempItemObj = pKernel->GetGameObj(temp_item);
		if (pTempItemObj == NULL)
		{
			continue;
		}

		const char *temp_config = pTempItemObj->GetConfig();
		const int temp_color = pTempItemObj->QueryInt(FIELD_PROP_COLOR_LEVEL);

		// 产出熔炼值
		const float smelt_value = GetOutputSmeltValue(pKernel, self, temp_item);

		// 记录并销毁此物品
		if (ContainerModule::m_pContainerModule->RemoveItem(pKernel, power_box, temp_item, 1, FUNCTION_EVENT_ID_SMELT) > 0)
		{
			total_smelt_value += smelt_value;

			// 熔炼消耗物品详细
			if (!log_comment.empty())
			{
				log_comment.append(",");
			}
			log_comment.append(temp_config);
			log_comment.append(":");
			log_comment.append(StringUtil::IntAsString(temp_color));
		}		
	}

	//记录玩法日志
	/*GamePlayerActionLog log;
	log.actionType = LOG_GAME_ACTION_MELT;
	log.actionState = LOG_GAME_ACTION_JOIN;
	log.comment = StringUtil::StringAsWideStr(log_comment.c_str());
	LogModule::m_pLogModule->SaveGameActionLog(pKernel, self, log);*/

	return (int)total_smelt_value;
}

//player熔炼消息回调
int EquipSmeltModule::OnCustomEquipSmelt(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	size_t nSize = args.GetCount();
	if (nSize < 2)
	{
		return 0;
	}

	// 如果玩家不存在
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 不在状态
	if (AsynCtrlModule::m_pAsynCtrlModule->IsWaiting(pKernel, self))
	{
		return 0;
	}

	int iType = args.IntVal(1);
	
	if (iType == CS_EQUIP_SMELT_EXE)
	{
		m_pEquipSmeltModule->DoSmeltEquip(pKernel, self, args);
	}

	return 0;
}

// 玩家上线
int EquipSmeltModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
									  const PERSISTID& sender, const IVarList& args)
{
	return 0;
}

// 执行装备熔炼
int EquipSmeltModule::DoSmeltEquip(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
    // 熔炼装备
    // args 的格式:
    //      [msg_id][sub_msg_id][equipt_uid]...[equipt_uid]
    //
    
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *player_obj = pKernel->GetGameObj(self);
	if (player_obj == NULL)
	{
		return 0;
	}

	// 开启等级限制
	if (!ActivateFunctionModule::m_pInstance->CheckActivateFunction(pKernel, self, AFM_SMELT))
	{
		return 0;
	}

    // 原料列表 (UniqueID)
    CVarList uid_list;
    uid_list.Append(args, 2, args.GetCount()-2);
    
    // 统计原料然后销毁
    const int output_smelt_value = ConsumeRawMaterial(pKernel, self, uid_list);   
	if (output_smelt_value <= 0)
    {
		return 0;
    }
 
	CapitalModule::m_pCapitalModule->IncCapital(pKernel, self, 
		CAPITAL_SMELT, output_smelt_value, FUNCTION_EVENT_ID_SMELT);
	
    // 通知客户端
    CVarList smelt_result;
	smelt_result << SERVER_CUSTOMMSG_SMELT;
    smelt_result << SC_EQUIP_SMELT_EXE_SUC;
	smelt_result << output_smelt_value;;   
    pKernel->Custom(self, smelt_result); 

	//LogModule::m_pLogModule->OnSmeltEquipment(pKernel, self, output_info.smelt_point, output_info.soul_amount);

    return 0;
}

// 检查 @item 是否存在于 @items 中
bool EquipSmeltModule::HasItem(const IVarList &items, const PERSISTID &item)
{
    int items_count = (int)items.GetCount();
    
    LoopBeginCheck(a)
    for (int i=0; i<items_count; ++i)
    {
        LoopDoCheck(a)
        
        PERSISTID temp_item = items.ObjectVal(i);
        if (temp_item == item)
        {
            return true;
        }
    }
    
    return false;
}

// 取得输出熔炼值
const float EquipSmeltModule::GetOutputSmeltValue(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID &item)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0.0f;
	}

	IGameObj *pItem = pKernel->GetGameObj(item);
	if (NULL == pItem)
	{
		return 0.0f;
	}

	// 取得原材料品质和等级
	int src_color = pItem->QueryInt(FIELD_PROP_COLOR_LEVEL);
	int src_level = pItem->QueryInt(FIELD_PROP_LEVEL);
	src_color = src_color <= 0 ? 1 : src_color;
	src_level = src_level <= 0 ? 1 : src_level;

	float args[MAX_PARAM] = { 0.0f };
	args[0] = (float)src_color;
	args[1] = (float)src_level;

	ExpTree exp;
	return exp.CalculateParam(EnvirValueModule::EnvirQueryString(ENV_VALUE_SMELT_VALUE_FORMULA), args);
}
