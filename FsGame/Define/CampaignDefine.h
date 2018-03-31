#include "..\..\public\Inlines.h"
//------------------------------------------------------------------------------
// 文件名:      CampaignDefine.h
// 内  容:		活动
// 说  明:
// 创建日期:    2017年2月13日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __CAMPAIGN_DEFINE_H__
#define __CAMPAIGN_DEFINE_H__

// 活动主表
#define PUB_CAMPAIGN_MAIN_REC "pub_campaign_main_rec"		// 公共区活动状态主表

enum
{
	CMR_PUB_COL_ACTIVITY_ID,					// 活动ID			 VTYPE_INT
	CMR_PUB_COL_ACTIVITY_TYPE,					// 活动类型			 VTYPE_INT
	CMR_PUB_COL_STATE,							// 活动状态			 VTYPE_INT
	CMR_PUB_COL_END_TIME,						// 活动状态结束时间	 VTYPE_INT64

	MAX_CMR_PUB_COL_NUM,
	MAX_CMR_PUB_ROW_NUM = 10
};

// 活动状态
enum CampaignState
{
	CAMPAIGN_CLOSE_STATE = -1,							// 活动关闭状态
	CAMPAIGN_BEGIN_STATE,								// 活动开始的第一个状态
};

// 活动类型
enum CampaignType
{
	CT_NONE_TYPE = -1,
	CT_ASURA_BATTLE,				// 修罗战场

	MAX_CAMPAIGN_TYPE_NUM
};

// 活动内部子消息消息
enum CampaignComand
{
	CAMPAIGN_STATE_CHANGE_MSG,		// 活动状态改变 int gametype, int state
	CAMPAIGN_BOSS_INVADE_SEND_REWARD, // boss入侵奖励发放
};

// 自定义阵营
enum CampType
{
	CAMP_NONE,
	CAMP_RED,			// 红方
	CAMP_BLUE,			// 蓝方

	CAMP_TYPE_TOTAL
};

// 获取阵营文言id
inline const char* GetCampTxtId(int nCampId)
{
	static char strTxtId[64] = { 0 };
	SPRINTF_S(strTxtId, "%s%d", "Camp_", nCampId);

	return strTxtId;
}
/************************************************************************/
/* 修罗战场活动的相关定义                                               */
/************************************************************************/
// 修罗战场活动状态
enum AsuraBattleState
{
	ASURA_BATTLE_CLOSE = -1,	// 活动关闭
	ASURA_BATTLE_READY,			// 活动准备状态
	ASURA_BATTLE_START,			// 活动开始
};

// 客户端->服务端 子消息
enum
{
	CS_ASURA_BATTLE_ENTER,					// 进入修罗战场
	CS_ASURA_BATTLE_COLLECTION,				// 采集战资	
	CS_ASURA_BATTLE_BUY_BUFF,				// 购买buff	 string buffid
	CS_ASURA_BATTLE_EXIT,					// 退出修罗战场		
	CS_ASURA_BATTLE_SUBMIT,					// 提交战资
	CS_ASURA_BATTLE_QUERY_RANK_LIST,		// 查询战场排行榜
};

// 服务端->客户端 子消息
enum
{
	/*!
	* @brief	阵营战斗数据
	* @param	int			排名
	以下循环2次	
	* @param	int			阵营类型
	* @param	int			总积分
	* @param	int			备战状态
	* @param	int			采集进度
	* @param	float x,z	积分第一玩家的位置
	*/
	SC_ASURA_BATTLE_FIGHT_INFO,			

	/*!
	* @brief	购买buff成功
	*/
	SC_ASURA_BATTLE_BUY_BUFF_SUC,

	/*!
	* @brief	活动结算
	* @param	int			江湖积分
	* @param	int			朝廷积分
	* @param	int			自己的名次
	* @param	int			自己的积分
	* @param	string		名次奖励
	* @param	string		胜负奖励
	* @param	int			前n名
	以下循环n次
	* @param	int			名次
	* @param	wstring		玩家名
	* @param	int			积分
	*/
	SC_ASURA_BATTLE_SETTLE_INFO,

	/*!
	* @brief	提示购买buff
	*/
	SC_ASURA_BATTLE_BUY_BUFF_TIP,

	/*!
	* @brief	修罗战场开始
	*/
	SC_ASURA_BATTLE_START,

