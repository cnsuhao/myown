//---------------------------------------------------------
//文件名:       ContainerDefine.h
//内  容:       容器的一些定义
//说  明:       
//          
//创建日期:      2014年10月31日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef _FSGAME_CONTAINER_DEFINE_H_
#define _FSGAME_CONTAINER_DEFINE_H_
#include <vector>
#include <map>

#define CONTAINER_START_POS 1     // 容器位置的起始位置

#define TEXT_COLOR_LEVEL       "ColorLevel"
#define TEXT_UNIQUE_ID         "UniqueID"
#define TEXT_AMOUNT            "Amount"
#define TEXT_MAX_AMOUNT        "MaxAmount"
#define TEXT_ITEM_TYPE         "ItemType"
#define TEXT_LOCKED_SIZE       "LockedSize" // 容器中锁定的容量
#define TEXT_CAN_OP            "CanOP"      // 是否可以操作
#define TEXT_CAN_ADD           "CanAdd"
#define TEXT_CAN_REMOVE        "CanRemove"
#define TEXT_CAN_ARRANGE       "CanArrange" //是否可以整理
// #define TOOL_BOX_NAME          "ToolBox"
// #define TOOL_BOX_NAME_WSTR     L"ToolBox"

#define ITEM_BOX_NAME_WSTR      L"ItemBox"      //消耗类道具容器
// #define POWER_BOX_NAME_WSTR     L"PowerBox"     //实力类道具容器
// #define BADGE_BOX_NAME_WSTR     L"BadgeBox"     //符文容器
// #define CHIP_BOX_NAME_WSTR      L"ChipBox"      //碎片容器
#define ITEM_BOX_NAME           "ItemBox"      //消耗类道具容器名字
// #define POWER_BOX_NAME          "PowerBox"     //实力类道具容器名字
// #define BADGE_BOX_NAME          "BadgeBox"     //符文容器名字
// #define CHIP_BOX_NAME           "ChipBox"      //碎片容器名字

#define EQUIP_BOX_NAME  "EquipBox"
#define EQUIP_BOX_NAME_WSTR  L"EquipBox"

// 印记容器
#define MARKER_BOX_NAME	"MarkerBox"
#define MARKER_BOX_NAME_WSTR	L"MarkerBox"

// 临时背包
#define   TEMPORARY_BAG_BOX_NAME_WSTR	L"TemporaryBagBox"


#define TEXT_ITEM_BIND_STATUS   "BindStatus"  // 物品绑定属性
// 绑定状态
enum 
{
	ITEM_NOT_BOUND	= 0, // 非绑定
	ITEM_BOUND		= 1, // 绑定
};

// 物品匹配状态
enum 
{
	ITEM_MATCH_BOUND = 0x01 << 0,		// 匹配绑定物品
	ITEM_MATCH_NOT_BOUND = 0x01 << 1,	// 匹配非绑定物品 

	ITEM_MATCH_FROZEN = 0x01 << 2,		// 匹配所有冻结的物品
	ITEM_MATCH_NOT_FROZEN = 0x01 << 3,	// 区配所有非冻结物品

	ITEM_MATCH_BOUND_AND_FROZEN = 0x01 << 4, // 匹配绑定并冻结的
	ITEM_MATCH_BOUND_NOT_FROZEN = 0x01 << 5, // 匹配绑定并不冻结的

	ITEM_MATCH_NOT_BOUND_AND_FROZEN = 0x01 << 6, // 匹配非绑定并冻结的
	ITEM_MATCH_NOT_BOUND_NOT_FROZEN = 0x01 << 7, // 匹配非绑定并不冻结的

	ITEM_MATCH_ALL = ITEM_MATCH_BOUND | ITEM_MATCH_NOT_BOUND | ITEM_MATCH_FROZEN | ITEM_MATCH_NOT_FROZEN
					| ITEM_MATCH_BOUND_AND_FROZEN | ITEM_MATCH_BOUND_NOT_FROZEN 
					| ITEM_MATCH_NOT_BOUND_AND_FROZEN | ITEM_MATCH_NOT_BOUND_NOT_FROZEN,		// 所有物品
};

#define ITEM_TYPE_GIFT          361 // 礼包的类型

// 容器的子消息定义
enum
{
    CONTAINER_SUBMSG_SALE = 1,		// 出售
    CONTAINER_SUBMSG_UNLOCK = 2,	// 解锁
    CONTAINER_SUBMSG_ARRANGE = 3,	// 整理
    CONTAINER_SUBMSG_EXPAND = 4,	// 扩充容器
	CONTAINER_SUBMSG_MOVEITEM = 5,	// 移动容器物品(穿装备等)
};

//客户端操作容器的类型
enum
{
    CONTAINER_OPTYPE_ALL,   //所有权限
    CONTAINER_OPTYPE_ADD,   //添加物品
    CONTAINER_OPTYPE_REMOVE,//移走物品
};

// 简单的物品
struct SBoxTestItem
{
    std::string config_id;
    int amount;
    int bind_status;		// 绑定状态
	PERSISTID item;			// 物品被创建后的PERSISTID
	int64_t frozenTime;		// 冻结过期时间

	SBoxTestItem() : config_id(""), amount(0), bind_status(ITEM_BOUND), frozenTime(0){}
};
typedef std::vector<SBoxTestItem> VectorBoxTestItem;
typedef std::map<int64_t, VectorBoxTestItem> BoxItemData;	// <容器对象，物品列表>

//容器整理结果
enum
{
    CONTAINER_ARRANGE_SUCCESS = 0,   //整理成功
    CONTAINER_ARRANGE_FAIL = 1,      //整理失败（也可能是无需整理）
};


#endif // _FSGAME_CONTAINER_DEFINE_H_
