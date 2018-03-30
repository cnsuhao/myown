//---------------------------------------------------------
//文件名:       ToolBoxModule.cpp
//内  容:       背包物品相关操作,例如物品的使用
//说  明:       
//          
//创建日期:      2014年11月22日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#include "ToolBoxModule.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"

#include "public/VarList.h"

#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/Define/ToolBoxDefine.h"
#include "FsGame/Define/ToolBoxSysInfoDefine.h"
#include "FsGame/Define/ContainerDefine.h"

#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/LogModule.h"

#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/ItemModule/ToolItem/ToolItemModule.h"
#include "FsGame/SocialSystemModule/SystemMailModule.h"
#include "server/LoopCheck.h"
#include "FsGame/Define/GameDefine.h"

//#include "FsGame/Middle/MiddleModule.h"

#define OLD_MESSAGE

#define TOOL_BOX_SUCCESS  1 // 与背包相关的操作成功标识
#define TOOL_BOX_FAILED   0 // 与背包相关的操作失败标识

#define TOOL_BOX_DEFAULT_CAPACITY 144
#define TOOL_BOX_MAX_CAPACITY     144
#define TOOL_BOX_FREE_CELL        36
#define TOOL_BOX_CELL_PRICE       10

#define ARRANGE_BAG_CD      10000 // 整理背包的cd, tick 计数

#define TOOL_BOX_CONFIG_FILE "ini/SystemFunction/Container/tool_box.xml"
#define TOOL_BOX_REMIND_USE_CONFIG "ini/SystemFunction/Container/RemindUse.xml"
#define TOOL_BOX_EQUIP_GOT_NOTIFY_CONFIG "ini/SystemFunction/Container/equip_got_notify.xml"

#define BAG_FULL_MAIL_ATTACHMENT_COUNT 10   //老背包多余物品放入邮件，每封邮件附件上限10个

ToolBoxModule* ToolBoxModule::m_pToolBoxModule = NULL;
ContainerModule* ToolBoxModule::m_pContainerModule = NULL;
AsynCtrlModule* ToolBoxModule::m_pAsynCtrlModule = NULL;
CapitalModule* ToolBoxModule::m_pCapitalModule = NULL;
ToolItemModule *ToolBoxModule::m_pToolItemModule= NULL;

ToolBoxModule::ToolBoxConfig ToolBoxModule::m_ToolBoxConfig;
ToolBoxModule::ItemBoxConfig ToolBoxModule::m_ItemBoxConfig;
ToolBoxModule::PowerBoxConfig ToolBoxModule::m_PowerBoxConfig;
ToolBoxModule::BadgeBoxConfig ToolBoxModule::m_BadgeBoxConfig;
ToolBoxModule::ChipBoxConfig ToolBoxModule::m_ChipBoxConfig;
std::vector<ToolBoxModule::EquipGotNotifySetting> ToolBoxModule::m_EquipGotNotifySetting;


inline int nx_reload_tool_box_config(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL != ToolBoxModule::m_pToolBoxModule)
    {
        ToolBoxModule::m_pToolBoxModule->LoadResource(pKernel);
    }

    return 0;
}

std::string ToolBoxModule::m_strItemType = "";
std::string ToolBoxModule::m_strItemConfigId = "";

//初始化以及资源加载
bool ToolBoxModule::Init(IKernel* pKernel)
{
    m_pToolBoxModule = this;
    m_pContainerModule = dynamic_cast<ContainerModule*>(pKernel->GetLogicModule("ContainerModule"));
    m_pAsynCtrlModule = dynamic_cast<AsynCtrlModule*>(pKernel->GetLogicModule("AsynCtrlModule"));
    m_pCapitalModule = dynamic_cast<CapitalModule*>(pKernel->GetLogicModule("CapitalModule"));
    m_pToolItemModule= dynamic_cast<ToolItemModule*>(pKernel->GetLogicModule("ToolItemModule"));

    Assert(	m_pContainerModule && m_pAsynCtrlModule && m_pCapitalModule && m_pToolItemModule);

    pKernel->AddEventCallback("player", "OnRecover", ToolBoxModule::OnPlayerRecover);
    pKernel->AddEventCallback("player", "OnReady", ToolBoxModule::OnPlayerReady);
	pKernel->AddEventCallback("player", "OnContinue", ToolBoxModule::OnPlayerReady);


    pKernel->AddEventCallback("ItemBox", "OnCreate", ToolBoxModule::OnItemBoxCreate);
    pKernel->AddEventCallback("ItemBox", "OnAfterAdd", ToolBoxModule::OnAfterAdd);


    // 客户端的老式消息
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CONTAINER, ToolBoxModule::OnCustomOldMsg);

    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_TOOL_BOX, ToolBoxModule::OnCustomMsg);

    DECL_LUA_EXT(nx_reload_tool_box_config);

    LoadResource(pKernel);

    return true;
}

bool ToolBoxModule::Shut(IKernel* pKernel)
{
    return true;
}

int ToolBoxModule::GetToolBoxCapacity()
{
    return m_ToolBoxConfig.capacity;
}
// 获取消耗类道具背包容量
int ToolBoxModule::GetItemBoxCapacity()
{
    return m_ItemBoxConfig.capacity;
}

int ToolBoxModule::GetTemporaryBoxCapacity()
{
	return m_ItemBoxConfig.temporary_box_cap;
}

