//--------------------------------------------------------------------
// 文件名:		CapitalModule.cpp
// 内  容:		角色资金管理
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------
#include "CapitalModule.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/util_ini.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "utils/json.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/pubdefine.h"
//#include "FsGame/Define/DynamicActDefine.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame\Define\SceneListDefine.h"
#include "FsGame\Define\CoolDownDefine.h"
#include "FsGame\SystemFunctionModule\CoolDownModule.h"
#include <sstream>
#include <time.h>
#include <math.h>
#include "public/VarList.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame\SystemFunctionModule\ResetTimerModule.h"
#include "CommonModule/ReLoadConfigModule.h"
#endif
#include "RewardModule.h"
#include "Define/ClientCustomDefine.h"
//#include "Define/SevenDayObjectDefine.h"
//#include "SkillModule/CriminalSystem.h"
//#include "Define/PKModelDefine.h"
#include "Define/RankingDefine.h"
#include "RankListModule.h"
//#include "Define/WeeklyScoreDefine.h"

#define SYS_INFO_CAPITAL_EXCHANGE_FAIL_NOT_ENOUGH "sys_info_capital_exchange_fail1"		// 消耗的原始货币不足

#define  CAPITAL_REC "capital_rec"    //资产表

//玩家每天累计增加资金记录（包括未加成功的资金）
const char *const DAY_TOPLIMIT_CAPITAL = "dayToplimitCapital";

#ifndef FSROOMLOGIC_EXPORTS

// 原型：bool nx_can_inc_capital(object self, int capital_type, int value)
// 功能：能否添加指定数量的资金
int nx_can_inc_capital(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_can_dec_capital, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_can_inc_capital, 1);
	CHECK_ARG_INT(state, nx_can_inc_capital, 2);
	CHECK_ARG_INT64(state, nx_can_inc_capital, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	int capital_type = pKernel->LuaToInt(state, 2);
	__int64 value = pKernel->LuaToInt64(state, 3);

	pKernel->LuaPushBool(state, CapitalModule::m_pCapitalModule->CanIncCapital(pKernel, self, capital_type, value));
	return 1;
}

// 原型：bool nx_can_dec_capital(object self, int capital_type, int value)
// 功能：能否减少指定数量的资金
int nx_can_dec_capital(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_can_dec_capital, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_can_dec_capital, 1);
	CHECK_ARG_INT(state, nx_can_dec_capital, 2);
	CHECK_ARG_INT64(state, nx_can_dec_capital, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	int capital_type = pKernel->LuaToInt(state, 2);
	__int64 value = pKernel->LuaToInt64(state, 3);

	pKernel->LuaPushBool(state, CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, capital_type, value));
	return 1;
}

// 原型：bool nx_inc_capital(object self, int capital_type, int value)
// 功能：添加指定数量的资金
int nx_inc_capital(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_inc_capital, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_inc_capital, 1);
	CHECK_ARG_INT(state, nx_inc_capital, 2);
	CHECK_ARG_INT64(state, nx_inc_capital, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	int capital_type = pKernel->LuaToInt(state, 2);
	__int64 value = pKernel->LuaToInt64(state, 3);

	bool result = CapitalModule::m_pCapitalModule->IncCapital(pKernel, self, capital_type, value, FUNCTION_EVENT_ID_GM_COMMAND);
	pKernel->LuaPushBool(state, result);

	return 1;
}

// 原型：bool nx_dec_capital(object self, int capital_type, int value)
// 功能：减少指定数量的资金
int nx_dec_capital(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_dec_capital, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_dec_capital, 1);
	CHECK_ARG_INT(state, nx_dec_capital, 2);
	CHECK_ARG_INT64(state, nx_dec_capital, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	int capital_type = pKernel->LuaToInt(state, 2);
	__int64 value = pKernel->LuaToInt64(state, 3);

	bool bResult = false;
	int result = CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, capital_type, value, FUNCTION_EVENT_ID_GM_COMMAND);
	if (result == DC_SUCCESS)
	{
		bResult = true;
	}

	pKernel->LuaPushBool(state, bResult);

	return 1;
}

