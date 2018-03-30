#include "GuildBuildingManage.h"
#include "FsGame\Define\GuildDefine.h"
#include "utils\extend_func.h"
#include "utils\XmlFile.h"
#include "utils\EnvirDefine.h"
#include "..\Domain\EnvirValue.h"
#include "GuildBaseManager.h"
#include "FsGame\Define\CommandDefine.h"
#include "GuildSkillManage.h"
#include "..\Domain\DomainRankList.h"
#include "FsGame\Define\RankingDefine.h"
#include "GuildRareTreasure.h"
#include "FsGame\Helper\GuildHelper.h"
#define BUILDING_LVUP_EXP "ini\\rule\\guild\\pub\\guild_building_levelup_exp.xml" 
#define BUILDING_LVUP_CONFIG "ini\\rule\\guild\\pub\\guild_building_levelup_config.xml"
#define GUILDING_MAINTAIN_COST "ini\\rule\\guild\\pub\\guild_building_maintain_cost.xml"
#include <time.h>
#include "utils\record.h"

GuildBuildingManage* GuildBuildingManage::m_pInstance = NULL;

 

 bool GuildBuildingManage::Init(IPubKernel* pPubKernel)
{
	ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(CheckGuildCapital);
	ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(CheckMainTainCostAndSelfGuildCapital);
	ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(CheckGuildFanRongDu);
	ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(ChekcGuildBuildTypeAndLevel);
	ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(CheckGuildJuYiTingBuildLv);
	ADD_GUILD_BUILD_LVUP_DO_DEC_ACTION(DoDecGuildCapital);
	ADD_GUILD_BUILD_LVUP_DO_DEC_ACTION(DoDecGuildFanYongDu);

	if (!LoadResConfig(pPubKernel)){
		return false;
	}

	std::wstring guildPubDataName = (std::wstring(GUILD_PUB_DATA) + m_ServerId).c_str();
	pPubKernel->AddTimer("add_building_exp", GuildBuildingManage::Update, GetName(), guildPubDataName.c_str(), BUILD_EXP_CYCLE_TIME, 0);


	if (m_pGuildPubData->FindAttr(CLOSE_SERVER_UNIX_TIME))
	{
		m_pGuildPubData->SetAttrInt64(CLOSE_SERVER_UNIX_TIME, ::time(NULL) );
		m_pGuildPubData->SetAttrSave(CLOSE_SERVER_UNIX_TIME,true);
	}

	int64_t closeTime = m_pGuildPubData->QueryAttrInt64(CLOSE_SERVER_UNIX_TIME);
	//计算关服开服 流逝的时间
	int diffSec = static_cast<int> (::time(NULL) - closeTime);
	if (diffSec)
	{
		AddAllGuildBuildingExp(pPubKernel, diffSec);
	}

	return true;
}

 bool GuildBuildingManage::LoadResConfig(IPubKernel*pPubKernel)
 {
	 return LoadGuildBuingLvUpExpConfig(pPubKernel) 
		 &&LoadGuildBuingLvUpConfig(pPubKernel)
		 && LoadGuildBuildingMaintainCost(pPubKernel);
}


 bool GuildBuildingManage::LoadGuildBuingLvUpExpConfig(IPubKernel*pPubKernel)
 {
	 m_guildBuildingLvUpExp.clear();
	 // 获得路径
	 std::string strFilePath = pPubKernel->GetResourcePath();
	 strFilePath += BUILDING_LVUP_EXP;
	 //加载失败
	 CXmlFile ini(strFilePath.c_str());
	 std::string log;
	 if (!ini.LoadFromFile(log))
	 {
		 ::extend_warning(
			 LOG_ERROR,
			 "[StaticDataQueryModule::LoadStaticData] %s",
			 log.c_str());
		 return false;
	 }
	 int sect_num = ini.GetSectionCount();
	 for (int i = 0; i < sect_num; i++)
	 {
		 int cols = ini.GetSectionItemCount(i);
		 for (int j = 0; j < cols; j++)
		 {
			 const char* key = ini.GetSectionItemKey(i, j);
			 const char* val = ini.GetSectionItemValue(i, j);
			 std::string keyName = key;
			 keyName = keyName.erase(0, 1);
			 int buildingType = StringUtil::StringAsInt(keyName.c_str());
			 int exp = StringUtil::StringAsInt(val);
			 m_guildBuildingLvUpExp[buildingType][i] = exp;

		 }

	 }

	 return true;
 }

 bool GuildBuildingManage::LoadGuildBuingLvUpConfig(IPubKernel*pPubKernel)
 {
	 m_doDecCostActionByBuildingLvUp.clear();
	 std::string strFilePath = pPubKernel->GetResourcePath();
	 strFilePath += BUILDING_LVUP_CONFIG;
	 //加载失败
	 CXmlFile ini(strFilePath.c_str());
	 std::string log;
	 if (!ini.LoadFromFile(log))
	 {
		 ::extend_warning(
			 LOG_ERROR,
			 "[StaticDataQueryModule::LoadStaticData] %s",
			 log.c_str());
		 return false;
	 }
	 int sect_num = ini.GetSectionCount();
	 for (int i = 0; i < sect_num; i++)
	 {
		 size_t res = 0;
		 if (!ini.FindSectionItemIndex(i, "BuildingType", res)){
			 return false;
		 }
		 int buildingType = StringUtil::StringAsInt( ini.GetSectionItemValue(i,res));
			
		 if (!ini.FindSectionItemIndex(i, "Level", res)){
			 return false;
		 }
		 int level = StringUtil::StringAsInt(ini.GetSectionItemValue(i, res));

		 int cols = ini.GetSectionItemCount(i);
		 for (int j = 0; j < cols; j++)
		 {
			 const char* key = ini.GetSectionItemKey(i, j);
			 const char* val = ini.GetSectionItemValue(i,j);
			 auto it = m_CheckBuildingLvUpConditoinList.find(key);
			 if (it != m_CheckBuildingLvUpConditoinList.end())
			 {
				 auto condition = it->second();
				 if (condition != NULL)
				 {
					 condition->LoadXml(val);
					 m_checkBuildingLvUpCondition[buildingType][level].push_back(condition);
				 }
			 }
			 auto its = m_DoDecCostActionByBuildingLvUpList.find(key);
			 if (its != m_DoDecCostActionByBuildingLvUpList.end())
			 {
				 auto action = its->second();
				 if (action != NULL)
				 {
					 action->LoadXml(val);
					 m_doDecCostActionByBuildingLvUp[buildingType][level].push_back(action);

				 }
			 }

		 }

	 }


	 return true;
 }

 bool GuildBuildingManage::LoadGuildBuildingMaintainCost(IPubKernel*pPubKernel)
 {
	 m_guildBuildingMaintainCost.clear();
		 // 获得路径
	 std::string strFilePath = pPubKernel->GetResourcePath();
	 strFilePath += GUILDING_MAINTAIN_COST;
	 //加载失败
	 CXmlFile ini(strFilePath.c_str());
	 std::string log;
	 if (!ini.LoadFromFile(log))
	 {
		 ::extend_warning(
			 LOG_ERROR,
			 "[StaticDataQueryModule::LoadStaticData] %s",
			 log.c_str());
		 return false;
	 }
	 int sect_num = ini.GetSectionCount();
	 for (int i = 0; i < sect_num; i++)
	 {
		 int cols = ini.GetSectionItemCount(i);
		 for (int j = 0; j < cols; j++)
		 {
			 const char* key = ini.GetSectionItemKey(i, j);
			 const char* val = ini.GetSectionItemValue(i, j);
			 std::string keyName = key;
			 keyName = keyName.erase(0, 1);
			 int buildingType = StringUtil::StringAsInt(keyName.c_str());
			 int exp = StringUtil::StringAsInt(val);
			 m_guildBuildingMaintainCost[buildingType][i + 1] = exp;

		 }

	 }
	 return true;
 }

 void GuildBuildingManage::OnAddGuildildBuildExp(IPubKernel*pPubKernel, const wchar_t*guildName, int expValue)
 {
	 const std::wstring guildDataName = guildName + m_ServerId;
	 IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	 if (guildData != NULL){

		 IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
		 if (pGuildBuildingRec != NULL)
		 {
			 int rows = pGuildBuildingRec->GetRows();

			 for (int i = 0; i < rows; i++)
			 {
				 int buildingState = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_STATE);
				 if (buildingState == GUILD_BUILDING_STATE_LEVELUPING)
				 {
					 int buildingExp = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_EXP);
					 buildingExp += expValue;
					 int buildinglevel = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_LEVEL);
					 int buildingType = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_TYPE);
					 int needExp = GetBuildingLvExp(buildingType, buildinglevel);
					 if (needExp !=-1&&buildingExp >= needExp)
					 {
						 ++buildinglevel;
						 pGuildBuildingRec->SetInt(i, GUILD_BUILDING_REC_COL_LEVEL, buildinglevel);
						 pGuildBuildingRec->SetInt(i, GUILD_BUILDING_REC_COL_STATE, GUILD_BUILDING_STATE_CAN_LEVELUP);
						 pGuildBuildingRec->SetInt(i, GUILD_BUILDING_REC_COL_EXP,0);
						 if (buildingType == GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_JU_YI_TING)
						 {
							 GuildLvUp(pPubKernel, guildName, buildinglevel);
						 }
						 else if (buildingType == GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_SHU_YUAN)
						 {
							 GuildSkillManage::m_pInstance->RefreshGuildSkillLevel(pPubKernel,guildName);
						 }
						 std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_LV_UP_SUCCESS) + L"," + StringUtil::IntAsWideStr(buildingType) + L"," + StringUtil::IntAsWideStr(buildinglevel);
						 GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());
						 GuildBaseManager::m_pInstance->ClearPetition(pPubKernel, guildName, buildingType);

						 // 通知帮会成员 建筑升级
						 BroadcastBuildingLevelup(pPubKernel, guildName, buildinglevel, buildingType);
					 }
					 else
					 {
						 pGuildBuildingRec->SetInt(i, GUILD_BUILDING_REC_COL_EXP, buildingExp);
					 }
				 }
			 }
		 }
	 }
 }
 void GuildBuildingManage::GuildLvUp(IPubKernel* pPubKernel, const wchar_t*guildName, int level)
 {

	 IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	 if (guildList == NULL)
	 {
		 return;
	 }
	 int guildRow = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	 if (guildRow < 0)
	 {
		 return;
	 }
	 guildList->SetInt(guildRow, GUILD_LIST_REC_COL_CAPTAIN_LEVEL, level);

	 GuildSortHelper_t guildSortHelperOld;
	 if (!GuildBaseManager::m_pInstance->DumpGuildSortHelper(guildList, guildRow, &guildSortHelperOld))
	 {
		 return;
	 }

	 std::wstring wstrlogArgs = std::wstring(guildName) + L"," + StringUtil::IntAsWideStr(level);
	 GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_UP_LEVEL, wstrlogArgs.c_str());

	 // 通知帮会成员 帮会升级
	 CommandGuildLevelChange(pPubKernel, guildName, level, true);

	 // 公会排序
	 GuildSortHelper_t guildSortHelperNew;
	 if (GuildBaseManager::m_pInstance->DumpGuildSortHelper(guildList, guildRow, &guildSortHelperNew))
	 {
		 GuildBaseManager::m_pInstance->SortGuild(&guildSortHelperOld, &guildSortHelperNew);
	 }

	 if (!PrepareGuildManage::m_pInstance->IsPrepareGulild(guildName))
	 {
		 // 更新公会等级排行榜
		 IPubSpace* pPubSpace = pPubKernel->GetPubSpace(PUBSPACE_DOMAIN);
		 if (NULL == pPubSpace)
		 {
			 return;
		 }

		 // 更新排行榜
		 CVarList update_msg;

		 int nFightPower = guildList->QueryInt(guildRow, GUILD_LIST_REC_COL_FIGHT_ABILITY);
		 int nNation = guildList->QueryInt(guildRow, GUILD_LIST_REC_COL_NATION);
		 int nBoom = GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_FANRONGDU);

		 int target_ident = 0;
		 const wchar_t* target_alias = L"";
		 GuildHelper::GetGuildIdent(guildName, target_ident, target_alias);

		 update_msg << RESERVED_NUMBER
			 << GUILD_LEVEL_RANK_LIST
			 << INSERT_IF_NOT_EXIST
			 << "guild_name" << guildName
			 << "guild_level" << level
			 << "guild_fight_power" << nFightPower
			 << "guild_nation" << nNation
			 << "guild_boom" << nBoom
			 << "guild_identifying" << target_ident;

		 DomainRankList::m_pDomainRankList->UpdateEntry(pPubKernel, pPubSpace, update_msg,1);
	 }
 }

 // 通知帮会成员 帮会等级改变
 int GuildBuildingManage::CommandGuildLevelChange(IPubKernel* pPubKernel, const wchar_t* guildName, int level, bool bUp)
 {
	 // 帮会公共空间
	 std::wstring guildDataName = guildName + m_ServerId;
	 IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	 if (pGuildData == NULL)
	 {
		 return 0;
	 }

	 // 成员表格
	 IRecord* pRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	 if (pRecord == NULL)
	 {
		 return 0;
	 }

	 // 成员数量
	 int memberCount = pRecord->GetRows();
	 // 消息
	 CVarList msg;
	 msg << (bUp ? COMMAND_GUILD_UP_LEVEL : COMMAND_GUILD_DOWN_LEVEL) << level;

	 // 通知成员帮会等级改变
	 LoopBeginCheck(a);
	 for (int i = 0; i < memberCount; ++i)
	 {
		 LoopDoCheck(a);

		 const wchar_t* playerName = pRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
		 if (StringUtil::CharIsNull(playerName))
		 {
			 continue;
		 }

		 // 通知玩家
		 pPubKernel->CommandByName(playerName, msg);
	 }

	 return 0;
 }

 void GuildBuildingManage::BroadcastBuildingLevelup(IPubKernel* pPubKernel, const wchar_t* guildName, int nLevel, int building)
 {
	 CVarList msg;
	 msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_BUILDING_LEVEL_UP_BROADCAST <<
		 guildName << building << nLevel;
	 pPubKernel->SendPublicMessage(0, 0, msg);
 }

 int GuildBuildingManage::GetBuildingLevel(const wchar_t* guildName, GUILD_BUILDING_TYPE type)
 {
	 const std::wstring guildDataName = guildName + m_ServerId;
	 IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	 if (guildData != NULL){

		 IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
		 if (pGuildBuildingRec != NULL)
		 {
			 int row = pGuildBuildingRec->FindInt(GUILD_BUILDING_REC_COL_TYPE, type);
			 if (row >= 0)
			 {
				 return pGuildBuildingRec->QueryInt(row, GUILD_BUILDING_REC_COL_LEVEL);
			 }
		 }
	 }
	 return 0;
 }

 void GuildBuildingManage::TimingMaintain(IPubKernel*pPubKernel, const char* space_name, const wchar_t* data_name)
 {
	//繁荣度维护消耗
	 int fanRongDuMaintainCost = EnvirValue::EnvirQueryInt(ENV_VALUE_FAN_RONG_DU_MAINTAIN_COST);
	 int maintainAddFanRonDu = EnvirValue::EnvirQueryInt(ENV_VALUE_MAINTAIN_ADD_FAN_RONG_DU);
	 IRecord* pSetList = GuildBuildingManage::m_pInstance->m_pGuildPubData->GetRecord(GUILD_SET_REC);
	 if (pSetList != NULL)
	 {
		 int nRows = pSetList->GetRows();
		 LoopBeginCheck(a);
		 for (int i = 0; i < nRows; ++i)
		 {
			 LoopDoCheck(a);
			 bool isMainTain = false;
			 std::wstring guildName = pSetList->QueryWideStr(i, GUILD_SET_REC_COL_GUILD_NAME);
			 int cost = GuildBuildingManage::m_pInstance->GetMainTainBuildingCost(guildName.c_str());
			 if (GuildNumManage::m_pInstance->CanDecGuildNumValue(guildName.c_str(), GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, cost))
			 {
				 GuildNumManage::m_pInstance->DecGuildNumValue(guildName.c_str(), GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, cost, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_MAINTAIN);
				 pSetList->SetInt(i, GUILD_SET_REC_COL_MAINTAIN_STATE, GUILD_MAINTAIN_STATE::MAINTAIN_SUCCESS);
				 isMainTain = true;
				 GuildNumManage::m_pInstance->AddGuildNumValue(guildName.c_str(), GUILD_NUM_TYPE::GUILD_NUM_FANRONGDU, maintainAddFanRonDu, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_MAINTAIN);
			 }
			 else if (GuildNumManage::m_pInstance->CanDecGuildNumValue(guildName.c_str(), GUILD_NUM_TYPE::GUILD_NUM_FANRONGDU, fanRongDuMaintainCost))
			 {
				 GuildNumManage::m_pInstance->DecGuildNumValue(guildName.c_str(), GUILD_NUM_TYPE::GUILD_NUM_FANRONGDU, fanRongDuMaintainCost, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_MAINTAIN);
				 pSetList->SetInt(i, GUILD_SET_REC_COL_MAINTAIN_STATE, GUILD_MAINTAIN_STATE::MAINTAIN_SUCCESS);
				 isMainTain = true;
				 std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_MAINTAIN_DEC_FRD) + L"," + StringUtil::IntAsWideStr(fanRongDuMaintainCost);
				 GuildBaseManager::m_pInstance->RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());
			 }
			 else
			 {
				 pSetList->SetInt(i, GUILD_SET_REC_COL_MAINTAIN_STATE, GUILD_MAINTAIN_STATE::MAINTAIN_FAILED);
				 GuildNumManage::m_pInstance->DecJianKangDu(guildName.c_str(), GUILD_NUM_CHANGE_MAINTAIN_FAILED_DEC);

				 std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_MAINTAIN_FAIL) ;
				 GuildBaseManager::m_pInstance->RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());
			 }
			
			 if (isMainTain)
			 {
				 GuildSkillManage::m_pInstance->MainTainSuccess(pPubKernel, guildName.c_str());
			 }
		 }
	 }

 }

