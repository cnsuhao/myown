//--------------------------------------------------------------------
// 文件名:		DomainManager.h
// 内  容:		公共数据服务器逻辑模块:全局公共数据的管理
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __PubDomainManager_h__
#define __PubDomainManager_h__

#include "IDomainData.h"
#include "../Public/IPubLogic.h"
#include <string>
#include "../../FsGame/Define/PubDefine.h"
#include <unordered_map>

class DomainManager : public IPubLogic
{
public:
	DomainManager();
	virtual ~DomainManager();

	static IPubLogic* Instance();

	/// \brief 公共数据空间名称
	virtual const char * GetName(){return PUBSPACE_DOMAIN;}

	/// \brief 公共数据服务器启动
	/// \param pPubKernel 核心指针，下同
	virtual int OnPublicStart(IPubKernel* pPubKernel);

	/// \brief 公共数据服务器关闭
	/// \param pPubKernel 核心指针，下同
	virtual int OnServerClose(IPubKernel* pPubKernel);

	/// \brief 公共数据项加载完成
	/// \param data_name 公共数据项名
	virtual int OnPubDataLoaded(IPubKernel* pPubKernel, const wchar_t * data_name);

	/// \brief 公共数据空间中的所有项加载完成
	/// \param pPubKernel 核心指针，下同
	virtual int OnPubSpaceLoaded(IPubKernel* pPubKernel);

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param msg 消息内容
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg);

public:
	// 更新配置
	virtual bool OnUpdateConfig(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg);

	/// \brief 获得指定名称的DomainData
	/// \param data_name 名称
	/// \param msg 消息内容
	virtual IDomainData * FindDomainData(const wchar_t * data_name);

	// for command
	virtual bool OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args);
protected:
	typedef std::unordered_map<std::wstring, IDomainData*> MapData;
	MapData m_mapDataList;

private:
	static DomainManager* m_pInstance;
};

#endif //__PubDomainManager_h__
