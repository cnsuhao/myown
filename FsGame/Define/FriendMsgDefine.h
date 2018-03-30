//--------------------------------------------------------------------
// 文件名:	    FriendMsgDefine.h
// 内  容:		好友系统-消息定义
// 说  明:		
// 创建日期:	2014年10月29日
// 整理日期:	2014年10月29日
// 创建人:		  
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __FriendMsgDefine_H__
#define __FriendMsgDefine_H__

enum CSFriendMsg
{
	//好友相关
	//客户端请求添加好友 格式：string msgid, wstring name
	CS_FRIEND_REQUEST_ADD_FRIEND = 1,
	//客户端同意好友请求 格式：string msgid, wstring name
	CS_FRIEND_CONFIRM_ADD_FRIEND = 2,
	//客户单请求删除好友 格式：string msgid, wstring name
	CS_FRIEND_REQUEST_DELETE_FRIEND = 3,
	//客户单请求添加黑名单 格式：string msgid, wstring name
	CS_FRIEND_ADD_BLACKLIST = 5,
	//客户单请求删除黑名单 格式：string msgid, wstring name
	CS_FRIEND_DELETE_BLACKLIST = 6,
	//客户单请求删除（忽略）好友申请 格式：string msgid, wstring name
	CS_FRIEND_DELETE_APPLY = 7,
	// 客户端请求刷新推荐好友申请 格式：string msgid, int type, wstring name(模糊搜索,空不限制,同等级一起限制)[也是模糊搜索](type目前为0)
	CS_FRIEND_REFRESH_FRIEND_RMD = 8,
	//好友信息更新,格式：string msgid
	CS_FRIEND_INFO_UPDATE = 9,
	//客户单请求删除所有黑名单 格式：string msgid
	CS_FRIEND_DELETE_ALL_BLACKLIST = 10,
	// 客户端请求删除仇人，消息格式：unsigned msgid, wstring name(仇人名称)
	CS_FRIEND_DELETE_ENEMY = 12,
	// 客户端请求删除所有仇人，消息格式：unsigned msgid
	CS_FRIEND_DELETE_ALL_ENEMY = 13,
	// 客户端请求附近的人
	CS_FRIEND_ALL_NEARBY = 14,
	// 给好友送礼
	CS_FRIEND_SEND_GIFT = 17,		//送礼   Args: uid, ID（FriendGift表id
	// 一键确认/拒绝好友请求
	CS_FRIEND_CONFIRM_ALL_APPLY = 18,		// 参数: yes/no (1:yes 0: no)
	// 结义申请
	CS_FRIEND_APPLAY_SWORN	= 19,	// str 玩家uid
	// 结义申请结果
	CS_FRIEND_APPLAY_SWORN_REPLY = 20,   //wstr 玩家名 int 结果0拒绝 1同意
	// 寻求援助
	CS_FRIEND_SEEK_AID = 21,			// str 玩家名
	// 修改好友称谓
	CS_FRIEND_CHANGE_FRIEND_TITILE  =22, //wstr 玩家名 wstring内容
	// 删除结义
	CS_FRIEND_DEL_SWORN = 23,           //wstr 玩家名
	// 援助
	CS_FRIEND_AID = 24,					//wstr 玩家名
	//亲密度奖励
	CS_FRIEND_INTIMACY_AWARD = 25,		//
	//一键添加好友
	CS_FRIEND_ADD_FRIEND_BY_ONE_KEY = 26, // 一键添加好友 wsring玩家名 
};
	

enum SSFrienMsg
{
	//好友玩家上下线通知
	SS_FRIEND_ONLINE_STATUS_UPDATE = 1,
	//通知对方删除好友
	SS_FRIEND_REMOVE_PLAYER = 2,
	//增加好友申请列表
	SS_FRIEND_APPLY = 4,
	//好友等级更改
	SS_FRIEND_LEVEL_UPDATE = 5,
	//好友战斗力更改
	SS_FRIEND_BATTLEABILITY_UPDATE = 6,
	//好友帮会更改
	SS_FRIEND_GUILDNAME_UPDATE = 7,
	//添加好友反馈
	SS_FRIEND_ADD_RESULT = 11,
	// 好友送花
	SS_FRIEND_SEND_GIFT = 13,		// 参数: sender uid, sender Name, flower number
	// 好友花数量更新
	SS_FRIEND_FLOWER_UPDATE = 14,	// 参数: pMyUid << nMySumFlower << nMyWeekFlower
	//增加彼此亲密度
	SS_FRIEND_ADD_MUTUAL_INTIMACY = 15,
	//增加亲密度
	SS_FRIEND_ADD_INTIMACY = 16,
	//更新好友teamid
	SS_FRIEND_UPDATE_TEAM_ID = 17,
	//申请结义
	SS_FRIEND_APPLAY_SWORN = 18,
	//申请结义结果
	SS_FRIEND_APPLAY_SWORN_RSP = 19,
	//结义成功
	SS_FRIEND_SWORN_SUCCESS = 20,
	//删除结义
	SS_FRIEND_DEL_SWORN = 21,
	//援助获得玩家坐标
	SS_FRIEND_AID_TARGET_POS_REQ = 22,
	//援助传送
	SS_FRIEND_AID_TARGET_POS_RSP = 23,
	//增加敌人
	SS_FRIEND_ADD_ONECE_ENEMY = 24,
	//设置好友vip
	SS_FRIEND_UPDATE_VIP = 26,
	//组队活动增加亲密度
	SS_FRIEND_TEAM_ADD_INTIMACY = 27,
};

enum FriendS2C
{
	S2C_SEEK_AID , //寻求援助 玩家名 sceneid
	S2C_APPLAY_SWORN,// 申请结义 wstr 玩家名
	S2C_FRIEND_SYSTEM_RECOMMEND ,// 系统好友推荐
	S2C_FRIEND_INTIMACY_AWARD,//亲密度礼包奖励

};

#endif