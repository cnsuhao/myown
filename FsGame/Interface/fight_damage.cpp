//------------------------------------------------------------------------------
// 文件名:      fight_damage.cpp
// 内  容:      战斗伤害结算相关实现
// 说  明:
// 创建日期:    2014年10月31日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------


#include "FsGame/Interface/FightInterface.h"

#include "FsGame/Define/FightDefine.h"
#include "FsGame/SkillModule/FightModule.h"


// 直接伤害对方
int FightInterface::Damage(IKernel* pKernel,
                           const PERSISTID& self,
                           const PERSISTID& target,
                           const PERSISTID& object,
                           const int64_t uuid,
                           bool bCanTriggerEvent,
                           int iDamageValue)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(target))
    {
        return 0;
    }

    return FightModule::m_pInstance->EventDamageTarget(pKernel,
                                                          self,
                                                          target,
                                                          object, 
                                                          uuid,
                                                          bCanTriggerEvent,
                                                          iDamageValue);
}