//--------------------------------------------------------------------
// 文件名:      AISystem.cpp
// 内  容:      AI系统
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include <algorithm>
#include "AISystem.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "Rule/AIRuleManage.h"

//template
#include "FsGame/NpcBaseModule/AI/Template/AITNormalFight.h"
#include "FsGame/NpcBaseModule/AI/Template/AITBoss.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/CommonModule/EnvirValueModule.h"
#include "Define/PlayerBaseDefine.h"
#include "Interface/FightInterface.h"

AISystem* AISystem::m_pAISystem = NULL;
FightModule * AISystem::m_pFightModule = NULL;
SkillModule * AISystem::m_pSkillModule = NULL;
MotionModule * AISystem::m_pMotionModule = NULL;
DropModule * AISystem::m_pDropModule = NULL;
ContainerModule * AISystem::m_pContainerModule = NULL;
LevelModule * AISystem::m_pLevelModule = NULL;
CapitalModule * AISystem::m_pCapitalModule = NULL;

#ifndef FSROOMLOGIC_EXPORTS
//CopyOffLineModule* AISystem::m_pCopyOffLineModule =NULL;
#endif // FSROOMLOGIC_EXPORTS

static const float MAX_SPRING_RANGE = 40.0f; // 最大触发范围

// 创建NPC组队
int nx_init_battle_npc(void *state)
{
	// 获取核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_init_battle_npc, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_init_battle_npc, 1);
	// 获取参数
	PERSISTID scene = pKernel->GetScene();
	int nTeamId = pKernel->LuaToInt(state, 1);

//	AISystem::CreateBattleTeam(pKernel, scene, 1, nTeamId, 10);
	return 1;
}

