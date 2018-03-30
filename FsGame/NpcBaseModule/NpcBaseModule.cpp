//--------------------------------------------------------------------
// 文件名:      NpcBaseModule.h
// 内  容:      NPC基础模块
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#include "NpcBaseModule.h"
#include "utils/util_func.h"
#include "FsGame/CommonModule/LuaScriptModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
//#include "FsGame/Define/SceretSceneDefine.h"

const char* const NPC_CONFIG_FILE = "ini/npc/PathConfig.xml";

NpcBaseModule* NpcBaseModule::m_pNpcBaseModule = NULL;

// 重加载NPC基础配置文件
int nx_reload_npc_base_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	
	return true == NpcBaseModule::m_pNpcBaseModule->LoadNpcPathConfig(pKernel) ? 1 : 0;

}

// 初始化
bool NpcBaseModule::Init(IKernel* pKernel)
{
	NpcBaseModule::m_pNpcBaseModule = this;
	// 注册回调
	pKernel->AddLogicClass("npc", TYPE_NPC, "");
	pKernel->AddEventCallback("npc", "OnLoad", OnLoad);

	LoadNpcPathConfig(pKernel);
	DECL_LUA_EXT(nx_reload_npc_base_config);
	return true;
}

// 释放
bool NpcBaseModule::Shut(IKernel* pKernel)
{
	return true;
}

// 载入回调
int NpcBaseModule::OnLoad(IKernel* pKernel, const PERSISTID& self, 
						  const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	pSelfObj->SetInt("Type", TYPE_NPC);
	pSelfObj->SetString("Config", pKernel->GetConfig(self));

	return 0;
}

// 加载物品配置
bool NpcBaseModule::LoadNpcPathConfig(IKernel* pKernel)
{
	std::string xmlPath = pKernel->GetResourcePath();
	xmlPath += NPC_CONFIG_FILE;

	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[NpcBaseModule::npc_config] failed");
		return false;
	}

	const char* ITEM_DIR = "ini/npc/";
	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a)
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a)
		const char* section = xml.GetSectionByIndex(i);
		const char* path = xml.ReadString(section, "Path", "");
		if (0 == path || 0 == strcmp("", path))
		{
			continue;
		}

		std::string tmp_file_name = ITEM_DIR;
		tmp_file_name += path;
		if (!pKernel->PreloadConfig(tmp_file_name.c_str()))
		{
			extend_warning(LOG_ERROR, "Load LoadNpcPathConfig %s Error!", tmp_file_name.c_str());
		}
	}

	return true;
}