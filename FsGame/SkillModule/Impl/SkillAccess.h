//------------------------------------------------------------------------------
// 文件名:      skill_access.h
// 内  容:      技能对象属性访问、技能状态相关查询
// 说  明:      该类只在技能系统内部使用，且不要直接定义SkillAccess的实例，而使用
//              其单例模式接口SkillAccessSingleton::Instance()
// 创建日期:    2014年10月27日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __SKILLACCESS_H__
#define __SKILLACCESS_H__

#include "Fsgame/Define/header.h"

#include "utils/Singleton.h"
#include "FsGame/Define/SkillDefine.h"
#include "FsGame/Define/FightDefine.h"
#include <vector>

struct SkillMainData;

class SkillAccess
{
public:
    //
    bool Init(IKernel* pKernel);

public:
    // 指定技能是否正在冷却
    bool IsCoolDown(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);
public:
    // 获取技能作用目标,位置,方向
    bool GetSkillTargetAndPos(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, PERSISTID& useskilltarget, float& dx, float& dy, float& dz, float& doirent);

    // 获得技能的冷却时间
    bool GetCoolDownTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, int& iPubCooldowntime, int& iCateCooldowntime, int& iCoolCategory);
	
	// 获取配置信息中多次命中时额外的伤害系数
	void GetExtraHitDamageRatio(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float& fExtraHitDamageRatio);

    // 获取技能的释放目标类型（有目标和无目标两种：返回USESKILL_TYPE_TARGET或者USESKILL_TYPE_POSITION）
    int GetUseSkillType(IKernel* pKernel, const PERSISTID& skill);

	// 验证位移的有效性 nObjType对应SkillDisplaceObject
	bool IsValidDisplace(IKernel* pKernel, const PERSISTID& self, const char* strSkillId, int nObjType, float dx, float dz);

	// 查询技能的位移类型和速度 nObjType对应SkillDisplaceObject
	bool QuerySkillDisplaceTypeAndSpeed(IKernel* pKernel, const char* strSkillId, const int nObjType, int& nOutDisplaceType, float& fDisplaceSpeed);

    /// -------------------------------------------------------------------------
    // 获取技能释放选择类型
    ESkillSelectType GetSkillSelectType(IKernel* pKernel, const PERSISTID& skill);

    // 获取技能射程参数(参数中有self的原因是，今后射程参数可能会受到self属性的影响）
    bool GetSkillCastRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, SkillCastRangeParams& cast_range_params,const SkillMainData* pSkillMain);

    // 获取技能作用范围参数(参数中有self的原因是，今后作用范围参数可能会受到self属性的影响，目前受蓄力进度的影响）
    bool GetSkillHitRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, int nHitIndex, SkillHitRangeParams& hit_range_params);

	// 通过技能id找到攻击距离
	const float GetSkillHitDistance(IKernel* pKernel, const char* szSkillId);

    // 获取技能可命中对象关系
    bool GetSkillHitTargetRelation(IKernel* pKernel, const PERSISTID& skill, int& iTargetRelationType, int& iTargetClassType);

	// 是否客户端选中目标命中
	bool IsClientHitTarget(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj);

	// 获取当前技能剩余的CD (单位:毫秒)
	int GetSkillLeftCD(IKernel* pKernel, const PERSISTID& self, const char* strSkill);

	// 获取当前命中次数
	int GetHitIndex(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj);

	// 计算仇恨值
	int ComputeHateValue(IKernel* pKernel, const PERSISTID& skill, int nDamValue);
private:
	// 通过范围id找到对应的参数
	bool GetSkillHitRangeByHitRangeID(IKernel* pKernel, const char* szHitRangeID, SkillHitRangeParams& hitRangeParams);
}; // end of class SkillAccess

typedef HPSingleton<SkillAccess>    SkillAccessSingleton;

#endif // __SKILLACCESS_H__



