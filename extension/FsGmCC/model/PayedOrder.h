//--------------------------------------------------------------------
// 文件名:		PayedOrder.h
// 内  容:		支付成功的订单
// 说  明:		
// 创建日期:		2016年9月26日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_PAYED_ORDER_INFO_H__
#define __MODEL_PAYED_ORDER_INFO_H__
#include <string>

//-----------------------------------------
// 新数据结构
//-----------------------------------------
// 物品项
struct GoodsItem
{
	std::string		ItemId;			// 物品ID
	int				Number;			// 物品数量
};

// 产品说明
struct Product
{
	std::string ProductId;	// 商品ID
	std::vector<GoodsItem> Items;		// 物品列表
	std::vector<GoodsItem> Rewards;		// 赠送物品列表
	std::vector<GoodsItem> Gifts;		// 首充物品列表
	std::vector<GoodsItem> Extra;		// 超过购买次数的用这个列表替代Items;
	float Price;			// 商品价格
	int Rule;				// 商品规则 EmProductRule
	int Times;				// 可购买次数 0 不限制( 配合Rule使用)
	int State;				// 当前状态
};

// 购买完成数据
struct Payment
{
	std::string RoleId;			// 玩家uid
	std::string OrderId;		// 订单Id
	std::string ProductId;		// 产品id
	float		Amount;			// 支付金额
	int			State;			// 订单状态 EmOrderState
	int			PayType;		// 支付类型 EmBillType
	int64_t		OrderTime;		// 下单时间
	int64_t		PayedTime;		// 支付时间
};

// 下单
struct ProductOrder
{
	std::string RoleId;			// 角色id
	std::string RoleName;		// 角色名
	std::string Token;			// 客户端token
	std::string ProductId;		// 产品id
};

// 下单返回
struct ProductOderInfo
{
	std::string RoleId;			// 角色id
	std::string ProductId;		// 产品id
	std::string OrderId;		// 订单id
	std::string Extra;			// 扩展字符串
	int			State;			// 订单状态 EmOrderState
	int			PayType;		// 支付类型(State为支付时有效) EmBillType
	float		Price;			// 实际支付金额
};

// 订单状态通知
struct OrderNotify
{
	std::string RoleId;				// 玩家uid
	std::string OrderId;			// 订单ID

	int State;						// 状态 EmOrderState
};
#endif 