// 获取实力类道具背包容量
int ToolBoxModule::GetPowerBoxCapacity()
{
    return m_PowerBoxConfig.capacity;
}
// 获取符文背包容量
int ToolBoxModule::GetBadgeBoxCapacity()
{
    return m_BadgeBoxConfig.capacity;
}
// 获取碎片背包容量
int ToolBoxModule::GetChipBoxCapacity()
{
    return m_ChipBoxConfig.capacity;
}

bool ToolBoxModule::LoadResource(IKernel* pKernel)
{
    if (!LoadToolBoxConfig(pKernel, TOOL_BOX_CONFIG_FILE))
    {
        return false;
    }
    

    if (!LoadEquipNofitySetting(pKernel, TOOL_BOX_EQUIP_GOT_NOTIFY_CONFIG))
    {
        return false;
    }
    
    return true;
}

// 背包设置
bool ToolBoxModule::LoadToolBoxConfig(IKernel* pKernel, const char *config_file)
{
    memset(&m_ToolBoxConfig, 0, sizeof(ToolBoxConfig));

    std::string res_path = pKernel->GetResourcePath();
    std::string config_file_path = res_path;
    config_file_path.append(config_file);

    // 背包的配置
    CXmlFile xml(config_file_path.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = config_file_path;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        Assert(0);
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();
    // 循环保护
    LoopBeginCheck(a);
    for (int i = 0; i < sec_count; ++i)
    {
        LoopDoCheck(a);
        const char *sec = sec_list.StringVal(i);

        if (strcmp(sec, ITEM_BOX_NAME) == 0)
        {
            //消耗类道具
            m_ItemBoxConfig.capacity = xml.ReadInteger(sec, "Capacity", TOOL_BOX_DEFAULT_CAPACITY);
            m_ItemBoxConfig.max_capacity = xml.ReadInteger(sec, "MaxCapacity", TOOL_BOX_MAX_CAPACITY);
			m_ItemBoxConfig.temporary_box_cap = xml.ReadInteger(sec, "TemporayBoxCapacity", TEMPORARY_BOX_DEFAULT_CAPACITY);

            m_ItemBoxConfig.locked_size = m_ItemBoxConfig.capacity - 
                xml.ReadInteger(sec, "FreeCellCount", TOOL_BOX_FREE_CELL);

            m_ItemBoxConfig.gold_type_to_unlock = xml.ReadInteger(sec, "GoldTypeToUnlock", 0);
            m_ItemBoxConfig.gold_to_unlock_cell = xml.ReadInteger(sec, "GoldToUnlockCell", TOOL_BOX_CELL_PRICE);
            m_ItemBoxConfig.arrange_cooldown_ms = xml.ReadInteger(sec, "ArrangeCoolDown", ARRANGE_BAG_CD);
            m_ItemBoxConfig.threshold_free_cell = xml.ReadInteger(sec, "ThresholdFreeCell", 10);
            m_ItemBoxConfig.reminder_interval = xml.ReadInteger(sec, "ReminderInterval", 10);
        }
//         else if (strcmp(sec, POWER_BOX_NAME) == 0)
//         {
//             //实力类道具
//             m_PowerBoxConfig.capacity = xml.ReadInteger(sec, "Capacity", TOOL_BOX_DEFAULT_CAPACITY);
//             m_PowerBoxConfig.max_capacity = xml.ReadInteger(sec, "MaxCapacity", TOOL_BOX_MAX_CAPACITY);
// 
//             m_PowerBoxConfig.locked_size = m_PowerBoxConfig.capacity - 
//                 xml.ReadInteger(sec, "FreeCellCount", TOOL_BOX_FREE_CELL);
// 
//             m_PowerBoxConfig.gold_type_to_unlock = xml.ReadInteger(sec, "GoldTypeToUnlock", 0);
//             m_PowerBoxConfig.gold_to_unlock_cell = xml.ReadInteger(sec, "GoldToUnlockCell", TOOL_BOX_CELL_PRICE);
//             m_PowerBoxConfig.arrange_cooldown_ms = xml.ReadInteger(sec, "ArrangeCoolDown", ARRANGE_BAG_CD);
//             m_PowerBoxConfig.threshold_free_cell = xml.ReadInteger(sec, "ThresholdFreeCell", 10);
//             m_PowerBoxConfig.reminder_interval = xml.ReadInteger(sec, "ReminderInterval", 10);
//         }
//         else if (strcmp(sec, BADGE_BOX_NAME) == 0)
//         {
//             //符文
//             m_BadgeBoxConfig.capacity = xml.ReadInteger(sec, "Capacity", TOOL_BOX_DEFAULT_CAPACITY);
//             m_BadgeBoxConfig.max_capacity = xml.ReadInteger(sec, "MaxCapacity", TOOL_BOX_MAX_CAPACITY);
// 
//             m_BadgeBoxConfig.locked_size = m_BadgeBoxConfig.capacity - 
//                 xml.ReadInteger(sec, "FreeCellCount", TOOL_BOX_FREE_CELL);
// 
//             m_BadgeBoxConfig.gold_type_to_unlock = xml.ReadInteger(sec, "GoldTypeToUnlock", 0);
//             m_BadgeBoxConfig.gold_to_unlock_cell = xml.ReadInteger(sec, "GoldToUnlockCell", TOOL_BOX_CELL_PRICE);
//             m_BadgeBoxConfig.arrange_cooldown_ms = xml.ReadInteger(sec, "ArrangeCoolDown", ARRANGE_BAG_CD);
//             m_BadgeBoxConfig.threshold_free_cell = xml.ReadInteger(sec, "ThresholdFreeCell", 10);
//             m_BadgeBoxConfig.reminder_interval = xml.ReadInteger(sec, "ReminderInterval", 10);
//         }
//         else if (strcmp(sec, CHIP_BOX_NAME) == 0)
//         {
//             //碎片
//             m_ChipBoxConfig.capacity = xml.ReadInteger(sec, "Capacity", TOOL_BOX_DEFAULT_CAPACITY);
//             m_ChipBoxConfig.max_capacity = xml.ReadInteger(sec, "MaxCapacity", TOOL_BOX_MAX_CAPACITY);
// 
//             m_ChipBoxConfig.locked_size = m_ChipBoxConfig.capacity - 
//                 xml.ReadInteger(sec, "FreeCellCount", TOOL_BOX_FREE_CELL);
// 
//             m_ChipBoxConfig.gold_type_to_unlock = xml.ReadInteger(sec, "GoldTypeToUnlock", 0);
//             m_ChipBoxConfig.gold_to_unlock_cell = xml.ReadInteger(sec, "GoldToUnlockCell", TOOL_BOX_CELL_PRICE);
//             m_ChipBoxConfig.arrange_cooldown_ms = xml.ReadInteger(sec, "ArrangeCoolDown", ARRANGE_BAG_CD);
//             m_ChipBoxConfig.threshold_free_cell = xml.ReadInteger(sec, "ThresholdFreeCell", 10);
//             m_ChipBoxConfig.reminder_interval = xml.ReadInteger(sec, "ReminderInterval", 10);
//         }
//         else if (strcmp(sec, TOOL_BOX_NAME) == 0)
//         {
//             //ToolBox
//             m_ToolBoxConfig.capacity = xml.ReadInteger(sec, "Capacity", TOOL_BOX_DEFAULT_CAPACITY);
//             m_ToolBoxConfig.max_capacity = xml.ReadInteger(sec, "MaxCapacity", TOOL_BOX_MAX_CAPACITY);
// 
//             m_ToolBoxConfig.locked_size = m_ToolBoxConfig.capacity - 
//                 xml.ReadInteger(sec, "FreeCellCount", TOOL_BOX_FREE_CELL);
// 
//             m_ToolBoxConfig.gold_type_to_unlock = xml.ReadInteger(sec, "GoldTypeToUnlock", 0);
//             m_ToolBoxConfig.gold_to_unlock_cell = xml.ReadInteger(sec, "GoldToUnlockCell", TOOL_BOX_CELL_PRICE);
//             m_ToolBoxConfig.arrange_cooldown_ms = xml.ReadInteger(sec, "ArrangeCoolDown", ARRANGE_BAG_CD);
//             m_ToolBoxConfig.threshold_free_cell = xml.ReadInteger(sec, "ThresholdFreeCell", 10);
//             m_ToolBoxConfig.reminder_interval = xml.ReadInteger(sec, "ReminderInterval", 10);
//         }
    }

    // 提示装备使用的配置
    std::string configRemindUse = res_path;
    configRemindUse.append(TOOL_BOX_REMIND_USE_CONFIG);

    CXmlFile xmlRemind(configRemindUse.c_str());
    if (!xmlRemind.LoadFromFile())
    {
        std::string err_msg = configRemindUse;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        Assert(0);
        return false;
    }

    // 取配置
    m_strItemType = xmlRemind.ReadString("0", "ItemType", "");
    m_strItemConfigId = xmlRemind.ReadString("0", "CofingID", "");
    // 整理
    m_strItemType = std::string(",") + m_strItemType + std::string(",");
    m_strItemConfigId = std::string(",") + m_strItemConfigId + std::string(",");

    return true;
}

// 装备提醒设置
bool ToolBoxModule::LoadEquipNofitySetting(IKernel* pKernel, const char *config_file)
{
    m_EquipGotNotifySetting.clear();
    
    std::string res_path = pKernel->GetResourcePath();
    std::string config_file_path = res_path;
    config_file_path.append(config_file);

    // 背包的配置
    CXmlFile xml(config_file_path.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = config_file_path;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        Assert(0);
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();
    // 循环保护
    LoopBeginCheck(b);
    for (int i = 0; i < sec_count; ++i)
    {
        LoopDoCheck(b);
        const char *sec = sec_list.StringVal(i);
        
        EquipGotNotifySetting setting;
        
        setting.equip_type = xml.ReadString(sec,  "EquipType", "");
        setting.level = xml.ReadInteger(sec, "Level", 0);
        setting.info_id = xml.ReadInteger(sec, "InfoID", 0);
        
        m_EquipGotNotifySetting.push_back(setting);
    }
    
    return true;
}

// 物品放入
int ToolBoxModule::OnAfterAdd(IKernel * pKernel, const PERSISTID & container, 
                      const PERSISTID & sender, const IVarList & args)
{
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    // 信息提醒仅限玩家的背包
    PERSISTID container_parent = pKernel->Parent(container);
    if (pKernel->Type(container_parent) != TYPE_PLAYER)
    {
        return 0;
    }

    int free_cell_count = m_pContainerModule->GetFreePosCount(pKernel, container);

    if (free_cell_count <= m_ItemBoxConfig.threshold_free_cell)
    {
        bool do_reminder = false;

        __int64 cur_time = util_get_time_64();

        // 为了防止频繁的提醒, 检查间隔时间
        if (!pKernel->FindData(container, "LastReminderTime"))
        {
            do_reminder = true;

            pKernel->AddData(container, "LastReminderTime", VTYPE_INT64);
            pKernel->AddData(container, "LastReminderSize", VTYPE_INT);

        }
        else
        {
            __int64 last_reminder_time = pKernel->QueryDataInt64(container, "LastReminderTime");
            int last_reminder_size = pKernel->QueryDataInt(container, "LastReminderSize");

            // 距上次提醒时间过去了多少秒
            int sec_escaped = int((cur_time - last_reminder_time)/1000);
            
            if (sec_escaped > m_ItemBoxConfig.reminder_interval)
            {
                do_reminder = true;
            }
            else
            {
                // 提醒时间没到, 检查背包是否满了
                if (free_cell_count == 0 && last_reminder_size !=0)
                {
                    // 背包满了, 并未提醒过
                    do_reminder = true;
                }
            }
        }

        if (do_reminder)
        {
            pKernel->SetDataInt64(container, "LastReminderTime", cur_time);
            pKernel->SetDataInt(container, "LastReminderSize", free_cell_count);

            CVarList tip_args;
            tip_args << free_cell_count; 
			::CustomSysInfo(pKernel, pKernel->Parent(container), SYSTEM_INFO_ID_7006, tip_args);
        }
    }
    
    // 获得物品提醒
    BroadcastOnGot(pKernel, container_parent, container, args.IntVal(0));

	// 提醒玩家使用
	RemindUseItem(pKernel, container, args.IntVal(0));

    return 0;
}

// 新增物品后提醒使用
int ToolBoxModule::RemindUseItem(IKernel * pKernel, const PERSISTID & container, int pos)
{
	// 获取指定物品的属性
	const PERSISTID& item = pKernel->GetItem(container, pos);
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (pItemObj == NULL)
	{
		// 物品不存在
		return 0;
	}

	// 取物品属性

	int itemType = pItemObj->QueryInt("ItemType");
	const char* configId = pKernel->GetConfig(item);
	const char* uniqueId = pKernel->QueryString(item, "UniqueID");
	int amount = pItemObj->QueryInt("Amount");

	if (StringUtil::CharIsNull(configId) || StringUtil::CharIsNull(uniqueId))
	{
		// 物品异常
		return 0;
	}

	// 检测是否需要提示
	if (m_strItemType.find(std::string(",") + StringUtil::IntAsString(itemType) + std::string(",")) == std::string::npos)
	{
		if (m_strItemConfigId.find(std::string(",") + configId + std::string(",")) == std::string::npos)
		{
			// 不需要提示
			return 0;
		}
	}

	// 已经提醒的数量
	int remindAmount = pItemObj->QueryInt("RemindAmount");
	if (remindAmount >= amount)
	{
		// 所有物品都已经提醒过了
		return 0;
	}
	
	// 设置已经提醒过的个数
	pItemObj->SetInt("RemindAmount", amount);

	// 发消息给客户端，提示使用
	CVarList args;
	args << SERVER_CUSTOMMSG_ITEM_REMIND
		<< TOOL_BOX_SUBMSG_NEW_USE
		<< configId
		<< uniqueId
		<< amount;
	pKernel->Custom(pKernel->Parent(container), args);

	return 0;
}


// 获得物品后的通知
int ToolBoxModule::BroadcastOnGot(IKernel * pKernel, const PERSISTID& self, const PERSISTID & container, int pos)
{
    // 获取指定物品的属性
    const PERSISTID& item = pKernel->GetItem(container, pos);
    IGameObj* item_obj = pKernel->GetGameObj(item);
    if (item_obj == NULL)
    {
        // 物品不存在
        return 0;
    }

    // 发送提醒
    const char *script = item_obj->GetScript();
    if (strcmp(script, "Equipment") == 0)
    {
		int equip_notify_flag = item_obj->QueryInt("EquipNotifyFlag");

		if (equip_notify_flag != 0)
		{
			// 已经提醒过
			return 0;
		}

		// 设置为已经提醒过
		item_obj->SetInt("EquipNotifyFlag", 1);

        // 装备
        std::string equip_type = item_obj->QueryString("EquipType");
        int level = item_obj->QueryInt("LimitLevel");
        
        const EquipGotNotifySetting *setting = GetEquipGotNotifyConfig(equip_type, level);
        
        if (setting != NULL)
        {
            CVarList param;
            param << pKernel->QueryWideStr(self, "Name");
            param << item_obj->GetConfig();
            
            ::CustomSysInfoBroadcast(pKernel, setting->info_id, param); 
        }
    }
    else
    {
        // 非装备，暂不提醒
        return 0;
    }

    return 0;
}

// 获取装备的提醒设置
const ToolBoxModule::EquipGotNotifySetting* ToolBoxModule::GetEquipGotNotifyConfig(const std::string& equip_type, int level)
{
    std::vector<EquipGotNotifySetting>::iterator it = m_EquipGotNotifySetting.begin();
    std::vector<EquipGotNotifySetting>::iterator it_end = m_EquipGotNotifySetting.end();

    LoopBeginCheck(c);
    for (; it != it_end; ++it)
    {
        LoopBeginCheck(c);
     
        EquipGotNotifySetting &temp = *it;
        
        if (temp.equip_type == equip_type && temp.level == level)
        {
            return &temp;
        }
    }
    
    return NULL;
}

// 玩家数据加载完毕后
int ToolBoxModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
                                   const PERSISTID& sender, const IVarList& args)
{
    PERSISTID exist_item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
    if (!pKernel->Exists(exist_item_box))
    {
        //创建消耗类道具背包
        if (m_ItemBoxConfig.capacity > 0)
        {
            PERSISTID item_box = pKernel->CreateContainer(self, ITEM_BOX_NAME, m_ItemBoxConfig.capacity);
            if (item_box.IsNull())
            {
                ::extend_warning(LOG_ERROR, "ToolBoxModule: create ItemBox failed.");
            }
        }
    }
    
    return 0;
}

// 玩家进入场景后
int ToolBoxModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
                                 const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    int online_flag = pSelfObj->QueryInt("Online");
    
    // 给背包设置视图, online_flag == 1 表示是在线玩家
//     if (!pKernel->FindViewport(self, VIEWPORT_TOOL_BOX) && online_flag == 1)
//     {
//         PERSISTID exist_base_box = pKernel->GetChild(self, TOOL_BOX_NAME_WSTR);
//         if (pKernel->Exists(exist_base_box))
//         {
//             pKernel->AddViewport(self, VIEWPORT_TOOL_BOX, exist_base_box);
//         }
//     }
    //添加各种背包的视图
    if (!pKernel->FindViewport(self, VIEWPORT_ITEM_BOX) && online_flag == 1)
    {
        PERSISTID exist_item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
        if (pKernel->Exists(exist_item_box))
        {
            pKernel->AddViewport(self, VIEWPORT_ITEM_BOX, exist_item_box);
        }
    }

	// 添加临时背包视窗
// 	if (online_flag == 1 && !pKernel->FindViewport(self, VIEWPORT_TEMP_BAG))
// 	{
// 		PERSISTID box = pKernel->GetChild(self, TEMPORARY_BAG_BOX_NAME_WSTR);
// 	 	if (pKernel->Exists(box))
// 	 	{
// 	 		pKernel->AddViewport(self, VIEWPORT_TEMP_BAG, box);
// 	 	}																		 
// 	}
    return 0;
}

