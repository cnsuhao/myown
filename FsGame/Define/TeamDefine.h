//--------------------------------------------------------------------
// 文件名:	  TeamDefine.h
// 内 容:	组队公共头文件
// 说 明:		
// 创建日期:	2014年11月19日
// 创建人:		 
//    :	    
//--------------------------------------------------------------------
#ifndef _FSGAME_TEAM_DEFINE_H_
#define _FSGAME_TEAM_DEFINE_H_
#define FORBID_TEAM_SCENEID  701
// 队伍表
#define TEAM_REC_NAME "team_rec"

//队伍主表名称
#define TEAM_MAIN_REC_NAME "team_main_rec"

#define TEAM_SCENE_CONFIG "ini/SocialSystem/Team/TeamType.xml"
#define TEAM_ACTIVITY_SCENE_CONFIG "ini/SocialSystem/Team/ActivityScene.xml"
// 队伍主表列定义
enum
{
    TEAM_MAIN_REC_COL_TEAM_ID,                  // 队伍ID （TYPE：VTYPE_INT）
    TEAM_MAIN_REC_COL_TEAM_MEMBER_REC_NAME,		// 成员表名称（TYPE：VTYPE_STRING）
    TEAM_MAIN_REC_COL_TEAM_CAPTAIN,             // 队长名称（TYPE：VTYPE_WIDESTR）
    TEAM_MAIN_REC_COL_PASSWORD,                 // 队伍密码（TYPE：VTYPE_WIDESTR）
    TEAM_MAIN_REC_COL_TEAM_UNIQUE_ID,           // 队伍的唯一ID（TYPE：VTYPE_STRING）
	TEAM_MAIN_REC_COL_TEAM_INFO,                // 队伍信息（TYPE：VTYPE_WIDESTR）
	TEAM_MAIN_REC_COL_TEAM_STATE,				// 队伍状态(匹配状态)（TYPE：VTYPE_INT）
	TEAM_MAIN_REC_COL_TEAM_DIG_INFO,			// 队伍挖宝信息 （TYPE：VTYPE_STRING）
	TEAM_MAIN_REC_COL_TEAM_VIEW,				// 队伍公开状态 （TYPE：VTYPE_INT）
	TEAM_MAIN_REC_COL_TEAM_AUTO_JOIN,			// 队伍自动加入 (TYpe: VTYPE_INT)
	TEAM_MAIN_REC_COL_TEAM_OBJECT_INDEX,		// 目标索引ID(TYpe: VTYPE_INT)
	TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH,			// 自动匹配(TYpe: VTYPE_INT)
	TEAM_MAIN_REC_COL_TEAM_LIMIT_MIN_LEVEL,		// 最低等级
	TEAM_MAIN_REC_COL_TEAM_LIMIT_MAX_LEVEL,		// 最高等级
	TEAM_MAIN_REC_COL_TEAM_OFF_JOIN_TEAM,		// 禁止加入队伍
    TEAM_MAIN_REC_COL_COUNT,
};

// 队伍成员表列定义
enum
{
	TEAM_REC_COL_UID,           //队员UID
    TEAM_REC_COL_NAME,           //队员名
	TEAM_REC_COL_BGLEVEL,		 //等级
	TEAM_REC_COL_BGJOB,          //职业
	TEAM_REC_COL_SEX,          //性别
	TEAM_REC_COL_FIGHT,          //战斗力
	TEAM_REC_COL_ISOFFLINE,		 //上次离线时间
	TEAM_REC_COL_TEAMWORK,		 //在队伍中职位      

    TEAM_REC_COL_SCENE,          //场景编号
    TEAM_REC_COL_BUFFERS,		 //buffer列表
    TEAM_REC_COL_HP,             //当前生命值
    TEAM_REC_COL_GUILDNAME,		 //公会名
    TEAM_REC_COL_MAXHP,          //最大生命值
    TEAM_REC_COL_FOLLOW_STATE,   //跟随状态
    TEAM_REC_COL_READY,			// 准备状态
	TEAM_REC_COL_CLONE_ID,		 //副本id
	TEAM_REC_COL_ONLINE,		 //Online
	TEAM_REC_COL_POSITION_X,		 //x坐标
	TEAM_REC_COL_POSITION_Z,		 //z坐标
	TEAM_REC_COL_BE_KICK,		//能否被踢出队伍(0是可以，1是不可以)
	TEAM_REC_COL_FIGHT_STATE,	 //战斗状态	
	TEAM_REC_COL_VIP_LV,			//vip
    TEAM_REC_COL_COUNT,
};
enum TeamType
{
	TeamNone = 0,//无
	TeamSecret,//装备副本
	TeamEscort,//押囚
	TeamWorldBoss,//世界boss
	TeamCamp,//阵营战
	TeamWanted,//悬赏
	TeamBorder,//边境
	TeamGuild,//组织入侵
	TeamBianstone,//砭石副本
};
// 准备状态
enum TeamReadyStates
{
	TEAM_READY_STATE_NONE = 0,	// 无状态
	TEAM_READY_STATE_READY,	// 准备状态
};

