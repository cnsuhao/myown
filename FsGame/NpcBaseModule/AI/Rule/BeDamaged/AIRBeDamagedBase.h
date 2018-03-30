//--------------------------------------------------------------------
// 文件名:      AIRBeDamagedBase.h
// 内  容:      被攻击规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIRBeDamagedBase : public AIRuleBase
{
public:
	AIRBeDamagedBase(void);
public:
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase & templateWrap);
protected:
    //记录战斗时每个”sender“对”self“的伤害
    int RecDamage(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);
};
