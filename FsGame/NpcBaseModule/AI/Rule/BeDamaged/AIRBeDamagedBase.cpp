//--------------------------------------------------------------------
// 文件名:      AIRBeDamagedBase.cpp
// 内  容:      被攻击规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRBeDamagedBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "Interface/FightInterface.h"


AIRBeDamagedBase::AIRBeDamagedBase()
{
	m_ruleType = AI_RULE_BE_DAMAGED;
	m_subRuleCode = AIRS_BE_DAMAGED_BASE;
}

int AIRBeDamagedBase::DoRule(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args, AITemplateBase &templateWrap)
{
	if(self == sender)
	{
		return AI_RT_FAIL;
	}

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_FAIL;
    }
    IGameObj *pSender = pKernel->GetGameObj(sender);
    if(pSender == NULL)
    {
        return AI_RT_FAIL;
    }

	// 宠物的话,找到它的主人
	PERSISTID damager = get_pet_master(pKernel, sender);

	//记录战斗时每个”sender“对”self“的伤害，先添加攻击列表
	RecDamage(pKernel, self, damager, args);

	if (pSelf->QueryInt("CurState") != AI_STATE_FIGHT)
	{
		//不在战斗中，进入战斗
		templateWrap.OnProcess(pKernel, AI_RULE_BEGIN_FIGHT, self, damager, args);
	}
	return AI_RT_SUCCESS;
}

//记录战斗时每个”sender“对”self“的伤害
int AIRBeDamagedBase::RecDamage(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_FAIL;
    }

	//这次对怪物伤害输出
	int nDamage = args.IntVal(1);
	int nDamageCate = args.IntVal(2);
	// 只记录技能产生的仇恨值
	if (nDamageCate != DAMAGE_CATE_SKILL)
	{
		return AI_RT_FAIL;
	}
	
	PERSISTID skill = args.ObjectVal(3);
	int nAddHateVal = FightInterfaceInstance->ComputeHateValue(pKernel, skill, nDamage);

    IRecord *pAttackerList = pSelf->GetRecord("AttackerList");
    if(pAttackerList == NULL)
    {
        return AI_RT_NULL;
    }

	//将攻击对象加入到表里  记录伤害值
	int row = pAttackerList->FindObject(AI_ATTACKER_REC_OBJECT, sender);
	int rows = pAttackerList->GetRows();
	int iMaxRows = pAttackerList->GetRowMax();
	if (row < 0)
	{
		if(rows < iMaxRows)
		{
			//如果表为空 直接写入首位
			CVarList values;
			values << sender << nAddHateVal << time(NULL) << time(NULL);
			pAttackerList->AddRowValue(-1, values);
		}
		else
		{
			//删除仇恨最小者且不是当前目标
			PERSISTID AITargetObejct = pSelf->QueryObject("AITargetObejct");
			if(!pKernel->Exists(AITargetObejct))
			{
				AITargetObejct = PERSISTID();
			}

			// 替换最后一个
			int minEnm = INT_MAX;
			int minRow = 0;
            LoopBeginCheck(a);
			for(int i = 0; i < rows; ++i)
			{
                LoopDoCheck(a);
				PERSISTID target = pAttackerList->QueryObject(i, AI_ATTACKER_REC_OBJECT);
				if(AITargetObejct != target)
				{
					int enm = pAttackerList->QueryInt(i, AI_ATTACKER_REC_DAMAGE);
					if(enm < minEnm)
					{
						minEnm = enm;
						minRow = i;
					}
				}
			}

			if (minRow >= 0)
			{
				pAttackerList->SetObject(minRow, AI_ATTACKER_REC_OBJECT ,sender);
				pAttackerList->SetInt(minRow, AI_ATTACKER_REC_DAMAGE, nAddHateVal);
				pAttackerList->SetInt64(minRow, AI_ATTACKER_REC_LASTTIME, ::time(NULL));
				pAttackerList->SetInt64(minRow, AI_ATTACKER_REC_FIRSTTIME, time(NULL));
			}
		}
	}
	else
	{
		//更新伤害总值 并计算仇恨值
		int sum = pAttackerList->QueryInt(row, AI_ATTACKER_REC_DAMAGE);
		sum = sum + nAddHateVal;
		pAttackerList->SetInt(row, AI_ATTACKER_REC_DAMAGE, sum);
		int64_t iTime = pAttackerList->QueryInt64(row, AI_ATTACKER_REC_FIRSTTIME);
		//没有第一次攻击时间进行记录
		if (iTime == 0)
		{
			pAttackerList->SetInt64(row, AI_ATTACKER_REC_FIRSTTIME, time(NULL));
		}
		//更新最后一次交互时间
		pAttackerList->SetInt64(row, AI_ATTACKER_REC_LASTTIME, ::time(NULL));
	}

	return AI_RT_SUCCESS;
}
