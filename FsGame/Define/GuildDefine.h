//--------------------------------------------------------------------
// 文件名:		GuildDefine.h
// 内  容:		公会系统-表格及类型定义
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __GuildDefine_H__
#define __GuildDefine_H__

#include <vector>
#include <string>
#include <set>
#include <map>
#include <functional>
#include "utils/util_config.h"
// 公会相关字符串定义
#define GUILD_CUSTOM_STRING_1    "ui_form_guild_1"   // 处于退会冷却时间，无法申请或者创建帮会
#define GUILD_CUSTOM_STRING_2    "ui_form_guild_2"   // 帮会数量已达上限
#define GUILD_CUSTOM_STRING_3    "ui_form_guild_3"   // 此功能30级开放
#define GUILD_CUSTOM_STRING_4    "ui_form_guild_4"   // 玩家已加入帮会
#define GUILD_CUSTOM_STRING_5    "ui_form_guild_5"   // 帮会名称重复，无法创建
#define GUILD_CUSTOM_STRING_6    "ui_form_guild_6"   // 帮会名称包含特殊符号或非法字符
#define GUILD_CUSTOM_STRING_7    "ui_form_guild_7"   // 元宝不足
#define GUILD_CUSTOM_STRING_8    "ui_form_guild_8"   // 玩家申请帮会数量已达上限
#define GUILD_CUSTOM_STRING_9    "ui_form_guild_9"   // 已经申请过该帮会
#define GUILD_CUSTOM_STRING_10   "ui_form_guild_10"  // 该帮会申请列表已满
#define GUILD_CUSTOM_STRING_11   "ui_form_guild_11"  // 公告包含非法字符
#define GUILD_CUSTOM_STRING_12   "ui_form_guild_12"  // 帮会名称为空
#define GUILD_CUSTOM_STRING_13   "ui_form_guild_13"  // 玩家不在线
#define GUILD_CUSTOM_STRING_14   "ui_form_guild_14"  // 邀请权限不足
#define GUILD_CUSTOM_STRING_15   "ui_form_guild_15"  // 该帮会人数已达上限
#define GUILD_CUSTOM_STRING_16   "ui_form_guild_16"  // 已邀请过该玩家
#define GUILD_CUSTOM_STRING_17   "ui_form_guild_17"  // 背包空间不足
#define GUILD_CUSTOM_STRING_18   "ui_form_guild_18"  // 不是同一个国家
#define GUILD_CUSTOM_STRING_19   "ui_form_guild_19"  // 帮会不存在
#define GUILD_CUSTOM_STRING_20   "ui_form_guild_20"  // 铜币不足
#define GUILD_CUSTOM_STRING_21   "ui_form_guild_21"  // 帮会贡献值不足
#define GUILD_CUSTOM_STRING_22   "ui_form_guild_22"  // 今日已经捐献
#define GUILD_CUSTOM_STRING_23   "ui_form_guild_23"  // 商店物品数量不足购买
#define GUILD_CUSTOM_STRING_24   "ui_form_guild_24"  // 该职位人数已达上限，职位变动失败
#define GUILD_CUSTOM_STRING_25   "ui_form_guild_25"  // xxx加入了帮会
#define GUILD_CUSTOM_STRING_26   "ui_form_guild_26"  // 审批失败，审批数据已过期
#define GUILD_CUSTOM_STRING_27   "ui_form_guild_27"  // 捐献成功
#define GUILD_CUSTOM_STRING_28   "ui_form_guild_28"  // 捐献失败
#define GUILD_CUSTOM_STRING_29   "ui_form_guild_29"  // 帮会名称字符超限
#define GUILD_CUSTOM_STRING_30   "ui_form_guild_30"  // 所在职位无权发布公告
#define GUILD_CUSTOM_STRING_31   "ui_form_guild_31"  // 无权任命
#define GUILD_CUSTOM_STRING_32   "ui_form_guild_32"  // 无权审批
#define GUILD_CUSTOM_STRING_33   "ui_form_guild_33"  // 无权踢人
#define GUILD_CUSTOM_STRING_34   "ui_form_guild_34"  // 公会活动期间 公会人员不允许变动
#define GUILD_CUSTOM_STRING_35   "ui_form_guild_35"  // 修改公告成功
#define GUILD_CUSTOM_STRING_36   "ui_form_guild_36"  // 修改公告失败
#define GUILD_CUSTOM_STRING_37   "ui_form_guild_37"  // 踢人过多,不能踢了
#define GUILD_CUSTOM_STRING_38   "ui_form_guild_38"  // 宣言包含非法字符
#define GUILD_CUSTOM_STRING_39   "ui_form_guild_39"  // 修改宣言成功
#define GUILD_CUSTOM_STRING_40   "ui_form_guild_40"  // 修改宣言失败
#define GUILD_CUSTOM_STRING_48   "ui_form_guild_48"  // 弹劾中不可执行该操作
#define GUILD_CUSTOM_STRING_63   "ui_form_guild_63"  // {0}手滑了！攻击他可获得额外活动奖励，大家不要留情喔！
#define GUILD_CUSTOM_STRING_64   "ui_form_guild_64"  // 恭喜！答对了！
#define GUILD_CUSTOM_STRING_65   "ui_form_guild_65"  // 你答错了！
#define GUILD_CUSTOM_STRING_68   "ui_form_guild_68"  // 答题超时！
#define GUILD_CUSTOM_STRING_69	 "ui_form_guild_str_69"	 //捐献次数用光了
#define GUILD_CUSTOM_STRING_70	 "ui_form_guild_str_70"	  //审批超时
#define GUILD_CUSTOM_STRING_71   "ui_form_guild_str_71"  // 无权自动同意
#define GUILD_CUSTOM_STRING_72	 "ui_form_guild_str_72"   //无权解锁技能
#define GUILD_CUSTOM_STRING_73	 "ui_form_guild_str_73"	 // 解锁技能所需公会等级不足
#define GUILD_CUSTOM_STRING_74	 "ui_form_guild_str_74"	 // 帮会帮贡不足
#define GUILD_CUSTOM_STRING_75	 "ui_form_guild_str_75"	 // 开启舞姬所需公会等级不足
#define GUILD_CUSTOM_STRING_76	 "ui_form_guild_str_76"	 // 舞姬即将开启
#define GUILD_CUSTOM_STRING_77	 "ui_form_guild_str_77"	 // 舞姬已开启过
#define GUILD_CUSTOM_STRING_78	 "ui_form_guild_str_78"	 // 不是舞姬开启时间
#define GUILD_CUSTOM_STRING_79	 "ui_form_guild_str_79"	 // 技能已解锁
#define GUILD_CUSTOM_STRING_80	 "ui_form_guild_str_80"	 // 当前组织没有该技能无法升级
#define GUILD_CUSTOM_STRING_81	 "ui_form_guild_str_81"	 // 技能升级失败贡献不足
#define GUILD_CUSTOM_STRING_82	 "ui_form_guild_str_82"	 // 个人贡献不足
#define GUILD_CUSTOM_STRING_83	 "ui_form_guild_str_83"	 // 组织技能已经到达最大等级无法升级
#define GUILD_CUSTOM_STRING_84	 "ui_form_guild_str_84"	 // 组织技能已到达玩家等级无法升级
#define GUILD_CUSTOM_STRING_85	 "ui_form_guild_str_85"	 // {0}组织拒绝了你加入公会的申请
#define GUILD_CUSTOM_STRING_86	 "ui_form_guild_str_86"	 // 互助请求冷却时间没到
#define GUILD_CUSTOM_STRING_87	 "ui_form_guild_str_87"	 // 当前互助请求不存在
#define GUILD_CUSTOM_STRING_88	 "ui_form_guild_str_88"	 // 当前互助道具已满
#define GUILD_CUSTOM_STRING_89	 "ui_form_guild_str_89"	 // 互助道具不存在
#define GUILD_CUSTOM_STRING_90	 "ui_form_guild_str_90"	 // 今日互助次数已满
#define GUILD_CUSTOM_STRING_92	 "ui_form_guild_str_92"	 //  商店没有出售当前物品请刷新
#define GUILD_CUSTOM_STRING_93	 "ui_form_guild_str_93"	 //  当前修改商店物品列表不足或超过
#define GUILD_CUSTOM_STRING_94	 "ui_form_guild_str_94"	 //  当前列表有物品等级超过了组织等级
#define GUILD_CUSTOM_STRING_95	 "ui_form_guild_str_95"	 //  出售列表保存成功
#define GUILD_CUSTOM_STRING_96	 "ui_form_guild_str_96"	 //  银元不足
#define GUILD_CUSTOM_STRING_97	 "ui_form_guild_str_97"	 //  无权更改商店出售物品
#define GUILD_CUSTOM_STRING_98	 "ui_form_guild_str_98"	 //  已经有人在挑战入侵boss 
#define GUILD_CUSTOM_STRING_99	 "ui_form_guild_str_99"	 //  有队员不在当前场景
#define GUILD_CUSTOM_STRING_100	 "ui_form_guild_str_100"  //  {0} 不知当前组织的成员
#define GUILD_CUSTOM_STRING_101	 "ui_form_guild_str_101"  //  不满足组织入侵人数
#define GUILD_CUSTOM_STRING_102	 "ui_form_guild_str_102"  //  组织短名已存在
#define GUILD_CUSTOM_STRING_103	 "ui_form_guild_str_103"  //  组织资金不足
#define GUILD_CUSTOM_STRING_105	 "ui_form_guild_str_105"  //  健康度礼包已领取
#define GUILD_CUSTOM_STRING_106	 "ui_form_guild_str_106"  //  个人组织资金已达上限
#define GUILD_CUSTOM_STRING_107	 "ui_form_guild_str_107"  //  已经有技能在研究
#define GUILD_CUSTOM_STRING_108	 "ui_form_guild_str_108"  //  技能不存在
#define GUILD_CUSTOM_STRING_109	 "ui_form_guild_str_109"  //  你没有这个权限
#define GUILD_CUSTOM_STRING_110	 "ui_form_guild_str_110"  //  已达到今日获取的个人组织资金上限
#define GUILD_CUSTOM_STRING_111	 "ui_form_guild_str_111"  //  玩家不在线不能邀请进组织
#define GUILD_CUSTOM_STRING_112	 "ui_form_guild_str_112"  //  超过每次分配最大金额





