//--------------------------------------------------------------------
// 文件名:		ICrossKernel.h
// 内  容:		跨区服务器核心接口
// 说  明:		
// 创建日期:	2012年10月31日	
// 创建人:		刘刚
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ICROSSKERNEL_H
#define _SERVER_ICROSSKERNEL_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "ICrossData.h"

// 公共数据逻辑接口版本号
#define CROSS_LOGIC_MODULE_VERSION 0x1000

class ICrossKernel;

// 定时器函数
typedef int (__cdecl* CROSS_TIMER_FUNC)(ICrossKernel* pCrossKernel, 
										 const wchar_t* data_name, int time) ;
	
// 跨区服务核心接口

class ICrossKernel
{
public:
	virtual ~ICrossKernel() = 0;
	
	/// \brief 添加跨区数据项
	/// \param name 跨区数据项名
	virtual bool AddCrossData(const wchar_t* name, bool save) = 0;
	/// \brief 删除跨区数据项
	/// \param name 跨区数据项名
	virtual bool RemoveCrossData(const wchar_t* name) = 0;
	/// \brief 查找跨区数据
	/// \param 跨区数据名称
	virtual bool FindCrossData(const wchar_t* name) = 0;
	/// \brief 获得跨区数据数量
	virtual int GetCrossDataCount() = 0;
	/// \brief 获得跨区数据名称列表
	/// \param result 跨区数据名称列表
	virtual int GetCrossDataList(IVarList& result) = 0;
	/// \brief 获得指定名称的跨区数据项
	/// \param 跨区数据名
	virtual ICrossData* GetCrossData(const wchar_t* name) = 0;
	/// \brief 保存跨区数据项
	/// \param data_name 数据项名称
	virtual bool SaveCrossData(const wchar_t* name) = 0;

	/// \brief 发消息到游戏服务器
	/// \param server_id 服务器编号
	/// \param scene_id 场景编号，为0表示第一个场景
	/// \param msg 消息内容
	virtual bool SendCrossMessage(int server_id, int scene_id, 
		const IVarList& msg) = 0;

	/// \brief 查找定时器
	/// \param name 定时器名称
	/// \param data_name 关联数据项名称
	virtual bool FindTimer(const char* name, const wchar_t* data_name) = 0;
	/// \brief 添加定时器
	/// \param name 定时器名称
	/// \param func 定时器函数指针
	/// \param data_name 关联数据项名称
	/// \param time 定时时长
	/// \param count 定时次数
	virtual bool AddTimer(const char* name, CROSS_TIMER_FUNC func, 
		const wchar_t* data_name, int time, int count) = 0;
	/// \brief 删除定时器
	/// \param name 定时器名称
	/// \param data_name 关联数据项名称
	virtual bool RemoveTimer(const char* name, const wchar_t* data_name) = 0;

	/// \brief 保存游戏日志
	/// \param name 名字
	/// \param type 类型
	/// \param content 内容
	/// \param comment 备注
	virtual bool SaveLog(const wchar_t* name, int type, 
		const wchar_t* content, const wchar_t* comment) = 0;
	
	/// \brief 输出到系统LOG文件
	/// \param info 信息内容
	virtual void Trace(const char* info) = 0;
	/// \brief 输出到控制台屏幕
	/// \param info 信息内容
	virtual void Echo(const char* info) = 0;

};

inline ICrossKernel::~ICrossKernel() {}

#endif // _SERVER_ICROSSKERNEL_H

