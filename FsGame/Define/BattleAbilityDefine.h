//--------------------------------------------------------------------
// 文件名:		BattleAbilityDefine.h
// 内  容:		战斗力统计
// 说  明:		
// 创建日期:	2017年04月12日
// 整理日期:	2017年04月12日
// 创建人:		刘明飞  
//--------------------------------------------------------------------

#ifndef __BATTLE_ABILITY_DEFINE_H__
#define __BATTLE_ABILITY_DEFINE_H__

#define BATTLLE_ABILITY_REC "battlle_ability_rec"		// 战斗力记录表

enum BatttleAbilityRecCol
{
	BAR_COL_TYPE,				// 类型	BatttleAbilityType
	BAR_COL_VALUE				// 战斗力值
};

// 战斗力统计类型
enum BatttleAbilityType
{
	BA_PROPERTY_TYPE,			// 玩家属性战斗力
	BA_SKILL_TYPE,				// 主动技能战斗力
	BA_PASSIVE_SKILL_TYPE,		// 被动技能战斗力(经脉、坐骑、侠客、印记)

	MAX_BATTLE_ABILITY_TYPE
};


// 公式类型
enum BAFormulaCalType
{
	BFC_SKILL_CAL = 1,				// 主动技能类型
	BFC_PASSIVE_SKILL_CAL			// 被动技能类型
};
#endif //__BATTLE_ABILITY_DEFINE_H__