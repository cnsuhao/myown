//--------------------------------------------------------------------
// 文件名:		FightActionControl.h
// 内  容:		战斗行为状态控制
// 说  明:		
//				
// 创建日期:	2016年04月06日
// 整理日期:	2016年04月06日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------
#include "FightActionControl.h"
#include "..\Interface\PropertyInterface.h"
#include "..\Define\FightDefine.h"
#include "..\Define\GameDefine.h"
#include "..\Interface\FightInterface.h"
#include "..\NpcBaseModule\AI\AIDefine.h"

// 初始化
bool FightActionControl::Init(IKernel* pKernel)
{
	// 移除硬直状态状态心跳
	DECL_HEARTBEAT(FightActionControl::H_RemoveActionState);

	return true;
}

// 改变战斗行为状态
bool FightActionControl::ChangeState(IKernel* pKernel, const PERSISTID& self, int nState, int nStiffTime)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 建筑NPC除外
	if (pSelfObj->GetClassType() == TYPE_NPC && pSelfObj->QueryInt("NpcType") > NPC_TYPE_BUILDING)
	{
		return false;
	}

	// 对象死亡了
	int nDead = 0;
	if (FPropertyInstance->GetDead(pSelfObj, nDead) && 1 == nDead)
	{
		return false;
	}

	// 获取当前状态
	int nCurState = 0;
	if (!FPropertyInstance->GetFightActionState(pSelfObj, nCurState))
	{
		return false;
	}

	// 强制恢复为正常状态(使用闪避技能的特殊处理)
	if (nCurState > FAS_NORMAL_STATE && FAS_NORMAL_STATE == nState)
	{
		// 去除原来的状态
		DELETE_HEART_BEAT(pKernel, self, "FightActionControl::H_RemoveActionState");
		FightInterfaceInstance->RemoveControlBuffer(pKernel, self, CVarList() << CB_CANT_MOVE << CB_STIFF_STATE);

		// 设置当前状态值
		FPropertyInstance->SetFightActionState(pSelfObj, nState);
		return true;
	}

	// 优先级较低,不做状态改变
	if (nState < nCurState || FAS_NORMAL_STATE == nState)
	{
		return false;
	}

	if (nStiffTime <= 0)
	{
		char strInfo[128] = {0};
		SPRINTF_S(strInfo, "FightActionControl::ChangeState oldstate = %d, newstate = %d, result = %d", nCurState, nState, nStiffTime);
		::extend_warning(LOG_INFO, strInfo);
		return false;
	}

	// 去除原来的状态
	DELETE_HEART_BEAT(pKernel, self, "FightActionControl::H_RemoveActionState");
	FightInterfaceInstance->RemoveControlBuffer(pKernel, self, CVarList() << CB_CANT_MOVE << CB_STIFF_STATE);

	
	// 增加新的
	FightInterfaceInstance->AddControlBuffer(pKernel, self, self, CVarList() << CB_CANT_MOVE << CB_STIFF_STATE, CONTROL_BUFF_FOREVER);
	pKernel->AddCountBeat(self, "FightActionControl::H_RemoveActionState", nStiffTime, 1);

	// 设置当前状态值
	FPropertyInstance->SetFightActionState(pSelfObj, nState);
	return true;
}

// 获取当前的行为状态
int FightActionControl::GetFightActionState(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	int nState = FAS_NORMAL_STATE;
	if(!FPropertyInstance->GetFightActionState(pSelfObj, nState))
	{
		return nState;
	}

	return nState;
}

// 重置为正常状态
void FightActionControl::ResetNormalState(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if(NULL == pSelfObj)
	{
		return;
	}

	FightInterfaceInstance->RemoveControlBuffer(pKernel, self, CVarList() << CB_CANT_MOVE << CB_STIFF_STATE);
	FPropertyInstance->SetFightActionState(pSelfObj, FAS_NORMAL_STATE);
}

// 移除硬直状态
int FightActionControl::H_RemoveActionState(IKernel* pKernel,const PERSISTID& self,int slice)
{
	FightActionControlSingleton::Instance()->ResetNormalState(pKernel, self);
	return 0;
}