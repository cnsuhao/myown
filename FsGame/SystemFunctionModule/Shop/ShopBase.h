// -------------------------------------------
// 文件名称： ShopBase.h
// 文件说明： 基础商店
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------
#ifndef _SHOP_BASE_H_
#define _SHOP_BASE_H_

#include "Fsgame/Define/header.h"
#include "ShopManagerDefine.h"
#include "FsGame/Define/Fields.h"

class ShopBase
{
public:
	ShopBase();

public:
	// 获得商店表名
	const char* GetShopRecName();

	// 取得商店类型
	const ShopTypes GetShopType();

	// 获得刷新信息
	const ShopRefreshInfo* GetShopRefreInfo();

	// 获得购买日志信息
	const ShopBuyLog* GetShopBuyLog();

public:
	// 初始化
	virtual bool Init(IKernel* pKernel) = 0;

	// 开启
	virtual void OpenShop(IKernel* pKernel, const PERSISTID& self);

	// 查询商店
	virtual void  QueryShop(IKernel* pKernel, const PERSISTID& self);

	// 手动刷新商店
	virtual void HandRefresh(IKernel* pKernel, const PERSISTID& self, bool free_refresh = true);

	// 购买
	virtual void Buy(IKernel *pKernel, const PERSISTID &self, int buy_index, int buy_amount);

	// 注册定时器
	virtual void RegistResetTimer(IKernel *pKernel, const PERSISTID &self) = 0;


	/************************************************************************/
	/*									         需要重写定时器函数			*/
	/************************************************************************/

public:
	// 物品刷新定时器
	static int ItemResetTimer(IKernel *pKernel, const PERSISTID &self, int slice);

	// 商店重置定时器
	static int ShopResetTimer(IKernel *pKernel, const PERSISTID &self, int slice);
	
protected:
	ShopTypes m_shop_type;
	std::string m_shop_rec_name;
	ShopRefreshInfo m_refresh_info;
	ShopBuyLog m_buy_log;

};

#endif // _SHOP_BASE_H_