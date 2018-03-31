//--------------------------------------------------------------------
// 文件名:      Domain_WorldBoss.h
// 内  容:      世界BOSS
// 说  明:		
// 创建日期:    2015年7月6日
// 创建人:      刘明飞
// 修改人:        
// 版权所有:    
//--------------------------------------------------------------------

#ifndef __Domain_WorldBoss_h__
#define __Domain_WorldBoss_h__

#include "IDomainData.h"

#include <map>

class Domain_WorldBoss : public IDomainData
{
public:
	Domain_WorldBoss();
public:
	virtual const std::wstring& GetDomainName(IPubKernel* pPubKernel);
	virtual std::wstring GetName(IPubKernel* pPubKernel){return GetDomainName(pPubKernel);}

	virtual bool GetSave();
	virtual int OnCreate(IPubKernel * pPubKernel, IPubSpace * pPubSpace); 
	virtual int OnLoad(IPubKernel * pPubKernel, IPubSpace * pPubSpace);
private:
	///  \brief 接收到场景服务器的消息
	///  \param source_id 发消息的服务器标识
	///  \param msg 消息内容
	virtual int OnMessage(IPubKernel * pPubKernel,
		IPubSpace * pPubSpace,
		int source_id,
		int scene_id,
		const IVarList & msg);


	// 记录世界boss伤害排行榜
	void UpdateBossRecInfo(IPubData* pWorldBossData, const IVarList & args);
private:
	static Domain_WorldBoss* m_pDomainWorldBoss;
};

#endif
