//--------------------------------------------------------------------
// 文件名:      AIREscapeBase.h
// 内  容:      基本开始战斗规则
// 说  明:
// 创建日期:    2016年10月29日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIREscapeBase : public AIRuleBase
{
public:
	AIREscapeBase(void);
public:
	virtual int DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap);
};
