// -------------------------------------------
// 文件名称： ShopManagerDefine.h
// 文件说明： 商店管理数据定义
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------

#ifndef _SHOP_MANAGER_DEFINE_H_
#define _SHOP_MANAGER_DEFINE_H_

#include <vector>
#include <string>
#include <map>
#include "FsGame/Define/LogDefine.h"

#define SHOP_GRID_INDEX_MIN 0	// 商店格子索引最小值
#define SHOP_GRID_INDEX_MAX 11	// 商店格子索引最大值
#define SHOP_FIRST_REFRESH_FLAG   -1  // 用于获取第一次刷新商店的规则的标识

// 商店管理类
struct ShopManagerData
{
	ShopManagerData(): 
		shop_type(0), 
		start_level(0), 
		shop_rule_file(""), 
		max_free_count(0), 
		shop_capital_type(0),
		first_free(0), 
		shop_vip_logic_name("")
	{
		refresh_cost_value.clear();
		grid_limit_property.clear();
		grid_limit_condition.clear();
		discount_limit_list.clear();
	}

	int shop_type;												// 商店类型
	int start_level;												// 开启等级
	std::string shop_rule_file;								// 商店规则文件
	int max_free_count;										// 免费刷新最大次数
	int shop_capital_type;									// 商店使用的货币类型
	std::vector<int> refresh_cost_value;				// 刷新花费
	int first_free;													// 第一次手动刷新是否免费并且不计数
	std::vector<std::string> grid_limit_property;	// 商店格子开启条件（属性值）
	std::vector<int> grid_limit_condition;			// 商店格子开启条件对应值
	std::string shop_vip_logic_name;					// 取vip等级对应的最大购买次数逻辑名
	std::vector<int> discount_limit_list;			// 商店格子是否可打折条件限制列表
};

// 掉落包规则
struct DropPack
{
	DropPack(): 
		drop_id(""), 
		lower_bound(0), 
		upper_bound(0){}

	std::string drop_id;		// 掉落包ID
	int lower_bound;			// 权重下限
	int upper_bound;			// 权重上限

};

typedef std::vector<DropPack> DropPackList;

// 商店物品规则
struct ShopRule
{
	ShopRule(): 
		player_level(0), 
		grid_index(0), 
		weight_sum(0)
	{
		drop_pack_list.clear();
	}

	int player_level;								// 玩家等级
	int grid_index;								// 商店格子索引

	DropPackList drop_pack_list;			// 掉落包
	int weight_sum;							// 总权重值
};

// 折扣规则
struct DiscountRule
{
	DiscountRule(): 
		discount(0.0f), 
		lower_bound(0), 
		upper_bound(0){}

	float discount;		// 折扣
	int lower_bound;	// 权重下限
	int upper_bound;	// 权重上限

};

typedef std::vector<DiscountRule> DiscountList; 

// 物品价目表
struct ItemPlants
{
	ItemPlants(): 
		main_price(0.0f), 
		diamond_price(0.0f), 
		discount_weight_sum(0)
	{
		discount_list.clear();
	}

	float main_price;			// 主流价格
	float diamond_price;		  // 元宝价格
	DiscountList discount_list;	// 折扣列表

	int discount_weight_sum;	// 折扣总权重值
	
};

// 商店的规则表, key = player_level
typedef std::map<int, std::vector<ShopRule>> ShopRuleMap;

// 通用商店包 key->shop_type商店类型，value->商店规则包
struct GeneralShop
{
	GeneralShop(): shop_type(0)
	{
		shopRuleMap.clear();
	}

	int shop_type;
	ShopRuleMap shopRuleMap;
};

// 物品价目表, key = 物品id_货币类型_物品品质，value->价目表
typedef std::map<std::string, ItemPlants> ItemPlantsMap;

// 别名
typedef std::vector<ShopManagerData> ShopManagerDataVector;
typedef std::vector<GeneralShop> GeneralShopVector;

// 商店刷新日志
struct ShopRefreshInfo
{
	int lose_capital_log;
	int game_action_log;
	int vip_buy_type;

