#include "CommonModule\CommRuleDefine.h"
//--------------------------------------------------------------------
// 文件名:      EquipBaptiseDefine.h
// 内  容:      装备洗练定义
// 说  明:		
// 创建日期:    2018年03月12日
// 创建人:       tzt 
//--------------------------------------------------------------------

#ifndef _EquipBaptiseDefinee_h__
#define _EquipBaptiseDefinee_h__

const char* const BAPTISE_PROP_FILE = "ini/Item/Baptise/BaptiseProp.xml";
const char* const BP_RANGE_FILE = "ini/Item/Baptise/BaptisePropRange.xml";
const char* const BP_LIMIT_FILE = "ini/Item/Baptise/BaptisePropLimit.xml";
const char* const BAPTISE_CONSUME_FILE = "ini/Item/Baptise/BaptiseConsume.xml";

// 洗练类型
enum BaptiseType
{
	BAPTISE_TYPE_JUNIOR = 1,	// 初级（银两）
	BAPTISE_TYPE_MIDDLE,		// 中级（材料）
};

// 属性锁定
enum LockPropState
{
	LOCK_PROP_STATE_UNLOCK = 0,		// 解锁状态
	LOCK_PROP_STATE_LOCK,			// 锁定状态	
};

// 客户端-服务器二级消息
enum C2S_Baptise_SubMsg
{
	/*!
	* @brief	请求执行一次洗练
	* @param int 洗练类型BaptiseType
	* @param int 洗练部位EQUIP_POS
	*/
	C2S_BAPTISE_SUBMSG_EXCUTE = 1,

	/*!
	* @brief	请求保存洗练结果
	* @param int 洗练部位EQUIP_POS
	*/
	C2S_BAPTISE_SUBMSG_RESULT_SAVE,

	/*!
	* @brief	请求放弃洗练结果
	* @param int 洗练部位EQUIP_POS
	*/
	C2S_BAPTISE_SUBMSG_RESULT_GIVEUP,

	/*!
	* @brief	请求锁定属性
	* @param int 洗练部位EQUIP_POS
	* @param string 操作属性	
	*/
	C2S_BAPTISE_SUBMSG_LOCK_PROP,

	/*!
	* @brief	请求解锁属性
	* @param int 洗练部位EQUIP_POS
	* @param string 操作属性
	*/
	C2S_BAPTISE_SUBMSG_UNLOCK_PROP,
};


// 洗练属性
struct BaptiseProp 
{
	int index;	// 槽位
	int job;	// 职业
	String_Vec prop_list;	// 洗练属性列表

	BaptiseProp() : 
		index(0), 
		job(0)
	{
		prop_list.clear();
	}
};
typedef std::vector<BaptiseProp> BaptisePropVec;

// 洗练属性查询
struct BPFinder
{
	int _idx;
	int _job;
	BPFinder(int idx, int job) :
		_idx(idx),
		_job(job)
	{}

	bool operator () (const BaptiseProp& cfg) const
	{
		return cfg.index == _idx && cfg.index == _job;
	}
};

// 洗练属性取值权重
struct BPRangeWeight
{
	int min_value;
	int max_value;
	int lower_weight;
	int upper_weight;

	BPRangeWeight() :
		min_value(0),
		max_value(0),
		lower_weight(0),
		upper_weight(0)
	{}
};
typedef std::vector<BPRangeWeight> BPRWeightVec;

// 洗练属性取值查询
struct BPRangeWeightFinder
{
	int weight_value;
	BPRangeWeightFinder(int value) : weight_value(value)
	{}
	
	bool operator () (const BPRangeWeight& cfg) const
	{
		return weight_value >= cfg.lower_weight 
			&& weight_value <= cfg.upper_weight;
	}
};

// 洗练属性取值
struct BPRange
{
	int index;
	int job;
	std::string prop_name;
	int baptise_type;
	int sum_weight;
	BPRWeightVec bprw_list;

	BPRange() :
		index(0),
		job(0),
		baptise_type(0), 
		sum_weight(0)
	{
		bprw_list.clear();
	}
};
typedef std::vector<BPRange> BPRangeVec;

// 洗练属性取值
struct BPRangeFinder
{
	int _index;
	int _job;
	int bp_type;
	std::string _name;
	BPRangeFinder(int idx, int job, int type, std::string name) : 
		_index(idx), 
		_job(job), 
		bp_type(type), 
		_name(name)
	{}

	bool operator () (const BPRange& cfg) const
	{
		return cfg.index == _index 
			&& cfg.job == _job 
			&& bp_type == cfg.baptise_type 
			&& strcmp(_name.c_str(), cfg.prop_name.c_str()) == 0;
	}
};

// 洗练属性值上限
struct BPLimitValue
{
	int index;
	int job;
	std::string prop_name;
	int lower_str_lvl;
	int upper_str_lvl;
	int max_value;

	BPLimitValue() :
		index(0),
		job(0),
		lower_str_lvl(0),
		upper_str_lvl(0),
		max_value(0)
	{}
};
typedef std::vector<BPLimitValue> BPLimitValueVec;

// 洗练属性值上限查询
struct BPLimitValueFinder
{
	int _index;
	int _job;
	int _str_lvl;
	std::string _name;
	BPLimitValueFinder(int idx, int job, int str_lvl, std::string name) :
		_index(idx),
		_job(job),
		_str_lvl(str_lvl),
		_name(name)
	{}

	bool operator () (const BPLimitValue& cfg) const
	{
		return cfg.index == _index
			&& cfg.job == _job
			&& strcmp(_name.c_str(), cfg.prop_name.c_str()) == 0
			&& _str_lvl >= cfg.lower_str_lvl 
			&& _str_lvl <= cfg.upper_str_lvl;
	}
};

// 洗练消耗
struct BaptiseConsume
{
	int baptise_type;
	Consume_Vec capitals;
	Consume_Vec items;

	BaptiseConsume(int _bt) :
		baptise_type(_bt)
	{
		capitals.clear();
		items.clear();
	}

	bool operator == (const BaptiseConsume& cfg) const
	{
		return cfg.baptise_type == baptise_type;
	}
};
typedef std::vector<BaptiseConsume> BConsumeVec;

#endif
