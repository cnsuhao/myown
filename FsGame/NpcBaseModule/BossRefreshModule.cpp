//--------------------------------------------------------------------
// 文件名:      BossRefreshModule.h
// 内  容:      Boss刷新模块
// 说  明:		
// 创建日期:    2016年6月28日
// 创建人:      刘明飞
// 修改人:        
//    :       
//--------------------------------------------------------------------

#include "BossRefreshModule.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "time.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
//#include "FsGame/Define/MapDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "utils/string_util.h"
#include "utils/util.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/NpcBaseModule/NpcCreatorModule.h"
//#include "FsGame/SceneBaseModule/SceneInfoModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "CommonModule/EnvirValueModule.h"
#include "Define/ModifyPackDefine.h"
#include "CommonModule/PropRefreshModule.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "Define/FightNpcDefine.h"
#include "CommonModule/CommRuleModule.h"

std::wstring	BossRefreshModule::m_domainName = L"";
BossRefreshModule * BossRefreshModule::m_pBossRefreshModule = NULL;
NpcCreatorModule * BossRefreshModule::m_pNpcCreatorModule = NULL;
AsynCtrlModule * BossRefreshModule::m_pAsynCtrlModule = NULL;
const char* const BOSS_REFRESH_INFO = "ini/npc/BossRefresh/SceneBoss.xml";

// 初始化
bool BossRefreshModule::Init(IKernel* pKernel)
{
    m_pBossRefreshModule = this;
    m_pNpcCreatorModule = (NpcCreatorModule*)pKernel->GetLogicModule("NpcCreatorModule");
    m_pAsynCtrlModule = (AsynCtrlModule*)pKernel->GetLogicModule("AsynCtrlModule");

    Assert(m_pBossRefreshModule != NULL && m_pNpcCreatorModule != NULL && m_pAsynCtrlModule != NULL);

    pKernel->AddEventCallback("scene", "OnCreate", BossRefreshModule::OnCreate);
    pKernel->AddEventCallback("NormalNpc", "OnEntry", OnBossEntry);

    pKernel->AddIntCommandHook("BossNpc", COMMAND_BEKILL, OnCommandBossBeKilled);
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_QUERY_BOSS_INFO, OnCustomQuerySceneBoss);

    LoadResource(pKernel);
    DECL_HEARTBEAT(BossRefreshModule::HB_BossCreate);

	RELOAD_CONFIG_REG("BossRefreshConfig", BossRefreshModule::ReloadConfig);
    return true;
}

// 释放
bool BossRefreshModule::Shut(IKernel* pKernel)
{
    return true;
}

// 是否为刷boss场景
bool BossRefreshModule::IsRefreshBossScene(IKernel* pKernel)
{
	int nSceneId = pKernel->GetSceneId();
	map<int, SceneRefreshInfo>::iterator it = m_pBossRefreshModule->m_MapSceneBossInfo.find(nSceneId);

	return it != m_pBossRefreshModule->m_MapSceneBossInfo.end();
}

//场景创建
int BossRefreshModule::OnCreate(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    int iSceneID = pKernel->GetSceneId();
    map<int, SceneRefreshInfo>::iterator it = m_pBossRefreshModule->m_MapSceneBossInfo.find(iSceneID);
    if (it == m_pBossRefreshModule->m_MapSceneBossInfo.end())
    {
        return 0;
    }

	IGameObj* pSceneObj = pKernel->GetGameObj(self);
	if (NULL == pSceneObj)
	{
		return 0;
	}

	IRecord* pBossRecord = pSceneObj->GetRecord(SCENE_BOSS_REC);
	if (pBossRecord == NULL)
	{
		return 0;
	}

	if (pBossRecord->GetRows() > 0)
	{
		return 0;
	}

	vector<BossInfo>& vecBoss = it->second.VecSceneBossInfo;
	int nSize = (int)vecBoss.size();
	LoopBeginCheck(e);
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(e);
		pBossRecord->AddRowValue(-1, CVarList() << vecBoss[i].sBossConfig.c_str() << PERSISTID() << 0.0f << 0.0f << 0.0f);
	}

    //没有心跳则增加检测心跳
    ADD_HEART_BEAT(pKernel, pKernel->GetScene(), "BossRefreshModule::HB_BossCreate", 5*1000);
    return 0;
}

