//--------------------------------------------------------------------
// 文件名:       SkillEventManager.cpp
// 内  容:        技能事件包管理
// 说  明:
// 创建日期:    2014年11月25日
// 创建人:		 
//    :       
//--------------------------------------------------------------------
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/SkillModule/inner/SkillEventBase.h"

#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "FsGame/Define/FightDefine.h"
#include "FsGame/Define/SkillDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "FsGame/SkillModule/impl/SkillStage.h"
#include "../../Define/CommandDefine.h"
#include "../../Define/GameDefine.h"
#include "../../Define/ClientCustomDefine.h"
#include "../../Define/ServerCustomDefine.h"
#include "../../Interface/PropertyInterface.h"
#include "../Impl/Skillaccess.h"
#include "utils/custom_func.h"
#include "../../NpcBaseModule/AI/AIDefine.h"

SkillEventManager* SkillEventManager::m_pInstance = NULL;

// 恢复移动间隔
//static const int RESTORE_MOVE_INTERVAL = 500;

//目标筛选
inline bool FixSkillTarget(IKernel* pKernel, const PERSISTID& self, PERSISTID& target, const ETargetType eTargetType)
{
    // target_type强制目标类型
    /*
    0传入目标
    2自已
    */

    if (TARGET_SEND != eTargetType)
    {
        if (TARGET_SELF_OBJ == eTargetType)
        {
            if (self != target)
            {
                target = self;
            }
            return true;
        }

        return false;
    }

    return true;
}

// 初始化
bool SkillEventManager::Init(IKernel* pKernel)
{
    m_pInstance = this;

	LoopBeginCheck(a)
	for (int i = 0;i < MAX_EXECUTE;++i)
	{
		LoopDoCheck(a)
		m_pEventContainer[i] = NULL;
	}

    InitEventExecutes(pKernel);

	LoopBeginCheck(b)
	for (int i = 0;i < MAX_EXECUTE;++i)
	{
		LoopDoCheck(b)
		SkillEventBase* pEvent = m_pEventContainer[i];
		if (NULL != pEvent)
		{
			pEvent->OnInit(pKernel);
		}
	}
    return true;
}

// 关闭
bool SkillEventManager::Shut(IKernel* pKernel)
{
	LoopBeginCheck(c)
	for (int i = 0;i < MAX_EXECUTE;++i)
	{
		LoopDoCheck(c)
		SkillEventBase* pEvent = m_pEventContainer[i];
		if (NULL != pEvent)
		{
			SAFE_DELETE(pEvent);
		}
	}
    return true;
}

// 处理单个执行操作, 事件函数的iEventType对于技能来说是ESkillEventType，对于buffer来说是EBufferEventType
bool SkillEventManager::ExecuteOperate(IKernel* pKernel,
							  const EEventExecType funcid,
							  const IVarList& OpParams,
							  const PERSISTID& self,
							  const PERSISTID& object,
							  const int64_t uuid,
							  const int iEventType,
							  const IVarList& params,
							  IVarList& outMsgParams)
{
	// 合理保护
	if (funcid <= EVENT_EXEC_INVALID || funcid >= MAX_EXECUTE)
	{
		return false;
	}

	if (NULL == m_pEventContainer[funcid])
	{
		return false;
	}

	static int s_count = 0;

	if (s_count >= 3)
	{
		// 事件递归调用次数过多，SkillEventManager::ExecuteOperate -- 技能或buf ID -- 事件号
		std::string info = "[ERROR MAX EXECUTE] SkillEventManager::ExecuteOperate -- ";
		info += pKernel->GetConfig(object);
		info += " -- ";
		info += StringUtil::IntAsString(funcid);
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	s_count += 1;

	// 执行特殊的技能或buff效果
	m_pEventContainer[funcid]->Execute(pKernel,
										OpParams,
										self,
										object,
										uuid,
										iEventType,
										params,
										outMsgParams);

	s_count -= 1;

	return true;
}


// 刷新技能之间相互影响的属性[无]
bool SkillEventManager::SkillBegin(IKernel* pKernel,
                                   const PERSISTID& self, 
                                   const PERSISTID& skill, 
                                   int64_t iSkillUUID,
                                   const PERSISTID& target,
                                   float dx, float dy, float dz)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, iSkillUUID, target,
                             SKILL_BEGIN, EPARAMTYPE_TARGET_XYZ,
                             CVarList() << dx << dy << dz);
}


