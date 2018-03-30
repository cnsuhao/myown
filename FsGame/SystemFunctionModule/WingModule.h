//--------------------------------------------------------------------
// 文件名:      WingModule.h
// 内  容:      翅膀模块
// 说  明:
// 创建日期:    2018年3月15日
// 创建人:      刘明飞 
//    :       
//--------------------------------------------------------------------
#ifndef _WING_MODULE_H_
#define _WING_MODULE_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include "CommonModule/CommRuleDefine.h"

class CapitalModule;
class ContainerModule;

class WingModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);

	//释放
	virtual bool Shut(IKernel* pKernel){ return true; }

	// 客户端相关消息
	static int  OnCustomWingMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 查询玩家的属性包
	int QueryWingLvPropId(IKernel* pKernel, const PERSISTID& self);
private:
	struct LevelData 
	{
		Consume_Vec		vecConsumeCapital;	// 升级消耗的钱
		Consume_Vec		vecConsumeItem;		// 消耗的物品
		int				nPropertyPak;		// 属性包id
	};
	
	typedef std::vector<LevelData> LevelDataVec;

	struct StepData
	{
		Consume_Vec		vecConsumeItem;		// 消耗的物品
		std::string		strProbability;		// 概率公式
		int				nMinBless;			// 失败获得的最小祝福值
		int				nMaxBless;			// 失败获得的最大祝福值
		int				nMaxWingLv;			// 解锁翅膀的最大等级
		int				nStepUpBlessVal;	// 升阶需要最低祝福值
		int				nWingModel;			// 翅膀外形
	};

	typedef std::vector<StepData> StepDataVec;

	enum 
	{
		MAX_BLESS_VALUE = 100,			// 最大祝福值(概率基数)
	};
private:
	//回调函数 玩家加载数据完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应翅膀功能激活
	static int OnCommandActivateWing(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应翅膀升级
	static int OnWingLevelUp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& item, const IVarList& args);

	// 重置升阶祝福值
	static int ResetStepBlessVal(IKernel *pKernel, const PERSISTID &self, int slice);

	// 响应翅膀升级
	void OnCustomLevelUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应翅膀升阶
	void OnCustomStepUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 读取配置
	bool LoadResource(IKernel* pKernel);

	// 读取升级配置
	bool LoadLevelResource(IKernel* pKernel);

	// 读取升阶配置
	bool LoadStepResource(IKernel* pKernel);

	// 检查升级翅膀的条件
	bool CheckUpgradeTerm(IKernel* pKernel, IGameObj* pSelfObj);

	// 消耗翅膀升级的物品和金钱
	bool LevelCostPlayerAssets(IKernel* pKernel, const PERSISTID& self, int nLevel);

	// 更新玩家翅膀属性
	bool UpdateWingModifyProp(IKernel* pKernel, const PERSISTID& self);

	// 查询升阶数据
	const StepData* QueryStepData(int nStep);

	// 查询升级数据
	const LevelData* QueryLevelData(int nLevel);

	static void ReloadConfig(IKernel* pKernel);
public:
	static WingModule* m_pWingModule;

private:         
	LevelDataVec	m_vecLevelData;		// 翅膀等级配置
	StepDataVec		m_vecStepData;		// 翅膀升阶配置

	static CapitalModule* m_pCapitalModule;
	static ContainerModule* m_pContainerModule;
};

#endif // _WING_MODULE_H_
