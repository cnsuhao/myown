// -------------------------------------------
// 文件名称： ShopUtils.cpp
// 文件说明： 商店工具类
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------

#include "ShopUtils.h"
#include "ShopBase.h"
#include "ShopManagerModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/ServerCustomDefine.h"

#include <algorithm>
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "CommonModule/FunctionEventModule.h"
#include "ItemModule/ToolItem/ToolItemModule.h"
#include "../DropModule.h"
#include "Define/GameDefine.h"
#include "SystemFunctionModule/CapitalModule.h"


const char* const SHOP_MANAGER_FILE = "ini/SystemFunction/Shop/PersonalShop/ShopManager.xml";	// 商店管理配置
const char* const ITEM_PLANTS_FILE = "ini/SystemFunction/Shop/PersonalShop/ItemPlants.xml";	// 物品价目表

bool ShopUtils::Init(IKernel* pKernel)
{
	if (!ShopUtilClass::Instance()->LoadResource(pKernel))
	{
		return false;
	}

	return true;
}

bool ShopUtils::Shut(IKernel* pKernel)
{
	return ShopUtilClass::Instance()->ReleaseResource(pKernel);
}

// 加载资源
bool ShopUtils::LoadResource(IKernel* pKernel)
{
	/*<Property ID="1" StartLevel="32" ShopRuleFile="ini/rule/equip_build/equip_smelt_shop.xml"
	 *<ItemConfigFile="ini/rule/equip_build/equip_smelt_config.xml" FreeMaxCount="3" RefreshMaxCount="5" 
	 *<CapitalType="2" RefreshCostValue="10,20,40,80,160" FirstFree="0"/>*/

	ReleaseResource(pKernel);

	// 加载物品价目表
	LoadItemPlants(pKernel);

	// 商店管理类配置
	std::string shop_manager_path = pKernel->GetResourcePath();
	shop_manager_path += SHOP_MANAGER_FILE;
	CXmlFile xml(shop_manager_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = shop_manager_path;
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

		// 不合法的商店类型
		int shop_type = atoi(secName);
		if (shop_type <= SHOP_TYPE_NONE 
			|| shop_type >= SHOP_TYPE_TOTAL)
		{
			continue;
		}

		// 排除重复的商店类型
		if (NULL != GetShopManagerData(shop_type))
		{
			continue;
		}
		ShopManagerData shop_manager;
		shop_manager.shop_type =shop_type;
		shop_manager.start_level = xml.ReadInteger(secName, "StartLevel", 0);
		shop_manager.shop_rule_file = xml.ReadString(secName, "ShopRuleFile", "");
		shop_manager.max_free_count = xml.ReadInteger(secName, "FreeMaxCount", 0);
		shop_manager.shop_capital_type = xml.ReadInteger(secName, "CapitalType", 0);
		xml.ReadList<int>(secName, "RefreshCostValue", ",", &shop_manager.refresh_cost_value);
		shop_manager.first_free = xml.ReadInteger(secName, "FirstFree", 0);
		xml.ReadList<std::string>(secName, "GridLimitProperty", ",", &shop_manager.grid_limit_property);
		xml.ReadList<int>(secName, "GridLimitCondition", ",", &shop_manager.grid_limit_condition);
		shop_manager.shop_vip_logic_name = xml.ReadString(secName, "ShopVipLogicName", "");
		xml.ReadList<int>(secName, "CanDiscount", ",", &shop_manager.discount_limit_list);

		m_vecShopManagerData.push_back(shop_manager);
	}

	// 读取商店规则配置、物品配置
	LoopBeginCheck(b);
	for (int i = 0; i < (int)m_vecShopManagerData.size(); ++i)
	{
		LoopDoCheck(b);
		ShopManagerData shop_manager_data = (ShopManagerData)m_vecShopManagerData[i];
		const char *shop_rule_file = shop_manager_data.shop_rule_file.c_str();

		if (StringUtil::CharIsNull(shop_rule_file) )
		{
			continue;
		}

		// 读取商店规则
		LoadShopRule(pKernel, shop_rule_file , (ShopTypes)shop_manager_data.shop_type);
	}

	return true;
}

// 资源释放
bool ShopUtils::ReleaseResource(IKernel* pKernel)
{
	return ReleaseAll(pKernel);
}

// 释放
bool ShopUtils::ReleaseAll(IKernel* pKernel)
{
	// 释放商店管理配置
	m_vecShopManagerData.clear();

	// 释放商店规则配置
	m_vecGeneralShop.clear();

	// 释放物品价目表
	m_ItemPlantsMap.clear();

	return true;
}

// 加载物品价目表配置
bool ShopUtils::LoadItemPlants(IKernel* pKernel)
{
	/*<Property ID="1" ItemConfigID="item000001" CapitalType="1" 
	 *<ItemColor="1" ItemPrice="100" DiamondPrice="0"/>*/

	// 物品价目表
	std::string item_plants_file = pKernel->GetResourcePath();
	item_plants_file += ITEM_PLANTS_FILE;
	CXmlFile xml(item_plants_file.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = item_plants_file;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(c);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(c);
		const char *secName = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(secName))
		{
			continue;
		}

		// 物品价目表配置
		const char *config_id = xml.ReadString(secName, "ItemConfigID", "");
		if (StringUtil::CharIsNull(config_id))
		{
			continue;
		}

		const int capital_type = xml.ReadInteger(secName, "CapitalType", 0);
		const int item_color = xml.ReadInteger(secName, "ItemColor", -1);

		ItemPlants item_plants;

		// 主流价格
		item_plants.main_price = xml.ReadFloat(secName, "ItemPrice", 0.0f);

		// 元宝加个
		item_plants.diamond_price = xml.ReadFloat(secName, "DiamondPrice", 0.0f);

		// 折扣列表
		const char *discount_str = xml.ReadString(secName, "DiscountList", "");
		item_plants.discount_weight_sum = 0;

		// 分配折扣的权重
		AssignDiscountWeight(discount_str, item_plants.discount_weight_sum, item_plants.discount_list);

		// 拼接key = 物品id_货币类型_物品品质
		char key[128] = {0};
		SPRINTF_S(key,"%s_%d_%d", config_id, capital_type, item_color);

		m_ItemPlantsMap.insert(std::make_pair(key, item_plants));
	}

	return true;
}

