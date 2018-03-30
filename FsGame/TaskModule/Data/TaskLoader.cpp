// -------------------------------------------
// 文件名称： TaskLoader.cpp
// 文件说明： 任务数据读取/查询
// 创建日期： 2018/02/24
// 创 建 人：  tzt 
// -------------------------------------------

#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include <algorithm>
#include "TaskLoader.h"
#include "../TaskDefine.h"
#include "TaskData_KillNpc.h"
#include "TaskData_Simple.h"

const char* const TASK_CONFIG_FILE = "ini/Task/TaskConfig.xml";

// 加载任务主体信息
bool TaskLoader::LoadRes(IKernel *pKernel)
{
    ReleaseRes();

	if (!LoadTaskConfig(pKernel))
	{
		return false;
	}
    
    return true;
}

// 释放任务数据
bool TaskLoader::ReleaseRes(const int task_type/* = 0*/)
{
	m_TaskConfig.clear();
	if (task_type >= TASK_TYPE_TOTAL || task_type <= TASK_TYPE_NONE)
	{
		TaskBaseDataMap::iterator start_it = m_TaskData.begin();
		TaskBaseDataMap::iterator end_it = m_TaskData.end();

		LoopBeginCheck(a);
		for (; start_it != end_it; ++start_it)
		{
			LoopDoCheck(a);
			TaskBaseDataS::iterator sub_start_it = start_it->second.begin();
			TaskBaseDataS::iterator sub_end_it = start_it->second.end();

			LoopBeginCheck(b);
			for (; sub_start_it != sub_end_it; ++sub_start_it)
			{
				LoopDoCheck(b);
				delete sub_start_it->second;
			}

			start_it->second.clear();
		}

		m_TaskData.clear();
	}
	else
	{
		// 任务信息
		TaskBaseDataMap::iterator iter = m_TaskData.find(task_type);
		if (iter != m_TaskData.end())
		{
			TaskBaseDataS::iterator sub_start_it = iter->second.begin();
			TaskBaseDataS::iterator sub_end_it = iter->second.end();

			LoopBeginCheck(c);
			for (; sub_start_it != sub_end_it; ++sub_start_it)
			{
				LoopDoCheck(c);
				delete sub_start_it->second;
			}

			iter->second.clear();
			m_TaskData.erase(iter);
		}		
	}

    return true;
}

// 重新加载任务数据
bool TaskLoader::ReloadRes(IKernel *pKernel, const int task_type /*= 0*/)
{
	ReleaseRes(task_type);
	LoadTaskConfig(pKernel, task_type);

	return true;
}

// 加载任务配置数据
bool TaskLoader::LoadTaskConfig(IKernel *pKernel, const int task_type/* = 0*/)
{
	std::string path = pKernel->GetResourcePath();
	path += TASK_CONFIG_FILE;

	CXmlFile xml(path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *sec_name = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec_name))
		{
			continue;
		}

		// 任务类型
		const int type = atoi(sec_name);
		if (type <= TASK_TYPE_NONE || type >= TASK_TYPE_TOTAL)
		{
			continue;
		}

		// 已有
		TaskConfig temp_data(type);
		TaskConfigList::const_iterator find_it = find(
			m_TaskConfig.begin(),
			m_TaskConfig.end(),
			temp_data);
		if (find_it != m_TaskConfig.end())
		{
			continue;
		}

		// 任务配置
		TaskConfig config(type);
		config.in_use = xml.ReadInteger(sec_name, "InUse", 0) == 0 ? true : false;
		config.file_name = xml.ReadString(sec_name, "FileName", "");
		const int limit_num = xml.ReadInteger(sec_name, "LimitNum", 0);
		config.limit_num = limit_num < 0 ? INT_MAX : limit_num;
		config.first_task = xml.ReadInteger(sec_name, "FirstTaskID", 0);
	
		m_TaskConfig.push_back(config);
	}

	// 全部加载
	if (task_type <= TASK_TYPE_NONE || task_type >= TASK_TYPE_TOTAL)
	{
		LoopBeginCheck(a);
		for (int type = TASK_TYPE_NONE; type < TASK_TYPE_TOTAL; ++type)
		{
			LoopDoCheck(a);
			if (!LoadTaskBaseData(pKernel, type, m_TaskData))
			{
				return false;
			}
		}
	}
	else
	{
		if (!LoadTaskBaseData(pKernel, task_type, m_TaskData))
		{
			return false;
		}
	}

	return true;
}

