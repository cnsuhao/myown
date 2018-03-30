// -------------------------------------------
// 文件名称： TaskStructDefine.h
// 文件说明： 任务数据结构定义
// 创建日期： 2018/02/23
// 创 建 人：  tzt   
// -------------------------------------------

#ifndef _TaskStructDefine_H_
#define _TaskStructDefine_H_

#include <map>
#include <vector>

typedef std::vector<int> NumList;	// 数字列表
typedef std::vector<std::string> StrList;	// 字符串列表

// 坐标信息
struct Position
{
	float x;	// x坐标
	float y;	// y坐标
	float z;	// z坐标
	float o;	// 朝向

	Position(float x, float y, float z, float o) : x(x), y(y), z(z), o(o) {}
	Position() : x(0.0f), y(0.0f), z(0.0f), o(0.0f){}
};
typedef std::vector<Position> PositionList;	// 坐标列表

// 键值对元素
struct CoupleElement
{
	std::string id;
	int num;

	CoupleElement(const char *_id) : num(0), id(_id){}

	bool operator == (const CoupleElement& element) const
	{
		return strcmp(id.c_str(), element.id.c_str()) == 0;
	}
};
typedef std::vector<CoupleElement> CoupleElementList;

// 任务配置信息
struct TaskConfig
{
	int type;
	std::string file_name;
	int limit_num;
	bool in_use;
	int first_task;

	TaskConfig(int _type) :
		type(_type),
		limit_num(0), 
		in_use(true), 
		first_task(0)
	{}

	bool operator == (const TaskConfig& config) const
	{
		return config.type == type;
	}

};
typedef std::vector<TaskConfig> TaskConfigList;

#endif