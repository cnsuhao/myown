// -------------------------------------------
// 文件名称： ShopGold.cpp
// 文件说明： 元宝商店
// 创建日期： 
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------

#include "ShopGold.h"
#include "ShopUtils.h"
#include "..\ResetTimerModule.h"
#include "Define\SystemInfo_SysFunction.h"

ShopGold::ShopGold()
{
	// 商店类型、表名
	m_shop_type = SHOP_TYPE_GOLD;
	m_shop_rec_name = FIELD_RECORD_SHOP_GOLD_REC;

	// 刷新信息
	//m_refresh_info.vip_buy_type = 0/*VIP_BUY_CHIVALRY_REFRESH*/;
	m_refresh_info.lose_capital_log = FUNCTION_EVENT_ID_SHOP_GOLD_REFRESH;

	// 购买日志
	m_buy_log.lack_capital = SYSTEM_INFO_ID_19005;
	//m_buy_log.pay_failed = STR_SHOP_SYSINFO_DEC_CHIVALRY_FAILED;
	m_buy_log.logSrcFunction = FUNCTION_EVENT_ID_SHOP_GOLD_BUY;
	/*m_buy_log.game_action = LOG_GAME_ACTION_SHOP_CHIVALRY_BUY;*/
}

// 初始化
bool ShopGold::Init(IKernel* pKernel)
{
	// 定时器定义
	DECL_RESET_TIMER(RESET_SHOP_GOLD, ShopGold::ItemResetTimer);
	DECL_RESET_TIMER(RESET_SHOP_GOLD_TIMES, ShopGold::ShopResetTimer);

	return true;
}

// 注册定时器
void ShopGold::RegistResetTimer(IKernel *pKernel, const PERSISTID &self)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 定时器注册
	REGIST_RESET_TIMER(pKernel, self, RESET_SHOP_GOLD);
	REGIST_RESET_TIMER(pKernel, self, RESET_SHOP_GOLD_TIMES);
}

// 物品刷新定时器
int ShopGold::ItemResetTimer(IKernel *pKernel, const PERSISTID &self, int slice)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 自动刷新物品
	ShopUtilClass::Instance()->RefreshShopItem(pKernel, self, SHOP_TYPE_GOLD);

	return 0;
}

// 商店重置定时器
int ShopGold::ShopResetTimer(IKernel *pKernel, const PERSISTID &self, int slice)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 自动重置商店次数
	ShopUtilClass::Instance()->RefreshShopTimes(pKernel, self, SHOP_TYPE_GOLD);

	return 0;
}