//创建BOSS心跳
int BossRefreshModule::HB_BossCreate(IKernel* pKernel, const PERSISTID& self, int silce)
{
    IGameObj* pSceneObj = pKernel->GetGameObj(self);
    if (NULL == pSceneObj)
    {
        return 0;
    }
    //不存在则不处理
    int iSceneID = pKernel->GetSceneId();
    std::map<int, SceneRefreshInfo>::iterator iter = m_pBossRefreshModule->m_MapSceneBossInfo.find(iSceneID);
    if (iter == m_pBossRefreshModule->m_MapSceneBossInfo.end())
    {
        return 0;
    }
    SceneRefreshInfo& sRefreshInfo = iter->second;
	// 是否需要刷boss
	if (sRefreshInfo.nOpen == 1 && m_pBossRefreshModule->NotRefresh(sRefreshInfo.beginTime, sRefreshInfo.endTime))
	{
		return 0;
	}
	//是否存在BOSS表
    IRecord* pSceneBossRec = pSceneObj->GetRecord(SCENE_BOSS_REC);
    if (pSceneBossRec == NULL)
    {
        return 0;
    } 
	// 获取公共区boss记录表
	IRecord* pBossPubRec = m_pBossRefreshModule->GetPubBossInSceneRec(pKernel, SCENE_BOSS_TOTAL_REC);
	if (NULL == pBossPubRec)
	{
		return 0;
	}
	int nRecRows = pSceneBossRec->GetRows();
	LoopBeginCheck(q);
	for (int i = 0;i < nRecRows;++i)
	{
		LoopDoCheck(q);
		const char* strBossConfig = pSceneBossRec->QueryString(i, SCENE_BOSS_CONFIG);
		int nPubRowIndex = pBossPubRec->FindString(SCENE_BOSS_INFO_CONFIG, strBossConfig);
		// 没找到说明刚开服,立刻刷新boss
		if (-1 == nPubRowIndex)
		{
			m_pBossRefreshModule->CreateBoss(pKernel, strBossConfig, pSceneBossRec, i);
			continue;
		}
		int64_t nDeadTime = pBossPubRec->QueryInt64(nPubRowIndex, SCENE_BOSS_DEAD_TIME);
		if (nDeadTime == BS_BOSS_BORN)
		{
			continue;
		}
		int64_t nCurTime = ::time(NULL);
		// 到时间刷boss
		int nInterval = m_pBossRefreshModule->QueryBossInterval(sRefreshInfo, strBossConfig);
		if (0 == nInterval)
		{
			continue;
		}
		if (nCurTime - nDeadTime > nInterval)
		{
			m_pBossRefreshModule->CreateBoss(pKernel, strBossConfig, pSceneBossRec, i);
		}
	}
    return 0;
}

//资源加载
bool BossRefreshModule::LoadResource(IKernel* pKernel)
{
	if (!LoadSceneBoss(pKernel))
	{
		return false;
	}

	return true;
}

//是否统计此场景内BOSS数量
bool BossRefreshModule::ContainSceneID(int sceneID)
{
    map<int, SceneRefreshInfo>::iterator it = m_MapSceneBossInfo.find(sceneID);
    if (it == m_MapSceneBossInfo.end())
    {
        return false;
    }
    return true;
}

