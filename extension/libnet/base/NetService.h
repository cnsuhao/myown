//--------------------------------------------------------------------
// 文件名:		NetService.h
// 内  容:		网络服务接口
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_SERVICE_H__
#define __NET_SERVICE_H__

#include "INetService.h"
#include "NetConnection.h"
#include "public/FastStr.h"
#include <vector>
#include <stack>
#include <assert.h>
#ifdef WIN32
#include <WinSock2.h>
#endif
#include "utils/LockUtil.h"
#include "utils/FsLockedQueue.h"
#include <memory>
#include "../common/public/Thread.h"

class NetIoDesc : public INetIoDesc
{
private:
	int m_nKey;
	size_t m_nReadBuf;
	size_t m_nSendBuf;
	size_t m_nMaxAccept;

	TFastStr<char, 32> m_ip;
	unsigned short m_nPort;
	bool m_bSSL;
public:
	NetIoDesc()
		: m_nKey(0)
		, m_nReadBuf(1024)
		, m_nSendBuf(1024)
		, m_nMaxAccept(1024)
		, m_nPort(0)
		, m_bSSL( false )
	{
	}

	virtual int GetIoKey() const
	{
		return m_nKey;
	}

	virtual size_t GetReadBufSize() const
	{
		return m_nReadBuf;
	}
	virtual size_t GetSendBufSize() const
	{
		return m_nSendBuf;
	}
	virtual size_t GetMaxAccept() const
	{
		return m_nMaxAccept;
	}


	virtual const char*		ListenIP() const
	{
		return m_ip.c_str();
	}
	virtual unsigned short	ListenPort() const
	{
		return m_nPort;
	}

	NetIoDesc& SetKey(int nKey)
	{
		m_nKey = nKey;
		return *this;
	}

	NetIoDesc& SetReadBufSize(size_t nReadBuf)
	{
		m_nReadBuf = nReadBuf;
		return *this;
	}


	NetIoDesc& SetSendBufSize(size_t nSendBuf)
	{
		m_nSendBuf = nSendBuf;
		return *this;
	}

	NetIoDesc& SetMaxAccept(size_t nMaxAccept)
	{
		m_nMaxAccept = nMaxAccept;
		return *this;
	}

	NetIoDesc& SetIP( const char* ip)
	{
		m_ip = ip;
		return *this;
	}

	NetIoDesc& SetListenPort(  unsigned short nPort )
	{
		m_nPort = nPort;
		return *this;
	}

	bool IsSSL() const
	{
		return m_bSSL;
	}

	void SetSSL(bool bSSL)
	{
		m_bSSL = true;
	}
};

class HttpIoDesc : public NetIoDesc
{
private:
	int m_nReqReuseNum;			// 请求连接重用数
	int m_nReqRetries;			// 请求重试次数

	// 请求添加的默认头
	std::map<std::string, std::string> m_mapHeaderDef;
public:
	HttpIoDesc() : NetIoDesc()
		, m_nReqReuseNum(1)
		, m_nReqRetries(0)
	{

	}

	HttpIoDesc& SetReqReuseNum(int nNum)
	{
		m_nReqReuseNum = nNum;
		return *this;
	}

	int GetReqReuseNum() const
	{
		return m_nReqReuseNum;
	}

	void SetReqDefHeader(const char* pHeaderKey, const char* pValue)
	{
		if (NULL != pHeaderKey)
		{
			if (NULL == pValue)
			{
				RemoveReqDefHeader(pHeaderKey);
			}
			else
			{
				m_mapHeaderDef[pHeaderKey] = pValue;
			}
		}
	}

	void RemoveReqDefHeader(const char* pHeaderKey)
	{
		if (NULL == pHeaderKey)
		{
			m_mapHeaderDef.erase(pHeaderKey);
		}
	}
	void ClearReqDefHeaers()
	{
		m_mapHeaderDef.clear();
	}

	const std::map<std::string, std::string>& GetDefHeaders() const
	{
		return m_mapHeaderDef;
	}

	int GetReqRetires() const
	{
		return m_nReqRetries;
	}
	HttpIoDesc& SetReqRetries(int nRetries)
	{
		m_nReqRetries = nRetries;
		return *this;
	}
};