// 分配折扣的权重值
void ShopUtils::AssignDiscountWeight(const char *str, int &weight_sum, DiscountList &discount_list)
{
	if (StringUtil::CharIsNull(str))
	{
		return;
	}

	// 取折扣和每个折扣的权重
	CVarList discounts;
	util_split_string(discounts, str, ";");

	// 为每个折扣分配权重
	LoopBeginCheck(d);
	for (int dis_num = 0; dis_num < (int)discounts.GetCount(); ++dis_num)
	{
		LoopDoCheck(d);
		DiscountRule discount_rule;
		CVarList one_discount;
		util_split_string(one_discount, discounts.StringVal(dis_num), ",");
		discount_rule.discount = StringUtil::StringAsFloat(one_discount.StringVal(0));	// 折扣
		int weigh_num = atoi(one_discount.StringVal(1));	// 权重
		weigh_num =  weigh_num > 0 ? weigh_num : 1;

		// 折扣权重下限、上限、总权重
		discount_rule.lower_bound = weight_sum;
		weight_sum += weigh_num;
		discount_rule.upper_bound = weight_sum;

		discount_list.push_back(discount_rule);
	}

	return;
}

/*!
* @brief	 加载商店规则配置 
* @param rule_file	商店规则文件路径
* @param shop_type	商店类型
* @return bool
*/
bool ShopUtils::LoadShopRule(IKernel* pKernel, const char* rule_file, const ShopTypes shop_type)
{
	// 配置文件的格式：
	//<Property ID="1" Level="10" Index="equip_level_10" ColorLevel="3" 
	//< Amount="1" Odds="700" FusionPrice="4800" DiamondPrice="0"/>

	std::string rule_path = pKernel->GetResourcePath();
	rule_path += rule_file;

	CXmlFile xml(rule_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = rule_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	CVarList sec_list;
	xml.GetSectionList(sec_list);
	GeneralShop general_shop;
	general_shop.shop_type = (int)shop_type;	// 商店类型

	// 商店规则解析
	int sec_count = (int)sec_list.GetCount();
	LoopBeginCheck(e);
	for (int i = 0; i < sec_count; ++i)
	{
		LoopDoCheck(e);
		const char *sec = sec_list.StringVal(i);

		ShopRule shop_rule;
		shop_rule.player_level = xml.ReadInteger(sec, "Level", 0);
		shop_rule.grid_index = xml.ReadInteger(sec, "GridIndex", 0);
		shop_rule.weight_sum = 0;	// 总权重初始化为0，通过计算进行赋值
		const char *drop_info = xml.ReadString(sec, "DropID", "");
		if (StringUtil::CharIsNull(drop_info))
		{
			continue;
		}

		// 取掉落包和每个掉落包的权重
		CVarList drop_list;
		util_split_string(drop_list, drop_info, ";");

		// 为每个掉落包分配权重
		LoopBeginCheck(f);
		for (unsigned int drop_num = 0; drop_num < drop_list.GetCount(); ++drop_num)
		{
			LoopDoCheck(f);
			DropPack drop_pack;
			CVarList drop;
			util_split_string(drop, drop_list.StringVal(drop_num), ",");
			drop_pack.drop_id = drop.StringVal(0);	// 掉落包ID
			int weigh_num = atoi(drop.StringVal(1));	// 掉落包权重
			weigh_num =  weigh_num > 0 ? weigh_num : 1;

			// 掉落包权重下限、上限、总权重
			drop_pack.lower_bound = shop_rule.weight_sum;
			shop_rule.weight_sum += weigh_num;
			drop_pack.upper_bound = shop_rule.weight_sum;

			shop_rule.drop_pack_list.push_back(drop_pack);
		}

		if (shop_rule.grid_index < SHOP_GRID_INDEX_MIN
			|| shop_rule.grid_index > SHOP_GRID_INDEX_MAX)
		{
			continue;
		}

		ShopRuleMap::iterator it = general_shop.shopRuleMap.find(shop_rule.player_level);
		if (it == general_shop.shopRuleMap.end())
		{
			std::vector<ShopRule> rule_list;
			rule_list.push_back(shop_rule);
			general_shop.shopRuleMap.insert(ShopRuleMap::value_type(shop_rule.player_level, rule_list));
		}
		else
		{
			std::vector<ShopRule> &rule_list = it->second;
			rule_list.push_back(shop_rule);
		}
	}

	m_vecGeneralShop.push_back(general_shop);

	return true;
}

/*!
* @brief 根据权重取物品单价折扣
* @param discount_list 物品单价折扣表
* @param weight_sum 折扣表总权重值
* @return float 返回物品单价折扣
*/
const float ShopUtils::GetDiscountByBound(const DiscountList &discount_list, const int weight_sum)
{
	// 根据权重取价格
	int seed = util_random_int(weight_sum); // 随机数：0~weight_sum
	LoopBeginCheck(g);
	for (int dis_num = 0; dis_num < (int)discount_list.size(); ++dis_num)
	{
		LoopDoCheck(g);	
		const DiscountRule &discount_rule = discount_list[dis_num];

		// 计算权重
		if (seed >= discount_rule.lower_bound
			&& seed < discount_rule.upper_bound)
		{
			return discount_rule.discount;
		}
	}

	return 0.0f;
}

/*!
* @brief 选择一个商店规则
* @param grid_index 商店格子索引
* @param rule_list 商店出产规则
* @return ShopRule* 指定唯一商店规则
*/
const ShopRule* ShopUtils::ChooseShopRule(const int grid_index, const std::vector<ShopRule> *rule_list)
{
	if (NULL == rule_list)
	{
		return NULL;
	}

	if (rule_list->empty())
	{
		return NULL;
	}


	std::vector<ShopRule>::const_iterator it = rule_list->begin();
	std::vector<ShopRule>::const_iterator it_end = rule_list->end();

	LoopBeginCheck(h);
	for (; it != it_end; ++it)
	{
		LoopDoCheck(h);
		const ShopRule *rule = &(*it);

		// 找到对应格子的掉落包
		if (grid_index == rule->grid_index)
		{
			return rule;
		}
	}

	return NULL;
}

/*!
* @brief	 选择一个掉落包（随机取得）
* @param items	一个商店规则
* @return const char* 随机得到一个掉落包ID
*/
const char* ShopUtils::ChooseDropPack(const ShopRule *shop_rule)
{
	if (NULL == shop_rule)
	{
		return "";
	}

	// 掉落包
	const DropPackList &drop_pack_list = shop_rule->drop_pack_list;

	if (drop_pack_list.empty())
	{
		return "";
	}

	// 随机数
	int seed = util_random_int(shop_rule->weight_sum); // 0~shop_rule->weight_sum
	LoopBeginCheck(j);
	for (int i = 0; i < (int)drop_pack_list.size(); ++i)
	{
		LoopDoCheck(j);
		const DropPack &drop_pack = drop_pack_list[i];
		if (seed >= drop_pack.lower_bound 
			&& seed < drop_pack.upper_bound)
		{
			if (!(drop_pack.drop_id.empty()))
			{
				return drop_pack.drop_id.c_str();
			}
		}
	}

	return "";
}

/*!
* @brief	 商店格子是否可打折 
* @param shop_type	商店类型
* @param row_index	要购买的商品索引
* @return int	 0表示可打折，1表示不可
*/
int ShopUtils::ShopGridCanDiscount(IKernel *pKernel, const PERSISTID &self, 
								   const int shop_type, int row_index)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 取出商店的基本配置
	const ShopManagerData *shop_manager_data = GetShopManagerData(shop_type);
	if (NULL == shop_manager_data)
	{
		return 0;
	}

	// 打折条件索引与商店格子索引一一对应
	int discount_index = (int)shop_manager_data->discount_limit_list.size() - 1;
	// 取得合法的索引，
	if (discount_index < 0)
	{
		return 0;
	}

	// 找到对应的索引，不足以最后一个值为准
	if (discount_index >= row_index)
	{
		discount_index = row_index;
	}
	else
	{
		return 0;
	}

	return shop_manager_data->discount_limit_list[discount_index];
}