//邀请组织名
#define GUILD_INVITE_GUILD_NAME "InviteGuildName"
//组织分红邮件
#define GUILD_BONUS_MAIL "guild_bonus_mail"

#define GUILD_CAPTAIN_MAIL "guild_captain_mail"

// 公会公共数据项名称
#define GUILD_PUB_DATA L"guild_pubdata"

// 公会公共数据表(GUILD_PUB_DATA+serverId)
// 公会列表表格
#define GUILD_LIST_REC "guild_list_rec"
// 公会排序表
#define GUILD_SORT_REC "guild_sort_rec"
// 申请者列表表格
#define GUILD_APPLY_REC "guild_apply_rec"
// 记录不在线玩家加入公会的表格（用于在玩家上线时设置其加入了公会）
#define GUILD_OUTLINE_JOIN_REC "guild_outline_join_rec"
// 记录不在线玩家退出公会的表格
#define GUILD_OUTLINE_QUIT_REC "guild_outline_quit_rec"
// 公会商店道具购买计数
#define GUILD_SHOP_BUY_REC "guild_shop_buy_rec"
// 公会成员购买的道具计数
#define GUILD_MEMBER_BUY_REC "guild_member_buy_rec"

//组织符号
#define GUILD_SYSMBOL_REC "guild_symbol_rec"

// 帮会数据版本号
#define  GUILD_DATA_VERSION_ID 1
//互助活动给予物品邮件
#define GUILD_MUTUAL_GIVE_MAIL "guild_mutual_give_mail"
//踢出公会邮件
#define MAIL_GUILD_FIRE "guild_fire"
// 组织互助开启等级
#define GUILD_MUTUAL_LIMIT_LEVEL 2
#define GUILD_MUTUAL_CD_TIMES	8*60*60
// 帮会场景分组号资源表
#define GUILD_STATION_GROUP_REC "guild_station_group_rec"   
enum
{
	GUILD_STATION_GROUP_REC_COL_GUILD_NAME    = 0,        	   // 帮会名 VTYPE_WIDESTR
	GUILD_STATION_GROUP_REC_COL_GROUPID,                       // 分组ID   VTYPE_INT
	GUILD_STATION_GROUP_REC_COL_MAX,			
};

// 公会私有数据表(guildName+serverId)
// 公会成员表格
#define GUILD_MEMBER_REC "guild_member_rec"
// 申请加入公会玩家表格
#define GUILD_JOIN_REC "guild_join_rec"
// 公会聊天记录表
#define GUILD_CHAT_HISTORY_REC "guild_chat_history_rec"

// 公会虚拟表名称
#define GUILD_LIST_VIRTUALREC_NAME "Game.GuildRec"

// 公会商店重置定时器名称
#define GUILD_TIMER_NAME "guild_timer_name"
// 公会自动降级检查定时器名称
#define GUILD_DOWN_LEVEL_TIMER_NAME "guild_dnlevel_timer"


//公会数据版本
const char* const GUILD_PUB_DATA_VERSION_ID = "pub_data_version_id";
//组织设置信息
#define GUILD_SET_REC "guild_set_rec"

//息兵符时间
#define STOPPING_WAR_TIME "stopping_war_times"


// 每次请求公会列表获取公会个数
const int REQUEST_GUILD_LIST_PRE_COUNT = 15;

// 退出公会后再次申请加入或创建的冷却时间（单位分钟）
const int GUILD_JOIN_CD = 24*60;
const int  ONE_DAY_SEC = 24 * 60 * 60;
const int GUILD_ONE_KEY_JOIN_CD = 1;
// 公会名称最大长度,单位为wchar_t
const int GUILD_NAME_MAX_LENGTH = 6;
// 可申请加入公会的最大数量
const int GUILD_APPLY_MAX_NUM = 10;
// 公会公告最大长度
const int GUILD_NOTICE_MAX_LENGTH = 50;
// 宣言最大长度
const int GUILD_DECLARATION_MAX_LENGTH = 18;

// 公会列表表格行数上限
const int GUILD_LIST_REC_ROW_MAX = 10000;
// 公会排序表格行数上限
const int GUILD_SORT_REC_ROW_MAX = 10000;
// 申请者列表表格行数上限
const int GUILD_APPLY_LIST_ROW_MAX = 10000;
// 不在线玩家加入公会的表格行数上限
const int GUILD_OUTLINE_JOIN_ROW_MAX = 10000;
// 不在线玩家退出公会的表格行数上限
const int GUILD_OUTLINE_QUIT_ROW_MAX = 10000;

