#ifndef _ApperanceDefine_h_
#define _ApperanceDefine_h_

#include <vector>
#define APPERANCE_PRI_FILE "ini/Item/Apperance/ApperancePri.xml"
#define APPERANCE_PKG_FILE "ini/Item/Apperance/ApperancePkg.xml"

// 外观来源
enum ApperanceSource
{
	APPERANCE_SOURCE_NONE = 0,

	APPERANCE_SOURCE_EQUIP,			// 装备
	APPERANCE_SOURCE_STRENGTHEN,	// 强化
	APPERANCE_SOURCE_FASHION,		// 时装

	APPERANCE_SOURCE_MAX,
};

// 外观类型
enum ApperanceType
{
	APPERANCE_TYPE_NONE = 0,

	APPERANCE_TYPE_WEAPON,		// 武器
	APPERANCE_TYPE_CLOTH,		// 衣服
	APPERANCE_TYPE_HAIR,		// 头
	APPERANCE_TYPE_WING,		// 翅膀

	APPERANCE_TYPE_MAX,
};

// 外观显示优先级
struct ApperancePri 
{
	int apperance_src;	// 来源
	int pri;			// 优先级

	ApperancePri(int src) :
		apperance_src(src),
		pri(0)
	{}

	bool operator == (const ApperancePri& cfg) const
	{
		return cfg.apperance_src == apperance_src;
	}
};
typedef std::vector<ApperancePri> ApperancePriVec;

// 外观属性包
struct ApperancePkg
{
	int apperance_id;
	int package;

	ApperancePkg(int id) :
		apperance_id(id),
		package(0)
	{}

	bool operator == (const ApperancePkg& cfg) const
	{
		return cfg.apperance_id == apperance_id;
	}
};
typedef std::vector<ApperancePkg> ApperancePkgVec;

#endif