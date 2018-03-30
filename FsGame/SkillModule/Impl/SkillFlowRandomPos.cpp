
//--------------------------------------------------------------------
// 文件名:      skill_flow_random_pos.cpp
// 内  容:      
// 说  明:      
// 创建日期:    2014年10月30日
// 创建人:      刘明飞
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------



#include "FsGame/skillmodule/impl/SkillFlow.h"

#include <string>
#include <vector>
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "public/portable.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/skillmodule/impl/skillaccess.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/skillmodule/impl/TargetFilter.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
//#include "FsGame/Interface/aiinterface.h"

namespace
{
    // 从一个容器里随机取n个元素
    bool RandomElement(const std::vector<int>& container, int count, std::vector<int>& result)
    {
        if (count <= 0)
        {
            return false;
        }

        int size = (int)container.size();
        if (size <= count)
        {
            std::vector<int> tmp(container);
            tmp.swap(result);
            return true;
        }

        // 在容器container中找到count个不重复的元素
        std::vector<bool> flags(size, false);
        for (int i=0; i<count; i++)
        {
            int index = ::util_random_int(size);
            if (!flags[index])
            {
                result.push_back(container[index]);
                flags[index] = true;
                continue;
            }
            else
            {
                bool bFound = false;
                // 向后查找
                int iBackwardIndex = index + 1;
                while (iBackwardIndex < size)
                {
                    if (!flags[iBackwardIndex])
                    {
                        bFound = true;
                        result.push_back(container[iBackwardIndex]);
                        flags[iBackwardIndex] = true;
                        break;
                    }
                    iBackwardIndex++;
                }
                // 向前查找
                int iForwardIndex = index - 1;
                while (!bFound && iForwardIndex >= 0)
                {
                    if (!flags[iForwardIndex])
                    {
                        bFound = true;
                        result.push_back(container[iForwardIndex]);
                        flags[iForwardIndex] = true;
                        break;
                    }
                    iForwardIndex--;
                }

                // 如果向前向后都找不到一个未被标示的元素，则返回。
                if (!bFound)
                {
                    return false;
                }
            } // end of if (!flags[index])
        } // end of for (int i=0; i<count; i++)

        return true;
    }

    bool RandomIndex(int size, int count, std::vector<int>& result)
    {
        std::vector<int> container(size);
        for (int i=0; i<size; ++i)
        {
            container[i] = i;
        }
        return RandomElement(container, count, result);
    }

    bool GetHitRangeCenterPos(const SkillHitRangeParams& params, float dx, float dy, float dz, float orient, float& cx, float& cy, float& cz)
    {
        switch (params.eHitRangeType)
        {
        case SKILL_HIT_RANGE_TYPE_SPHERE:
            {
                FmVec2 pos;
                float fShapeOrient, r, R, angle;
                SkillUtilFuncs::GetSphereParams(params, FmVec3(dx, dy, dz), orient, pos, fShapeOrient, r, R, angle);
                cx = pos.x;
                cz = pos.y;
                cy = dy;
            }
            break;

        case SKILL_HIT_RANGE_TYPE_CYLINDER:
            {
                FmVec2 pos;
                float fShapeOrient, r, R, angle;
                SkillUtilFuncs::GetCylinderParams(params, FmVec3(dx, dy, dz), fShapeOrient, pos, fShapeOrient, r, R, angle);
                cx = pos.x;
                cz = pos.y;
                cy = dy;
            }
            break;

        case SKILL_HIT_RANGE_TYPE_RECTANGULAR:
            {
                FmVec2 pos;
                float fShapeOrient, width, length;
                SkillUtilFuncs::GetRectangularParams(params, FmVec3(dx, dy, dz), fShapeOrient, pos, fShapeOrient, width, length);
                cx = pos.x;
                cz = pos.y;
                cy = dy;
            }
            break;

        default:
            break;
        } // end switch ...

        return true;
    }
}

bool SkillFlow::RandomHitTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, 
                                float dx, float dy, float dz, int count, IVarList& result)
{
    CVarList targetList;
    GetHitRangeTargets(pKernel, self, skill, dx, dy, dz, targetList);
    int targetCount = (int)targetList.GetCount();
    int n = count < targetCount ? count : targetCount;
    for (int i=0; i<n; i++)
    {
        result << targetList.ObjectVal(i);
    }

    return true;
}

bool SkillFlow::RandomHitTargetPos(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, 
                                   float dx, float dy, float dz, int count, IVarList& result)
{
    CVarList targetList;
    GetHitRangeTargets(pKernel, self, skill, dx, dy, dz, targetList);
    int targetCount = (int)targetList.GetCount();
    int n = count < targetCount ? count : targetCount;
    for (int i=0; i<n; i++)
    {
        PERSISTID obj = targetList.ObjectVal(i);
        if (!pKernel->Exists(obj))
        {
            continue;
        }
        float x, y, z, orient;
        pKernel->GetLocation(obj, x, y, z, orient);
        result << x << y << z;
    }
    return true;
}