// 公会成员表最大行数
const int GUILD_MEMBER_REC_ROW_MAX = 100000;
// 申请加入公会玩家表的最大行数
const int GUILD_JOIN_REC_ROW_MAX = 50;

// 公会动态表格最大行数
const int GUILD_TRENDS_REC_ROW_MAX = 100;
// 公会商店购买表格最大行数
const int GUILD_SHOP_BUY_REC_ROW_MAX = 100000;
// 公会成员购买表格最大行数
const int GUILD_MEMBER_BUY_REC_ROW_MAX = 100000;
// 公会成员捐献表格最大行数
const int GUILD_MEMBER_DONATE_REC_ROW_MAX = 100000;
// 公会聊天记录最大行数
const int GUILD_CHAT_HISTORY_ROW_MAX = 30;

// 公会列表表格列
enum
{
    GUILD_LIST_REC_COL_NAME = 0,                      // 公会名 VTYPE_WIDESTR
	GUILD_LIST_REC_COL_CAPTAIN_UID,                   // 会长UID VTYPE_STRING
    GUILD_LIST_REC_COL_CAPTAIN,                       // 会长名称 VTYPE_WIDESTR
	GUILD_LIST_REC_COL_CAPTAIN_LEVEL,                 // 会长等级 VTYPE_INT
    GUILD_LIST_REC_COL_ANNOUNCEMENT,                  // 公告 VTYPE_WIDESTR
	GUILD_LIST_REC_COL_RANK_DATE,                     // 公会排名等级值产生的时间 VTYPE_INT64
//	GUILD_LIST_REC_COL_NATION,                        // 公会所属国家 VTYPE_INT
    GUILD_LIST_REC_COL_MEMBER_LOGIN_TIME,             // 公会成员最近登陆退出时间记录 VTYPE_INT64(解散使用)
	GUILD_LIST_REC_COL_DECLARATION,					  // 宣言 VTYPE_WIDESTR
	GUILD_LIST_REC_COL_FIGHT_ABILITY,				  // 公会战斗力 VTYPE_INT
    GUILD_LIST_REC_COL_COUNT,
};

// 公会排序表
enum
{
	GUILD_SORT_REC_COL_NAME = 0,                      // 公会名称 VTYPE_WIDESTR
	GUILD_SORT_REC_COL_NATION,                        // 公会所属国家 VTYPE_INT
	GUILD_SORT_REC_COL_COUNT,
};

// 申请者列表表格列
enum
{
	GUILD_APPLY_REC_COL_NAME_UID = 0,   // 申请者名称UID VTYPE_STRING
	GUILD_APPLY_REC_COL_NAME,           // 申请者名称 VTYPE_WIDESTR
	GUILD_APPLY_REC_COL_GUILD,          // 申请加入的公会集合 VTYPE_WIDESTR
	GUILD_APPLY_REC_COL_COUNT,
};

// 记录不在线玩家加入公会的表格的列
enum
{
	GUILD_OUTLINE_JOIN_REC_COL_PLAYER_NAME_UID = 0,     // 玩家名称UID VTYPE_STRING
    GUILD_OUTLINE_JOIN_REC_COL_PLAYER_NAME,             // 玩家名称 VTYPE_WIDESTR
    GUILD_OUTLINE_JOIN_REC_COL_GUILD_NAME,              // 公会名称 VTYPE_WIDESTR
    GUILD_OUTLINE_JOIN_REC_COL_JOIN_DATE,               // 加入时间 VTYPE_INT64
    GUILD_OUTLINE_JOIN_REC_COL_COUNT,
};

enum GUILD_MAINTAIN_STATE
{
	MAINTAIN_FAILED, //维护失败
	MAINTAIN_SUCCESS,//维护成功

};

// 会会商店购买计数表
enum
{
    GUILD_SHOP_BUY_REC_COL_ITEMID = 0,              // GuildName+serverId+ItemId+_index VTYPE_WIDESTR
    GUILD_SHOP_BUY_REC_COL_BUY_COUNT,                // 购买次数 VTYPE_INT
    GUILD_SHOP_BUY_REC_COL_COUNT,
};

// 公会成员购买计数表
enum 
{
    GUILD_MEMBER_BUY_REC_COL_UID_ITEMID = 0,        // 玩家UID+ItemId+_index VTYPE_STRING
    GUILD_MEMBER_BUY_REC_COL_BUY_COUNT,             // 购买次数 VTYPE_INT
    GUILD_MEMBER_BUY_REC_COL_COUNT,
};

//自动同意进入公会
enum 
{
	GUILD_AUTO_AGREE_ON = 1,//
	GUILD_AUTO_AGREE_OFF = 0, //
};



// 公会私有表格
// 公会成员表格列
enum
{
	GUILD_MEMBER_REC_COL_NAME_UID = 0,		    // 成员名称UID VTYPE_STRING
    GUILD_MEMBER_REC_COL_NAME,				    // 成员名称 VTYPE_WIDESTR
    GUILD_MEMBER_REC_COL_SEX,                   // 成员性别 VTYPE_INT
    GUILD_MEMBER_REC_COL_POSITION,			    // 职位 VTYPE_INT
	GUILD_MEMBER_REC_COL_LEVEL,				    // 等级 VTYPE_INT
	GUILD_MEMBER_REC_COL_CAREER,			    // 职业 VTYPE_INT
	GUILD_MEMBER_REC_COL_FIGHT_ABILITY,		    // 战斗力 VTYPE_INT
    GUILD_MEMBER_REC_COL_DEVOTE,			    // 总贡献度 VTYPE_INT
	GUILD_MEMBER_REC_COL_OUTLINE_DATE,		    // 最后一次下线时间 VTYPE_INT64
	GUILD_MEMBER_REC_COL_ONLINE,			    // 在线状态 VTYPE_INT
    GUILD_MEMBER_REC_COL_DEVOTE_SILVER,         // 玩家捐献银元数量 VTYPE_INT
	GUILD_MEMBER_REC_COL_VIP_LEVEL,				// vip等级
	GUILD_MEMBER_REC_COL_FLAG_BOX_NUM,			// 战旗宝箱个数

	GUILD_MEMBER_REC_COL_JOIN_TIME,				// 加入公会时间 VTYPE_INT64
	GUILD_MEMBER_REC_COL_TEACH_NUM,				// 当天传功次数 VTYPE_INT
	GUILD_MEMBER_REC_COL_BE_TEACH_NUM,			// 当天被传功次数 VTYPE_INT

    GUILD_MEMBER_REC_COL_COUNT,
};

// 申请加入公会玩家表格列
enum
{
	GUILD_JOIN_REC_COL_PLAYER_NAME_UID = 0,		// 玩家名称UID VTYPE_STRING
    GUILD_JOIN_REC_COL_PLAYER_NAME,		        // 玩家名称 VTYPE_WIDESTR
    GUILD_JOIN_REC_COL_SEX,                     // 成员性别 VTYPE_INT
	GUILD_JOIN_REC_COL_LEVEL,			        // 等级 VTYPE_INT
	GUILD_JOIN_REC_COL_CAREER,			        // 职业 VTYPE_INT
	GUILD_JOIN_REC_COL_FIGHT_ABILITY,	        // 战斗力 VTYPE_INT
	GUILD_JOIN_REC_COL_VIP,				       // vip VTYPE_INT
	GUILD_JOIN_REC_COL_ONLINE,					// 是否在线 VTYPE_INT
    GUILD_JOIN_REC_COL_COUNT,
};

