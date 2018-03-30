//--------------------------------------------------------------------
// 文件名:      AITemplateBase.cpp
// 内  容:      NPC基础模板
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "../AISystem.h"
#include "../AIDefine.h"
#include "../../AI/Rule/AIRuleManage.h"
#include "AITemplateBase.h"
#include "../Rule/AIRuleBase.h"
#include "utils/util_func.h"
#include "../AIFunction.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/CommonModule/PropRefreshModule.h"
#include "FsGame/Interface/FightInterface.h"
#include <time.h>
#include "FsGame/Define/GameDefine.h"
//#include "FsGame/SocialSystemModule/BattleTeamModule.h"
#include "utils/string_util.h"
#include "utils/custom_func.h"
//#include "FsGame/SceneBaseModule/SecretSceneModule.h"
#include "FsGame/SystemFunctionModule/MotionModule.h"
//#include "FsGame/Define/DeadManagerDefine.h"
#include "FsGame/CommonModule/EnvirValueModule.h"
//#include "../../BossRefreshModule.h"
#include "Define/ServerCustomDefine.h"

// 临时用下
static const char* DEFINE_STRING_SCENE_NAME = "scene_";

//构造函数
AITemplateBase::AITemplateBase()
{
	m_templateType = AI_TEMP_BASE;

	RegistEventRule(AI_RULE_SPRING, AIRS_SPRING_BASE);
	//选择攻击对象AI规则
	RegistEventRule(AI_RULE_SEL_TARTGET, AIRS_SEL_TARGET_BASE); 
	RegistEventRule(AI_RULE_BE_DAMAGED, AIRS_BE_DAMAGED_BASE);
	RegistEventRule(AI_RULE_PATROL, AIRS_PATROL_BASE);
}

//初始化
bool AITemplateBase::Init(IKernel* pKernel)
{
    return true;
}

//析构函数
AITemplateBase::~AITemplateBase()
{
	m_EventRuleVct.clear();
}

// 获得本模板类型
const AI_TEMPLATE_TYPE AITemplateBase::GetTemplateType()
{
    return m_templateType;
}

//注册这个模板处理规则
bool AITemplateBase::RegistEventRule(AI_RULE_TYPE nEvent, int nRule)
{
    if(m_EventRuleVct.empty())
    {
        m_EventRuleVct.resize(AI_RULE_MAX, -1);
    }

    if (nEvent < 0 || nEvent >= AI_RULE_MAX)
    {
        return false;
    }

    m_EventRuleVct[nEvent] = nRule;

    return true;
}


//模版处理控制,执行AI
int AITemplateBase::OnProcess(IKernel *pKernel, AI_RULE_TYPE ruleType, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return AI_RT_IGNORE;
	}
    
    //检查有没有这个处理规则
    if(ruleType < 0 || ruleType >= AI_RULE_MAX)
    {
        return AI_RT_IGNORE;
    }

    //检查有没有注册执行逻辑AI
    if(m_EventRuleVct[ruleType] == -1)
    {
        return AI_RT_IGNORE;
    }

    //执行AI规则
    return AIRuleManage::GetRuleItem(ruleType, m_EventRuleVct[ruleType]).DoRule(pKernel, self, sender, args, *this);

}

//每次移动完成后回调
int AITemplateBase::OnBasicActionMsg(IKernel * pKernel , const PERSISTID & self , int msg , const IVarList & args)
{
	if (!pKernel->Exists(self))
		return AI_RT_IGNORE;

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_IGNORE;
    }

	//已经死亡
	if (pSelf->QueryInt("Dead") > 0)
		return AI_RT_IGNORE;

	int cur_state = pSelf->QueryInt("CurState");
	if (cur_state == AI_STATE_PATROL)
	{
		pSelf->SetDataInt("PatrolRestTime", 0);
		pSelf->SetInt("SubState",AI_PATROL_WAIT);
	}
	else if (cur_state == AI_STATE_BACK)
	{
        //回出生点完成，清除攻击表
		IRecord* pAttackerRecord = pSelf->GetRecord("AttackerList");
		if (pAttackerRecord != NULL)
		{
			pAttackerRecord->ClearRow();
		}
        
        //是否要巡逻
		int iBornStand = pSelf->QueryInt("BornStand");
		if (iBornStand != 1)
		{
			BeginPatrol(pKernel,self);
		}
		else
		{
            //设置为出生状态
		    ChangeState(pKernel, self, AI_STATE_BORN);
	    }

        //重新恢复血量
		int nCantRestore = pSelf->QueryInt(FIELD_PROP_CANT_RESTORE_HP);
		if (0 == nCantRestore)
		{
			int64_t max_hp = pSelf->QueryInt64("MaxHP");
			pSelf->SetInt64("HP", max_hp);
		}
		//添加心跳处理
    	ADD_COUNT_BEAT(pKernel, self, "AIHeartBeat::HB_AIRatate", 1000, 1);
	}

	return AI_RT_SUCCESS;
}
//改变AI状态
int AITemplateBase::ChangeState(IKernel * pKernel, const PERSISTID & self, int state) const
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	pSelfObj->SetInt("CurState", state);
	return AI_RT_SUCCESS;
}

