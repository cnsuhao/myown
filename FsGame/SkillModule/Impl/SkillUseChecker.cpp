//--------------------------------------------------------------------
// 文件名:      Server\FsGame\SkillModule\impl\skillUseChecker.h
// 内  容:      检查技能使用的限制条件的实现函数全部定义于此
// 说  明:
// 创建日期:	2014年10月31日
// 创建人:		 
//    :       
//--------------------------------------------------------------------
#include "FsGame/SkillModule/impl/SkillUseChecker.h"

#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/Define/CoolDownDefine.h"
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "FsGame/SkillModule/impl/SkillAccess.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "../../Interface/PropertyInterface.h"
#include "SystemFunctionModule/EnergyModule.h"

//是否可对某对象主动使用该技能（返回1是正确的能正常使用该技能, 其它值是错误码）
int SkillUseChecker::CanUseSkill(IKernel* pKernel, const PERSISTID& self, 
                                 float x, float y, float z, float orient, const PERSISTID& skill,
                                 const PERSISTID& target, float dx, float dy, float dz)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

    // 检查技能是否已经在冷却时间里面
	if(SkillAccessSingleton::Instance()->IsCoolDown(pKernel, self, skill))
	{
		return USESKILL_RESULT_ERR_COOLDOWN;
	}

	// 检查是否在公共冷却中
	if (CoolDownModule::m_pInstance->IsCoolDown(pKernel, self, COOLDOWN_CATEGOTYID_SKILL))
	{
		return USESKILL_RESULT_ERR_PUBLICCD;
	}

    // 检测射程
    int result = CheckCastRange(pKernel, self, target, skill);
    if (USESKILL_RESULT_SUCCEED != result)
    {
        return result;
    }

	// 特殊的限制技能释放的属性判断
	result = HasPropertyLimit(pKernel, self, target, skill);
    return result;
}

// 特殊的限制技能释放的属性判断
int SkillUseChecker::HasPropertyLimit(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

    IGameObj* pTargetObj = pKernel->GetGameObj(target);
    if (NULL != pTargetObj && self != target)
    {
        if (pTargetObj->FindAttr("CantBeAttack") && pTargetObj->QueryInt("CantBeAttack") > 0)
        {
            return USESKILL_RESULT_ERR_CANTBEATTACK;
        }
    }

	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (pSkillObj == NULL)
	{
		return USESKILL_RESULT_ERR_NOSKILL;
	}

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pSkillMain)
	{
		return USESKILL_RESULT_ERR_CONFIG_DATA;
	}

    //是否已经死亡
    if (!pSelfObj->FindAttr(FIGHTER_DEAD) || (pSelfObj->QueryInt(FIGHTER_DEAD) != 0))
    {
        //暂时不能使用技能
        return USESKILL_RESULT_ERR_DEAD;
    }

    //自身属性的判断，是否可以使用技能
    if (pSelfObj->FindAttr("CantUseSkill") && pSelfObj->QueryInt("CantUseSkill") > 0)
    {
        //能否使用技能
        return USESKILL_RESULT_ERR_CANTUSESKILL;
    }

	// 检查蓝够不够
	if (!EnergyModule::IsMPEnough(pKernel, self, pSkillMain->GetConsumeMP()) )
	{
		return USESKILL_RESULT_ERR_MP;
	}

	// 处于缴械状态,不能普通攻击
	if (pSelfObj->QueryInt(FIELD_PROP_CANT_NORMAL_ATTACK) > 0 && pSkillMain->GetIsNormalAttack())
	{
		return USESKILL_RESULT_ERR_CANT_NORMAL_ATTACK;
	}

	// 检查沉默状态,沉默中不能释放技能,对沉默无效的技能除外
	int nSilent = 0;
	if (FPropertyInstance->GetSilent(pSelfObj, nSilent) && nSilent > 0 && !pSkillMain->GetIsUnSilent())
	{
		return USESKILL_RESULT_ERR_SILENT;
	}

	// 检查蓄力技能的次数
// 	if (pSkillMain->GetIsStorage() && pSkillObj->QueryInt("StorageNum") <= 0)
// 	{
// 		return USESKILL_RESULT_ERR_STORAGE_NUM;
// 	}
// 
// 	// 硬直状态检测
// 	if (!pSkillMain->GetIsUseInStiff() && pSelfObj->QueryInt("StiffState")> 0)
// 	{
// 		return USESKILL_RESULT_ERR_STIFF;
// 	}

    return USESKILL_RESULT_SUCCEED;
}

// 射程判断
int SkillUseChecker::CheckCastRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill)
{
    if (!pKernel->Exists(skill))
    {
        return USESKILL_RESULT_ERR_NOSKILL;
    }

    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

    IGameObj* pTargetObj = pKernel->GetGameObj(target);
    if (NULL == pTargetObj)
    {
        return USESKILL_RESULT_SUCCEED;
    }

    // 瞬发不需要判断射程
    if (SkillAccessSingleton::Instance()->GetSkillSelectType(pKernel, skill) == SKILL_SELECT_TYPE_DEFAULT)
    {
        return USESKILL_RESULT_SUCCEED;
    }
    // 获取射程参数判断射程
    SkillCastRangeParams skillCastRangeParams;
    bool ret = SkillAccessSingleton::Instance()->GetSkillCastRange(pKernel, self, skill, skillCastRangeParams, NULL);
    if (!ret)
    {
        return USESKILL_RESULT_ERR_OTHER;
    }

    float x, y, z, orient;
    pKernel->GetLocation(self, x, y, z, orient);
    float dx, dy, dz, dorient;
    pKernel->GetLocation(target, dx, dy, dz, dorient);
    // 判断目标点是否在有效距离内
    float dist2d = ::util_dot_distance(x, z, dx, dz);
    if (dist2d < skillCastRangeParams.fRangeDistMin)
    {
        return TARGETFILTER_RESULT_ERR_TOONEAR;
    }
    else if (dist2d > skillCastRangeParams.fRangeDistMax)
    {
        return TARGETFILTER_RESULT_ERR_TOOFAR;
    }

    return USESKILL_RESULT_SUCCEED;
}

bool SkillUseChecker::CheckSkillRelation(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const PERSISTID& target)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(skill))
    {
        return false;
    }

    if (!pKernel->Exists(target))
    {
        return false;
    }

    // 获取技能配置
    int iTargetRelationType = HIT_TARGET_RELATION_TYPE_NONE;
    int iTargetClassType = HIT_TARGET_TYPE_NONE;
    if (!SkillAccessSingleton::Instance()->GetSkillHitTargetRelation(pKernel, skill, iTargetRelationType, iTargetClassType))
    {
        return false;
    }

    return USESKILL_RESULT_SUCCEED == TargetFilter::SatisfyRelation(pKernel, self, target, iTargetRelationType, iTargetClassType);
}

// 判断技能高绩效包对象关系
bool SkillUseChecker::CheckEventEffectRelation(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const char* szSkillEffectPackID)
{
    int iTargetRelationType = HIT_TARGET_RELATION_TYPE_NONE;
    int iTargetClassType = HIT_TARGET_TYPE_NONE;

    if (HIT_TARGET_RELATION_TYPE_NONE != iTargetRelationType && HIT_TARGET_TYPE_NONE != iTargetClassType)
    {
        int iRelationResult = TargetFilter::SatisfyRelation(pKernel, self, target, iTargetRelationType, iTargetClassType);
        if (TARGETFILTER_RESULT_SUCCEED != iRelationResult)
        {
            return false;
        }
    }

    return true;
}
