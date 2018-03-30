//----------------------------------------------------------
// 文件名:      SwitchManagerModule.cpp
// 内  容:      功能开关定义
// 说  明:
// 创建人:        
// 创建日期:    2015年6月30日
//    :        
//----------------------------------------------------------
#include "SwitchManagerModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/PubDefine.h"
//#include "FsGame/Define/BackStageDefine.h"
#include "FsGame/Define/GameDefine.h"

#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/util_func.h"

SwitchManagerModule* SwitchManagerModule::m_SwitchManagerModule = NULL;

// 原型：bool nx_set_function(function_id, new_state)
// 功能：设置功能状态
int nx_set_function(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_set_function, 2);
    // 检查参数类型
    CHECK_ARG_INT(state, nx_set_function, 1);
    CHECK_ARG_INT(state, nx_set_function, 2);

    const int function_id = pKernel->LuaToInt(state, 1);
    int new_state = pKernel->LuaToInt(state, 2);
    new_state = new_state > 0 ? 1 : 0;

    pKernel->LuaPushBool(state, SwitchManagerModule::SetFunctionEnable(pKernel, function_id, new_state));
    return 1;
}

bool SwitchManagerModule::Init(IKernel* pKernel)
{
    m_SwitchManagerModule = this;

	// 场景加载
	pKernel->AddEventCallback("scene", "OnCreate", OnSceneCreate);

	// 注册GM命令
    DECL_LUA_EXT(nx_set_function);

	//注册加载公共数据到数据库延迟心跳
	DECL_HEARTBEAT(SwitchManagerModule::CB_DomainDB);

    return true;
}

bool SwitchManagerModule::Shut(IKernel* pKernel)
{
    return true;
}

//获取域名
const std::wstring SwitchManagerModule::GetDomainName(IKernel* pKernel)
{
    if (m_DomainName.empty())
    {
        wchar_t wstr[256];
        int server_id = 0;
#ifndef FSROOMLOGIC_EXPORTS
		server_id = pKernel->GetServerId();
#endif
        SWPRINTF_S(wstr, L"Domain_Switch_%d", server_id);
        m_DomainName = wstr;
    }

    return m_DomainName;
}

//检查功能是否开启
bool SwitchManagerModule::CheckFunctionEnable(IKernel* pKernel, const int function_id, const PERSISTID& self)
{
	CVarList msg = m_SwitchManagerModule->QueryPubFunctionState(pKernel, function_id);
    const int curState = msg.IntVal(0);
    bool ret = curState > 0 ? true : false;

	if (!ret && pKernel->Exists(self))
	{
		SendCustomSysInfo(pKernel, self, function_id);
	}

    return ret;
}

//消息解析
bool SwitchManagerModule::GetSwitchFunctionInfo(IKernel* pKernel, const char* content)
{
	if (pKernel == NULL || StringUtil::CharIsNull(content))
	{
		return false;
	}

	// 解析内容
	TiXmlDocument xml;
	xml.Parse(content);
	TiXmlHandle xmlHandle(&xml);
	TiXmlElement* pNode = xmlHandle.FirstChild("Property").ToElement();
	if (pNode == NULL)
	{
		return false;
	}

	// 获取id
	const TiXmlElement* pElement = (TiXmlElement*)pNode->FirstChild("functionid");
	if (pElement == NULL)
	{
		return false;
	}

	const char* ID = pElement->GetText();
	if (StringUtil::CharIsNull(ID))
	{
		return false;
	}

	// 获取类型
	pElement = (TiXmlElement*)pNode->FirstChild("state");
	if (pElement == NULL)
	{
		return false;
	}

	const char* state = pElement->GetText();
	if (StringUtil::CharIsNull(state))
	{
		return false;
	}

	//功能最新状态
	const int NewState = StringUtil::StringAsInt(state);

	//当为功能开启全部时ID = all，将所有功能开关状态设置为NewState
	if (0 == ::strcmp("all", ID))
	{
		LoopBeginCheck(a);	//防止死循环
		for (int i = 1; i < SWITCH_FUNCTION_MAX; ++i)
		{
			LoopDoCheck(a); //防止死循环
			// 将微信分享屏蔽在一键全开之外，因为微信分享暂时不在平台开放
// 			if (SWITCH_FUNCTION_SHARE_WEIXIN == i)
// 			{
// 				continue;
// 			}

			if (!SetFunctionEnable(pKernel, i, NewState))
			{
				return false;
			}
		}
	}
	else
	{
		//截取前缀ID = switch_1 改为 function_id = 1
		//当为一键多控的话 ID = switch_1,switch_2,switch_3改为function_id = 1,2,3
		CVarList section_list;
		StringUtil::SplitString(section_list, ID, ",");
		const int section_count = static_cast<int>(section_list.GetCount());

		LoopBeginCheck(b);	//防止死循环
		for (int i = 0; i < section_count; ++i)
		{
			LoopDoCheck(b);	//防止死循环
			CVarList function_list;
			StringUtil::SplitString(function_list, section_list.StringVal(i), "_");
			// 功能Id
			const int FunctionID = StringUtil::StringAsInt(function_list.StringVal(1));

			if (!SetFunctionEnable(pKernel, FunctionID, NewState))
			{
				return false;
			}
		}
	}
	
	return true;
}

