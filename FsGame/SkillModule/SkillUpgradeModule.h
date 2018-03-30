//---------------------------------------------------------
//文件名:       SkillUpgradeModule.h
//内  容:       技能和技能升级模块
//说  明:  
//          
//创建日期:      2016年4月5日
//创建人:         刘明飞
//修改人:
//   :         
//---------------------------------------------------------

#ifndef FSGAME_SKILL_SKILL_UPGRADE_H_
#define FSGAME_SKILL_SKILL_UPGRADE_H_

#include "Fsgame/Define/header.h"
#include <string>
#include <map>
#include <vector>

class ContainerModule;
class CapitalModule;
class LogModule;
class DailyActivityModule;

class SkillUpgradeModule : public ILogicModule
{
public:

    virtual bool Init(IKernel* pKernel);

    virtual bool Shut(IKernel *pKernel);


public:
	enum
	{
		PUBLIC_CAREER_SKILL = 0,			// 通用技能标志
	};
    // 玩家数据恢复
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& player,
        const PERSISTID& sender, const IVarList& args);

    // 处理来自客户端的消息
    static int OnSkillSetupMsg(IKernel *pKernel, const PERSISTID &player,
        const PERSISTID & sender, const IVarList & args);

    // 玩家升级技能的请求
    int OnUpgradeSkill(IKernel *pKernel, const PERSISTID &player, int nUpgradeType, const IVarList & args);

    // 玩家角色升级
    static int OnPlayerLevelUp(IKernel* pKernel, const PERSISTID& self,
        const char* property, const IVar& old);

	// 获取玩家的普通技能
	int GetUsableNormalSkill(IKernel *pKernel, const PERSISTID &player, std::vector<std::string> *out_skill);

	// 载入配置资源
	bool LoadResource(IKernel* pKernel);
private:
	// 升级方式
	enum UpgradeType
	{
		UPGRADE_ONE_LEVEL,			// 升一级
		UPGRADE_MULTI_LEVEL			// 升到最高级
	};

    //　普通技能的定义, 普通技能不属于任何技能
    struct NormalSkillDef
    {	
		NormalSkillDef() : id(""), career_id(0), level_to_unlock(0){}
        std::string id;
        int career_id;
        int level_to_unlock;
    };

	// 技能的基本定义
	struct SkillBaseDef
	{
		SkillBaseDef() : id(""), seq_no(0), career_id(0), level_to_unlock(0), gold_type_to_upgrade(0),
		gold_amount_to_upgrade(0), max_level(0){}
		std::string id;         // 技能ID
		int seq_no;             // 技能的序号
		int career_id;			// 技能所属职业ID
		int level_to_unlock;    // 解锁此技能需要的玩家等级
		int gold_type_to_upgrade;      // 升级此技能需要的金币类型
		int gold_amount_to_upgrade;    // 升级此技能需要的金币
		int max_level;                 // 此技能的最高等级
	};

	// 技能等级的价格, 就是升级到等级N需要的花费
	struct SkillLevelCostDef
	{
		SkillLevelCostDef() : skill_seq_no(0), skill_level(0), capital_type(0), capital_amount(0){}
		int skill_seq_no;    // 技能序号, 每个职业有四个技能, 他们的序号分别是 1,2,3,4
		int skill_level;    // 技能等级
		int capital_type;   // 此等级需要的资金类型
		int capital_amount; // 此等级需要的资金数量
	};
private:
	// 处理技能升级的逻辑
	bool UpgradeSkillLevel(IKernel * pKernel, const PERSISTID & player, const char * skill_id, int nUpgradeType, int &nOldCittaLevel, int& nNewCittaLevel);
    
	// 获取技能配置
	const SkillBaseDef* GetSkillDef(const char *skill_id);

	// 获取技能消耗配置
	const SkillLevelCostDef* SkillUpgradeModule::GetSkillLevelCost(int skill_seqno, int level);

	// 设置玩家的普通技能
	int SetPlayerNormalSkill(IKernel *pKernel, const PERSISTID &player);

	// 玩家技能学习
	int LearnPlayerSkill(IKernel *pKernel, const PERSISTID &player);

	// 载入技能配置
	bool LoadSkill(IKernel* pKernel);

	// 载入技能等级价格
	bool LoadSkillLevelCost(IKernel* pKernel);

	//　载入普通技能
	bool LoadNormalSkill(IKernel* pKernel);
	
	// 查询某个技能的战斗力
// 	int QuerySkillBA(const char* strSkillId, int nSkillLevel);
// 
// 	// 计算技能升级后的战斗力加成
// 	int CalBAOnSkillUpgrade(const char* strSkillId, int nPrevLevel, int nNewLevel);
// 
// 	// 计算玩家的技能战斗力
// 	int CalPlayerSkillBA(IKernel *pKernel, const PERSISTID &self);
// 
// 	// 把战斗力加给玩家
// 	int AddBAToPlayer(IKernel* pKernel, const PERSISTID& self, int the_battle_ability);

	static void ReloadConfig(IKernel* pKernel);
public:

    static SkillUpgradeModule *m_pSkillUpgradeModule;

private:
    static CapitalModule *m_pCapitalModule;

    // 技能的等级价格设定
    typedef std::map<std::string, SkillLevelCostDef> SkillLevelCostMap;
    SkillLevelCostMap m_mapSkillLevelCost;

    // 全部的技能
    typedef std::map<std::string, SkillBaseDef> SkillMap;
    SkillMap m_mapSkillData;

    // 全部普通技能
    typedef std::map<std::string, NormalSkillDef> NormalSkillMap;
    NormalSkillMap m_mapNormalSkill;
}; // End of class

#endif