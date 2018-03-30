//------------------------------------------------------------------------------
// 文件名:      SkillStage.h
// 内  容:      技能阶段消息发送
// 说  明:      该类只在技能系统内部使用，且不要直接定义SkillStage的实例，而使用
//              其单例模式接口SkillStageSingleton::Instance()
// 创建日期:    2014年10月23日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __SKILLSTAGE_H__
#define __SKILLSTAGE_H__

#include "Fsgame/Define/header.h"

#include "utils/singleton.h"
#include "FsGame/Define/SkillDefine.h"
#include "FsGame/Define/SkillEventManagerDefine.h"

enum EMessageSendType
{
    EMSG_SEND_TYPE_ALL,         // 消息广播给可视范围内的所有人
    EMSG_SEND_TYPE_SELF,        // 消息只发给自己
    EMSG_SEND_TYPE_SELF_TARGET, // 消息只发给自己和对方
};

class SkillStage
{
public:
    // 开始	
    bool CustomSkillBeginStage(IKernel* pKernel,
                               const PERSISTID& self,
                               const char* szSkillConfigID,
                               const int64_t uuid, 
                               const ESkillStageTargetType eSkillStageTargetType, 
                               const IVarList& params);

    // 吟唱
    bool CustomSkillPrepareStage(IKernel* pKernel,
                                 const PERSISTID& self,
                                 const int64_t uuid,
                                 const int iPrepareTime);

    // 引导
    bool CustomSkillLeadStage(IKernel* pKernel,
                              const PERSISTID& self,
                              const int64_t uuid,
                              const int iLeadTime);

    // 立即发送效果，不合并消息数据
    bool CustomSkillEffectStageRealTime(IKernel* pKernel,
                                        const PERSISTID& self,                      // 攻击者
                                        const int64_t uuid,                         // 技能uuid
                                        const ESkillEventType eEventType,           // 技能事件类型
                                        const PERSISTID& target,                    // 效果作用对象 
                                        const ESkillStageEffectType eEffectType,    // 效果效果类型ID
                                        const IVarList& params,                     // 效果所需发送的数据
                                        const EMessageSendType eMsgSendType);       // 消息发送给哪些对象

    // 消息只发给自己
    bool CustomSkillEffectStageBySelf(IKernel* pKernel,
                                      const PERSISTID& self,              // 攻击者
                                      const int64_t uuid,                 // 技能uuid
                                      const int iCompressValue,			 // 技能事件和特效类型合并值
                                      const IVarList& params);            // eEventType类型的所有效果的参数，每个效果的参数需要按照这样的格式存放信息

    // 效果
    bool CustomSkillEffectStage(IKernel* pKernel,
                                const PERSISTID& self,              // 攻击者
                                const int64_t uuid,                 // 技能uuid
                                const int iCompressValue,			 // 技能事件和特效类型合并值
                                const IVarList& params);            // eEventType类型的所有效果的参数，每个效果的参数需要按照这样的格式存放信息

    // 打断
    bool CustomSkillBreakStage(IKernel* pKernel,
                               const PERSISTID& self,
                               const int64_t uuid,
                               const int reason);

    // 结束
    bool CustomSkillFinish(IKernel* pKernel,
                           const PERSISTID& self,
                           const int64_t uuid,
                           const int reason);

	// 处理技能空放
	int CustomSkillWithoutTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float fOrient);

	// 发送播放击飞效果
//	int CustomSkillHitFly(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,const PERSISTID& skill);

	// 客户端命中失败返回 仅测试用
	int CustomSkillClientHitFailed(IKernel* pKernel, const PERSISTID& self, const char* strSkillId, int nFailedType, const IVarList& targetList);

}; // end of class SkillStage

typedef HPSingleton<SkillStage> SkillStageSingleton;


#endif // __SKILLSTAGE_H__


