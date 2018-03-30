//--------------------------------------------------------------------
// 文件名:      AITNormalFight.cpp
// 内  容:      普通NPC模板
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AITNormalFight.h"
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include <time.h>

AITNormalFight::AITNormalFight(void)
{
	m_templateType = AI_TEMP_NORMAL_FIGHT;

	//激发规则
	RegistEventRule(AI_RULE_SPRING, AIRS_SPRING_BASE);
	//巡逻规则
	RegistEventRule(AI_RULE_PATROL, AIRS_PATROL_BASE);
	//受到伤害的AI规则
	RegistEventRule(AI_RULE_BE_DAMAGED, AIRS_BE_DAMAGED_BASE); 
	//开始战斗AI规则
	RegistEventRule(AI_RULE_BEGIN_FIGHT, AIRS_BEGIN_FIGHT_BASE); 
	//执行战斗AI规则
	RegistEventRule(AI_RULE_FIGHT, AIRS_FIGHT_BASE);   
	//选择攻击对象AI规则
	RegistEventRule(AI_RULE_SEL_TARTGET, AIRS_SEL_TARGET_BASE); 
	//退出战斗AI规则
	RegistEventRule(AI_RULE_END_FIGHT, AIRS_END_FIGHT_BASE);  
	//追击攻击者
	RegistEventRule(AI_RULE_CHASE_TARGET, AIRS_CHASE_TARGET_BASE);
	//被杀死
	RegistEventRule(AI_RULE_ONDEAD, AIRS_ONDEAD_BASE);
	//逃跑
	RegistEventRule(AI_RULE_ESCAPE, AIRS_ESCAPE_BASE);

 }

//析构函数
AITNormalFight::~AITNormalFight()
{

}