// 技能开始准备[单]
bool SkillEventManager::SkillPrepare(IKernel* pKernel, 
                                     const PERSISTID& self, 
                                     const PERSISTID& skill, 
                                     const int64_t uuid,
                                     const PERSISTID& target, 
                                     float x, float y, float z)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
                             SKILL_PREPARE, EPARAMTYPE_TARGET_XYZ, 
                             CVarList() << x << y << z);
}

// 技能准备之后// [单]
bool SkillEventManager::SkillAfterPrepare(IKernel* pKernel,
                                          const PERSISTID& self,
                                          const PERSISTID& skill, 
                                          const int64_t uuid, 
                                          const PERSISTID& target,
                                          float x, float y, float z)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
                             SKILL_AFTER_PREPARE, EPARAMTYPE_TARGET_XYZ,
                             CVarList() << x << y << z);
}

// 技能命中之前[单]
bool SkillEventManager::SkillBeforeHit(IKernel* pKernel,
                                       const PERSISTID& self,
                                       const PERSISTID& skill,
                                       const int64_t uuid)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, PERSISTID(),
                             SKILL_BEFORE_HIT, EPARAMTYPE_TARGET, CVarList());
}

// 技能命中对象[多]
bool SkillEventManager::SkillAfterHit(IKernel* pKernel,
                                      const PERSISTID& self,
                                      const PERSISTID& skill,
                                      const int64_t uuid,
                                      const PERSISTID& target)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
                             SKILL_AFTER_HIT, EPARAMTYPE_TARGET, CVarList());
}

// 技能命中并已伤害了对象
bool SkillEventManager::SkillHitDamage(IKernel* pKernel,
                                       const PERSISTID& self,
                                       const PERSISTID& skill,
                                       const int64_t uuid,
                                       const PERSISTID& target,
									   const int nDamageVal)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
                             SKILL_HIT_DAMAGE, EPARAMTYPE_TARGET_DAMAGE_INFO, CVarList() << nDamageVal);
}

// 技能使用结束[单]
bool SkillEventManager::SkillFinish(IKernel* pKernel, 
                                    const PERSISTID& self,
                                    const PERSISTID& skill,
                                    const int64_t uuid,
                                    const PERSISTID& target)
{
    return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
                             SKILL_FINISH, EPARAMTYPE_TARGET, CVarList());
}

// 技能被打断[单]
bool SkillEventManager::SkillBreak(IKernel* pKernel,
                                   const PERSISTID& self, 
                                   const PERSISTID& skill,
                                   const int64_t uuid)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    if (!pSelfObj->FindAttr(FIGHTER_CUR_SKILL_TARGET))
    {
        return false;
    }
    PERSISTID target = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);

	return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, target,
		SKILL_BREAK, EPARAMTYPE_TARGET, CVarList());
}

// 技能使用结束之后
bool SkillEventManager::SkillAfterFinish(IKernel* pKernel,
							 const PERSISTID& self,
							 const PERSISTID& skill,
							 const int64_t uuid)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	return m_pInstance->SkillEventProcess(pKernel, self, skill, uuid, PERSISTID(),
		SKILL_AFTER_FINISH, EPARAMTYPE_TARGET, CVarList());
}

// 事件注册
bool SkillEventManager::RegistSkillEvent(const EEventExecType func_id)
{
	if (func_id < EVENT_EXEC_INVALID || func_id >= MAX_EXECUTE)
	{
		return false;
	}

	SkillEventBase* pEventBase = SkillEventBase::CreateSkillEvent(func_id);
	if (NULL == pEventBase)
	{
		return false;
	}

	m_pEventContainer[func_id] = pEventBase;

	return true;
}

