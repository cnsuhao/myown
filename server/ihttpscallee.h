//--------------------------------------------------------------------
// 文件名:		IHttpsCallee.h
// 内  容:		Https模块接口
// 说  明:		
// 创建日期:	2015年01月06日
// 创建人:		xiecy
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _I_VOICE_CALLEE_H
#define _I_VOICE_CALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "../server/IKernel.h"
#include "../server/ICreator.h"
#include "../server/https/IHttpsKernel.h"
#include "../server/ITcpKernel.h"

/*
继存自ILogicModule，只是为了共用公共的接口，我们并不需要去实现它
*/
class IHttpsCallee : public ILogicModule
{
public:
	virtual ~IHttpsCallee() = 0;

	/// \brief	逻辑模块初始化
	/// \param	pHttpKernel	HTTPS kernel
	/// \return	bool		返回处理结果
	virtual bool OnHttpsStart(IHttpsKernel* pHttpKernel) = 0;

	/// \brief	接收到外部的连接
	/// \param	connid		连接id
	/// \param	addr		地址
	/// \param	port		端口号
	/// \return	void
	virtual void OnOuterAccept(int connector_id, const char* addr, int port) = 0;

	/// \brief	接收到外部的连接关闭
	/// \param	connid		连接id
	/// \param	addr		地址
	/// \param	port		端口号
	/// \return	void
	virtual void OnOuterClose(int connector_id, const char* addr, int port) = 0;

	/// \brief	收到外部命令
	/// \param	connid		连接id
	/// \param	args		varlist
	/// \return	int
	virtual int OnOuterCommand( int connid, const IVarList& args ) = 0;

	//---------------------------------------------------------------------------

	// 初始化
	virtual bool Init(IKernel* pKernel) { return true; }

	// 关闭
	virtual bool Shut(IKernel* pKernel) { return true; }

	// 场景启动之前的初始化（所有OnCreateClass已经执行完毕）
	virtual bool BeforeLaunch(IKernel* pKernel) { return true; }
};

inline IHttpsCallee::~IHttpsCallee() {}

#endif // _I_VOICE_CALLEE_H