//设置开启功能
bool SwitchManagerModule::SetFunctionEnable(IKernel* pKernel, const int function_id, const int newState)
{
    if (function_id <= SWITCH_FUNCTION_MIN || function_id >= SWITCH_FUNCTION_MAX)
    {
        return false;
    }

    const int tempNewState = newState > 0 ? 1 : 0;
	//得到开关公共数据状态
	CVarList func_list = m_SwitchManagerModule->QueryPubFunctionState(pKernel, function_id);
    const int curState = func_list.IntVal(0);
    if (tempNewState == curState)
    {
        return true; // 没变
    }

	//得到开关公共数据名称
	const std::string function_name = func_list.StringVal(1);
    // 发消息给公服更新信息
#ifndef FSROOMLOGIC_EXPORTS
    CVarList msg;
    msg << PUBSPACE_DOMAIN << m_SwitchManagerModule->GetDomainName(pKernel).c_str() << SP_DOMAIN_MSG_FUNCTION_STATE 
		<< function_id << tempNewState << function_name << SWITCH_REC_TYPE_CHANGE;
    pKernel->SendPublicMessage(msg);
#endif

	// 修改数据库中的功能开关状态
	m_SwitchManagerModule->SaveSwitchConfigToDB(pKernel, function_id, tempNewState, function_name);

    return true;
}

//查看功能开关公共数据状态
const CVarList& SwitchManagerModule::QueryPubFunctionState(IKernel* pKernel, const int function_id)
{
	static CVarList switch_list;
	//错误时条用
	switch_list << 1 << "";

    // 得到公共数据区
#ifndef FSROOMLOGIC_EXPORTS
    IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
    if (!pPubSpace)
    {
        return switch_list;
    }

    // 得到公共数据项
    IPubData* pSwitchPubData = pPubSpace->GetPubData(GetDomainName(pKernel).c_str());
    if (NULL == pSwitchPubData)
    {
        return switch_list;
    }

    if (!pSwitchPubData->FindRecord(DOMAIN_SWITCH_REC))
    {
        return switch_list;
    }

    IRecord* pSwitchRec = pSwitchPubData->GetRecord(DOMAIN_SWITCH_REC);
    if (NULL == pSwitchRec)
    {
        return switch_list;
    }

    const int row = pSwitchRec->FindInt(SWITCH_REC_COL_FUNCTION_ID, function_id);
    if (row < 0)
    {
        // 公服表中没找到该功能
        return switch_list;
    }
	
	//功能开关状态和名称
	const int func_state = pSwitchRec->QueryInt(row, SWITCH_REC_COL_CUR_STATE);
	const std::string func_name = pSwitchRec->QueryString(row, SWITCH_REC_COL_FUNCTION_NAME);

	switch_list.Clear();
	switch_list << func_state << func_name;
#endif
    return switch_list;
}