//GM设置每日增加资金上限
int nx_set_capital_monitor(void* state)
{
    // 获得核心指针
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 公共空间
    IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
    if (pPubSpace == NULL)
    {
        return 0;
    }

    // 公共数据
    wchar_t pubDataName[256];
    SWPRINTF_S(pubDataName, L"Domain_SceneList%d", pKernel->GetServerId());
    IPubData* pPubData = pPubSpace->GetPubData(pubDataName);
    if (pPubData == NULL)
    {
        return 0;
    }

    // 上架表
    IRecord* pRecord = pPubData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
    if (pRecord == NULL)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_set_capital_monitor, 3);
    // 检查参数类型
    CHECK_ARG_INT(state, nx_set_capital_monitor, 1);
    CHECK_ARG_INT64(state, nx_set_capital_monitor, 2);
    CHECK_ARG_INT64(state, nx_set_capital_monitor, 3);

    int capitalType = pKernel->LuaToInt(state, 1);

    if(CapitalModule::m_pCapitalModule == NULL)
    {
        return 0;
    }

    if(!CapitalModule::m_pCapitalModule->IsCapitalDefined(capitalType))
    {
        return 0;
    }

    CVarList msg;
    msg << COMMAND_MSG_REREAD_CFG_GM
        << "OnSetCapitalMonitorMaxVal"
        << pKernel->LuaToInt(state, 1)
        << pKernel->LuaToInt64(state, 2)
        << pKernel->LuaToInt64(state, 3);

    int rows = pRecord->GetRows();

    LoopBeginCheck(a1);
    for(int i = 0; i < rows; ++i)
    {
        LoopDoCheck(a1);

        pKernel->CommandToScene(pRecord->QueryInt(i, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id), msg);
    }
    
    return 1;
}

#endif // FSROOMLOGIC_EXPORTS

CapitalModule* CapitalModule::m_pCapitalModule = NULL;

#ifndef FSROOMLOGIC_EXPORTS
LogModule* CapitalModule::m_pLogModule = NULL;
#endif // FSROOMLOGIC_EXPORTS

bool CapitalModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	m_pCapitalModule = this;

#ifndef FSROOMLOGIC_EXPORTS
	m_pLogModule = static_cast<LogModule*>(pKernel->GetLogicModule("LogModule"));
	//m_domainName = L"";

	Assert(m_pLogModule != NULL);
#endif // FSROOMLOGIC_EXPORTS

	if (!LoadResource(pKernel))
	{
		Assert(false);

		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_can_inc_capital);
	DECL_LUA_EXT(nx_can_dec_capital);
	DECL_LUA_EXT(nx_inc_capital);
	DECL_LUA_EXT(nx_dec_capital);
    DECL_LUA_EXT(nx_set_capital_monitor);
#endif // FSROOMLOGIC_EXPORTS

    //玩家从数据库恢复数据完成（玩家上线）
    pKernel->AddEventCallback("player", "OnRecover", CapitalModule::OnPlayerRecover);

    //GM设置每日增加资金上限
    pKernel->AddIntCommandHook("scene", COMMAND_MSG_REREAD_CFG_GM, CapitalModule::OnSetCapitalMonitorMaxVal);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_EXCHANGE_CAPITAL, CapitalModule::OnCustomExchangeCapital);

#ifndef FSROOMLOGIC_EXPORTS
	DECL_RESET_TIMER(RESET_DDY_TOPLIMIT, CapitalModule::ResetDayToplimit);
	RELOAD_CONFIG_REG("CapitalConfig", CapitalModule::ReloadConfig);
#endif // FSROOMLOGIC_EXPORTS

	return true;
}

bool CapitalModule::Shut(IKernel* pKernel)
{
	return true;
}

// 货币是否已定义
bool CapitalModule::IsCapitalDefined(const std::string& name)
{
	LoopBeginCheck(a)
	for(unsigned int i = 0; i< m_vecCaptialType.size(); i++)
	{
		LoopDoCheck(a)
		if (m_vecCaptialType[i].prop == name)
		{
			return true;
		}
	}
	return false;
}

bool CapitalModule::IsCapitalDefined(const int& type)
{
	if (type < 0 || type >= (int)m_vecCaptialType.size())
	{
		return false;
	}

	return true;
}

// 根据货币名字查询货币类型
int CapitalModule::GetCapitalType(const std::string& name)
{
	LoopBeginCheck(b)
	for(unsigned int i=0; i<m_vecCaptialType.size(); i++)
	{
		LoopDoCheck(b)
		if (m_vecCaptialType[i].prop==name)
		{
			return i;
		}
	}
	return -1;
}

