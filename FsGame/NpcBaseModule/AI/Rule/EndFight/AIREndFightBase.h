//--------------------------------------------------------------------
// 文件名:      AIREndFightBase.h
// 内  容:      基本结束规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIREndFightBase : public AIRuleBase
{
public:
	AIREndFightBase(void);
public:
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase & templateWrap);
};
