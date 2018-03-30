//--------------------------------------------------------------------
// 文件名:		EntHttpServer.h
// 内  容:		http服务器实现
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __ENT_HTTP_SERVER_H__
#define __ENT_HTTP_SERVER_H__

#include "EntNetBase.h"
#include "NoticeInfo.h"
#include "ExchGiftInfo.h"
#include "PayedOrder.h"
#include "PushData.h"
#include "utils/json.h"
#include "cpptempl.h"
#include "action/IHttpAction.h"

class SoloGmcc;
class EntRpc;

struct config_info
{
	std::string name;			// 名称
	std::string key;			// 配置key
	int32_t		catalog;		// 配置分类
	int32_t		cfgid;			// 配置子id
	std::string path;			// 配置文件所在路径
	std::string tplpath;		// 模板所在路径
};

struct tpl_info
{
	cpptempl::auto_data data;
	const config_info* cfg;
	int64_t		file_version;
};

class EntHttp : public EntNetBase
{
public:
	EntHttp();

	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;

	virtual bool Startup() override;
	virtual bool Stop() override;
	virtual void Execute(float seconds);

	virtual int GetNetKey() const;

	// 推送相关参数
	const char* GetPushUrl() const;
	void SetPushUrl( const char* url );
	const char* GetPushKey() const;
	void SetPushKey(const char* key);
	const char* GetPushSecret() const;
	void SetPushSecret(const char* secret);
	const char* GetPushSound() const;
	void SetPushSound(const char* sound);
	int GetPushBuilderId() const;
	void SetPushBuilderId(int nBuilderId);
	int GetPushTimeLive() const;
	void SetPushTimeLive(int nValue);
	bool IsProduction() const;
	void SetProduction(bool bProduction);

	// 请求重试次数
	void SetReqResueNum(int nNum);
	int GetReqResueNum();
	// 请求重用数
	void SetReqRetries(int nNum);
	int GetReqRetries();

	// 默认请求头(启用服务后调用有效)
	void AddReqDefHeader(const char* pszKey, const char* pszValue);
	void RemoveReqDefHeader(const char* pszKey);
	void ClearReqDefHeaders();

protected:
	virtual INetIoDesc* GetIoDesc();

