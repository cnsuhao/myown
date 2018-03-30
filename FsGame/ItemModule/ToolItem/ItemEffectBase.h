//--------------------------------------------------------------------
// 文件名:      ItemEffectBase.h
// 内  容:      物品效果基类
// 说  明:		所有的物品的效果都继承此基类
// 创建日期:    2016年3月4日
// 创建人:      
//    :       
//--------------------------------------------------------------------
#ifndef _ITEM_EFFECT_BASE_H_
#define _ITEM_EFFECT_BASE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ToolItemDefine.h"

// 所有技能事件基类
class ItemEffectBase
{
public:
	ItemEffectBase();
	virtual ~ItemEffectBase();

	// 创建物品效果
	static ItemEffectBase* CreateItemEffect(const ItemEffectType nEffectType);

	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 尝试应用, 失败返回false
	virtual bool TryApplyItemEffect(IKernel* pKernel, const PERSISTID& user, const PERSISTID& item, const ItemEffectParam& effect_entry);

	// 把物品的效果应用到玩家身上
	virtual bool ApplyItemEffect(IKernel* pKernel, const PERSISTID& user, const PERSISTID& item, const ItemEffectParam& effect_entry, IVarList& showItems) = 0;
};

#endif