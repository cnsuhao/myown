//--------------------------------------------------------------------
// 文件名:		ICore.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_ICORE_H
#define _PUBLIC_ICORE_H

#include "Macros.h"
#include "PersistId.h"
#include "IVar.h"
#include "IVarList.h"
#include <stdarg.h>

// 核心接口版本号
#define FX_MODULE_CORE_VERSION 1003

// 核心接口

class IEntity;
class IInterface;
class IEntInfo;
class IFileSys;
class IIniFile;

class ICore
{
public:
	struct performance_t
	{
		double dTotalTime;
		double dWinMsgProcTime;
		double dAsyncProcTime;
		double dExecuteTime;
		double dDisplayTime;
		double dScriptTime;
	};
	
	typedef bool (*TRACE_FUNC)(void* context, const char* info);
	typedef double (*FRAME_TIME_FUNC)(double prev_frame_time, 
		double total_frame_time, double* elapse_time, 
		double* total_elapse_time);

public:
	virtual ~ICore() = 0;

	// 跨模块内存管理
	virtual void* MemoryAllocDebug(size_t size, const char* file, 
		int line) = 0;
	virtual void* MemoryAlloc(size_t size) = 0;
	virtual void MemoryFree(void* ptr, size_t size) = 0;
	
	// 获得程序实例句柄
	virtual void* GetInstance() = 0;
	// 获得窗口句柄
	virtual void* GetWindowHandle() = 0;
	// 获得工作路径
	virtual const char* GetWorkPath() = 0;
	// 获得脚本路径
	virtual const char* GetScriptPath() = 0;
	// 获得资源路径
	virtual const char* GetResourcePath() = 0;

	// 执行逻辑帧数
	virtual void SetExecuteFPS(int value) = 0;
	virtual int GetExecuteFPS() = 0;

	// 执行显示帧数
	virtual void SetDisplayFPS(int value) = 0;
	virtual int GetDisplayFPS() = 0;
	
	// 主循环休眠时间
	virtual void SetSleep(int value) = 0;
	virtual int GetSleep() = 0;

	// 控制程序退出
	virtual void SetQuit(bool value) = 0;
	virtual bool GetQuit() const = 0;
	
	// 处理窗口消息
	virtual bool ProcessWinMsg(void* hwnd, unsigned int msg, 
		unsigned int param1, unsigned int param2, int& result) = 0;
	// 添加不重复的窗口消息（有则替换）
	virtual bool PutSoleWinMsg(void* hwnd, unsigned int msg,
		unsigned int param1, unsigned int param2) = 0;
	// 主循环运行
	virtual bool Execute() = 0;
	
	// 保存日志到文件
	virtual bool TraceLog(const char* log) = 0;
	virtual bool TraceLogW(const wchar_t* log) = 0;
	// 重定向日志到指定函数
	virtual bool SetTraceFunc(TRACE_FUNC func, void* context) = 0;
	// 导出程序信息到文件
	virtual bool DumpFile(const char* type, const char* file) = 0;
	
	// 获得文件系统
	virtual IFileSys* GetFileSys() = 0;
	
	// 查找功能接口
	virtual bool FindInterface(const char* name) = 0;
	// 获得功能接口
	virtual IInterface* GetInterface(const char* name) = 0;
	// 获得同一名字空间中的功能接口
	virtual IInterface* GetInterfaceSameSpace(
		IInterface* pInterface, const char* name) = 0;
	// 释放功能接口
	virtual void ReleaseInterface(IInterface* pInterface) = 0;
	
	// 查找实体类信息
	virtual IEntInfo* GetEntInfo(const char* name) = 0;
	// 获得主实体
	virtual IEntity* GetMainEntity() = 0;
	// 获得实体
	virtual IEntity* GetEntity(const PERSISTID& id) = 0;
	// 获得所有的实体对象ID
	virtual size_t GetAllEntity(IVarList& result) = 0;
	// 查找名字符合的第一个实体
	virtual IEntity* LookupEntity(const char* name) = 0;
	// 查找名字符合的所有实体
	virtual size_t LookupEntityMore(const char* name, 
		IVarList& result) = 0;
	// 创建实体
	virtual IEntity* CreateEntity(const char* name) = 0;
	// 带参数创建实体
	virtual IEntity* CreateEntityArgs(const char* name, 
		const IVarList& args) = 0;
	// 创建同一名字空间中的实体
	virtual IEntity* CreateEntitySameSpace(IEntity* pEntity, 
		const char* name, const IVarList& args) = 0;
	// 删除实体
	virtual bool DeleteEntity(const PERSISTID& id) = 0;
	// 是否允许实体被脚本删除
	virtual void SetCanDelByScript(IEntity* pEntity, bool value) = 0;
	