// 公会动态表格列
enum
{
    GUILD_TRENDS_REC_COL_NAME_UID = 0,      // 玩家名称UID VTYPE_STRING
    GUILD_TRENDS_REC_COL_NAME,              // 玩家名称 VTYPE_WIDESTR
    GUILD_TRENDS_REC_COL_LEVEL,             // 玩家等级 VTYPE_INT
    GUILD_TRENDS_REC_COL_DATE,              // 日期 VTYPE_INT64
    GUILD_TRENDS_REC_COL_TYPE,              // 动态类型 VTYPE_INT
    GUILD_TRENDS_REC_COL_PARAM1,            // 参数1 VTYPE_INT
    GUILD_TRENDS_REC_COL_PARAM2,            // 参数2 VTYPE_INT
    GUILD_TRENDS_REC_COL_PARAM3,            // 参数3 VTYPE_INT
    GUILD_TRENDS_REC_COL_COUNT,
};

// 公会日志
const char* const GUILD_LOG_REC = "guild_log_record";
enum
{
	GUILD_LOG_COL_type = 0,		// 日志类型 int
	GUILD_LOG_COL_time = 1,		// 日志时间 int_64
	GUILD_LOG_COL_args = 2,		// 日志参数 wstring 以逗号分隔
	GUILD_LOG_COL_TOTAL,
};


enum GUILD_BUILDING_TYPE
{
	BUILD_BUILDING_TYPE_JU_YI_TING=1,  //聚义厅
	BUILD_BUILDING_TYPE_JIN_KU,		 //金库
	BUILD_BUILDING_TYPE_XIANG_FANG,  //厢房
	BUILD_BUILDING_TYPE_CANGKU,		 //仓库
	BUILD_BUILDING_TYPE_SHU_YUAN,    //书院
	BUILD_BUILDING_TYPE_SHANG_DIAN,	 //商店
	BUILD_BUILDING_TYPE_MAX,

};
//组织建筑
#define GUILD_BUILDING_REC "guild_building_rec"
//组织建筑
enum{

	GUILD_BUILDING_REC_COL_TYPE ,	//建筑类型
	GUILD_BUILDING_REC_COL_LEVEL,	//建筑等级
	GUILD_BUILDING_REC_COL_EXP,		//建筑当前经验
	GUILD_BUILDING_REC_COL_STATE,	//建设状态
	GUILD_BUILDING_REC_COL_ISBAO,	//是否使用秘宝
	GUILD_BUILDING_REC_COL_MAX,


};

// 建筑状态
enum GUILD_BUILDING_STATE
{
	GUILD_BUILDING_STATE_CAN_LEVELUP,			//可以进行升级
	GUILD_BUILDING_STATE_LEVELUPING,    //进行中
	GUILD_BUILDING_STATE_STOP,			//停止
};

// 建筑是否使用秘宝
enum GUILD_BUILDING_USED_BAO
{
	GUILD_BUILDING_BAO_NONE,		// 未使用
	GUILD_BUILDING_BAO_USED,		// 使用 
};

#define GUILD_NUM_REC "guild_num_rec"
enum
{
	GUILD_NUM_REC_COL_TYPE ,
	GUILD_NUM_REC_COL_ENABLE_VALUE, //可以使用的资金
	GUILD_NUM_REC_COL_LOCK_VALUE,   //锁定的资金
	GUILD_NUM_REC_COL_MAX,

};
enum{
	GUILD_OPERATE_START_LVUP,
	GUILD_OPERATE_STOP_LVUP,

};
//组织分红记录
#define GUILD_BONUS_REC  "guild_bonus_rec"
// 组织分红记录 
enum GUILD_BONUS_REC_COL
{
	GUILD_BONUS_REC_COL_NAME, //玩家名
	GUILD_BONUS_REC_COL_TIMES,//分红次数
	GUILD_BONUS_REC_COL_MAX,
};



//组织可以分红的资金
#define GUILD_BONUS_VALUE "guild_bonus_value"
//分红被锁定的资金下一周才可以分配
#define GUILD_BONUS_LOCK_VALUE "guild_bonus_lock_value"

enum GUILD_GIVE_OUT_BONUS_TYPE
{
	GUILD_GIVE_OUT_BONUS_ALL,   //所有人
	GUILD_GIVE_OUT_BONUS_ELITE, //精英及以上
	GUILD_GIVE_OUT_BONUS_ASSIGN, //指定

};

// 组织奖励
enum GUILD_NUM_AWARD
{

	GUILD_AWARD_TYPE_CAPITAL = 1,	// 组织资金
	GUILD_AWARD_TYPE_ANDINGDU,		// 安定度
	GUILD_AWARD_TYPE_FANRONGDU,		// 繁荣度
	GUILD_AWARD_TYPE_XINGDONGLI,	// 行动力
	GUILD_AWARD_TYPE_JIANKANDU,		// 健康度
	GUILD_AWARD_TYPE_BUILDING_EXP,	// 组织建设度
	
};



//组织数值类型
enum GUILD_NUM_TYPE
{
	GUILD_NUM_CAPITAL=1, //组织资金
	GUILD_NUM_ANDINGDU,//安定度
	GUILD_NUM_FANRONGDU, //繁荣度
	GUILD_NUM_XINGDONGLI,//行动力
	GUILD_NUM_JIANKANDU,//健康度
	


};
//秘宝类型
enum GUILD_RARE_TREASURE_TYPE
{
	GUILD_RARE_TREASURE_NONE = 0, //无
	GUILD_RARE_TREASURE_WELKIN ,//秘宝天宫
	GUILD_RARE_TREASURE_STOPPING_WAR ,//秘宝息兵
	GUILD_RARE_TREASURE_MAX,

};
//秘宝使用返回
enum GUILD_USER_RARE_TREASURE_RET
{
	GUILD_USER_RARE_TREASURE_RET_ERROR = -1,//未知错误
	GUILD_USER_RARE_TREASURE_RET_SUCCESS = 0,//成功
	GUILD_USER_RARE_TREASURE_RET_STOPPING_WAR_IS_USING, //已经使用秘宝
};





//组织数值购买
#define GUILD_NUM_BUY_REC "guild_num_buy_rec"
enum
{
	GUILD_NUM_BUY_REC_COL_TYPE, //组织数值购买类型
	GUILD_NUM_BUY_REC_COL_TIMES,//组织数值购买次数
	GUILD_NUM_BUY_REC_COL_MAX,
};




//组织技能
#define GUILD_PUB_SKILL_REC "guild_pub_skill_rec"
enum {
	GUILD_PUB_SKILL_REC_COL_SKILLID, //技能id
	GUILD_PUB_SKILL_REC_COL_LEVEL,   //技能等级
	GUILD_PUB_SKILL_REC_COL_EXP,     //技能经验
	GUILD_PUB_SKILL_REC_COL_STUDY_STATE,   //研究状况
	GUILD_PUB_SKILL_REC_COL_MAX,


};
//组织技能
enum GUILD_PUB_SKILL_STUDY_STATE{

	GUILD_PUB_SKILL_STUDY_STATE_CAN_STUDY,//可以进行研究
	GUILD_PUB_SKILL_STUDY_STATE_STUDYING,//进行升降中
	GUILD_PUB_SKILL_STUDY_STATE_STOP,		//暂停中


};

