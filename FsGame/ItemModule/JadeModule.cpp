//--------------------------------------------------------------------
// 文件名:      JadeModule.h
// 内  容:      玉珏模块
// 说  明:
// 创建日期:    2018年3月23日
// 创建人:      刘明飞 
//    :       
//--------------------------------------------------------------------

#include "JadeModule.h"
#include "Define/JadeDefine.h"
#include "Define/ClientCustomDefine.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "CommonModule/SwitchManagerModule.h"
#include "utils/extend_func.h"
#include "Define/ViewDefine.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"
#include "utils/custom_func.h"
#include "CommonModule/ContainerModule.h"
#include "ItemBaseModule.h"
#include "Define/Fields.h"
#include "utils/string_util.h"
#include "CommonModule/PropRefreshModule.h"
#include "Define/StaticDataDefine.h"
#include "Interface/FightInterface.h"
#include "utils/XmlFile.h"
#include "Define/ServerCustomDefine.h"
#include "Define/CommandDefine.h"
#include "Define/BattleAbilityDefine.h"
#include "Interface/ILuaExtModule.h"
#include "CommonModule/CommRuleModule.h"

JadeModule* JadeModule::m_pJadeModule = NULL;
ContainerModule* JadeModule::m_pContainerModule = NULL;

// 测试玉珏消息
int nx_jade_client_msg(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_jade_client_msg, 3);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_jade_client_msg, 1);
	CHECK_ARG_INT(state, nx_jade_client_msg, 2);
	CHECK_ARG_STRING(state, nx_jade_client_msg, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	int nSubMsg = pKernel->LuaToInt(state, 2);
	const char* strJade = pKernel->LuaToString(state, 3);


	JadeModule::OnCustomJadeMsg(pKernel, self, PERSISTID(), CVarList() << CLIENT_CUSTOMMSG_JADE << nSubMsg << strJade);
	return 1;
}

//初始化
bool JadeModule::Init(IKernel* pKernel)
{

	m_pJadeModule = this;

	m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	Assert(NULL != m_pJadeModule && NULL != m_pContainerModule);

	pKernel->AddEventCallback("player", "OnRecover", JadeModule::OnPlayerRecover);
	pKernel->AddEventCallback("player", "OnReady", JadeModule::OnPlayerReady);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_JADE, JadeModule::OnCustomJadeMsg);

	if (!LoadResource(pKernel))
	{
		return false;
	}


	RELOAD_CONFIG_REG("JadeConfig", JadeModule::ReloadConfig);

	DECL_LUA_EXT(nx_jade_client_msg);
	return true;
}

// 客户端相关消息
int  JadeModule::OnCustomJadeMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self) || args.GetCount() < 2 || args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_INT)
	{
		return 0;
	}

	int nSubMsg = args.IntVal(1);
	switch (nSubMsg)
	{
	case JADE_C2S_LEVEL_UP:
		m_pJadeModule->OnCustomLevelUp(pKernel, self, args);
		break;
	case JADE_C2S_STEP_UP:
		m_pJadeModule->OnCustomStepUp(pKernel, self, args);
		break;
	case JADE_C2S_COMPOSE:
		m_pJadeModule->OnCustomCompose(pKernel, self, args);
		break;
	case JADE_C2S_WEAR:
		m_pJadeModule->OnCustomWear(pKernel, self, args);
		break;
	}
	return 0;
}

//回调函数 玩家加载数据完成
int JadeModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	PERSISTID box = pKernel->GetChild(self, JADE_BOX_WSTR);
	if (!pKernel->Exists(box))
	{
		box = pKernel->CreateContainer(self, JADE_BOX_STR, MAX_JADE_BOX_CAPACITY);
		if (!pKernel->Exists(box))
		{
			extend_warning(LOG_ERROR, "[Error]create JadeBox error!");
			return 0;
		}

		pKernel->SetWideStr(box, "Name", JADE_BOX_WSTR);
		pKernel->SetUnsave(box, false);
	}

	// 设置当前佩戴的玉珏
	const char* strJadeId = pSelfObj->QueryString(FIELD_PROP_ACTIVE_JADE_ID);
	PERSISTID wear_jade = m_pContainerModule->FindItem(pKernel, box, strJadeId);
	if (pKernel->Exists(wear_jade))
	{
		pSelfObj->SetObject(FIELD_PROP_ACTIVE_JADE, wear_jade);
		m_pJadeModule->UpdateJadeModifyProp(pKernel, self);
	}

	return 0;
}

int JadeModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	// 判断
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	if (!pKernel->FindViewport(self, VIEWPORT_JADE_BOX))
	{
		PERSISTID box = pKernel->GetChild(self, JADE_BOX_WSTR);
		if (pKernel->Exists(box))
		{
			pKernel->AddViewport(self, VIEWPORT_JADE_BOX, box);
		}
		else
		{
			::extend_warning(pKernel, "[JadeModule::OnPlayerReady]: JADE_BOX not Exists");
		}
	}

	return 0;
}

// 响应玉珏升级
void JadeModule::OnCustomLevelUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//功能是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_JADE, self))
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	const char* strJadeId = args.StringVal(2);
	PERSISTID jade_box = pKernel->GetChild(self, JADE_BOX_WSTR);
	PERSISTID itembox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(jade_box) || !pKernel->Exists(itembox))
	{
		return;
	}

	PERSISTID jade = m_pContainerModule->FindItem(pKernel, jade_box, strJadeId);
	IGameObj* pJadeObj = pKernel->GetGameObj(jade);
	if (NULL == pJadeObj)
	{
		return;
	}

	int nRequestLv = pJadeObj->QueryInt(FIELD_PROP_LEVEL) + 1;
	const LevelData* pData = QueryLevelData(strJadeId, nRequestLv);
	if (NULL == pData)
	{
		return;
	}

	// 玩家等级是否满足
	int nSelfLv = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	if (nSelfLv < pData->nPlayerLvLimit)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7020, CVarList());
		return;
	}

	// 消耗物品是否足够
	if (!CommRuleModule::CanDecItems(pKernel, self, pData->vecConsumeItem))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7021, CVarList());
		return;
	}

	// 扣道具
	CommRuleModule::ConsumeItems(pKernel, self, pData->vecConsumeItem, FUNCTION_EVENT_ID_JADE);

	// 玉珏升级,更新玩家属性
	pJadeObj->SetInt(FIELD_PROP_LEVEL, nRequestLv);
	UpdateJadeModifyProp(pKernel, self);

	// 通知客户端升级成功
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_JADE << JADE_S2C_LEVEL_UP_SUC);

	// 记录升级日志
// 	RoleUpgradeLog log;
// 
// 	log.type = LOG_ROLE_UPGRADE_WING_LEVEL;
// 	log.levelBefore = nRequestLevel - 1;
// 	log.levelAfter = nRequestLevel;
// 	LogModule::m_pLogModule->SaveRoleUpgradeLog(pKernel, self, log);
// 
// 	pKernel->Command(self, self, CVarList() << COMMAND_WING_LEVEL_UP << nRequestLevel);
// 	CVarList msg;
// 	msg << SERVER_CUSTOMMSG_WING << WING_S2C_LEVEL_UP_SUC;
// 	pKernel->Custom(self, msg);
}

