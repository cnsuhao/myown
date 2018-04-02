//--------------------------------------------------------------------
// 文件名:		TeamOfferDefine.h
// 内  容:		组队悬赏定义
// 说  明:		
// 创建日期:		2017年01月12日
// 创建人:		tongzt
// 修改人:		 
//--------------------------------------------------------------------
#ifndef _TEAM_OFFER_DEFINE_H_
#define _TEAM_OFFER_DEFINE_H_

#include <vector>
#include <map>

// 常量定义
const char* const TEAM_OFFER_CAPTAIN_BOX_FILE = "ini/TeamOffer/CaptainBox.xml";	// 队长宝箱
const char* const TEAM_OFFER_AWARD_RULE_FILE = "ini/TeamOffer/AwardRule.xml";	// 个人奖励规则

// 结构定义
// 队长宝箱
struct CaptainBox
{
	int offer_num;
	int reward_id;

	CaptainBox(const int num) :
		offer_num(num),
		reward_id(0)
	{
	}

	bool operator == (const CaptainBox& cfg) const
	{
		return cfg.offer_num == offer_num;
	}
};
typedef std::vector<CaptainBox> CaptainBoxVec;

// 个人奖励规则
struct OfferAwardRule 
{
	int lower_num;
	int upper_num;
	int lower_level;
	int upper_level;

	int reward_id;

	OfferAwardRule() :
		lower_num(0),
		upper_num(0), 
		lower_level(0),
		upper_level(0), 
		reward_id(0)
	{
	}
};
typedef std::vector<OfferAwardRule> OfferAwardRuleVec;


// 枚举定义
// 组队悬赏记录表（公共区）
#define TEAMOFFER_REC "teamoffer_rec"
enum TeamOfferRec
{
	TEAMOFFER_REC_COL_TEAM_ID = 0,	// 队伍ID
	TEAMOFFER_REC_COL_TASK_ID,		// 悬赏任务ID
	TEAMOFFER_REC_COL_GROUP_ID,		// 分组ID
	TEAMOFFER_REC_COL_STATE,		// 状态
	TEAMOFFER_REC_COL_CREATOR,		// 刷怪器
	TEAMOFFER_REC_COL_STATE_TIME,	// 状态开始时间
	
	TEAMOFFER_REC_COL_MAX,	
};

// 组队悬赏状态
enum TeamOfferStates
{
	TEAMOFFER_STATE_NONE = 1,	// 未开始
	TEAMOFFER_STATE_DOING = 2,	// 进行
	TEAMOFFER_STATE_END = 3,	// 结束
};

// 结果枚举
enum Results
{
	RESULTS_FAILED = 0,	// 失败
	RESULTS_SUCCESS,		// 成功
};

// 服务器->公共区子消息
enum S2P_TeamOffer_Sub_Msg
{
	S2P_TEAMOFFER_SUB_MSG_NONE = 0,

	/*!
	* @brief	发起一次组队悬赏
	* @param wstring 队长名
	* @param int 队伍ID
	* @param int 悬赏任务ID
	* @param int 分组号
	* @param int 状态
	* @param object 刷怪器
	*/
	S2P_TEAMOFFER_SUB_MSG_LAUNCH,

	/*!
	* @brief	终止一次组队悬赏
	* @param int 队伍ID
	* @param int 悬赏任务ID
	* @param int 分组号
	*/
	S2P_TEAMOFFER_SUB_MSG_STOP,

	/*!
	* @brief	组队悬赏开始通知
	* @param int 队伍ID
	* @param int 悬赏任务ID
	* @param int 分组号
	* @param object 刷怪器
	*/
	S2P_TEAMOFFER_SUB_MSG_START,

	/*!
	* @brief	组队悬赏有效验证
	* @param int 队伍ID
	* @param int 分组ID
	*/
	S2P_TEAMOFFER_SUB_MSG_CHECK,

