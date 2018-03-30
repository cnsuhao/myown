//--------------------------------------------------------------------
// 文件名:      AIRSpringBase.cpp
// 内  容:      基础触发规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRSpringBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/Interface/FightInterface.h"
#include <time.h>
#include "FsGame/Define/Classes.h"

AIRSpringBase::AIRSpringBase()
{
	m_ruleType = AI_RULE_SPRING;
	m_subRuleCode = AIRS_SPRING_BASE;
}

int AIRSpringBase::DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
	IGameObj * selfObj = pKernel->GetGameObj(self);
	IGameObj *pSender = pKernel->GetGameObj(sender);
	if (selfObj == NULL || pSender == NULL)
	{
		return AI_RT_IGNORE;
	}

	// 验证是否为能伤害目标
	if (!FightModule::m_pInstance->CanDamageTarget(pKernel, self, sender))
	{
		return AI_RT_IGNORE;
	}
    
    //如果在安全区就不触发
    if(pSender->FindData("SafeAreaFlag"))
    {
        return AI_RT_IGNORE;
    }

	//死亡，逃跑状态不触发。
	int curState = selfObj->QueryInt("CurState");
	if (curState == AI_STATE_DIED || curState == AI_ESCAPE)
	{
		return AI_RT_IGNORE;
	}

	//主动或被动，被动不触发
	int iInitiative = selfObj->QueryInt("Initiative");
	if (iInitiative == 0)
	{
		return AI_RT_IGNORE;
	}

	//把触发对象记录到AttackerList
    IRecord *pAttackerList = selfObj->GetRecord("AttackerList");
    if(pAttackerList == NULL)
    {
        return AI_RT_NULL;
    }
	int row = pAttackerList->FindObject(AI_ATTACKER_REC_OBJECT, sender);
	if (row < 0)
	{
		if( pAttackerList->GetRows() >= pAttackerList->GetRowMax())
		{
			return AI_RT_IGNORE;
		}

		row = pAttackerList->AddRow(-1);
		if (row < 0)
		{ 
			return AI_RT_IGNORE;
		}

		//记录该攻击者
		pAttackerList->SetObject(row, AI_ATTACKER_REC_OBJECT, sender);
		pAttackerList->SetInt(row, AI_ATTACKER_REC_DAMAGE, 0);
		pAttackerList->SetInt64(row, AI_ATTACKER_REC_LASTTIME, ::time(NULL));
		pAttackerList->SetInt64(row, AI_ATTACKER_REC_FIRSTTIME, 0);
	}

    if (selfObj->QueryInt("CurState") != AI_STATE_FIGHT)
    {
        //不在战斗中，进入战斗
        return templateWrap.OnProcess(pKernel, AI_RULE_BEGIN_FIGHT, self, sender, args);
    }
	return AI_RT_IGNORE;
}