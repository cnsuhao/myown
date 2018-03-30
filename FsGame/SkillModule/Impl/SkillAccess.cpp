//------------------------------------------------------------------------------
// 文件名:      SkillAccess.cpp
// 内  容:      技能对象属性访问、技能状态相关查询
// 说  明:      该类只在技能系统内部使用，且不要直接定义SkillAccess的实例，而使用
//              其单例模式接口SkillAccessSingleton::Instance()
// 创建日期:    2014年10月27日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/skillmodule/impl/skillaccess.h"

#include "FsGame/define/CommandDefine.h"
#include "FsGame/define/CoolDownDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/Define/SkillEventFuncIdDefine.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/exptree.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/skillmodule/impl/SkillFlow.h"
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/SkillModule/SkillToSprite.h"
#include <cmath>
#include "FsGame/Define/staticdatadefine.h"
#include "../../Define/SkillDefine.h" 
#include "utils/string_util.h"
#include "../FightActionControl.h"
#include "../../NpcBaseModule/AI/AIDefine.h"

bool SkillAccess::Init(IKernel* pKernel)
{
    return true;
}

// 指定技能是否正在冷却
bool SkillAccess::IsCoolDown(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

 	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (pSkillMain == NULL)
	{
		return false;
	}

	// 先验证知否取消的CD限制
	int nCancelCD = 0;
	if(pSkillObj->FindData("TempCancelCD"))
	{
		nCancelCD = pSkillObj->QueryDataInt("TempCancelCD");
	}
	if(nCancelCD > 0)
	{
		return false;
	}

    int iCategory = pSkillMain->GetCoolDownCategory();
    return CoolDownModule::m_pInstance->IsCoolDown(pKernel, self, iCategory);
}

// 获取技能作用目标,位置,方向
bool SkillAccess::GetSkillTargetAndPos(IKernel* pKernel,
                                       const PERSISTID& self,
                                       const PERSISTID& skill,
                                       PERSISTID& useskilltarget,
                                       float& dx,
                                       float& dy,
                                       float& dz,
                                       float& doirent)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL == pSkillMain)
	{
		return false;
	}

    bool  ret = true;

    float fSelfX, fSelfY, fSelfZ, fSelfOrient;
    pKernel->GetLocation(self, fSelfX, fSelfY, fSelfZ, fSelfOrient);

    // 先设置默认朝向
    doirent = ::util_dot_angle(fSelfX, fSelfZ, dx, dz);

    // 技能释放选择类型
    ESkillSelectType eSelectType = (ESkillSelectType)pSkillMain->GetSelectType();
    switch (eSelectType)
    {
        case SKILL_SELECT_TYPE_DEFAULT:          // 瞬发（无选择）
        {
			// 没有位移的用自己的位置
			if (DISPLACE_NONE_TYPE == pSkillMain->GetDisplaceType())
			{
				if (pSelfObj->GetClassType() == TYPE_PLAYER)
				{
					doirent = fSelfOrient;
				}

				dx = fSelfX;
				dy = fSelfY;
				dz = fSelfZ;
			}
			else
			{
				// 验证位置的有效性
				if (!SkillAccessSingleton::Instance()->IsValidDisplace(pKernel, self, pSkillMain->GetID(), DISPLACE_SELF_TYPE, dx, dz))
				{
					return false;
				}
			}
        }
        break;

        case SKILL_SELECT_TYPE_SELECT_OBJECT:    // 对象选取辅助（目标）
        {
			// 在线玩家会找 FIGHTER_CUR_SKILL_TARGET
			if (pKernel->Type(self) == TYPE_PLAYER && 1 == pSelfObj->QueryInt("Online")) 
			{
				//当鼠标离开目标会触发目标清空，所以不用lastobject
				useskilltarget = pSelfObj->QueryObject(FIGHTER_CUR_SKILL_TARGET);
			}
			// 没有目标,把客户端选择的目标作为攻击目标
            if (!pKernel->Exists(useskilltarget))
            {
                ret = false;
                break;
            }
            // 判断是都锁定对象自身
            if (pSkillMain->GetIsHitSelf())
            {
                useskilltarget = self;
                doirent = fSelfOrient;
            }
        }
        break;
		case SKILL_SELECT_TYPE_POS:          // 选取某个位置
			{
				if (pSkillMain->GetIsHitSelf())
				{
					useskilltarget = self;
				}
			}
			break;
        default:
			{
				const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
				ret = false;
				::extend_warning(LOG_WARNING,
								 "[SkillAccess::GetSkillTargetAndPos] SKILL %s unknown !", szSkillConfigID);
			}
            break;
    }

    return ret;
}


