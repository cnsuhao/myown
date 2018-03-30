//--------------------------------------------------------------------
// 文件名:      Server\FsRoomLogic\skill_module\impl\target_filter.h
// 内  容:      根据距离，区域的形状对目标进行过滤的系统定义
// 说  明:
// 创建日期:
// 创建人:
//    :       
//--------------------------------------------------------------------
#ifndef __TargetFilter_h__
#define __TargetFilter_h__

#include "Fsgame/Define/header.h"
#include "FsGame/Define/SkillDefine.h"
#include "utils/shape_collision.h"



//辅助的检测条件
struct FilterConditionParamStruct
{
    //bool bCheckHeight;      // 是否检测目标的高度差
    //float fHeightDiff;      // 可容忍的高度差
    float fSelfBodyRadius;      // 以自己位置为中心的圆的半径（受伤范围）

    int iRelationType;
    int iHitTargetType;
    bool bSortByHP;         // 按血量百分比排序，血量百分比越低到高
    bool bSortByDist;       // 是否按远近距离进行排序
    bool bIncludeDead;      // 是否包含死亡对象

    explicit FilterConditionParamStruct()
       // , bCheckHeight(false)   // 默认不检查高度差
       // , fHeightDiff(0.0f)     // 默认可容忍的高度差为0.0f
       : fSelfBodyRadius(0.5f)     // 默认受伤半径为0.0f

        , iRelationType(0)
        , iHitTargetType(0)
        , bSortByHP(false)
        , bSortByDist(false)      // 默认不检查排序
        , bIncludeDead(false)
    {

    }

    explicit FilterConditionParamStruct(const FilterConditionParamStruct& ins)
    {
        if (this != &ins)
        {
            //bCheckHeight = ins.bCheckHeight;
            //fHeightDiff = ins.fHeightDiff;
            fSelfBodyRadius = ins.fSelfBodyRadius;

            iRelationType = ins.iRelationType;
            iHitTargetType = ins.iHitTargetType;
            bSortByHP = ins.bSortByHP;
            bSortByDist = ins.bSortByDist;
            bIncludeDead = ins.bIncludeDead;
        }
    }
};


struct FilterParamStruct
{  
    float fTargetBodyRadius;    // 目标受伤半径
    float fShapeOrient;		// 形状朝向,对于矩形，此方向和长度的方向同向

    explicit FilterParamStruct()
        : fTargetBodyRadius(0.0f)
        , fShapeOrient(0.0f)
    {
    }

    explicit FilterParamStruct(const FilterParamStruct& ins)
    {
        if (this != &ins)
        {
            fTargetBodyRadius = ins.fTargetBodyRadius;
            fShapeOrient = ins.fShapeOrient;
        }
    }
};

class TargetFilter
{
public:
    static int GetHitRangeTargets(IKernel* pKernel, const PERSISTID& self, const FmVec3& basePos, PERSISTID locktarget,
                           const SkillHitRangeParams& hitRangeParams, const FilterConditionParamStruct& filterCondition,
                           int iCount, IVarList& result);


    static ETargetFilterResult IsInHitRange(IKernel* pKernel, const FmVec3& basePos, const FmVec3& targetPos,
        const SkillHitRangeParams& hitRangeParams, const FilterParamStruct& filterParams);

public:

    /// \brief 判断目标对象是否满足技能各种关系类型
    /// \param [in] pKernel 引擎内核指针
    /// \param [in] self 判断的主角
    /// \param [in] target 判断的目标
    /// \param [in] iHitRelationType  技能锁定对象的关系类型
    /// \param [in] iHitTargetType 技能可锁定的对象类型
    /// \param [in] iAddHitTargetType 技能可锁定的附加对象
    static ETargetFilterResult SatisfyRelation(IKernel* pKernel, const PERSISTID& self,
                                        const PERSISTID& target, int iHitRelationType, int iHitTargetType = 0);

protected:
    /// \brief 获取一定范围内的对象
    /// \param [in] pKernel 内核指针
    /// \param [in] self
    /// \param [in] basePos 范围形状基点坐标
    /// \param [in] locktarget 锁定对象
    /// \param [in] SkillHitRangeParams 作用范围参数结构
    /// \param [in] iInternelClassType 作用的逻辑对象类型
    /// \param [in] iCount 需要获取的数量
    /// \param [out] result 获取对象的列表
    /// \return 返回获取对象的数量
    static int GetTargetList(IKernel* pKernel, const PERSISTID& self, const FmVec3& basePos, PERSISTID locktarget,
                      const SkillHitRangeParams& hitRangeParams, int iInternelClassType, int iCount, IVarList& result);

    /// \brief 检测命中目标的关系类型是否符合
    /// \param [in] pKernel 内核指针
    /// \param [in] self    判断的主角
    /// \param [in] target  判断的目标
    /// \param [in] iRelationType 技能锁定对象的关系类型
    /// \return 返回检测结果
    static ETargetFilterResult  CheckHitTargetRelationType(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj, int iRelationType);

    /// \brief 检测命中目标的命中对象类型是否合法
    /// \param [in] pKernel 内核指针
    /// \param [in] self    判断的主角
    /// \param [in] target  判断的目标
    /// \param [in] iHitTargetType 技能锁定对象的命中类型
    static ETargetFilterResult CheckHitTargetType(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj, int iHitTargetType);

	// 检查分组号
	static bool CheckGroupID(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj);

	// 检查施法者与目标等级 针对玩家之间的pk限制
	static bool CheckLevelLimit(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj);
private:
    static bool SortObjectByDist(IKernel* pKernel, const PERSISTID& self, IVarList& result);
    static bool SortObjectByHP(IKernel* pKernel, IVarList& result);
};
#endif
