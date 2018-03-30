//--------------------------------------------------------------------
// 文件名:	GameAnalyticsProperties.h
// 内  容:	数据属性宏定义
// 说  明:	
// 创建日期:	2016年08月24日
// 创建人:	李海罗
// 整理人:            
//--------------------------------------------------------------------
#ifndef __GAME_ANALYTICS_PROPERTIES_DEFINED_H__
#define __GAME_ANALYTICS_PROPERTIES_DEFINED_H__

#define GAME_ANALYTICS_PROPERTIES_PROJECT		"project"		// 项目名
#define GAME_ANALYTICS_PROPERTIES_CHANNEL		"channelid"		// 渠道
#define GAME_ANALYTICS_PROPERTIES_GAME_NAME		"gamename"		// 游戲名
#define GAME_ANALYTICS_PROPERTIES_DEPLOY_ID		"deployid"		// 布署id
#define GAME_ANALYTICS_PROPERTIES_GAME_ID		"gameid"		// 游戏id
#define GAME_ANALYTICS_PROPERTIES_GAME_VER		"gamever"		// 游戏版本
#define GAME_ANALYTICS_PROPERTIES_DISTRICT_NAME	"districtname"	// 游戏分区名
#define GAME_ANALYTICS_PROPERTIES_SERVER_ID		"srvid"			// 服务器id
#define GAME_ANALYTICS_PROPERTIES_DISTRICT_ID	"districtid"	// 分区id
#define GAME_ANALYTICS_PROPERTIES_PRODUCTION_ID	"productionid"	// 生产id
#define GAME_ANALYTICS_PROPERTIES_TIMESTAMP		"timestamp"		// 时间戳
#define GAME_ANALYTICS_PROPERTIES_DATE			"rdate"			// 日期时间
#define GAME_ANALYTICS_PROPERTIES_ONLINETIME	"onlinetime"	// 在线时长
#define GAME_ANALYTICS_PROPERTIES_CTIME			"ctime"			// 创建时间
#define GAME_ANALYTICS_PROPERTIES_LTIME			"ltime"			// 登陆时间
#define GAME_ANALYTICS_PROPERTIES_QTIME			"qtime"			// 退出时间
#define GAME_ANALYTICS_PROPERTIES_CLIENT_VER	"clientver"		// 客户端版本
#define GAME_ANALYTICS_PROPERTIES_IS_REGISTER	"isregister"	// 是否注冊

#define GAME_ANALYTICS_PROPERTIES_ONLINE_COUNT	"onlinecount"	// 在线人数


#define GAME_ANALYTICS_PROPERTIES_UID			"uid"			// 账号id
#define GAME_ANALYTICS_PROPERTIES_ANID			"anid"			// 匿名id
#define GAME_ANALYTICS_PROPERTIES_NICK			"nick"			// 昵称
#define GAME_ANALYTICS_PROPERTIES_SEX			"sex"			// 性别
#define GAME_ANALYTICS_PROPERTIES_BORN			"bron"			// 出生日期
#define GAME_ANALYTICS_PROPERTIES_REGISTER_IP	"register_ip"	// 注册IP
#define GAME_ANALYTICS_PROPERTIES_LOGIN_IP		"login_ip"		// 最后登陆IP
#define GAME_ANALYTICS_PROPERTIES_PLATFORM		"platform"		// 注册平台
#define GAME_ANALYTICS_PROPERTIES_WIFI			"wifi"			// wifi
#define GAME_ANALYTICS_PROPERTIES_MANUFACTURER	"manufacturer"	// 制造商
#define GAME_ANALYTICS_PROPERTIES_DEVICE_ID		"device_id"		// 设备唯一标识
#define GAME_ANALYTICS_PROPERTIES_MODEL			"model"			// 终端类型
#define GAME_ANALYTICS_PROPERTIES_OS			"os"			// 终端操作系统
#define GAME_ANALYTICS_PROPERTIES_OS_VER		"os_ver"		// 操作系统版本
#define GAME_ANALYTICS_PROPERTIES_ONLINE		"online"		// 在线标志


#define GAME_ANALYTICS_PROPERTIES_ROLE_ID		"roleid"		// 角色id
#define GAME_ANALYTICS_PROPERTIES_ROLE_NAME		"rolename"		// 角色名
#define GAME_ANALYTICS_PROPERTIES_ROLE_JOB		"job"			// 角色职业
#define GAME_ANALYTICS_PROPERTIES_ROLE_FLAG		"flag"			// 角色标志
#define GAME_ANALYTICS_PROPERTIES_ROLE_CHARMS	"charms"		// 魅力值
#define GAME_ANALYTICS_PROPERTIES_ROLE_POWER	"power"			// 战力值
#define GAME_ANALYTICS_PROPERTIES_ROLE_PAYED	"payed"			// 充值元宝数



