//------------------------------------------------------------------------------
// 文件名:       SkillFlow.cpp
// 内  容:        技能流程相关处理
// 说  明:        该类只在技能系统内部使用，且不要直接定义SkillFlow的实例，而使用
//              其单例模式接口SkillFlowSingleton::Instance()
// 创建日期:    2014年10月27日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/skillmodule/impl/SkillFlow.h"
#include "FsGame/define/CommandDefine.h"
#include "FsGame/Define/FlowCategoryDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "utils/custom_func.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/skillmodule/FlowModule.h"
#include "FsGame/skillmodule/impl/skillaccess.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/skillmodule/impl/SkillStage.h"
#include "FsGame/skillmodule/impl/TargetFilter.h"
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Interface/PropertyInterface.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/SkillModule/SkillToSprite.h"
#include "SkillUseChecker.h" 
#include "../../Define/ServerCustomDefine.h"
#include "../FightModule.h"
#include "../../Define/SkillDefine.h"
#include "utils/util.h"
#include "../../Define/GameDefine.h"
#include "../../Define/CoolDownDefine.h"
#include "SystemFunctionModule/EnergyModule.h"
//#include "../GaintModule.h"
//#include "FsGame/SceneBaseModule/SceneInfoModule.h"

#define MAX_HIT_FLOAT_SPEED 10.0f

int nx_set_no_cooldown(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_no_cooldown, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_no_cooldown, 1);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!pSelfObj->FindData("NoCoolDown"))
	{
		pSelfObj->AddDataInt("NoCoolDown", 0);
	}
	int nNoCoolDown = pSelfObj->QueryDataInt("NoCoolDown");
	nNoCoolDown = nNoCoolDown > 0? 0:1;
	pSelfObj->SetDataInt("NoCoolDown", nNoCoolDown);
	if (nNoCoolDown > 0)
	{
		//::CustomSysInfo(pKernel, self, TIPSTYPE_GMINFO_MESSAGE, "gm_info", CVarList() << L"set NoCoolDown.");
	}
	else
	{
		//::CustomSysInfo(pKernel, self, TIPSTYPE_GMINFO_MESSAGE, "gm_info", CVarList() << L"set CoolDown");
	}
	return 0;
}

bool SkillFlow::Init(IKernel* pKernel)
{
    //注册新的流程
    FlowModule::m_pInstance->RegisterFlow(FLOW_CATEGORY_SKILL,
                                          SkillFlow::OnFlowBeforePrepare,
                                          SkillFlow::OnFlowPrepare,
                                          SkillFlow::OnFlowBeforeLead,
                                          NULL,
                                          SkillFlow::OnFlowHit,
                                          SkillFlow::OnFlowBreak,
                                          SkillFlow::OnFlowFinish);

	DECL_LUA_EXT(nx_set_no_cooldown);

	DECL_HEARTBEAT(SkillFlow::H_SkillStiffStateEnd);
    return true;
}

//使用技能的最终接口，所有对技能使用的调用都会走到这里。
int SkillFlow::UseSkill(IKernel* pKernel, const PERSISTID& self,
                        float x, float y, float z, const PERSISTID& skill,
                        const PERSISTID& target, float dx, float dy, float dz, float dorient)
{
    // 参数有效性检测
    if (!pKernel->Exists(self))
    {
        return USESKILL_RESULT_NO_OBJECT;
	}

	//技能可释放检查
    int result = SkillUseCheckerSingleton::Instance()->CanUseSkill(pKernel, self, x, y, z, dorient, skill, target, dx, dy, dz);
    if (USESKILL_RESULT_SUCCEED == result)
    {
        // 检查释放可以打断正在流程当中使用的技能
        result = CheckForceUseSkill(pKernel, self, skill);
    }
    // 判断返回值 
    if (USESKILL_RESULT_SUCCEED != result)
    {
        return result; 
    }

	// 更新位置
	UpdateObjectLocate(pKernel, self, target, skill, x, y, z, dorient);

	// 添加技能硬直状态
	AddSkillStiffState(pKernel, self, skill);

    // 开始使用技能
    BeginUseSkill(pKernel, self, x, y, z, dorient, skill, target, dx, dy, dz);

	return USESKILL_RESULT_SUCCEED;
}

