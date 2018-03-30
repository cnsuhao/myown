//---------------------------------------------------------
//文件名:       ContainerModule.h
//内  容:       容器
//说  明:       
//          
//创建日期:      2014年10月31日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#include "ContainerModule.h"

#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/util_ini.h"
#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "public/VarList.h"

#include "FsGame/Define/ContainerDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ToolBoxSysInfoDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/Define/CoolDownDefine.h"
//#include "FsGame/Define/OffLineDefine.h"

#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"

#include "FsGame/ItemModule/ItemBaseModule.h"
//#include "FsGame/ItemModule/EquipmentModule.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "server/LoopCheck.h"

#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"

#include <algorithm>
#include <list>
#include <vector>
//#include "FsGame/SkillModule/MeridianModule.h"
#include "FsGame/Define/Fields.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#include "FsGame/ItemModule/ToolItem/ToolItemModule.h"
#endif
#include "FsGame/CommonModule/FunctionEventModule.h"
#include "EnvirValueModule.h"
#include "Define/ItemTypeDefine.h"

const char* const BOX_RULE_CONFIG = "ini/SystemFunction/Container/containerRule.xml";      //背包规则

//测试物品使用的GM命令
// /use_item config_id
int nx_place_item(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == ContainerModule::m_pContainerModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_place_item, 4);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_place_item, 1);
    CHECK_ARG_STRING(state, nx_place_item, 2);
	CHECK_ARG_INT(state, nx_place_item, 3);
	CHECK_ARG_INT(state, nx_place_item, 4);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
    const char *config_id = pKernel->LuaToString(state, 2);
    int amount = pKernel->LuaToInt(state, 3);
	int bind = pKernel->LuaToInt(state, 4);
    if (!pKernel->Exists(player))
    {
        return 0;
    }

	if (StringUtil::CharIsNull(config_id) || amount <= 0)
    {
        return 0;
    }

    // 创建物品放入背包
    PERSISTID toolbox = pKernel->GetChild(player, ITEM_BOX_NAME_WSTR);
    if (toolbox.IsNull())
    {
        return 0;
    }
   
	int ret = ContainerModule::m_pContainerModule->PlaceItem(pKernel, toolbox, config_id, amount, FUNCTION_EVENT_ID_GM_COMMAND, true, bind);

	pKernel->LuaPushBool(state, ret > 0);

    return 1;
}

int nx_set_container(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == ContainerModule::m_pContainerModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_set_container, 3);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_set_container, 1);
    CHECK_ARG_WIDESTR(state, nx_set_container, 2);
    CHECK_ARG_INT(state, nx_set_container, 3);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
    const wchar_t * boxName = pKernel->LuaToWideStr(state, 2);
    int value = pKernel->LuaToInt(state, 3);

    if (!pKernel->Exists(player))
    {
        return 0;
    }

    PERSISTID box = pKernel->GetChild(player, boxName);
    if (!pKernel->Exists(box))
    {
        return 0;
    }
    IGameObj* pBoxObj = pKernel->GetGameObj(box);
    if (NULL == pBoxObj)
    {
        return 0;
    }
    pBoxObj->SetInt("LockedSize", value);

    return 1;
}

//物品整理时的排序项
struct S_SORT_ITEM
{
    PERSISTID   obj_id;
    int item_type;
    int color_level;
    int amount;
    int item_level; // 物品的限制等级
    int item_pos;  // 武器的装备位置

    S_SORT_ITEM():item_type(0),color_level(0),amount(0),item_level(0),item_pos(0)
    {}

    bool operator<(const S_SORT_ITEM& outArg)
    {
        // 潜规则, 礼包排第一
        if (item_type == ITEM_TYPE_GIFT)
        {  
            return true;
        } 
        else if (outArg.item_type == ITEM_TYPE_GIFT)
        {
            return false;
        }
        
        // item_type 升序
        if(item_type < outArg.item_type)
        {
            return true;
        } 
        else if (item_type > outArg.item_type)
        {
            return false;
        }

        // color_level 降序
        if(outArg.color_level < color_level)
        {
            return true;
        }
        else if (outArg.color_level > color_level)
        {
            return false;
        }
        
        // item_pos 升序
        if(item_pos < outArg.item_pos)
        {
            return true;
        } 
        else if (item_pos > outArg.item_pos)
        {
            return false;
        }
        
        // item_level降序
        if(outArg.item_level < item_level)
        {
            return true;
        }
        else if (outArg.item_level > item_level)
        {
            return false;
        }

        // amount 降序
        if (outArg.amount < amount)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

ContainerModule* ContainerModule::m_pContainerModule = NULL;
AsynCtrlModule* ContainerModule::m_pAsynCtrlModule = NULL;
CoolDownModule *ContainerModule::m_pCoolDownModule = NULL;
ItemBaseModule *ContainerModule::m_pItemBaseModule = NULL;
//EquipmentModule* ContainerModule::m_pEquipmentModule = NULL;

bool ContainerModule::Init(IKernel* pKernel)
{
    m_pContainerModule = this;
    m_pAsynCtrlModule = dynamic_cast<AsynCtrlModule*>(pKernel->GetLogicModule("AsynCtrlModule"));
    m_pCoolDownModule = dynamic_cast<CoolDownModule*>(pKernel->GetLogicModule("CoolDownModule"));
    m_pItemBaseModule = dynamic_cast<ItemBaseModule*>(pKernel->GetLogicModule("ItemBaseModule"));
//    m_pEquipmentModule = dynamic_cast<EquipmentModule*>(pKernel->GetLogicModule("EquipmentModule"));

	Assert(m_pAsynCtrlModule && m_pCoolDownModule && m_pItemBaseModule /*&& m_pEquipmentModule*/);

    pKernel->AddEventCallback("container", "OnNoAdd", ContainerModule::OnNoAdd);
    pKernel->AddEventCallback("container", "OnNoRemove", ContainerModule::OnNoRemove);

    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CONTAINER, ContainerModule::OnClientMsg);

    DECL_LUA_EXT(nx_place_item);
    DECL_LUA_EXT(nx_set_container);

    LoadBoxRule(pKernel);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("ContainerConfig", ContainerModule::ReloadConfig);
#endif
    return true;
}

bool ContainerModule::Shut(IKernel* pKernel)
{
    return true;
}

//
int ContainerModule::OnNoAdd(IKernel* pKernel, const PERSISTID& container,
                             const PERSISTID& sender, const IVarList& args)
{
    int pos = args.IntVal(0);
    if (pos == 0)
    {
        return 0;
    }

    if (!m_pContainerModule->IsValidPos(pKernel, container, pos))
    {
        return 1;
    }

    return 0;
}

//
int ContainerModule::OnNoRemove(IKernel * pKernel, const PERSISTID & container, 
                                const PERSISTID & sender, const IVarList & args)
{
    PERSISTID player = ::find_firsttype_parent(pKernel, container, TYPE_PLAYER);
    if (pKernel->Exists(player)  && m_pAsynCtrlModule->IsWaiting(pKernel, player))
    {
        return 1;
    }

    return 0;
}

int ContainerModule::OnAfterAdd(IKernel * pKernel, const PERSISTID & container, 
                      const PERSISTID & sender, const IVarList & args)
{
    return 0;
}

int ContainerModule::OnRemove(IKernel * pKernel, const PERSISTID & container, 
                    const PERSISTID & sender, const IVarList & args)
{
    return 0;
}

/// 来自玩家的消息
int ContainerModule::OnClientMsg(IKernel* pKernel, const PERSISTID& player, 
                       const PERSISTID& sender, const IVarList& args)
{    /*
         收到的消息

            [msg_id][sub_msg_id][...]

         回应的消息

            [msg_id][sub_msg_id][...]
     */
    int sub_msg_id = args.IntVal(1);

    switch (sub_msg_id)
    {
	case CONTAINER_SUBMSG_MOVEITEM:
		return OnMoveItem(pKernel, player, sender, args);
    default:
        break;
    }

    return 0;
}

// 移动物品
int ContainerModule::OnMoveItem(IKernel* pKernel, const PERSISTID& player, 
							  const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	if (m_pAsynCtrlModule->IsWaiting(pKernel, player))
	{
		return 0;
	}

	if (pPlayerObj->QueryInt("Dead") > 0)
	{
		return 0;
	}
	if ( args.GetCount() < 4 )
	{
		return 0;
	}

	int srcviewid = args.IntVal(2);
	int srcpos = args.IntVal(3);
    int destviewid = 0;
    int destpos = 0;
    //脱装备时客户端取不到对应视图里的位置，因此不再传这两个参数
    if (args.GetCount() >= 6)
    {
        destviewid = args.IntVal(4);
        destpos = args.IntVal(5);
    }

	PERSISTID srcbox = pKernel->GetViewportContainer(player, srcviewid);

    if (!pKernel->Exists(srcbox))
    {
        return 0;
    }
    //判断位置是否正确
    if (srcpos <= 0 || srcpos > GetMaxUsablePos(pKernel, srcbox))
        return 0;

    PERSISTID srcitem = pKernel->GetItem(srcbox, srcpos);
    if (srcitem.IsNull())
        return 0;

	PERSISTID destbox;
    if (destviewid > 0)
    {
        destbox = pKernel->GetViewportContainer(player, destviewid);
    }
    else
    {
        const char* itemConfig = pKernel->GetConfig(srcitem);
        if (StringUtil::CharIsNull(itemConfig))
        {
            return 0;
        }
        destbox = GetBoxContainer(pKernel, player, itemConfig);
        destpos = GetFreePos(pKernel, destbox);
    }

	IGameObj* pDestBoxObj = pKernel->GetGameObj(destbox);
	if (pDestBoxObj == NULL)
	{
		return 0;
	}

	//判断是否可以操作容器
	if (!m_pContainerModule->CanOperateContainer(pKernel, player, srcbox, CONTAINER_OPTYPE_REMOVE, srcpos))
		return 0;

	if (!m_pContainerModule->CanOperateContainer(pKernel, player, destbox, CONTAINER_OPTYPE_ADD, destpos))
		return 0;

	//如果是放入装备栏则先要检测能否装备
// 	if (destviewid == VIEWPORT_EQUIP)
// 	{
// 		EQUIPCHECK_RESULT result = EquipmentModule::m_pEquipmentModule->CanEquip(pKernel, player, srcitem, destpos);
// 		if( result != EQUIPCHECK_RESULT_SUCCEED)
// 		{
// 			fast_string fast_string_id = ::util_int64_as_string(result + 1200);
// 			::CustomSysInfo(pKernel, player, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, fast_string_id.c_str(), CVarList());
// 			return 0;
// 		}
// 	}


	PERSISTID destitem = pKernel->GetItem(destbox, destpos);

    if (destviewid == VIEWPORT_EQUIP && srcviewid == VIEWPORT_ITEM_BOX 
        && destpos != 0)
    {
        if (!pKernel->Exchange(srcbox, srcpos, destbox, destpos))
        {
           // ::CustomSysInfo(pKernel, player, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_CONTAINER_NOT_REPLACE, CVarList());
        }

        return 0;
    }

	//判断源物品是否可以存放到目标容器中
	if (m_pContainerModule->TryAddItem(pKernel, destbox, srcitem, destpos) == 0)
	{
		if (srcviewid != destviewid)
		{
            // 容器空间不足
            CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_3301, CVarList());

			return 0;
		}

	}
	
	//确定能够移动，开始移动物品
	return m_pContainerModule->PlaceItem(pKernel, destbox, srcitem, FUNCTION_EVENT_ID_SYS, false, destpos);
}

//////////////////////////////////////////////////////////////////////////
// 容器维护接口
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 查询接口
//////////////////////////////////////////////////////////////////////////