#define GAME_ANALYTICS_PROPERTIES_PAY_ORDER_ID		"pay_orderid"		// 订单id
#define GAME_ANALYTICS_PROPERTIES_PAY_ITEM_ID		"pay_itemid"		// 物品id
#define GAME_ANALYTICS_PROPERTIES_PAY_ITEM_COUNT	"pay_item_count"	// 物品数量
#define GAME_ANALYTICS_PROPERTIES_PAY_ITEM_PRICE	"pay_item_price"	// 物品价格
#define GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT		"pay_amount"		// 支付金额
#define GAME_ANALYTICS_PROPERTIES_PAY_TOTAL_AMOUNT	"pay_total_amount"	// 总支付金额
#define GAME_ANALYTICS_PROPERTIES_PAY_REAL_AMOUNT	"pay_real_amount"	// 真实支付金额
#define GAME_ANALYTICS_PROPERTIES_PAY_VIRTUAL_AMOUNT "pay_virtual_amount"	// 虚拟货币支付金额
#define GAME_ANALYTICS_PROPERTIES_PAY_RESULT		"pay_result"		// 支付结果
#define GAME_ANALYTICS_PROPERTIES_PAY_PLATFORM		"pay_platform"		// 支付平台
#define GAME_ANALYTICS_PROPERTIES_PAY_REASON		"pay_reason"		// 支付失败原因
#define GAME_ANALYTICS_PROPERTIES_PAY_FIRST			"pay_first"			// 首充
#define GAME_ANALYTICS_PROPERTIES_PAY_TIMES			"pay_times"			// 充值次数
#define GAME_ANALYTICS_PROPERTIES_PAY_LAST_TIME		"pay_last_time"		// 最后充值时间
#define GAME_ANALYTICS_PROPERTIES_PAY_AMOUNT_1		"pay_vir_amount"	// 虚拟货币支付金额


#define GAME_ANALYTICS_PROPERTIES_GAME_MODULE	"game_module"	// 游戏模块
#define GAME_ANALYTICS_PROPERTIES_GAME_ACTION	"game_action"	// 游戏动作
#define GAME_ANALYTICS_PROPERTIES_EXTRA_PARAM	"extra_param"	// 额外参数

#define GAME_ANALYTICS_PROPERTIES_EXP			"exp"			// 经验值
#define GAME_ANALYTICS_PROPERTIES_INC_EXP		"inc_exp"		// 增加的经验值
#define GAME_ANALYTICS_PROPERTIES_ORIG_EXP		"orig_exp"		// 原始的经验值
#define GAME_ANALYTICS_PROPERTIES_LEVEL			"level"			// 玩家等级
#define GAME_ANALYTICS_PROPERTIES_SILVER		"silver"		// 玩家元宝
#define GAME_ANALYTICS_PROPERTIES_COPPER		"copper"		// 玩家铜币
#define GAME_ANALYTICS_PROPERTIES_SMELT			"smelt"			// 玩家熔炼值
#define GAME_ANALYTICS_PROPERTIES_EXPLOIT		"exploit"		// 玩家功勋值
#define GAME_ANALYTICS_PROPERTIES_SOUL			"soul"			// 玩家战魂
#define GAME_ANALYTICS_PROPERTIES_HONOR			"honor"			// 玩家荣誉
#define GAME_ANALYTICS_PROPERTIES_CHIVALRY		"chivalry"		// 玩家侠义值
#define GAME_ANALYTICS_PROPERTIES_TRADE_SILVER	"trade_silver"	// 交易银元
#define GAME_ANALYTICS_PROPERTIES_CAMPVALUE		"camp_value"	// 阵营值
#define GAME_ANALYTICS_PROPERTIES_WISHSCORE		"wish_score"	// 祈福积分
#define GAME_ANALYTICS_PROPERTIES_GUILD_CURRENCY "guild_currency"	// 组织贡献

#define GAME_ANALYTICS_PROPERTIES_PRE_LEVEL		"pre_level"		// 玩家上一个等级
#define GAME_ANALYTICS_PROPERTIES_MERIDIAN_LEVEL	"meridian_level"		// 玩家经脉等级
#define GAME_ANALYTICS_PROPERTIES_MERIDIAN_PRE_LEVEL "meridian_pre_level"	// 玩家经脉上一个等级

