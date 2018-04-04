//--------------------------------------------------------------------
// 文件名:      WorldBossNpc.h
// 内  容:      世界Boss模块
// 说  明:		
// 创建日期:    2015年6月23日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef __WorldBossNpc_H__
#define __WorldBossNpc_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>

#include "FsGame/SystemFunctionModule/RewardModule.h"
#include "CommonModule/CommRuleDefine.h"

class ChannelModule;
class RewardModule;
class DropModule;
class LandPosModule;
class CapitalModule;
class CoolDownModule;
class ActYearBoss;
class SnsPushModule;

// 配置时间间隔
struct TimeInterval_t
{
	TimeInterval_t():
		m_BeginTime(0),
		m_EndTime(0){}
	int m_BeginTime;   // 开始时间(当天秒)
	int m_EndTime;     // 结束时间(当天秒)
};

// 配置时间间隔
struct GMTimeInfo
{
	GMTimeInfo() :
		m_NoticeBeginTime(0),
		m_NoticeEndTime(0),
		m_ActiveBeginTime(0),
		m_ActiveEndTime(0)
	{

	}

	int m_NoticeBeginTime;   // 通知开始时间(当天秒)
	int m_NoticeEndTime;     // 通知结束时间(当天秒
	int m_ActiveBeginTime;   // 活动开始时间(当天秒)
	int m_ActiveEndTime;     // 活动结束时间(当天秒)
};

// 活动详情配置
struct WorldBossActive_t
{
	WorldBossActive_t():
		m_ID(0),
		m_SceneID(0),
		m_NoticeBeginTime(0), 
		m_NoticeEndTime(0),
		m_ActiveBeginTime(0), 
		m_ActiveEndTime(0), 
		m_PlayerLevel(0)
		{
			m_Weeks.clear();
		}
	int                        m_ID;                  // ID
	int                        m_SceneID;             // 场景ID
	std::vector<int>           m_Weeks;               // 星期
	int                        m_NoticeBeginTime;     // 公告开始时间(当天秒)
	int                        m_NoticeEndTime;       // 公告结束时间(当天秒)
	int                        m_ActiveBeginTime;     // 活动开始时间(当天秒)
	int                        m_ActiveEndTime;       // 活动结束时间(当天秒)
	std::string				   m_strBossID;           // BOSSId
	PosInfo					   m_BossPos;             // BOSS出生坐标
	PosInfo					   m_PlayerPos;			 // 玩家传送坐标
	int                        m_PlayerLevel;         // 玩家参加活动最低等级
};

// AI伤害值数据拷贝结构
struct CopyAIHurt_t
{
	CopyAIHurt_t():
		m_player_name(L""), 
		m_damage(0){}

	std::wstring m_player_name; // 玩家名字
	int m_damage;				// 伤害值统计

	bool operator < (const CopyAIHurt_t& ch) const
	{
		return m_damage > ch.m_damage;
	}

	bool operator > (const CopyAIHurt_t& ch) const
	{
		return m_damage < ch.m_damage;
	}
};

// 排名奖励配置
struct RankAward_t
{
	RankAward_t():
		m_MinRank(0),
		m_MaxRank(0),
		m_nWinRewardId(0),
		m_nFailRewardId(0){}
	int m_MinRank;             // 最小排名
	int m_MaxRank;             // 最大排名
	int m_nWinRewardId;        // 胜利奖励id
	int m_nFailRewardId;       // 失败奖励id
};

// 奖励
struct WorldBossAward_t
{
	WorldBossAward_t(): m_BossCfgID("")
		{
			m_vecRankAward.clear();
		}

	std::string                m_BossCfgID;      // boss ID
	int						   m_nLastHurtAward;  // 伤害奖励
	std::vector<RankAward_t>   m_vecRankAward;   // 排名奖励 
};

// 活动开启规则
struct ConstConfig_t
{
	ConstConfig_t() : nEndRankNum(0),
					nReadyTime(0),
					nBornCGTime(0),
					nLeaveSceneDelay(0),
					nActiveRankNum(0)
	{

	}

	int nEndRankNum;				// 排名最大显示玩家数
	int nReadyTime;					// 准备时间
	int nBornCGTime;				// 出生后播放CG时间
	int nLeaveSceneDelay;			// 结束后离开场景延迟时间
	int nActiveRankNum;				// 活动中实时排行人数
};

// buff配置信息
struct BuffConfig
{
	BuffConfig():
		index(0),
		buffId(""),
		buffPrice(0),
		moneyType(0),
		limitBuy(0),
		coolDownBuy(0),
		buyRatio(0.0),
		addFight(0.0){}

	int         index;             // buff层级
	std::string buffId;            // buff ID
	int         buffPrice;         // buff 价格
	int         moneyType;         // 购买钱币类型
	int         limitBuy;          // 限制购买次数
	int         coolDownBuy;       // 冷却时间
	double      buyRatio;          // 购买概率
	double      addFight;          // 增加攻击百分比
};


