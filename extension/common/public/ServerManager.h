//--------------------------------------------------------------------
// 文件名:		ServerManager.h
// 内  容:		服务管理信息
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
//--------------------------------------------------------------------
#ifndef __OUTER_SERVER_MANAGER_H__
#define __OUTER_SERVER_MANAGER_H__
#include "base/INetType.h"
#include "utils/FsStream.h"
#include <map>

struct ServerInfo
{
	int64	ID;				// 索相标识
	int		ServerType;		// 服务器类型
	int		ServerId;		// 服务器id
	SESSID	ConnId;			// 连接id
};

class ServerManager
{
private:
	ServerManager(){};
	ServerManager(const ServerManager& src) = delete;
	ServerManager( ServerManager&& src) = delete;
	ServerManager& operator =(const ServerManager& src) = delete;
	ServerManager& operator =( ServerManager&& src) = delete;

private: 
	std::map<SESSID, ServerInfo*>		m_mapCSInfo;		// 连接管理
	std::map<int64, ServerInfo*>	m_mapISInfo;		// 索引id关理

public:
	static ServerManager* GetInstance()
	{
		static ServerManager s_Instance;
		return &s_Instance;
	}

	~ServerManager()
	{
		RemoveAll();
	}

	int64 PushServer( const ServerInfo& srv )
	{
		assert(srv.ConnId != 0);
		int64 id = ((int64)srv.ServerType << 32) | srv.ServerId;
		ServerInfo* pInfo = m_mapISInfo[id];
		if ( NULL == pInfo )
		{
			pInfo = new ServerInfo;
			*pInfo = srv;
			m_mapISInfo[id] = pInfo;
			m_mapCSInfo[pInfo->ConnId] = pInfo;
		}
		else
		{
			assert(false);
		}
		return id;
	}

	void RemoveByID(int64 id)
	{
		std::map<int64, ServerInfo*>::iterator itr = m_mapISInfo.find(id);
		if (itr != m_mapISInfo.end())
		{
			ServerInfo* pInfo = itr->second;
			m_mapCSInfo.erase(pInfo->ConnId);
			m_mapISInfo.erase(itr);
			delete pInfo;
		}
	}

	void RemoveByNID( const SESSID& conid )
	{
		std::map<SESSID, ServerInfo*>::iterator itr = m_mapCSInfo.find(conid);
		if (itr != m_mapCSInfo.end())
		{
			ServerInfo* pInfo = itr->second;
			m_mapISInfo.erase(pInfo->ID);
			m_mapCSInfo.erase(itr);
			delete pInfo;
		}
	}

	ServerInfo* FindByID(int64 id)
	{
		std::map<int64, ServerInfo*>::iterator itr = m_mapISInfo.find(id);
		if (itr != m_mapISInfo.end())
		{
			ServerInfo* pInfo = itr->second;
			return pInfo;
		}

		return NULL;
	}

	ServerInfo* FindByNID(const SESSID& conid)
	{
		std::map<SESSID, ServerInfo*>::iterator itr = m_mapCSInfo.find(conid);
		if (itr != m_mapCSInfo.end())
		{
			ServerInfo* pInfo = itr->second;
			return pInfo;
		}

		return NULL;
	}

	ServerInfo* Find( int nType, int nSrvId )
	{
		int64 id = ((int64)nType << 32) | nSrvId;
		return FindByID( id );
	}

	void RemoveAll()
	{
		for (std::map<int64, ServerInfo*>::iterator itr = m_mapISInfo.begin();
			itr != m_mapISInfo.end(); ++itr)
		{
			delete itr->second;
		}
		m_mapISInfo.clear();
		m_mapCSInfo.clear();
	}
};

#endif // END __OUTER_SERVER_MANAGER_H__