#define GAME_ANALYTICS_PROPERTIES_COIN_ORIGINAL		"coin_orignial"	// 原始值
#define GAME_ANALYTICS_PROPERTIES_COIN_NEW_VALUE	"con_newvalue"	// 新值
#define GAME_ANALYTICS_PROPERTIES_COIN_VALUE		"coin_value"	// 消耗或增加值 
#define GAME_ANALYTICS_PROPERTIES_COIN_ID			"coin_id"		// 货币id

#define GAME_ANALYTICS_PROPERTIES_VIP		"vip"					// 玩家vip
#define GAME_ANALYTICS_PROPERTIES_PRE_VIP	"pre_vip"				// 玩家上一个vip


#define GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY		"player_property"		// 属性
#define GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_VALUE	"player_value"			// 属性值
#define GAME_ANALYTICS_PROPERTIES_PLAYER_PROPERTY_PRE_VALUE	"player_pre_value"	// 属性原来值

#define GAME_ANALYTICS_PROPERTIES_ITEM_OLD_ID		"item_old_id"	// 原物品ID
#define GAME_ANALYTICS_PROPERTIES_ITEM_ID		"item_id"	// 物品ID
#define GAME_ANALYTICS_PROPERTIES_ITEM_NAME		"item_name"	// 物品名称
#define GAME_ANALYTICS_PROPERTIES_ITEM_COUNT	"item_count"// 物品数量
#define GAME_ANALYTICS_PROPERTIES_ITEM_NUM		"item_num"	// 物品变动数
#define GAME_ANALYTICS_PROPERTIES_ITEM_ORIG_COUNT	"item_orig_count"	// 物品变动前数量
#define GAME_ANALYTICS_PROPERTIES_ITEM_COLOR	"item_color"	//物品品质
#define GAME_ANALYTICS_PROPERTIES_ITEM_PRICE	"item_price"	//价格
#define GAME_ANALYTICS_PROPERTIES_ITEM_PROPS	"item_props"	//物品属性
#define GAME_ANALYTICS_PROPERTIES_ITEM_LIST		"item_list"		//物品列表

#define GAME_ANALYTICS_PROPERTIES_TRANSID		"trans_id"		// 交易id
#define GAME_ANALYTICS_PROPERTIES_TRANS_TYPE	"trans_type"	// 交易类型
#define GAME_ANALYTICS_PROPERTIES_SENDER		"sender"		// 发起方
#define GAME_ANALYTICS_PROPERTIES_RECIVER		"reciver"		// 接受方
#define GAME_ANALYTICS_PROPERTIES_SENDER_ITEM	"sender_item"	// 发送方物品
#define GAME_ANALYTICS_PROPERTIES_RECIVER_ITEM	"reciver_item"	// 接收方物品
#define GAME_ANALYTICS_PROPERTIES_SENDER_COUNT	"sender_count"	// 发送方物品数量
#define GAME_ANALYTICS_PROPERTIES_RECIVER_COUNT	"reciver_count"	// 接收方物品数量

#define GAME_ANALYTICS_PROPERTIES_GUIDE_ID		"guide_id"		// 引导id
#define GAME_ANALYTICS_PROPERTIES_GUIDE_STEP	"guide_step"	// 引导步
#define GAME_ANALYTICS_PROPERTIES_TASK_ID		"task_id"		// 任务id
#define GAME_ANALYTICS_PROPERTIES_TASK_NAME		"task_name"		// 任务名称
#define GAME_ANALYTICS_PROPERTIES_TASK_TYPE		"task_type"		// 任务类型
#define GAME_ANALYTICS_PROPERTIES_TASK_OPT_WAY	"task_opt_way"	// 操作类型
#define GAME_ANALYTICS_PROPERTIES_TASK_STATE	"task_state"	// 任务状态
#define GAME_ANALYTICS_PROPERTIES_BEFORE_STAR	"before_star"	// 刷星前星级
#define GAME_ANALYTICS_PROPERTIES_AFTER_STAR	"after_star"	// 刷星后星级
#define GAME_ANALYTICS_PROPERTIES_AFTER_TASK	"after_task"	// 刷星后任务
#define GAME_ANALYTICS_PROPERTIES_BEFORE_TASK	"before_task"	// 刷星前任务

