#if !defined __CoolDownDefine_h__
#define __CoolDownDefine_h__

//冷却类型定义
enum COOLDOWN_CATEGORY
{
    COOLDOWN_CATEGOTYID_PUB             = 0,            // 公共冷却(目前对于道具物品技能是无效的)
    COOLDOWN_CATEGOTYID_SKILL           = 1,            // 技能公共冷确
    COOLDOWN_CATEGOTYID_ITEM            = 2,            // 道具公共冷确

    //各聊天频道冷却类型
    //[BEGING, END] 这之间的值请不要使用
    COOLDOWN_CAREGOTYID_CHATTYPE_BEGIN  = 50,
	COOLDOWN_CHAT_WHISPER,						// 私聊CD
	COOLDOWN_CHAT_WORLD,						// 世界CD
	COOLDOWN_CHAT_GUILD,						// 帮会CD
	COOLDOWN_CHAT_TEAM,							// 组队CD
	COOLDOWN_CHAT_WORLD_NEW,					// 小喇叭CD
    COOLDOWN_CAREGOTYID_CHATTYPE_END    = 100,

    // 其它自定义冷却200-2000																															 
    COOLDOWN_CATEGOTYID_RIDESPURT       = 200,
    COOLDOWN_ARRANGE_TOOLBOX,					// 整理背包的冷却时间
    COOLDOWN_CAPITAL_MONITOR ,					// 今天加资金上限提示
    COOLDOWN_ARRANGE_ITEMBOX,					// 整理消耗道具背包的冷却时间
	COOLDOWN_NPC_ESCAPE,						// npc逃跑行为CD

    // 技能冷却ID
    COOLDOWN_SKILL_BEGIN                = 2000,
	COOLDOWN_PlAYER_NORMAL_SKILL		= 5000,

	// 触发buffCD
	COOLDOWN_SKILL_BUFF					= 19000,
    COOLDOWN_SKILL_END                  = 19999, 

    // 道具冷却ID
    COOLDOWN_ITEM_BEGIN                 = 24000, 
    COOLDOWN_ITEM_END                   = 29999, 
};
#endif