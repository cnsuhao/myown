
//------------------------------------------------------------------------------
// 文件名:      fight_module.h
// 内  容:      战斗系统接口
// 说  明:
// 创建日期:    2013年2月17日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef _FightModule_H_
#define _FightModule_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/FightDefine.h"
#include <cmath>

/////////////////////////////////////////////////////////////

class FightModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

public:

    /**
    *@brief  根据战斗规则公式给对方造成伤害
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 造成伤害的伤害来源对象
    *@param  [in] target 被伤害的目标
    *@param  [in] skill 技能对象
    *@param  [in] iSkillUUID 技能uuid
    *@return bool 
    */
    bool SkillDamageTarget(IKernel* pKernel,
                           const PERSISTID& self,
                           const PERSISTID& target,
                           const PERSISTID& skill,
                           const int64_t iSkillUUID);

    /**
    *@brief  事件伤害处理专用接口，防止因伤害消息触发伤害事件函数进入无限循环
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 攻击者
    *@param  [in] target 攻击目标
    *@param  [in] object 技能对象或buffer对象，对于无法确定的，可以是空
    *@param  [in] uuid 技能对象或buffer对象相应的uuid，对于无法确定的object，可以是空
    *@param  [in] bCanTriggerEvent 是否能触发事件
    *@param  [in] iDamageValue 造成的伤害值
    *@return bool 
    */
    bool EventDamageTarget(IKernel* pKernel, 
                           const PERSISTID& self,
                           const PERSISTID& target,
                           const PERSISTID& object,
                           const int64_t uuid,
                           bool bCanTriggerEvent, 
                           int iDamageValue);

	/**
    *@brief  技能事件伤害
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 攻击者
    *@param  [in] target 攻击目标
    *@param  [in] object 技能对象
    *@param  [in] uuid 技能对象或buffer对象相应的uuid，对于无法确定的object，可以是空
    *@param  [in] fRate 伤害比率
    *@return bool 
    */
    bool SkillEventDamageTarget(IKernel* pKernel, 
                           const PERSISTID& self,
                           const PERSISTID& target,
                           const PERSISTID& skill,
                           const int64_t iSkillUUID,
						   const float fRate);

public:

    /**
    *@brief  直接杀死目标
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 攻击者
    *@param  [in] target 被杀死对象
    *@object [in] skill or buffer
    */
    void KillTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& object, const int64_t uuid, int nDamageCate);

	//能够攻击目标
	static bool CanDamageTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target);
protected:
    // TODO
    // 技能命中事件消息处理
    bool CommandMsgProcess(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                           const PERSISTID& skill, const int64_t iSkillUUID,
                           EHIT_TYPE ePhyHitType);


    // 向客户端发送减血消息
    void CustomSkillDamageMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                              int iHPDamage, int iHitType, const PERSISTID& skill,
                              const int64_t iSkillUUID);


    // 对目标造成伤害的处理，
    int Damage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                const PERSISTID& skill, const int64_t iSkillUUID, int nTempDamageValue,
                EHIT_TYPE ePhyHitType);

	// Miss状态下的处理
	void MissSkillDamageTargetHandle(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill, const int64_t iSkillUUID);

    // 真正伤害处理函数
    int RealDamage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
                   int iHPDamage, int iHitType, bool bIsDamageEvent,
                   int iDamageCate, const IVarList& params);

protected:

	bool BufferModifyDamage(IKernel* pKernel, const PERSISTID& buffer, float& hp_damage);

    /// ----------------------------------------伤害值修正------------------------------------
    // 根据伤害计算公式调整伤害值
    void ModifyDamageByFormula(IKernel* pKernel, const PERSISTID& self,
                               const PERSISTID& skill, const PERSISTID& target,
                               EHIT_TYPE ePhyHitType,
                               float& fDamageValue);

	// 特殊NPC伤害处理
	bool ModifyDamageBySpecialState(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& target, int& nOutDamageValue);

	// 伤害目标前
	bool BeforeDamgeTarget( IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const PERSISTID& target,
				const int64_t iSkillUUID, int& nOutHitType, int& nOutDamageVal);
public:
    //静态变量，给静态函数调用
    static FightModule* m_pInstance;
};

#endif