// 响应玉珏升阶
void JadeModule::OnCustomStepUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	//功能是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_JADE, self))
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	const char* strJadeId = args.StringVal(2);
	PERSISTID jade_box = pKernel->GetChild(self, JADE_BOX_WSTR);
	PERSISTID itembox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(jade_box) || !pKernel->Exists(itembox))
	{
		return;
	}

	PERSISTID jade = m_pContainerModule->FindItem(pKernel, jade_box, strJadeId);
	IGameObj* pJadeObj = pKernel->GetGameObj(jade);
	if (NULL == pJadeObj)
	{
		return;
	}

	int nRequestStep = pJadeObj->QueryInt(FIELD_PROP_STEP) + 1;
	int nJadeLv = pJadeObj->QueryInt(FIELD_PROP_LEVEL);
	const StepData* pData = QueryStepData(strJadeId, nRequestStep);
	const JadeMainData* pMainData = QueryJadeMainData(strJadeId);
	if (NULL == pData || NULL == pMainData)
	{
		return;
	}

	// 玉珏等级是否满足
	if (nJadeLv < pData->nJadeLvLimit)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7020, CVarList());
		return;
	}

	// 消耗物品是否足够
	if (!CommRuleModule::CanDecItems(pKernel, self, pData->vecConsumeItem) || !m_pContainerModule->TryRemoveItems(pKernel, itembox, pMainData->strPiece.c_str(), pData->nPieceNum))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7022, CVarList());
		return;
	}

	// 扣道具
	CommRuleModule::ConsumeItems(pKernel, self, pData->vecConsumeItem, FUNCTION_EVENT_ID_JADE);
	m_pContainerModule->RemoveItems(pKernel, itembox, pMainData->strPiece.c_str(), pData->nPieceNum, FUNCTION_EVENT_ID_JADE, true);

	// 玉珏升阶,附带技能升级
	pJadeObj->SetInt(FIELD_PROP_STEP, nRequestStep);

	// 正在装备的玉珏,更新下对应技能等级
	PERSISTID wearjade = pSelfObj->QueryObject(FIELD_PROP_ACTIVE_JADE);
	if (wearjade == jade)
	{
		FightInterfaceInstance->UpdateSkillData(pKernel, self, pMainData->strSkillid.c_str(), nRequestStep);
	}

	// 通知客户端升阶成功
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_JADE << JADE_S2C_STEP_UP_SUC);

	// 记录升阶日志
}

// 响应碎片合成玉珏
void JadeModule::OnCustomCompose(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || args.GetCount() < 3 || args.GetType(2) != VTYPE_STRING)
	{
		return;
	}

	const char* strJadeId = args.StringVal(2);
	const JadeMainData* pJadeData = QueryJadeMainData(strJadeId);
	if (NULL == pJadeData)
	{
		return;
	}

	// 检查玩家是否开启玉珏功能
	if (!ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_JADE_FUNCTION))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7017, CVarList());
		return;
	}	 
	PERSISTID jadebox = pKernel->GetChild(self, JADE_BOX_WSTR);
	PERSISTID itembox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(jadebox) || !pKernel->Exists(itembox))
	{
		return;
	}
	// 检查是否有同类玉珏
	PERSISTID jade = m_pContainerModule->FindItem(pKernel, jadebox, strJadeId);
	if (!jade.IsNull())
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7018, CVarList());
		return;
	}
	// 检查碎片是否足够
	if (!m_pContainerModule->TryRemoveItems(pKernel, itembox, pJadeData->strPiece.c_str(), pJadeData->nPieceNum))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7019, CVarList());
		return;
	}

	// 扣道具
	m_pContainerModule->RemoveItems(pKernel, itembox, pJadeData->strPiece.c_str(), pJadeData->nPieceNum, FUNCTION_EVENT_ID_JADE, true);

	// 增加新合成的玉珏
	jade = ItemBaseModule::m_pItemBaseModule->CreateItem(pKernel, jadebox, strJadeId, 1);

	IGameObj* pJadeObj = pKernel->GetGameObj(jade);
	if (NULL != pJadeObj)
	{
		pJadeObj->SetInt(FIELD_PROP_LEVEL, 1);
		pJadeObj->SetInt(FIELD_PROP_STEP, 1);
		const char* strActiveJade = pSelfObj->QueryString(FIELD_PROP_ACTIVE_JADE_ID);
		if (StringUtil::CharIsNull(strActiveJade))
		{
			// 穿戴玉珏
			OnCustomWear(pKernel, self, args);
		}
		
		// 记日志

	}
}

