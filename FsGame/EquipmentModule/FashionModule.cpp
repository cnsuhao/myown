//---------------------------------------------------------
//文件名:       FashionModule.h
//内  容:       时装模块
//说  明:       
//创建日期:      2017年02月21日
//创建人:        tongzt 
//修改人:         
//---------------------------------------------------------

#include "FashionModule.h"

#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"

#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/PropRefreshModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "FsGame/Define/GameDefine.h"
#include "FsGame/CommonModule/FunctionEventModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/ItemTypeDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "CommonModule/EnvirValueModule.h"
#include "Define/GuildDefine.h"
#include "CommonModule/CommRuleModule.h"
#include "EquipmentModule.h"
#include "ApperanceModule.h"
#include "SystemFunctionModule/WingModule.h"

FashionModule *FashionModule::m_pFashionModule = NULL;

//　重新载入配置
int nx_reload_fashion_config(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL != FashionModule::m_pFashionModule)
    {
       FashionModule::m_pFashionModule->LoadResource(pKernel);
    }

    return 1;
}

// 解锁
int nx_add_fashion(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == FashionModule::m_pFashionModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_add_fashion, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_add_fashion, 1);
    CHECK_ARG_INT(state, nx_add_fashion, 2);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
	const int config_id = pKernel->LuaToInt(state, 2);
    
	//FashionModule::m_pFashionModule->GmUnlock(pKernel, player, config_id, 0, LOG_FASHION_UNLOCK_USEITEM);
    
    return 0;
}

// 穿上
int nx_puton_fashion(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == FashionModule::m_pFashionModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_puton_fashion, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_puton_fashion, 1);
    CHECK_ARG_INT(state, nx_puton_fashion, 2);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
	const int config_id = pKernel->LuaToInt(state, 2);

	FashionModule::m_pFashionModule->PutOn(pKernel, player, config_id);

    return 0;
}

// 脱下
int nx_takeoff_fashion(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == FashionModule::m_pFashionModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_takeoff_fashion, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_takeoff_fashion, 1);
    CHECK_ARG_INT(state, nx_takeoff_fashion, 2);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
	const int config_id = pKernel->LuaToInt(state, 2);

	FashionModule::m_pFashionModule->TakeOff(pKernel, player, config_id);

    return 0;
}

// 重新锁定
int nx_lock_fashion(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == FashionModule::m_pFashionModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_lock_fashion, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_lock_fashion, 1);
    CHECK_ARG_INT(state, nx_lock_fashion, 2);

	// 获取参数
	PERSISTID player = pKernel->LuaToObject(state, 1);
	const int config_id = pKernel->LuaToInt(state, 2);

	FashionModule::m_pFashionModule->GMLock(pKernel, player, config_id);

    return 0;
}

bool FashionModule::Init(IKernel* pKernel)
{
    m_pFashionModule = this;
   
	Assert(m_pFashionModule != NULL);

	// 上线
    pKernel->AddEventCallback("player", "OnRecover", FashionModule::OnPlayerRecover, 9999);
    // 客户端消息
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_FASHION, FashionModule::OnCustomMsg);
    
    DECL_LUA_EXT(nx_reload_fashion_config);
    DECL_LUA_EXT(nx_add_fashion);
    DECL_LUA_EXT(nx_puton_fashion);
    DECL_LUA_EXT(nx_takeoff_fashion);
    DECL_LUA_EXT(nx_lock_fashion);

	DECL_HEARTBEAT(FashionModule::HB_ValidTime);
    
    if (!LoadResource(pKernel))
    {
        return false;
    }

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("FashionConfig", FashionModule::ReloadConfig);
#endif
    return true;
}

bool FashionModule::Shut(IKernel *pKernel)
{
	m_FashionCfgVec.clear();
    return true;
}

bool FashionModule::LoadResource(IKernel *pKernel)
{
    // 载入时装的配置
    if (!LoadFashionConfig(pKernel))
    {
        return false;
    }
    
    return  true;
}

