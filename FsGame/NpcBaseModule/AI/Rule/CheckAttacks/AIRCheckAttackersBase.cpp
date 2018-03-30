//--------------------------------------------------------------------
// 文件名:      AIRCheckAttackersBase.cpp
// 内  容:      基本检查攻击者规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRCheckAttackersBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include <time.h>

AIRCheckAttackersBase::AIRCheckAttackersBase()
{
	m_ruleType = AI_RULE_CHECK_ATTACKERS;
	m_subRuleCode = AIRS_CHECK_ATTACKERS_BASE;
}

int AIRCheckAttackersBase::DoRule(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args, AITemplateBase &templateWrap)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    IRecord *pAttackerList = pSelfObj->GetRecord("AttackerList");
    if(pAttackerList == NULL)
    {
        return AI_RT_NULL;
    }
	int rows = pAttackerList->GetRows();
	//删除不合法的对象
	LoopBeginCheck(a);
	for (int i = rows - 1; i >= 0; i--)
	{
		LoopDoCheck(a);
		PERSISTID target = pAttackerList->QueryObject(i, AI_ATTACKER_REC_OBJECT);
		IGameObj* pTargetObj = pKernel->GetGameObj(target);
		// 删除不存在对象和友方对象
		if (pTargetObj == NULL)
		{
			pAttackerList->RemoveRow(i);
			//continue;
		}

		// 玩家死了,攻击的伤害不清0
//         if(pTargetObj->QueryInt("Dead") > 0)
//         {
//             pAttackerList->SetInt(i, AI_ATTACKER_REC_DAMAGE, 0);
//         }
	}
	return AI_RT_SUCCESS;
}
