//--------------------------------------------------------------------
// 文件名:      AIREscapeBase.cpp
// 内  容:      逃跑规则
// 说  明:
// 创建日期:    2016年10月29日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIREscapeBase.h"
#include "public/Inlines.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIHeartBeat.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/Define/CoolDownDefine.h"


AIREscapeBase::AIREscapeBase()
{
	m_ruleType = AI_RULE_ESCAPE;
	m_subRuleCode = AIRS_ESCAPE_BASE;
}

int AIREscapeBase::DoRule(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args, AITemplateBase &templateWrap)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
    {
        return AI_RT_NULL;
    }

	if (CoolDownModule::m_pInstance->IsCoolDown(pKernel, self, COOLDOWN_NPC_ESCAPE))
	{
		return AI_RT_NULL;
	}

	int64_t nCurHP = pSelfObj->QueryInt64("HP");
	int64_t nMaxHP = pSelfObj->QueryInt64("MaxHP");
	if (nMaxHP <= 0 || nCurHP <= 0 || pSelfObj->QueryInt("Dead") > 0)
	{
		return AI_RT_NULL;
	}
	// 转化为百分比
	float fCurHPRate = (float)(double(nCurHP) / double(nMaxHP));
	float fWarnHPRate = pSelfObj->QueryFloat("EscapeWarningHPRate");


	// 血量在警戒线以下 逃跑
	if (fCurHPRate <= fWarnHPRate)
	{
		// 设置npc仇恨对象
		PERSISTID ai_target = pSelfObj->QueryObject("AITargetObejct");
		if (!pKernel->Exists(ai_target))
		{
			return AI_RT_NULL;
		}
		if (!pSelfObj->FindData("EB_HateObject"))
		{
			pSelfObj->AddDataObject("EB_HateObject", ai_target);
		}
		else
		{
			pSelfObj->SetDataObject("EB_HateObject", ai_target);
		}

		//退出战斗 状态改变
		AISystem::ClearFight(pKernel, self);
		templateWrap.ChangeState(pKernel, self, AI_ESCAPE);

		AIHeartBeat::HB_CheckEscape(pKernel, self, 0);

		int nEscapeLastTime = pSelfObj->QueryInt("EscapeLastTime");
		if (nEscapeLastTime > 0)
		{
			ADD_COUNT_BEAT(pKernel, self, "AIHeartBeat::HB_EndEscape", nEscapeLastTime, 1);
		}
		
		ADD_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_CheckEscape", 800);

		int nEscapeCD = pSelfObj->QueryInt("EscapeCD");
		if (nEscapeCD > 0)
		{
			CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, COOLDOWN_NPC_ESCAPE, nEscapeCD);
		}
	}

	return AI_RT_SUCCESS;
}
