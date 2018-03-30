#include "SceneBaseModule.h"
#include "utils/util_func.h"
#include "FsGame/Define/PubDefine.h"
//#include "FsGame/Define/NationDefine.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/Define/GameDefine.h"
#include <ctime>
#include "utils/string_util.h"
#include "CommonModule/LandPosModule.h"
//#include "FsGame/Middle/MiddleModule.h"

// 初始化
bool SceneBaseModule::Init(IKernel* pKernel)
{
	pKernel->AddLogicClass("scene", TYPE_SCENE, "");

	pKernel->AddClassCallback("scene",  "OnCreateClass", OnCreateClass);

	pKernel->AddEventCallback("scene", "OnCreate", SceneBaseModule::OnCreate);
	pKernel->AddEventCallback("scene", "OnDestroy", SceneBaseModule::OnDestroy);
	pKernel->AddEventCallback("player", "OnEntry", SceneBaseModule::OnEntry);
	pKernel->AddEventCallback("player", "OnLeaveScene", SceneBaseModule::OnLeave);

//	pKernel->AddEventCallback("scene", "OnPublicMessage", OnPublicMessage);

	return true;
}

// 释放
bool SceneBaseModule::Shut(IKernel* pKernel)
{
	return true;
}

int SceneBaseModule::OnCreateClass(IKernel* pKernel, int index)
{
	createclass_from_xml(pKernel, index, "struct\\scene\\scene.xml");
	return 0;
}

int SceneBaseModule::OnCreate(IKernel* pKernel, const PERSISTID& self, 
							  const PERSISTID& sender, const IVarList& args)
{
	//出生点
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	const char * born = pSelf->QueryString("Born");
	if (strlen(born) > 0)
	{
		float x, y, z, orient;
		int temp;
		if (pKernel->GetSwitchLocation(born, temp, x, y, z, orient))
			pKernel->SetSceneBorn(x, pKernel->GetMapHeight(x, z), z, orient);
	}

	//设定国家属性
	int sceneid = pKernel->GetSceneId();
	if ( sceneid > 1024 )
	{
		sceneid = pKernel->GetPrototypeSceneId(sceneid);
	}

	pSelf->SetInt("ID", sceneid);
	//1-500代表每个国家场景
// 	int iNation = GetNationTypeBySceneId(sceneid);
// 	if (iNation != NATION_NONE)
// 	{
// 		pSelf->SetInt("Nation", iNation);
// 	}
	
#ifndef FSROOMLOGIC_EXPORTS
	// 请求注册
	//MiddleModule::Instance()->OnRequireRegsit(pKernel);
	// 公共空间名称
	static std::wstring strPubSceneList = 
		std::wstring(DOMAIN_SCENELIST) + StringUtil::IntAsWideStr(pKernel->GetServerId());

	CVarList msg;
	msg << PUBSPACE_DOMAIN;
	msg << strPubSceneList;
	msg << SP_DOMAIN_MSG_SCENELIST_ADD_SCENE;
	msg << pKernel->GetSceneId();
	msg << pKernel->GetConfig(self);
	msg << pKernel->GetMemberId();

	pKernel->SendPublicMessage(msg);
#endif // FSROOMLOGIC_EXPORTS
	
	return 0;
}

int SceneBaseModule::OnDestroy(IKernel* pKernel, const PERSISTID& self, 
							   const PERSISTID& sender, const IVarList& args)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 公共空间名称
	static std::wstring strPubSceneList = 
		std::wstring(DOMAIN_SCENELIST) + StringUtil::IntAsWideStr(pKernel->GetServerId());

	CVarList msg;
	msg << PUBSPACE_DOMAIN;
	msg << strPubSceneList;
	msg << SP_DOMAIN_MSG_SCENELIST_DEL_SCENE;
	msg << pKernel->GetSceneId();
	//msg << pKernel->GetConfig(self);

	pKernel->SendPublicMessage(msg);
#endif // FSROOMLOGIC_EXPORTS

	return 0;
}

int SceneBaseModule::OnEntry(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	//保护跨场景切换玩家的高度
	len_t dx, dy, dz, oo;
	pKernel->GetLocation(self, dx, dy, dz, oo);

	len_t sceneY = dy;
	// 不是第一次进入游戏,而是切换场景
	if (pSelf->FindData("EntrySceneTime"))
	{
		sceneY = pKernel->GetMapHeight(dx, dz);
	}

	pKernel->Locate(self, dx, sceneY, dz, oo);

	ADD_DATA_INT64(pSelf, "EntrySceneTime");
	pSelf->SetDataInt64("EntrySceneTime", ::time(NULL));
	return 0;
}


int SceneBaseModule::OnLeave(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf || !pKernel->Exists(sender))
	{
		return 0;
	}

	//记录进入场景临时属性用作日志(add by SunJian 2015-04-25)
	if (pSelf->FindData("EntrySceneTime"))
	{
#ifndef FSROOMLOGIC_EXPORTS
		SceneLog log;
		log.loginTime = pSelf->QueryDataInt64("EntrySceneTime");
		log.logoutTime = ::time(NULL);
		LogModule::m_pLogModule->SaveSceneLog(pKernel, self, log);
#endif // FSROOMLOGIC_EXPORTS

		//pSelf->RemoveData("EntrySceneTime");
	}
	return 0;
}

void SceneBaseModule::KickAllPlayerOutScene(IKernel* pKernel, int nGroup)
{
	CVarList playerList;
	pKernel->GetChildList( pKernel->GetScene(), TYPE_PLAYER, playerList );
	for (size_t i = 0; i < playerList.GetCount(); i++)
	{
		LandPosModule::m_pLandPosModule->PlayerReturnLandPosi(pKernel, playerList.ObjectVal(i));
	}
}

void SceneBaseModule::ClearNonPlayerInScene(IKernel* pKernel, int nGroup)
{
	CVarList playerList;
	pKernel->GetChildList(pKernel->GetScene(), TYPE_NPC, playerList);
	for (size_t i = 0; i < playerList.GetCount(); i++)
	{
		PERSISTID& object_ = playerList.ObjectVal(i);
		const char* pszScript_ = pKernel->GetScript(object_);
// 		if (strcmp(pszScript_, CLASS_NAME_PET) == 0)
// 		{
// 			continue;
// 		}

		pKernel->DestroySelf(object_);
	}
}