#include "EntNetManager.h"
#include "EntEcho.h"
#include <stdio.h>
#include "public/Module.h"
#include "FsLogger.h"

DECLARE_ENTITY("FsGmcc", EntEcho, EntNetBase)

DECLARE_METHOD_VOID_1(EntEcho, SendEchoLine, const char*);
DECLARE_METHOD_VOID_0(EntEcho, EndEcho);

bool EntEcho::Init(const IVarList& args)
{
	FS_LOG_INFO("SoloEcho::Init(const IVarList& args)");
	m_pNetConnFactory = NetCreator::CreateConnectionFactory();
	m_pActionPacker = &m_packer;
	return EntNetBase::Init(args);
}

bool EntEcho::Shut()
{
	FS_LOG_INFO("SoloEcho::Shut()");
	if (EntNetBase::Shut())
	{
		if (NULL != m_pNetConnFactory)
		{
			NetCreator::FreeConnectionFactory(m_pNetConnFactory);
			m_pNetConnFactory = NULL;
		}
		m_pActionPacker = NULL;
		return true;
	}
	return false;
}

bool EntEcho::Startup()
{
	m_lastID = 0;
	if (EntNetBase::Startup())
	{
		if (!m_pService->Listen())
		{
			FS_LOG_ERROR("listen server failed");
			return false;
		}
		g_pCore->AddExecute(this);
		return true;
	}

	FS_LOG_FATAL("start server fatal");
	return false;
}

bool EntEcho::Stop()
{
	m_lastID = 0;
	return EntNetBase::Stop();
}

void EntEcho::Execute(float seconds)
{
	EntNetBase::Execute(seconds);
}

void EntEcho::SendEchoLine(const char* echo)
{
	if (NULL != m_pService && NULL != echo)
	{
		TextAction* pAction = new TextAction();
		NetTextMessage* pMsg = dynamic_cast<NetTextMessage*>( pAction->Request());
		pMsg->GetMutableBody() = echo;
		m_pService->Send( m_lastID, std::shared_ptr<INetAction>(pAction) );
	}
}

void EntEcho::EndEcho()
{
	if (NULL != m_pService )
	{
		m_pService->Close(m_lastID);
	}
}

int EntEcho::GetNetKey() const
{
	return EM_NET_KEY_ECHO;
}

void EntEcho::OnInit()
{
	EntNetBase::OnInit();

	BindAction("TEXT", ACTION_BINDER(EntEcho::OnCommand, this), 0);
}

void EntEcho::OnCommand(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
{
	TextAction* txtAction = dynamic_cast<TextAction*>(action.get());
	if (NULL == txtAction)
	{
		FS_LOG_WARN("recv empty action");
		return;
	}

	NetTextMessage* txtMessage = dynamic_cast<NetTextMessage*>(txtAction->Request());
	if (NULL == txtMessage)
	{
		FS_LOG_WARN("recv empty action");
		return;
	}
	 
	m_lastID = sid;
	CVarList res;
	res.AddString(txtMessage->GetMsgBody());
	GetCore()->ExecCallback( this, "on_input", res );
}

void EntEcho::OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	EntNetBase::OnConnection(serid, sessid, pAction);

	SYSActionEnter* pEnter = dynamic_cast<SYSActionEnter*>(pAction.get());
	if ( NULL != pEnter )
	{
		m_lastID = sessid;
		CVarList res;
		res.AddString(pEnter->GetPeerHost() ? pEnter->GetPeerHost() : "" );
		res.AddInt(pEnter->GetPeerPort());
		GetCore()->ExecCallback(this, "on_open", res);
	}
}

void EntEcho::OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
{
	EntNetBase::OnClose(serid, sessid, pAction);

	SYSActionExit* pExit = dynamic_cast<SYSActionExit*>(pAction.get());
	if (NULL != pExit)
	{
		m_lastID = sessid;

		CVarList res;
		res.AddString(pExit->GetPeerHost() ? pExit->GetPeerHost() : "");
		res.AddInt(pExit->GetPeerPort());
		GetCore()->ExecCallback(this, "on_close", res);
	}
}
