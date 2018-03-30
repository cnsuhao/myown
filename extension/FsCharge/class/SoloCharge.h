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
#include "action\IHttpAction.h"
#include "ServerManager.h"
#include <set>
#define LOGIN_ERROR "cas:authenticationFailure"
#define LOGIN_SUCCESS "cas:authenticationSuccess"
#define LOGIN_ATTRIBUTES "cas:attributes"
#define LOGIN_WHITELISTLIMIT "cas:whiteListLimit"
#define LOGIN_SERVER_LIMIT "cas:openTimeLimit"
#define LOGIN_CAS_NAME "cas:name"

#define LOGIN_TIMER_OVER 300
#ifdef ANALY_BY_USER
class SensorsAnalyticsCollector;
typedef SensorsAnalyticsCollector GameAnalytics;
#else
class SensorsAnalyticsCollectorEx;
typedef SensorsAnalyticsCollectorEx GameAnalytics;
#endif

class EntHttpService;
class SoloCharge : public EntNetBase
{
public:
	SoloCharge();
	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;

	// 启动服务
	bool Startup();
	// 停止服务
	bool Stop();
	virtual void Execute(float seconds);

	virtual EMPackResult Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const;
	virtual EMPackResult UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const;

	virtual int GetNetKey() const;

public:
	unsigned int GetClientVersion() const { return m_nClientVersion;  }
	void SetClientVersion(unsigned int nVersion) { m_nClientVersion = nVersion; }

	const char* GetAppId() const { return m_strAppId.c_str(); }
	void SetAppId(const char* pszAppId) { if (NULL != pszAppId) m_strAppId = pszAppId; }

	bool GetVerify( ) const { return m_bEnableVerify; }
	void SetVerify(bool bEnable) { m_bEnableVerify = bEnable; }

	// 未完成账号登陆
	bool GetAccIncompleted() const { return m_bAccInCompleted; }
	void SetAccIncompleted(bool bEnable) { m_bAccInCompleted = bEnable; }

	// 设置登陆不验证的用户及有效次数
	void SetNoVerifyUser(const char* pszAcc, int nValidNum);
	void ClearNoVerifyUser();
	std::string DumpNoVerifyUser();
protected:
	virtual void OnInit();
	//virtual void OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	//virtual void OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);


	// 注册
	void OnActionServerRegister(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 登陆
	void OnActionLogin(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 退出
	void OnActionLogout(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 心跳
	void OnActionKeep(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 自定义消息
	void OnCustom(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	// 其它消息
	void OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	//删除超时请求
	void DelOverTimerReq();
private:
	void OnHandleGameReady(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action, NetWorldMessage_Custom* pReq, size_t nArgsOffset);
	//void RequestGameState();
	int CheckClientVersion(unsigned int nClientVersion) const;
public:
	void OnLoginResult(const char*name,int isResult,const char* verifyStr);
	

protected:
//#define TEST
#ifdef TEST
	NetTextMessagePacker m_packer;
#else
	NetWorldMessagePacker m_packer;
#endif
	struct LogicReq
	{
		SESSID							m_sessid;
		std::shared_ptr< INetAction >	m_action;
		time_t							m_timestamp;
	};


	// 登陆串缓存
	struct LoginStringInfo
	{
		std::string strLoginString;		// 登陆串信息
		time_t		tExpiredTime;		// 过期时间
	};

	EntHttpService* m_pHttpService;
	// 逻辑请求<请求串, 请求信息>
	typedef std::map<std::string, LogicReq> USERLOGICREQ;
	std::map<std::string,LogicReq>  m_userLogicReq;

	// 登陆中的玩家，限制重复请求
	std::set<std::string>		m_userPendding;	

	typedef std::map<std::string, LoginStringInfo> MapLoginStringCache;
	MapLoginStringCache m_logonStringChche;

	// 神策日志
	GameAnalytics* m_pAnaCollector;

	//ServerInfo* m_pWorldSrv;
	//time_t m_tcheckTime;
	bool			m_bGameReady;
	unsigned int	m_verifyKey;
	time_t			m_nThreshold;
	unsigned int	m_nClientVersion;		// 允许通过的最低客户端版本

	// 客户端连接web标识
	std::string		m_strAppId;

	// 不验证用户<用户名， 使用次数>
	std::map<std::string, int>	m_mapNoVeifyUsers;
	// 是否启用账号验证功能(默认为true, 为false时禁用验证功能，任何登陆都返回成功)
	bool			m_bEnableVerify;

	// 是否允许未完成的账号重复登陆
	bool			m_bAccInCompleted;
};

#endif // END __ENT_SOLO_CHARGE_H__