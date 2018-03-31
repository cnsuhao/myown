//---------------------------------------------------------
//文件名:       ArenaModule.h
//内  容:       竞技场
//说  明:       
//          
//创建日期:      2015年06月25日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef _FSGAME_ARENAMODULE_H_
#define _FSGAME_ARENAMODULE_H_

#include "Fsgame/Define/header.h"

#include <string>
#include <vector>
#include <map>

#include "FsGame/Define/ArenaModuleDefine.h"

class RankListModule;
class CapitalModule;
class LevelModule;
class CopyOffLineModule;
class LandPosModule;
class RewardModule;
class VipModule;
class DynamicActModule;
class LogModule;

class ArenaModule : public ILogicModule
{    
public:

    enum NotifyStrategy
    {
        DO_NOT_NOTIFY = 0, // 不要发送通知
    };
    
	// 初始化
	virtual bool Init(IKernel* pKernel);
	
	// 释放
	virtual bool Shut(IKernel* pKernel);
	
	// 加载资源
	virtual bool LoadResource(IKernel* pKernel);
	
	// 竞技场配置
	bool LoadArenaConfig(IKernel* pKernel, const char *file_name);
	
    // 候选人规则
    bool LoadCandidateConfig(IKernel* pKernel, const char *file_name);
    
    // 挑战奖励设置
    bool LoadChallengeRewardConfig(IKernel* pKernel, const char *file_name);
    
    // 排名奖励设置
    bool LoadRankRewardConfig(IKernel* pKernel, const char *file_name); 
    
    // 载入排行榜设置
    bool LoadRankListConfig(IKernel* pKernel, const char *file_name);
    
    // 载入晋级奖励配置
    bool LoadPromotionReward(IKernel* pKernel, const char *file_name);
    
    // 名次提升奖励
    bool LoadRankUpReward(IKernel* pKernel, const char *file_name);
    
    // 获取domain的名字
    static const std::wstring& GetDomainName(IKernel * pKernel);
    
    // 获取玩家的名次, 结果让人 result 中, 成功返回true
    //  result 的格式： [名次][等级段下限][等级段上限] 
    //      例如： [7][30][39] 表示玩家第7名，等级段在 [30, 39] 之间
    static bool GetArenaRank(IKernel* pKernel, const PERSISTID& player, CVarList &result);

    // GM 的重置命令, 仅仅用于GM
    static void GMReset(IKernel* pKernel, const PERSISTID& player);
    
private:

    // 角色坐标
    struct Coordinate
    {
		 Coordinate() : x(0.0f), z(0.0f), o(0.0f){}
        len_t x;
        len_t z;
        len_t o;
    };

    // 竞技场的配置
    struct ArenaConfig
    {
		 ArenaConfig() : open_level(0), scene_id(0), buy_chance_limit(0), chosen_count(0),
			 free_chance(0), count_down(0), round_time(0), wait_before_close(0), 
			 challenge_cd(0), challenge_clear_cd_fee(0), nBroadRank(0)
		 {
			 memset(chance_fee, 0, sizeof(int)*ARENA_CHALLENGE_CHANCE_MAX_SIZE);
		 }
        int open_level;                 // 开启等级
        int scene_id;                   // 竞技场场景ID
        Coordinate challenger_born;     // 挑战者出生点
        Coordinate challengee_born;     // 被挑战者出生点
        int buy_chance_limit;           // 每日可购买挑战次数限制
        int chosen_count;               // 挑战的候选人数
        std::string npc_config;         // 镜像NPC的config
        int free_chance;                // 每日免费挑战次数
		int challenge_cd;				// 挑战冷却
		int challenge_clear_cd_fee;		// 清除挑战CD花费的元宝
        int chance_fee[ARENA_CHALLENGE_CHANCE_MAX_SIZE];   // 超出免费竞技次数后的费用
        int count_down;                 // 回合开始前的倒计时(秒)
        int round_time;                 // 回合时间(秒)(超出时间挑战失败)
        int wait_before_close;          // 挑战结束后等待时间(秒)
        std::string rank_reward_name;   // 竞技场的排名奖励
        std::string challenger_win;     // 挑战者获胜的通知 
        std::string challenger_failed;  // 挑战者落败的通知 
        std::string challengee_win;     // 被挑战者获胜的通知 
        std::string challengee_failed;  // 被挑战者落败的通知 
		int nBroadRank;					// 广播变化名次的排名
    };