// 队伍拾取分配方式划分
enum
{
    TEAM_PICK_MODE_FREE,        // 自由拾取
    TEAM_PICK_MODE_RANDOM,      // 随机分配
    TEAM_PICK_MODE_ROLL,        // Roll点拾取
    TEAM_PICK_MODE_CAPTAIN,     // 队长拾取
    TEAM_PICK_MODE_NEED,        // 需求拾取
    TEAM_PICK_MODE_COUNT,
};

enum
{
	TEAM_STATE_IDLE = 0,	//空闲中
	TEAM_STATE_GAMING = 1,	//游戏中
};


enum 
{
	TEAM_PRIVATE = 0,  //私有队伍
	TEAM_PUBLIC = 1,   //公开队伍
};
enum 
{
	TEAM_AUTO_JOIN_CLOSE, //自动加入队伍关闭
	TEAM_AUTO_JOIN_OPEN,  //自动加入队伍打开

};

// 队伍类型定义

// 队伍内职位
enum
{
    TYPE_TEAM_PLAYER,    // 组员
    TYPE_TEAM_CAPTAIN,   // 队长
};

//离开队伍
enum LEAVE_TYPE
{
    LEAVE_TYPE_EXIT,      //自己离队
    LEAVE_TYPE_EXPEL,	  //被队长踢出
    LEAVE_TYPE_OFFLINE,	  //掉线（队伍解散）
};

enum TEAM_MATCH
{
    TEAM_REQUEST_NO,       //无请求权限
    TEAM_REQUEST_YES,      //可以发送请求
};


enum TEAM_AUTO_MATCH
{
	TEAM_AUTO_MATCH_OFF, // 关闭
	TEAM_AUTO_MATCH_ON, //打开
	TEAM_AUTO_MATCH_BUSY,//繁忙

};

enum
{
	TEAM_JOIN_TEAM_ON, //能够加入队伍
	TEAM_JOIN_TEAM_OFF, //不能够加入队伍


};


enum TEAM_IS_KICK
{
	TEAM_IS_KICK_OFF = 0, //能被踢(默认能被踢)
	TEAM_IS_KICK_ON = 1,  //不能踢
};

// 更新队员位置开关
enum UpdatePosiSwitch
{
	UPDATE_POSI_ON = 1,	// 打开
	UPDATE_POSI_OFF = 2,	// 关闭
};

// 客户端二级消息
enum CUSTOM_MESSAGE_TEAM
{
    CLIENT_CUSTOMMSG_TEAM_CREATE = 1,					//创建队伍
    CLIENT_CUSTOMMSG_TEAM_KICKOUT = 2,                  // 踢出队伍
    CLIENT_CUSTOMMSG_TEAM_DESTROY = 3,                  // 解散队伍
    CLIENT_CUSTOMMSG_TEAM_LEAVE = 4,                    // 退出队伍
    CLIENT_CUSTOMMSG_TEAM_CHANGE_CAPTAIN = 5,			//移交队长
	CLIENT_CUSTOMMSG_TEAM_NEARBY = 6,				//附近队伍
	CLIENT_CUSTOMMSG_TEAM_MEMBER_POSITION_UPDATE = 7,	//请求刷新队伍成员位置
    CLIENT_CUSTOMMSG_TEAM_CLEAR_APPLY = 8,              //清除申请
    CLIENT_CUSTOMMSG_TEAM_APPLY_LIST = 9,              //获取申请列表
	CLIENT_CUSTOMMSG_TEAM_SET_VIEW = 10,			 //设置公开状态
    CLIENT_CUSTOMMSG_TEAM_REFRESH = 11,              //通知刷新列表
	CLIENT_CUSTOMMSG_TEAM_SET_AUTO_JOIN = 12,		 //自动加入
	CLIENT_CUSTOMMSG_TEAM_GET_TEAM_INFO = 13,		  //队伍信息
	CLIENT_CUSTOMMSG_TEAM_SET_TEAM_OBJECT_INDEX = 14,	 //设置队伍目标类型
	CLIENT_CUSTOMMSG_TEAM_SET_TEAM_LIMIT_LEVEL = 15,	 //设置队伍限制等级
	CLIENT_CUSTOMMSG_TEAM_ENTRY_SECRET_SCENE =  16,			//进入组队秘境
	CLIENT_CUSTOMMSG_TEAM_AUTO_MATCH =  17,			//自动匹配

