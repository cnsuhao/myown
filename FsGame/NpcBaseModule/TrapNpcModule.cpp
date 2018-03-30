//--------------------------------------------------------------------
// 文件名:		TrapNpc.cpp
// 内  容:		陷阱NPC
// 说  明:		
// 创建日期:	2015年07月11日
// 创建人:		  
//    :	    
//--------------------------------------------------------------------

#include "TrapNpcModule.h"
#include "utils/extend_func.h"
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventManagerModule.h"
#include "../SkillModule/Impl/TargetFilter.h"
#include "../Define/Skilldefine.h"
#include "utils/custom_func.h"
#include "../Define/GameDefine.h"
#include "utils/string_util.h"
#include "FsGame/Interface/FightInterface.h"

TrapEventManagerModule* TrapNpcModule::m_pTrapEventManager = NULL;
// 初始化
bool TrapNpcModule::Init(IKernel* pKernel)
{
	m_pTrapEventManager = (TrapEventManagerModule*)pKernel->GetLogicModule("TrapEventManagerModule");
	assert(m_pTrapEventManager != NULL);

	pKernel->AddEventCallback("TrapNpc", "OnEntry", TrapNpcModule::OnEntry);
	pKernel->AddEventCallback("TrapNpc", "OnSpring", TrapNpcModule::OnSpring);
	pKernel->AddEventCallback("TrapNpc", "OnEndSpring", TrapNpcModule::OnEndSpring);
	pKernel->AddEventCallback("TrapNpc", "OnDestroy", TrapNpcModule::OnDestroy);
	
    DECL_HEARTBEAT(TrapNpcModule::HB_Delay_Spring);

	return true;
}

// 释放
bool TrapNpcModule::Shut(IKernel* pKernel)
{
	return true;
}

// 进入场景
int TrapNpcModule::OnEntry(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	return 0;
}

// 载入回调
int TrapNpcModule::OnSpring(IKernel* pKernel, const PERSISTID& self, 
					  const PERSISTID& sender, const IVarList& args)
{
	//1、验证对象是否存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if ( NULL == pSelfObj || !pKernel->Exists(sender) )
	{
		return 0;
	}
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if ( NULL == pSenderObj )
	{
		return 0;
	}

	//2、触发对象如果是无敌的过滤
	if( !pSenderObj->FindAttr("CantBeAttack") || pSenderObj->QueryInt("CantBeAttack") > 0 )
	{
		return 0;
	}

	//获取触发事件
	SpringEvent funcID = (SpringEvent)pSelfObj->QueryInt("SpringEvent");
	
	//判定对象是否需要过滤
	PERSISTID object = self;
	PERSISTID master = pSelfObj->QueryObject("Master");
	if (pKernel->Exists(master))
	{
		object = master;
	}

	int iSpringType = pSelfObj->QueryInt("SpringType");
	int iSpringRelation = pSelfObj->QueryInt("SpringRelation");// EHitTargetRelationType
	// 默认为作用于敌方单位
	if (0 == iSpringRelation)
	{
		iSpringRelation = HIT_TARGET_RELATION_TYPE_ENIMY;
	}
	//技能目标类型于陷阱目标类型转换匹配
	int niHitTargetType = TYPE_PLAYER == iSpringType ? HIT_TARGET_TYPE_PLAYER : HIT_TARGET_TYPE_MONSTER;

	if(USESKILL_RESULT_SUCCEED != TargetFilter::SatisfyRelation(pKernel, object, sender, iSpringRelation, niHitTargetType))
	{
		return 0;
	}
	
	m_pTrapEventManager->ExecuteOperate(pKernel, self, sender, funcID);


	// 修改Npc模型资源
    const char* resourceB = pSelfObj->QueryString("ResourceB");
    if( resourceB != NULL )
	{
		pSelfObj->SetString("Resource", resourceB);
	}

	// 是否立即摧毁陷阱
	int iSpringDestroy = pSelfObj->QueryInt("SpringDestroy");
	if (iSpringDestroy == SPRING_DESTROY_IMMEDIATELY)
	{
		pKernel->DestroySelf(self);
	}
	else if( iSpringDestroy == SPRING_DESTROY_EFFECT_TIME_OUT )
	{
		pSelfObj->SetInt("LifeTime", pSelfObj->QueryInt("EffectTime"));
	}
	return 0;
}

//脱离
int TrapNpcModule::OnEndSpring(IKernel* pKernel, const PERSISTID& self,  const PERSISTID& sender, const IVarList& args)
{
	//1、验证对象是否存在
	if ( !pKernel->Exists(self) || !pKernel->Exists(sender) )
	{
		return 0;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if ( NULL == pSelfObj || !pKernel->Exists(sender) )
	{
		return 0;
	}

	//获取触发事件
	SpringEvent funcID = (SpringEvent)pSelfObj->QueryInt("EndSpringEvent");
	m_pTrapEventManager->ExecuteOperate(pKernel, self, sender, funcID);

	return 0;
}

//摧毁
int TrapNpcModule::OnDestroy(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	//1、验证自身对象是否为空
	if ( !pKernel->Exists(self) 
		|| !pKernel->Exists(sender) ) 
	{
		return 0;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if ( NULL == pSelfObj || !pKernel->Exists(sender) )
	{
		return 0;
	}

	//2、获取触发事件
	SpringEvent funcID = (SpringEvent)pSelfObj->QueryInt("DestroyEvent");
	m_pTrapEventManager->ExecuteOperate(pKernel, self, sender, funcID);

	return 0;
}

// 延迟心跳
int TrapNpcModule::HB_Delay_Spring(IKernel* pKernel, const PERSISTID& self, int silce)
{
    if (!pKernel->Exists(self))
    {
        return 0;
    }
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return 0;
    }
    IRecord * SelfRecord = pSelfObj->GetRecord("SpringInfoRec");
    if (SelfRecord == NULL)
    {
        return 0;
    }

    //表数据满则不做以下流程处理
    if( SelfRecord->GetRows() >= SelfRecord->GetRowMax() )
    {
        return 0;
    }

    CVarList playerList;
    len_t iSpringRange = pSelfObj->QueryFloat("SpringRange");
    pKernel->GetAroundList(self, iSpringRange, TYPE_PLAYER, 0, playerList);
    if (playerList.GetCount() <= 0)
    {
        return 0;
    }
    const char* BuffID = pSelfObj->QueryString("EventValue");
    if (StringUtil::CharIsNull(BuffID))
    {
        return 0;
    }
    LoopBeginCheck(b);
    for (int i = 0; i < (int)playerList.GetCount(); ++i)
    {
        LoopDoCheck(b);
        PERSISTID player = playerList.ObjectVal(i);
        if (!pKernel->Exists(player))
        {
            continue;
        }
        //添加玩家BUFF
        FightInterfaceInstance->AddBuffer(pKernel, player, self, BuffID);

        //将其加入触发者列表
        CVarList arg_list;
        arg_list << self << BuffID;
        SelfRecord->AddRowValue(-1, arg_list);
    }
    
    return 0;
}