// 世界boss成长配置
struct WorldBossGrowUp 
{
	int			nSceneId;			// 刷boss的场景
	int			nKillDurationTime;	// boss升级的条件(单位:s)(在此时间内杀死boss,则等级提升)
	int			nBasePackageId;		// 初始属性id
	int			nIncrement;			// 每次增加的等级
	int			nMaxLevel;			// 最大等级
};

typedef std::vector<WorldBossGrowUp> WorldBossGrowUpVec;

class WorldBossNpc : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);
	// 释放
	virtual bool Shut(IKernel* pKernel);

public:
	// 加载活动配置文件
	static bool LoadActiveInfoResource(IKernel* pKernel);
	// 加载奖励配置文件
	static bool LoadAwardResource(IKernel* pKernel);
	// 加载活动配置文件
	static bool LoadActiveRuleResource(IKernel* pKernel);
	// 加载世界boss成长配置
	bool LoadWorldBossGrowUpConfig(IKernel* pKernel);

	// 判定世界boss场景是否可进入
	bool IsSceneEnter(IKernel* pKernel, const PERSISTID& self, int nSceneId);
public:
	// 场景创建
	static int OnCreateScene(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 监听NPC死亡
	static int OnBossBeKilled(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 客户端消息处理
	static int OnCustomMessage(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家进入场景准备就绪
	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家断线重连
	static int OnPlayerContinue(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 断线重连等待回调
	static int OnCustomReady(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 内部命令回调
	static int OnCommandMessage(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);
	
	// GM命令
	static int OnCommandGMRestartActive(IKernel* pKernel, const PERSISTID& self, 
		const IVarList& args);

	// GM命令
	static int OnCommandGMEndActive(IKernel* pKernel, const PERSISTID& self, 
		const IVarList& args);

	// 通知活动结束
	static int NoticeActiveFinish(IKernel* pKernel, const PERSISTID& self, 
		bool bossBeKilled);

	// 角色数据恢复
	static int OnRecover(IKernel* pKernel, const PERSISTID& player,
		const PERSISTID& sender, const IVarList& args);

	// 玩家进入场景后回调
	static int OnPlayerEntryScence(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 监听BOSS被伤害
	static int WorldBossNpc::OnCommandBossBeDamaged(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);
public:
	// 最后一刀奖励
	static int ActiveLastDamageAward(IKernel* pKernel, const PERSISTID& boss, 
		const PERSISTID& killer);

	// 参与奖励
	//static int ActiveActorAward(IKernel* pKernel, const PERSISTID& CurrScene);

	// 排名奖励 nResult ActiveResult
	static int ActiveRankAward(IKernel* pKernel, const PERSISTID& CurrScene, int nResult);

	// 物品掉落
	static int DropItems(IKernel* pKernel,const PERSISTID& Attacker, 
		const PERSISTID& boss, const std::string& strItemList);

public:
	// 是否有年兽活动开启
	static int QueryBossActiveType();

private:
	// 创建BOSS
	static PERSISTID CreateWorldBoss(IKernel* pKernel, const WorldBossActive_t* pActiveInfo, const int& iCurrSceneID,  std::string& strBossPos, int iActiveTime, int iWeek);

	// 从BOSS身上拷贝伤害数据
	static bool CopyAndSortHurtRecordFromAI(IKernel* pKernel, 
		const PERSISTID& self, const PERSISTID& sender);

	// 清除一轮活动数据
	static bool ClearActiveData(IKernel* pKernel);

	// 查询世界boss活动状态
	void OnQueryWorldBossState(IKernel* pKernel, const PERSISTID& self);

	// 回复世界boss基本数据查询
	int OnQueryBaseInfo(IKernel* pKernel, const PERSISTID& self);
private:
	// 获取当前场景活动信息
	static WorldBossActive_t* GetActiveInfoCfg(IKernel* pKernel, const int iID);

	// 获取当前场景活动信息
	static WorldBossActive_t* GetActiveInfoCfgBySceneId(IKernel* pKernel, const int nSceneId);

	// 根据玩家找出活动场景
	static int GetActiveSceneByPlayer(IKernel* pKernel, const PERSISTID& self);

	// 获取活动奖励配置信息指针
	static WorldBossAward_t* GetBossAward(IKernel* pKernel, const char* strBossCfgID);

	// 处理配置文件里时间字符串
	static int ParseTime(const char* timesStr);

	static void ParseBossID(const char* strBossIDs, std::map<int, std::string>& mapBossIDs);

	// 算出星期几
	static void ParseWeek(const char* weeksStr,std::vector<int>& weeks);

	// 获取当前时间活动状态
	static int GetWorldBossActiveState(IKernel* pKernel, const PERSISTID& self, 
		const WorldBossActive_t* pActiveInfo, int iCurrSec);

	static int GetGMWorldBossActiveState(IKernel* pKernel, const PERSISTID& self, 
		const WorldBossActive_t* pActiveInfo, int iCurrSec);

	// 获取活动剩余时间
	static int GetRemainTime(const WorldBossActive_t* pActiveInfo);

	// 查询当前的活动
	static int QueryCurActivity(IKernel* pKernel);

	// 清除场景内的所有npc
	static void ClearSceneNpc(IKernel* pKernel);

	static void ReloadConfig(IKernel* pKernel);
	//////////////////////////////////公共数据////////////////////////////////////////
public:
	// 获取空间
	static IPubData* GetPubData(IKernel * pKernel);

	// 获取空间名
	static const std::wstring& GetDomainName(IKernel * pKernel);

	// 发消息给Pub服务器
	static bool SendMsgToPubServer(IKernel* pKernel, const IVarList &msg);

	// 获取活动状态
	static int GetActiveStatusFromPUB(IKernel* pKernel, int iSceneID);

	// 验证是否为世界boss场景
	static bool IsWorldBossScene(int iSceneID);
private:
	// 设置活动状态
	static int OnSaveActiveStatusToPUB(IKernel* pKernel, 
		const int iSceneID, const int iStatus, const int iID);

	// 获取活动配置ID
	static int GetActiveCfgIDFromPUB(IKernel* pKernel, int iSceneID);
	
	// 添加临时属性
	void AddSceneData(IKernel* pKernel, const PERSISTID& Scene);

	// member数据同步
	void SyncMemberData(IKernel* pKernel);

	//活动广播
	void ActiveNotice(IKernel* pKernel, const PERSISTID& Scene);

	// 活动开始
	void ActiveOpening(IKernel* pKernel, const PERSISTID& Scene, const WorldBossActive_t* pActiveInfo, const tm* pNow, int iState, int nConfigIndex);

	// 活动结束
	void ActiveClosed(IKernel* pKernel, const PERSISTID& Scene, int iState);
	//--------------------------------心跳----------------------------------//
private:
	// 定时发送活动即将开启信息
	static int HB_CheckNotice(IKernel* pKernel, const PERSISTID& self, int slice);

	// 世界Boss活动开启检测
	static int HB_CheckWorldBossActive(IKernel* pKernel, const PERSISTID& self, int slice);

	// 结算界面延迟推送((BOSS被击杀))
	static int HB_SendCustomMsg(IKernel* pKernel, const PERSISTID& self, int slice);

	// 开始播放CG
	static int HB_StartPlayCG(IKernel* pKernel, const PERSISTID& self, int slice);

	// 开始刷Boss
	static int HB_BossBorn(IKernel* pKernel, const PERSISTID& self, int slice);

	// 实时显示伤害列表
	static int HB_ShowAttackRank(IKernel* pKernel, const PERSISTID& self, int slice);

	// 离开场景心跳
	static int HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice);

	// 获取世界boss的属性包 
	int	QueryCurPackageId(IKernel* pKernel);

	// 获取成长配置
	const WorldBossGrowUp* GetGrowUpData(int nSceneId);

	// 更新boss等级
	void UpdateGrowUpLevel(IKernel* pKernel, bool bBeKilled);
public:
	static WorldBossNpc* m_pWorldBossNpc;            // 对外指针
	static ChannelModule* m_pChannelModule;          // 频道模块
	static RewardModule* m_pRewardModule;            // 领奖模块
	static DropModule* m_pDropModule;                // 掉落模块
	static LandPosModule* m_pLandPosModule;          // 传送
	static CapitalModule* m_pCapitalModule;          // 付费

public:
	//----------------------------公共数据名称--------------------------------//
	static std::wstring m_domainName;

	// 伤害copy加排序
	typedef std::vector<CopyAIHurt_t> CopyAIHurtVector;
	
	typedef std::vector<WorldBossAward_t> BossAwardVec;
	typedef std::vector<WorldBossActive_t> WorldBossActiveVec;

private:
	//----------------------------加载配置文件数据----------------------------//
	static std::vector<int> m_SceneScopeVec;
	//static std::map<int, std::vector<int>> m_SceneScopeMap;
	static BossAwardVec m_vecBossAward;
	static WorldBossActiveVec m_vecActiveInfo;
	
	//WBAllOtherNpcMap	m_mapAllOtherNpc;		// 世界boss场景小怪配置
	WorldBossGrowUpVec	m_vecWorldBossGrowUp;	// 世界boss成长配置

	// 活动规则测试
	typedef ConstConfig_t ConstConfig;
	static ConstConfig m_kConstConfig;
	


	// GM 设置活动时间
	static GMTimeInfo GMTime;

	// 活动时长
	static TimeInterval_t m_NoticeTimeCount;
	static TimeInterval_t m_ActiveTimeCount;

	static int			m_nCustomSceneId;			// 通知客户端的场景id

	
// 	static std::map<int, BuffConfig> m_CopperBuffInfoMap;
// 	static std::map<int, BuffConfig> m_SilverBuffInfoMap;
};

#endif