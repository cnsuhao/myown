//--------------------------------------------------------------------
// 文件名:      SkillEventFuncSkill.h
// 内  容:      技能事件逻辑
// 说  明:		
// 创建日期:    2014年11月24日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _Skill_Event_Func_Skill_H_
#define _Skill_Event_Func_Skill_H_

#include "SkillEventBase.h"
#include <vector>
#include <map>

// 	// 闪烁
// EVENT_EXEC_FLICKER_MOVE = 301,
class SkillEventFlickerMove : public SkillEventBase
{
public:
	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 运行技能事件
	// 参数1:伤害系数
	virtual bool Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
		const PERSISTID& skill, const int64_t uuid, const int iEventType,
		const IVarList& params, IVarList& outMsgParams);

};

// 召唤图腾(持续范围攻击npc)
// EVENT_EXEC_CALL_TOTEM = 302,
class SkillEventCallTotem : public SkillEventBase
{
public:
	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 运行技能事件
	// 参数1:伤害系数
	virtual bool Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
		const PERSISTID& skill, const int64_t uuid, const int iEventType,
		const IVarList& params, IVarList& outMsgParams);
private:
	
	// 创建陷阱
	static int H_TotemAttack(IKernel* pKernel, const PERSISTID& self, int slice);
};
#endif