// 根据货币类型查询货币名字
std::string CapitalModule::GetCapitalName(const int& type)
{
	if (!IsCapitalDefined(type))
	{
		return "";
	}

	return m_vecCaptialType[type].prop;
}

//对外接口
//获得资金类型的种类数量
int CapitalModule::GetCapitalTypeCount(IKernel* pKernel)
{
	return (int)m_vecCaptialType.size();
}

//获得某种类型的资金的当前值
__int64 CapitalModule::GetCapital(IKernel* pKernel, const PERSISTID& self, int capital_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (!CapitalModule::IsValid(pKernel, self, capital_type))
	{
		return 0;
	}

	__int64 ret = 0;
	const char* proname = m_vecCaptialType[capital_type].prop.c_str();
	IRecord* pRecord = pSelfObj->GetRecord(CAPITAL_REC);
	if (pRecord != NULL)
	{
		int row = pRecord->FindString(CAPITAL_NAME, proname);
		if (row > -1)
		{
			ret = pRecord->QueryInt64(row, CAPITAL_VALUE);
		}
	}

	return  ret;
}

//增加某种类型的资金的当前值
bool CapitalModule::IncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value,
							   EmFunctionEventId log_type, const wchar_t* comment/* = L""*/)
{
    if(pKernel == NULL)
    {
        return false;
    }
    
    //增加资前数值
    __int64 beforNum = GetCapital(pKernel, self, capital_type);

	// 看看是否需要红名惩罚衰减,
	float fRate = 1.0f;
// 	if (CriminalSystemSingleton::Instance()->QueryCiminalRate(pKernel, fRate, self, log_type, CP_EXP_RATE) && capital_type == CAPITAL_COPPER)
// 	{
// 		value = (__int64)((float)value * fRate);
// 	}

    //正常增加部分
    __int64 normalAddNum = 0;

	CAPITAL_MONITOR result;

    //监视货币日增加,GM操作和冲充不监视
	if (log_type == FUNCTION_EVENT_ID_GM_COMMAND) // 货币兑换不计入监控
    {
        result = CAPITAL_MONITOR_NORMAL;
    }
    else
    {
		//不正常增加部分
		__int64 abnormalAddNum = 0;
        result = CapitalMonitor(pKernel, self, capital_type, value, normalAddNum, abnormalAddNum);
    }
    
    //增加资金修改是否成功
    bool ret = false;
    if(result != CAPITAL_MONITOR_TOPLIMIT)
    {
        ret = Inner_IncCapital(pKernel, self, capital_type, value);
    }
    else if(normalAddNum > 0)
    {
        ret = Inner_IncCapital(pKernel, self, capital_type, normalAddNum);
    }

    //增加资完成后数值
    __int64 afterNum = GetCapital(pKernel, self, capital_type);

    //“监视货币日增加”引起的增加资金修改失败记日志
     if( ret == false )
     {
         return ret;
     }

 #ifndef FSROOMLOGIC_EXPORTS
    //保存货币增加日志
	if(LogModule::m_pLogModule != NULL)
	{
		// 铜币 && 打怪掉落的铜币 && 任务奖励货币 && 杀人加货币(军功) 不记录日志
		if( capital_type == CAPITAL_COPPER )
		{
			if (log_type == FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP)
			{
				// TEMP?:暂时开启所有日志
				// return ret;
			}
		}

		LOG_ACT_STATE_TYPE state = ret ? LOG_ACT_STATE_SUCCESS : LOG_ACT_STATE_FAIL;
		MoneyLog log;
		log.capitalType   = capital_type;
		log.eventID       = log_type;
		log.eventType     = LOG_ACT_EVENT_GAIN;
		log.before        = beforNum;
		log.money         = value;
		log.after         = afterNum;
		log.state         = state;
		LogModule::m_pLogModule->SaveMoneyLog(pKernel, self, log);

    }


// 	CVarList s2s_seven_obj_msg;
// 	s2s_seven_obj_msg << COMMAND_SEVEN_DAY_OBJ
// 		<< COMMAND_SEVEN_DAY_OBJ_ADD_CAPITAL << capital_type << value;
// 	pKernel->Command(self, self, s2s_seven_obj_msg);
// 
// 	// 通知周常积分模块
// 	CVarList s2s_ws_msg;
// 	s2s_ws_msg << COMMAND_WEEKLY_SCORE
// 			   << S2S_WEEKLY_SCORE_SUBMSG_CAPITAL_CHANGE
// 			   << WEEKLY_SCORE_CAL_WAY_INC
// 			   << capital_type
// 			   << (afterNum - beforNum);
// 	pKernel->Command(self, self, s2s_ws_msg);

#endif // FSROOMLOGIC_EXPORTS

	return ret;
}

