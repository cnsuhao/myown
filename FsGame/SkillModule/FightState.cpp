//--------------------------------------------------------------------
// 文件名:		FightState.cpp
// 内  容:		战斗状态
// 说  明:		
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#include "FightState.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include <time.h>
#include "Define/FightDefine.h"
#include "utils/EnvirDefine.h"
#include "CommonModule/EnvirValueModule.h"

FightState * FightState::m_pFightState = NULL;

// 初始化
bool FightState::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	m_pFightState = this;

	Assert( m_pFightState != NULL );

	pKernel->AddEventCallback("player", "OnReady",FightState::OnReady);

	pKernel->AddIntCommandHook("player", COMMAND_DAMAGETARGET, FightState::OnCommandUpdateFightState);
	pKernel->AddIntCommandHook("npc", COMMAND_DAMAGETARGET, FightState::OnCommandUpdateFightState);

	pKernel->AddIntCommandHook("player", COMMAND_HIT_MISS, FightState::OnCommandUpdateFightState);
	pKernel->AddIntCommandHook("npc", COMMAND_HIT_MISS, FightState::OnCommandUpdateFightState);

	DECL_HEARTBEAT(FightState::HB_ResetFightState);

	return true;
}

// 释放
bool FightState::Shut(IKernel* pKernel)
{
	return true;
}

//就绪
int FightState::OnReady(IKernel* pKernel, const PERSISTID& self, 
							  const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(sender))
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);

	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nLeaveFightTime = EnvirValueModule::EnvirQueryInt(ENV_VALUE_LEAVE_FIGHT_TIME) * 1000;
	ADD_HEART_BEAT(pKernel, self, "FightState::HB_ResetFightState", nLeaveFightTime);

	return 0;
}

// 更新玩家战斗状态
int FightState::OnCommandUpdateFightState(IKernel* pKernel, const PERSISTID& self,
												const PERSISTID& sender, const IVarList& args)
{
	int mainCMD = args.IntVal(0);

	switch (mainCMD)
	{
	case COMMAND_DAMAGETARGET:
	case COMMAND_HIT_MISS:
		{
			PERSISTID hurtTarget =args.ObjectVal(1);
			int64_t curTime = ::time(NULL);
			int nLeaveFightTime = EnvirValueModule::EnvirQueryInt(ENV_VALUE_LEAVE_FIGHT_TIME) * 1000;
			if (pKernel->Exists(self) && pKernel->Exists(hurtTarget) && pKernel->Type(hurtTarget) == TYPE_PLAYER)
			{
				IGameObj* pHurtTargetObj = pKernel->GetGameObj(hurtTarget);
				if (pHurtTargetObj != NULL)
				{
					if (pHurtTargetObj->FindAttr("FightState") && pHurtTargetObj->FindAttr("FightTime"))
					{
						pHurtTargetObj->SetInt("FightState", 1);
						pHurtTargetObj->SetInt64("FightTime", curTime + nLeaveFightTime);
					}
				}
			}

			IGameObj* pSelfObj = pKernel->GetGameObj(self);
			if (pSelfObj != NULL && pSelfObj->GetClassType() == TYPE_PLAYER)
			{
				if (pSelfObj->FindAttr("FightState") && pSelfObj->FindAttr("FightTime"))
				{
					pSelfObj->SetInt("FightState", 1);
					pSelfObj->SetInt64("FightTime", curTime + nLeaveFightTime);
				}
			}
		}

		break;
	default:
		break;
	}

	return 0;
}

//重置战斗状态
int FightState::HB_ResetFightState(IKernel * pKernel, const PERSISTID & self, int slice)
{
	if (!pKernel->Exists(self))
	{
		return 1;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);

	if (NULL == pSelfObj)
	{
		return 1;
	}

	if (!pSelfObj->FindAttr("FightState") || !pSelfObj->FindAttr("FightTime"))
	{
		return 1;
	}

	int iFightState = pSelfObj->QueryInt("FightState");
	if (iFightState == 0)
	{
		return 1;
	}

	int64_t iFightTime = pSelfObj->QueryInt64("FightTime");
	int64_t curTime = ::time(NULL);
	if (curTime > iFightTime)
	{
		pSelfObj->SetInt("FightState", 0);
		pSelfObj->SetInt64("FightTime", 0);
	}

	return 1;
}