// ItemBox 实体创建后
int ToolBoxModule::OnItemBoxCreate(IKernel* pKernel, const PERSISTID& item_box,
                           const PERSISTID& sender, const IVarList& args)
{
    if (item_box.IsNull())
    {
        return 0;
    }

    //设置基本属性
    pKernel->SetWideStr(item_box, "Name", ITEM_BOX_NAME_WSTR);
    pKernel->SetInt(item_box, "LockedSize", m_ItemBoxConfig.locked_size);
    return 0;
}

// 处理客户端发送的老式消息
int ToolBoxModule::OnCustomOldMsg(IKernel* pKernel, const PERSISTID& self,
                          const PERSISTID& sender, const IVarList& args)
{
    if (!pKernel || !pKernel->Exists(self))
    {
        return 0;
    }

    /*
    收到的消息

        [msg_id][sub_msg_id][...]

    回应的消息

        [msg_id][sub_msg_id][...]
    */
    int sub_msg_id = args.IntVal(1);

    switch (sub_msg_id)
    {
    case CONTAINER_SUBMSG_SALE:
        {
            OnSellItem(pKernel, self, sender, args);
        }
        break;
    case CONTAINER_SUBMSG_UNLOCK:
        {
            OnUnlockCell(pKernel, self, sender, args);
        }
        break;
    case CONTAINER_SUBMSG_ARRANGE:
        {
            OnArrange(pKernel, self, sender, args);
        }
        break;
    case CONTAINER_SUBMSG_EXPAND:
        {
            //OnExpand(pKernel, self, sender, args);
        }
        break;
    default:
        break;
    }

    return 0;
}