// 查询物品在容器中的位置，返回0表示不存在
int ContainerModule::GetItemPos(IKernel* pKernel, const PERSISTID& container,
                                const PERSISTID& request_item)
{
    if (!pKernel->Exists(container))
    {
        return 0;
    }
    PERSISTID box;
    const char* script = pKernel->GetScript(container);
    if (strcmp(script, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }
        box = m_pContainerModule->GetBoxContainer(pKernel, self, pKernel->GetConfig(request_item));
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }
    int max_usable_pos = GetMaxUsablePos(pKernel, box);
	// 循环保护
	LoopBeginCheck(a);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; ++i)
    {
		LoopDoCheck(a);
        PERSISTID item = pKernel->GetItem(box, i);
        if (item.IsNull())
        {
            continue;
        }

        if (request_item == item)
        {
            return i;
        }
    }

    return 0;
}

//获得指定物品的数量
int ContainerModule::GetItemCount(IKernel* pKernel,const PERSISTID& container,
									const char* configid, int match /*= ITEM_MATCH_ALL*/ )
{
    if (!pKernel->Exists(container))
    {
        return 0;
    }

	if (StringUtil::CharIsNull(configid))
    {
        return 0;
    }

	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return 0;
		}
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return 0;
		}
	}
	else
	{
		box = container;
	}

	int item_amount = 0;

	int max_usable_pos = GetMaxUsablePos(pKernel, box);
	// 循环保护
	LoopBeginCheck(b);
	for (int i = CONTAINER_START_POS; i <= max_usable_pos; ++i)
	{
		LoopDoCheck(b);
		PERSISTID item = pKernel->GetItem(box, i);
		if (item.IsNull())
		{
			continue;
		}

		// 配置id是否相同
		if (strcmp(pKernel->GetConfig(item), configid) != 0)
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if ( ToolItemModule::IsMatchState(pKernel, item, match))
#endif
		{
			IGameObj* pItemObj = pKernel->GetGameObj(item);
			if (pItemObj != NULL)
			{
				item_amount += pItemObj->QueryInt(TEXT_AMOUNT);
			}
		}
	}

	return item_amount;
}

//从容器中查找指定的物品, 返回找到的第一个物品
PERSISTID ContainerModule::FindItem(IKernel* pKernel, const PERSISTID& container,
	const char* configid, int match/* = ITEM_MATCH_ALL*/)
{
	if (!pKernel->Exists(container))
	{
		return PERSISTID();
	}

	if (StringUtil::CharIsNull(configid))
	{
		return PERSISTID();
	}

	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return PERSISTID();
		}
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return PERSISTID();
		}
	}
	else
	{
		box = container;
	}

	int max_usable_pos = GetMaxUsablePos(pKernel, box);
	// 循环保护
	LoopBeginCheck(c);
	for (int i = CONTAINER_START_POS; i <= max_usable_pos; ++i)
	{
		LoopDoCheck(c);
		PERSISTID item = pKernel->GetItem(box, i);
		if (item.IsNull())
		{
			continue;
		}

		const char* itemConfig = pKernel->GetConfig(item);
		if (StringUtil::CharIsNull(itemConfig))
		{
			continue;
		}

		// 配置id是否相同
		if (strcmp(itemConfig, configid) != 0)
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if ( ToolItemModule::IsMatchState(pKernel, item, match) )
#endif
		{
			return item;
		}
	}

	return PERSISTID();
}


PERSISTID ContainerModule::FindItemBindPrior(IKernel* pKernel, const PERSISTID& container, const char* configid, int match /*= ITEM_MATCH_ALL*/)
{
	if (!pKernel->Exists(container))
	{
		return PERSISTID();
	}

	if (StringUtil::CharIsNull(configid))
	{
		return PERSISTID();
	}

	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return PERSISTID();
		}
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return PERSISTID();
		}
	}
	else
	{
		box = container;
	}

	PERSISTID findItem;
	int max_usable_pos = GetMaxUsablePos(pKernel, box);
	// 循环保护
	LoopBeginCheck(c);
	for (int i = CONTAINER_START_POS; i <= max_usable_pos; ++i)
	{
		LoopDoCheck(c);
		PERSISTID item = pKernel->GetItem(box, i);
		if (item.IsNull())
		{
			continue;
		}
		IGameObj* pItem = pKernel->GetGameObj(item);
		if (NULL == pItem)
		{
			continue;
		}
		const char* itemConfig = pKernel->GetConfig(item);
		if (StringUtil::CharIsNull(itemConfig))
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if (ToolItemModule::IsMatchState(pKernel, item, match))
		{
			int nBound = pKernel->QueryInt(item, FIELD_PROP_BIND_STATUS);
			if (nBound == ITEM_BOUND)
			{
				return item;
			}
#endif
			// 记录第一个找到的
			if (findItem.IsNull())
			{
				findItem = item;
			}
#ifndef FSROOMLOGIC_EXPORTS
		}
#endif
	}

	return findItem;
}

PERSISTID ContainerModule::FindItemByUid(IKernel* pKernel,
										 const PERSISTID& container,
										 const char * uid)
{
	if (!pKernel->Exists(container))
	{
		return PERSISTID();
	}

    if (StringUtil::CharIsNull(uid))
    {
        return PERSISTID();
    }

    int max_usable_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(e);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; ++i)
    {
		LoopDoCheck(e);
        PERSISTID item = pKernel->GetItem(container, i);
        if (item.IsNull())
        {
            continue;
        }

		IGameObj* pItemObj = pKernel->GetGameObj(item);
		if (pItemObj != NULL)
		{
			if (strcmp(pItemObj->QueryString(TEXT_UNIQUE_ID), uid) == 0)
			{
				return item;
			}
		}
    }

	return PERSISTID();
}

// 测试@self是否可以操作指定容器container
bool ContainerModule::CanOperateContainer(IKernel* pKernel, const PERSISTID& self,
                                          const PERSISTID& container, int optype, int pos)
{
	IGameObj* pContainerObj = pKernel->GetGameObj(container);
	if (pContainerObj == NULL)
	{
		return false;
	}

    //玩家可操作容器的标志
    if (!pContainerObj->FindAttr(TEXT_CAN_OP))
    {
        return false;
    }

    //不是自己的容器，不允许操作
    if (!SelfIsOwner(pKernel, self, container))
    {
        return false;
    }

    // 判断操作类型及标志位信息
    if (!CheckContainerOperFlag(pKernel, container, optype))
    {
        return false;
    }

    return true;
}

//获得容器的有效容量(排除锁定的)
int ContainerModule::GetAvailableCapacity(IKernel* pKernel, const PERSISTID& container)
{
	IGameObj* pContainerObj = pKernel->GetGameObj(container);
	if (pContainerObj == NULL)
	{
		return 0;
	}

    const char* script = pKernel->GetScript(container);
    if (StringUtil::CharIsNull(script))
    {
        return 0;
    }
    if (strcmp(script, ITEM_BOX_NAME) == 0)
//         strcmp(script, POWER_BOX_NAME) == 0 ||
//         strcmp(script, BADGE_BOX_NAME) == 0 ||
//         strcmp(script, CHIP_BOX_NAME) == 0)
    {
        return pKernel->GetCapacity(container) - pContainerObj->QueryInt(TEXT_LOCKED_SIZE);
    }
    else
    {
        return pKernel->GetCapacity(container);
    }
}

//获得容器中的所有物品
int ContainerModule::GetAllItems(IKernel* pKernel, const PERSISTID& container, IVarList& res)
{
    if (!pKernel->Exists(container))
    {
        return 0;
    }
    const char* script = pKernel->GetScript(container);

    int max_usable_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(g);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
    {
		LoopDoCheck(g);
        PERSISTID item = pKernel->GetItem(container, i);
        if (item.IsNull())
        {
            continue;
        }

        res << item;
    }

    return (int)res.GetCount();
}

//获得容器中的所有物品
int ContainerModule::GetAllItems(IKernel* pKernel, const PERSISTID& container,
                                 VectorBoxTestItem *item_list)
{
    if (item_list == NULL)
    {
        return 0;
    }

    int max_usable_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(f);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
    {
		LoopDoCheck(f);
        PERSISTID item = pKernel->GetItem(container, i);
		IGameObj* pItemObj = pKernel->GetGameObj(item);
		if (pItemObj != NULL)
		{
            SBoxTestItem simple_item;
            simple_item.config_id = pKernel->GetConfig(item);
			simple_item.amount = pItemObj->QueryInt( FIELD_PROP_AMOUNT );
			simple_item.bind_status = pItemObj->QueryInt( FIELD_PROP_BIND_STATUS );
			simple_item.frozenTime = pItemObj->QueryInt64( FIELD_PROP_ITEM_FROZEN_TIME );

            item_list->push_back(simple_item);
        }
    }

    return (int)item_list->size();
}

//获得一个有效的空闲的位置(为添加物品做准备)
//返回0表示无空闲位置
int ContainerModule::GetFreePos(IKernel* pKernel, const PERSISTID& container)
{
    // 检查容器有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }
//     const char* script = pKernel->GetScript(container);
//     if (strcmp(script, ITEM_BOX_NAME) == 0)
//     {
//         return 0;
//     }

    int max_usable_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(h);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
    {
		LoopDoCheck(h);
        PERSISTID item = pKernel->GetItem(container, i);
        if (item.IsNull())
        {
            return i;
        }
    }

    return 0;
}

// 获取容器中空闲位置的数量
int ContainerModule::GetFreePosCount(IKernel* pKernel, const PERSISTID& container)
{
    if (!pKernel->Exists(container))
    {
        return 0;
    } 
//     const char* script = pKernel->GetScript(container);
//     if (strcmp(script, ITEM_BOX_NAME) == 0)
//     {
//         return 0;
//     }

    int count = 0;
    int max_usable_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(i);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
    {
		LoopDoCheck(i);
        PERSISTID item = pKernel->GetItem(container, i);
        if (item.IsNull())
        {
            ++count;
        }
    }

    return count;
}

//////////////////////////////////////////////////////////////////////////
// 放入物品接口
//////////////////////////////////////////////////////////////////////////

// 尝试把@item放入容器@container中, 返回可以放入的数量
// 此操作不会真正放入物品, 仅仅是计算可放入的数量
int ContainerModule::TryAddItem(IKernel* pKernel, const PERSISTID& container,
                                const PERSISTID& item, int destpos)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (!pKernel->Exists(item))
    {
        return 0;
    }
    PERSISTID box;
    const char* script = pKernel->GetScript(container);
    if (strcmp(script, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }
        box = m_pContainerModule->GetBoxContainer(pKernel, self, pKernel->GetConfig(item));
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }

    if (destpos == 0)
    {
        return TryAddItemAuto(pKernel, box, item);
    }
    else
    {
        return TryAddItemToPosition(pKernel, box, item, destpos);
    }
}

// 尝试把指定数量@item的物品放入容器@container中, 返回可以放入的数量
// 此操作不会真正放入物品, 仅仅是计算可放入的数量
int ContainerModule::TryAddItem(IKernel* pKernel, const PERSISTID& container, SBoxTestItem* request)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (request == NULL)
    {
        return 0;
    }
    PERSISTID box;
    const char* script = pKernel->GetScript(container);
    if (strcmp(script, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }
        box = m_pContainerModule->GetBoxContainer(pKernel, self, request->config_id.c_str());
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }

    // 装备不可以叠加
#ifndef FSROOMLOGIC_EXPORTS
	if (ToolItemModule::IsCanWearItem(pKernel, request->config_id.c_str()))
    {
        int free_pos = GetFreePos(pKernel, box);
        if (free_pos != 0)
        {
            return 1; // 表示可以放入一个装备
        }
        else
        {
            return 0;
        }
    }
