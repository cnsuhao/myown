//------------------------------------------------------------------------------
// 文件名:      SkillEventFuncIdDefine.h
// 内  容:      技能事件回调函数ID定义
// 说  明:
// 创建日期:    2014年11月24日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __SKILLEVENTFUNCIDDEFINE_H__
#define __SKILLEVENTFUNCIDDEFINE_H__

enum EEventExecType
{
	// 通用功能执行: 1~200
	// 技能独有功能执行 300~598
	// BUffer独有功能执行 620~699
	EVENT_EXEC_INVALID = 0,

	// 通用功能执行:1~200 ------------------------------------------------------
	//添加BUFF
	EVENT_EXEC_ADDBUFFER = 1,

	EVENT_EXEC_COMMON_END,
	//////////////////////////////////////////////////////////////////////////
	// 技能独有功能执行300~500--------------------------------------------------
	EVENT_EXEC_SKILL_START = 300,
	// 闪烁
	EVENT_EXEC_FLICKER_MOVE,

	// 召唤图腾(持续范围攻击npc)
	EVENT_EXEC_CALL_TOTEM,

	EVENT_EXEC_SKILL_END,
	//////////////////////////////////////////////////////////////////////////
	// BUffer独有功能执行600~800 ----------------------------------------------
	EVENT_EXEC_BUFF_START = 599,

    // BufferHP改变(加血或伤害)
    EVENT_EXEC_BUFF_CHANGE_TARGET_HP = 600,

	EVENT_EXEC_BUFF_END,

    EVENT_EXEC_TEST_TEST = 699,
	// 当前最大执行编号
	MAX_EXECUTE
};

#endif // __SKILLEVENTFUNCIDDEFINE_H__
