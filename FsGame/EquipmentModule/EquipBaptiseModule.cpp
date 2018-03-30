//--------------------------------------------------------------------
// 文件名:      EquipBaptiseModule.cpp
// 内  容:      洗练模块
// 说  明:		
// 创建日期:    2018年03月12日
// 创建人:      tzt        
//--------------------------------------------------------------------

#include "EquipBaptiseModule.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/ClientCustomDefine.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "FsGame/Define/GameDefine.h"
#include "utils/json.h"
#include "CommonModule/CommRuleModule.h"
#include <algorithm>
#include "utils/util.h"
#include "EquipDefine.h"
#include "EquipStrengthenModule.h"
#include "Define/ContainerDefine.h"
#include "Define/CommandDefine.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"
#include "CommonModule/EnvirValueModule.h"

EquipBaptiseModule *EquipBaptiseModule::m_pEquipBaptiseModule = NULL;


//初始化
bool EquipBaptiseModule::Init(IKernel* pKernel)
{
	m_pEquipBaptiseModule = this;

	assert(m_pEquipBaptiseModule != NULL);

	// 上线
	pKernel->AddEventCallback("player", "OnRecover", EquipBaptiseModule::OnPlayerRecover, -1);
	// 客户端准备就绪
	pKernel->AddEventCallback("player", "OnReady", EquipBaptiseModule::OnPlayerReady);
	// 洗练客户端消息
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_BAPTISE, OnCustomMsg);

	if (!LoadRes(pKernel))
	{
		Assert(false);
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("EquipBaptiseModule", EquipBaptiseModule::ReloadEquipConfig);
#endif		

    return true;
}

//释放
bool EquipBaptiseModule::Shut(IKernel* pKernel)
{
	ReleaseRes(pKernel);

    return true;
}


// 配置加载
bool EquipBaptiseModule::LoadRes(IKernel* pKernel)
{
	ReleaseRes(pKernel);

	if (!LoadBPRes(pKernel))
	{
		return false;
	}

	if (!LoadBPRangeRes(pKernel))
	{
		return false;
	}

	if (!LoadBPLimitRes(pKernel))
	{
		return false;
	}

	if (!LoadBConsumeRes(pKernel))
	{
		return false;
	}

	return true;
}

bool EquipBaptiseModule::ReleaseRes(IKernel* pKernel)
{
	m_BaptisePropVec.clear();
	m_BPRangeVec.clear();
	m_BPLimitValueVec.clear();
	m_BConsumeVec.clear();

	return true;
}

// 加载洗练属性配置
bool EquipBaptiseModule::LoadBPRes(IKernel* pKernel)
{
	std::string path = pKernel->GetResourcePath();
	path += BAPTISE_PROP_FILE;

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

		const int index = xml.ReadInteger(sec_name, "Index", 0);
		const int job = xml.ReadInteger(sec_name, "Job", 0);
		// 合法检查
		if (!CommRuleModule::m_pThis->IsValidEquipPos(index) 
			|| !CommRuleModule::m_pThis->IsValidJob(job))
		{
			continue;
		}

		BaptiseProp config;
		config.index = index;
		config.job = job;
		xml.ReadList<std::string>(sec_name, "PropertyList", ",", &config.prop_list);

		m_BaptisePropVec.push_back(config);
	}

	return true;
}