//获得技能的公共冷却时间
bool SkillAccess::GetCoolDownTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
                                  int& iPubCooldowntime, int& iCateCooldowntime, int& iCoolCategory)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
    if (NULL == pSkillMain)
    {
        return false;
    }
	// CD为成长属性
	int nCoolDownTime = SkillDataQueryModule::m_pInstance->QuerySkillGrowProperty(pKernel, skill, SKILL_GP_COOL_DOWN_TIME);

	// 暂时去掉偏差处理,有需要加回来
// 	int nCDTimeDiff = SkillDataQueryModule::m_pInstance->GetSkillTimeDiff(pKernel, self, COOL_DOWN_TIME_DIFF);
// 	int nPublicCDTimeDiff = SkillDataQueryModule::m_pInstance->GetSkillTimeDiff(pKernel, self, PUBLIC_CD_TIME_DIFF);
//     //计算时间
//     iPubCooldowntime = __max(pSkillMain->GetPublicCDTime() - nPublicCDTimeDiff, 0);
// 
// 	// 被动技能影响的CD
// 	int nPSCooldowntime = 0;
// 	CVarList args;
// 	if (SkillDataQueryModule::m_pInstance->QueryPassiveSkillEffect(pKernel, args, self, skill, PS_CHANGE_SKILL_CONFIG, SKILL_CD_TIME_NAME))
// 	{
// 		nPSCooldowntime = args.IntVal(0);
// 	}
	
	//iCateCooldowntime = __max(nCoolDownTime - nCDTimeDiff + nPSCooldowntime, 0);
	
	iCateCooldowntime = nCoolDownTime;
	iPubCooldowntime = pSkillMain->GetPublicCDTime();
    iCoolCategory = pSkillMain->GetCoolDownCategory();

    return true;
}

// 获取额外伤害命中系数
void SkillAccess::GetExtraHitDamageRatio(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float& fExtraHitDamageRatio)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return ;
    }

	fExtraHitDamageRatio = 1.0f;
	const SkillMainData* pSkillMainData = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL != pSkillMainData)
	{
        const StringArray& vExtraHitDamageRatioList = pSkillMainData->GetExtraHitDamage(); 
        const int count = (int)vExtraHitDamageRatioList.GetCount();
        if (count == 0)
        {
            return;
        }
        int nHitIndex = GetHitIndex(pKernel, pSelfObj, pSkillObj);

		// 额外命中第一次命中
		if (nHitIndex < 0)
		{
			return;
		}
		
		// 名次次数大于配置个数,取最后一个配置
		nHitIndex = __min(nHitIndex, count - 1);
		fExtraHitDamageRatio = StringUtil::StringAsFloat(vExtraHitDamageRatioList.StringVal(nHitIndex));
	}
}

//获取技能是施放目标类型（有目标和无目标两种：返回USESKILL_TYPE_TARGET或者USESKILL_TYPE_POSITION）
int SkillAccess::GetUseSkillType(IKernel* pKernel, const PERSISTID& skill)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return -1;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (pSkillMain == NULL)
	{
		return -1;
	}
    ESkillSelectType eSelectType = (ESkillSelectType)pSkillMain->GetSelectType();
	int nSkillTargetType = 0;
    switch (eSelectType)
    {
        case SKILL_SELECT_TYPE_SELECT_OBJECT:   // 对象选取辅助（目标）
            nSkillTargetType = USESKILL_TYPE_TARGET;
			break;

        case SKILL_SELECT_TYPE_DEFAULT:         // 瞬发（无选择）
            nSkillTargetType = pSkillMain->GetIsHitSelf() ? USESKILL_TYPE_TARGET : USESKILL_TYPE_POSITION;
			break;
		case SKILL_SELECT_TYPE_POS:				// 选取某个位置
			nSkillTargetType = pSkillMain->GetIsHitSelf() ? USESKILL_TYPE_TARGET : USESKILL_TYPE_POSITION;
			break;
        default:
			{
				const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
				::extend_warning(LOG_WARNING,
					 "[SkillAccess::GetUseSkillType] SKILL %s unknown SelectType: %d !", szSkillConfigID, eSelectType);
			}               
            break;
    }

    return nSkillTargetType;
}

