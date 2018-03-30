//--------------------------------------------------------------------
// 文件名:		ISnsLogic.h
// 内  容:		数据服务器逻辑数据空间接口
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __ISnsLogic_h__
#define __ISnsLogic_h__

#include "../../server/ISnsCallee.h"
#include "../../server/ISnsKernel.h"

class ISnsLogic
{
public:
	ISnsLogic(const std::wstring & sns_name, const int sns_id = 0) :m_snsName(sns_name), m_snsId(sns_id){}

	virtual ~ISnsLogic(){}

public:
	virtual const std::wstring & GetSnsName(ISnsKernel * pSnsKernel){ return m_snsName; }

	virtual int GetSnsId(ISnsKernel * pSnsKernel){ return m_snsId; }
	virtual bool GetSave(){ return true; }

	// \brief Sns逻辑对象被创建
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnModuleCreate(ISnsKernel* pSnsKernel, const IVarList& args) = 0;

	// \brief Sns服务器所以的Sns数据已经加载完成准备就绪(在若次OnSnsLoad之后)
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnReady(ISnsKernel* pSnsKernel, const IVarList& args) = 0;

	// \brief sns数据在内存中创建
	// \param args
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnCreate(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args) = 0;

	// \brief 加载sns数据
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnLoad(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args) = 0;

	// \brief 收到消息
	// \param args
	// \distribute_id 分区id
	// \server_id  服务器编号
	// \memeber_id  场景服务器编号
	// \scene_id  发送场景的id
	// \uid  sns uid //sns数据关键字
	// \args  member发送的消息体
	virtual int OnMessage(ISnsKernel* pSnsKernel, int distribute_id, int server_id,
		int memeber_id, int scene_id, const char* uid, const IVarList& args) = 0;
	virtual bool OnUpdateConfig(ISnsKernel* pSnsKernel, int distribute_id, int server_id,
		int memeber_id, int scene_id, const char* uid, const IVarList& args){return false;};

protected:
	std::wstring m_snsName;
	int m_snsId;

};

#endif //__ISnsLogic_h__
