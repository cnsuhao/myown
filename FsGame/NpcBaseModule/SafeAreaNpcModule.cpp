//--------------------------------------------------------------------
// 文件名:		SafeAreaNpc.cpp
// 内  容:		安全区NPC
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2015年03月30日
// 创建人:		  ( )
// 修改人:        ( )
//    :	    
//--------------------------------------------------------------------

#include "SafeAreaNpcModule.h"
#include "FsGame/CommonModule/LuaScriptModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
//#include "FsGame/SocialSystemModule/ReviveManagerModule.h"
#include "FsGame/Define/GameDefine.h"
//#include "FsGame/Define/SafeAreaSysInfoDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/exptree.h"
#include "utils/XmlFile.h"
#include "utils/util_func.h"
#include "../Define/FightDefine.h"
#include "../Interface/FightInterface.h"
#include "Define/CommandDefine.h"
#include "Define/SafeAreaSysInfoDefine.h"
#include "CommonModule/CommRuleModule.h"

#define SAFE_AREA_NPC_OLD           0   // 老的安全区
#define SAFE_AREA_NPC_ALL_NATION    1   // 中立安全区
#define SAFE_AREA_NPC_OWN_NATION    2   // 保护本国玩家
#define SAFE_AREA_NPC_ENEMY_NATION  3   // 保护敌国玩家

#define SAFE_AREA_NPC_ENTRY  0   // 进入安全区
#define SAFE_AREA_NPC_LEAVE  1   // 离开安全区

AsynCtrlModule * SafeAreaNpcModule::m_pAsynCtrlModule = NULL;
SafeAreaNpcModule * SafeAreaNpcModule::m_pSafeAreaNpc = NULL;

static const char* RECT_AREA_INDEX = "RectAreaIndex";

// 初始化
bool SafeAreaNpcModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	//初始化指针
	m_pSafeAreaNpc = this;

	m_pAsynCtrlModule = static_cast<AsynCtrlModule*>(pKernel->GetLogicModule("AsynCtrlModule"));

	Assert( NULL != m_pAsynCtrlModule );

	pKernel->AddEventCallback("SafeAreaNpc","OnSpring", SafeAreaNpcModule::OnSpring);
	pKernel->AddEventCallback("SafeAreaNpc", "OnEndSpring", SafeAreaNpcModule::OnEndSpring);
	pKernel->AddEventCallback("SafeAreaNpc", "OnEntry", SafeAreaNpcModule::OnEntry);
	pKernel->AddEventCallback("SafeAreaNpc", "OnDestroy", SafeAreaNpcModule::OnDestroy);
	pKernel->AddEventCallback("player","OnLeaveScene",SafeAreaNpcModule::OnLeaveScene);

	pKernel->AddEventCallback("player", "OnEntry", SafeAreaNpcModule::OnPlayerEnterScene);
	pKernel->AddIntCommandHook("player", COMMAND_GROUP_CHANGE, SafeAreaNpcModule::OnCommandChangeGroup);

//	DECL_HEARTBEAT(SafeAreaNpcModule::HB_CheckRectangularSafeArea);
													   
// 	if (!LoadRectanularSafeAreaConfig(pKernel))
// 	{
// 		return false;
// 	}

	return true;
}

// 释放
bool SafeAreaNpcModule::Shut(IKernel* pKernel)
{
	return true;
}

