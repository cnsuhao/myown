// -------------------------------------------
// 文件名称： ShopManagerModule.cpp
// 文件说明： 商店管理模块
// 创建日期： 2016/03/29
// 创 建 人：  tongzt
// 修改人：    
// -------------------------------------------

#include "ShopManagerModule.h"
#include "ShopUtils.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/CommandDefine.h"

#include "utils/extend_func.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "ShopGold.h"

/* 
** 静态成员初始化 */
ShopManagerModule *ShopManagerModule::m_pShopManagerModule = NULL;

// 资源重载
inline int nx_reload_shop_resource(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);

	ShopUtilClass::Instance()->Init(pKernel);

	return 1;
}

// 检查所有商店的物品是否正常
inline int nx_check_all_item(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_check_all_item, 1);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_check_all_item, 1);
	PERSISTID self = pKernel->LuaToObject(state, 1);

	ShopUtilClass::Instance()->ValidateShopConf(pKernel, self);

	return 1;
}

// 检查所有商店的物品是否正常
inline int nx_refresh_shop(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_refresh_shop, 1);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_refresh_shop, 1);
	PERSISTID self = pKernel->LuaToObject(state, 1);

	LoopBeginCheck(b);
	for (int i = SHOP_TYPE_NONE + 1; i < SHOP_TYPE_TOTAL; ++i)
	{
		LoopDoCheck(b);
		ShopUtilClass::Instance()->RefreshShopItem(pKernel, self, i);
	}

	return 1;
}

// 模块初始化
bool ShopManagerModule::Init( IKernel* pKernel )
{
	m_pShopManagerModule = this;
	Assert(m_pShopManagerModule != NULL);

	// 上线
	pKernel->AddEventCallback("player", "OnRecover", ShopManagerModule::OnPlayerRecover);

	// 客户端消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_SHOP_MANAGER, ShopManagerModule::OnCustomShop);

	// 客户端购买次数相关消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_BUY_INFO, ShopManagerModule::OnCustomBuyTimes);

	// 玩家升级
	pKernel->AddIntCommandHook("player", COMMAND_LEVELUP_CHANGE, ShopManagerModule::OnCommandLevelUp);

	// 资源加载
	if (!ShopUtilClass::Instance()->Init(pKernel))
	{
		Assert(false);
		return false;
	}

	// 模板注册
	if (!InitShopTemplate(pKernel))
	{
		Assert(false);
		return false;
	}

	// GM命令
	DECL_LUA_EXT(nx_reload_shop_resource);
	DECL_LUA_EXT(nx_check_all_item);
	DECL_LUA_EXT(nx_refresh_shop);

	RELOAD_CONFIG_REG("ShopConfig", ShopManagerModule::ReloadConfig);

	return true;
}

// 关闭
bool ShopManagerModule::Shut( IKernel* pKernel )
{
	return ShopUtilClass::Instance()->Shut(pKernel);
}

// 获得商店指针
ShopBase* ShopManagerModule::GetShop(const int shop_type)
{
	if (shop_type > SHOP_TYPE_NONE 
		&& shop_type < SHOP_TYPE_TOTAL)
	{
		return m_pArrayShop[shop_type];
	}

	return NULL;
}

// 注册商店模板
bool ShopManagerModule::RegistShopTemplate(IKernel* pKernel, ShopBase *pShopBase)
{
	if (NULL == pShopBase)
	{
		return false;
	}

	// 非法类型
	if (pShopBase->GetShopType() <= SHOP_TYPE_NONE 
		|| pShopBase->GetShopType() >= SHOP_TYPE_TOTAL)
	{
		delete pShopBase;
		extend_warning(pKernel,"Register Shop Failed");
		return false;
	}

	// 注册
	if ( NULL == m_pArrayShop[pShopBase->GetShopType()])
	{
		if (pShopBase->Init(pKernel))
		{
			m_pArrayShop[pShopBase->GetShopType()] = pShopBase;
			return true;
		}
		
		return false;
	}

	return false;
}

