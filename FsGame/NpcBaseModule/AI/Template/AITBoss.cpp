//--------------------------------------------------------------------
// 文件名:      AITBoss.cpp
// 内  容:      BOSS模板
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AITBoss.h"
#include "../Rule/AIRuleBase.h"
#include "../AIDefine.h"
#include "FsGame/Define/GameDefine.h"


AITBoss::AITBoss(void)
{
	m_templateType = AI_TEMP_BOSS;

	//激发规则
	//巡逻
	RegistEventRule(AI_RULE_PATROL, AIRS_PATROL_BASE);
	//触发
	RegistEventRule(AI_RULE_SPRING, AIRS_SPRING_BASE);
	//受到伤害的AI规则
	RegistEventRule(AI_RULE_BE_DAMAGED, AIRS_BE_DAMAGED_BASE); 
	//开始战斗AI规则
	RegistEventRule(AI_RULE_BEGIN_FIGHT, AIRS_BEGIN_FIGHT_BASE); 
	//执行战斗AI规则
	RegistEventRule(AI_RULE_FIGHT, AIRS_FIGHT_BASE);   
	//检查攻击表
	RegistEventRule(AI_RULE_CHECK_ATTACKERS, AIRS_CHECK_ATTACKERS_BASE);
	//选择攻击对象AI规则
	RegistEventRule(AI_RULE_SEL_TARTGET, AIRS_SEL_TARGET_BASE); 
	//退出战斗AI规则
	RegistEventRule(AI_RULE_END_FIGHT, AIRS_END_FIGHT_BASE);  
	//被杀死
	RegistEventRule(AI_RULE_ONDEAD, AIRS_ONDEAD_BASE);
	//追击攻击者
	RegistEventRule(AI_RULE_CHASE_TARGET, AIRS_CHASE_TARGET_BASE);
    //休养
    //RegistEventRule(AI_RULE_RECUPERATE, AIRS_RECUPERATE_BOSS);
	//逃跑
	RegistEventRule(AI_RULE_ESCAPE, AIRS_ESCAPE_BASE);
 }

//析构函数
AITBoss::~AITBoss()
{

}

//每次移动完成后回调
int AITBoss::OnBasicActionMsg(IKernel * pKernel , const PERSISTID & self , int msg , const IVarList & args)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_IGNORE;
    }

    //已经死亡
    if (pSelf->QueryInt("Dead") > 0)
    {
        return AI_RT_IGNORE;
    }

    //当前状态
    int cur_state = pSelf->QueryInt("CurState");
    if (cur_state == AI_STATE_PATROL)
    {
        pSelf->SetDataInt("PatrolRestTime", 0);
        pSelf->SetInt("SubState",AI_PATROL_WAIT);
    }
    else if (cur_state == AI_STATE_BACK)
    {
        //OnProcess(pKernel, AI_RULE_RECUPERATE, self);
        
        //回到也生点转身，完成
        ADD_COUNT_BEAT(pKernel, self, "AIHeartBeat::HB_AIRatate", 1000, 1);
    }
    
    return 0;
}