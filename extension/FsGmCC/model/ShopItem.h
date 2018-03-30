//--------------------------------------------------------------------
// 文件名:		ShopItem.h
// 内  容:		商城商品定义
// 说  明:		
// 创建日期:		2016年11月27日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_SHOP_ITEM_H__
#define __MODEL_SHOP_ITEM_H__
#include <string>
#include <vector>

struct ShopItem
{
	std::string		id;					// 每个礼包唯一标识
	std::wstring	name;				// 名字
	std::wstring	desc;				// 介绍
	std::string		goods;				// 具体物品（格式: 物品1:数量,物品2:数量...）
	int				price;				// 出售价格
	int				color;				// 品质
	int				count;				// 总数量
	int				level;				// 购买限制的玩家等级，小于等于0 不限制
	int				vip;				// 购买限制的vip等级，小于等于0 不限制
	int				purchaseNum;		// 购买限制的购买数，如果为刷新类型，则为每天可购买数
	int				refreshType;		// 是否每天刷新，0为永不刷新
	int				discount;			// 折扣(整数 以10为百分比 1折就是10%)
	int				catalog;			// 分类
	time_t			onShowTime;			// 上架时间
	time_t			offShowTime;		// 下架时间
	time_t			onSellTime;			// 出售时间
	time_t			offSellTime;		// 停止时间

	time_t			newGoodTime;		// 新品显示截止时间
};

// 商城分类标签
struct ShopTag
{
	int id;				// 标签id
	std::wstring name;	// 标签名
};

struct ActivityItem
{
	int				value;				// 目标值
	std::wstring	name;				// 名字
	std::string		goods;				// 具体物品（格式: 物品1:数量,物品2:数量...）
	int				count;				// 总数量
};

struct ActivityInfo
{
	std::wstring	name;				// 名字
	std::wstring	desc;				// 介绍
	int actType;						// 活动类型
	time_t			startTime;			// 开始时间
	time_t			endTime;			// 结束时间
	std::vector<ActivityItem> items;		// 活动项
};

struct ConfigLoadInfo
{
	std::string key;
	std::string path;
	int32_t		catalog;
	int32_t		cfgid;
};
#endif 