/*!
* @brief	触发回调
* @param    引擎指针
* @param    安全区NPC对象
* @param    玩家对象
* @param    回调携带参数
* @return	bool
*/
int SafeAreaNpcModule::OnSpring(IKernel* pKernel, const PERSISTID& self, 
						  const PERSISTID& sender, const IVarList& args)
{
	//1、验证对象是否存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL || !pKernel->Exists(sender))
	{
		return 0;
	}

	//2、验证是否为玩家对象
	if ( TYPE_PLAYER != pKernel->Type(sender) ) 
	{
		if (pKernel->Type(sender) != TYPE_NPC)
		{
			return 0;
		}

// 		const char *script = pKernel->GetScript(sender);
// 		if (strcmp(script, CLASS_NAME_ESCORT_NPC) != 0 
// 			&& strcmp(script, CLASS_NAME_BATTLE_NPC) != 0)
// 		{
// 			return 0;
// 		}
	}

	int nNation = 0/*pSelfObj->QueryInt(FIELD_PROP_NATION)*/;
	int nSafeRule = pSelfObj->QueryInt(FIELD_PROP_SAFE_RULE);

	//3、更新进入安全区属性状态
	m_pSafeAreaNpc->UpdateEntryState(pKernel, sender, nNation, nSafeRule);

	return 0;
}

/*!
* @brief	结束触发回调
* @param    引擎指针
* @param    安全区NPC对象
* @param    玩家对象
* @param    回调携带参数
* @return	bool
*/
int SafeAreaNpcModule::OnEndSpring(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	//1、验证对象是否存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL || !pKernel->Exists(sender) ) 
	{
		return 0;
	}

	//2、验证是否为玩家对象
	if ( TYPE_PLAYER != pKernel->Type(sender) ) 
	{
		if (pKernel->Type(sender) != TYPE_NPC)
		{
			return 0;
		}

// 		if (strcmp(pKernel->GetScript(sender), CLASS_NAME_ESCORT_NPC) != 0)
// 		{
// 			return 0;
// 		}
	}

	int nNation = 0/*pSelfObj->QueryInt(FIELD_PROP_NATION)*/;
	int nSafeRule = pSelfObj->QueryInt(FIELD_PROP_SAFE_RULE);

	//3、更新离开安全区属性状态
	m_pSafeAreaNpc->UpdateLeaveState(pKernel, sender, nNation, nSafeRule);

	return 0;
}

/*!
* @brief	安全区NPC销毁
* @param    引擎指针
* @param    安全区NPC对象
* @param    场景对象
* @param    回调携带参数
* @return	bool
*/
int SafeAreaNpcModule::OnDestroy(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	//1、验证自身对象是否为空
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (!pKernel->Exists(sender) ) 
	{
		return 0;
	}

	//2、验证是否为:Npc类型与场景类型
	if ( TYPE_NPC != pKernel->Type(self)
		|| TYPE_SCENE != pKernel->Type(sender) ) 
	{
		return 0;
	}

	int nNation = 0/*pSelfObj->QueryInt(FIELD_PROP_NATION)*/;
	int nSafeRule = pSelfObj->QueryInt(FIELD_PROP_SAFE_RULE);

	//3、获取Npc有效范围:默认100.0f
	float distance = 100.0f;
	if ( pSelfObj->FindAttr("SpringRange") )
	{
		distance = pSelfObj->QueryFloat("SpringRange");
		distance = distance * 2;
	}

	//4、获取Npc有效范围内的账户列表
	CVarList CResult;
	pKernel->GetAroundList(self, distance, TYPE_PLAYER, 0, CResult);
	int nCount = static_cast<int>(CResult.GetCount());

	//5、遍历设置安全区内账户对应的属性
	LoopBeginCheck(a)
	for( int i = 0; i < nCount; ++i )
	{
		LoopDoCheck(a)
		//5.1、验证对象有效性
		PERSISTID obj = CResult.ObjectVal(i);
		if (!pKernel->Exists(obj)) 
		{
			continue;
		}

		//5.2、更新离开安全区属性状态
		m_pSafeAreaNpc->UpdateLeaveState(pKernel, obj, nNation, nSafeRule);
	}

	return 0;
}

