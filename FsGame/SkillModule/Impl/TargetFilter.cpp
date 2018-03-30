
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/Interface/PropertyInterface.h"
//#include "FsGame/SkillModule/SceneAttackManage.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include "../../SystemFunctionModule/StaticDataQueryModule.h"
#include "../../Define/StaticDataDefine.h"
#include "../../Define/FightDefine.h"
//#include "../../Define/SceretSceneDefine.h"
#include "../../NpcBaseModule/AI/AIDefine.h"
#include "CommonModule/EnvirValueModule.h"
//#include "../PKModelModule.h"


//搜索影响的半径值
#define  SEARCHINFLUENCERADIUS  12

int TargetFilter::GetHitRangeTargets(IKernel* pKernel, const PERSISTID& self, const FmVec3& basePos,
                                     PERSISTID locktarget, const SkillHitRangeParams& hitRangeParams,
                                     const FilterConditionParamStruct& filterCondition,
                                     int iCount, IVarList& result)
{
    if (!pKernel->Exists(self))
    {
        return 0;
    }
#if _DEBUG
    extend_warning(LOG_TRACE, "[TargetFilter::GetHitRangeTargets] self：%d-%d  base pos(%f, %f)",
        self.nIdent, self.nSerial, basePos.x, basePos.z);
#endif
    //找要查找的内部对象类型
    int iInternelClassType = SkillUtilFuncs::ConvertTargetInnerClassType(filterCondition.iHitTargetType);

    int iGetCount = iCount;
    if (iCount > 99)
    {
        iGetCount = 99;
    }
    else if (iCount < 20) // 尽量多选些目标
    {
        iGetCount = iCount + 20;
    }
    else
    {
        iGetCount = iCount * 2;
    }

    CVarList vRes;
    // 获取对象
    GetTargetList(pKernel, self, basePos, locktarget, hitRangeParams, iInternelClassType, iGetCount, vRes);
    if (filterCondition.bSortByDist)
    {
        SortObjectByDist(pKernel, self, vRes);
    }
    else if (filterCondition.bSortByHP)
    {
        SortObjectByHP(pKernel, vRes);
    }

	LoopBeginCheck(a);
    int count = (int)vRes.GetCount();
    for (int i = 0; i < count; ++i)
    {
		LoopDoCheck(a);
        PERSISTID tmpobj = vRes.ObjectVal(i);
        float tx, ty, tz;

        IGameObj* pTempObj = pKernel->GetGameObj(tmpobj);
        if (NULL == pTempObj)
        {
            continue;
        }
        tx = pTempObj->GetPosiX();
        ty = pTempObj->GetPosiY();
        tz = pTempObj->GetPosiZ();
#if _DEBUG
        extend_warning(LOG_TRACE, "[TargetFilter::GetHitRangeTargets] foreach obj:%d-%d  obj pos(%f, %f)",
            tmpobj.nIdent, tmpobj.nSerial, tx, tz);
#endif
        // 不包括死亡对象
        if (!filterCondition.bIncludeDead && pTempObj->FindAttr("Dead") && pTempObj->QueryInt("Dead") > 0)
        {
#if _DEBUG
            extend_warning(LOG_TRACE, "[TargetFilter::GetHitRangeTargets] reject Dead target：%d-%d", tmpobj.nIdent, tmpobj.nSerial);
#endif
            continue;
        }

        if (TARGETFILTER_RESULT_SUCCEED != SatisfyRelation(pKernel, self, tmpobj, filterCondition.iRelationType, filterCondition.iHitTargetType))
        {
#if _DEBUG
            extend_warning(LOG_TRACE, "[TargetFilter::GetHitRangeTargets] no satisfy relation target：%d-%d", tmpobj.nIdent, tmpobj.nSerial);
#endif
            continue;
        }

        FilterParamStruct filterParams;
        // 1.目标半径
		filterParams.fTargetBodyRadius = pTempObj->QueryFloat("CollideRadius");

        // 2.角色方向
        filterParams.fShapeOrient = pKernel->GetOrient(self);

        // 范围过滤
        if (TARGETFILTER_RESULT_SUCCEED != IsInHitRange(pKernel, basePos, FmVec3(tx, ty, tz), hitRangeParams, filterParams))
        {
#if _DEBUG
            extend_warning(LOG_TRACE, "[%s] outer target：%d-%d  target pos:(%f, %f)",
                __FUNCTION__, tmpobj.nIdent, tmpobj.nSerial, tx, tz);
#endif
            continue;
        }

        iCount--;
        result << tmpobj;

        if (iCount <= 0)
        {
            break;
        }
    }

    return (int)result.GetCount();
}


