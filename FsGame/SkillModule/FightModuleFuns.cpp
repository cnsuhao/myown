//------------------------------------------------------------------------------
// 文件名:       FightDamageCalculator.cpp
// 内  容:        战斗结算流程
// 说  明:
// 创建日期:    2013年2月16日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/SkillModule/FightModule.h"

#include <stdlib.h>
#include "FsGame/define/commanddefine.h"
#include "FsGame/define/servercustomdefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/SkillModule/impl/FightDataAccess.h"
#include "FsGame/SkillModule/impl/SkillAccess.h"
#include "FsGame/SkillModule/impl/SkillStage.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/impl/SkillFlow.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "../Interface/PropertyInterface.h"
#include "FsGame/SkillModule/BufferModule.h"
#include "utils/custom_func.h"
#include "FsGame/Define/GameDefine.h"
#include "Impl/FormulaPropIndexDefine.h"
#include "FightActionControl.h"
#include "FsGame/Interface/FightInterface.h"
#include "FsGame/CommonModule/ActionMutex.h"
//#include "Define/FrontierSceneDefine.h"

bool FightModule::CommandMsgProcess(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                                    const PERSISTID& skill, const int64_t iSkillUUID,
                                    EHIT_TYPE ePhyHitType)
{
    if (!pKernel->Exists(skill))
    {
        return false;
    }

    //命中文字特效，技能事件及buffer事件的命令
    switch (ePhyHitType)
    {
//     case PHY_HIT_TYPE_DODGE:
//         {
//             SkillStageSingleton::Instance()->CustomSkillEffectStageRealTime(pKernel, self,
//                 iSkillUUID, SKILL_AFTER_HIT, target, eSkillStageEffectType_Dodge, CVarList(), EMSG_SEND_TYPE_SELF_TARGET);
//             //闪避事件发生了
// 			pKernel->Command(self, target, CVarList() << COMMAND_SELF_DODGE << self);
// 			pKernel->Command(self, self, CVarList() << COMMAND_BE_DODGE << target);
//             break;
//         }
    case PHY_HIT_TYPE_VA:           // 具有伤害的通知，放在damage中
        {
            //暴击事件
            pKernel->Command(self, self, CVarList() << COMMAND_SELF_VA << target);
            pKernel->Command(self, target, CVarList() << COMMAND_BEVA << self);
            break;
        }
    default:
        break;
    }

    return true;
}

int FightModule::Damage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                         const PERSISTID& skill, const int64_t iSkillUUID,
                         int nTempDamageValue, EHIT_TYPE ePhyHitType)

