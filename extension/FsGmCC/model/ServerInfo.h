//--------------------------------------------------------------------
// 文件名:		ServerInfo.h
// 内  容:		服务器在web中注册信息定义
// 说  明:		
// 创建日期:		2016年10月25日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_SERVER_INFO_H__
#define __MODEL_SERVER_INFO_H__
#include <string>
#include <vector>

struct SrvInfo
{
	long long Identity;
	long long GameId;
	long long ChannelId;
	long long ProductionId;
	long long ServerId;
	long long AreaId;
	long long DeployId;

	SrvInfo()
	{
		memset(this, 0, sizeof(SrvInfo));
	}

};
#endif 