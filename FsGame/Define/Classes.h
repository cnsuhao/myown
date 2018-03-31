//-------------------------------------------------------------------- 
// 文件名:		Classes.h 
// 内  容:		类名定义模块 
// 说  明:		类名称定义 
// 创建者:		 
// 日  期:      	 
//-------------------------------------------------------------------- 
#ifndef __CLASS_DEFINE_H__  
#define __CLASS_DEFINE_H__ 
 
 
//<class id="discard_helper"  
//	type="TYPE_HELPER"  
//	desc="引擎要用到的类"  
//	path="" /> 
#ifndef CLASS_NAME_DISCARD_HELPER 
#define CLASS_NAME_DISCARD_HELPER				"discard_helper" 
#endif 
 
//<class id="player"  
//	type="TYPE_PLAYER"  
//	desc="角色的基类，服务器默认类型"  
//	path="" /> 
#ifndef CLASS_NAME_PLAYER 
#define CLASS_NAME_PLAYER				"player" 
#endif 
 
//<class id="role"  
//	type=""  
//	desc="角色创建时，系统内部调用的对象定义，服务器默认类型"  
//	path="" /> 
#ifndef CLASS_NAME_ROLE 
#define CLASS_NAME_ROLE				"role" 
#endif 
 
//<class id="scene"  
//	type="TYPE_SCENE"  
//	desc="场景的基类，服务器默认类型"  
//	path="" /> 
#ifndef CLASS_NAME_SCENE 
#define CLASS_NAME_SCENE				"scene" 
#endif 
 
//<class id="SecretScene"  
//	type=""  
//	desc="秘境场景"  
//	path="" /> 
#ifndef CLASS_NAME_SECRET_SCENE 
#define CLASS_NAME_SECRET_SCENE				"SecretScene" 
#endif 
 
//<class id="WorldBossScene"  
//	type=""  
//	desc="世界boss场景"  
//	path="struct\scene\world_boss_scene.xml" /> 
#ifndef CLASS_NAME_WORLD_BOSS_SCENE 
#define CLASS_NAME_WORLD_BOSS_SCENE				"WorldBossScene" 
#endif 
 
//<class id="AsuraBattleScene"  
//	type=""  
//	desc="修罗战场"  
//	path="struct\scene\asura_battle_scene.xml" /> 
#ifndef CLASS_NAME_ASURA_BATTLE_SCENE 
#define CLASS_NAME_ASURA_BATTLE_SCENE				"AsuraBattleScene" 
#endif 
 
//<class id="npc"  
//	type="TYPE_NPC"  
//	desc="所有NPC的基类"  
//	path="struct\npc\npc.xml" /> 
#ifndef CLASS_NAME_NPC 
#define CLASS_NAME_NPC				"npc" 
#endif 
 
//<class id="NormalNpc"  
//	type=""  
//	desc="普通NPC"  
//	path="struct\npc\NormalNpc.xml" /> 
#ifndef CLASS_NAME_NORMAL_NPC 
#define CLASS_NAME_NORMAL_NPC				"NormalNpc" 
#endif 
 
//<class id="SecretNpc"  
//	type=""  
//	desc="秘境NPC"  
//	path="" /> 
#ifndef CLASS_NAME_SECRET_NPC 
#define CLASS_NAME_SECRET_NPC				"SecretNpc" 
#endif 
 
//<class id="SummonNpc"  
//	type=""  
//	desc="召唤NPC"  
//	path="" /> 
#ifndef CLASS_NAME_SUMMON_NPC 
#define CLASS_NAME_SUMMON_NPC				"SummonNpc" 
#endif 
 
//<class id="TrapNpc"  
//	type=""  
//	desc="陷阱NPC"  
//	path="struct\npc\TrapNpc.xml" /> 
#ifndef CLASS_NAME_TRAP_NPC 
#define CLASS_NAME_TRAP_NPC				"TrapNpc" 
#endif 
 
//<class id="ManySkillNpc"  
//	type=""  
//	desc="有多个技能的NPC"  
//	path="struct\npc\ManySkillNpc.xml" /> 
#ifndef CLASS_NAME_MANY_SKILL_NPC 
#define CLASS_NAME_MANY_SKILL_NPC				"ManySkillNpc" 
#endif 
 
//<class id="EliteNpc"  
//	type=""  
//	desc="精英NPC"  
//	path="" /> 
#ifndef CLASS_NAME_ELITE_NPC 
#define CLASS_NAME_ELITE_NPC				"EliteNpc" 
#endif 
 