bool AISystem::Init(IKernel* pKernel)
{
	m_pAISystem = this;
	m_pFightModule = (FightModule*)pKernel->GetLogicModule("FightModule");
	m_pSkillModule = (SkillModule*)pKernel->GetLogicModule("SkillModule");
	m_pMotionModule = (MotionModule*)pKernel->GetLogicModule("MotionModule");
	m_pDropModule = (DropModule*)pKernel->GetLogicModule("DropModule");
	m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pLevelModule = (LevelModule*)pKernel->GetLogicModule("LevelModule");
	m_pCapitalModule = (CapitalModule*)pKernel->GetLogicModule("CapitalModule");

	Assert(m_pAISystem != NULL && m_pFightModule !=NULL && m_pSkillModule != NULL && m_pMotionModule != NULL 
		&& m_pDropModule != NULL && m_pContainerModule != NULL && m_pLevelModule != NULL && m_pCapitalModule != NULL);

#ifndef FSROOMLOGIC_EXPORTS
// 	m_pCopyOffLineModule = (CopyOffLineModule*)pKernel->GetLogicModule("CopyOffLineModule");
// 	Assert(m_pCopyOffLineModule != NULL);
#endif // FSROOMLOGIC_EXPORTS

    pKernel->AddEventCallback("NormalNpc", "OnCreateArgs", AISystem::OnCreateArgs);
	pKernel->AddEventCallback("FunctionNpc", "OnCreateArgs", AISystem::OnCreateArgs);
	pKernel->AddEventCallback("NormalNpc", "OnEntry", OnEntry);
	pKernel->AddEventCallback("NormalNpc", "OnSpring", OnSpring);
	pKernel->AddEventCallback("NormalNpc", "OnDestroy", OnDestroy);
	
	pKernel->AddIntCommandHook("NormalNpc", COMMAND_BEDAMAGE, AISystem::OnBeDamaged);
	pKernel->AddIntCommandHook("NormalNpc", COMMAND_BEKILL, AISystem::OnBeKilled);
// 	pKernel->AddIntCommandHook("player", COMMAND_RELIVE, AISystem::OnPlayerRelive);
// 	pKernel->AddIntCommandHook("NormalNpc", COMMAND_RELIVE, AISystem::OnRelive);
// 
// 	pKernel->AddIntCommandHook("NormalNpc", COMMAND_NPC_STIFF_OVER, AISystem::OnFightOverMove);
// 
// 	//拷贝玩家数据完成
// 	pKernel->AddIntCommandHook("BattleNpc", COMMAND_OFFLINE_DATA_COPY_FINISH, AISystem::OnCopyDataComplete);

	//初始化模板
	InitTemplateVector(pKernel);
	//加载NPC技能权值
	LoadNPCSkill(pKernel);
    //加载NPC技能包
    LoadNpcSkillPackage(pKernel);
	//加载护送NPC路径
	LoadConvoyPath(pKernel);
	//拦截怪物配置
	LoadInterceptInfo(pKernel);
	//加载巡逻路径
	LoadPatrolPath(pKernel);
	//加载国战NPC
	LoadBattleNpcBornIndex(pKernel);
	//加载移动路点
	LoadBattleNpcMoveType(pKernel);
	// 加载小队数据
	LoadBattleNpcTeam(pKernel);
	// 加载小队队形
	LoadBattleTeamFormation(pKernel);
	//加载神秘宝箱
	LoadBoxConfig(pKernel);
	// 加载等级属性包
	LoadLevelPackage(pKernel);

	//战斗心跳
	DECL_HEARTBEAT(AIHeartBeat::HB_AIFighting);

	//巡逻心跳
	DECL_HEARTBEAT(AIHeartBeat::HB_AIPatrol);

	//检测战斗心跳
	DECL_HEARTBEAT(AIHeartBeat::HB_AIEndFight);
	// 护送Npc定位
// 	DECL_HEARTBEAT(AIHeartBeat::HB_LocationConvyNpc);
// 	// 延迟护送
// 	DECL_HEARTBEAT(AIHeartBeat::HB_DelayConvoy);

	// 押送Npc跟随心跳
	/*DECL_HEARTBEAT(AIHeartBeat::HB_EscortNpcFollow);*/

	//出生心跳
	DECL_HEARTBEAT(AIHeartBeat::HB_AIBornEnd);

	//转身心跳
	DECL_HEARTBEAT(AIHeartBeat::HB_AIRatate);

	//查找
	DECL_HEARTBEAT(AIHeartBeat::HB_AIFind);

	//拷贝玩家
	DECL_HEARTBEAT(AIHeartBeat::HB_AIClonePlayer);

    //普通怪回出生点超时保护
    DECL_HEARTBEAT(AIHeartBeat::HB_BACK_BORN_TIMEOUT);

    //演武国战士兵搜寻心跳
//     DECL_HEARTBEAT(AIHeartBeat::HB_Nation_Fight_Find);
// 
//     //演武国战设尸体不可见
//     DECL_HEARTBEAT(AIHeartBeat::HB_Nation_Dead_Invisible);
	//扇形npc状态改变回调
	//DECL_CRITICAL(AISectorSpringRangeNpc::c_OnAICurStateChange);

	//
	//DECL_HEARTBEAT(AISectorSpringRangeNpc::HB_ScanAround);

	DECL_HEARTBEAT(AIHeartBeat::HB_EndEscape);

	DECL_HEARTBEAT(AIHeartBeat::HB_CheckEscape);

	DECL_LUA_EXT(nx_init_battle_npc);

	//初始化规则管理器
	AIRuleManage::Init(pKernel);
	AIFunction::Init(pKernel);
	return true;
}

// 关闭
bool AISystem::Shut(IKernel* pKernel)
{
	m_pAISystem->m_mapSkillConfig.clear();
	m_pAISystem->m_mapConvoyStep.clear();
	m_pAISystem->m_mapIntercept.clear();
	m_pAISystem->m_mapPatrolInfo.clear();
	m_pAISystem->m_mapBattleNpcInfo.clear();
	m_pAISystem->m_mapMoveTypeInfo.clear();
	AIFunction::Shut(pKernel);
	return true;
}

