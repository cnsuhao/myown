//---------------------------------------------------------
//文件名:       RankingDefine.h
//内  容:       排行榜模块的定义
//说  明:       
//          
//创建日期:      2014年11月06日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef FSGAME_DEFINE_RANKING_DEFINE_H
#define FSGAME_DEFINE_RANKING_DEFINE_H

// 排行榜最多允许50列
#define MAX_RANKING_COL_COUNT	50
#define RESERVED_NUMBER			0 // 随机的数字, 用于填充保留字段

#define DO_NOT_INSERT			0 // 不要插入
#define INSERT_IF_NOT_EXIST		1 // 如果不存在就插入

// 排行榜的二级消息
enum ERankingSubMsg
{
    RANKING_SUB_MSG_QUERY = 1,          // 根据条件查询
    RANKING_SUB_MSG_QUERY_TOP_N,        // 查询前N个, 已废弃
    RANKING_SUB_MSG_UPDATE_ENTRY,       // 数据发生变化, 刷新排行榜
    RANKING_SUB_MSG_DELETE_ENTRY,       // 删除一条记录
    RANKING_SUB_MSG_CLEAR,              // 清空排行榜
    RANKING_SUB_MSG_UPDATE_ENTRY_CAL,   // 已抛弃的ID
    RANKING_SUB_MSG_CLEAR_PLANNED,      // 排行榜数据依据计划清空
    RANKING_SUB_MSG_DATE,               // 排行榜的数据日期
    RANKING_SUB_MSG_SWAP,               // 交换排行榜中的两行
	RANKING_SUB_MSG_CLEAR_AWARD,        // 排行榜数据依据计划清空发送奖励
};

// 排行榜清空策略
enum ERankingClear
{
    RANKING_CLEAR_ON_REQUEST = 0, // 根据请求清空
    RANKING_CLEAR_ON_DAY = 1,     // 每天清空
    RANKING_CLEAR_ON_WEEK = 2,    // 每周一的零点清空
    RANKING_CLEAR_ON_MONTH = 3,   // 每月第一天的零点清空
};

// 下面的定义根据使用者需要自行添加

// 排行榜
#define PLAYER_LEVEL_RANK_LIST                  "player_level_rank_list"				// 等级排行榜
#define PLAYER_FIGHT_POWER_RANK_LIST            "player_fight_power_rank_list"			// 战斗力排行榜
#define PLAYER_RED_PACKET_RANK_LIST             "player_red_packet_rank_list"           // 玩家发放红包消耗元宝排行榜
#define PLAYER_LEVEL_RANK_LIST_30_35            "player_level_rank_list_30_35"			// 分段等级排行榜
#define REVENGE_ARENA_RANK_LIST					"revenge_arena_rank_list"				// 生死擂复仇胜利排行榜																				   
#define OFFER_REWARD_RANK_LIST					"offer_reward_rank_list"				// 悬赏排行榜
#define CHASE_SUCCESS_RANK_LIST					"chase_success_rank_list"				// 缉拿排行榜
#define	ARENA_RANK_LIST							"arena_rank_list"						// 竞技场
#define PUB_PLAYER_CHARM_RANK_NAME				"player_charm_rank"						// 魅力值排行榜排名
#define PEERAGE_RANK_LIST						"peerage_rank_list"						// 爵位排行榜
#define WORLD_TOWER_RANK_LIST					"world_tower_rank_list"					// 世界塔排行榜
#define PET_RANK_LIST							"pet_rank_list"							// 侠客排行榜
#define GUILD_LEVEL_RANK_LIST					"guild_level_rank_list"					// 公会等级排行榜
#define GUILD_FIGHT_POWER_RANK_LIST				"guild_fight_power_rank_list"			// 公会战斗力排行榜
#define TRADE_SILVER_RANK_LIST					"trade_silver_rank_list"				// 银币排行榜
#define ACHIEVEMENT_RANK_LIST					"achievement_rank_list"					// 成就排行榜
#define WEEKLYSCORE_RANK_LIST					"weekly_score_rank_list"				// 周常积分个人排行榜
#define GUILD_WEEKLYSCORE_RANK_LIST				"guild_weekly_score_rank_list"			// 公会周常积分排行榜

#define PVP_EXP_RANK_LIST						"player_pvp_exp_list"					// pvp竞技排行榜
#define GLOBAL_PLAYER_GVG_RANK_LIST				"global_player_gvg_rank"				// gvg全服个人榜
#define GLOBAL_GUILD_GVG_RANK_LIST				"global_guild_gvg_rank"					// gvg全服帮会榜
#endif
