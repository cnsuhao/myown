//--------------------------------------------------------------------
// 文件名:      Domain_Boss.h
// 内  容:      Boss记录表
// 说  明:		
// 创建日期:    2018年03月30日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __Domain_Boss_h__
#define __Domain_Boss_h__

#include "IDomainData.h"
#include <vector>
#include <string>

class Domain_Boss : public IDomainData
{
public:
	Domain_Boss();
public:
	virtual const std::wstring & GetDomainName(IPubKernel * pPubKernel);
	virtual std::wstring GetName(IPubKernel* pPubKernel){return GetDomainName(pPubKernel);}

	virtual bool GetSave();
	virtual int OnCreate(IPubKernel * pPubKernel, IPubSpace * pPubSpace); 
	virtual int OnLoad(IPubKernel * pPubKernel, IPubSpace * pPubSpace);

	///  \brief 接收到场景服务器的消息
	///  \param source_id 发消息的服务器标识
	///  \param msg 消息内容
	virtual int OnMessage(IPubKernel * pPubKernel,
						  IPubSpace * pPubSpace,
						  int source_id,
						  int scene_id,
						  const IVarList & msg);
};

#endif	 // Domain_Boss