/*!
 * @brief	客户端主导技能逻辑处理
 * @param	self 施法者
 * @param	x,z,dorient 技能释放时施法者位置
 * @param	skill 技能对象
 * @param	targetList 目标列表
 * @return	int
 */
int SkillFlow::DoSkillHitByClient(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const int64_t nSkilluid, const int nCurClientHitIndex, const IVarList& targetList, const IVarList& moveTargetList)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSelfObj || NULL == pSkillObj)
	{
		return 0;
	}

	// "SkillBeginTime","ClientHitsIndex"
	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL == pSkillMain)
	{
		return false;
	}

	if (!pSkillMain->GetIsClientHit())
	{
		return false;
	}

	const char* strSkillId = pKernel->GetConfig(skill);
	// 1.技能持续时间验证
// 	if (!pSelfObj->FindData("SkillBeginTime"))
// 	{
// 		return false;
// 	}
// 
// 	if(pSkillMain->GetValidTime() < util_get_time_64() - pSelfObj->QueryDataInt64("SkillBeginTime"))
// 	{
// 		extend_warning(LOG_INFO, "Skill is %s, InValid is ValidTime,ValidTime is %d, Time Diff is %d",
// 			strSkillId, pSkillMain->GetValidTime(), util_get_time_64() - pSelfObj->QueryDataInt64("SkillBeginTime"));
// 		//SkillStageSingleton::Instance()->CustomSkillClientHitFailed(pKernel, self, strSkillId, VALID_TIME_FAILED, CVarList());
// 		return false;
// 	}
	
	// 2.命中次数索引验证
	if (!pSkillObj->FindData("ClientHitsIndex"))
	{
		return false;
	}
	int nCurHitIndex = pSkillObj->QueryDataInt("ClientHitsIndex");
	// 发重复名次次数保护
	if (nCurHitIndex == nCurClientHitIndex)
	{
		return false;
	}
	++nCurHitIndex;
	pSkillObj->SetDataInt("ClientHitsIndex", nCurHitIndex);
	if (nCurHitIndex != nCurClientHitIndex)
	{
		extend_warning(LOG_INFO, "Skill is %s, InValid is HitsIndex, server index:%d, client index:%d", strSkillId, nCurHitIndex, nCurClientHitIndex);
		//SkillStageSingleton::Instance()->CustomSkillClientHitFailed(pKernel, self, strSkillId, VALID_HIT_INDEX_FAILED, CVarList());
		return false;
	}

	// 开始结算命中目标
	int nTargetCount = (int)targetList.GetCount();

	// 要位移的参数格式为 PERSISTID, x, z, speed
	int nMoveTargetCount = (int)moveTargetList.GetCount() / 4;

	// 击中的总不目标数 
	int nTotalTargetCount = nTargetCount + nMoveTargetCount;
	// 击中目标数保护
	int nServerHitCount = pSkillMain->GetHitCount();
	if (nServerHitCount < nTotalTargetCount)
	{
		return false; 
	}
	
	// 有目标时再响应命中前处理
	if (nTotalTargetCount > 0)
	{
		pKernel->Command(self, self, CVarList() << COMMAND_SKILL_BEFORE_HIT << skill);
		SkillEventManager::SkillBeforeHit(pKernel, self, skill, nSkilluid);
	}

	// 客户端命中的消息可能技能流程结束之后,这时玩家的FIGHTER_CUR_SKILL已经清空,在这里重新设置下
	PERSISTID curskill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
	if (curskill.IsNull())
	{
		pSelfObj->SetObject(FIGHTER_CUR_SKILL, skill);
	}

	// 结算无位移的目标
	LoopBeginCheck(a)
	for (int i = 0;i < nTargetCount;++i)
	{
		LoopDoCheck(a)
		// 取得目标并处理命中逻辑
		PERSISTID target = targetList.ObjectVal(i);
		OnHitOneTarget(pKernel, target, skill, pSelfObj, pSkillMain, nSkilluid);
	}

	// 结算有位移的目标
	LoopBeginCheck(d);
	int nArgsNum = (int)moveTargetList.GetCount();
	for (int i = 0;i < nArgsNum;i += 4)
	{
		LoopDoCheck(d);
		// 取得目标并处理命中逻辑
		PERSISTID target = moveTargetList.ObjectVal(i);
		if(!pKernel->Exists(target))
		{
			continue;
		}

		// 取得目标的位移数据并处理位移
		float fPosX = moveTargetList.FloatVal(i + 1);
		float fPosZ = moveTargetList.FloatVal(i + 2);
		float fSpeed = moveTargetList.FloatVal(i + 3);
		
		fSpeed = __min(fSpeed, MAX_HIT_FLOAT_SPEED);

		OnHitOneTarget(pKernel, target, skill, pSelfObj, pSkillMain, nSkilluid);
		if(!pKernel->Exists(target))
		{
			continue;
		}
		// 验证位移的有效性
		if(!SkillAccessSingleton::Instance()->IsValidDisplace(pKernel, self, strSkillId, DISPLACE_TARGET_TYPE, fPosX, fPosZ) || fSpeed <= 0.0f)
		{
			continue;
		}

		bool bMotionSuc = pKernel->MotionNoRotate(target, fSpeed, fPosX, fPosZ, MOTION_MODE_HITBACK);
		if (!bMotionSuc)
		{
			char strInfo[128] = {0};
			SPRINTF_S(strInfo, "DoSkillHitByClient skillid = %s, PosX=%f, PosZ=%f, Speed=%f", strSkillId, fPosX, fPosZ, fSpeed);
			::extend_warning(LOG_INFO, strInfo);
		}
	}
	return 0;
}

