//--------------------------------------------------------------------
// 文件名:		NpcDefine.h
// 内  容:		NPC模块公共头文件
// 说  明:		
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		zhangqs(张青山)
// 版权所有:	苏州蜗牛数字科技股份有限公司
//--------------------------------------------------------------------

#ifndef __NpcDefine_H__
#define __NpcDefine_H__

#pragma region
//NPC相关常量定义
const char* const NPC_RESOURCE_NPC = "ResourceNpc";
const char* const NPC_REVIVE_NPC = "ReviveNpc";
#pragma endregion

#pragma region
//场景相关常量定义
const char* const SCENE_NATION_BATTLE_SCENE = "scene";
const char* const SCENE_NATION_BATTLE_SCENE_ASSIGN = "NationBattleScene";
const char* const SCENE_GUILD_DEFEND_ACTIVITY_SCENE_ASSIGN = "GuildDefendActivityScene";
#pragma endregion

// 类似玩家的npc类型
enum BattleNpc
{
	NORMAL_BATTLE_NPC = 0,	// 正常npc
	ARENA_BATTLE_NPC,		// 竞技场npc
	WORK_NPC				// 打工npc
};
#endif // __NpcDefine_H__