{
    IGameObj* pTargetObj = pKernel->GetGameObj(target);
    if (NULL == pTargetObj)
    {
        return false;
    }

    CVarList vParams;
    if (pKernel->Exists(skill))
    {
        vParams << skill;
        vParams << iSkillUUID;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (!pSkillMain)
	{
		return false;
	}
	DAMAGE_CATE cate = NONE_DAMAGE_TYPE == pSkillMain->GetDamageType() ? DAMAGE_CATE_DEFAULT : DAMAGE_CATE_SKILL;
	int iRealDamage = RealDamage(pKernel, self, target, nTempDamageValue, ePhyHitType, true, cate, vParams);

    return iRealDamage;
}


void FightModule::CustomSkillDamageMsg(IKernel* pKernel, const PERSISTID& self, 
                                       const PERSISTID& target, int iHPDamage, int iHitType,
                                       const PERSISTID& skill, const int64_t iSkillUUID)
{
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (pSelfObj == NULL || NULL == pTargetObj || NULL == pSkillObj)
	{
		return;
	}

	const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
    ESkillStageEffectType eEffectType = eSkillStageEffectType_None;
    CVarList vEffectParams;

    //发送被伤害的自定义消息
    switch (iHitType)
    {
    case PHY_HIT_TYPE_VA:
        {
            eEffectType = eSkillStageEffectType_VA;
            vEffectParams << iHPDamage << self;
        }
        break;
    default:
        {
            //默认情况下,技能为一般命中
            eEffectType = eSkillStageEffectType_DecHP;
            vEffectParams << iHPDamage << self;
        }
        break;
    }

	// 获取受击类型
	int nHitStartIndex = 0;
	nHitStartIndex = SkillAccessSingleton::Instance()->GetHitIndex(pKernel, pSelfObj, pSkillObj);
	vEffectParams << nHitStartIndex+1;
	
    // 通知客户端技能阶段
    SkillStageSingleton::Instance()->CustomSkillEffectStageRealTime(pKernel, self, iSkillUUID,
        SKILL_HIT_DAMAGE, target, eEffectType, vEffectParams, EMSG_SEND_TYPE_ALL);
}

// 根据伤害计算公式调整伤害值
void FightModule::ModifyDamageByFormula(IKernel* pKernel, const PERSISTID& self,
                                        const PERSISTID& skill, const PERSISTID& target,
                                        EHIT_TYPE ePhyHitType,
                                        float& fDamageValue)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (pSkillMain == NULL)
	{
		return;
	}

    PERSISTID attacker = self;
	float fTempHPDamage = 0.0f;

	int nDamageIndex = FORMULA_PROP_BasePhysAttackDamage;
	int nCritDamgeInex = FORMULA_PROP_CritPhysDamage;
	if (pSkillMain->GetDamageType() == MAGIC_DAMAGE_TYPE)
	{
		nDamageIndex = FORMULA_PROP_BaseMagicAttackDamage;
		nCritDamgeInex = FORMULA_PROP_CritMagicDamage;
	}
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	IGameObj* pAttacker = pKernel->GetGameObj(attacker);
	if (NULL == pTargetObj || NULL == pAttacker)
	{
		return;
	}
	// 霸体减伤率计算 只在PVP时生效
	int nGanitDecRate = 0;
	if (TYPE_PLAYER == pAttacker->GetClassType() && TYPE_PLAYER == pTargetObj->GetClassType())
	{
		int nGaintVal = pTargetObj->QueryInt(FIELD_PROP_GAINT_VAL);
		if (nGaintVal > 0)
		{
			nGanitDecRate = pTargetObj->QueryInt(FIELD_PROP_GAINT_DEC_HURT_RATE);
		}
	}

	//计算最终伤害值
	if ( PHY_HIT_TYPE_NORMAL == ePhyHitType)
	{
		//1. 普通伤害值
		FightDataAccessSingleton::Instance()->GetFormulaValue(pKernel, attacker, target, skill, nDamageIndex, fTempHPDamage);
	}
	//2.暴击伤害
	else  if ( PHY_HIT_TYPE_VA == ePhyHitType) 
	{
		FightDataAccessSingleton::Instance()->GetFormulaValue(pKernel, attacker, target, skill, nCritDamgeInex, fTempHPDamage);
	}

	// 获取多次命中的伤害系数
	float fExtraHitDamageRatio = 1.0f;
	SkillAccessSingleton::Instance()->GetExtraHitDamageRatio(pKernel, self, skill, fExtraHitDamageRatio);
	
	fDamageValue = fTempHPDamage * fExtraHitDamageRatio;
	// 霸体减伤处理
	if (nGanitDecRate > 0)
	{
		fDamageValue = fDamageValue * (1 - (float)nGanitDecRate / 10000);
	}
}

//伤害对方
int FightModule::RealDamage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                            int iHPDamage, int iHitType,
                            bool bIsDamageEvent, int iDamageCate,
                            const IVarList& params)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(target))
    {
        return 0;
    }

    // 获取真正收到伤害的对象
    PERSISTID msgTarget = target;
    PERSISTID hurtTarget = target;
    if (!pKernel->Exists(hurtTarget))
    {
        hurtTarget = target;
    }

    IGameObj* pTargetObj = pKernel->GetGameObj(hurtTarget);
    if (NULL == pTargetObj)
    {
        return 0;
    }

    //已经死了
    int dead = 0;
    if (FPropertyInstance->GetDead(pTargetObj, dead) && dead > 0)
    {
        return 0;
    }

    // 检测伤害值
    if (iHPDamage <= 0)
    {
        return 0;
    }

    int64_t iOldHP = 0;
    if (!FPropertyInstance->GetHP(pTargetObj, iOldHP))
    {
        return 0;
    }

	// 免疫NPC攻击,只有玩家能打的处理
