

#include "AsynCtrlModule.h"
#include "FsGame/Interface/ILuaExtModule.h"
//#include "FsGame/SocialSystemModule/BattleTeamModule.h"
//#include "FsGame/SceneBaseModule/SceneInfoModule.h"
//#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/Define/CommandDefine.h"
//#include "FsGame/Define/NationDefine.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/QuickHash.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
//#include "FsGame/SceneBaseModule/SceneInfoModule.h"

AsynCtrlModule*		AsynCtrlModule::m_pAsynCtrlModule = NULL;

// 挖宝配置信息
#define SWITCH_LOCATE_CONFLICT_INFO     "ini/Scene/ConflictScene/conflict_scene.xml"

//传送场景所有玩家到目标场景（传场景批量优化测试）
int nx_switch_all_player( void* state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_switch_all_player, 2);

	CHECK_ARG_INT(state, nx_switch_all_player, 1);

	CHECK_ARG_INT(state, nx_switch_all_player, 2);

	//目标场景ID
	int desscene = pKernel->LuaToInt(state, 1);
	int switch_num = pKernel->LuaToInt(state, 2);

	int curid = pKernel->GetSceneId();

	CVarList playerlist;
	pKernel->GetScenePlayerList(curid, playerlist);

	int real_player_num = int(playerlist.GetCount());
	int real_switch_player_num = __min(real_player_num, switch_num);

	if (real_switch_player_num < 1)
	{
		pKernel->LuaPushBool(state, true);
		return 1;
	}

	LoopBeginCheck(a)
	for (int i = 0; i < real_switch_player_num; i++)
	{
		LoopDoCheck(a)
		PERSISTID& player = pKernel->FindPlayer(playerlist.WideStrVal(i));
		if (pKernel->Exists(player))
		{
			AsynCtrlModule::m_pAsynCtrlModule->SwitchBorn(pKernel, player, desscene);
		}
	}

	pKernel->LuaPushBool(state, true);

	return 1;
}

//传送目标对象到目标场景
int nx_switch_object( void* state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_switch_object, 2);

	// 检查每个参数类型
	CHECK_ARG_INT(state, nx_switch_object, 1);
	CHECK_ARG_OBJECT(state, nx_switch_object, 2);

	// 目标场景ID
	int desscene = pKernel->LuaToInt(state, 1);
	PERSISTID object = pKernel->LuaToObject(state, 2);

	int curid = pKernel->GetSceneId();

	if (!pKernel->Exists(object))
	{
		return 1;
	}

	if (curid == desscene)
	{
		// 不用切场景
		AsynCtrlModule::m_pAsynCtrlModule->MoveToBorn(pKernel, object, desscene);
	}
	else
	{
		// 需要切场景
		AsynCtrlModule::m_pAsynCtrlModule->SwitchBorn(pKernel, object, desscene);
	}		

	pKernel->LuaPushBool(state, true);

	return 1;
}

bool AsynCtrlModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	m_pAsynCtrlModule = this;

	pKernel->AddEventCallback("player", "OnReady", AsynCtrlModule::OnReady, -2);
	pKernel->AddEventCallback("player", "OnContinue", AsynCtrlModule::OnContinue, -1);

	DECL_HEARTBEAT(AsynCtrlModule::H_Asyn_SwitchLocate);
	DECL_HEARTBEAT(AsynCtrlModule::H_Asyn_SwitchBorn);

	DECL_LUA_EXT(nx_switch_all_player);
	DECL_LUA_EXT(nx_switch_object);

	// 加载资源
	//LoadResource(pKernel);

	return true;
}

bool AsynCtrlModule::Shut(IKernel* pKernel)
{
	return true;
}

// 资源加载
bool AsynCtrlModule::LoadResource(IKernel *pKernel)
{
	m_pAsynCtrlModule->m_MapConflictScene.clear();

	std::string xmlPath = pKernel->GetResourcePath();
	xmlPath += SWITCH_LOCATE_CONFLICT_INFO;

	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[AsynCtrlModule::LoadResource] failed!");
		return false;
	}

	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(b);	//防止死循环
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(b);	//防止死循环
		const char* section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		// 场景地点
		int SceneID = xml.ReadInteger(section, "SceneID", 0);
		// 冲突场景表
		std::string ConflictLists = xml.ReadString(section, "ConflictLists", "");

		m_MapConflictScene.insert(std::make_pair(SceneID, ConflictLists));
	}

	return true;
}

