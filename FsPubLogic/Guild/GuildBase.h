#ifndef __GUILD_BASE_H__
#define __GUILD_BASE_H__
#include "server\IPubKernel.h"
#include "FsGame\Define\PubDefine.h"
#include "GuildManager.h"

class GuildBase{
public:
	GuildBase(){
		m_pGuildPubData = PubGuildManager::m_pInstance->m_pGuildPubData;
		m_ServerId = PubGuildManager::m_pInstance->m_ServerId;
		m_pPubSpace = PubGuildManager::m_pInstance->m_pPubSpace;
	};
	virtual ~GuildBase(){};
	virtual bool Init(IPubKernel* pPubKernel) =0;
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg){
		return 0;
	};
	virtual bool CreateGuildInit(IPubData* guildData){ return true; };
	// 公共数据空间名称
	virtual const char* GetName(){
		return PUBSPACE_GUILD;
	};
	//代码补丁接口
	virtual bool Patch(IPubData* guildData){ return true; };

	virtual void ShutDown(){};
	virtual bool CallBack(){ return true; };
	IPubSpace* m_pPubSpace{NULL};         // 公会公共数据区
	IPubData* m_pGuildPubData{ NULL };      // 公会公共数据项
	std::wstring m_ServerId;        // 服务器编号

};
















#endif