	virtual void OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	virtual void OnReply(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 初始化
	bool OnRspProfile(std::shared_ptr<INetAction>& action);
	// 通告
	bool OnRspNotice(std::shared_ptr<INetAction>& action);
	// 兑换码
	bool OnRspExchGift(std::shared_ptr<INetAction>& action);
	// 商品列表
	bool OnRspProducts(std::shared_ptr<INetAction>& action);
	// 下单
	bool OnRspPreOrder(std::shared_ptr<INetAction>& action);
	// 订单状态
	bool OnRspOrdeState(std::shared_ptr<INetAction>& action);
	// 充值返利
	bool OnRspRebateInfo(std::shared_ptr<INetAction>& action);
	// 充值返利结果
	bool OnRspRebateResult(std::shared_ptr<INetAction>& action);
	// 命令确认
	bool OnRspConfirmCmd(std::shared_ptr<INetAction>& action);
	// 推送
	bool OnRspPushNotification(std::shared_ptr<INetAction>& action);
	// 活动配置
	bool OnRspConfig(std::shared_ptr<INetAction>& action);
	// 商城配置
	bool OnRspMallItems(std::shared_ptr<INetAction>& action);
	// 邮件确认
	bool OnRspConfirmMail(std::shared_ptr<INetAction>& action);

private:
	//void OnSetShopItemList(Json::Value& root);
	void OnSetLevelingConfig(Json::Value& root);
public:
	// 初始化Http配置
	void OnReqProfile();
	// 兑换请求
	void OnReqExchGift( const ExchGiftInfo& info );
	// 请求指定的通知
	void OnReqNotify(const NotifyInfo& notify);
	// 请求所有有效的通知数据
	void OnReqAllData();
	// 请求所有商品列表
	void OnReqProducts();
	// 通知订单状态
	void OnNotifyOrder( const OrderNotify& notify );
	// 下单请求
	void OnReqPreOrder(const ProductOrder& order);
	// 发送通知
	void OnReqPushNotification(const PushData& data);
	// 请求活动配置
	void OnReqConfig( int nType );
	// 请求商成列表
	void OnReqMallItems();
	// 邮件确认
	void OnReqConfirmMail(int64_t nMailId, int32_t nResult, bool bIsGlobal, const std::map<std::string, int32_t>& resultMap);
	// gm结果确认
	void OnReqConfirmGm(int64_t nCmdId, int32_t nResult, const std::string& roleid, const std::string& message);
	// 查询充值返利
	void OnReqPayedRebate(const std::string& uid, const std::string& rid);
	// 领取充值返利
	void OnReqPayedRebatePick(const std::string& uid, const std::string& rid, const std::wstring& name );
protected:
	void RequestNotifyData( const NotifyInfo* pNotify );
	bool LoadConfig();
	// bAdaCataLog: nCfgId未找到时，只取类型信息
	const config_info* GetConfigData(int32_t nCata, int32_t nCfgId, bool bAdaCataLog = true) const;
	// 解析模板数据
	bool ParseConfigTplData(int32_t nCata, const Json::Value& root, std::map<int64_t, tpl_info>& tpldatas, int64_t version) const;
	// 解析json数据
	bool JsonToTplData(const Json::Value& root, cpptempl::auto_data& data) const;
	// 保存模板数据
	void HandleConfigTplData(const std::map<int64_t, tpl_info>& tpldatas) const;
protected:
	// url
	struct RequestInfo
	{
		std::string url;
		int			method;
	};

	virtual void OnInit();

	void FillPushData(Json::Value& root, const PushData& data) const;

	void DumpAction( std::shared_ptr<INetAction>& action );
	// 重发请求
	bool RepRequest( std::shared_ptr<INetAction>& action );

	bool ParseGoodsItem(const Json::Value& items, std::vector<GoodsItem>& goods);

	int HandleCommonRsp(IHttpRequest* req, IHttpResponse* res, Json::Reader& reader, Json::Value& root, Json::Value** ppdata, int& nHttpCode, const char* pszFuncName);
	bool ConfigRequestUrl();
	bool ParseRequest(RequestInfo& req, const char* pszUrl);
	IHttpRequest* InitRequest(const RequestInfo& req, std::shared_ptr<INetAction>& action, int nTag, int nTimeOut);
private:
	SoloGmcc*		m_pMainEntity;
	EntRpc*			m_pRpcService;

	// profile信息
	std::string m_strGameid;
	std::string m_strAreaId;

	RequestInfo m_urlProfile;				// 初始化
	RequestInfo m_urlNotice;				// 通知
	RequestInfo m_urlGift;					// 兑换码
	RequestInfo m_urlAcitvity;				// 日常活动
	RequestInfo m_urlMall;					// 商城
	RequestInfo m_urlMailConfirm;			// 邮件确认
	RequestInfo m_urlCmdConfirm;			// 命令确认
	RequestInfo m_urlGoods;					// 商品列表
	RequestInfo m_urlPreOrder;				// 下单请求
	RequestInfo m_urlOrderState;			// 订单状态修改
	RequestInfo m_urlRebateInfo;			// 返利信息
	RequestInfo m_urlRebatePick;			// 领取返利

	// 推送参数
	std::string m_strUrlPush;			// 推送地址
	std::string m_strPushKey;			// 推送服务分配的key
	std::string m_strPushSecret;		// 推送服务分配的密钥
	std::string m_strPushAuthString;	// 推送服务验证串(base64), 是appkey:secret，第一次发送产生
	std::string m_strDefaultSound;		// 默认声音
	int			m_nDefaultBuilderId;	// 默认显示样式

	int			m_nTimeToLive;			// 有效时间(0:使用默认的)
	bool		m_bEnvProduction;		// 生产环境(true为正式环境)

	// 缓存消息包
	struct ResendAction
	{
		std::shared_ptr<INetAction> action;
		float						elapsed;
	};
	std::map<INetAction*, ResendAction> m_cacheActions;
	// 配置模板映射信息
	std::map<int64_t, config_info>	m_mapConfigs;

	// 配置文件id与key映射，请求时转换使用
	std::map<int, std::string> m_mapConfigIds;
	int m_nReqReuseNum;
	int m_nReqRetries;
};

#endif // END __INT_HTTP_SERVER_H__