// 加载任务基础数据
bool TaskLoader::LoadTaskBaseData(IKernel *pKernel, const int task_type,
	TaskBaseDataMap& data_map)
{
	if (task_type <= TASK_TYPE_NONE 
		|| task_type >= TASK_TYPE_TOTAL)
	{
		return true;
	}

	TaskConfig temp_data(task_type);
	TaskConfigList::const_iterator find_it = find(
		m_TaskConfig.begin(),
		m_TaskConfig.end(),
		temp_data);
	if (find_it == m_TaskConfig.end())
	{
		return false;
	}

	// 不使用
	if (!find_it->in_use)
	{
		return true;
	}

	// 文件路径为空
	if (find_it->file_name.empty())
	{
		return false;
	}

	std::string path = pKernel->GetResourcePath();
	path.append("ini/Task/");
	path += find_it->file_name;

	CXmlFile xml(path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = path;
		err_msg.append(" does not exists!!");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}


	TaskBaseDataMap::iterator iter = m_TaskData.find(task_type);
	if (iter == m_TaskData.end())
	{
		TaskBaseDataS datas;
		m_TaskData.insert(TaskBaseDataMap::value_type(task_type, datas));
		iter = m_TaskData.find(task_type);
		if (iter == m_TaskData.end())
		{
			return false;
		}
	}
	
	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *sec_name = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec_name))
		{
			continue;
		}

		const int task_id = atoi(sec_name);
		const int rule = xml.ReadInteger(sec_name, "Rule", 0);

		// 创建任务数据类
		TaskBaseData *pData = CreateTaskDataClass(xml, sec_name, rule);
		if (NULL == pData)
		{
			continue;
		}

		// 加载基本任务信息
		pData->task_id = task_id;
		pData->type = task_type;
		pData->rule = rule;

		pData->accept_min_level = xml.ReadInteger(sec_name, "AcceptMinLevel", 0);
		const int max_level = xml.ReadInteger(sec_name, "AcceptMaxLevel", 0);
		pData->accept_max_level = max_level <= 0 ? INT_MAX : max_level;
		pData->count = xml.ReadInteger(sec_name, "Count", 0);
		pData->calculate_mode = xml.ReadInteger(sec_name, "CalculateMode", 0);
		xml.ReadList<int>(sec_name, "PrevTaskList", ",", &pData->prev_task_list);
		xml.ReadList<int>(sec_name, "PostTaskList", ",", &pData->post_task_list);
		pData->task_star = xml.ReadInteger(sec_name, "TaskStar", 0);
		pData->is_group = xml.ReadInteger(sec_name, "IsGroup", 0);
		pData->auto_pathfinding = xml.ReadInteger(sec_name, "AutoPathFinding", 0);
		pData->is_auto_submit = xml.ReadInteger(sec_name, "IsAutoSubmit", 0);
		pData->is_switch_scene = xml.ReadInteger(sec_name, "IsSwitchScene", 0);
		pData->is_switch_position = xml.ReadInteger(sec_name, "IsSwitchPosition", 0);
		pData->is_show_reward = xml.ReadInteger(sec_name, "IsShowReward", 0);
		xml.ReadList<int>(sec_name, "SceneList", ",", &pData->scene_list);
		pData->reward_id = xml.ReadInteger(sec_name, "RewardID", 0);
		ParsePosition(xml.ReadString(sec_name, "PositionList", ""), pData->posi_list);

		iter->second.insert(TaskBaseDataS::value_type(task_id, pData));
	}

	return true;
}

// 创建对应的任务数据类
TaskBaseData * TaskLoader::CreateTaskDataClass(const CXmlFile &xml, const char *sec_name,
	const int task_rule)
{
	if (task_rule <= TASK_RULE_NONE 
		|| task_rule >= TASK_RULE_TOTAL)
	{
		return NULL;
	}

	switch (task_rule)
	{
	case TASK_RULE_KILL_NPC:	// 击杀怪物
		{
			TaskData_KillNpc *pData = NEW TaskData_KillNpc();
			pData->kill_way = xml.ReadInteger(sec_name, "KillWay", 0);
			ParseCoupleStr(xml.ReadString(sec_name, "AddInfo", ""), pData->npc_list);

			return pData;
		}		
	case TASK_RULE_DIALOGUE:	// 对话
	case TASK_RULE_USE_ITEM:	// 物品使用
	case TASK_RULE_GATHER:		// 采集
	case TASK_RULE_PLAY_CG:		// 播放cg
		{
			TaskData_Simple *pData = NEW TaskData_Simple();
			pData->add_info = xml.ReadString(sec_name, "AddInfo", "");
			return pData;
		}
	default:
		return NEW TaskBaseData();
	}
}