// 处理客户端发送消息
int ToolBoxModule::OnCustomMsg(IKernel* pKernel, const PERSISTID& self,
                               const PERSISTID& sender, const IVarList& args)
{
    if (!pKernel || !pKernel->Exists(self))
    {
        return 0;
    }
    
    PERSISTID scene = pKernel->GetScene();
    const char *scene_script = pKernel->GetScript(scene);
    if (strcmp(scene_script, "ArenaScene") == 0)
    {
        // 竞技场中不准使用物品
        return 0;
    }

    int subMsg = args.IntVal(1);
    switch (subMsg)
    {
    case TOOL_BOX_SUBMSG_SELL:
        {
            OnSellItem(pKernel, self, sender, args);
        }
        break;
    case TOOL_BOX_SUBMSG_UNLOCK:
        {
            OnUnlockCell(pKernel, self, sender, args);
        }
        break;
    case TOOL_BOX_SUBMSG_ARRANGE:
        {
            OnArrange(pKernel, self, sender, args);
        }
        break;
    case TOOL_BOX_SUBMSG_EXPAND:
        {
            //OnExpand(pKernel, self, sender, args);
        }
        break;
	case TOOL_BOX_SUBMSG_USE_ITEM:
	case TOOL_BOX_SUBMSG_USE_ITEM_MERGE:
		{
			OnCustomUseItem(pKernel, self, sender, args);
		}
        break;
	case TOOL_BOX_SUBMSG_USE_ITEM_ON_ITEM:
		break;
	case TOOL_BOX_SUBMSG_PICKUP_TEMPORAR_BOX:
		OnCustomPickupTemporayBox( pKernel, self, sender, args );
    default:
        break;
    }

    return 0;
}