ETargetFilterResult TargetFilter::IsInHitRange(IKernel* pKernel, const FmVec3& basePos, const FmVec3& targetPos,
                                               const SkillHitRangeParams& hitRangeParams, const FilterParamStruct& filterParams)
{
    ECollideResult ret = ECOLLIDE_RESULT_ERR_UNKNOWN;
    switch (hitRangeParams.eHitRangeType)
    {
    case SKILL_HIT_RANGE_TYPE_SELECT_OBJECT:     // 仅作用于锁定对象
        return TARGETFILTER_RESULT_SUCCEED;
        break;

    case SKILL_HIT_RANGE_TYPE_SPHERE:            // 作用于球形范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, r, R, angle;
            SkillUtilFuncs::GetSphereParams(hitRangeParams, basePos, filterParams.fShapeOrient, pos, fShapeOrient, r, R, angle);
            SectorRing2D rangeShape(pos, fShapeOrient, r, R, angle);

            Cricle2D targetShape(FmVec2(targetPos.x, targetPos.z), filterParams.fTargetBodyRadius);

            ret = ShapeCollision::Intersects(targetShape, rangeShape);
#if _DEBUG
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] sphere Collision info----");
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] pos:(%f,%f,%f)  fShapeOrient:%f r:%f  R:%f, angle:%f,  ret:%d",
                pos.x, basePos.y, pos.y, fShapeOrient, r, R, angle, ret);
#endif
        }
        break;
    case SKILL_HIT_RANGE_TYPE_CYLINDER:          // 作用于圆柱范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, r, R, angle;
            SkillUtilFuncs::GetCylinderParams(hitRangeParams, basePos, filterParams.fShapeOrient, pos, fShapeOrient, r, R, angle);

            SectorRing2D rangeShape(pos, fShapeOrient, r, R, angle);

            Cricle2D targetShape(FmVec2(targetPos.x, targetPos.z), filterParams.fTargetBodyRadius);

            ret = ShapeCollision::Intersects(targetShape, rangeShape);
#if _DEBUG
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] cylinder Collision info----");
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] pos:(%f,%f,%f)  fShapeOrient:%f r:%f  R:%f, angle:%f,  ret:%d",
                pos.x, basePos.y, pos.y, fShapeOrient, r, R, angle, ret);
#endif
        }
        break;
    case SKILL_HIT_RANGE_TYPE_RECTANGULAR:       // 作用于矩形范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, width, length;
            SkillUtilFuncs::GetRectangularParams(hitRangeParams, basePos, filterParams.fShapeOrient, pos, fShapeOrient, width, length);

			float fLength = length;
			float fWidth = width;

            Rectangle2D rangeShape(pos, fShapeOrient, fWidth, fLength);

            Cricle2D targetShape(FmVec2(targetPos.x, targetPos.z), filterParams.fTargetBodyRadius);

            ret = ShapeCollision::Intersects(targetShape, rangeShape);
#if _DEBUG
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] rectangle Collision info----");
            extend_warning(LOG_TRACE, 
                "[TargetFilter::IsInHitRange] pos:(%f,%f,%f)  fShapeOrient:%f width:%f  lenght:%f, ret:%d",
                pos.x, basePos.y, pos.y, fShapeOrient, width, length, ret);
#endif
        }
        break;
    case SKILL_HIT_RANGE_TYPE_POS_CYLINDER:         // 作用某个位置的圆柱范围
		return TARGETFILTER_RESULT_SUCCEED;
        break;

    default:
        break;
    }

    return ECOLLIDE_RESULT_INTERSECT == ret ? TARGETFILTER_RESULT_SUCCEED : TARGETFILTER_RESULT_ERR_OTHER;
}