enum GUILD_SKILL_TYPE
{
	GUILD_SKILL_TYPE_NOMAL,  //普通技能
	GUILD_SKILL_TYPE_SPECIAL,//特殊技能
};

#define GUILD_RARE_TREASURE_REC "guild_rare_treasure_rec"
enum
{
	GUILD_RARE_TREASURE_REC_COL_TYPE,//秘宝类型
	GUILD_RARE_TREASURE_REC_COL_NUM, //秘宝数量
	GUILD_RARE_TREASURE_REC_COL_MAX,
};



//每日贡献组织资金
#define GUILD_GAIN_CAPITAL_REC "gulid_gain_capital_rec"

enum
{
	GUILD_GAIN_CAPITAL_REC_COL_NAME,         //玩家名
	GUILD_GAIN_CAPITAL_REC_COL_VALUE,        //每日获得组织资金值
	GUILD_GAIN_CAPITAL_REC_COL_EXTRA_AWARD_SATE,  //额外奖励状态 0：不可以领取， 1：可以领取
	GUILD_GAIN_CAPITAL_REC_COL_MAX,


};

#define DAILY_GET_GUILD_CAPITAL "dailygetguildcapital" //每日获取多少组织资金


// 公会成员请愿：竞拍 临时表 [生存周期：竞拍时间段内]
const int   C_MAX_GUILD_PETITION_AUCTION = 100000;
const char  C_REC_GUILD_PETITION_AUCTION[] = "guild_petition_auction_rec";
enum
{
	E_FIELD_GUILD_PETITION_AUCTION_WCHAR_NAME = 0,	// 成员名称 VTYPE_WIDESTR
};




const int GUILD_LOG_REC_MAX_ROW		= 256;	// 表最大行数
const int GUILD_LOG_REC_PAGE_COUNT	= 20;	// 每页个数
const int GUILD_LOG_DONATE_COUNT	= 4;	// 捐献日志下发条数
const int GUILD_SKILL_MAX_ROW		= 50;	// 最大技能数
const int GUILD_MUTUAL_RANK_REC_MAX_ROW = 100;   //组织互助榜单
const int GUILD_SHOP_SELL_LIST_MAX_ROW = 100;	//商店可出售物品列表
const int GUILD_TOMORROW_SHOP_SELL_LIST_MAX_ROW = 100;//商店明日可出售物品列表
const int GUILD_BUILDING_REC_MAX_ROW = 50;	//组织建筑数量
const int GUILD_NUM_REC_MAX_ROW      = 10;	//组织数值
const int GUILD_SET_ERC_MAX_ROW = 20;		//组织设置信息
const int GUILD_RARE_TREASURE_MAX_ROW = 50; //组织秘宝数量
const int GUILD_BONUS_MAX_ROW = 10000;		//组织分红次数
// 公会日志类型 参数客户端回到文言中查找对应串
enum GuildLogType
{
	GUILD_LOG_TYPE_NONE				= 0,	// 无类型
	GUILD_LOG_TYPE_JOIN				= 1,	// 加入帮会		参数 玩家名称
	GUILD_LOG_TYPE_LEAVE			= 2,	// 退出帮会		参数 玩家名称
	GUILD_LOG_TYPE_BE_LEAVE			= 3,	// 被踢出帮会	参数 发起人 玩家名称
	GUILD_LOG_TYPE_ADD_GUILD_NUM	= 4,	// 增加组织数值		参数 原因  玩家名称 组织数值类型 数量 
	GUILD_LOG_TYPE_DEC_GUILD_NUM	= 5,	// 减少组织数值		参数 原因 玩家名称 组织数值类型 数量 
	GUILD_LOG_TYPE_UP_LEVEL			= 6,	// 帮会升级		参数 帮会名称 帮会等级
	GUILD_LOG_TYPE_DOWN_LEVEL		= 7,	// 帮会降级		参数 帮会名称 帮会等级
	GUILD_LOG_TYPE_ACTIVITY			= 8,	// 帮会活动		参数 防守波数 经验数
	GUILD_LOG_TYPE_PROMOTION		= 9,	// 升职			参数 玩家名称 官职类型(是一个数字，需要转换为官职文字)
	GUILD_LOG_TYPE_DEMOTION			= 10,	// 降职			参数 玩家名称 官职类型(是一个数字，需要转换为官职文字)
    GUILD_LOG_TYPE_IMPEACH_START    = 11,   // 弹劾发起     参数 发起人名称
    GUILD_LOG_TYPE_IMPEACH_SUCCEED  = 12,   // 弹劾成功     参数 发起人名称
    GUILD_LOG_TYPE_IMPEACH_FAIL     = 13,   // 弹劾失败     参数 发起人名称
    GUILD_LOG_TYPE_PROMOTION_NEW	= 14,	// 升职			参数  发起人 玩家名称 官职(文言中对应的key "GuildLevel职位类型值")
    GUILD_LOG_TYPE_DEMOTION_NEW		= 15,	// 降职			参数 发起人 玩家名称 官职(文言中对应的key "GuildLevel职位类型值")
	GUILD_LOG_TYPE_DEC_DONATE		= 16,	// 扣除帮贡		参数 玩家名 消耗原因 以前帮贡 扣除帮贡 当前帮贡
	GUILD_LOG_TYPE_DONATE_TRADE_SILVER = 17,	// 捐献银元		参数 玩家名称 捐献数量
	GUILD_LOG_TYPE_BUILDING         =18,    //组织建筑日志     参数 int  类型
	GUILD_LOG_TYPE_SKILL            = 19,   //技能日志
	GUILD_LOG_USE_RARE_TREASURE     = 20,   // 使用组织秘宝   参数 int wstring 玩家名   
	GUILD_LOG_BONUS					 = 21,    // 分红  wstring 玩家名 int 总共分红
	GUILD_LOG_GUILD_BATTLE			= 22,   //组织战事
	GUILD_LOG_MEMBER_EVENT			= 23,   //组织成员事件
	GUILD_LOG_ACTIVITY_EVENT		= 24,   //组织活动事件
};
//分红日志
enum GUILD_LOG_BONUS_TYPE
{
	GUILD_LOG_BONUS_TYPE_ADD_BONUS,		  //添加分红
	GUILD_LOG_BONUS_TYPE_GIVE_OUT_BONUS, //分红
};

//组织成员事件类型
enum GUILD_LOG_MEMBER_EVENT_TYPE
{
	GUILD_LOG_MEMBER_EVENT_PLAYER_LV_UP, //玩家升级 wstring 玩家名 int 等级
	GUILD_LOG_MEMBER_EVENT_UNLOCK_ACHIEVEMENT, //玩家升级 wstring string 成就
	GUILD_LOG_MEMBER_EVENT_GET_TITLE,		//称号  wstring 玩家名  int 称号id
	GUILD_LOG_MEMBER_EVENT_GET_EQUIPMENT, //稀有装备
};
//组织活动事件类型
enum GUILD_LOG_ACTIVITY_EVENT_TYPE
{
	GUILD_LOG_ACTIVITY_EVENT_TYPE_KIDNAP ,//绑票wstring 玩家名 
	GUILD_LOG_ACTIVITY_EVENT_TYPE_FINISH_TASK,//组织任务wstring 玩家名 

};


