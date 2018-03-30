//--------------------------------------------------------------------
// 文件名:      AIRPatrolBase.h
// 内  容:      巡逻规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "../AIRuleBase.h"

//执行巡逻规则基类
class AIRPatrolBase : public AIRuleBase
{
public:
	AIRPatrolBase(void);
public:
	virtual int DoRule(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap);

protected:
	//巡逻下一点
	virtual int NextPatrol(IKernel * pKernel, const PERSISTID & self,const AITemplateBase &templateWrap);
};
