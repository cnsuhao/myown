//--------------------------------------------------------------------
// 文件名:		IHttpsResponse.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年12月24日
// 创建人:		xiecy
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_HTTPS_RESPONSE_H_
#define _I_HTTPS_RESPONSE_H_

#include <queue>
#include <string>

class IHttpsResponse;

typedef void (*HTTPS_ON_RES_CALLBACK)(IHttpsResponse*, void *);

//HTTPS请求回复
class IHttpsResponse
{
public:

	virtual ~IHttpsResponse() = 0;

	/// \brief 获取连接ID
	virtual int GetConnid() = 0;

	/// \brief 获取消息ID
	virtual int GetMsgId() = 0;

	/// \brief 获取https请求标识
	virtual const char* GetHttpsResquestId() = 0;

	/// \brief 获取https请求返回的信息
	virtual const char* GetHttpsResponse() = 0;
};

inline IHttpsResponse::~IHttpsResponse() {}

#endif //_I_HTTPS_RESPONSE_H_