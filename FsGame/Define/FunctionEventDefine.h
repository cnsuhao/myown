//--------------------------------------------------------------------
// 文件名:		FunctionEventDefine.h
// 内  容:		游戏功能事件定义
// 说  明:		
// 创建日期:		2017年02月10日
// 创建人:		李海罗
//    :	    
//--------------------------------------------------------------------
#ifndef __FUNCTION_EVENT_DEFINE_H__
#define __FUNCTION_EVENT_DEFINE_H__
#include "../../utils/enum.h"

// 游戏功能事件类型 上线后索引不能变化，与配置文件需要一一对应(ini/SystemFunction/function_event.xml)
#define FUNCTION_EVENT_ID_NONE -1
enum EmFunctionEventId
{
	FUNCTION_EVENT_ID_SYS = 0,						// 未分类的系统功能
	
	// CommonModule 1-50
	FUNCTION_EVENT_ID_GM_COMMAND = 1,				// GM命令
	FUNCTION_EVENT_ID_GMCC_WEB,						// GMCC WEB
	
	// ItemModule 51-100
	FUNCTION_EVENT_ID_SELL_ITEM = 51,				// 向NPC出售物品
	FUNCTION_EVENT_ID_USE_ITEM,						// 使用物品
	FUNCTION_EVENT_ID_JADE,							// 玉珏系统
	// NpcBaseModule 101-200

	// SceneBaseModule 201-300 

	// SkillModule 301-400
	FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL = 301,	
	FUNCTION_EVENT_ID_UPDATE_SKILL,					// 技能升级
	FUNCTION_EVENT_ID_CRIME_SYSTEM,					// 罪恶系统

	// SocialSystemModule 401-600
	FUNCTION_EVENT_ID_FRIEND = 401,
	FUNCTION_EVENT_ID_PRESENT_GIFT,					// 送礼
	FUNCTION_EVENT_ID_SYSTEM_MAIL,					// 系统邮件
	FUNCTION_EVENT_ID_ITEM_TO_MAIL,					// 物品进入邮件事件
	FUNCTION_EVENT_ID_GUILD_CREATE_FAILED_RET,		// 公会创建失败
	FUNCTION_EVENT_ID_GUILD_SHOP_BUY,				// 公会商店购买
	FUNCTION_EVENT_ID_GUILD_CREATE,					// 公会创建
	FUNCTION_EVENT_ID_GUILD_SKILL,					// 公会技能

	// SystemFunctionModule 601-700
	FUNCTION_EVENT_ID_ROLE_BORN = 601,				// 创建角色
	FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP,			// 击杀怪物掉落
	FUNCTION_EVENT_ID_CAPITAL_EXCHANGE,				// 货币兑换
	FUNCTION_EVENT_ID_WING,							// 翅膀系统
	FUNCTION_EVENT_ID_SHOP_GOLD_REFRESH,			// 元宝商店刷新
	FUNCTION_EVENT_ID_SHOP_GOLD_BUY,				// 元宝商店购买
	FUNCTION_EVENT_ID_SHOPING_FAILED_RET,			// 累积商店购买失败返还
	FUNCTION_EVENT_ID_RIDE_SKIN_UNLOCK,				// 解锁坐骑皮肤
	FUNCTION_EVENT_ID_RIDE_TRAIN,					// 坐骑培养
	
	// TaskModule 701-750

	// CampaignModule 751-850

	// EquipmentModule 851-900
	FUNCTION_EVENT_ID_JEWEL_INLAY = 851,		// 宝石镶嵌
	FUNCTION_EVENT_ID_JEWEL_COMPOSE,			// 宝石合成
	FUNCTION_EVENT_ID_JEWEL_DISMANTLE,			// 宝石拆除
	FUNCTION_EVENT_ID_STRENGTHE,				// 装备强化
	FUNCTION_EVENT_ID_SMELT,					// 装备熔炼
	FUNCTION_EVENT_ID_FORGING,					// 装备锻造
	FUNCTION_EVENT_ID_BAPTISE,					// 装备洗练
	FUNCTION_EVENT_ID_BAPTISE_LOCK,				// 装备洗练属性锁定
	FUNCTION_EVENT_ID_FASION_UNLOCK,			// 时装解锁

	// OPActivity 901-950

	// CampaignModule 951-1000
	FUNCTION_EVENT_ID_BUY_ARENA = 951,			// 比武场
	FUNCTION_EVENT_ID_CLEAR_ARENA_CD,			// 清除比武场cd
	FUNCTION_EVENT_ID_ARENA,					// 比武场
	FUNCTION_EVENT_ID_WORLDBOSS_LAST_ATTACK,	// 世界boss最后一刀奖励
	FUNCTION_EVENT_ID_ASURA_BATTLE,				// 修罗战场

	FUNCTION_EVENT_ID_END = 1024,					// 结束符
};


ENUM_TO_STRING_EX(EmFunctionEventId, FUNCTION_EVENT_ID_END,
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SYS)

	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GM_COMMAND)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_ROLE_BORN)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GMCC_WEB)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SELL_ITEM)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_USE_ITEM)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_JADE)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_UPGRADE_PASSIVE_SKILL)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_UPDATE_SKILL)	 
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_CRIME_SYSTEM)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_FRIEND)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_PRESENT_GIFT)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SYSTEM_MAIL)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_ITEM_TO_MAIL)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GUILD_CREATE_FAILED_RET)		// 公会创建失败
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GUILD_SHOP_BUY)				// 公会商店购买
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GUILD_CREATE)					// 公会创建
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_GUILD_SKILL)					// 公会技能

	ENUM_ITEM_STR(FUNCTION_EVENT_ID_ROLE_BORN)				// 创建角色
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP)			// 击杀怪物掉落
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_CAPITAL_EXCHANGE)				// 货币兑换
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_WING)							// 翅膀系统
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SHOP_GOLD_REFRESH)			// 元宝商店刷新
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SHOP_GOLD_BUY)				// 元宝商店购买
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SHOPING_FAILED_RET)			// 累积商店购买失败返还
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_RIDE_SKIN_UNLOCK)				// 解锁坐骑皮肤
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_RIDE_TRAIN)					// 坐骑培养

	
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_CAPITAL_EXCHANGE)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SELL_ITEM)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_USE_ITEM)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_JEWEL_INLAY)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_JEWEL_COMPOSE)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_JEWEL_DISMANTLE)
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_STRENGTHE)				// 装备强化
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_SMELT)				// 装备熔炼
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_FORGING)					// 装备锻造
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_BAPTISE)					// 装备洗练
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_BAPTISE_LOCK)				// 装备洗练属性锁定
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_BUY_ARENA)				// 比武场
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_CLEAR_ARENA_CD)			// 比武场CD
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_ARENA)					// 比武场
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_WORLDBOSS_LAST_ATTACK)	// 世界boss最后一刀奖励
	ENUM_ITEM_STR(FUNCTION_EVENT_ID_ASURA_BATTLE)			// 修罗战场
	);
#endif // __FUNCTION_EVENT_DEFINE_H__