// 时装配置
bool FashionModule::LoadFashionConfig(IKernel* pKernel)
{
	m_FashionCfgVec.clear();

	std::string config_file_path = pKernel->GetResourcePath();
	config_file_path.append(FASHION_CONFIG_FILE);

	CXmlFile xml(config_file_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = config_file_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	CVarList sec_list;
	xml.GetSectionList(sec_list);

	int sec_count = (int)sec_list.GetCount();

	LoopBeginCheck(a)
	for (int i = 0; i < sec_count; i++)
	{
		LoopDoCheck(a)
		const char *sec = sec_list.StringVal(i);
		if (StringUtil::CharIsNull(sec))
		{
			continue;
		}

		const int fashion_id = StringUtil::StringAsInt(sec);
		FashionCfg cfg(fashion_id);
		cfg.type = xml.ReadInteger(sec, "Type", 0);
		cfg.sex_limit = xml.ReadInteger(sec, "SexLimit", 0);
		cfg.job_limit = xml.ReadInteger(sec, "ClassLimit", 0);
		cfg.special_item = xml.ReadInteger(sec, "SpecialItem", 0);
		cfg.pre_fashion = xml.ReadInteger(sec, "PreFashionID", 0);

		const char *capitals = xml.ReadString(sec, "UnlockFee", "");
		CommRuleModule::ParseConsumeVec(capitals, cfg.unlock_fee);
		
		const char *items = xml.ReadString(sec, "UnlockItems", "");
		CommRuleModule::ParseConsumeVec(items, cfg.unlock_items);
		
		m_FashionCfgVec.push_back(cfg);
	}

	return true;
}

// 取得时装配置
const FashionCfg* FashionModule::GetFashionCfg(const int id)
{
	FashionCfg temp(id);
	FashionCfgVec::const_iterator find_it = find(
		m_FashionCfgVec.begin(), 
		m_FashionCfgVec.end(), 
		temp);
	if (m_FashionCfgVec.end() == find_it)
	{
		return NULL;
	}

	return &(*find_it);
}

// 可否解锁
bool FashionModule::CanUnlock(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box))
	{
		return false;
	}

	// 时装解锁表
	IRecord *pRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pRec)
	{
		return false;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return false;
	}

	// 前置是否已解锁
	if (!IsPreUnlock(pKernel, self, cfg->pre_fashion))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5301, CVarList());
		return false;
	}

	// 是否已解锁
	const int exist_row = pRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row >= 0)
	{
		// 永久
		if (pRec->QueryInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME) == 0)
		{
			return false;
		}
	}

	// 职业是否匹配
	if (!IsSuit(cfg->job_limit, pSelfObj->QueryInt(FIELD_PROP_JOB)))
	{
		return false;
	}

	// 性别不匹配
	if (!IsSuit(cfg->sex_limit, pSelfObj->QueryInt(FIELD_PROP_SEX)))
	{
		return false;
	}

	// 所需货币类型、数量
	if (!CommRuleModule::CanDecCapitals(pKernel, self, cfg->unlock_fee))
	{
		return false;
	}
	
	// 所需物品是否满足
	if (!CommRuleModule::CanDecItems(pKernel, self, cfg->unlock_items))
	{
		return false;
	}

	return true;
}

// 是否已解锁
bool FashionModule::IsPreUnlock(IKernel* pKernel, const PERSISTID& self, 
	const int pre_fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 无前置
	if (pre_fashion_id == 0)
	{
		return true;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return false;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(pre_fashion_id);
	if (NULL == cfg)
	{
		// 该时装无效
		return true;
	}

	// 已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, pre_fashion_id);
	if (exist_row >= 0)
	{
		// 永久
		if (pUnlockRec->QueryInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME) == 0)
		{
			return true;
		}	
	}

	return false;
}

