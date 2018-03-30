//--------------------------------------------------------------------
// 文件名:		FsPubLogic.h
// 内  容:		公共数据服务器逻辑模块
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __FsPubLogic_h__
#define __FsPubLogic_h__

#include "../server/IPubCallee.h"
#include "../server/PubLogicDll.h"
#include <vector>
#include "Public/IPubLogic.h"

// 公共数据回调接口

extern std::vector<IPubLogic*> g_vecLogic;

class pub_callee: public IPubCallee
{
public:
	//构造
	pub_callee();
	//析构
	~pub_callee();

	/// \brief 公共数据服务器启动
	/// \param pPubKernel 核心指针，下同
	virtual int OnPublicStart(IPubKernel* pPubKernel);

	/// \brief 公共数据项创建
	/// \param space_name 公共数据空间名
	/// \param data_name 公共数据项名,逻辑可以再此回调中统一添加属性，表。
	virtual int OnPubDataCreate(IPubKernel* pPubKernel, 
		const char* space_name, const wchar_t* data_name);

	/// \brief 公共数据项加载完成
	/// \param space_name 公共数据空间名
	/// \param data_name 公共数据项名
	virtual int OnPubDataLoaded(IPubKernel* pPubKernel, 
		const char* space_name,
		const wchar_t* data_name);

	/// \brief 公共数据空间中的所有项加载完成
	/// \param space_name 公共数据空间名
	virtual int OnPubSpaceLoaded(IPubKernel* pPubKernel, 
		const char* space_name);

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param msg 消息内容
	virtual int OnPublicMessage(IPubKernel* pPubKernel,
		int source_id, 
		int scene_id, 
		const IVarList& msg);

	// 服务器每帧调用
	virtual int OnFrameExecute(IPubKernel* pPubKernel,
		int frame_ticks); 

	// 接收到主服务器状态和玩家数量信息
	virtual int OnServerState(IPubKernel* pPubKernel,
		const char* world_state,
		int max_players,
		int player_count, 
		int online_count,
		int offline_count,
		int queue_count); 

	// 接收到可以加载到游戏的离线角色信息
	virtual int OnRoleOffline(IPubKernel* pPubKernel,
		int count,
		const IVarList& info); 

	/// \brief 服务器关闭通知
	virtual int OnServerClose(IPubKernel* pPubKernel);

	//主游戏服务器是否就绪
	bool IsServerReady(){
		return m_bServerReady;
	}
public:
	// for command
	bool SendPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& msg);
public:

	static pub_callee * pInstance;

private:
	// 处理通用消息
	bool ProcessCommonMessage(IPubKernel* pPubKernel,
		int source_id, 
		int scene_id, 
		const IVarList& msg);

	//修改属性，格式：string pubspace, wstring pubdata, int msgid, string prop, var value, ...;
	bool SetPubDataProp(IPubKernel* pPubKernel, 
		IPubData* pPubData, 
		int source_id,
		int scene_id, 
		const IVarList& msg);
	//增加属性，格式：string pubspace, wstring pubdata, int msgid, string prop, int value, ...;
	bool IncPubDataProp(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);

	//增加某个表，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int maxrow, int cols, ...;
	bool AddPubDataRec(IPubKernel* pPubKernel,
		IPubData* pPubData,
		int source_id,
		int scene_id,
		const IVarList& msg);

	//清除某个表的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	bool ClearPubDataRec(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id,
		int scene_id, 
		const IVarList& msg);
	//删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, ...;
	bool RemovePubDataRecRow(IPubKernel* pPubKernel, 
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);
	//添加某个表的一行内容，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
	bool AddPubDataRecRowValue(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);

	//设置某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	bool SetPubDataRecValue(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id,
		int scene_id, 
		const IVarList& msg);
	//增加某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
	bool IncPubDataRecValue(IPubKernel* pPubKernel, 
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);

	//根据关键值删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue;
	bool RemovePubDataRecRowByKey(IPubKernel* pPubKernel, 
		IPubData* pPubData, 
		int source_id,
		int scene_id, 
		const IVarList& msg);
	//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	bool SetPubDataRecValueByKey(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);
	//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	bool IncPubDataRecValueByKey(IPubKernel* pPubKernel,
		IPubData* pPubData, 
		int source_id, 
		int scene_id, 
		const IVarList& msg);
	

private:
	//更新配置，命令格式：string pubspace, wstring pubdata, int msgid
	bool UpdateConfig(IPubKernel* pPubKernel, 			 
		IPubData* pPubData,		 
		int source_id,	 
		int scene_id, 		 
		const IVarList& msg);
private:
	bool m_bServerReady;		// 主游戏服务器是否就绪
	int m_nMaxPlayers;			// 在线玩家服务器承载上限
	int m_nPlayerCount;			// 总玩家数量
	int m_nOnlineCount;			// 在线玩家数量
	int m_nOfflineCount;		// 离线玩家数量
	int m_nQueueCount;			// 正在排队的玩家数量
	float m_fGetOfflineTimer;	// 定时获取离线角色的计数器

};

//根据名称寻找分类公共数据逻辑模块（目前仅有："domain", "guild"）
inline IPubLogic * FindPubLogic(const char * pub_name)
{

    LoopBeginCheck(a);
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
        LoopDoCheck(a);

		if (strcmp(pub_name, g_vecLogic[i]->GetName()) == 0)
		{
			return g_vecLogic[i];
		}
	}

	return NULL;
}

#endif //__FsPubLogic_h__
