//--------------------------------------------------------------------
// 文件名:		CriminalSystem.h
// 内  容:		罪恶惩罚系统
// 说  明:					
// 创建日期:	2017年07月24日
// 创建人:		liumf   
//--------------------------------------------------------------------

#ifndef __CRIMINAL_SYSTEM_H__
#define __CRIMINAL_SYSTEM_H__

#include "Fsgame/Define/header.h"
#include "utils/Singleton.h"
#include <vector>

class CriminalSystem
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

	// 配置读取
	bool LoadResource(IKernel* pKernel);

	// 花费元宝消除罪恶值
	int OnCustomCostMoneyCleanCrime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 增加罪恶值积分
	void AddDecCrimeScore(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pDeadNpcObj);

	// 查询红名惩罚系数 不是红名返回false	  nFunctionType功能类型  nQueryType查询系数类型
	bool QueryCiminalRate(IKernel* pKernel, float& fOutPublishRate, const PERSISTID& self, int nFunctionType, int nQueryType);

	// 必要的条件验证
	static bool IsValidPublicScenePVP(IKernel* pKernel, const PERSISTID& attacker, const PERSISTID& target, IGameObj*& pAttackerObj, IGameObj*& pTargetObj);
private:
	// 罪恶常量
	struct CriminalConst 
	{
		int		nGrayToNormalTime;		// 灰名变正常的时间(s)
		int		nRedCrimeValue;			// 红名的罪恶值
		int		nKillCrimerChivalry;	// 杀红名增加的侠义值
		int		nTimeToCrimeRate;		// 在线多久减1点罪恶值 单位(m)
		int		nClearCrimeCostRate;	// 清除1点罪恶值消耗的货币
		int		nJailClearCrimeCostRate;	// 坐牢时清除1点罪恶值消耗的货币
		int		nClearCrimeMoneyType;	// 货币类型
		int		nScroeToCrimeRate;		// 多少分减1点罪恶值
		int		nMaxChivalry;			// 每天最多杀红名得的侠义值
		int		nMaxCrimeValue;			// 最大罪恶值
		int		nForceBeJailCrimeVal;	// 强制坐牢的罪恶值
	};

	// 罪恶值规则
	struct CriminalRule 
	{
		int		nMinLevelDiff;			// 最小等级差
		int		nMaxLevelDiff;			// 最大等级差
		int		nCriminalValue;			// 增加的罪恶值
	};

	typedef std::vector<CriminalRule> CriminalVec;

	// 罪恶值杀怪积分
	struct CrimieScore
	{
		int		nNpcType;				// npc类型
		int		nScore;					// 积分
	};

	typedef std::vector<CrimieScore> CrimieScoreVec;

	// 罪恶惩罚数据
	struct CrimiePublishData
	{
		int		nFunctionType;			// 功能类型
		float	fCapitalPublishRate;	// 金币衰减
		float	fExpPublishRate;		// 经验衰减
	};

	typedef std::vector<CrimiePublishData> CrimPDVec;

	enum 
	{
		 CHECK_CRIME_STATE_TIME_INTERVAL = 5000,		// 罪恶值在线检测频率
	};

	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 脱离灰名心跳
	static int HB_CleanCrimeState(IKernel * pKernel, const PERSISTID & self, int slice);

	// 在线减少罪恶值
	static int HB_DecCrimeValue(IKernel * pKernel, const PERSISTID & self, int slice);

	// 响应伤害某个玩家
	static int OnCommandDamagePlayer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应被杀死
	//static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应杀死玩家
	static int OnCommandKillPlayer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//战斗状态发生改变
	static int C_OnFightStateChanged(IKernel *pKernel, const PERSISTID &self, const char *property, const IVar &old);

	//罪恶值变化
	static int C_OnCrimeValueChanged(IKernel *pKernel, const PERSISTID &self, const char *property, const IVar &old);

	// 读取常量配置
	bool LoadConstConfig(IKernel* pKernel);

	// 读取罪恶值增加规则
	bool LoadCriminalRule(IKernel* pKernel);

	// 读取罪恶值杀怪积分规则
	bool LoadCrimeScorelRule(IKernel* pKernel);

	// 读取罪恶惩罚规则数据
	bool LoadCrimiePublishData(IKernel* pKernel);

	// 查询增加的罪恶值
	int QueryAddCriminalValue(IGameObj* pAttackerObj, IGameObj* pTargetObj);

	// 查询增加的积分
	int QueryAddDecCrimeScore(IGameObj* pTargetObj);

	// 增加侠义值
	//void AddChivalry(IKernel* pKernel, IGameObj* pPlayerObj);

	// 杀红名获得的侠义值重置
	//static int ResetAddChivalry(IKernel *pKernel, const PERSISTID &self, int slice);
private:
	CriminalConst		m_kCriminalConst;		// 罪恶系统常量
	CriminalVec			m_vecCriminalValue;		// 罪恶值增加规则
	CrimieScoreVec		m_vecCrimieScore;		// 罪恶值积分规则
	CrimPDVec			m_vecCrimiePublishData;	// 惩罚规则数据
};

typedef HPSingleton<CriminalSystem> CriminalSystemSingleton;
#endif //__GAINT_MODULE_H__
