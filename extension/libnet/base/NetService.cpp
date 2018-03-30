#include "NetService.h"
#include "../common/app/FsAppHelper.h"

void NetService::Close(const NID& id)
{
	this->Post([=]()->void {
		NetConnection* pConn = Lookup(id);
		if (NULL != pConn)
		{
			pConn->Close(EM_NET_REASON_SELF_DISCONNECT);
		}
		this->RemoveConnection(id, EM_NET_REASON_PEER_DISCONNECT, 0);
	});
}

bool NetService::Send(const NID& id, const char* szMsg, size_t nLen)
{
	// 必须复制  防止不在同一线程
	std::shared_ptr<DataBuffer> db(new DataBuffer(nLen));
	db->Write(szMsg, nLen);
	this->Post([this, id, db]()->void {
		NetConnection* pConn = Lookup(id);
		if (NULL != pConn && pConn->IsConnected())
		{
			if (!pConn->Send(db->GetBuffer(), db->GetWritePos()))
			{
				pConn->Close(EM_NET_REASON_SND_BUFFER_NO_ENGTH);
			}
		}
	});

	return true;
}

bool NetService::Send(const NID& id, std::shared_ptr<INetAction>& action)
{
	this->Post([this, id, action]()->void{
		NetConnection* pConn = Lookup(id);
		if (NULL != pConn && pConn->IsConnected() )
		{
			FsStream stream;
			INetActionPacker* packer = GetPacker();
			std::shared_ptr<INetAction> p = action;
			if (NULL != packer && packer->Pack(pConn, stream, p) == EM_PACK_SUCCESS)
			{
				if (!pConn->Send(stream.GetData(), stream.GetWritePos()))
				{
					pConn->Close(EM_NET_REASON_SND_BUFFER_NO_ENGTH);
				}
			}
		}
	});
	return true;
}

bool NetService::Startup()
{
	if (IsRunning())
		return false;

	if (NULL != m_pThread)
	{
		delete m_pThread;
		m_pThread = NULL;
	}

	m_pThread = new CThread( NetService::OnThreadWorker, NULL, NULL, this, 0, 0 );
	return m_pThread->Start();
}

bool NetService::Stop( int nWaitMillsec )
{
	this->Post([=]()->void{
		this->CloseAllConnection();
	});

	if (NULL != m_pThread)
	{
		m_pThread->Stop();
		delete m_pThread;
		m_pThread = NULL;
	}
	return true;
}

void NetService::NewConntion(NETFD fd, const std::string& ip, unsigned short port, int key, void* context, bool isAccept)
{
	if (isAccept)
	{
		if (NULL != m_pFilter)
		{
			if (!m_pFilter->AllowConnect(ip.c_str(), port))
			{
				this->Post([=]()->void{
					this->InnerDeneyConnection(fd, ip, port, key, context);
				});
				return;
			}
		}
	}

	NetConnection* pConn = this->InnerNewConnection(fd, ip, port, key, context, isAccept);
	if (NULL != pConn)
	{
		pConn->SetState(EM_CONN_STATE_CONNECTING);
		if (isAccept)
		{
			pConn->OnConnected();
		}
	}
}

void NetService::RemoveConnection( const NID& nid, EMNetReason reason, int nErrCode )
{
	// 延迟删除
	this->Post([=]()->void{
		NetConnection* pNetConn = this->InnerRemoveConnection( nid );
		if (NULL != pNetConn)
		{
			pNetConn->OnClosed(reason, nErrCode);
			pNetConn->SetService(NULL);
			pNetConn->SetConnId(NULL_NID);
			m_pFactory->FreeConnection(pNetConn);
		}
	});
}

NetConnection* NetService::InnerNewConnection(NETFD fd, const std::string& ip, unsigned short port, 
							int key, void* context, bool isAccept)
{
	size_t nSndBuf = this->m_pIoDesc->GetSendBufSize();
	size_t nRecvBuf = this->m_pIoDesc->GetReadBufSize();

	assert(NULL != m_pFactory);
	if (NULL == m_pFactory)
	{
		return NULL;
	}

	INetConnection* pConn = m_pFactory->CreateConnection(nSndBuf, nRecvBuf, isAccept);
	assert(NULL != pConn);
	if (NULL == pConn) return NULL;

	NetConnection* pNetConn = dynamic_cast<NetConnection*>(pConn);
	assert(NULL != pNetConn);
	if (NULL == pNetConn)
	{
		m_pFactory->FreeConnection(pConn);
		return NULL;
	}

	size_t nIndex = 0;
	{
		if (m_lstFreeIndex.size() > 0)
		{
			nIndex = m_lstFreeIndex.top();
			m_lstFreeIndex.pop();
			assert(m_vecConns[nIndex - 1] == NULL);
			m_vecConns[nIndex - 1] = pNetConn;
		}
		else
		{
			m_vecConns.push_back(pNetConn);
			nIndex = m_vecConns.size();
		}
	}

	++m_nCounter;
	NID id = nIndex;
	id = (id << 32) | m_nCounter;
	pNetConn->SetConnId(id)
		.SetService(this)
		.SetFD(fd)
		.SetRemotePort(port)
		.SetRemotIP(ip.c_str())
		.SetUD(key);;

	return pNetConn;
}

void NetService::InnerDeneyConnection(NETFD fd, const std::string& ip, unsigned short port, int key, void* context)
{
	
}

NetConnection* NetService::InnerRemoveConnection( const NID& nid )
{
	size_t nIndex = (size_t)(nid >> 32) - 1;

	if (nIndex < m_vecConns.size())
	{
		NetConnection* pConn = m_vecConns[nIndex];
		if (NULL != pConn && pConn->GetConnId() == nid)
		{
			m_vecConns[nIndex] = NULL;
			m_lstFreeIndex.push(nIndex + 1);
			return pConn;
		}
	}

	return NULL;
}

void NetService::CloseAllConnection()
{
	for (std::vector<NetConnection*>::iterator itr = m_vecConns.begin(); itr != m_vecConns.end(); ++itr)
	{
		NetConnection* conn = *itr;
		if (NULL != conn)
		{
			conn->Close(EM_NET_REASON_PEER_DISCONNECT);
		}
	}

	//m_vecConns.clear();
}

void NetService::Process()
{
	SAction p;
	while (m_queueActions.DeQueue(p))
	{
		p();
	}
}

void NetService::Run()
{
	while ( NULL != m_pThread && !m_pThread->GetQuit() )
	{
		Process();
		std::this_thread::yield();
	}

	// 最后再处理一次
	Process();
}

void NetService::RunOnce()
{
	Process();
}

void NetService::OutLog(int nLevel, const char* msg)
{
	printf("[%d]%s\n", nLevel, msg);
}

void NetService::Cleanup()
{
	for (std::vector<NetConnection*>::iterator itr = m_vecConns.begin(); itr != m_vecConns.end(); ++itr)
	{
		NetConnection* conn = *itr;
		if (NULL != conn)
		{
			conn->SetService(NULL);
			conn->SetConnId(NULL_NID);
			m_pFactory->FreeConnection(conn);
		}
	}

	m_vecConns.clear();
}

void NetService::OnThreadWorker( void* pContext )
{
	NetService* pService = (NetService*) pContext;
	FsFramework::app::EventThreadStart();

	if (NULL != pService)
	{
		pService->Run();
	}

	FsFramework::app::EventThreadStop();
}
