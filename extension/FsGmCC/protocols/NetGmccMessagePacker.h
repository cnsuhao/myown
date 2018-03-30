//--------------------------------------------------------------------
// 文件名:		NetVarListPacker.h
// 内  容:		
// 说  明:		
// 创建日期:	2016年9月17日
// 创建人:		李海罗
// 版权所有:	苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#include "NetWorldAction.h"

#ifndef __NET_GMCC_MESSAGE_PACKER_H__
#define __NET_GMCC_MESSAGE_PACKER_H__

// 消息打包器
class NetGmccMessagePacker : public NetVarListPacker
{
protected:
	virtual INetVarListActionProtocol* CreateAction(int nKey) const;
};

#endif 