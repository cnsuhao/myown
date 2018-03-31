//--------------------------------------------------------------------
// 文件名:      WorldBossNpcDefine.h
// 内  容:      WorldBossNpcDefine的相关定义
// 说  明:		
// 创建日期:    2015年06月23日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef __WorldBossNpc_Define_H__
#define __WorldBossNpc_Define_H__

// 没有找到对应活动场景
#define SCENE_NOT_EXIST 0;

// 规定时间内没有人击杀BOSS
#define LAST_KILL_BOSS_NAME L"sys"
// GM标识
#define FLAG_WORLD_BOSS_GM "WorldBossGM"
// 活动ID
#define WORLD_BOSS_ACTIVE_CFG_ID "active_cfg_id"

// 活动标识
#define FLAG_WORLD_BOSS_VISIT "WorldBossVisitFlag"

// 活动标识
#define FLAG_WORLD_BOSS_CONFIG_DATA_INDEX "WorldBossConfigDataIndex"

enum FlagWorldBossVisit
{
	FLAG_WORLD_BOSS_VISIT_CLOSED  = 0,   // 活动结束,关闭
	FLAG_WORLD_BOSS_VISIT_NOTICED = 1,   // 准备通知阶段
	FLAG_WORLD_BOSS_VISIT_ACTIVED = 2,   // 开始进入场景
	FLAG_WORLD_BOSS_VISIT_BORNED = 3,    // BOSS出生
};


// 世界boss活动类型
enum WorldBossActivityType
{
	WBAT_NONE_WORLD_BOSS = 0,
	WBAT_AM_WORLD_BOSS,				// 中午的世界boss
	WBAT_PM_WORLD_BOSS				// 下午的世界boss
};

#define WORLD_BOSS_DAMAGE_REC "world_boss_damage_rec"		// 世界boss伤害表

enum
{
	WBD_COL_PLAYER_RANK,						// 玩家排名
	WBD_COL_PLAYER_NAME,						// 玩家名字
	WBD_COL_PLAYER_TOTAL_DAMAGE,				// 总伤害

	MAX_WBD_COL_NUM,
};

#define WORLD_BOSS_ELITE_NPC_REC "world_boss_elite_npc_rec"		// 世界boss精英怪表
enum
{
	WBEN_COL_NPC_OBJECT,						// npc对象号
	WBEN_COL_NPC_INDEX,							// npc序号

	MAX_WBEN_COL_NUM,
};


// 存放在PUB服务上活动状态表
#define WORLD_BOSS_ACTIVE_STATUS_REC "world_boss_active_status_rec"
enum WorldBossActiveStatus_COL
{
	WORLD_BOSS_STATUS_REC_COL_SCENEID = 0,   // 场景ID
	WORLD_BOSS_STATUS_REC_COL_STATUS  = 1,   // 活动状态 对应WorldBossActiveStatus
	WORLD_BOSS_STATUS_REC_COL_ID      = 2,   // 活动配置ID
};

// 存放BOSS成长信息
#define PUB_WORLD_BOSS_GROW_UP_REC "world_boss_grow_up_rec"
enum WorldBossGroupUp_COL
{
	WORLD_BOSS_GROW_UP_REC_COL_SCENEID      = 0,	// 场景ID
	WORLD_BOSS_GROW_UP_REC_COL_LEVEL,				// boss等级

	MAX_WORLD_BOSS_GROW_ROWS = 50
};

// 世界Boss攻击列表
#define WORLDBOSS_ATTACK_LIST_REC "worldboss_attack_list_rec"
enum 
{
	WORLDBOSS_ATTACK_LIST_REC_PLAYER_NAME	= 0,    // 攻击对象名字
	WORLDBOSS_ATTACK_LIST_REC_DAMAGE				// 总伤害
};

// 公共区数据
#define PUB_KILL_WORLD_BOSS_PLAYER_NAME_REC "kill_world_boss_player_name_rec"
enum 
{
	KWBP_PUB_COL_SCENE_ID = 0,
	KWBP_PUB_COL_KILLER_NAME,						// 杀死boss玩家的名字
	KWBP_PUB_COL_BOSS_STATE,						// boss 状态

	MAX_KWBP_PUB_COL_NUM,
	MAX_KWBP_PUB_ROW_NUM = 100
};

