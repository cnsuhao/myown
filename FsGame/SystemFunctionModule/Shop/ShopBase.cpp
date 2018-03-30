// -------------------------------------------
// 文件名称： ShopBase.cpp
// 文件说明： 基础商店
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------

#include "ShopBase.h"
#include "ShopUtils.h"

ShopBase::ShopBase() : m_shop_type(SHOP_TYPE_NONE)
{
}

// 获得商店表名
const char* ShopBase::GetShopRecName()
{
	return m_shop_rec_name.c_str();
}

// 取得商店类型
const ShopTypes ShopBase::GetShopType()
{
	return m_shop_type;
}

// 获得刷新信息
const ShopRefreshInfo* ShopBase::GetShopRefreInfo()
{
	return &m_refresh_info;
}

// 获得购买日志信息
const ShopBuyLog* ShopBase::GetShopBuyLog()
{
	return &m_buy_log;
}

/*!
* @brief	开启商店
* @return void
*/
void ShopBase::OpenShop(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 商店开启
	ShopUtilClass::Instance()->OpenShop(pKernel, self, m_shop_type);
}

/*!
* @brief	查询商店详细信息（商品信息、免费刷新次数、购买刷新金额等）
* @return void
*/
void ShopBase::QueryShop(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 商店查询
	ShopUtilClass::Instance()->RetrieveShop(pKernel, self, m_shop_type);
}

/*!
* @brief	 玩家手动刷新商店
* @param free_refresh	是否付费
* @return void
*/
void ShopBase::HandRefresh(IKernel* pKernel, const PERSISTID& self, 
							   bool free_refresh /*= true*/)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 手动刷新商店
	ShopUtilClass::Instance()->OnPlayerRefreshShop(pKernel, self, m_shop_type, free_refresh);
}

// 购买
void ShopBase::Buy(IKernel *pKernel, const PERSISTID &self, int buy_index, int buy_amount)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 购买商品
	ShopUtilClass::Instance()->DoBuyShopItem(pKernel, self, m_shop_type, buy_index, buy_amount);
}
