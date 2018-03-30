
//------------------------------------------------------------------------------
// 文件名:      SkillFlow.h
// 内  容:      技能流程相关处理
// 说  明:      该类只在技能系统内部使用，且不要直接定义SkillFlow的实例，而使用
//              其单例模式接口SkillFlowSingleton::Instance()
// 创建日期:    2014年10月27日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __SKILLFLOW_H__
#define __SKILLFLOW_H__

#include "Fsgame/Define/header.h"

#include "utils/singleton.h"
#include "FsGame/skillmodule/impl/skilldefinedatastruct.h"

class SkillFlow
{
public:
    bool Init(IKernel* pKernel);

public:
    
    /**
    *@brief  使用技能的最终接口，所有对技能使用的调用都会走这里
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 施法者
    *@param  [in] x,y,z 技能释放时施法者位置
    *@param  [in] skill 技能对象
    *@param  [in] target 技能作用对象
    *@param  [in] dx,dy,dz 技能作用点位置
    *@param  [in] dorient 技能作用方向
	*@param  [in] nBeginEffect 开始移动效果类型 0为没有移动效果
    *@return int 返回1表示可以释放，其他值代表对应才错误号返回
    */
    int UseSkill(IKernel* pKernel, const PERSISTID& self,
                 float x, float y, float z, const PERSISTID& skill,
                 const PERSISTID& target, float dx, float dy, float dz, float dorient);

	/*!
	 * @brief	客户端主导技能逻辑处理
	 * @param	self 施法者
	 * @param	x,z,dorient 技能释放时施法者位置
	 * @param	skill 技能对象
	 * @param	nSkilluid 技能uid
	 * @param	nCurHitIndex 当前命中次数
	 * @param	targetList 不可移动的目标列表
	 * @param	moveTargetList 要移动的目标列表(带有击退、击飞等效果)
	 * @return	int
	 */
	int DoSkillHitByClient(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const int64_t nSkilluid,
						 const int nCurHitIndex, const IVarList& targetList, const IVarList& moveTargetList);
public:
    // 是否是技能流程
    bool IsSkillFlow(IKernel* pKernel, const PERSISTID& self);

    // 是否正在引导
    bool IsLeading(IKernel* pKernel, const PERSISTID& self);

    // 是否在准备阶段中
    bool IsPreparing(IKernel* pKernel, const PERSISTID& self);

    // 是否正在命中期
    bool IsHiting(IKernel* pKernel, const PERSISTID& self);

    // 判断是否在正在使用技能
    int IsSkillUseBusy(IKernel* pKernel, const PERSISTID& self);

    /**
    *@brief  打断技能流程
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 施法者
    *@param  [in] breaker 技能流程打断者
    *@param  [in] bForce 是否强制打断
    *@return bool 
    */
    bool BreakSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& breaker, bool bForce, int iBreakReason);

    // 获取作用范围内的目标
    bool GetHitRangeTargets(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
        float x, float y, float z, int nHitIndex, IVarList& result);
	// 判断释放可以强制释放技能
	int CheckForceUseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

private:
    // -------------------------------流程回调----------------------------------
    //流程准备期开始的回调函数
    static bool OnFlowBeforePrepare(IKernel* pKernel, const PERSISTID& self);
    // 流程准备期结束的回调函数
    static bool OnFlowPrepare(IKernel* pKernel, const PERSISTID& self);
    //流程引导开始的回调函数
    static bool OnFlowBeforeLead(IKernel* pKernel, const PERSISTID& self);
    // 流程命中期结束的回调函数
    static bool OnFlowHit(IKernel* pKernel, const PERSISTID& self);
    // 流程被打断的回调函数
    static bool OnFlowBreak(IKernel* pKernel, const PERSISTID& self);
    // 流程等待期结束的回调函数
    static bool OnFlowFinish(IKernel* pKernel, const PERSISTID& self);