// 加载洗练属性取值配置
bool EquipBaptiseModule::LoadBPRangeRes(IKernel* pKernel)
{
	std::string path = pKernel->GetResourcePath();
	path += BP_RANGE_FILE;

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

		const int index = xml.ReadInteger(sec_name, "Index", 0);
		const int job = xml.ReadInteger(sec_name, "Job", 0);
		// 合法检查
		if (!CommRuleModule::m_pThis->IsValidEquipPos(index)
			|| !CommRuleModule::m_pThis->IsValidJob(job))
		{
			continue;
		}

		BPRange config;
		config.index = index;
		config.job = job;
		config.baptise_type = xml.ReadInteger(sec_name, "BaptiseType", 0);
		config.prop_name = xml.ReadString(sec_name, "Property", "");

		String_Vec range_str_list;
		xml.ReadList<std::string>(sec_name, "RangeWithWeight", ";", &range_str_list);
		
		int sum_weight = 0;
		LoopBeginCheck(b);
		for (int idx = 0; idx < (int)range_str_list.size(); ++idx)
		{
			LoopDoCheck(b);
			BPRangeWeight range_cfg;
			CVarList result;
			util_split_string(result, range_str_list[idx], ",");
			range_cfg.min_value = result.IntVal(0);
			range_cfg.max_value = result.IntVal(1);
			int weight = result.IntVal(2);
			range_cfg.lower_weight = sum_weight + 1;			
			sum_weight += weight;
			range_cfg.upper_weight = sum_weight;
			config.bprw_list.push_back(range_cfg);
		}

		config.sum_weight = sum_weight;

		m_BPRangeVec.push_back(config);
	}

	return true;
}

// 加载洗练属性上限配置
bool EquipBaptiseModule::LoadBPLimitRes(IKernel* pKernel)
{
	std::string path = pKernel->GetResourcePath();
	path += BP_LIMIT_FILE;

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

		const int index = xml.ReadInteger(sec_name, "Index", 0);
		const int job = xml.ReadInteger(sec_name, "Job", 0);
		// 合法检查
		if (!CommRuleModule::m_pThis->IsValidEquipPos(index)
			|| !CommRuleModule::m_pThis->IsValidJob(job))
		{
			continue;
		}

		BPLimitValue config;
		config.index = index;
		config.job = job;
		config.prop_name = xml.ReadString(sec_name, "Property", "");
		config.lower_str_lvl = xml.ReadInteger(sec_name, "LowerStrLvl", 0);
		config.upper_str_lvl = xml.ReadInteger(sec_name, "UpperStrLvl", 0);
		config.max_value = xml.ReadInteger(sec_name, "MaxValue", 0);

		 m_BPLimitValueVec.push_back(config);
	}

	return true;
}

// 加载洗练消耗配置
bool EquipBaptiseModule::LoadBConsumeRes(IKernel* pKernel)
{
	std::string path = pKernel->GetResourcePath();
	path += BAPTISE_CONSUME_FILE;

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

		BaptiseConsume config(xml.ReadInteger(sec_name, "BaptiseType", 0));

		// 货币
		CommRuleModule::ParseConsumeVec(xml.ReadString(sec_name, "Capitals", ""), config.capitals);
		CommRuleModule::ParseConsumeVec(xml.ReadString(sec_name, "Items", ""), config.items);

		m_BConsumeVec.push_back(config);
	}

	return true;
}

// 取得洗练属性表
const String_Vec * EquipBaptiseModule::GetPropList(const int index, const int job)
{
	BaptisePropVec::const_iterator find_it = find_if(
		m_BaptisePropVec.begin(),
		m_BaptisePropVec.end(),
		BPFinder(index, job));
	if (find_it == m_BaptisePropVec.end())
	{
		return NULL;
	}

	return &(find_it->prop_list);
}

// 取得洗练属性取值
const int EquipBaptiseModule::GetPropValue(const int index, const int job, 
	const int baptise_type, const char *prop_name)
{
	BPRangeVec::const_iterator find_it = find_if(
		m_BPRangeVec.begin(),
		m_BPRangeVec.end(),
		BPRangeFinder(index, job, baptise_type, prop_name));
	if (find_it == m_BPRangeVec.end())
	{
		return 0;
	}

	// 随机权重
	const int random_weight = util_random_int(find_it->sum_weight) + 1;

	BPRWeightVec::const_iterator find_it2 = find_if(
		find_it->bprw_list.begin(),
		find_it->bprw_list.end(),
		BPRangeWeightFinder(random_weight));
	if (find_it2 == find_it->bprw_list.end())
	{
		return 0;
	}

	const int min = find_it2->min_value;
	int max = find_it2->max_value;
	if (min < 0)
	{
		max = max - min + 1;
		if (max < 0)
		{
			return 0;
		}

		int random_value = util_range_random_int(0, max);
		return random_value + min;
	}
	else
	{
		return util_range_random_int(min, max);
	}
}

