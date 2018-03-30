//--------------------------------------------------------------------
// 文件名:		EntNetBase.h
// 内  容:		网络实体基类
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __ENT_NET_BASE_H__
#define __ENT_NET_BASE_H__

#include "public/IEntity.h"
#include "public/FastStr.h"
#include "base/NetService.h"
#include "action/INetAction.h"
#include <list>
#include "NetHandler.h"
#include "FsLogger.h"

class EntNetBase : public IEntity, public INetFilter, protected IActionBinder
{
public:
	EntNetBase();
	virtual ~EntNetBase();

	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;
	virtual void Execute(float seconds);

	// 启动服务
	virtual bool Startup();
	// 停止服务
	virtual bool Stop();

	void SetIP(const char* ip)
	{
		m_strIP = ip;
	};
	const char* GetIP() const
	{
		return m_strIP.c_str();
	}

	void SetListenPort(unsigned nPort)
	{
		m_nPort = nPort;
	}
	unsigned short GetListenPort() const
	{
		return m_nPort;
	}

	void SetAcceptSize( int nAcceptSize )
	{
		m_nMaxSize = nAcceptSize;
	}
	int GetAcceptSize() const
	{
		return m_nMaxSize;
	}

	void SetReadBufSize(int nReadBuf)
	{
		m_nReadBuf = nReadBuf;
	}
	int GetReadBufSize() const
	{
		return m_nReadBuf;
	}

	void SetSendBufSize(int nSendBuf)
	{
		m_nSendBuf = nSendBuf;
	}
	int GetSendBufSize() const
	{
		return m_nSendBuf;
	}

	void SetReplyKeep(bool bReply)
	{
		m_bReplyKeep = bReply;
	}

	bool GetReplyKeep() const
	{
		return m_bReplyKeep;
	}

	void SetLogLevel(int nLevel);
	int GetLogLevel() const;

	bool AnyIP() const
	{
		return m_bAnyIP;
	}

	void SetWhiteTables(const char* ips);
	bool IsWhiteIP(const char* ip) const;

	bool SetValue(const char* pszKey, const char* pszValue);
	const char* GetValue(const char* pszKey, const char* defValue);
	int GetIntValue(const char* pszKey, int defValue);
	float GetFloatValue(const char* pszKey, float defValue);

	void WriteMessage(const char* msg, int nLogLevel = EM_LOGGER_LEVEL_INFO) const;

	//--------------------------------------------------------------------
	// interface filter
	//--------------------------------------------------------------------
	virtual bool AllowNewConnect();
	virtual bool AllowConnect(const char* szRemoteIp, int nRemotePort);

protected:
	virtual INetIoDesc* GetIoDesc();
	virtual INetService* GetService() const
	{
		return m_pService;
	}
	virtual int GetNetKey() const = 0; 

public:
	//--------------------------------------------------------------------
	// interface IActionBinder
	//--------------------------------------------------------------------
	virtual bool BindAction(int nActionId, ACTION_EVENT action, int nPriority);
	virtual bool BindAction(const char* nActionKey, ACTION_EVENT action, int nPriority);

protected:
	virtual void OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	virtual void OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);

	virtual void OnUnRegisterAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	virtual void OnRawDataAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	virtual void OnErrAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);

	virtual void OnUnHandleAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);
	virtual void OnCompletedAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);

	virtual void OnEmptyAction(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction);

	virtual void OnInit();
	virtual void OnShut();

	virtual void DumpBinMessage(const char* pszDesc, BinaryMessage* bin);
protected:
	TFastStr<char, 128> m_strIP;
	TFastStr<char, 128> m_strAllowIP;
	unsigned short		m_nPort;
	int					m_nMaxSize;
	int					m_nReadBuf;
	int					m_nSendBuf;
	TFastStr<char, 128>	m_casIp;
	unsigned short		m_nCasPort;
	TFastStr<char, 128> m_casSevice;

	// 连接白名单
	std::list<std::string> m_strWhiteTables;
	// 任意ip连接
	bool	m_bAnyIP;
	// 回复keep信息
	bool	m_bReplyKeep;

	INetIoDesc*			m_pIoDesc;
	INetService*		m_pService;
	NetHandler*			m_pHandler;
	ActionHandleLinkManager*	m_pLinkManaer;
	INetActionPacker*		m_pActionPacker;
	INetConnectionFactory*	m_pNetConnFactory;

	std::map<std::string, std::string> m_mpaValues;
};

#endif // END __ENT_NET_BASE_H__