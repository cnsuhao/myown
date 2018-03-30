//------------------------------------------------------------------------------
// 文件名:      SkillStage.cpp
// 内  容:      技能各阶段消息发送
// 说  明:      该类只在技能系统内部使用，且不要直接定义SkillStage的实例，而使用
//              其单例模式接口SkillStageSingleton::Instance()
// 创建日期:    2014年10月23日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------


#include "FsGame/skillmodule/impl/SkillStage.h"

#include <cmath>
#include <cassert>
#include <string>
#include "FsGame/define/ServerCustomDefine.h"
#include "FsGame/Define/SkillDefine.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/util_macros.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "../../Define/CommandDefine.h"
#include "SkillFlow.h"
#include "Skillaccess.h"
#include "../../Interface/PropertyInterface.h"
#include "../../NpcBaseModule/AI/AIDefine.h"
#include "utils/string_util.h"

//////////////////////////////////////////////////
// 开始
bool SkillStage::CustomSkillBeginStage(IKernel* pKernel,
                                       const PERSISTID& self, 
                                       const char* szSkillConfigID,
                                       const int64_t uuid,
                                       const ESkillStageTargetType eSkillStageTargetType,
                                       const IVarList& params)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_BEGIN << eSkillStageTargetType << uuid << szSkillConfigID << self;

	int count = (int)params.GetCount();

	switch (eSkillStageTargetType)
	{
	case SKILL_STAGE_TARGET_TYPE_TARGET:          //目标：PERSISTID
		{
			if (count < 1)
			{
				return false;
			}
			msg << params.ObjectVal(0);
		}
		break;

	case SKILL_STAGE_TARGET_TYPE_POSITION:        //位置：x,y,z
		{
			if (count < 2)
			{
				return false;
			}

			//压缩坐标扩大10倍，客户端误差值0.1
			msg << util_compress_int2((int)(params.FloatVal(0)*10), (int)(params.FloatVal(1)*10));
		}
		break;

	default:
		::extend_warning(LOG_WARNING,
			"[SkillStage::CustomSkillResetDest] unknown SKILL_STAGE_TARGET_TYPE: %d !", eSkillStageTargetType);
		break;
	}

	SafeCustomByKen(pKernel, self, msg);
	return true;
}

// 吟唱
bool SkillStage::CustomSkillPrepareStage(IKernel* pKernel,
                                         const PERSISTID& self,
                                         const int64_t uuid,
                                         const int iPrepareTime)
{
    if (!pKernel->Exists(self))
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_PREPARE << uuid << iPrepareTime;

	SafeCustomByKen(pKernel, self, msg);
    return true;
}

// 引导
bool SkillStage::CustomSkillLeadStage(IKernel* pKernel,
                                      const PERSISTID& self,
                                      const int64_t uuid, 
                                      const int iLeadTime)
{
    if (!pKernel->Exists(self))
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_LEAD << uuid << iLeadTime;

    SafeCustomByKen(pKernel, self, msg);
    return true;
}

// 立即发送效果，不合并消息数据
bool SkillStage::CustomSkillEffectStageRealTime(IKernel* pKernel,
                                                const PERSISTID& self,                      // 攻击者
                                                const int64_t uuid,                         // 技能uuid
                                                const ESkillEventType eEventType,           // 技能事件类型
                                                const PERSISTID& target,                    // 效果作用对象 
                                                const ESkillStageEffectType eEffectType,    // 效果效果类型ID
                                                const IVarList& params,                     // 效果所需发送的数据
                                                const EMessageSendType eMsgSendType)        // 消息发送给哪些对象
{
    CVarList outMsgParams;
	outMsgParams << target << params;

	PERSISTID selfMaster = get_pet_master(pKernel, self);
    switch (eMsgSendType)
    {
    case EMSG_SEND_TYPE_ALL:
        return CustomSkillEffectStage(pKernel, selfMaster, uuid, util_compress_int4(eEventType, eEffectType), outMsgParams);
    case EMSG_SEND_TYPE_SELF:
        {
            return CustomSkillEffectStageBySelf(pKernel, selfMaster, uuid, util_compress_int4(eEventType, eEffectType), outMsgParams);
        }
    case EMSG_SEND_TYPE_SELF_TARGET:
        {
            
            PERSISTID targetMaster = get_pet_master(pKernel, target);
            CustomSkillEffectStageBySelf(pKernel, selfMaster, uuid, util_compress_int4(eEventType, eEffectType), outMsgParams);
			if (selfMaster != targetMaster)
			{
				CustomSkillEffectStageBySelf(pKernel, targetMaster, uuid, util_compress_int4(eEventType, eEffectType), outMsgParams);
			}
            return true;
        }
    default:
        break;
    }

    return false;
}

// 消息只发给自己
bool SkillStage::CustomSkillEffectStageBySelf(IKernel* pKernel,
                                              const PERSISTID& self,              // 攻击者
                                              const int64_t uuid,                 // 技能uuid
													const int iCompressValue,			 // 技能事件和特效类型合并值
                                              const IVarList& params)             // eEventType类型的所有效果的参数，每个效果的参数需要按照这样的格式存放信息
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    if (pSelfObj->GetClassType() != TYPE_PLAYER)
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_EFFECT << iCompressValue << uuid << params;

	SafeCustom(pKernel, self, msg);
    return true;
}