//减少某种类型的资金的当前值
//返回实际减少的值
int CapitalModule::DecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value,
							  int logType, const char* itemConfigId/* = ""*/, 
							  const char* itemInstanceId/* = ""*/, int itemCount/* = 0*/,
							  int cmd /*= -1*/, const IVarList* var /*= NULL*/, const char * reason /*= ""*/)
{
	__int64 beforNum = GetCapital(pKernel, self, capital_type);
	int ret = Inner_DecCapital(pKernel, self, capital_type, value);
	__int64 afterNum = GetCapital(pKernel, self, capital_type);

	if (ret != DC_SUCCESS)
	{
		return ret;
	}																 

#ifndef FSROOMLOGIC_EXPORTS
	//记录货币日志
	if ( m_pLogModule != NULL && ret == DC_SUCCESS )
	{
		//保存货币减少日志
		MoneyLog log;
		log.capitalType   = capital_type;
		log.eventID       = logType;
		log.eventType     = LOG_ACT_EVENT_LOSE;
		log.money         = value;
		log.before        = beforNum;
		log.after         = afterNum;

		log.state         = LOG_ACT_STATE_SUCCESS;
		LogModule::m_pLogModule->SaveMoneyLog(pKernel, self, log);
	}
#endif // FSROOMLOGIC_EXPORTS

	// 通知周常积分模块
// 	CVarList s2s_ws_msg;
// 	s2s_ws_msg << COMMAND_WEEKLY_SCORE
// 			   << S2S_WEEKLY_SCORE_SUBMSG_CAPITAL_CHANGE
// 			   << WEEKLY_SCORE_CAL_WAY_DEC
// 			   << capital_type
// 			   << (beforNum - afterNum);
// 	pKernel->Command(self, self, s2s_ws_msg);

	return ret;
}

//是否能够增加某种类型的资金的当前值
//如果未达最大值，返回实际可增加的值数量
bool CapitalModule::CanIncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value)
{
	//判断是否是一个有效的值
	if (value <= 0)
	{
		return false;
	}

	//判断是否有效的资金类型
	if (!CapitalModule::IsValid(pKernel, self, capital_type))
	{
		return false;
	}

	//最大资金值，如果最大资金值未定义，则不能执行操作
	__int64 maxvalue = CapitalModule::GetMaxValue(pKernel, capital_type);
	if (maxvalue <= 0)
	{
		return false;
	}

	//当前资金值
	__int64 curvalue = CapitalModule::GetCapital(pKernel, self, capital_type);
	if (curvalue >= maxvalue)//是否已达最大值
	{
		return false;
	}

	//新的资金值
	__int64 newvalue = curvalue + value;
	if (newvalue < 0)
	{
		return false;
	}

	if (newvalue > maxvalue)//新值不能超过最大值
	{
		return false;
	}

	return true;
}

//是否能够减少某种类型的资金的当前值
//如果未达最小值，返回实际可减少的值数量
bool CapitalModule::CanDecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value)
{
	//判断是否是一个有效的值
	if (value < 0)
	{
		return false;
	}

	//判断是否有效的资金类型
	if (!CapitalModule::IsValid(pKernel, self, capital_type))
	{
		return false;
	}

	//当前资金值
	__int64 curvalue = CapitalModule::GetCapital(pKernel, self, capital_type);
	if (curvalue < value)
	{
		return false;
	}

	return true;
}

//是否是有效的资金类型
bool CapitalModule::IsValid(IKernel* pKernel, const PERSISTID& self, int capital_type)
{
	if (capital_type < 0 || capital_type >= (int)m_vecCaptialType.size())
	{
		return false;
	}

	return true;
}