//技能事件处理
bool SkillEventManager::SkillEventProcess(IKernel* pKernel,
                                          const PERSISTID& self,
                                          const PERSISTID& skill, 
                                          const int64_t uuid,
                                          const PERSISTID& target, 
                                          const ESkillEventType eSkillEventType,
                                          const EParamType eParamType,
                                          const IVarList& exParams)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

    CVarList vEffectPacks;
    if (!GetEffectPackList(pKernel, self, skill, vEffectPacks))
    {
        return false;
    }

    CVarList params;// 处理函数需要的其他参数
    CVarList vEventParams;

    CVarList outMsgParams;
    CVarList totalMsgParams;
    int iMsgCount = 0;

    size_t iEffectListCount = vEffectPacks.GetCount();
	LoopBeginCheck(d)
    for (size_t i = 0; i < iEffectListCount; ++i)
    {
		LoopDoCheck(d)
		const char* szEffectID = vEffectPacks.StringVal(i);
        const SkillEffectData* pSkillEffect = SkillDataQueryModule::m_pInstance->GetSkillEffectProp(szEffectID);
        if (NULL == pSkillEffect)
        {
            continue;
        }

		if (!CanExecute(pKernel, self, target, pSkillEffect, eSkillEventType))
		{
            continue;
		}

		if (!GetParams(pKernel, self, skill, target, pSkillEffect, eParamType, exParams, params))
		{
            continue;
		}

        // 事件函数id
        EEventExecType eFuncID = (EEventExecType)pSkillEffect->GetFuncNo();

        // 执行事件
        outMsgParams.Clear();
		vEventParams.Clear();
        pSkillEffect->GetParams().ConvertToVarList(vEventParams);
		ExecuteOperate(pKernel, eFuncID, vEventParams, self, skill, uuid, eSkillEventType, params, outMsgParams);
        if (outMsgParams.GetCount() > 0)
        {
            iMsgCount++;
            totalMsgParams << outMsgParams;
        }
    }

    return true;
}

// 获取技能当前命中次序的高级效果包
bool SkillEventManager::GetEffectPackList(IKernel* pKernel,
										  const PERSISTID& self,
                                          const PERSISTID& skill,
                                          CVarList& vEffectPackList)
{
    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
    if (pSkillMain == NULL)
    {
        return false;
    }

    vEffectPackList.Clear();

	CVarList effectList;
    pSkillMain->GetSkill_Effect().ConvertToVarList(effectList);

	CVarList vPassiveEffect;
	// 没有附加效果的,添加配置的技能效果
	if(!SkillDataQueryModule::m_pInstance->QueryPassiveSkillEffect(pKernel, vPassiveEffect, self, skill, PS_ADD_SKILL_EFFECT, ""))
	{
		vEffectPackList << effectList;
	}
	else // 增加被动技能的附加效果,如果附加效果中,有与技能配置效果相同的,则替换技能配置效果
	{
		bool bAddEffect = true;
		LoopBeginCheck(a);
		for (int i = 0; i < (int)effectList.GetCount(); ++i)
		{
			LoopDoCheck(a);
			const char* strSkillEffect = effectList.StringVal(i);
			const SkillEffectData* pSkillEffect = SkillDataQueryModule::m_pInstance->GetSkillEffectProp(strSkillEffect);
			if (NULL == pSkillEffect)
			{
				continue;
			}
			// 不是技能的效果,不需要检查
			if (pSkillEffect->GetFuncNo() <= EVENT_EXEC_SKILL_START || pSkillEffect->GetFuncNo() >= EVENT_EXEC_SKILL_END)
			{
				vEffectPackList << strSkillEffect;
				continue;
			}

			bAddEffect = true;
			// 检查配置中的效果事件类型与附加的效果事件类型是否有相同的
			LoopBeginCheck(a);
			for (int j = 0; j < (int)vPassiveEffect.GetCount(); ++j)
			{
				LoopDoCheck(a);
				const char* strPassiveEffect = vPassiveEffect.StringVal(j);
				const SkillEffectData* pPassiveEffect = SkillDataQueryModule::m_pInstance->GetSkillEffectProp(strPassiveEffect);
				if(NULL == pPassiveEffect)
				{
					continue;
				}
				if (pPassiveEffect->GetFuncNo() == pSkillEffect->GetFuncNo())
				{
					bAddEffect = false;
					break;
				}
			}
			// 不相同的添加
			if (bAddEffect)
			{
				vEffectPackList << strSkillEffect;
			}
		}
		// 添加附加的效果
		vEffectPackList << vPassiveEffect;
	}
	return true;
}

