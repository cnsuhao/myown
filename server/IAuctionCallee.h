//--------------------------------------------------------------------
// 文件名:		IAuctionCallee.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IAUCTIONCALLEE_H
#define _SERVER_IAUCTIONCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 拍卖行数据回调接口

class IAuctionKernel;

class IAuctionCallee
{
public:
	virtual ~IAuctionCallee() = 0;

	/// \brief 拍卖行数据服务器启动
	/// \param pAuctionKernel 核心指针，下同
	virtual int OnAuctionStart(IAuctionKernel* pAuctionKernel) = 0;

	/// \brief 拍卖行属性项加载完成
	virtual int OnAuctionAttrLoaded(IAuctionKernel* pAuctionKernel, const wchar_t * auction_data) = 0;

	/// \brief 拍卖行数据项加载完成
	virtual int OnAuctionDataLoaded(IAuctionKernel* pAuctionKernel) = 0;

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param scene_id 发消息的场景标识
	/// \param msg 消息内容
	virtual int OnAuctionMessage(IAuctionKernel* pAuctionKernel, int source_id, 
		int scene_id, const IVarList& msg) = 0;

	/// \brief 服务器每帧调用
	/// \param frame_ticks 此帧的毫秒数
	virtual int OnFrameExecute(IAuctionKernel* pAuctionKernel, int frame_ticks) = 0;

	/// \brief 接收到主服务器状态和玩家数量信息
	/// \param world_state 主服务器状态
	/// \param max_players 在线玩家上限
	/// \param player_count 玩家总数
	/// \param online_count 在线玩家数量
	/// \param offline_count 离线玩家数量
	/// \param queue_count 正在排队的玩家数量
	virtual int OnServerState(IAuctionKernel* pAuctionKernel,
		const char* world_state, int max_players, int player_count, 
		int online_count, int offline_count, int queue_count) = 0;

	/// \brief 接收到可以加载到游戏的离线角色信息
	/// \param count 角色数量
	/// \param info 角色信息序列（string account, wstring role_name, double resume_time, string resume_info）
	virtual int OnRoleOffline(IAuctionKernel* pAuctionKernel, int count,
		const IVarList& info) = 0;

	/// \brief 服务器关闭通知
	virtual int OnServerClose(IAuctionKernel* pAuctionKernel) = 0;
};

inline IAuctionCallee::~IAuctionCallee() {}

#endif // _SERVER_IAUCTIONCALLEE_H

