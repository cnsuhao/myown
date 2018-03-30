//--------------------------------------------------------------------
// 文件名:      Domain_SceneList.h
// 内  容:      所有被创建的场景列表
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __Domain_SceneList_h__
#define __Domain_SceneList_h__

#include "IDomainData.h"
#include "../../utils/extend_func.h"

class Domain_SceneList : public IDomainData
{
public:

	Domain_SceneList() : IDomainData(L"", 0) {}

	// 公共数据区名
	virtual std::wstring & GetDomainName(IPubKernel* pPubKernel) 
	{
		if (m_domainName.empty())
		{
			wchar_t wstr[256];
			int server_id = pPubKernel->GetServerId();
			SWPRINTF_S(wstr, L"Domain_SceneList%d", server_id);
			m_domainName = wstr;
		}

		return m_domainName;
	}

	//是否保存到数据库
	virtual bool GetSave(){return false;}

	virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace);

	virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace);

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param msg 消息内容
	virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		int source_id, int scene_id, const IVarList& msg);

	void CreateTable(IPubData * pSceneListData, const char * table_name);
};

#endif //__Domain_SceneList_h__