#define GAME_ANALYTICS_PROPERTIES_SCENE_ID			"scene_id"			// 副本id
#define GAME_ANALYTICS_PROPERTIES_SCENE_NAME		"scene_name"		// 副本名
#define GAME_ANALYTICS_PROPERTIES_SCENE_CHAPTER		"sccene_chapter"	// 副本章节id
#define GAME_ANALYTICS_PROPERTIES_SCENE_TYPE		"sccene_type"		// 副本类型
#define GAME_ANALYTICS_PROPERTIES_SCENE_RESULT		"sccene_result"		// 副本结果
#define GAME_ANALYTICS_PROPERTIES_SCENE_DIED_NUM	"sccene_died_num"	// 副本中死亡次数
#define GAME_ANALYTICS_PROPERTIES_SCENE_REASON		"sccene_reason"		// 副本失败原因
#define GAME_ANALYTICS_PROPERTIES_SCENE_STAY_TIME	"scene_stay_time"	// 场景停留时间

#define GAME_ANALYTICS_PROPERTIES_VISIT_RESULT	"visit_result"	// 功能访问结果
#define GAME_ANALYTICS_PROPERTIES_VISIT_VALUE	"visit_value"	// 功能访问值


//-------------------------------------------------------------------
// 通用属性
//-------------------------------------------------------------------
#define GAME_ANALYTICS_PROPERTIES_COMM_NUMBER		"comm_number"		// 数量
#define GAME_ANALYTICS_PROPERTIES_COMM_LEVEL		"comm_level"		// 当前等级
#define GAME_ANALYTICS_PROPERTIES_COMM_PRE_LEVEL	"comm_pre_level"	// 原始等级
#define GAME_ANALYTICS_PROPERTIES_COMM_CONFIG_ID	"config_id"			// 配置ID
#define GAME_ANALYTICS_PROPERTIES_COMM_C_VALUE		"comm_cvalue"		// 改变值
#define GAME_ANALYTICS_PROPERTIES_COMM_REWARD 		"comm_reward"		// 奖励
#define GAME_ANALYTICS_PROPERTIES_COMM_INDEX 		"comm_index"		// 索引id



//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 扩展属性
//-------------------------------------------------------------------
// 技能
#define GAME_EXTRA_PROPERTIES_SKILL_ID		"skillid"		// 技能id
#define GAME_EXTRA_PROPERTIES_SKILL_NAME	"skill_name"	// 技能名称
#define GAME_EXTRA_PROPERTIES_SKILL_LEVEL		"skill_level"		// 技能等级
#define GAME_EXTRA_PROPERTIES_SKILL_ORIGlEVEL	"skill_orig_level"	// 技能升级前等级
#define GAME_EXTRA_PROPERTIES_SKILL_PASSIVE	"skill_passive"			// 是否被动技能

// 成就
#define GAME_EXTRA_PROPERTIES_ACHIEVE_ID	"chieve_id"			// 成就id
#define GAME_EXTRA_PROPERTIES_ACHIEVE_NAME	"chieve_name"		// 成就名

// 好友
#define GAME_EXTRA_PROPERTIES_FRIEND_SELF_NAME		"frid_self_name"	// 操作者姓名
#define GAME_EXTRA_PROPERTIES_FRIEND_NAME			"frid_name"			// 好友名称
#define GAME_EXTRA_PROPERTIES_FRIEND_RID			"frid_rid"			// 好友角色id
#define GAME_EXTRA_PROPERTIES_FRIEND_GIFT_NUM		"frid_gift_num"		// 好友礼物数量
#define GAME_EXTRA_PROPERTIES_FRIEND_INTLEVEL		"frid_int_level"	// 好友亲密度等级
#define GAME_EXTRA_PROPERTIES_FRIEND_INC_INTIMACY	"frid_inc_intimacy"	// 好友增加亲密度
#define GAME_EXTRA_PROPERTIES_FRIEND_IS_SAME_SEX	"frid_is_same_sex"  //与好友是否是同一性别
#define GAME_EXTRA_PROPERTIES_FRIEND_IS_SWORN_ADD	"frid_is_sworn_add"  //1:结义结缘 -1：删除结义结缘