	// 获得实体属性
	virtual bool GetProperty(IEntity* pEntity, 
		const char* prop, IVar& value) = 0;
	// 设置实体属性
	virtual bool SetProperty(IEntity* pEntity,
		const char* prop, const IVar& value) = 0;
	// 调用实体方法
	virtual bool InvokeMethod(IEntity* pEntity,
		const char* func, const IVarList& args, IVarList& res) = 0;
	
	// 运行脚本扩展函数
	virtual bool RunFunction(const char* func, 
		const IVarList& args, IVarList& res) = 0;
	
	// 同步运行脚本
	virtual bool RunScript(const char* script, const char* func,
		const IVarList& args, IVarList* res = NULL) = 0;
	// 运行异步进程
	virtual bool ExecAsyncProc(const char* script, const char* func,
		const IVarList& args, IVarList* res = NULL) = 0;
	// 查找异步进程
	virtual bool FindAsyncProc(const char* script, const char* func,
		const PERSISTID& id) = 0;
	// 终止异步进程
	virtual bool KillAsyncProc(const char* script, const char* func,
		const PERSISTID& id) = 0;
	// 产生异步事件，返回被触发的过程数量
	virtual size_t GenAsyncEvent(const PERSISTID& id, 
		const char* event, const IVarList& args) = 0;
	// 实体绑定脚本
	virtual bool BindScript(IEntity* pEntity, const char* script) = 0;
	// 实体绑定逻辑类
	virtual bool BindLogic(IEntity* pEntity, const char* logic,
		const IVarList& args) = 0;
	// 查找脚本回调
	virtual bool FindCallback(IEntity* pEntity, const char* event) = 0;
	// 执行脚本回调
	virtual bool ExecCallback(IEntity* pEntity, const char* event, 
		const IVarList& args, IVarList* res = NULL) = 0;
	// 重新加载所有脚本
	virtual bool ReloadAllScript() = 0;
	
	// 添加到运行队列
	virtual bool AddExecute(IEntity* pEntity) = 0;
	// 从运行队列移除
	virtual bool RemoveExecute(IEntity* pEntity) = 0;

	// 添加窗口消息处理（msg必须小于0x4000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProc(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// 添加立即处理窗口消息（msg必须小于0x4000，prior越大级别越高，先加入优先）
	virtual bool AddMsgProcInstant(IEntity* pEntity, unsigned int msg, 
		int prior = 0) = 0;
	// 移除窗口消息处理
	virtual bool RemoveMsgProc(IEntity* pEntity, unsigned int msg) = 0;

	// 全局变量
	virtual bool FindGlobalValue(const char* name) = 0;
	virtual bool RemoveGlobalValue(const char* name) = 0;
	virtual bool SetGlobalValue(const char* name, const IVar& value) = 0;
	virtual IVar& GetGlobalValue(const char* name) = 0;
	virtual size_t GetGlobalCount() = 0;
	virtual size_t GetGlobalList(IVarList& result) = 0;
	
	// 调试日志
	virtual bool LogWrite(int type, const char* format, va_list vlist) = 0;
	virtual void LogSetDropMode(bool value) = 0;
	virtual bool LogRegisterThread(int type, size_t start_addr, 
		size_t stack_size) = 0;
	
	// 设置单件实体
	virtual bool SetSingle(IEntity* pEntity) = 0;
	// 获得单件实体
	virtual IEntity* GetSingle(const char* entity_name) = 0;
	
	// 是否平滑每帧时间
	virtual void SetSmoothFrameTime(bool value) = 0;
	virtual bool GetSmoothFrameTime() const = 0;

	// 获得当前帧时间
	virtual double GetFrameSeconds() const = 0;
	// 获得当前内部计时总时间
	virtual double GetTotalSeconds() const = 0;
	
	// 设置是否记录文件访问日志
	virtual void SetLogFileIO(bool value) = 0;
	// 设置是否记录脚本计时日志
	virtual void SetLogScript(bool value) = 0;
	
	// 获得运行性能计数器
	virtual performance_t* GetPerformance() = 0;

	// 设置处理帧时间的函数
	virtual bool SetFrameTimeFunc(FRAME_TIME_FUNC func) = 0;

	// 是否零售正式版本
	virtual void SetRetailVersion(bool value) = 0;
	virtual bool GetRetailVersion() const = 0;
	
	// 创建配置文件读写接口
	virtual IIniFile* CreateIniFile(const char* filename, 
		int read_raw_file = 0) = 0;

	// 回收长时间未使用的脚本数据
	virtual int CollectUnuseScript(int seconds) = 0;
};

inline ICore::~ICore() {}

#endif // _PUBLIC_ICORE_H

