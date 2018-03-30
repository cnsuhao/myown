//--------------------------------------------------------------------
// 文件名:      AIRSelTargetBase.cpp
// 内  容:      基础设置目标规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRSelTargetBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/Define/Fields.h"
#include "utils/string_util.h"

AIRSelTargetBase::AIRSelTargetBase()
{
	m_ruleType = AI_RULE_SEL_TARTGET;
	m_subRuleCode = AIRS_SEL_TARGET_BASE;
}


int AIRSelTargetBase::DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_NULL;
    }

    IRecord *pAttackerList = pSelf->GetRecord("AttackerList");
    if(pAttackerList == NULL)
    {
        return AI_RT_NULL;
    }
    
	//最大追击范围
	float distance2 = pSelf->QueryFloat("ChaseRange") * pSelf->QueryFloat("ChaseRange");
	
	//出生点
	float fBornX = pSelf->QueryFloat("BornX");
	float fBornZ = pSelf->QueryFloat("BornZ");

	PERSISTID CurAITargetObject = pSelf->QueryObject("AITargetObejct");
    IGameObj *pCurAITargetObject = NULL;
    if(pKernel->Exists(CurAITargetObject))
    {
        pCurAITargetObject = pKernel->GetGameObj(CurAITargetObject);
    }

    //当前攻击目标总伤害
    int iCurDamageValue = 0;
    if(pCurAITargetObject != NULL && pCurAITargetObject->QueryInt("Dead") != 1)
    {
		// 不在同一分组
		if (pCurAITargetObject->QueryInt(FIELD_PROP_GROUP_ID) != pSelf->QueryInt(FIELD_PROP_GROUP_ID))
		{
			CurAITargetObject = PERSISTID();
		}

		// 如果无敌
		if (pCurAITargetObject->GetClassType() == TYPE_PLAYER 
			&& pCurAITargetObject->QueryInt("CantBeAttack") == 1 
			/*&& !StringUtil::CharIsNull(pCurAITargetObject->QueryString(FIELD_PROP_TASK_RESOURCE))*/)
		{
			CurAITargetObject = PERSISTID();
		}
		else
		{
			int iAITargetRow = pAttackerList->FindObject(AI_ATTACKER_REC_OBJECT, CurAITargetObject);
			if (iAITargetRow >= 0)
			{
				iCurDamageValue = pAttackerList->QueryInt(iAITargetRow, AI_ATTACKER_REC_DAMAGE);
			}

			//如果在安全区就不触发
			//当前攻击目标已离开最大追击范围
			len_t x = pCurAITargetObject->GetPosiX();
			len_t z = pCurAITargetObject->GetPosiZ();
			if (util_dot_distance2(fBornX, fBornZ, x, z) > distance2 || pCurAITargetObject->FindData("SafeAreaFlag") || pCurAITargetObject->QueryInt("Invisible") > 0)
			{
				CurAITargetObject = PERSISTID();
			}
		}
              
    }
    else
    {
        CurAITargetObject = PERSISTID();
    }


	int index = -1;
	int max_enm = -1;
	PERSISTID newTarget = PERSISTID();
	int rows = pAttackerList->GetRows();
    LoopBeginCheck(a);
	for (int i = 0; i < rows; ++i)
	{
        LoopDoCheck(a);
		newTarget = pAttackerList->QueryObject(i, AI_ATTACKER_REC_OBJECT);

		//就是当前攻击者
		if(CurAITargetObject == newTarget)
		{
			continue;
		}

		//对象不存在
		if (!pKernel->Exists(newTarget))
		{
			continue;
		}
		IGameObj *pNewTarget = pKernel->GetGameObj(newTarget);
		if (pNewTarget == NULL)
		{
			continue;
		}

		// 不在同一分组
		if (pNewTarget->QueryInt(FIELD_PROP_GROUP_ID) != pSelf->QueryInt(FIELD_PROP_GROUP_ID))
		{
			continue;
		}
		
        //已死亡
		if (pNewTarget->QueryInt("Dead") == 1 || pNewTarget->QueryInt("Invisible") > 0)
		{
			continue;
		}
		//超出势力范围
		if (util_dot_distance2(fBornX, fBornZ, pNewTarget->GetPosiX(), pNewTarget->GetPosiZ()) > distance2)
		{
			continue;
		}

        //如果在安全区就不触发
        if(pNewTarget->FindData("SafeAreaFlag"))
        {
            continue;
        }

		// 如果无敌
		if (pNewTarget->QueryInt("CantBeAttack") == 1)
		{
			continue;
		}

		//获取攻击伤害(所有在攻击表的玩家伤害值不能底于0)
		int n = pAttackerList->QueryInt(i, AI_ATTACKER_REC_DAMAGE);
		if(max_enm < n)
		{
			max_enm = n;
			index = i;
		}
	}

	//找仇恨值最大的
	if (index != -1)
	{
		newTarget = pAttackerList->QueryObject(index, AI_ATTACKER_REC_OBJECT);
	}

    if(index == -1 && CurAITargetObject.IsNull())
    {
       return AI_RT_END_FIGHT;
    }
    else if(CurAITargetObject.IsNull())
    {
        pSelf->SetObject("AITargetObejct", newTarget);
    }
    else if(max_enm > (1.2 * iCurDamageValue))
    {
        pSelf->SetObject("AITargetObejct", newTarget);
    }
    else
    {
        pSelf->SetObject("AITargetObejct", CurAITargetObject);
    }

	return AI_RT_SUCCESS;
}