// 公会
#define GAME_EXTRA_PROPERTIES_GUILD_NAME		"guild_name"		// 公会名
#define GAME_EXTRA_PROPERTIES_GUILD_LEVEL		"guild_level"		// 公会等级
#define GAME_EXTRA_PROPERTIES_GUILD_OP_TARGET	"guild_op_target"	// 公会操作目标
#define GAME_EXTRA_PROPERTIES_GUILD_POSITION	"guild_position"	// 公会成员职位
#define GAME_EXTRA_PROPERTIES_GUILD_DONATE_NUM	"guild_donate_num"	// 帮项值
#define GAME_EXTRA_PROPERTIES_GUILD_COST_VALUE	"guild_cost_value"	// 帮项花费值值

// 下属
#define GAME_EXTRA_PROPERTIES_PET_LEVEL			"pet_level"			// 下属等级
#define GAME_EXTRA_PROPERTIES_PET_LAST_LEVEL	"pet_last_level"	// 升级后等级
#define GAME_EXTRA_PROPERTIES_PET_STEP			"pet_step"			// 下属阶级
#define GAME_EXTRA_PROPERTIES_PET_LAST_STEP		"pet_last_step"		// 升阶后阶级
#define GAME_EXTRA_PROPERTIES_PET_STAR			"pet_star"			// 下属星级
#define GAME_EXTRA_PROPERTIES_PET_LAST_STAR		"pet_last_star"		// 升星后星级
#define GAME_EXTRA_PROPERTIES_PET_COLOR			"pet_color"			// 下属品质
#define GAME_EXTRA_PROPERTIES_PET_SKILL_POINT	"pet_skill_point"	// 下属技能点



// 坐骑
#define GAME_EXTRA_PROPERTIES_RIDE_STEP			"ride_step"			// 坐骑阶级
#define GAME_EXTRA_PROPERTIES_RIDE_LAST_STEP	"ride_last_step"	// 升阶后阶级
#define GAME_EXTRA_PROPERTIES_RIDE_STAR			"ride_star"			// 坐骑星级
#define GAME_EXTRA_PROPERTIES_RIDE_LAST_STAR	"ride_last_star"	// 升星后星级
#define GAME_EXTRA_PROPERTIES_RIDE_ADD_FEED		"ride_add_feed"		// 增加饲养值
#define GAME_EXTRA_PROPERTIES_RIDE_SKIN_ID		"ride_skin_id"		// 坐骑皮肤ID

// 爵位
#define GAME_EXTRA_PROPERTIES_PEERAGE_LVL		"peerage_lvl"		// 爵位等级
#define GAME_EXTRA_PROPERTIES_PEERAGE_LAST_LVL	"peerage_last_lvl"	// 升级后等级
#define GAME_EXTRA_PROPERTIES_PEERAGE_CONSUME_EXPLOIT	"peerage_consume_exloit"	// 升级消耗功勋

// 押囚
#define GAME_EXTRA_PROPERTIES_ESCORT_ID			"escort_id"			// 押囚id
#define GAME_EXTRA_PROPERTIES_ESCORT_WORK		"escort_work"		// 押囚职位
#define GAME_EXTRA_PROPERTIES_ESCORT_NPC_ID		"escort_npc_id"		// 囚车id
#define GAME_EXTRA_PROPERTIES_ESCORT_COLOR		"escort_color"		// 囚车品质
#define GAME_EXTRA_PROPERTIES_ESCORT_STARTTIEM	"escort_starttime"	// 押送开始时间
#define GAME_EXTRA_PROPERTIES_ESCORT_ENDTIEM	"escort_endtime"	// 押送结束时间
#define GAME_EXTRA_PROPERTIES_ESCORT_RESULT		"escort_result"		// 押囚结果

// 组队悬赏
#define GAME_EXTRA_PROPERTIES_TEAMOFFER_LEVEL		"teamoffer_level"		// 组队悬赏等级
#define GAME_EXTRA_PROPERTIES_TEAMOFFER_NUM			"teamoffer_num"			// 组队悬赏次数
#define GAME_EXTRA_PROPERTIES_TEAMOFFER_PASSTIME	"teamoffer_passtime"	// 组队悬赏通关时间
#define GAME_EXTRA_PROPERTIES_TEAMOFFER_RESULT		"teamoffer_result"		// 组队悬赏结果

// 印记
#define GAME_EXTRA_PROPERTIES_MARKER_ID			"marker_id"			// 印记id
//装备
#define GAME_EXTRA_PROPERTIES_EQUIP_NOW_LEVEL	"equip_now_level"   //装备现在等级
#define GAME_EXTRA_PROPERTIES_EQUIP_OLD_LEVEL	"equip_old_level"   //旧装备等级
#define GAME_EXTRA_PROPERTIES_EQUIP_POS			"equip_pos"			//裝备位置
#define GAME_EXTRA_PROPERTIES_SMELT_POINT		"smelt_point"		//熔炼值
#define GAME_EXTRA_PROPERTIES_SOUL_AMOUNT		"soul_amount"		//器魂


