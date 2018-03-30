#include "GuildNumManage.h"
#include "utils\string_util.h"
#include "GuildBaseManager.h"
#include "utils\XmlFile.h"
#include "utils\extend_func.h"
#include "GuildBuildingManage.h"
#include "utils\record.h"
#include "FsGame\Define\GuildDefine.h"
GuildNumManage* GuildNumManage::m_pInstance = NULL;

#define GUILD_NUM_INIT_CONFIG "ini\\rule\\guild\\pub\\guild_num_init_config.xml"
#define GUILD_BUILD_ADDITION "ini\\rule\\guild\\pub\\guild_building_addition_guild_num_upper_limit.xml"
#define GUILD_NUM_BUY_CONFIG "ini\\rule\\guild\\pub\\guild_num_buy_cost.xml"
#define GUILD_JIAN_KANG_DU_CONFIG "ini\\rule\\guild\\pub\\guild_jian_kang_du_config.xml"


bool GuildNumManage::Init(IPubKernel* pPubKernel)
{
	if (!LoadResConfig(pPubKernel))
	{
		return false;
	}

	return true;
}

bool GuildNumManage::CreateGuildInit(IPubData* guildData)
{
	if (guildData == NULL){
		return false;
	}
	if (!guildData->FindRecord(GUILD_NUM_REC))
	{
		//数值
		CVarList cols;
		cols << VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT;
		if (!guildData->AddRecord(GUILD_NUM_REC, GUILD_NUM_REC_MAX_ROW, GUILD_NUM_REC_COL_MAX, cols))
		{
			return false;
		}
	}
	if (!guildData->FindRecord(GUILD_NUM_BUY_REC))
	{ //数值购买计数
		CVarList cols;
		cols << VTYPE_INT
			<< VTYPE_INT;
		if (!guildData->AddRecord(GUILD_NUM_BUY_REC, GUILD_NUM_REC_MAX_ROW, GUILD_NUM_BUY_REC_COL_MAX, cols))
		{
			return false;
		}
	}
	if (!guildData->FindRecord(GUILD_GAIN_CAPITAL_REC))
	{ //每日贡献组织资金
		CVarList cols;
		cols << VTYPE_WIDESTR
			 << VTYPE_INT
			 << VTYPE_INT;
		if (!guildData->AddRecord(GUILD_GAIN_CAPITAL_REC, GUILD_NUM_REC_MAX_ROW, GUILD_GAIN_CAPITAL_REC_COL_MAX, cols))
		{
			return false;
		}
	}

	{
		//组织数值初始化
		IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
		if (pGuildNumRc == NULL){
			return false;
		}

		for (auto it : m_initGuildNumCofig)
		{
			int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, it.first);
			if (row < 0)
			{
				pGuildNumRc->AddRowValue(-1, CVarList() << it.first << it.second.m_initNum << 0);
			}
		}
	}

	if (!guildData->FindAttr(DAILY_GET_GUILD_CAPITAL))
	{
		guildData->AddAttr(DAILY_GET_GUILD_CAPITAL, VTYPE_INT);
		guildData->SetAttrSave(DAILY_GET_GUILD_CAPITAL,true);
	}

	return true;
}

int GuildNumManage::OnPublicMessage(IPubKernel* pPubKernel, int source_id, int scene_id, const IVarList& msg)
{
	const wchar_t* guildName = msg.WideStrVal(1);
	int msgId = msg.IntVal(2);
	switch (msgId)
	{
		case SP_GUILD_ADD_GUILD_NUM:
		{
			auto numType = static_cast<GUILD_NUM_TYPE>(msg.IntVal(3));
			int numValue = msg.IntVal(4);
			auto numChangeDes = static_cast<GUILD_NUM_CHANGE>(msg.IntVal(5));
			const wchar_t* playerName = msg.WideStrVal(5);
			AddGuildNumValue(guildName, numType, numValue, numChangeDes, playerName);
		}break;

		case SP_GUILD_DEC_GUILD_NUM:
		{
			int index = 3;
			GUILD_NUM_TYPE guildNumType = static_cast<GUILD_NUM_TYPE>(msg.IntVal(index++));
			int value = msg.IntVal(index++);
			GUILD_NUM_CHANGE desc = static_cast<GUILD_NUM_CHANGE>(msg.IntVal(index++));
			const wchar_t* playerName = msg.WideStrVal(index++);
			if (DecGuildNumValue(guildName, guildNumType, value, desc, playerName))
			{
				if (!StringUtil::CharIsNull(playerName ))
				{
					CVarList msg;
					msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_NUM_CHANGE
						<< playerName << guildNumType << GetGuildNumValue(guildName, guildNumType);
					pPubKernel->SendPublicMessage(source_id, scene_id, msg);
				}
			}

		}break;
		case SP_GUILD_NUM_BUY:
		{
			OnBuyGuildNum(pPubKernel, source_id, scene_id, msg);
		}break;
		case SP_GUILD_GET_JIAN_KANG_DU_GIF:
		{
			OnGetJianKangDuGif(pPubKernel,source_id,scene_id,msg);
		}break;
	}

	return 0;
}

