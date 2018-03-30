//--------------------------------------------------------------------
// 文件名:      RankListDefine
// 内  容:      排行榜头文件
// 说  明:      玩家数据排行榜
// 创建日期:    2014-10-15
// 创建人:      liumf
// 版权所有:    苏州蜗牛数字科技股份有限公司
//--------------------------------------------------------------------
#ifndef _RANKLISTDEFINE_H
#define _RANKLISTDEFINE_H

#include "Fsgame/Define/header.h"
#include <vector>

// 排行榜类类型
enum RankClassType
{
	RANK_CLASS_NONE = -1,
	RANK_CLASS_PLAYER,			// 玩家排行榜类型
};

// 玩家排行榜类型
enum RankType
{
	RANK_LIST_NONE = -1,
	RANK_LIST_LEVEL,				// 等级排行榜
 	RANK_LIST_FIGHT_POWER,			// 战斗力排行榜
// 	RANK_LIST_KILL_NUM,				// 杀人排行榜
// 	RANK_LIST_CF_FIGHT_HURT,		// 国战输出伤害排行榜
// 	RANK_LIST_CF_BEAR_HURT,			// 国战承受伤害排行榜

	MAX_RANK_LIST_NUM
};

// 排行榜定义基础属性
struct RankListProp
{
	RankType	nRankType;			// 排行榜类别(对应数组下标)
	std::string strPubRecName;		// 排行榜对应公共区表名
	std::vector<std::string> vecMainPro;   	// 排序属性(可能不只一个)	
	int			nMainLimit;			// 主属性限制--大于等于该数才能上排行榜
	int			nCommandId;			// 响应的内部消息号(对应CommandDefine.h)

	//默认构造
	RankListProp(): 
		nRankType(RANK_LIST_NONE),
		strPubRecName(""),
		nMainLimit(1),
		nCommandId(0)
	{
		vecMainPro.clear();
	}
};

// 向公共数据区发的消息
enum RankListMessage
{
	/*!
	 * @brief	每条消息的前6个参数分别为1."domain" 2.string 共数据区名 3.int 消息号
	4.string 数据表名 5.string 玩家的uid 6.int 排序主属性的值.后面不同的数据可根据不同的排行榜自行添加

	1.玩家等级榜 param 7.玩家战斗力
	 */
	PUBDATA_MSG_RANKLIST_UPDATE,

	/*!
	 * @brief	查询等级排行榜
	 * @param	string 排行榜表名
	 * @param	
	 */
	PUBDATA_MSG_RANKLIST_QUERY,

	/*!
	 * @brief	删除数据
	 * @param	string 排行榜表名
	 * @param	
	 */
	PUBDATA_MSG_RANKLIST_DELETE,
};

#endif