/*!
* @brief	  取得对应类型的商店管理配置 
* @param shop_type	商店类型
* @return 得到的商店管理配置
*/
const ShopManagerData* ShopUtils::GetShopManagerData(const int shop_type)
{
	// 数据检索
	struct ShopManagerDataFinder
	{
		ShopManagerDataFinder(int shop_type):_shop_type(shop_type){};

		bool operator ()(const ShopManagerData &data)
		{
			return _shop_type == data.shop_type;
		}

		int _shop_type;
	};

	// 查找shop_type对应的配置
	ShopManagerDataVector::const_iterator iter = std::find_if(ShopUtilClass::Instance()->m_vecShopManagerData.begin(),
		ShopUtilClass::Instance()->m_vecShopManagerData.end(), ShopManagerDataFinder(shop_type));
	if (iter == ShopUtilClass::Instance()->m_vecShopManagerData.end())
	{
		return NULL;
	}

	return &(*iter);
}

/*!
* @brief	  取得对应类型的商店规则 
* @param shop_type	商店类型
* @return 得到的商店规则
*/
const GeneralShop* ShopUtils::GetShopRule(const int shop_type) const
{
	// 检索方法
	struct GeneralShopFinder
	{
		GeneralShopFinder(int shop_type):_shop_type(shop_type){};

		bool operator ()(const GeneralShop &data)
		{
			return _shop_type == data.shop_type;
		}

		int _shop_type;
	};

	// 查找shop_type的商店规则
	GeneralShopVector::const_iterator iter = std::find_if(ShopUtilClass::Instance()->m_vecGeneralShop.begin(),
		ShopUtilClass::Instance()->m_vecGeneralShop.end(), GeneralShopFinder(shop_type));
	if (iter == ShopUtilClass::Instance()->m_vecGeneralShop.end())
	{
		return NULL;
	}

	return &(*iter);
}

/*!
* @brief 取得对应物品价目表
* @param shop_type 商店类型（据此取得货币类型）
* @param config_id 物品ID
* @param item_color 物品品质
* @param main_price 最终取得的物品主流单价
* @param diamond_price 最终取得的物品元宝单价
* @param discount 最终取得的物品折扣
* @return bool 返回值 true取得 false未取得
*/
bool ShopUtils::GetItemPlants(IKernel* pKernel, const int shop_type, 
							  const char *config_id, const int item_color, float &main_price, float &diamond_price, float &discount)
{
	// 根据shop_type取出商店管理
	const ShopManagerData *shop_manager = GetShopManagerData(shop_type);
	if (NULL == shop_manager)
	{
		return false;
	}

	// 货币类型
	int capital_type = shop_manager->shop_capital_type;

	// 拼接key = 物品id_货币类型_物品品质
	char key[128] = {0};
	SPRINTF_S(key,"%s_%d_%d", config_id, capital_type, item_color);

	ItemPlants item_plants;
	ItemPlantsMap::const_iterator iter = m_ItemPlantsMap.find(key);
	if (iter == m_ItemPlantsMap.end())
	{
		// 未找到
		extend_warning(LOG_ERROR, "[%s][%d]Can't get items price from plants[%s]", __FILE__, __LINE__, key);
		return false;
	}

	// 找到合适的价目标
	item_plants = iter->second;

	// 主流价格
	main_price = item_plants.main_price;

	// 计算元宝价格
	diamond_price = item_plants.diamond_price;

	// 折扣
	discount = GetDiscountByBound(item_plants.discount_list, item_plants.discount_weight_sum);

	return true;
}

// 把 float 转成INT, 向上取整
int ShopUtils::Float2IntRoundUp(float f)
{
	int ret = (int) f;
	int temp = (int) (f * 10);

	if (temp % 10 == 0)
	{
		return ret;
	}
	else
	{
		return ret + 1;
	}
}

/*!
* @brief 根据商店类型生成一个相应等级的商店物品
* @param shop_type 商店类型
* @param grid_index 商店格子索引
* @param output_item	产出的物品（一个）
* @return bool
*/
bool ShopUtils::GenerateShopItem(IKernel *pKernel, const PERSISTID &self, 
								 const int shop_type, const int grid_index, IVarList& output_item)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	const GeneralShop *general_shop = GetShopRule(shop_type);
	if (NULL == general_shop)
	{
		return false;
	}

	int player_level = pSelfObj->QueryInt("Level");

	int level_x0 = player_level/10 * 10;	    // 处理 10, 20, 30 这样的级别
	int level_x5 = player_level/10 * 10 + 5;	// 处理 15, 25, 35 这样级别

	const std::vector<ShopRule> *rule_list = NULL;

	// 为玩家找到合适的等级段
	int test_level = level_x0;	// player_level >= level_x0 并且 player_level < level_x5
	if (player_level >= level_x5)
	{
		test_level = level_x5;	// player_level >= level_x5 并且 player_level < level_x0
	}

	// 找到可用的最高等级的规则列表
	LoopBeginCheck(k);
	while (test_level >= 0)
	{
		LoopDoCheck(k);
		ShopRuleMap::const_iterator it = general_shop->shopRuleMap.find(test_level);
		if (it == general_shop->shopRuleMap.end())
		{
			test_level -= 5;	// 未找到，则找上一个等级段
		}
		else
		{
			rule_list = &(it->second);
			break;
		}
	}

	if (NULL == rule_list)
	{
		return false;
	}

	// 选取一个规则
	const ShopRule *shop_rule = ChooseShopRule(grid_index, rule_list);

	return OutPutItems(pKernel, self, shop_rule, shop_type, grid_index, output_item);
}