// 响应穿戴玉珏
void JadeModule::OnCustomWear(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || args.GetCount() < 3 || args.GetType(2) != VTYPE_STRING)
	{
		return;
	}

	PERSISTID jade_box = pKernel->GetChild(self, JADE_BOX_WSTR);
	const char* strJadeId = args.StringVal(2);
	if (!pKernel->Exists(jade_box))
	{
		return;
	}

	PERSISTID jade = m_pContainerModule->FindItem(pKernel, jade_box, strJadeId);
	if (!pKernel->Exists(jade))
	{
		return;
	}

	PERSISTID wear_jade = pSelfObj->QueryObject(FIELD_PROP_ACTIVE_JADE);
	// 已经穿戴了
	if (jade == wear_jade)
	{
		return;
	}
	
	// 删除老玉珏的技能
	if (pKernel->Exists(wear_jade))
	{
		const char* strWearJadeId = pKernel->GetConfig(wear_jade);
		const JadeMainData* pJadeData = QueryJadeMainData(strWearJadeId);
		if (NULL == pJadeData)
		{
			return;
		}

		FightInterfaceInstance->RemoveSkillData(pKernel, self, pJadeData->strSkillid.c_str());
	}
	
	// 穿戴玉珏
	pSelfObj->SetObject(FIELD_PROP_ACTIVE_JADE, jade);
	pSelfObj->SetString(FIELD_PROP_ACTIVE_JADE_ID, strJadeId);

	// 增加新玉珏的技能
	const JadeMainData* pJadeData = QueryJadeMainData(strJadeId);
	if (NULL == pJadeData)
	{
		return;
	}

	int nJadeStep = pKernel->QueryInt(jade, FIELD_PROP_STEP);
	FightInterfaceInstance->UpdateSkillData(pKernel, self, pJadeData->strSkillid.c_str(), nJadeStep);

	// 更新属性
	UpdateJadeModifyProp(pKernel, self);
}

// 读取配置
bool JadeModule::LoadResource(IKernel* pKernel)
{
	ClearMainData();

	if (!LoadMainResource(pKernel))
	{
		return false;
	}

	if (!LoadLevelResource(pKernel))
	{
		return false;
	}

	if (!LoadStepResource(pKernel))
	{
		return false;
	}
	return true;
}

// 读取主配置
bool JadeModule::LoadMainResource(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Item/Jade/JadeMain.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);

		JadeMainData* pData = NEW JadeMainData;
		if (NULL == pData)
		{
			return false;
		}

		pData->strJadeId = section;
		pData->strPiece = xmlfile.ReadString(section, "PieceId", "");
		pData->nPieceNum = xmlfile.ReadInteger(section, "PieceNum", 0);
		pData->strSkillid = xmlfile.ReadString(section, "SkillId", "");

		JadeMainDataMapIter iter = m_mapJadeMainData.find(pData->strJadeId);
		if (iter == m_mapJadeMainData.end())
		{
			m_mapJadeMainData.insert(make_pair(pData->strJadeId, pData));
		}
	}

	return true;
}

// 读取升阶配置
bool JadeModule::LoadStepResource(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Item/Jade/JadeStep.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);
		const char* strJadeId = xmlfile.ReadString(section, "JadeId", "");
		JadeMainDataMapIter iter = m_mapJadeMainData.find(strJadeId);
		if (iter == m_mapJadeMainData.end())
		{
			std::string info = "JadeId Not Find " + pathName + std::string(strJadeId);
			::extend_warning(LOG_ERROR, info.c_str());
			continue;
		}

		StepData data;
		data.nStepLv = xmlfile.ReadInteger(section, "Step", 0);
		const char* strConsumeData = xmlfile.ReadString(section, "ConsumeItem", "");
		CommRuleModule::ParseConsumeVec(strConsumeData, data.vecConsumeItem);
		data.nPieceNum = xmlfile.ReadInteger(section, "PieceNum", 0);
		data.nJadeLvLimit = xmlfile.ReadInteger(section, "JadeLvLimit", 0);
		data.nSkillLv = xmlfile.ReadInteger(section, "SkillLv", 0);

		iter->second->vecStepData.push_back(data);
	}

	return true;
}

