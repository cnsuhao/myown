//--------------------------------------------------------------------
// 文件名:		IPlatformCallee.h
// 内  容:		玩家实力匹配数据逻辑回调接口定义
// 说  明:		
// 创建日期:	2013年11月26日
// 创建人:		丁有进
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IPLATFORMCALLEE_H
#define _SERVER_IPLATFORMCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 玩家实力匹配数据回调接口

class IPlatformKernel;

class IPlatformCallee
{
public:
	virtual ~IPlatformCallee() = 0;

	/// \brief 平台数据服务器启动
	/// \param pPlatformKernel 核心指针，下同
	virtual int OnPlatformStart(IPlatformKernel* pPlatformKernel) = 0;

	/// \brief 创建玩家报名表
	/// \result 如果存在表 APPLY_TABLE_NAME, 不会被调用
	virtual bool OnCreateApplyTable(IPlatformKernel* pPlatformKernel) = 0;

	/// \brief 接收到场景服务器的报名消息
	/// \param role_uid 角色uid
	/// \param command 消息内容
	virtual int OnRequestApply(IPlatformKernel* pPlatformKernel, 
		const char* role_uid, const IVarList& command) = 0;

	/// \brief 接收到场景服务器的报名消息
	/// \param leader_uid 队长uid
	/// \param uid_list	 成员uid 列表
	/// \param command 消息内容
	virtual int OnRequestApplyGroups(IPlatformKernel* pPlatformKernel, 
		const char* leader_uid, const IVarList& uid_list, const IVarList& command) = 0;

	/// \brief 定时匹配（匹配逻辑在这个回调里面）
	/// \param msg 消息内容
	virtual int OnMatchingRole(IPlatformKernel* pPlatformKernel) = 0;

	/// \brief 房间custom 消息
	/// \param command 消息内容
	virtual int OnRoomCustom(IPlatformKernel* pPlatformKernel, const IVarList& command) = 0;

	/// \brief Member custom 消息
	/// \param command 消息内容
	virtual int OnMemberCustom(IPlatformKernel* pPlatformKernel, const IVarList& command) = 0;

	/// \brief 服务器关闭通知
	virtual int OnServerClose(IPlatformKernel* pPlatformKernel) = 0;

	/// \brief extra 服务器消息处理
	virtual int OnExtraMessage(IPlatformKernel* pPlatformKernel, int index , const IVarList& command) = 0;
};

inline IPlatformCallee::~IPlatformCallee() {}

#endif // _SERVER_IPlatformCALLEE_H

