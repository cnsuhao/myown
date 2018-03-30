//--------------------------------------------------------------------
// 文件名:		SoloCharge.h
// 内  容:		计费登陆主实体配置
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __ENT_SOLO_CHARGE_H__
#define __ENT_SOLO_CHARGE_H__

#include "EntNetBase.h"
#include "NetWorldAction.h"
#include "MailData.h"
#include "NoticeInfo.h"
#include "CmdInfo.h"
#include "ExchGiftInfo.h"
#include "NetGmccMessagePacker.h"
#include "PayedOrder.h"
#include "PlayerData.h"
#include "ServerInfo.h"
#include "ShopItem.h"
#include "ProtocolsID.h"

class EntHttp;
class EntRpc;
struct ServerInfo;
class SoloGmcc : public EntNetBase
{
public:
	SoloGmcc();
	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;

	// 启动服务
	bool Startup();
	// 停止服务
	bool Stop();
	virtual void Execute(float seconds);
	virtual int GetNetKey() const;

	void SetGameName(const char* pszName);
	const char* GetGameName() const;

	void SetHeartbeat(int nHeartbeat);
	int GetHeartbeat();
protected:
	virtual void OnInit();
	virtual void OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	virtual void OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);

	// 注册
	void OnActionServerRegister(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 心跳
	void OnActionKeep(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 其它消息
	void OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 游戏服务器回应确认消息
	void OnGameAck(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 游戏服务器启动好了
	void OnGameReady(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 游戏请求服务器信息
	void OnGameReqSrvInfo(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 兑换礼品卡
	void OnExchGift(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 同步所有通知
	void OnSyncNotice(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 查询玩家属性数据返回
	void OnQueryPropData(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 查询玩家表数据返回
	void OnQueryRecordData(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 推送通知
	void OnPushNotification(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 商品信息返回
	void OnShopItemList(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 订单状态确认
	void OnOrderNotify(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 客户端下单
	void OnClientOrder(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 查询返利
	void OnGameQueryRebate(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 领取返利成功
	void OnGamePickedRebate(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 服务器上报人数
	void OnReportOnlineCount(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);


	//----------------------------------------------------------------------------------------
	// world server 回应消息
	//----------------------------------------------------------------------------------------
	// gm执行回应
	void OnGmResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 邮件结果
	void OnMailResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 通知结果
	void OnNoticeResult(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
public:
	//----------------------------------------------------------------------------------------
	// 对外功能接口
	//----------------------------------------------------------------------------------------
	// 初始化profile成功
	void OnProfileInited(const SrvInfo& info);
	/*@function:游戏GM(踢出玩家, 禁言.....)
	*/
	void SendGameCommand(const CmdInfo& cmd);
	/*@function:发送邮件
	*/
	void SendMail(const MailData& mail);
	/*@function:发送公告
	*/
	void SendNotice( const NoticeData& notice );
	// 删除通知
	void DeleteNotice(int64_t nNoticeId);
	/*@function:发送礼包
	*/
	void SendExchGiftItems(const ExchGiftResult& exchResult);

	// 购买结果通知
	void SendPayment(const Payment& payment);
	void SendProductOrder(const ProductOderInfo& order);
	void SendProducts(const std::vector<Product>& products);
	// 充值返利
	void SendPayedRebateInfo(const std::string rid, int nType, int nRebate);
	// 发送充值结果
	void SendPayedRebateResult(const std::string rid, int nResult, int nRebate);

	// 查询玩家属性
	bool QueryPlayerProps(const CmdPropQuery& query);
	// 查询表数据
	bool QueryPlayerRecordData(const CmdRecordQuery& query);

	//-------------------------------------------------
	// 商城信息
	//-------------------------------------------------
	bool ShopPushItem(const std::wstring& name, const std::wstring& desc, int64 startTime, int64 endTime, 
									const std::vector<ShopTag>& catalogs, const std::vector<ShopItem>& items);
	bool ShopRemoveItem(const std::vector<std::string>& removes);
	bool ShopQueryItem();

	// 设置活动配置
	void SetActivityConfig(ActivityInfo data);
	// 发送配置信息
	void SendLoadConfigInfo(const std::vector<ConfigLoadInfo>& cfgInfo);
protected:
	void OnInnerGameReady();
	void SendSrvInfo();
	void CheckGameReady();
protected:
//#define TEST
#ifdef TEST
	NetTextMessagePacker m_packer;
#else
	NetGmccMessagePacker m_packer;
#endif

	EntHttp* m_pHttpService;
	EntRpc* m_pRpcService;
	ServerInfo* m_pWorldSrv;

	time_t	m_tcheckTime;
	SrvInfo m_srvInfo;
	bool	m_bSelfReady;
	bool	m_bGameReady;

	std::string m_strGameName;
	int		m_nHeatbeat;		// 心跳检测(s)

	// 需要确认的消息
	struct PendingInfo
	{
		std::shared_ptr<INetAction> action;		// 发送中的action
		float elapsed;						// 流逝时间
	};

	std::map<uint64_t, PendingInfo> m_mapPendings;
	uint64_t m_nSeq;								// 计数器
};

#endif // END __ENT_SOLO_CHARGE_H__