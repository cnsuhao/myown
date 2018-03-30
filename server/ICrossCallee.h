//--------------------------------------------------------------------
// 文件名:		ICrossCallee.h
// 内  容:		跨区数据逻辑回调接口定义
// 说  明:		
// 创建日期:	2012年11月02日
// 创建人:		刘刚
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ICROSSCALLEE_H
#define _SERVER_ICROSSCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 跨区数据回调接口

class ICrossKernel;

class ICrossCallee
{
public:
	virtual ~ICrossCallee() = 0;

	/// \brief 跨区数据服务器启动
	/// \param pCrossKernel 核心指针，下同
	virtual int OnCrossStart(ICrossKernel* pCrossKernel) = 0;

	/// \brief 跨区数据项加载完成
	/// \param wsDataName 跨区数据项名
	virtual int OnCrossDataLoaded(ICrossKernel* pCrossKernel, const wchar_t* wsDataName) = 0;

	/// \brief 接收到场景服务器的消息
	/// \param nServerId 发消息的服务器id
	/// \param nSceneId 发消息的场景id
	/// \param msg 消息内容
	virtual int OnCrossMessage(ICrossKernel* pCrossKernel, int nServerId, 
		int nSceneId, const IVarList& msg) = 0;

	/// \brief 新增服务器
	/// \param nServerId 服务器标识
	/// \param wsDataName 服务器附加跨区数据名称
	/// \param msg 消息内容
	virtual int OnAddServer(ICrossKernel* pCrossKernel, int nServerId, 
		const wchar_t* wsDataName, const IVarList& msg) = 0;

	/// \brief 移除服务器
	/// \param nServerId 服务器标识
	/// \param wsDataName 服务器附加跨区数据名称
	/// \param msg 消息内容(服务的接入信息idc1:ip1:port1,idc2:ip2:port2)
	virtual int OnRemoveServer(ICrossKernel* pCrossKernel, int nServerId, 
		const wchar_t* wsDataName, const IVarList& msg) = 0;

	/// \brief 服务器每帧调用
	/// \param nFrameTicks 此帧的毫秒数
	virtual int OnFrameExecute(ICrossKernel* pCrossKernel, int nFrameTicks) = 0;

	/// \brief 服务器关闭通知
	virtual int OnServerClose(ICrossKernel* pCrossKernel) = 0;
};

inline ICrossCallee::~ICrossCallee() {}

#endif // _SERVER_ICROSSCALLEE_H

