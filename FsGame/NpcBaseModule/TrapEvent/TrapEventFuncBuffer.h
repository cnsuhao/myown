//--------------------------------------------------------------------
// 文件名:      TrapEventFuncBuffer.h
// 内  容:      陷阱NPC触发Buff事件逻辑
// 说  明:		
// 创建日期:    2015年07月13日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#ifndef _Trap_Event_Func_Buffer_H_
#define _Trap_Event_Func_Buffer_H_
#include "TrapEventBase.h"

//单人BUFF
//SPRINGEVENT_SINGLE_BUFF = 3, 
class TrapEventSingleBuff : public TrapEventBase
{
public:
	virtual bool Execute(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& target, const IVarList& vEventParams);
};


//群体BUFF
//SPRINGEVENT_MUTI_BUFF = 4, 
class TrapEventMutiBuff : public TrapEventBase
{
public:
	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 延迟心跳
	static int HB_Delay_Spring(IKernel* pKernel, const PERSISTID& self, int silce);

	// 运行技能事件
	virtual bool Execute(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& target, const IVarList& vEventParams);
private:
	static void OnAddBuff(IKernel* pKernel, const PERSISTID& self);
};

#endif