// 出售物品
int ToolBoxModule::OnSellItem(IKernel* pKernel, const PERSISTID& self,
                      const PERSISTID& sender, const IVarList& args)
{
    /*
    收到的消息

        [msg_id][sub_msg_id][item_unique_id][amount]

    回应的消息

        [msg_id][sub_msg_id][item_unique_id][amount]
    */
    const char *request_item_unique_id = args.StringVal(2);
    int request_amount = args.IntVal(3);
    int viewID = args.IntVal(4);

    if (request_amount <=0)
    {
        return 0;
    }
    PERSISTID container = pKernel->GetViewportContainer(self, viewID);

    //PERSISTID container = pKernel->GetChild(self, TOOL_BOX_NAME_WSTR);
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    PERSISTID item = ContainerModule::FindItemByUid(pKernel, container, request_item_unique_id);
    if (!pKernel->Exists(item))
    {
        return 0;
    }

    // 任务物品不可以出售
    if (strcmp(pKernel->GetScript(item), "TaskItem") == 0)
    {
        return 0;
    }

    int exist_amount = pKernel->QueryInt(item, TEXT_AMOUNT);
    if (exist_amount < request_amount)
    {
        return 0;
    }

	// 物品ID
	const char* itemConfig = pKernel->GetConfig(item);

    // 物品单价
    int item_price = pKernel->QueryInt(item, "SellPrice");
	int nColor = pKernel->QueryInt(item, FIELD_PROP_COLOR_LEVEL);
    if (exist_amount == request_amount)
    {
        pKernel->DestroySelf(item);
    }
    else
    {
        pKernel->SetInt(item, TEXT_AMOUNT, exist_amount-request_amount);
    }

    int gold_amount = request_amount * item_price;

	m_pCapitalModule->IncCapital(pKernel, self, 
		CAPITAL_COPPER, gold_amount, FUNCTION_EVENT_ID_SELL_ITEM);

#if defined(OLD_MESSAGE)

    CVarList response_msg;
    response_msg << SERVER_CUSTOMMSG_CONTAINER;
    response_msg << CONTAINER_SUBMSG_SALE;
    response_msg << request_item_unique_id;
    response_msg << request_amount;
    response_msg << viewID;

    pKernel->Custom(self, response_msg);

#endif

    CVarList tip_args;
    tip_args << gold_amount; // 增加的资金数
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7007, tip_args);

	// NOTE: 不是调用Remove接口，需要自己添回日志 [2/15/2017 lihailuo] 
	if (ContainerModule::IsNeedSaveItemLog(pKernel, itemConfig, nColor))
	{
		ItemLog log;
		log.eventType = LOG_ACT_EVENT_LOSE;
		log.itemID = itemConfig;
		log.itemNum = request_amount;
		log.before = exist_amount;
		log.after = exist_amount - request_amount;
		log.state = LOG_ACT_STATE_SUCCESS;
		log.eventID = FUNCTION_EVENT_ID_SELL_ITEM;
		log.colorLevel = nColor;
		LogModule::m_pLogModule->SaveItemLog(pKernel, self, log);
	}

    return 0;
}

