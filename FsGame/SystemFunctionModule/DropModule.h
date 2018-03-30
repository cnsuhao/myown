//--------------------------------------------------------------------
// 文件名:      DropModule.h
// 内  容:      掉落系统模块
// 说  明:      用于处理NPC掉落随机物品
// 创建日期:    2014年11月3日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#ifndef FSGAME_GENERAL_DROP_MODULE_H
#define FSGAME_GENERAL_DROP_MODULE_H

#include "Fsgame/Define/header.h"
#include <vector>
#include <string>
#include <map>

//绑定在玩家身上的物品掉落表
#define ITEM_DROP_RECORD "item_drop_record"
#define ITEM_DROP_DEFAULT_CD_TIME 60

enum
{
	ITEM_DROP_RECORD_ITEM_ID = 0,
	ITEM_DROP_RECORD_ITEM_COUNT,
	ITEM_DROP_RECORD_TIEM_CD_TIME,
};

//秘境掉落保存表
#define SECRET_DROP_RECORD "secret_drop_rec"

enum
{
    ITEM_DROP_CONFIG_ID = 0,
    ITEM_DROP_AMOUNT    = 1,
};

class ContainerModule;
class CapitalModule;

class DropModule : public ILogicModule
{

public:

	// 初始化
	virtual bool Init(IKernel * pKernel);

	// 释放
	virtual bool Shut(IKernel * pKernel);

    bool LoadResource(IKernel * pKernel);