	CLIENT_CUSTOMMSG_TEAM_GET_TEAM_LIST =  18,			//获取队伍列表
	CLIENT_CUSTOMMSG_TEAM_MATCH_TEAM_INFO = 19,		//获取自动匹配队伍的信息
	CLIENT_CUSTOMMSG_TEAM_TRANSFER_CAPTAIN_SITE = 20,  //传送队长所在场景
	CLIENT_CUSTOMMSG_TEAM_JOIN_TEAM_RESULT			= 21,  //加入队伍(成功返回)
	CLIENT_CUSTOMMSG_TEAM_REQ_ADD_TEAM = 22,		 //请求加入队伍 int 请求类型 string 名字 int 等级 int 战力 int 性别 int 职业
	CLIENT_CUSTOMMSG_TEAM_CONVENE = 23,				//召集
	CLIENT_CUSTOMMSG_TEAM_FULL = 24,				//队伍已满
	CLIENT_CUSTOMMSG_SET_TEAM_FOLLOW = 25,			//设置队伍跟随状态

	// 请求时无参数
	// 返回时
	/*!
	* @brief	返回推荐列表
	* @param	int			人数n
	以下循环n次
	* @param	wstring		名字
	* @param	int			等级
	* @param	int			国籍
	* @param	int			职业
	* @param	int			性别
	* @param	int			战斗力
	*/
	CLIENT_CUSTOMMSG_TEAM_REQUEST_RECOMMAND_LIST = 26,			//获取组队推荐列表

};

enum TEAM_TYPE
{
	TEAM_TYPE_NONE  = 0,//无类型
	TEAM_TYPE_CLONE = 1,//副本
};
//队伍检查
enum TEAM_CHECK_CONDITION_MSG
{
	COMMAND_TEAM_CHECK_REQ = 1, //检查请求
	COMMAND_TEAM_CHECK_RSP = 2, //检测结果返回
};
//检查请求
enum TEAM_CHECK_CONDITION_REQ
{
	
	COMMAND_TEAM_CHECK_NONE = 0,//无类型
	COMMAND_TEAM_CHECK_ENTRY_SECRET_CONDITION = 1,	//组队秘境检查
	COMMAND_TEAM_CHECK_ESCORT = 2,	// 组队押囚
	COMMAND_TEAM_CHECK_TEAMOFFER = 5,	// 组队悬赏
	COMMAND_TEAM_CHECK_GUILD_INBREAK = 8,//组织入侵组队
};
// 组队服务端二级消息
enum COMMAND_TEAM_SECOND_MSG
{
	// 创建队伍
	COMMAND_TEAM_CREATE_MSG = 1,
	// 添加成员
	COMMAND_TEAM_ADDMEM_MSG = 2,
	// 离开队伍
	COMMAND_TEAM_LEAVE_MSG = 3,
	// 移交队长
	COMMAND_TEAM_CHGCAP_MSG = 4,
	// T出队伍
	COMMAND_TEAM_KICKOUT_MSG = 5,
	// 队员位置更新
	COMMAND_TEAM_UPDATE_MEMBER_POSI_MSG = 6, 
	//加入队伍
	COMMAND_TEAM_TEAM_JOIN = 7,
	//加入成员
	COMMAND_TEAM_TEAM_ADD_MUMBER = 8,

	//获取目标坐标
	COMMAND_GET_TARGET_POS_REQ,
	//获得目标回应
	COMMAND_GET_TARE_POS_RSP,
	//清除申请列表中的人
	COMMAND_DEL_PLAYER_APPLAY,

};

// 附近队伍id和距离
struct NearbyTeamRecommend 
{
	NearbyTeamRecommend(): teamid(0), distance(0.0f){}
	int teamid;
	float distance;
	PERSISTID playid;
};

//队伍上限人数
#define MAX_MEMBER_IN_NORMALTEAM 4

#define FOR_EACH_TEAM_MEMBER(pKernel, player) \
    CVarList teamList; \
    TeamModule::m_pTeamModule->GetTeamMemberList(pKernel,player,teamList); \
    for(int i=0; i<static_cast<int>(teamList.GetCount()); i++)




typedef bool(*CheckTeamCondition)(IKernel*pKernel, const PERSISTID& self, const IVarList &var);

typedef void(*CheckTeamResult)(IKernel*pKernel, const PERSISTID& self, const IVarList &var);

typedef bool(*MatchFinal)(IKernel*pKernel, const PERSISTID& self,int objID);

#endif // _FSGAME_TEAM_DEFINE_H_
