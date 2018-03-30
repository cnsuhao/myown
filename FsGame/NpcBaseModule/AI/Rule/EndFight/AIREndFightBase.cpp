//--------------------------------------------------------------------
// 文件名:      AIREndFightBase.cpp
// 内  容:      基本结束规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIREndFightBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/NpcBaseModule/AI/AIFunction.h"
#include "public/Inlines.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame\Interface\FightInterface.h"

AIREndFightBase::AIREndFightBase()
{
	m_ruleType = AI_RULE_END_FIGHT;
	m_subRuleCode = AIRS_END_FIGHT_BASE;
}

int AIREndFightBase::DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
	//退出战斗
    AISystem::ClearFight(pKernel, self);
    //设置状态
	templateWrap.ChangeState(pKernel, self, AI_STATE_BACK);

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_NULL;
    }

    pSelf->SetInt("SubState", AI_PATROL_WAIT);

	//已经死亡
	if (pSelf->QueryInt("Dead") > 0)
	{
		return AI_RT_IGNORE;
	}

    //强制打断技能
    if(FightInterfaceInstance != NULL)
    {
        FightInterfaceInstance->BreakSkill(pKernel, self, self, true);
    }

    float pBornX = pSelf->QueryFloat("BornX");
    float pBornZ = pSelf->QueryFloat("BornZ");

    //判断是不是在出生点
    float px = pSelf->GetPosiX();
    float pz = pSelf->GetPosiZ();
    if (FloatEqual(px ,pBornX) && FloatEqual(pz, pBornZ))
    {
        AIFunction::MotionCallBack(pKernel, self, PERSISTID(), CVarList());
        return AI_RT_SUCCESS;
    }

	//出生状态，直接返回出生点,出生状态为1不巡逻
	int iBornStand = pSelf->QueryInt( "BornStand");
	if (iBornStand == 1)
	{
		//回到出生点
        AIFunction::StartMoveToPoint(pKernel, self, pBornX, pBornZ);
        //普通怪回出生点超时保护
        if (!pKernel->FindHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT"))
        {
            pKernel->AddCountBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT", AIFunction::CountFindPathPointListTime(pKernel, self) + 2000, 1);
        }
		return AI_RT_IGNORE;
	}

    //回到出生点
	if (AIFunction::StartMoveToPoint(pKernel, self, pBornX, pBornZ) == AI_RT_FAIL)
	{
        //回出生点寻路失败
		float pHight  = pKernel->GetWalkHeight(pBornX,pBornZ);
		float pOrient = pSelf->GetOrient();
		pKernel->MoveTo(self,pBornX,pHight,pBornZ,pOrient);

// #ifndef FSROOMLOGIC_EXPORTS
// 		BattleTeamModule::m_pBattleTeamModule->AddLittleStepGridRec(pKernel, self, pBornX, pBornZ);
// #endif // FSROOMLOGIC_EXPORTS

        AIFunction::MotionCallBack(pKernel, self, PERSISTID(), CVarList());
	}

    //普通怪回出生点超时保护
    if (!pKernel->FindHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT"))
    {
        pKernel->AddCountBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT", AIFunction::CountFindPathPointListTime(pKernel, self) + 2000, 1);
    }
    
	return AI_RT_SUCCESS;
}
