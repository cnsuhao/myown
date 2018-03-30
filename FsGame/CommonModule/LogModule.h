//--------------------------------------------------------------------
// 文件名:      LogModule.h
// 内  容:      日志模块
// 说  明:
// 创建日期:    2014年10月20日
// 整理日期:    2015年09月10日
// 创建人:       
// 整理人:       
//    :       
//--------------------------------------------------------------------
#ifndef __LOGMODULE_H__
#define __LOGMODULE_H__

#include "Fsgame/Define/header.h"
#include "FsGame/Define/LogDefine.h"

//#ifndef FSROOMLOGIC_EXPORTS

#include "log4cplus/logger.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/layout.h"
#include "log4cplus/configurator.h"

//#endif
#include "FsGame/Analytics/IGameAnalyticsCollector.h"
#include "extension/FsGmCC/model/ServerInfo.h"





//游戏玩法日志结构
struct GamePlayerActionLog
{
	GamePlayerActionLog()
		:name(L""),
		nation(0),
		actionType(0),
		actionState(0),
	    comment(L"")
	{}
	std::wstring     name;                 //昵称
	int              nation;               //国家
	int              actionType;           //玩法类型 [ LogDefine: LOG_GAME_ACTION_TYPE ]
	int              actionState;          //玩法状态 [ LogDefine: LOG_GAME_ACTION_STATE] 
    std::wstring     comment;              //扩展字段:可不填
}; 

//角色登录日志结构
struct RoleLog
{
	RoleLog()
		:ip(""),
		loginTime(0),
		logoutTime(0),
		mac(""),
		osType(""),
		comment(L"")
	{}
	std::string     ip;              //登陆IP (varchar)
	time_t          loginTime;       //上线时间 (datetime)
	time_t          logoutTime;      //下线时间  (datetime)
	std::string     mac;             //客户端MAC (varchar)
	std::string     osType;          //设备系统类型 (varchar)
    std::wstring    comment;         //扩展字段:可不填
};

//任务日志结构
struct TaskLog
{
	TaskLog()
		:taskID(0),
		type(0),
		opt_type(0), 
		state(0),
		comment(L"")
	{}
	int	taskID;          // 任务ID 
	int	type;            // 类型
	int state;			 // 状态
	int	opt_type;		 // 操作类型（0：正常 1：跳过）
    std::wstring	comment;         // 扩展字段:可不填
};

//货币日志结构
struct MoneyLog
{
	MoneyLog()
		:capitalType(0),
		eventID(0),
		eventType(0),
		eventSerial(""),
		before(0),
		money(0),
		after(0),
        senderName(L""),
        senderUid(""),
		state(0),
		comment(L"")
	{}
	int             capitalType;     //币种类型 (int)
	int             eventID;         //异动途径 [ LogDefine: LOG_CAPITAL_EVENT_TYPE ]
	int             eventType;       //异动类型 [ LogDefine: LOG_ACT_EVENT_TYPE ]
	std::string     eventSerial;     //异动序列号
	__int64         before;          //异动前金钱存量
	__int64         money;           //异动金额
	__int64         after;           //异动后金钱存量
	std::wstring    senderName;      //另一方玩家昵称
	std::string     senderUid;       //另一方玩家uid
	int             state;           //异动状态
    std::wstring    comment;         //扩展字段:可不填
};

//物品日志结构
struct ItemLog
{
	ItemLog()
		:eventID(0),
		eventType(0),
		eventSerial(""),
		itemID(""),
		itemUid(""),
		itemLifeTime(0),
		before(0),
		itemNum(0),
		after(0),
		colorLevel(-1),
		senderName(L""),
		senderUid(""),
		state(0),
		comment(L"")
	{}
	int             eventID;         //异动途径ID [ LogDefine: LOG_ITEM_TYPE ]
	int             eventType;       //异动类型   [ LogDefine: LOG_ACT_EVENT_TYPE ]
	std::string     eventSerial;     //异动序列号
	std::string     itemID;          //物品ID
	std::string     itemUid;         //物品uid
	int             itemLifeTime;    //物品有效时间
	int             before;          //异动前金钱存量
	int             itemNum;         //物品异动数量
	int             after;           //异动后金钱存量)
	int             colorLevel;      //装备品质(int)初始化
	std::wstring    senderName;      //另一方玩家昵称
	std::string     senderUid;       //另一方玩家uid
	int             state;           //异动状态
    std::wstring    comment;         //扩展字段:可不填
};

