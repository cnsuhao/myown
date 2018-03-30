//--------------------------------------------------------------------
// 文件名:      ItemEffectBase.h
// 内  容:      物品效果基类
// 说  明:		所有的技能事件都继承此基类
// 创建日期:    2016年3月4日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "ItemEffectBase.h"
// #include "ItemEffectIncExp.h"
// #include "ItemEffectIncCapital.h"
// #include "ItemEffectAddRide.h"
// #include "ItemEffectAddRideSkin.h"
// #include "ItemEffectGift.h"
// #include "ItemEffectDine.h"
// #include "ItemEffectIncBuffer.h"
// #include "ItemEffectIncProp.h"
// #include "ItemEffectIncHP.h"
// #include "ItemEffectAddCloneCoin.h"
// #include "ItemEffectRemoveBuff.h"
// #include "ItemEffectVip.h"
// #include "ItemEffectWelfareCard.h"
// #include "ItemEffectIncFriendship.h"
// #include "ItemEffectMultiExpRate.h"
// #include "ItemEffectUnlockFashion.h"
// #include "ItemEffectRewardPack.h"
// #include "ItemEffectAcceptTask.h"
// #include "ItemEffectAddSkill.h"

ItemEffectBase::ItemEffectBase()
{

}

ItemEffectBase::~ItemEffectBase()
{

}

// 创建物品效果
ItemEffectBase* ItemEffectBase::CreateItemEffect(const ItemEffectType nEffectType)
{
	ItemEffectBase *pEffectBase = NULL;
// 	switch(nEffectType)
// 	{
// 	case ITEM_EFFECT_EXP:
// 		pEffectBase = NEW ItemEffectIncExp;
// 		break;
// 	case ITEM_EFFECT_CAPITAL:
// 		pEffectBase = NEW ItemEffectIncCapital;
// 		break;
// 	case ITEM_EFFECT_RIDE:
// 		pEffectBase = NEW ItemEffectAddRide;
// 		break;
// 	case ITEM_EFFECT_RIDE_SKIN:
// 		pEffectBase = NEW ItemEffectAddRideSkin;
// 		break;
// 	case ITEM_EFFECT_GIFT:
// 		pEffectBase = NEW ItemEffectGift;
// 		break;
// 	case ITEM_EFFECT_DINE:
// 		pEffectBase = NEW ItemEffectDine;
// 		break;
// 	case ITEM_EFFECT_BUFFER:
// 		pEffectBase = NEW ItemEffectIncBuffer;
// 		break;
// 	case ITEM_EFFECT_PROP:
// 		pEffectBase = NEW ItemEffectIncProp;
// 		break;
// 	case ITEM_EFFECT_HP:
// 		pEffectBase = NEW ItemEffectIncHP;
// 		break;
// 	case Item_EffECT_ADD_CLONE_COIN:
// 		pEffectBase = NEW ItemEffectAddCloneCoin;
// 		break;
// 	case ITEM_EFFECT_REMOVE_BUFF:
// 		pEffectBase = NEW ItemEffectRemoveBuff;
// 		break;
// 	case ITEM_EFFECT_VIP_CARD:
// 		pEffectBase = NEW ItemEffectVip;
// 		break;
// 	case ITEM_EFFECT_WELFARE_CARD:
// 		pEffectBase = NEW ItemEffectWelfareCard;
// 		break;
// 	case ITEM_EFFECT_FRIENDSHIP:
// 		pEffectBase = NEW ItemEffectIncFriendship;
// 		break;
// 	case ITEM_EFFECT_MULTI_EXP_RATE:
// 		pEffectBase = NEW ItemEffectMultiExpRate;
// 		break;
// 	case ITEM_EFFECT_UNLOCK_FASHION:
// 		pEffectBase = NEW ItemEffectUnlockFashion;
// 		break;
// 	case ITEM_EFFECT_REWARD_PACK:										  
// 		pEffectBase = NEW ItemEffectRewardPack;
// 		break;
// 	case ITEM_EFFECT_ACCEPT_RECOVERT_TASK:
// 		pEffectBase = NEW ItemEffectAcceptTask;
// 		break;
// 	case ITEM_EFFECT_ADD_SKILL:
// 		pEffectBase = NEW ItemEffectAddSkill;
// 		break;
// 	default:
// 		break;
//	}

	return pEffectBase;
}

// 初始化
bool ItemEffectBase::OnInit(IKernel* pKernel)
{
	return true;
}

// 尝试应用, 失败返回false
bool ItemEffectBase::TryApplyItemEffect(IKernel *pKernel, const PERSISTID &user, const PERSISTID& item, const ItemEffectParam& effect_entry)
{
	return true;
}