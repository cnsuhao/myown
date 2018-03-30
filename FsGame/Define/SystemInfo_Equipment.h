//--------------------------------------------------------------------
// 文件名:	SystemInfo_Equipment.h
// 内 容:	装备系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Equipment_H_
#define _SystemInfo_Equipment_H_


// EquipmentModule 装备系统(5001-7000)【子系统详细划分】
enum SystemInfo_Equipment
{
#pragma region // JewelModule宝石（5001-5100）	

#pragma endregion

#pragma region // EquipStrengthenModule强化（5101-5200）	
	SYSTEM_INFO_ID_5101 = 5101,		// 强化已达到上限，无需再强化
#pragma endregion

#pragma region // EquipForgingModule锻造（5201-5300）	
	SYSTEM_INFO_ID_5201 = 5201,		// 职业不匹配
#pragma endregion

#pragma region // FashionModule时装（5301-5400）	
	SYSTEM_INFO_ID_5301 = 5301,		// 前置时装未解锁
	SYSTEM_INFO_ID_5302,			// 当前时装已解锁
	SYSTEM_INFO_ID_5303,			// 当前时装职业不匹配
	SYSTEM_INFO_ID_5304,			// 当前时装性别不匹配
#pragma endregion

};


#endif