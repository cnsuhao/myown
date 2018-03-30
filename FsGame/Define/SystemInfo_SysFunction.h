//--------------------------------------------------------------------
// 文件名:	SystemInfo_SysFunction.h
// 内 容:	基础系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_SysFunction_H_
#define _SystemInfo_SysFunction_H_


// SystemFunctionModule 基础系统(19001-21000)【子系统详细划分】
enum SystemInfo_SysFunction
{
	
#pragma region // Shop商店系统(19001-19200)
	SYSTEM_INFO_ID_19001 = 19001,	// 购买失败
	SYSTEM_INFO_ID_19002,			// 付费刷新次数已用完
	SYSTEM_INFO_ID_19003,			// 没有购买该物品的权限
	SYSTEM_INFO_ID_19004,			// 商品数量不够
	SYSTEM_INFO_ID_19005,			// 元宝不足

#pragma endregion	

#pragma region // Other其他(19201-19300)
	SYSTEM_INFO_ID_19201 = 19201,	// 该功能处于冷却中，{0}秒后可以使用
	SYSTEM_INFO_ID_19202,			// 查看{0}的物品失败，生成物品信息失败
	SYSTEM_INFO_ID_19203,			// 兑换失败
	SYSTEM_INFO_ID_19204,			// 今日{0}获取量已经达到每日峰值！每日上限：{1}


#pragma endregion	

#pragma region // 翅膀系统(19301-19400)
	SYSTEM_INFO_ID_19301 = 19301,	// 翅膀升级失败,已到最大级别
	SYSTEM_INFO_ID_19302,			// 翅膀升级失败,钱不够
	SYSTEM_INFO_ID_19303,			// 翅膀升级失败,材料不够
	SYSTEM_INFO_ID_19304,			// 翅膀升阶失败,材料不够
	SYSTEM_INFO_ID_19305,			// 翅膀升阶失败,翅膀等级不足
#pragma endregion	

#pragma region // 坐骑系统(19401-19500)
	SYSTEM_INFO_ID_19401 = 19401,	// 皮肤已解锁
	SYSTEM_INFO_ID_19402,			// 前置皮肤未解锁
#pragma endregion	
	
};


#endif