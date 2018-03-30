//--------------------------------------------------------------------
// 文件名:		PubDefine.h
// 内  容:		游戏服务器与公共数据服务器之间传递的消息定义
// 说  明:		游戏服务器向公共数据服务器发送消息时，前三个字段总是：
//				fast_string pubspace, wstring pubdata, int msgid
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------
#ifndef __PubDefine_h__
#define __PubDefine_h__

//PubSpace名称
const char* const PUBSPACE_DOMAIN = "domain";
const char* const PUBSPACE_GUILD = "guild";

//二级PubSpace名称
const wchar_t* const GUILD_LOGIC = L"Guild_logic";
const wchar_t* const DOMAIN_CHANNEL = L"Domain_Channel_%d";

const wchar_t* const DOMAIN_NATION = L"Domain_Nation";

const wchar_t* const DOMAIN_TEAM = L"Domain_Team";

const wchar_t* const DOMAIN_NATIONOFFICIAL = L"Domain_NationOfficial_%d";

const wchar_t* const DOMAIN_LOOK = L"Domain_Look_%d";

//操作公共数据的消息ID范围（Domain和Guild的消息ID的编号不能在此范围）
#define PUBDATA_MSG_ID_MIN      1000
#define PUBDATA_MSG_ID_MAX      1999

//操作所有PubData的消息
enum
{
	//修改属性，格式：string pubspace, wstring pubdata, int msgid, string prop, var value, ...;
	PUBDATA_MSG_SET_PROP = 1000,
	//增加属性，格式：string pubspace, wstring pubdata, int msgid, string prop, int value, ...;
	PUBDATA_MSG_INC_PROP = 1001,
	//清除某个表的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	PUBDATA_MSG_CLEAR_REC = 1100,
	//删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, ...;
	PUBDATA_MSG_REMOVE_REC_ROW = 1101,
	//添加某个表的一行内容，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	PUBDATA_MSG_ADD_REC_ROW_VALUE = 1102,
	//设置某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	PUBDATA_MSG_SET_REC_VALUE = 1103,
	//增加某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	PUBDATA_MSG_INC_REC_VALUE = 1104,
	//根据关键值删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue;
	PUBDATA_MSG_REMOVE_REC_ROW_BYKEY = 1105,
	//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	PUBDATA_MSG_SET_REC_VALUE_BYKEY = 1106,
	//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	PUBDATA_MSG_INC_REC_VALUE_BYKEY = 1107,
	//更新配置，命令格式：string pubspace, wstring pubdata, int msgid
	PUBDATA_MSG_UPDATE_CONFIG = 1108,
	//修改最大循环次数
	PUBDATA_MSG_MAX_CIRCULATE_COUNT = 1109,
	//增加某个表，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int maxrow, int cols, ...;
	PUBDATA_MSG_ADD_REC = 1110,
};

// DOMAIN相关消息（场景服与公共服间的通讯消息）
enum //Scene->Pub
{
	//添加已创建的场景，格式：string "domain", wstring L"Domain_SceneList", int msgid, int sceneid, string scene_nameid
	SP_DOMAIN_MSG_SCENELIST_ADD_SCENE = 1,
	// 删除已销毁的场景，格式：string "domain", wstring L"Domain_SceneList", int msgid, int sceneid, string scene_nameid
	SP_DOMAIN_MSG_SCENELIST_DEL_SCENE,
	//监视货币日增加异常数据（记一次异常资金记录）
	SP_DOMAIN_MSG_CAPITAL_MONITOR,
	// 功能开关的设置 格式：int functionid, int state
	SP_DOMAIN_MSG_FUNCTION_STATE,
	// 通用排行榜的消息, 子消息ID见 RankListDefine.h
	SP_DOMAIN_MSG_GENERAL_RANK_LIST,

	// public服务准备就绪
	SP_SERVICE_READY_PUBLIC,		