/*!
* @brief	 产出物品
* @param shop_type	商店类型
* @param grid_index	商店格子
* @param output_items	必出商品（一个或多个）
* @return bool
*/
bool ShopUtils::OutPutItems(IKernel *pKernel, const PERSISTID &self, 
							const ShopRule *shop_rule, const int shop_type, const int grid_index, IVarList& output_item)
{
	if (NULL == shop_rule)
	{
		return false;
	}

	// 随机取出一个掉落包
	const char *drop_pack = ChooseDropPack(shop_rule);
	if (StringUtil::CharIsNull(drop_pack))
	{
		return false;
	}

	CVarList output_of_drop;
	// 按掉落包出产物品
	DropModule::m_pDropModule->GetItemsFromDropID(pKernel, drop_pack, output_of_drop, self);
	int item_counts = (int)output_of_drop.GetCount()/2;
	if (item_counts < 1)
	{
#if defined _DEBUG
		output_item << "1"
							  << -1
							  << 1
							  << 0
							  << 0
							  << 100;
		extend_warning(LOG_ERROR, "[%s][%d]Can't get items from dropID[%s]", __FILE__, __LINE__, drop_pack);
		return false;
#endif
		output_item << "Item2010001"
							  << -1
							  << 1
							  << 999999
							  << 0
							  << 100;
		extend_warning(LOG_ERROR, "[%s][%d]Can't get items from dropID[%s]", __FILE__, __LINE__, drop_pack);
		return false;
	}

	// 随机一件物品
	int item_index = util_random_int(item_counts);
	const char *item_config = output_of_drop.StringVal(item_index*2);
	int item_amount = output_of_drop.IntVal(item_index*2 + 1);
	int item_color = 1;	//item_amount 用来查找物品价格

	if (StringUtil::CharIsNull(item_config))
	{
		return false;
	}

	// 对物品潜规则，物品是装备，品质为数量，数量为1
	if (ToolItemModule::IsCanWearItem(pKernel, item_config))	// 是装备或者符文
	{
		item_color = item_amount;
		item_amount = 1;
	}

	//物品单价
	float main_price = 0.0f;
	float diamond_price = 0.0f;
	float discount = 0.0f;
	GetItemPlants(pKernel, shop_type, item_config, item_color, main_price, diamond_price, discount);

	// 判断格子是否可打折
	discount = discount > 0 ? discount : 100.0f;
	discount = ShopGridCanDiscount(pKernel, self, shop_type, grid_index) == 0 ? discount : 100.0f;

	// 输出物品信息
	output_item << item_config
					      << item_color
						  << item_amount
						  << main_price
						  << diamond_price
						  << discount;

	return true;
}

/*!
* @brief	 获取第一次刷新的必出物品
* @param shop_type	商店类型
* @param grid_index	商店格子
* @param output_items	必出商品（一个或多个）
* @return bool
*/
bool ShopUtils::GenerateShopItemForFirstRefresh(IKernel *pKernel, const PERSISTID &self, 
												const int shop_type, IVarList& output_items)
{
	const GeneralShop *general_shop =  GetShopRule(shop_type);
	if (NULL == general_shop)
	{
		return false;
	}

	// 找到第一次刷新的规则
	const std::vector<ShopRule> *rule_list = NULL;
	ShopRuleMap::const_iterator it_rule_map = general_shop->shopRuleMap.find(SHOP_FIRST_REFRESH_FLAG);	// 第一次刷新配置规则
	if (it_rule_map == general_shop->shopRuleMap.end())
	{
		return false;
	}
	else
	{
		rule_list = &(it_rule_map->second);
	}

	if (rule_list->empty())
	{
		return false;
	}

	std::vector<ShopRule>::const_iterator it_rule = rule_list->begin();
	std::vector<ShopRule>::const_iterator it_rule_end = rule_list->end();

	int grid_idx = 0;
	LoopBeginCheck(l);
	for (; it_rule != it_rule_end; ++it_rule)
	{
		LoopDoCheck(l);
		const ShopRule *shop_rule = &(*it_rule);
		if (NULL == shop_rule)
		{
			continue;
		}

		// 物品产出
		if (OutPutItems(pKernel, self, shop_rule, shop_type, grid_idx, output_items))
		{
			++grid_idx;
		}
	}

	return true;
}

/*!
* @brief	 商店格子受限控制，计算物品索引是否受限 
* @param shop_type	商店类型
* @param row_index	要购买的商品索引
* @return int	0表示不受限，1表示受限制
*/
const int ShopUtils::CalGridLimitCondition(IKernel *pKernel, const PERSISTID &self, 
										   const int shop_type, int row_index)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 取出商店的基本配置
	const ShopManagerData *shop_manager_data = GetShopManagerData(shop_type);
	if (NULL == shop_manager_data)
	{
		return 0;
	}

	// 限制属性索引与物品的格子索引一一对应
	int property_index = (int)shop_manager_data->grid_limit_property.size() - 1;
	// 取得合法的属性索引，
	if (property_index < 0)
	{
		return 0;
	}
	// 找到对应的属性索引，不足以最后一个属性为准
	if (property_index > row_index)
	{
		property_index = row_index;
	}

	// 限制属性名
	const char *property_name = shop_manager_data->grid_limit_property[property_index].c_str();
	if (StringUtil::CharIsNull(property_name))
	{
		return 0;
	}

	// 限制属性不存在
	if (!pSelfObj->FindAttr(property_name))
	{
		return 0;
	}

	// 属性类型 
	int property_type = pSelfObj->GetAttrType(property_name);
	int limit_value = 0;
	// 默认转化为int类型数值
	if (VTYPE_INT == property_type)
	{
		limit_value = pSelfObj->QueryInt(property_name);
	}
	else if (VTYPE_INT64 == property_type)
	{
		limit_value = (int)pSelfObj->QueryInt64(property_name);
	}
	else if (VTYPE_FLOAT == property_type)
	{
		limit_value = (int)pSelfObj->QueryFloat(property_name);
	}
	else if (VTYPE_DOUBLE == property_type)
	{
		limit_value = (int)pSelfObj->QueryDouble(property_name);
	}
	else
	{
		// 不接受非数值型的属性
		return 0;
	}

	// 条件索引：条件索引与物品的格子索引一一对应
	int condition_index = (int)shop_manager_data->grid_limit_condition.size() - 1;
	if (condition_index < 0)
	{
		return 0;
	}

	// 找到对应的条件索引，条件不足，以最后一个条件为准
	if (condition_index > row_index)
	{
		condition_index = row_index;
	}

	// 格子受限制
	if (limit_value < shop_manager_data->grid_limit_condition[condition_index])
	{
		return 1;
	}

	return 0;
}

