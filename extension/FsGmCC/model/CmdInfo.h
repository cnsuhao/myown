//--------------------------------------------------------------------
// 文件名:		CmdInfo.h
// 内  容:		Gm信息定义
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_CMD_INFO_H__
#define __MODEL_CMD_INFO_H__
#include <string>
#include <vector>

struct CmdInfo
{
	std::string roleId;
	std::string command;
	std::string operateRoleId;
	std::vector<std::string>  cmdArgs;
	int64_t		seq;
};
#endif 