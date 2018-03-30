#include "NetCreator.h"
#include "libevent/LibEventService.h"
#include "libevent/LibEventHttpService.h"
#include "libevent/LibEventHttpConnectionFactory.h"
#include "libevent/LibEventConnectionFactory.h"

INetService* NetCreator::Create(INetIoDesc* pDesc, bool bHttp /* = false */)
{
	INetService* pService = NULL;

	if (bHttp)
	{
		pService = new LibEventHttpService(pDesc);
	}
	else
	{
		pService = new LibEventService(pDesc);
	}

	return pService;
}

void NetCreator::Free(INetService* service)
{
	delete service;
}

INetConnectionFactory* NetCreator::CreateConnectionFactory(bool bHttp /*= false*/)
{
	if (bHttp)
	{
		return new LibEventHttpConnectionFactory();
	}

	return new LibEventConnectionFactory();
}

void NetCreator::FreeConnectionFactory(INetConnectionFactory* factory)
{
	delete factory;
}
