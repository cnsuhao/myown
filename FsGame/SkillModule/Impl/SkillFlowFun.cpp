//------------------------------------------------------------------------------
// 文件名:       SkillFlowFun.cpp
// 内  容:        流程处理函数
// 说  明:
// 创建日期:    2014年10月30日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/skillmodule/impl/SkillFlow.h"
#include "FsGame/define/CommandDefine.h"
#include "FsGame/define/ServerCustomDefine.h"
#include "FsGame/Define/FlowCategoryDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/skillmodule/impl/skillaccess.h"
#include "FsGame/skillmodule/impl/SkillStage.h"
#include "FsGame/skillmodule/impl/skilldataquerymodule.h"
#include "FsGame/skillmodule/inner/SkillEventManager.h"
#include "FsGame/skillmodule/FlowModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "SkillConfigData.h"
#include "../FightModule.h"
#include "../../Define/staticdatadefine.h"
#include "FightDataAccess.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "SkillUseChecker.h"
#include "../../Define/CoolDownDefine.h"


//////////////////////////////////////////////////////////////////////////
//流程准备期开始的回调函数

// 技能准备开始前
bool SkillFlow::OnFlowBeforePrepare(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

#ifdef _DEBUG
    const char* strSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
    extend_warning(LOG_TRACE,
        "[SkillFlow::OnFlowBeforePrepare] self:%d-%d skill configid:%s.",
        self.nIdent, self.nSerial, strSkillConfigID);
#endif

    int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    PERSISTID target = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);
    float dx = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_X);
    float dy = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Y);
    float dz = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Z);
    
    int iPrepareTime = pSelfObj->QueryInt(FIGHTER_FLOW_PREPARE_TIME);
    SkillStageSingleton::Instance()->CustomSkillPrepareStage(pKernel, self, iSkillUUID, iPrepareTime);

    SkillEventManager::SkillPrepare(pKernel, self, skill, iSkillUUID, target, dx, dy, dz);
    return true;
}

//技能准备时间到，开始发动技能或开始引导技能，并计算消耗
bool SkillFlow::OnFlowPrepare(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

#ifdef _DEBUG
    const char* strSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
    extend_warning(LOG_TRACE,
        "[SkillFlow::OnFlowPrepare] self:%d-%d skill configid:%s .",
        self.nIdent, self.nSerial, strSkillConfigID);
#endif

    float fSelfX, fSelfY, fSelfZ, fSelfO;
    pKernel->GetLocation(self, fSelfX, fSelfY, fSelfZ, fSelfO);

    PERSISTID target = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);
    float dx = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_X);
    float dy = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Y);
    float dz = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Z);

    float dorient = 0.0f;
    if (!SkillAccessSingleton::Instance()->GetSkillTargetAndPos(pKernel, self, skill, target, dx, dy, dz, dorient))
    {
        return false;
    }

    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_X, dx);
    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_Y, dy);
    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_Z, dz);

    // 准备期结束处理
    SkillFlowSingleton::Instance()->DoPrepareSkill(pKernel, self, skill, target, dx, dy, dz);
    return true;
}

//流程引导开始的回调函数
bool SkillFlow::OnFlowBeforeLead(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);

    int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    int iLeadTime = pSelfObj->QueryInt(FIGHTER_FLOW_LEAD_TIME);

#ifdef _DEBUG
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (pSkillObj != NULL)
	{
		const char* strSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
		extend_warning(LOG_TRACE,
			"[SkillFlow::OnFlowBeforeLead] self:%d-%d skill configid:%s leadtime:%d.",
			self.nIdent, self.nSerial, strSkillConfigID, iLeadTime);
	}
#endif 

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
    if (!pSkillMain)
    {
        return false;
    }

    // 通知进入引导阶段
    SkillStageSingleton::Instance()->CustomSkillLeadStage(pKernel, self, iSkillUUID, iLeadTime);

    return true;
}