bool GuildBuildingManage::IsMaintain(const wchar_t*guildName)
{
	IRecord* pGuildSetList = m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetList != NULL)
	{
		int row = pGuildSetList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
		if (row >= 0)
		{
			return pGuildSetList->QueryInt(row, GUILD_SET_REC_COL_MAINTAIN_STATE) == GUILD_MAINTAIN_STATE::MAINTAIN_SUCCESS;
		}
	}
	return false;
}

 bool GuildBuildingManage::CreateGuildInit(IPubData* guildData)
{
	if (guildData == NULL){
		return false;
	}

	//公会建筑
	CVarList cols;
	cols << VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT;

	IRecord* pBuildRec_ = guildData->GetRecord( GUILD_BUILDING_REC );
	if (NULL == pBuildRec_)
	{
		if (!guildData->AddRecord(GUILD_BUILDING_REC, GUILD_BUILDING_REC_MAX_ROW, GUILD_BUILDING_REC_COL_MAX, cols))
		{
			return false;
		}
		guildData->SetRecordSave(GUILD_BUILDING_REC, true);
	}
	else if (check_record_changed(pBuildRec_, cols, GUILD_BUILDING_REC_MAX_ROW))
	{
		// 重新构造表
		change_pub_record(guildData, GUILD_BUILDING_REC, cols, CVarList(), GUILD_BUILDING_REC_MAX_ROW, GUILD_BUILDING_REC_COL_TYPE);
	}

	return true;

}

