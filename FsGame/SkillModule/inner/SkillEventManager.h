//--------------------------------------------------------------------
// 文件名:      skill_event_manager.h
// 内  容:      技能事件处理器
// 说  明:
// 创建日期:    2014年11月25日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _SkillEventManager_H_
#define _SkillEventManager_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/SkillEventFuncIdDefine.h"
#include "FsGame/Define/SkillEventManagerDefine.h"

//检查参数数量
#define CHECK_PARAM_COUNT(real_count, expect_count)\
	if (real_count < expect_count)\
	{\
	return false;\
	}

struct SkillEffectData;
class SkillEventBase;

class SkillEventManager : public ILogicModule
{
public:
    SkillEventManager() {
		memset(m_pEventContainer, 0, sizeof(m_pEventContainer));
	}
    ~SkillEventManager() {}

public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 清理
    virtual bool Shut(IKernel* pKernel);

public:

    // 加载事件处理函数
    bool InitEventExecutes(IKernel* pKernel);

	// 处理单个执行操作, 事件函数的iEventType对于技能来说是ESkillEventType，对于buffer来说是EBufferEventType
	bool ExecuteOperate(IKernel* pKernel,
						const EEventExecType funcid,
						const IVarList& OpParams,
						const PERSISTID& self,
						const PERSISTID& object,
						const int64_t uuid,
						const int iEventType,
						const IVarList& params,
						IVarList& outMsgParams);
public:

    // 技能检测可以释放
    static bool SkillBegin(IKernel* pKernel,
                           const PERSISTID& self,
                           const PERSISTID& skill,
                           int64_t iSkillUUID,
                           const PERSISTID& target,
                           float dx, float dy, float dz);

    // 技能开始准备
    static bool SkillPrepare(IKernel* pKernel,
                             const PERSISTID& self, 
                             const PERSISTID& skill,
                             const int64_t uuid,
                             const PERSISTID& target,
                             float x, float y, float z);

    // 技能准备之后
    static bool SkillAfterPrepare(IKernel* pKernel, 
                                  const PERSISTID& self,
                                  const PERSISTID& skill,
                                  const int64_t uuid,
                                  const PERSISTID& target,
                                  float x, float y, float z);

    // 技能命中之前
    static bool SkillBeforeHit(IKernel* pKernel,
                               const PERSISTID& self,
                               const PERSISTID& skill,
                               const int64_t uuid);

    // 技能命中对象
    static bool SkillAfterHit(IKernel* pKernel,
                              const PERSISTID& self,
                              const PERSISTID& skill, 
                              const int64_t uuid,
                              const PERSISTID& target);

    // 技能命中并已伤害了对象
    static bool SkillHitDamage(IKernel* pKernel,
                               const PERSISTID& self,
                               const PERSISTID& skill, 
                               const int64_t uuid,
                               const PERSISTID& target,
							   const int nDamageVal);

    // 技能使用结束
    static bool SkillFinish(IKernel* pKernel,
                            const PERSISTID& self,
                            const PERSISTID& skill,
                            const int64_t uuid,
                            const PERSISTID& target);

    // 技能被打断
    static bool SkillBreak(IKernel* pKernel,
                           const PERSISTID& self, 
                           const PERSISTID& skill,
                           const int64_t uuid);

	// 技能使用结束之后
	static bool SkillAfterFinish(IKernel* pKernel,
		const PERSISTID& self,
		const PERSISTID& skill,
		const int64_t uuid);

private:
	// 事件注册
	bool RegistSkillEvent(const EEventExecType func_id);

    //技能事件处理
    bool SkillEventProcess(IKernel* pKernel,
                                  const PERSISTID& self,
                                  const PERSISTID& skill,
                                  const int64_t uuid,
                                  const PERSISTID& target, 
                                  const ESkillEventType eSkillEventType,
                                  const EParamType eParamType,
                                  const IVarList& exParams);

    // 获取技能当前命中次序的高级效果包
    bool GetEffectPackList(IKernel* pKernel,
								  const PERSISTID& self,
                                  const PERSISTID& skill,
                                  CVarList& vEffectPackList);

    // 判断指定高绩效包的执行条件
    bool CanExecute(IKernel* pKernel,
                           const PERSISTID& self,
                           const PERSISTID& target,
                           const SkillEffectData* pSkillEffect,
                           const ESkillEventType eSkillEventType);

    // 获取事件执行参数
    bool GetParams(IKernel* pKernel,
                          const PERSISTID& self,
                          const PERSISTID& skill,
                          const PERSISTID& target,
                          const SkillEffectData* pSkillEffect,
                          const EParamType eParamType,
                          const IVarList& exParams,
                          CVarList& params);
private:
	SkillEventBase*		m_pEventContainer[MAX_EXECUTE];			// 技能事件容器

public:
    static SkillEventManager*   m_pInstance;
};

#endif // _SkillEventManager_H_