int TargetFilter::GetTargetList(IKernel* pKernel, const PERSISTID& self,  const FmVec3& basePos,
                                PERSISTID locktarget, const SkillHitRangeParams& hitRangeParams,
                                int iInternelClassType, int iCount, IVarList& result)
{
    CVarList totalResult; // 总在对象集合

   // FmVec3 pos = basePos;
    float orient = pKernel->GetOrient(self);
    switch (hitRangeParams.eHitRangeType)
    {
    case SKILL_HIT_RANGE_TYPE_SELECT_OBJECT:     // 仅作用于锁定对象
        {
            if (!pKernel->Exists(locktarget))
            {
                break;
            }
            totalResult << locktarget;
        }
        break;

    case SKILL_HIT_RANGE_TYPE_SPHERE:            // 作用于球形范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, r, R, angle;
            SkillUtilFuncs::GetSphereParams(hitRangeParams, basePos, orient, pos, fShapeOrient, r, R, angle);

            float fOutRadius = R + SEARCHINFLUENCERADIUS;  // 添加影响的半径值
            
#if _DEBUG
			float fInnerRadius = (r >= SEARCHINFLUENCERADIUS) ? (r - SEARCHINFLUENCERADIUS) : 0.0f;

            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] sphere config info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] centre_dist:%f max_dist:%f, min_dist:%f, trun_angle:%f, up_angle:%f, offset_orint:%f, basepos_offsetorint:%f",
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistCentre, 
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMax,
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMin,
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereTurnAngle,
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereUpAngle,
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereOffsetOrient,
                hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereBasepointOffsetOrient
                );

            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] sphere trace info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] begin pos:(%f,%f,%f)  inner_radius:%f  outer_radius:%f, classtype:%d  count:%d",
                pos.x, basePos.y, pos.y, fInnerRadius, fOutRadius, iInternelClassType, iCount);
#endif
            pKernel->GetPointAroundList(self, pos.x, pos.y, fOutRadius, iInternelClassType, iCount, totalResult, false);
        }
        break;

    case SKILL_HIT_RANGE_TYPE_CYLINDER:          // 作用于圆柱范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, r, R, angle;
            SkillUtilFuncs::GetCylinderParams(hitRangeParams, basePos, orient, pos, fShapeOrient, r, R, angle);
            float fOutRadius = R + SEARCHINFLUENCERADIUS;	//添加影响半径值
#if _DEBUG
			float fInnerRadius = (r >= SEARCHINFLUENCERADIUS) ? (r - SEARCHINFLUENCERADIUS) : 0.0f;

            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] cylinder config info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] centre_dist:%f max_dist:%f, min_dist:%f, trun_angle:%f, height:%f, offset_orint:%f, basepos_offsetorint:%f",
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistCentre, 
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMax,
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMin,
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle,
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderHeight,
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient,
                hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient
                );

            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] cylinder trace info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] begin pos:(%f,%f,%f)  inner_radius:%f  outer_radius:%f  sector_orient:%f, classtype:%d  count:%d",
                pos.x, basePos.y, pos.y, fInnerRadius, fOutRadius, fShapeOrient, iInternelClassType, iCount);
#endif
            pKernel->GetPointAroundList(self, pos.x, pos.y, fOutRadius, iInternelClassType, iCount, totalResult, false);
        }
        break;

    case SKILL_HIT_RANGE_TYPE_RECTANGULAR:       // 作用于矩形范围
        {
            FmVec2 pos(basePos.x, basePos.z);
            float fShapeOrient, width, length;
            SkillUtilFuncs::GetRectangularParams(hitRangeParams, basePos, orient, pos, fShapeOrient, width, length);
            float fLength = length + SEARCHINFLUENCERADIUS;
            float fWidth = width + SEARCHINFLUENCERADIUS;
            float fHalfLength = fLength * 0.5f;
            // 方向位移向量
            FmVec2 offset = FmVec2(sin(fShapeOrient), cos(fShapeOrient)) * fHalfLength;
            // 将矩形中心点位移到边上
            FmVec2 startPos = pos - offset;
            FmVec2 endPos = pos + offset;
#if _DEBUG
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] rectangular config info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] centre_dist:%f length:%f, width:%f, height:%f offset_orint:%f, basepos_offsetorint:%f",
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularDistCentre, 
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularLength,
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularWidth,
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularHeight,
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient,
                hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient
                );

            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] rectangular trace info--");
            extend_warning(LOG_TRACE, "[TargetFilter::GetTargetList] start pos:(%f,%f,%f)  end pos:(%f,%f,%f), line_radius:%f, classtype:%d  count:%d",
                startPos.x, basePos.y, startPos.y, endPos.x, basePos.y, endPos.y,
                fWidth, iInternelClassType, iCount);
#endif
            pKernel->TraceObjectList(self, startPos.x, basePos.y, startPos.y, 
                                     endPos.x, basePos.y, endPos.y, 
                                     fWidth, iInternelClassType, iCount, result, false);
        }
        break;

    case SKILL_HIT_RANGE_TYPE_POS_CYLINDER:         // 作用某个位置的圆柱范围
        {
		   float fRadius = 0.0f;
		   SkillUtilFuncs::GetPosCylinderParams(hitRangeParams, fRadius);
           pKernel->GetPointAroundList(self, basePos.x, basePos.z, fRadius, iInternelClassType, iCount, totalResult, false);
        }
        break;

    default:
        break;
    }

    result << totalResult;

    return (int)result.GetCount();
}