#endif
    int endPos = GetMaxUsablePos(pKernel, box);
    int max_amount = atoi(pKernel->GetConfigProperty(request->config_id.c_str(), TEXT_MAX_AMOUNT));

    int allowed_amount = 0;

	// NOTE:判断更改绑定值 [2/23/2017 lihailuo]
	if (request->bind_status == ITEM_BOUND)
	{
#ifndef FSROOMLOGIC_EXPORTS
		request->bind_status = ToolItemModule::IsBindAble(pKernel, request->config_id.c_str());
#endif
	}

	// 循环保护
	LoopBeginCheck(o);
    for (int i = CONTAINER_START_POS; i <= endPos; i++)
    {
		LoopDoCheck(o);
        PERSISTID temp_item = pKernel->GetItem(box, i);
        if (temp_item.IsNull())
        {
            allowed_amount += max_amount;
        }
        else
        {
			if (CanMergeItem(pKernel, temp_item, request))
			{
				IGameObj* pItemObj = pKernel->GetGameObj(temp_item);
				if (pItemObj != NULL)
				{
					int space_left = pItemObj->QueryInt(TEXT_MAX_AMOUNT)
						- pItemObj->QueryInt(TEXT_AMOUNT);

					allowed_amount += space_left;
				}
			}
        }

        if (allowed_amount >= request->amount)
        {
            break;
        }
    }

    return allowed_amount;
}

// 尝试把指定数量@count的物品@config放入容器@container中, 返回可以放入的数量
// 此操作不会真正放入物品, 仅仅是计算可放入的数量
int ContainerModule::TryAddItem(IKernel* pKernel, const PERSISTID& container,
							const char* config, int count, int bound, int64_t frozenTime/* = 0*/)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (StringUtil::CharIsNull(config))
    {
        return 0;
    }

    if (count < 0)
    {
        return 0;
    }

    SBoxTestItem request;
    request.config_id = config;
    request.amount = count;
    request.bind_status = bound;
	request.frozenTime = frozenTime;

    return TryAddItem(pKernel, container, &request);
}

// 尝试把多个物品放入容器中, 全部可以放入返回true
bool ContainerModule::TryAddItems(IKernel* pKernel, const PERSISTID& container, VectorBoxTestItem *item_list)
{
    if (item_list == NULL)
    {
        return false;
    }

    if (!pKernel->Exists(container))
    {
        return false;
    }

	// 整理数据(按背包类型和配置合并方式汇总) [2/7/2017 lihailuo]
	BoxItemData pushData;
	MergeItemData(pKernel, container, *item_list, pushData);

	return TryAddItems(pKernel, pushData);
}

bool ContainerModule::TryAddItems( IKernel* pKernel, BoxItemData& item_datas )
{
	BoxItemData::iterator request_it = item_datas.begin();
	BoxItemData::iterator request_it_end = item_datas.end();

	// 循环保护
	LoopBeginCheck(p);
	for (; request_it != request_it_end; ++request_it)
	{
		LoopDoCheck(p);
		// 放入到某一类型的背包
		PERSISTID box;
		box.nData64 = request_it->first;
		if (!pKernel->Exists(box))
		{
			return false;
		}

		// 此背包重叠后所需要的空格子数
		int nNeedFreePos = 0;
		// 取出现有物品
		VectorBoxTestItem current_items;
		GetAllItems(pKernel, box, &current_items);

		// 测试物品全部放入背包
		VectorBoxTestItem& items = request_it->second;
		for ( VectorBoxTestItem::iterator itr = items.begin(); itr != items.end(); ++itr )
		{
			// 获取物品最大重叠数量
			SBoxTestItem &request_item = *itr;

			// 装备不可以叠加
#ifndef FSROOMLOGIC_EXPORTS
			if (ToolItemModule::IsCanWearItem(pKernel, request_item.config_id.c_str()))
			{
				++nNeedFreePos;
				continue;
			}
#endif

			// NOTE:判断更改绑定值 [2/23/2017 lihailuo]
			if (request_item.bind_status == ITEM_BOUND)
			{
#ifndef FSROOMLOGIC_EXPORTS
				request_item.bind_status = ToolItemModule::IsBindAble(pKernel, request_item.config_id.c_str());
#endif
			}

			int max_amount = atoi(pKernel->GetConfigProperty(request_item.config_id.c_str(), FIELD_PROP_MAX_AMOUNT));

			VectorBoxTestItem::iterator current_it = current_items.begin();
			VectorBoxTestItem::iterator current_it_end = current_items.end();
			// 循环保护
			LoopBeginCheck(q);
			// 堆叠后剩余的数量
			int nLeftAmount = request_item.amount;
			for (; current_it != current_it_end; ++current_it)
			{
				LoopDoCheck(q);
				SBoxTestItem &current_item = *current_it;
				if (!CanMergeItem(pKernel, &current_item, &request_item))
				{
					continue;
				}

				int allowd_amount = max_amount - current_item.amount;
				if (nLeftAmount > allowd_amount)
				{
					current_item.amount = max_amount;
					nLeftAmount -= allowd_amount;
				}
				else
				{
					current_item.amount += request_item.amount;
					nLeftAmount = 0;
					break;
				}
			}

			// 计算还需要几个空格
			if (nLeftAmount > 0)
			{
				if (max_amount == 0)
				{
					max_amount = 1;
				}
				int pos_need = (nLeftAmount + max_amount - 1) / max_amount;
				nNeedFreePos += pos_need;
			}
		}

		// 计算容器中物数量是否足够
		int free_pos_item_box = GetFreePosCount(pKernel, box);
		if (free_pos_item_box < nNeedFreePos)
		{
			return false;
		}
	}

	return true;
}

// 增加物品的数量, 返回实际增加的数量
int ContainerModule::IncItemAmount(IKernel* pKernel, const PERSISTID &item,
                                   int amount, bool trigger_event)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (pItemObj == NULL)
	{
		return 0;
	}

    int max_amount = pItemObj->QueryInt(TEXT_MAX_AMOUNT);
    int cur_amount = pItemObj->QueryInt(TEXT_AMOUNT);

    int allowed_amount = max_amount - cur_amount;
    
    if (allowed_amount < 0)
    {
        // 错误： 当前数量超过最大数量
        return 0;
    }
    
    int saved_amount = 0;

    if (allowed_amount >= amount)
    { 
        saved_amount = amount;
    }
    else
    {
        saved_amount = allowed_amount;
    }

    if (saved_amount > 0)
    {
        pItemObj->SetInt(TEXT_AMOUNT, cur_amount + saved_amount);

        if (trigger_event)
        {
            CVarList after_add_msg;
            after_add_msg << pKernel->GetIndex(item);
            pKernel->RunEventCallback("OnAfterAdd", pKernel->Parent(item), item, after_add_msg);
        }
    }

    return saved_amount;
}


// 删除物品的数量, 返回时间删除的数量
int ContainerModule::DecItemAmount(IKernel* pKernel, const PERSISTID &item,
                                   int amount, bool trigger_event)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (pItemObj == NULL)
	{
		return 0;
	}

    if (amount <= 0)
    {
        return 0;
    }

    int cur_amount = pItemObj->QueryInt(TEXT_AMOUNT);;

    if (cur_amount > amount)
    {
        pItemObj->SetInt(TEXT_AMOUNT, cur_amount - amount);

        if (trigger_event)
        {
            pKernel->RunEventCallback("OnRemove", pKernel->Parent(item), item, CVarList());
        }

        return amount;
    }
    else
    {
        pKernel->DestroySelf(item);
        return cur_amount;
    }
}

//将物品存放到容器中（destpos==0,不指定位置）
int ContainerModule::PlaceItem(IKernel* pKernel, const PERSISTID& container,
							const PERSISTID& item, 
							EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/, bool bIsGain /*= true*/,
							int destpos/* = 0*/)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (!pKernel->Exists(item))
    {
        return 0;
    }

	PERSISTID player;
    PERSISTID box;
    const char* script = pKernel->GetScript(container);
    if (strcmp(script, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }
		player = self;
        box = m_pContainerModule->GetBoxContainer(pKernel, self, pKernel->GetConfig(item));
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }

    if (destpos == 0)
    {
        if (box == pKernel->Parent(item))
        {
            // 同一容器中移动，必须指定位置, 否则不允许
            return 0;
        }

        return PlaceItemAuto(pKernel, player, box, item, srcFunctionId, bIsGain);
    }
    else
    {
		return PlaceItemToPosition(pKernel, player, box, item, destpos, srcFunctionId, bIsGain);
    }
}


// 将多件物品放入容器, 失败返回false
// 如果想确保全部放入，请先调用TryAddItems()进行判断
bool ContainerModule::PlaceItems(IKernel* pKernel, const PERSISTID& container,
						VectorBoxTestItem *item_list, 
						EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/, 
						bool bIsGain /*= true*/)
{
    if (item_list == NULL)
    {
        return false;
    }

    if (!pKernel->Exists(container))
    {
        return false;
    }

    int amount_saved = 0;
   
    VectorBoxTestItem::iterator request_it = item_list->begin();
    VectorBoxTestItem::iterator request_it_end = item_list->end();
	// 循环保护
	LoopBeginCheck(t);
    for (; request_it != request_it_end; ++request_it)
    {
		LoopDoCheck(t);
       SBoxTestItem &request_item = *request_it;

	   amount_saved += PlaceItem(pKernel, container, &request_item, srcFunctionId, bIsGain);
        
    }

    return (amount_saved > 0) ? true : false;
}


bool ContainerModule::PlaceItems(IKernel* pKernel, BoxItemData& item_datas,
							EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/,
							bool bIsGain /*= true*/)
{
	BoxItemData::iterator request_it = item_datas.begin();
	BoxItemData::iterator request_it_end = item_datas.end();

	// 循环保护
	LoopBeginCheck(p);
	for (; request_it != request_it_end; ++request_it)
	{
		LoopDoCheck(p);
		// 放入到某一类型的背包
		PERSISTID box;
		box.nData64 = request_it->first;
		if (!pKernel->Exists(box))
		{
			return false;
		}

		if (!PlaceItems(pKernel, box, &request_it->second, srcFunctionId, bIsGain))
		{
			return false;
		}
	}

	return true;
}

//将物品存放到容器中, 返回放入的数量
int ContainerModule::PlaceItem(IKernel* pKernel, const PERSISTID& container,
							const char *config_id, int request_amount,
							EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/,
							bool bIsGain /*= true*/, 
							int bound /*= ITEM_NOT_BOUND*/, int64_t frozenTime/* = 0*/)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (StringUtil::CharIsNull(config_id))
    {
        return 0;
    }

    if (request_amount < 0)
    {
        return 0;
    }

    SBoxTestItem request;
    request.config_id = config_id;
    request.amount = request_amount;
    request.bind_status = bound;
	request.frozenTime = frozenTime;

	return PlaceItem(pKernel, container, &request, srcFunctionId, bIsGain);
}


// 把物品放入背包，返回放入的数量. 
// @saved_items 里返回实际保存的对象和数量, 格式为:
//      [CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]...[CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]
int ContainerModule::PlaceItem(IKernel* pKernel, const PERSISTID& container,
                     const char *config_id, int request_amount, CVarList &saved_items,
					 EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/,
					 bool bIsGain /*= true*/,
					 int bound /*= ITEM_NOT_BOUND*/, int64_t frozenTime/* = 0*/)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (StringUtil::CharIsNull(config_id))
    {
        return 0;
    }

    if (request_amount < 0)
    {
        return 0;
    }

    SBoxTestItem request;
    request.config_id = config_id;
    request.amount = request_amount;
    request.bind_status = bound;
	request.frozenTime = frozenTime;

    return PlaceItem(pKernel, container, &request, saved_items, srcFunctionId, bIsGain);
}

