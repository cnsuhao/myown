//--------------------------------------------------------------------
// 文件名:      SkillEventFuncBuffer.h
// 内  容:      技能Buff事件逻辑
// 说  明:		
// 创建日期:    2014年11月25日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _Skill_Event_Func_Buffer_H_
#define _Skill_Event_Func_Buffer_H_


#include "SkillEventBase.h"

// Buffer伤害事件 
// EVENT_EXEC_BUFF_CHANGE_TARGET_HP = 600,
class SkillEventBuffChangeHP : public SkillEventBase
{
public:
	// 运行技能事件
	virtual bool Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
		const PERSISTID& buff, const int64_t uuid, const int iEventType,
		const IVarList& params, IVarList& outMsgParams);
};

#endif