//宝石
#define GAME_EXTRA_PROPERTIES_JEWEL_MATERIAL	"jewel_material"		// 宝石合成材料
#define GAME_EXTRA_PROPERTIES_JEWEL_MATERIAL_NUM	"jewel_material_num"	// 宝石合成材料数量
#define GAME_EXTRA_PROPERTIES_JEWEL_TYPE	"jewel_type"	//宝石类型

//洗练附加属性
#define GAME_EXTRA_BAPTISE_INDEX		"baptise_index"				//装备格子索引,byte,(1-10)
#define GAME_EXTRA_BAPTISE_PROP_ONE 	"baptisepropone"			//对应装备格子的第一个属性的洗炼值,int
#define GAME_EXTRA_BAPTISE_PROP_TWO 	"baptiseproptwo"			//对应装备格子的第二个属性的洗炼值,int
#define GAME_EXTRA_BAPTISE_PROP_THREE  	"baptisepropthree"		   //对应装备格子的第三个属性的洗炼值,int
#define GAME_EXTRA_BAPTISE_PROP_FOUR 	"baptisepropfour"			//对应装备格子的第四个属性的洗炼值,int

//舞姬

#define GAME_EXTRA_GIRL_AWARD_EXP_PRCE 	"girl_exp_prce"					//打赏经验百分比
//npc
#define GAME_EXTRA_PROPERTIES_NPC_ID				"npc_id"						//npcid

// 商店
#define GAME_EXTRA_SHOP_TYPE 			"shop_type"					// 商店类型
#define GAME_EXTRA_WELFARE_CARD_TYPE 	"shop_welfare_card_type"	// 福利卡类型


// 竞技场
#define GAME_EXTRA_ARENA_COST_TYPE 		"arena_cost_type"				// 竞技场消耗类型
#define GAME_EXTRA_ARENA_PRE_RANK 		"arena_pre_rank"				// 竞技场旧排名
#define GAME_EXTRA_ARENA_CUR_RANK 		"arena_cur_rank"				// 竞技场当前排名

// 悬赏系统
#define GAME_EXTRA_OR_TYPE 				"or_type"						// 悬赏任务的类型
#define GAME_EXTRA_OR_TASK_TARGET_UID 	"or_task_target_uid"			// 悬赏目标的uid
#define GAME_EXTRA_OR_TASK_ID 			"or_task_id"					// 悬赏任务的id
#define GAME_EXTRA_OR_TASK_ISSUER_UID 	"or_task_issuer_uid"			// 悬赏发布者的uid

// 生死擂
#define GAME_EXTRA_RA_TARGET_UID 		"ra_target_uid"					// 目标id
#define GAME_EXTRA_RA_RESULT 			"ra_result"						// 挑战结果
#define GAME_EXTRA_RA_ARENA_ID 			"ra_arena_id"					// 场次id

// 阵营
#define GAME_EXTRA_CAMP_ID				"camp"							// 阵营
#define GAME_EXTRA_CAPM_SEL_RANDOM		"camp_sel_random"				// 随机阵营选择

// 交易
#define GAME_EXTRA_TRANSCTION_TARGET	"transction_target"				// 交易目标
#define GAME_EXTRA_TRANSCTION_TAX		"transction_tax"				// 交易税

//师徒
#define GAME_EXTRA_MASTER_APRTC_TARGET_NAME "masteraprtc_target_name" //师徒对方名字
#define GAME_EXTRA_MASTER_APRTC_IDENTITY "masteraprtc_identity"     //玩家身份

// 禁言/封号
#define GAME_EXTRA_DURATION_TIME		"duration_time"				//持续时长
#define GAME_EXTRA_FORBID_EXPIRED_TIME  "forbid_expired_time"		//禁言过期时间
#define GAME_EXTRA_BLOCK_EXPIRED_TIME  "block_expired_time"			//封号过期时间

//黑夜城boss
#define GAME_EXTRA_DC_BOSS_ID			"dc_boss_id"				//黑夜城bossid
#define GAME_EXTRA_DC_DROP_TYPE			"dc_drop_type"				//黑夜城boss奖励类型(0安慰包1主掉落包)

#endif