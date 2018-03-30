//--------------------------------------------------------------------
// 文件名:		IPubLogic.h
// 内  容:		公共数据服务器逻辑公共数据空间接口
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __IPubLogic_h__
#define __IPubLogic_h__

#include "../../server/IPubKernel.h"

class IPubLogic
{
public:
	/// \brief 公共数据空间名称
	virtual const char * GetName() = 0;

	/// \brief 公共数据服务器启动
	/// \param pPubKernel 核心指针，下同
	virtual int OnPublicStart(IPubKernel* pPubKernel) = 0;

	/// \brief 公共数据服务器关闭
	/// \param pPubKernel 核心指针，下同
	virtual int OnServerClose(IPubKernel* pPubKernel) {return 0;}

	/// \brief 公共数据空间中的所有项加载完成
	/// \param pPubKernel 核心指针，下同
	virtual int OnPubSpaceLoaded(IPubKernel* pPubKernel) = 0;

	/// \brief 公共数据项加载完成
	/// \param data_name 公共数据项名
	virtual int OnPubDataLoaded(IPubKernel* pPubKernel, const wchar_t * data_name) = 0;

	/// \brief 接收到场景服务器的消息
	/// \param source_id 发消息的服务器标识
	/// \param msg 消息内容
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg) = 0;

public:
	// 更新配置
	virtual bool OnUpateConfig(IPubKernel* pPubKernel, int source_id, 
		int scene_id, const IVarList& msg){ return false;};
	// for command
	virtual bool OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args) { return false; }

};

#endif //__IPubLogic_h__