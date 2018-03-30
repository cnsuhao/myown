//--------------------------------------------------------------------
// 文件名:      NpcBaseModule.h
// 内  容:      NPC基础模块
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __NpcBaseModule_H__
#define __NpcBaseModule_H__

#include "Fsgame/Define/header.h"

class NpcBaseModule: public ILogicModule  
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

	// 加载物品配置
	bool LoadNpcPathConfig(IKernel* pKernel);

public:

	static int OnLoad(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	//精英NPC位置
	/*static int QueryElitePos(IKernel* pKernel, const PERSISTID& player, 
		const PERSISTID& sender, const IVarList& args);*/
public:
	static NpcBaseModule* m_pNpcBaseModule;
};

#endif // __NpcBaseModule_H__
