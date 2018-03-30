//--------------------------------------------------------------------
// 文件名:      AIRSpringBase.h
// 内  容:      基础触发规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIRSpringBase : public AIRuleBase
{
public:
	AIRSpringBase(void);
public:
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase & templateWrap);
};