	ShopRefreshInfo() : 
		lose_capital_log(FUNCTION_EVENT_ID_SYS),
		//game_action_log(LOG_GAME_ACTION_MIN), 
		vip_buy_type(-1){};
};

// 商店购买日志
struct ShopBuyLog
{
	int lack_capital;	// 资金不足信息
	int pay_failed;	// 扣款失败信息
	int logSrcFunction;		// 日志来源
	int game_action;	// 玩法日志

	ShopBuyLog() : 
		lack_capital(0), 
		pay_failed(0), 
		logSrcFunction(FUNCTION_EVENT_ID_SYS)/*,
		game_action(LOG_GAME_ACTION_MIN)*/{};
};

// 商店类型
enum ShopTypes
{
	SHOP_TYPE_NONE = 0,

	SHOP_TYPE_GOLD = 1,		// 元宝商店

	SHOP_TYPE_TOTAL,	
};

// 商店管理表
enum ShopManagerRecCols
{
	SHOP_REC_TYPE = 0,									// 商店类型
	SHOP_REC_CUR_REFRESH_COUNT,				// 当前刷新次数
	SHOP_REC_CUR_PAY_REFRESH_COUNT,		// 当前付费刷新次数
	SHOP_REC_MAX_FREE_REFRESH_COUNT,	// 免费刷新最大次数
	SHOP_REC_TRADE_CAPITALTYPE,					// 商店使用的货币类型
	SHOP_REC_REFRESH_COSTVALUE,				// 下次刷新价格
	SHOP_REC_REFRESH_ISFIRST,						// 是否第一次手动刷新

};

// 商店物品表字段（通用）
enum ShopRecCols
{
	COMMON_SHOP_REC_CONFIG = 0,		// 物品config
	COMMON_SHOP_REC_COLORLVL = 1,	// 物品品质
	COMMON_SHOP_REC_NUM = 2,			// 物品数量
	COMMON_SHOP_REC_PRICE = 3,			// 价格
	COMMON_SHOP_REC_DIAMOND = 4,	// 需要钻石
	COMMON_SHOP_REC_DISCOUNT = 5,	// 折扣
};

//客户端->服务端 子消息
enum CSShopSubMsg
{
	/*!
	* @brief	商店刷新
	* @param	int 商店类型（ShopTypes）
	*/
	CS_SUB_SHOP_REFRESH	= 1,

	/*!
	* @brief	商店消费
	* @param	int 商店类型（ShopTypes）
	* @param	int 购买索引
	* @param	int 购买数量
	*/
	CS_SUB_SHOP_BUY,

	/*!
	* @brief	查询熔炼商店
	* @param	int 商店类型（ShopTypes）
	*/
	CS_SUB_SHOP_RETRIEVE,
};

// 服务端->客户端 子消息
enum SCShopSubMsg
{
	/*!
	* @brief	购买成功
	* @param	int 商店类型（ShopTypes）
	* @param	int 购买索引
	* @param	string 购买物品id
	* @param	int 物品剩余数量
	* @param	int 购买数量
	*/
	SC_SUB_SHOP_BUY_SUC	= 1,

	/*!
	* @brief	查询熔炼商店成功
	* @param	int 商店类型（ShopTypes）
	* @param	int 商店交易货币类型
	* @param	int 剩余免费刷新次数
	* @param	int 下次刷新花费
	* @param	int 商品数量n
	* @param	以下循环n次
	* @param	int 格子索引
	* @param	string 物品ID
	* @param	int 物品品质
	* @param	int 物品数目
	* @param	int 价格
	* @param	int 元宝价格
	* @param	float 折扣
	* @param	int 格子是否受限(0表示不受限，1表示受限制)
	*/
	SC_SUB_SHOP_RETRIEVE,

	/*!
	* @brief	商店的刷新统计
	* @param	int 商店类型（ShopTypes）
	* @param	int 剩余免费刷新次数
	* @param	int 下次刷新价格
	*/
	SC_SUB_SHOP_REFRESH_STAT,
};

#endif  // _SHOP_MANAGER_DEFINE_H_