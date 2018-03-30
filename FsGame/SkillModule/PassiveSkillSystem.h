//----------------------------------------------------------
// 文件名:      PassiveSkillUpgrade.h
// 内  容:      被动技能养成系统
// 说  明:		
// 创建日期:    2015年6月24日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------

#ifndef _PASSIVESKILLUPGRADE_H_
#define _PASSIVESKILLUPGRADE_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>
#include "utils/Singleton.h"
#include "FsGame/Define/Skilldefine.h"

class SkillDataQueryModule;
struct PassiveSkillLevelInfo;
struct PassiveSkill;
class ContainerModule;
#ifndef FSROOMLOGIC_EXPORTS
class SystemMailModule;
#endif
class CapitalModule;

class PassiveSkillSystem
{
public:
	// 初始化
	bool Init(IKernel* pKernel);

	/*!
	 * @brief	增加或更新被动技能 宠物被动技能用到
	 * @param	self 对象号(可能是宠物也可能是玩家)
	 * @param	nSkillId 技能id
	 * @param	nSkillLevel 技能等级
	 * @param	nSkillSrc  技能来源
	 * @return	bool
	 */
	bool UpdatePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId, int nSkillLevel, int nSkillSrc);

	/*!
	 * @brief	移除被动技能
	 * @param	self 对象号(可能是宠物也可能是玩家)
	 * @param	nSkillId技能id
	 * @return	bool
	 */
	bool RemovePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId);

	// 查找被动技能等级,没找到返回0
	int QueryPassiveSkillLevel(IKernel* pKernel, const PERSISTID &self, int nSkillId);

	// 响应被动技能升级消息(记日志)
	static int OnCustomOnPassiveSkillUpgrade(IKernel* pKernel, const PERSISTID &self,
		const PERSISTID & sender, const IVarList & args);

	// 响应被动技能重置消息(记日志)
	static int OnCustomResetPassiveSkill(IKernel* pKernel, const PERSISTID &self,
		const PERSISTID & sender, const IVarList & args);

	// 学习所有被动技能(测试、调试使用)
	void LearnAllPassiveSkill(IKernel* pKernel, const PERSISTID &self);
private:
	enum 
	{
		PS_ADD_EFFECT,			// 增加特效
		PS_REMOVE_EFFECT,		// 移除特效

		MAX_PS_EFFECT_INDEX = 32 // 最大32位
	};
	// 重置技能点
	struct ResetCost 
	{
		ResetCost() : strItemId(""), nItemNum(0){}
		std::string strItemId;			// 花费的物品
		int			nItemNum;			// 物品数量
	};

	//回调函数 玩家加载数据完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 技能点
	struct SkillPoint
	{
		SkillPoint() : nMeridianLevel(0), nAddSkillPoint(0){}
		int			nMeridianLevel;		// 玩家等级
		int			nAddSkillPoint;		// 增加的技能点
	};

	typedef std::vector<SkillPoint> SkillPointVec;

	// 检查技能的升级条件
	bool CheckUpgradeTerm(IKernel* pKernel, IGameObj* pSelfObj, const PassiveSkill* pMainInfo, const PassiveSkillLevelInfo* pSkillInfo, const int nRequestSkillLevel);

	// 扣除玩家财产(钱,材料,技能点)
	bool UpgradeCostPlayerAssets(IKernel* pKernel, IGameObj* pSelfObj, const PassiveSkillLevelInfo* pSkillInfo);

	// 响应技能升级后的变化
	void OnPassiveSkillChange(IKernel* pKernel, const PERSISTID &self, int nSkillId, const PassiveSkillLevelInfo* pSkillInfo);

	// 归还某个被动的钱、技能点、材料
	void GiveBackOneSkillAssets(IKernel* pKernel, int& outSkillPoint, int nSkillId, int nSkillLevel);

	// 读取技能点配置
	bool LoadSkillPointAddConfig(IKernel* pKernel);

	// 查询对应等级增加的技能点
	int QueryAddSkillPoint(int nPlayerLevel);

	// 技能重置物品检查及扣除
	bool ResetCostItem(IKernel* pKernel, const PERSISTID &self);

	// 删除被动技能的处理
	bool OnRemovePassiveSkill(IKernel* pKernel, const PERSISTID &self, int nSkillId, int nSkillLevel);

	// 添加被动技能影响的属性
	void AddAllPassiveSkillPro(IKernel* pKernel, IGameObj* pSelfObj);

	// 执行被动技能重置
	void ExecuteResetPassiveSkill(IKernel* pKernel, const PERSISTID& self);

	// 检查是否有学习的被动技能
	bool IsHavePassiveSkill(IKernel* pKernel, const PERSISTID& self);

	// 统计某个技能消耗的技能点
	int StatOneSkillCostPoint(int nSkillId, int nSkillLevel, const PassiveSkill* pMainInfo);
private:
	SkillPointVec		m_vecSkillPointRec;	// 玩家技能点配置表

	static SkillDataQueryModule* m_pSkillDataQueryModule;
	static ContainerModule*		m_pContainerModule;
	static CapitalModule*		m_pCapitalModule;

#ifndef FSROOMLOGIC_EXPORTS
	static SystemMailModule*	m_pSystemMailModule;
#endif // FSROOMLOGIC_EXPORTS
};

typedef HPSingleton<PassiveSkillSystem> PassiveSkillSystemSingleton;

#endif