//技能命中时间到
bool SkillFlow::OnFlowHit(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    const PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

#ifdef _DEBUG
	const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);

    extend_warning(LOG_TRACE,
        "[SkillFlow::OnFlowHit] self:%d-%d skill configid:%s.",
        self.nIdent, self.nSerial, szSkillConfigID);
#endif

    int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    PERSISTID target = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);

    float x = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_X);
    float y = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Y);
    float z = pSelfObj->QueryFloat(FIGHTER_CUR_SKILL_TARGET_Z);

    int iUseType = SkillAccessSingleton::Instance()->GetUseSkillType(pKernel, skill);
    if (USESKILL_TYPE_TARGET == iUseType)
    {
        if (!pKernel->Exists(target))
        {
            return false;//目标对象已不存在
        }
        float o;
        pKernel->GetLocation(self, x, y, z, o);
    }
    if (!SkillFlowSingleton::Instance()->DoHitSkill(pKernel, self, skill, iSkillUUID, target, x, y, z))
    {
        return false;
    }

    return true;
}



//流程被打断的回调函数
bool SkillFlow::OnFlowBreak(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

#ifdef _DEBUG
    const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
    extend_warning(LOG_TRACE,
        "[SkillFlow::OnFlowBreak] self:%d-%d skill configid:%s.",
        self.nIdent, self.nSerial, szSkillConfigID);
#endif

    SkillFlowSingleton::Instance()->DoBreakSkill(pKernel, self, skill);
    // 通知客户端技能被打断
    int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);

    int iBreakReason = SKILL_BREAK_REASON_TYPE_NONE;
    if (pSelfObj->FindData("SkillBreakReason"))
    {
        iBreakReason = pSelfObj->QueryDataInt("SkillBreakReason");
        pSelfObj->SetDataInt("SkillBreakReason", SKILL_BREAK_REASON_TYPE_NONE); // 查询后，立刻还原
    }

	// 清除公共CD
	CoolDownModule::m_pInstance->EndCoolDown(pKernel, self, COOLDOWN_CATEGOTYID_SKILL);

    SkillStageSingleton::Instance()->CustomSkillBreakStage(pKernel, self, iSkillUUID, iBreakReason);

    SkillFlowSingleton::Instance()->ClearUseSkillData(pKernel, self, skill);
    return true;
}

//流程等待期结束的回调函数
bool SkillFlow::OnFlowFinish(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    // 执行结束流程处理
    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

#ifdef _DEBUG
    const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
    extend_warning(LOG_TRACE,
        "[SkillFlow::OnFlowFinish] self:%d-%d skill configid:%s.",
        self.nIdent, self.nSerial, szSkillConfigID);
#endif

	SkillFlowSingleton::Instance()->DoFinishSkill(pKernel, self, skill);
    // 防止SkillFlow::OnFinishSkill执行后导致self被销毁
    if (!pKernel->Exists(self))
    {
        return false;
    }
    // 通知客户端技能结束阶段
    int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    SkillStageSingleton::Instance()->CustomSkillFinish(pKernel, self, iSkillUUID, 0);

    SkillFlowSingleton::Instance()->ClearUseSkillData(pKernel, self, skill);

	// 技能结束后
	SkillEventManager::SkillAfterFinish(pKernel, self, skill, 0);
    return true;
}

//////////////////////////////////////////////////////////////////////////
//执行技能的准备，准备动作结束时调用
bool SkillFlow::DoPrepareSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
                               const PERSISTID& target, float x, float y, float z)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
    if (NULL == pSkillMain)
    {
        return false;
    }

#ifdef _DEBUG
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (pSkillObj != NULL)
	{
		const char* strSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
		extend_warning(LOG_TRACE,
			"[SkillFlow::DoPrepareSkill] self:%d-%d skill configid:%s target:%d-%d target pos(%f,%f,%f).",
			self.nIdent, self.nSerial, strSkillConfigID, 
			target.nIdent, target.nSerial, x, y, z);
	}
