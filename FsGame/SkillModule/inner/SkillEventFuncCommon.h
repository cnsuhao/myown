//--------------------------------------------------------------------
// 文件名:      SkillEventFuncCommon.h
// 内  容:      技能通用事件逻辑
// 说  明:		
// 创建日期:    2014年11月25日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _Skill_Event_Func_Common_H_
#define _Skill_Event_Func_Common_H_


#include "SkillEventBase.h"

//////////////////////////////////////////////////////////////////////////
//添加BUFF
//EVENT_EXEC_ADDBUFFER = 1,
class SkillEventAddBuffer : public SkillEventBase
{
public:
	// 运行技能事件
	// 1    :添加BUFFER
	// 参数1: 添加的BUFF的ConfigID
	virtual bool Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
		const PERSISTID& skill, const int64_t uuid, const int iEventType,
		const IVarList& params, IVarList& outMsgParams);
private:
	// buff分类
	enum BuffTime
	{
		BOSS_ADD_BUFF,			// BOSS加的buff
		NORMAL_NPC_ADD_BUFF,	// 普通小怪加的buff

		MAX_BUFF_TYPE_NUM
	};

	enum
	{
		BASE_RATE_NUM = 100		// 概率基数
	};

	// 找到当前buff的时间比率
	const float GetBufferTimeRatio(IKernel* pKernel, const PERSISTID& self, const char* strBuffTimeRatio);
};

#endif