//--------------------------------------------------------------------
// 文件名:      AIRuleBase.h
// 内  容:      基础规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "Fsgame/Define/header.h"
#include "../Template/AITemplateBase.h"

//寻路点表
enum
{
	AI_FINDPATH_POS_X,
	AI_FINDPATH_POS_Z
};

class AIRuleBase
{
public:

	AIRuleBase(void);

public:

    virtual bool Init(IKernel *pKernel);

	//获得该规则的类型
	const AI_RULE_TYPE GetRuleType() const
    {
        return m_ruleType;
    };

	//获得该规则的子类型
	const int GetSubRuleType() const
    {
        return m_subRuleCode;
    };

	//执行该规则
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase & templateWrap);

protected:

    //规则主类型
	AI_RULE_TYPE m_ruleType;

    //规则子类型
	int m_subRuleCode;
};

