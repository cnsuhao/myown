//--------------------------------------------------------------------
// 文件名:      TrapEventBase.cpp
// 内  容:      地雷触发事件基类
// 说  明:		所有的地雷触发事件都继承此基类
// 创建日期:    2015年07月13日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventBase.h"
#include "TrapEventFuncBuffer.h"
#include "TrapEventFuncDestory.h"

TrapEventBase::TrapEventBase()
{

}

TrapEventBase::~TrapEventBase()
{

}

// 创建技能事件
TrapEventBase* TrapEventBase::CreateTrapEvent(const SpringEvent func_id)
{
	TrapEventBase* pEventBase = NULL;

	switch(func_id)
	{
	//单人BUFF
	case SPRINGEVENT_SINGLE_BUFF:
		pEventBase = new TrapEventSingleBuff;
		break;
	//群体BUFF
	case SPRINGEVENT_MUTI_BUFF:
		pEventBase = new TrapEventMutiBuff;
		break;
	case SPRINGEVENT_DESTORY_OBJ:
		pEventBase = new TrapEventFuncDestory;
		break;
	}

	return pEventBase;
}

// 初始化
bool TrapEventBase::OnInit(IKernel* pKernel)
{
	return true;
}