// 读取升级配置
bool JadeModule::LoadLevelResource(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Item/Jade/JadeLevel.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	int nSectionCount = xmlfile.GetSectionCount();

	LoopBeginCheck(e);
	for (int i = 0; i < nSectionCount; ++i)
	{
		LoopDoCheck(e);
		const char* section = xmlfile.GetSectionByIndex(i);
		const char* strJadeId = xmlfile.ReadString(section, "JadeId", "");
		JadeMainDataMapIter iter = m_mapJadeMainData.find(strJadeId);
		if (iter == m_mapJadeMainData.end())
		{
			std::string info = "JadeId Not Find " + pathName + std::string(strJadeId);
			::extend_warning(LOG_ERROR, info.c_str());
			continue;
		}

		LevelData data;
		data.nLevel = xmlfile.ReadInteger(section, "Level", 0);
		const char* strConsumeData = xmlfile.ReadString(section, "ConsumeItem", "");
		CommRuleModule::ParseConsumeVec(strConsumeData, data.vecConsumeItem);
		data.nPackageId = xmlfile.ReadInteger(section, "PackageId", 0);
		data.nPlayerLvLimit = xmlfile.ReadInteger(section, "PlayerLvLimit", 0);

		iter->second->vecLevelData.push_back(data);
	}

	return true;
}

// 更新玩家玉珏属性
void JadeModule::UpdateJadeModifyProp(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	PERSISTID ActiveJade = pSelfObj->QueryObject(FIELD_PROP_ACTIVE_JADE);
	IGameObj* pJadeObj = pKernel->GetGameObj(ActiveJade);
	if (NULL == pJadeObj)
	{
		return;
	}
	const char* strJadeId = pJadeObj->GetConfig();
	int nJadeLevel = pJadeObj->QueryInt(FIELD_PROP_LEVEL);

	const LevelData* pLevelData = JadeModule::m_pJadeModule->QueryLevelData(strJadeId, nJadeLevel);
	if (NULL == pLevelData)
	{
		return;
	}
	PropRefreshModule::UpdateModifyProp(pKernel, self, STATIC_DATA_JADE_ATTRIBUTE_PACK, JADE_PROPPAK_ID, pLevelData->nPackageId);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
	return;
}

// 查询升阶数据
const JadeModule::StepData* JadeModule::QueryStepData(std::string strJadeId, int nStep)
{
	const StepData* pData = NULL;
	const JadeMainData* pMainData = QueryJadeMainData(strJadeId);
	if (NULL != pMainData)
	{
		int nSize = pMainData->vecStepData.size();
		LoopBeginCheck(w);
		for (int i = 0; i < nSize;++i)
		{
			LoopDoCheck(w);
			const StepData* pTmpData = &pMainData->vecStepData[i];
			if (pTmpData->nStepLv == nStep)
			{
				pData = pTmpData;
				break;
			}
		}
	}

	return pData;
}

// 查询升级数据
const JadeModule::LevelData* JadeModule::QueryLevelData(std::string strJadeId, int nLevel)
{
	const LevelData* pData = NULL;
	const JadeMainData* pMainData = QueryJadeMainData(strJadeId);
	if (NULL != pMainData)
	{
		int nSize = pMainData->vecLevelData.size();
		LoopBeginCheck(w);
		for (int i = 0; i < nSize; ++i)
		{
			LoopDoCheck(w);
			const LevelData* pTmpData = &pMainData->vecLevelData[i];
			if (pTmpData->nLevel == nLevel)
			{
				pData = pTmpData;
				break;
			}
		}
	}

	return pData;
}

// 查询玉珏数据
const JadeModule::JadeMainData*	JadeModule::QueryJadeMainData(std::string strJadeId)
{
	const JadeMainData* pFindData = NULL;
	JadeMainDataMapIter iter = m_mapJadeMainData.find(strJadeId);
	if (iter != m_mapJadeMainData.end())
	{
		pFindData = iter->second;
	}

	return pFindData;
}

// 清除配置数据
void JadeModule::ClearMainData()
{
	for (JadeMainDataMapIter iter = m_mapJadeMainData.begin();iter != m_mapJadeMainData.end();++iter)
	{
		JadeMainData* pData = iter->second;
		if (NULL == pData)
		{
			continue;
		}

		pData->vecLevelData.clear();
		pData->vecStepData.clear();

		SAFE_DELETE(pData);
		iter->second = NULL;
	}

	m_mapJadeMainData.clear();
}

void JadeModule::ReloadConfig(IKernel* pKernel)
{
	m_pJadeModule->LoadResource(pKernel);
}