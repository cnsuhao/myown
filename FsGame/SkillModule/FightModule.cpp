
//------------------------------------------------------------------------------
// 文件名:       FightModule.cpp
// 内  容:        战斗系统接口
// 说  明:
// 创建日期:    2013年2月17日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------


#include "FsGame/SkillModule/FightModule.h"

#include "FsGame/define/CommandDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "utils/util_func.h"
#include "FsGame/SkillModule/impl/SkillStage.h"
#include "FsGame/SkillModule/impl/SkillAccess.h"
#include "FsGame/SkillModule/impl/FightDataAccess.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/SkillModule/BufferModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/Interface/PropertyInterface.h"
#include "public/Converts.h"
#include "FightActionControl.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "PKModelModule.h"
//#include "Define/PKModelDefine.h"
//#include "PKModelModule.h"
//#include "Impl/PVPRecAttacker.h"

FightModule* FightModule::m_pInstance = NULL;

//初始化
bool FightModule::Init(IKernel* pKernel)
{
    m_pInstance = this;

    if (!FightDataAccessSingleton::Instance()->Init(pKernel))
    {
        ::extend_warning(LOG_ERROR, "[FightModule::Init] sub module init error.");
        return false;
    }

	if (!FightActionControlSingleton::Instance()->Init(pKernel))
	{
		return false;
	}

// 	if (!PVPRecAttackerSingleton::Instance()->Init(pKernel))
// 	{
// 		return false;
// 	}

    return true;
}

//释放
bool FightModule::Shut(IKernel* pKernel)
{
    return true;
}

// 根据战斗规则公式给对方造成伤害
bool FightModule::SkillDamageTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                                    const PERSISTID& skill, const int64_t iSkillUUID)
{
	int nHitType = 0;
	int nDamageVal = 0;
	if (!BeforeDamgeTarget(pKernel, self, skill, target, iSkillUUID, nHitType, nDamageVal))
	{
		return false;
	}

    // 造成伤害
	int nRealDam = Damage(pKernel, self, target, skill, iSkillUUID, nDamageVal, (EHIT_TYPE)nHitType);

	//执行命中后事件
	SkillEventManager::SkillHitDamage(pKernel, self, skill, iSkillUUID, target, nRealDam);
    return true;
}

// Miss状态下的处理方式
void FightModule::MissSkillDamageTargetHandle(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill, const int64_t iSkillUUID)
{
	ESkillStageEffectType effectType = eSkillStageEffectType_Miss;

	// 通知客户端技能阶段
    SkillStageSingleton::Instance()->CustomSkillEffectStageRealTime(pKernel, self, 
        iSkillUUID, SKILL_AFTER_HIT, target, effectType, CVarList() << self, EMSG_SEND_TYPE_SELF_TARGET);

	//攻击未命中
	pKernel->Command(self, self, CVarList() << COMMAND_HIT_MISS << target);
	return ;
}


// 事件伤害处理专用接口，防止因伤害消息触发伤害事件函数进入无限循环
bool FightModule::EventDamageTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                                    const PERSISTID& object, const int64_t uuid, bool bCanTriggerEvent,
                                    int iDamageValue)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || NULL == pTargetObj)
	{
		return false;
	}

	if (!CanDamageTarget(pKernel, self, target))
	{
		return false;
	}

	if (pSelfObj->QueryInt("CantAttack") > 0)
	{
		return false;
	}

    if (iDamageValue <= 0)
    {
        iDamageValue = 1; // 最小伤害为1
    }

	// 是否为建筑攻击伤害
// 	int nBuildDamageValue = 0;
// 	if (ModifyDamageByFrontierBuild(pKernel, self, target, nBuildDamageValue))
// 	{
// 		iDamageValue = nBuildDamageValue;
// 	}
// 	else
// 	{
// 		// 是否固定被伤害
// 		int nFixBeDamage = pTargetObj->QueryInt("FixBeDamaged");
// 		if (nFixBeDamage > 0)
// 		{
// 			iDamageValue = nFixBeDamage;
// 		}
// 	}
	
    const char* szObjectScript = "";
    CVarList vParams;
    if (pKernel->Exists(object))
    {
        vParams << object;
        vParams << uuid;
        szObjectScript = pKernel->GetScript(object);
    }

    DAMAGE_CATE eDamageCate = DAMAGE_CATE_DEFAULT;
    if (strcmp(szObjectScript, AMBER_BUFFER) == 0)
    {
        eDamageCate = DAMAGE_CATE_BUFFER;
    }
    else if (strcmp(szObjectScript, AMBER_SKILL_NORMAL) == 0)
    {
		eDamageCate = DAMAGE_CATE_SKILL;
    }

    RealDamage(pKernel, self, target, iDamageValue, PHY_HIT_TYPE_NORMAL, bCanTriggerEvent, eDamageCate, vParams);
    return true;
}

bool FightModule::SkillEventDamageTarget(IKernel* pKernel, 
										const PERSISTID& self,
										const PERSISTID& target,
										const PERSISTID& skill,
										const int64_t iSkillUUID,
										const float fRate)
{
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (pTargetObj == NULL)
	{
		return false;
	}

	int nHitType = 0;
	int nDamageVal = 0;
	if(!BeforeDamgeTarget(pKernel, self, skill, target, iSkillUUID, nHitType, nDamageVal))
	{
		return false;
	}

	float fTempDamageValue = (float)nDamageVal;

	// 是否固定被伤害
	if (pTargetObj->QueryInt("FixBeDamaged") <= 0)
	{
		// 按百分比修正伤害值
		fTempDamageValue *= fRate;
	}

	int iHPDamage = 0;
	if (fTempDamageValue > 0 && fTempDamageValue < 1.01f)
	{ // 防止计算结果小于1时伤害值是0
		iHPDamage = 1;
	}
	else
	{
		iHPDamage = (int)fTempDamageValue; 
	}

    RealDamage(pKernel, self, target, iHPDamage, nHitType, true, DAMAGE_CATE_SKILL, CVarList() << skill << iSkillUUID);
    
    return true;
}

bool FightModule::CanDamageTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}

	// 暂定不能伤害自己
	if (self == target)
	{
		return false;
	}

	int dead = 0;
	if (!FPropertyInstance->GetDead(pSelfObj, dead) || dead > 0)
	{
		return false;
	}

	if (!FPropertyInstance->GetDead(pTargetObj, dead) || dead > 0)
	{
		return false;
	}

	int iCantBeAttack = 0;
	if (FPropertyInstance->GetCantBeAttack(pTargetObj, iCantBeAttack) && iCantBeAttack > 0)
	{
		return false;
	}

	int iInvisible = pTargetObj->QueryInt("Invisible");
	if (iInvisible > 0)
	{
		return false;
	}

	int iCantAttack = 0;
	if (FPropertyInstance->GetCantAttack(pSelfObj, iCantAttack) && iCantAttack > 0)
	{
		return false;
	}



	if (pTargetObj->GetClassType() == TYPE_NPC)
	{
		int iNpcType = pTargetObj->QueryInt("NpcType");
		if (iNpcType >= NPC_TYPE_FUNCTION)
		{
			return false;
		}
	}

	return PKModelModule::IsBeAttacked(pKernel, self, target);
}