// 取得洗练属性上限值
const int EquipBaptiseModule::GetPropLimitValue(const int index, const int job, 
	const int str_lvl, const char *prop_name)
{
	BPLimitValueVec::const_iterator find_it = find_if(
		m_BPLimitValueVec.begin(),
		m_BPLimitValueVec.end(),
		BPLimitValueFinder(index, job, str_lvl, prop_name));

	if (find_it == m_BPLimitValueVec.end())
	{
		return 0;
	}

	return find_it->max_value;
}

// 取得洗练消耗
const BaptiseConsume * EquipBaptiseModule::GetBaptiseConsume(const int baptise_type)
{
	BaptiseConsume temp(baptise_type);
	BConsumeVec::const_iterator find_it = find(
		m_BConsumeVec.begin(),
		m_BConsumeVec.end(),
		temp);
	if (find_it == m_BConsumeVec.end())
	{
		return NULL;
	}

	return &(*find_it);
}

// 拼接json字符串
void EquipBaptiseModule::ConvertToJsonStr(const String_Vec& str_list, const Integer_Vec int_list,
	std::string& json_str)
{
	json_str.clear();
	const int size = (int)str_list.size();
	json_str += "{";
	for (int i = 0; i < size; ++i)
	{
		int value = 0;
		if (i < (int)int_list.size())
		{
			value = int_list[i];
		}

		json_str = json_str + "\"" + str_list[i] + "\""+": " + StringUtil::IntAsString(value);
		if (i < size - 1)
		{
			json_str += ",";
		}		
	}

	json_str += "}";
}

// 纠正洗练值
void EquipBaptiseModule::CorrectBaptiseValue(IKernel* pKernel, const PERSISTID& self, 
	const int equip_pos, const char *prop_name, const int str_lvl, int& final_value)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	// 当前洗练数据
	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, equip_pos);
	if (bap_row < 0)
	{
		return;
	}

	const char *baptise_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_VALUE_INFO);
	if (StringUtil::CharIsNull(baptise_info))
	{
		return;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root_baptise;

	if (!reader.parse(baptise_info, root_baptise))
	{
		return;
	}

	if (!root_baptise[prop_name].isInt())
	{
		return;
	}

	// 当前总值
	const int cur_total_value = root_baptise[prop_name].asInt();

	// 上限值
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);
	const int max_value = GetPropLimitValue(equip_pos, job, str_lvl, prop_name);

	int sum_value = cur_total_value + final_value;
	if (final_value < 0)
	{

		final_value = sum_value < 0 ? (0 - cur_total_value) : final_value;
	}
	else
	{
		sum_value = sum_value > max_value ? max_value : sum_value;
		final_value = sum_value - cur_total_value;
	}		
}

// 指定部位洗练属性是否全部达到上限
bool EquipBaptiseModule::IsAllPropMaxed(IKernel* pKernel, const PERSISTID& self, 
	const int equip_pos)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return false;
	}

	// 当前洗练数据
	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, equip_pos);
	if (bap_row < 0)
	{
		return false;
	}

	const char *baptise_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_VALUE_INFO);
	if (StringUtil::CharIsNull(baptise_info))
	{
		return false;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root_baptise;

	if (!reader.parse(baptise_info, root_baptise))
	{
		return false;
	}

	// 取出可洗练的属性列表
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);	// 职业
	const String_Vec *prop_list = GetPropList(equip_pos, job);
	if (NULL == prop_list)
	{
		// 无可洗练属性
		return false;
	}

	String_Vec::const_iterator start_it = prop_list->begin();
	String_Vec::const_iterator end_it = prop_list->end();

	const int str_lvl = EquipStrengthenModule::m_pInstance->GetStrLvlByPos(pKernel, self, equip_pos);
	bool is_all_maxed = true;
	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		const char *prop_name = start_it->c_str();

		if (!root_baptise[prop_name].isInt())
		{
			continue;
		}

		// 当前值
		const int cur_value = root_baptise[prop_name].asInt();

		// 上限值
		const int max_value = GetPropLimitValue(equip_pos, job, str_lvl, prop_name);
		if (cur_value < max_value)
		{
			is_all_maxed = false;
			break;
		}		
	}

	return is_all_maxed;
}

