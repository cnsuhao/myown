//--------------------------------------------------------------------
// 文件名:		NetChargeCustom.h
// 内  容:		计费自定义的协议
// 说  明:		
// 创建日期:		2016年10月26日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_CHARGE_CUSTOM_H__
#define __NET_WORLD_CHARGE_CUSTOM_H__

enum EmNetworldCustem
{
	NET_WORLD_CUSTOM_LOGIN_READY = 1,				// 准备好可以登陆了 GAME->CHARGE 参数: 是否准备好了(int) 游戏id(int64) 服务器id(int64) 分区id(int64) channelid(渠道id)int64 nProduction(int64)
	NET_WORLD_CUSTOM_LOGIN_STATE_ACK,				// 回应登陆状态	CHARGE->GAME 参数: 无
};

#endif // END __NET_WORLD_CHARGE_CUSTOM_H__