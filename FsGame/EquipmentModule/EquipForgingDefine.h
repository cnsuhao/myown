#ifndef EQUIP_FORGING_DEFINE_H_
#define EQUIP_FORGING_DEFINE_H_

#include <string>
#include <vector>
#include "CommonModule\CommRuleDefine.h"

#define EQUIP_FORGING_CONFIG "ini/Item/Forging/EquipForging.xml"

// 客户端-服务器
enum C2S_Forging_SubMsg
{
	/*!
	* @brief	锻造装备
	* @param string 图纸
	*/
	C2S_FORGING_SUBMSG_EXECUTE = 1,
};

// 服务器-客户端
enum S2C_FORGING_SUBMSG
{
	/*!
	* @brief	锻造成功
	*/
    S2C_FORGING_SUBMSG_SUCCESS = 1,
};

// 品质权重
struct OutputColorWeight
{
	int output_color;	// 输出品质
	int lower_weight;
	int upper_weight;

	OutputColorWeight() :
		output_color(0),
		lower_weight(0),
		upper_weight(0)
	{}
};
typedef std::vector<OutputColorWeight> OutputColorWeightVec;

// 输出品质查找
struct OutputColorFinder
{
	int weight;
	OutputColorFinder(int _w) : weight(_w)
	{}

	bool operator () (const OutputColorWeight& cfg) const
	{
		return weight >= cfg.lower_weight && weight <= cfg.upper_weight;
	}
};

// 锻造规则
typedef std::vector<std::string> OutputList;
struct ForgingRule
{
	std::string drawing;		// 图纸
	std::string output_equip;			// 锻造成功产出的装备
	OutputColorWeightVec color_weight_list;
	int sum_weight;

	Consume_Vec items;	// 消耗材料
	Consume_Vec capitals;	// 消耗货币

	ForgingRule(const char *id) :
		drawing(id), 
		sum_weight(0)
	{
		items.clear();
		capitals.clear();
		output_equip.clear();
		color_weight_list.clear();
	}

	bool operator == (const ForgingRule &cfg) const
	{
		return strcmp(drawing.c_str(), cfg.drawing.c_str()) == 0;
	}
};
typedef std::vector<ForgingRule> ForgingRuleVec;

#endif
