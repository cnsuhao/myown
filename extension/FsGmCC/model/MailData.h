//--------------------------------------------------------------------
// 文件名:		MailInfo.h
// 内  容:		邮件信息定义
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_MAIL_DATA_H__
#define __MODEL_MAIL_DATA_H__
#include <string>
#include <stdint.h>

struct MailData
{
	int64_t			MailId;
	int32_t			MailType;
	int64_t			CreateTime;
	int32_t			Status;
	int32_t			Period;

	std::string		Title;
	std::string		Content;
	std::string		senderId;
	std::vector<std::string>	ReciverIds;

	bool			GlobalMail;		// 是否广播邮件
	bool			HasAttach;
	std::string		AttachInfo;
};
#endif 