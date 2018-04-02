//----------------------------------------------------------
// 文件名:      Domain_Teamoffer.h
// 内  容:      组队缉盗系统
// 说  明:
// 创建日期:    2018年04月02日
// 创建人:      tzt 
// 修改人:    
//    :       
//----------------------------------------------------------

#ifndef __Domain_Teamoffer_H__
#define __Domain_Teamoffer_H__

#include "IDomainData.h"

class Domain_Teamoffer : public IDomainData
{
public:
	Domain_Teamoffer() : IDomainData(L"", 0)
	{} 

	virtual const std::wstring & GetDomainName(IPubKernel * pPubKernel);

	//名称
	virtual std::wstring GetName(IPubKernel* pPubKernel){return GetDomainName(pPubKernel);}

	//是否保存到数据库
	virtual bool GetSave(){return true;}

	virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace);

	virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace);
private:
	// 接收到场景服务器的消息
	virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace, 
		int source_id, int scene_id, const IVarList& msg);

	// 组队悬赏建表
	void CreateTeamOfferRec(IPubKernel* pKernel, IPubData* pPubData);

	// 组队悬赏消息处理
	void OnTeamOfferMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace, 
		int source_id, int scene_id, const IVarList& msg);

	// 组队悬赏发起
	void OnLaunch(IPubKernel* pPubKernel, IPubData* pPubData, 
		const IVarList& msg, int source_id, int scene_id);

	// 取消发起组队悬赏
	void OnStop(IPubKernel* pPubKernel, IPubData* pPubData,
		const IVarList& msg, int source_id, int scene_id);

	// 组队悬赏开始
	void OnStart(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		const IVarList& msg, int source_id, int scene_id);

	// 组队悬赏有效验证
	void OnCheck(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		const IVarList& msg, int source_id, int scene_id);

	// 组队悬赏结果
	void OnResult(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		const IVarList& msg, int source_id, int scene_id);

	// 重置一次组队悬赏
	void OnReset(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		const IVarList& msg, int source_id, int scene_id);

	// 公共区发送到场景服
	void SendP2SMsg(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& sub_p2s_msg);

private:
	static Domain_Teamoffer* m_pDomain_Teamoffer;
};
#endif