// 验证位移的有效性
bool SkillAccess::IsValidDisplace(IKernel* pKernel, const PERSISTID& self, const char* strSkillId, int nObjType, float dx, float dz)
{
	bool bValid = false;
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	// npc不需要验证
	if (pSelfObj->GetClassType() != TYPE_PLAYER)
	{
		return true;
	}

	// 找到对应的技能配置
	const SkillMainData* pSkillData = SkillDataQueryModule::m_pInstance->GetSkillMainConfig(strSkillId);
	if (NULL == pSkillData)
	{
		return false;
	}
	// 与释放者的最大距离
	int nMaxDis = DISPLACE_SELF_TYPE == nObjType ? pSkillData->GetDisplaceRange() : pSkillData->GetTargetDisplaceRange();

	float fMaxDis = (float)(nMaxDis * nMaxDis);
	// 计算施法者与目标的距离
	float fRealDis2 = util_dot_distance2(dx, dz, pSelfObj->GetPosiX(), pSelfObj->GetPosiZ());
	// 对客户端位移数据的验证
	if (fRealDis2 <= fMaxDis)
	{
		bValid = true;
	}

	return bValid;
}

// 查询技能的位移类型和速度 nObjType对应SkillDisplaceObject
bool SkillAccess::QuerySkillDisplaceTypeAndSpeed(IKernel* pKernel, const char* strSkillId, const int nObjType, int& nOutDisplaceType, float& fDisplaceSpeed)
{
	nOutDisplaceType = DISPLACE_NONE_TYPE;
	fDisplaceSpeed = 0.0f;
	// 找到对应的技能配置
	const SkillMainData* pSkillData = SkillDataQueryModule::m_pInstance->GetSkillMainConfig(strSkillId);
	if (NULL == pSkillData)
	{
		return false;
	}

	if (DISPLACE_SELF_TYPE == nObjType)
	{ 
		nOutDisplaceType = static_cast<SkillDisplaceType>(pSkillData->GetDisplaceType());
		fDisplaceSpeed = pSkillData->GetDisplaceSpeed();
	}
	else
	{
		nOutDisplaceType = static_cast<SkillDisplaceType>(pSkillData->GetTargetDisplaceType());
		fDisplaceSpeed = SKILL_HITBACK_MOTION_SPEED;
	}
	return true;
}

// 获取技能释放选择类型
ESkillSelectType SkillAccess::GetSkillSelectType(IKernel* pKernel, const PERSISTID& skill)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return SKILL_SELECT_TYPE_DEFAULT;
    }

	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (pSkillMain == NULL)
	{
		return SKILL_SELECT_TYPE_DEFAULT;
	}
    return (ESkillSelectType)pSkillMain->GetSelectType();
}

// 获取技能射程参数
bool SkillAccess::GetSkillCastRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, SkillCastRangeParams& cast_range_params,const SkillMainData* pSkillMain)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

	if (NULL == pSkillMain)
	{
		pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
		if (NULL == pSkillMain)
		{
			return false;
		}
	}
	
    cast_range_params.fRangeDistMax = (float)pSkillMain->GetRangeDistMax();        // 最大有效距离1
    cast_range_params.fRangeDistMin = (float)pSkillMain->GetRangeDistMin();        // 最小有效距离0

    if (!pSelfObj->FindData("RangeDistMax"))
    {
        pSelfObj->AddDataFloat("RangeDistMax", 0.0f);
    }
    pSelfObj->SetDataFloat("RangeDistMax", cast_range_params.fRangeDistMax);
    if (!pSelfObj->FindData("RangeDistMin"))
    {
        pSelfObj->AddDataFloat("RangeDistMin", 0.0f);
    }
    pSelfObj->SetDataFloat("RangeDistMin", cast_range_params.fRangeDistMin);
    return true;
}

// 获取技能作用范围参数
bool SkillAccess::GetSkillHitRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, int nHitIndex, SkillHitRangeParams& hit_range_params)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

	const SkillMainData *pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (pSkillMain == NULL)
	{
		return false;
	}

	CVarList vHitRangeIDs;
    pSkillMain->GetHitRangeID().ConvertToVarList(vHitRangeIDs);

	// 得到技能的命中次数
	int nSkillHitCount = pSkillObj->QueryInt(SKILL_HURT_COUNT);

	int nIndex = 0;
	// 多次命中技能,每次命中范围不同
	if(nSkillHitCount > 1)
	{
		nIndex = nHitIndex - 1;// 命中次数从1计数
	}

    if (!GetSkillHitRangeByHitRangeID(pKernel, vHitRangeIDs.StringVal(nIndex), hit_range_params))
    {
        return false;
    }

    return true;
}