int GuildBuildingManage::OnPublicMessage(IPubKernel* pPubKernel, int source_id, int scene_id, const IVarList& msg)
{
	const wchar_t* guildName = msg.WideStrVal(1);
	int msgId = msg.IntVal(2);
	switch (msgId)
	{
		case SP_GUILD_BUILDING_LVUP_OPERATE:{
			int operate = msg.IntVal(3);
			if (operate == GUILD_OPERATE_START_LVUP)
			{
				OnLevelUpBuilding(pPubKernel, source_id, scene_id, msg);
			}
			else if (operate == GUILD_OPERATE_STOP_LVUP)
			{
				StopLevelUpBuilding(pPubKernel, source_id, scene_id, msg);
			}
	
		}break;
		case SP_GUILD_BUILDING_LEVEL_ADD_EXP:{
			int addValue = msg.IntVal(3);
			OnAddGuildildBuildExp(pPubKernel, guildName, addValue);
		}break;
	}
	return 0;
}


int GuildBuildingManage::GetBuildingLvingNum(IPubKernel* pPubKernel, const wchar_t* guildName)
{
	int num = 0;
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData != NULL){

		IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
		for (int i = 0; i < pGuildBuildingRec->GetRows();i++)
		{
			if (pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_STATE) == GUILD_BUILDING_STATE_LEVELUPING)
			{
				++num;
			}
		}
	}

	return num;
}