bool GuildNumManage::LoadResConfig(IPubKernel*pPubKernel)
{
	return LoadGuildNumInitConfig(pPubKernel)
		&& LoadGuildBuildingAdditoinUpperLimit(pPubKernel)
		&& LoadGuildNumBuyCostConfig(pPubKernel)
		&& LoadGuildJianKangDuConfig(pPubKernel);
}

bool GuildNumManage::LoadGuildNumInitConfig(IPubKernel*pPubKernel)
{

	m_initGuildNumCofig.clear();
	std::string xmlPath = pPubKernel->GetResourcePath();
	xmlPath += GUILD_NUM_INIT_CONFIG;
	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[MasterAprtcModule::LoadResource] failed!");
		return false;
	}

	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a);	
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a);	
		const char* section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		int type = atoi(section);
		GuildNumInit info;
		info.m_initNum = xml.ReadInteger(section, "InitNum", 20);
		info.m_upperLimit = xml.ReadInteger(section, "UpperLimit", 60);
		m_initGuildNumCofig.insert(std::make_pair(type, info));
	}
	return true;
}

bool GuildNumManage::LoadGuildBuildingAdditoinUpperLimit(IPubKernel*pPubKernel)
{
	m_guildBuildingAddition.clear();
	std::string xmlPath = pPubKernel->GetResourcePath();
	xmlPath += GUILD_BUILD_ADDITION;
	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[MasterAprtcModule::LoadResource] failed!");
		return false;
	}

	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a);
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a);
		const char* section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		int guildNumType = xml.ReadInteger(section,"GuildNumType",-1);
		int  guildBuildType =  xml.ReadInteger(section, "GuildBuildType", -1);
		int  guildLevel =  xml.ReadInteger(section, "GuildBuildingLevel", -1);
		int  guildUpperLimit =  xml.ReadInteger(section, "UpperLimit", -1);
	
		auto it = m_guildBuildingAddition.find(guildNumType);
		if (it != m_guildBuildingAddition.end())
		{
			it->second.m_addition[guildLevel] = guildUpperLimit;
		}
		else
		{
			GuildBuildingAddition addition;
			addition.m_GuidBuingType = guildBuildType;
			addition.m_addition[guildLevel] = guildUpperLimit;
			m_guildBuildingAddition[guildNumType] = addition;
		}
	}
	return true;
}

bool GuildNumManage::LoadGuildNumBuyCostConfig(IPubKernel*pPubKernel)
{
	m_buyGuildNumConfig.clear();
	std::string xmlPath = pPubKernel->GetResourcePath();
	xmlPath += GUILD_NUM_BUY_CONFIG;
	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[MasterAprtcModule::LoadResource] failed!");
		return false;
	}

	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a);
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a);
		const char* section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		GuildNumBuyCost  info;

		int guildNumType = xml.ReadInteger(section, "GuildNumType", 0);
		info.m_recoverValue  = xml.ReadInteger(section, "RecoverValue", 0);
		info.m_buyMaxTimes = xml.ReadInteger(section, "BuyMaxTimes", 0);
		info.m_buyValue = xml.ReadInteger(section, "BuyValue", 0);

		const char* buyCost = xml.ReadString(section, "BuyCost", "");
		
		CVarList var;
		StringUtil::SplitString(var, buyCost, ",");
		for (size_t i = 0; i < var.GetCount(); i++)
		{
			info.m_buyCost.push_back(var.IntVal(i));
		}
		m_buyGuildNumConfig[guildNumType] = info;
	}
	return true;
}


