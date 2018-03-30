#include "RpcServer.h"
#include "thrift/transport/TServerTransport.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/TServerSocket.h"
#include "thrift/concurrency/StdThreadFactory.h"
#include <boost/make_shared.hpp>
#include <assert.h>
#include <mutex>

using namespace apache::thrift;
using namespace apache::thrift::server;
using namespace apache::thrift::transport;
using namespace apache::thrift::concurrency;

RpcServer::RpcServer()
	: m_pServer( NULL )
	, m_bRunning( false )
{
	m_bExitThread.store(true);
}

void RpcServer::Start(const boost::shared_ptr<RpcHandler>& handler, int nPort)
{
	Stop(); 

	boost::shared_ptr<TProcessor> processor(new GMCCServerProcessor(handler));
	boost::shared_ptr<ThreadManager> threadManager(ThreadManager::newSimpleThreadManager());
	boost::shared_ptr<ThreadFactory> threadFactory( new StdThreadFactory(true) );
	threadManager->threadFactory(threadFactory);
	m_pServer = new TThreadPoolServer(processor,
									boost::make_shared<TServerSocket>(nPort),
									boost::make_shared<TBufferedTransportFactory>(),
									boost::make_shared<TBinaryProtocolFactory>(),
									threadManager );
	m_bRunning = true;
}

void RpcServer::Stop()
{
	if (NULL != m_pServer)
	{
		m_pServer->stop();
		while ( !m_bExitThread )
		{
			std::this_thread::yield();
		}
		delete m_pServer;
		m_pServer = NULL;
	}

	m_bRunning = false;
}

void RpcServer::Run()
{
	if (NULL != m_pServer)
	{
		m_bExitThread.store(false);

		boost::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager = m_pServer->getThreadManager();
		assert(threadManager.get());
		threadManager->start();
		m_pServer->serve();

		m_bExitThread.store(true);
	}
}

bool RpcServer::IsRunning()
{
	return m_bRunning;
}