#define BOSS_ACTIVE_OPEN_TIMES "boss_active_open_times"
enum
{
	BOSS_ACTIVE_OPEN_TIMES_REC_SCENE      = 0,
	BOSS_ACTIVE_OPEN_TIMES_REC_OPEN_TIMES = 1,
};

// 世界BOSS活动状态
enum WorldBossActiveStatus
{
	STATE_WBOSS_ACTIVE_CLOSE       = 0,      // 活动关闭
	STATE_WBOSS_ACTIVE_NOTICE      = 1,      // 活动开启公告
	STATE_WBOSS_ACTIVE_OPEN        = 2,      // 活动正式开启
};    


// 客户端子消息定义
enum CS_WorldBossSubMsgID
{
	CS_WORLD_BOSS_ACTIVE_TRANSMIT            = 0,	// 活动开启 传送到活动场景 int sceneid
	CS_WORLD_BOSS_ACTIVE_BACK_TO_LAST_SCENE,		// 活动中途退出  返回上一个场景,找不到就回皇城
	CS_WORLD_BOSS_ACTIVE_QUERY_ACTIVE_STATUS,		// 查询当前活动状态
	CS_WORLD_BOSS_ACTIVE_QUERY_BASE_INFO,			// 查询世界boss击杀的基本数据
// 	CS_WORLD_BOSS_ACTIVE_QUERY_CURR_DAY_TYPE,		//  暂时不用 查询当天的活动类型
// 	CS_YEAR_BOSS_BUY_BUFF,							// 暂时不用 年兽活动购买buff int-购买货币类型 
// 	CS_YEAR_BOSS_QUERY_COOLDOWN_TIME,				// 暂时不用 年兽活动查询购买buff CD时间	

};
// 服务端子消息定义
enum SC_WorldBossSSubMsgId
{
	SC_WORLD_BOSS_ACTIVE_OPEN          = 0,             // 活动开启  int剩余时间
	/*!
	* @brief	结算界面
	* @param	int			我的排名
	* @param	int			我的伤害
	* @param	string		我的奖励
	*/
	SC_WORLD_BOSS_ACTIVE_ACTOR_AWARD,					

	SC_WORLD_BOSS_ACTIVE_CLOSE,							// 活动结束 
	/*!
	 * @brief	 活动基本数据
	 * @param	int		n个世界boss场景
	 以下循环n次
	 * @param	int		场景id
	 * @param	wstring	击杀boss玩家名字
	 * @param	int		Boss状态 WorldBossActiveStatus 0关闭 2开启
	 */
	SC_WORLD_BOSS_ACTIVE_BASE_INFO,		

	SC_WORLD_BOSS_START_PLAY_CG,					// 开始播放CG

	/*!
	* @brief	实时排名数据
	* @param	int		前n名
	以下循环n次
	* @param	int		玩家名次
	* @param	wstring 玩家名字
	* @param	int		玩家伤害值
	*/
	SC_WORLD_BOSS_ACTIVE_HURT_RANK,					// 实时排行伤害列表  

	/*!
	* @brief	实时排名数据
	* @param	int			我的排名
	* @param	int			我的伤害
	*/
	SC_WORLD_BOSS_SELF_HURT_INFO,					// 玩家自己的实时排名数据

	/*!
	* @brief	存活的npc数据
	* @param	int		n个精英怪
	以下循环n次
	* @param	int		序号
	*/
	SC_WORLD_BOSS_LIVE_ELITE_NPC,							// 场景内怪的状态

	/*!
	* @brief	npc死亡
	* @param	int		npc序号
	*/
	SC_WORLD_BOSS_ELITE_NPC_DEAD,							// 某个精英怪死亡

	/*!
	* @brief	活动进入窗口
	*/
	SC_WORLD_BOSS_ACTIVE_SHOW_ENTRY_PANEL,

	SC_WORLD_BOSS_ACTIVE_LEVEL_LIMIT,             // 暂时不用 等级不够 
	SC_WORLD_BOSS_ACTIVE_CURR_TYPE,             // 暂时不用 当天活动类型 int活动类型 int当天秒数
	SC_YEAR_BOSS_ADD_BUFF_SUCCESS,             // 暂时不用 购买buff成功   int-铜币buff层级   int-元宝buff层级 
	SC_YEAR_BOSS_ADD_BUFF_FAIL,             // 暂时不用 购买buff失败. 
	SC_YEAR_BOSS_QUERY_COOLTIME,             // 暂时不用  冷却时间.         

};

