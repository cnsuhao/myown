//--------------------------------------------------------------------
// 文件名:      AIRuleBase.cpp
// 内  容:      基础规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRuleBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"


AIRuleBase::AIRuleBase(void)
{
	m_ruleType = AI_RULE_MAX;

	m_subRuleCode = 0;
}

bool AIRuleBase::Init(IKernel *pKernel)
{
    return true;
}

int AIRuleBase::DoRule(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args, AITemplateBase &templateWrap)
{
	return AI_RT_FAIL;
}