// 执行一次洗练
void EquipBaptiseModule::ExecuteBaptise(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equipbox))
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	// 洗练结果表
	IRecord *pBpResultRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_RESULT_REC);
	if (NULL == pBpResultRec)
	{
		return;
	}

	// 功能是否已开启
	if (!ActivateFunctionModule::m_pInstance->CheckActivateFunction(pKernel, self, AFM_BAPTISE))
	{
		return;
	}

	const int baptise_type = args.IntVal(2);	// 洗练类型
	const int baptise_pos = args.IntVal(3);	// 洗练部位
	if (!CommRuleModule::m_pThis->IsValidEquipPos(baptise_pos))
	{
		return;
	}

	// 取出洗练部位的装备
	PERSISTID equip = pKernel->GetItem(equipbox, baptise_pos);
	IGameObj *pEquip = pKernel->GetGameObj(equip);
	if (NULL == pEquip)
	{
		return;// 无装备
	}

	// 洗练部位强化等级限制
	const int str_lvl = EquipStrengthenModule::m_pInstance->GetStrLvlByPos(pKernel, self, baptise_pos);
	if (str_lvl < EnvirValueModule::EnvirQueryInt(ENV_VALUE_BAPTISE_LIMIT_STR_LVL))
	{
		return;
	}

	// 洗练部位装备品质限制
	if (pEquip->QueryInt(FIELD_PROP_COLOR_LEVEL) < EnvirValueModule::EnvirQueryInt(ENV_VALUE_BAPTISE_LIMIT_COLOR))
	{
		return;
	}

	// 所有洗练属性已达上限
	if (IsAllPropMaxed(pKernel, self, baptise_pos))
	{
		return;
	}

	// 洗练消耗
	const BaptiseConsume *consume = GetBaptiseConsume(baptise_type);
	if (NULL == consume)
	{
		return;
	}

	// 材料是否足够
	if (!CommRuleModule::CanDecItems(pKernel, self, consume->items))
	{
		return;
	}

	// 货币是否足够
	if (!CommRuleModule::CanDecCapitals(pKernel, self, consume->capitals))
	{
		return;
	}

	// 扣材料
	if (!CommRuleModule::ConsumeItems(pKernel, self, consume->items, FUNCTION_EVENT_ID_BAPTISE))
	{
		return;
	}

	// 扣货币
	if (!CommRuleModule::ConsumeCapitals(pKernel, self, consume->capitals, FUNCTION_EVENT_ID_BAPTISE))
	{
		return;
	}

	// 对应部位的洗练数据
	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, baptise_pos);
	if (bap_row < 0)
	{
		return;
	}

	// 属性锁定状态
	const char *lock_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO);
	if (StringUtil::CharIsNull(lock_info))
	{
		return;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root_lock;
	if (!reader.parse(lock_info, root_lock))
	{
		return;
	}

	// 开始洗练
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);	// 职业

	// 取出可洗练的属性列表
	const String_Vec *prop_list = GetPropList(baptise_pos, job);
	if (NULL == prop_list)
	{
		// 无可洗练属性
		return;
	}

	// 逐个属性洗练
	String_Vec::const_iterator start_it = prop_list->begin();
	String_Vec::const_iterator end_it = prop_list->end();

	String_Vec prop_name_list;
	Integer_Vec prop_value_list;
	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		const char *prop_name = start_it->c_str();
		if (!pSelf->FindAttr(prop_name))
		{
			Assert(false);
			::extend_warning(LOG_ERROR,"Baptise Property[%s] incorrect", prop_name);
			continue;
		}

		if (!root_lock[prop_name].isInt())
		{
			continue;
		}
		
		// 已锁定，不可洗练
		const int cur_lock_state = root_lock[prop_name].asInt();
		if (LOCK_PROP_STATE_LOCK == cur_lock_state)
		{
			continue;
		}

		// 本次洗练的值
		int baptise_value = GetPropValue(baptise_pos, job, baptise_type, prop_name);

		// 洗练值纠正（不得超过该属性上限或出现<0）
		CorrectBaptiseValue(pKernel, self, baptise_pos, prop_name, str_lvl, baptise_value);

		prop_name_list.push_back(prop_name);
		prop_value_list.push_back(baptise_value);
	}

	// 转换成json格式字符串
	std::string baptise_result = "";
	ConvertToJsonStr(prop_name_list, prop_value_list, baptise_result);

	// 洗练结果保存在临时洗练表
	CVarList row_value;
	row_value << baptise_pos
			  << baptise_result;
	
	// 有记录则删除
	const int exist_row = pBpResultRec->FindInt(COLUMN_BAPTISE_RESULT_REC_POS, baptise_pos);
	if (exist_row >= 0)
	{
		pBpResultRec->RemoveRow(exist_row);
	}

	// 暂时保存洗练结果
	pBpResultRec->AddRowValue(-1, row_value);
}