/*!
* @brief	  取得不同类型商店付费刷新的最大购买次数 
* @param shop_type	商店类型
* @return int	付费刷新的最大购买次数，与玩家VipLevel有关
*/
const int ShopUtils::GetShopMaxBuyCount(IKernel *pKernel, const PERSISTID &self, const int shop_type)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 商店管理配置
	const ShopManagerData *shop_manager_data = GetShopManagerData(shop_type);
	if (NULL == shop_manager_data)
	{
		return 0;
	}

	return 0/* VipModule::m_pVipModule->GetNumber(pKernel, self, shop_manager_data->shop_vip_logic_name)*/;
}

/*!
* @brief	 根据商店类型取得下次刷新价格
* @param shop_type	商店类型
* @param cur_refresh_count	当前免费刷新次数
* @param cur_pay_refresh_count	当前付费刷新次数
* @return int 返回下次刷新价格 -1表示非法数据
*/
const int ShopUtils::GetNextRefreshCostValue(IKernel* pKernel, const PERSISTID& self, 
											 const int shop_type, int cur_refresh_count /*= 0*/, int cur_pay_refresh_count /*= 0*/)
{
	const ShopManagerData *shop_manager_data = GetShopManagerData(shop_type);
	if (NULL == shop_manager_data)
	{
		return -1;
	}

	// 当前免费刷新次数 = 当前刷新次数 - 当前付费刷新次数
	int cur_free_refresh_count = cur_refresh_count - cur_pay_refresh_count;
	if (shop_manager_data->max_free_count > cur_free_refresh_count)	// 可以免费刷新，默认下次刷新价格为0
	{
		return 0;
	}

	// 计算下次刷新价格
	int cost_index = (int)shop_manager_data->refresh_cost_value.size() - 1;
	if (cost_index < 0)
	{
		return 0;
	}

	if (cost_index > cur_pay_refresh_count)
	{
		cost_index = cur_pay_refresh_count;
	}

	// 返回对应的配置价格
	return shop_manager_data->refresh_cost_value[cost_index];
}

/*!
* @brief	 玩家手动刷新商店
* @param shop_type	商店类型
* @param free_refresh	是否付费刷新
* @return void
*/
void ShopUtils::DoRefreshShop(IKernel* pKernel, const PERSISTID& self, 
							  const int shop_type, bool free_refresh /*= true*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在或未开启
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 商店不存在
	const ShopManagerData *shop_manager_data = GetShopManagerData(shop_type);
	if (NULL == shop_manager_data)
	{
		return;
	}

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}
	const ShopRefreshInfo* refresh_info = pShop->GetShopRefreInfo();
	if (NULL == refresh_info)
	{
		return;
	}

	// 当前刷新次数
	int cur_refresh_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_CUR_REFRESH_COUNT);
	// 付费刷新次数
	int pay_refresh_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_CUR_PAY_REFRESH_COUNT);
	// 免费刷新最大次数
	int max_free_refresh_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_MAX_FREE_REFRESH_COUNT);
	// 刷新价格
	int refresh_cost = pShopManagerRec->QueryInt(exist_row, SHOP_REC_REFRESH_COSTVALUE);

	// 付费刷新最大购买次数
	int max_pay_refresh_count = GetShopMaxBuyCount(pKernel, self, shop_type);

	// 免费刷新剩余次数
	int remain_free_count = max_free_refresh_count - (cur_refresh_count - pay_refresh_count);
	if (remain_free_count <= 0 && pay_refresh_count >= max_pay_refresh_count && free_refresh == false)
	{
		// 付费刷新次数已用完
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19002, CVarList());
		return;
	}

	// 免费次数已用完，但有付费刷新次数
	if (remain_free_count <= 0 && free_refresh)
	{
		// 通知客户端弹出付费刷新的界面
		CVarList pay_msg;
		pay_msg << SERVER_CUSTOMMSG_BUY_INFO
						<< refresh_info->vip_buy_type
						<< refresh_cost		// 购买花费
						<< pay_refresh_count	// 当前购买次数
						<< max_pay_refresh_count;	// 最大购买次数
		pKernel->Custom(self, pay_msg);
		return;
	}

	// 计算刷新价格
	if (refresh_cost > 0)
	{
		// 资金是否够
		__int64 sliver_have = CapitalModule::m_pCapitalModule->GetCapital(pKernel, self, CAPITAL_GOLD);
		if (refresh_cost > sliver_have)
		{
			// 元宝不足
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7011, CVarList());

			return;
		}

		// 扣除资金
		if (DC_SUCCESS != CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, 
			CAPITAL_GOLD, refresh_cost, refresh_info->lose_capital_log))
		{
			// 扣元宝失败
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7011, CVarList());
			return;
		}

		// 记录一次付费刷新次数
		pShopManagerRec->SetInt(exist_row, SHOP_REC_CUR_PAY_REFRESH_COUNT, ++pay_refresh_count);
	}

	// 执行刷新
	++cur_refresh_count;	// 当前刷新次数递增	
	// 计算下次刷新价格、最大免费刷新次数
	int next_refresh_cost = GetNextRefreshCostValue(pKernel, self, shop_type, 
		cur_refresh_count, pay_refresh_count);

	next_refresh_cost = next_refresh_cost < 0 ? 0 : next_refresh_cost;

	pShopManagerRec->SetInt(exist_row, SHOP_REC_CUR_REFRESH_COUNT, cur_refresh_count);
	pShopManagerRec->SetInt(exist_row, SHOP_REC_REFRESH_COSTVALUE, next_refresh_cost);

	// 进行刷新
	RefreshShopItem(pKernel, self, shop_type);

	// 玩法日志-商店刷新
	/*GamePlayerActionLog log;
	log.actionType    =   refresh_info->game_action_log;
	log.actionState   =    LOG_GAME_ACTION_JOIN;
	LogModule::m_pLogModule->SaveGameActionLog(pKernel, self, log);

	LogModule::m_pLogModule->OnShopRefresh(pKernel, self, shop_type, refresh_cost);*/
}