/*!
* @brief	安全区NPC进入场景
* @param    引擎指针
* @param    安全区NPC对象
* @param    场景对象
* @param    回调携带参数
* @return	bool
*/
int SafeAreaNpcModule::OnEntry(IKernel * pKernel, const PERSISTID & self,
						 const PERSISTID & sender, const IVarList & args)
{
	//1、验证自身对象是否为空
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (!pKernel->Exists(sender) ) 
	{
		return 0;
	}

    //2、验证是否为:Npc类型与场景类型
	if ( TYPE_NPC != pKernel->Type(self)
		|| TYPE_SCENE != pKernel->Type(sender) ) 
	{
		return 0;
	}

	//3、获取Npc有效范围:默认0.0f
	float distance = 0.0f;
	if ( pSelfObj->FindAttr("SpringRange") )
	{
		distance = pSelfObj->QueryFloat("SpringRange");
	}

	int nNation = 0/*pSelfObj->QueryInt(FIELD_PROP_NATION)*/;
	int nSafeRule = pSelfObj->QueryInt(FIELD_PROP_SAFE_RULE);

	//4、获取Npc有效范围内的账户列表
	CVarList CResult;
	pKernel->GetAroundList(self, distance, TYPE_PLAYER, 0, CResult);
	int nCount = static_cast<int>(CResult.GetCount());
	
	//5、遍历设置安全区内账户对应的属性
	LoopBeginCheck(b)
	for( int i = 0; i < nCount; ++i )
	{
		LoopDoCheck(b)
		//5.1、验证对象有效性
		PERSISTID obj = CResult.ObjectVal(i);
		if ( !pKernel->Exists(obj) ) 
		{
			continue;
		}

		//5.2、更新离开安全区属性状态
		m_pSafeAreaNpc->UpdateEntryState(pKernel, obj, nNation, nSafeRule);
	}

	return 0;
}

/*!
* @brief	玩家离开场景
* @param    引擎指针
* @param    玩家对象
* @param    sender对象
* @param    回调携带参数
* @return	bool
*/
int SafeAreaNpcModule::OnLeaveScene(IKernel* pKernel, const PERSISTID& self,
							  const PERSISTID& sender, const IVarList& args)
{
	//1、基本参数验证
	if ( !pKernel->Exists(self) ) 
	{
		return 0;
	}

	//2、类型判断
    if ( TYPE_PLAYER != pKernel->Type(self) ) 
	{
		return 0;
	}

	//3、获取玩家操作对象
	IGameObj * selfObj = pKernel->GetGameObj(self);
	if ( NULL == selfObj ) 
	{
		return 0;
	}

	//3、验证离开场景是否需清除玩家安全区内的临时标识
	if ( selfObj->FindData( "SafeAreaFlag" ) && selfObj->QueryDataInt("SafeAreaFlag") == 1 )
	{
		//3.1、更新离开安全区属性状态
		m_pSafeAreaNpc->UpdateLeaveState(pKernel, self, 0, 0);
	}

	return 0;
}

// 玩家进入场景
int SafeAreaNpcModule::OnPlayerEnterScene(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
// 	if (m_pSafeAreaNpc->NeedCheckRectangularSafeArea(pKernel))
// 	{
// 		ADD_HEART_BEAT(pKernel, self, "SafeAreaNpcModule::HB_CheckRectangularSafeArea", 500);
// 		ADD_DATA_INT(pSelfObj, RECT_AREA_INDEX);
// 	}

	return 0;
}