//初始化模板
void AISystem::InitTemplateVector(IKernel *pKernel)
{
    LoopBeginCheck(a);
	for (int i = 0; i < AI_TEMP_MAX; ++i)
	{
        LoopDoCheck(a);
		m_vctAITemplate.push_back(NULL);
	}
	//注册普通NPC模板
	RegistTemplate(pKernel, new AITemplateBase);
	//注册普通战斗NPC模板
	RegistTemplate(pKernel, new AITNormalFight);
	//BossAI
// 	RegistTemplate(pKernel, new AITBoss);
// 	//神秘宝箱
// 	RegistTemplate(pKernel, new AITBox);
// 	//宠物NPC模板
// 	RegistTemplate(pKernel, new AITPet);

#ifndef FSROOMLOGIC_EXPORTS
	//注册护送NPC
// 	RegistTemplate(pKernel, new AITConvoy);
// 	//注册拦截NPC
// 	RegistTemplate(pKernel, new AITIntercept);
// 	//注册护送NPC
// 	RegistTemplate(pKernel, new AITEscort);
// 	//注册巡逻NPC
// 	RegistTemplate(pKernel, new AITPatrolOnlyNpc);
// 	//注册国战队长
// 	RegistTemplate(pKernel, new AITBattleLeadNpc);
// 	//注册小队成员
// 	RegistTemplate(pKernel, new AITBattleNpc);
// 	//离线玩家拦截
// 	RegistTemplate(pKernel, new AITInterceptPlayer);
// 	//离线挂机
// 	//RegistTemplate(pKernel, new AITOutLinePlayer);
//     //国家守卫将军
//     RegistTemplate(pKernel, new AITNationGuard);
//     // 竞技场AI
//     RegistTemplate(pKernel, new AITArenaNpc);
//     //注册巡逻野怪NPC
//     RegistTemplate(pKernel, new AITPatrolMonsterNpc);
// 	//扇形响应区npc
// 	RegistTemplate(pKernel,	new AISectorSpringRangeNpc );
// 	//秘境防守npc
// 	RegistTemplate(pKernel, new	AISecretSceneGuardNpc);
// 	//被保护目标
// 	RegistTemplate(pKernel, new AISecretProgectObjNpc);
// 	//逃跑npc
// 	RegistTemplate(pKernel, new AIEscapeNpc);
// 	//战斗巡逻ncp
// 	RegistTemplate(pKernel ,new AIFightPatrolNpc);
// 	//阵营战buffer Npc
// 	RegistTemplate(pKernel, new AITBattleBuffNpc);
// 	//死亡生成陷阱 Npc
// 	RegistTemplate(pKernel, new AIDeadGenTrapNpc);
// 	//体验场景boss
// 	RegistTemplate(pKernel, new AIStartSceneBoss);
// 	//体验场景boss血量控制
// 	RegistTemplate(pKernel, new AIStartSceneControlHpBoss);
// 	//边境箭塔
// 	RegistTemplate(pKernel, new AITowerNpc);
// 	//护送boss
// 	RegistTemplate(pKernel, new AIInvadeBossNpc);
    
#endif // FSROOMLOGIC_EXPORTS
}

// 注册模板
bool AISystem::RegistTemplate(IKernel *pKernel, AITemplateBase *pTemplateBase)
{
	if (pTemplateBase->GetTemplateType() < 0 || pTemplateBase->GetTemplateType() >= (int)m_vctAITemplate.size())
	{
		delete pTemplateBase;
		extend_warning(pKernel,"加载AI模板失败");
        assert(0);
		return false;
	}
	if (m_vctAITemplate[pTemplateBase->GetTemplateType()] == NULL)
	{
        pTemplateBase->Init(pKernel);
		m_vctAITemplate[pTemplateBase->GetTemplateType()] = pTemplateBase;
	}
    else
    {
        extend_warning(pKernel, "重复注册或类型定义");
        assert(0);
    }

	return true;
}

//找不到对应模板时用基础模板
AITemplateBase AISystem::s_AITemplateBase;

//获得AI模板对象,找不到对应模板时用返回基础模板
AITemplateBase &AISystem::GetAITemlate(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		assert(0);
		return s_AITemplateBase;
	}

	if (!pSelfObj->FindAttr("AITemplate"))
	{
		return s_AITemplateBase;
	}

	//取得NPC配设的AI模板类型
	AI_TEMPLATE_TYPE AITemplateType = (AI_TEMPLATE_TYPE)pSelfObj->QueryInt("AITemplate");

	if (AITemplateType < 0 || AITemplateType >= AI_TEMP_MAX || AITemplateType >= (int)m_vctAITemplate.size())
    {
        assert(0);
        return s_AITemplateBase;
    }

	return *m_vctAITemplate[AITemplateType];
}