// 是否是技能流程
bool SkillFlow::IsSkillFlow(IKernel* pKernel, const PERSISTID& self)
{
    if (FLOW_CATEGORY_SKILL == FlowModule::m_pInstance->QueryCategory(pKernel, self)) 
    {
        return true; 
    }

    return false;
}

//是否正在引导期
bool SkillFlow::IsLeading(IKernel* pKernel, const PERSISTID& self)
{
    if (FlowModule::m_pInstance->IsLeading(pKernel, self, FLOW_CATEGORY_SKILL))
    {
        return true;
    }

    if (FlowModule::m_pInstance->IsHiting(pKernel, self, FLOW_CATEGORY_SKILL))
    {
        return true;
    }

    return false;
}

// 是否在准备阶段中
bool SkillFlow::IsPreparing(IKernel* pKernel, const PERSISTID& self)
{
    return FlowModule::m_pInstance->IsPreparing(pKernel, self, FLOW_CATEGORY_SKILL);
}

// 是否暂停了技能流程
// bool SkillFlow::IsPause(IKernel* pKernel, const PERSISTID& self)
// {
//     return FlowModule::m_pInstance->IsPause(pKernel, self, FLOW_CATEGORY_SKILL);
// }

// 是否正在命中期
bool SkillFlow::IsHiting(IKernel* pKernel, const PERSISTID& self)
{
    return FlowModule::m_pInstance->IsHiting(pKernel, self, FLOW_CATEGORY_SKILL);
}


