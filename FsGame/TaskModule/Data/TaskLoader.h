// -------------------------------------------
// 文件名称： TaskLoader.h
// 文件说明： 任务数据读取/查询
// 创建日期： 2018/02/23
// 创 建 人：  tzt
// -------------------------------------------

#ifndef _TaskLoader_H_
#define _TaskLoader_H_

#include "TaskBaseData.h"
#include "Fsgame/Define/header.h"

class CXmlFile;

typedef std::map<int, TaskBaseData *> TaskBaseDataS;	// key=任务ID value=任务基础信息
typedef std::map<int, TaskBaseDataS> TaskBaseDataMap;	// key=任务类型 value=该类型任务信息列表

/*
** 任务信息读取类
*/

class TaskLoader
{
public:
	static TaskLoader &instance()
    {
		static TaskLoader task_loader;
		return task_loader;
    }

public:
    // 加载全部任务数据
    bool LoadRes(IKernel *pKernel);

    // 释放任务数据
	bool ReleaseRes(const int task_type = 0);

	// 重新加载任务数据
	bool ReloadRes(IKernel *pKernel, const int task_type = 0);

private:
	// 加载任务配置数据
	bool LoadTaskConfig(IKernel *pKernel, const int task_type = 0);

	// 加载任务基础数据
	bool LoadTaskBaseData(IKernel *pKernel, const int task_type, 
		TaskBaseDataMap& data_map);

	// 创建对应的任务数据类
	TaskBaseData *CreateTaskDataClass(const CXmlFile &xml, const char *sec_name,
		const int task_rule);

public:
	// 分割并解析列表类型的键值对字符串
	void ParseCoupleStr(const char *soure_str, CoupleElementList& ele_list, 
		const char *del1 = ",", const char *del2 = ":");

	// 分割并解析键值对字符串
	void ParseCoupleStr(const char *soure_str, CoupleElement& element,
		const char *del1 = ":");

	// 分割并解析类型为整型单值字符串
	void ParseSinglelStr(const char *soure_str, NumList& ele_list,
		const char *del1 = ",");

	// 分割并解析类型为字符的单值字符串
	void ParseSinglelStr(const char *soure_str, StrList& ele_list,
		const char *del1 = ",");

	// 解析坐标信息
	void ParsePosition(const char *soure_str, PositionList& position_list,
		const char *del1 = ";", const char *del2 = ",");

    // 获取一条任务记录
	const TaskBaseData* GetTaskBase(const int task_id) const;

	// 获取某种任务配置
	const TaskConfig* GetTaskConfig(const int task_type) const;

	// 获取某种任务类型的任务表
	const TaskBaseDataS* GetTaskTableByType(const int task_type) const;

private:
    // 禁用默认构造函数
	TaskLoader() {}
    // 禁用默认析构函数
	~TaskLoader() {}
    // 禁用拷贝构造函数
	TaskLoader(const TaskLoader &) {}
    // 禁用赋值构造函数
	TaskLoader &operator= (const TaskLoader &) {}

private:
	TaskBaseDataMap m_TaskData;
	TaskConfigList m_TaskConfig;
};

#endif
