//--------------------------------------------------------------------
// 文件名:		PushData.h
// 内  容:		推送数据定义
// 说  明:		
// 创建日期:		2016年10月20日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_GAME_PUSH_DATA_H__
#define __MODEL_GAME_PUSH_DATA_H__
#include <string>
#include <map>
#include <vector>
#include "PushDef.h"

struct PushData
{
	int msgType;						// 消息类型(EmPushType)
	int recvierPlatform;				// 接收平台(EmClientPlatform多个值)
	std::map<int, std::vector<std::string> > recivers;	// <接收类型(EmPushRecvierType中的一个值), 接收者标识>

	std::string title;					// 标提
	std::string content;				// 内容
	std::string sound;					// 声音
	int builderid;						// android显示样式
	int badge;							// 小图标数字
	int content_available;				// 推送唤醒 (ios7后可用)
	std::map<std::string, std::string>	extra;	// 扩展字段

public:
	PushData()
		: msgType(EM_PUSH_NOTIFICATION)
		, recvierPlatform( EM_CLIENT_ALL )
		, builderid(0)
		, content_available( 0 )
		, badge(0)
	{

	}
};

#endif 