//---------------------------------------------------------
//文件名:       PubCmdDefine.h
//内  容:       公共数据命令
//说  明:       掠夺(Plunder),
//
//创建日期:     2017年08月01日
//创建人:       kevin 
// 版权所有:	WhalesGame Technology co.Ltd  
//---------------------------------------------------------

#ifndef _FSPUB_CMD_DEFINE_H_
#define _FSPUB_CMD_DEFINE_H_

//命令命令定义
enum EPubCommandType
{
	E_PUB_CMD_UNKNOWN = 0,
	//1~99 system.cmd
	E_PUB_CMD_READY = 1,			// 准备就绪

	E_PUB_CMD_ACTIVITY_STATUS = 10, // 活动状态
	E_PUB_CMD_EVERY_DAY_ZERO  = 11,	// 每日凌晨
	E_PUB_CMD_EVERY_DAY_RESET = 12,	// 每日重置 ，时间： 05:00

	E_PUB_CMD_EVERY_PER_HOUR   = 13,	// 每小时
	E_PUB_CMD_EVERY_PER_MINUTE = 14,	// 每分钟
	E_PUB_CMD_EVERY_PER_SECOND = 15,	// 每秒钟
	//100~199 logic.cmd
	E_PUB_CMD_GUILD_DISMISS        = 100,	//解散工会
	E_PUB_CMD_GUILD_CAPTAIN_CHANGE = 101,	//工会会长变更
	E_PUB_CMD_MAX,
};

#endif