// 进入场景
int AISystem::OnEntry(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args)
{
	if (pKernel->Type(sender) == TYPE_SCENE)
	{
		IGameObj* pSelfObj = pKernel->GetGameObj(self);
		if (pSelfObj == NULL)
		{
			return 0;
		}

		//CampaignGuildChaosFight::SetNpcLevelPackage(pKernel, self);
//#ifndef FSROOMLOGIC_EXPORTS
//		if (pKernel->GetServerId() == 7440001 || pKernel->GetServerId() == 7440005)
//		{
//			//////////////////////////////////////////////////////////////////////////
//			// 看这里！！Debug用，记得注释		//
//			//////////////////////////////////////////////////////////////////////////
//			AIHeartBeat::HB_EscortNpcFollow(pKernel, self, 0);
//			ADD_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_EscortNpcFollow", 10000);
//			//////////////////////////////////////////////////////////////////////////
//			// 看这里！！Debug用，记得注释		//
//			//////////////////////////////////////////////////////////////////////////
//		}
//		else
//		{
//#if defined _DEBUG
//			//////////////////////////////////////////////////////////////////////////
//			// 看这里！！Debug用，记得注释		//
//			//////////////////////////////////////////////////////////////////////////
//			AIHeartBeat::HB_EscortNpcFollow(pKernel, self, 0);
//			ADD_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_EscortNpcFollow", 10000);
//			//////////////////////////////////////////////////////////////////////////
//			// 看这里！！Debug用，记得注释		//
//			//////////////////////////////////////////////////////////////////////////
//#endif
//		}
//#endif

		// 设置行走高度
		float fWalkStep = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_NPC_WALK_STEP_DEFAULT);
		pSelfObj->SetFloat("WalkStep", fWalkStep);

		// 记录出生时的高度
		//pSelfObj->SetFloat(FIELD_PROP_BORN_HEIGHT, pSelfObj->GetPosiY());

        AISystem::Instance()->GetAITemlate(pKernel, self).OnBorn(pKernel, self, sender, args);
	}
	return 0;
}

//被触发
int AISystem::OnSpring(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args)
{
	if (pKernel->Exists(self) && pKernel->Exists(sender))
	{
        AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_SPRING, self, sender, args);
	}
	return 0;
}

//对象被销毁
int AISystem::OnDestroy(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	if (pKernel->Exists(self) && pKernel->Exists(sender))
	{
		AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_DESTORY, self, sender, args);
	}
	return 0;
}


//被伤害
int AISystem::OnBeDamaged(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
    AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_BE_DAMAGED, self, sender, args);
	return 0;
}

//被击杀
int AISystem::OnBeKilled(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
    AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_ONDEAD, self, sender, args);

	return 0;
}

//开始巡逻
int AISystem::OnPatrol(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
    AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_PATROL, self, sender, args);
	return 0;
}

//复活事件
int AISystem::OnRelive(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
    AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_RELIVE, self, sender, args);
	return 0;
}

// 实用技能后移动
int AISystem::OnFightOverMove(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	// 随机移动行为
	IGameObj* pAttacker = pKernel->GetGameObj(sender);
	if (NULL == pAttacker)
	{
		return 0;
	}

	// 计算是否移动
	int nFightMoveRate = pAttacker->QueryInt(FIELD_PROP_FIGHT_MOVE_RATE);
	int nRandomRate = util_random_int(100);
	if (nRandomRate >= nFightMoveRate)
	{
		return 0; 
	}

	// 移动的的距离
	float nDis = pAttacker->QueryFloat(FIELD_PROP_FIGHT_MOVE_DIS);
	PERSISTID target = pAttacker->QueryObject(FIELD_PROP_AITARGET_OBEJCT);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (NULL == pTargetObj)
	{
		return 0;
	}
//	float fDesX, fDesY, fDesZ;
	// 寻找移动的坐标
// 	if (SkillEventExecuteUtil::RandomFindAroundPos(pKernel, pTargetObj, 10, nDis, nDis, fDesX, fDesY, fDesZ))
// 	{
// 		AIFunction::StartPatrolToPoint(pKernel, sender, fDesX, fDesZ, false);
// 	}

	// 逃跑行为
	AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_ESCAPE, self, sender, args);
	return 0;
}