int ContainerModule::PlaceItem(IKernel* pKernel, const PERSISTID& container, SBoxTestItem *request,
					EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/, bool bIsGain/* = true*/ )
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (request == NULL)
    {
        return 0;
    }

    if (request->amount < 0)
    {
        return 0;
    }

	PERSISTID player;
    PERSISTID box;
    const char* boxScript = pKernel->GetScript(container);
    if (strcmp(boxScript, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }
		player = self;
        box = m_pContainerModule->GetBoxContainer(pKernel, self, request->config_id.c_str());
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }

    int start_Pos = CONTAINER_START_POS;
    int end_pos = GetMaxUsablePos(pKernel, box);
    int max_amount = atoi(pKernel->GetConfigProperty(request->config_id.c_str(), TEXT_MAX_AMOUNT));
    if (max_amount <= 0)
    {
        return 0;
    }

	int accepted_amount = 0;
    // 放入的是装备
#ifndef FSROOMLOGIC_EXPORTS
   if (ToolItemModule::IsCanWearItem(pKernel, request->config_id.c_str()))
    {
        int free_pos = GetFreePos(pKernel, box);
        if (free_pos == 0)
        {
            return 0;
        }

        // ItemBaseModule::CreateItem 接口会为装备设置随机属性
        PERSISTID item = m_pItemBaseModule->CreateItem(pKernel, pKernel->GetScene(),
							request->config_id.c_str(), request->amount, srcFunctionId, bIsGain);
        if (pKernel->Exists(item))
        {
			// 搜集此物品的ID
			request->item = item;

//#ifndef FSROOMLOGIC_EXPORTS
			// NOTE:判断更改绑定值 [2/23/2017 lihailuo]
			if ( request->bind_status == ITEM_BOUND && ToolItemModule::IsBindAble(pKernel, item) )
			{
				pKernel->SetInt(item, FIELD_PROP_BIND_STATUS, request->bind_status);
			}
			// 设置属性
			pKernel->SetInt64(item, FIELD_PROP_ITEM_FROZEN_TIME, request->frozenTime);
//#endif
			pKernel->PlacePos(item, box, free_pos);
			accepted_amount = 1;
			goto LBL_END;
        }
        else
        {
            return 0;
        }
    }
#endif

   // NOTE:判断更改绑定值 [2/23/2017 lihailuo]
   if (request->bind_status == ITEM_BOUND)
   {
#ifndef FSROOMLOGIC_EXPORTS
	   request->bind_status = ToolItemModule::IsBindAble(pKernel, request->config_id.c_str());
#endif
   }

	// 循环保护
	LoopBeginCheck(u);
    for (int i = start_Pos; i <= end_pos; i++)
    {
		LoopDoCheck(u);
        PERSISTID container_item = pKernel->GetItem(box, i);
        if (container_item.IsNull())
        {
            continue;
        }
        else
        {
            if (!CanMergeItem(pKernel, container_item, request))
            {
                continue;
            }

			// 搜集此物品的ID
			request->item = container_item;
            accepted_amount += IncItemAmount(pKernel, container_item,
                                             request->amount-accepted_amount);
        }

        if (accepted_amount >= request->amount)
        {
            break;
        }
    }

    // 剩下的放入空闲格子中
    if (accepted_amount < request->amount)
    {
        int cell_need = ((request->amount - accepted_amount) + max_amount -1) / max_amount;

		// 循环保护
		LoopBeginCheck(v);
        for (int i = 0; i < cell_need; ++i)
        {
			LoopDoCheck(v);
            if (accepted_amount == request->amount)
            {
                break;
            }

            // 找到一个空格子
            int free_pos = GetFreePos(pKernel, box);
            if (free_pos == 0)
            {
				break;
            }

            int save_count = request->amount - accepted_amount;
            save_count = (save_count > max_amount) ? max_amount : save_count;

            PERSISTID item = pKernel->CreateFromConfig(pKernel->GetScene(), "", request->config_id.c_str());
			IGameObj* pItemObj = pKernel->GetGameObj(item);
			if (pItemObj != NULL)
			{
				// 搜集此物品的ID
				request->item = item;

				// 设置属性
				pItemObj->SetInt( FIELD_PROP_BIND_STATUS, request->bind_status);
				pItemObj->SetInt64( FIELD_PROP_ITEM_FROZEN_TIME, request->frozenTime);
                pItemObj->SetInt(TEXT_AMOUNT, save_count);
                pKernel->PlacePos(item, box, free_pos);

                accepted_amount += save_count;
            }
            else
            {
                break;
            }
        }
    }
#ifndef FSROOMLOGIC_EXPORTS
LBL_END:
#endif
	// 物品获得处理
	if (accepted_amount > 0 && !player.IsNull() && bIsGain )
	{
		int nTotalCount = GetItemCount(pKernel, box, request->config_id.c_str());
		OnGainItem(pKernel, player, request->config_id.c_str(), accepted_amount, nTotalCount, srcFunctionId, request->amount);
	}

    return accepted_amount;
}

// 把物品放入背包，返回放入的数量. 
// @result 里返回实际保存的对象和数量, 格式为:
//      [CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]...[CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]
int ContainerModule::PlaceItem(IKernel* pKernel, const PERSISTID& container,
								SBoxTestItem *request, CVarList &saved_items,
								EmFunctionEventId srcFunctionId /*= EmFunctionEventId_NONE*/, bool bIsGain/* = true*/)
{
    // 判断容器的有效性
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (request == NULL)
    {
        return 0;
    }

    if (request->amount < 0)
    {
        return 0;
    }

	PERSISTID player;
    PERSISTID box;
    const char* boxScript = pKernel->GetScript(container);
    if (strcmp(boxScript, ITEM_BOX_NAME) == 0)
    {
        PERSISTID self = pKernel->Parent(container);
        if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
        {
            return 0;
        }

		player = self;
        box = m_pContainerModule->GetBoxContainer(pKernel, self, request->config_id.c_str());
        if (!pKernel->Exists(box))
        {
            return 0;
        }
    }
    else
    {
        box = container;
    }

    int start_Pos = CONTAINER_START_POS;
    int end_pos = GetMaxUsablePos(pKernel, box);
    int max_amount = atoi(pKernel->GetConfigProperty(request->config_id.c_str(), TEXT_MAX_AMOUNT));
    if (max_amount <= 0)
    {
        return 0;
    }

	int accepted_amount = 0;
    // 放入的是装备
#ifndef FSROOMLOGIC_EXPORTS
	if (ToolItemModule::IsCanWearItem(pKernel, request->config_id.c_str()))
    {
        int free_pos = GetFreePos(pKernel, box);
        if (free_pos == 0)
        {
            return 0;
        }

        // ItemBaseModule::CreateItem 接口会为装备设置随机属性
        PERSISTID item = m_pItemBaseModule->CreateItem(pKernel, pKernel->GetScene(),
								request->config_id.c_str(), request->amount, srcFunctionId, bIsGain);

		IGameObj* pItemObj = pKernel->GetGameObj(item);
		if (pItemObj != NULL)
		{
			// 放入的物品信息
            saved_items << pKernel->GetConfig(item);
            saved_items << pItemObj->QueryString(TEXT_UNIQUE_ID);
            saved_items << 1;
            saved_items << pItemObj->QueryInt(TEXT_COLOR_LEVEL);

			// 设置属性
			// NOTE:判断更改绑定值 [2/23/2017 lihailuo]
			if (request->bind_status == ITEM_BOUND && ToolItemModule::IsBindAble(pKernel, item))
			{
				pItemObj->SetInt(FIELD_PROP_BIND_STATUS, request->bind_status);
			}
			pItemObj->SetInt64(FIELD_PROP_ITEM_FROZEN_TIME, request->frozenTime);
			pKernel->PlacePos(item, box, free_pos);

            accepted_amount = 1;
			goto LBL_END;
        }
        else
        {
            pKernel->DestroySelf(item);
            return 0;
        }
    }
#endif

	// NOTE:判断更改绑定值 [2/23/2017 lihailuo]
	if (request->bind_status == ITEM_BOUND)
	{
#ifndef FSROOMLOGIC_EXPORTS
		request->bind_status = ToolItemModule::IsBindAble(pKernel, request->config_id.c_str());
#endif
	}

	// 循环保护
	LoopBeginCheck(w);
    for (int i = start_Pos; i <= end_pos; i++)
    {
		LoopDoCheck(w);
        PERSISTID container_item = pKernel->GetItem(box, i);
		IGameObj* pItemObj = pKernel->GetGameObj(container_item);
		if (pItemObj == NULL)
		{
            continue;
        }
        else
        {
            if (!CanMergeItem(pKernel, container_item, request))
            {
                continue;
            }

            // 预先记录要保存的物品, 物品有可能在 IncItemAmount() 后被摧毁
            const char *container_item_config = pKernel->GetConfig(container_item);
            const char *container_item_uniqueid = pItemObj->QueryString(TEXT_UNIQUE_ID);
            int container_item_color_level = pItemObj->QueryInt(TEXT_COLOR_LEVEL);

            int temp_saved = IncItemAmount(pKernel, container_item,
                request->amount-accepted_amount);

            accepted_amount += temp_saved;

            if (temp_saved > 0)
            {
                // 放入的物品信息
                saved_items << container_item_config;
                saved_items << container_item_uniqueid;
                saved_items << temp_saved;
                saved_items << container_item_color_level;
            }
        }

        if (accepted_amount >= request->amount)
        {
            break;
        }
    }

    // 剩下的放入空闲格子中
    if (accepted_amount < request->amount)
    {
		if (max_amount == 0)
		{
			max_amount = 1;
		}
        int cell_need = ((request->amount - accepted_amount) + max_amount -1) / max_amount;

		// 循环保护
		LoopBeginCheck(x);
        for (int i = 0; i < cell_need; ++i)
        {
			LoopDoCheck(x);
            if (accepted_amount == request->amount)
            {
                break;
            }

            // 找到一个空格子
            int free_pos = GetFreePos(pKernel, box);
            if (free_pos == 0)
            {
                break;
            }

            int save_count = request->amount - accepted_amount;
            save_count = (save_count > max_amount) ? max_amount : save_count;

            PERSISTID item = pKernel->CreateFromConfig(pKernel->GetScene(), "", request->config_id.c_str());
			IGameObj* pItemObj = pKernel->GetGameObj(item);
			if (pItemObj != NULL)
			{
                accepted_amount += save_count;

                // 放入的物品信息
                saved_items << pKernel->GetConfig(item);
                saved_items << pItemObj->QueryString(TEXT_UNIQUE_ID);
                saved_items << save_count;
                saved_items << pItemObj->QueryInt(TEXT_COLOR_LEVEL);

				// 设置属性
                pItemObj->SetInt(TEXT_AMOUNT, save_count);
				pItemObj->SetInt(FIELD_PROP_BIND_STATUS, request->bind_status);
				pItemObj->SetInt64(FIELD_PROP_ITEM_FROZEN_TIME, request->frozenTime);
				pKernel->PlacePos(item, box, free_pos);
            }
            else
            {
                break;
            }
        }
    }
#ifndef FSROOMLOGIC_EXPORTS
LBL_END :
#endif
	// 物品获得处理
	if (accepted_amount > 0 && !player.IsNull() && bIsGain)
	{
		int nTotalCount = GetItemCount(pKernel, box, request->config_id.c_str());
		OnGainItem(pKernel, player, request->config_id.c_str(), accepted_amount, nTotalCount, srcFunctionId, request->amount);
	}

    return accepted_amount;
}

