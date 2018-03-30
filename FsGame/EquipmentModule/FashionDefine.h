//---------------------------------------------------------
//文件名:       FashionDefine.h
//内  容:       时装模块的定义
//说  明:               
//创建日期:      2017年02月21日
//创建人:         tongzt
//修改人:    
//---------------------------------------------------------

#ifndef FSGAME_FASHION_DEFINE_H_
#define FSGAME_FASHION_DEFINE_H_

#include <string>
#include <vector>
#include <map>
#include "CommonModule\CommRuleDefine.h"

#define FASHION_CONFIG_FILE	"ini/Item/Apperance/Fashion.xml"

// 时装配置
struct FashionCfg 
{
	FashionCfg(int _id) :
		fashion_id(_id),
		type(0),
		sex_limit(0),
		job_limit(0), 
		special_item(0)
		{
			unlock_fee.clear();
			unlock_items.clear();
		}

	int fashion_id;
	int type;
	int sex_limit;
	int job_limit;
	
	Consume_Vec unlock_fee;
	Consume_Vec unlock_items;
	int special_item;
	int pre_fashion;

	bool operator == (const FashionCfg& cfg) const
	{
		return cfg.fashion_id == fashion_id;
	}
};
typedef std::vector<FashionCfg> FashionCfgVec;

// 客户端->服务器二级消息
enum C2S_Fashion_SubMsg
{
	/*!
	* @brief	请求解锁
	* @param int 时装id
	*/
	C2S_FASHION_SUBMSG_UNLOCK = 1,

	/*!
	* @brief	请求穿时装
	* @param int 时装id
	*/
	C2S_FASHION_SUBMSG_PUTON,

	/*!
	* @brief	请求脱时装
	* @param int 时装id
	*/
	C2S_FASHION_SUBMSG_TAKEOFF,
};

// 服务器->客户端二级消息
enum S2C_Fashion_SubMsg
{
	/*!
	* @brief	时装时效结束
	* @param int 时装id
	*/
	S2C_FASHION_SUBMSG_VALID_TIME_END= 1,

	
};

#endif