//角色升级结构
struct RoleUpgradeLog
{
	RoleUpgradeLog()
		:type(0),
		levelBefore(0),
		levelAfter(0),
		skillID(""),
		comment(L"")
	{}
	int             type;            //升级类型 (varchar)  LOG_ROLE_UPGRADE_TYPE
	int             levelBefore;     //升级前等级 (int)
	int             levelAfter;      //升级后等级 (int)
	std::string     skillID;         //技能ID (varchar)
    std::wstring    comment;         //扩展字段:可不填
};

//惩罚日志结构
struct PunishLog
{
	PunishLog()
		:gmAccount(""),
		type(0),
		minutes(0),
		comment(L"")
	{}
	std::string     gmAccount;       //操作人通行证 (varchar)
	int             type;            //惩罚类型 (int)
	int             minutes;         //惩罚时间 (int)
    std::wstring    comment;         //扩展字段:可不填
};

//帮会日志结构
struct GuildLog
{
	GuildLog()
		:account("0"),
	    playerName(L""),
	    guildName(L""),
		grade(0),
		byRoleName(L""),
		operateType(0),
        capitalType(0),
		capitalNum(0),
		donateNum(0),
		comment(L"")
	{}
	std::string     account;         //操作账户
	std::wstring    playerName;      //操作人昵称
	std::wstring    guildName;       //公会名称
	int             grade;           //公会等级
	std::wstring    byRoleName;      //被操作人角色名 (varchar)
	int             operateType;     //操作类型 (int)
	int             capitalType;     //币种类型 (int) 
	int             capitalNum;      //异动后值 (int)
	int             donateNum;       //帮贡 
    std::wstring    comment;         //扩展字段:可不填
};

//BOSS日志结构
struct BossLog
{
	BossLog()
		:sceneID(0),
		bossID(""),
		comment(L"")
	{}
	int             sceneID;         //场景编号 (varchar)
	std::string     bossID;          //boss编号 (varchar)
    std::wstring    comment;         //扩展字段:可不填
};


//场景日志结构
struct SceneLog
{
	SceneLog()
		:loginTime(0),
		logoutTime(0),
		comment(L"")
	{}
	int64_t         loginTime;       //进入场景时间
    int64_t         logoutTime;      //离开场景时间
    std::wstring    comment;         //扩展字段:可不填
};




//宠物、坐骑log
struct PetRideLog
{
	PetRideLog()
		:type(0),
		targetID(""),
		levelBefore(0),
		levelAfter(0),
		skillID(""),
		comment(L"")
	{}
	int             type;            //操作类型 [ LogDefine: LOG_PET_RIDE_TYPE]  
	std::string     targetID;        //宠物、坐骑ID
	int             levelBefore;     //升级前等级
	int             levelAfter;      //升级后等级
	std::string     skillID;         //技能ID
    std::wstring    comment;         //扩展字段:可不填
};

// 坐骑基础log
struct RideBaseLog 
{
	RideBaseLog()
		:opt_type(0), 
		config_id(""),
		cur_step(0),
		cur_star(0)
		{
		}

	int opt_type;
	std::string config_id;
	int cur_step;
	int cur_star;
};

// 坐骑饲养log
struct RideFeedLog : public RideBaseLog
{
	RideFeedLog()
		: item_id(""),
		item_num(0),
		add_feed_value(0)
		{
		}

	std::string item_id;
	int item_num;
	int add_feed_value;
};

// 坐骑升级log
struct RideUpgradeLog : public RideBaseLog
{
	RideUpgradeLog()
	: last_step(0), 
	last_star(0)
	{
	}

	int last_step;
	int last_star;
};

// 坐骑技能升级log
struct RideSkillUpgradeLog : public RideBaseLog
{
	RideSkillUpgradeLog()
	:
	old_level(0),
	last_level(0)
	, capital_type(0)
	, capital_num(0)
	, item_num(0)
	{
	}

