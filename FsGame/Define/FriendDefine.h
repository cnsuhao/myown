//--------------------------------------------------------------------
// 文件名:		FriendDefine.h
// 内  容:		好友模块
// 说  明:		一常相关宏定义等
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __FriendDefine_H__ 
#define __FriendDefine_H__ 
#include "utils/util_config.h"


#define DOMAIN_FRIEND_RECOMMEND_REC		L"Domain_Friend_Recommend_%d"
#define SNS_FRIEND_SPACE				L"SnsFriend"


enum
{
	TEAM_ADD_INTIMACY_OFFER,//缉盗
	TEAM_ADD_INTIMACY_SECRET,//组队禁地
	TEAM_ADD_INTIMACY_INBREAK,//组织入侵
	TEAM_ADD_INTIMACY_ESCORT,//组队押囚
	TEAM_ADD_INTIMACY_KILL_NPC,//野外挂机
};






// 场景服务器到公共服务器消息定义
enum 
{
	FRIEND_SEVER_PUB_ADD_ONLINE_PLAYER_INFO = 0,     // 添加在线玩家信息
	FRIEND_SEVER_PUB_DEL_ONLINE_PLAYER_INFO,		 // 删除在线玩家信息
	FRIEND_SEVER_PUB_SET_LEVEL_INFO,				 // 设置等级
	FRIEND_SEVER_PUB_SET_BATTLEABILITY_INFO,		 // 设置战斗力
	FRIEND_SEVER_PUB_SET_GUILDNAME_INFO,		 // 设置帮会名
	FRIEND_SEVER_PUB_SET_TEAM_ID,			//设置队伍id
	FRIEND_SEVER_PUB_SET_VIP,						//设置vip
};

//公共服下发消息
enum
{
	PUB_FRIEND_MARRY_UPDATE = 0,				// 我的UID
};

// 公共数据区在线玩家表
#define PUB_ONLINE_PLAYER_REC "pub_online_player_rec"
// 在线玩家表列定义
enum OnlinePlayerRec
{
	PUB_COL_PLAYER_UID = 0,			// 玩家UID
	PUB_COL_PLAYER_NAME,	        // 玩家名字
	PUB_COL_PLAYER_LEVEL,		    // 玩家等级
	PUB_COL_PLAYER_JOB,				// 玩家职业
	PUB_COL_PLAYER_SEX,				// 玩家性别
	PUB_COL_PLAYER_BATTLE_ABLITITY,	// 玩家战斗力
	PUB_COL_PLAYER_GUILD_NAME,	//	帮会名
	PUB_COL_PLAYER_TEAM_ID,		//	teamID
	PUB_COL_PLAYER_VIP,			//vip
	PUB_COL_PLAYER_MAX,
};
//好友关系
enum FriendRelation
{
	FRIEND_RELATION_NOMAL,	//普通好友关系
	FRIEND_RELATION_SWORN,	//结拜
};


//申请类型
enum ApplyType
{
    APPLY_TYPE_FRIEND = 0,      //申请好友
   
};

enum
{
	FRIEND_SWORN_ADD = 1, //添加结义
	FRIEND_SOWRN_DEL = -1, //删除结义

};

struct NearbyRecommend 
{
	NearbyRecommend(): distance(0.0f){}
	PERSISTID pid;
	float distance;
};


// 好友礼物表
DECLARE_CONFIG_OBJECT_BEGIN(ConfigFriendGiftItem)

#define CONFIG_CLASS_NAME ConfigFriendGiftItem
#define PROPLIST      \
		DECLARE_CONFIG_PROP_FIELD_EX(ID, "自增长ID", 1, size_t )	\
		DECLARE_CONFIG_PROP_FIELD(ItemID, "花费值", string)		\
		DECLARE_CONFIG_PROP_FIELD_EX(Value, "花数量",  0, size_t)	

#include "utils/util_config_template.h"

DECLARE_CONFIG_OBJECT_END()

// 好友亲密度升级表
DECLARE_CONFIG_OBJECT_BEGIN(ConfigFriendIntimacyUpgradeItem)

#define CONFIG_CLASS_NAME ConfigFriendIntimacyUpgradeItem
#define PROPLIST      \
		DECLARE_CONFIG_PROP_FIELD_EX(ID, "自增长ID", 1, size_t )	\
		DECLARE_CONFIG_PROP_FIELD_EX(Exp, "所需经验值", 0xFFFFFFFF, size_t) 

#include "utils/util_config_template.h"

DECLARE_CONFIG_OBJECT_END()


// 好友亲密度升级表
DECLARE_CONFIG_OBJECT_BEGIN(ConfigSwornBuff)

#define CONFIG_CLASS_NAME ConfigSwornBuff
#define PROPLIST      \
		DECLARE_CONFIG_PROP_FIELD_EX(ID, "自增长ID", 1, size_t )	\
		DECLARE_CONFIG_PROP_FIELD(BuffID, "buffid", string) 

#include "utils/util_config_template.h"

DECLARE_CONFIG_OBJECT_END()

// 组队好友亲密
DECLARE_CONFIG_OBJECT_BEGIN(ConfigTeamAddIntimacy)

#define CONFIG_CLASS_NAME ConfigTeamAddIntimacy
#define PROPLIST      \
		DECLARE_CONFIG_PROP_FIELD_EX(ID, "自增长ID", 1, size_t )	\
		DECLARE_CONFIG_PROP_FIELD_EX(DailyLimit, "每日获取上线",0,size_t )  \
		DECLARE_CONFIG_PROP_FIELD_EX(TeamOffer, "缉盗",0,size_t )  \
		DECLARE_CONFIG_PROP_FIELD_EX(TeamSecret, "组队禁地",0,size_t )  \
		DECLARE_CONFIG_PROP_FIELD_EX(TeamInbreak, "组织入侵",0,size_t )  \
		DECLARE_CONFIG_PROP_FIELD_EX(TeamEscort, "组队押囚",0,size_t )  \
		DECLARE_CONFIG_PROP_FIELD_EX(TeamKillNpc, "野外挂机",0 ,size_t)  


#include "utils/util_config_template.h"

DECLARE_CONFIG_OBJECT_END()


#endif