// 判断指定高绩效包的执行条件
bool SkillEventManager::CanExecute(IKernel* pKernel,
                                   const PERSISTID& self,
                                   const PERSISTID& target,
                                   const SkillEffectData* pSkillEffect,
                                   const ESkillEventType eSkillEventType)
{
    if (NULL == pSkillEffect)
    {
        return false;
    }
    
    // 事件类型EEventType
    ESkillEventType eCurSkillEventType = (ESkillEventType)pSkillEffect->GetEventType();
    if (eCurSkillEventType != eSkillEventType)
    {
        return false;
    }

    return true;
}

bool SkillEventManager::GetParams(IKernel* pKernel,
                                  const PERSISTID& self,
                                  const PERSISTID& skill,
                                  const PERSISTID& target,
                                  const SkillEffectData* pSkillEffect,
                                  const EParamType eParamType,
                                  const IVarList& exParams,
                                  CVarList& params)
{
    if (NULL == pSkillEffect)
    {
        return false;
    }

    PERSISTID cur_target = target;
    ETargetType eTargetType = (ETargetType)pSkillEffect->GetTargetType();
    if (EPARAMTYPE_SKILL == eParamType)
    {
        if (TARGET_SEND != eTargetType) //efreshProperty target must be skill it self
        {
            extend_warning(LOG_INFO, "[SkillEventManager::RefreshProperty] target must be skill it self.");
            return false;
        }
    }

    if (EPARAMTYPE_SKILL != eParamType)
    {
        //目标筛选方式
        if (!FixSkillTarget(pKernel, self, cur_target, eTargetType))
        {
            return false;
        }
    }

    //获取参数
    params.Clear();
    switch (eParamType)
    {
    case EPARAMTYPE_SKILL:
        params << skill;
        break;
    case EPARAMTYPE_TARGET:
        {
            params << cur_target;
        }
        break;
    case EPARAMTYPE_TARGET_XYZ:
        params << cur_target;
        params << exParams.FloatVal(0) << exParams.FloatVal(1) << exParams.FloatVal(2);
        break;
	case EPARAMTYPE_TARGET_DAMAGE_INFO:
		params << cur_target;
		params << exParams.IntVal(0);
		break;
    default:
        break;
    }

    return true;
}

// 注册各个不同技能事件函数
bool SkillEventManager::InitEventExecutes(IKernel* pKernel)
{
	// 通用
	LoopBeginCheck(e)
	for (int i = EVENT_EXEC_ADDBUFFER;i < EVENT_EXEC_COMMON_END;++i)
	{
		LoopDoCheck(e)
		RegistSkillEvent((EEventExecType)i);
	}

	// 技能
	LoopBeginCheck(f)
		for (int i = EVENT_EXEC_FLICKER_MOVE; i < EVENT_EXEC_SKILL_END; ++i)
	{
		LoopDoCheck(f)
		RegistSkillEvent((EEventExecType)i);
	}

	// buff
	LoopBeginCheck(g)
	for (int i = EVENT_EXEC_BUFF_CHANGE_TARGET_HP;i < EVENT_EXEC_BUFF_END;++i)
	{
		LoopDoCheck(g)
		SkillEventManager::RegistSkillEvent((EEventExecType)i);
	}
    return true;
}