// 保存洗练结果
void EquipBaptiseModule::SaveResult(IKernel* pKernel, const PERSISTID& self, 
	const int equip_pos)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	// 洗练结果表
	IRecord *pBpResultRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_RESULT_REC);
	if (NULL == pBpResultRec)
	{
		return;
	}


	PERSISTID equip_box = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equip_box))
	{
		return;
	}

	if (!CommRuleModule::m_pThis->IsValidEquipPos(equip_pos))
	{
		return;
	}

	// 取得选中的装备
	PERSISTID equip = pKernel->GetItem(equip_box, equip_pos);
	if (!pKernel->Exists(equip))
	{
		return;
	}

	// 是否有洗练数据
	const int rst_row = pBpResultRec->FindInt(COLUMN_BAPTISE_RESULT_REC_POS, equip_pos);
	if (rst_row < 0)
	{
		return;
	}

	// 要保存的洗练数据
	const char *need_save_info = pBpResultRec->QueryString(rst_row, COLUMN_BAPTISE_RESULT_REC_RESULT_INFO);
	if (StringUtil::CharIsNull(need_save_info))
	{
		return;
	}

	// 当前洗练数据
	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, equip_pos);
	if (bap_row < 0)
	{
		return;
	}

	const char *baptise_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_VALUE_INFO);
	if (StringUtil::CharIsNull(baptise_info))
	{
		return;
	}

	// 属性锁定状态
	const char *lock_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO);
	if (StringUtil::CharIsNull(lock_info))
	{
		return;
	}

	// 取得可洗练属性
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);
	const String_Vec *prop_name_list = GetPropList(equip_pos, job);
	if (NULL == prop_name_list)
	{
		return;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root_result;
	Json::Value root_baptise;
	Json::Value root_lock;

	if (!reader.parse(need_save_info, root_result) 
		|| !reader.parse(baptise_info, root_baptise)
		|| !reader.parse(lock_info, root_lock))
	{
		return;
	}

	String_Vec::const_iterator start_it = prop_name_list->begin();
	String_Vec::const_iterator end_it = prop_name_list->end();

	String_Vec update_prop_list;
	Integer_Vec update_value_list;
	LoopBeginCheck(a);
	for (; start_it != end_it; ++start_it)
	{
		LoopDoCheck(a);
		const char *prop_name = start_it->c_str();
		if (!pSelf->FindAttr(prop_name))
		{
			Assert(false);
			::extend_warning(LOG_ERROR, "Baptise Property[%s] incorrect", prop_name);
			continue;
		}

		// 锁定的属性不操作
		if (!root_lock[prop_name].isInt())
		{
			continue;
		}

		// 已锁定，不可操作
		const int cur_lock_state = root_lock[prop_name].asInt();
		if (LOCK_PROP_STATE_LOCK == cur_lock_state)
		{
			continue;
		}

		// 本次洗练的值
		if (root_result[prop_name].isInt() && root_baptise[prop_name].isInt())
		{
			const int incc_value = root_result[prop_name].asInt();
			int baptise_value = root_baptise[prop_name].asInt();
			baptise_value += incc_value;
			root_baptise[prop_name] = baptise_value;

			update_prop_list.push_back(prop_name);
			update_value_list.push_back(incc_value);
		}				
	}

	// 最终结果保存
	std::string final_value_str = root_baptise.toStyledString();
	pBpRec->SetString(bap_row, COLUMN_BAPTISE_REC_VALUE_INFO, final_value_str.c_str());
	pBpResultRec->RemoveRow(rst_row);

	// 属性包刷新
	std::string update_prop = "";
	ConvertToJsonStr(update_prop_list, update_value_list, update_prop);
	pKernel->Command(equip, self, CVarList() << COMMAND_EQUIP_BAPTISE_PKG << update_prop);
}