//保存功能开关配置到数据库中
bool SwitchManagerModule::SaveSwitchConfigToDB(IKernel* pKernel, const int function_id, const int newState, const std::string& function_name)
{
	//加前缀将function_id = 1 改为 functionID = switch_1;
	std::string functionID = "switch_";
	functionID += StringUtil::IntAsString(function_id);
	
	// 更新基本配置信息键值（nx_config表）
// 	if (!pKernel->UpdateManageBase(functionID.c_str(), util_string_as_widestr(function_name.c_str()).c_str(), NX_CONFIG_TYPE_SWITCH, L"", newState))
// 	{
// 		return false;
// 	}

	// 更新运营配置信息键值（nx_config表）
	//if (!pKernel->UpdateManageInfo(functionID.c_str(), L"", NX_CONFIG_TYPE_SWITCH, L"", newState, 0))
	//{
	//	return false;
	//}
	return true;
}

// 延迟加载公共数据到数据库
int SwitchManagerModule::CB_DomainDB(IKernel* pKernel, const PERSISTID& self, int slice)
{
	m_SwitchManagerModule->LoadResource(pKernel);

	return 0;
}

// 场景创建, 在场景上增加延迟心跳
int SwitchManagerModule::OnSceneCreate(IKernel* pKernel, const PERSISTID& scene, 
									const PERSISTID& sender, const IVarList& args)
{
	// 场景会分散在多个member上，所以确保每个member都有一个心跳
	// 每个member只响应自己加载的场景回调，所以此处所响应的场景都是这个member上的
	static bool alreadyAddBeat = false;
	if (alreadyAddBeat)
	{
		// 场景服务器已经存在这个心跳
		return 0;
	}

	// 计时器，延迟加载配置文件，确保在数据库连接成功后加载
	ADD_COUNT_BEAT(pKernel, scene, "SwitchManagerModule::CB_DomainDB", SWITCH_REC_DELAY_TIME, 1);

	alreadyAddBeat = true;

	return 0;
}

// 加载公共数据到数据库
bool SwitchManagerModule::LoadResource(IKernel* pKernel)
 {
#ifndef FSROOMLOGIC_EXPORTS
	// 得到公共数据区
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (!pPubSpace)
	{
		// 创建失败
		return false;
	}

	// 得到公共数据项
	IPubData* pSwitchPubData = pPubSpace->GetPubData(GetDomainName(pKernel).c_str());
	if (NULL == pSwitchPubData)
	{
		// 创建失败
		return false;
	}

	if (!pSwitchPubData->FindRecord(DOMAIN_SWITCH_REC))
	{
		// 创建失败
		return false;
	}

	IRecord* pSwitchRec = pSwitchPubData->GetRecord(DOMAIN_SWITCH_REC);
	if (NULL == pSwitchRec)
	{
		// 创建失败
		return false;
	}

	const int row = pSwitchRec->GetRows();
	const int DefineRow = static_cast<int>(SWITCH_FUNCTION_MAX) - 1;
	if (row != DefineRow)
	{
		// 创建失败
		return false;
	}

	LoopBeginCheck(c);	//防止死循环
	for ( int i = 1; i < SWITCH_FUNCTION_MAX; ++i )
	{
		LoopDoCheck(c);	//防止死循环
		int irow = pSwitchRec->FindInt(SWITCH_REC_COL_FUNCTION_ID, i);
		if (irow < 0)
		{
			// 创建失败
			return false;
		}

		std::string function_name = pSwitchRec->QueryString(irow, SWITCH_REC_COL_FUNCTION_NAME);
		int function_state = pSwitchRec->QueryInt(irow, SWITCH_REC_COL_CUR_STATE);

		m_SwitchManagerModule->SaveSwitchConfigToDB(pKernel, i, function_state, function_name);
	}
#endif
	return true;
};

//通知客户端
void SwitchManagerModule::SendCustomSysInfo(IKernel* pKernel, const PERSISTID& self, const int functin_id)
{
	if (!pKernel->Exists(self))
	{
		return;
	}
	//暂时做成统一提示
	//     char szBuff[32];
	//     SPRINTF_S(szBuff, "sys_switch_%d", functin_id);
	//CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, "sys_switch_functoin", CVarList());
}