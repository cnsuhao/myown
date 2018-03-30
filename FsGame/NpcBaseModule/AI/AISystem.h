//--------------------------------------------------------------------
// 文件名:      AISystem.h
// 内  容:      AI系统
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "Fsgame/Define/header.h"
#include "SDK/rapidxml/rapidxml.hpp"
#include "AIDefine.h"
#include "Template/AITemplateBase.h"
#include "Rule/AIRuleBase.h"
#include "FsGame/SkillModule/FightModule.h"
#include "FsGame/SystemFunctionModule/MotionModule.h"
#include "FsGame/SkillModule/Skillmodule.h"
#include "AIHeartBeat.h"
#include "AIFunction.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/SystemFunctionModule/DropModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
//#include "FsGame/SocialSystemModule/CopyOffLineModule.h"
#include "utils/geometry_func.h"
#include <time.h>

using namespace rapidxml;

class AIHeartBeat;
class AIBasicAction;
class AISystem : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);
	// 关闭
	virtual bool Shut(IKernel* pKernel);

public:
    //对象巡逻
    static int OnPatrol(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//获取模板
	AITemplateBase &GetAITemlate(IKernel* pKernel, const PERSISTID& self);
	//获取护送点
	static bool GetConvoyPoint(IKernel* pKernel, const PERSISTID& self, const char* npcId);
	//获取拦截怪
	static bool GetInterceptNpc(IKernel* pKernel, const PERSISTID& self, const char* npcId, int step, CVarList& args);
	//获取怪物的路径点
	static bool GetPatrolNpcPoint(IKernel* pKernel, const PERSISTID& self);
	//设置怪物技能
	static bool InsertNpcSkill(IKernel* pKernel, const PERSISTID& self, const char* skillStr);
	//获得神秘宝箱掉落	
	static bool	GetBoxDeadDrop(const std::string &configId, DropWeight &drop);
	//自主触发一次
	static int OnAutoSpring(IKernel* pKernel, const PERSISTID& self);
	// 基本行为结束
	static int OnActionEnd(IKernel* pKernel, const PERSISTID& self, int msg);
	// 创建小队
	static bool CreateBattleTeam(IKernel* pKernel, const PERSISTID& self, int iNation, int nTeamId, int iLevel);
	//设置行走路径点
	bool SetBattleNpcPath(IKernel* pKernel, const PERSISTID& self, int nCamp);
	//计算其他NPC偏移位置
	bool GetOtherNpcPos(IKernel* pKernel, float PosX, float PosZ, int iIndex, float& OtherPosX, float& OtherPosZ);
	//设定整个队伍状态
	int SetBattleNpcTeamStatus(IKernel* pKernel, const PERSISTID& self, int iStatus);
    //获取技能配置信息
    static bool GetSkillConfig(IKernel* pKernel, const PERSISTID& self, const char* skillID, NpcSkill& npcSkill);
    //清除战斗信息
    static bool ClearFight(IKernel* pKernel, const PERSISTID& self);
	// 技能包是否存在
	static bool IsSkillPkgExist(const char* skill_pkg);
	//获得npc普通技能
	static const char* GetNpcNomalSkill (const char* skillCfg);
	// 获取触发范围
	static float GetSpringRange(IGameObj* pSelfObj);

	// 取得等级属性包
	static const NpcLevelPackage* GetNpcLevelPackage(const char *p_config_id, const int n_level);

	// 取得Npc属性包
	static const int GetNpcPackage(IGameObj* pSelfObj);
//静态方法
public:

    //取得AISystem实体指针
    static AISystem *Instance()
    {
        if(m_pAISystem == NULL)
        {
            extend_warning(LOG_CRITICAL, "AISystem模块还没有初始化就使用！");
            assert(0);
            m_pAISystem = new AISystem();
            return m_pAISystem;
        }
        return m_pAISystem;
    }

//注删的静态回调函数
private:
    //对象进入场景时触发
    static int OnEntry(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //对象被触发时触发
    static int OnSpring(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	//对象被销毁
	static int OnDestroy(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //玩家复活
    static int OnPlayerRelive(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //被伤害
    static int OnBeDamaged(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //对象被杀死时触发
    static int OnBeKilled(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //对象复活
    static int OnRelive(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 实用技能后移动
	static int OnFightOverMove(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    // 带参数的创建
    static int OnCreateArgs(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	//拷贝玩家数据完成
	static int OnCopyDataComplete(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);
private:
    //初始化模板
    void InitTemplateVector(IKernel *pKernel);
    //注册模板
    bool RegistTemplate(IKernel *pKernel, AITemplateBase *pTemplateBase);
	//怪物技能使用权值配置
	static bool LoadNPCSkill(IKernel* pKernel);
    //怪物技能包
    static bool LoadNpcSkillPackage(IKernel *pKernel);
	//加载巡逻路径
	static bool LoadPatrolPath(IKernel* pKernel);
	//加载护送NPC路径
	static bool LoadConvoyPath(IKernel* pKernel);
	//加载拦截怪物
	static bool LoadInterceptInfo(IKernel* pKernel);
	//国战NPC配置
	static bool LoadBattleNpcBornIndex(IKernel* pKernel);
	//行走路径配置
	static bool LoadBattleNpcMoveType(IKernel* pKernel);
	//读取创建小队配置
	static bool LoadBattleNpcTeam(IKernel* pKernel);
	//读取小队队形配置
	static bool LoadBattleTeamFormation(IKernel* pKernel);
	//神秘宝箱配置
	static bool LoadBoxConfig(IKernel *pKernel);
	// 等级属性包配置
	static bool LoadLevelPackage(IKernel *pKernel);

	//在目标点范围内寻找一个随机点
	static bool RandomFindAroundPos(float& fOutX, float& fOutZ);
	// 查询小队配置
	const BattleTeam* QueryTeamConfig(int nTeamId);
	// 查询小队队形
	const BattleTeamFormation* QueryTeamFormation(const int scene_id);

public:
	//战斗系统
	static FightModule* m_pFightModule;
	//移动
	static MotionModule* m_pMotionModule;
	//技能
	static SkillModule* m_pSkillModule;
	//掉落
	static DropModule* m_pDropModule;
	//背包
	static ContainerModule* m_pContainerModule;
	//经验
	static LevelModule* m_pLevelModule;
	//金钱
	static CapitalModule* m_pCapitalModule;

#ifndef FSROOMLOGIC_EXPORTS
	//离线模块
	//static CopyOffLineModule* m_pCopyOffLineModule;
#endif // FSROOMLOGIC_EXPORTS

private:

    //AI本身
    static AISystem *m_pAISystem;

    //找不到对应模板时用基础模板
    static AITemplateBase s_AITemplateBase;

	//所有的模版管理
	std::vector< AITemplateBase* >  m_vctAITemplate;
	//技能配置使用权值
	std::map<std::string,NpcSkill> m_mapSkillConfig;
    //NPC技能配包配置
    std::map<std::string, NpcSkillPackage> m_mapSkillPackage;
	//护送配置
	std::map<std::string, std::vector<Point> > m_mapConvoyStep;
	//拦截配置
	std::map<std::string,std::vector<Intercept>> m_mapIntercept;
	//巡逻NPC配置
	std::map<std::string, std::vector<PatrolPos>> m_mapPatrolInfo;
	//国战NPC配置
	std::map<int, BattleNpcStruct> m_mapBattleNpcInfo;
	//行走方式配置
	std::map<int, MoveTypeDef> m_mapMoveTypeInfo;
	// 小队配置
	BattleTeamVec m_vecBattleTeamConfig;
	// 队形配置
	BattleTeamFormationVec m_BattleTeamFormationVec;
	//神秘宝箱
	MAP_BOXWEIGHT	m_mapBoxInfo;
	// 等级属性包
	NpcLevelPackageVec m_NpcLevelPackageVec;
};