//设置出生
int AITemplateBase::OnBorn(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	if ( !pKernel->Exists(self) )
	{
		return 0;
	}
	
	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if ( pSelfObj == NULL )
	{
		return 0;
	}

    //出生公告
    BornNotice(pKernel, self);

	//静态NPC不能移动
	if ( pSelfObj->QueryInt("NpcType") >= NPC_TYPE_FUNCTION )
	{
		return 1;
	}
    
    //出生时默认CurState = 0;
	if (pSelfObj->QueryInt("CurState") == AI_STATE_BORN)
	{
        //设置出生初始化属性包
		AITemplateBase::InitPropAfterBorn( pKernel, self );
	
		//没有移动速度，就不会设置寻路
		len_t fWalkSpeed = pSelfObj->QueryFloat("RunSpeed");
		int iBornStand = pSelfObj->QueryInt("BornStand");
		if (!FloatEqual( fWalkSpeed, 0.0f) && iBornStand == 0)
		{
            //生成巡逻路径
			AITemplateBase::InitRandomPatrolPath(pKernel, self);
		}
		
        //初始化怪物初始技能
		AITemplateBase::InitSkillAfterBorn(pKernel, self);

        //设置碰撞相关
		pSelfObj->SetInt("PathThrough", 0);
		pSelfObj->SetInt("PathGrid", 1);

        //如果追击范围少于触发范围，追击范围就等于触发范围.
        if(pSelfObj->FindAttr("ChaseRange"))
        {
            float chaseRange = pSelfObj->QueryFloat("ChaseRange");
			float springRange = AISystem::GetSpringRange(pSelfObj);
            if(chaseRange <= springRange)
            {
                pSelfObj->SetFloat("ChaseRange", springRange);
            }
        }

		//设置为满血
		int64_t max_hp = pSelfObj->QueryInt64("MaxHP");
		pSelfObj->SetInt64("HP", max_hp);

        // 出生时间(主要用于出生时播发一个动后才开始执行AI)
        int nBornTime = pSelfObj->QueryInt("BornTime");
        if (nBornTime > 0)
        {
            ADD_COUNT_BEAT(pKernel, self, "AIHeartBeat::HB_AIBornEnd", nBornTime, 1);
        }
        else
        {
            BeginPatrol(pKernel,self);
        }
	}
	
	return AI_RT_SUCCESS;
}

//开始巡逻
int AITemplateBase::BeginPatrol(IKernel * pKernel, const PERSISTID & self) const
{
    IGameObj * pSelfObj = pKernel->GetGameObj(self);
    if ( pSelfObj == NULL )
    {
        return 0;
    }
    
	// 没有巡逻表的不会巡逻
    IRecord *pPatrolPointRec = pSelfObj->GetRecord("PatrolPointRec");
    if(pPatrolPointRec == NULL)
    {
        return 0;
    }
	if(pPatrolPointRec->GetRows() == 0)
	{
		return 0;
	}
	//进入巡逻状态
	ChangeState(pKernel, self, AI_STATE_PATROL);

	int nCantRestore = pSelfObj->QueryInt(FIELD_PROP_CANT_RESTORE_HP);
	if (0 == nCantRestore)
	{
		//重新恢复血量
		int64_t max_hp = pSelfObj->QueryInt64("MaxHP");
		pSelfObj->SetInt64("HP", max_hp);
	}
	
	//初始化当前巡逻步骤
	if (!pSelfObj->FindData("PatrolStep"))
    {
        pSelfObj->AddDataInt("PatrolStep", -1);
    }
    else
    {
        pSelfObj->SetDataInt("PatrolStep", -1);
    }
    

	//初始化巡逻休息时间
	if (!pSelfObj->FindData("PatrolRestTime"))
    {
        pSelfObj->AddDataInt("PatrolRestTime", 0);
    }
    else
    {
        pSelfObj->SetDataInt("PatrolRestTime", 0);
    }

	//设置回走标志
	if (!pSelfObj->FindData("PatrolBack"))
    {
        pSelfObj->AddDataInt("PatrolBack", 0);
    }
    else
    {
        pSelfObj->SetDataInt("PatrolBack", 0);
    }
	    

	int PatrolFreq = 2 * 1000 + util_random_int(2000);
	
	//增加巡逻心跳
	if (!pKernel->FindHeartBeat(self, "AIHeartBeat::HB_AIPatrol"))
	{
		pKernel->AddHeartBeat(self, "AIHeartBeat::HB_AIPatrol", PatrolFreq);
	}
	return AI_RT_SUCCESS;
}

