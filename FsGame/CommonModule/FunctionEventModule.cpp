//--------------------------------------------------------------------
// 文件名:      FunctionEventModule.h
// 内  容:      游戏功能配置模块
// 说  明:
// 创建日期:    2017年02月10日
// 创建人:      李海罗
//--------------------------------------------------------------------
#include "FunctionEventModule.h"
#include "FsGame/Define/ContainerDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "ReLoadConfigModule.h"
#endif
#include "utils/string_util.h"

// 常量配置路径
#define PATH_FUNCTION_EVENT_CONFIG "ini/SystemFunction/FunctionEvent.xml"		

FunctionEventModule::FuncInfo FunctionEventModule::ms_funcInfo[FUNCTION_EVENT_ID_END] = { { ITEM_NOT_BOUND, 0 } };

bool FunctionEventModule::Init(IKernel *pKernel)
{
	ReloadConfig(pKernel);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("FunctionEventConfig", FunctionEventModule::ReloadConfig);
#endif
	return true;
}

bool FunctionEventModule::Shut(IKernel *pKernel)
{
	return true;
}

// 查询int常量
int FunctionEventModule::GetItemBindState( int nFunctionId )
{
	if (nFunctionId < FUNCTION_EVENT_ID_SYS || nFunctionId >= FUNCTION_EVENT_ID_END )
	{
		return ITEM_NOT_BOUND;
	}

	return ms_funcInfo[nFunctionId].bind_state ? ITEM_BOUND : ITEM_NOT_BOUND;
}



bool FunctionEventModule::GetItemBindPriorUse(int nFunctionId)
{
	if (nFunctionId < FUNCTION_EVENT_ID_SYS || nFunctionId >= FUNCTION_EVENT_ID_END)
	{
		return false;
	}

	return ms_funcInfo[nFunctionId].bind_prior_use;
}

// 重新加载配置
void FunctionEventModule::ReloadConfig(IKernel *pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += PATH_FUNCTION_EVENT_CONFIG;

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		::extend_warning(LOG_ERROR, "not exist %s", pathName.c_str());
		return;
	}

	// 重置所有状态
	memset(ms_funcInfo, 0, sizeof(ms_funcInfo));

	// 变量个数与枚举个数要一致
	const int iSectionCount = (int)xmlfile.GetSectionCount();
	LoopBeginCheck(a);
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(a);
		const char* section = xmlfile.GetSectionByIndex(i);
		int nIndex = EnumIndex<EmFunctionEventId>(section);
		if (nIndex < FUNCTION_EVENT_ID_SYS || nIndex >= FUNCTION_EVENT_ID_END)
		{
			::extend_warning(LOG_WARNING, "[FunctionEventModule::ReloadConfig] function id overflow[id:%s]", section);
			continue;
		}

		int nBindState = ITEM_NOT_BOUND;
		int nBindPriorUse = 0;
		const char* strValue = xmlfile.ReadString(section, "BindState", "");
		if (!StringUtil::CharIsNull(strValue))
		{
			nBindState = StringUtil::StringAsInt(strValue);
		}
		strValue = xmlfile.ReadString(section, "BindPriorUse", "");
		if (!StringUtil::CharIsNull(strValue))
		{
			nBindPriorUse = StringUtil::StringAsInt(strValue);
		}

		ms_funcInfo[nIndex].bind_state = nBindState == ITEM_BOUND;
		ms_funcInfo[nIndex].bind_prior_use = nBindPriorUse != 0;
	}
}