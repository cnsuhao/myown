//--------------------------------------------------------------------
// 文件名:		RewardDefine.h
// 内  容:		奖励
// 说  明:		
//				
// 创建日期:	2018年02月23日
// 整理日期:	2018年02月23日    
//--------------------------------------------------------------------

#ifndef __RewardDefine_H__
#define __RewardDefine_H__

const char* const STR_LEVEL_EXP = "LevelExp";		// 玩家经验标识

// 放入策略
enum
{
	REWARD_MAIL_ON_FULL = 1, // 背包满了就通过邮件发送
	REWARD_DISCARD_ON_FULL = 2, // 背包满了就丢弃放不下的
	REWARD_FAILED_ON_FULL = 3, // 背包满了就返回失败
	REWARD_MAIL = 4, // 通过邮件发送
};

enum  //二级消息
{
	S2C_SHOW_ITEM_LIST = 0,	//显示获得的物品 int 物品数量 

};

// 奖励类型
enum AwardType
{
	CAPITAL_AWARD,
	ITEM_AWARD,
	EXP_AWARD
};

#endif // __RewardDefine_H__