// 初始化商店模板
bool ShopManagerModule::InitShopTemplate(IKernel* pKernel)
{
	for (int i = SHOP_TYPE_NONE + 1; i < SHOP_TYPE_TOTAL; ++i)
	{
		m_pArrayShop[i] = NULL;
	}

	// 元宝商店
	if (!RegistShopTemplate(pKernel, NEW ShopGold))
	{
		return false;
	}

	//// 熔炼商店
	//if (!RegistShopTemplate(pKernel, NEW ShopSmelt))
	//{
	//	return false;
	//}

	//// 武勋商店
	//if (!RegistShopTemplate(pKernel, NEW ShopExploit))
	//{
	//	return false;
	//}

	//// 竞技场商店
	//if (!RegistShopTemplate(pKernel, NEW ShopArena))
	//{
	//	return false;
	//}

	

	//// 侠义值商店
	//if (!RegistShopTemplate(pKernel, NEW ShopChivalry))
	//{
	//	return false;
	//}

	return true;
}

// 商店开张
int ShopManagerModule::ExecOpenShop( IKernel* pKernel, const PERSISTID& self )
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 检查所有类型商店是否可开启
	LoopBeginCheck(a);
	for (int i = SHOP_TYPE_NONE + 1; i < SHOP_TYPE_TOTAL; ++i)
	{
		LoopDoCheck(a);
		ShopBase *pShop = m_pArrayShop[i];
		if (NULL != pShop)
		{
			pShop->OpenShop(pKernel, self);
		}
	}

	return 0;
}

/*!
* @brief	处理查询
* @param shop_type	商店类型
* @return int
*/
int ShopManagerModule::ExecQueryShop(IKernel *pKernel, const PERSISTID &self, 
									 const int shop_type)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 合法性验证
	if (shop_type <= SHOP_TYPE_NONE 
		|| shop_type >= SHOP_TYPE_TOTAL)
	{
		return 0;
	}

	// 商店指针
	ShopBase *pShop = GetShop(shop_type);
	if (NULL == pShop)
	{
		return 0;
	}

	// 商店查询
	pShop->QueryShop(pKernel, self);

	return 0;
}

/*!
* @brief	处理商店刷新
* @param shop_type	商店类型
* @return int
*/
int ShopManagerModule::ExecRefreshShop( IKernel *pKernel, const PERSISTID &self, 
									   const int shop_type )
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 合法性验证
	if (shop_type <= SHOP_TYPE_NONE 
		|| shop_type >= SHOP_TYPE_TOTAL)
	{
		return 0;
	}

	// 商店指针
	ShopBase *pShop = GetShop(shop_type);
	if (NULL == pShop)
	{
		return 0;
	}

	// 免费手动刷新
	pShop->HandRefresh(pKernel, self);

	return 0;
}

/*!
* @brief	处理商店付费刷新
* @param vip_buy_type	付费类型
* @return int
*/
int ShopManagerModule::ExecPay2RefreshShop(IKernel *pKernel, const PERSISTID &self, 
										   const int vip_buy_type)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	LoopBeginCheck(b);
	for (int i = SHOP_TYPE_NONE + 1; i < SHOP_TYPE_TOTAL; ++i)
	{
		LoopDoCheck(b);
		// 商店指针
		ShopBase *pShop = m_pArrayShop[i];
		if (NULL == pShop)
		{
			continue;
		}

		// 刷新信息
		const ShopRefreshInfo* refresh_info = pShop->GetShopRefreInfo();
		if (NULL == refresh_info)
		{
			continue;
		}

		// 满足付费类型
		if (vip_buy_type == refresh_info->vip_buy_type)
		{
			// 付费手动刷新
			pShop->HandRefresh(pKernel, self, false);
		}
	}

	return 0;
}

