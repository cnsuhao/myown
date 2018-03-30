//--------------------------------------------------------------------
// 文件名:		LuaExtModule.h
// 内  容:		定义LUA脚本的扩展函数
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __LuaExtModule_H__
#define __LuaExtModule_H__

#include "FsGame/Interface/ILuaExtModule.h"
#include <map>

class LuaExtModule : public ILuaExtModule
{
public:
	virtual bool Init(IKernel* pKernel);
	virtual bool Shut(IKernel* pKernel);

public:
	//消息回调修改最大循环此时
	static int CommandSeMaxCirculateCount(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
	//修改最大循环次数
	static void SeMaxCirculateCount(IKernel* pKernel, const PERSISTID& self, int count);

#ifndef FSROOMLOGIC_EXPORTS
	// 分发GM命令
	static int OnCommandDispatchGM(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
#endif // FSROOMLOGIC_EXPORTS

	// 注册GM分发命令
	static int RegisterCallBackFunction(const char* strFunc, GM_CALL_BACK_FUNC func);

private:
	// GM命令调用的函数
	typedef std::map<std::string, GM_CALL_BACK_FUNC> MapGMFunc;
	static MapGMFunc m_mapGMFunc;
};

#endif // __LuaExtModule_H__