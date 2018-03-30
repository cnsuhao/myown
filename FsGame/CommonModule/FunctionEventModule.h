//--------------------------------------------------------------------
// 文件名:      FunctionEventModule.h
// 内  容:      游戏功能配置模块
// 说  明:
// 创建日期:    2017年02月10日
// 创建人:      李海罗
//--------------------------------------------------------------------

#ifndef _FUNCTION_EVENT_MODULE_H_
#define _FUNCTION_EVENT_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/FunctionEventDefine.h"

/* ----------------------
 * 事件计时器类
 * ----------------------*/
class FunctionEventModule : public ILogicModule
{
public:
    virtual bool Init(IKernel *pKernel);
    virtual bool Shut(IKernel *pKernel);

	// 查询功能物品绑定状态
	static int	GetItemBindState(int nFunctionId);
	// 查询物品是否优先使用
	static bool	GetItemBindPriorUse(int nFunctionId);

private:
	// 重新加载配置
	static void ReloadConfig(IKernel *pKernel);

private:
#pragma pack(push, 1) 
	struct FuncInfo
	{
		bool bind_state;
		bool bind_prior_use;
	};
#pragma pack(pop)
	static FuncInfo	ms_funcInfo[FUNCTION_EVENT_ID_END];
};

#endif // _FUNCTION_EVENT_MODULE_H_