// 是否可传送到目标场景(DestSceneID克隆场景需要传入原场景)
bool AsynCtrlModule::CanSwitchLocate(IKernel* pKernel, const wchar_t* selfName, const int DestSceneID)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 开关
// 	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_SCENE_CONFLICT))
// 	{
// 		return true;
// 	}
#endif // FSROOMLOGIC_EXPORTS

	if (StringUtil::CharIsNull(selfName))
	{
		return false;
	}

	// 玩家当前场景
	int CurScene = pKernel->GetPlayerScene(selfName);

	// 克隆场景取原型场景
	if (CurScene >= CLONE_SCENE)
	{
		CurScene = pKernel->GetPrototypeSceneId(CurScene);
	}

 	if (!SceneToScene(CurScene, DestSceneID))
 	{
 		// 场景冲突，无法传送
		::CustomSysInfoByName(pKernel, selfName, SYSTEM_INFO_ID_3101, CVarList());
 
 		return false;
 	}

	return true;
}

// 场景切换
bool AsynCtrlModule::SceneToScene(const int curScene, const int targetScene)
{
	std::map<int, std::string>::iterator it = m_MapConflictScene.find(curScene);
	if (it == m_MapConflictScene.end())
	{
		return true;
	}

	CVarList ConflictList;
	util_split_string(ConflictList, it->second, ":");
	size_t ConflictCnt = ConflictList.GetCount();

	LoopBeginCheck(ab);		// 防止死循环
	for (size_t i = 0; i < ConflictCnt; ++i)
	{
		LoopDoCheck(ab);	// 防止死循环
		if (targetScene == ConflictList.IntVal(i))
		{
			return false;
		}
	}

	return true;
}
// 是否等候异步操作的过程中
bool AsynCtrlModule::IsWaiting(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pObj = pKernel->GetGameObj(self);

	if (NULL == pObj)
	{
		return false;
	}

	if (!pObj->FindAttr("AsynWait"))
	{
		return false;
	}

	if (pObj->QueryInt("AsynWait") <= 0)
	{
		return false;
	}
	
	unsigned int beginticks = pObj->QueryInt("AsynWaitBeginTime");
	unsigned int now = ::util_get_ticks();

	if (now - beginticks > 600 * 1000)
	{
		// 超过600秒钟还没被设回来，不管了
		EndWait(pKernel, self);
		return false;
	}

	return true;
}

// 开始等候异步操作的过程
bool AsynCtrlModule::BeginWait(IKernel* pKernel, const PERSISTID& self, int asyn_type)
{
	IGameObj* pObj = pKernel->GetGameObj(self);
	if (NULL == pObj)
	{
		return false;
	}

	pObj->SetInt("AsynWait", asyn_type);
	pObj->SetInt("AsynWaitBeginTime", ::util_get_ticks());

	return true;
}

// 结束等候异步操作的过程
bool AsynCtrlModule::EndWait(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pObj = pKernel->GetGameObj(self);
	if (NULL == pObj)
	{
		return false;
	}

	if (pObj->QueryInt("AsynWait") > 0)
	{
		pObj->SetInt("AsynWait", 0);
	}

	return true;
}

// 异步场景切换接口 
// 切换场景到指定的场景号和坐标
bool AsynCtrlModule::SwitchLocate(IKernel* pKernel, const PERSISTID& self, int scene,
								  float x, float y, float z, float orient, const bool bFlag)
{
#ifndef FSROOMLOGIC_EXPORTS
// 	if (!bFlag
// 		&& !SceneInfoModule::m_pSceneInfoModule->IsCanEnterScene(pKernel, scene, self))
// 	{
// 		return false;
// 	}
#endif // FSROOMLOGIC_EXPORTS

	if (pKernel->FindHeartBeat(self, "AsynCtrlModule::H_Asyn_SwitchLocate"))
	{
		//如果已经等待中了不处理了
		return false;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);

	if (pSelf == NULL)
	{		
		return false;
	}

	if (!IsWaiting(pKernel, self) )	
	{
		BeginWait(pKernel, self, ASYN_TYPE_SWITCHLOCATE);
		if (pKernel->SwitchLocate(self, scene, x, y, z, orient))
		{			
			return true;
		}
		else
		{
			EndWait(pKernel, self);
			return false;
		}
	}

	if (!pSelf->FindData("AsynSwitchLocateScene"))
	{
		pSelf->AddDataInt("AsynSwitchLocateScene", scene);
		pSelf->AddDataFloat("AsynSwitchLocateX", x);
		pSelf->AddDataFloat("AsynSwitchLocateY", y);
		pSelf->AddDataFloat("AsynSwitchLocateZ", z);
		pSelf->AddDataFloat("AsynSwitchLocateO", orient);
	}

	pSelf->SetDataInt("AsynSwitchLocateScene", scene);
	pSelf->SetDataFloat("AsynSwitchLocateX", x);
	pSelf->SetDataFloat("AsynSwitchLocateY", y);
	pSelf->SetDataFloat("AsynSwitchLocateZ", z);
	pSelf->SetDataFloat("AsynSwitchLocateO", orient);

	pKernel->AddHeartBeat(self, "AsynCtrlModule::H_Asyn_SwitchLocate", 1000);
	
	return true;
}