#endif

    const int64_t iSkillUUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    SkillEventManager::SkillAfterPrepare(pKernel, self, skill, iSkillUUID, target, x, y, z);

    return true;
}

//执行技能的命中
bool SkillFlow::DoHitSkill(IKernel* pKernel, const PERSISTID& self,
                           const PERSISTID& skill, const int64_t iSkillUUID,
                           const PERSISTID& target, float x, float y, float z)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

	// 是否由客户端命中
	if(SkillAccessSingleton::Instance()->IsClientHitTarget(pKernel, pSelfObj, pSkillObj))
	{
		return true;
	}
	
#ifdef _DEBUG
	const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);

    extend_warning(LOG_TRACE,
        "[SkillFlow::DoHitSkill] self:%d-%d skill configid:%s target:%d-%d self pos(%f,%f,%f)  IsEffectCollideSkill:%d.",
        self.nIdent, self.nSerial, szSkillConfigID, 
        target.nIdent, target.nSerial, x, y, z, 0);
#endif

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (pSkillMain == NULL)
	{
		return false;
	}

	// 不是伤害技能不处理命中效果
	if(NONE_DAMAGE_TYPE == pSkillMain->GetDamageType())
	{
		return true;
	}

    //命中次数
    int hits = pSkillObj->QueryInt(SKILL_CUR_HITS) + 1;
    pSkillObj->SetInt(SKILL_CUR_HITS, hits);

    //统计技能影响的对象
    CVarList targetList;
    GetHitRangeTargets(pKernel, self, skill, x, y, z, hits, targetList);

	SkillEventManager::SkillBeforeHit(pKernel, self, skill, iSkillUUID);

	pKernel->Command(self, self, CVarList() << COMMAND_SKILL_BEFORE_HIT << skill);

    //开始执行命中的效果
    DoEffectHitTargetProcess(pKernel, self, skill, iSkillUUID, target, targetList);
    return true;
}

// 特效命中处理
bool SkillFlow::DoEffectHitTargetProcess(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, int64_t iSkillUUID,
                                         const PERSISTID& target, const IVarList& targetList)
 {
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (pSkillMain == NULL)
	{
		return false;
	}

	size_t count = targetList.GetCount();
	//处理每个命中目标
	LoopBeginCheck(i)
	for (size_t i = 0; i < count; i++)
	{
		LoopDoCheck(i)
		PERSISTID temptarget = targetList.ObjectVal(i);

		pSelfObj->SetObject(FIGHTER_CUR_SKILL_TARGET, temptarget);
		SkillEventManager::SkillAfterHit(pKernel, self, skill, iSkillUUID, temptarget);
		FightModule::m_pInstance->SkillDamageTarget(pKernel, self, temptarget, skill, iSkillUUID);
	}
    return true;
}

//技能使用结束[单]
bool SkillFlow::DoFinishSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    //清空目标
    PERSISTID target = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);
    int64_t uuid = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    SkillEventManager::SkillFinish(pKernel, self, skill, uuid, target);

    //// 使用技能结束消息增加攻方参数
    CVarList msg;
    msg << COMMAND_SKILL_FINISH << skill << self;
    if (!pKernel->Exists(self))
    {
        return false;
    }
    pKernel->Command(self, self, msg);
    
    // 如果有目标，并且不是自己，则给目标发命令
//     if (pKernel->Exists(target))
//     {
//         if (self != target)
//         {
//             pKernel->Command(self, target, msg);
//         }
//     }

    return true;
}

//技能使用被打断
bool SkillFlow::DoBreakSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    //发送打断命令
    CVarList msg;
    msg << COMMAND_SKILL_BREAKED << skill;
    pKernel->Command(self, self, msg);

    if (!pKernel->Exists(skill))
    {
        return true;
    }

    // 触发技能事件
    int64_t uuid = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
    SkillEventManager::SkillBreak(pKernel, self, skill, uuid);

    return true;
}