//------------------------------------------------------------------------------
// 文件名:       FightDataAccess.cpp
// 内  容:        战斗数据获取
// 说  明:
// 创建日期:    2013年2月16日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/SkillModule/impl/FightDataAccess.h"

#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/exptree.h"
#include "utils/cau.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "../../Define/FightPropertyDefine.h"
#include "FormulaManagerModule.h"
#include "Define/CommandDefine.h"
#include "Define/Fields.h"

bool FightDataAccess::Init(IKernel* pKernel)
{
	pKernel->AddIntCommandHook("player", COMMAND_SKILL_BEFORE_HIT, FightDataAccess::OnCommandBeforeDamge);
    return true;
}


//获取物理攻击命中类型
EHIT_TYPE FightDataAccess::GetHitType(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& target, const PERSISTID& src)
{
    if (!pKernel->Exists(target))
    { 
        return PHY_HIT_TYPE_NORMAL;
    }

	// 获得命中率
    float fHitRate = 0.0f;
	GetFormulaValue(pKernel, self, target, src, FORMULA_PROP_TrueHitRate, fHitRate);

	EHIT_TYPE ePhyHitType = PHY_HIT_TYPE_NORMAL;
	//获得随机数
 	int x = ::util_random_int(100);
    int iMiss = 100 - (int)fHitRate;
	if (x < iMiss)
	{
		ePhyHitType = PHY_HIT_TYPE_MISS;
		return ePhyHitType;
	}

    float fCritRate = 0.0f;
	// 物理暴击率
	GetFormulaValue(pKernel, self, target, src, FORMULA_PROP_TrueCritRate, fCritRate);

	//获得随机数值
	x = ::util_random_int(100);
	if (x < (int)fCritRate)
	{
		ePhyHitType = PHY_HIT_TYPE_VA;
	}

    return ePhyHitType;
}

bool FightDataAccess::GetFormulaValue(IKernel* pKernel,
									  const PERSISTID& self,
									  const PERSISTID& target,
									  const PERSISTID& skill,
									  int iFormulaIndex,
									  float& fValue)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSelfObj || NULL == pTargetObj || NULL == pSkillObj)
	{
		return false;
	}

	if (!FormulaManagerModule::m_instance->GetValue(pSelfObj, pTargetObj, pSkillObj, iFormulaIndex, fValue))
	{
		return false;
	}

	return true;
}

// 响应结算伤害前,计算技能公式系数
int FightDataAccess::OnCommandBeforeDamge(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	PERSISTID skill = args.ObjectVal(1);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (!pKernel->Exists(self) || NULL == pSkillObj)
	{
		return 0;
	}

	const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL == pMainData)
	{
		return 0;
	}

	float fSkillFormula = 0.0f;
	const char* strSkillFormula = pMainData->GetSkillFormula();
	if (!StringUtil::CharIsNull(strSkillFormula))
	{
		ExpTree exp;
		fSkillFormula = exp.CalculateEvent(pKernel, self, skill, PERSISTID(), PERSISTID(), strSkillFormula);
	}
	
	pSkillObj->SetFloat(FIELD_PROP_SKILL_FORMULA, fSkillFormula);
	return 0;
}