// 打断技能
bool SkillFlow::BreakSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& breaker, bool bForce, int iBreakReason)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    //不在技能流程中
    if (!IsSkillFlow(pKernel, self))
    {
        return false;
    }

    PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    if (!pKernel->Exists(skill))
    {
        return false;
    }

    bool bbreak = bForce;
    if (!bbreak)
    {
        if (IsPreparing(pKernel, self) || IsLeading(pKernel, self))
        {
            bbreak = true;
        }
    }

    if (bbreak)
    {
        if (!pSelfObj->FindData("SkillBreakReason"))
        {
            pSelfObj->AddDataInt("SkillBreakReason", 0);
        }
        pSelfObj->SetDataInt("SkillBreakReason", iBreakReason);

        bool ret = FlowModule::m_pInstance->BreakFlow(pKernel, self, FLOW_CATEGORY_SKILL);

        return ret;
    }

    //未能打断，增加技能的准备时间
    return false;
}

//检查技能的基本使用条件
int  SkillFlow::CheckForceUseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    int result = USESKILL_RESULT_ERR_OTHER;
    do
    {
        IGameObj* pSelfObj = pKernel->GetGameObj(self);
        if (NULL == pSelfObj)
        {
            break;
        }
        if (!pKernel->Exists(skill))
        {
            break;
        }

        //判断玩家是否正在释放技能
        result = IsSkillUseBusy(pKernel, self);
        //打断当前正在使用的技能，使用下一个技能
        if (USESKILL_RESULT_SUCCEED != result)
        {
            PERSISTID curSkill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
			const SkillMainData* pData = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
			if (NULL == pData)
			{
				break;
			}
			// 打断技能
			BreakSkill(pKernel, self, self, true, SKILL_BREAK_REASON_TYPE_NEW_SKILL);
			result = USESKILL_RESULT_SUCCEED;
        }
    }
    while (0);

    return result;
}

//判断是否在正在使用技能
int SkillFlow::IsSkillUseBusy(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

    // 判断是否正在命中过程中
    if (IsHiting(pKernel, self))
    {
        return USESKILL_RESULT_ERR_HITING_BUSY;
    }

    // 判断是否正在引导技能
    if (IsLeading(pKernel, self))
    {
        return USESKILL_RESULT_ERR_LEADING_BUSY;
    }

    // 判断是否正在准备技能（技能不在Pause状态）
    if (IsPreparing(pKernel, self))
    {
        return USESKILL_RESULT_ERR_PREPARE_BUSY;
    }

	// 判定是否在使用技能硬直状态中
	if (pKernel->FindHeartBeat(self, "SkillFlow::H_SkillStiffStateEnd"))
	{
		return USESKILL_RESULT_ERR_STIFF_STATE;
	}

    return USESKILL_RESULT_SUCCEED;
}

//////////////////////////////////////////////////////////////////////////
//开始使用技能
void SkillFlow::BeginUseSkill(IKernel* pKernel, const PERSISTID& self,
                              float x, float y, float z, float orient, const PERSISTID& skill,
                              const PERSISTID& target, float dx, float dy, float dz)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(skill))
    {
        return ;
    }

    // 获取当前技能唯一ID
    int64_t iSkillUUID = (int64_t)::util_gen_unique_id();

    // 技能释放前，初始化信息设置
    InitBeginUseSkillInfo(pKernel, self, skill, iSkillUUID, target, dx, dy, dz);

    // 发送技能开始的命令
    pKernel->Command(self, self, CVarList() << COMMAND_SKILL_BEGIN << skill);

    // 发送技能开始的客户消息
    PrepareUseSkill(pKernel, self, skill, iSkillUUID, target, dx, dy, dz, orient);

    // 开始技能流程
    BeginSkillFlow(pKernel, self);

    return;
}

