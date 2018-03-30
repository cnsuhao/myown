
//--------------------------------------------------------------------
// 文件名:      interface\skill_configdata.h
// 内  容:      技能配置到数据结构之间的转换
// 说  明:      
// 创建日期:    2014年10月27日
// 创建人:       
//    :       
//--------------------------------------------------------------------



#ifndef __SKILL_CONFIGDATA_H__
#define __SKILL_CONFIGDATA_H__

#include "FsGame/Define/SkillDefine.h"
#include "public/IVarList.h"
#include "utils/math/math.h"


struct SkillHitRangeParams;

namespace SkillUtilFuncs
{
    extern bool GetSkillHitRangeData(const IVarList& configdata, int iHitRangeType, float fRangeScale, SkillHitRangeParams& hitRangeParams);

    // 跟进配置参数处理偏移，转换成对象图形参数
    extern bool GetSphereParams(const SkillHitRangeParams& hitRangeParams, const FmVec3& basePos, const float orient, FmVec2& pos, float& fShapeOrient, float& r, float& R, float& angle);
    extern bool GetCylinderParams(const SkillHitRangeParams& hitRangeParams, const FmVec3& basePos, const float orient, FmVec2& pos, float& fShapeOrient, float& r, float& R, float& angle);
    extern bool GetRectangularParams(const SkillHitRangeParams& hitRangeParams, const FmVec3& basePos, const float orient, FmVec2& pos, float& fShapeOrient, float& width, float& length);
	extern bool GetPosCylinderParams(const SkillHitRangeParams& hitRangeParams, float& fRadius);
	
	// 技能筛选目标关系
    extern int GetTargetRelationType(const IVarList& params);
    extern int GetTargetRelationType(const char* szConfigRelationType);
    extern int GetTargetClassType(const IVarList& params);
    extern int GetTargetClassType(const char* szConfigClassType);
    extern int ConvertTargetInnerClassType(int iConfigClassType);
}

#endif // __SKILL_CONFIGDATA__H__
