//--------------------------------------------------------------------
// 文件名:      AIRChaseTargetBase.cpp
// 内  容:      基本追击规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

//追击目标AI规则基类，其他追击目标AI规则由此派生
class AIRChaseTargetBase : public AIRuleBase
{
public:
	AIRChaseTargetBase(void);
public:
	//执行规则
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args, AITemplateBase &templateWrap);
protected:
	//是不是在技能范围内
	virtual int InSkillRange(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender);

	// 是否能追击玩家
	bool IsCanChasePlayer(IKernel * pKernel, const PERSISTID& target);
};