//将物品自动存放到容器中
int ContainerModule::PlaceItemAuto(IKernel* pKernel, const PERSISTID& player, const PERSISTID& container,
				const PERSISTID& request_item, EmFunctionEventId srcFunctionId/* = FUNCTION_EVENT_ID_SYS*/, bool bIsGain/* = true*/)
{
	IGameObj* pItemObj = pKernel->GetGameObj(request_item);
	if (pItemObj == NULL)
	{
		return 0;
	}

    //默认对象不删除
    bool doRemove = false;

    int request_amount = pItemObj->QueryInt(TEXT_AMOUNT);
    int max_amount = pItemObj->QueryInt(TEXT_MAX_AMOUNT);

    if (request_amount <= 0 || request_amount > max_amount)
    {
        return 0;
    }

    // 遍历容器去存放物品

    int start_pos = CONTAINER_START_POS;
    int end_pos = GetMaxUsablePos(pKernel, container);
    int accepted_amount = 0;

	// 循环保护
	LoopBeginCheck(y);
    for (int i = start_pos; i <= end_pos; i++)
    {
		LoopDoCheck(y);
        PERSISTID item_in_container = pKernel->GetItem(container, i);
        if (item_in_container.IsNull())
        {
            // 发现一个空格子

            if (max_amount == 1)
            {
                //此物品无法堆叠, 只能使用空闲位子
                pKernel->PlacePos(request_item, container, i);

                accepted_amount += max_amount;

                break;
            }
        }

        // 物品有可能在容器的OnAfterAdd() 回调中被删除, 不要移除此处的判断
        if (pKernel->Exists(request_item))
        {
            //　尝试合并
            if (CanMergeItem(pKernel, request_item, item_in_container))
            {
                //原有对象可删除
                doRemove = true;

                accepted_amount += IncItemAmount(pKernel, item_in_container, request_amount-accepted_amount);
            }
        }

        if (accepted_amount == request_amount)
        {
            break;
        }
    }

    if (accepted_amount < request_amount)
    {
        int free_pos = GetFreePos(pKernel, container);
        if (free_pos == 0)
        {
            return accepted_amount;
        }

        // 物品有可能在容器的OnAfterAdd() 回调中被删除, 不要移除此处的判断
		IGameObj* pItemObj = pKernel->GetGameObj(request_item);
		if (pItemObj != NULL)
		{
            // 修改ITEM的数量, 然后放入格子中
            pItemObj->SetInt(TEXT_AMOUNT, request_amount-accepted_amount);
            pKernel->PlacePos(request_item, container, free_pos);
            doRemove = false;

            accepted_amount = request_amount;
        }
    }
    //放入空格子的不删除对象，累加数量的删除请求的对象
    if (accepted_amount == request_amount && doRemove)
    {
        pKernel->DestroySelf(request_item);
    }

	// 记录获得日志
	if (bIsGain && accepted_amount > 0)
	{
		const char* pszItemId = pItemObj->GetConfig();
		int nTotalCount = GetItemCount(pKernel, container, pszItemId);
		OnGainItem(pKernel, player, pszItemId, accepted_amount, nTotalCount, srcFunctionId, pItemObj->QueryInt(FIELD_PROP_COLOR_LEVEL));
	}

    return accepted_amount;
}

//放置物品到容器的指定位置
int ContainerModule::PlaceItemToPosition(IKernel* pKernel,
										const PERSISTID& player,
                                         const PERSISTID& container,
                                         const PERSISTID& request_item,
										 int pos, EmFunctionEventId srcFunctionId/* = FUNCTION_EVENT_ID_SYS*/, bool bIsGain/* = true*/)
{
    // 检查位置是否有效
    if (pos < CONTAINER_START_POS 
        || pos > GetMaxUsablePos(pKernel, container))
    {
        return 0;
    }

	// 非装备
	IGameObj* pItemObj = pKernel->GetGameObj(request_item);
	if (pItemObj == NULL)
	{
		return 0;
	}

    // 装备的放入
#ifndef FSROOMLOGIC_EXPORTS
	if (ToolItemModule::IsCanWearItem(pKernel, pItemObj->GetConfig()))
    {
        // 装备只能放入空格子中
        PERSISTID dest_item = pKernel->GetItem(container, pos);
        if (dest_item.IsNull())
        {
            pKernel->PlacePos(request_item, container, pos);

            return 1;
        }
        else
        {
            return 0;
        }
    }
#endif

    int request_amount = pItemObj->QueryInt(TEXT_AMOUNT);
    int max_amount = pItemObj->QueryInt(TEXT_MAX_AMOUNT);

    if (request_amount <=0 || request_amount > max_amount)
    {
        return 0;
    }

    // 获取指定位置的物品
    PERSISTID dest_item = pKernel->GetItem(container, pos);

	int accept_count = request_amount;
    if (dest_item.IsNull())
    {
        // 空格子直接放入
        pKernel->PlacePos(request_item, container, pos);
    }
	else if (CanMergeItem(pKernel, request_item, dest_item))
    {
        // 合并
        int saved = IncItemAmount(pKernel, dest_item, request_amount);
        if (saved == request_amount)
        {
            pKernel->DestroySelf(request_item);
        }
        else if (saved > 0)
        {
            pItemObj->SetInt(TEXT_AMOUNT, request_amount - saved);
        }

		accept_count = saved;
    }

	// 记录获得日志
	if (bIsGain && accept_count > 0)
	{
		const char* pszItemId = pItemObj->GetConfig();
		int nTotalCount = GetItemCount(pKernel, container, pszItemId);
		OnGainItem(pKernel, player, pszItemId, accept_count, nTotalCount, srcFunctionId, pItemObj->QueryInt(FIELD_PROP_COLOR_LEVEL));
	}
	return accept_count;
}

void ContainerModule::OnGainItem(IKernel* pKernel, const PERSISTID& player, const char* szItemId, int nNum, int nTatalNum, EmFunctionEventId src, int nColorLv)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 是否不需要记日志
	if (!IsNeedSaveItemLog(pKernel, szItemId, nColorLv))
	{
		return;
	}

	// 品质
	int color = StringUtil::StringAsInt(pKernel->GetConfigProperty(szItemId, FIELD_PROP_COLOR_LEVEL));
	if (ToolItemModule::IsCanWearItem(pKernel, szItemId))
	{
		color = nColorLv;
	}
	
	//奖励物品日志
	ItemLog log;
	log.eventID = src;
	log.eventType = LOG_ACT_EVENT_GAIN;
	log.itemID = szItemId;
	log.itemNum = nNum;
	log.state = LOG_ACT_STATE_SUCCESS;

	log.after = nTatalNum;
	log.before = log.after - nNum;
	log.colorLevel = color;

	LogModule::m_pLogModule->SaveItemLog(pKernel, player, log);

	// 稀有物品提示
	ToolItemModule::m_pToolItemModule->TipsSpecialItem(pKernel, player, szItemId, color, src);
#endif
}

void ContainerModule::OnLoseItem(IKernel* pKernel, const PERSISTID& player, const char* szItemId, int nNum, int nTatalNum, EmFunctionEventId src)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 是否不需要记日志
	if (!IsNeedSaveItemLog(pKernel, szItemId, 0))
	{
		return;
	}
	// 失去物品日志
	ItemLog log;
	log.eventID = src;
	log.eventType = LOG_ACT_EVENT_LOSE;
	log.itemID = szItemId;
	log.itemNum = nNum;
	log.state = LOG_ACT_STATE_SUCCESS;

	log.after = nTatalNum;
	log.before = log.after + nNum;

	LogModule::m_pLogModule->SaveItemLog(pKernel, player, log);

#endif
}

//////////////////////////////////////////////////////////////////////////
// 删除物品接口
//////////////////////////////////////////////////////////////////////////

// 尝试从容器中删除指定物品的数量, 可以删除指定的数量返回true
// 此操作不会真正删除物品
bool ContainerModule::TryRemoveItems(IKernel* pKernel, const PERSISTID& container,
                                     const char* configid, int count, 
									 int match /*= ITEM_MATCH_ALL*/)
{
    if (!pKernel->Exists(container))
    {
        return false;
    }

    if (StringUtil::CharIsNull(configid))
    {
        return false;
    }

    if (count <= 0)
    {
        return false;
    }

	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return false;
		}
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return false;
		}
	}
	else
	{
		box = container;
	}

	int request_amount = count;
	// 当要删除的物品是装备时, 数量为1
	bool isEquip = false;
#ifndef FSROOMLOGIC_EXPORTS
	if (ToolItemModule::IsCanWearItem(pKernel, configid))
	{
		isEquip = true;
	}
#endif
	int end_pos = GetMaxUsablePos(pKernel, box);
	int allowed_amount = 0;

	// 循环保护
	LoopBeginCheck(z);
	for (int i = CONTAINER_START_POS; i <= end_pos; i++)
	{
		LoopDoCheck(z);
		PERSISTID item = pKernel->GetItem(box, i);

		IGameObj* pItemObj = pKernel->GetGameObj(item);
		if (pItemObj == NULL)
		{
			continue;
		}

		// 配置id是否相同
		if (strcmp(pItemObj->GetConfig(), configid) != 0)
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if ( ToolItemModule::IsMatchState(pKernel, item, match) )
#endif
		{
			if (isEquip)
			{
				++allowed_amount;
			}
			else
			{
				allowed_amount += pItemObj->QueryInt(FIELD_PROP_AMOUNT);
			}
		}

		if (allowed_amount >= request_amount)
		{
			return true;
		}
	}

	return false;
}

bool ContainerModule::TryRemoveItems( IKernel* pKernel, const PERSISTID& container, 
					const std::map<std::string, int>& items, int match/* = ITEM_MATCH_ALL*/)
{
	for ( std::map<std::string, int>::const_iterator itr = items.begin(); itr != items.end(); ++itr )
	{
		if ( !TryRemoveItems( pKernel, container, itr->first.c_str(), itr->second, match) )
		{
			return false;
		}
	}
	return true;
}


//从容器中删除指定物品的数量, 返回删除的数量
int ContainerModule::RemoveItems(IKernel* pKernel, const PERSISTID& container,
                                 const char* configid, int count,
								 EmFunctionEventId srcFunctionId/* = EmFunctionEventId_NONE*/, bool bIsConsume/* = true*/,
								 int match /*= ITEM_MATCH_ALL*/)
{
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    if (StringUtil::CharIsNull(configid))
    {
        return 0;
    }

    if (count <= 0)
    {
        return 0;
    }

	PERSISTID player;
	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return 0;
		}
		player = self;
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return 0;
		}
	}
	else
	{
		box = container;
	}

	int request_amount = count;
	int end_pos = GetMaxUsablePos(pKernel, box);
	int removed_amount = 0;

	// 循环保护
	LoopBeginCheck(aa);
	for (int i = CONTAINER_START_POS; i <= end_pos; i++)
	{
		LoopDoCheck(aa);
		PERSISTID item = pKernel->GetItem(box, i);

		if (item.IsNull())
		{
			continue;
		}

		// 查询种类是否相同
		if (strcmp(pKernel->GetConfig(item), configid) != 0)
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if (!ToolItemModule::IsMatchState(pKernel, item, match))
		{
			continue;
		}
#endif
		removed_amount += DecItemAmount(pKernel, item, request_amount - removed_amount);
		if (removed_amount >= request_amount)
		{
			break;
		}
	}

	// 记录消耗日志
	if (!player.IsNull() && bIsConsume && removed_amount > 0)
	{
		int nTotalCount = GetItemCount(pKernel, box, configid);
		OnLoseItem(pKernel, player, configid, removed_amount, nTotalCount, srcFunctionId);
	}

	return removed_amount;
}

bool ContainerModule::RemoveItems( IKernel* pKernel, const PERSISTID& container, 
					const std::map<std::string, int>& items, std::map<std::string, int>* pResult/* = NULL*/,
					EmFunctionEventId srcFunctionId/* = EmFunctionEventId_NONE*/, bool bIsConsume/* = true*/,
					int match/* = ITEM_MATCH_ALL*/)
{
	bool bRemoveAll = true;

	LoopBeginCheck(a);
	for ( std::map<std::string, int>::const_iterator itr = items.begin();
		itr != items.end(); ++itr )
	{
		LoopDoCheck(a);
		int nNum = RemoveItems(pKernel, container, itr->first.c_str(), itr->second, srcFunctionId, bIsConsume, match);
		if ( bRemoveAll && nNum != itr->second )
		{
			bRemoveAll = false;
		}

		if ( NULL != pResult )
		{
			pResult->insert( std::make_pair(itr->first, nNum ));
		}
	}

	return bRemoveAll;
}

