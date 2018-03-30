//--------------------------------------------------------------------
// 文件名:      AIRChaseTargetBase.h
// 内  容:      基本追击规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRChaseTargetBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "utils/util_func.h"
#include <math.h>
#include "FsGame/Interface/FightInterface.h"
#include "FsGame/SystemFunctionModule/MotionModule.h"
#include "FsGame/Define/Fields.h"
#include "FsGame/CommonModule/EnvirValueModule.h"

AIRChaseTargetBase::AIRChaseTargetBase()
{
	m_ruleType = AI_RULE_CHASE_TARGET;
	m_subRuleCode = AIRS_CHASE_TARGET_BASE;
}

//是不是在技能范围内
int AIRChaseTargetBase::InSkillRange(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	float dist = pKernel->Distance2D(self, sender);
	//碰撞半径
	if (dist < pSelfObj->QueryFloat("MinAttackDist"))
	{
		return AI_RANGE_NEAR;
	}
	else if (dist > pSelfObj->QueryFloat("MaxAttackDist"))
	{
		return AI_RANGE_OVER;
	}
	return AI_RANGE_OK;
}

int AIRChaseTargetBase::DoRule(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args, AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_IGNORE;
    }

	//是不是还在战斗
	if (pSelf->QueryInt("CurState") != AI_STATE_FIGHT)
	{
		return AI_RT_IGNORE;
	}

	//如果不可行走
 	if ( pSelf->QueryInt("CantMove") > 0)
 	{
 		return AI_RT_IGNORE;
 	}

	//得到当前攻击目标
	PERSISTID curTarget = pSelf->QueryObject("AITargetObejct");
	if (!pKernel->Exists(curTarget))
	{
		return AI_RT_IGNORE;
	}

	if (!IsCanChasePlayer(pKernel, curTarget))
	{
		return AI_RT_IGNORE;
	}


    // 正在释放技能
// 	if (USESKILL_RESULT_SUCCEED != FightInterfaceInstance->IsSkillUseBusy(pKernel, self))
// 	{
// 		// 持续施法的要处理追击
// 		if(USESKILL_RESULT_ERR_LEADING_BUSY == FightInterfaceInstance->IsSkillUseBusy(pKernel, self))
// 		{
// 			USOperationState type = USOperationState(FightInterfaceInstance->GetCurUSOperationState(pKernel, self));
// 			if(type == USO_CHANGE_ORIENT)
// 			{
// 				pKernel->Rotate(self, pKernel->Angle(self, curTarget), PI2);
// 			}
// 			else if(type == USO_CAN_MOTION)
// 			{
//                 //在技能攻击中追击
// 				AIFunction::StartMoveToObject(pKernel, self, curTarget, 0, 2);
// 			}
// 		}
// 		return AI_RT_IGNORE;
// 	}
	
	//是否在攻击范围内
	int iIn = InSkillRange(pKernel, self, curTarget);

    float runSpeed = pSelf->QueryFloat("RunSpeed");

	if(iIn == AI_RANGE_OK || iIn == AI_RANGE_NEAR)
	{
		//正好
        if (!FloatEqual(runSpeed, 0.0f))
        {
		    MotionModule::MotionStop(pKernel, self);
        }
		return AI_RT_SUCCESS;
	}

	//不能移动，塔类
	if (FloatEqual(runSpeed, 0.0f))
	{
		return AI_RT_IGNORE;
	}
	
	float minDistance = pSelf->QueryFloat("MinAttackDist");
	float maxDistance = pSelf->QueryFloat("MaxAttackDist");

	////距离太远,跑向目标
	if (AIFunction::StartMoveToObject(pKernel, self, curTarget, minDistance, maxDistance) == AI_RT_FAIL)
	{
		return AI_RT_END_FIGHT;    
	}
	return AI_RT_IGNORE;
}

// 是否能追击玩家
bool AIRChaseTargetBase::IsCanChasePlayer(IKernel * pKernel, const PERSISTID& target)
{
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (NULL == pTargetObj)
	{
		return false;
	}
	// 不是玩家肯定能追击
	if (pTargetObj->GetClassType() != TYPE_PLAYER)
	{
		return true;
	}

#ifndef FSROOMLOGIC_EXPORTS
	// 玩家使用轻功不能追击
// 	if (FightInterfaceInstance->IsInFlySkill(pKernel, target))
// 	{
// 		return false;
// 	}
#endif // !FSROOMLOGIC_EXPORTS

	return true;
}