/*!
* @brief	 玩家第一次刷新商店（为引导提供的免费不计数刷新）
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::OnFirstTimeRefreshShop(IKernel* pKernel, const PERSISTID& self, 
									   const int shop_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}

	// 商品表
	IRecord * pRec = pSelfObj->GetRecord(pShop->GetShopRecName());
	if (NULL == pRec)
	{
		return;
	}

	pRec->ClearRow();

	CVarList dropItems; // 最终商品表
	int total_cell_count = pRec->GetRowMax(); // 商店容量
	int item_attr_count = 6; // 每个物品有5个属性

	// 先取得必需要出的物品
	GenerateShopItemForFirstRefresh(pKernel, self, shop_type, dropItems);

	// 计算商店剩余的格子
	int used_cell_count = (int)dropItems.GetCount()/item_attr_count;

	// 剩下的格子按照正常规则生成物品
	LoopBeginCheck(m);
	for (int i = used_cell_count; i < total_cell_count; ++i)
	{
		LoopDoCheck(m);
		GenerateShopItem(pKernel, self, shop_type, i, dropItems);
	}

	int item_count = (int)(dropItems.GetCount()/item_attr_count);
	if(item_count > total_cell_count)
	{
		item_count = total_cell_count;
	}

	int item_index = -1;
	LoopBeginCheck(n);
	for (int i = 0; i < item_count; ++i)
	{
		LoopDoCheck(n);
		std::string itemConfig = dropItems.StringVal(++item_index);
		int itemColor = dropItems.IntVal(++item_index);
		int itemAmount = dropItems.IntVal(++item_index);
		int price = dropItems.IntVal(++item_index);
		int diamond = dropItems.IntVal(++item_index);
		float discount = dropItems.FloatVal(++item_index);

		if (!itemConfig.empty() && itemAmount > 0)
		{
			CVarList new_row;
			new_row << itemConfig;
			new_row << itemColor;
			new_row << itemAmount;
			new_row << price;
			new_row << diamond;
			new_row << discount;

			pRec->AddRowValue(-1, new_row);
		}
	}
}

/*!
* @brief	 商店开启
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::OpenShop(IKernel* pKernel, const PERSISTID& self, const int shop_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 玩家等级
	int self_level = pSelfObj->QueryInt("Level");

	const ShopManagerData *shop_manager_shop = ShopUtilClass::Instance()->GetShopManagerData(shop_type);
	if (NULL == shop_manager_shop)
	{
		return;
	}

	// 已经开启
	if (pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type) >= 0)
	{
		return;
	}

	// 未开启，检查是否达到开启等级
	if (self_level < shop_manager_shop->start_level)
	{
		return;
	}

	// 计算下次刷新价格
	int next_refresh_cost = ShopUtilClass::Instance()->GetNextRefreshCostValue(pKernel, self, shop_type);
	if (-1 == next_refresh_cost)
	{
		return;
	}

	// 开启商店，商店管理表添加一条记录
	CVarList row_value;
	row_value << shop_type	// 商店类型
					  << 0	// 当前刷新次数
					  << 0	// 当前付费刷新次数
					  << shop_manager_shop->max_free_count		// 免费刷新最大次数
					  << shop_manager_shop->shop_capital_type	// 商店通用货币类型
					  << next_refresh_cost	// 刷新花费
					  << shop_manager_shop->first_free;	// 是否第一次手动刷新
	pShopManagerRec->AddRowValue(-1, row_value);

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}

	// 第一次刷新商店
	IRecord * pRec = pSelfObj->GetRecord(pShop->GetShopRecName());
	if (NULL == pRec)
	{
		return;
	}

	if (pRec->GetRows() == 0)
	{
		ShopUtilClass::Instance()->RefreshShopItem(pKernel, self, shop_type);
	}
}

/*!
* @brief	 商店物品刷新通用处理
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::RefreshShopItem(IKernel *pKernel, const PERSISTID &self, const int shop_type)
{
	// 玩家不存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在或未开启
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}

	// 商店不存在
	IRecord * pRec = pSelfObj->GetRecord(pShop->GetShopRecName());
	if (NULL == pRec)
	{
		return;
	}

	pRec->ClearRow();	// 清空上一次商品表

	// 根据玩家等级, 填充商店
	CVarList dropItems;
	LoopBeginCheck(o);
	for (int i = 0; i < pRec->GetRowMax(); ++i)
	{
		LoopDoCheck(o);

		// 根据玩家等级为特定格子生成一个商店物品
		ShopUtilClass::Instance()->GenerateShopItem(pKernel, self, shop_type, i, dropItems);
	}

	int item_index = -1;
	int item_count = (int)(dropItems.GetCount()/6);
	LoopBeginCheck(p);
	for (int i = 0; i < item_count; ++i)
	{
		LoopDoCheck(p);
		std::string itemConfig = dropItems.StringVal(++item_index);
		int itemColor = dropItems.IntVal(++item_index);
		int itemAmount = dropItems.IntVal(++item_index);
		int price = dropItems.IntVal(++item_index);
		int diamond = dropItems.IntVal(++item_index);
		float discount = dropItems.FloatVal(++item_index);

		if (!itemConfig.empty() && itemAmount > 0)
		{
			CVarList new_row;
			new_row << itemConfig;
			new_row << itemColor;
			new_row << itemAmount;
			new_row << price;
			new_row << diamond;
			new_row << discount;

			pRec->AddRowValue(-1, new_row);
		}
	}

	// 重新查询商店信息，推送给客户端
	ShopUtilClass::Instance()->RetrieveShop(pKernel, self, shop_type);
}

/*!
* @brief	 商店刷新次数通用处理
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::RefreshShopTimes(IKernel *pKernel, const PERSISTID &self, const int shop_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在或未开启
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 重置商店的当前刷新次数、已付费刷新次数、下次刷新价格
	int refresh_cost_value = ShopUtilClass::Instance()->GetNextRefreshCostValue(pKernel, self, shop_type);

	refresh_cost_value = refresh_cost_value < 0 ? 0 : refresh_cost_value;

	pShopManagerRec->SetInt(exist_row, SHOP_REC_CUR_REFRESH_COUNT, 0);	// 当前刷新次数
	pShopManagerRec->SetInt(exist_row, SHOP_REC_CUR_PAY_REFRESH_COUNT, 0);	// 当前付费刷新次数
	pShopManagerRec->SetInt(exist_row, SHOP_REC_REFRESH_COSTVALUE, refresh_cost_value);	// 下次刷新价格

	// 通知客户端刷新成功
	CVarList args;
	args << SERVER_CUSTOMMSG_SHOP_INFO
			<< SC_SUB_SHOP_REFRESH_STAT
			<< shop_type
			<< pShopManagerRec->QueryInt(exist_row, SHOP_REC_MAX_FREE_REFRESH_COUNT)	// 剩余免费刷新次数
			<< refresh_cost_value; // 下次刷新价格
	pKernel->Custom(self, args);
}

/*!
* @brief	玩家手动刷新商店
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::OnPlayerRefreshShop(IKernel* pKernel, const PERSISTID& self, const int shop_type, bool free_refresh)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在或未开启
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 判断是否需要为引导做一次免费不计数刷新 0表示需要 1表示不需要
	int first_time_refresh = pShopManagerRec->QueryInt(exist_row, SHOP_REC_REFRESH_ISFIRST);
	if (0 == first_time_refresh)
	{
		// 第一次刷新商店是免费的，同时也不占用免费刷新机会
		ShopUtilClass::Instance()->OnFirstTimeRefreshShop(pKernel, self, shop_type);

		// 设置标签为已使用过免费不计数
		pShopManagerRec->SetInt(exist_row, SHOP_REC_REFRESH_ISFIRST, 1);

		// 重新查询商店信息推送给客户端
		ShopUtilClass::Instance()->RetrieveShop(pKernel, self, shop_type);
	}
	else
	{
		// 正常计数刷新
		ShopUtilClass::Instance()->DoRefreshShop(pKernel, self, shop_type, free_refresh);
	}
}

/*!
* @brief	购买商店物品处理 
* @param shop_type	商店类型
* @param buy_index	要购买的商品索引
* @param buy_amount	要购买的商品个数
* @return void
*/
void ShopUtils::DoBuyShopItem(IKernel *pKernel, const PERSISTID &self, 
							  const int shop_type, int buy_index, int buy_amount)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}
	const ShopBuyLog *buy_log = pShop->GetShopBuyLog();
	if (NULL == buy_log)
	{
		return;
	}

	// 取出商店交易货币类型
	int capital_type = pShopManagerRec->QueryInt(exist_row, SHOP_REC_TRADE_CAPITALTYPE);

	// 背包是否存在
	PERSISTID tool_box = pKernel->GetChild(self, L"ItemBox");
	if (!pKernel->Exists(tool_box))
	{
		return;
	}

	// 商店不存在
	IRecord * pRec = pSelfObj->GetRecord(pShop->GetShopRecName());
	if (NULL == pRec)
	{
		return;
	}

	// 购买索引
	if (buy_index < 0 || buy_index >= pRec->GetRows())
	{
		return;
	}

	// 该物品是否受限制
	if (1 == ShopUtilClass::Instance()->CalGridLimitCondition(pKernel, self, shop_type, buy_index))
	{
		// 没有购买该物品的权限
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19003, CVarList());
		return;
	}

	// 商品数量不够
	int item_amount = pRec->QueryInt(buy_index, COMMON_SHOP_REC_NUM);
	if (buy_amount > item_amount)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19004, CVarList());
		return;
	}

	//设置物品购买后的数量
	int item_leftAmount = item_amount - buy_amount;

	// 获得购买物品id、品质和数量
	const char * item_config = pRec->QueryString(buy_index, COMMON_SHOP_REC_CONFIG);
	int item_color = pRec->QueryInt(buy_index, COMMON_SHOP_REC_COLORLVL);
	

	// 此处潜规则,(物品的品质和数量的关系)
	int real_amount = buy_amount; // 物品真实数量
	if (ToolItemModule::IsCanWearItem(pKernel, item_config))
	{
		// 如果是装备或符文，则把品质付给数量， 因为在创建物品的时候，装备的数量属性会被当做品质
		buy_amount = item_color;
		real_amount = 1;
		item_leftAmount = 0;
	}
	if (StringUtil::CharIsNull(item_config) || item_amount < 0)
	{
		return;
	}

	// 包裹空间是否够
	int nBindState = FunctionEventModule::GetItemBindState(buy_log->logSrcFunction);
	if (ContainerModule::TryAddItem(pKernel, tool_box, item_config, buy_amount, nBindState) < real_amount)
	{
		// 背包空间不足
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7004, CVarList());
		return;
	}

	// 折扣
	float discount = pRec->QueryFloat(buy_index, COMMON_SHOP_REC_DISCOUNT);

	// 主要交易货币价格
	int price = pRec->QueryInt(buy_index, COMMON_SHOP_REC_PRICE) * real_amount;
	price = ShopUtilClass::Instance()->Float2IntRoundUp(price*discount/100);

	// 元宝货币类型，非必须
	int need_diamond = pRec->QueryInt(buy_index, COMMON_SHOP_REC_DIAMOND) * real_amount;
	need_diamond = ShopUtilClass::Instance()->Float2IntRoundUp(need_diamond*discount/100);
	if (price <= 0 && need_diamond <= 0)	// 价格异常
	{
		return;
	}

	// 价格是否够
	int64_t have_capital = CapitalModule::m_pCapitalModule->GetCapital(pKernel, self, capital_type);
	int64_t have_diamond = CapitalModule::m_pCapitalModule->GetCapital(pKernel, self, CAPITAL_GOLD);
	if ((int64_t)price > have_capital)
	{
		// 交易货币不足
		CustomSysInfo(pKernel, self, buy_log->lack_capital, CVarList());
		return;
	}

	// 钻石是否够
	if ((int64_t)need_diamond > have_diamond)
	{
		// 元宝不足
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7011, CVarList());
		return;
	}

	// 扣货币
	if (price > 0 && DC_SUCCESS != CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, 
		capital_type, price, buy_log->logSrcFunction))
	{
		// 扣货币失败
		CustomSysInfo(pKernel, self, buy_log->pay_failed, CVarList());
		return;
	}

	// 扣元宝
	if (need_diamond > 0 && DC_SUCCESS != CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, 
		CAPITAL_GOLD, need_diamond, buy_log->logSrcFunction))
	{
		// 扣元宝失败
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7012, CVarList());
		// 返还已扣货币
		if (price > 0)
		{
			// 返还购买失败扣的银元
			CapitalModule::m_pCapitalModule->IncCapital(pKernel, self, capital_type,
				price, FUNCTION_EVENT_ID_SHOPING_FAILED_RET);
		}

		return;
	} 

	// 放入物品
	const int put_cnt = ContainerModule::PlaceItem(pKernel, tool_box, item_config, buy_amount, 
					(EmFunctionEventId)buy_log->logSrcFunction, true, nBindState);

	// 购买成功
	if (put_cnt > 0)
	{
		pRec->SetInt(buy_index, COMMON_SHOP_REC_NUM, item_leftAmount);

		// 通知客户端购买商品的信息
		CVarList buy_args;
		buy_args << SERVER_CUSTOMMSG_SHOP_INFO
						<< SC_SUB_SHOP_BUY_SUC
						<< shop_type
						<< buy_index
						<< item_config
						<< item_leftAmount
						<< real_amount;
		pKernel->Custom(self, buy_args);
	}
	else	// 购买失败，返还已扣金额
	{
		// 购买失败
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19001, CVarList());

		// 返还已扣货币
		if (price > 0)
		{
			// 返还购买失败扣的货币
			CapitalModule::m_pCapitalModule->IncCapital(pKernel, self, capital_type,
				price, FUNCTION_EVENT_ID_SHOPING_FAILED_RET);
		}

		// 返还已扣元宝
		if (need_diamond > 0)
		{
			// 返还购买失败扣的银元
			CapitalModule::m_pCapitalModule->IncCapital(pKernel, self, CAPITAL_GOLD,
				need_diamond, FUNCTION_EVENT_ID_SHOPING_FAILED_RET);
		}

		// 记录购买失败物品
		extend_warning(LOG_WARNING, "[%s][%d] buy failed[%d:%s:%d:%d]", __FILE__, __LINE__, 
			shop_type, item_config, price, need_diamond);
	}

	// 玩法日志-商店购买
	/*GamePlayerActionLog actionLog;
	actionLog.actionType = buy_log->logSrcFunction;
	actionLog.actionState = LOG_GAME_ACTION_JOIN;
	LogModule::m_pLogModule->SaveGameActionLog(pKernel, self, actionLog);

	int nLogCapitalType = need_diamond > 0 ? CAPITAL_GOLD : capital_type;
	int nLogCapitalCost = need_diamond > 0 ? need_diamond : price;
	LogModule::m_pLogModule->OnShopExchange(pKernel, self, shop_type, nLogCapitalType, nLogCapitalCost, item_config, item_amount);*/
}