//判断目标对象是否满足各种关系类型
ETargetFilterResult TargetFilter::SatisfyRelation(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                                                  int iRelationType, int iHitTargetType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return TARGETFILTER_RESULT_ERR_OTHER;
	}
	// 功能类NPC不可以战斗
	if(strcmp(pTargetObj->GetScript(), "FunctionNpc") == 0 )
	{
		return TARGETFILTER_RESULT_ERR_OTHER;
	}

	bool bSameGroup = CheckGroupID(pKernel, pSelfObj, pTargetObj);

	// 去掉新手保护检查
	bool bLevelLim = CheckLevelLimit(pKernel, pSelfObj, pTargetObj);

    // 检查技能作用对象关系
    int result1 = CheckHitTargetRelationType(pKernel, pSelfObj, pTargetObj, iRelationType);
    // 检查技能作用对象类型
    int result2 = CheckHitTargetType(pKernel, pSelfObj, pTargetObj, iHitTargetType);

    if (TARGETFILTER_RESULT_SUCCEED == result1 && TARGETFILTER_RESULT_SUCCEED == result2 && bSameGroup && bLevelLim)
    {
        return TARGETFILTER_RESULT_SUCCEED;
    }

    return TARGETFILTER_RESULT_ERR_OTHER;
}

// 检查技能作用对象关系
ETargetFilterResult TargetFilter::CheckHitTargetRelationType(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj, int iRelationType)
{
    // 是否能作用自己
    if (pSelfObj == pTargetObj)
    {
        if ((iRelationType & HIT_TARGET_RELATION_TYPE_SELF) != 0)
        {
            return TARGETFILTER_RESULT_SUCCEED;
        }
        return TARGETFILTER_RESULT_ERR_OTHER;
    }

    // 友好方
    if ((iRelationType & HIT_TARGET_RELATION_TYPE_FRIEND) != 0)
    {
// 		if (PKModelModule::IsFriendly(pKernel, pSelfObj->GetObjectId(), pTargetObj->GetObjectId()))
//         {
//             return TARGETFILTER_RESULT_SUCCEED;
//         }
    }

    // 敌对方
    if ((iRelationType & HIT_TARGET_RELATION_TYPE_ENIMY) != 0)
    {
// 		if (PKModelModule::IsBeAttacked(pKernel, pSelfObj->GetObjectId(), pTargetObj->GetObjectId()))
//         {
//             return TARGETFILTER_RESULT_SUCCEED;
//         }
    }


    return TARGETFILTER_RESULT_ERR_OTHER;
}

ETargetFilterResult TargetFilter::CheckHitTargetType(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj, int iHitTargetType)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return TARGETFILTER_RESULT_ERR_OTHER;
	}
    int type = pTargetObj->GetClassType();
    if (type == TYPE_PLAYER) // 目标对象是玩家
    {
        if ((iHitTargetType & HIT_TARGET_TYPE_PLAYER) != 0) // 判断是否可以对玩家类型的目标起作用
        {
            return TARGETFILTER_RESULT_SUCCEED;
        }
    }
    else if (type == TYPE_NPC) // 目标对象是npc
    {
		if ((iHitTargetType & HIT_TARGET_TYPE_MONSTER) != 0) // 判断是否对NPC启作用
		{
			//静态NPC不能被攻击
			// 不在出生状态下 才能被攻击
			if ( pTargetObj->QueryInt("NpcType") < NPC_TYPE_FUNCTION && pTargetObj->QueryInt("BornState") == 0)
			{
				return TARGETFILTER_RESULT_SUCCEED;
			}
		}
    }

    return TARGETFILTER_RESULT_ERR_OTHER;
}

// 检查分组号
bool TargetFilter::CheckGroupID(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}
	bool bCheck = true;
	int nSelfGroupId = pSelfObj->QueryInt("GroupID");
	int nTargetGroupId = pTargetObj->QueryInt("GroupID");

	// GroupID等于0则所有分组都可见
	if (nSelfGroupId != 0 && nTargetGroupId != 0)
	{
		if (nSelfGroupId != nTargetGroupId)
		{
			// 不在一个分组里可以删除战斗关系
			bCheck = false;
		}
	}

	return bCheck;
}

