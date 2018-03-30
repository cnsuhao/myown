// -------------------------------------------
// 文件名称： ShopGold.h
// 文件说明： 元宝商店
// 创建日期： 
// 创 建 人：  tongzt
// 修改人：
// -------------------------------------------
#ifndef _ShopGold_H_
#define _ShopGold_H_

#include "ShopBase.h"

class ShopGold : public ShopBase
{
public:
	ShopGold();

public:


public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 注册定时器
	virtual void RegistResetTimer(IKernel *pKernel, const PERSISTID &self);

	// 物品刷新定时器
	static int ItemResetTimer(IKernel *pKernel, const PERSISTID &self, int slice);

	// 商店重置定时器
	static int ShopResetTimer(IKernel *pKernel, const PERSISTID &self, int slice);

};

#endif