//--------------------------------------------------------------------
// 文件名:	GameAnalyticsEventNames.h
// 内  容:	游戏分析事件名
// 说  明:	
// 创建日期:	2016年08月24日
// 创建人:	李海罗
// 整理人:            
//--------------------------------------------------------------------
#ifndef __GAME_ANALYTICS_EVENT_NAMES_H__
#define __GAME_ANALYTICS_EVENT_NAMES_H__

#define  GAME_ANALYTICS_EVENT_GAME_START		"game_start"	// 开始游戏
#define  GAME_ANALYTICS_EVENT_GAME_END			"game_end"		// 关闭游戏
#define  GAME_ANALYTICS_EVENT_SET_CLIENT		"set_client"	// 设置客户端信息

#define  GAME_ANALYTICS_EVENT_NEW_USER			"new_user"		// 注册账号
#define  GAME_ANALYTICS_EVENT_DEL_USER			"del_user"		// 删除账号

#define  GAME_ANALYTICS_EVENT_SIGNIN			"signin"		// 登陆
#define  GAME_ANALYTICS_EVENT_SIGNOUT			"signout"		// 退出

#define  GAME_ANALYTICS_EVENT_NEW_ROLE			"new_role"		// 创建角色
#define  GAME_ANALYTICS_EVENT_DEL_ROLE			"del_role"		// 删除角色
#define  GAME_ANALYTICS_EVENT_ROLE_ENTRY		"role_entry"	// 角色进入
#define  GAME_ANALYTICS_EVENT_ROLE_EXIT			"role_exit"		// 角色退出

#define  GAME_ANALYTICS_EVENT_U_NEW_ROLE		"u_new_role"	// 用户创建角色
#define  GAME_ANALYTICS_EVENT_U_ROLE_ENTRY		"u_role_entry"	// 用户角色进入

#define  GAME_ANALYTICS_EVENT_PAY_START		"pay_start"			// 支付开始
#define  GAME_ANALYTICS_EVENT_PAY_END		"pay_end"			// 支付结束

#define  GAME_ANALYTICS_EVENT_INC_EXP		"inc_exp"			// 经验值增加
#define  GAME_ANALYTICS_EVENT_LEVELUP		"levelup"			// 玩家升级
#define  GAME_ANALYTICS_EVENT_VIPUP			"vip_up"			// vip升级
#define  GAME_ANALYTICS_EVENT_INC_COIN		"inc_coin"			// 增加货币
#define  GAME_ANALYTICS_EVENT_DEC_COIN		"dec_coin"			// 消耗货币
#define  GAME_ANALYTICS_EVENT_GAIN_ITEM		"gain_item"			// 获得物品
#define  GAME_ANALYTICS_EVENT_COST_ITEM		"cost_item"			// 消耗物品
#define  GAME_ANALYTICS_EVENT_PROPERTY_CHANGED "property_change"// 属性值变化
#define  GAME_ANALYTICS_EVENT_TRANSZCTION	"transaction"		// 交易事件
#define  GAME_ANALYTICS_EVENT_GUIDE			"guide"				// 引导事件
#define  GAME_ANALYTICS_EVENT_MERIDIAN_UP	"meridian_up"		// 内力经脉升级

#define  GAME_ANALYTICS_EVENT_TASK_ACCEPT	"task_accept"		// 任务开始
#define  GAME_ANALYTICS_EVENT_TASK_COMPLETE	"task_complete"		// 任务完成
#define  GAME_ANALYTICS_EVENT_TASK_SUBMIT	"task_give"		// 任务提交

#define  GAME_ANALYTICS_EVENT_COPY_SCENE_OPEN	"cp_scene_open"		// 副本开启
#define  GAME_ANALYTICS_EVENT_COPY_SCENE_ENTRY	"cp_scene_entry"	// 进入副本
#define  GAME_ANALYTICS_EVENT_COPY_SCENE_EXIT	"cp_scene_exit"		// 退出副本

#define  GAME_ANALYTICS_EVENT_VISIT_FUNCTION	"visit_func"		// 访问功能

#define  GAME_ANALYTICS_EVENT_ONLINE_COUNT		"online_count"	// 在线人数


// 扩展事件
#define GAME_EXTRA_EVENT_SKILL_LEVELUP	"skill_levelup"		// 技能升级

