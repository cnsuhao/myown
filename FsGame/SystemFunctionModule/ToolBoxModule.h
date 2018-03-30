//---------------------------------------------------------
//文件名:       ToolBoxModule.h
//内  容:       背包物品相关操作,例如物品的使用
//说  明:       
//          
//创建日期:      2014年11月22日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef FSGAME_PLAYER_BASE_MODULE_TOOL_BOX_MODULE_H
#define FSGAME_PLAYER_BASE_MODULE_TOOL_BOX_MODULE_H

#include "Fsgame/Define/header.h"

#include <map>
#include <string>
#include <vector>
#include "Define/ToolBoxDefine.h"

class ContainerModule;
class ToolItemModule;
class CapitalModule;
class AsynCtrlModule;

class ToolBoxModule: public ILogicModule
{
public:
    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

    bool LoadResource(IKernel* pKernel);

public:

    // 提醒设置
    enum NotifyFlag 
    {
        NEED_NOTIFY = 1, // 0000 0001  需要通知
        NOTIFY_DONE = 2, // 0000 0010  已经通知过
    };

    // 获取容量
    static int GetToolBoxCapacity();
    // 获取消耗类道具背包容量
    static int GetItemBoxCapacity();
	// 临时背包容量
	static int GetTemporaryBoxCapacity();
    // 获取实力类道具背包容量
    static int GetPowerBoxCapacity();
    // 获取符文背包容量
    static int GetBadgeBoxCapacity();
    // 获取碎片背包容量
    static int GetChipBoxCapacity();

    // 玩家是否可以使用物品
    static bool CanPlayerUseItem(IKernel* pKernel, const PERSISTID& self);

private:
 
    // 装备获得提醒设置
    struct EquipGotNotifySetting
    {
		EquipGotNotifySetting(): equip_type(""), level(0), info_id(0){}
        std::string equip_type;     // 装备类型
        int level;                  // 装备等级
        int info_id;				// 提醒信息
    };

    // 背包设置
    static bool LoadToolBoxConfig(IKernel* pKernel, const char *config_file);
    

    // 装备提醒设置
    static bool LoadEquipNofitySetting(IKernel* pKernel, const char *config_file);

    // 物品放入
    static int OnAfterAdd(IKernel * pKernel, const PERSISTID & container, 
        const PERSISTID & sender, const IVarList & args);

    // 玩家数据加载完毕后
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 玩家进入场景后
    static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // ItemBox 实体创建后
    static int OnItemBoxCreate(IKernel* pKernel, const PERSISTID& item_box,
        const PERSISTID& sender, const IVarList& args);

    // 处理客户端发送消息
    static int OnCustomMsg(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 处理客户端发送的老式消息
    static int OnCustomOldMsg(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 出售物品
    static int OnSellItem(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 解锁格子
    static int OnUnlockCell(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 整理背包
    static int OnArrange(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 扩展背包
    static int OnExpand(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 直接使用道具
    static int OnCustomUseItem(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

	// 从临时背包提取物品
	static int OnCustomPickupTemporayBox(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

    // 扣除金币
    static bool DeductGold(IKernel *pKernel, const PERSISTID &self, 
        int gold_type, int amount, int log_type = 0);

	// 新增物品后提醒使用
	static int RemindUseItem(IKernel * pKernel, const PERSISTID & container, int pos);
	
	// 获得物品后的通知
	static int BroadcastOnGot(IKernel * pKernel, const PERSISTID& self, const PERSISTID & container, int pos);
	
	// 获取装备的提醒设置
	static const EquipGotNotifySetting* GetEquipGotNotifyConfig(const std::string& equip_type, int level);

    //解锁背包格子
    static void UnLock(IKernel* pKernel, const PERSISTID& container, int confirmAdd, int& appendAdd);

public:
    //平均分配开启的格子到4个背包
    static bool AssignPos(IKernel* pKernel, const PERSISTID& self);
private:

    struct ToolBoxConfig
    {
		ToolBoxConfig(): capacity(0), max_capacity(0), locked_size(0), gold_type_to_unlock(0), gold_to_unlock_cell(0),
			arrange_cooldown_ms(0), threshold_free_cell(0), reminder_interval(0),
			temporary_box_cap(TEMPORARY_BOX_DEFAULT_CAPACITY){}
        int capacity;               // 容量(格子数)
        int max_capacity;           // 的最大容量
        int locked_size;            // 锁定的容量
        int gold_type_to_unlock;    // 解锁格子需要的金币类型
        int gold_to_unlock_cell;    // 解锁一个格子需要的金币
        int arrange_cooldown_ms;    // 整理背包的冷却时间(毫秒)
        int threshold_free_cell;    // 空闲格子的提醒阀值, 当空闲格子数小于等于这个阀值的时候提醒玩家
        int reminder_interval;      // 间隔多久提醒一次, 单位秒

		int temporary_box_cap;		// 临时背包容量(格子数)
    };

    struct ItemBoxConfig:ToolBoxConfig 
    {
    };

    struct PowerBoxConfig:ToolBoxConfig 
    {
    };

    struct BadgeBoxConfig:ToolBoxConfig 
    {
    };

    struct ChipBoxConfig:ToolBoxConfig 
    {
    };

	static std::string m_strItemType;		// 物品类型
	static std::string m_strItemConfigId;	// 物品id

    static ToolBoxConfig m_ToolBoxConfig;   //总背包容器
    static ItemBoxConfig m_ItemBoxConfig;   //消耗类道具容器
    static PowerBoxConfig m_PowerBoxConfig; //实力类道具容器
    static BadgeBoxConfig m_BadgeBoxConfig; //符文容器
    static ChipBoxConfig m_ChipBoxConfig;   //碎片容器
    
    static std::vector<EquipGotNotifySetting> m_EquipGotNotifySetting;

    static ContainerModule* m_pContainerModule;
    static ToolItemModule* m_pToolItemModule;
    static CapitalModule* m_pCapitalModule;
	static AsynCtrlModule* m_pAsynCtrlModule;

public:
    static ToolBoxModule* m_pToolBoxModule;
};

#endif 