//--------------------------------------------------------------------
// 文件名:      BattleAbilityModule.h
// 内  容:      战斗力统计
// 说  明:
// 创建日期:    2017/04/12
// 创建人:      刘明飞
//    :       
//--------------------------------------------------------------------

#ifndef __BATTLE_ABILITY_MODULE_H__
#define __BATTLE_ABILITY_MODULE_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>
#include "Define/PlayerBaseDefine.h"

class BattleAbilityModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);

	//释放
	virtual bool Shut(IKernel* pKernel);
public:
	// 读取配置
	bool LoadConfig(IKernel* pKernel);
private:
	// 属性值战力数据
	struct ValueBAData  
	{
		ValueBAData() : strProName("")
		{
			ZeroMemory(arrRate, sizeof(arrRate));
		}
		std::string		strProName;			// 属性名
		float	arrRate[PLAYER_JOB_MAX];	// 战斗力系数
	};

	typedef std::vector<ValueBAData>   ValueBADataVec;

	// 百分比战力数据
	struct RateBAData
	{
		std::string			strProName;		// 属性名 
		std::vector<float>	vecLevelRate;	// 等级系数列表

		bool operator == (const RateBAData &other) const
		{
			return strProName == other.strProName;
		}
	};

	typedef std::vector<RateBAData>		RateBADataVec;

	struct FormulaBAData
	{
		std::string		strFormulaId;	  // 公式id
		int				nCalType;		  // 计算类型	(BAFormulaCalType)
		std::string		strFormula;		  // 公式
	};

	// 公式战力数据(技能、被动技能)
	typedef std::map<std::string, FormulaBAData>		FormulaBADataMap;

	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应更新战斗力
	static int OnCommandUpdateBattleAbility(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static void ReloadConfig(IKernel* pKernel);

	// 读取属性值战力数据
	bool LoadValueBAConfig(IKernel* pKernel);

	// 读取百分比战力数据
	//bool LoadRateBAConfig(IKernel* pKernel);

	// 读取公式战力数据
	bool LoadFormulaBAConfig(IKernel* pKernel);

	// 更新属性战斗力
	void UpdatePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj);

	// 更新主动技能战斗力
	void UpdateSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj);

	// 更新被动技能战斗力
	void UpdatePassiveSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj);

	// 计算某一个玩家值属性增加的战力
	float CalValuePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const ValueBAData& data);

	// 计算某一个玩家百分比属性增加的战力
	float CalRatePropertyBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const RateBAData& data);

	// 计算某个主动技能增加的战力
	float CalSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skill);

	// 计算某个被动技能增加的战力
	float CalPassiveSkillBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, int nPassiveSkillId);

	// 统计总战斗力
	void StatisticsTotalBattleAbility(IKernel* pKernel, const PERSISTID& self, IGameObj* pSelfObj);

	// 增加一条百分比配置
	void ParseRateBAData(const char* strProName, float fRate);

	// 查询属性值
	float QueryProperty(IGameObj* pSelfObj, const char* strProName);

	// 更新某个类型的战力值
	void RefreshOneTypeBAValue(IGameObj* pSelfObj, int nBAType, float fBAValue);

	// 计算公式战斗力
	float CalFormulaBattleAbility(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skill, const FormulaBAData& data);

	// 查询某个属性的战力系数
	float QueryPropertyBARate(IGameObj* pSelfObj, const char* strProName);
public:
	static BattleAbilityModule* m_pInstance;
private:
	ValueBADataVec		m_vecValueBAData;		// 属性值战力数据
	RateBADataVec		m_vecRateBAData;		// 百分比战力数据
	FormulaBADataMap	m_mapFormulaBAData;		// 公式战力数据
};
#endif // __BATTLE_ABILITY_MODULE_H__