// 技能释放前的信息设置
void SkillFlow::InitBeginUseSkillInfo(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& skill, int64_t iSkillUUID,
                                      const PERSISTID& target, float dx, float dy, float dz)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return ;
    }

    // 开始做技能的准备
    pSkillObj->SetInt(SKILL_CUR_HITS, 0);//技能当前打击次数
    pSelfObj->SetObject(FIGHTER_CUR_SKILL, skill);//目前使用技能
    pSelfObj->SetObject(FIGHTER_CUR_SKILL_TARGET, target);
    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_X, dx);
    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_Y, dy);
    pSelfObj->SetFloat(FIGHTER_CUR_SKILL_TARGET_Z, dz);
    pSelfObj->SetInt64(FIGHTER_CUR_SKILL_UUID, iSkillUUID);
	pSelfObj->SetInt64(FIGHTER_CUR_SKILL_START_TIME, ::util_get_time_64());

	// 客户端命中验证初始化 
	InitClientValidData(pKernel, self, skill);
}


void SkillFlow::PrepareUseSkill(IKernel* pKernel, const PERSISTID& self,
                                       const PERSISTID& skill, int64_t iSkillUUID,
                                       const PERSISTID& target, 
                                       float dx, float dy, float dz, float orient)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill); 
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return ;
    }

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
    if (NULL == pSkillMain)
    {
        return;
    }
    
    int iUseType = SkillAccessSingleton::Instance()->GetUseSkillType(pKernel, skill);

    // 通知客户端开始使用技能
    ESkillStageTargetType eSkillStageTargetType = SKILL_STAGE_TARGET_TYPE_NONE;
    CVarList params;
    if (USESKILL_TYPE_TARGET == iUseType)
    {
        eSkillStageTargetType = SKILL_STAGE_TARGET_TYPE_TARGET;
        params << target;
    }
    else
    {
        eSkillStageTargetType = SKILL_STAGE_TARGET_TYPE_POSITION;
        params << dx << dz;
    }

	const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
	int nCoolDownTime = ComputeSkillCoolTime(pKernel, self, skill);

    SkillStageSingleton::Instance()->CustomSkillBeginStage(pKernel, self, szSkillConfigID, iSkillUUID, eSkillStageTargetType, params);

	// 处理耗蓝
	EnergyModule::UpdateEnergy(pKernel, self, ENERGY_MP, pSkillMain->GetConsumeMP());
}


//开始使用技能分步
void SkillFlow::BeginSkillFlow(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return ;
    }

    const PERSISTID skill = pSelfObj->QueryObject(FIGHTER_CUR_SKILL);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return;
    }

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL == pSkillMain)
	{
		return;
	}

    int iPrepareTime = 0, iLeadTime = 0, iLeadSepTime = 0;

    // 获取当前的配置时间
    GetSkillCollocateTime(pKernel, self, skill, pSkillMain, iPrepareTime, iLeadTime, iLeadSepTime);

    if (pSkillMain->GetIsLead() && iLeadTime > 0)
    {
        int iHitTime = pSkillMain->GetHitTime();

        // 引导技能
        pSkillObj->SetInt(SKILL_HURT_COUNT, 1);
        FlowModule::m_pInstance->BeginLeadFlow(pKernel, self, FLOW_CATEGORY_SKILL, iPrepareTime, iLeadTime, iLeadSepTime, iHitTime);
        return ;
    }

    // 多次命中的流程
    CVarList vHitTimeList;
    if (!GetMultipleFlowRealHitTime(pKernel, self, pSkillObj, pSkillMain, vHitTimeList))
    {
        return;
    }
    //开始流程
    pSkillObj->SetInt(SKILL_HURT_COUNT, (int)vHitTimeList.GetCount());
    FlowModule::m_pInstance->BeginFlow(pKernel, self, FLOW_CATEGORY_SKILL, iPrepareTime, vHitTimeList);
    return;
}

