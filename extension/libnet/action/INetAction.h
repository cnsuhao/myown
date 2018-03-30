//--------------------------------------------------------------------
// 文件名:		INetAction.h
// 内  容:		网络动作定义
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_NET_ACTION_H__
#define __I_NET_ACTION_H__

#include "IAction.h"
#include <memory>

typedef IMessage INetMessage;
typedef IRequest INetRequest;
typedef IResponse INetResponse;
typedef IAction INetAction;

#define NetAction		Action
#define NetReqAction	ReqAction

class INetConnection; 
enum EMPackResult
{
	EM_PACK_SUCCESS,		// 成功
	EM_PACK_BROKEN,			// 断包
	EM_PACK_UNREGISTER,		// 未注册
	EM_PACK_TIMEOUT,		// 回复超时
	EM_PACK_ERROR,			// 包错误
	EM_PACK_RAW_MSG,		// 原始包
	EM_PACK_EMPTY,			// 空包
};
class INetActionPacker
{
public:
	virtual ~INetActionPacker() = 0;
	virtual EMPackResult Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const = 0;
	virtual EMPackResult UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const = 0;
};
inline INetActionPacker::~INetActionPacker(){}


#endif // END __I_NET_ACTION_H__