    // 挑战的候选人规则
    struct CandidateRule
    {
		CandidateRule() : lower_rank(0), upper_rank(0), candidate_count(0){}
        int lower_rank;          // 此规则要求的玩家最低名次
        int upper_rank;          // 此规则要求的玩家最高名次
        int candidate_count;     // 此规则产生的候选人数
    };

    // 竞技场奖品
    struct ArenaAward
    {
		ArenaAward() : str_exp(""), str_capital(""), str_item(""){}
        std::string str_exp;					  // 经验计算公式
        std::string str_capital;
        std::string str_item;
    };

    // 挑战奖励
    struct ChallengeReward
    {
		ChallengeReward() : lower_level(0), upper_level(0){}
        int lower_level;
        int upper_level;
        ArenaAward winner_award;
        ArenaAward loser_award;
    };

    // 排名奖励
    struct RankReward
    {
		RankReward() : rank_list_name(""), lower_rank(0), upper_rank(0){}
        std::string rank_list_name;
        int lower_rank;
        int upper_rank;
        ArenaAward award;
    };
    
    // 竞技场的排行榜
    struct ArenaRankListConfig
    {
		ArenaRankListConfig() : lower_level(0), upper_level(0), name(""), level(0), need_notify(0){}
        int lower_level;
        int upper_level;
        std::string name;
        int level;       // 排行榜的等级
        int need_notify; // 升入此榜是否需要通知
    };
    
    // 竞技场的晋级奖励
    struct PromotionReward
    {
		PromotionReward() : rank_list_name(""), reward_name(""), capital(""), item(""){}
        std::string rank_list_name;
        std::string reward_name;
        std::string capital;
        std::string item;
    };
    
    // 名次提升奖励
    struct RankUpReward
    {
		RankUpReward() : rank_list_name(""), lower_rank(0), upper_rank(0){}
        std::string rank_list_name;
        int lower_rank;
        int upper_rank;
        
		ArenaAward award;
    };
	
	struct RanklistRecInfo
	{
		RanklistRecInfo() : rec(NULL), level(0){}
        std::string name;
        int level;
        IRecord *rec;
	};
	
public:

    // 角色数据恢复
    static int OnRecover(IKernel* pKernel, const PERSISTID& player,
        const PERSISTID& sender, const IVarList& args);
    
    //进入场景
    static int OnAfterEntryScene(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);
 
    // 就绪
    static int OnReady(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);
 
    // 客户端重连
    static int OnContinueReady(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);
 
    // 玩家离开场景
    static int OnLeaveScene(IKernel *pKernel, const PERSISTID &self,
        const PERSISTID &sender, const IVarList &args);

	static int OnPlayerContinue(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	//接收sns消息
	static int OnSnsMessage(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args);

	//--------------------------Sns服务器发来的消息处理函数-------------------------------
	// 查询战报回复
	static int OnSnsQueryFightResult(IKernel* pKernel, const IVarList& args);
    
    // 角色数据保存
    static int OnCreateRobot(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList &args);

    // 玩家等级变化
    static int OnPlayerLevel(IKernel* pKernel, const PERSISTID &self,
        const PERSISTID & sender, const IVarList & args);
        
    // 来自客户端的消息
    static int OnCustomMessage(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);

    // 来自公服的消息
    static int OnPublicMessage(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);
    
    // 发给玩家的command
    static int OnPlayerCommand(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);   
        
    // 开始战斗前的倒计时
    static int HB_BeforeFight(IKernel* pKernel, const PERSISTID& self, int slice);
    
    // 回合检查的计时器
    static int HB_RoundCheck(IKernel* pKernel, const PERSISTID& self, int slice);

    // 延时退出竞技场
    static int HB_CloseWait(IKernel* pKernel, const PERSISTID& self, int slice);
    
    // 离开竞技场场景
    static int HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice);