	std::string skill_id;
	int old_level;
	int last_level;
	int capital_type;
	int capital_num;
	std::string item_id;
	int item_num;
};

// 坐骑皮肤log
struct RideSkinLog : public RideBaseLog
{
	RideSkinLog()
	: skin_id("")
	{
	}

	std::string skin_id;
};


// 下属激活log
struct PetActiveLog : RideBaseLog
{
	PetActiveLog()
	: cur_level(0), 
	color(0), 
	item_id(""), 
	item_num(0)
	{
	}

	int cur_level;
	int color;
	std::string item_id;
	int item_num;
};

// 下属升级log
struct PetUpgradeLog : RideBaseLog
{
	PetUpgradeLog()
	: cur_level(0),
	last_level(0),
	last_star(0),
	last_step(0), 
	capital_type(0), 
	capital_num(0), 
	item_id(""), 
	item_num(0)
	{
	}

	int cur_level;
	int last_level;
	int last_star;
	int last_step;
	int capital_type;
	int capital_num;
	std::string item_id;
	int item_num;
};

// 下属技能升级log
struct PetSkillUpgradeLog : RideBaseLog
{
	PetSkillUpgradeLog()
	: pet_level(0),
	cur_skill_lvl(0),
	last_skill_lvl(0),
	capital_type(0),
	capital_num(0),
	skill_point(0)
	{
	}

	std::string skill_id;
	int pet_level;
	int cur_skill_lvl;
	int last_skill_lvl;
	int capital_type;
	int capital_num;
	int skill_point;
};

// 下属觉醒日志
struct PetAwakenLog
{
	int opt_type;
	std::string pet_id;
	std::string skills;

	PetAwakenLog() :
		opt_type(0)
	{
	}
};

// 押囚开始log
struct EscortStartLog 
{
	EscortStartLog()
	: opt_type(0), 
	escort_id(0),
	escort_work(0),
	start_time(0), 
	npc_id(""),
	color(0),
	capital_type(0),
	capital_num(0),
	item_id(""),
	item_num(0)
	{
	}

	int opt_type;
	int escort_id;
	int escort_work;
	int64_t start_time;
	std::string npc_id;
	int color;
	int capital_type;
	int capital_num;
	std::string item_id;
	int item_num;
};


// 押囚结束log
struct EscortEndLog
{
	EscortEndLog()
	: opt_type(0),	
	escort_id(0),
	escort_work(0), 
	start_time(0),
	color(0), 
	end_time(0), 
	result(0)
	{
	}

	int opt_type;
	int escort_work;
	int escort_id;
	int64_t start_time;
	std::string npc_id;
	int color;
	int64_t end_time;
	int result;

};

// 组队悬赏log
struct TeamOfferLog
{
	TeamOfferLog()
	: opt_type(0),
	offer_num(0),
	offer_level(0),
	pass_time(0),
	offer_result(0)
	{
	}

	int opt_type;
	int offer_num;
	int pass_time;
	int offer_level;
	int offer_result;

};

//PVP玩法log
struct PVPLog
{
	PVPLog()
		:beChallenge(L""),
		beforeRank(0),
		afterRank(0),
        startTime(0),
		endTime(0),
		result(0),
		comment(L"")
	{}
	std::wstring    beChallenge;     //被挑战人昵称
	int             beforeRank;      //挑战前排名级
	int             afterRank;       //挑战后排名
	int64_t         startTime;       //开始时间
	int64_t         endTime;         //结束时间
	int             result;          //挑战结果
    std::wstring    comment;         //扩展字段:可不填
};

//PVP玩法log
struct RevengeArenaLog
{
	RevengeArenaLog()
		:wsEnemyName(L""),
		startTime(0),
		endTime(0),
		result(0)
	{}
	std::wstring    wsEnemyName;     //被挑战人昵称
	int64_t         startTime;       //开始时间
	int64_t         endTime;         //结束时间
	int             result;          //挑战结果
};