int ContainerModule::RemoveItem(IKernel* pKernel, const PERSISTID& container, const PERSISTID& item, int count, 
			EmFunctionEventId srcFunctionId /*= FUNCTION_EVENT_ID_SYS*/, bool bIsConsume /*= true */)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (NULL == pItemObj || !pKernel->Exists(container))
	{
		return 0;
	}

	PERSISTID self = pKernel->Parent(container);
	if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
	{
		return 0;
	}

	int nRemoved = 0;
	const char* strItem = pItemObj->GetConfig();
#ifndef FSROOMLOGIC_EXPORTS
	if (ToolItemModule::IsCanWearItem(pKernel, strItem))
	{
		nRemoved = RemoveWearItem(pKernel, self, item, srcFunctionId);
	}
	else
#endif
	{
		if (count <= 0)
		{
			return 0;
		}
		// update by kevin 2017-07-06 16:44
		IGameObj* pDcObj = pKernel->GetGameObj(item);
		Assert(pDcObj != NULL);

		int nAmount = pDcObj->QueryInt( FIELD_PROP_AMOUNT);
		const char* pszItemId = pKernel->GetConfig(item);
		nRemoved = count;
		if (nAmount > count)
		{
			pDcObj->SetInt( FIELD_PROP_AMOUNT, nAmount - count);
		}
		else
		{
			nRemoved = nAmount;
			pKernel->Destroy(container, item);
		}

		if (bIsConsume)
		{
			int nTotalCount = GetItemCount(pKernel, container, pszItemId);
			OnLoseItem(pKernel, self, pszItemId, nRemoved, nTotalCount, srcFunctionId);
		}
	}

	return nRemoved;
}

int ContainerModule::RemoveItemsBindPrior(IKernel* pKernel, const PERSISTID& container, const char* configid, int count,
							EmFunctionEventId srcFunctionId/* = EmFunctionEventId_NONE*/, bool bIsConsume/* = true*/,
							int match /*= ITEM_MATCH_ALL*/)
{
	if (!pKernel->Exists(container))
	{
		return 0;
	}

	if (StringUtil::CharIsNull(configid))
	{
		return 0;
	}

	if (count <= 0)
	{
		return 0;
	}

	PERSISTID player;
	PERSISTID box;
	const char* script = pKernel->GetScript(container);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID self = pKernel->Parent(container);
		if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
		{
			return 0;
		}
		player = self;
		box = m_pContainerModule->GetBoxContainer(pKernel, self, configid);
		if (!pKernel->Exists(box))
		{
			return 0;
		}
	}
	else
	{
		box = container;
	}

	int request_amount = count;
	int end_pos = GetMaxUsablePos(pKernel, box);
	int removed_amount = 0;

	// 临时保存非绑定数对象
	std::vector<PERSISTID> vecUnBindItems;

	// 循环保护
	LoopBeginCheck(aa);
	for (int i = CONTAINER_START_POS; i <= end_pos; i++)
	{
		LoopDoCheck(aa);
		PERSISTID item = pKernel->GetItem(box, i);

		if (item.IsNull())
		{
			continue;
		}

		// 查询种类是否相同
		if (strcmp(pKernel->GetConfig(item), configid) != 0)
		{
			continue;
		}

#ifndef FSROOMLOGIC_EXPORTS
		if (!ToolItemModule::IsMatchState(pKernel, item, match))
		{
			continue;
		}

		int nBound = pKernel->QueryInt(item, FIELD_PROP_BIND_STATUS);
		if (nBound != ITEM_BOUND)
		{
			vecUnBindItems.push_back(item);
			continue;
		}
#endif
		removed_amount += DecItemAmount(pKernel, item, request_amount - removed_amount);
		if (removed_amount >= request_amount)
		{
			break;
		}
	}

	// 未达到数量从非绑定对象中移除
	if (removed_amount < request_amount)
	{
		for ( std::vector<PERSISTID>::iterator itr = vecUnBindItems.begin(); itr != vecUnBindItems.end(); ++itr )
		{
			removed_amount += DecItemAmount(pKernel, *itr, request_amount - removed_amount);
			if (removed_amount >= request_amount)
			{
				break;
			}
		}
	}

	// 记录消耗日志
	if (!player.IsNull() && bIsConsume && removed_amount > 0)
	{
		int nTotalCount = GetItemCount(pKernel, box, configid);
		OnLoseItem(pKernel, player, configid, removed_amount, nTotalCount, srcFunctionId);
	}

	return removed_amount;
}

bool ContainerModule::RemoveItemsBindPrior(IKernel* pKernel, const PERSISTID& container, const std::map<std::string, int>& items,
											std::map<std::string, int>* pResult /*= NULL*/,
											EmFunctionEventId srcFunctionId/* = EmFunctionEventId_NONE*/, bool bIsConsume/* = true*/,
											int match /*= ITEM_MATCH_ALL*/)
{
	bool bRemoveAll = true;

	LoopBeginCheck(a);
	for (std::map<std::string, int>::const_iterator itr = items.begin();
		itr != items.end(); ++itr)
	{
		LoopDoCheck(a);
		int nNum = RemoveItemsBindPrior(pKernel, container, itr->first.c_str(), itr->second, srcFunctionId, bIsConsume, match);
		if (bRemoveAll && nNum != itr->second)
		{
			bRemoveAll = false;
		}

		if (NULL != pResult)
		{
			pResult->insert(std::make_pair(itr->first, nNum));
		}
	}

	return bRemoveAll;
}

// 从容器中删除可穿戴物品(装备、砭石)
int ContainerModule::RemoveWearItem(IKernel* pKernel, const PERSISTID& player, const PERSISTID& item, EmFunctionEventId srcFunctionId)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (NULL == pItemObj || !pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER)
	{
		return 0;
	}

	const char* strItem = pItemObj->GetConfig();
#ifndef FSROOMLOGIC_EXPORTS
	if (!ToolItemModule::IsCanWearItem(pKernel, strItem))
	{
		return 0;
	}
#endif
	int nColorLevel = pItemObj->QueryInt(FIELD_PROP_COLOR_LEVEL);
	const char* strItemUid = pItemObj->QueryString(FIELD_PROP_UNIQUE_ID);

	pKernel->DestroySelf(item);
	
	// 是否不需要记日志
	if (!IsNeedSaveItemLog(pKernel, strItem, nColorLevel))
	{
		return 1;
	}
	// 失去物品日志
	ItemLog log;
	log.eventID = srcFunctionId;
	log.eventType = LOG_ACT_EVENT_LOSE;
	log.itemID = strItem;
	log.itemNum = 0;
	log.state = LOG_ACT_STATE_SUCCESS;
	log.itemUid = strItemUid;
	log.colorLevel = nColorLevel;

	log.after = 0;
	log.before = 1;

	LogModule::m_pLogModule->SaveItemLog(pKernel, player, log);

	return 1;
}

