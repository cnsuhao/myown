//--------------------------------------------------------------------
// 文件名:      ResetTimerModule.h
// 内  容:      时间重置模块
// 说  明:
// 创建日期:    2016年03月28日
// 创建人:        tongzt
// 修改人:         
//--------------------------------------------------------------------

#ifndef _RESET_TIMER_MODULE_H_
#define _RESET_TIMER_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ResetTimerDefine.h"


/* ----------------------
 * 定时刷新类
 * ----------------------*/
class ResetTimerModule : public ILogicModule
{
public:
    virtual bool Init(IKernel *pKernel);
    virtual bool Shut(IKernel *pKernel);

    bool LoadResource(IKernel *pKernel);

public:
    // 注册时不提供注册刷新
    static bool Register(IKernel *pKernel, const PERSISTID &self, 
		int type);

    // 根据类型取消定时刷新
    static bool UnRegister(IKernel *pKernel, const PERSISTID &self, 
		int type);

	// 注册函数指针
	static void InitFunc(const ResetTimerTypes fun_type, ResetTimer func );

	// 查询时间节点之间刷新次数
	static int GetResetTimes(IKernel *pKernel, const PERSISTID &self, 
		int nType, const double startTime, const double endTime);

	// 可否周刷新
	static bool CanWeekReset(
		const ResetData *data, 
		const int cur_hour, 
		const double cur_date, 
		const int cur_weekday, 
		const int last_hour, 
		const double last_date, 
		const int last_weekday);

	// 可否日刷新
	static bool CanDayReset(
		const ResetData *data, 
		const int cur_hour, 
		const double cur_date, 
		const int last_hour, 
		const double last_date);

private:
    // 玩家进入场景
    static int OnEntry(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

    // 玩家准备就绪
    static int OnReady(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

    // 玩家恢复数据
    static int OnRecover(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

    // 首次刷新心跳
    static int HB_ResetFirst(IKernel *pKernel, const PERSISTID &self, 
		int slice);

    // 定时刷新心跳
    static int HB_ResetTimer(IKernel *pKernel, const PERSISTID &self, 
		int slice);
    
	static void ReloadConfig(IKernel* pKernel);
public:
    static ResetFuncMap         m_funcMap;
    static ResetdDataMap        m_ResetDataMap;
    static ResetTimerModule*    m_resetTimerModule;
};

/*!
 * @brief 定义定时器
 * @param fun_type 功能类型
 * @param func 功能函数
 */
inline void DECL_RESET_TIMER(const ResetTimerTypes fun_type, ResetTimer func)
{
	ResetTimerModule::InitFunc(fun_type, func);
}

/*!
 * @brief 注册定时器
 * @pKernel 引擎指针
 * @self 操作对象
 * @param fun_type 功能类型
 */
inline void REGIST_RESET_TIMER(IKernel *pKernel, const PERSISTID &self, int fun_type)
{
	ResetTimerModule::Register(pKernel, self, fun_type);
}

#endif // _RESET_TIMER_MODULE_H_