//结束巡逻
int AITemplateBase::EndPatrol(IKernel * pKernel , const PERSISTID & self) const
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	pSelfObj->SetInt("SubState", AI_PATROL_WAIT);

	//删除巡逻心跳
	if (pKernel->FindHeartBeat(self,"AIHeartBeat::HB_AIPatrol"))
	{
		MotionModule::MotionStop(pKernel, self);
		pKernel->RemoveHeartBeat(self,"AIHeartBeat::HB_AIPatrol");
	}
	return AI_RT_SUCCESS;
}

//产生随机的巡逻点
void AITemplateBase::InitRandomPatrolPath(IKernel * pKernel, const PERSISTID & self, bool doStop ) const
{
	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if ( pSelfObj == NULL )
	{
		return;
	}

	//查看追击范围生成初始路径
	float radius = pSelfObj->QueryFloat("PatrolRange");
	if ( FloatEqual(radius, 0.0f) )
	{
		return;
	}

	//已经有了巡逻
    IRecord *pPatrolPointRec = pSelfObj->GetRecord("PatrolPointRec");
    if(pPatrolPointRec == NULL)
    {
        return;
    }
	int rows = pPatrolPointRec->GetRows();
	if (rows > 0)
		return;

	// 4个方向,角度预定义
	const float fAngle[4] = {PI_4, -PI_4 + PI, PI_4 + PI, -PI_4};

	// 用于保证所取的象限不同
	int bUsed[4] = {0, 0, 0, 0};

	float random_orient = 0.0f;
	float random_radius = 0.0f;

	// 在半径范围内随机生成point_num个路径点
	float x = pSelfObj->GetPosiX();
	float z = pSelfObj->GetPosiZ();
	float y = pSelfObj->GetPosiY();

	float lastX = x;
	float lastZ = z;
	float lastY = y;

	float newX = 0.0f;
	float newZ = 0.0f;
	float newY = 0.0f;

	int orient_index = 0;

    LoopBeginCheck(a);
	for (int i = 0; i < AI_PARTOL_COUNT; ++i)
	{
        LoopDoCheck(a);
		// 得到随机的角度和半径(这个随机方式保证每次随机处于不同的4个象限之一)
		orient_index = util_random_int(4);

        LoopBeginCheck(b);
		while (bUsed[orient_index] > 0)
		{
            LoopDoCheck(b);
			orient_index = util_random_int(4);
		}

		bUsed[orient_index] = 1;

		random_orient = fAngle[orient_index] + util_random_float(PI_4);
		random_radius = 0.75f * radius + util_random_float(0.25f * radius);

		// 获得随机点
		newX = x + random_radius * cos(random_orient);
		newZ = z + random_radius * sin(random_orient);
		newY = GetCurFloorHeight(pKernel, newX, lastY, newZ);

		// 判断本次随机的点和上次的点之间是否可以行走
		float bx, by, bz;
		if (!pKernel->TraceLineWalk(2.0f, lastX, 
			lastY, lastZ, newX, newZ, bx, by, bz))
		{
			newX = bx;
			newZ = bz;
			newY = by;
		}

		int newIndex = pKernel->GetGridIndexByPos(newX,newZ);
		int lastIndex = pKernel->GetGridIndexByPos(lastX,lastZ);
		if (newIndex == lastIndex)
		{
			continue;
		}

		// 添加巡逻表，保存上个点坐标
		lastX = newX;
		lastZ = newZ;
		lastY = newY;

		// 随机一个巡逻点的停留时间(ms) 5-10秒
        int stop_time = 1000 * (5 + util_random_int(5));
        if (!doStop)
        {
            //强化石秘境，要求巡逻停顿时间减短
            stop_time = 1000;
        }
		
		CVarList var;
		var << lastX << lastZ << stop_time;
		pPatrolPointRec->AddRowValue(-1, var);
	}

    //生成巡逻路径失败
    if(pPatrolPointRec->GetRows() < 1)
    {
        pSelfObj->SetInt("BornStand", 1);
    }
}