//国家系统log
struct NationLog
{
	NationLog()
		:n_operate_type(0),
		params(""),
		comment(L"")
	{}
	int             n_operate_type;         //操作类型:[ LogDefine: LOG_NATION_TYPE]
	std::string     params;                 //根据类型对应数据:官员职位,获得投票数(没有则不填)
    std::wstring    comment;                //扩展字段:可不填
	std::string     account;                //账户
	std::wstring    name;                   //昵称
};


//密境log
struct SceretLog
{
	SceretLog()
		:sceretID(0),
		startTime(0),
		endTime(0),
		deadCount(0),
		result(0)
	{}
	int            sceretID;                //密境场景ID
	int64_t        startTime;               //开始时间
	int64_t        endTime;                 //结束时间
	int            deadCount;               //死亡次数
	int            result;                  //结果: [ LogDefine: LOG_SCERET_RESULT_TYPE]
    std::wstring   comment;                 //扩展字段:可不填
};


//称号log
struct TitleLog
{
	TitleLog()
		:titleID(0),
		operateType(0),
		comment(L"")
	{}
	int            titleID;                 //称号ID
	int            operateType;             //类型: [ LogDefine: LOG_ACT_EVENT_TYPE]
	std::wstring   comment;                 //扩展字段:可不填
};

//抽奖log
struct ExtractAwardLog
{
	ExtractAwardLog()
		:extractType(0),
		params(""),
		awardLevel(0),
		comment(L"")
	{}
	int            extractType;             //类型: [ LogDefine: LOG_EXTRACT_AWARD_TYPE]
	std::string    params;                  //详细:根据类型填写.列bossID等(没有可不填)
	int            awardLevel;              //奖励等级
	std::wstring   comment;                 //扩展字段:可不填
};

//订单log
struct TraceOrderLog
{
	TraceOrderLog()
		:customType(0),
		account(""),
		name(L""),
		strMoney(""),
		traceOrder(""),
		comment(L"")
	{}
	int            customType;              //自定义类型: [ LogDefine: LOG_CUSTOM_TYPE]    
	std::string	   account;					// 账号
	std::wstring   name;					// 昵称
	std::string    buyIndex;                //购买物品索引
	std::string    strMoney;                //支付钱币
	std::string    traceOrder;              //详细:根据类型填写.列bossID等
	std::wstring   comment;                 //扩展字段:可不填
};

//抽奖log
struct MailLog
{
	MailLog()
		:account(""),
		name(L""),
		templateType(""),
		capitalParams(""),
		itemParams(""),
		comment(L"")
	{}
	std::string	   account;					// 账号
	std::wstring   name;					// 昵称
	std::string    templateType;            //邮件模版类型
	std::string    capitalParams;           //货币参数
	std::string    itemParams;              //物品参数
	std::wstring   comment;                 //扩展字段:可不填
};

//装备日志结构
struct EquipLog
{
	EquipLog()
	:operateType(0),
	equipPos(0),
	equipSlot(0),
	beforeGrade(0),
	afterGrade(0),
	sourceItemID(""),
	targetItemID(""),
	deductItemIDs(""),
	deductItemNums(""),
	capitalType(0),
	capitalNum(0),
	comment(L"")
	{}
	int             operateType;     //操作类型  (int)
	int             equipPos;        //装备栏位  (int)
	int             equipSlot;       //栏位孔位  (int)
	int             beforeGrade;     //强化后等级  (int)
	int             afterGrade;      //强化后等级  (int)
	std::string     sourceItemID;    //处理前物品ID (varchar)
	std::string     targetItemID;    //处理后物品ID (varchar)
	std::string     deductItemIDs;   //消耗物品 (varchar)    (多个逗号分割)
	std::string     deductItemNums;  //消耗物品数量 (varchar)  (与物品一一对应)
	int             capitalType;     //货币类型  (int)
	int             capitalNum;      //消耗货币数 (int)
	std::wstring    comment;         //扩展字段:可不填
};

// 时装日志结构
struct FashionLog
{
	FashionLog()
	:operateType(0),
	fashion_id(""),
	unlock_items(""),
	unlock_capitals(""),
	comment(L"")
	{
	}