// 解锁
void FashionModule::Unlock(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return;
	}

	// 条件不满足
	if (!CanUnlock(pKernel, self, fashion_id))
	{
		return;
	}

	if (!CommRuleModule::ConsumeCapitals(pKernel, self, cfg->unlock_fee, FUNCTION_EVENT_ID_FASION_UNLOCK))
	{
		return;
	}

	if (!CommRuleModule::ConsumeItems(pKernel, self, cfg->unlock_items, FUNCTION_EVENT_ID_FASION_UNLOCK))
	{
		return;
	}

	// 是否存在时效性解锁时装
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row >= 0)
	{
		// 有时效则设置为永久
		if (pUnlockRec->QueryInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME) > 0)
		{
			pUnlockRec->SetInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME, 0);
		}
	}
	else
	{
		// 解锁
		CVarList row_value;
		row_value << fashion_id
				  << cfg->type
				  << util_get_time_64()
				  << int64_t(0);
		pUnlockRec->AddRowValue(-1, row_value);
	}

	// 公告
	if (cfg->special_item > 0)
	{
		/*char str_nation[128] = { 0 };
		SPRINTF_S(str_nation, "Nation%d", pSelfObj->QueryInt(FIELD_PROP_NATION));
		const wchar_t* name = pSelfObj->QueryWideStr("Name");

		CVarList args;
		args << str_nation
		<< name
		<< pSelfObj->QueryInt(FIELD_PROP_VIP_LEVEL)
		<< fashion_id;
		::CustomSysInfoBroadcast(pKernel, TIPSTYPE_SPECIAL_ITEM_MESSAGE, SYS_INFO_UNLOCK_FASHION, args);*/
	}


	// 解锁日志
	/*FashionLog log;
	log.operateType = operate_type;
	log.fashion_id = fashion_id;
	log.unlock_capitals = cfg->capitals.c_str();
	log.unlock_items = cfg->items.c_str();
	LogModule::m_pLogModule->SaveFashionLog(pKernel, self, log);	*/
}

// 穿
void FashionModule::PutOn(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return;
	}

	// 是否已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row < 0)
	{
		return;
	}

	// 移除同类型同来源外观
	ApperanceModule::m_pThis->Remove(pKernel, self, ApperanceType(cfg->type), APPERANCE_SOURCE_FASHION);

	// 添加要穿戴的外观
	ApperanceModule::m_pThis->Add(pKernel, self, ApperanceType(cfg->type), APPERANCE_SOURCE_FASHION, fashion_id);
}

// 脱
void FashionModule::TakeOff(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return;
	}

	// 是否已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row < 0)
	{
		return;
	}

	// 当前外观是否穿戴
	if (ApperanceModule::m_pThis->GetApperanceByType(pKernel, self, ApperanceType(cfg->type)) != fashion_id)
	{
		return;
	}

	ApperanceModule::m_pThis->Remove(pKernel, self, ApperanceType(cfg->type), APPERANCE_SOURCE_FASHION);

	// 翅膀处理
	if (cfg->type == APPERANCE_TYPE_WING)
	{
		const int wing_default = WingModule::m_pWingModule->GetWingDefaultModel(pKernel, self);
		ApperanceModule::m_pThis->Add(pKernel, self, APPERANCE_TYPE_WING, APPERANCE_SOURCE_FASHION, wing_default);
	}
	
}

// 是否适用
bool FashionModule::IsSuit(int limit, const int v)
{
	if (limit == 0)
	{
		return false;
	}

	int temp = 0;
	while (limit != 0)
	{
		temp = limit % 10;
		if (temp == v)
		{
			return true;
		}

		limit /= 10;
	}

	return false;
}

// 找出适合玩家的时装id
const int FashionModule::FindRightFashion(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& fashion_list)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	const int job = pSelfObj->QueryInt(FIELD_PROP_JOB);
	const int sex = pSelfObj->QueryInt(FIELD_PROP_SEX);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)fashion_list.GetCount(); ++i)
	{
		LoopDoCheck(a);
		const int fashion_id = fashion_list.IntVal(i);

		// 取得时装配置
		const FashionCfg *cfg = GetFashionCfg(fashion_id);
		if (NULL == cfg)
		{
			continue;;
		}		

		// 职业是否匹配
		if (!IsSuit(cfg->job_limit, job))
		{
			continue;
		}

		// 性别不匹配
		if (!IsSuit(cfg->sex_limit, sex))
		{
			continue;
		}

		return fashion_id;
	}

	return 0;
}