bool GuildNumManage::LoadGuildJianKangDuConfig(IPubKernel*pPubKernel)
{
	
	std::string xmlPath = pPubKernel->GetResourcePath();
	xmlPath += GUILD_JIAN_KANG_DU_CONFIG;
	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[MasterAprtcModule::LoadResource] failed!");
		return false;
	}

	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a);
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a);
		const char* section = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(section))
		{
			continue;
		}

		m_guildJianKangDuConfig.m_dailyRecoverValue = xml.ReadInteger(section, "DailyRecoverValue", 0);
		m_guildJianKangDuConfig.m_needGuildCapital = xml.ReadInteger(section, "NeedGuildCapital", 0);
		m_guildJianKangDuConfig.m_guildActivityFialed = xml.ReadInteger(section, "GuildActivityFailed", 0);
		m_guildJianKangDuConfig.m_maintainFailed = xml.ReadInteger(section, "MaintainFailed", 0);
		m_guildJianKangDuConfig.m_guildRecoverFailed = xml.ReadInteger(section, "GuildRecoverFailed", 0);
		m_guildJianKangDuConfig.m_extraAwardNeedGainCapital = xml.ReadInteger(section, "DailyExtraNeedGainCapital", 0);
		m_guildJianKangDuConfig.m_extraAward = xml.ReadString(section, "ExtraAward", "");

		for (int i = 1; i <= 6; i++)
		{
			std::string keyNamne ="Gift" + StringUtil::IntAsString(i);
			std::string gif = xml.ReadString(section, keyNamne.c_str(),"");

			CVarList ret;
			StringUtil::SplitString(ret, gif.c_str(), ";");
			CVarList ret2;
			StringUtil::SplitString(ret2, ret.StringVal(0), ",");
			GuildJianKangDuGif info;
			info.m_lower_limit = ret2.IntVal(0);
			info.m_upper_limit = ret2.IntVal(1);
			info.m_award = ret.StringVal(1);
			m_guildJianKangDuConfig.m_gif.push_back(info);

		}


	}
	return true;
}

void GuildNumManage::AddPersoinalGainCapital(IPubData*guildData, const wchar_t*playerName, int value)
{
	if (guildData == NULL){
		return;
	}
	if (StringUtil::CharIsNull(playerName)){
		return;
	}

	IRecord* pGainCapital = guildData->GetRecord(GUILD_GAIN_CAPITAL_REC);
	if (pGainCapital == NULL){ return; }

	int row = pGainCapital->FindWideStr(GUILD_GAIN_CAPITAL_REC_COL_NAME, playerName);
	if (row >= 0)
	{
		int rowValue = pGainCapital->QueryInt(row, GUILD_GAIN_CAPITAL_REC_COL_VALUE);
		rowValue += value;
		pGainCapital->SetInt(row, GUILD_GAIN_CAPITAL_REC_COL_VALUE, rowValue);
	}
	else
	{
		pGainCapital->AddRowValue(-1,CVarList()<<playerName<<value<<0 );
	}

		
}

bool GuildNumManage::AddGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int addValue, GUILD_NUM_CHANGE desc, const wchar_t* playerName /*= NULL*/)
{

	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}

	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}

	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}

	int curValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);
	int maxValue = GetGuildNumUpperLimit(guildNumType,guildName);
	// 超出值后不写入值，其它行为日志保留 [12/1/2017 lihailuo]
	//if (curValue >= maxValue){
	//	return false;
	//}

	int retValue = curValue + addValue;
	if (retValue>maxValue){
		retValue = maxValue;
	}
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE, retValue);
	

	if (guildNumType == GUILD_NUM_TYPE::GUILD_NUM_CAPITAL)
	{
		int dailyGetguildCapital = guildData->QueryAttrInt(DAILY_GET_GUILD_CAPITAL);
		dailyGetguildCapital += addValue;
		guildData->SetAttrInt(DAILY_GET_GUILD_CAPITAL, dailyGetguildCapital);
		AddPersoinalGainCapital(guildData,playerName, addValue);
	
	}

	switch (desc)
	{
		case GUILD_NUM_CHANGE_GM:
		case GUILD_NUM_CHANGE_TASK:   //任务
		case GUILD_NUM_CHANGE_KIDNAP:			    //绑票
		{
		}break;
		default:
		{
			std::wstring wstrlogArgs;
			wstrlogArgs = StringUtil::IntAsWideStr(desc) + L",";
			if (!StringUtil::CharIsNull(playerName))
			{
				wstrlogArgs += std::wstring(playerName);
			}
			wstrlogArgs += L"," + StringUtil::IntAsWideStr(guildNumType) + L"," + StringUtil::IntAsWideStr(addValue);
			GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_ADD_GUILD_NUM, wstrlogArgs.c_str());
		}break;
	}

	return true;
}