// 获取蓄力，准备，引导时间
bool SkillFlow::GetSkillCollocateTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,const SkillMainData* pSkillMain,
										int& iPrepareTime, int& iLeadTime, int& iLeadSepTime)
{
    if (NULL == pSkillMain)
    {
        return false;
    }

    if (pSkillMain->GetIsPrepare())
    {
        iPrepareTime = pSkillMain->GetPrepareTime();
    }
    if (pSkillMain->GetIsLead())
    {
//		int nLeadTimeDiff = SkillDataQueryModule::m_pInstance->GetSkillTimeDiff(pKernel, self, LEAD_TIME_DIFF);

		// 被动技能影响的持续时间
// 		int nPSCooldowntime = 0;
// 		CVarList args;
// 		if (SkillDataQueryModule::m_pInstance->QueryPassiveSkillEffect(pKernel, args, self, skill, PS_CHANGE_SKILL_CONFIG, SKILL_LEAD_TIME_NAME))
// 		{
// 			nPSCooldowntime = args.IntVal(0);
// 		}

        //iLeadTime = __max(pSkillMain->GetLeadTime() - nLeadTimeDiff + nPSCooldowntime, 0);
		iLeadTime = pSkillMain->GetLeadTime();
        iLeadSepTime = pSkillMain->GetLeadSepTime();
    }

    return true;
}

// 获得多命中流的命中时间列表
bool SkillFlow::GetMultipleFlowRealHitTime(IKernel* pKernel, const PERSISTID& self,  IGameObj* pSkill, const SkillMainData *pSkillMain, IVarList& hittimelist)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj || NULL == pSkillMain || NULL == pSkill)
    {
        return false;
    }
	
	// 多次命中时间
	const SkillDataQueryModule::HitTimeDataVec* pHitTimeData = SkillDataQueryModule::m_pInstance->QueryHitTimeList(pSkillMain->GetID());
	if (NULL == pHitTimeData)
	{
		// 命中时间差
		//int nHitTimeDiff = SkillDataQueryModule::m_pInstance->GetSkillTimeDiff(pKernel, self, HIT_TIME_DIFF);
		// 计算命中时间
		//int iHitTime = __max(pSkillMain->GetHitTime() - nHitTimeDiff, 0);

		hittimelist << pSkillMain->GetHitTime();
	}
	else
	{
		const SkillDataQueryModule::HitTimeDataVec& vecHitTimeData = *pHitTimeData;

		// 命中时间差
		//int nHitTimeDiff = SkillDataQueryModule::m_pInstance->GetSkillTimeDiff(pKernel, self, HIT_TIME_DIFF);
		size_t count = vecHitTimeData.size();
		// 配置多次命中的用多次命中时间,不存在用hittime
		if (count > 0)
		{
			LoopBeginCheck(b);
			for (size_t i = 0; i < count; i++)
			{
				LoopDoCheck(b);
				int nTime = vecHitTimeData[i].nHitTime;
				//int iExHitTime = __max(nTime - nHitTimeDiff, 0);
				hittimelist << nTime;
			}
		}
	}
	
    return true;
}

// 更新玩家位置
void SkillFlow::UpdateObjectLocate(IKernel* pKernel,
                                        const PERSISTID& self,
										const PERSISTID& target,
										const PERSISTID& skill,
                                        float x, float y, float z, float dorient)
{
	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pSkillMain)
	{
		return ;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}
	if (pKernel->Type(self) == TYPE_PLAYER)
    {
		float server_x = pKernel->GetPosiX(self);
		float server_z = pKernel->GetPosiZ(self);		

		const float ALLOW_DIST_OFFSET_SQRT = 5.0f * 5.0f;
		//验证偏移并更新位置
		if (util_dot_distance2(server_x, server_z, x, z) < ALLOW_DIST_OFFSET_SQRT)
		{
			pKernel->Locate(self, x, y, z, dorient);
		}

		// 
    }
}

// 增加硬直状态
void SkillFlow::AddSkillStiffState(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}
	// 在线玩家不用添加
	if(pSelfObj->GetClassType() == TYPE_PLAYER && 1 == pSelfObj->QueryInt("Online"))
	{
		return;
	}
	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pSkillMain)
	{
		return;
	}
	int nCastStiffTime = pSkillMain->GetCastStiffTime();
	if (nCastStiffTime <= 0)
	{
		return;
	}
	ADD_COUNT_BEAT(pKernel, self, "SkillFlow::H_SkillStiffStateEnd", nCastStiffTime, 1);
}