// 响应分组变化 
int SafeAreaNpcModule::OnCommandChangeGroup(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nSceneId = pKernel->GetSceneId();
	if (nSceneId > OUTDOOR_SCENE_MAX)
	{
		return 0;
	}
	int nChangeGroupType = args.IntVal(1);
	int nRectAreaIndex = pSelfObj->QueryDataInt(RECT_AREA_INDEX);
	// 如果在矩形安全区内,进入分组去掉无敌状态
	if (nRectAreaIndex > NOT_IN_RECT_SAFE_AREA)
	{
		CVarList bufftypelist;
		bufftypelist << CB_CANT_BE_ATTACK;

		const RectangularSafeArea* pData = m_pSafeAreaNpc->QueryRectangularSafeAreaInfo(nRectAreaIndex);
		if (NULL == pData)
		{
			return 0;
		}

		if (GROUP_CHANGE_TYPE_ENTRY == nChangeGroupType)
		{
			FightInterfaceInstance->RemoveControlBuffer(pKernel, self, bufftypelist);
			if (pSelfObj->FindData("SafeAreaFlag"))
			{
				pSelfObj->RemoveData("SafeAreaFlag");
			}
		}
		else
		{
			FightInterfaceInstance->AddControlBuffer(pKernel, self, self, bufftypelist, CONTROL_BUFF_FOREVER);
			if (!pSelfObj->FindData("SafeAreaFlag"))
			{
				pSelfObj->AddDataInt("SafeAreaFlag", 1);
			}
		}
	}	
	return 0;
}

/*!
* @brief	更新进入安全区状态
* @param    引擎指针
* @param    玩家对象
* @param	安全区NPC对象
*/
void SafeAreaNpcModule::UpdateEntryState(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule)
{
	//1、基本参数校验
	if ( !pKernel->Exists(player) ) 
	{
		return;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return;
	}

	//2、验证是否保护
	if (!IsSafe(pKernel, player, nNation, nSafeRule, SAFE_AREA_NPC_ENTRY))
	{
		return;
	}

	//3、设置是否能攻击与被攻击属性(1:否)
	CVarList bufftypelist;
	bufftypelist << CB_CANT_BE_ATTACK;
	FightInterfaceInstance->AddControlBuffer(pKernel, player, player, bufftypelist,  CONTROL_BUFF_FOREVER);

	//4、安全区内统一不占格子，可穿透
	playerObj->SetInt("PathGrid",TYPE_PLAYER_PATH_GRID_OFF);
	playerObj->SetInt("PathThrough",PATH_THROUGH_ON);

	//5、标记玩家进入安全区临时数据(0:安全区内,1:安全区外)
	ADD_DATA_INT(playerObj, "SafeAreaFlag");
	playerObj->SetDataInt("SafeAreaFlag", 1);
	
	return;
}

/*!
* @brief	更新离开安全区状态
* @param    引擎指针
* @param    玩家对象
* @param	安全区NPC对象
*/
void SafeAreaNpcModule::UpdateLeaveState(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule)
{
	//1、基本参数校验
	if ( !pKernel->Exists(player) ) 
	{
		return;
	}
	IGameObj *playerObj = pKernel->GetGameObj(player);
	if (NULL == playerObj)
	{
		return;
	}

	//2、离开必定重置被攻击状态,此处调用只为发消息
	IsSafe(pKernel, player, nNation, nSafeRule, SAFE_AREA_NPC_LEAVE);

	// 上囚车状态下不删除
// 	PERSISTID escort_npc = PERSISTID();
// 	if (playerObj->GetClassType() == TYPE_PLAYER)
// 	{
// 		escort_npc = playerObj->QueryObject(FIELD_PROP_LINK_NPC);
// 	}
// 	
// 	if (escort_npc.IsNull())
// 	{
// 		//3、设置是否能攻击与被攻击属性(0:是)
// 		CVarList bufftypelist;
// 		bufftypelist << CB_CANT_BE_ATTACK;
// 		FightInterfaceInstance->RemoveControlBuffer(pKernel, player, bufftypelist);
// 	}
	
	//4、调用同一接口设置穿透属性
	//ReviveManagerModule::SetObjectPathThroughState(pKernel, player);
	
	//5、清除玩家进入安全区临时数据
	if( playerObj->FindData("SafeAreaFlag"))
	{
		playerObj->RemoveData("SafeAreaFlag");
	}

	// 除去在矩形安全区标识
	if (playerObj->FindData(RECT_AREA_INDEX))
	{
		playerObj->SetDataInt(RECT_AREA_INDEX, NOT_IN_RECT_SAFE_AREA);
	}
    
	return;
}