// 解锁格子
int ToolBoxModule::OnUnlockCell(IKernel* pKernel, const PERSISTID& self,
                                const PERSISTID& sender, const IVarList& args)
{
    /*
    收到的消息

    [msg_id][sub_msg_id][cell_index]

    回应的消息

    [msg_id][sub_msg_id][cell_index][unlocked_count]
    */

    int cell_index = args.IntVal(2);
    int viewID = args.IntVal(3);

    if (cell_index <= 0)
    {
        return 0;
    }

    PERSISTID container = pKernel->GetViewportContainer(self, viewID);

    //PERSISTID container = pKernel->GetChild(self, TOOL_BOX_NAME_WSTR);

    if(!pKernel->Exists(container))
    {
        return 0;
    }

    int capacity = pKernel->GetCapacity(container);
    int locked_size = pKernel->QueryInt(container, TEXT_LOCKED_SIZE);

    if (locked_size == 0)
    {
        // 没有格子处于锁定状态
        return 0;
    }

    int locked_cell_start = capacity - locked_size + 1;

    if (cell_index < locked_cell_start || cell_index > capacity)
    {
        // 要解锁的位置不正确
        return 0;
    }

    int count_to_unlock = cell_index - locked_cell_start + 1;
    int unlock_cost = count_to_unlock * m_ItemBoxConfig.gold_to_unlock_cell;

	//modify by   (2015-04-22)--若有铜币需判断类型放入铜币消耗类日志
// 	if (!DeductGold(pKernel, self, m_ItemBoxConfig.gold_type_to_unlock, unlock_cost, FUNCTION_EVENT_ID_UNLOCK_BAG))
//     {
//         // 金币扣除失败
//         CVarList unlock_tip_args;
//         unlock_tip_args << unlock_cost;
//         ::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
//             SYS_INFO_TOOL_BOX_UNLOCK_FAILED_GOLD, unlock_tip_args);
// 
//         return 0;
//     }

    locked_size -= count_to_unlock;
    pKernel->SetInt(container, TEXT_LOCKED_SIZE, locked_size);

    if (count_to_unlock > 0)
    {

        CVarList response_msg;
        response_msg << SERVER_CUSTOMMSG_TOOL_BOX;
        response_msg << CONTAINER_SUBMSG_UNLOCK;
        response_msg << cell_index;
        response_msg << count_to_unlock;
        response_msg << unlock_cost;
        response_msg << viewID;

        pKernel->Custom(self, response_msg);

        CVarList unlock_tip_args;
        unlock_tip_args << count_to_unlock;
        unlock_tip_args << unlock_cost;
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7008, unlock_tip_args);

		{
			//成就
// 			int own_count = capacity - locked_size;
// 			if (own_count>0)
// 			{
// 				MiddleModule::Instance()->OnAchievementCondition(pKernel, self, EAchievementDstCond_BagCount, own_count);
// 			}
		}
    }

    return 0;
}