bool SkillFlow::RandomCirclePosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, 
                                     float dx, float dy, float dz, float orient, int count, IVarList& result)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

    if (!pSkillObj->FindData("MulRandomPosData"))
    {
        MulRandom data;
        pSkillObj->AddDataBinary("MulRandomPosData", &data, sizeof(data));
    }
    MulRandom* pdata = (MulRandom*)pSkillObj->QueryDataBinary("MulRandomPosData");
    if (NULL == pdata)
    {
        return false;
    }
    pdata->Clear();

    // 获取技能配置的命中范围包
    std::vector<SkillHitRangeParams> vecHitRangeList;
    SkillAccessSingleton::Instance()->GetSkillHitRange(pKernel, self, skill, vecHitRangeList);
    int iHitRangeCount = (int)vecHitRangeList.size();
    if (iHitRangeCount <= 0)
    {
        return false;
    }

    // 默认取第一个命中范围包
    SkillHitRangeParams& params = vecHitRangeList[0];
    // 只处理圆形区域
    if (SKILL_HIT_RANGE_TYPE_CYLINDER != params.eHitRangeType)
    {
        return false;
    }

    if (count > MulRandom::MAX_SIZE)
    {
        count = MulRandom::MAX_SIZE;
    }

    // 默认是圆形区域
    FmVec2 pos;
    float fShapeOrient, r, R, angle;
    SkillUtilFuncs::GetCylinderParams(params, FmVec3(dx, dy, dz), fShapeOrient, pos, fShapeOrient, r, R, angle);

    // 获得命中范围中心点位置
    float cx, cy, cz;
    GetHitRangeCenterPos(params, dx, dy, dz, orient, cx, cy, cz);
    for (int i=0; i<count; i++)
    {
        float angle = ::util_random_float(PI2);
        float dest = ::util_random_float(R);
        float tx = cx + dest*sin(angle);
        float tz = cz + dest*cos(angle);
        result << tx << cy << tz;
        pdata->destXs[i] = tx;
        pdata->destZs[i] = tz;
    }
    pdata->type = RANDOM_TYPE_CIRCLE_POSITION;
    pdata->count = count;

    return true;
}

bool SkillFlow::RandomRectPosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, 
                                   float dx, float dy, float dz, float orient, int count, IVarList& result)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

    if (!pSkillObj->FindData("MulRandomPosData"))
    {
        MulRandom data;
        pSkillObj->AddDataBinary("MulRandomPosData", &data, sizeof(data));
    }
    MulRandom* pdata = (MulRandom*)pSkillObj->QueryDataBinary("MulRandomPosData");
    if (NULL == pdata)
    {
        return false;
    }

    // 获取技能配置的命中范围包
    std::vector<SkillHitRangeParams> vecHitRangeList;
    SkillAccessSingleton::Instance()->GetSkillHitRange(pKernel, self, skill, vecHitRangeList);
    int iHitRangeCount = (int)vecHitRangeList.size();
    if (iHitRangeCount <= 0)
    {
        return false;
    }

    // 默认取第一个命中范围包
    SkillHitRangeParams& params = vecHitRangeList[0];
    // 只处理圆形区域
    if (SKILL_HIT_RANGE_TYPE_CYLINDER != params.eHitRangeType)
    {
        return false;
    }

    if (count > MulRandom::MAX_SIZE)
    {
        count = MulRandom::MAX_SIZE;
    }

    // 默认是圆形区域
    FmVec2 pos;
    float fShapeOrient, r, R, angle;
    SkillUtilFuncs::GetCylinderParams(params, FmVec3(dx, dy, dz), fShapeOrient, pos, fShapeOrient, r, R, angle);

    // 获得命中范围中心点位置
    float cx, cy, cz;
    GetHitRangeCenterPos(params, dx, dy, dz, orient, cx, cy, cz);
    for (int i=0; i<count; i++)
    {
        float angle = ::util_random_float(PI2);
        float dest = ::util_random_float(R);
        float orient = ::util_random_float(PI2);
        pdata->rectOrient[i] = orient;
        float tx = cx + dest*sin(angle);
        float tz = cz + dest*cos(angle);
        result << tx << cy << tz;
        pdata->destXs[i] = tx;
        pdata->destZs[i] = tz;
    }
    pdata->type = RANDOM_TYPE_RECT_POSITION;
    pdata->count = count;

    return true;
}