	int	operateType;     // 操作类型  (int)
	std::string	fashion_id;    // 时装id (varchar)
	std::string	unlock_items;	// 解锁物品
	std::string	unlock_capitals;  // 解锁费用
	std::wstring comment;         // 扩展字段:可不填
};

// 日志基本数据
struct LogBaseInfo
{
	std::string		strAccount;			// 账号
	std::string		strUid;				// uid
	int				nLevel;				// 等级
	int				nVipLv;				// Vip等级
	//int				nJob;				// 职业
};

// 经脉日志结构
struct MeridianLog
{
	MeridianLog()
	:operateType(0),
	stoneid(""),
	beforeGrade(0),
	afterGrade(0),
	beforeExp(0),
	afterExp(0),
	deductItemIDs(""),
	deductItemNums(""),
	capitalType(0),
	capitalNum(0)
	{}
	int             operateType;     //操作类型  (int)
	std::string     stoneid;         // 砭石id  (string)
	int             beforeGrade;     //强化前等级  (int)
	int             afterGrade;      //强化后等级  (int)
	int             beforeExp;		 //强化前经验  (int)
	int             afterExp;        //强化后经验  (int)
	std::string     deductItemIDs;   //消耗物品 (varchar)    (多个逗号分割)
	std::string     deductItemNums;  //消耗物品数量 (varchar)  (与物品一一对应)
	int             capitalType;     //货币类型  (int)
	int             capitalNum;      //消耗货币数 (int)
	std::string		paklist;		 // 属性包列表 (varchar) (id1,id2,id3)
};

#ifdef ANALY_BY_USER
class SensorsAnalyticsCollector;
typedef SensorsAnalyticsCollector GameAnalytics;
#else
class SensorsAnalyticsCollectorEx;
typedef SensorsAnalyticsCollectorEx GameAnalytics;
#endif
class LogModule : public ILogicModule
{
public:
    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

	void SetSrvInfo(IKernel* pKernel, const SrvInfo& info);
public:
	// 设置客户端终端信息
	void OnClientInfo( IKernel* pKernel, const PERSISTID& player, const char* ip, 
							const char* platform, 
							const char* clientver,
							const char* manufacturer,
							const char* deviceid,
							const char* model,
							const char* os,
							const char* osver,
							bool bwifi
							);
#ifdef ANALY_BY_USER
	// 注册用户
	void OnNewUser(
							const char* uid,		// 用户唯一id (require)
							const char* anid,		// 用户原匿名id(option)
							const char* nick,		// 昵称/用户名(require)
							int sex,				// 性别(option)
							__int64 bron,			// 出生日期(option)
							const char* ip,			// 终端id(option)
							const char* platform,	// 注册平台(option)
							bool wifi,				// 是否使用wifi(option)
							const char* manufacturer,	// 制造商(option)
							const char* devid,		// 终端设备唯一标识(option)
							const char* model,		// 终端类型(option)
							const char* os,			// 终端系统(option)
							const char* os_ver		// 终端系统版本(option)
							);
	// 删除账号
	void OnDelUser(const char* uid);
#endif
	// 登陆成功
	void OnUserLogin(const char* uid,			// 用户唯一id (require)
							const char* nick,		// 昵称/用户名(require)
							const char* ip,			// 终端ip(option)
							const char* platform,	// 平台(option)
							const char* manufacturer,	// 制造商(option)
							const char* devid,		// 终端设备唯一标识(option)
							const char* model,		// 终端类型(option)
							const char* os,			// 终端系统(option)
							const char* os_ver,		// 终端系统版本(option)
							bool bReset = false) ;	// 重置登陆事件
	// 退出登陆
	void OnUserLogout(const char* uid, int64_t timestamp = 0, bool bReset = false);
	// 创建角色
	void OnCreateRole(IKernel* pKernel, const PERSISTID& player);
	// 角色进入
	void OnRoleEntry(IKernel* pKernel, const PERSISTID& player, bool bReset = false);
	// 角色退出 bReset: 是否是重置退出
	void OnRoleExit(IKernel* pKernel, const PERSISTID& player, bool bReset = false);

	//角色日志登录日志
	bool SaveRoleLog(IKernel* pKernel, const PERSISTID& player, const RoleLog &log);

