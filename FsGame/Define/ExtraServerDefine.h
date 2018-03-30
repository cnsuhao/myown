/*
    附加服务器连接索引定义
    Liangxk
*/

#if !defined __EXTRA_SERVER_DEFINE_H__
#define __EXTRA_SERVER_DEFINE_H__

// Extra服务器连接索引
enum extra_server_index_type
{
	ESI_MONITOR_SERVER			= 0,        // 监控服务器连接索引
	ESI_WEB_EXCHANGE_SERVER		= 1,        // 线下交易服务器连接索引
	ESI_DUMP_SERVER				= 2,        // 转储服务器
	ESI_MOBILE_SERVER			= 3,        // 移动设备服务器
	ESI_SDOWEB_SERVER			= 4,        // 盛大 WEB 服务器
	ESI_APPLE_ORDER_SERVER		= 5,        // 苹果内购充值服务器
	ESI_VOICE_SERVER			= 6,		// 语音模块
	ESI_CHARGE_REWARD_SERVER	= 7,		// 充值奖励服务器
	ESI_GLOBAL_RANK_SERVER		= 8,		// 全局排行榜服务器
	ESI_GMCC_SERVER				= 9,		// gmcc服务器
};

#endif // __EXTRA_SERVER_DEFINE_H__