//
bool CapitalModule::SetCaptialValue(IKernel* pKernel, const PERSISTID& self, const int capital_type, const __int64 newvalue)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	if (!CapitalModule::IsValid(pKernel, self, capital_type))
	{
		return false;
	}

	IRecord* pRec = pSelfObj->GetRecord(CAPITAL_REC);
	if (NULL == pRec)
	{
		return false;
	}

	int row = pRec->FindInt(CAPITAL_TYPE, capital_type);
	if (row < 0)
	{
		const char* proname = GetCapitalProp(capital_type);;
		if (NULL == proname)
		{
			return false;
		}
		pRec->AddRowValue(-1, CVarList() << proname << capital_type << newvalue);

		return true;
	}
	pRec->SetInt64(row, CAPITAL_VALUE, newvalue);

	return true;
}

//获取某种类型资金的最大值，返回-1时为失败操作
__int64 CapitalModule::GetMaxValue(IKernel* pKernel, int capital_type)
{
	if (capital_type < 0 || capital_type >= (int)m_vecCaptialType.size())
	{
		return false;
	}

	return m_vecCaptialType[capital_type].maxvalue;
}

//获取某种类型资金的属性名称
const char* CapitalModule::GetCapitalProp(int capital_type)
{
	if (capital_type < 0 || capital_type >= (int)m_vecCaptialType.size())
	{
		return "";
	}

	return m_vecCaptialType[capital_type].prop.c_str();
}

#ifndef FSROOMLOGIC_EXPORTS
int CapitalModule::AddCapital(IKernel *pKernel, const PERSISTID &player, const IVarList &capital_list, EmFunctionEventId capitalEventID)
{
	int capital_count = (int)(capital_list.GetCount());

	if (capital_count == 0)
	{
		return 0;
	}

	capital_count = capital_count / 2;
	int index = 0;
	int nCount = 0;
	LoopBeginCheck(a)
	for (int i = 0; i < capital_count; ++i)
	{
		LoopDoCheck(a)

		const char *capital_name = capital_list.StringVal(index++);
		int64_t capital_amount = capital_list.Int64Val(index++);
		if (StringUtil::CharIsNull(capital_name) || capital_amount == 0)
		{
			continue;
		}

		int capital_type = m_pCapitalModule->GetCapitalType(capital_name);
		if (m_pCapitalModule->IncCapital(pKernel, player, capital_type, capital_amount, capitalEventID))
		{
			++nCount;
		}
	}

	return nCount;
}

int CapitalModule::AddCapital(IKernel *pKernel, const PERSISTID &player, const char* pszCaptialInfo, EmFunctionEventId capitalEventID)
{
	if (StringUtil::CharIsNull( pszCaptialInfo ))
	{
		return 0;
	}

	CVarList strList;
	StringUtil::SplitString(strList, pszCaptialInfo, ",");

	CVarList paraList;
	int nCount = strList.GetCount();
	LoopBeginCheck(a);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(a);
		std::string captial = strList.StringVal(i);
		CVarList strInfo;
		StringUtil::SplitString(strInfo, captial.c_str(), ":");
		const char* pszName = strInfo.StringVal(0);
		const char* pszNum = strInfo.StringVal(1);
		if (StringUtil::CharIsNull(pszName) || StringUtil::CharIsNull(pszNum))
		{
			continue;
		}
		paraList << pszName << StringUtil::StringAsInt(pszNum);
	}

	return AddCapital(pKernel, player, paraList, capitalEventID);
}
#endif

//从Rule/CapitalDefine.ini文件中读取资金类型的定义
bool CapitalModule::LoadResource(IKernel* pKernel)
{
	m_vecCaptialType.clear();
	std::string pathname = pKernel->GetResourcePath();
	pathname += "ini/Item/Capital.xml";
	CXmlFile inifile(pathname.c_str());

	if (!inifile.LoadFromFile())
	{
		::extend_warning(LOG_WARNING, "[Error]CapitalModule::LoadResource: No found file:");
		::extend_warning(LOG_WARNING, pathname.c_str());

		return false;
	}

	//资金类型数
	int type_count = inifile.ReadInteger("CapitalType", "TypeCount", 0);
	if (type_count == 0)
	{
		return false;
	}

	m_vecCaptialType.resize(type_count);

	LoopBeginCheck(c)
	for (int i = 0; i < type_count; i++)
	{
		LoopDoCheck(c)
		fast_string key = "C";
		key += ::util_int64_as_string(i).c_str();

		const char* info = inifile.ReadString("CapitalType", key.c_str(), "");
		CVarList res;
		::util_split_string(res, info, ",");
		m_vecCaptialType[i].prop = res.StringVal(0);
		m_vecCaptialType[i].maxvalue = ::_atoi64(res.StringVal(1));
        m_vecCaptialType[i].warnDayVal = ::_atoi64(res.StringVal(2));
        m_vecCaptialType[i].maxDayVal = ::_atoi64(res.StringVal(3));
	}

	// 兑换规则
	if (!LoadExchangeRuleConfig(pKernel))
	{
		return false;
	}

	return true;
}