	//玩家离开场景
	static int OnPlayerLeaveScene(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 分配掉落奖励
	void AllocateDropAward(IKernel* pKernel, const PERSISTID& self, const PERSISTID& deadnpc, const IVarList& killsers);

    // 根据 @drop_id 得出掉落的物品的config_id, 放入 @drop_items
    // 多个 drop_id 用逗号分隔
    // @drop_items 由多组 [物品configID][物品数量] 组成 
    static int GetItemsFromDropID(IKernel *pKernel, const char* drop_id, IVarList& output_items,
        const PERSISTID &player);

	//将掉落的物品存入玩家的物品掉落表"item_drop_record"中（表在player.xml定义）
	//表的格式3个字段：@itemid + @itemnum拼接的表示掉落物品的字符串，@itemnum字段，@itemcdtime字段
	static bool SaveItemsToPLayerDropRecord(IKernel *pKernel, const PERSISTID& player, std::string& item_id, int item_num);

	//心跳回调函数，每5秒钟检测一次玩家身上的掉落物品表，将超时未检的物品删除
	static int HB_CheckDropRecord(IKernel* pKernel, const PERSISTID& self, int slice);

	//真实掉落，客户端拾取掉落物品，自定义消息回调函数
	static int OnCustomPickUpDropItem(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//玩家真实掉落物品接口
	// args 的格式
	//      [drop_item_id][color_level or drop_item_num][lock_state]
	static int PlayerDropItems(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 掉落包是否存在
	static bool IsDropPkgExist(const char *pkg);
	// 发送掉落表中物品到邮件
	static  void SendDropRecordItemByMail(IKernel*pKernel,const PERSISTID& self);
    
private:

    // 掉落的物品
    struct DropItem 
    {
        DropItem():item_type(0), drop_condition(""), item_config_id(""), amount(0), 
            weight(0), weight_min(0), weight_max(0){};
        int item_type;              // 掉落的物品的类型(例如:物品，子掉落包)
        std::string drop_condition; // 掉落条件(@Level>10,@Job=1)
        std::string item_config_id; // 物品config_id
        int amount;                 // 掉落数量
        int weight;                 // 在掉落包中的权重计算因子
        int weight_min;
        int weight_max;
    };

    // 掉落ID的条目
    struct DropEntry
    {
        DropEntry():drop_package(""), drop_all(0), drop_count(0){};
        std::string drop_package;   // 掉落包
        int drop_all;               // 是否掉落包中全部物品
        int drop_count;             // 掉落的包数量
    };

    // 掉落包中的物品, 每个掉落包可以包含多个掉落物品
    // key = package_name
    typedef std::map<std::string, std::vector<DropItem> > DropItemMap;

    // 掉落ID, 每个掉落ID可以包含多个掉落项目(掉落包)
    // key = drop_id, value = drop_entry
    typedef std::map<std::string, std::vector<DropEntry> > DropEntryMap;

    // 掉落的物品的类型
    typedef std::map<std::string, int > DropItemTypeMap;

private:

    // 从列表中掉落物品
    static int DropItemFromList(IKernel *pKernel, const std::vector<DropItem> * drop_items, 
        const DropEntry *drop_entry, const PERSISTID& player, IVarList &outpu_items);

    // 从子包中掉落, 掉落的物品放入@result_list
    static int DropFromSubPackage(IKernel *pKernel, const std::string& sub_pck_name, const PERSISTID& player,
        std::vector<DropItem>& result_list);

    // 过滤不可以参与掉落的物品, 把可以参与掉落的物品放入@result_list
    static int FilterItems(IKernel *pKernel, const std::vector<DropItem> * drop_items,
        const PERSISTID& player, std::vector<DropItem>& result_list);

    // 从@drop_tems 中根据权重， 把符合条件的物品放入@result_list
    static int SelectItemByWeight(std::vector<DropItem>& drop_items,
        std::vector<DropItem>& result_list);

    // 收否符合掉落条件
    static bool MatchDropCondition(IKernel *pKernel, const DropItem& drop_item, const PERSISTID& player);

    // 生成一个介于 [min, max] 闭区间的随机数作为掉落因子
    // 如果max是0, 返回0
    static int GenerateDropFactor(int min, int max);

    // 计算掉落包中物品的权重
    static int CalculateItemWeight(std::vector<DropItem>& drop_items);

    // 计算布尔表达式的结果
    static bool BoolExp(IKernel *pKernel, const PERSISTID& player, const char* src_exp);

    // 求解变量的值，结果存储在 @var_value中, 类型存放在@var_type 中
    // 解析失败返回false
    static bool SolveVariable(IKernel *pKernel, const PERSISTID& player,
        const std::string& var_name, std::string& var_value, int *var_type);

    // 求解布尔表达式的值
    static bool SolveBoolExp(int var_type, const std::string& left_val, const std::string& op,
        const std::string& right_val);

    //////////////////////////////////////////////////////////////////////////
    // 以下是配置相关的接口
    //////////////////////////////////////////////////////////////////////////

    // 取得物品种类
    static int GetDropItemType(std::string &type_name);

    // 根据掉落ID获取掉落配置
    static const std::vector<DropEntry> * GetDropEntry(const char *drop_id);

    // 根据掉落包获得掉落物品
    static const std::vector<DropItem> * GetPackageItems(const std::string &package_name);

    // 获取子包的掉落物品
    static const std::vector<DropItem> * GetSubpackageItems(const std::string &package_name);

    // 掉落类型
    bool LoadDropItemTypeConfig(IKernel *pKernel);

    // 掉落包
    bool LoadDropPackageConfig(IKernel *pKernel);

    // 子掉落包
    bool LoadSubpacketConfig(IKernel *pKernel);

    // 掉落ID
    bool LoadDropIDConfig(IKernel *pKernel);

	static void ReloadConfig(IKernel* pKernel);
private:
    static DropItemMap m_PackageItems; // 主掉落包
    static DropEntryMap m_DropEntry;   // 掉落ID
    static DropItemTypeMap m_DropItemType; // 掉落的物品类型
    static DropItemMap m_SubpackageItems;  // 子掉落包
public:

	static DropModule*          m_pDropModule;
	static ContainerModule*		m_pContainerBaseModule;
	static CapitalModule*       m_pCapitalModule;

};

#endif // End of include guard