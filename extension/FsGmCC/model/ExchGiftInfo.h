//--------------------------------------------------------------------
// 文件名:		ExchGiftInfo.h
// 内  容:		礼品兑换结果定义
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_EXCH_GIFT_INFO_H__
#define __MODEL_EXCH_GIFT_INFO_H__
#include <string>

struct ExchGiftInfo
{
	std::string		key;			// 兑换key
	std::string		account;		// 账号	
	std::string		roleid;			// 玩家uid
};

struct ExchGiftResult
{
	std::string roleId;
	std::string key;
	int result;
	std::string capitals;
	std::string items;
};
#endif 