int GuildNumManage::GetGuildNumValue( const wchar_t* guildName, GUILD_NUM_TYPE guildNumType)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}
	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}
	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}
	return  pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);
}

int GuildNumManage::CanAddGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}

	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}
	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}

	int curValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);
	int maxValue = GetGuildNumUpperLimit(guildNumType,guildName);
	if (curValue >= maxValue){
		return false;
	}
	return true;
}

bool GuildNumManage::DecGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int decValue, GUILD_NUM_CHANGE desc, const wchar_t* playerName /*= NULL*/)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}

	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}

	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}

	int curValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);

	if (curValue < decValue)
	{
		return false;
	}

	int retValue = curValue - decValue;
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE, retValue);


	if (GUILD_NUM_CHANGE_GM != desc)
	{
		std::wstring wstrlogArgs = StringUtil::IntAsWideStr(desc) + L",";
		if (!StringUtil::CharIsNull(playerName))
		{
			wstrlogArgs += playerName;
		}

		wstrlogArgs += L"," + StringUtil::IntAsWideStr(guildNumType) + L"," + StringUtil::IntAsWideStr(decValue);
		GuildBaseManager::m_pInstance->RecordGuildLog(guildName, GUILD_LOG_TYPE_DEC_GUILD_NUM, wstrlogArgs.c_str());
	}
	return true;
}

bool GuildNumManage::AddGuildNumLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int lockValue)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}
	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}
	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}
	int curEnableValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);
	if (curEnableValue < lockValue){
		return false;
	}
	int retEnableValue = curEnableValue - lockValue;
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE, retEnableValue);
	int curLockValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_LOCK_VALUE);
	int retLockValue = curLockValue + lockValue;
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_LOCK_VALUE, retLockValue);
	return true;
}

int GuildNumManage::GetGuildNumLockValue( const wchar_t* guildName, GUILD_NUM_TYPE guildNumType)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}
	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}
	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}
	return   pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_LOCK_VALUE);
}

bool GuildNumManage::CanDecGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int value)
{
	return GetGuildNumValue(guildName,guildNumType) >= value;
}

bool GuildNumManage::DecGuildNumLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int decValue) 
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}

	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}

	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}

	int curLockValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_LOCK_VALUE);

	if (curLockValue < decValue)
	{
		return false;
	}
	int retLockValue = curLockValue - decValue;
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_LOCK_VALUE, retLockValue);
	return true;
}

bool GuildNumManage::GuildUnLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int unLockValue)
{
	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return false;
	}

	IRecord * pGuildNumRc = guildData->GetRecord(GUILD_NUM_REC);
	if (pGuildNumRc == NULL){
		return false;
	}
	int row = pGuildNumRc->FindInt(GUILD_NUM_REC_COL_TYPE, guildNumType);
	if (row < 0){
		return false;
	}
	int curLockValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_LOCK_VALUE);
	if (curLockValue < unLockValue){
		return false;
	}

	int enableValue = pGuildNumRc->QueryInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE);
	enableValue += unLockValue;
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_ENABLE_VALUE, enableValue);
	pGuildNumRc->SetInt(row, GUILD_NUM_REC_COL_LOCK_VALUE, curLockValue - unLockValue);
	return true;
}

int GuildNumManage::GetGuildNumUpperLimit(GUILD_NUM_TYPE type, const wchar_t* guildName)
{
	int upperLimit = GetGuildBuildingAditionUpperLimit(type,guildName);
	auto it = m_initGuildNumCofig.find(type);
	if (it != m_initGuildNumCofig.end()){
		upperLimit +=  it->second.m_upperLimit;
	}
	return upperLimit;
}

int GuildNumManage::GetGuildBuildingAditionUpperLimit(GUILD_NUM_TYPE type, const wchar_t*guildName)
{
	auto it = m_guildBuildingAddition.find(type);
	if (it != m_guildBuildingAddition.end())
	{
		auto buildType = static_cast <GUILD_BUILDING_TYPE>(it->second.m_GuidBuingType);
		size_t buildLevel = GuildBuildingManage::m_pInstance->GetBuildingLevel(guildName, buildType);
		if (buildLevel < it->second.m_addition.size())
		{
			return it->second.m_addition[buildLevel];
		}
		
	}
	return 0;
}