class NetService : public INetService
{
protected:
	INetIoDesc* m_pIoDesc;
	INetFilter* m_pFilter;
	INetConnectionFactory* m_pFactory;
	INetIoHandler* m_pHandler;
	INetActionPacker* m_pPacker;

	// **** 不加锁， 保证连接操作只在同一线程
	std::vector<NetConnection*> m_vecConns;
	std::stack<size_t>	m_lstFreeIndex;

	typedef std::function<void()> SAction;
	FsLockedQueue<SAction>	m_queueActions;

	int			  m_nCounter;
	CThread*	  m_pThread;
public:
	NetService(INetIoDesc* desc)
		: m_pIoDesc(desc)
		, m_pFilter(NULL)
		, m_pFactory(NULL)
		, m_pPacker( NULL )
		, m_pHandler( NULL )
		, m_nCounter( 0 )
		, m_pThread(NULL)
	{

	}

	virtual ~NetService(){	};

	virtual NetService& SetHandler(INetIoHandler* handler)
	{
		m_pHandler = handler;
		return *this;
	}
	virtual NetService& SetFilter(INetFilter* filter)
	{
		m_pFilter = filter;
		return *this;
	}
	virtual NetService& SetFactory(INetConnectionFactory* factory)
	{
		m_pFactory = factory;
		return *this;
	}
	virtual NetService& SetPacker(INetActionPacker* packer)
	{
		m_pPacker = packer;
		return *this;
	}

	virtual int GetKey() const
	{
		return NULL != m_pIoDesc ? m_pIoDesc->GetIoKey() : 0;
	}

	virtual const INetIoDesc* GetDesc() const
	{
		return m_pIoDesc;
	}
	virtual INetFilter* GetFilter()
	{
		return m_pFilter;
	}
	virtual INetActionPacker* GetPacker()
	{
		return m_pPacker;
	}
	virtual INetConnectionFactory* GetFactory()
	{
		return m_pFactory;
	}
	virtual INetIoHandler* GetHandler()
	{
		return m_pHandler;
	}

	virtual bool IsExists(const NID& id) const
	{
		return Lookup(id) != NULL;
	}

	virtual size_t GetConnCount() const
	{
		return m_vecConns.size() - m_lstFreeIndex.size();
	}
	virtual void Close(const NID& id);
	virtual bool Connect(const char* ip, unsigned short port, int nTimout, int ud)
	{
		return false;
	}
	virtual bool Send(const NID& id, const char* szMsg, size_t nLen);
	virtual bool Send(const NID& id, std::shared_ptr<INetAction>& action);

	virtual bool Startup();
	virtual bool Stop( int nWaitMillsec );
	virtual bool IsRunning() const
	{
		return NULL != m_pThread && !m_pThread->GetQuit();
	}

	virtual void NewConntion(NETFD fd, const std::string& ip, unsigned short port, int key, void* context, bool isAccept);
	virtual void RemoveConnection( const NID& nid, EMNetReason reason, int nErrCode );
	virtual void Process();
	virtual void Post(std::function<void() > p)
	{
		m_queueActions.EnQueue( p );
	}

	virtual void Run();
	virtual void RunOnce();

	virtual void OutLog(int nLevel, const char* msg);
protected:
	virtual NetConnection* Lookup(const NID& id) const
	{
		size_t nIndex = (size_t)(id >> 32) - 1;
		if (nIndex < m_vecConns.size())
		{
			if (NULL == m_vecConns[nIndex] || m_vecConns[nIndex]->GetConnId() == id)
			{
				return m_vecConns[nIndex];
			}
		}

		return NULL;
	}

	virtual NetConnection* InnerNewConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context, bool isAccept);
	virtual void InnerDeneyConnection( NETFD fd, const std::string& ip, unsigned short port, int key, void* context );
	virtual NetConnection* InnerRemoveConnection(const NID& nid );
	virtual void CloseAllConnection();

	virtual void Cleanup();

private:
	static void __cdecl OnThreadWorker(void* pContext);
};

#endif // END __NET_SERVICE_H__