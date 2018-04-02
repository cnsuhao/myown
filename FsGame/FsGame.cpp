//--------------------------------------------------------------------
// 文件名:		FsGame.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年10月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#include "server/LogicDll.h"
#include "public/Inlines.h"

//SceneBaseModule
#include "SceneBaseModule/SceneBaseModule.h"

//PlayerBaseModule
#include "SystemFunctionModule/CreateRoleModule.h"
#include "SystemFunctionModule/PlayerBaseModule.h"
#include "SystemFunctionModule/ToolBoxModule.h"

//NpcBaseModule
#include "NpcBaseModule/NpcCreatorModule.h"
#include "NpcBaseModule/NpcBaseModule.h"
#include "NpcBaseModule/AI/AISystem.h"
#include "NpcBaseModule/DoorNpcModule.h"
#include "NpcBaseModule/SafeAreaNpcModule.h"
#include "NpcBaseModule/TrapNpcModule.h"
#include "NpcBaseModule/TrapEvent/TrapEventManagerModule.h"
//#include "NpcBaseModule/SpyNpc.h"

//ItemModule
#include "ItemModule/ItemBaseModule.h"
#include "ItemModule/ToolItem/ToolItemModule.h"
#include "ItemModule/JadeModule.h"

// #include "ItemModule/SoulModule.h"
// #include "ItemModule/SoulEffect.h"
// #include "ItemModule/FashionModule.h"

// #include "ItemModule/ItemComposeModule.h"

// EquipmentModule
#include "EquipmentModule/EquipmentModule.h"
#include "EquipmentModule/EquipStrengthenModule.h"
#include "EquipmentModule/EquipSmeltModule.h"
#include "EquipmentModule/JewelModule.h"
#include "EquipmentModule/EquipBaptiseModule.h"
#include "EquipmentModule/EquipModifyPackModule.h"
#include "EquipmentModule/EquipForgingModule.h"
#include "EquipmentModule/ApperanceModule.h"

// SocialSystemModule
#include "SocialSystemModule/SystemMailModule.h"
#include "SocialSystemModule/FriendModule.h"
#include "SocialSystemModule/TeamModule.h"
#include "SocialSystemModule/RequestModule.h"
#include "SocialSystemModule/ChatModule.h"

// TaskModule
#include "TaskModule/TaskManager.h"

//SystemFunctionModule
#include "SystemFunctionModule/ChannelModule.h"
#include "SystemFunctionModule/MotionModule.h"
#include "SystemFunctionModule/RankListModule.h"
#include "SystemFunctionModule/StaticDataQueryModule.h"
#include "SystemFunctionModule/DropModule.h"
#include "SystemFunctionModule/CoolDownModule.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "SystemFunctionModule/RewardModule.h"
#include "SystemFunctionModule/ResetTimerModule.h"
#include "SystemFunctionModule/Shop/ShopManagerModule.h"
#include "SystemFunctionModule/LookModule.h"
#include "SystemFunctionModule/EnergyModule.h"
#include "SystemFunctionModule/WingModule.h"
#include "SystemFunctionModule/BattleAbilityModule.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"
#include "SystemFunctionModule/RideModule.h"
#include "SystemFunctionModule/MapModule.h"

//CommonMoudle
#include "CommonModule/GMModule.h"
#include "CommonModule/LogicClassModule.h"
#include "CommonModule/LuaExtModule.h"
#include "CommonModule/AsynCtrlModule.h"
#include "CommonModule/LuaScriptModule.h"
#include "CommonModule/PathFindingModule.h"
#include "CommonModule/VirtualRecModule.h"
#include "CommonModule/LevelModule.h"
#include "CommonModule/PropRefreshModule.h"
#include "CommonModule/ContainerModule.h"
#include "CommonModule/PubModule.h"
#include "CommonModule/LogModule.h"
#include "CommonModule/SnsDataModule.h"
#include "CommonModule/LandPosModule.h"
#include "CommonModule/SwitchManagerModule.h"
#include "CommonModule/ActionMutex.h"
#include "CommonModule/EnvirValueModule.h"
#include "CommonModule/FunctionEventModule.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "CommonModule/CommRuleModule.h"
//#include "CommonModule/PasselNpcCreator.h"