// 读取兑换规则配置
bool CapitalModule::LoadExchangeRuleConfig(IKernel* pKernel)
{
	m_vecExchangeRules.clear();
	std::string pathname = pKernel->GetResourcePath();
	pathname += "ini/SystemFunction/capital_exchange.xml";
	CXmlFile xmlfile(pathname.c_str());
	if (!xmlfile.LoadFromFile())
	{
		::extend_warning(LOG_WARNING, "[Error]CapitalModule::LoadResource: No found file:");
		::extend_warning(LOG_WARNING, pathname.c_str());
		return false;
	}

	int nSectionCount = (int)xmlfile.GetSectionCount();
	m_vecExchangeRules.reserve(nSectionCount);
	LoopBeginCheck(w);
	for (int i = 0; i < nSectionCount;++i)
	{
		LoopDoCheck(w);
		CapitalExchangeRule data;
		const char* strSection = xmlfile.GetSectionByIndex(i);
		data.nExchangeId = StringUtil::StringAsInt(strSection);
		data.strSrcCapital = xmlfile.ReadString(strSection, "SrcCapital", "");
		data.strDesCapital = xmlfile.ReadString(strSection, "DesCapital", "");
		data.nExchangeRate = xmlfile.ReadInteger(strSection, "ExchangeRate", 0);

		m_vecExchangeRules.push_back(data);
	}

	return true;
}

// 执行实际的增加金钱
bool CapitalModule::Inner_IncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	IRecord* pRecord = pSelfObj->GetRecord(CAPITAL_REC);
	if (pRecord == NULL)
	{
		return false;
	}

	//判断是否能够增加或减少资金
	if (!CapitalModule::CanIncCapital(pKernel, self, capital_type, value))
	{
		return false;
	}

	//计算新的资金值
	__int64 newvalue = value + GetCapital(pKernel, self, capital_type); 
	if (newvalue > CapitalModule::GetMaxValue(pKernel, capital_type))
	{
		//保证安全
		return false;
	}

	const char* proname = m_vecCaptialType[capital_type].prop.c_str();

	//设置资金值
	int row = pRecord->FindString(CAPITAL_NAME, proname);
	if (row < 0)
	{
		pRecord->AddRowValue(-1, CVarList() << proname << capital_type << newvalue);
	}
	else
	{
		pRecord->SetInt64(row, CAPITAL_VALUE, newvalue);
	}


	return true;
}

// 执行实际的扣除金钱
int CapitalModule::Inner_DecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value)
{
	//判断是否能够增加或减少资金
	if (!CapitalModule::CanDecCapital(pKernel, self, capital_type, value))
	{
		return DC_NOT_ENOUGH;
	}

	//计算新的资金值
	__int64 newvalue =  GetCapital(pKernel, self, capital_type) - value; 
	if (newvalue < 0)
	{
		return DC_NOT_ENOUGH;
	}

	//设置资金值
	if (!SetCaptialValue(pKernel, self, capital_type, newvalue))
	{
		return DC_ERROR;
	}

	return DC_SUCCESS;
}

