//--------------------------------------------------------------------
// 文件名:		LuaScriptModule.h
// 内  容:		定义LUA脚本的扩展函数
// 说  明:		部分lua脚本转移到c++中来
//				RunLuaScript函数用来给逻辑部分调用lua脚本, 如果脚本已经移植到c++中, 则直接调用注册的函数, 否则尝试调用lua脚本
//				OUTPUT_SCRIPT_INVOKE_INFO_TOFILE宏定义(dubug有效)旨在调查游戏正常运行中每个脚本调用频率, 用以参考是否将脚本转移到c中
//				这个宏每天将脚本调用次数输出到一个不同的log文件中(区分member)
//				如果要将lua文件移到c中, 仅仅需要以原lua文件路径以及调用函数名称(script.."_"..func)作为key注册到c接口中即可
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __LuaScriptModule_H__
#define __LuaScriptModule_H__

#include "Fsgame/Define/header.h"
#include <map>
#include <unordered_map>

#if defined _DEBUG
void extend_output_script_times(IKernel* pKernel, const PERSISTID& player, bool bFile = false);

void on_member_start(IKernel* pKernel);

// 输出脚本调用次数到文件中
#define OUTPUT_SCRIPT_INVOKE_INFO_TOFILE(pKernel, player) extend_output_script_times(pKernel, player, true)

// 输出脚本调用次数到gm面版
#define PRINT_SCRIPT_INVOKE_INFO(pKernel, player) extend_output_script_times(pKernel, player, false)
#define INIT_SCRIPT_INVOKE(pKernel) on_member_start(pKernel)

#else

#define OUTPUT_SCRIPT_INVOKE_INFO_TOFILE(pKernel, player)
#define PRINT_SCRIPT_INVOKE_INFO(pKernel, player)
#define INIT_SCRIPT_INVOKE(pKernel)

#endif

typedef int (*LUASCRIPT_FUNC)(IKernel* pKernel, const IVarList& in, int rtNums, IVarList* out);

// 注册LUA函数
bool RegLuaFunc(const char* key, LUASCRIPT_FUNC func);

// 调用lua接口
bool RunLua(IKernel* pKernel, const char* src, const char* func, const IVarList& in,
			int rtNums = 0, IVarList* pOut = NULL);


class LuaScriptModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

public:
	// 执行LUA脚本
	bool RunLuaScript(IKernel* pKernel, const char* src, const char* func, const IVarList& in,
		int rtNums, IVarList* pOut);

	// 注册LUA脚本
	bool RegLuaFunc(const char* key, LUASCRIPT_FUNC func, bool bCover = false);

public:
	static LuaScriptModule* m_pLuaScriptModule;

private:
	typedef std::unordered_map<std::string, LUASCRIPT_FUNC> LuaFuncMap;

	LuaFuncMap m_functions;
};

#define REG_LUA_FUNC(key, func) RegLuaFunc(key, func);

#endif // __LuaScriptModule_H__