// 任务
#define GAME_EXTRA_EVENT_TASK_OP	"task_op"				// 任务操作
#define GAME_EXTRA_EVENT_TASK_OP_ONE_KEY	"task_op_onekey"// 任务一鍵完成
#define GAME_EXTRA_EVENT_TASK_OP_REFRESH_STAR	"task_op_refresh_star"// 刷新星級任務
#define GAME_EXTRA_EVENT_TASK_OP_REFRESH	"task_op_refresh"// 刷新任務
#define GAME_EXTRA_EVENT_ACHIEVE	"achieve_completed"		// 成就完成
#define GAME_EXTRA_EVENT_ACHIEVE_REWARD	"achieve_reward"	// 成就奖励领取

// 好友
#define GAME_EXTRA_EVENT_FRIEND_OP	"friend_op"				// 好友操作
#define GAME_EXTRA_EVENT_SND_GIFT	"friend_snd_gift"		// 好友送花
#define GAME_EXTRA_EVENT_RCV_GIFT	"friend_rcv_gift"		// 好友收花
#define GAME_EXTRA_EVENT_TEAM_FIGHT "friend_team_fight"		// 好友组队打怪增加亲密度
#define GAME_EXTRA_EVENT_CAHRMS_CHANGED "charms_changed"	// 魅力值发生变化
#define GAME_EXTRA_EVENT_SWORN		"friend_sworn"			// 好友结义结缘
//装备
#define GAME_EXTRA_EVENT_EQUIP_STRENGTHEN "equip_strengthen" //装备强化
#define GAME_EXTRA_EVENT_EQUIP_FORGE "equip_forge"				  //装备锻造
#define GAME_EXTRA_EVENT_EQUIP_BAPTISE "equip_baptise"			//装备洗练
#define GAME_EXTRA_EVENT_EQUIP_SMELT "equip_smelt"				//熔炼

//宝石功能
#define GAME_EXTRA_EVENT_JEWEL_INLAY	"jewel_inlay"				//宝石镶嵌
#define GAME_EXTRA_EVENT_JEWEL_UP		"jewel_up"					//宝石升级
#define GAME_EXTRA_EVENT_JEWEL_DISMANTLE "jewel_dismantle"		//宝石卸下
// 公会
#define GAME_EXTRA_EVENT_GUILD_EVENT		"guild_event"		// 公会操作事件
#define GAME_EXTRA_EVENT_GUILD_PROMOTION	"guild_promotion"	// 公会成员升级
#define GAME_EXTRA_EVENT_GUILD_DEMOTION		"guild_demotion"	// 公会成员降职
#define GAME_EXTRA_EVENT_GUILD_DONATE		"guild_donate"		// 公会捐献
#define GAME_EXTRA_EVENT_GUILD_DONATE_COST	"guild_donate_cost"	// 帮贡消耗

// 宠物
#define GAME_EXTRA_EVENT_PET_ACTIVE			"pet_active"		// 下属激活
#define GAME_EXTRA_EVENT_PET_UPGRADE		"pet_upgrade"		// 下属升级
#define GAME_EXTRA_EVENT_PET_SKILLUP		"pet_skill_up"		// 下属技能升级

// 坐骑
#define GAME_EXTRA_EVENT_RIDE_ACTIVE		"ride_active"		// 坐骑激活
#define GAME_EXTRA_EVENT_RIDE_FEED			"ride_feed"			// 坐骑饲养
#define GAME_EXTRA_EVENT_RIDE_UPGRADE		"ride_upgrade"		// 坐骑升级
#define GAME_EXTRA_EVENT_RIDE_SKILL_UPGRADE	"ride_skill_upgrade"// 坐骑技能升级
#define GAME_EXTRA_EVENT_RIDE_SKIN			"ride_skin"			// 坐骑皮肤

// 爵位
#define GAME_EXTRA_EVENT_PEERAGE_UPGRADE	"peerage_upgrade"	// 爵位升级

// 押囚
#define GAME_EXTRA_EVENT_ESCORT_START		"escort_start"		// 押囚开始
#define GAME_EXTRA_EVENT_ESCORT_END			"escort_end"		// 押囚结束

// 组队悬赏
#define GAME_EXTRA_EVENT_TEAMOFFER_END			"teamoffer_end"		// 组队悬赏结束