    // 完成一次击杀
    static int OnKillObject(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
        
    // 被击杀
    static int OnBeKilled(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 晋级玩家
//     static int PromotePlayer(IKernel* pKernel, const PERSISTID& self,
//         const char *current_ranklist_name, const char *new_ranklist_name);
// 
//     // 晋级玩家
//     static int PromotePlayer(IKernel* pKernel, const PERSISTID& self,
//         const char *current_ranklist_name, const char *new_ranklist_name, const IVarList &row_data);

	// 获取候选被挑战者
	static int OnGetChallengeCandidate(IKernel * pKernel, const PERSISTID & player,
		const PERSISTID & sender, const IVarList & args);

	// 请求挑战某人
	static int OnChallengeSomeone(IKernel * pKernel, const PERSISTID & player,
		const PERSISTID & sender, const IVarList & args);
private:

    // 查询自己的竞技场信息
    static int OnSelfArenaInfo(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);

    // 购买竞技场次数
    static int OnBuyArenaChance(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);
    
    // 放弃挑战
    static int OnGiveUpChallenge(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);
	
	// 重置挑战CD
	static int OnClearChallengeCD(IKernel * pKernel, const PERSISTID & player,
		const PERSISTID & sender, const IVarList & args);

	// 响应战报数据请求
	static int OnRequestFightResult(IKernel * pKernel, const PERSISTID & player,
		const PERSISTID & sender, const IVarList & args);

    // 获取玩家的竞技场信息
    static bool GetSelfArenaInfo(IKernel * pKernel, const PERSISTID & player,
        IVarList& summary);
    
    // 挑战结束, @result 输赢结果, @reason 原因
    static int OnChallengeEnd(IKernel * pKernel, const PERSISTID &challenger, int result, int reason);
    
    // 根据竞技结果，发送奖励
    static bool RewardByChallenge(IKernel * pKernel, const PERSISTID &challenger, int challenger_prev_rank,
		int challenger_new_rank, const wchar_t *challengee_name, int result);

	// 记录双方的战报数据
	void RecordFightResultData(IKernel * pKernel, const PERSISTID &challenger, const wchar_t *challengee_name, int challenger_prev_rank, int challenger_new_rank, int challengee_rank, int nResult);

	// 保存一条战报数据
	void SaveOneFightResultData(IKernel * pKernel, const PERSISTID &player, const wchar_t *enemy_name, int challenger_prev_rank, int challenger_new_rank, int nResult);

    // Public 服务器对挑战请求的回应
    static int OnPubChallengeRequest(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // Public 服务器对玩家晋升的回应
    static int OnPubPromote(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // Public 服务器对候选人的回应
    static int OnPubCandidate(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // Public 服务器对挑战结束的回应
    static int OnPubChallengeEnd(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

	// 增加一条战报数据
	static int OnAddFightResultData(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	// VIP升级回调
	static int C_OnVIPLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
public:

   // 挑战某人
    static bool ChallengeSomeone(IKernel * pKernel, const PERSISTID & player,
        const wchar_t* challengee_name);

    // 发放竞技场排名奖励
    static bool RewardByArenaRank(IKernel *pKernel);

    static int ResetArenaChanceTimer(IKernel* pKernel, const PERSISTID& self, int slice = 0);

    // 发放竞技奖励
    static bool RewardByPromotion(IKernel* pKernel, const PERSISTID& self, const char *rank_list_name);
    
    // 是否竞技场场景
    static bool IsArenaScene(IKernel * pKernel);
    
	// gm命令解锁自己的竞技场状态 bLockState为true 将竞技场状态锁定(是为了给质检测试用的), false清空竞技场挑战状态
	static bool ResetArenaStateByGm(IKernel * pKernel, const wchar_t* strName, int nLockState);

	// 清理玩家排行, 玩家有可能在多个榜中存有数据，只保留最高的榜
	static int PlayerRanklistClean(IKernel * pKernel);
private:

    // 解锁挑战选手
    static bool UnlockPlayer(IKernel * pKernel, const PERSISTID & player);
    
    // 冻结玩家
    static void FreezePlayer(IKernel * pKernel, const PERSISTID & player);
    
    // 解冻玩家
    static void UnfreezePlayer(IKernel * pKernel, const PERSISTID & player);

    // 等待一段时间然后离开竞技场
    static void WaitThenLeaveArena(IKernel * pKernel, const PERSISTID & player);
    
    // 立刻离开竞技场
    static void LeaveArenaNow(IKernel * pKernel, const PERSISTID & player);
    
    // 此玩家是否可以被挑战
    static bool IsValidChallengee(const char *challenger_uid,
        CVarList &challengee_info, const char *rank_list_name);
    
    // 重置竞技场次数
    static bool ResetArenaChance(IKernel * pKernel, const PERSISTID & player);
    
    // 送玩家去皇城
    static bool GoToCity(IKernel * pKernel, const PERSISTID & player);
    
    static bool IsRobot(IKernel * pKernel, const char *ranklist_name, const wchar_t *role_name);
    
    // 发送最佳名次突破奖励
    static int RewardByRankUp(IKernel *pKernel, const PERSISTID & player, const std::string &capital_reward, const std::string& item_reward, int new_rank);
    
    //////////////////////////////////////////////////////////////////////////
    // 私有的辅助函数
    //////////////////////////////////////////////////////////////////////////

    // 取得候选人规则
    static const CandidateRule* GetCandidateRule(int player_rank);
    
    // 取得挑战奖励设置
    static const ChallengeReward* GetChallengeReward(int player_level);
    
    // 取得排名奖励设置
    static const std::vector<RankReward>* GetRankReward(const std::string &rank_list_name);

    // 根据排行榜的名字获得晋级奖励
    static const PromotionReward* GetPromotionReward(const std::string &rank_list_name);
    
    // 取得竞技场排行榜的表
    static IRecord* GetArenaRankListRecord(IKernel * pKernel, const char* rank_list_name);

    // 根据等级取得对应的竞技场排行榜
    static const char* GetRankListName(IKernel * pKernel, int level);

    // 解析费用
    static bool ParseFee(std::string &str_fee, int *fee_array, int fee_array_size);

    // 解析坐标
    static bool ParseCoordinate(const std::string &str_coordinate, Coordinate& coordinate);

    // 发竞技场的消息到public服务器
    static void SendArenaMsgToPublic(IKernel * pKernel, const IVarList& msg);

    // 排行榜的一行
    static bool PrepareDataForArenaRankList(IKernel * pKernel, const PERSISTID & player,
        const char *rank_list_name, IVarList& row_data);
    
    // 把玩家加到竞技场的排行榜中
    static bool InsertPlayerToArenaRankList(IKernel * pKernel, const wchar_t *player_name,
                                            const char *rank_list_name, IVarList& row_data);
    
    // 排行榜的通知策略
    static NotifyStrategy GetRankListNotifyStrategy(std::string &rank_list_name);
    
    // 根据名次提升，获取奖励
	static bool GetRankUpReward(std::string &capital_reward, std::string& item_reward, const std::string &rank_list_name, int new_rank, int old_rank);
        
    // 是否历史最好名次
    static bool IsBestRank(IKernel * pKernel, const PERSISTID & player,
        int old_rank, int new_rank);
    
    // 把 float 转成INT, 向上取整
   // static int Float2IntRoundUp(float f);
    
    // 比较排行榜的等级
    static bool CompareRanklistLevel(const RanklistRecInfo &a, const RanklistRecInfo &b);
    
	// 重新加载配置
	static void ReloadConfig(IKernel * pKernel);

	// 冻结分组内所有对象
	void FreezeAllObject(IKernel* pKernel, IGameObj* pSelfObj);

	// 来自公服的消息
	static int OnCreateScene(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	// 导出公共区表数据
	void ExportPubRecData(IKernel* pKernel, int nStart, int nEnd);

	// 
	void ExportLevelRankData(IKernel* pKernel, const char* strRecName, int nStart, int nEnd, int nNo);
private:

    static RankListModule * m_pRankListModule;
    static CapitalModule * m_pCapitalModule;
    static LevelModule * m_pLevelModuel;
    static CopyOffLineModule *m_pCopyOfflineModule;
    static LandPosModule * m_pLandPosModule;
    static RewardModule * m_pRewardModule;
    static VipModule * m_pVipModule;
	static DynamicActModule * m_pDynamicActModule;
	static LogModule * m_pLogModule;
    
    typedef std::map<std::string, std::vector<RankReward> > RankRewardMap;
    typedef std::map<std::string, PromotionReward > PromotionRewardMap;
    
    typedef std::map<std::string, std::vector<RankUpReward> > RankUpRewardMap;
    
    static ArenaConfig m_ArenaConfig; 
    static std::vector<CandidateRule> m_CandidateRule;      // 竞技场候选人规则
    static std::vector<ChallengeReward> m_ChanllengeReward; // 竞技场挑战奖励
    static RankRewardMap m_RankReward;                      // 竞技场排名奖励
    static std::vector<ArenaRankListConfig> m_ArenaRankList;      // 竞技场的排行榜
    static PromotionRewardMap m_PromotionReward;            // 晋级奖励
    static RankUpRewardMap m_RankUpReward;             // 名次提升奖励
    
    static std::wstring m_domainName;  // 公共服务器的Domain数据名称
public:

	static ArenaModule* m_pArenaModule;

};

#endif