//1都是给进攻方用的，2都是给防守方用的
enum GUILD_LOG_GUILD_BATTLE_TYPE
{
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_BEGIN,//势力战竞拍开始 int towerid
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_END1,//势力战竞拍结束 int towerid atta_name
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_END2,//势力战竞拍结束 int towerid atta_name

	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_BEGIN,//掠夺战竞拍开始int towerid 

	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_END,//掠夺战竞拍结束 int towerid atta_name

	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_BEFORE1,//势力准备战开始 
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_BEFORE2,//势力准备战开始 

	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_BEGIN1,//势力战开始 int towerid atta_name own_name
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_BEGIN2,//势力战开始 int towerid atta_name own_name
	
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_END1,//势力战结束 int towerid int (结果) atta_name own_name
	GUILD_LOG_GUILD_BATTLE_TYPE_OCCUPY_ACTIVITY_END2,//势力战结int towerid int (结果) atta_name own_name束

	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_BEFORE1,//掠夺准备战开始 
	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_BEFORE2,//掠夺准备战开始

	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_BEGIN1,//掠夺战开始  int towerid atta_name own_name
	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_BEGIN2,//掠夺战开始 int towerid atta_name own_name

	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_END1,//掠夺战结束 int towerid atta_name own_name money item
	GUILD_LOG_GUILD_BATTLE_TYPE_PLUNDER_ACTIVITY_END2,//掠夺战结束 int towerid atta_name own_name money iteme
	

};

enum GUILD_LOG_TYPE_BUILDING_TYPE{
	BUILDING_LOG_TYPE_START_BUILDING,        //建造建筑物 参数  wstring 玩家名 int 建筑类型 参数 int等级
	BUILDING_LOG_TYPE_CONTINUE_BUILDING,     //继续建筑物 参数 wstring 玩家名 int 建筑类型 参数 int等级
	BUILDING_LOG_TYPE_STOP_BUILDING,        //停止建筑物 参数  wstring 玩家名 int 建筑类型 参数 int等级
    BUILDING_LOG_TYPE_LV_UP_SUCCESS,		//升级成功  参数 int 建筑类型 参数 int等级
	BUILDING_LOG_TYPE_MAINTAIN_ADD_STUDY_EXP, //维护成功增加研究经验 int 加经验
	BUILDING_LOG_TYPE_MAINTAIN_DEC_FRD,    //维护组织资金不足扣除繁荣度 int 消耗的繁荣度
	BUILDING_LOG_TYPE_MAINTAIN_FAIL,    //维护失败
};

enum GUILD_LOG_TYPE_SKILL_TYPE{
	SKILL_LOG_TYPE_UNLOCK, //解锁   技能id  wstring 玩家名
	SKILL_LOG_TYPE_START_STUDY,  //研究 技能id  wstring 玩家名
	SKILL_LOG_TYPE_CONTINUE_STUDY,//继续研究 技能id  wstring 玩家名
	SKILL_LOG_TYPE_STOP_STUDY, //停止研究 技能id  wstring 玩家名
	SKILL_LOG_TYPE_SKILL_LV_UP, //技能升级  技能id 技能等级
};

enum GUILD_NUM_CHANGE
{

	GUILD_NUM_CHANGE_GM, //		gm 增加
	GUILD_NUM_CHANGE_DONATE, //帮会捐献
	GUILD_NUM_CHANGE_BUILD_LVUP, //建筑升级
	GUILD_NUM_CHANGE_BUY_GUILD_NUM,//购买组织数值
	GUILD_NUM_CHANGE_CHANGE_GUILD_NOTIFY,//组织公告
	GUILD_NUM_CHANGE_CHANGE_SHORT_NAME,	//修改短名
	GUILD_NUM_CHANGE_CHANGE_RECOVER,	//每日恢复
	GUILD_NUM_CHANGE_CHANGE_MAINTAIN,	//维护
	GUILD_NUM_CHANGE_DAILY_GET,			//健康度保底获取
	GUILD_NUM_CHANGE_SKILL_STUDY,		//技能研究
	GUILD_NUM_CHANGE_SKILL_UNLOCK,		//技能解锁
	GUILD_NUM_CHANGE_PLUNDER_AUCTION_DEC,	//竞拍者：  竞拍扣除
	GUILD_NUM_CHANGE_PLUNDER_AUCTION_BACK,  //竞拍者：  竞拍返回
	GUILD_NUM_CHANGE_PLUNDER_ADD,	//掠夺战（掠夺者）：新增
	GUILD_NUM_CHANGE_PLUNDER_DEC,	//掠夺战（被掠夺者）：扣除
	GUILD_NUM_CHANGE_SHOP,			 //组织商店
	GUILD_NUM_CHANGE_OCCUPY_AUCTION_DEC,	//组织城竞拍扣除
	GUILD_NUM_CHANGE_OCCUPY_AUCTION_BACK,   //组织城竞拍返回
	GUILD_NUM_CHANGE_TOWN_ADD,			    //组织城
	GUILD_NUM_CHANGE_TASK,   //任务
	GUILD_NUM_CHANGE_KIDNAP,			    //绑票
	GUILD_NUM_CHANGE_DANCING,			    //舞姬扣除
	GUILD_NUM_CHANGE_DAILY_GET_GUILD_CAPITAL_NOT_ENOUGHT, //每日获取资金不足 扣除健康度
	GUILD_NUM_CHANGE_MAINTAIN_FAILED_DEC,	//维护失败 扣除健康度
	GUILD_NUM_CHANGE_NOT_OPEN_GUILD_ACTIVITY,//未开启组织活动 扣除健康度
	GUILD_NUM_CHANGE_PLUNDER_AUCTION_INIT_DEC,	 //被掠夺者： 竞拍前，扣除锁定  <不需要日志>
	GUILD_NUM_CHANGE_PLUNDER_AUCTION_INIT_BACK,  //被掠夺者： 掠夺后，返回解锁  <不需要日志>
};



// 公会成员的职位
enum
{
	GUILD_POSITION_NONE = -1,
	GUILD_POSITION_CAPTAIN = 0,             // 会长
	GUILD_POSITION_ASSISTANT_CAPTAIN,       // 副帮主
	GUILD_POSITION_GENERAL,                 // 长老
	GUILD_POSITION_ELDER,                   // 精英
	GUILD_POSITION_MEMBER,                  // 一般成员
    GUILD_POSITION_COUNT,
};

//客户端职位字典前段
const wchar_t* const GUILD_LEVEL =  L"GuildLevel";

// 公会动态类型
enum
{
	GUILD_TRENDS_TYPE_JOIN = 0,          // 加入公会
	GUILD_TRENDS_TYPE_INVITE_JOIN,       // 被邀请加入公会
	GUILD_TRENDS_TYPE_QUIT,              // 退出公会
	GUILD_TRENDS_TYPE_FIRE,              // 被踢出公会
	GUILD_TRENDS_TYPE_BUILD,             // 建设公会
	GUILD_TRENDS_TYPE_UPLEVEL,           // 升级公会
};