	//游戏玩法异动日志
	bool SaveGameActionLog(IKernel* pKernel, const PERSISTID& player, const GamePlayerActionLog &log);

    //任务日志
    bool SaveTaskLog(IKernel* pKernel, const PERSISTID& player, const TaskLog &log);

	//货币异动日志
	bool SaveMoneyLog(IKernel* pKernel, const PERSISTID& player, const MoneyLog &log);

	//物品异动日志
	bool SaveItemLog(IKernel* pKernel, const PERSISTID& player, const ItemLog &log);
    
	//玩家升级日志
	bool SaveRoleUpgradeLog(IKernel* pKernel, const PERSISTID& player, const RoleUpgradeLog &log);

	//帮会日志
	//bool SaveGuildLog(IKernel* pKernel, const PERSISTID& player, const GuildLog &log);

	//击杀BOSS日志
	bool SaveBossLog(IKernel* pKernel, const PERSISTID& player, const BossLog &log);

	//场景日志
	bool SaveSceneLog(IKernel* pKernel, const PERSISTID& player, const SceneLog &log);

    // 记录玩家日志
    bool SavePlayerLog(IKernel* pKernel, const PERSISTID& player, const int logtype, const wchar_t* comment);

    // 记录GM日志
    bool SaveGMLog(IKernel* pKernel, const PERSISTID& player,const wchar_t* GMContent, const wchar_t* comment = L"Use GM Command");