	/*!
	* @brief	战场实时排行榜
	* @param	int			自己的名次

	* @param	int			江湖总杀人数
	* @param	int			前n名
	以下循环n次
	* @param	int			名次
	* @param	wstring		玩家名
	* @param	int			积分
	* @param	int			总杀人数
	* @param	int			总死亡数
	* @param	int			总助攻数

	* @param	int			朝廷总杀人数
	* @param	int			前n名
	以下循环n次
	* @param	int			名次
	* @param	wstring		玩家名
	* @param	int			积分
	* @param	int			总杀人数
	* @param	int			总死亡数
	* @param	int			总助攻数
	*/
	SC_ASURA_BATTLE_FIGHTINT_RANK_INFO,

	/*!
	* @brief	boss战斗数据
	* @param	int			江湖总伤害
	* @param	int			朝廷总伤害
	* @param	float		boss血量百分比
	*/
	SC_ASURA_BATTLE_FIGHT_BOSS_DATA,

	/*!
	* @brief	交付物资成功
	*/
	SC_ASURA_BATTLE_SUBMIT_MATERIAL,
};

// 阵营采集阶段
enum CAMPCollectState
{
	CAMP_COLLECT_START,			// 筹备阶段
	CAMP_COLLECT_READY,			// 备战阶段
	CAMP_COLLECT_FIGHT,			// 全副武装阶段
};

// 玩家积分表
enum 
{
	 CAMP_PLAYER_NONE = -1,
	 CAMP_PLAYER_COL_NAME,		// 玩家名
	 CAMP_PLAYER_COL_SCORE,		// 积分
	 CAMP_PLAYER_COL_KILL,		// 杀人数
	 CAMP_PLAYER_COL_DEAD,		// 死亡数
	 CAMP_PLAYER_COL_ASSIST,	// 助攻数
};

#define SYS_INFO_ASURA_BATTLE_READY_NOTICE			"sys_info_asura_battle_ready_notice"	// 修罗战场即将开始
#define SYS_INFO_ASURA_BATTLE_START_NOTICE			"sys_info_asura_battle_start_notice"    // 修罗战场开始

#define SYS_INFO_ASURA_BATTLE_ENTER_FAILED_LEVEL "sys_info_asura_battle_enter_failed1"	// 玩家等级不足
#define SYS_INFO_ASURA_BATTLE_ENTER_FAILED_CLOSE "sys_info_asura_battle_enter_failed2"	// 修罗战场未开启

#define SYS_INFO_ASURA_BATTLE_COLLECT_FAILED_NOT_EXIST "sys_info_asura_battle_collect_failed1"	// 战资不存在了

#define SYS_INFO_ASURA_BATTLE_KILL_PLAYER_NOTICE "sys_info_asura_battle_kill_player_notice"	// {0}的{1}杀死了 {2}的{3}
#define SYS_INFO_ASURA_BATTLE_BUY_BUFF_FAILED_HAD "sys_info_asura_battle_buy_buff_failed1"	// 已经有buff了 
#define SYS_INFO_ASURA_BATTLE_BUY_BUFF_FAILED_CAPITAL "sys_info_asura_battle_buy_buff_failed2"	// 钱不够

#define SYS_INFO_ASURA_BATTLE_TIME_LEFT_TIP "sys_info_asura_battle_time_left_tip"	// 剩余结束时间提示
#define SYS_INFO_ASURA_BATTLE_FIRST_TIP "sys_info_asura_battle_first_tip"		// {0}的{1}和{2}的{3}排名第一,快去击杀
#define SYS_INFO_ASURA_BATTLE_DRAW_RESULT "sys_info_asura_battle_draw_result"		// 平局结果
#define SYS_INFO_ASURA_BATTLE_WIN_RESULT "sys_info_asura_battle_win_result"			// {0}胜利了
#define SYS_INFO_ASURA_BATTLE_BOSS_BORN "sys_info_asura_battle_boss_born"			// Boss出生公告
#define SYS_INFO_ASURA_BATTLE_BOSS_TIP "sys_info_asura_battle_boss_tip"				// Boss提示公告
#define SYS_INFO_ASURA_BATTLE_BOSS_DEAD "sys_info_asura_battle_boss_dead"			// Boss死亡公告 boss被杀,{0}阵营胜利,全员增加了{1}分
#define SYS_INFO_ASURA_BATTLE_COLLECT_STATE_CHANGE "sys_info_asura_battle_collect_state_change"			// {0}阵营达到了{1}采集状态
#define SYS_INFO_ASURA_BATTLE_ADD_SCORE_KILL "sys_info_asura_battle_add_score_kill"			// 击杀{0}获得积分{1}
#define SYS_INFO_ASURA_BATTLE_ADD_SCORE_ASSIST "sys_info_asura_battle_add_score_assist"		// 击杀{0},参与助攻获得积分{1}


#endif // __CAMPAIGN_DEFINE_H__
