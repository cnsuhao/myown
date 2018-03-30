//--------------------------------------------------------------------
// 文件名:      AIRFightBase.h
// 内  容:      基本战斗规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIRFightBase: public AIRuleBase
{
public:
	AIRFightBase(void);
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase & templateWrap);
    //获取使用的技能
    virtual const char* GetUseSkill(IKernel* pKernel, const PERSISTID& self);
	int DoFight(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap);
};
