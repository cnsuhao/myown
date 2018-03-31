//---------------------------------------------------------
//文件名:       ArenaModuleDefine.h
//内  容:       竞技场的一些定义
//说  明:       
//          
//创建日期:      2015年06月25日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef _FSGAME_ARENA_MODULE_DEFINE_H_
#define _FSGAME_ARENA_MODULE_DEFINE_H_

// 竞技场挑战次数的数组大小
#define ARENA_CHALLENGE_CHANCE_MAX_SIZE     100

// 竞技场客户端消息
enum ArenaC2SMsg
{
	ARENA_C2S_QUERY_SELF_INFO	= 1,				// 查询自己的竞技场数据
	ARENA_C2S_GET_CANDIDATE,						// 获取候选人(用于挑战)
	ARENA_C2S_CHALLENGE_SOMEONE,					// 请求挑战某人 wstring 挑战玩家的名字
	ARENA_C2S_GIVE_UP,								// 放弃挑战
	ARENA_C2S_CHALLENGE_CLEAR_CD,					// 重置竞技场挑战CD
	ARENA_C2S_REQUEST_FIGHT_RESULT,					// 请求战报数据
};

// 竞技场服务器消息
enum ArenaS2CMsg
{
	/*!
	* @brief	回复自己的竞技场数据
	* @param	int 竞技场今日挑战次数
	* @param	int 竞技场总挑战次数
	* @param	int 竞技场排名
	* @param	int 战斗力
	* @param	int 战魂
	* @param	string 所在排行榜
	*/
	ARENA_S2C_REV_SELF_INFO		= 1,

	/*!
	* @brief	回复挑战候选人数据
	* @param	player_uid,player_account,player_name,player_career,player_sex,player_nation,player_level,player_online_type,player_online,player_vip_level
	*/
	ARENA_S2C_REV_CANDIDATE_INFO,

	ARENA_S2C_COUNTDOWN_BEFORE_FIGHT,				// 开打前的倒计时开始 int 倒计时(单位:S)
	ARENA_S2C_FIGHT,								// 开打 int 一局的时间(单位:S)

	/*!
	* @brief	挑战结束 
	* @param	int 结果 0 胜 1 败
	* @param	string 货币奖励
	* @param	string 物品奖励
	* @param	int 原来的排名
	* @param	int 新排名
	* @param	int 历史最好排名
	* @param	int 突破历史最好排名的奖励
	*/
	ARENA_S2C_CHALLENGE_END,		

	/*!
	* @brief	购买挑战次数成功
	* @param	int 当前挑战次数
	*/
	ARENA_S2C_BUY_CHANCE_SUCCESS,	

	/*!
	* @brief	回复战报数据
	* @param	int 当前战报数n
	以下循环n次
	* @param	int 战果 0胜 1败
	* @param	wstring  敌方名字
	* @param	int  战前排名
	* @param	int  战后排名
	* @param	int64  战斗时间
	*/
	ARENA_S2C_REV_FIGHT_RESULT_DATA,

	/*!
	* @brief	战报更新提示
	*/
	ARENA_S2C_FIGHT_RESULT_CHANGE_TIP,
};

//与public服务器的通信二级消息
enum ArenaPubSubMsgID
{
    ARENA_PUB_SUB_MSG_NULL                   = 1, // 
    ARENA_PUB_SUB_MSG_GIVE_RANK_REWARD       = 2, // 通知发送排名奖励
    ARENA_PUB_SUB_MSG_CHALLENGE_REQUEST      = 3, // 挑战申请
    ARENA_PUB_SUB_MSG_SWAP_RANK              = 4, // 交换排名
    ARENA_PUB_SUB_MSG_PROMOTE                = 5, // 晋级玩家
    ARENA_PUB_SUB_MSG_GET_CANDIDATE          = 6, // 获取候选人
    ARENA_PUB_SUB_MSG_UNLOCK_PLAYER          = 7, // 解锁挑战双方
    ARENA_PUB_SUB_MSG_ADD_TO_RANKLIST        = 8, // 把玩家加入排行榜
    ARENA_PUB_SUB_MSG_CHALLENGE_END          = 9, // 挑战结束
	ARENA_PUB_SUB_MSG_GM_RESET_ARENA_STATE   = 10,// gm重置一个玩家的竞技场状态