// 是否可使用物品解锁
bool FashionModule::CanUse(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& fashion_list)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return false;
	}

	// 取出适合玩家的时装id
	const int fashion_id = FashionModule::m_pFashionModule->FindRightFashion(pKernel, self, fashion_list);

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return false;
	}

	// 前置是否已解锁
	if (!IsPreUnlock(pKernel, self, cfg->pre_fashion))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5301, CVarList());
		return false;
	}
	
	// 是否已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row >= 0)
	{
		// 永久
		if (pUnlockRec->QueryInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME) == 0)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5302, CVarList());
			return false;
		}		
	}

	return true;
}

// GM锁定时装
void FashionModule::GMLock(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return;
	}

	// 是否已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row < 0)
	{		
		return;
	}

	// 锁定
	pUnlockRec->RemoveRow(exist_row);

	// 未穿戴
	if (ApperanceModule::m_pThis->GetApperanceByType(pKernel, self, ApperanceType(cfg->type)) != fashion_id)
	{
		return;
	}

	ApperanceModule::m_pThis->Remove(pKernel, self, ApperanceType(cfg->type), APPERANCE_SOURCE_FASHION);
}

// 无条件解锁指定时装
bool FashionModule::GmUnlock(IKernel* pKernel, const PERSISTID& self, 
	const int fashion_id, const int valid_day, const int log_type)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return false;
	}

	// 取得时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return false;
	}

	// 职业是否匹配
	if (!IsSuit(cfg->job_limit, pSelfObj->QueryInt(FIELD_PROP_JOB)))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5303, CVarList());
		return false;
	}

	// 性别不匹配
	if (!IsSuit(cfg->sex_limit, pSelfObj->QueryInt(FIELD_PROP_SEX)))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5304, CVarList());
		return false;
	}

	// 增加时效
	const int64_t add_valid_time = valid_day * 24 * 3600 * 1000;

	// 是否已解锁
	const int exist_row = pUnlockRec->FindInt(COLUMN_FASHION_UNLOCK_REC_ID, fashion_id);
	if (exist_row >= 0)
	{
		// 永久解锁
		int64_t valid_time = pUnlockRec->QueryInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME);
		if (valid_time == 0)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5302, CVarList());
			return false;
		}
		else
		{
			// 时效累计
			valid_time += add_valid_time;

			// 新解锁为永久则代替时效性的
			valid_time = add_valid_time == 0 ? add_valid_time : valid_time;

			pUnlockRec->SetInt64(exist_row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME, valid_time);
		}
	}
	else
	{
		// 解锁
		CVarList row_value;
		row_value << fashion_id
				  << cfg->type
				  << util_get_time_64()
				  << add_valid_time;
		pUnlockRec->AddRowValue(-1, row_value);
	}

	// 解锁日志
	/*FashionLog log;
	log.operateType = log_type;
	log.fashion_id = fashion_id;
	log.unlock_capitals = cfg->capitals.c_str();
	log.unlock_items = cfg->items.c_str();
	LogModule::m_pLogModule->SaveFashionLog(pKernel, self, log);*/

	// 公告
	if (cfg->special_item > 0)
	{
		/*char str_nation[128] = { 0 };
		SPRINTF_S(str_nation, "Nation%d", pSelfObj->QueryInt(FIELD_PROP_NATION));
		const wchar_t* name = pSelfObj->QueryWideStr("Name");

		CVarList args;
		args << str_nation
		<< name
		<< pSelfObj->QueryInt(FIELD_PROP_VIP_LEVEL)
		<< fashion_id;
		::CustomSysInfoBroadcast(pKernel, TIPSTYPE_SPECIAL_ITEM_MESSAGE, SYS_INFO_UNLOCK_FASHION, args);*/
	}

	return true;
}