// 会多次遍历，比较耗
bool SkillFlow::RandomHitTargetRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, 
                                     float dx, float dy, float dz, float orient, int count, IVarList& result)
{
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (!pKernel->Exists(self) || NULL == pSkillObj)
    {
        return false;
    }

    if (!pSkillObj->FindData("MulRandomPosData"))
    {
        MulRandom data;
        pSkillObj->AddDataBinary("MulRandomPosData", &data, sizeof(data));
    }
    MulRandom* pdata = (MulRandom*)pSkillObj->QueryDataBinary("MulRandomPosData");
    if (NULL == pdata)
    {
        return false;
    }

    if (count > MulRandom::MAX_SIZE)
    {
        count = MulRandom::MAX_SIZE;
    }

    // 获取技能配置的命中范围包
    std::vector<SkillHitRangeParams> vecHitRangeList;
    SkillAccessSingleton::Instance()->GetSkillHitRange(pKernel, self, skill, vecHitRangeList);
    int iHitRangeCount = (int)vecHitRangeList.size();

    // 随机取n个随机范围包索引
    int n = count < iHitRangeCount ? count : iHitRangeCount;
    std::vector<int> vecIndexs;
    RandomIndex(iHitRangeCount, n, vecIndexs);

    // 根据随机取到的索引找到命中范围包配置数据，计算范围中心点
    for (int i=0; i<n; ++i)
    {
        int index = vecIndexs[i];
        SkillHitRangeParams& params = vecHitRangeList[index];
        // 获得命中范围中心点位置
        float cx, cy, cz;
        GetHitRangeCenterPos(params, dx, dy, dz, orient, cx, cy, cz);
        result << cx << cy << cz;
        pdata->hitRagneIndexs[i] = index;
    }

    pdata->type = RANDOM_TYPE_HITRANGE;
    pdata->count = n;

    return true;
}


bool SkillFlow::GetSkillHitRangeByRandom(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, std::vector<SkillHitRangeParams>& vecRandomRanges)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSelfObj || NULL == pSkillObj)
    {
        return false;
    }

    const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
    const SkillMainData* pSkillMain = SkillDataQueryModule::m_pInstance->GetSkillMain(szSkillConfigID);
    if (NULL == pSkillMain)
    {
        return false;
    }

    if (!pSkillObj->FindData("MulRandomPosData"))
    {
        return false;
    }
    MulRandom* pdata = (MulRandom*)pSkillObj->QueryDataBinary("MulRandomPosData");
    if (NULL == pdata || pdata->count <= 0)
    {
        return false;
    }

    SkillHitRangeParams data;
    switch (pdata->type)
    {
    case RANDOM_TYPE_CIRCLE_POSITION:    // 随机圆形位置
        {
            data.eHitRangeType = SKILL_HIT_RANGE_TYPE_CYLINDER;
            for (int i=0; i<pdata->count; ++i)
            {
                data.skillhitrangeunion.cylinder.fHitRangeCylinderDistCentre = 0.0f;
                data.skillhitrangeunion.cylinder.fHitRangeCylinderDistMax = pSkillMain->GetRandomRadius();
                data.skillhitrangeunion.cylinder.fHitRangeCylinderDistMin = 0.0f;
                data.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle = PI2;
                data.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient = 0.0f;
                data.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient = 0.0f;

                vecRandomRanges.push_back(data);
            }
        }
        break;
            
    case RANDOM_TYPE_RECT_POSITION:      // 随机矩形位置
        {
            data.eHitRangeType = SKILL_HIT_RANGE_TYPE_RECTANGULAR;
            for (int i=0; i<pdata->count; ++i)
            {
                data.skillhitrangeunion.rectangular.fHitRangeRectangularDistCentre = 0.0f;
                data.skillhitrangeunion.rectangular.fHitRangeRectangularLength = pSkillMain->GetRandomLength();
                data.skillhitrangeunion.rectangular.fHitRangeRectangularWidth = pSkillMain->GetRandomWidth();
                data.skillhitrangeunion.rectangular.fHitRangeRectangularHeight = 0.0f;
                data.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient = 0.0f;
                data.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient = 0.0f;

                vecRandomRanges.push_back(data);
            }
        }
        break;
            
    case RANDOM_TYPE_HITRANGE:           // 随机命中范围
        {
            std::vector<SkillHitRangeParams> vecHitRangeList;
            SkillAccessSingleton::Instance()->GetSkillHitRange(pKernel, self, skill, vecHitRangeList);
            int iHitRangeCount = (int)vecHitRangeList.size();
            if (iHitRangeCount <= 0)
            {
                return false;
            }

            for (int i=0; i<pdata->count; i++)
            {
                int index = pdata->hitRagneIndexs[i];
                if (index < iHitRangeCount)
                {
                    vecRandomRanges.push_back(vecHitRangeList[index]);
                }
            }
        }
        break;

    default:
        break;
    } // end of switch...

    return true;
}

