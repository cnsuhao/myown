//--------------------------------------------------------------------
// 文件名:		NoticeInfo.h
// 内  容:		公告信息定义
// 说  明:		
// 创建日期:		2016年9月14日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_NOTICE_H__
#define __MODEL_NOTICE_H__
#include <string>
#include <stdint.h>

enum EmNotifyType
{
	EM_NOTIFY_NOTICE = 1,		// 公告
	EM_NOTIFY_ACTIVITY,			// 活动
	EM_NOTIFY_PAYED,			// 支付
	EM_NOTIFY_SIGNIN,
	EM_NOTIFY_STOREITEM,
	EM_NOTIFY_CONFIG,
};

enum EmNotifyOp
{
	EM_NOTIFY_OP_ADD,
	EM_NOTIFY_OP_DEL,
	EM_NOTIFY_OP_EDIT,
};

struct NotifyInfo
{
	int32_t			NotifyType;		// 通知类型
	int32_t			OpType;			// 操作类型(增删改) 
	std::string		AccId;			// 账号id
	std::string		RoleId;			// 对象id
	std::string		Content;		// 内容扩展
};

struct NoticeData
{
	std::string		Name;			// 名称
	std::string		Content;		// 内容
	int64_t			NoticeId;		// 通知id
	int64_t			CreateTime;		// 创建时间
	int64_t			EndTime;		// 结束时间
	int64_t			Interval;		// 播放间隔
	int32_t			Loop;			// 循环次数
	int32_t			Type;			// 通知类型
};
#endif 