//监视货币日增加量合法性
const CAPITAL_MONITOR CapitalModule::CapitalMonitor(IKernel *pKernel, const PERSISTID &self, const int capitalType, const __int64 addNum, __int64 &normalAddNum, __int64 &abnormalAddNum)
{

#ifndef FSROOMLOGIC_EXPORTS
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return CAPITAL_MONITOR_NORMAL;
	}

	// 货币监控开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_CAPITAL_MONITOR))
	{
		return CAPITAL_MONITOR_NORMAL;
	}

    //判断是否是有效类型
    if (!CapitalModule::IsValid(pKernel, self, capitalType))
    {
        return CAPITAL_MONITOR_TOPLIMIT;
    }

    std::string capitalName = GetCapitalName(capitalType);

    IRecord *dayToplmitCapitalRec = pSelfObj->GetRecord(DAY_TOPLIMIT_CAPITAL);
    if(dayToplmitCapitalRec == NULL)
    {
        return CAPITAL_MONITOR_TOPLIMIT;
    }
    __int64 lastNum = 0;
    int row = dayToplmitCapitalRec->FindInt(CAPITAL_TYPE, capitalType);
    if(row >= 0)
    {
        __int64 curNum = dayToplmitCapitalRec->QueryInt64(row, CAPITAL_VALUE);
        lastNum = curNum + addNum;
        dayToplmitCapitalRec->SetInt64(row, CAPITAL_VALUE, lastNum);
    }
    else
    {
        dayToplmitCapitalRec->AddRowValue(-1, CVarList() << capitalName.c_str() << capitalType << addNum);
        lastNum = addNum;
    }

    //判断异常级别
    CAPITAL_MONITOR sort;
    if(lastNum > m_vecCaptialType[capitalType].maxDayVal)
    {
        if(lastNum - addNum > m_vecCaptialType[capitalType].maxDayVal)
        {
            normalAddNum = 0;
            abnormalAddNum = addNum;
        }
        else
        {
            normalAddNum = m_vecCaptialType[capitalType].maxDayVal - (lastNum - addNum);
            abnormalAddNum = addNum - normalAddNum;
        }
        sort = CAPITAL_MONITOR_TOPLIMIT;
        
        //客户端弹窗提示
        if(CoolDownModule::m_pInstance != NULL && !CoolDownModule::m_pInstance->IsCoolDown(pKernel, self, COOLDOWN_CAPITAL_MONITOR))
        {
            CVarList var;
			var << capitalName.c_str() << m_vecCaptialType[capitalType].maxDayVal;
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19204, var);
            CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, COOLDOWN_CAPITAL_MONITOR, 300000);
        }
    }
    else if(lastNum > m_vecCaptialType[capitalType].warnDayVal)
    {
        if(lastNum - addNum > m_vecCaptialType[capitalType].warnDayVal)
        {
            normalAddNum = 0;
            abnormalAddNum = addNum;
        }
        else
        {
            normalAddNum = m_vecCaptialType[capitalType].warnDayVal - (lastNum - addNum);
            abnormalAddNum = addNum - normalAddNum;
        }
        sort = CAPITAL_MONITOR_WARN;
    }
    else
    {
        normalAddNum = addNum;
        abnormalAddNum = 0;
        sort = CAPITAL_MONITOR_NORMAL;
    }

    //记录到公共数据服务器
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf != NULL && sort != CAPITAL_MONITOR_NORMAL)
    {
        wchar_t pubDataName[256];
        SWPRINTF_S(pubDataName, L"Domain_CapitalMonitor_%d", pKernel->GetServerId());
        CVarList pub_msg;
        pub_msg << PUBSPACE_DOMAIN
                << pubDataName
                << SP_DOMAIN_MSG_CAPITAL_MONITOR
                << pSelf->QueryString("Uid")
                << pSelf->GetName()
                << sort
                << capitalType
                << abnormalAddNum
                << time(NULL);
        pKernel->SendPublicMessage(pub_msg);
    }

    return sort;
#else
	// 战场服务器不进行监控
	return CAPITAL_MONITOR_NORMAL;
#endif // FSROOMLOGIC_EXPORTS
}

bool CapitalModule::IsDailyCanAddCapital(IKernel *pKernel, const PERSISTID &self, int capital_type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}


	//判断是否是有效类型
	if (!CapitalModule::IsValid(pKernel, self, capital_type))
	{
		return false;
	}

	std::string capitalName = GetCapitalName(capital_type);

	IRecord *dayToplmitCapitalRec = pSelfObj->GetRecord(DAY_TOPLIMIT_CAPITAL);
	if (dayToplmitCapitalRec == NULL)
	{
		return false;
	}
	__int64 lastNum = 0;
	int row = dayToplmitCapitalRec->FindInt(CAPITAL_TYPE, capital_type);
	if (row >= 0)
	{
		__int64 curNum = dayToplmitCapitalRec->QueryInt64(row, CAPITAL_VALUE);
		if (m_vecCaptialType[capital_type].maxDayVal <= curNum){
			return false;
		}

	}

	return true;

}