//SkillModule
#include "SkillModule/FightModule.h"
#include "SkillModule/FightState.h"
#include "SkillModule/Skillmodule.h"
#include "SkillModule/inner/SkillEventManager.h"
#include "SkillModule/flowModule.h"
#include "SkillModule/Impl/SkillDataQueryModule.h"
#include "SkillModule/BufferModule.h"
//#include "SkillModule/inner/SkillBreakManager.h"
#include "SkillModule/Impl/FormulaManagerModule.h"
#include "SkillModule/PKModelModule.h"
#include "SkillModule/SkillUpgradeModule.h"
//#include "SkillModule/FlySkillModule.h"

int64_t g_nMaxCirculateCount = 50000;

void SetMaxCirculateCount(int count)
{
	if (g_nMaxCirculateCount != count)
	{
		g_nMaxCirculateCount = count;
	}
}

// 定义游戏逻辑DLL
DECLARE_LOGIC_DLL;

// SkillModule
LOGIC_MODULE_CREATOR(BufferModule);
LOGIC_MODULE_CREATOR(FightModule);
LOGIC_MODULE_CREATOR(FightState);
LOGIC_MODULE_CREATOR(SkillModule);
//LOGIC_MODULE_CREATOR(SkillBreakManagerModule);
LOGIC_MODULE_CREATOR(SkillEventManager);
LOGIC_MODULE_CREATOR(FlowModule);
LOGIC_MODULE_CREATOR(SkillDataQueryModule);
LOGIC_MODULE_CREATOR(SkillUpgradeModule);
LOGIC_MODULE_CREATOR(FormulaManagerModule);
LOGIC_MODULE_CREATOR(PKModelModule);
//LOGIC_MODULE_CREATOR(FlySkillModule);



//SystemFunctionModule
LOGIC_MODULE_CREATOR(ChannelModule);
LOGIC_MODULE_CREATOR(MotionModule);
LOGIC_MODULE_CREATOR(RankListModule);
LOGIC_MODULE_CREATOR(StaticDataQueryModule);
LOGIC_MODULE_CREATOR(CoolDownModule);
LOGIC_MODULE_CREATOR(DropModule);
LOGIC_MODULE_CREATOR(CapitalModule);
LOGIC_MODULE_CREATOR(RewardModule);
LOGIC_MODULE_CREATOR(ShopManagerModule);
LOGIC_MODULE_CREATOR(LookModule);
LOGIC_MODULE_CREATOR(EnergyModule);
LOGIC_MODULE_CREATOR(WingModule);
LOGIC_MODULE_CREATOR(BattleAbilityModule);
LOGIC_MODULE_CREATOR(ActivateFunctionModule);
LOGIC_MODULE_CREATOR(RideModule);
LOGIC_MODULE_CREATOR(MapModule);

//ItemModule
LOGIC_MODULE_CREATOR(ItemBaseModule);
LOGIC_MODULE_CREATOR(ToolItemModule);
LOGIC_MODULE_CREATOR(JadeModule);
// LOGIC_MODULE_CREATOR(ItemComposeModule);


// EquipmentModule
LOGIC_MODULE_CREATOR(EquipmentModule);
LOGIC_MODULE_CREATOR(EquipModifyPackModule);
LOGIC_MODULE_CREATOR(EquipSmeltModule);
LOGIC_MODULE_CREATOR(EquipStrengthenModule);
LOGIC_MODULE_CREATOR(EquipBaptiseModule);
LOGIC_MODULE_CREATOR(JewelModule);
LOGIC_MODULE_CREATOR(EquipForgingModule);
LOGIC_MODULE_CREATOR(ApperanceModule);

//SocialSystemModule
LOGIC_MODULE_CREATOR(SystemMailModule);
LOGIC_MODULE_CREATOR(FriendModule);
LOGIC_MODULE_CREATOR(TeamModule);
LOGIC_MODULE_CREATOR(RequestModule);
LOGIC_MODULE_CREATOR(ChatModule);

// TaskModule
LOGIC_MODULE_CREATOR(TaskManager)

//NpcBaseModule
LOGIC_MODULE_CREATOR(NpcCreatorModule);
LOGIC_MODULE_CREATOR(NpcBaseModule);
LOGIC_MODULE_CREATOR(AISystem);
LOGIC_MODULE_CREATOR(DoorNpcModule);
LOGIC_MODULE_CREATOR(SafeAreaNpcModule);
LOGIC_MODULE_CREATOR(TrapNpcModule);
LOGIC_MODULE_CREATOR(TrapEventManagerModule);

//PlayerBaseModule
LOGIC_MODULE_CREATOR(CreateRoleModule);
LOGIC_MODULE_CREATOR(PlayerBaseModule);
LOGIC_MODULE_CREATOR(ToolBoxModule);
LOGIC_MODULE_CREATOR(SceneBaseModule);