// 检查施法者与目标等级 针对玩家之间的pk限制
bool TargetFilter::CheckLevelLimit(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}
// 	if (PKModelModule::IsFriendly(pKernel, pSelfObj->GetObjectId(), pTargetObj->GetObjectId()))
// 	{
// 		return true;
// 	}
	bool bLimit = false;
	int nSelfType = pSelfObj->GetClassType();
	int nTargetType = pTargetObj->GetClassType();
	// 玩家之间PK有等级保护
// 	if (nSelfType == TYPE_PLAYER && nTargetType == TYPE_PLAYER)
// 	{
// 		int nPkLimitLv = EnvirValueModule::EnvirQueryInt(ENV_VALUE_PK_LIMIT_LEVEL);
// 		int nSelfLv = pSelfObj->QueryInt("Level");
// 		int nTargetLv = pTargetObj->QueryInt("Level");
// 		// 施法者与目标的等级都要大于nPkLimitLv,才能PK
// 		if( nSelfLv > nPkLimitLv && nTargetLv > nPkLimitLv)
// 		{
// 			bLimit = true;
// 		}
// 	}
// 	else
// 	{
// 		bLimit = true;
// 	}
	
	return bLimit;
}

namespace
{
    struct DistanceComparer
    {
        DistanceComparer(IKernel* pKernerl, const PERSISTID& tar) : m_pKernel(pKernerl), m_target(tar)
        {
        }

        bool operator()(const PERSISTID& obj1, const PERSISTID& obj2)
        {
            return m_pKernel->Distance2D(obj1, m_target) < m_pKernel->Distance2D(obj2, m_target);
        }
        IKernel* m_pKernel;
        PERSISTID m_target;
    };

    struct ObjectHPComparer
    {
        ObjectHPComparer(IKernel* pKernerl) : m_pKernel(pKernerl)
        {
        }

        bool operator()(const PERSISTID& objLeft, const PERSISTID& objRight)
        {
            IGameObj* pGameObjLeft = m_pKernel->GetGameObj(objLeft);
            IGameObj* pGameObjRight = m_pKernel->GetGameObj(objRight);
            if (NULL == pGameObjLeft || NULL == pGameObjRight)
            {
                return false;
            }
            int64_t hpLeft = pGameObjLeft->QueryInt64("HP");
            int64_t hpRight = pGameObjRight->QueryInt64("HP");
            int64_t maxhpLeft = pGameObjLeft->QueryInt64("MaxHP");
            int64_t maxhpRight = pGameObjRight->QueryInt64("MaxHP");
			if (maxhpLeft <= 0 || maxhpRight <=0)
			{
				return false;
			}
            float fRatioLeft = hpLeft * 1.0f / maxhpLeft;
            float fRatioRight = hpRight * 1.0f / maxhpRight;
            return fRatioLeft < fRatioRight;
        }
        IKernel* m_pKernel;
    };
}

// 对列表中的对象进行排序
bool TargetFilter::SortObjectByDist(IKernel* pKernel, const PERSISTID& self, IVarList& result)
{
    std::vector<PERSISTID> vctTargets;
    size_t num = result.GetCount();
	LoopBeginCheck(b)
    for (size_t t = 0; t < num; ++t)
    {
		LoopDoCheck(b)
        PERSISTID target = result.ObjectVal(t);
        if (pKernel->Exists(target))
        {
            vctTargets.push_back(target);
        }
    }

    std::sort(vctTargets.begin(), vctTargets.end(), DistanceComparer(pKernel, self));
    result.Clear();

    num = vctTargets.size();
	LoopBeginCheck(c)
    for (size_t t = 0; t < num; ++t)
    {
		LoopDoCheck(c)
        PERSISTID target = vctTargets[t];
        if (pKernel->Exists(target))
        {
            result << target;
        }
    }

    return true;
}

bool TargetFilter::SortObjectByHP(IKernel* pKernel, IVarList& result)
{
    std::vector<PERSISTID> vctTargets;
    size_t num = result.GetCount();
	LoopBeginCheck(d)
    for (size_t t = 0; t < num; ++t)
    {
		LoopDoCheck(d)
        PERSISTID target = result.ObjectVal(t);
        if (pKernel->Exists(target))
        {
            vctTargets.push_back(target);
        }
    }

    std::sort(vctTargets.begin(), vctTargets.end(), ObjectHPComparer(pKernel));
    result.Clear();

    num = vctTargets.size();
	LoopBeginCheck(e)
    for (size_t t = 0; t < num; ++t)
    {
		LoopDoCheck(e)
        PERSISTID target = vctTargets[t];
        if (pKernel->Exists(target))
        {
            result << target;
        }
    }
    return true;
}