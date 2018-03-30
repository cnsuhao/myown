// -------------------------------------------
// 文件名称： ShopManagerModule.h
// 文件说明： 商店管理模块
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：   
// -------------------------------------------
#ifndef _SHOP_MANAGER_MODULE_H_
#define _SHOP_MANAGER_MODULE_H_
#include "Fsgame/Define/header.h"
#include "ShopManagerDefine.h"
#include "ShopBase.h"

class ShopManagerModule : public ILogicModule
{

public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

	// 获得商店指针
	ShopBase* GetShop(const int shop_type);

private:
	// 注册商店模板
	bool RegistShopTemplate(IKernel* pKernel, ShopBase *pShopBase);

	// 初始化商店模板
	bool InitShopTemplate(IKernel* pKernel);


	/************************************************************************/
	/*												主处理函数				*/
	/************************************************************************/

public:
	// 商店开张
	int ExecOpenShop(IKernel* pKernel, const PERSISTID& self);

	// 处理查询商店
	int ExecQueryShop(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type);

	// 处理商店刷新
	int ExecRefreshShop(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type);

	// 处理商店付费刷新
	int ExecPay2RefreshShop(IKernel *pKernel, const PERSISTID &self, 
		const int vip_buy_type);

	// 处理不同类型的商店消费
	int ExecShopping(IKernel *pKernel, const PERSISTID &self, 
		const int shop_type, const int buy_index, const int buy_amount);

	// 注册商店相关定时器
	int RegistShopResetTimer(IKernel *pKernel, const PERSISTID &self);

private:
	/************************************************************************/
	/*												回调函数					*/
	/************************************************************************/

	// 等级提升 
	static int OnCommandLevelUp(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 客户端消息回调
	static int OnCustomShop(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 客户端购买次数相关消息回调
	static int OnCustomBuyTimes(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static void ReloadConfig(IKernel *pKernel);

private:
	ShopBase* m_pArrayShop[SHOP_TYPE_TOTAL];

public:
	static ShopManagerModule *m_pShopManagerModule;
};

#endif // _SHOP_MANAGER_MODULE_H_