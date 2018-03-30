//---------------------------------------------------------
//文件名:       SnsDataDefine.h
//内  容:       
//说  明:       
//          
//创建日期:      2014年12月25日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef FSGAME_SNS_DATA_DEFINE_H_
#define FSGAME_SNS_DATA_DEFINE_H_

#include <string>

#define SNS_DATA_SECTION_ATTR                 "PlayerAttribute"
#define SNS_DATA_SECTION_EQUIPMENT            "PlayerEquipment"
#define SNS_DATA_SECTION_SKILL                "PlayerSkill"
#define SNS_DATA_SECTION_PLAYER_PET           "PlayerPet"
#define SNS_DATA_SECTION_PLAYER_RIDE          "PlayerRide"
#define SNS_DATA_SECTION_EQUIPMENT_BAPTISE    "PlayerEquipmentBaptise"
#define SNS_DATA_SECTION_WING_EQUIPMENT       "WingEquipment"

// 创建/加载SNS数据的时候的使用的标识
// 31 是随机选择的一个数据
enum
{
    SNS_DATA_DO_NOTHING         = 31,  // 啥都不做的消息ID
    CREATE_SNS_DATA_DEFAULT     = 32,  // 创建默认的SNS数据
    LOAD_SNS_DATA_DEFAULT       = 33,  // 载入默认的SNS数据
    CREATE_SNS_DATA_OFFLINE     = 34,  // 创建离线的SNS数据
    LOAD_SNS_DATA_OFFLINE       = 35,  // 载入离线的SNS数据
    CREATE_SNS_DATA_MSGPUSH     = 36,  // 创建MsgPush的SNS数据
    LOAD_SNS_DATA_MSGPUSH       = 37,  // 载入MsgPush的SNS数据
	CREATE_SNS_DATA_RED_PACKET  = 38,  // 创建RedPacket的SNS数据
	LOAD_SNS_DATA_RED_PACKET    = 39,  // 载入RedPacket的SNS数据
	SNS_DATA_CREATE_PLAYER      = 40,  // 创建SNS数据
	SNS_DATA_QUERY_PLAYER       = 41,  // 查询玩家的SNS数据
};

// 客户端通信的子消息定义
enum
{
    SNS_DATA_SUBMSG_QUERY_OFFLINE_DATA     = 1,   // 查询离线数据
};

// 服务器之间通信的子消息定义
enum
{
    SNS_DATA_SAVE_PLAYER_DATA       = 1,   // 保存离线数据
    SNS_DATA_QUERY_PLAYER_DATA      = 2,   // 查询离线数据
    SNS_DATA_SHOW_ME_YOUR_DATA      = 3,   // 发给查询者自己的数据
    SNS_DATA_HERE_IS_DATA_YOU_WANT  = 4,   // 收到目标用户的数据
    SNS_DATA_MODIFY_PLAYER_DATA     = 5,   // 修改玩家数据
    SNS_DATA_CREATE_PLAYER_ENTRY    = 6,   // 在SNS上创建玩家条目
};

// 标识是谁在请求SNS数据的
enum
{
    SNS_DATA_REQUEST_FROM_NA                = 1,
    SNS_DATA_REQUEST_FORM_RANK_LIST         = 2,    //废弃不再使用
	SNS_DATA_REQUEST_FORM_CHAT              = 3,
	SNS_DATA_REQUEST_FORM_FRIEND_UPDATE     = 4,
	SNS_DATA_REQUEST_FORM_FRIEND_ENEMY      = 5,
	SNS_DATA_REQUEST_FORM_FRIEND_FIND       = 6,
	SNS_DATA_REQUEST_FORM_NATION_OFFICIAL   = 7,
    SNS_DATA_REQUEST_FORM_ADD_FRIEND_APPLY  = 8,
	SNS_DATA_REQUEST_FORM_RANK_LIST_PLAYER  = 9,    //排行榜-玩家基本信息(属性、装备、洗练)
	SNS_DATA_REQUEST_FORM_RANK_LIST_PET     = 10,   //排行榜-玩家宠物信息
	SNS_DATA_REQUEST_FORM_RANK_LIST_RIDE    = 11,   //排行榜-玩家坐骑信息
	SNS_DATA_REQUEST_FORM_RANK_LIST_WING    = 12,   //排行榜-玩家翅膀信息
	SNS_DATA_REQUEST_FORM_FRIEND_BLACK		= 13,		// 更新黑名单
	SNS_DATA_REQUEST_FORM_MASTERAPRTC_UPDATE = 14,	 //师徒-更新
	SNS_DATA_REQUEST_FORM_ADD_MASTERAPRTC_APPLY = 15,	 //师徒-增加申请
	SNS_DATA_REQUEST_FORM_TEAM_PLAYER_LIST	= 16, // 组队-队友信息
};

#endif // FSGAME_SNS_DATA_DEFINE_H_