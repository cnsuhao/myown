//--------------------------------------------------------------------
// 文件名:		FightNpcDefine.h
// 内  容:		NPC模块公共头文件
// 说  明:		
//				
// 创建日期:	2018年03月30日
// 整理日期:	2018年03月30日
//--------------------------------------------------------------------

#ifndef __BOSS_DEFINE_H__
#define __BOSS_DEFINE_H__

//场景BOSS信息缓存表
#define SCENE_BOSS_TOTAL_REC "scene_boss_total_rec"   

enum BossState
{
	BS_BOSS_SLEEP = -1,								// boss不刷状态
	BS_BOSS_BORN,									// boss出生
	BS_BOSS_DEAD,									// boss死亡
};

enum
{
	SCENE_BOSS_INFO_CONFIG = 0,						// bossid
	SCENE_BOSS_DEAD_TIME,							// boss死亡时间 1表示boss存在,要刷boss
	SCENE_BOSS_INFO_SCENEID,						// 所在场景

	SCENE_BOSS_INFO_MAX,
};


// 类似玩家的npc类型
enum BattleNpc
{
	NORMAL_BATTLE_NPC = 0,	// 正常npc
	ARENA_BATTLE_NPC,		// 竞技场npc
	WORK_NPC				// 打工npc
};
#endif // __BOSS_DEFINE_H__
