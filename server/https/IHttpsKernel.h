//--------------------------------------------------------------------
// 文件名:		IHttpsKernel.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年12月24日
// 创建人:		xiecy
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _I_HTTPS_KERNEL_H_
#define _I_HTTPS_KERNEL_H_

#include "IHttpsResponse.h"

// HTTPS服务器逻辑接口版本
#define HTTPS_LOGIC_MODULE_VERSION 0x0001

class IVarList;

class IHttpsKernel
{
public:

	virtual ~IHttpsKernel() = 0;

	/// \brief	日志打印
	/// \param	info		日志内容
	/// \return	void		返回void
	virtual void WriteInfo(const char* info) = 0;

	/// \brief	添加HTTPS的请求地址
	/// \param	reqUrlIndex	请求HTTPS地址索引号
	/// \param	url			请求HTTPS地址
	/// \return	void		返回void
	virtual void AddHttpsRequestUrl(int reqUrlIndex, const char* url) = 0;

	/// \brief	发送消息给游戏服务器
	/// \param	connid		连接ID
	/// \param	args		变参
	/// \return	bool		返回bool
	virtual bool SendMsgToGameServer(int connid, const IVarList& args) = 0;

	/// \brief	发送https请求
	/// \param	connid		连接ID
	/// \param	reqUrlIndex	请求HTTPS地址索引号
	/// \param	command		请求命令(Post方式：方法名,Get方式：方法名加参数)
	/// \param	paramlist	请求参数(Post方式：有参数，Get方式：无参数)
	/// \param	msgId		消息ID
	/// \param	flagId		请求消息的标识
	/// \param	cb			回调地址
	/// \param	content		回调对象
	/// \return	bool		返回bool
	virtual bool SendHttpsRequest(int connid, int reqUrlIndex, const char* command, const char* paramlist,
		int msgId, const char* flagId = "",HTTPS_ON_RES_CALLBACK cb = NULL,void* content = NULL) = 0;

	/// \brief	发送https请求,带自定义header
	/// \param	connid		连接ID
	/// \param	reqUrlIndex	请求HTTPS地址索引号
	/// \param	command		请求命令(Post方式：方法名,Get方式：方法名加参数)
	/// \param	paramlist	请求参数(Post方式：有参数，Get方式：无参数)
	/// \param	custom_header_name		请求自定义header名称(CVarList() << "If-Modified-Since")
	/// \param	custom_header_content	请求自定义header内容(CVarList() << "Fri, 13 May 2016 03:40:07 GMT")
	/// \param	msgId		消息ID
	/// \param	flagId		请求消息的标识
	/// \param	cb			回调地址
	/// \param	content		回调对象
	/// \return	bool		返回bool
	virtual bool SendHttpsRequestEx(int connid, int reqUrlIndex, const char* command, const char* paramlist,
		const IVarList& custom_header_name, const IVarList& custom_header_content,
		int msgId, const char* flagId = "",HTTPS_ON_RES_CALLBACK cb = NULL,void* content = NULL) = 0;
};

inline IHttpsKernel::~IHttpsKernel() {}


#endif // _I_HTTPS_KERNEL_H_