	/*!
	* @brief	增加一条战斗结果数据
	* @param	int 战果 0胜 1败
	* @param	wstring  敌方名字
	* @param	int  战前排名
	* @param	int  战后排名
	* @param	int64  战斗时间
	*/
	ARENA_MSG_ADD_FIGHT_RESULT_DATA			 = 11,
};

// 通知型command消息
enum ArenaPlayerNotify
{
    ARENA_PLAYER_NOTIFY_RANK            = 1, // 排名变化
    ARENA_PLAYER_NOTIFY_CHALLENGE       = 2,  // 在竞技场挑战
    ARENA_PLAYER_NOTIFY_CHALLENGE_WIN   = 3,  // 在竞技场挑战获得胜利
};

// 竞技场排行榜的关键列
enum ArenaRankListCol
{
    ARENA_RANK_COL_UID = 0,
};

// 玩家的竞技场状态
//enum RoleArenaState
//{
//    ARENA_STATE_NOT_ARENA   = 0,   // 不在竞技场
//    ARENA_STATE_IN_ARENA    = 1,   // 正在竞技场
//};

// 竞技场的标识符
enum ArenaFlag
{
    ARENA_FLAG_NOT_ARENA    = 0, // 默认状态不在竞技场
    ARENA_FLAG_ENTERING     = 1, // 正在进入竞技场
    ARENA_FLAG_READY        = 2, // 在竞技场中就绪
    ARENA_FLAG_IN_BATTLE    = 4, // 在竞技场中战斗
    ARENA_FLAG_END          = 5, // 竞技结束结束
    ARENA_FLAG_ESCAPE_RETURN  = 6, // 玩家逃跑又回来
};

enum ArenaChallengeResult
{
    ARENA_RET_CHALLENGE_WIN        = 0,    // 挑战获胜
    ARENA_RET_CHALLENGE_LOSE       = 1,    // 挑战失败
    ARENA_RET_TIME_OUT              = 2,    // 超时未分出胜负
    ARENA_RET_ONE_DIE               = 3,    // 一方死亡
    ARENA_RET_ESCAPE                = 4,    // 挑战者逃跑
};

// 竞技场代码
enum ArenaCode
{
    ARENA_CODE_NULL                     = 0, // 木有意义的代码
    
    ARENA_CODE_SUCCESS                  = 1, // 成功码
    ARENA_CODE_FAILED                   = 2, // 失败码
    
    ARENA_CODE_ERROR                    = 10, // 未知原因的错误
    ARENA_CODE_ONE_IN_CHALLENE          = 11, // 一方在挑战者
    ARENA_CODE_ONE_NOT_FOUND            = 12, // 双方不在同一个榜中
    ARENA_CODE_CHALLENGEE_LOW_RANK      = 13, // 对手名次低于自己
    ARENA_CODE_RANK_LIST_FULL           = 14, // 排行榜满了
    
};

// gm命令
enum
{
	UNLOCK_ARENA_STATE = 0,			// gm命令解锁竞技场状态
	LOCK_ARENA_STATE,				// gm命令锁定竞技场状态
};

// SNS相关定义
#define SNS_ARENA_SPACE				L"sns_arena_space"

#define  SNS_ARENA_FIGHT_RESULT_REC	"arena_fight_result_rec"		// 离线战报数据表
enum 
{
	SNS_ARENA_COL_FIGHT_RESULT,				// 战斗结果
	SNS_ARENA_COL_FIGHT_ENEMY_NAME,			// 敌方名字
	SNS_ARENA_COL_FIGHT_PRE_RANK,			// 战斗前的排名
	SNS_ARENA_COL_FIGHT_CUR_RANK,			// 战斗后的排名
	SNS_ARENA_COL_FIGHT_TIME,				// 战斗时间

	MAX_ARENA_COL_NUM,
	MAX_ARENA_ROW_NUM = 20
};

#endif