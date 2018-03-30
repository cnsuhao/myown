
//--------------------------------------------------------------------
// 文件名:      skill_module\impl\skill_configdata.cpp
// 内  容:      
// 说  明:      
// 创建日期:    2014年4月4日
// 创建人:       
//    :       
//--------------------------------------------------------------------

#include "FsGame/SkillModule/impl/SkillConfigData.h"

#include "utils/util_func.h"
#include "utils/extend_func.h"


namespace SkillUtilFuncs
{
    // 跟进配置参数处理偏移，转换成对象图形参数
    bool GetSphereParams(const SkillHitRangeParams& hitRangeParams, 
        const FmVec3& basePos, const float orient,
        FmVec2& pos, float& fShapeOrient,
        float& r, float& R, float& angle)
    {
        // 先基于原点旋转，顺时针旋转
        float fOrient = orient - hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereBasepointOffsetOrient;
        // 攻击者到图形中心连线的方向
        FmVec2 direct(sin(fOrient), cos(fOrient));// 注意：这里是(x,z）朝向是z+的夹角
        // 图形中心位置
        pos = FmVec2(basePos.x, basePos.z) + direct * hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistCentre;
        // 再基于图形中心点旋转图形朝向，顺时针旋转
        fShapeOrient = fOrient - hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereOffsetOrient;
        r = hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMin;
        R = hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMax;
        angle = hitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereTurnAngle;

        return true;
    }

    bool GetCylinderParams(const SkillHitRangeParams& hitRangeParams,
        const FmVec3& basePos, const float orient, 
        FmVec2& pos, float& fShapeOrient,
        float& r, float& R, float& angle)
    {
        // 先基于原点旋转，顺时针旋转
        float fOrient = orient - hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient;
        // 攻击者到图形中心连线的方向
        FmVec2 direct(sin(fOrient), cos(fOrient)); // 注意：这里是(x,z）朝向是z+的夹角
        // 图形中心位置
        pos = FmVec2(basePos.x, basePos.z) + direct * hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistCentre;
        // 再基于图形中心点旋转图形朝向，顺时针旋转
        fShapeOrient = fOrient - hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient;
        r = hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMin;
        R = hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMax;
        angle = hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle;
        return true;
    }

    bool GetRectangularParams(const SkillHitRangeParams& hitRangeParams,
        const FmVec3& basePos, const float orient, 
        FmVec2& pos, float& fShapeOrient,
        float& width, float& length)
    {
        // 先基于原点旋转，顺时针旋转
        float fOrient = orient - hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient;
        // 攻击者到图形中心连线的方向
        FmVec2 direct(sin(fOrient), cos(fOrient));// 注意：这里是(x,z）朝向是z+的夹角
        // 图形中心位置
        pos = FmVec2(basePos.x, basePos.z) + direct * hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularDistCentre;
        // 再基于图形中心点旋转图形朝向，顺时针旋转
        fShapeOrient = orient - hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient;
        length = hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularLength;
        width = hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularWidth;
        return true;
    }

	bool GetPosCylinderParams(const SkillHitRangeParams& hitRangeParams, float& fRadius)
	{
		fRadius = hitRangeParams.skillhitrangeunion.poscylinder.fHitRangeRadius;
		return true;
	}

    //////////////////////////////////////////////////////////////////////////
    int GetTargetRelationType(const IVarList& params)
    {
        int iRelationType = HIT_TARGET_RELATION_TYPE_NONE;
        const int g_HitTargetRelationTypeArray[] =
        {
            HIT_TARGET_RELATION_TYPE_SELF,       // 对应配置中的序号，0
            HIT_TARGET_RELATION_TYPE_FRIEND,     // 对应配置中的序号，1
            HIT_TARGET_RELATION_TYPE_ENIMY,      // 对应配置中的序号，2
        };

        int count = sizeof(g_HitTargetRelationTypeArray) / sizeof(const int);
		LoopBeginCheck(a)
        for (size_t t = 0; t < params.GetCount(); ++t)
        {
			LoopDoCheck(a)
            int index = atoi(params.StringVal(t));
            if (index >= 0 && index < count)
            {
                iRelationType |= (EHitTargetRelationType)g_HitTargetRelationTypeArray[index];
            }
        }

        return iRelationType;
    }

    int GetTargetRelationType(const char* szConfigRelationType)
    {
        CVarList vRes;
        ::util_split_string(vRes, szConfigRelationType, ",");		
        return GetTargetRelationType(vRes);
    }

    int GetTargetClassType(const IVarList& params)
    {
        int iHitTargetType = HIT_TARGET_TYPE_NONE;
        const int g_HitTargetTypeArray[] =
        {
            HIT_TARGET_TYPE_PLAYER,              // 玩家 对应配置中的序号，0
            HIT_TARGET_TYPE_MONSTER,             // 怪物 对应配置中的序号，1
            HIT_TARGET_TYPE_FUNCTION_NPC,        // 功能NPC 对应配置中的序号，2
            HIT_TARGET_TYPE_TASK_NPC,            // 任务NPC 对应配置中的序号，3
            HIT_TARGET_TYPE_BROKEN_NPC,          // 破碎NPC 对应配置中的序号，4
            HIT_TARGET_TYPE_PET_NPC,             // 宠物npc 对应配置中的序号，5
        };

        
        int count = sizeof(g_HitTargetTypeArray) / sizeof(const int);
		LoopBeginCheck(b)
        for (size_t t = 0; t < params.GetCount(); ++t)
        {
			LoopDoCheck(b)
            int index = atoi(params.StringVal(t));
            if (index >= 0 && index < count)
            {
                iHitTargetType |= (EHitTargetType)g_HitTargetTypeArray[index];
            }
        }
        return iHitTargetType;
    }

    int GetTargetClassType(const char* szConfigClassType)
    {
        CVarList vRes;
        ::util_split_string(vRes, szConfigClassType, ",");
        return GetTargetClassType(vRes);
    }

    int ConvertTargetInnerClassType(int iConfigClassType)
    {
        int iInternelClassType = 0;
        if (HIT_TARGET_TYPE_NONE == iConfigClassType)
        {
            iInternelClassType = TYPE_PLAYER | TYPE_NPC;
        }
        else
        {
            // 是否包含玩家
            if ((HIT_TARGET_TYPE_PLAYER & iConfigClassType) != 0)
            {
                iInternelClassType = TYPE_PLAYER;
            }

            // 是否包含npc
            if (HIT_TARGET_TYPE_PLAYER != iConfigClassType)
            {
                iInternelClassType |= TYPE_NPC;
            }
        }

        return iInternelClassType;
    }

} // end of namespace SkillUtilFuncs
