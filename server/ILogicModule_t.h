//--------------------------------------------------------------------
// 文件名:		ILogicModule_t.h
//内  容:       游戏逻辑模块
//说  明:       继承于：ILogicModule_t
//
//创建日期:     2017年07月21日
//创建人:       kevin 
// 版权所有:	WhalesGame Technology co.Ltd      
//--------------------------------------------------------------------

#ifndef _I_SERVER_LOGIC_GAME_MODULE_H
#define _I_SERVER_LOGIC_GAME_MODULE_H

#include "utils/Singleton.h"

// Game逻辑模块接口

class ILogicModule_t : public ILogicModule
{
public:
	virtual ~ILogicModule_t() = 0;
public:
	// command
	virtual void OnModuleCommand(IKernel* pKernel, int cmd_id, const IVarList& args) = 0;
};

inline ILogicModule_t::~ILogicModule_t() {}

//辅助module查找。
template<class T>
inline T* __WgFindLogicModuleByName(IKernel * pKernel,const char* module_name)
{
	T* p_dc = static_cast<T*>(pKernel->GetLogicModule(module_name));
	if (p_dc == NULL)
	{
		Assert(0);
	}
	return  p_dc;
}
inline ILogicModule_t* WgFindModuleByName(IKernel * pKernel, const char* module_name)
{
	ILogicModule_t* p_dc = static_cast<ILogicModule_t*>(pKernel->GetLogicModule(module_name));
	if (p_dc == NULL)
	{
		Assert(0);
	}
	return  p_dc;
}
#define  WgFindLogicModuleByName(pKernel,module_name)  __WgFindLogicModuleByName<module_name>(pKernel,#module_name)


#endif // _SERVER_ILOGICMODULE_H

