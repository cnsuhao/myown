#ifndef EquipStrengthenDefine_h__
#define EquipStrengthenDefine_h__
#include "FsGame/CommonModule/CommRuleDefine.h"

//装备强化操作二级消息分类
enum EQUIP_STRENGTHEN_CLIENT_MSG
{
	EQUIP_STRENGTHEN_ONCE = 0,      //一次强化操作
	EQUIP_STRENGTHEN_AUTO = 1,      //一键强化到操作
};

// 强化消耗
struct StrenthenConsume
{
	int level;	// 强化等级
	Consume_Vec items;	// 物品
	Consume_Vec capitals;	//  货币

	StrenthenConsume(int _lvl) : level(_lvl)
	{
		items.clear();
		capitals.clear();
	}

	bool operator == (const StrenthenConsume& cfg) const
	{
		return cfg.level == level;
	}
};
typedef std::vector<StrenthenConsume> StrenthenConsumeVec;

// 强化等级限制
struct StrengthenLimit
{
	int level;	// 装备等级
	std::vector<int> color_to_limit;

	StrengthenLimit(int _lvl) : level(_lvl)
	{
		color_to_limit.clear();
	}

	bool operator == (const StrengthenLimit& cfg) const
	{
		return cfg.level == level;
	}
};
typedef std::vector<StrengthenLimit> StrengthenLimitVec;

// 强化套装配置
struct StrengthenSuit
{
	int lower_str_lvl;
	int upper_str_lvl;
	int active_str_pkg;

	StrengthenSuit() :
		lower_str_lvl(0),
		upper_str_lvl(0),
		active_str_pkg(0)
	{
	}
};
typedef std::vector<StrengthenSuit> StrengthenSuitList;
// 套装查找
struct StrengthenSuitFinder
{
	StrengthenSuitFinder(int str_lvl) :min_str_lvl(str_lvl)
	{
	};
	bool operator ()(const StrengthenSuit &data) const
	{
		return (min_str_lvl >= data.lower_str_lvl
			&& min_str_lvl <= data.upper_str_lvl);
	}

	int min_str_lvl;
};

// 强化外观
struct StrengthenApperance
{
	std::string equip_id;	// 装备id
	int lower_lvl;
	int uppper_lvl;
	int str_apperance;

	StrengthenApperance() :
		lower_lvl(0),
		uppper_lvl(0), 
		str_apperance(0)
	{}
};
typedef std::vector<StrengthenApperance> StrengthenApperanceVec;
// 外观查找
struct StrengthenApperanceFinder
{
	StrengthenApperanceFinder(int lvl, std::string id) : 
	str_lvl(lvl), 
	equip_id(id)
	{
	};
	bool operator ()(const StrengthenApperance &data) const
	{
		return (str_lvl >= data.lower_lvl
			&& str_lvl <= data.uppper_lvl)
			&& strcmp(equip_id.c_str(), data.equip_id.c_str());
	}

	int str_lvl;
	std::string equip_id;
};

#endif // EquipStrengthenDefine_h__