	/*!
	* @brief	组队悬赏结果
	* @param int 结果（0：失败 1：成功）
	* @param int 队伍ID
	*/
	S2P_TEAMOFFER_SUB_MSG_RESULT,

	/*!
	* @brief	重置一次组队悬赏
	* @param int 队伍ID
	*/
	S2P_TEAMOFFER_SUB_MSG_RESET,
};

// 公共区->服务器子消息
enum P2S_TeamOffer_Sub_Msg
{
	P2S_TEAMOFFER_SUB_MSG_NONE = 0,

	/*!
	* @brief	通知队伍成员同步悬赏任务
	* @param wstring 队长名
	* @param int 队伍ID
	* @param int 悬赏任务ID
	*/
	P2S_TEAMOFFER_SUB_MSG_SYNC_TASK,

	/*!
	* @brief	通知队伍进入分组副本
	* @param int 队伍ID
	* @param 成员名，以下循环n次
	* @param wstring 成员名

	*/
	P2S_TEAMOFFER_SUB_MSG_ENTRY_GROUP,

	/*!
	* @brief	组队悬赏结果
	* @param int 结果（0：失败 1：成功）
	* @param int 队伍ID
	* @param object 刷怪器
	* @param int 持续时间（s）
	* @param 成员名，以下循环n次
	* @param wstring 成员名
	*/
	P2S_TEAMOFFER_SUB_MSG_RESULT,

};

// 服务器->服务器子消息
enum S2S_TeamOffer_Sub_Msg
{
	S2S_TEAMOFFER_SUB_MSG_NONE = 0,

	/*!
	* @brief	通知队长发起同步悬赏任务
	* @param int 队伍id
	* @param int 悬赏任务id
	*/
	S2S_TEAMOFFER_SUB_MSG_SYNC_TASK,

	/*!
	* @brief	进入组队悬赏分组副本
	* @param int 队伍ID
	*/
	S2S_TEAMOFFER_SUB_MSG_ENTRY,

	/*!
	* @brief	组队悬赏结果
	* @param int 结果（0：失败 1：成功）
	* @param int 队伍ID
	* @param object 刷怪器
	* @param int 持续时间（s）
	* @param 成员名，以下循环n次
	* @param wstring 成员名
	*/
	S2S_TEAMOFFER_SUB_MSG_RESULT,

	/*!
	* @brief	通知队员同步悬赏任务
	* @param int 队伍id
	* @param int 悬赏任务id
	*/
	S2S_TEAMOFFER_SUB_MSG_MEMBER_SYNC_TASK,

	/*!
	* @brief	通知队队长继续下一环
	*/
	S2S_TEAMOFFER_SUB_MSG_CONTINUE,

	/*!
	* @brief	通知成员删除旧的悬赏任务
	*/
	S2S_TEAMOFFER_SUB_MSG_DELETE_TASK,

};

// 服务器->客户端子消息
enum S2C_TeamOffer_Sub_Msg
{
	S2C_TEAMOFFER_SUB_MSG_NONE = 0, 

	/*!
	* @brief	组队悬赏结果
	* @param int 结果（0：失败 1：成功）
	* 成功的情况下：
	* @param int 当前次数
	* @param int 总次数
	* @param int 通关时间（ms）
	* @param int 奖励ID
	*/
	S2C_TEAMOFFER_SUB_MSG_RESULT,

	/*!
	* @brief	组队悬赏倒计时
	* @param int 悬赏倒计时(ms)
	*/
	S2C_TEAMOFFER_SUB_MSG_COUNTDOWN,

	/*!
	* @brief	组队悬赏伤害表
	* @param int 数据量n
	* 以下循环n次
	* @param wstring 玩家名
	* @param int 职业
	* @param int 伤害值
	*/
	S2C_TEAMOFFER_SUB_MSG_DAMAGE_INFO,

	/*!
	* @brief	本环悬赏已结束，请重新接取下一环悬赏
	* @param int 已完成次数
	*/
	S2C_TEAMOFFER_SUB_MSG_CONTINUE_NEXT,
};