bool GuildBuildingManage::OnLevelUpBuilding(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& msg)
{
	const wchar_t* guildName = msg.WideStrVal(1);
	const wchar_t* playerName = msg.WideStrVal(4);
	int buildingType = msg.IntVal(5);
	int useMiBao = msg.IntVal(6);
	

	int retNum = GetBuildingLvingNum(pPubKernel, guildName);

	if (!useMiBao&&retNum >= 1){
		//不用秘宝可建设最大数量为1
		SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId, playerName, BUILD_LVUP_BUILDING_SAME_TIME_MAX_NUM);
		return false;
	}
	else if (useMiBao&& retNum >= 2){
		//使用秘宝，可建设组大数量为2
		SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId, playerName, BUILD_LVUP_BUILDING_SAME_TIME_MAX_NUM);
		return false;
	}
	else if (useMiBao&&retNum == 0){
		//使用秘宝，但正在升级的建筑为0个，则不使用秘宝
		useMiBao = false;
	}

	if (useMiBao)
		if (!GuildRareTreasure::m_pInstance->GetRareTreasureNum(guildName, GUILD_RARE_TREASURE_TYPE::GUILD_RARE_TREASURE_WELKIN)){
			SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId, playerName, BUILD_LVUP_MI_MAO_ENOUGHT);
			return false;
		}
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL)
	{
		return false;
	}

	IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
	if (pGuildBuildingRec == NULL){
		return false;
	}

	int buildingLevel = 0;
	int row = pGuildBuildingRec->FindInt(GUILD_BUILDING_REC_COL_TYPE, buildingType);
	if (row < 0)
	{
		CVarList rowValue;
		rowValue << buildingType << 0 << 0 << GUILD_BUILDING_STATE_CAN_LEVELUP <<  GUILD_BUILDING_BAO_NONE;
		row = pGuildBuildingRec->AddRowValue(-1, rowValue);
	}
	else
	{
		int  buildingState =  pGuildBuildingRec->QueryInt(row,GUILD_BUILDING_REC_COL_STATE);
		buildingLevel = pGuildBuildingRec->QueryInt(row, GUILD_BUILDING_REC_COL_LEVEL) ;
		if (buildingState == GUILD_BUILDING_STATE_STOP)
		{
			if (useMiBao){
				GuildRareTreasure::m_pInstance->DecRareTreasure(guildName, GUILD_RARE_TREASURE_TYPE::GUILD_RARE_TREASURE_WELKIN, 1, playerName);
				pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_ISBAO, GUILD_BUILDING_BAO_USED);
			}
			else
			{
				pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_ISBAO, GUILD_BUILDING_BAO_NONE);
			}

			pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_STATE, GUILD_BUILDING_STATE_LEVELUPING);
			SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId, playerName, BUILD_LVUP_RET_SUCCESS);
			std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_CONTINUE_BUILDING) + L"," + std::wstring(playerName) + L"," + StringUtil::IntAsWideStr(buildingType) + L"," + StringUtil::IntAsWideStr(buildingLevel + 1);
			GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());
			return true;
		}
	}

	{ //  升级条件检查
		auto it = m_checkBuildingLvUpCondition.find(buildingType);
		if (it == m_checkBuildingLvUpCondition.end()){
			return false;
		}

		auto its = it->second.find(buildingLevel);
		if (its == it->second.end()){
			return false;
		}

		for (auto itt : its->second){
			int ret = itt->Run(guildName);
			if (ret != BUILD_LVUP_RET_SUCCESS)
			{
				SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId, playerName, ret);
				return false;
			}
		}

	}

	{//扣除消耗
		auto it = m_doDecCostActionByBuildingLvUp.find(buildingType);
		if (it == m_doDecCostActionByBuildingLvUp.end()){
			return false;
		}

		auto its = it->second.find(buildingLevel);
		if (its == it->second.end()){
			return false;
		}

		for (auto itt : its->second)
		{
			 itt->Run(guildName);
		}
	}
	pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_STATE, GUILD_BUILDING_STATE_LEVELUPING);
	SendGuildingLevelUpResult(pPubKernel, sourceId, sceneId,  playerName, BUILD_LVUP_RET_SUCCESS);
	if (useMiBao){
		GuildRareTreasure::m_pInstance->DecRareTreasure(guildName, GUILD_RARE_TREASURE_TYPE::GUILD_RARE_TREASURE_WELKIN, 1,playerName);
		pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_ISBAO, GUILD_BUILDING_BAO_USED);
	}
	else
	{
		pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_ISBAO, GUILD_BUILDING_BAO_NONE);
	}
	std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_START_BUILDING) + L","+std::wstring(playerName) + L","  +StringUtil::IntAsWideStr(buildingType) + L"," + StringUtil::IntAsWideStr(buildingLevel + 1);
	GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());
	return true;

}