//0点重置当天累计增加资金记录值
int CapitalModule::ResetDayToplimit(IKernel *pKernel, const PERSISTID &self, int slice)
{
    if(pKernel == NULL)
    {
        return 0;
    }

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	IRecord* pRecord = pSelfObj->GetRecord(DAY_TOPLIMIT_CAPITAL);
	if (pRecord == NULL)
	{
		return 0;
	}
    
    pRecord->ClearRow();

    return 0;
}

//玩家从数据库恢复数据完成（玩家上线）
int CapitalModule::OnPlayerRecover(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{

#ifndef FSROOMLOGIC_EXPORTS
	REGIST_RESET_TIMER(pKernel, self, RESET_DDY_TOPLIMIT);
#endif // FSROOMLOGIC_EXPORTS

    return 0;
}

//设置每日增加资金上限
bool CapitalModule::SetCapitalMonitorMaxVal(const int capitalType, const __int64 warnDayVal, const __int64 maxDayVal)
{
    if(capitalType < 0 || capitalType >= int(m_vecCaptialType.size()))
    {
        return false;
    }

    m_vecCaptialType[capitalType].warnDayVal = warnDayVal;
    m_vecCaptialType[capitalType].maxDayVal = maxDayVal;

    return true;
}


//GM设置每日增加资金上限
int CapitalModule::OnSetCapitalMonitorMaxVal(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
    if(strcmp(args.StringVal(1), "OnSetCapitalMonitorMaxVal") != 0)
    {
        return 0;
    }

    if (NULL == CapitalModule::m_pCapitalModule)
    {
        return 0;
    }
    CapitalModule::m_pCapitalModule->SetCapitalMonitorMaxVal(args.IntVal(2), args.Int64Val(3), args.Int64Val(4));

    return 0;
}

void CapitalModule::ReloadConfig(IKernel* pKernel)
{
	m_pCapitalModule->LoadResource(pKernel);
}

// 响应客户端消息
int CapitalModule::OnCustomExchangeCapital(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetCount() < 3 || args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_INT || args.GetType(2) != VTYPE_INT)
	{
		return 0;
	}

	int nExchangeId = args.IntVal(1);
	int nCostSrcCapital = args.IntVal(2);
	const CapitalExchangeRule* pRuleData = m_pCapitalModule->QueryExchangeRule(nExchangeId);
	// 参数验证
	if (NULL == pRuleData || nCostSrcCapital <= 0)
	{
		return 0;
	}

	// 获取货币类型
	int nSrcCapitalType = m_pCapitalModule->GetCapitalType(pRuleData->strSrcCapital);
	int nDesCapitalType = m_pCapitalModule->GetCapitalType(pRuleData->strDesCapital);
	if (-1 == nSrcCapitalType || -1 == nDesCapitalType)
	{
		return 0;
	}

	// 验证消耗的货币是否足够
	if (!m_pCapitalModule->CanDecCapital(pKernel, self, nSrcCapitalType, nCostSrcCapital))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19203, CVarList());
		return 0;
	}	  

	// 消耗原货币
	if (m_pCapitalModule->DecCapital(pKernel, self, nSrcCapitalType, nCostSrcCapital, FUNCTION_EVENT_ID_CAPITAL_EXCHANGE) != DC_SUCCESS)
	{
		return 0;
	}

	// 增加目标货币
	m_pCapitalModule->IncCapital(pKernel, self, nDesCapitalType, nCostSrcCapital * pRuleData->nExchangeRate, FUNCTION_EVENT_ID_CAPITAL_EXCHANGE);

	// 货币兑换成功	 int nType int nNum
	//	SERVER_CUSTOMMSG_CAPITAL_EXCHANGE_SUC = 211,
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_CAPITAL_EXCHANGE_SUC << nDesCapitalType << nCostSrcCapital * pRuleData->nExchangeRate);
	return 0;
}

// 查询兑换规则
const CapitalModule::CapitalExchangeRule* CapitalModule::QueryExchangeRule(int nExchangeId)
{
	CapitalExchangeRule* pFindRule = NULL;
	int nSize = (int)m_vecExchangeRules.size();
	LoopBeginCheck(q);
	for (int i = 0; i < nSize;++i)
	{
		LoopDoCheck(q);
		if (nExchangeId == m_vecExchangeRules[i].nExchangeId)
		{
			pFindRule = &m_vecExchangeRules[i];
			break;
		}
	}

	return pFindRule;
}