int GuildNumManage::GuildNumDailyReset(IPubKernel* pPubKernel)
{
	
	//清除组织数值购买记录
	auto ClearGuildNumBuyRec = [](IPubData *guildData){
		if (guildData == NULL){
			return;
		}
		IRecord * pGuildNumBuyRec = guildData->GetRecord(GUILD_NUM_BUY_REC);
		if (pGuildNumBuyRec != NULL){
			pGuildNumBuyRec->ClearRow();
		}
	};

	GuildJianKangDu jianKangDuConfig = m_guildJianKangDuConfig;
	
	//组织健康度
	auto DealJianKangDu = [jianKangDuConfig](IPubData *guildData, const wchar_t* guildName){
		if (guildData == NULL){
			return;
		}
		//每日健康度恢复
		int value = guildData->QueryAttrInt(DAILY_GET_GUILD_CAPITAL);
		if (value >= jianKangDuConfig.m_needGuildCapital)
		{
			GuildNumManage::m_pInstance->AddGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_JIANKANDU, jianKangDuConfig.m_dailyRecoverValue, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_DAILY_GET);
		}
		else
		{
			GuildNumManage::m_pInstance->DecJianKangDu(guildName,GUILD_NUM_CHANGE_DAILY_GET_GUILD_CAPITAL_NOT_ENOUGHT);
		}
		guildData->SetAttrInt(DAILY_GET_GUILD_CAPITAL, 0);
	};


	
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList != NULL)
	{
		int nRows = pGuildList->GetRows();
		LoopBeginCheck(a);
		for (int i = 0; i < nRows; ++i)
		{
			LoopDoCheck(a);

			
			std::wstring guildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
			const std::wstring guildDataName = guildName + m_ServerId;
			IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
			if (guildData != NULL){
				ClearGuildNumBuyRec(guildData);
				DealJianKangDu(guildData, guildName.c_str());
			}
		}
	}


	//每日恢复
	for (auto it : m_buyGuildNumConfig)
	{
		IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
		if (pGuildList != NULL)
		{
			int nRows = pGuildList->GetRows();
			LoopBeginCheck(a);
			for (int i = 0; i < nRows; ++i)
			{
				LoopDoCheck(a);
				std::wstring guildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
				AddGuildNumValue(guildName.c_str(),static_cast<GUILD_NUM_TYPE> (it.first), it.second.m_recoverValue, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_RECOVER);
			}
		}
	}




	{			
		IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
		if (pGuildList != NULL)
		{
			int nRows = pGuildList->GetRows();
			LoopBeginCheck(a);
			for (int i = 0; i < nRows; ++i)
			{
				LoopDoCheck(a);
				std::wstring guildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
				const std::wstring guildDataName = guildName + m_ServerId;
				IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
				if (guildData != NULL){
					IRecord* pGainCapital = guildData->GetRecord(GUILD_GAIN_CAPITAL_REC);
					if (pGainCapital == NULL){ continue; }

					for (int row = 0; row < pGainCapital->GetRows(); row++)
					{
						int value = pGainCapital->QueryInt(row, GUILD_GAIN_CAPITAL_REC_COL_VALUE);
						if (value >= jianKangDuConfig.m_extraAwardNeedGainCapital)
						{
							pGainCapital->SetInt(row, GUILD_GAIN_CAPITAL_REC_COL_EXTRA_AWARD_SATE,1);
						}
						else
						{
							pGainCapital->SetInt(row, GUILD_GAIN_CAPITAL_REC_COL_EXTRA_AWARD_SATE, 0);
						}
						pGainCapital->SetInt(row, GUILD_GAIN_CAPITAL_REC_COL_VALUE,0);
					}
				}
			}
		}
	
	}
	return 0;
}

void GuildNumManage::DecJianKangDu(const wchar_t*guildName, int ret)
{
	int decValue = 0;

	switch (ret)
	{
		case GUILD_NUM_CHANGE_DAILY_GET_GUILD_CAPITAL_NOT_ENOUGHT:
		{
			decValue = m_guildJianKangDuConfig.m_guildRecoverFailed;
		}break;
		case GUILD_NUM_CHANGE_MAINTAIN_FAILED_DEC:
		{
			decValue = m_guildJianKangDuConfig.m_maintainFailed;
		}break;
		case GUILD_NUM_CHANGE_NOT_OPEN_GUILD_ACTIVITY : {
			decValue = m_guildJianKangDuConfig.m_guildActivityFialed;
		}break;
		default:
			return;
	}
	DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_JIANKANDU, decValue, static_cast<GUILD_NUM_CHANGE>(ret));
}