//CommonMoudle
LOGIC_MODULE_CREATOR(GMModule);
LOGIC_MODULE_CREATOR(AsynCtrlModule);
LOGIC_MODULE_CREATOR(PathFindingModule);
LOGIC_MODULE_CREATOR(VirtualRecModule);
LOGIC_MODULE_CREATOR(LevelModule);
LOGIC_MODULE_CREATOR(PropRefreshModule);
LOGIC_MODULE_CREATOR(LogModule);
LOGIC_MODULE_CREATOR(ContainerModule);
LOGIC_MODULE_CREATOR(PubModule);
LOGIC_MODULE_CREATOR(ResetTimerModule);
LOGIC_MODULE_CREATOR(SnsDataModule);
LOGIC_MODULE_CREATOR(LandPosModule);
LOGIC_MODULE_CREATOR(SwitchManagerModule);
LOGIC_MODULE_CREATOR(ActionMutexModule);
LOGIC_MODULE_CREATOR(CommRuleModule);
//LOGIC_MODULE_CREATOR(PasselNpcCreator);

// 脚本优先
//LOGIC_MODULE_CREATOR(QueueModule);
LOGIC_MODULE_CREATOR(EnvirValueModule);
LOGIC_MODULE_CREATOR(FunctionEventModule);
LOGIC_MODULE_CREATOR(ReLoadConfigModule);
LOGIC_MODULE_CREATOR(LuaScriptModule);
LOGIC_MODULE_CREATOR(LuaExtModule);



// 这个逻辑定义必须放在最后
LOGIC_MODULE_CREATOR(LogicClassModule);

void OnConfigError( bool bError, const char* msg )
{
	if (NULL != msg)
	{
		if ( bError )
		{
			::extend_warning(LOG_ERROR, msg);
		}
		else
		{
			::extend_warning(LOG_INFO, msg);
		}
	}
}

#ifdef _WIN32
#ifdef _DEBUG

#include "utils/LeakChecker.h"

const char* g_leak_checker_file = "mem_fsgame_server.log";
CLeakChecker g_LeakChecker;
/*
关于内存泄露的CRT内存监视

1>对于全局变量和类的静态变量。无法正常监视.（所有游戏中，再用全局和静态指针 用NEW去分配）
2>如果内存泄露的序号都是一样的时候。
可以考虑

例如以下内存泄露文件报告内容
Dumping objects ->
{4010}normal block at 0x05E938D8, 4096 bytes long.
Data: <                > 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Object dump complete.
 
//其总{4010}是内存分配序号， Data: 是内存内容

_CrtSetBreakAlloc(4010);//4010是内存序号。(给内存分配器下分配断点)

// 如果分配的内存序号是固定的,你可以考虑使用下面的方法.效率要慢一些.
// {4010}normal block at 0x05E938D8, 4096 bytes long. 方法就是调试器的内存分配器进行HOOK
// 详细请参考一下代码
// 并调用g_ALLOC_HOOK = _CrtSetAllocHook(MyAllocHook);

//方法重新监控内存分配器
int MyAllocHook( int nAllocType,void *pvData, size_t nSize,int nBlockUse,long lRequest, const unsigned char* szFileName,int nLine)
{

//#define _FREE_BLOCK      0
//#define _NORMAL_BLOCK    1
//#define _CRT_BLOCK       2
//#define _IGNORE_BLOCK    3
//#define _CLIENT_BLOCK    4

if( nBlockUse == _CRT_BLOCK)//C运行时库那不能的内存分配
{
return TRUE;
}
if( nBlockUse == _NORMAL_BLOCK)
{
// if( (nSize>900) && (nSize < 1100))
if( nSize == 968 )
{
char mbzSize[512];
sprintf(mbzSize,"MemSize nSize:%d :file:%s line:%d\n",nSize,szFileName,nLine);
::OutputDebugStringA(mbzSize);
}
}

return TRUE;
}

_CRT_ALLOC_HOOK  g_ALLOC_HOOK;
*/

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_LeakChecker.Initialize();
		srand((unsigned int)time(NULL));
		//   g_ALLOC_HOOK = _CrtSetAllocHook(MyAllocHook);
		//      _CrtSetBreakAlloc(4010);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//  _CrtSetAllocHook(g_ALLOC_HOOK);
		g_LeakChecker.DumpMemory();
		g_LeakChecker.Shutdown();
		break;
	default:
		break;
	}

	return TRUE;
}

#endif // _DEBUG
#endif // _WIN32
