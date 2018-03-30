//--------------------------------------------------------------------
// 文件名:		ReviveDefine.h
// 内  容:		死亡重生
// 说  明:		
//				
// 创建日期:	2018年02月23日
// 整理日期:	2018年02月23日    
//--------------------------------------------------------------------

#ifndef __REVIVE_DEFINE_H__
#define __REVIVE_DEFINE_H__

//(ini\SystemFunction\Revive\RevivePanel.xml)中RelivePanel字段
enum ReviveType
{
	NEARBY_REVIVE,		// 就近复活
	CITY_REVIVE,		// 皇城复活
	LOCAL_REVIVE,		// 原地复活
	WAIT_HELP,			// 等待救助

	MAX_REVIVE_NUM,
};

// 子消息
enum ReviveC2SMsg
{
	CS_REQUEST_REVIVE,  // 请求复活 int 复活类型		
};

enum ReviveS2CMsg
{
	SC_DEAD_PANEL,		// 死亡面板信息 string panel int gold
};
#endif // __RewardDefine_H__