// 放弃洗练结果
void EquipBaptiseModule::GiveupResult(IKernel* pKernel, const PERSISTID& self, 
	const int equip_pos)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练结果表
	IRecord *pBpResultRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_RESULT_REC);
	if (NULL == pBpResultRec)
	{
		return;
	}

	// 是否有洗练数据
	const int rst_row = pBpResultRec->FindInt(COLUMN_BAPTISE_RESULT_REC_POS, equip_pos);
	if (rst_row < 0)
	{
		return;
	}

	pBpResultRec->RemoveRow(rst_row);
}

// 锁定属性
void EquipBaptiseModule::LockProp(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	const int baptise_pos = args.IntVal(2);
	const char *prop_name = args.StringVal(3);

	if (!CommRuleModule::m_pThis->IsValidEquipPos(baptise_pos))
	{
		return;
	}

	if (StringUtil::CharIsNull(prop_name))
	{
		return;
	}

	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, baptise_pos);
	if (bap_row < 0)
	{
		return;
	}

	const char *lock_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO);
	if (StringUtil::CharIsNull(lock_info))
	{
		return;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(lock_info, root))
	{
		return;
	}

	if (!root[prop_name].isInt())
	{
		return;
	}

	const int cur_lock_state = root[prop_name].asInt();

	// 已锁定，无需操作
	if (LOCK_PROP_STATE_LOCK == cur_lock_state)
	{
		return;
	}

	Consume_Vec lock_capital;
	Consume capital;
	capital.id = StringUtil::IntAsString(EnvirValueModule::EnvirQueryInt(ENV_VALUE_BAPTISE_PROP_LOCK_CAPITAL));
	capital.num = EnvirValueModule::EnvirQueryInt(ENV_VALUE_BAPTISE_PROP_LOCK_CAPITAL_NUM);
	lock_capital.push_back(capital);

	// 锁定费用是否足够
	if (!CommRuleModule::m_pThis->CanDecCapitals(pKernel, self, lock_capital))
	{
		return;
	}

	// 扣除费用
	if (!CommRuleModule::m_pThis->ConsumeCapitals(pKernel, self, lock_capital, FUNCTION_EVENT_ID_BAPTISE_LOCK))
	{
		return;
	}

	// 锁定
	root[prop_name] = LOCK_PROP_STATE_LOCK;
	std::string new_lock_info = root.toStyledString();
	pBpRec->SetString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO, new_lock_info.c_str());
}