// 分割并解析列表类型的键值对字符串
void TaskLoader::ParseCoupleStr(const char *soure_str, CoupleElementList& ele_list, 
	const char *del1 /*= ","*/, const char *del2 /*= ":"*/)
{
	CVarList result;
	util_split_string(result, soure_str, del1);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)result.GetCount(); ++i)
	{
		LoopDoCheck(a);
		CVarList final_result;
		util_split_string(final_result, result.StringVal(i), del2);
		if (final_result.GetCount() < 2)
		{
			continue;
		}

		CoupleElement element(final_result.StringVal(0));
		element.num = final_result.IntVal(1);
		ele_list.push_back(element);
	}
}

// 分割并解析键值对字符串
void TaskLoader::ParseCoupleStr(const char *soure_str, CoupleElement& element, 
	const char *del1 /*= ":"*/)
{
	CVarList result;
	util_split_string(result, soure_str, del1);
	if (result.GetCount() < 2)
	{
		return;
	}

	element.id = result.StringVal(0);
	element.num = result.IntVal(1);
}

// 分割并解析类型为整型单值字符串
void TaskLoader::ParseSinglelStr(const char *soure_str, NumList& ele_list, 
	const char *del1 /*= ","*/)
{
	CVarList result;
	util_split_string(result, soure_str, del1);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)result.GetCount(); ++i)
	{
		LoopDoCheck(a);
		ele_list.push_back(result.IntVal(i));
	}
}

// 分割并解析类型为字符的单值字符串
void TaskLoader::ParseSinglelStr(const char *soure_str, StrList& ele_list, 
	const char *del1 /*= ","*/)
{
	CVarList result;
	util_split_string(result, soure_str, del1);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)result.GetCount(); ++i)
	{
		LoopDoCheck(a);
		ele_list.push_back(result.StringVal(i));
	}
}

// 解析坐标信息
void TaskLoader::ParsePosition(const char *soure_str, PositionList& position_list,
	const char *del1/* = ";"*/, const char *del2/* = ","*/)
{
	CVarList result;
	util_split_string(result, soure_str, del1);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)result.GetCount(); ++i)
	{
		LoopDoCheck(a);
		CVarList final_result;
		util_split_string(final_result, result.StringVal(i), del2);
		if (final_result.GetCount() < 4)
		{
			continue;
		}

		Position position;
		position.x = final_result.FloatVal(0);
		position.y = final_result.FloatVal(1);
		position.z = final_result.FloatVal(2);
		position.o = final_result.FloatVal(3);
		position_list.push_back(position);
	}
}

// 获取一条任务记录
const TaskBaseData* TaskLoader::GetTaskBase(const int task_id) const
{
	TaskBaseDataMap::const_iterator start_it = m_TaskData.begin();
	TaskBaseDataMap::const_iterator end_it = m_TaskData.end();

	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		const TaskBaseDataS& data_map = start_it->second;
		TaskBaseDataS::const_iterator find_it = data_map.find(task_id);
		if (find_it != data_map.end())
		{
			return find_it->second;
		}
	}

    return NULL;
}

// 获取某种任务配置
const TaskConfig* TaskLoader::GetTaskConfig(const int task_type) const
{
	TaskConfig temp_data(task_type);
	TaskConfigList::const_iterator find_it = find(
		m_TaskConfig.begin(),
		m_TaskConfig.end(),
		temp_data);
	if (find_it == m_TaskConfig.end())
	{
		return NULL;
	}

	return &(*find_it);
}

// 获取某种任务类型的任务表
const TaskBaseDataS* TaskLoader::GetTaskTableByType(const int task_type) const
{
	TaskBaseDataMap::const_iterator find_it = m_TaskData.find(task_type);
	if (find_it == m_TaskData.end())
	{
		return NULL;
	}

	return &(find_it->second);
}