//NPC进入场景
int BossRefreshModule::OnBossEntry(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    if (!pKernel->Exists(self))
    {
        return 0;
    }
    IGameObj* pNpcObj = pKernel->GetGameObj(self);
    if (NULL == pNpcObj)
    {
        return 0;
    }
    if (pNpcObj->QueryInt("NpcType") != NPC_TYPE_BOSS)
    {
        //不是BOSS
        return 0;
    }
    int sceneID = pKernel->GetSceneId();
	const BossInfo* pBossInfo = m_pBossRefreshModule->FindBossInfo(pKernel, pNpcObj->GetConfig());
	if (!m_pBossRefreshModule->ContainSceneID(sceneID) || NULL == pBossInfo)
    {
        //此场景不统计或配置中没有的boss不记录
        return 0;
    }

    //通知小红点刷新
    //pKernel->CommandByWorld(CVarList() << COMMAND_ZONES_ENTRY);

    CVarList msg;
	msg << PUBSPACE_DOMAIN
		<< GetDomainName(pKernel)
		<< SP_DOMAIN_MSG_SCENE_BOSS_STATE
		<< pNpcObj->GetConfig()
		<< BS_BOSS_BORN
		<< sceneID;

    pKernel->SendPublicMessage(msg);

	m_pBossRefreshModule->SendBossStateChangeMsg(pKernel, pNpcObj->GetConfig(), BS_BOSS_BORN);
    return 0;
}

//NPC销毁
int BossRefreshModule::OnCommandBossBeKilled(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    IGameObj* pNpcObj = pKernel->GetGameObj(self);
    if (NULL == pNpcObj)
    {
        return 0;
    }
    if (pNpcObj->QueryInt("NpcType") != NPC_TYPE_BOSS)
    {
        //不是BOSS
        return 0;
    }
    int sceneID = pKernel->GetSceneId();
    if (!m_pBossRefreshModule->ContainSceneID(sceneID))
    {
        //此场景不统计
        return 0;
    }

	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL == pSceneObj)
	{
		return 0;
	}
	//是否存在BOSS表
	IRecord* pSceneBossRec = pSceneObj->GetRecord(SCENE_BOSS_REC);
	if (pSceneBossRec == NULL)
	{
		return 0;
	}
	const char* strBossConfig = pNpcObj->GetConfig();
	int nRowsIndex = pSceneBossRec->FindString(SCENE_BOSS_CONFIG, strBossConfig);
	if (-1 == nRowsIndex)
	{
		return 0;
	}

	pSceneBossRec->SetObject(nRowsIndex, SCENE_BOSS_OBJ, PERSISTID());
	IGameObj* pKillerObj = pKernel->GetGameObj(sender);
	if (NULL == pKillerObj)
	{
		return 0;
	}
	const wchar_t* wsKiller = pKillerObj->QueryWideStr(FIELD_PROP_NAME);

    CVarList msg;
	msg << PUBSPACE_DOMAIN
		<< GetDomainName(pKernel)
		<< SP_DOMAIN_MSG_SCENE_BOSS_STATE
		<< strBossConfig
		<< BS_BOSS_DEAD
		<< sceneID
		<< wsKiller;

	pKernel->SendPublicMessage(msg);

	m_pBossRefreshModule->SendBossStateChangeMsg(pKernel, pNpcObj->GetConfig(), BS_BOSS_DEAD);			
    return 0;
}

//处理客户端消息
int BossRefreshModule::OnCustomQuerySceneBoss(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    if (!pKernel->Exists(self))
    {
        return 0;
    }
    if (args.GetCount() < 1)
    {
        return 0;
    }

	//查询BOSS信息
	m_pBossRefreshModule->QueryBossInfoInScene(pKernel, self);
    return 0;
}