// 工会权值类型
enum GuildPriv
{
	GUILD_PRIV_SET_POSITION = 1,           //设置职位
	GUILD_PRIV_RATIFY,		            //批准入会
	GUILD_PRIV_FIRE,			        //踢人
	GUILD_PRIV_ANNOUNCEMENT,	        //发布公告
	GUILD_PRIV_STATION_DANCING_GIRL,	//帮会舞姬
	GUILD_PRIV_AUTO_AGREE,				//帮会自动同意
	GUILD_PRIV_UNLOCK_SKILL,			//解锁技能
	GUILD_PRIV_BUILDING_LEVEL_UP,		//建筑升级
	GUILD_PRIV_GUILD_NUM_BUY,		   // 组织数值购买
	GUILD_PRIV_CHANGE_SHORT_NAME,	    //修改组织简称
	GUILD_PRIV_START_STUDY_SKILL,	    //开始研究技能
	GUILD_PRIV_USE_RARE_TREASURE,		//使用息兵符
	GUILD_PRIV_GIVE_OUT_BONUS,			//分红
};

enum{
	Normal_Donate = 0, //普通帮贡
	Boss_Donate = 1// boss 帮贡
};


// 组织互助
enum
{
	GUILD_MUTUAL_HELP_REC_COL_INDEX,			// index
	GUILD_MUTUAL_HELP_REC_COL_NAME,				//玩家名
	GUILD_MUTUAL_HELP_REC_COL_POSITION,			//职位
	GUILD_MUTUAL_HELP_REC_COL_ITEM_ID,			//道具id
	GUILD_MUTUAL_HELP_REC_COL_TIMES,			//当前已经获得的道具数量
	GUILD_MUTUAL_HELP_REC_COL_TIMES_MAX,		//可获得道具数量最大值
	GUILD_MUTUAL_HELP_REC_COL_MAX,				
};
enum 
{
	GUILD_MUTUAL_RANK_REC_COL_NAME, //玩家名
	GUILD_MUTUAL_RANK_REC_COL_DONATE, //贡献值
	GUILD_MUTUAL_RANK_REC_COL_MAX,
};
enum 
{
	GUILD_SHOP_SELL_LIST_REC_COL_ITEM,//商店可出售物品列表
	GUILD_SHOP_SELL_LIST_COL_MAX,
};
enum 
{
	GUILD_TOMORROW_SHOP_SELL_LIST_COL_ITEM,//明日商店可出售物品列表
	GUILD_TOMORROW_SHOP_SELL_LIST_COL_MAX,
};




enum 
{
	GUILD_SYSMBOL_REC_COL_GUILD_NAME,		//组织名
	GUILD_SYSMBOL_REC_COL_IDENTIFYING,      //组织标识
	GUILD_SYSMBOL_REC_COL_SHORT_NAME,       //组织简称
};




enum GUILD_SET_REC_COL
{
	GUILD_SET_REC_COL_GUILD_NAME,
	GUILD_SET_REC_COL_AUTO_AGREE,					  // 自动同意   VTYPE_INT
	GUILD_SET_REC_COL_AUTO_AGREE_MIN_LEVEL,		  // 自动同意需要最小等级
	GUILD_SET_REC_COL_AUTO_AGREE_MAX_LEVEL,		  // 自动同意需要最大等级
	GUILD_SET_REC_COL_AUTO_AGREE_ABILITY,		  // 自动同意需要战斗力
	GUILD_SET_REC_COL_DANCING_GIRL,				  // 舞姬      VTYPE_INT
	GUILD_SET_REC_COL_AUTO_OPEN_DANCING,			  // 组织舞姬预约开启时间 VTYPE_INT
	GUILD_SET_REC_COL_MAINTAIN_STATE,				 //  组织维护状态    VTYPE_INT
};
//请愿表
#define GUILD_PETITION_REC "guild_petition_rec"
enum GUILD_PETITION_REC_COL
{
	GUILD_PETITION_REC_COL_NAME, //玩家名
	GUILD_PETITION_REC_COL_TYPE, //类型

};
//请愿类型
enum GUILD_PETITION_TYPE
{
	GUILD_PETITION_TYPE_JU_YI_TING = 1,  //聚义厅升级
	GUILD_PETITION_TYPE_JIN_KU,		 //金库升级
	GUILD_PETITION_TYPE_XIANG_FANG,  //厢房升级
	GUILD_PETITION_TYPE_CANGKU,		 //仓库升级
	GUILD_PETITION_TYPE_SHU_YUAN,    //书院升级
	GUILD_PETITION_TYPE_SHANG_DIAN,	 //商店升级
	GUILD_PETITION_TYPE_MAX,
};




// 创建公会配置数据结构
typedef struct CreateGuildConfig_s
{
	CreateGuildConfig_s()
		:m_LevelLimit(0),
		Silver(0),
		m_nFireLimit(0)
	{}

	int m_LevelLimit;   // 玩家等级限制
	int Silver;       // 元宝创建公会
	int m_nFireLimit;	// 公会一天踢人的次数限制
}GuildCreateConfig_t;

// 公会等级配置结构
typedef struct GuildUpLevelConfig_s
{
    GuildUpLevelConfig_s() : m_Level(0),
		m_MaxMember(0), 
		m_DeputyCaptainNum(0),
		m_GeneralNum(0),
		m_Elder(0)
	{}

    int m_Level;
    int m_MaxMember;            // 最大成员数量
    int m_DeputyCaptainNum;     // 副帮主数量
    int m_GeneralNum;             // 长老数量
	int m_Elder;				// 精英数量
   

}GuildUpLevelConfig_t;
typedef std::vector<GuildUpLevelConfig_t> GuildUpLevelConfigVector;


struct GuildBoss {

	GuildBoss() {
		m_Level = 0;
		m_LevelUpExp = 0;
	}

	int m_Level;
	int m_LevelUpExp;
};

typedef std::map<std::string, GuildBoss> GuildBossLevelConfig;

// 帮会捐献
typedef struct GuildDonateConfig_s
{
    GuildDonateConfig_s() : m_Id(0), m_CapitalNumber(0), 
		 m_RewardPlayerDonate(0), m_GuildDonate(0) {}

    int m_Id;
    int m_CapitalNumber;        // 表示捐献的货币数量
    int m_RewardPlayerDonate;   // 表示玩家获得的帮贡值
	int m_GuildDonate;			// 组织帮贡经验
} GuildDonateConfig_t;
typedef std::vector<GuildDonateConfig_t> GuildDonateConfigVector;

// 公会职务权限配置信息结构
typedef struct GuildPositionConfig_s
{
	GuildPositionConfig_s()
		: m_Position(0),
		m_privList(0)
	{}

	int m_Position;                    // 职位	
	int m_privList;					   //权限列表
}GuildPositionConfig_t;
typedef std::vector<GuildPositionConfig_t> GuildPositionConfigVector;
typedef GuildPositionConfigVector::iterator GuildPositionConfigVectorIter;



struct GuildSkillLevelUpSpend{
	// 资金
	std::map<int, int> m_vCurrency;
	// 物品
	std::map<std::string, int> m_vItems;
};
typedef std::map<int,std::map<int, GuildSkillLevelUpSpend> > GuildSkillLevelUpSpendMap;
//组织互助道具key itemid,value 道具总捐献次数

struct GuildMutual{
	GuildMutual() :m_donate(0), m_limitTimes(0){}
	int m_donate;
	int m_limitTimes;
};


enum
{
	GUILD_DONATE_DEC_DANCING_GIRL,		//帮贡扣除召唤舞姬
	GUILD_DONATE_DEC_UNLOCK_SKILL,		//解锁技能
	GUILD_DONATE_DEC_CHANGE_SHOP_LIST,	//修改商店物品

};