// 整理背包
int ToolBoxModule::OnArrange(IKernel* pKernel, const PERSISTID& self,
                             const PERSISTID& sender, const IVarList& args)
{
    /*
    收到的消息

    [msg_id][sub_msg_id]

    回应的消息

    */
    if (args.GetCount() < 3)
    {
        return 0;
    }
    int viewID = args.IntVal(2);
    PERSISTID container = pKernel->GetViewportContainer(self, viewID);
    if (!pKernel->Exists(container))
    {
        return 0;
    }
    int coolDownms = m_ItemBoxConfig.arrange_cooldown_ms;
    const char* script = pKernel->GetScript(container);
    if (strcmp(script, ITEM_BOX_NAME) == 0)
    {
        coolDownms = m_ItemBoxConfig.arrange_cooldown_ms;
    }

	if (m_pContainerModule->ArrangeItem(pKernel, container, coolDownms))
	{
		CVarList response_msg;
		response_msg << SERVER_CUSTOMMSG_TOOL_BOX;
		response_msg << CONTAINER_SUBMSG_ARRANGE;
		response_msg << TOOL_BOX_SUCCESS;
		response_msg << viewID;

		pKernel->Custom(self, response_msg);
	}
//     if (!m_pContainerModule->ArrangeItem(pKernel, container, coolDownms))
//     {
//         ::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_TOOL_BOX_ARRANGE_FAILED, CVarList());
//     
// 
//         CVarList response_msg;
//         response_msg << SERVER_CUSTOMMSG_TOOL_BOX;
//         response_msg << CONTAINER_SUBMSG_ARRANGE;
//         response_msg << TOOL_BOX_FAILED;
//         response_msg << viewID;
// 
//         pKernel->Custom(self, response_msg);
//     }
//     else
//     {
//         CVarList response_msg;
//         response_msg << SERVER_CUSTOMMSG_TOOL_BOX;
//         response_msg << CONTAINER_SUBMSG_ARRANGE;
//         response_msg << TOOL_BOX_SUCCESS;
//         response_msg << viewID;
// 
//         pKernel->Custom(self, response_msg);
//     }

    return 0;
}

// 扩展背包
int ToolBoxModule::OnExpand(IKernel* pKernel, const PERSISTID& self,
                            const PERSISTID& sender, const IVarList& args)
{
    /*
    收到的消息

    [msg_id][sub_msg_id][count_to_expand]

    回应的消息

    [msg_id][sub_msg_id][result][count_expanded]

    result: 1=success, 0=failed
    */

    int count_to_expand = args.IntVal(2);

    if (count_to_expand <= 0)
    {
        return 0;
    }

    int count_expanded = 0;

    PERSISTID container = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
    if (pKernel->Exists(container))
    { 
        int new_capacity = pKernel->GetCapacity(container) + count_to_expand;
        if (new_capacity > m_ItemBoxConfig.max_capacity)
        {
            // 超出了扩展限制
            count_expanded = 0;
        }
        else
        {
            // 执行扩容
            if (pKernel->ExpandContainer(container, new_capacity))
            {
                count_expanded = count_to_expand;
            }
        }
    }

    if (count_expanded > 0)
    {

    #if defined(OLD_MESSAGE)

            CVarList response_msg;
            response_msg << SERVER_CUSTOMMSG_CONTAINER;
            response_msg << CONTAINER_SUBMSG_EXPAND;
            response_msg << count_expanded;

            pKernel->Custom(self, response_msg);

    #endif

        CVarList tip_args;
        tip_args << count_expanded;
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7009, tip_args);
    }

    return 0;
}

// 直接使用道具
int ToolBoxModule::OnCustomUseItem(IKernel* pKernel, const PERSISTID& self,
                                   const PERSISTID& sender, const IVarList& args)
{
    /*
        收到的消息:
            [msg_id][sub_msg_id][item_unique_id][amount]

        回复的消息
            无
    */

	int useType = args.IntVal(1);
    const char *item_unique_id = args.StringVal(2);
    int request_amount = args.IntVal(3);
    int viewID = args.IntVal(4);

    if (strlen(item_unique_id) == 0)
    {
        return 0;
    }

    if (request_amount <= 0)
    {
        return 0;
    }

    // 玩家是否可使用物品
    if (!CanPlayerUseItem(pKernel, self))
    {
        return 0;
    }

    PERSISTID container = pKernel->GetViewportContainer(self, viewID);

    //PERSISTID container = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
    if (!pKernel->Exists(container))
    {
        return 0;
    }

    PERSISTID item = m_pContainerModule->FindItemByUid(pKernel, container, item_unique_id);

    if (item.IsNull())
    {
        return 0;
    }

    // 检查数量是否合法
    int allowed_amount = pKernel->QueryInt(item, "Amount");
    if (request_amount > allowed_amount)
    {
        return 0;
    }
    
    // 找到物品的CD时间, 如果有的话
    int cd_time = 0;
    if (pKernel->Find(item, "CoolDownID"))
    {
        cd_time = pKernel->QueryInt(item, "CoolDownTime");
    }
   
    // 执行使用操作
    bool ret = m_pToolItemModule->UseItem(pKernel, self, sender, item, request_amount);

	// 某些效果客户端不需要提示(碎片)
	if (useType == TOOL_BOX_SUBMSG_USE_ITEM_MERGE)
	{
		return 0;
	}

    // 通知客户端操作结果
    CVarList result_msg;
    result_msg << SERVER_CUSTOMMSG_TOOL_BOX;
    result_msg << useType;
    result_msg << item_unique_id;
    result_msg << request_amount;
    if (ret)
    {
        result_msg << 1; //成功
        result_msg << cd_time;
    }
    else
    {
        result_msg << 0; // 失败
        result_msg << cd_time;
    }

    result_msg << viewID;

    pKernel->Custom(self, result_msg);
    return 0;
}