// 	int nGaintNPCAttack = 0;
// 	if (FPropertyInstance->GetGaintNPCAttack(pTargetObj, nGaintNPCAttack) && nGaintNPCAttack == 1 && pKernel->Type(self) != TYPE_PLAYER)
// 	{
// 		return 0;
// 	}

    // object可能是skill 或buffer
    PERSISTID object = params.ObjectVal(0);
    int64_t uuid = params.Int64Val(1);    

    if (iOldHP <= 0)
    {
		KillTarget(pKernel, self, hurtTarget, object, uuid, iDamageCate);
        return 0;
    }

	// 玩家正在使用轻功,不会受到技能伤害
	if (pTargetObj->GetClassType() == TYPE_PLAYER && DAMAGE_CATE_SKILL == iDamageCate && pTargetObj->QueryInt(FIELD_PROP_JUMP_SEC) > 0)
	{
		return 0;
	}

    //即将给目标造成伤害
    pKernel->Command(self, self, CVarList() << COMMAND_BEFORE_DAMAGETARGET << hurtTarget << iHPDamage);

	// 发送扣血之前的消息
	pKernel->Command(self, target, CVarList() << COMMAND_BEFORE_BEDAMAGE << iHPDamage);

	// 对减免伤害进行处理
	int nAbsorbDamageRate = 0;
	if (!FPropertyInstance->GetAbsorbDamageRate(pTargetObj, nAbsorbDamageRate))
	{
		return 0;
	}
	if (nAbsorbDamageRate > 0)
	{
		float fAdRate = (float)nAbsorbDamageRate / 100.0f;
		iHPDamage = (int)((float)iHPDamage * (1 - fAdRate));
	}
	
	FPropertyInstance->SetHP(pTargetObj, iOldHP-iHPDamage);

    //被伤害后的HP
    int64_t iCurHP = 0;
    FPropertyInstance->GetHP(pTargetObj, iCurHP);
    // 真正造成的伤害
    int realdamage = (int)(iOldHP - iCurHP);

    //发送伤害事件Command
    if (bIsDamageEvent) // 防止伤害的回调事件中又调这个函数，导致死循环，堆栈溢出
    {
        if (pKernel->Exists(self) && pKernel->Exists(hurtTarget))
        {
            //伤害事件发生了
            pKernel->Command(self, self, CVarList() << COMMAND_DAMAGETARGET << hurtTarget << iHPDamage << iHitType);

            //被伤害事件发生了
            pKernel->Command(self, hurtTarget, CVarList() << COMMAND_BEDAMAGE << iHPDamage << iDamageCate << object);
        }
    }

    //向客户端发送减血消息
    if (iHPDamage > 0)
    {
        //如果是技能伤害，发送扣血消息
        if (DAMAGE_CATE_SKILL == iDamageCate)
        {
            CustomSkillDamageMsg(pKernel, self, msgTarget, iHPDamage, iHitType, object, uuid);
        }
		else if (DAMAGE_CATE_BUFFER == iDamageCate  || DAMAGE_CATE_DEFAULT == iDamageCate)
		{
			::CustomDisplayStage(pKernel, msgTarget, msgTarget, CUSTOM_DEC_HP_WITH_EFFECT, CVarList() << iHPDamage, true);
		}
    }

    //挂掉了
    if (iCurHP <= 0)
    {
		KillTarget(pKernel, self, hurtTarget, object, uuid, iDamageCate);
    }

    return realdamage;
}


