//--------------------------------------------------------------------
// 文件名:		LuaScriptModule.cpp
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

#include "LuaScriptModule.h"
#include "utils/extend_func.h"
#include <time.h>
#include "public/AutoMem.h"
#include "utils/util_func.h"

#if defined _DEBUG
	struct degug_invoke_lua_t
	{
		int count;
		__int64 use_time;

		degug_invoke_lua_t() : count(0), use_time(0) {}
	};

	std::map<std::string, degug_invoke_lua_t> g_script_times;
#endif

// 注册LUA函数
bool RegLuaFunc(const char* key, LUASCRIPT_FUNC func)
{
	if (LuaScriptModule::m_pLuaScriptModule)
	{
		return LuaScriptModule::m_pLuaScriptModule->RegLuaFunc(key, func);
	}

	return false;
}

// 调用LUA脚本, 游戏逻辑中应该调用这个接口
bool RunLua(IKernel* pKernel, const char* src, const char* func, const IVarList& in, int rtNums, IVarList* pOut)
{
	if (LuaScriptModule::m_pLuaScriptModule)
	{
		return LuaScriptModule::m_pLuaScriptModule->RunLuaScript(pKernel, src, func, in, rtNums, pOut);
	}

	return false;
}

LuaScriptModule* LuaScriptModule::m_pLuaScriptModule = NULL;

// 初始化
bool LuaScriptModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	// 指针
	m_pLuaScriptModule = this;

	// 初始化脚本调用
	INIT_SCRIPT_INVOKE(pKernel);

	return true;
}

// 关闭
bool LuaScriptModule::Shut(IKernel* pKernel)
{
	return true;
}

// 执行脚本
bool LuaScriptModule::RunLuaScript(IKernel* pKernel, const char* src, const char* func, const IVarList& in, 
								   int rtNums, IVarList* pOut)
{
	static char buf[512];

	SPRINTF_S(buf, "%s_%s", src, func);

	char* key = strlwr(buf);

	LuaFuncMap::iterator it = m_functions.find(key);

	if (it != m_functions.end())
	{
		LUASCRIPT_FUNC f = it->second;
		f(pKernel, in, rtNums, pOut);

		return true;
	}

	if (!pKernel->LuaFindScript(src))
	{
		if (!pKernel->LuaLoadScript(src))
		{
			return false;
		}
	}

	if (!pKernel->LuaFindFunc(src, func))
	{
		return false;
	}

#if defined _DEBUG
	__int64 st = util_get_time_64();
#endif

	bool bret = pKernel->LuaRunFunc(src, func, in, rtNums, pOut);

#if defined _DEBUG
	st = util_get_time_64() - st;
	degug_invoke_lua_t& arg = g_script_times[buf];
	arg.count += 1;
	arg.use_time += st;
#endif

	return bret;
}

// 注册脚本接口
bool LuaScriptModule::RegLuaFunc(const char* key, LUASCRIPT_FUNC func, bool bCover)
{
	// 不允许注册空指针, key也不能是空或空指针
	if (func == NULL || key == NULL || key[0] == NULL)
	{
		return false;
	}

	// key转换为小写
	size_t len = strlen(key) + 1;
	TAutoMem<char,128> auto_str(len);
	char* buf = auto_str.GetBuffer();
	strcpy_s(buf, len, key);
	buf[len-1] = '\0';
	char* reg_key = strlwr(buf);


	// 如果已经注册了这个key, 那么考虑bCover(是否覆盖原来的函数)
	LuaFuncMap::iterator it = m_functions.find(reg_key);

	if (it != m_functions.end())
	{
		if (!bCover)
		{
			return false;
		}

		it->second = func;
		return true;
	}

	// 增加一个注册
	m_functions[reg_key] = func;
	
	return true;
}

// 以下用来测试脚本调用(仅debug有效)
#if defined _DEBUG

void extend_get_file_name(IKernel* pKernel, char* filename, size_t size)
{
	time_t now = time(NULL);
	tm* t = localtime(&now);
	
#ifndef FSROOMLOGIC_EXPORTS
	int memberId = pKernel->GetMemberId();
#else
	int memberId = 0;
#endif // FSROOMLOGIC_EXPORTS

	SafeSprintf(filename, size, "script_invoke_%d__%d_%d_%d.log", memberId, 
		t->tm_year+1900, t->tm_mon+1, t->tm_mday);
	
}



void extend_output_script_times(IKernel* pKernel, const PERSISTID& player, bool bFile/* = false*/)
{
	if (bFile)
	{
		char filename[256];
		extend_get_file_name(pKernel, filename, sizeof(filename));

		FILE* file = fopen(filename, "w+");
		if (file == NULL)
		{
			return;
		}

		char buf[512];

		LoopBeginCheck(a);
		for (std::map<std::string, degug_invoke_lua_t>::iterator it = g_script_times.begin(); it != g_script_times.end(); ++it)
		{
			LoopDoCheck(a);

			degug_invoke_lua_t& arg = it->second;
			SPRINTF_S(buf, "%s\t%d\t%I64d\n", it->first.c_str(), 
				arg.count, arg.use_time);

			fwrite(buf, strlen(buf), 1, file);
		}

		fclose(file);
	}
	else
	{
		LoopBeginCheck(b);
		for (std::map<std::string, degug_invoke_lua_t>::iterator it = g_script_times.begin(); it != g_script_times.end(); ++it)
		{
			LoopDoCheck(b);

			degug_invoke_lua_t& arg = it->second;
			CVarList var;
			var << "sysinfo" << 1 << "{@0:file}={@1:num}   usetime={@2:num}" << it->first 
				<< arg.count << arg.use_time;
			pKernel->Custom(player, var);
		}
	}
}

void on_member_start(IKernel* pKernel)
{
	char filename[256];
	extend_get_file_name(pKernel, filename, sizeof(filename));

	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		return;
	}

	// 取文件长度
	::fseek(file, 0, SEEK_END);
	size_t length = ::ftell(file);
	::fseek(file, 0, SEEK_SET);

	char* buf = NEW char[length+1];
	if (length > 1 && buf != NULL)
	{
		// 全部加载到内存
		fread((void*)buf, 1, length, file);

		buf[length] = 0;

		const char* src = buf;
		const char* num = NULL;
		const char* t	= NULL;
		int c;
		LoopBeginCheck(c);
		for (size_t i = 0; i < length-1; ++i)
		{
			LoopDoCheck(c);

			c = buf[i];
			if ( c == 0)
			{
				continue;
			}

			if (c == '\t')
			{
				if (num == NULL)
				{
					num = &buf[i+1];
				}
				else
				{
					t = &buf[i+1];
				}
				
				buf[i] = NULL;
				continue;
			}
			if (c == 0x0d && buf[i+1] == 0x0a/* c == '\n'*/)
			{
				buf[i] = 0;
				buf[++i] = 0;

				if (src != NULL && num != NULL && t != NULL)
				{
					degug_invoke_lua_t& arg = g_script_times[src];
					arg.count += atoi(num);
					arg.use_time += _atoi64(t);
				}

				src = NULL;
				num = NULL;
				t   = NULL;

				continue;
			}

			if (src == NULL)
			{
				src = &buf[i];
			}
		}

		delete[] buf;
	}

	fclose(file);
}

#endif