// 获取作用范围内的目标
bool SkillFlow::GetHitRangeTargets(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
                                   float x, float y, float z, int nHitIndex, IVarList& result)
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
	//是否用自己的坐标选目标 
// 	if (pSkillMain->GetIsUseSelfPos())
// 	{
// 		x = pSelfObj->GetPosiX();
// 		y = pSelfObj->GetPosiY();
// 		z = pSelfObj->GetPosiZ();
// 	}

	// 每次命中范围由多个改为一个,
	// 技能范围的数组改为多次命中时,每次命中对应的范围 [3/6/2015 liumf]
    SkillHitRangeParams HitRangeParams;
    if (!SkillAccessSingleton::Instance()->GetSkillHitRange(pKernel, self, skill, nHitIndex, HitRangeParams))
    {
        return false;
    }

    FilterConditionParamStruct filterCondition;
    filterCondition.iRelationType = HIT_TARGET_RELATION_TYPE_NONE;
    filterCondition.iHitTargetType = HIT_TARGET_TYPE_NONE;
    if (!SkillAccessSingleton::Instance()->GetSkillHitTargetRelation(pKernel, skill,
        filterCondition.iRelationType, filterCondition.iHitTargetType))
    {
        return false;
    }

    PERSISTID locktarget;
    if (pSkillMain->GetIsHitSelf())
    {
        locktarget = self;
    }
    else
    {
        locktarget = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);
    }

    // 是否需要对结果由远及近排序
    filterCondition.bSortByDist = pSkillMain->GetIsByDist();
    filterCondition.bSortByHP = pSkillMain->GetIsByHP();
    filterCondition.bIncludeDead = pSkillMain->GetIncludeDead();
    int count = pSkillMain->GetHitCount();//
    // 判断在下面的作用范围集合中的目标
    CVarList vTotalTargets;

    TargetFilter::GetHitRangeTargets(pKernel, self, FmVec3(x, y, z), locktarget, HitRangeParams, filterCondition, count, vTotalTargets);

	// 确认目标是否存在
	LoopBeginCheck(c)
    for (size_t i = 0; i < vTotalTargets.GetCount(); ++i)
    {
		LoopDoCheck(c)
        PERSISTID tmpobj = vTotalTargets.ObjectVal(i);
        if (!pKernel->Exists(tmpobj))
        {
            continue;
        }
        bool bExistsFinalResult = false;
		LoopBeginCheck(d)
        for (size_t j = 0; j < result.GetCount(); ++j)
        {
			LoopDoCheck(d)
            PERSISTID existsObj = result.ObjectVal(j);
            if (tmpobj == existsObj)
            {
                bExistsFinalResult = true;
                break;
            }
        }
        if (!bExistsFinalResult)
        {
            result << tmpobj;
        }

        if ((int)result.GetCount() >= count)
        {
            break;
        }
    }
    return true;
}

//冷却处理
int SkillFlow::ComputeSkillCoolTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return 0;
    }

    //开始进行冷却处理
    int iPubCoolTime = 0;
    int iCoolDownTime = 0;
    int iCoolCategory = 0;//分类冷却
    SkillAccessSingleton::Instance()->GetCoolDownTime(pKernel, self, skill, iPubCoolTime, iCoolDownTime, iCoolCategory);
	
	// 公共CD
	CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, COOLDOWN_CATEGOTYID_SKILL, iPubCoolTime);
    
    CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, iCoolCategory, iCoolDownTime);
	return iCoolDownTime;
}