/*!
* @brief	查询商店详细信息（商品信息、免费刷新次数、购买刷新金额等）
* @param shop_type	商店类型
* @return void
*/
void ShopUtils::RetrieveShop(IKernel* pKernel, const PERSISTID& self, 
							 const int shop_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 商店管理表
	IRecord *pShopManagerRec = pSelfObj->GetRecord(FIELD_RECORD_SHOP_MANAGER_REC);
	if (NULL == pShopManagerRec)
	{
		return;
	}

	// 商店不存在或未开启
	int exist_row = pShopManagerRec->FindInt(SHOP_REC_TYPE, shop_type);
	if (exist_row < 0)
	{
		return;
	}

	// 获得商店指针
	ShopBase *pShop = ShopManagerModule::m_pShopManagerModule->GetShop(shop_type);
	if (NULL == pShop)
	{
		return;
	}

	// 商店不存在
	IRecord * shop_rec = pSelfObj->GetRecord(pShop->GetShopRecName());
	if (NULL == shop_rec)
	{
		return;
	}

	// 货币类型
	int capital_type = pShopManagerRec->QueryInt(exist_row, SHOP_REC_TRADE_CAPITALTYPE);

	// 最大免费刷新次数
	int max_free_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_MAX_FREE_REFRESH_COUNT);
	// 已刷新次数
	int cur_refresh_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_CUR_REFRESH_COUNT);
	// 已付费刷新次数
	int cur_pay_refresh_count = pShopManagerRec->QueryInt(exist_row, SHOP_REC_CUR_PAY_REFRESH_COUNT);
	// 剩余免费次数
	int remain_free_count = max_free_count - (cur_refresh_count - cur_pay_refresh_count);
	remain_free_count = remain_free_count < 0 ? 0 : remain_free_count;

	// 下次刷新需要的价格
	int next_refresh_cost = pShopManagerRec->QueryInt(exist_row, SHOP_REC_REFRESH_COSTVALUE);
	next_refresh_cost = next_refresh_cost < 0 ? 0 : next_refresh_cost;

	// 商品列表
	CVarList items_in_shop;
	int items_count = shop_rec->GetRows();
	int items_in_shop_count = 0;

	LoopBeginCheck(q);
	for (int i = 0; i < items_count; ++i)
	{
		LoopDoCheck(q);
		CVarList item;
		shop_rec->QueryRowValue(i, item);
                   
		// 需要计算每个商品格子是否受限制
		items_in_shop << i 
								 << item 
								 << ShopUtilClass::Instance()->CalGridLimitCondition(pKernel, self, shop_type, i);
		++items_in_shop_count;
	}

	// 查询熔炼商店里的物品
	CVarList shop_info;
	shop_info << SERVER_CUSTOMMSG_SHOP_INFO
					 << SC_SUB_SHOP_RETRIEVE
					 << shop_type	// 商店类型
					 << capital_type	// 商店交易货币
					 << remain_free_count	// 剩余免费刷新次数
					 << next_refresh_cost	// 下次刷新花费
					 << items_in_shop_count	// 商品数量
					 << items_in_shop;	// 商品信息

	pKernel->Custom(self, shop_info);
}