int ToolBoxModule::OnCustomPickupTemporayBox(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 取出要提出的格子物品
	std::vector<int> vecGrids;
	int nCount = (int)args.GetCount();
	for (int i = 2; i < nCount; ++i)
	{
		vecGrids.push_back(args.IntVal(i));
	}

	EmToolBoxResult eResult = EM_TOOL_BOX_RESULT_SUCCEED;
	int nNum = 0;
	do 
	{
		// 是否有先选中
		if (vecGrids.size() == 0)
		{
			Assert(false);
			break;
		}

		// 格子空间是否够
		PERSISTID toolBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
		if (!pKernel->Exists(toolBox))
		{
			eResult = EM_TOOL_BOX_RESULT_ERROR;
			Assert(false);
			break;
		}
		int nFreePos = ContainerModule::m_pContainerModule->GetFreePosCount(pKernel, toolBox);
		if (nFreePos < (int)vecGrids.size())
		{
			// 背包空间不足
			eResult = EM_TOOL_BOX_RESULT_NO_SPACE;
			//CVarList tip_args;
			//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, SYS_INFO_TOOL_BOX_INSUFFICIENT, tip_args);
			break;
		}

		// 临时背包
		PERSISTID tempBox = pKernel->GetChild(self, TEMPORARY_BAG_BOX_NAME_WSTR);
		if (!pKernel->Exists(tempBox))
		{
			eResult = EM_TOOL_BOX_RESULT_ERROR;
			Assert(false);
			break;
		}

		// 循环读取临时背包格子移动到背包
		for (std::vector<int>::const_iterator itr = vecGrids.begin(); itr != vecGrids.end(); ++itr)
		{
			PERSISTID srcitem = pKernel->GetItem(tempBox, *itr);
			if (srcitem.IsNull())
				continue;;

			//确定能够移动，开始移动物品
			int destpos = m_pContainerModule->GetFreePos(pKernel, toolBox);
			if (destpos < 0)
			{
				break;
			}

			// 交换背包物品
			if ( pKernel->Exchange(tempBox, *itr, toolBox, destpos) )
			{
				++nNum;
			}
		}

		// 有物品变化 整理临时背包
		if (nNum > 0)
		{
			ContainerModule::ArrangeItem(pKernel, tempBox);
		}
	} while (false);

	CVarList retMsg;
	retMsg << SERVER_CUSTOMMSG_TOOL_BOX << TOOL_BOX_SUBMSG_PICKUP_TEMPORAR_BOX << eResult << nNum;
	pKernel->Custom(self, retMsg);
	return 0;
}

// 玩家是否可以使用物品
bool ToolBoxModule::CanPlayerUseItem(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

    if (pSelfObj->QueryInt("Dead") > 0)
    {
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7001, CVarList());
        return false;
    }

    return true;
}

// 扣除金币
bool ToolBoxModule::DeductGold(IKernel *pKernel, const PERSISTID &self, 
                                    int gold_type, int amount, int log_type)
{
    if (m_pCapitalModule->CanDecCapital(pKernel, self, gold_type, amount))
    {
        int ret = m_pCapitalModule->DecCapital(pKernel, self, 
            gold_type, amount, log_type);

        if (ret == DC_SUCCESS)
        {
            return true;
        }
    }

    return false;
}

//平均分配开启的格子到4个背包
bool ToolBoxModule::AssignPos(IKernel* pKernel, const PERSISTID& self)
{
    //锁住的格子数
//     int lockSize = pBox->QueryInt(TEXT_LOCKED_SIZE);
//     //初始锁定的格子数
//     int initLockSize = m_ToolBoxConfig.locked_size;
// 
//     int unlockSize = initLockSize -lockSize;
//     if (unlockSize <= 0)
//     {
//         //没有解锁过格子
//         return true;
//     }
//     int everyAddSize = unlockSize/4;
//     int surplusSize = unlockSize%4;
// 
//     PERSISTID itemBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
// 
//     if (!pKernel->Exists(itemBox))
//     {
//         return false;
//     }
//     UnLock(pKernel, itemBox, everyAddSize, surplusSize);

    return true;
}

//解锁背包格子
void ToolBoxModule::UnLock(IKernel* pKernel, const PERSISTID& container, int confirmAdd, int& appendAdd)
{
    IGameObj* pBoxObj = pKernel->GetGameObj(container);
    if (NULL == pBoxObj)
    {
        return;
    }
    int lockSize = pBoxObj->QueryInt(TEXT_LOCKED_SIZE);
    lockSize = lockSize - confirmAdd;
    if (appendAdd > 0)
    {
        //每个背包解锁一个位置
        lockSize = lockSize - 1;
        appendAdd = appendAdd - 1;
    }
    if (lockSize <= 0)
    {
        lockSize = 0;
    }

    pBoxObj->SetInt(TEXT_LOCKED_SIZE, lockSize);
}