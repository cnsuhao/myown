//--------------------------------------------------------------------
// 文件名:      AIRBeginFightBase.cpp
// 内  容:      基本开始战斗规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRBeginFightBase.h"
#include "public/Inlines.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIHeartBeat.h"


AIRBeginFightBase::AIRBeginFightBase()
{
	m_ruleType = AI_RULE_BEGIN_FIGHT;
	m_subRuleCode = AIRS_BEGIN_FIGHT_BASE;
}

int AIRBeginFightBase::DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_NULL;
    }

	//没有攻击距离的不进行触发
	float maxDist = pSelf->QueryFloat("MaxAttackDist");
	if (FloatEqual(maxDist, 0.0f))
	{
		return AI_RT_IGNORE;
	}

    if (pKernel->FindHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT"))
    {
        pKernel->RemoveHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT");
    }

	//状态改变
	templateWrap.EndPatrol(pKernel, self);
	templateWrap.ChangeState(pKernel, self, AI_STATE_FIGHT);		

	//增加战斗的心跳
	if (!pKernel->FindHeartBeat(self, "AIHeartBeat::HB_AIFighting"))
	{
        pSelf->SetInt("LastAttackTime", pSelf->QueryInt("FightAttackFreq"));
		pKernel->AddHeartBeat(self, "AIHeartBeat::HB_AIFighting", AI_NORMAL_CHASE_RATE);
	}

	//若当前还没有攻击目标，就马上设一个，攻击
	PERSISTID AITargetObejct = pSelf->QueryObject("AITargetObejct");
	if(!pKernel->Exists(AITargetObejct) && pKernel->Exists(sender))
	{
		pSelf->SetObject("AITargetObejct", sender);
	}
	AIHeartBeat::HB_AIFighting(pKernel, self, 0);

	return AI_RT_SUCCESS;
}