int GuildBuildingManage::Update(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time)
{
	m_pInstance->AddAllGuildBuildingExp(pPubKernel, BUILD_EXP_CYCLE_TIME / 1000);
	m_pInstance->m_pGuildPubData->SetAttrInt64(CLOSE_SERVER_UNIX_TIME, ::time(NULL));
	return 0;
}

void GuildBuildingManage::AddAllGuildBuildingExp(IPubKernel* pPubKernel,int exp)
{
	IRecord* guildList = GuildBuildingManage::m_pInstance->m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList != NULL)
	{
		for (int row = 0; row < guildList->GetRows(); row++)
		{
			const wchar_t* guildName = guildList->QueryWideStr(row, GUILD_LIST_REC_COL_NAME);
			m_pInstance->OnAddGuildildBuildExp(pPubKernel, guildName, exp);
		}
	}
}

void GuildBuildingManage::SendGuildingLevelUpResult(IPubKernel* pPubKernel, int sourceId, int sceneId, const wchar_t* playerName, int result)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_BUILDING_LEVEL_UP <<
		  playerName << result;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}

int GuildBuildingManage::GetBuildingLvExp(int buildingType, int level)
{
	auto it = m_guildBuildingLvUpExp.find(buildingType);
	if (it != m_guildBuildingLvUpExp.end())
	{
		auto its = it->second.find(level);
		if (its != it->second.end())
		{
			return its->second;
		}
	}
	return -1;
}

