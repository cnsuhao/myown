//----------------------------------------------------------
// 文件名:      switchmanagerdefine.h
// 内  容:      功能开关定义
// 说  明:
// 创建人:        
// 创建日期:    2015年6月30日
//    :        
//----------------------------------------------------------
#ifndef __SWITCHMANAGER_DEFINE_H_
#define __SWITCHMANAGER_DEFINE_H_

// 功能开关表
const char* const DOMAIN_SWITCH_REC = "switch_rec";

// 功能开关域数据加载到数据库定时器时间,1分钟
#define SWITCH_REC_DELAY_TIME       (60*1000)           

// 表列
enum SwitchRecCol
{
    SWITCH_REC_COL_FUNCTION_ID = 0,		// 功能Id
	SWITCH_REC_COL_FUNCTION_NAME = 1,	// 功能名称
    SWITCH_REC_COL_CUR_STATE = 2,       // 当前状态(1:开启 0:关闭)
	SWITCH_REC_COL_TOTAL,
};

enum SwitchRecType
{
	SWITCH_REC_TYPE_INIT = 0,		// 资源初始化加载
	SWITCH_REC_TYPE_CHANGE = 1,		// 外部改动
};

// 功能id(与ini/SystemFunction/function_switch.xml相同)
//$@ Switch
enum SwitchFunctionType
{
    SWITCH_FUNCTION_MIN = 0,

	SWITCH_FUNCTION_CAPITAL_MONITOR,		//  = 货币限额监控
	SWITCH_FUNCTION_PLUG_IN,				//  = 外挂检测
	SWITCH_FUNCTION_PASSIVE_SKILL,			//	= 被动技能
	SWITCH_FUNCTION_SKILL_UP,				//	= 技能升级	
	SWITCH_FUNCTION_WING,					//	= 翅膀功能	
	SWITCH_FUNCTION_JADE,					//	= 玉珏功能	
	SWITCH_FUNCTION_GUILD_CREATE,			//  = 公会创建
	SWITCH_FUNCTION_GUILD_JOIN,				//  = 加入公会
	SWITCH_FUNCTION_GUILD_LEAVE,			//  = 离开公会
	SWITCH_FUNCTION_GUILD_DONATE,			//  = 公会捐献
	SWITCH_FUNCTION_GUILD_SHOP,				//  = 公会商店
	SWITCH_FUNCTION_ARENA,					//  = 比武场
	SWITCH_FUNCTION_WORLD_BOSS,				//  = 世界boss
	SWITCH_FUNCTION_CAMPAIGN_ASURA_BATTLE,	//  = 修罗战场

	SWITCH_FUNCTION_MAX,
};

#endif