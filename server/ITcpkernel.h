//--------------------------------------------------------------------
// 文件名:		ITcpkernel.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年12月24日
// 创建人:		xiecy
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_TCP_KERNEL_H_
#define _I_TCP_KERNEL_H_

class IVarList;

class ITcpKernel
{
public:
	virtual ~ITcpKernel() = 0;

	//发送消息给游戏服务器
	virtual bool SendMsgToGameServer(int connid, const IVarList& args) = 0;

	// 获取App的账号
	virtual const char* GetTaskDevAccount() = 0;

	// 获取App密码
	virtual const char* GetTaskDevPwd() = 0;

	// 获取即时通讯云的应用ID
	virtual const char* GetTaskAppKeyIM() = 0;

	// 获取语音通讯云的应用ID
	virtual const char* GetTaskAppKeyVoice()= 0;
};

inline ITcpKernel::~ITcpKernel() {}

#endif //_I_TCP_KERNEL_H_