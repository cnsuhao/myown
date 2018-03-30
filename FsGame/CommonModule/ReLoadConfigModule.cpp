//--------------------------------------------------------------------
// 文件名:		ReLoadConfigModule.h
// 内  容:		重新加载配置
// 说  明:		
// 创建日期:	2016年09月13日
// 整理日期:	2016年09月13日
// 创建人:		刘明飞  
//--------------------------------------------------------------------
#include "ReLoadConfigModule.h"
#include "FsGame\Define\CommandDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "server\KnlConst.h"
#include "FsGame\Define\PubDefine.h"
#include "FsGame\Define\SceneListDefine.h"
#include "utils\string_util.h"
#include "utils\extend_func.h"

ReLoadConfigModule* ReLoadConfigModule::m_pReLoadConfigModule = NULL;

//gm重载配置文件
int nx_reload_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_reload_config, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_reload_config, 1);


	const char* strKey = pKernel->LuaToString(state, 1);

	pKernel->LuaPushBool(state, ReLoadConfigModule::m_pReLoadConfigModule->SendReloadConfigCommand(pKernel, strKey));
	return 1;
}

// 初始化
bool ReLoadConfigModule::Init(IKernel* pKernel)
{
	m_pReLoadConfigModule = this;
	//gm重载配置文件
	pKernel->AddIntCommandHook("scene", COMMAND_MSG_REREAD_CFG_GM, ReLoadConfigModule::OnCommandRereadLoadResource);

	DECL_LUA_EXT(nx_reload_config);

	m_mapReloadConfig.clear();
	return true;
}

// 释放
bool ReLoadConfigModule::Shut(IKernel* pKernel)
{
	return true;
}

// 注册重载配置回调
void ReLoadConfigModule::RegisterReloadConfig(const char* strKey, RELOAD_CONFIG_CALLBACK pReloadCall)
{
	if (NULL == strKey || StringUtil::CharIsNull(strKey) || NULL == pReloadCall)
	{
		return;
	}
	ReloadConfigCallMap::iterator iter = m_mapReloadConfig.find(strKey);
	if (iter == m_mapReloadConfig.end())
	{
		m_mapReloadConfig.insert(std::make_pair(strKey, pReloadCall));
	}
}

// 发送重载配置请求
bool ReLoadConfigModule::SendReloadConfigCommand(IKernel* pKernel, const char* strKey)
{
	if (StringUtil::CharIsNull(strKey))
	{
		return false;
	}
	// 公共空间
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return false;
	}

	// 公共数据
	wchar_t pubDataName[256];
	SWPRINTF_S(pubDataName, L"Domain_SceneList%d", pKernel->GetServerId());
	IPubData* pPubData = pPubSpace->GetPubData(pubDataName);
	if (pPubData == NULL)
	{
		return false;
	}

	// 上架表
	IRecord* pRecord = pPubData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
	if (pRecord == NULL)
	{
		return false;
	}

	CVarList msg;
	msg << COMMAND_MSG_REREAD_CFG_GM
		<< strKey;

	int rows = pRecord->GetRows();
	LoopBeginCheck(a);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(a);
		pKernel->CommandToScene(pRecord->QueryInt(i, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id), msg);
	}

	return true;
}

// 重载配置文件
int ReLoadConfigModule::OnCommandRereadLoadResource(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	const char* strKey = args.StringVal(1);
	ReloadConfigCallMap::iterator iter = m_pReLoadConfigModule->m_mapReloadConfig.find(strKey);
	if (iter == m_pReLoadConfigModule->m_mapReloadConfig.end())
	{
		return 0;
	}

	RELOAD_CONFIG_CALLBACK pReloadCall = iter->second;
	if (NULL == pReloadCall)
	{
		return 0;
	}

	pReloadCall(pKernel);

	extend_warning(LOG_WARNING, "RereadLoadResource %s success", strKey);
	return 0;
}