//设置怪物初始属性
void AITemplateBase::InitPropAfterBorn(IKernel* pKernel, const PERSISTID& self) const
{
	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if ( pSelfObj == NULL )
	{
		return;
	}

	//先获取设置怪物的基础属性
	int strBasePackage = AISystem::GetNpcPackage(pSelfObj);
	if ( strBasePackage > 0 )
	{
		MapPropValue mapPropValue;
		//添加基础实力
		CVarList vPropNameList;
		CVarList vPropValueList;

		//查看对应包的数据
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_NPC_BASE_PACK, 
			util_int_as_string(strBasePackage).c_str(), vPropNameList, vPropValueList);

        LoopBeginCheck(c);
		for ( size_t i = 0; i < vPropNameList.GetCount(); ++i )
		{
            LoopDoCheck(c);
			// 获取计算公式数据
			float fVal = (float)vPropValueList.IntVal(i);
			if ( FloatEqual(fVal, 0.0f) )
			{
				continue;
			}

			PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
				vPropNameList.StringVal(i), fVal, 0, mapPropValue);
		}

		PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	}

	float fX = pSelfObj->GetPosiX();
	float fZ = pSelfObj->GetPosiZ();
	float fOrient = pSelfObj->GetOrient();

	//设置出生点
	if ( pSelfObj->FindAttr("BornX") )
	{
		pSelfObj->SetFloat("BornX", fX);
	}

	if ( pSelfObj->FindAttr("BornZ") )
	{
		pSelfObj->SetFloat("BornZ", fZ);
	}

	if ( pSelfObj->FindAttr("BornOrient") )
	{
		pSelfObj->SetFloat("BornOrient", fOrient);
	}

	//设置NPC HP为MAXHP
	pSelfObj->SetInt64("HP", pSelfObj->QueryInt64("MaxHP"));
	
	//设置主动或者被动
	if ( pSelfObj->QueryInt("Initiative") == 1 )
	{
		pSelfObj->SetInt("NoSpring", 0);
	}
	else
	{
		pSelfObj->SetInt("NoSpring", 1);
	}
}

//设置怪物初始技能
void AITemplateBase::InitSkillAfterBorn(IKernel* pKernel, const PERSISTID& self) const
{
    IGameObj * pSelfObj = pKernel->GetGameObj(self);
    if ( pSelfObj == NULL )
    {
        return;
    }
    
	//根据技能包配置对应其他技能
	const char* skillStr = pSelfObj->QueryString("SkillStr");
	if (!StringUtil::CharIsNull(skillStr))
	{
		AIFunction::InitSkill(pKernel, self, skillStr);
	}
	
	len_t minAttackDist = 0.0f;
	len_t maxAttackDist = 0.0f;
	FightInterfaceInstance->ReadyUseRandomSkill(pKernel, self, minAttackDist, maxAttackDist);

	maxAttackDist = __max(2.0f, maxAttackDist);
	pSelfObj->SetFloat("MinAttackDist", minAttackDist);
	pSelfObj->SetFloat("MaxAttackDist", maxAttackDist);
}

//NPC出生公告（现在用于有国家的BOSS）
void AITemplateBase::BornNotice(IKernel *pKernel, const PERSISTID &self) const
{
    IGameObj * pSelfObj = pKernel->GetGameObj(self);
    if ( pSelfObj == NULL )
    {
        return;
    }

#ifndef FSROOMLOGIC_EXPORTS
	// 是否为需要发送通告的场景
// 	if (!BossRefreshModule::m_pBossRefreshModule->IsRefreshBossScene(pKernel))
// 	{
// 		return;
// 	}
#endif 

// 	int sceneID = pKernel->GetSceneId();
//     int npcType = pSelfObj->QueryInt("NpcType");
// 	const char* strBossId = pSelfObj->GetConfig();
//     if(npcType == NPC_TYPE_BOSS)
//     {
//         std::string sceneName = DEFINE_STRING_SCENE_NAME;
//         sceneName.append(StringUtil::IntAsString(sceneID));
// 
// 		CVarList msg;
// 		msg << SERVER_CUSTOMMSG_BOSS_BORN << strBossId << sceneID;
// 		pKernel->CustomByWorld(msg);
//     }
}