	//// 组队相关
	//当队伍重新上线，格式：string "domain", wstring L"Domain_Team", int msgid, int team_id,wstring player_name
	SP_DOMAIN_MSG_TEAM_ON_RECOVER,
	//创建队伍，格式：string "domain", wstring L"Domain_Team", int msgid, wstring captain_name, wstring member_name,int team_camp
	SP_DOMAIN_MSG_TEAM_CREATE,
	//解散队伍，格式：string "domain", wstring L"Domain_Team", int msgid, int nTeamID, wstring player_name
	SP_DOMAIN_MSG_TEAM_DESTROY,
	//加入队伍，格式：string "domain", wstring L"Domain_Team", int msgid, int nTeamID, wstring player_name, IVarList values
	SP_DOMAIN_MSG_TEAM_JOIN,
	//离开队伍，格式：string "domain", wstring L"Domain_Team", int msgid, int nTeamID, wstring player_name, int reason(0自己离开，1被队长开除)
	SP_DOMAIN_MSG_TEAM_LEAVE,
	//更新队伍成员信息，格式：string "domain", wstring L"Domain_Team", int msgid, int nTeamID, wstring player_name, int col_1, IVar value_1, ..., int col_n, IVar value_n
	SP_DOMAIN_MSG_TEAM_UPDATE_MEMBER,
	//更新队伍信息(更换队长等)，格式：string "domain", wstring L"Domain_Team", int msgid, int nTeamID, wstring player_name, int col, IVar value
	SP_DOMAIN_MSG_TEAM_UPDATE_INFO,
	//设置队员位置和职位，格式：string "domain", wstring L"Domain_Team", int msgid, int team_id,int set_param,wstring player_name
	SP_DOMAIN_MSG_TEAM_SET_MEMBER,
	//增加到匹配列表string "domain", wstring L"Domain_Team", int msgid,wstring player_name, int col_1, IVar value_1, ..., int col_n, IVar value_n 
	SP_DOMAIN_MSG_TEAM_ADD_AUTO_MATCH_LIST,
	//查询匹配队伍信息  "domain", wstring L"Domain_Team", int msgid
	SP_DOMAIN_MSG_TEAM_MATCH_INFO,
	//玩家下线
	SP_DOMAIN_MSG_TEAM_PLAYER_STORE,
};

// 公会相关消息（场景服与公共服间的通讯消息）
enum
{
	///////////////////////////////////////////////////////////////////////////
	// scene_server -> pub_server
	///////////////////////////////////////////////////////////////////////////
	SP_GUILD_MSG_CREATE = 200,              // 请求创建公会
	SP_GUILD_MSG_APPLY_JOIN,                // 请求加入公会
	SP_GUILD_MSG_ACCEPT_JOIN,               // 接受加入公会申请的请求
	SP_GUILD_MSG_REFUSE_JOIN,               // 拒绝加入公会申请
	SP_GUILD_MSG_MEMBER_ONLINE_STATE,       // 公会成员在线状态改变
	SP_GUILD_MSG_DELETE_JOIN_RECORD,        // 请求删除不在线玩家加入公会表格的一条记录
	SP_GUILD_MSG_UPDATE_PROPS,              // 请求更新成员属性（等级、战斗力）
	SP_GUILD_MSG_UPDATE_APPLY_PROPS,        // 请求更新申请表中的属性（等级、战斗力）
	SP_GUILD_MSG_UPDATE_APPLY_ONLINE_STATE, // 公会成员在线状态改变
	SP_GUILD_MSG_CANCEL_APPLY_JOIN,         // 取消加入公会请求
	SP_GUILD_MSG_CHANGE_NOTICE,             // 修改公告
	SP_GUILD_MSG_QUIT,                      // 退出公会
	SP_GUILD_MSG_FIRE,                      // 踢出公会
	SP_GUILD_MSG_DISMISS,                   // 解散公会
	SP_GUILD_MSG_DONATE,                    // 公会捐献
	SP_GUILD_MSG_PROMOTION,                 // 公会升职
	SP_GUILD_MSG_DEMOTION,                  // 公会降职
	SP_GUILD_MSG_BUY_ITEM,                  // 请求购买公会商店物品，扣除个人贡献值
	SP_GUILD_START_ONE_KEY_APPLY_JOIN,		//一键申请加入
	SP_GUILD_CHANGE_DECLARATION,			// 修改宣言
	SP_GUILD_MSG_CAPTAIN_TRANSFER,          // 请求移交帮主
	SP_GUILD_MSG_CHAT_INFO,					// 请求保存聊天信息
	SP_GUILD_RECORD_GUILD_LOG,				// 记录帮会日志 int 日志类型 日志参数
	SP_GUILD_SET_AUTO_AGREE_STATE,			//设置帮会自动加入
	SP_GUILD_AGREE_INVITE_JOIN_GUILD,		//同意邀请加入公会
	SP_GUILD_TIGGER_TIMER,					//触发定时器
	SP_GUILD_DEC_GUILD_NUM,					//扣除帮派资金
	SP_GUILD_ADD_GUILD_NUM,					//增加组织数值
	SP_GUILD_UNLOCK_SKILL,					//组织技能解锁
	SP_GUILD_BUILDING_LVUP_OPERATE,			//建筑升级
	SP_GUILD_BUILDING_LEVEL_ADD_EXP,		//建筑增加经验
	SP_GUILD_NUM_BUY,						//购买公会数值
	SP_GUILD_SKILL_LVUP_OPERATE,				//开始研究技能