/*!
* @brief	是否保护(国家验证)
* @param    引擎指针
* @param    玩家对象
* @param	安全区NPC对象
* @return   是否保护
*/
bool SafeAreaNpcModule::IsSafe(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule, const int type)
{
	//1、基本参数验证
	IGameObj * pSceneObj = pKernel->GetGameObj(pKernel->GetScene());
	if( pSceneObj == NULL )
	{
		return false;
	}

	IGameObj * pPlayerObj = pKernel->GetGameObj(player);
	if( pPlayerObj == NULL)
	{
		return false;
	}

	switch (nSafeRule)
	{
	case SAFE_AREA_NPC_OLD:              //老的安全区
		{
			if( type == SAFE_AREA_NPC_ENTRY )
			{
				ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9001);
			}
			else
			{
				ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9002);
			}
			return true;
		}
		break;
	case SAFE_AREA_NPC_ALL_NATION:      //保护所有玩家
		{
			if( type == SAFE_AREA_NPC_ENTRY )
			{
                ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9005);
			}
			else
			{
				ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9008);
			}
			return true;
		}
		break;
	case SAFE_AREA_NPC_OWN_NATION:      //保护本国玩家
		{
			if( pPlayerObj->QueryInt("Nation") == nNation)
			{
				if( type == SAFE_AREA_NPC_ENTRY )
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9003);
				}
				else
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9006);
				}
				return true;
			}
			else
			{
				if( type == SAFE_AREA_NPC_ENTRY )
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9004);
				}
				else
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9007);
				}
			}
		}
		break;
	case SAFE_AREA_NPC_ENEMY_NATION:    //保护敌国玩家
		{
			if( pPlayerObj->QueryInt("Nation") != nNation )
			{
				if( type == SAFE_AREA_NPC_ENTRY )
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9003);
				}
				else
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9006);
				}
				return true;
			}
			else
			{
				if( type == SAFE_AREA_NPC_ENTRY )
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9004);
				}
				else
				{
					ResponseClientMsg(pKernel, player, SYSTEM_INFO_ID_9007);
				}
			}
		}
		break;
	default:
		break;
	}

	return false;
}

//响应客户端文言提示消息
void SafeAreaNpcModule::ResponseClientMsg(IKernel* pKernel, const PERSISTID& player, const int info_id)
{
	CustomSysInfo(pKernel, player, info_id, CVarList());	
}

// 读取矩形安全区配置
bool SafeAreaNpcModule::LoadRectanularSafeAreaConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/rule/safearea/rectangular_safe_area.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	m_vecRectangularSafeArea.clear();
	m_vecRectangularSafeArea.reserve(nSectionCount);
	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		RectangularSafeArea data;
		data.nAreaId = StringUtil::StringAsInt(section);
		data.nSceneId = xmlfile.ReadInteger(section, "SceneID", 0);
		data.nNation = xmlfile.ReadInteger(section, "Nation", 0);
		data.nSafeRule = xmlfile.ReadInteger(section, "SafeRule", 0);

		const char* strPos = xmlfile.ReadString(section, "CenterPos", "");
		FmVec2 pos;
		ParsePos(pos, strPos);

		float orient = xmlfile.ReadFloat(section, "Orient", 0.0f);
		float width = xmlfile.ReadFloat(section, "Width", 0.0f);
		float height = xmlfile.ReadFloat(section, "Height", 0.0f);

		data.kRectangle = Rectangle2D(pos, orient, width, height);
		
		m_vecRectangularSafeArea.push_back(data);
	}
	return true;
}

// 解析坐标
bool SafeAreaNpcModule::ParsePos(FmVec2& outPos, const char* strPos)
{
	if (StringUtil::CharIsNull(strPos))
	{
		return false;
	}

	CVarList pos;
	StringUtil::SplitString(pos, strPos, ",");
	if (pos.GetCount() != 2)
	{
		return false;
	}
	outPos.x = pos.FloatVal(0);
	outPos.y = pos.FloatVal(1);
	return true;
}

