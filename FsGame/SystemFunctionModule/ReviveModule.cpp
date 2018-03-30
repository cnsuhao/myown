//--------------------------------------------------------------------
// 文件名:		ReviveModule.h
// 内  容:		死亡重生
// 说  明:		
//				
// 创建日期:	2018年03月13日
// 整理日期:	
// 创建人:		liumf   
//--------------------------------------------------------------------
#include "Define/ReviveDefine.h"
#include "ReviveModule.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "Define/ClientCustomDefine.h"
#include "Define/CommandDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "Define/ServerCustomDefine.h"
#include "EnergyModule.h"

#define REVIVE_PANEL_CONFIG_PATH "ini/SystemFunction/Revive/RevivePanel.xml"

ReviveModule* ReviveModule::m_pReviveModule = NULL;

// 初始化
bool ReviveModule::Init(IKernel* pKernel)
{
	m_pReviveModule = this;

	Assert(m_pReviveModule != NULL);


	pKernel->AddEventCallback("player", "OnReady", ReviveModule::OnPlayerReady);          //玩家上线就近复活死亡
	pKernel->AddEventCallback("player", "OnRecover", ReviveModule::OnPlayerRecover);      //玩家登录数据恢复
	pKernel->AddEventCallback("player", "OnContinue", ReviveModule::OnPlayerReady);    

	pKernel->AddIntCommandHook("player", COMMAND_BEKILL, ReviveModule::OnCommandBeKill);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_REVIVE, ReviveModule::OnCustomRevive);

	if (!LoadConfig(pKernel))
	{
		return false;
	}

	RELOAD_CONFIG_REG("ReviveManagerConfig", ReviveModule::ReLoadConfig);
	return true;
}

/// \brief 上线
int ReviveModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	return 0;
}

int ReviveModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 玩家进入场景时,仍为死亡状态,就近复活
	int nDead = pSelfObj->QueryInt(FIELD_PROP_DEAD);
	if (nDead > 0)
	{
		m_pReviveModule->NearbyRevive(pKernel, self);
	}
	return 0;
}

//请求复活消息处理 
int ReviveModule::OnCustomRevive(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& msg)
{
	if (!pKernel->Exists(self) || msg.GetCount() < 2 || msg.GetType(0) != VTYPE_INT || msg.GetType(1) != VTYPE_INT)
	{
		return 0;
	}

	int nSubMsg = msg.IntVal(1);
	switch (nSubMsg)
	{
	case CS_REQUEST_REVIVE:
		m_pReviveModule->OnCustomRequestRevive(pKernel, self, msg);
		break;
	}
	return 0;
}

// 相应请求复活
void ReviveModule::OnCustomRequestRevive(IKernel* pKernel, const PERSISTID& self, const IVarList& msg)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || msg.GetCount() < 3 || msg.GetType(2) != VTYPE_INT)
	{
		return;
	}

	int nReviveType = msg.IntVal(2);

	// 判断人物是否已复活状态
	if (pSelfObj->QueryInt("Dead") <= 0)
	{
		return;
	}

	// 校验复活类型
	if (!CheckReviveType(pKernel, nReviveType))
	{
		return;
	}

	switch (nReviveType)
	{
	case NEARBY_REVIVE:
		NearbyRevive(pKernel, self);
		break;
	case CITY_REVIVE:
		CityRevive(pKernel, self);
		break;
	case LOCAL_REVIVE:
		LocalRevive(pKernel, self);
		break;
	}
}

// 玩家死亡
int ReviveModule::OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	RevivePanelConfig_s* pReviveData = m_pReviveModule->GetRevivePanelConfig(pKernel);
	if (NULL == pReviveData)
	{
		return 0;
	}

	CVarList msg;
	msg << SERVER_CUSTOMMSG_REVIVE << SC_DEAD_PANEL << pReviveData->m_revivePanel.c_str();
	pKernel->Custom(self, msg);
	return 0;
}

