//--------------------------------------------------------------------
// 文件名:      SkillEventFuncSkill.cpp
// 内  容:      技能事件逻辑
// 说  明:		
// 创建日期:    2014年11月24日
// 创建人:       
//    :       
//--------------------------------------------------------------------

#include "SkillEventFuncSkill.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "utils/geometry_func.h"

#include "FsGame/SkillModule/impl/SkillAccess.h"
#include "FsGame/SkillModule/impl/SkillStage.h"
#include "FsGame/SkillModule/impl/SkillFlow.h"
#include "FsGame/SkillModule/impl/FightDataAccess.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/SkillModule/inner/SkillEventExecuteUtil.h"
#include "FsGame/SkillModule/SkillModule.h"
#include "FsGame/SkillModule/FightModule.h"

#include "FsGame/Interface/PropertyInterface.h"
#include "FsGame/Interface/FightInterface.h"
#include "utils/custom_func.h"
#include "utils/exptree.h"
#include "../../Define/CommandDefine.h"
#include "../../Define/ServerCustomDefine.h"
#include "../../Define/BufferDefine.h"
#include "../../Define/GameDefine.h"
#include "../../NpcBaseModule/AI/Template/AITemplateBase.h"
#include "utils/util_func.h"
#include "../SkillToSprite.h"
#include "../../Define/ClientCustomDefine.h"
#include "../SceneAttackManage.h"
#include "Define/Fields.h"

using namespace SkillEventExecuteUtil;

// 初始化
bool SkillEventFlickerMove::OnInit(IKernel* pKernel)
{
	return true;
}

// 	// 闪烁
// EVENT_EXEC_FLICKER_MOVE = 314,
// 运行技能事件
// 参数1:伤害系数
bool SkillEventFlickerMove::Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
	const PERSISTID& skill, const int64_t uuid, const int iEventType,
	const IVarList& params, IVarList& outMsgParams)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || !pKernel->Exists(skill))
	{
		return false;
	}

	// 闪烁的距离
	float fFlickDis = vEventParams.FloatVal(2);
	
	const SkillMainData* pSkillData = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pSkillData)
	{
		return false;
	}
	float fSpeed = pSkillData->GetDisplaceSpeed();
	// 玩家闪烁,位置由客户端来选择后发给服务器,服务器做验证
	if (TYPE_PLAYER == pSelfObj->GetClassType())
	{
		float fPosX = params.FloatVal(0);
		float fPosY = params.FloatVal(1);
		float fPosZ = params.FloatVal(2);

		SkillEventExecuteUtil::MotionObject(pKernel, pSelfObj, fSpeed, fPosX, fPosZ);
	}
	else
	{
// 		const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
// 		if (NULL == pMainData)
// 		{
// 			return false;
// 		}
// 		// 获取位移的位置
// 		float fDesX, fDesY, fDesZ;
// 		SkillEventExecuteUtil::GetFinalPosByOrient(pKernel, pSelfObj->GetPosiX(), pSelfObj->GetPosiY(), pSelfObj->GetPosiZ(), pSelfObj->GetOrient(), fFlickDis, fDesX, fDesY, fDesZ);
// 		float fSpeed = pMainData->GetDisplaceSpeed();
// 
// 		SkillEventExecuteUtil::MotionObject(pKernel, pSelfObj, fSpeed, fDesX, fDesZ);
	}
	return true;
}

// 初始化
bool SkillEventCallTotem::OnInit(IKernel* pKernel)
{
	DECL_HEARTBEAT(SkillEventCallTotem::H_TotemAttack);
	return true;
}

// 运行技能事件
// 参数1:伤害系数
bool SkillEventCallTotem::Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
	const PERSISTID& skill, const int64_t uuid, const int iEventType,
	const IVarList& params, IVarList& outMsgParams)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	const char* strNpcId = vEventParams.StringVal(1);
	float fSkillRadio = vEventParams.FloatVal(2);
	int nInterval = vEventParams.IntVal(3);
	int nLastTime = vEventParams.IntVal(4);

	float fPosX = params.FloatVal(0);
	float fPosZ = params.FloatVal(2);
	float fPosY = pKernel->GetMapHeight(fPosX, fPosZ);

	PERSISTID npc = pKernel->CreateObjectArgs("", strNpcId, 0, fPosX, fPosY, fPosZ, pSelfObj->GetOrient(), CVarList() << CREATE_TYPE_PROPERTY_VALUE << FIELD_PROP_GROUP_ID<< -1);
	IGameObj* pNpcObj = pKernel->GetGameObj(npc);
	if (NULL == pNpcObj || nInterval <= 0)
	{
		return false;
	}

	pNpcObj->SetObject(FIELD_PROP_MASTER, self);
	pNpcObj->SetObject(FIELD_PROP_SKILL, skill);
	pNpcObj->AddDataFloat("CT_Radio", fSkillRadio);
	pNpcObj->SetInt(FIELD_PROP_LIFE_TIME, nLastTime);

	FightInterfaceInstance->AddControlBuffer(pKernel, npc, npc, CVarList() << CB_CANT_MOVE << CB_CANT_BE_ATTACK, CONTROL_BUFF_FOREVER);

	int nAttackNum = nLastTime / nInterval;
	if (nAttackNum <= 0)
	{
		return false;
	}

	// 生成时攻击一次
	H_TotemAttack(pKernel, npc, 0);
	ADD_COUNT_BEAT(pKernel, npc, "SkillEventCallTotem::H_TotemAttack", nInterval, nAttackNum);
	return true;
}

// 创建陷阱
int SkillEventCallTotem::H_TotemAttack(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pNpcObj = pKernel->GetGameObj(self);
	if (NULL == pNpcObj)
	{
		return 0;
	}

	PERSISTID master = pNpcObj->QueryObject(FIELD_PROP_MASTER);
	PERSISTID skill = pNpcObj->QueryObject(FIELD_PROP_SKILL);
	IGameObj* pMasterObj = pKernel->GetGameObj(master);
	if (NULL == pMasterObj || !pKernel->Exists(skill))
	{
		return 0;
	}
	float fSkillRadio = 1.0f;
	if (pNpcObj->FindData("CT_Radio"))
	{
		fSkillRadio = pNpcObj->QueryDataFloat("CT_Radio");
	}

	CVarList attacklist;
	DamgeRangeMultiEnemy(pKernel, skill, pMasterObj, NULL, fSkillRadio, 1.0f, false, SKILL_EVENT_DAMAGE, 0, attacklist);
	return 0;
}