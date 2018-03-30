//--------------------------------------------------------------------
// 文件名:		PushDef.h
// 内  容:		推送服务定义
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_PUSH_DEFINE_H__
#define __NET_PUSH_DEFINE_H__

// 通知类型
enum EmPushType
{
	EM_PUSH_NOTIFICATION = 0x01,		// 通知
	EM_PUSH_CUSTOM = 0x02,				// 自定义(只有android支持)
};

// 通知接收者类型
enum EmPushRecvierType
{
	EM_PUSH_RECVIER_TAG = 2,			// 标签
	EM_PUSH_RECVIER_ALIAS = 3,			// 别名
	EM_PUSH_RECVIER_BROADCAST = 4,		// 广播
	EM_PUSH_RECVIER_REGISTRATIONID = 5,// 服务分配id
};

// 客户端平台定义
enum EmClientPlatform
{
	EM_CLIENT_IOS = 0x01,
	EM_CLIENT_ANDROID = 0x02,
	EM_CLIENT_WINPHONE = 0x04,

	EM_CLIENT_ALL = EM_CLIENT_IOS | EM_CLIENT_ANDROID | EM_CLIENT_WINPHONE,
};



#endif  // __NET_PUSH_DEFINE_H__ 