//玩家复活
int AISystem::OnPlayerRelive(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args)
{
	CVarList temp;
	CVarList result;
	pKernel->GetAroundList(self, 100.0f, TYPE_NPC, 20, result);
    LoopBeginCheck(b);
	for(int i = 0; i < static_cast<int>(result.GetCount()); ++i)
	{
        LoopDoCheck(b);
		PERSISTID npc = result.ObjectVal(i);
        IGameObj *pNpc = pKernel->GetGameObj(npc);
        if(pNpc == NULL)
        {
            continue;
        }
		if (pNpc->QueryInt("NpcType") >= NPC_TYPE_FUNCTION)
		{
			continue;
		}
		if ( pKernel->Exists(npc) )
		{
			if(pNpc->QueryInt("Dead") < 1)
				OnSpring(pKernel, npc, self, temp);
		}
	}

	return 0;
}

// 自主触发一次
int AISystem::OnAutoSpring(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//检测周围玩家进行一轮触发
	float SpringRange = AISystem::GetSpringRange(pSelfObj);;
	//查看周围的玩家
	CVarList arg_list;
	CVarList temp;
	pKernel->GetAroundList(self, SpringRange, TYPE_PLAYER|TYPE_NPC, 40, arg_list);
    LoopBeginCheck(c);
	for ( size_t i = 0; i < arg_list.GetCount(); ++i )
	{
        LoopDoCheck(c);
		PERSISTID player = arg_list.ObjectVal(i);
		if ( pKernel->Exists(player) )
		{
			OnSpring(pKernel, self, player, temp);
		}
	}
	return 0;
}

//AI基本行为结束后处理
int	AISystem::OnActionEnd(IKernel* pKernel, const PERSISTID& self, int msg)
{	
    AISystem::Instance()->GetAITemlate(pKernel,self).OnBasicActionMsg(pKernel, self, msg);
	return 0;
}

bool AISystem::ClearFight(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return false;
    }

    //清除攻击对象
    pSelf->SetObject("AITargetObejct", PERSISTID());
    pSelf->SetInt("LastAttackTime", 0);

    //删除战斗的心跳
    if (pKernel->FindHeartBeat(self,"AIHeartBeat::HB_AIFighting"))
    {
        pKernel->RemoveHeartBeat(self,"AIHeartBeat::HB_AIFighting");
    }

    //清除技能信息
    IRecord* record = pSelf->GetRecord(NPC_SKILL_REC);
    if (record == NULL)
    {
        return false;
    }
    int rows = record->GetRows();
    LoopBeginCheck(d);
    for (int i = 0; i < rows; i++)
    {
        LoopDoCheck(d);
        record->SetInt(i, BOSS_SKILL_VALUE_CUR, 0);//当前权值重置
    }

    return true;
}

// 技能包是否存在
bool AISystem::IsSkillPkgExist(const char* skill_pkg)
{
	std::map<std::string, NpcSkillPackage>::iterator pakIter = AISystem::m_pAISystem->m_mapSkillPackage.find(skill_pkg);
	if (pakIter == m_pAISystem->m_mapSkillPackage.end())
	{
		return false;
	}

	return true;
}
 const char*  AISystem::GetNpcNomalSkill(const char* skillCfg)
{
	std::map<std::string, NpcSkillPackage>::iterator pakIter = AISystem::m_pAISystem->m_mapSkillPackage.find(skillCfg);
	if (pakIter != m_pAISystem->m_mapSkillPackage.end())
	{
		return pakIter->second.normalSkill.c_str();
	}

	return NULL;
}

