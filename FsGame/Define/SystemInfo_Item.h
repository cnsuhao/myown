//--------------------------------------------------------------------
// 文件名:	SystemInfo_Item.h
// 内 容:	物品系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Item_H_
#define _SystemInfo_Item_H_


// ItemModule 物品(7001-9000)【子系统可详细划分】
enum SystemInfo_Item
{

#pragma region // ToolItemModule 道具7001-7100
	SYSTEM_INFO_ID_7001 = 7001,	// 死亡不可以使用道具
	SYSTEM_INFO_ID_7002,		// 物品冷却中
	SYSTEM_INFO_ID_7003,		// 等级不足, 需要等级{0}
	SYSTEM_INFO_ID_7004,		// 背包空间不足
	SYSTEM_INFO_ID_7005,		// 珍惜物品提示:{0:阵营}{1:vip}{2:玩家}从{3:来源}获得{4:物品}args<<阵营<<姓名<<vip<<来源<<id<<品质
	SYSTEM_INFO_ID_7006,		// 背包低容量提醒，不足{0}个格子
	SYSTEM_INFO_ID_7007,		// 成功出售, 获得{0}个铜币
	SYSTEM_INFO_ID_7008,		// 成功解锁{0}个个格子, 花费{1}个元宝
	SYSTEM_INFO_ID_7009,		// 背包扩充成功, 获得{0}个格子
	SYSTEM_INFO_ID_7010,		// 背包空间不足, 物品已通过邮件发送
	SYSTEM_INFO_ID_7011,        // 元宝不足
	SYSTEM_INFO_ID_7012,		// 扣银元失败
	SYSTEM_INFO_ID_7013,		// 银元提示
	SYSTEM_INFO_ID_7014,		// 铜币提示
	SYSTEM_INFO_ID_7015,		// 熔炼值提示
	SYSTEM_INFO_ID_7016,		// 经验提示

	SYSTEM_INFO_ID_7017,		// 玉珏合成失败,功能未开启
	SYSTEM_INFO_ID_7018,		// 玉珏合成失败,已存在此玉珏
	SYSTEM_INFO_ID_7019,		// 玉珏合成失败,碎片不够
	SYSTEM_INFO_ID_7020,		// 玉珏升级失败,玩家等级不足
	SYSTEM_INFO_ID_7021,		// 玉珏升级失败,升级石不足
	SYSTEM_INFO_ID_7022,		// 玉珏升阶失败,材料不够或碎片不够
#pragma endregion
	
};


#endif