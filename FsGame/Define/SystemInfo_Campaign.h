//--------------------------------------------------------------------
// 文件名:	SystemInfo_Campaign.h
// 内 容:	玩法系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Campaign_H_
#define _SystemInfo_Campaign_H_


// CampaignModule 玩法系统(1-3000) 【子系统详细划分】
enum SystemInfo_Campaign
{
	
#pragma region 	// ArenaModule 比武场	(1 - 50)
	SYSTEM_INFO_ID_1 = 1,			// 竞技场忙，请重试	(异常错误)
	SYSTEM_INFO_ID_2,				// 挑战CD中
	SYSTEM_INFO_ID_3,				// 达到购买上限
	SYSTEM_INFO_ID_4,				// 元宝不足, 需要{@n}元宝
	SYSTEM_INFO_ID_5,				// 清除挑战CD失败 钱不够
	SYSTEM_INFO_ID_6,				// 不可以挑战, 你或者对方在挑战中
	SYSTEM_INFO_ID_7,				// 不可以挑战, 你或者对方已晋级
	SYSTEM_INFO_ID_8,				// 不可以挑战名次低于自己的对手
	SYSTEM_INFO_ID_9,				// 不可以挑战，请刷新
	SYSTEM_INFO_ID_10,				// 玩家{0}成功取代玩家{1}，晋级第{2}名。
#pragma endregion

#pragma region 	// WorldBossNpc 世界boss	(51 - 100)
	SYSTEM_INFO_ID_51 = 51,			// 活动需要达到{0}！
	SYSTEM_INFO_ID_52,			    // 时间到活动结束！
	SYSTEM_INFO_ID_53,			    // 世界BOSS3分钟后出现 请大家做好准备！
	SYSTEM_INFO_ID_54,			    // 恭喜{0}击杀了世界BOSS{1}，获得了{2}！
#pragma endregion

#pragma region 	// CampaignAsuraBattle 修罗战场	(101 - 150)
	SYSTEM_INFO_ID_101 = 101,		// 修罗战场即将开始
	SYSTEM_INFO_ID_102,				// Boss死亡公告 boss被杀,{0}阵营胜利,全员增加了{1}分
	SYSTEM_INFO_ID_103,				// 修罗战场未开启
	SYSTEM_INFO_ID_104,				// 玩家等级不足
	SYSTEM_INFO_ID_105,				// 已经有buff了
	SYSTEM_INFO_ID_106,				// 钱不够
	SYSTEM_INFO_ID_107,				// 击杀{0}获得积分{1}
	SYSTEM_INFO_ID_108,				// 击杀{0},参与助攻获得积分{1}
	SYSTEM_INFO_ID_109,				// {0}阵营达到了{1}采集状态
	SYSTEM_INFO_ID_110,				// 剩余结束时间提示
	SYSTEM_INFO_ID_111,				// {0}的{1}和{2}的{3}排名第一,快去击杀
	SYSTEM_INFO_ID_112,				// Boss提示公告
	SYSTEM_INFO_ID_113,				// Boss出生公告
	SYSTEM_INFO_ID_114,				// 平局结果
	SYSTEM_INFO_ID_115,				// {0}胜利了
#pragma endregion
};


#endif