// 服务器发公共服务器子消息定义
enum SP_WorldBossSubMsgId 
{
	//SP_WORLD_BOSS_ACTIVE_STATUS_QUERY    = 0,  // 查询状态
	SP_WORLD_BOSS_ACTIVE_STATUS_SET      = 1,  // 设置状态
	//SP_WORLD_BOSS_ACTIVE_TYPE_SET        = 2,  // 设置设置活动类型
	SP_WORLD_BOSS_BE_KILL_TIME_SET       = 3,  // 设置BOSS被击杀次数
	SP_WORLD_BOSS_BE_KILL_TIME_SET_GM    = 4,  // 设置BOSS被击杀次数
	//SP_WORLD_BOSS_CREATE_TIME_SET        = 5,  // 设置BOSS创建时间
	//SP_WORLD_BOSS_TODAY_CHECKED          = 6,  // 设置当天是否检测过活动类型
	//SP_WORLD_BOSS_ACTIVE_MODEL           = 7,  // 设置活动模式

	SP_NATION_BATTLE_OPEN				= 10,	// 国战开启
	SP_WORLD_BOSS_OPEN					= 11,	// 世界boss开启
	SP_GUILD_DEFEND_OPEN				= 12,	// 帮会战开启
	SP_SAVE_ACTIVE_TIMES				= 13,	// 记录活动开启次数
	SP_UPDATE_BOSS_REC_INFO				= 14,	// 记录boss的数据
};
// 公共服务器发服务器发子消息定义
enum PS_WorldBossSubMsgId
{
	PS_QUERY_ACTIVE_STATUS            = 0,   // 查询状态结果
};

enum SS_WorldBossSubMsgId
{
	SS_WORLD_BOSS_ACTIVE_RESTART        = 0,   // 活动开启
	SS_WORLD_BOSS_ACTIVE_UPDATE_DATA    = 1,   // 更新member上数据
	SS_WORLD_BOSS_ACTIVE_END            = 2,   // 活动结束
};

// 活动类型
enum ActiveTypeCode
{
	ACTIVE_TYPE_MIN           = 0,
	ACTIVE_TYPE_NATION_BATTLE = 1,   // 国战
	AVTIVE_TYPE_GUILD         = 2,   // 帮会活动
	ACTIVE_TYPE_WORLD_BOSS    = 3,   // 世界BOSS（下午）
	AVTIVE_TYPE_GVG_GUILD     = 4,   // 帮会跨服站
	ACTIVE_TYPE_WORLD_BOSS_AM = 5,   // 世界BOSS（上午）
	ACTIVE_TYPE_YEAR_BOSS_AM  = 6,   // 年兽BOSS（上午）
	ACTIVE_TYPE_YEAR_BOSS     = 7,   // 年兽BOSS（下午）
	ACTIVE_TYPE_MAX,
};

// 活动类型
enum IsActiveChecked
{
	ACTIVE_IS_CHECKED_OFF     = 0,
	ACTIVE_IS_CHECKED_ON      = 1,
};

// 活动结果
enum ActiveResult
{
	FAIL_KILL_WORLD_BOSS = 0,		// 击杀世界boss失败
	SUC_KILL_WORLD_BOSS				// 击杀世界boss成功
};

enum ActiveRule
{
	ACTIVE_RULE_IGNORE  = 0,
	ACTIVE_RULE_OPEN    = 1,
};

#define TIMER_WORLDBOSS_ACTIVE 5  // 秒

enum BranchActiveType
{
	ACTIVE_MODEL_TYPE_MIN       = 0,
	ACTIVE_MODEL_TYPE_WORLDBOSS = 1,
    ACTIVE_MODEL_TYPE_YEARBOSS  = 2,
	ACTIVE_MODEL_TYPE_MAX,
};

enum BossActiveBuffPayType
{
	BOSS_ACTIVE_BUFF_PAY_TYPE_COPPER = 1,
	BOSS_ACTIVE_BUFF_PAY_TYPE_SILVER = 2,
};

#endif

