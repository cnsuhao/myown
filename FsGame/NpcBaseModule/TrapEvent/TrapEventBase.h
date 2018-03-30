//--------------------------------------------------------------------
// 文件名:      TrapEventBase.h
// 内  容:      地雷触发事件基类
// 说  明:		所有的地雷触发事件都继承此基类
// 创建日期:    2015年07月13日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#ifndef _Trap_Event_Base_H_
#define _Trap_Event_Base_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define//TrapEventFuncIdDefine.h"

// 所有技能事件基类
class TrapEventBase
{
public:
	TrapEventBase();
	virtual ~TrapEventBase();

	// 创建技能事件
	static TrapEventBase* CreateTrapEvent(const SpringEvent func_id);

	// 初始化
	virtual bool OnInit(IKernel* pKernel);

	// 运行技能事件
	virtual bool Execute(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& target, const IVarList& vEventParams) = 0;
};

#endif