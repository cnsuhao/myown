//--------------------------------------------------------------------
// 文件名:      AIRuleManage.h
// 内  容:      AI系统里所有执行规则对象管理器
// 说  明:
// 创建日期:    2015年3月30日
// 创建人:       
// 修改人:        
//--------------------------------------------------------------------

#ifndef __AIRULEMANAGE_H__
#define __AIRULEMANAGE_H__

#include "../AIDefine.h"
#include "AIRuleBase.h"

class AIRuleManage
{
public:

    //初始化
    static bool Init(IKernel *pKernel);

    //取得一个指定的AI执行规则，如找不到返回主类型下的基类
    static AIRuleBase &GetRuleItem(AI_RULE_TYPE ruleType, unsigned int subType);

    ~AIRuleManage();

private:

    //禁用构造拷贝
    AIRuleManage(const AIRuleManage &a);
    //禁用赋值函数
    AIRuleManage & operator =(const AIRuleManage &a);


    //找不到时返回这个
    static AIRuleBase m_AIRuleBase;

    //初始化规则
    static bool RegistAIRule(IKernel *pKernel, AIRuleBase * pRuleBase);

    //规则对象保存表,二维数组，第一层是执行类型，第二层是这一类型的那种执行。
    static std::vector<std::vector<AIRuleBase*>> m_ruleList;
};

#endif//__AIRULEMANAGE_H__