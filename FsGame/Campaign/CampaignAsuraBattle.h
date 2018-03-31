//--------------------------------------------------------------------
// 文件名:      CampaignAsuraBattle.h
// 内  容:      修罗战场活动
// 说  明:
// 创建日期:    2017年10月19日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#ifndef _Campaign_Asura_Battle_H_
#define _Campaign_Asura_Battle_H_

#include "Fsgame/Define/header.h"
#include "CampaignBase.h"
#include <vector>

class CampaignModule;
class LandPosModule;

class CampaignAsuraBattle : public CampaignBase
{
public:
	virtual bool Init(IKernel* pKernel);

	virtual void OnChangeCampainState(IKernel* pKernel, int nCurState);

	virtual bool LoadResource(IKernel* pKernel);

	// 活动是否开启
	static bool IsCampaignStart(IKernel* pKernel);

	// 响应客户端消息
	static int OnCustomAsuraBattle(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
private:  
	// 响应玩家进入场景
	static int OnPlayerReady(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 响应玩家离开场景
	static int OnPlayerLeaveScene(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 响应活动消息
	static int OnCommandCampaignMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应BOSS被攻击
	static int OnCommandBossBeDamage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应BOSS被杀死
	static int OnCommandBossBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应活动状态改变
	int OnCommandCampaignStateChange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应进入修罗战场
	void OnCustomEnterScene(IKernel* pKernel, const PERSISTID& self);

	// 响应采集战资
	void OnCustomCollectMaterial(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应提交战资
	void OnCustomSubmitMaterial(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应查询排行榜
	void OnCustomQueryRankList(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应购买buff
	void OnCustomBuyBuff(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应退出修罗战场
	void OnCustomExitScene(IKernel* pKernel, const PERSISTID& self);

	// 读取采集物品配置
	bool LoadMaterialConfig(IKernel* pKernel);

	// 响应目标被杀
	static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, const  PERSISTID& sender, const IVarList& args);

	// 读取buff配置
	bool LoadCampBuffConfig(IKernel* pKernel);

	// 读取常量配置
	bool LoadConstConfig(IKernel* pKernel);

	// 读取商店buff配置
	bool LoadShopBuffConfig(IKernel* pKernel);

	// 读取boss积分规则
	bool LoadBossScoreRule(IKernel* pKernel);

	// 是否在修罗战场中
	static bool IsInAsuraBattleScene(IKernel* pKernel);

	// 采集物品生成检测心跳
//	static int HB_GenerateMaterial(IKernel* pKernel, const PERSISTID& self, int slice);

	// 发放结算奖励
	static int HB_SendReward(IKernel* pKernel, const PERSISTID& self, int slice);

	// 活动结束,离开场景
	static int HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice);

	// 在某点生成n个战资
	void GenerateMaterialAtPos(IKernel* pKernel, int nPosIndex);

	// 更新玩家自己的积分
	void UpdateFightData(IKernel* pKernel, const PERSISTID& self, int nChangeScore, int nColType = -1, int nChangeFightData = 0);

	// 根据阵营获取积分表
	IRecord* GetCampScoreRec(IKernel* pKernel, int nCamp);

	// 更新阵营数据表
	void UpdateCampFightRec(IKernel* pKernel, int nCamp, int nColType, int nChangeVal);

	// 获取当前采集阶段
	int QueryCollectState(int nCollectNum);

	// 响应阵营激活采集阶段
	void OnActiveCampCollectState(IKernel* pKernel, int nCamp, int nCollectState);

	// 查询当前的采集状态
	int QueryCampCollectState(IKernel* pKernel, int nCamp);

	// 计算增加的积分
	int ComputeAddScore(IKernel* pKernel, const PERSISTID& self, int nScore);

	// 检测广播消息通告
	static int HB_CheckBroadcastInfo(IKernel* pKernel, const PERSISTID& self, int slice);

	// 剩余时间通告
	void BroadcastTimeLeftTip(IKernel* pKernel);

	// 通告当前阵营积分第一
	void BroadcastFirstPlayerTip(IKernel* pKernel, int nSlice);

	// 设置当前战场的mvp
	void SetBattleMVP(IKernel* pKernel);

	// 检测boss刷新
	void CheckBossBorn(IKernel* pKernel);

	// 同步boss战斗数据
	void SyncBossFightData(IKernel* pKernel);

	// 必要战斗数据同步(积分榜重新排序)
	void SyncCampFightInfo(IKernel* pKernel);

	// 获取阵营当前在场景中排名第一的玩家的位置
	bool FindFirstPlayerPos(IKernel* pKernel, float& fOutPosX, float& fOurPosZ, int nCamp);

	// 生成同步的阵营战斗数据
	void GenerateCampFightInfo(IKernel* pKernel, IVarList& outFightInfo, int nCamp);

	// 生成同步的排行榜数据
	void GenerateFightRankNum(IKernel* pKernel, IVarList& outFightInfo, int nCamp);

	// 活动结束
	void CampaignOver(IKernel* pKernel);

	// 胜负判定
	int GenerateResult(IKernel* pKernel);

	// 读取名次奖励
	bool LoadRankRewardConfig(IKernel* pKernel);

	// 按照阵营发放结算奖励
	void SendBattleRewardByCamp(IKernel* pKernel, int nCamp);

	// 获取双方阵营的总积分
	bool GetCampFightScore(IKernel* pKernel, int& nOutJHScore, int& nOurCTScore);

	// 更新玩家自己的数据
	void UpdatePlayerData(IKernel* pKernel, const PERSISTID& self, int nChangeScore, int nColType = -1, int nChangeVal = -1);

	// 获取玩家数据列
	int GetPlayerColType(IKernel* pKernel, int nCampColType);

	// 清空玩家战斗数据
	void ClearPlayerFightData(IKernel* pKernel, const PERSISTID& self);

	// 查询玩家战斗数据
	int QueryPlayerFightData(IKernel* pKernel, const PERSISTID& self, int nColType);

	// 统计boss伤害数据
	bool StatDamageBossData(IKernel* pKernel, const PERSISTID& self, int& nJHTotalDamVal, int& nCTTotalDamVal, int& nJHPlayerNum, int& nCTPlayerNum);
private:
	enum 
	{
		CHECK_BROADCAST_INTERVAL = 5000,			// 广播信息检测间隔
		BATTLE_DRAW_RESULT = 3,						// 平局了,双方都发胜利奖励
		CLEAR_PLAYER_FIGHT_DATA = -1,				// 清空玩家战斗数据
	};
	// 复活点
	struct Pos
	{
		Pos() : fPosX(0.0f),
					fPosZ(0.0f)	,
					fOrient(0.0f)
		{

		}
		float	fPosX;
		float	fPosZ;
		float	fOrient;
	};

	// 常量配置
	struct ConstData 
	{
		int				nSceneId;				// 场景id
		int				nMinLevel;				// 最低玩家等级
		float			fKillScoreRate;			// 杀人积分扣除比例
		float			fKillAddRate;			// 击杀者增加积分比例
		float			fATAddRate;				// 助攻积分比例
		float			fPeerageAddRate;		// 官阶加成比例
		int				nBroadcastCondition;	// 杀人通告的条件
		int				nPlayerInitScore;		// 玩家的初始积分
		Pos				kJHBornPos;				// 江湖出生点
		Pos				kCTBornPos;				// 朝廷出生点
		int				nGenrateInterval;		// 生成采集物品间隔时间(s)
		int				nKillBaseScore;			// 杀人获得的基础分
		float			fMaxPeerageAddRate;		// 爵位加成比例上限
		float			fMinPeerageAddRate;		// 爵位加成比例下限
		float			fLeaveDecScoreRate;		// 离开活动场景扣除的积分
		std::string		strWinReward;			// 胜利通用奖励
		std::string		strLoseReward;			// 失败通用奖励
		int				nDelaySendRewardTime;	// 延迟发放奖励时间
		int				nDelayExitTime;			// 延迟退出活动时间
		int				nSettleRankNum;			// 结算显示排名数
		int				nScoreFirstTipInterval;	// 积分第一提示间隔
		int				nDeadBuyTipNum;			// 死亡购买buff提示
		int				nFightRankNum;			// 实时排行榜显示个数
		int				nBossBornTime;			// boss出生时间(离结束还有多久 单位:min)
		int				nBossTipTime;			// boss出生提示(离结束还有多久 单位:min)
		Pos				kBossBornPos;			// Boss出生位置
		std::string		strBossId;				// Bossid
	};

	// 采集
	struct MaterialGenRule 
	{
		Pos			kCenterPos;					// 中心点
		int			nJHStartScore;				// 江湖筹备期积分
		int			nCTStartScore;				// 朝廷筹备期积分
		int			nJHFightScore;				// 江湖备战期积分
		int			nCTFightScore;				// 朝廷备战期积分
		float		fRadius2;					// 区域半径
	};

	typedef std::vector<MaterialGenRule> MaterialGenRuleVec;

	// 阵营buff
	struct CampBuff
	{
		int			nCollectState;				// 采集阶段
		int			nMinCollectNum;				// 最小采集数
		int			nMaxCollectNum;				// 最大采集数
		std::string strBuffId;					// Buff
	};

	typedef std::vector<CampBuff> CampBuffVec;

	// 商店buff
	struct ShopBuff 
	{
		std::string	strBuffId;					// buff
		int			nCapitalCost;				// 货币数
		int			nCapitalType;				// 货币类型
	};

	typedef std::vector<ShopBuff> ShopBuffVec;

	typedef std::vector<int> TimeLeftTipVec;

	// 排名数据
	struct SortData
	{
		SortData() :
		wsPlayerName(L""),
		nSortVal(0){}

		std::wstring wsPlayerName; // 玩家名字
		int nSortVal;				// 数值统计
		int nKillNum;				// 杀敌数
		int nDeadNum;				// 死亡数
		int nAssistNum;				// 助攻数

		bool operator < (const SortData& ch) const
		{
			return nSortVal > ch.nSortVal;
		}
	};

	typedef std::vector<SortData>	SortDataVec;

	// 排名奖励数据
	struct RankReward 
	{
		int			nHightRank;		// 较高排名
		int			nLowRank;		// 较低排名
		std::string	strReward;		// 奖励
	};

	typedef std::vector<RankReward> RankRewadVec;

	// boss积分
	struct BossScoreRule 
	{
		int			nMinNum;		// 击杀玩家数区间
		int			nMaxNum;		
		int			nAddScore;		// 增加的积分
	};

	typedef std::vector<BossScoreRule> BossScoreRuleVec;

	// 解析出生点坐标
	bool ParsePos(Pos& outPos, const char* strPos);

	// 根据buffid查询商店buff数据
	const ShopBuff* QueryShopBuffData(const char* strBuffId);

	// 提示结束的时间点配置解析
	bool ParseTimeTipData(const char* strInfo);

	// 对积分表排序
	bool ExecuteRecordDataSort(IKernel* pKernel, SortDataVec& outVecSort, int nCamp);

	// 根据采集状态,查找buff
	const CampBuff* QueryCollectData(int nCollectState);

	// 查询玩家当前的积分排名
	int QueryPlayerRank(IKernel* pKernel, const wchar_t* wsName, int nCamp);

	// 获取名次的奖励数据
	const char* GetRankReward(int nRank);

	// 清除战场数据
	void ClearBattleData(IKernel* pKernel);

	// 查询某个阵营的mvp
	const wchar_t* QueryMVPPlayer(IKernel* pKernel, int nCamp);

	// 查询击杀boss增加的积分
	int QueryKillBossAddScore(int nPlayerNum);
private:															    
	ConstData				m_kConstData;				// 活动场景配置
	MaterialGenRuleVec		m_vecMaterialGenRule;		// 战资生成规则
	CampBuffVec			m_vecCampBuff;			// 阵营buff
	ShopBuffVec				m_vecShopBuff;				// 玩家购买的buff
	TimeLeftTipVec			m_vecTimeLeftTip;			// 时间提示
	SortDataVec				m_vecJHSortData;			// 江湖排名数据
	SortDataVec				m_vecCTSortData;			// 朝廷排名数据
	RankRewadVec			m_vecRankReward;			// 排名奖励
	BossScoreRuleVec		m_vecBossScoreRule;			// boss积分规则

	static CampaignModule*	m_pCampaignModule;

	static 	CampaignAsuraBattle*	m_pCampaignAsuraBattle;
	static	LandPosModule*			m_pLandPosModule;
};

#endif
