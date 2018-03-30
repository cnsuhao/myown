

#ifndef _SERVER_ISNSCALLEE_H
#define _SERVER_ISNSCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 公共数据回调接口

class ISnsKernel;

class ISnsCallee
{
public:
	virtual ~ISnsCallee() = 0;

	// \brief Sns服务器已启动，此时逻辑模块已加载完成
	// \param pKernel 核心指针，下同
	virtual int OnServerStarted(ISnsKernel* pKernel){return 1;}

	// \brief Sns逻辑对象被创建
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnSnsModuleCreate(ISnsKernel* pKernel, const IVarList& args) {return 1;}

	// \brief Sns服务器所以的Sns数据已经加载完成准备就绪(在若次OnSnsLoad之后)
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnServerReady(ISnsKernel* pKernel, const IVarList& args) {return 1;}

	// \brief sns数据在内存中创建
	// \param args
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataCreate(ISnsKernel* pKernel, const char* uid,const IVarList& args) {return 1;}

	// \brief 加载sns数据
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataLoad(ISnsKernel* pKernel, const char* uid,const IVarList& args) {return 1;}

	// \brief sns 数据从内存中卸载
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataUnload(ISnsKernel* pKernel,const char* uid, const IVarList& args) {return 1;}

	// \brief sns 数据从数据库中删除
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataRemove(ISnsKernel* pKernel, const char* uid,const IVarList& args) {return 1;}

	// \brief 收到消息
	// \param args
	// \distribute_id 分区id
	// \server_id  服务器编号
	// \memeber_id  场景服务器编号
	// \scene_id  发送场景的id
	// \uid  sns uid //sns数据关键字
	// \args  member发送的消息体
	virtual int OnMemberMessage(ISnsKernel* pKernel, int distribute_id,int server_id,int memeber_id,int scene_id, const char* uid,const IVarList& args) {return 1;}

	/// \brief Sns服务器关闭以前消息通知
	/// \param nServerCloseDelay 还剩余的多少时间服务器关闭
	virtual int OnServerCloseBeforeNotify(ISnsKernel* pKernel, int nServerCloseDelay){ return 1; }

	/// \brief 服务器每帧调用
	/// \param nFrameTicks 此帧的毫秒数
	virtual int OnFrameExecute(ISnsKernel* pCrossKernel, int nFrameTicks) { return 1; }

	/// \brief sns服务器关闭通知
	virtual int OnServerClose(ISnsKernel* pKernel){ return 1; }
};

inline ISnsCallee::~ISnsCallee() {}

#endif // _SERVER_IPUBCALLEE_H