// 一键解锁时装
bool FashionModule::OnekeyUnlock(IKernel* pKernel, const PERSISTID& self, 
	const int log_type, const char *fashion_str, const int valid_day/* = 0*/, const char *delims /*= ","*/)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	CVarList fashion_list;
	util_split_string(fashion_list, fashion_str, delims);

	const int fashion_id = FindRightFashion(pKernel, self, fashion_list);

	// 时装配置
	const FashionCfg *cfg = GetFashionCfg(fashion_id);
	if (NULL == cfg)
	{
		return false;
	}

	// 前置是否已解锁
	if (!IsPreUnlock(pKernel, self, cfg->pre_fashion))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_5301, CVarList());
		return false;
	}

	return GmUnlock(pKernel, self, fashion_id, valid_day, log_type);
}

// 一键锁定时装
void FashionModule::OnekeyLock(IKernel* pKernel, const PERSISTID& self, 
	const char *fashion_str, const char *delims /*= ","*/)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	CVarList fashion_list;
	util_split_string(fashion_list, fashion_str, delims);

	const int fashion_id = FindRightFashion(pKernel, self, fashion_list);

	GMLock(pKernel, self, fashion_id);
}

// 时效检查
void FashionModule::ValidTimeCheck(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 时装解锁表
	IRecord *pUnlockRec = pSelfObj->GetRecord(FIELD_RECORD_FASHION_UNLOCK_REC);
	if (NULL == pUnlockRec)
	{
		return;
	}

	// 当前时间
	const int64_t now_time = util_get_time_64();

	// 检查
	LoopBeginCheck(a);
	for (int row = 0; row < pUnlockRec->GetRows(); ++row)
	{
		LoopDoCheck(a);
		// 时效
		const int64_t valid_time = pUnlockRec->QueryInt64(row, COLUMN_FASHION_UNLOCK_REC_VALID_TIME);

		// 永久
		if (valid_time == 0)
		{
			continue;
		}

		// 解锁时间
		const int64_t unlock_time = pUnlockRec->QueryInt64(row, COLUMN_FASHION_UNLOCK_REC_UNLOCK_TIME);

		// 时间差
		const int64_t diff_time = now_time - unlock_time;

		// 时效结束
		if (diff_time >= valid_time)
		{
			const int fashion_id = pUnlockRec->QueryInt(row, COLUMN_FASHION_UNLOCK_REC_ID);
			GMLock(pKernel, self, fashion_id);

			// 时效已结束
			CVarList s2c_msg;
			s2c_msg << SERVER_CUSTOMMSG_FASHION
					<< S2C_FASHION_SUBMSG_VALID_TIME_END
					<< fashion_id;
			pKernel->Custom(self, s2c_msg);
		}
	}
}

// 玩家数据恢复完成
int FashionModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
                           const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 时效检查
	HB_ValidTime(pKernel, self, 0);
	ADD_HEART_BEAT(pKernel, self, "FashionModule::HB_ValidTime", 30 * 1000);
    
    return 0;
}

// 来自玩家的消息
int FashionModule::OnCustomMsg(IKernel* pKernel, const PERSISTID& self,
    const PERSISTID& sender, const IVarList& args)
{
    //
    // 请求的协议格式:
    //      [msg_id][sub_msg_id]...
    //
	if (!pKernel->Exists(self))
	{
		return 0;
	}
    
    int sub_msg_id = args.IntVal(1);
    
    switch(sub_msg_id)
    {
	case C2S_FASHION_SUBMSG_UNLOCK:	// 解锁
		{
			m_pFashionModule->Unlock(pKernel, self, args.IntVal(2));
		}
		break;
	case C2S_FASHION_SUBMSG_PUTON:	// 穿
		{
			m_pFashionModule->PutOn(pKernel, self, args.IntVal(2));
		}
		break;
	case C2S_FASHION_SUBMSG_TAKEOFF:	// 脱
		{
			m_pFashionModule->TakeOff(pKernel, self, args.IntVal(2));
		}
		break;
	default:
		break;
    }
 
    return 0;
}

// 时效性检查
int FashionModule::HB_ValidTime(IKernel* pKernel, const PERSISTID& self, 
	int slice)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	m_pFashionModule->ValidTimeCheck(pKernel, self);

	return 0;
}

// 重载
void FashionModule::ReloadConfig(IKernel* pKernel)
{
	m_pFashionModule->LoadResource(pKernel);
}