// 清空容器
bool ContainerModule::ClearAllItems(IKernel* pKernel,  const PERSISTID& self, const PERSISTID& container)
{
	IGameObj* pContainerObj = pKernel->GetGameObj(container);
	if (pContainerObj == NULL)
	{
		return false;
	}

    // 判断是否可以操作容器
    if (!m_pContainerModule->CanOperateContainer(pKernel, self, container, CONTAINER_OPTYPE_ALL, 0))
    {
        return false;
    }

    //不是自己的容器，不允许操作
    if (!SelfIsOwner(pKernel, self, container))
    {
        return false;
    }

    // 判断是否允许执行整理操作
    if (pContainerObj->QueryInt("CantArrange") > 0)
    {
        return false;
    }

    // 获取物品对象，个数为0则返回
    int end_pos = GetMaxUsablePos(pKernel, container);

	// 循环保护
	LoopBeginCheck(ab);
    for (int i = CONTAINER_START_POS; i <= end_pos; i++)
    {
		LoopDoCheck(ab);
        PERSISTID item = pKernel->GetItem(container, i);

        if (item.IsNull())
        {
            continue;
        }

        pKernel->DestroySelf(item);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// 排序整理接口
//////////////////////////////////////////////////////////////////////////

bool ContainerModule::ArrangeItem(IKernel* pKernel, const PERSISTID& container, int cooldown_ms)
{
    if (!pKernel->Exists(container))
    {
        return false;
    }

    // 找到此容器的玩家
    PERSISTID player = ::find_firsttype_parent(pKernel, container, TYPE_PLAYER);
    if (!pKernel->Exists(player))
    {
        return false;
    }

	// 判断是背包才判断冷却 [6/30/2015 liumf]
	const char* strScript = pKernel->GetScript(container);

    bool isBag = false;
    int coolDownType = COOLDOWN_ARRANGE_TOOLBOX;
    if (strcmp(strScript, ITEM_BOX_NAME) == 0)
    {
        isBag = true;
        coolDownType = COOLDOWN_ARRANGE_ITEMBOX;
    }
//     else if (strcmp(strScript, POWER_BOX_NAME) == 0)
//     {
//         isBag = true;
//         coolDownType = COOLDOWN_ARRANGE_POWERBOX;
//     }
//     else if (strcmp(strScript, BADGE_BOX_NAME) == 0)
//     {
//         isBag = true;
//         coolDownType = COOLDOWN_ARRANGE_BADGEBOX;
//     }
//     else if (strcmp(strScript, CHIP_BOX_NAME) == 0)
//     {
//         isBag = true;
//         coolDownType = COOLDOWN_ARRANGE_CHIPBOX;
//     }

    if (isBag && m_pCoolDownModule->IsCoolDown(pKernel, player, coolDownType))
    {
        // 冷却的剩余时间
        int64_t millisec_left = m_pCoolDownModule->GetMillisecRemain(pKernel, player, coolDownType);
        int sec_left = (int)(millisec_left/1000);
        
        // 因为已经判定在冷却中, 所以不能返回小于0的冷却时间
        sec_left = sec_left <= 0 ? 1 : sec_left;
        
        CVarList args;
        args << sec_left;
        
        //整理物品的频率太快，请稍等片刻
        ::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_3302, args);
        return false;
    }

    // 合并后的物品
    std::vector<PERSISTID> merged_items;

    // 先合并物品
	// 循环保护
	LoopBeginCheck(ac);
    int max_usable_pos = GetMaxUsablePos(pKernel, container);
    for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
    {
		LoopDoCheck(ac);
        PERSISTID container_item = pKernel->GetItem(container, i);
		IGameObj* pContainerObj = pKernel->GetGameObj(container_item);
		if (pContainerObj == NULL)
		{
			continue;
		}

        int max_amount = pContainerObj->QueryInt(TEXT_MAX_AMOUNT);
        int container_item_amount = pContainerObj->QueryInt(TEXT_AMOUNT);

        if (container_item_amount == max_amount)
        {
            // 物品数量已经堆叠到最大, 直接塞入列表中
            merged_items.push_back(container_item);
            continue;
        }

        std::vector<PERSISTID>::iterator merged_it = merged_items.begin();
        std::vector<PERSISTID>::iterator merged_it_end = merged_items.end();
		// 循环保护
		LoopBeginCheck(ad);
        for (; merged_it != merged_it_end; ++merged_it)
        {
			LoopDoCheck(ad);
            PERSISTID merged_item = *merged_it;
			IGameObj* pItemObj = pKernel->GetGameObj(merged_item);
			if (pItemObj == NULL)
			{
				// 非正常情况
				return false;
			}

            int merged_item_amount = pItemObj->QueryInt(TEXT_AMOUNT);
            if (merged_item_amount == max_amount)
            {
                continue;
            }

            if (!CanMergeItem(pKernel, merged_item, container_item))
            {
                continue;
            }
			
			// 合并到其他同类物品上
            int saved_amount = IncItemAmount(pKernel, merged_item, container_item_amount, false);
			// 合并后剩余的数量
			container_item_amount -= saved_amount;
            // 数量无 合并完成
			if (container_item_amount <= 0)
			{
				break;
			}
        }// End of [merge_items] loop

		// 合并后
        if (container_item_amount > 0)
        {
			// 仍然有剩余，则更改为当前剩余数量
			pContainerObj->SetInt(TEXT_AMOUNT, container_item_amount);
			// 加入可以合并队列 用来合并以后的同类物品
            merged_items.push_back(container_item);
        }
		else
		{
			// 全部被合并掉了, 则删除此物品
			pKernel->DestroySelf(container_item);
		}
    }

    std::list<S_SORT_ITEM> items_list;

    std::vector<PERSISTID>::iterator merged_it = merged_items.begin();
    std::vector<PERSISTID>::iterator merged_it_end = merged_items.end();
	// 循环保护
	LoopBeginCheck(ae);
    for (; merged_it != merged_it_end; ++merged_it)
    {
		LoopDoCheck(ae);
        PERSISTID merged_item = *merged_it;
        if (!pKernel->Exists(merged_item))
        {
            // 非正常情况
            continue;
        }
    
        IGameObj *item_obj = pKernel->GetGameObj(merged_item);
        if (item_obj == NULL)
        {
            continue;
        }

        const char *item_script = item_obj->GetScript();

        S_SORT_ITEM sort_item;
        sort_item.obj_id = merged_item;
        sort_item.item_type = item_obj->QueryInt("ItemType");
        sort_item.color_level = item_obj->QueryInt("ColorLevel");
        sort_item.amount = item_obj->QueryInt("Amount");
        
		if (strcmp(item_script, "Equipment") == 0)
        {
            // 装备类型
            const char *equip_type = item_obj->QueryString("EquipType");
            
            sort_item.item_type = 0; // 装备忽略类型排序
            sort_item.item_level = item_obj->QueryInt("LimitLevel");
           // sort_item.item_pos = StaticDataQueryModule::m_pInstance->QueryInt(STATIC_DATA_GOBAL_CONST_PACK, GLOBAL_CONST_SEC, equip_type);
        }
		else if (strcmp(item_script, "PartItem") == 0 || strcmp(item_script, "Bianstone") == 0)
        {
            if (item_obj->FindAttr("LimitLevel"))
            {
                sort_item.item_level = item_obj->QueryInt("LimitLevel");
            }
            
        }
        else
        {
            if (item_obj->FindAttr("LimitLevel"))
            {
                sort_item.item_level = item_obj->QueryInt("LimitLevel");
            }
        }

        items_list.push_back(sort_item);
    }

    // 排序
    items_list.sort();

    // 把排序好的物品放入容器中
    int sorted_count = (int)items_list.size();
    int pos_idx;
	// 循环保护
	LoopBeginCheck(af);
    for (int i = 0; i < sorted_count; i++)
    {
		LoopDoCheck(af);
        pos_idx = i+1;

        PERSISTID item = items_list.front().obj_id;
        int item_idx = pKernel->GetIndex(item);

        PERSISTID item_in_pos_idx = pKernel->GetItem(container, pos_idx);
        if (item_in_pos_idx.IsNull())
        {
            pKernel->PlacePos(item, container, pos_idx);
        }
        else
        {
            if (pos_idx != item_idx)
            {
                pKernel->Exchange(container, pos_idx, container, item_idx);
            }
        }

        items_list.pop_front();
    }

    // 是背包,开始冷却
    if (strcmp(strScript, ITEM_BOX_NAME) == 0)
    {
        m_pCoolDownModule->BeginCoolDown(pKernel, player, coolDownType, cooldown_ms);
    }

    return true;
}

//是否可以合并物品
bool ContainerModule::CanMergeItem(IKernel* pKernel, const PERSISTID& srcitem, const PERSISTID& destitem)
{
	IGameObj* pSrcItemObj = pKernel->GetGameObj(srcitem);
	if (pSrcItemObj == NULL)
	{
		return false;
	}

	IGameObj* pDestItemObj = pKernel->GetGameObj(destitem);
	if (pDestItemObj == NULL)
	{
		return false;
	}

    // 自己不能和自己合并
    if (srcitem == destitem)
    {
        return false;
    }

    //判断是否同类物品
    const char* src_config = pKernel->GetConfig(srcitem);
    const char* dest_config = pKernel->GetConfig(destitem);

    if (strcmp(src_config, dest_config) != 0)
    {
        return false;
    }
    if (pKernel->FindConfigProperty(src_config, TEXT_MAX_AMOUNT))
    {
        int max_amount = atoi(pKernel->GetConfigProperty(src_config, TEXT_MAX_AMOUNT));
        if (max_amount <= 1)
        {
            return false;
        }
    }
    // 检查绑定属性
    bool src_has_bind_config = pSrcItemObj->FindAttr(TEXT_ITEM_BIND_STATUS);
    bool dest_has_bind_config = pDestItemObj->FindAttr(TEXT_ITEM_BIND_STATUS);
    if (src_has_bind_config && dest_has_bind_config)
    {
        if (pSrcItemObj->QueryInt(TEXT_ITEM_BIND_STATUS) != pDestItemObj->QueryInt(TEXT_ITEM_BIND_STATUS))
        {
            return false;
        }
    }
    else if (src_has_bind_config != dest_has_bind_config)
    {
        return false;
    }

	// 冻结物品时间不一致不可以合并
	int64_t nSrcFrozenTime = pSrcItemObj->QueryInt64(FIELD_PROP_ITEM_FROZEN_TIME);
	int64_t nDstFrozenTime = pDestItemObj->QueryInt64(FIELD_PROP_ITEM_FROZEN_TIME);
	if (nSrcFrozenTime != nDstFrozenTime)
	{
		// 判断是否过了冻结期
		int64_t tNow = util_get_utc_time();
		if (nSrcFrozenTime > tNow || nDstFrozenTime > tNow)
		{
			return false;
		}
	}

    return true;
}

// 是否可以把@request 的物品合并到@dest_item 上
bool ContainerModule::CanMergeItem(IKernel* pKernel, const PERSISTID& dest_item,
                                   const SBoxTestItem *request)
{
	IGameObj* pItemObj = pKernel->GetGameObj(dest_item);
	if (pItemObj == NULL)
	{
		return false;
	}

    if (request == NULL)
    {
        return false;
    }

	const char *script = pKernel->GetConfigProperty(request->config_id.c_str(), "Script");
	bool is_soul = strcmp(script, "Soul") == 0;
    if (pKernel->FindConfigProperty(request->config_id.c_str(), TEXT_MAX_AMOUNT) && !is_soul)
    {
        int max_amount = atoi(pKernel->GetConfigProperty(request->config_id.c_str(), TEXT_MAX_AMOUNT));
        if (max_amount <= 1)
        {
            return false;
        }
    }


	if (strcmp(pKernel->GetConfig(dest_item), request->config_id.c_str()) != 0)
	{
		return false;
	}

    // 检查绑定属性
    if (pItemObj->FindAttr(TEXT_ITEM_BIND_STATUS))
    {
        if (pItemObj->QueryInt(TEXT_ITEM_BIND_STATUS) != request->bind_status)
        {
            return false;
        }
    }
	else if (request->bind_status == ITEM_BOUND)
	{
		return false;
	}
	
	// 冻结物品时间不一致不可以合并
	int64_t nSrcFrozenTime = pItemObj->QueryInt64(FIELD_PROP_ITEM_FROZEN_TIME);
	if (nSrcFrozenTime != request->frozenTime)
	{
		// 判断是否过了冻结期
		int64_t tNow = util_get_utc_time();
		if (nSrcFrozenTime > tNow || request->frozenTime > tNow)
		{
			return false;
		}
	}

    return true;
}

// 是否可以合并两个物品
bool ContainerModule::CanMergeItem(IKernel* pKernel, const SBoxTestItem *dest_item,
                                   const SBoxTestItem *request_item)
{
    if (dest_item == NULL || request_item == NULL)
    {
        return false;
    }

    if (dest_item->config_id != request_item->config_id)
    {
        return false;
    }

    if (dest_item->bind_status != request_item->bind_status)
    {
        return false;
    }

	// 冻结物品时间不一致不可以合并
	if (dest_item->frozenTime != request_item->frozenTime)
	{
		// 判断是否过了冻结期
		int64_t tNow = util_get_utc_time();
		if (dest_item->frozenTime > tNow || request_item->frozenTime > tNow)
		{
			return false;
		}
	}

    if (pKernel->FindConfigProperty(dest_item->config_id.c_str(), TEXT_MAX_AMOUNT))
    {
        int max_amount = atoi(pKernel->GetConfigProperty(dest_item->config_id.c_str(), TEXT_MAX_AMOUNT));
        if (max_amount <= 1)
        {
            return false;
        }
    }
    return true;
}

bool ContainerModule::SelfIsOwner(IKernel* pKernel,
                                      const PERSISTID& self,
                                      const PERSISTID& container)
{
    PERSISTID owner = ::find_firsttype_parent(pKernel, container, TYPE_PLAYER);
    if (!pKernel->Exists(owner) || owner != self)
    {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
// 私有的辅助函数
//////////////////////////////////////////////////////////////////////////


bool ContainerModule::CheckContainerOperFlag(IKernel* pKernel,
                                             const PERSISTID& container,
                                             int opType)
{
	IGameObj* pContainerObj = pKernel->GetGameObj(container);
	if (pContainerObj == NULL)
	{
		return false;
	}

    switch (opType)
    {
    case CONTAINER_OPTYPE_ADD:
        {
            if (pContainerObj->QueryInt("CantAdd") > 0)
            {
                return false;
            }
        }
        break;
    case CONTAINER_OPTYPE_REMOVE:
        {
            if (pContainerObj->QueryInt("CantRemove") > 0)
            {
                return false;
            }
        }
        break;
    case CONTAINER_OPTYPE_ALL:
        {
            if (pContainerObj->QueryInt("CantAdd") > 0 ||
                pContainerObj->QueryInt("CantRemove") > 0)
            {
                return false;
            }
        }
        break;
    }
    return true;
}

//判断是否是一个有效的位置(锁定的格子不是有效的位置)
bool ContainerModule::IsValidPos(IKernel* pKernel,
                                     const PERSISTID& container,
                                     int pos)
{
    // 检查容器有效性
    if (!pKernel->Exists(container))
    {
        return false;
    }

    if (pos < CONTAINER_START_POS
        || pos > GetMaxUsablePos(pKernel, container))
    {
        return false;
    }

    return true;
}

// 获取可用位置中，最大的那个
int ContainerModule::GetMaxUsablePos(IKernel* pKernel, const PERSISTID& container)
{
	IGameObj* pContainerObj = pKernel->GetGameObj(container);
	if (pContainerObj == NULL)
	{
		return 0;
	}

    int max_usable_pos = pKernel->GetCapacity(container);
    
    if (strcmp(pKernel->GetScript(container), ITEM_BOX_NAME) == 0)
    {
        max_usable_pos -= pContainerObj->QueryInt(TEXT_LOCKED_SIZE);
    }                     

    return max_usable_pos;
}

void ContainerModule::ReloadConfig(IKernel* pKernel)
{
	m_pContainerModule->LoadBoxRule(pKernel);
}

// 尝试把@item放入容器@container中, 返回可以放入的数量
// 此操作不会真正放入物品, 仅仅是计算可放入的数量
int ContainerModule::TryAddItemAuto(IKernel* pKernel, const PERSISTID& container,
                                     const PERSISTID& item)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (pItemObj == NULL)
	{
		return 0;
	}

    int request_amount = pItemObj->QueryInt(TEXT_AMOUNT);
    int max_amount = pItemObj->QueryInt(TEXT_MAX_AMOUNT);
    int allowed_amount = 0;

    int max_pos = GetMaxUsablePos(pKernel, container);
	// 循环保护
	LoopBeginCheck(ah);
	for (int i = CONTAINER_START_POS; i <= max_pos; ++i)
	{
		LoopDoCheck(ah);
		PERSISTID temp_item = pKernel->GetItem(container, i);
		IGameObj* pTempItemObj = pKernel->GetGameObj(temp_item);
		if (pTempItemObj == NULL)
		{
			allowed_amount += max_amount;
		}
		else if (CanMergeItem(pKernel, item, temp_item))
		{
			allowed_amount += (max_amount - pTempItemObj->QueryInt(TEXT_AMOUNT));
		}

        if (allowed_amount >= request_amount)
        {
            break;
        }
	}

	return allowed_amount;
}

// 尝试把@item放入容器@container中的指定位置@pos中, 返回可以放入的数量
// 此操作不会真正放入物品, 仅仅是计算可放入的数量
int ContainerModule::TryAddItemToPosition(IKernel* pKernel, const PERSISTID& container,
                                          const PERSISTID& srcitem, int pos)
{
	//必须是容器中的有效位置
	if (!IsValidPos(pKernel, container, pos))
	{
		return 0;
	}


	IGameObj* pSrcItemObj = pKernel->GetGameObj(srcitem);
	if (pSrcItemObj == NULL)
	{
		return 0;
	}

    int max_amount = pSrcItemObj->QueryInt(TEXT_MAX_AMOUNT);

	PERSISTID dest_item = pKernel->GetItem(container, pos);
	IGameObj* pDestItemObj = pKernel->GetGameObj(dest_item);
	if (pDestItemObj == NULL)
	{
        return max_amount;
	}
    else
    {
        //目标位置有物品
        if (CanMergeItem(pKernel, srcitem, dest_item))
        {
            int allowed_amount = max_amount - pDestItemObj->QueryInt(TEXT_AMOUNT);
            return allowed_amount;
        }
        else
        {
            return 0;
        }
    }
}

// 解析字符串型物品 已冒号和逗号分隔 例如:"物品id:数量,物品id:数量"
bool ContainerModule::ParseItems(IKernel* pKernel, const char* strItems, VectorBoxTestItem& itemList, int num/* = 1*/, int nBindState/* = ITEM_NOT_BOUND*/)
{
	// 检测字符串
	if (pKernel == NULL || StringUtil::CharIsNull(strItems))
	{
		return false;
	}

	// 清空列表
	itemList.clear();

	// 先以逗号解析
	CVarList list1;
	StringUtil::ParseToVector(strItems, ',', list1);

	// 检测是否有数据
	if (list1.IsEmpty())
	{
		return false;
	}

	// 获取个数
	int list1Count = static_cast<int>(list1.GetCount());

	// 分隔出来的物品字符串
	const char* stritem = "";
	// 分隔出来的物品数据
	CVarList list2;
	// 物品结构
	SBoxTestItem item;
	// 遍历list1
	// 循环保护
	LoopBeginCheck(list1);
	for (int i = 0; i < list1Count; ++i)
	{
		LoopDoCheck(list1);

		// 取物品数据字符串
		stritem = list1.StringVal(i);
		if (StringUtil::CharIsNull(stritem))
		{
			continue;
		}

		// 以冒号分隔字符串
		list2.Clear();
		StringUtil::ParseToVector(stritem, ':', list2);
		if (list2.IsEmpty())
		{
			continue;
		}

		// 检测数量 只有物品id 和 物品数量 两个属性, 所以长度必然为2
		if (list2.GetCount() != 2)
		{
			continue;
		}
		
		// 解析数据
		item.config_id = list2.StringVal(0);
		item.amount = StringUtil::StringAsInt(list2.StringVal(1));
		item.bind_status = nBindState;

		// 加入容器中
		LoopBeginCheck(ab);
		int itemCount = num;
		while(itemCount > 0)
		{
			LoopDoCheck(ab);
			itemList.push_back(item);
			--itemCount;
		} 
	}

	return true;
}

int ContainerModule::MergeItemData(IKernel* pKernel, const PERSISTID& defBox, const VectorBoxTestItem& itemList, BoxItemData& boxItemData)
{
	// 1. 首先合并可重叠的物品
	VectorBoxTestItem tempItems;
	for ( VectorBoxTestItem::const_iterator itr = itemList.begin(); itr != itemList.end(); ++itr)
	{
		bool bCambine = false;

		const SBoxTestItem& src = *itr;
		for (VectorBoxTestItem::iterator itorTemp = tempItems.begin(); itorTemp != tempItems.end(); ++itorTemp)
		{
			SBoxTestItem& dst = *itorTemp;
			if ( CanMergeItem(pKernel, &src, &dst) )
			{
				dst.amount += src.amount;
				bCambine = true;
				break;
			}
		}

		if (!bCambine)
		{
			tempItems.push_back(*itr);
		}
	}

	// 2. 按背包分类
	PERSISTID self;
	const char* script = pKernel->GetScript(defBox);
	if (strcmp(script, ITEM_BOX_NAME) == 0)
	{
		PERSISTID parent = pKernel->Parent(defBox);
		if (pKernel->Exists(parent) && pKernel->Type(parent) == TYPE_PLAYER)
		{
			self = parent;
		}
	}
	for (VectorBoxTestItem::iterator itorTemp = tempItems.begin(); itorTemp != tempItems.end(); ++itorTemp)
	{
		SBoxTestItem& item = *itorTemp;

		int64_t nBoxId = defBox.nData64;
		if (!self.IsNull())
		{
			const PERSISTID box = m_pContainerModule->GetBoxContainer(pKernel, self, item.config_id.c_str());
			nBoxId = box.nData64;
		}
		VectorBoxTestItem& datas = boxItemData[nBoxId];
		datas.push_back(item);
	}

	return (int)tempItems.size();
}

bool ContainerModule::IsNeedSaveItemLog(IKernel* pKernel, const char* szItemId, int nColorLv)
{
	const char * isNoNeedLog = pKernel->GetConfigProperty(szItemId, FIELD_PROP_IS_NO_NEED_LOG);
	if (strcmp(isNoNeedLog, "1") == 0)
	{
		return false;
	}

	// 装备到达一定品质才记日志
// 	int nLogColorLv = EnvirValueModule::EnvirQueryInt(ENV_VALUE_LOG_EQUIP_COLOR_LV);
// #ifndef FSROOMLOGIC_EXPORTS
// 	if (ToolItemModule::IsCanWearItem(pKernel, szItemId) && (nColorLv < nLogColorLv || nColorLv >= COLOR_MAX))
// 	{
// 		return false;
// 	}
// #endif
	return true;
}

//加载物品所属背包规则
bool ContainerModule::LoadBoxRule(IKernel* pKernel)
{
    m_mapBoxRule.clear();
    std::string xml_file_path = pKernel->GetResourcePath();
    xml_file_path += BOX_RULE_CONFIG;

    // 背包的配置
    CXmlFile xml(xml_file_path.c_str());
    if (!xml.LoadFromFile())
    {
        extend_warning(LOG_WARNING, "[ContainerModule::LoadBoxRule] failed");
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();
    
    LoopBeginCheck(ai);
    for (int i = 0; i < sec_count; ++i)
    {
        LoopDoCheck(ai);
        const char *sec = sec_list.StringVal(i);

		ItemTypeLim valLim;
		valLim.nMinVal		= xml.ReadInteger(sec, "MinItemTypeVal", 0);
		valLim.nMaxVal		= xml.ReadInteger(sec, "MaxItemTypeVal", 0);

        m_mapBoxRule.insert(std::make_pair(sec, valLim));
    }
    return true;
}

//物品所属背包的Script
const char* ContainerModule::GetBoxScript(IKernel* pKernel, const char* itemConfig)
{
    const char* boxScript = "";
    int nItemType = StringUtil::StringAsInt(pKernel->GetConfigProperty(itemConfig, TEXT_ITEM_TYPE));

    std::map<std::string, ItemTypeLim>::iterator iter = m_pContainerModule->m_mapBoxRule.begin();
    std::map<std::string, ItemTypeLim>::iterator iter_end = m_pContainerModule->m_mapBoxRule.end();
    LoopBeginCheck(aj)
    for ( ;iter != iter_end; ++iter)
    {
        LoopDoCheck(aj)
        const std::string& name = iter->first;
        const ItemTypeLim& valLim = iter->second;
		if (nItemType >= valLim.nMinVal && nItemType <= valLim.nMaxVal)
		{
			boxScript = name.c_str();
			break;
		}
    }
    return boxScript;
}

//物品所属背包的名字
PERSISTID ContainerModule::GetBoxContainer(IKernel* pKernel, const PERSISTID& self, const char* itemConfig)
{
    const char* script = m_pContainerModule->GetBoxScript(pKernel, itemConfig);
    if (StringUtil::CharIsNull(script))
    {
		::extend_warning(LOG_ERROR, "[ContainerModule::GetBoxContainer] item owner box not found(%s)", itemConfig);
        return PERSISTID();
    }
    std::wstring name = StringUtil::StringAsWideStr(script);
    PERSISTID container = pKernel->GetChild(self, name.c_str());
    if (!pKernel->Exists(container))
    {
		::extend_warning(LOG_ERROR, "[ContainerModule::GetBoxContainer] box not found(item:%s box:%s)", itemConfig, script);
        return PERSISTID();
    }
    return container;
}

// 物品放入背包，背包空间不足放入临时背包
bool ContainerModule::PlaceItemsToBagOrTemp(IKernel* pKernel, const PERSISTID& self,
											const char *config_id, int request_amount, 
											EmFunctionEventId srcFunctionId/* = FUNCTION_EVENT_ID_SYS*/, bool bIsGain/* = true*/,
											int bound/* = ITEM_NOT_BOUND*/, int64_t frozenTime/* = 0*/ )
 {
	if (config_id == NULL || request_amount <= 0)
	{
		return false;
	}

	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 放入正常背包
	PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolBox))
	{
		return false;
	}

	if (TryAddItem(pKernel, toolBox, config_id, request_amount, bound))
	{
		return PlaceItem(pKernel, toolBox, config_id, request_amount, srcFunctionId, bIsGain, bound) > 0;
	}

	// 放入临时背包
	PERSISTID tempBox = pKernel->GetChild(self, TEMPORARY_BAG_BOX_NAME_WSTR);
	if (!pKernel->Exists(tempBox))
	{
		return false;
	}
	if (TryAddItem(pKernel, tempBox, config_id, request_amount, bound))
	{
		return PlaceItem(pKernel, tempBox, config_id, request_amount, srcFunctionId, bIsGain, bound) > 0;
	}

	return false;
}

bool ContainerModule::PlaceItemsToBagOrTemp(IKernel* pKernel, const PERSISTID& self, VectorBoxTestItem *item_list, EmFunctionEventId srcFunctionId /*= FUNCTION_EVENT_ID_SYS*/, bool bIsGain /*= true*/)
{
	if (item_list == NULL)
	{
		return false;
	}

	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 放入正常背包
	PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if ( !pKernel->Exists(toolBox) )
	{
		return false;
	}

	if (TryAddItems(pKernel, toolBox, item_list))
	{	
		return PlaceItems(pKernel, toolBox, item_list, srcFunctionId, bIsGain);
	}

	// 放入临时背包
	PERSISTID tempBox = pKernel->GetChild(self, TEMPORARY_BAG_BOX_NAME_WSTR);
	if ( !pKernel->Exists(tempBox) )
	{
		return false;
	}

	if (TryAddItems(pKernel, tempBox, item_list))
	{
		return PlaceItems(pKernel, tempBox, item_list, srcFunctionId, bIsGain);
	}

	return false;
}