//查询场景和BOSS信息
int BossRefreshModule::QueryBossInfoInScene(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return 0;
    }
	
	IRecord* pPubBossRec = m_pBossRefreshModule->GetPubBossInSceneRec(pKernel, SCENE_BOSS_TOTAL_REC);
	if (pPubBossRec == NULL)
	{
		return 0;
	}

	int nBossRows = pPubBossRec->GetRows();
	CVarList data;
	data << nBossRows;
	LoopBeginCheck(w);
	for (int i = 0;i < nBossRows;++i)
	{
		LoopDoCheck(w);
		const char* strBossConfig = pPubBossRec->QueryString(i, SCENE_BOSS_INFO_CONFIG);
		data << strBossConfig;
		int nSceneId = pPubBossRec->QueryInt(i, SCENE_BOSS_INFO_SCENEID);
		std::map<int, SceneRefreshInfo>::iterator iter = m_pBossRefreshModule->m_MapSceneBossInfo.find(nSceneId);
		if (iter == m_pBossRefreshModule->m_MapSceneBossInfo.end())
		{
			return 0;
		}
		SceneRefreshInfo& sRefreshInfo = iter->second;

		// 是否需要刷boss
		if (sRefreshInfo.nOpen == 1 && m_pBossRefreshModule->NotRefresh(sRefreshInfo.beginTime, sRefreshInfo.endTime))
		{
			data << BS_BOSS_SLEEP;
		}
		else
		{
			int64_t nDeadTime = pPubBossRec->QueryInt64(i, SCENE_BOSS_DEAD_TIME);
			if (BS_BOSS_BORN == nDeadTime)
			{
				data << nDeadTime;
			}
			else
			{
				int nInterval = QueryBossInterval(sRefreshInfo, strBossConfig);
				if (0 == nInterval)
				{
					continue;
				}
				int nLeftTime = (int)(nInterval - (::time(NULL) - nDeadTime));
				data << (int64_t)nLeftTime;
			}
		}

		const wchar_t* wsKiller = pPubBossRec->QueryWideStr(i, SCENE_BOSS_INFO_KILLER);
		data << nSceneId << wsKiller;
	}
   
    CVarList list;
    list << SERVER_CUSTOMMSG_BOSS_INFO
		<< SC_SUBMSG_QUERY_BOSS_IN_SCENE
		<< data;

    pKernel->Custom(self, list);

    return 0;
}

// 查询boss数据
const BossRefreshModule::BossInfo* BossRefreshModule::FindBossInfo(IKernel* pKernel, const string& strBossConfig)
{
	int nSceneId = pKernel->GetSceneId();
	SceneInfoMap::iterator iter = m_MapSceneBossInfo.find(nSceneId);
	if (iter == m_MapSceneBossInfo.end())
	{
		return NULL;
	}

	vector<BossInfo>& bossinfo = iter->second.VecSceneBossInfo;
	BossInfo* pFindInfo = NULL;
	int nSize = (int)bossinfo.size();
	LoopBeginCheck(w);
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(w);
		BossInfo& info = bossinfo[i];
		if (info.sBossConfig == strBossConfig)
		{
			pFindInfo = &bossinfo[i];
			break;
		}
	}
	return pFindInfo;
}

//创建boss
void BossRefreshModule::CreateBoss(IKernel* pKernel, const char* strBossConfig, IRecord* pBossRecord, int nRowIndex)
{
	if (NULL == pBossRecord || nRowIndex < 0 || nRowIndex > pBossRecord->GetRows())
	{
		return;
	}
	const BossInfo* pBossInfo = FindBossInfo(pKernel, strBossConfig);
	if (NULL == pBossInfo)
	{
		return;
	}

	float fBornY = pKernel->GetMapHeight(pBossInfo->BossPos.fPosX, pBossInfo->BossPos.fPosZ);
	PERSISTID boss = pKernel->CreateObjectArgs("", pBossInfo->sBossConfig.c_str(), 0, pBossInfo->BossPos.fPosX, fBornY, pBossInfo->BossPos.fPosZ, pBossInfo->BossPos.fOrient, CVarList() << CREATE_TYPE_PROPERTY_VALUE << FIELD_PROP_GROUP_ID << -1);
	if (!pKernel->Exists(boss))
	{
		return;
	}

	pBossRecord->SetObject(nRowIndex, SCENE_BOSS_OBJ, boss);
	pBossRecord->SetFloat(nRowIndex, SCENE_BOSS_POS_X, pBossInfo->BossPos.fPosX);
	pBossRecord->SetFloat(nRowIndex, SCENE_BOSS_POS_Z, pBossInfo->BossPos.fPosZ);
	pBossRecord->SetFloat(nRowIndex, SCENE_BOSS_POS_ORIENT, pBossInfo->BossPos.fOrient);
}