//<class id="BossNpc"  
//	type=""  
//	desc="BOSSNPC"  
//	path="struct\npc\BossNpc.xml" /> 
#ifndef CLASS_NAME_BOSS_NPC 
#define CLASS_NAME_BOSS_NPC				"BossNpc" 
#endif 
 
//<class id="WorldBossNpc"  
//	type=""  
//	desc="世界boss"  
//	path="struct\npc\WorldBossNpc.xml" /> 
#ifndef CLASS_NAME_WORLD_BOSS_NPC 
#define CLASS_NAME_WORLD_BOSS_NPC				"WorldBossNpc" 
#endif 
 
//<class id="AsuraBossNpc"  
//	type=""  
//	desc="修罗战场boss"  
//	path="struct\npc\AsuraBossNpc.xml" /> 
#ifndef CLASS_NAME_ASURA_BOSS_NPC 
#define CLASS_NAME_ASURA_BOSS_NPC				"AsuraBossNpc" 
#endif 
 
//<class id="FunctionNpc"  
//	type=""  
//	desc="功能NPC"  
//	path="" /> 
#ifndef CLASS_NAME_FUNCTION_NPC 
#define CLASS_NAME_FUNCTION_NPC				"FunctionNpc" 
#endif 
 
//<class id="DoorNpc"  
//	type=""  
//	desc="传送门"  
//	path="struct\npc\DoorNpc.xml" /> 
#ifndef CLASS_NAME_DOOR_NPC 
#define CLASS_NAME_DOOR_NPC				"DoorNpc" 
#endif 
 
//<class id="SafeAreaNpc"  
//	type=""  
//	desc="安全区"  
//	path="struct\npc\SafeNpc.xml" /> 
#ifndef CLASS_NAME_SAFE_AREA_NPC 
#define CLASS_NAME_SAFE_AREA_NPC				"SafeAreaNpc" 
#endif 
 
//<class id="item"  
//	type="TYPE_ITEM"  
//	desc="所有物品的基类"  
//	path="" /> 
#ifndef CLASS_NAME_ITEM 
#define CLASS_NAME_ITEM				"item" 
#endif 
 
//<class id="Equipment"  
//	type=""  
//	desc="可装备物品"  
//	path="struct\item\equipment.xml" /> 
#ifndef CLASS_NAME_EQUIPMENT 
#define CLASS_NAME_EQUIPMENT				"Equipment" 
#endif 
 
//<class id="ToolItem"  
//	type=""  
//	desc="道具"  
//	path="struct\item\ToolItem.xml" /> 
#ifndef CLASS_NAME_TOOL_ITEM 
#define CLASS_NAME_TOOL_ITEM				"ToolItem" 
#endif 
 
//<class id="BoxItem"  
//	type=""  
//	desc="宝箱"  
//	path="" /> 
#ifndef CLASS_NAME_BOX_ITEM 
#define CLASS_NAME_BOX_ITEM				"BoxItem" 
#endif 
 
//<class id="Jewel"  
//	type=""  
//	desc="宝石"  
//	path="struct\item\jewel.xml" /> 
#ifndef CLASS_NAME_JEWEL 
#define CLASS_NAME_JEWEL				"Jewel" 
#endif 
 
//<class id="Jade"  
//	type=""  
//	desc="玉珏"  
//	path="struct\item\jade.xml" /> 
#ifndef CLASS_NAME_JADE 
#define CLASS_NAME_JADE				"Jade" 
#endif 
 
//<class id="MoneyItem"  
//	type=""  
//	desc="掉落钱币"  
//	path="" /> 
#ifndef CLASS_NAME_MONEY_ITEM 
#define CLASS_NAME_MONEY_ITEM				"MoneyItem" 
#endif 
 
//<class id="StrengthenStone"  
//	type=""  
//	desc="强化石"  
//	path="" /> 
#ifndef CLASS_NAME_STRENGTHEN_STONE 
#define CLASS_NAME_STRENGTHEN_STONE				"StrengthenStone" 
#endif 
 
//<class id="PolishedStone"  
//	type=""  
//	desc="洗练石"  
//	path="" /> 
#ifndef CLASS_NAME_POLISHED_STONE 
#define CLASS_NAME_POLISHED_STONE				"PolishedStone" 
#endif 
 
//<class id="RideItem"  
//	type=""  
//	desc="坐骑"  
//	path="struct\item\ride.xml" /> 
#ifndef CLASS_NAME_RIDE_ITEM 
#define CLASS_NAME_RIDE_ITEM				"RideItem" 
#endif 
 