bool GuildBuildingManage::StopLevelUpBuilding(IPubKernel* pPubKernel, int source_id, int scene_id, const IVarList& msg)
{
	const wchar_t* guildName = msg.WideStrVal(1);
	const wchar_t* playerName = msg.WideStrVal(4);
	int buildingType = msg.IntVal(5);

	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL)
	{
		return false;
	}

	IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
	if (pGuildBuildingRec == NULL){
		return false;
	}

	int buildingLevel = 0;
	int row = pGuildBuildingRec->FindInt(GUILD_BUILDING_REC_COL_TYPE, buildingType);
	if (row >= 0)
	{
		int  buildingState = pGuildBuildingRec->QueryInt(row, GUILD_BUILDING_REC_COL_STATE);
		if (buildingState == GUILD_BUILDING_STATE_LEVELUPING)
		{
			pGuildBuildingRec->SetInt(row, GUILD_BUILDING_REC_COL_STATE, GUILD_BUILDING_STATE_STOP);
			SendGuildingLevelUpResult(pPubKernel, source_id, scene_id, playerName, BUILD_LVUP_STOP_BUILDING_SUCCESS);
			return true;
		}
	}
	SendGuildingLevelUpResult(pPubKernel, source_id, scene_id, playerName, BUILD_LVUP_STOP_BUILDING_ERROR);

	std::wstring  wstrlogArgs = StringUtil::IntAsWideStr(BUILDING_LOG_TYPE_STOP_BUILDING) + L"," + std::wstring(playerName) + L","+ StringUtil::IntAsWideStr(buildingType) + L"," + StringUtil::IntAsWideStr(buildingLevel);
	GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_BUILDING, wstrlogArgs.c_str());

	return false;
}

int GuildBuildingManage::GetMainTainBuildingCost(const wchar_t*guildName)
{
	int totalCost = 0;
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData != NULL){

		IRecord * pGuildBuildingRec = guildData->GetRecord(GUILD_BUILDING_REC);
		if (pGuildBuildingRec != NULL)
		{
			int rows = pGuildBuildingRec->GetRows();

			for (int i = 0; i < rows; i++)
			{
				int buildingLevel = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_LEVEL);
				int buildingType = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_TYPE);
				auto it = m_guildBuildingMaintainCost.find(buildingType);
				if (it != m_guildBuildingMaintainCost.end())
				{
					auto its = it->second.find(buildingLevel);
					if (its != it->second.end())
					{
						totalCost += its->second;
					}
				}
			}
		}
	}

	return totalCost;
}