/*!
* @brief	处理不同类型的商店消费
* @param shop_type	商店类型
* @param shop_type	购买的商品索引
* @return int
*/
int ShopManagerModule::ExecShopping( IKernel *pKernel, const PERSISTID &self, 
									const int shop_type, const int buy_index, int buy_amount)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 合法性验证
	if (shop_type <= SHOP_TYPE_NONE 
		|| shop_type >= SHOP_TYPE_TOTAL)
	{
		return 0;
	}

	// 商店指针
	ShopBase *pShop = GetShop(shop_type);
	if (NULL == pShop)
	{
		return 0;
	}

	// 购买商品
	pShop->Buy(pKernel, self, buy_index, buy_amount);

	return 0;
}

// 注册商店相关定时器
int ShopManagerModule::RegistShopResetTimer(IKernel *pKernel, const PERSISTID &self)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 注册商店定时器
	LoopBeginCheck(c);
	for (int i = SHOP_TYPE_NONE + 1; i < SHOP_TYPE_TOTAL; ++i)
	{
		LoopDoCheck(c);
		// 商店指针
		ShopBase *pShop = m_pArrayShop[i];
		if (NULL == pShop)
		{
			continue;
		}

		// 注册
		pShop->RegistResetTimer(pKernel, self);
	}

	return 0;
}

// 等级改变回调
int ShopManagerModule::OnCommandLevelUp(IKernel *pKernel, const PERSISTID &self, 
										const PERSISTID &sender, const IVarList &args)
{
	// 检查是否还有未开张的商店，有则开启
	return m_pShopManagerModule->ExecOpenShop(pKernel, self);
}

// 玩家上线
int ShopManagerModule::OnPlayerRecover(IKernel* pKernel,const PERSISTID& self,
									  const PERSISTID& sender,const IVarList& args)
{
	// 上线检查是否有需要开启的商店
	m_pShopManagerModule->ExecOpenShop(pKernel, self);

	return m_pShopManagerModule->RegistShopResetTimer(pKernel, self);
}

// 客户端消息回调
int ShopManagerModule::OnCustomShop( IKernel* pKernel, const PERSISTID& self, 
									const PERSISTID& sender, const IVarList& args )
{
	// 如果玩家不存在
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 参数合法性检查
	if (args.GetCount() < 3)
	{
		return 0;
	}

	int sub_msg = args.IntVal(1);	// 操作指令
	int shop_type = args.IntVal(2);	// 商店类型

	// 查询商店信息
	if (CS_SUB_SHOP_RETRIEVE == sub_msg)	
	{
		return m_pShopManagerModule->ExecQueryShop(pKernel, self, shop_type);
	}
	// 免费手动刷新商店
	else if (CS_SUB_SHOP_REFRESH == sub_msg)	
	{
		return m_pShopManagerModule->ExecRefreshShop(pKernel, self, shop_type);
	}
	// 购买商品
	else if (CS_SUB_SHOP_BUY == sub_msg)	
	{
		// 玩家指定购买商品的索引
		if (args.GetCount() < 5)
		{
			return 0;
		}

		// 购买
		int buy_index = args.IntVal(3);
		int buy_amount = args.IntVal(4);
		return m_pShopManagerModule->ExecShopping(pKernel, self, shop_type, buy_index, buy_amount);
	}

	return 0;
}

// 客户端购买次数相关消息回调
int ShopManagerModule::OnCustomBuyTimes( IKernel* pKernel, const PERSISTID& self, 
										const PERSISTID& sender, const IVarList& args )
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	if (args.GetCount() < 2)
	{
		return 0;
	}

	// 付费类型
	int vip_buy_type = args.IntVal(1);
	
	// 付费刷新商店
	return m_pShopManagerModule->ExecPay2RefreshShop(pKernel, self, vip_buy_type);
}

void ShopManagerModule::ReloadConfig(IKernel *pKernel)
{
	ShopUtilClass::Instance()->Init(pKernel);
}