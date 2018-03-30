//--------------------------------------------------------------------
// 文件名:      SkillEventBase.h
// 内  容:      技能事件基类
// 说  明:		所有的技能事件都继承此基类
// 创建日期:    2014年11月24日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _Skill_Event_Base_H_
#define _Skill_Event_Base_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/SkillEventFuncIdDefine.h"

// 所有技能事件基类
class SkillEventBase
{
public:
	SkillEventBase();
	virtual ~SkillEventBase();

	// 创建技能事件
	static SkillEventBase* CreateSkillEvent(const EEventExecType func_id);

	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 运行技能事件
	virtual bool Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
		const PERSISTID& skill, const int64_t uuid, const int iEventType,
		const IVarList& params, IVarList& outMsgParams) = 0;
protected:

private:

};

#endif