// 客户端->服务器子消息
enum C2S_TeamOffer_Sub_Msg
{
	C2S_TEAMOFFER_SUB_MSG_NONE = 0,

	/*!
	 * @brief	发起一次组队悬赏
	 */
	C2S_TEAMOFFER_SUB_MSG_LAUNCH,

	 /*!
	 * @brief	进入悬赏分组
	 */
	C2S_TEAMOFFER_SUB_MSG_ENTRY_GROUP,

	 /*!
	 * @brief	退出组队悬赏
	 */
	C2S_TEAMOFFER_SUB_MSG_LEAVE,	

	/*!
	* @brief	请求组队悬赏伤害数据
	*/
	C2S_TEAMOFFER_SUB_MSG_DAMAGE_DATA,
};


// 系统消息定义
#define  STR_TEAMOFFER_SYSINFO_NUM_NOT_ENOUGH "str_teamoffer_sysinfo_num_not_enough"	// 悬赏次数不足
#define  STR_TEAMOFFER_SYSINFO_TOO_FAR "str_teamoffer_sysinfo_too_far"	// 距离悬赏引导Npc太远
#define  STR_TEAMOFFER_SYSINFO_OFFLINE "str_teamoffer_sysinfo_offline"	// 队员{@0:name}离线
#define  STR_TEAMOFFER_SYSINFO_NOT_IN_OFFERSCENE "str_teamoffer_sysinfo_not_in_offerscene"	// 队员{@0:name}不在悬赏场景
#define  STR_TEAMOFFER_SYSINFO_LEVEL "str_teamoffer_sysinfo_level"	// 队员{@0:name}等级不足
#define  STR_TEAMOFFER_SYSINFO_NOT_TEAMCAPTAIN "str_teamoffer_sysinfo_not_teamcap"	// 不是队长，无法发起
#define  STR_TEAMOFFER_SYSINFO_PEOPLE_NOENOUGH "str_teamoffer_sysinfo_people_notenough"	// 队伍人数不足

#define  STR_TEAMOFFER_SYSINFO_001 "str_teamoffer_sysinfo_001"	// 悬赏未开启
#define  STR_TEAMOFFER_SYSINFO_002 "str_teamoffer_sysinfo_002"	// 悬赏任务已存在
#define  STR_TEAMOFFER_SYSINFO_003 "str_teamoffer_sysinfo_003"	// 队员{0:name}在进行其他活动
#define  STR_TEAMOFFER_SYSINFO_004 "str_teamoffer_sysinfo_004"	// 队员{0:name}悬赏次数不足
#define  STR_TEAMOFFER_SYSINFO_005 "str_teamoffer_sysinfo_005"	// 无悬赏任务
#define  STR_TEAMOFFER_SYSINFO_006 "str_teamoffer_sysinfo_006"	// 不在悬赏场景
#define  STR_TEAMOFFER_SYSINFO_007 "str_teamoffer_sysinfo_007"	// 已在副本中
#define  STR_TEAMOFFER_SYSINFO_008 "str_teamoffer_sysinfo_008"	// 组队悬赏未开始
#define  STR_TEAMOFFER_SYSINFO_009 "str_teamoffer_sysinfo_009"	// 组队悬赏结算中
#define  STR_TEAMOFFER_SYSINFO_010 "str_teamoffer_sysinfo_010"	// 您已退出组队悬赏
#define  STR_TEAMOFFER_SYSINFO_011 "str_teamoffer_sysinfo_011"	// 队员{0:name}没有匹配的悬赏任务
#define  STR_TEAMOFFER_SYSINFO_012 "str_teamoffer_sysinfo_012"	// 队员{0:name}在副本中
#define  STR_TEAMOFFER_SYSINFO_013 "str_teamoffer_sysinfo_013"	// 悬赏已结束
#define  STR_TEAMOFFER_SYSINFO_014 "str_teamoffer_sysinfo_014"	// 组队悬赏进行中

#endif	// _TEAM_OFFER_DEFINE_H_