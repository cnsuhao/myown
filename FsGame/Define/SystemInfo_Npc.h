//--------------------------------------------------------------------
// 文件名:	SystemInfo_Npc.h
// 内 容:	Npc系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Npc_H_
#define _SystemInfo_Npc_H_

// NpcBaseModule Npc管理(9001-11000)【子系统详细划分】
enum SystemInfo_Npc
{
	
#pragma region // SafeAreaNpcModule安全区Npc（9001-9100）
	
	SYSTEM_INFO_ID_9001 = 9001,		// 进入安全区域，安全区域内无法进行pk
	SYSTEM_INFO_ID_9002,			// 离开安全区域，可能会遭到其他国家玩家的攻击！
	SYSTEM_INFO_ID_9003,			// 你已经进入本国安全区，将会受到安全保护
	SYSTEM_INFO_ID_9004,			// 你已经进入敌国安全区，有可能会受到敌国玩家攻击
	SYSTEM_INFO_ID_9005,			// 你已经进入中立安全区，将会受到安全保护
	SYSTEM_INFO_ID_9006,			// 离开本国安全区，可能会遭到敌国玩家的攻击
	SYSTEM_INFO_ID_9007,			// 离开敌国安全区
	SYSTEM_INFO_ID_9008,			// 离开中立安全区，可能会遭到敌国玩家的攻击

#pragma endregion

};


#endif