// 通过技能id找到攻击距离
const float SkillAccess::GetSkillHitDistance(IKernel* pKernel, const char* szSkillId)
{
	float fDis = 0.0f;
	do 
	{
		if (NULL == szSkillId || StringUtil::CharIsNull(szSkillId))
		{
			break;
		}
		const SkillMainData* pMainData = SkillDataQueryModule::m_pInstance->GetSkillMainConfig(szSkillId);
		if (NULL == pMainData)
		{
			break;
		}

		fDis = pMainData->GetRangeDistMax();
	} while (false);
	
	return fDis;
}

// 获取技能可命中对象关系
bool SkillAccess::GetSkillHitTargetRelation(IKernel* pKernel, const PERSISTID& skill, int& iTargetRelationType, int& iTargetClassType)
{
    iTargetRelationType = HIT_TARGET_RELATION_TYPE_NONE;
    iTargetClassType = HIT_TARGET_TYPE_NONE;

    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false;
    }

    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
    if (NULL == pSkillMain)
    {
        return false;
    }

    CVarList vTargetRelations, vTargetClasss;
    pSkillMain->GetHitTargetRelationType().ConvertToVarList(vTargetRelations);
    pSkillMain->GetHitTargetType().ConvertToVarList(vTargetClasss);
    iTargetRelationType = SkillUtilFuncs::GetTargetRelationType(vTargetRelations);
    iTargetClassType = SkillUtilFuncs::GetTargetClassType(vTargetClasss);
    return true;
}

// 是否客户端选中目标命中
bool SkillAccess::IsClientHitTarget(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj)
{
	if (NULL == pSelfObj || NULL == pSkillObj)
	{
		return false;
	}
	const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(pSkillObj);
	if (NULL == pSkillMain)
	{
		return false;
	}

	// 由客户端命中的技能,在线玩家不做命中处理,命中由客户端通知服务器结算
	if (pSkillMain->GetIsClientHit() && pSelfObj->GetClassType() == TYPE_PLAYER && pSelfObj->QueryInt("Online") == 1 && !pSelfObj->FindData("RobotPlayer"))
	{
		return true;
	}

	return false;
}

// 获取当前命中次数
int SkillAccess::GetHitIndex(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj)
{
	if(NULL == pSelfObj || NULL == pSkillObj)
	{
		return 0;
	}
	// 将命中次数改为从0计数
	int nHitIndex = 0;
	if (IsClientHitTarget(pKernel, pSelfObj, pSkillObj))
	{
		// 客户端命中从1计数
		if (pSkillObj->FindData("ClientHitsIndex"))
		{
			nHitIndex = pSkillObj->QueryDataInt("ClientHitsIndex") - 1;
		}
	}
	else
	{
		nHitIndex = pSkillObj->QueryInt(SKILL_CUR_HITS) - 1;
	}

	return nHitIndex;
}

// 计算仇恨值
int SkillAccess::ComputeHateValue(IKernel* pKernel, const PERSISTID& skill, int nDamValue)
{
	const SkillMainData* pSkillData = SkillDataQueryModule::m_pInstance->GetSkillMain(pKernel, skill);
	if (NULL == pSkillData)
	{
		return 0;
	}

	int		nHateValue = pSkillData->GetHateValue();
	float	fHateValue = pSkillData->GetHateRate();

	int nAddHateValue = nHateValue + (int)(fHateValue * nDamValue);
	return nAddHateValue;
}

// 通过范围id找到对应的参数
bool SkillAccess::GetSkillHitRangeByHitRangeID(IKernel* pKernel, const char* szHitRangeID, SkillHitRangeParams& hitRangeParams)
{
	const SkillHitRangeData* pSkillHitRange = SkillDataQueryModule::m_pInstance->GetHitRangePropByID(szHitRangeID);

	if (NULL == pSkillHitRange)
	{
		return false;
	}

	hitRangeParams = pSkillHitRange->GetHitRangeParams();
	return true;
}