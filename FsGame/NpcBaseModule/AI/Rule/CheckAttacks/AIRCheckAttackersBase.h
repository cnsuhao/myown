//--------------------------------------------------------------------
// 文件名:      AIRCheckAttackersBase.h
// 内  容:      基本检查攻击者规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

//检查攻击表,第一攻击者AI规则，其他类由此派生
class AIRCheckAttackersBase : public AIRuleBase
{
public:
	AIRCheckAttackersBase(void);
public:
	//执行规则
	virtual int DoRule(IKernel * pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args, AITemplateBase &templateWrap);

private:
};