// 读取场景boss数据
bool BossRefreshModule::LoadSceneBoss(IKernel* pKernel)
{
	m_MapSceneBossInfo.clear();
	std::string xmlPath = pKernel->GetResourcePath();
	xmlPath += BOSS_REFRESH_INFO;

	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[BossRefreshModule::LoadSceneBoss] failed");
		return false;
	}

	try
	{
		char* pfilebuff = GetFileBuff(xmlPath.c_str());

		if (NULL == pfilebuff)
		{
			return false;
		}

		xml_document<> doc;

		doc.parse<rapidxml::parse_default>(pfilebuff);

		xml_node<>* pNodeRoot = doc.first_node("Object");
		if (NULL == pNodeRoot)
		{
			return false;
		}
		xml_node<>* pNode = pNodeRoot->first_node("Property");
		if (NULL == pNode)
		{
			return false;
		}

		LoopBeginCheck(d);
		while (pNode != NULL)
		{
			LoopDoCheck(d);
			SceneRefreshInfo tempSceneRefreshInfo;
			int sceneID = convert_int(QueryXmlAttr(pNode, "ID"), 0);
			tempSceneRefreshInfo.nOpen = convert_int(QueryXmlAttr(pNode, "Open"), 0);
			tempSceneRefreshInfo.beginTime = util_convert_string_to_sec(QueryXmlAttr(pNode, "BeginTime"));
			tempSceneRefreshInfo.endTime = util_convert_string_to_sec(QueryXmlAttr(pNode, "EndTime"));

			xml_node<>* pItemPare = pNode->first_node("BossItems");
			if (NULL == pItemPare)
			{
				continue;
			}
			xml_node<>* pItem = pItemPare->first_node("BossItem");
			if (NULL == pItem)
			{
				continue;
			}
			LoopBeginCheck(e);
			while (pItem != NULL)
			{
				LoopDoCheck(e);
				BossInfo tempBossInfo;
				const char* str = QueryXmlAttr(pItem, "Boss");
				tempBossInfo.sBossConfig = str;
				tempBossInfo.nInterval = convert_int(QueryXmlAttr(pItem, "Interval"), 0);
				const char* strPosInfo = QueryXmlAttr(pItem, "Position");
				if (!CommRuleModule::ParsePosInfo(tempBossInfo.BossPos, strPosInfo))
				{
					extend_warning(LOG_WARNING, "%s, %d scene, %s boss pos error", BOSS_REFRESH_INFO, sceneID, str);
					continue;
				}

				tempSceneRefreshInfo.VecSceneBossInfo.push_back(tempBossInfo);
				pItem = pItem->next_sibling("BossItem");
			}

			m_MapSceneBossInfo.insert(make_pair(sceneID, tempSceneRefreshInfo));
			pNode = pNode->next_sibling("Property");
		}
	}
	catch (parse_error& e)
	{
		::extend_warning(pKernel, e.what());

		return false;
	}
	return true;
}

//获取公共服场景BOSS信息表
IRecord * BossRefreshModule::GetPubBossInSceneRec(IKernel * pKernel, const char* strRecName)
{
    IPubSpace * pDomainSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
    if (NULL == pDomainSpace)
    {
        return NULL;
    }

    // 获取公共空间
    IPubData * pPubData = pDomainSpace->GetPubData(GetDomainName(pKernel).c_str());
    if (NULL == pPubData)
    {
        return NULL;
    }

	IRecord * sceneBossRec = pPubData->GetRecord(strRecName);
    if (NULL == sceneBossRec)
    {
        return NULL;
    }

    return sceneBossRec;
}