// 解锁属性
void EquipBaptiseModule::UnLockProp(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	const int baptise_pos = args.IntVal(2);
	const char *prop_name = args.StringVal(3);

	if (!CommRuleModule::m_pThis->IsValidEquipPos(baptise_pos))
	{
		return;
	}

	if (StringUtil::CharIsNull(prop_name))
	{
		return;
	}

	const int bap_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, baptise_pos);
	if (bap_row < 0)
	{
		return;
	}

	const char *lock_info = pBpRec->QueryString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO);
	if (StringUtil::CharIsNull(lock_info))
	{
		return;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(lock_info, root))
	{
		return;
	}

	if (!root[prop_name].isInt())
	{
		return;
	}

	const int cur_lock_state = root[prop_name].asInt();

	// 已是解锁状态，不可解锁
	if (LOCK_PROP_STATE_UNLOCK == cur_lock_state)
	{
		return;
	}

	// 解锁
	root[prop_name] = LOCK_PROP_STATE_UNLOCK;
	std::string new_lock_info = root.toStyledString();
	pBpRec->SetString(bap_row, COLUMN_BAPTISE_REC_LOCK_INFO, new_lock_info.c_str());
}

// 初始化每个装备部位的洗练信息（上线调用）
void EquipBaptiseModule::InitBaptise(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 洗练表
	IRecord *pBpRec = pSelf->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL == pBpRec)
	{
		return;
	}

	// 玩家职业
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);

	// 按槽位检查	
	LoopBeginCheck(a);
	for (int pos = EQUIP_POS_STR_MIN; pos <= EQUIP_POS_STR_MAX; ++pos)
	{
		LoopDoCheck(a);
		// 取洗练属性列表
		const String_Vec *prop_list = GetPropList(pos, job);
		if (NULL == prop_list)
		{
			continue;
		}

		const int exist_row = pBpRec->FindInt(COLUMN_BAPTISE_REC_POS, pos);
		if (exist_row >= 0)
		{	
			continue;
		}

		std::string value_info = "";
		Integer_Vec value_list;
		ConvertToJsonStr(*prop_list, value_list, value_info);
		if (value_info.empty())
		{
			continue;
		}

		CVarList row_value;
		row_value << pos
				  << value_info
				  << value_info;
		pBpRec->AddRowValue(-1, row_value);
	}
}

// 取得玩家的槽位的洗练属性列表
void EquipBaptiseModule::GetPropNameList(IKernel* pKernel, const PERSISTID& self, 
	const int equip_pos, String_Vec& prop_name_list)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 玩家职业
	const int job = pSelf->QueryInt(FIELD_PROP_JOB);

	const String_Vec *prop_list = GetPropList(equip_pos, job);
	if (NULL == prop_list)
	{
		return;
	}

	prop_name_list = *prop_list;
}

// 玩家上线
int EquipBaptiseModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	// 初始化洗练属性
	m_pEquipBaptiseModule->InitBaptise(pKernel, self);
	return 0;
}

// 玩家准备就绪
int EquipBaptiseModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	
	return 0;
}

// 处理客户端发送消息
int EquipBaptiseModule::OnCustomMsg(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	const int sub_msg = args.IntVal(1);
	switch (sub_msg)
	{
	case C2S_BAPTISE_SUBMSG_EXCUTE:	// 执行一次洗练
		m_pEquipBaptiseModule->ExecuteBaptise(pKernel, self, args);
		break;

	case C2S_BAPTISE_SUBMSG_RESULT_SAVE:	// 请求保存洗练结果
		m_pEquipBaptiseModule->SaveResult(pKernel, self, args.IntVal(2));
		break;

	case C2S_BAPTISE_SUBMSG_RESULT_GIVEUP:	// 请求放弃洗练结果
		m_pEquipBaptiseModule->GiveupResult(pKernel, self, args.IntVal(2));
		break;

	case C2S_BAPTISE_SUBMSG_LOCK_PROP:	// 请求锁定属性
		m_pEquipBaptiseModule->LockProp(pKernel, self, args);
		break;

	case C2S_BAPTISE_SUBMSG_UNLOCK_PROP:	// 请求解锁属性
		m_pEquipBaptiseModule->UnLockProp(pKernel, self, args);
		break;
	default:
		break;
	}

	return 0;
}

// 重新加载装备配置
void EquipBaptiseModule::ReloadEquipConfig(IKernel* pKernel)
{
	EquipBaptiseModule::m_pEquipBaptiseModule->LoadRes(pKernel);
}