private:
    // -----------------------------流程处理函数--------------------------------
    // 准备时间已到
    bool DoPrepareSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
                        const PERSISTID& target, float x, float y, float z);

    // 命中时间已到，开始执行技能的效果
    bool DoHitSkill(IKernel* pKernel, const PERSISTID& self,
                    const PERSISTID& skill, const int64_t iSkillUUID,
                    const PERSISTID& target, float x, float y, float z);

    // 特效命中处理
    bool DoEffectHitTargetProcess(IKernel* pKernel, const PERSISTID& self, 
                                  const PERSISTID& skill, int64_t iSkillUUID,
                                  const PERSISTID& target, const IVarList& targetList);

    // 技能使用结束
    bool DoFinishSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

    // 技能使用被打断
    bool DoBreakSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

private:
    // 开始使用技能
    void BeginUseSkill(IKernel* pKernel, const PERSISTID& self,
                       float x, float y, float z, float orient, const PERSISTID& skill,
                       const PERSISTID& target, float dx, float dy, float dz);
	
	// 技能释放前的信息设置
    void InitBeginUseSkillInfo(IKernel* pKernel, const PERSISTID& self,
                               const PERSISTID& skill, int64_t iSkillUUID, 
                               const PERSISTID& target, float dx, float dy, float dz);

    void PrepareUseSkill(IKernel* pKernel, const PERSISTID& self,
                                const PERSISTID& skill, int64_t iSkillUUID,
                                const PERSISTID& target, 
                                float dx, float dy, float dz, float orient);

    bool RandomHitTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float dx, float dy, float dz, int count, IVarList& result);
    bool RandomHitTargetPos(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float dx, float dy, float dz, int count, IVarList& result);
    bool RandomHitTargetRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float dx, float dy, float dz, float orient, int count, IVarList& result);
    bool RandomCirclePosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float dx, float dy, float dz, float orient, int count, IVarList& result);
    bool RandomRectPosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float dx, float dy, float dz, float orient, int count, IVarList& result);
    bool GetSkillHitRangeByRandom(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, std::vector<SkillHitRangeParams>& vecRandomRanges);

    // 开始使用技能流
    void BeginSkillFlow(IKernel* pKernel, const PERSISTID& self);

    // 获取技能蓄力，准备,引导时间
    bool GetSkillCollocateTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,const SkillMainData *pSkillMain,
							int& iPrepareTime, int& iLeadTime, int& iLeadSepTime);
    
    // 获得多命中流的命中时间列表
    bool GetMultipleFlowRealHitTime(IKernel* pKernel, const PERSISTID& self, IGameObj* pSkill, const SkillMainData *pSkillMain, IVarList& hittimelist);

    //////////////////////////////////////////////////////////////////////////
    // 清除使用技能过程中设置的相关属性
    bool ClearUseSkillData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

    // 更新释放者位置
    void UpdateObjectLocate(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill, float x, float y, float z, float dorient);

	// 增加硬直状态
	void AddSkillStiffState(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

    // 冷却处理 返回CD时间
    int ComputeSkillCoolTime(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	// 群攻伤害需要一起发送的技能处理逻辑
	//void DoRandomMultiAttack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const int64_t nSkillUid, const int nHitCount, const IVarList& targetList);

	// 客户端命中验证初始化
	void InitClientValidData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	// 技能硬直状态结束,有这个心跳的对象只能播放技能,不能做其他AI行为
	static int H_SkillStiffStateEnd(IKernel* pKernel, const PERSISTID& creator, int slice);

	// 结算命中一个目标
	void OnHitOneTarget(IKernel* pKernel, PERSISTID target, const PERSISTID& skill, IGameObj* pSelfObj, const SkillMainData* pSkillMain, const int64_t nSkilluid);
}; // end of class SkillFlow

typedef HPSingleton<SkillFlow> SkillFlowSingleton;

#endif // __SKILLFLOW_H__