void GuildNumManage::OnGetJianKangDuGif(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* guildName = args.WideStrVal(1);
	const wchar_t *playerName = args.WideStrVal(3);
	int value = GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_JIANKANDU);
	std::string award;
	for (auto it : m_guildJianKangDuConfig.m_gif)
	{
		if (it.m_lower_limit <= value && it.m_upper_limit>=value)
		{
			award = it.m_award;
			break;
		}
			
	}

	if (award.empty()){
		return;
	}

	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return;
	}

	IRecord * pGainCapital = guildData->GetRecord(GUILD_GAIN_CAPITAL_REC);
	if (pGainCapital == NULL){
		return;
	}
	
	 int row = pGainCapital->FindWideStr(GUILD_GAIN_CAPITAL_REC_COL_NAME, playerName);
	if (row >= 0)
	{
		int state = pGainCapital->QueryInt(row, GUILD_GAIN_CAPITAL_REC_COL_EXTRA_AWARD_SATE);
		if (state == 1)
		{
			award += "," + m_guildJianKangDuConfig.m_extraAward;
		}
	}

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_JIAN_KANG_DU_GIF <<
		playerName << award ;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}

void GuildNumManage::OnBuyGuildNum(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* guildName = args.WideStrVal(1);
	const wchar_t *playerName = args.WideStrVal(3);
	int guildNumType = args.IntVal(4);

	const std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return;
	}
	IRecord * pGuildNumBuyRec = guildData->GetRecord(GUILD_NUM_BUY_REC);
	if (pGuildNumBuyRec == NULL){
		return;
	}
	int buyTimes = 0;
	int row = pGuildNumBuyRec->FindInt(GUILD_NUM_BUY_REC_COL_TYPE, guildNumType);
	if (row < 0)
	{
		row = pGuildNumBuyRec->AddRowValue(-1, CVarList() << guildNumType << 0);
	}
	else
	{
		buyTimes = pGuildNumBuyRec->QueryInt(row, GUILD_NUM_BUY_REC_COL_TIMES);
	}

	auto it = m_buyGuildNumConfig.find(guildNumType);
	if (it == m_buyGuildNumConfig.end()){
		return;
	}

	int buyCost = 0;

	if (buyTimes >= it->second.m_buyMaxTimes)
	{
		SendGuildNumBuyResult(pPubKernel, sourceId, sceneId, playerName, guildNumType, GUILD_NUM_BUY_TIMES_NO_ENOUGH);
		return;
	}

	if (buyTimes > (int)it->second.m_buyCost.size())
	{
		buyCost = *(it->second.m_buyCost.rbegin());
	}
	else
	{
		buyCost = it->second.m_buyCost[buyTimes];
	}
	if (!CanDecGuildNumValue(guildName,GUILD_NUM_TYPE::GUILD_NUM_CAPITAL,buyCost))
	{
		SendGuildNumBuyResult(pPubKernel, sourceId, sceneId,  playerName, guildNumType, GUILD_NUM_BUY_GUILD_CAPITAL_NO_ENOUGH);
		return;
	}
	if (!CanAddGuildNumValue(guildName, static_cast<GUILD_NUM_TYPE>(guildNumType)))
	{
		SendGuildNumBuyResult(pPubKernel, sourceId, sceneId, playerName, guildNumType, GUILD_NUM_BUY_REACH_MAX);
		return;
	}
	pGuildNumBuyRec->SetInt(row, GUILD_NUM_BUY_REC_COL_TIMES, ++buyTimes);

	DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, buyCost, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_BUY_GUILD_NUM, playerName);
	AddGuildNumValue(guildName, static_cast<GUILD_NUM_TYPE>(guildNumType), it->second.m_buyValue, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_BUY_GUILD_NUM, playerName);
	int valueNow = GetGuildNumValue(guildName, static_cast<GUILD_NUM_TYPE>(guildNumType));
	SendGuildNumBuyResult(pPubKernel, sourceId, sceneId, playerName, guildNumType, GUILD_NUM_BUY_SUCCES, valueNow);

}
void GuildNumManage::SendGuildNumBuyResult(IPubKernel* pPubKernel, int sourceId, int sceneId, const wchar_t* playerName, int guildNumType, int result,int value)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_NUM_BUY <<
		playerName << guildNumType << result << value;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}

