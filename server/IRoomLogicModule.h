//--------------------------------------------------------------------
// 文件名:		IRoomLogicModule.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月5日
// 创建人:		丁有进
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IROOMLOGICMODULE_H
#define _SERVER_IROOMLOGICMODULE_H

#include "../public/Macros.h"
#include "../server/LoopCheck.h"
// 逻辑模块接口
//#include "../server/IRoomKernel.h"
class IKernel;

class IRoomLogicModule
{
public:
	virtual ~IRoomLogicModule() = 0;

	// 初始化
	virtual bool Init(IKernel* pKernel) = 0;
	// 关闭
	virtual bool Shut(IKernel* pKernel) = 0;
	// 场景启动之前的初始化（所有OnCreateClass已经执行完毕）
	virtual bool BeforeLaunch(IKernel* pKernel) { return true; }
};

inline IRoomLogicModule::~IRoomLogicModule() {}

#endif // _SERVER_ILOGICMODULE_H