struct GuildSkillConfig{
	int m_initLevel{ 0 };
	int m_unlockShuYuanLevel{ 0 };
	bool m_autoUnlock{ false };
	int  m_unlockCost;
	int m_startStudyCost;
	GUILD_SKILL_TYPE m_skillType{ GUILD_SKILL_TYPE::GUILD_SKILL_TYPE_NOMAL };
};
struct GuildSkillLv{
	int m_exp;
	int m_shuYuanLv;//需要书院等级

};
typedef std::map<int, GuildSkillConfig> GuildSkillConfigMap;
typedef std::map<int, std::map<int, GuildSkillLv>> GuildSkillLvExp;
//商店出售物品数量key组织等级 value 可出售物品数
typedef std::map<int, int> GuildShopSellNum;


// 用于公会会长自动移交，选择新会长排序
struct GuildMemberSortHelper_t
{
	GuildMemberSortHelper_t() : memberName(L""), position(0), contrib(0), level(0), ability(0) {}
    GuildMemberSortHelper_t(const wchar_t *n, int p, int c, int l, int a) :
        memberName(n), position(p), contrib(c), level(l), ability(a) {}

    bool operator >= (const GuildMemberSortHelper_t &other) const 
    {
        if (position == other.position) {
            if (contrib == other.contrib) {
                if (level == other.level) {
                    if (ability == other.ability) {

                    }
                    else {
                        return ability > other.ability;
                    }
                }
                else {
                    return level > other.level;
                }
            }
            else {
                return contrib > other.contrib;
            }
        }
        else {
            return position < other.position;   // 数值小的职务高
        }
        return false;
    }

    std::wstring memberName;
    int position;
    int contrib;
    int level;
    int ability;
};
typedef std::multiset<GuildMemberSortHelper_t, std::greater_equal<GuildMemberSortHelper_t> > GuildMemberSortHelperSet;

struct GuildShopConfigItem_t
{	
	GuildShopConfigItem_t(): m_Index(0), m_ItemId(""), m_MemberLimit(0), m_GuildTotal(0), m_UnlockLevel(0){}
	int m_Index;
    std::string m_ItemId;
    int m_MemberLimit;
    int m_GuildTotal;
    int m_UnlockLevel;
	int m_addGuildCapital{0}; //增加组织帮贡
	std::vector<std::pair<int, int>> m_decCapital;//需要扣除的货币
};
typedef std::vector<GuildShopConfigItem_t> GuildShopConfigVector;


#define GUILD_PUB_RESET "guild_pub_reset" 
#define GUILD_BUILDING_MAINTAIN "guild_building_maintain"
class IPubKernel;
typedef void (*GuildTimerCb) (IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);

struct GuildTimerCallback_t
{
    GuildTimerCallback_t() : m_TimerName(""),
		m_CallbackName(""),
		m_Hour(0), 
		m_Minute(0),
		m_CallbackFunc(NULL)
	{}
    GuildTimerCallback_t(int h, int m) : m_Hour(h),
		m_Minute(m),
		m_CallbackFunc(NULL)
	{}

    bool operator == (const GuildTimerCallback_t &other) const
    {
        return m_Hour == other.m_Hour && m_Minute == other.m_Minute;
    }
    bool operator < (const GuildTimerCallback_t &other) const 
    {
        if (m_Hour < other.m_Hour)
        {
            return true;
        }
        if (m_Hour == other.m_Hour)
        {
            return m_Minute < other.m_Minute;
        }
        return false;
    }
    int operator - (const GuildTimerCallback_t &other) const
    {
        int hours = m_Hour - other.m_Hour;
        int minutes = m_Minute - other.m_Minute;
        return hours * 3600 + minutes * 60;
    }
    
    std::string m_TimerName;
    std::string m_CallbackName;
    int m_Hour;
    int m_Minute;
    GuildTimerCb m_CallbackFunc;
};
typedef std::vector<GuildTimerCallback_t> GuildTimerCallbackVector;




struct GuildPrepareInfo{
	int m_nation{ 0 };
	std::wstring m_shortName;
	int m_guildIdentifying{ 0 };
	


};


typedef std::map<std::wstring, GuildPrepareInfo> GUILDPREPAREINFO;


#define MAIL_TEACH_REWARD	"mail_teach_reward"

#define CONFIG_PATH_TECAH_REWARD	"ini/rule/guild/guild_teach_reward.xml"
// 传功奖励配置
DECLARE_CONFIG_OBJECT_BEGIN(CfgTeachReward)

#define CONFIG_CLASS_NAME CfgTeachReward
#define PROPLIST      \
	DECLARE_CONFIG_PROP_FIELD_EX(ID, "玩家传功等级", 0, size_t)	\
	DECLARE_CONFIG_PROP_FIELD(SendReward, "传功者奖励", string) \
	DECLARE_CONFIG_PROP_FIELD(GetReward, "接收者奖励", string)

#include "utils/util_config_template.h"
DECLARE_CONFIG_OBJECT_END()


#define TEMP_FIELD_TEACH_TARGET "_temp_teach_target"		// 传功交互对象
#define TEMP_FIELD_TEACH_TYPE "_temp_teach_type"			// 传功类型
#define TEMP_FIELD_TEACH_STATE "_temp_teach_state"			// 传功状态

// 传功状态
enum EmTeachState
{
	EM_TEACH_STATE_NONE,				// 初始状态
	EM_TEACH_STATE_ENTER,				// 进入状态
	EM_TEACH_STATE_READY,				// 准备好了
	EM_TEACH_STATE_COMPLETED,			// 完成了
};

// 传功类型
enum EmTeachType
{
	EM_TEACH_SENDER = 1,		// 传功者
	EM_TEACH_RECIVER = 2,		// 接收者
};

// 传功请求方式
enum EmTeachMethod
{
	EM_TEACH_METHOD_REQ = 1,	// 请求
	EM_TEACH_METHOD_PSH ,		// 推送
};

// 传功回应结果
enum EmTeachAckResult
{
	EM_TEACH_ACK_ACCEPT = 1,	// 接受
	EM_TEACH_ACK_REFUSE,		// 拒绝
};

// 传功结果
enum EmTeachResult
{
	EM_TEACH_RESULT_SUCCEED,			// 成功
	EM_TEACH_RESULT_OFFLINE,			// 对方不在线
	EM_TEACH_RESULT_NO_GUILD,			// 未加入组织或散人组织不可以传功
	EM_TEACH_RESULT_PROTECT,			// 对方在传功保护期(加入组织低于16小时)
	EM_TEACH_RESULT_PROTECT_SELF,		// 自己在传功保护期(加入组织低于16小时)
	EM_TEACH_RESULT_NO_PLAYER,			// 不是公回成员
	EM_TEACH_RESULT_NUM_LIMIT,			// 对方传功次数用完
	EM_TEACH_RESULT_NUM_LIMIT_SELF,		// 自己传功次数用完
	EM_TEACH_RESULT_IN_ACTIVITY,		// 对方在活动中
	EM_TEACH_RESULT_IN_ACTIVITY_SELF,	// 自己在活动中
	EM_TEACH_RESULT_LEVEL_LOWER,		// 传功等级太低
	EM_TEACH_RESULT_REQ_TIMEOUT,		// 玩家请求超时了
	EM_TEACH_RESULT_ENT_FAILED,			// 玩家进入场景失败
	EM_TEACH_RESULT_PLAYER_DATA_ERROR,	// 玩家数据错误
	EM_TEACH_RESULT_REFUSE,				// 拒绝请求
};

#endif //__GuildDefine_H__