#include "EntNetManager.h"
#include "SoloEcho.h"
#include <stdio.h>
#include "public/Module.h"
#include "FsLogger.h"

DECLARE_ENTITY("FxCharge", SoloEcho, EntNetBase)

DECLARE_METHOD_VOID_1(SoloEcho, SendEchoLine, const char*);
DECLARE_METHOD_VOID_0(SoloEcho, EndEcho);

bool SoloEcho::Init(const IVarList& args)
{
	FS_LOG_INFO("SoloEcho::Init(const IVarList& args)");
	m_pNetConnFactory = NetCreator::CreateConnectionFactory();
	m_pActionPacker = &m_packer;
	return EntNetBase::Init(args);
}

bool SoloEcho::Shut()
{
	FS_LOG_INFO("SoloEcho::Shut()");
	if (EntNetBase::Shut())
	{
		if (NULL != m_pNetConnFactory)
		{
			NetCreator::FreeConnectionFactory(m_pNetConnFactory);
			m_pNetConnFactory = NULL;
		}
		return true;
	}
	return false;
}

bool SoloEcho::Startup()
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

bool SoloEcho::Stop()
{
	m_lastID = 0;
	g_pCore->RemoveExecute(this);
	return EntNetBase::Stop();
}

void SoloEcho::Execute(float seconds)
{
	EntNetBase::Execute(seconds);
}

EMPackResult SoloEcho::Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen /*= NULL*/) const
{
	return m_packer.Pack(conn, stream, action, nOutLen);
}

EMPackResult SoloEcho::UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen /*= NULL*/) const
{
	return m_packer.UnPack(conn, stream, action, nOutLen);
}

void SoloEcho::SendEchoLine(const char* echo)
{
	if (NULL != m_pService && NULL != echo)
	{
		TextAction* pAction = new TextAction();
		NetTextMessage* pMsg = dynamic_cast<NetTextMessage*>( pAction->Request());
		pMsg->GetMutableBody() = echo;
		m_pService->Send( m_lastID, std::shared_ptr<INetAction>(pAction) );
	}
}

void SoloEcho::EndEcho()
{
	if (NULL != m_pService)
	{
		m_pService->Close(m_lastID);
	}
}

int SoloEcho::GetNetKey() const
{
	return EM_NET_KEY_ECHO;
}

void SoloEcho::OnInit()
{
	EntNetBase::OnInit();

	BindAction("TEXT", ACTION_BINDER(SoloEcho::OnCommand, this), 0);
}

void SoloEcho::OnCommand(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action)
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

void SoloEcho::OnConnection(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
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

void SoloEcho::OnClose(const SERID& serid, const SESSID& sessid, std::shared_ptr<INetAction>& pAction)
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