// 加载配置
bool ReviveModule::LoadConfig(IKernel* pKernel)
{
	if (!LoadRevivePanelConfig(pKernel))
	{
		return false;
	}
	return true;
}

// 重新加载
void ReviveModule::ReLoadConfig(IKernel* pKernel)
{
	m_pReviveModule->LoadConfig(pKernel);
}

// 加载复活面板配置
bool ReviveModule::LoadRevivePanelConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += REVIVE_PANEL_CONFIG_PATH;

	CXmlFile xml(pathName.c_str());

	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_ERROR, "[ReviveModule::LoadRevivePanelConfig %s] failed", pathName.c_str());
		return false;
	}

	m_mapRevivePanelConfig.clear();
	// xml读取
	LoopBeginCheck(b);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(b);
		const char *section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		RevivePanelConfig_s data;
		data.m_sceneID = StringUtil::StringAsInt(section);
		data.m_revivePanel = xml.ReadString(section, "RevivePanel", "");

		RevivePanelConfigMap::iterator iter = m_mapRevivePanelConfig.find(data.m_sceneID);
		if (iter == m_mapRevivePanelConfig.end())
		{
			m_mapRevivePanelConfig.insert(std::make_pair(data.m_sceneID, data));
		}
	}
	return true;
}

//应场景复活配置
ReviveModule::RevivePanelConfig_s* ReviveModule::GetRevivePanelConfig(IKernel* pKernel)
{
	int nSceneId = pKernel->GetSceneId();
// 	if (pKernel->GetSceneClass() == 2)	 副本场景
// 	{
// 		nSceneId = pKernel->GetPrototypeSceneId(nSceneId);
// 	}
	
	RevivePanelConfigMap::iterator  it = m_mapRevivePanelConfig.find(nSceneId);
	if (it == m_mapRevivePanelConfig.end())
	{
		return NULL;
	}

	return &it->second;
}

// 原地复活
void ReviveModule::LocalRevive(IKernel* pKernel, const PERSISTID& self)
{
	 // 判断元宝是否足够

	PlayerRevive(pKernel, self);
}

// 就近复活
void ReviveModule::NearbyRevive(IKernel* pKernel, const PERSISTID& self)
{
	PlayerRevive(pKernel, self);

	//1、初始化坐标
	len_t dx = 0.0f;
	len_t dy = 0.0f;
	len_t dz = 0.0f;
	len_t oo = 0.0f;

	pKernel->GetSceneBorn(dx, dy, dz, oo);

	pKernel->MoveTo(self, dx, dy, dz, oo);
}

// 城中复活
void ReviveModule::CityRevive(IKernel* pKernel, const PERSISTID& self)
{
	NearbyRevive(pKernel, self);
}

// 复活玩家
void ReviveModule::PlayerRevive(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 玩家为死亡状态
	if (pSelfObj->QueryInt("Dead") > 0)
	{
		// 设置玩家HP
		int64_t nMaxHP = pSelfObj->QueryInt64(FIELD_PROP_MAX_HP);
		pSelfObj->SetInt("Dead", 0);

		EnergyModule::UpdateEnergy(pKernel, self, ENERGY_HP, nMaxHP);
	}
}

// 校验复活类型
bool ReviveModule::CheckReviveType(IKernel* pKernel, const int reviveType)
{
	// 参数保护
	if (reviveType < LOCAL_REVIVE || reviveType >= MAX_REVIVE_NUM)
	{
		return false;
	}

	// 配置复活场景验证(无配置限制直接返回)
	RevivePanelConfig_s* config = GetRevivePanelConfig(pKernel);
	if (config == NULL)
	{
		return false;
	}

	//3、循环匹配复活类型
	CVarList vList;
	StringUtil::SplitString(vList, config->m_revivePanel, ",");
	int len = static_cast<int>(vList.GetCount());
	bool bCheck = false;
	LoopBeginCheck(b);
	for (int i = 0; i < len; ++i)
	{
		LoopDoCheck(b);
		if (reviveType == vList.IntVal(i))
		{
			bCheck = true;
			break;
		}
	}

	return bCheck;
}