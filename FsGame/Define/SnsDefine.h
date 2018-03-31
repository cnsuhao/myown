//--------------------------------------------------------------------
// 文件名:		SnsDefine.h
// 内  容:		游戏服务器与sns数据服务器之间传递的消息定义
// 说  明:		游戏服务器向sns数据服务器发送消息时，前三个字段总是：
//				fast_string snsspace, wstring snsdata, int msgid
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )   
//    :	    
//--------------------------------------------------------------------

#ifndef __SnsDefine_h__
#define __SnsDefine_h__

//SnsSpace名称
const char* const SPACE_SNS = "sns";

//操作数据的消息ID范围（消息ID的编号不能在此范围）
#define SNSDATA_MSG_ID_MIN      1000
#define SNSDATA_MSG_ID_MAX      1999

//操作所有SnsData的消息
enum
{
	//修改属性，格式：string pubspace, wstring pubdata, int msgid, string prop, var value, ...;
	SNSDATA_MSG_SET_PROP = 1000,
	//增加属性，格式：string pubspace, wstring pubdata, int msgid, string prop, int value, ...;
	SNSDATA_MSG_INC_PROP = 1001,
	//清除某个表的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	SNSDATA_MSG_CLEAR_REC = 1100,
	//删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, ...;
	SNSDATA_MSG_REMOVE_REC_ROW = 1101,
	//添加某个表的一行内容，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	SNSDATA_MSG_ADD_REC_ROW_VALUE = 1102,
	//设置某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	SNSDATA_MSG_SET_REC_VALUE = 1103,
	//增加某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	SNSDATA_MSG_INC_REC_VALUE = 1104,
	//根据关键值删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue;
	SNSDATA_MSG_REMOVE_REC_ROW_BYKEY = 1105,
	//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	SNSDATA_MSG_SET_REC_VALUE_BYKEY = 1106,
	//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	SNSDATA_MSG_INC_REC_VALUE_BYKEY = 1107,
	//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid
	SNSDATA_MSG_UPDATE_CONFIG = 1108,
	//修改最大循环次数
	SNSDATA_MSG_SET_MAX_CIRCULATE_COUNT = 1109,
};

enum
{
	OFFLINE = 0, //下线
	ONLINE = 1, //上线
};

// SNS消息(由逻辑服务器发往SNS服务器的消息)
enum                                
{
	// 拉玩家上线
	SNS_MSG_PUSH_PLAYER_OFFLINE = 2000,
	// 推送注册信息
	SNS_MSG_PUSH_PLAYER_REGISTER = 2001,
	// 发送广播消息
	SNS_MSG_PUSH_PLAYER_SEND_MSG = 2002, //wstring 标题，wstring内容
	// 查询某个玩家所有的战斗数据,包括属性、容器
	SNS_MSG_QUERY_PLAYER_FIGHT_DATA = 2003,
	// 取消拉离线玩家上线
	SNS_MSG_CANCEL_PUSH_OFFLINE_PLAYER = 2004,
	// 玩家下线定时消息
	SNS_MSG_PUSH_PLAYER_OFFLINE_TIME = 2005, //int 0下线1上线
	// 发送广播消息
	SNS_MSG_PUSH_PLAYER_SEND_MSG_SINGLE = 2006,
	//查找玩家属性(支持:int,int64,float,double,sting) cmd_id(int32),"attr_name,attr_name",{ident_id(int64),target_uid(char)}..=>{ident_id,"attr_name=attr_value,attr_name=attr_value"}
	SNS_MSG_QUERY_PLAYER_PROPERTY = 2007,

	//////////////////////////////////////////////////////////////////////////
	// 好友系统消息定义
	SNS_MSG_FRIEND_APPLY = 2100,            // 好友申请
	SNS_MSG_FRIEND_DELETE,					// 好友解除
	SNS_MSG_FRIEND_RESULT,                  // 申请结果
	SNS_MSG_FRIEND_QUERY_FRIEND_APPLY,      // 查询申请
	SNS_MSG_FRIEND_QUERY_FRIEND_DELETE,     // 删除申请
	SNS_MSG_FRIEND_QUERY_FRIEND_RESULT,     // 查询结果
	SNS_MSG_FRIEND_PUSH_APPLY,              // 推送申请
	SNS_MSG_FRIEND_PUSH_DELETE,             // 推送解除关系
	SNS_MSG_FRIEND_PUSH_RESULT,             // 推送结果
	SNS_MSG_FRIEND_GIFT_GIVE,				// 离线送花
	SNS_MSG_FRIEND_GIFT_GET,				// 送花获取
	SNS_MSG_FRIEND_GIFT_PUSH,				// 送花推送
	SNS_MSG_FRIEND_PUSH_ADD_INTIMACY,		// 离线增加亲密度保存
	SNS_MSG_FRIEND_ADD_INTIMACY_GET,		// 获取增加亲密度
	SNS_MSG_FRIEND_ADD_INTIMACY_RESULT,		// 获取增加亲密度结果
	SNS_MSG_FRIEND_ADD_DEL_SWORN,				// 添加删除结义
	SNS_MSG_FRIEND_QUERY_DEL_SWORN,				// 获取删除结义
	SNS_MSG_FRIEND_PUSH_DEL_SWORN,				// 返回删除结义
	SNS_MSG_FRIEND_ADD_ENEMY,				//增加敌人
	SNS_MSG_FRIEND_QUERY_ENEMY,				//获取敌人
	SNS_MSG_FRIEND_PUSH_ENEMY,				//返回敌人

	//////////////////////////////////////////////////////////////////////////
	// 竞技场相关
	SNS_MSG_ARENA_SAVE_FIGHT_RESULT = 2200,					// 竞技场保存战报数据
	SNS_MSG_ARENA_QUERY_FIGHT_RESULT,					 // 竞技场查询离线战报数据
	SNS_MSG_ARENA_PUSH_FIGHT_RESULT,					 // 推送离线战报数据

};

#endif // __SnsDefine_h__
