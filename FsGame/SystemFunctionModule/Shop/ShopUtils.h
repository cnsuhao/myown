// -------------------------------------------
// 文件名称： ShopUtils.h
// 文件说明： 商店工具类
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------
#ifndef _SHOP_UTILS_H_
#define _SHOP_UTILS_H_

#include "Fsgame/Define/header.h"
#include "ShopManagerDefine.h"
#include "utils/singleton.h"

class ShopUtils
{
public:
	// 初始化
	static bool Init(IKernel* pKernel);

	// 释放
	static bool Shut(IKernel* pKernel);

public:
	// 加载资源
	bool LoadResource(IKernel* pKernel);

	// 释放配置文件
	bool ReleaseResource(IKernel* pKernel);

	/************************************************************************/
	/*									         资源配置														   */
	/************************************************************************/
private:
	// 资源释放
	bool ReleaseAll(IKernel* pKernel);

	// 加载物品价目表配置
	bool LoadItemPlants(IKernel* pKernel);

	// 分配折扣的权重值
	void AssignDiscountWeight(const char *str, int &weight_sum, DiscountList &discount_list);

	// 加载商店规则配置
	bool LoadShopRule(IKernel* pKernel, const char* rule_file, const ShopTypes shop_type);

	// 根据权重取物品单价折扣
	const float GetDiscountByBound(const DiscountList &discount_list, const int weight_sum);

	// 选择一个商店规则
	const ShopRule* ChooseShopRule(const int grid_index, const std::vector<ShopRule> *rule_list);

	// 选择一个掉落包（随机取得）
	const char* ChooseDropPack(const ShopRule *shop_rule);

	// 商店格子是否可打折
	// 返回值：0表示可打折，1表示不可
	int ShopGridCanDiscount(IKernel *pKernel, const PERSISTID &self, const int shop_type, int row_index);

	// 取得对应类型的商店管理配置
	const ShopManagerData* GetShopManagerData(const int shop_type);

	// 取得对应类型的商店规则
	const GeneralShop* GetShopRule(const int shop_type) const;

	// 取得对应物品价目表
	bool GetItemPlants(IKernel* pKernel, const int shop_type, const char *config_id, 
		const int item_color, float &main_price, float &diamond_price, float &discount);

	// 把 float 转成INT, 向上取整
	int Float2IntRoundUp(float f);

	/************************************************************************/
	/*									         商品产出														   */
	/************************************************************************/

	// 产出物品
	bool OutPutItems(IKernel *pKernel, const PERSISTID &self, 
		const ShopRule *shop_rule, const int shop_type, const int grid_index, IVarList& output_item);

	// 根据商店类型生成一个相应等级的商店物品
	bool GenerateShopItem(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type, const int grid_index, IVarList& output_item);

	// 获取第一次刷新的必出物品
	bool GenerateShopItemForFirstRefresh(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type, IVarList& output_items);

	// 商店格子受限控制，计算物品索引是否受限
	// 返回值：0表示不受限，1表示受限制
	const int CalGridLimitCondition(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type, int row_index);

	// 取得不同类型商店的最大购买次数
	const int GetShopMaxBuyCount(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type);

	// 根据当前刷新次数和商店类型取得下次刷新价格
	const int GetNextRefreshCostValue(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type, int cur_refresh_count = 0, int cur_pay_refresh_count = 0);

	// 玩家刷新商店
	void DoRefreshShop(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type, bool free_refresh = true);

	// 玩家第一次刷新商店
	void OnFirstTimeRefreshShop(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type);

	/************************************************************************/
	/*									         外部接口														   */
	/************************************************************************/
public:
	// 商店开启
	void OpenShop(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type);

	// 商店物品刷新通用处理 
	void RefreshShopItem(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type);

	// 商店刷新次数通用处理
	void RefreshShopTimes(IKernel *pKernel, const PERSISTID &self, const int shop_type);

	// 玩家刷新商店
	void OnPlayerRefreshShop(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type, bool free_refresh);

	// 购买商店物品处理
	void DoBuyShopItem(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type, int buy_index, int buy_amount);

	// 查询商店
	void RetrieveShop(IKernel* pKernel, const PERSISTID& self, 
		const int shop_type);

	//////////////////////////////////物品检查GM命令////////////////////////////////////////

	// 检查所有商店的掉落包、产出物品、价格是否正常配置
	void ValidateShopConf(IKernel *pKernel, const PERSISTID &self);

private:
	ShopManagerDataVector m_vecShopManagerData;
	GeneralShopVector m_vecGeneralShop;
	ItemPlantsMap m_ItemPlantsMap;
};

// 单例模式
typedef HPSingleton<ShopUtils> ShopUtilClass;

#endif	// _SHOP_UTILS_H_