// 印记
#define GAME_EXTRA_EVENT_MARKER_EXCH		"marker_exch"		// 印记兑换
#define GAME_EXTRA_EVENT_MARKER_ACTIVE		"marker_active"		// 印记激活
#define GAME_EXTRA_EVENT_MARKER_BAPTISE		"marker_baptise"	// 印记洗炼
#define GAME_EXTRA_EVENT_MARKER_LEVELUP		"marker_levelup"	// 印记升级
//公会舞姬
#define GAME_EXTRA_EVENT_AWARD_DANCING_GIRL   "award_girl"      // 打赏舞姬
#define GAME_EXTRA_EVENT_SUMMER_DANCING_GIRL  "summer_girl"		 //召唤舞姬
//秘境军令购买
#define GAME_EXTRA_EVENT_BUY_CLONE_COIN     "buy_clone_coin"    //购买军令
// 竞技场
#define GAME_EXTRA_EVENT_ARENA_GAME_COST	"arena_game_cost"	// 竞技场消耗   LOG_ARENA_COST_TYPE
#define GAME_EXTRA_EVENT_ARENA_GAME_RANK	"arena_game_rank"	// 竞技场排名变化

// 悬赏
#define GAME_EXTRA_EVENT_OFFER_REWARD_ISSUE		"offer_reward_issue"	// 悬赏发布玩家事件
#define GAME_EXTRA_EVENT_OFFER_REWARD_ACCEPT	"offer_reward_accpet"	// 悬赏领取玩家事件

// 商店
#define GAME_EXTRA_EVENT_SHOP_REFRESH			"shop_refresh"				// 商店刷新
#define GAME_EXTRA_EVENT_SHOP_EXCHANGE			"shop_exchange"				// 商品兑换 
#define GAME_EXTRA_EVENT_SHOP_BUY_WELFARE_CARD	"shop_buy_welfare_card"		// 购买福利卡

// 生死擂
#define GAME_EXTRA_EVENT_RA_CHALLENGE		"ra_challenge"		// 挑战生死擂
#define GAME_EXTRA_EVENT_RA_SAY_GOOD		"ra_say_good"		// 挑战生叫好

// 阵营
#define GAME_EXTRA_EVENT_SELECT_CAMP		"select_camp"		// 选择阵营
// 交易
#define  GAME_EXTRA_EVENT_AUCTION_TRANS		"auct_trans"		// 交易行交易

//师徒
#define GAME_EXTRA_MASTER_APRTC_ADD		"masteraprtc_add"   // 添加师傅徒弟
#define GAME_EXTRA_MASTER_APRTC_DEL		"masteraprtc_del"   // 删除师傅徒弟
#define GAME_EXTRA_MASTER_ATRTC_FINISH	"masteraptrtc_finish" //出师

//引导场景
#define GAME_EXTRA_EVENT_SKIP_UP_START_SCENE		"skip_up_start_scene"			// 跳过引导场景

// 禁言/封号
#define GAME_EXTRA_EVENT_FORBID_SPEAK		"forbid_speak"		// 禁言
#define GAME_EXTRA_EVENT_ALLOW_SPEAK		"allow_speak"		// 解除禁言
#define GAME_EXTRA_EVENT_BLOCK_PLAYER		"block_player"		// 封号
#define GAME_EXTRA_EVENT_RESUME_PLAYER		"resume_player"		// 解除封号

//limit scene

#define GAME_EXTRA_EVENT_ENTRY_FRO_LIMIT_SCENE			  "fro_entry_limit_scene"//进入限时场景
#define GAME_EXTRA_EVENT_FRO_GET_LIMIT_SCENE_AWARD        "fro_get_limit_scene_award"//获得场景宝箱奖励

//黑夜城
#define GAME_EXTRA_DARD_CITY_KILL_BOSS		"dard_city_kill_boss"	// 黑夜城击杀BOSS

// 夜袭宝库
#define GAME_EXTRA_FRONTIER_TREASURY_AUCTION "frontier_treas_auction"	// 拍卖竞价获得

// 限时礼包
#define GAME_EXTRA_LIMIT_GIFT_BOUGHT "limit_gift_bought"	// 购买限时礼包
// 成长基金
#define GAME_EXTRA_FUND_BOUGHT		"fund_bought"	// 购买成长基金
// 七日礼包
#define GAME_EXTRA_SEVEN_GIFT_BOUGHT "seven_gift_bought"	// 购买七日礼包

#endif