#if !defined __ILUAEXTMODULE_H__
#define __ILUAEXTMODULE_H__

#include "Fsgame/Define/header.h"

#include "SDK/lua/lstate.h"

#define CHECK_ARG_NUM(state, name, num) if (pKernel->LuaGetArgCount(state) != num) \
{ pKernel->LuaErrorHandler(state, #name" argument number error"); return 1; }
#define CHECK_ARG_INT(state, name, index) if (! pKernel->LuaIsInt(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not int"); return 1; }
#define CHECK_ARG_INT64(state, name, index) if (! pKernel->LuaIsInt64(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not int64"); return 1; }
#define CHECK_ARG_FLOAT(state, name, index) if (! pKernel->LuaIsFloat(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not float"); return 1; }
#define CHECK_ARG_DOUBLE(state, name, index) if (! pKernel->LuaIsDouble(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not double"); return 1; }
#define CHECK_ARG_STRING(state, name, index) if (! pKernel->LuaIsString(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not string"); return 1; }
#define CHECK_ARG_WIDESTR(state, name, index) if (! pKernel->LuaIsWideStr(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not wide string"); return 1; }
#define CHECK_ARG_OBJECT(state, name, index) if (! pKernel->LuaIsObject(state, index)) \
{ pKernel->LuaErrorHandler(state, #name" argument "#index" not object"); return 1; }

// GM命令分发执行函数
typedef int (__cdecl* GM_CALL_BACK_FUNC)(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
#define DECL_DISPATCH_GM_FUNCTION(back) \
{ \
	LuaExtModule::RegisterCallBackFunction(#back, back); \
}

class ILuaExtModule : public ILogicModule
{
public:
	//获得IKernel指针
	static IKernel * GetKernel(void * state)
	{
		return (IKernel*)((lua_State*)state)->pCore;
	}

	//获得参数列表
	static bool GetVarList(void * state, int beg, int end, IVarList & msg)
	{
		IKernel * pKernel = GetKernel(state);


        LoopBeginCheck(a);
		for (int i = beg; i <= end; ++i)
		{
            LoopDoCheck(a);

			// 整数的判断必须放在最前面
			if (pKernel->LuaIsInt(state, i))
			{
				msg.AddInt(pKernel->LuaToInt(state, i));
			}
			else if (pKernel->LuaIsString(state, i))
			{
				msg.AddString(pKernel->LuaToString(state, i));
			}
			else if (pKernel->LuaIsInt64(state, i))
			{
				msg.AddInt64(pKernel->LuaToInt64(state, i));
			}
			else if (pKernel->LuaIsFloat(state, i))
			{
				msg.AddFloat(pKernel->LuaToFloat(state, i));
			}
			else if (pKernel->LuaIsDouble(state, i))
			{
				msg.AddDouble(pKernel->LuaToDouble(state, i));
			}
			else if (pKernel->LuaIsWideStr(state, i))
			{
				msg.AddWideStr(pKernel->LuaToWideStr(state, i));
			}
			else if (pKernel->LuaIsObject(state, i))
			{
				msg.AddObject(pKernel->LuaToObject(state, i));
			}
			else 
			{
				return false;
			}
		}

		return true;
	}
};

#endif //__ILUAEXTMODULE_H__