// 检查所有商店的掉落包、产出物品、价格是否正常配置
void ShopUtils::ValidateShopConf(IKernel *pKernel, const PERSISTID &self)
{
	LoopBeginCheck(x)
		for (int shops = 0; shops < (int)m_vecGeneralShop.size(); ++shops)
		{
			LoopDoCheck(x);
			const GeneralShop &general_shop = m_vecGeneralShop[shops];
			const ShopRuleMap &shop_rule_map = general_shop.shopRuleMap;
			const int shop_type = general_shop.shop_type;
			ShopRuleMap::const_iterator it1 = shop_rule_map.begin();
			LoopBeginCheck(y)
				while (it1 != shop_rule_map.end())
				{
					LoopDoCheck(y);
					const std::vector<ShopRule> &shop_rule_list = it1->second;
					LoopBeginCheck(z)
						for (int rules = 0; rules < (int)shop_rule_list.size(); ++rules)
						{
							LoopDoCheck(z);
							const ShopRule & shop_rule = shop_rule_list[rules];
							const DropPackList &drop_pack_list = shop_rule.drop_pack_list;
							LoopBeginCheck(aa)
								for (int drops = 0; drops < (int)drop_pack_list.size(); ++drops)
								{
									LoopDoCheck(aa);
									const DropPack & drop_pack = drop_pack_list[drops];
									const char *drop_id = drop_pack.drop_id.c_str();
									CVarList output_of_drop;
									// 按掉落包出产物品
									DropModule::m_pDropModule->GetItemsFromDropID(pKernel, drop_id, output_of_drop, self);
									int item_counts = (int)output_of_drop.GetCount()/2;
									if (item_counts < 1)
									{
										extend_warning(LOG_ERROR, "[%s][%d]Can't get items from dropID[%s] of Level[%d] from shop_type[%d]", 
											__FILE__, __LINE__, drop_id, shop_rule.player_level, shop_type);
										continue;
									}
									// 随机一件物品
									LoopBeginCheck(bb)
										for (int indexs = 0; indexs < item_counts; ++indexs)
										{
											LoopDoCheck(bb);
											const char *item_config = output_of_drop.StringVal(indexs*2);
											int item_amount = output_of_drop.IntVal(indexs*2 + 1);
											int item_color = 1;	// 用来查找物品价格

											if (StringUtil::CharIsNull(item_config))
											{
												continue;
											}	

											// 对物品潜规则，物品是装备，品质为数量，数量为1
											if (ToolItemModule::IsCanWearItem(pKernel, item_config))	// 是装备或者砭石
											{
												item_color = item_amount;
												item_amount = 1;
											}

											float main_price = 0.0f;
											float diamond_price = 0.0f;
											float discount = 0.0f;
											GetItemPlants(pKernel, shop_type, item_config, item_color, main_price, diamond_price, discount);
										}
								}
						}

						++it1;
				}
		}

}