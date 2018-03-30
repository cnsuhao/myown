//--------------------------------------------------------------------
// 文件名:		TrapNpcModule.h
// 内  容:		陷阱NPC
// 说  明:		
// 创建日期:	2015年07月11日
// 创建人:		  
//    :	    
//--------------------------------------------------------------------

#ifndef __TrapNpc_H__
#define __TrapNpc_H__

#include "Fsgame/Define/header.h"

class TrapEventManagerModule;
class TrapNpcModule: public ILogicModule  
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);
	// 释放
	virtual bool Shut(IKernel* pKernel);

private:
	// 进入场景
	static int OnEntry(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 载入回调
	static int OnSpring(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);
	// 卸载回调
	static int OnEndSpring(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
	// 销毁回调
	static int OnDestroy(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
public:
    // 延迟心跳
    static int HB_Delay_Spring(IKernel* pKernel, const PERSISTID& self, int silce);
public:
	static TrapEventManagerModule* m_pTrapEventManager;
};

#endif // __TrapNpc_H__
