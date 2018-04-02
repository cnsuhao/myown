//---------------------------------------------------------
//文件名:       MapModule.h
//内  容:       地图相关操作模块
//说  明:       
//				
//创建日期:      2015年4月21日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#include "MapModule.h"
#include "FsGame/Define/MapDefine.h"
#include "utils/extend_func.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/SystemFunctionModule/ToolBoxModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "utils/custom_func.h"
#include <algorithm>
#include "FsGame/CommonModule/LuaExtModule.h"
#include "CommonModule/EnvirValueModule.h"
#include "CommonModule/ReLoadConfigModule.h"

#ifdef _DEBUG
// 测试
int MapModule::nx_map_switch_scene(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_map_switch_scene, 2);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_map_switch_scene, 1);
	CHECK_ARG_INT(state, nx_map_switch_scene, 2);
	PERSISTID player = pKernel->LuaToObject(state, 1);
	int sceneId = pKernel->LuaToInt(state, 2);

	CVarList var;
	var << CLIENT_CUSTOMMSG_MAP
		<< CS_REQUEST_MAP_SWITCH_SCENE
		<< sceneId;
	m_pMapModule->OnCustomCallBack(pKernel, player, player, var);

	pKernel->LuaPushBool(state, true);
	return 1;
}
#endif

// 初始化静态指针
MapModule* MapModule::m_pMapModule = NULL;
AsynCtrlModule* MapModule::m_pAsynCtrlModule = NULL;

// 初始化
bool MapModule::Init(IKernel* pKernel)
{
	// 赋予自身
	m_pMapModule = this;
	m_pAsynCtrlModule = dynamic_cast<AsynCtrlModule*>(pKernel->GetLogicModule("AsynCtrlModule"));

	assert(m_pMapModule != NULL 
		&& m_pAsynCtrlModule != NULL);

	// 注册进入场景回调
	pKernel->AddEventCallback("player", "OnAfterEntryScene", MapModule::OnPlayerEntry);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_MAP, MapModule::OnCustomCallBack);

	// 加载资源
	LoadResource(pKernel);
#ifdef _DEBUG
	// 测试用
	DECL_LUA_EXT(nx_map_switch_scene);
#endif

	RELOAD_CONFIG_REG("MapConfig", MapModule::ReLoadConfig);
	return true;
}

// 销毁
bool MapModule::Shut(IKernel* pKernel)
{
	return true;
}

bool MapModule::LoadResource(IKernel* pKernel)
{
	return true;
}

// 进入场景
int MapModule::OnPlayerEntry(IKernel * pKernel, const PERSISTID & self,
									 const PERSISTID & sender, const IVarList & args)
{
	return 0;
}

// 接收客户端信息
int MapModule::OnCustomCallBack(IKernel * pKernel, const PERSISTID & self,
										const PERSISTID & sender, const IVarList & args)
{
	// 验证对象是否存在
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 检验消息个数
	if (args.GetCount() < 2)
	{
		return 0;
	}

	// 取消息id
	int subMsgId = args.IntVal(1);
	switch (subMsgId)
	{
	case CS_REQUEST_MAP_SWITCH_SCENE:
		m_pMapModule->OnCustomSwitchScene(pKernel, self, sender, args);
		break;
	}

	return 0;
}

// 请求切换场景
int MapModule::OnCustomSwitchScene(IKernel * pKernel, const PERSISTID & self,
										const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 验证参数个数
	if (args.GetCount() != 3)
	{
		return 0;
	}


	// 取场景Id
	int sceneId = args.IntVal(2);

	// 验证场景是否存在
	if (!pKernel->GetSceneExists(sceneId))
	{
		extend_warning(LOG_ERROR, "[%s][%d] scene not exist %d", __FUNCTION__, __LINE__, sceneId);
		return 0;
	}

	// 战斗状态是否可以切换场景
	int fightState = pSelfObj->QueryInt("FightState");
	if (fightState > 0)
	{
		return 0;
	}

// 	int curSceneID = pKernel->GetSceneId();
// 	if (curSceneID > OUTDOOR_SCENE_MAX || sceneId > OUTDOOR_SCENE_MAX)
// 	{
// 		return 1;
// 	} 



	// 切场景
	if (!m_pAsynCtrlModule->SwitchBorn(pKernel, self, sceneId))
	{
		// 提示客户端 切场景失败
	//	::CustomSysInfo(pKernel, self, SYS_INFO_MAP_STRING8, args);
		return 0;
	}
	
	return 1;
}

void MapModule::ReLoadConfig(IKernel * pKernel)
{
	m_pMapModule->LoadResource(pKernel);
}