	///////////////////////////////////////////////////////////////////////////
	// pub_server -> scene_server  300~399
	///////////////////////////////////////////////////////////////////////////
	PS_GUILD_MSG_CREATE = 300,              // 创建公会结果回复
	PS_GUILD_MSG_APPLY_JOIN,                // 加入公会请求结果
	PS_GUILD_MSG_ACCEPT_JOIN,               // 接受申请的结果
	PS_GUILD_MSG_REFUSE_JOIN,               // 拒绝了加入公会的申请
	PS_GUILD_MSG_CANCEL_APPLY,              // 取消申请加入公会
	PS_GUILD_MSG_CHANGE_NOTICE,             // 修改公告结果
	PS_GUILD_MSG_QUIT,                      // 退出公会回复
	PS_GUILD_MSG_FIRE,                      // 踢出公会回复
	PS_GUILD_MSG_DISMISS,                   // 解散公会回复
	PS_GUILD_MSG_DISMISS_PUBLIC,            // 解散公会回复
	PS_GUILD_MSG_GUILD_GONE,                // 通知成员公会解散
	PS_GUILD_MSG_DONATE,                    // 公会捐献结果
	PS_GUILD_MSG_PROMOTION,                 // 公会升职
	PS_GUILD_MSG_DEMOTION,                  // 公会降职
	PS_GUILD_MSG_GM_MODIFY_PROP,            // GM修改公会属性的结果
	PS_GUILD_CHANGE_DECLARATION,        	// 修改公告结果
	PS_GUILD_MSG_CAPTAIN_TRANSFER,          // 请求移交帮主
	PS_GUILD_CHANGE_GUILD_SHORT_NAME,		// 组织短名
	PS_GUILD_MSG_NEW_APPLY,					// 新的请求
	PS_GUILD_MSG_MODIFY_DEVOTE,             // 通知修改了个人剩余贡献值
	PS_GUILD_MSG_BUY_ITEM,                  // 通知购买商品扣除贡献值结果
	PS_GUILD_NUM_CHANGE,				    // 帮会资金变动
};

//DOMAIN服务器下传消息定义 400~499
enum //Pub->Scene
{
	//public服务准备就绪
	PS_SERVICE_READY_PUBLIC = 400,
	//场景数据消息
	PS_SCENE_DATA_MSG,

	//设置队伍成员职位，格式：string "domain", wstring L"Domain_Team", int msgid,wstring player_name
	PS_DOMAIN_SERVERMSG_TEAM_SET_MEMBERS_WORK,

	//设置判断离线成员上线是否成功，格式：string "domain", wstring L"Domain_Team", int msgid, wstring member_name,int nTeamId
	PS_DOMAIN_SERVERMSG_TEAM_SET_PLAYERONRECOVER,

	//队伍创建，格式：string "domain", wstring L"Domain_Team", int msgid, wstring captain_name, wstring member_name, int nTeamID
	PS_DOMAIN_SERVERMSG_TEAM_CREATE,

	//队伍解散，格式：string "domain", wstring L"Domain_Team", int msgid, wstring member_name
	PS_DOMAIN_SERVERMSG_TEAM_DESTROY,

	//成员加入队伍，格式：string "domain", wstring L"Domain_Team", int msgid, wstring player_name, int nTeamID(加入的member_name角色需要此信息)
	PS_DOMAIN_SERVERMSG_TEAM_JOIN,

	//成员离开队伍，格式：string "domain", wstring L"Domain_Team", int msgid, wstring member_name, wstring player_name, int reason(0自己离开，1被队长开除)
	PS_DOMAIN_SERVERMSG_TEAM_LEAVE,

	//成员更新信息，格式：string "domain", wstring L"Domain_Team", int msgid, wstring member_name, wstring player_name, int col
	PS_DOMAIN_SERVERMSG_TEAM_UPDATE_MEMBER,

	//队伍更新信息，格式：string "domain", wstring L"Domain_Team", int msgid, wstring member_name, int col
	PS_DOMAIN_SERVERMSG_TEAM_UPDATE_INFO,

	//刷新队伍成员人数，格式：string "domain", wstring L"Domain_Team", int msgid
	PS_DOMAIN_SERVERMSG_TEAM_REFRSH_NUM,

	//通知队伍增加成员  格式： string "domain", wstring L"Domain_Team", int msgid, wstring member_name, wstring player_name, int nTeamID(加入的member_name角色需要此信息)
	PS_DOMAIN_SERVERMSG_TEAM_ADD_MEMBER,
};

// 公会相关操作结果
enum
{
	GUILD_MSG_REQ_FAIL = 0,     // 请求失败
	GUILD_MSG_REQ_SUCC = 1,     // 请求成功
};

#endif // __PubDefine_h__