//<class id="container"  
//	type="TYPE_ITEM"  
//	desc="所有玩家可操作容器的基类"  
//	path="struct\container\container.xml" /> 
#ifndef CLASS_NAME_CONTAINER 
#define CLASS_NAME_CONTAINER				"container" 
#endif 
 
//<class id="ItemBox"  
//	type=""  
//	desc="基础背包"  
//	path="struct\container\tool_box.xml" /> 
#ifndef CLASS_NAME_ITEM_BOX 
#define CLASS_NAME_ITEM_BOX				"ItemBox" 
#endif 
 
//<class id="EquipBox"  
//	type=""  
//	desc="装备栏"  
//	path="struct\container\equip_box.xml" /> 
#ifndef CLASS_NAME_EQUIP_BOX 
#define CLASS_NAME_EQUIP_BOX				"EquipBox" 
#endif 
 
//<class id="JadeBox"  
//	type=""  
//	desc="玉珏栏"  
//	path="" /> 
#ifndef CLASS_NAME_JADE_BOX 
#define CLASS_NAME_JADE_BOX				"JadeBox" 
#endif 
 
//<class id="RideBox"  
//	type=""  
//	desc="坐骑容器"  
//	path="" /> 
#ifndef CLASS_NAME_RIDE_BOX 
#define CLASS_NAME_RIDE_BOX				"RideBox" 
#endif 
 
//<class id="fixcontainer"  
//	type="TYPE_ITEM"  
//	desc="所有玩家不可操作的容器的基类"  
//	path="" /> 
#ifndef CLASS_NAME_FIXCONTAINER 
#define CLASS_NAME_FIXCONTAINER				"fixcontainer" 
#endif 
 
//<class id="SkillContainer"  
//	type=""  
//	desc="SkillContainer"  
//	path="" /> 
#ifndef CLASS_NAME_SKILL_CONTAINER 
#define CLASS_NAME_SKILL_CONTAINER				"SkillContainer" 
#endif 
 
//<class id="BufferContainer"  
//	type=""  
//	desc="BufferContainer"  
//	path="" /> 
#ifndef CLASS_NAME_BUFFER_CONTAINER 
#define CLASS_NAME_BUFFER_CONTAINER				"BufferContainer" 
#endif 
 
//<class id="Skill"  
//	type="TYPE_ITEM"  
//	desc="技能"  
//	path="struct\skill\skill.xml" /> 
#ifndef CLASS_NAME_SKILL 
#define CLASS_NAME_SKILL				"Skill" 
#endif 
 
//<class id="SkillNormal"  
//	type=""  
//	desc="普通技能"  
//	path="struct\skill\skillnormal.xml" /> 
#ifndef CLASS_NAME_SKILL_NORMAL 
#define CLASS_NAME_SKILL_NORMAL				"SkillNormal" 
#endif 
 
//<class id="SkillPassive"  
//	type=""  
//	desc="被动技能"  
//	path="" /> 
#ifndef CLASS_NAME_SKILL_PASSIVE 
#define CLASS_NAME_SKILL_PASSIVE				"SkillPassive" 
#endif 
 
//<class id="Buffer"  
//	type="TYPE_ITEM"  
//	desc="Buffer"  
//	path="struct\skill\buffer.xml" /> 
#ifndef CLASS_NAME_BUFFER 
#define CLASS_NAME_BUFFER				"Buffer" 
#endif 
 
//<class id="RandomNpcCreator"  
//	type="TYPE_NPC"  
//	desc="随机对象生成器"  
//	path="" /> 
#ifndef CLASS_NAME_RANDOM_NPC_CREATOR 
#define CLASS_NAME_RANDOM_NPC_CREATOR				"RandomNpcCreator" 
#endif 
 
//<class id="RandomNpcCreatorItem"  
//	type="TYPE_HELPER"  
//	desc="随机对象生成器的子项"  
//	path="" /> 
#ifndef CLASS_NAME_RANDOM_NPC_CREATOR_ITEM 
#define CLASS_NAME_RANDOM_NPC_CREATOR_ITEM				"RandomNpcCreatorItem" 
#endif 
 
//<class id="NpcCreatorByTime"  
//	type="TYPE_HELPER"  
//	desc="时间刷新npc机制"  
//	path="struct\npc\creator\ncp_createByTime.xml" /> 
#ifndef CLASS_NAME_NPC_CREATOR_BY_TIME 
#define CLASS_NAME_NPC_CREATOR_BY_TIME				"NpcCreatorByTime" 
#endif 
 
 
#endif // __CLASS_DEFINE_H__