    // 邮件奖励日志
	bool SaveMailAwardLog(IKernel* pKernel, const MailLog &log);
private:
	//场景创建
	static int OnCreate(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 记录在线玩家数
	static int H_RecOnlineCount(IKernel* pKernel, const PERSISTID& creator, int slice);

public:
	// 充值
	void OnPaying(IKernel* pKernel, const PERSISTID& player, const char* orderid, const char* itemid, int count, float price, bool first);
	// 充值完成
	void OnPayed(IKernel* pKernel, const PERSISTID& player, const char* orderid,
		float amount, int result, bool first, bool bIsReal,
		const char* platform,
		const char* os);
	// 充值完成
	void OnPayed( const char* roleid, const char* uid, int level, int vip,
		const char* orderid,
		float amount, float totalamount, float realamount, int paytimes, int result, bool first, bool bIsReal,
		const char* platform,
		const char* os);
	// 增加经验
	void OnIncExp(IKernel* pKernel, const PERSISTID& player,
									int value,					// 增加值(require)
									int newValue,				// 增加后的值
									int origValue,				// 原始值
									int source					// 来源
									);
	// 升级
	void OnLevelup(IKernel* pKernel, const PERSISTID& player, int level, int original);

	// 获得游戏币
	void OnGainCoin(IKernel* pKernel, const PERSISTID& player,
		int coid,				// 游戏币id(金/银/铜/声望....)(require), 游戏中定义
		int64_t value,			// 增加值(require)
		int64_t newvalue,		// 当前新值(require)
		int64_t original,		// 原始值(require)
		int module,				// 功能模块(option), 游戏中定义(在什么模块)
		int func,				// 功能事件(option), 游戏中定义(做什么事获得)
		const char* extra = NULL// 额外说明(option), 对event进一步说明
		);
	// 消耗游戏币
	void OnCostCoin(IKernel* pKernel, const PERSISTID& player,
		int coid,				// 游戏币id(金/银/铜/声望....)(require), 游戏中定义
		int64_t value,			// 消耗值(require)
		int64_t newvalue,		// 当前新值
		int64_t original,		// 原始值(require)
		int module,				// 功能模块(option), 游戏中定义(在什么模块)
		int func,				// 功能事件(option), 游戏中定义(做什么事获得)
		const char* extra = NULL// 额外说明(option), 对event进一步说明
		);

	// 获得物品
	void OnGainItem(IKernel* pKernel, const PERSISTID& player,
		const char* itemid,		// 物品id(require)
		int color,				// 物品品质
		int num,				// 获得数值(require)
		int count,				// 当前拥有数
		int orig,				// 原来拥有数
		int module,				// 功能模块(option), 游戏中定义(在什么模块)
		int func,				// 功能事件(option), 游戏中定义(做什么事获得)
		const char* extra = NULL// 额外说明(option), 对event进一步说明
		);
	// 使用物品
	void OnCostItem(IKernel* pKernel, const PERSISTID& player,
		const char* itemid,		// 物品id(require)
		int color,				// 物品品质
		int num,				// 获得数值(require)
		int count,				// 当前拥有数
		int orig,				// 原来拥有数
		int module,				// 功能模块(option), 游戏中定义(在什么模块)
		int func,				// 功能事件(option), 游戏中定义(做什么事获得)
		const char* extra = NULL// 额外说明(option), 对event进一步说明
		);

	void OnSkipUpStartScene(IKernel* pKernel, const PERSISTID& player);
	// 访问某个功能(比如说功能开启)-----------------------------------------------------
	void OnVisitFunction(IKernel* pKernel, const PERSISTID& player,
			int module,				// 功能模块(require), 游戏中定义(在什么模块)
			int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
			bool result,			// 结果(require) true:成功 false: 失败
			int value,				// 额外说明(option), 对func进一步说明
			const char* extra		// 额外说明(option), 对func进一步说明
		);

	//----------------------------------------------------------------------
	// 好友交互
	//----------------------------------------------------------------------
	// 好友操作事件(添加，解除，...)
	void OnFriendEvent(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int opType);
	// 好友送花
	void OnFriendSendGift(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nGift, int nIntLevel, int nIncIntimacy);
	// 好友收花
	void OnFriendRecvGift(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nGift, int nIntLevel, int nIncIntimacy);
	// 好友组队增加亲密度
	void OnFriendTeamFight(IKernel* pKernel, const PERSISTID& player, const wchar_t* pFriendName, int nIntLevel, int nIncIntimacy);
	// 魅力值发生变化
	void OnFriendCharmChanged(IKernel* pKernel, const PERSISTID& player, int nCharm, int nTotalCharm, int nReason);
	// 结义
	void OnSworn(IKernel*pKernel, const PERSISTID& player, const wchar_t* playerName, int isSameSex, int isAdd);

	//------------------------------------------------------------------------------
	// 聊天监控
	//------------------------------------------------------------------------------
	//void OnTextChat(IKernel* pKernel, const PERSISTID& player, int nType, const wchar_t* pTarget, const wchar_t* pContent);

	// 交易
	//void OnTransaction(IKernel* pKernel, const PERSISTID& player, const wchar_t* pszTarget, const char* pszItem, int nColor, int nNum, int nPrice, int nTax, int nType);

	// 禁言/封号(时间单位:s)
	void OnForbidSpeak(IKernel* pKernel, const PERSISTID& player, int64_t nInterval, int64_t nExpiredTime);
	void OnAllowSpeak(IKernel* pKernel, const PERSISTID& player);
	void OnBlockPlayer(IKernel* pKernel, const PERSISTID& player, int64_t nInterval, int64_t nExpiredTime);
	void OnBlockPlayer(IKernel* pKernel, const char* pszRoleId, int64_t nInterval, int64_t nExpiredTime);
	void OnResumePlayer(IKernel* pKernel, const PERSISTID& player);
	void OnResumePlayer(IKernel* pKernel, const char* pszRoleId);

	void FillCommonArgs(IKernel*pKernel, const PERSISTID&player, KVArgs& args);		
private:
	// 获取玩家的基本数据
	bool GetPlayerBaseInfo(IKernel *pKernel, const PERSISTID& player, LogBaseInfo& outBaseInfo);
public:
	static LogModule* m_pLogModule;
	
	GameAnalytics* GetGameAnaCollector()
	{
		return m_pAnaCollector;
	}
private:
    // 获取标准格式的日志时间
    std::string GetLogTime(time_t slice = 0);

	GameAnalytics* m_pAnaCollector;

//#ifndef FSROOMLOGIC_EXPORTS
	log4cplus::Logger m_pChatLogger;
//#endif

	static time_t ms_nNextReportCount;	// 下次报数时间
};


#endif //__LOGMODULE_H__