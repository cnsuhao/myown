//--------------------------------------------------------------------
// 文件名:		IPubCallee.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IPUBCALLEE_H
#define _SERVER_IPUBCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 公共数据回调接口

class IPubKernel;

class IPubCallee
{
public:
	virtual ~IPubCallee() = 0;

	/// \brief 公共数据服务器启动
	/// \param pPubKernel 核心指针，下同
	virtual int OnPublicStart(IPubKernel* pPubKernel) = 0;

#ifndef _DEBUG
	/// \brief 公共数据项创建
	/// \param space_name 公共数据空间名
	/// \param data_name 公共数据项名,逻辑可以再此回调中统一添加属性，表。
	virtual int OnPubDataCreate(IPubKernel* pPubKernel, 
		const char* space_name, const wchar_t* data_name) = 0;
#endif

	/// \brief 公共数据项加载完成
	/// \param space_name 公共数据空间名
	/// \param data_name 公共数据项名
	virtual int OnPubDataLoaded(IPubKernel* pPubKernel, 
		const char* space_name, const wchar_t* data_name) = 0;

	/// \brief 公共数据空间中的所有项加载完成
	/// \param space_name 公共数据空间名
	virtual int OnPubSpaceLoaded(IPubKernel* pPubKernel, 
		const char* space_name) = 0;

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param scene_id 发消息的场景标识
	/// \param msg 消息内容
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg) = 0;

	/// \brief 服务器每帧调用
	/// \param frame_ticks 此帧的毫秒数
	virtual int OnFrameExecute(IPubKernel* pPubKernel, int frame_ticks) = 0;

	/// \brief 接收到主服务器状态和玩家数量信息
	/// \param world_state 主服务器状态
	/// \param max_players 在线玩家上限
	/// \param player_count 玩家总数
	/// \param online_count 在线玩家数量
	/// \param offline_count 离线玩家数量
	/// \param queue_count 正在排队的玩家数量
	virtual int OnServerState(IPubKernel* pPubKernel,
		const char* world_state, int max_players, int player_count, 
		int online_count, int offline_count, int queue_count) = 0;

	/// \brief 接收到可以加载到游戏的离线角色信息
	/// \param count 角色数量
	/// \param info 角色信息序列（string account, wstring role_name, double resume_time, string resume_info）
	virtual int OnRoleOffline(IPubKernel* pPubKernel, int count,
		const IVarList& info) = 0;

	/// \brief 服务器关闭通知
	virtual int OnServerClose(IPubKernel* pPubKernel) = 0;
};

inline IPubCallee::~IPubCallee() {}

#endif // _SERVER_IPUBCALLEE_H