// 客户端命中验证初始化
void SkillFlow::InitClientValidData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSelfObj || NULL == pSkillObj || pSelfObj->GetClassType() != TYPE_PLAYER)
	{
		return;
	}
	// 记录在线玩家技能使用的开始时间
	if (!pSelfObj->FindData("SkillBeginTime"))
	{
		pSelfObj->AddDataInt64("SkillBeginTime", util_get_time_64());
	}
	else
	{
		pSelfObj->SetDataInt64("SkillBeginTime", util_get_time_64());
	}
	// 当前技能的打击次数
	if (!pSkillObj->FindData("ClientHitsIndex"))
	{
		pSkillObj->AddDataInt("ClientHitsIndex", 0);
	}
	else
	{
		pSkillObj->SetDataInt("ClientHitsIndex", 0);
	}
}

// 清除使用技能过程中设置的相关属性
bool SkillFlow::ClearUseSkillData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }
	// 重置前一个技能的uid
	int64_t nPreSkillUID = pSelfObj->QueryInt64(FIGHTER_CUR_SKILL_UUID);
	pSelfObj->SetInt64(FIGHTER_PRE_SKILL_UUID, nPreSkillUID);

    pSelfObj->SetObject(FIGHTER_CUR_SKILL, PERSISTID());
    pSelfObj->SetInt64(FIGHTER_CUR_SKILL_UUID, 0);

    pSelfObj->SetObject(FIGHTER_CUR_SKILL_TARGET, PERSISTID());

    //恢复转向速度
    if (pSelfObj->FindData("BackupRollSpeed"))
    {
        float fBackupRollSpeed =pSelfObj->QueryDataFloat("BackupRollSpeed");
        if (fBackupRollSpeed < 0.001f) //避免转速为0
        {
            fBackupRollSpeed = PI2;
        }
        pSelfObj->SetFloat("RollSpeed", fBackupRollSpeed);
        pSelfObj->RemoveData("BackupRollSpeed");
    }

	pSkillObj->SetInt(SKILL_CUR_HITS, 0);//技能当前打击次数
    return true;
}

// 技能硬直状态结束,有这个心跳的对象只能播放技能,不能做其他AI行为
int SkillFlow::H_SkillStiffStateEnd(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	DELETE_HEART_BEAT(pKernel, creator, "SkillFlow::H_SkillStiffStateEnd");
	pKernel->Command(creator, creator, CVarList() << COMMAND_NPC_STIFF_OVER);
	return 0;
}

// 结算命中一个目标
void SkillFlow::OnHitOneTarget(IKernel* pKernel, PERSISTID target, const PERSISTID& skill, IGameObj* pSelfObj, const SkillMainData* pSkillMain, const int64_t nSkilluid)
{
	IGameObj* pTarget = pKernel->GetGameObj(target);
	if (NULL == pTarget || NULL == pSkillMain || NULL == pSelfObj || !pKernel->Exists(skill))
	{
		return;
	}

	PERSISTID self = pSelfObj->GetObjectId();
	// 验证目标是否可攻击
	if (!SkillUseCheckerSingleton::Instance()->CheckSkillRelation(pKernel, self, skill, target))
	{
		return;
	}

	pSelfObj->SetObject(FIGHTER_CUR_SKILL_TARGET, target);
	// 施法距离验证
	// 与释放者的最大距离
// 	int nValidDis = pSkillMain->GetValidDistance();
// 
// 	float fValidDis = (float)(nValidDis * nValidDis);
// 	// 计算施法者与目标的距离
// 	float fRealDis2 = util_dot_distance2(pTarget->GetPosiX(), pTarget->GetPosiZ(), pSelfObj->GetPosiX(), pSelfObj->GetPosiZ());
// 
// 	// 验证通过进行命中
// 	if (fRealDis2 <= fValidDis)
// 	{
		SkillEventManager::SkillAfterHit(pKernel, self, skill, nSkilluid, target);
		FightModule::m_pInstance->SkillDamageTarget(pKernel, self, target, skill, nSkilluid);
// 	}
// 	else
// 	{
// 		extend_warning(LOG_INFO, "Skill is %s, InValid is Distance, RealDis2:%f, ValidDis:%f", pSkillMain->GetID(), fRealDis2, fValidDis);
// 	}
}