//--------------------------------------------------------------------
// 文件名:		GuildManager.h
// 内  容:		公会系统管理类
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:		 
//    :	    
//--------------------------------------------------------------------

#ifndef __PubGuildManager_h__
#define __PubGuildManager_h__

#include "../Public/IPubLogic.h"
#include "../../server/IKernel.h"
#include "../../system/WinPortable.h"
#include "../../FsGame/Define/GuildDefine.h"
class GuildBase;


class PubGuildManager : public IPubLogic
{
public:
	PubGuildManager();
	virtual ~PubGuildManager();
	static IPubLogic* Instance();
	// 公共数据服务器启动
	virtual int OnPublicStart(IPubKernel* pPubKernel);
	// 公共数据服务器关闭
	virtual int OnServerClose(IPubKernel* pPubKernel);
	// 公共数据空间中的所有项加载完成
	virtual int OnPubSpaceLoaded(IPubKernel* pPubKernel);
	// 公共数据项加载完成
	virtual int OnPubDataLoaded(IPubKernel* pPubKernel, const wchar_t* data_name);
	// 处理场景服务器的消息
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg);
	const char* GetName();
	// 重置计时器
	void ResetGuildTimer(IPubKernel* pPubKernel, const port_date_time_t& now);
	//回调所有子类
	bool CallBackAllSub(std::function<bool(GuildBase*)> func);

	//回调所有组织
	bool CallBackAllGuild(std::function<bool(IPubData*)>);

	// 帮会定时回调刷新
	void UpdateGuildTimer(IPubKernel* pPubKernel);

	// 获取帮会数据
	static IPubData* GetGuildPubData(IPubKernel* pPubKernel, const wchar_t* guildname);
private:
	
    static void ShopResetTimerCb(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);
   
	///////////////////////////////////////////////////////////////////////////
	// 处理场景服务器消息函数
	///////////////////////////////////////////////////////////////////////////
	// 请求购买公会商品，扣除个人贡献度
	int OnBuyShopItem(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	bool PubGuildManager::OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args);
	bool PubGuildManager::OnFilterMessage(IPubKernel* pPubKernel, const wchar_t* guildname, int msgid, int source_id, int scene_id, const IVarList& args);
	// 通知购买商品扣除个人贡献值结果
	void SendBuyItemResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int result, int index, const char* itemId, int itemNum, int consume);
	//增加子类管理
	void AddGuildSub(GuildBase* pBase);
	//初始化定时器
	bool InitTimer(IPubKernel* pPubKernel);
	static int GuildTimerCallback(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time);
	//组织信息重置
	static void GuildPubReset(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);
public:
	static PubGuildManager* m_pInstance;
	std::wstring m_ServerId;        // 服务器编号
	IPubSpace* m_pPubSpace;         // 公会公共数据区
	IPubData* m_pGuildPubData;      // 公会公共数据项
    GuildTimerCallbackVector  m_GuildTimer; // 帮会相关重置时间
	GuildBossLevelConfig m_guildBossLevelConfig;	//公会boss配置	
	std::vector<GuildBase*>  m_guildSubManage;
	
};

#endif //__PubGuildManager_h__