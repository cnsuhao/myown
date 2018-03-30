//--------------------------------------------------------------------
// 文件名:      DoorNpcModule.h
// 内  容:      功能NPC模块
// 说  明:		
// 创建日期:    2014年12月6日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __DoorNpcModule_H__
#define __DoorNpcModule_H__

#include "Fsgame/Define/header.h"

class AsynCtrlModule;
class DoorNpcModule: public ILogicModule  
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);
public:

	static int OnSpring(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static int OnEndSpring(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static AsynCtrlModule * m_pAsynCtrlModule;
};

#endif // __DoorNpcModule_H__
