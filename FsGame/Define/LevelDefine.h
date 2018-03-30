#ifndef _Level_Define_H_
#define _Level_Define_H_

#define PUB_WORLD_LEVEL	 "pub_world_level"		 // 服务器世界等级

// 子消息
enum WorldLevelC2SMsg
{
	WLM_C2S_QUERY_ADD_EXP_DATA = 0,		// 查询世界等级附加经验数据
};

enum WorldLevelS2CMsg
{
	/*!
	* @brief	回复世界等级附加经验数据
	* @param	int 	世界等级
	*/
	WLM_S2C_REV_ADD_EXP_DATA = 0,
};
#endif