//杀死对方
void FightModule::KillTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& object, const int64_t uuid, int nDamageCate)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pTargetObj = pKernel->GetGameObj(target);
    if (NULL == pSelfObj || NULL == pTargetObj)
    {
        return;
    }

    // 如果目标死亡，不能重复被杀死
    int dead = 0;
    if (!FPropertyInstance->GetDead(pTargetObj, dead) || dead > 0)
    {
        return ;
    }

	// 切磋中不死亡
	if (ActionMutexModule::IsThisAction(pKernel, self, ACTION_INFORMAL_PVP))
	{
		FPropertyInstance->SetHP(pTargetObj, 0);
		return;
	}
   
	FPropertyInstance->SetDead(pTargetObj, 1);
	FPropertyInstance->SetHP(pTargetObj, 0);

    //被杀死事件发生
    if (pKernel->Exists(self) && pKernel->Exists(target))
    {
		// 宠物
		if (pSelfObj->GetClassType() == TYPE_NPC && (0 == strcmp(pSelfObj->GetScript(), "PetNpc")))
		{
			PERSISTID master = pSelfObj->QueryObject("Master");
			if (pKernel->Exists(master))
			{
				//杀死事件发生
				pKernel->Command(master, target, CVarList() << COMMAND_BEKILL << object << nDamageCate);  
				pKernel->Command(master, master, CVarList() << COMMAND_KILLTARGET << target << object << self);
			}			
		}
		else
		{
			//杀死事件发生
			pKernel->Command(self, target, CVarList() << COMMAND_BEKILL << object << nDamageCate);  
			pKernel->Command(self, self, CVarList() << COMMAND_KILLTARGET << target << object);
		}
    }
    else
    {
        ::extend_warning(LOG_INFO, "[FightModule::KillTarget] no send COMMAND_BEKILL self:%d-%d target:%d-%d",
            self.nIdent, self.nSerial, target.nIdent, target.nSerial);
    }

	// 只有技能造成的死亡才发消息
	if (DAMAGE_CATE_SKILL == nDamageCate)
	{
		PERSISTID killer = self;
		// 宠物
		if (pSelfObj->GetClassType() == TYPE_NPC && (0 == strcmp(pSelfObj->GetConfig(), "PetNpc")))
		{
			killer = get_sceneobj_master(pKernel, self);
		}
		//SkillStageSingleton::Instance()->CustomSkillHitFly(pKernel, killer, target, object);
	}

    return;
}

// 特殊NPC伤害处理
// bool FightModule::ModifyDamageBySpecialState(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, int& nOutDamageValue)
// {
// 	IGameObj* pGameSelf = pKernel->GetGameObj(self);
// 	IGameObj* pGameTarget = pKernel->GetGameObj(target);
// 	if (NULL == pGameSelf || NULL == pGameTarget)
// 	{
// 		return false;
// 	}
// 
// 	bool bModify = false;
// 	
// 	// 是否只受到固定伤害
// 	int nFixBeDamaged = pGameTarget->QueryInt("FixBeDamaged");
// 	if (nFixBeDamaged > 0)
// 	{
// 		nOutDamageValue = nFixBeDamaged;
// 		bModify = true;
// 	}
// 
// 	return bModify;
// }

// 伤害目标前
bool FightModule::BeforeDamgeTarget( IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const PERSISTID& target,
					   const int64_t iSkillUUID, int& nOutHitType, int& nOutDamageVal) 
{
	if (!pKernel->Exists(self) || !pKernel->Exists(skill) || !pKernel->Exists(target))
	{
		return false;
	}

	if (!CanDamageTarget(pKernel, self, target))
	{
		return false;
	}
	
	// 计算命中类型
	EHIT_TYPE ePhyHitType = FightDataAccessSingleton::Instance()->GetHitType(pKernel, self, target, skill);
	const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pMainData)
	{
		return false;
	}
	
	nOutHitType = (int)ePhyHitType;
	nOutDamageVal = 0;
	// Miss了就不走伤害计算了
	if (PHY_HIT_TYPE_MISS == ePhyHitType)
	{
		MissSkillDamageTargetHandle(pKernel, self, target, skill, iSkillUUID);
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 命中事件消息处理
	CommandMsgProcess(pKernel, self, target, skill, iSkillUUID, ePhyHitType);

	// 是否攻击特殊的NPC	 后结算
// 	if (ModifyDamageBySpecialState(pKernel, self, target, nOutDamageVal))
// 	{
// 		return true;
// 	}

	//获取伤害公式的伤害值
	float fTempDamageValue = 0.0f;
	// 伤害值修正 
	ModifyDamageByFormula(pKernel, self, skill, target, ePhyHitType, fTempDamageValue);

	// 伤害值不能小于1
	nOutDamageVal = __max(1, (int)fTempDamageValue);
	return true;
}