// 获取触发范围
 float AISystem::GetSpringRange(IGameObj* pSelfObj)
 {
	 if (NULL == pSelfObj || pSelfObj->GetClassType() != TYPE_NPC)
	 {
		 return 0.0f;
	 }

	 float fSpringRange = pSelfObj->QueryFloat("SpringRange");
	 if (fSpringRange >= MAX_SPRING_RANGE)
	 {
		 fSpringRange = MAX_SPRING_RANGE;
	 }
	 return fSpringRange;
 }

 // 取得等级属性包
 const NpcLevelPackage* AISystem::GetNpcLevelPackage(const char *p_config_id, const int n_level)
 {
	 if (StringUtil::CharIsNull(p_config_id)
		 || n_level <= 0)
	 {
		 return NULL;
	 }

	 struct LevelPackageFinder 
	 {
		 LevelPackageFinder(const char *_s_key, const int _n_value) : 
		 s_key(_s_key), n_value(_n_value)
		 {}

		 std::string s_key;
		 int n_value;

		 bool operator () (const NpcLevelPackage& cfg) const
		 {
			 return (strcmp(cfg.s_config_id.c_str(), s_key.c_str()) == 0 && 
				 (n_value >= cfg.n_lower_level && n_value <= cfg.n_upper_level));
		 }
	 };

	 NpcLevelPackageVec::const_iterator find_it = find_if(
		 m_pAISystem->m_NpcLevelPackageVec.begin(),
		 m_pAISystem->m_NpcLevelPackageVec.end(),
		 LevelPackageFinder(p_config_id, n_level));
	 if (find_it == m_pAISystem->m_NpcLevelPackageVec.end())
	 {
		 return NULL;
	 }

	 return &(*find_it);
 }

 // 取得Npc属性包
 const int AISystem::GetNpcPackage(IGameObj* pSelfObj)
 {
	 if (NULL == pSelfObj)
	 {
		 return 0;
	 }

	 // 属性包等级
	 const int n_npc_package_lvl = pSelfObj->QueryInt(FIELD_PROP_NPC_PACKAGE_LEVEL);
	 if (n_npc_package_lvl > 0)
	 {
		 // 取包
		 const NpcLevelPackage *cfg = GetNpcLevelPackage(pSelfObj->GetConfig(), n_npc_package_lvl);
		 if (NULL == cfg)
		 {
			 return pSelfObj->QueryInt(FIELD_PROP_NPC_BASE_PACKAGE);
		 }
		 else
		 {
			 return cfg->n_package_id;
		 }
	 }
	 else
	 {
		 return pSelfObj->QueryInt(FIELD_PROP_NPC_BASE_PACKAGE);
	 } 
 }

 //拷贝玩家数据完成
int AISystem::OnCopyDataComplete(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!pSelfObj->FindAttr(FIELD_PROP_AITEMPLATE))
	{
		return 0;
	}

	int nAITemplate = pSelfObj->QueryInt(FIELD_PROP_AITEMPLATE);
	if (AI_TEMP_BATTLENPC != nAITemplate && AI_TEMP_ARENA_NPC != nAITemplate && AI_TEMP_INTERCEPTPLAYER != nAITemplate)
	{
		return 0;
	}

// 	IRecord* pSkillRec = pSelfObj->GetRecord(FIELD_RECORD_BATTLE_SKILL_REC);
// 	if (NULL == pSkillRec)
// 	{
// 		return 0;
// 	}
// 
// 	pSkillRec->ClearRow();
// #ifndef FSROOMLOGIC_EXPORTS
// // 	if (NULL != OffLineModule::m_pOffLineModule)
// // 	{
// // 		OffLineModule::m_pOffLineModule->AddSkillToRec(pKernel, pSelfObj, pSkillRec);
// // 	}
// #endif
// 
// 	// 预创建玩家的爵位等级,肯定是0级
// 	int nPeerageLevel = pSelfObj->QueryInt(FIELD_PROP_PEERAGE_LEVEL);
// 	// 拷贝预创建玩家数据的阵营战npc增加一个实力buff
// 	if (AI_TEMP_BATTLENPC == nAITemplate && 0 == nPeerageLevel)
// 	{
// 		const char* strStrenBuff = EnvirValueModule::EnvirQueryString(ENV_VALUE_BATTLE_NPC_STREN_BUFF);
// 		FightInterfaceInstance->AddBuffer(pKernel, self, self, strStrenBuff);
// 	}
	return 0;
}