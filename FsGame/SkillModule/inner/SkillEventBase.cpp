//--------------------------------------------------------------------
// 文件名:      SkillEventBase.h
// 内  容:      技能事件基类
// 说  明:		所有的技能事件都继承此基类
// 创建日期:    2014年11月24日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "FsGame/SkillModule/inner/SkillEventBase.h"
#include "SkillEventFuncSkill.h"
#include "SkillEventFuncCommon.h"
#include "SkillEventFuncBuffer.h"


SkillEventBase::SkillEventBase()
{

}

SkillEventBase::~SkillEventBase()
{

}

// 创建技能事件
SkillEventBase* SkillEventBase::CreateSkillEvent(const EEventExecType func_id)
{
	SkillEventBase* pEventBase = NULL;

	switch(func_id)
	{
	// 通用
	case EVENT_EXEC_ADDBUFFER:
		pEventBase = new SkillEventAddBuffer;
		break;
	// 技能
	case EVENT_EXEC_FLICKER_MOVE:
		pEventBase = new SkillEventFlickerMove;
		break;
	case EVENT_EXEC_CALL_TOTEM:
		pEventBase = new SkillEventCallTotem;
		break;
	// Buffer
	case EVENT_EXEC_BUFF_CHANGE_TARGET_HP:
		pEventBase = new SkillEventBuffChangeHP;
		break;
	}

	return pEventBase;
}

// 初始化
bool SkillEventBase::OnInit(IKernel* pKernel)
{
	return true;
}
