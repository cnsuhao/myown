//--------------------------------------------------------------------
// 文件名:      ItemTypeDefine.h
// 内  容:      物品常用定义
// 说  明:		
// 创建日期:    2014年10月24日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef ItemTypeDefine_h__
#define ItemTypeDefine_h__

// 品质设置
enum
{
	COLOR_MIN = -1,

	COLOR_ZERO = 0,
	COLOR_ONE,
	COLOR_TWO,
	COLOR_THREE,
	COLOR_FOUR,
	COLOR_FIVE,

	COLOR_MAX,
};

//物品类型
enum ItemType
{
	//物品类型定义
	//==========================================
	//1-99道具类
	ITEMTYPE_NONE = 0,
	//==========================================
	//1-99使用类道具
	//以下为道具的具体分类
	ITEMTYPE_TOOLITEM_MIN                   = 1,  //使用类道具的最小编号
	ITEMTYPE_ADD_PHYPOWER                   = 1,  //加体力药
	ITEMTYPE_ADD_STAYPOWER                  = 2,  //加历练药
	//附魔道具(装备属性的提升)
	ITEMTYPE_APPEND_PRPERTY                 = 3,  //提升装备属性
	ITEMTYPE_ADD_TASK                       = 4,  //触发任务
	ITEMTYPE_CAPITAL                        = 5,  //增加金钱的道具
	ITEMTYPE_COMPOSITE_FORMULA              = 6,  //合成配方
	ITEMTYPE_CARRY_MAILBOX                  = 7,  //随身邮箱功能
	ITEMTYPE_TASKTOOL                       = 8,  //可以使用的任务道具
	ITEMTYPE_CALL                           = 9,  //召唤npc道具
	ITEMTYPE_ADDBUFFER                      = 10, //给 自己，其他玩家，npc 添加buffer
	ITEMTYPE_GOHOME                         = 11, //回程 物品
	ITEMTYPE_INLAY                          = 12, //镶嵌道具
	ITEMTYPE_INLAY1                         = 13, //1类宝石镶嵌
	ITEMTYPE_INLAY2                         = 14, //2宝石镶嵌
	ITEMTYPE_INLAY3                         = 15, //3宝石镶嵌
	ITEMTYPE_INLAY4                         = 16, //4宝石镶嵌
	ITEMTYPE_EVALUATE                       = 17, //鉴定道具
	ITEMTYPE_SPLIT_TOOL                     = 18, //拆解宝石道具
	ITEMTYPE_STRENGTHEN_STONE               = 19, //强化石
	ITEMTYPE_APPEND_PAPER                   = 20, //附魔图纸
	ITEMTYPE_ITEM_PACKAGE                   = 21, //物品包（配掉落id串）
	ITEMTYPE_ITEM_PATCH						= 22, //碎片类道具（n个碎片合成一个物品）
	ITEMTYPE_ITEM_GIFT						= 23, //礼品包（配物品id串）
	ITEMTYPE_ITEM_TREASURE_PATCH            = 24, //夺宝碎片
	ITEMTYPE_ITEM_PATERNOSTER               = 25, //符文
	ITEMTYPE_ITEM_INTRODUCE                 = 26, //说明性道具（没有任何意义）
	ITEMTYPE_ITEM_ADD_PROPERTY				= 27, //加属性道具
    ITEMTYPE_ITEM_CREATE_NPC				= 28, //创建一个NPC
    ITEMTYPE_ADD_PETEXP                     = 30, //增加宠物经验
    ITEMTYPE_ITEM_PETSTONE                  = 31, //武魂石
	ITEMTYPE_MONTH_CARD                     = 32, //月卡道具
	ITEMTYPE_TOOLITEM_MAX                   = 99, //注意:使用类道具最高数字编号
	//==========================================
	//道具分类结束
	//==========================================

	//==========================================
	//100-199装备类
	ITEMTYPE_EQUIP_MIN                      = 100,
	ITEMTYPE_EQUIP_MAX                      = 199,
	ITEMTYPE_WEAPON_MIN                     = 100,
	ITEMTYPE_WEAPON_MAX                     = 129,
	//==========================================
	//以下为装备的具体分类
	//==========================================
	//武器100~129
	ITEMTYPE_WEAPON_SWORD                   = 101, //剑
	ITEMTYPE_WEAPON_MACE                    = 103, //锤
	ITEMTYPE_WEAPON_SPEAR                   = 106, //长柄武器
	ITEMTYPE_WEAPON_DOUBLE_SWORD            = 102, //双手剑
	ITEMTYPE_WEAPON_GUN                     = 115, //枪械
	ITEMTYPE_WEAPON_WHIP                    = 129, //鞭子
	//护具130~199
	ITEMTYPE_EQUIP_BELT                     = 139, //腰带
	ITEMTYPE_EQUIP_HAT                      = 140, //头盔
	ITEMTYPE_EQUIP_CLOTH                    = 141, //上衣
	ITEMTYPE_EQUIP_PANTS                    = 142, //裤子
	ITEMTYPE_EQUIP_SHONES                   = 143, //鞋子
	ITEMTYPE_EQUIP_HAND                     = 146, //手套
	ITEMTYPE_EQUIP_NECK                     = 147, //项链
	ITEMTYPE_EQUIP_MANTLE                   = 148, //披风
	ITEMTYPE_EQUIP_RING                     = 149, //戒指
	ITEMTYPE_EQUIP_SHOULDERS                = 151, //肩甲
	//==========================================
	//装备分类结束
	//==========================================

	//200-299功能类道具

	//300-499商城道具
	//==========================================
	//其他类型1000+
	//==========================================
	//怪物掉落金钱物件类型
	ITEMTYPE_DROPMONEY                      = 1100,

	ITEMTYPE_AWARDMONEY                      = 1101,                             //奖励金钱

	ITEMTYPE_AWARDEXP                     = 1102,                                    //奖励经验
	ITEMTYPE_AWARDHP                         = 1103,                                 //奖励HP

	// 杂物,只能捡取得到，只能卖到商店,没有其它功能
	ITEMTYPE_ZAWU                           = 1200,
	//以下为合成材料的具体分类 2010 ~ 2200
	//==========================================
	//开始
	ITEMTYPE_COMPOSE_MATRIAL_MIN            = 2010, //开始
	ITEMTYPE_SLOT_ITEM                      = 2011, //打孔钻
	//附魔道具(装备属性的提升)
	ITEMTYPE_APPEND_ITEM                    = 2012, //附魔材料
	ITEMTYPE_FUEL                           = 2013, //熔炼缩时道具
	//结束
	ITEMTYPE_COMPOSE_MATRIAL_MAX            = 2200, //结束

	ITEMTYPE_MARKER_MIN = 3000,				// 印记物品索引开始
	ITEMTYPE_MARKER_ITEM = 3000,			// 印记物品
	ITEMTYPE_MARKER_MAX = 3100,				// 印记物品索引开始

	ITEMTYPE_MAX							= 9999,
};

// 物品稀有程度定义
enum SpecialType
{
	SPECIAL_TYPE_NORMAL		= 0,		// 普通
	SPECIAL_TYPE_VALUABLE	= 1,		// 贵重
	SPECIAL_TYPE_SPECIAL	= 2,		// 稀有
};
// 物品来源字符定义
const char* const FROM_SHOP_TEMP			= "shop00";	// 商店模板
const char* const FROM_SHOP_MALL			= "shop005";	// 商城
const char* const FROM_MELTING				= "Melting001";	// 熔炼

#endif // ItemTypeDefine_h__
