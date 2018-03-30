//--------------------------------------------------------------------
// 文件名:	SystemInfo_Common.h
// 内 容:	通用模块系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Common_H_
#define _SystemInfo_Common_H_


// CommonModule 通用系统(3001-5000)【子系统详细划分】	
enum SystemInfo_Common
{
#pragma region // ActionMutex功能互斥 3001-3100
	SYSTEM_INFO_ID_3001 = 3001,		// 任务分组中，无法进行其他操作，请退出后再尝试
#pragma endregion

#pragma region // AsynCtrlModule场景切换3101-3200	
	SYSTEM_INFO_ID_3101 = 3101,		// 场景冲突，无法传送,请退出当前场景，再进行传送
#pragma endregion

#pragma region	// GMModule GM模块3201-3300
	SYSTEM_INFO_ID_3201 = 3201,		// 执行GM命令失败!
	SYSTEM_INFO_ID_3202,			// 当前级别小于限制级别，不可以执行此命令
#pragma endregion

#pragma region	// ContainerModule 容器3301-3400
	SYSTEM_INFO_ID_3301 = 3301,		// 容器空间不足
	SYSTEM_INFO_ID_3302,			// 背包整理太频繁(整理冷却中){0:剩余冷却时间s}
#pragma endregion


};


#endif