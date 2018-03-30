//--------------------------------------------------------------------
// 文件名:		IDomainData.h
// 内  容:		公共数据服务器逻辑公共数据空间接口
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __IPubDomainData_h__
#define __IPubDomainData_h__

#include "../../server/IPubKernel.h"
#include "../../public/Inlines.h"
#include <string>

class IDomainData
{
public:
	IDomainData(const std::wstring & domain_name, const int domain_id=0) :
	  m_domainName(domain_name), m_domainId(domain_id)
	  {
	  }

	  virtual ~IDomainData()
	  {
	  }

public:
	virtual const std::wstring & GetDomainName(IPubKernel * pPubKernel)
	{
		return m_domainName;
	}

	virtual int GetDomainId(IPubKernel * pPubKernel)
	{
		return m_domainId;
	}
	virtual bool GetSave() = 0;

	/// \brief 公共数据空间中的所有项加载完成
	/// \param pPubKernel 核心指针，下同
	virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace) = 0;

	/// \brief 公共数据项加载完成
	/// \param data_name 公共数据项名
	virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace) = 0;

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param msg 消息内容
	virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		int source_id, int scene_id, const IVarList& msg) = 0;

public:
	//更新配置
	virtual bool OnUpdateConfig(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		int source_id, int scene_id, const IVarList& msg){return false;};
public:
	// for command
	virtual bool OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args) { return false; }
protected:
	std::wstring m_domainName;
	int m_domainId;
};

#endif //__IPubDomainData_h__
