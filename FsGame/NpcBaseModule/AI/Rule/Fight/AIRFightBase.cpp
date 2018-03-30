//--------------------------------------------------------------------
// 文件名:      AIRFightBase.cpp
// 内  容:      基本战斗规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRFightBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "utils/util_func.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/Interface/FightInterface.h"
#include "utils/string_util.h"
#include <time.h>

AIRFightBase::AIRFightBase()
{
	m_ruleType = AI_RULE_FIGHT;
	m_subRuleCode = AIRS_FIGHT_BASE;
}

int AIRFightBase::DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

	//已经死亡
	if (pSelf->QueryInt("Dead") > 0)
	{
		return 0;
	}

    //出生状态
	if (pSelf->QueryInt("BornState") > 0)
	{
		return 0;
	}

	//检查攻击表
	int ai_ret = templateWrap.OnProcess(pKernel, AI_RULE_CHECK_ATTACKERS, self, sender, args);
// 	if(ai_ret == AI_RT_END_FIGHT)
// 	{
// 		return templateWrap.OnProcess(pKernel, AI_RULE_END_FIGHT, self, sender, args);
// 	}

	//选择目标
	ai_ret = templateWrap.OnProcess(pKernel, AI_RULE_SEL_TARTGET, self, sender, args);
	if(ai_ret == AI_RT_END_FIGHT)
	{
		return templateWrap.OnProcess(pKernel, AI_RULE_END_FIGHT, self, sender, args);
	}

	//开始攻击行为
	return DoFight(pKernel, self, sender, args, templateWrap); 
}

int AIRFightBase::DoFight(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

	//得到目标对象
	PERSISTID target = pSelf->QueryObject("AITargetObejct");
	if(!pKernel->Exists(target))
	{
		return 0;
	}

    //增加时间技能权值累加
    IRecord *npcSkillRec = pSelf->GetRecord(NPC_SKILL_REC);
    if(npcSkillRec == NULL)
    {
        return AI_RT_NULL;
    }
    NpcSkill skillInfoTemp;
    int skills = npcSkillRec->GetRows();
    LoopBeginCheck(a);
    for (int i = 0; i < skills; i++)
    {
        LoopDoCheck(a);
        if (!AISystem::GetSkillConfig(pKernel, self, npcSkillRec->QueryString(i, BOSS_SKILL_ID), skillInfoTemp))
        {
            continue;
        }
        npcSkillRec->SetInt(i, BOSS_SKILL_VALUE_CUR, (npcSkillRec->QueryInt(i, BOSS_SKILL_VALUE_CUR) + skillInfoTemp.time));
    }

    //取得攻击技能
    const char* resultSkill = AIRFightBase::GetUseSkill(pKernel, self);
    if (StringUtil::CharIsNull(resultSkill))
    {
        return AI_RT_NULL;
    }

    //取得当前攻击技能可用距离
    len_t fMinDis = 0.0f;
    len_t fMaxDis = 0.0f;
    FightInterfaceInstance->ReadyUseRandomSkill(pKernel, self, fMinDis, fMaxDis, resultSkill);
    if(fMaxDis <= 0) //保证不小于等于0
    {
        fMaxDis = 2;
    }
    pSelf->SetFloat("MinAttackDist", fMinDis);
    pSelf->SetFloat("MaxAttackDist", fMaxDis);

    //查看是否到达追击距离
    if (templateWrap.OnProcess(pKernel, AI_RULE_CHASE_TARGET, self, target, args) != AI_RT_SUCCESS)
    {
        return AI_RT_IGNORE;
    }

    //检查上次攻击过去的时间是否满足攻击频率
    int newlastAttackTime = pSelf->QueryInt("LastAttackTime") + args.IntVal(0);
    if(newlastAttackTime < pSelf->QueryInt("FightAttackFreq"))
    {
        //更新上次攻击过去的时间
        pSelf->SetInt("LastAttackTime", newlastAttackTime);
        return AI_RT_IGNORE;
    }
    pSelf->SetInt("LastAttackTime", 0);

	if (strcmp(resultSkill, "") != 0)
	{
        //使用攻击技能
		FightInterfaceInstance->UseSkill(pKernel, self, resultSkill, CVarList() << target);        

        //重置当前使用攻击技能的
        int row = npcSkillRec->FindString(BOSS_SKILL_ID, resultSkill);
        if (row >= 0)
        {
            npcSkillRec->SetInt(row, BOSS_SKILL_VALUE_CUR, 0);
        }

        //每次攻击时累计技能权重，不包含这次用的
        NpcSkill skillInfoTempConfig;
        int skillCount = npcSkillRec->GetRows();
        LoopBeginCheck(b);
        for (int i = 0; i < skillCount; i++)
        {
            LoopDoCheck(b);
            //这次用的
            if(i == row)
            {
                continue;
            }

            if (!AISystem::GetSkillConfig(pKernel, self, npcSkillRec->QueryString(i, BOSS_SKILL_ID), skillInfoTempConfig))
            {
                continue;
            }
                
            npcSkillRec->SetInt(i, BOSS_SKILL_VALUE_CUR, (npcSkillRec->QueryInt(i, BOSS_SKILL_VALUE_CUR) + skillInfoTempConfig.addWeighting));
        }

        //记录攻击交互时间
        IRecord *pAttackerList = pSelf->GetRecord("AttackerList");
        if(pAttackerList == NULL)
        {
            return AI_RT_NULL;
        }
        row = pAttackerList->FindObject(AI_ATTACKER_REC_OBJECT, target);
        if(row >= 0)
        {
            pAttackerList->SetInt64(row, AI_ATTACKER_REC_LASTTIME, ::time(NULL));
        }
	}

	return AI_RT_SUCCESS;
}

//获取可用技能
const char* AIRFightBase::GetUseSkill(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return "";
    }

    IRecord *npcSkillRec = pSelf->GetRecord(NPC_SKILL_REC);
    if(npcSkillRec == NULL)
    {
        return "";
    }

    int npcSkillRecLength = npcSkillRec->GetRows();
    if (npcSkillRecLength <= 0)
    {
        return "";
    }

    int64_t curHP = pSelf->QueryInt64("HP");
    int64_t maxHP = pSelf->QueryInt64("MaxHP");
    if(maxHP <= 0 || curHP <= 0)
    {
        return "";
    }
    int curHPPercent = (int) (100 * curHP / maxHP);

    const char *resultSkill = "";
    int priorTemp = -1;
    NpcSkill itemSkillTemp;
    LoopBeginCheck(c);
    for(int i = 0; i < npcSkillRecLength; ++i)
    {
        LoopDoCheck(c);
        if(!AISystem::GetSkillConfig(pKernel, self, npcSkillRec->QueryString(i, BOSS_SKILL_ID), itemSkillTemp))
        {
            continue;
        }

        //血条件
        if(curHPPercent > itemSkillTemp.condition)
        {
            continue;
        }

        //当前权值条件
        if(npcSkillRec->QueryInt(i, BOSS_SKILL_VALUE_CUR) < itemSkillTemp.maxWeighting)
        {
            continue;
        }

        //冷却中
        if (FightInterfaceInstance->IsCoolDown(pKernel, self, itemSkillTemp.skillID.c_str()))
        {
            continue;
        }

        //选择优先级最高的
        if(itemSkillTemp.prior > priorTemp)
        {
            priorTemp = itemSkillTemp.prior;
            resultSkill = npcSkillRec->QueryString(i, BOSS_SKILL_ID);
        }
    }

    return resultSkill;
}