bool SkillStage::CustomSkillEffectStage(IKernel* pKernel,
                                        const PERSISTID& self,              // 攻击者
                                        const int64_t uuid,                 // 技能uuid
                                        const int iCompressValue,           // 技能事件和特效类型合并值
                                        const IVarList& params)             // eEventType类型的所有效果的参数，每个效果的参数需要按照这样的格式存放信息
{
    if (!pKernel->Exists(self))
    {
        return false;
    }
    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_EFFECT << iCompressValue << uuid << params;

	SafeCustomByKen(pKernel, self, msg);
    return true;
}

// 打断
bool SkillStage::CustomSkillBreakStage(IKernel* pKernel,
                                       const PERSISTID& self,
                                       const int64_t uuid,
                                       const int reason)
{
    if (!pKernel->Exists(self))
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_BREAK << uuid;

    SafeCustomByKen(pKernel, self, msg);
    return true;
}

// 结束
bool SkillStage::CustomSkillFinish(IKernel* pKernel,
                                   const PERSISTID& self,
                                   const int64_t uuid,
                                   const int reason)
{
    if (!pKernel->Exists(self))
    {
        return false;
    }

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_FINISH << uuid;

    SafeCustomByKen(pKernel, self, msg);
    return true;
}
 
// 处理技能空放 朝向可能与客户端不同步 用客户端的朝向
int SkillStage::CustomSkillWithoutTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float fOrient)
{
	// 检查释放可以打断正在流程当中使用的技能
	int result = SkillFlowSingleton::Instance()->CheckForceUseSkill(pKernel, self, skill);

	// 判断返回值 
	if (USESKILL_RESULT_SUCCEED != result)
	{
		return result; 
	}

	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSkillObj || NULL == pSelfObj)
	{
		return 0;
	}

	const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
	int iPubCoolTime = 0;
	int iCoolDownTime = 0;
	int iCoolCategory = 0;//分类冷却
	SkillAccessSingleton::Instance()->GetCoolDownTime(pKernel, self, skill, iPubCoolTime, iCoolDownTime, iCoolCategory);

	CVarList msg;
	msg << SERVER_CUSTOMMSG_SKILL << SKILL_STAGE_TYPE_BEGIN << util_compress_int4(SKILL_STAGE_TARGET_TYPE_NONE) << 0 << szSkillConfigID << self << fOrient;

	pKernel->Command(self, self, CVarList() << COMMAND_SKILL_BEGIN << skill);
	SafeCustomByKen(pKernel, self, msg);
	return 0;
}

// 发送播放击飞效果
// int SkillStage::CustomSkillHitFly(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,const PERSISTID& skill)
// {
// 	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	IGameObj* pTargetObj = pKernel->GetGameObj(target);
// 	if (NULL == pSkillObj || NULL == pSelfObj || NULL == pTargetObj)
// 	{
// 		return 0;
// 	}
// 
// 	const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
// 	if (NULL == pMainData)
// 	{
// 		return 0;
// 	}
// 	// 技能是否具有击飞效果
// 	if (!pMainData->GetIsCanHitFly())
// 	{
// 		return 0;
// 	}
// 	
// 	// 只有玩家杀死npc才播放击飞效果
// 	if (pSelfObj->GetClassType() != TYPE_PLAYER || pTargetObj->GetClassType() != TYPE_NPC)
// 	{
// 		return 0;
// 	}
// 
// 	// 只有普通怪才能被击飞
// 	int nNpcType = pTargetObj->QueryInt("NpcType");
// 	if (nNpcType != NPC_TYPE_ORDINARY)
// 	{
// 		return 0;
// 	}
// 
// 	// NPC是否死亡
// 	int nDead = 0;
// 	if (FPropertyInstance->GetDead(pTargetObj, nDead) && 1 == nDead)
// 	{
// 		// 发送击飞消息
// 		CVarList msg;
// 		msg << SERVER_CUSTOMMSG_SKILL_DEAD_EFFECT << target;
// 		SafeCustom(pKernel, self, msg);
// 	}
// 	return 0;
// }

// 客户端命中失败返回 仅测试用
/*!
	 * @brief	客户端命中失败消息,仅为测试使用
	 * @param	wstring 施法者名字
	 * @param	string 技能id
	 * @param	int 失败原因(0 时间验证失败 1命中索引验证失败 2距离验证失败)
	 * @param	int 命中目标个数n
	 以下循环n次
	 *  @param	PERSISTID 目标
	 */
int SkillStage::CustomSkillClientHitFailed(IKernel* pKernel, const PERSISTID& self, const char* strSkillId, int nFailedType, const IVarList& targetList)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf || StringUtil::CharIsNull(strSkillId))
	{
		return 0;
	}
	CVarList msgSend;
	const wchar_t* wsPlayerName = pSelf->QueryWideStr("Name");
	float x = pSelf->GetPosiX();
	float z = pSelf->GetPosiZ();
	msgSend << SERVER_CUSTOMMSG_SKILL_HIT_FAILED << wsPlayerName << strSkillId << nFailedType << x << z;
	int nCount = (int)targetList.GetCount();
	msgSend << nCount;
	LoopBeginCheck(c)
	for (int i = 0;i < nCount;++i)
	{
		LoopDoCheck(c)
		PERSISTID target = targetList.ObjectVal(i);
		IGameObj* pTargetObj = pKernel->GetGameObj(target);
		if (pTargetObj == NULL)
		{
			continue;
		}
		int nType = pTargetObj->QueryInt("NpcType");
		x = pTargetObj->GetPosiX();
		z = pTargetObj->GetPosiZ();
		msgSend << targetList.ObjectVal(i) << nType << x << z;
	}

	SafeCustom(pKernel, self, msgSend);
	return 0;
}