// 是否需要检测矩形安全区 
bool SafeAreaNpcModule::NeedCheckRectangularSafeArea(IKernel* pKernel)
{
	bool bIsCheck = false;
	int nSceneId = pKernel->GetSceneId();
	int nSize = (int)m_vecRectangularSafeArea.size();
	LoopBeginCheck(e);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(e);
		const RectangularSafeArea& data = m_vecRectangularSafeArea[i];
		if (data.nSceneId == nSceneId)
		{
			bIsCheck = true;
			break;
		}
	}

	return bIsCheck;
}

// 检测是否进入离开矩形安全区
int SafeAreaNpcModule::HB_CheckRectangularSafeArea(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || !pSelfObj->FindData(RECT_AREA_INDEX))
	{
		return 0;
	}

	int nRectAreaIndex = pSelfObj->QueryDataInt(RECT_AREA_INDEX);
	// 原来在安全区中,验证是否已离开安全区,没在安全区,再验证是否进入安全区
	if (nRectAreaIndex > NOT_IN_RECT_SAFE_AREA)
	{
		const RectangularSafeArea* pData = m_pSafeAreaNpc->QueryRectangularSafeAreaInfo(nRectAreaIndex);
		if (NULL == pData)
		{
			return 0;
		}

		if (!m_pSafeAreaNpc->CheckInRectSafeArea(pKernel, self, pData->kRectangle))
		{
			m_pSafeAreaNpc->UpdateLeaveState(pKernel, self, pData->nNation, pData->nSafeRule);
		}
	}
	else
	{
		m_pSafeAreaNpc->CheckEnterRectangularSafeArea(pKernel, self);
	}
	return 0;
}

// 检测是否在矩形安全区内 
void SafeAreaNpcModule::CheckEnterRectangularSafeArea(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}
	int nSceneId = pKernel->GetSceneId();
	int nSize = (int)m_vecRectangularSafeArea.size();
	LoopBeginCheck(e);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(e);
		const RectangularSafeArea& data = m_vecRectangularSafeArea[i];
		if (data.nSceneId != nSceneId)
		{
			continue;
		}

		if (CheckInRectSafeArea(pKernel, self, data.kRectangle))
		{
			UpdateEntryState(pKernel, self, data.nNation, data.nSafeRule);	
			pSelfObj->SetDataInt(RECT_AREA_INDEX, data.nAreaId);
			break;
		}
	}

}

// 检测是否在指定的安全区内
bool SafeAreaNpcModule::CheckInRectSafeArea(IKernel* pKernel, const PERSISTID& self, const Rectangle2D& rect)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	float fPosX = pSelfObj->GetPosiX();
	float fPosZ = pSelfObj->GetPosiZ();
	float fRadius = pSelfObj->QueryFloat("CollideRadius");

	Cricle2D targetShape(FmVec2(fPosX, fPosZ), fRadius);

	int result = ShapeCollision::Intersects(targetShape, rect);
	return ECOLLIDE_RESULT_INTERSECT == result;
}

// 根据id查询安全区数据
const SafeAreaNpcModule::RectangularSafeArea* SafeAreaNpcModule::QueryRectangularSafeAreaInfo(int nIndex)
{
	RectangularSafeArea* pFindData = NULL;
	int nSize = (int)m_vecRectangularSafeArea.size();
	LoopBeginCheck(e);
	for (int i = 0; i < nSize; ++i)
	{
		LoopDoCheck(e);
		const RectangularSafeArea& data = m_vecRectangularSafeArea[i];
		if (data.nAreaId == nIndex)
		{
			pFindData = &m_vecRectangularSafeArea[i];
			break;
		}
	}

	return pFindData;
}