// 查询某个场景的野外boss数据 outData 格式:outData << bossid << lefttime << bossid << lefttime..
bool BossRefreshModule::QueryBossInfo(IKernel * pKernel, int nSceneId, IVarList& outData)
{
	outData.Clear();
	IRecord* pPubBossRec = m_pBossRefreshModule->GetPubBossInSceneRec(pKernel, SCENE_BOSS_TOTAL_REC);
	if (pPubBossRec == NULL)
	{
		return false;
	}

	int nBossRows = pPubBossRec->GetRows();
	LoopBeginCheck(w);
	for (int i = 0; i < nBossRows; ++i)
	{
		LoopDoCheck(w);
		const char* strBossConfig = pPubBossRec->QueryString(i, SCENE_BOSS_INFO_CONFIG);
		int nTmpSceneId = pPubBossRec->QueryInt(i, SCENE_BOSS_INFO_SCENEID);
		if (nTmpSceneId != nSceneId)
		{
			continue;
		}
		std::map<int, SceneRefreshInfo>::iterator iter = m_pBossRefreshModule->m_MapSceneBossInfo.find(nSceneId);
		if (iter == m_pBossRefreshModule->m_MapSceneBossInfo.end())
		{
			continue;
		}
		SceneRefreshInfo& sRefreshInfo = iter->second;
		outData << strBossConfig;
		// 是否需要刷boss
		if (sRefreshInfo.nOpen == 1 && m_pBossRefreshModule->NotRefresh(sRefreshInfo.beginTime, sRefreshInfo.endTime))
		{
			outData << BS_BOSS_SLEEP;
		}
		else
		{
			int64_t nDeadTime = pPubBossRec->QueryInt64(i, SCENE_BOSS_DEAD_TIME);
			if (BS_BOSS_BORN == nDeadTime)
			{
				outData << nDeadTime;
			}							 
			else
			{
				int nInterval = QueryBossInterval(sRefreshInfo, strBossConfig);
				if (nInterval > 0)
				{
					int64_t nLeftTime = (int64_t)(nInterval - (::time(NULL) - nDeadTime));
					outData << nLeftTime;
				}
			}
		}
	}

	return true;
}

// 场景boss状态改变
void BossRefreshModule::SendBossStateChangeMsg(IKernel * pKernel, const char* strBossId, int nState)
{
	if (StringUtil::CharIsNull(strBossId))
	{
		return;
	}

	int nSceneId = pKernel->GetSceneId();
	std::map<int, SceneRefreshInfo>::iterator iter = m_pBossRefreshModule->m_MapSceneBossInfo.find(nSceneId);
	if (iter == m_pBossRefreshModule->m_MapSceneBossInfo.end())
	{
		return;
	}
	SceneRefreshInfo& sRefreshInfo = iter->second;

	CVarList msg;
	msg << SERVER_CUSTOMMSG_BOSS_INFO << SC_SUBMSG_BOSS_STATE_CHANGE << strBossId;
	
	// 是否需要刷boss
	if (sRefreshInfo.nOpen == 1 && m_pBossRefreshModule->NotRefresh(sRefreshInfo.beginTime, sRefreshInfo.endTime))
	{
		msg << BS_BOSS_SLEEP;
	}
	else
	{
		if (nState == BS_BOSS_DEAD)
		{
			int nInterval = QueryBossInterval(sRefreshInfo, strBossId);
			if (0 == nInterval)
			{
				return;
			}
			msg << nInterval;
		}
		else
		{
			msg << BS_BOSS_BORN;
		}
	}

	pKernel->CustomByScene(nSceneId, msg);
}

// 查询boss的刷新间隔
int BossRefreshModule::QueryBossInterval(const SceneRefreshInfo& sceneInfo, const char* strBoss)
{
	int nInterval = 0;
	int nSize = sceneInfo.VecSceneBossInfo.size();
	LoopBeginCheck(s);
	for (int i = 0; i < nSize;++i)
	{
		const BossInfo& info = sceneInfo.VecSceneBossInfo[i];
		if (strcmp(info.sBossConfig.c_str(), strBoss) == 0)
		{
			nInterval = info.nInterval;
			break;
		}
	}

	return nInterval;
}

//是否在不刷新时间段内
bool BossRefreshModule::NotRefresh(int beginTime, int endTime)
{
    int curTime = util_convert_zerotime_to_sec();
    if (beginTime > endTime)
    {
        //跨天
        if (curTime >= beginTime || curTime <= endTime)
        {
            return true;
        }
    }
    else if (beginTime < endTime)
    {
        //当天
        if (curTime >= beginTime && curTime <= endTime)
        {
            return true;
        }
    }

    return false;
}

// 重新加载技能配置
void BossRefreshModule::ReloadConfig(IKernel* pKernel)
{
	m_pBossRefreshModule->LoadResource(pKernel);
}

// 获取公共数据名称
const std::wstring & BossRefreshModule::GetDomainName(IKernel * pKernel)
{
	//基本检查
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Boss_%d", server_id);

		m_domainName = wstr;
	}

	return m_domainName;
}