// 切换到场景的出生点
bool AsynCtrlModule::SwitchBorn(IKernel* pKernel, const PERSISTID& self, int scene, const bool bFlag)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return false;
	}

#ifndef FSROOMLOGIC_EXPORTS
// 	if (!bFlag
// 		&& !SceneInfoModule::m_pSceneInfoModule->IsCanEnterScene(pKernel, scene, self))
// 	{
// 		return false;
// 	}
#endif // FSROOMLOGIC_EXPORTS

	if (!IsWaiting(pKernel, self))
	{
		m_pAsynCtrlModule->BeginWait(pKernel, self, ASYN_TYPE_SWITCHLOCATE);

		if(!pKernel->SwitchBorn(self, scene))
		{
			m_pAsynCtrlModule->EndWait(pKernel,self);
			return false;
		}
		return true;
	}

	if (!pPlayer->FindData("AsynSwitchBornScene"))
	{
		pPlayer->AddDataInt("AsynSwitchBornScene", scene);
	}
	else
	{
		pPlayer->SetDataInt("AsynSwitchBornScene", scene);
	}

	pKernel->AddHeartBeat(self, "AsynCtrlModule::H_Asyn_SwitchBorn", 1000);

	return true;
}

// 移动到当前场景出生点
bool AsynCtrlModule::MoveToBorn(IKernel* pKernel, const PERSISTID& self, int scene)
{
	if (!pKernel->GetSceneExists(scene))
	{
		return false;
	}

	float x, y, z, orient;
	pKernel->GetSceneBorn(x, y, z, orient);

	pKernel->MoveTo(self, x, y, z, orient);

#ifndef FSROOMLOGIC_EXPORTS
//	BattleTeamModule::m_pBattleTeamModule->AddLittleStepGridRec(pKernel, self, x, z);
#endif // FSROOMLOGIC_EXPORTS

	return true;
}

// 异步切换场景的心跳
int AsynCtrlModule::H_Asyn_SwitchLocate(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	if (m_pAsynCtrlModule->IsWaiting(pKernel, self))
	{
		return 0;
	}

	int scene = pPlayer->QueryDataInt( "AsynSwitchLocateScene");

#ifndef FSROOMLOGIC_EXPORTS
// 	if (!SceneInfoModule::m_pSceneInfoModule->IsCanEnterScene(pKernel, scene, self))
// 	{
// 		return 0;
// 	}
#endif // FSROOMLOGIC_EXPORTS

	pKernel->RemoveHeartBeat(self, "AsynCtrlModule::H_Asyn_SwitchLocate");

	float x = pPlayer->QueryDataFloat("AsynSwitchLocateX");
	float y = pPlayer->QueryDataFloat("AsynSwitchLocateY");
	float z = pPlayer->QueryDataFloat("AsynSwitchLocateZ");
	float orient = pPlayer->QueryDataFloat("AsynSwitchLocateO");

	m_pAsynCtrlModule->BeginWait(pKernel, self, ASYN_TYPE_SWITCHLOCATE);
	
	if (!pKernel->SwitchLocate(self, scene, x, y, z, orient))
	{
		m_pAsynCtrlModule->EndWait(pKernel, self);
	}
	
	return 0;
}

int AsynCtrlModule::H_Asyn_SwitchBorn(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	if (m_pAsynCtrlModule->IsWaiting(pKernel, self))
	{
		return 0;
	}

	int scene = pPlayer->QueryDataInt("AsynSwitchBornScene");

#ifndef FSROOMLOGIC_EXPORTS
// 	if (!SceneInfoModule::m_pSceneInfoModule->IsCanEnterScene(pKernel, scene, self))
// 	{
// 		return 0;
// 	}
#endif // FSROOMLOGIC_EXPORTS

	pKernel->RemoveHeartBeat(self, "AsynCtrlModule::H_Asyn_SwitchBorn");
	pKernel->SwitchBorn(self, scene);

	return 0;
}

int AsynCtrlModule::OnReady(IKernel * pKernel, const PERSISTID & self, 
							const PERSISTID & sender, const IVarList & args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	if (ASYN_TYPE_SWITCHLOCATE != pPlayer->QueryInt("AsynWait"))
	{
		return 0;
	}

	// 进入场景
	m_pAsynCtrlModule->EndWait(pKernel, self);

	return 0;
}

int AsynCtrlModule::OnContinue(IKernel * pKernel, const PERSISTID & self, 
					  const PERSISTID & sender, const IVarList & args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int nAsynType = pPlayer->QueryInt("AsynWait");
	if (ASYN_TYPE_SWITCHLOCATE != nAsynType )
	{
		return 0;
	}

	// 进入场景
	m_pAsynCtrlModule->EndWait(pKernel, self);

	return 0;
}
