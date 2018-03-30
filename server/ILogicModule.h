//--------------------------------------------------------------------
// 文件名:		ILogicModule.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ILOGICMODULE_H
#define _SERVER_ILOGICMODULE_H

#include "../public/Macros.h"
#include "../server/LoopCheck.h"
// 逻辑模块接口

class IKernel;

class ILogicModule
{
public:
	virtual ~ILogicModule() = 0;

	// 初始化
	virtual bool Init(IKernel* pKernel) = 0;
	// 关闭
	virtual bool Shut(IKernel* pKernel) = 0;
	// 场景启动之前的初始化（所有OnCreateClass已经执行完毕）
	virtual bool BeforeLaunch(IKernel* pKernel) { return true; }
};

inline ILogicModule::~ILogicModule() {}

#endif // _SERVER_ILOGICMODULE_H

