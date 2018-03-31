//--------------------------------------------------------------------
// 文件名:		GuildManager.cpp
// 内  容:		公会系统管理类
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "GuildManager.h"
#include <time.h>
#include <algorithm>
#include "../../FsGame/SocialSystemModule/GuildLoadResource.h"
#include "../../FsGame/Define/PubDefine.h"
#include "../../utils/util_func.h"
#include "../../utils/string_util.h"
#include "../../utils/util.h"
#include "../../utils/record.h"
#include "../../FsGame/Define/GameDefine.h"
#include "utils/custom_func.h"
#include "server/LoopCheck.h"
#include "FsGame/Define/CommandDefine.h"
 
#include "../Domain/EnvirValue.h"
#include <list>
#include "GuildBaseManager.h"
#include "FsGame/Define/PubCmdDefine.h"
#include "GuildNumManage.h"
 

// 一天的小时数
int DAY_HOURS = 24;
// 一小时分钟数
int HOUR_MINUTES = 60;
// 一分钟秒数
int MINUTE_SECOND = 60;
// 十分钟秒数
int TEN_MINUTE_SECOND = 600;
// 一小时的秒数
int HOUR_SECONDS = 3600;

PubGuildManager* PubGuildManager::m_pInstance = NULL;

PubGuildManager::PubGuildManager()
:m_pPubSpace(NULL),m_pGuildPubData(NULL)
{
}

PubGuildManager::~PubGuildManager()
{
}

IPubLogic* PubGuildManager::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = NEW PubGuildManager;
	}

	return m_pInstance;
}

const char* PubGuildManager::GetName()
{
	return PUBSPACE_GUILD;
}

// 公共数据服务器启动
int PubGuildManager::OnPublicStart(IPubKernel* pPubKernel)
{
	m_ServerId = ::util_int64_as_widestr(pPubKernel->GetServerId());
	return 1;
}

// 公共数据服务器关闭
int PubGuildManager::OnServerClose(IPubKernel* pPubKernel)
{
	// 获取当前时间
	int year, month, day, hour, minute, second;
	util_get_year_month_day(year, month, day);
	util_get_hour_minute_second(hour, minute, second);

	// 记录关闭服务器时间
	m_pGuildPubData->SetAttrInt("CloseYear", year);
	m_pGuildPubData->SetAttrInt("CloseMonth", month);
	m_pGuildPubData->SetAttrInt("CloseDay", day);
	m_pGuildPubData->SetAttrInt("CloseHour", hour);
	m_pGuildPubData->SetAttrInt("CloseMinute", minute);
	m_pGuildPubData->SetAttrInt("CloseSecond", second);

	pPubKernel->SavePubSpace(GetName());

	return 1;
}

// 公共数据空间中的所有项加载完成
int PubGuildManager::OnPubSpaceLoaded(IPubKernel* pPubKernel)
{
	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(GetName());
	if (pPubSpace == NULL)
	{
		return 0;
	}
	m_pPubSpace = pPubSpace;

	std::wstring guildPubDataName = std::wstring(GUILD_PUB_DATA) + m_ServerId;
	// 检测是否创建了公会的公共数据项，如果没有则创建
	if (!pPubSpace->FindPubData(guildPubDataName.c_str()))
	{
		if (!pPubSpace->AddPubData(guildPubDataName.c_str(), true))
		{
			return 0;
		}
	}

	// 获取公会的公共数据项
	IPubData* pGuildPubData = pPubSpace->GetPubData(guildPubDataName.c_str());
	if (pGuildPubData == NULL)
	{
		return 0;
	}
	m_pGuildPubData = pGuildPubData;

	AddGuildSub(new GuildBaseManager());
	//AddGuildSub(new GuildBuildingManage());
	AddGuildSub(new GuildNumManage());
	//AddGuildSub(new GuildSkillManage());
	//AddGuildSub(new PrepareGuildManage());
	//AddGuildSub(new GuildRareTreasure());
	//AddGuildSub(new GuildBonus());
	//AddGuildSub(new GuildOccupyFlag());
	//AddGuildSub(new GuildWeeklyScore());
	CallBackAllSub([pPubKernel](GuildBase *pBase)->bool{
		if (pBase != NULL)
		{
			if (!pBase->Init(pPubKernel)){
				return false;
			}
		}
		return true;
	});


	CallBackAllGuild(
		[](IPubData* pGuildData)->bool{
		return m_pInstance->CallBackAllSub([pGuildData](GuildBase *pBase)->bool{
			if (pBase != NULL)
			{
				if (!pBase->CreateGuildInit(pGuildData)){
					return false;
				}
			}
			return true;
		});
	});

	CallBackAllGuild(
		[](IPubData* pGuildData)->bool{
		return m_pInstance->CallBackAllSub([pGuildData](GuildBase *pBase)->bool{
			if (pBase != NULL)
			{
				if (!pBase->Patch(pGuildData)){
					return false;
				}
			}
			return true;
		});
	});
	
	pPubKernel->SavePubSpace(GetName());


	// 读取配置
	
	InitTimer(pPubKernel);
	UpdateGuildTimer(pPubKernel);

	port_date_time_t now;
	Port_GetLocalTime(&now);
	ResetGuildTimer(pPubKernel, now);

	return 1;
}

// 公共数据项加载完成
int PubGuildManager::OnPubDataLoaded(IPubKernel* pPubKernel, const wchar_t* data_name)
{
	return 1;
}


// 帮会定时回调刷新
void PubGuildManager::UpdateGuildTimer(IPubKernel* pPubKernel)    
{
	// 获取当前时间
	int year, month, day, hour, minute, second;   
	util_get_year_month_day(year, month, day);
	util_get_hour_minute_second(hour, minute, second);

	// 获取公会的公共数据项
	int closeYear = m_pGuildPubData->QueryAttrInt("CloseYear");
	
	

	// 没有关闭服务器时间
	if (closeYear <= 0)
	{
		// 获取当前时间
		int year, month, day, hour, minute, second;
		util_get_year_month_day(year, month, day);
		util_get_hour_minute_second(hour, minute, second);
		// 记录关闭服务器时间
		m_pGuildPubData->SetAttrInt("CloseYear", year);
		m_pGuildPubData->SetAttrInt("CloseMonth", month);
		m_pGuildPubData->SetAttrInt("CloseDay", day);
		m_pGuildPubData->SetAttrInt("CloseHour", hour);
		m_pGuildPubData->SetAttrInt("CloseMinute", minute);
		m_pGuildPubData->SetAttrInt("CloseSecond", second);
	}
	 closeYear = m_pGuildPubData->QueryAttrInt("CloseYear");
	int closeMonth = m_pGuildPubData->QueryAttrInt("CloseMonth");
	int closeDay = m_pGuildPubData->QueryAttrInt("CloseDay");
	int closeHour = m_pGuildPubData->QueryAttrInt("CloseHour");
	int closeMinute = m_pGuildPubData->QueryAttrInt("CloseMinute");

	double closeTime = 0.0f;
	util_encode_date(closeYear, closeMonth, closeDay, closeTime);
	double nowTime = 0.0f;
	util_encode_date(year, month, day, nowTime);
	double diffTime = util_get_time_diff(nowTime, closeTime);
	if (diffTime < 0.0f)
	{
		return;
	}

	// 包含范围类型定义
	enum RangeType
	{
		RT_ALL,
		RT_INCLUDE,
		RT_EXCEPT,
	};

	RangeType rangeType = RT_ALL;

	GuildTimerCallback_t minTime;
	GuildTimerCallback_t maxTime;

	LoopBeginCheck(a);
	do
	{
		LoopDoCheck(a);
		// 超过两天+
		if (diffTime > DBL_EPSILON + 1)
		{
			break;
		}

		// 隔天
		if (diffTime > DBL_EPSILON)
		{
			// 超过一天+
			if (hour > closeHour)
			{
				break;
			}

			// 超过一天+
			if (hour == closeHour && minute >= closeMinute)
			{
				break;
			}

			// 超过一天-
			if (hour < closeHour || hour == closeHour && minute < closeMinute)
			{
				rangeType			= RT_EXCEPT;
				minTime.m_Hour		= hour;
				minTime.m_Minute	= minute;
				maxTime.m_Hour		= closeHour;
				maxTime.m_Minute	= closeMinute;

				break;
			}

			break;
		}

		// 当天
		rangeType			= RT_INCLUDE;
		minTime.m_Hour		= closeHour;
		minTime.m_Minute	= closeMinute;
		maxTime.m_Hour		= hour;
		maxTime.m_Minute	= minute;

	} while (0);

	// 根据刷新类型，调用回调函数
	std::wstring guildPubDataName = (GUILD_PUB_DATA + m_ServerId).c_str();
	GuildTimerCallbackVector::iterator iter(m_GuildTimer.begin());

	switch (rangeType)
	{
	case RT_INCLUDE: // 时间区间内
		{
			// 循环保护
			LoopBeginCheck(at);

			for ( ; iter != m_GuildTimer.end(); ++iter)
			{
				LoopDoCheck(at);
				GuildTimerCallback_t& guildTime = *iter;
				if (minTime < guildTime && guildTime < maxTime)
				{
					guildTime.m_CallbackFunc(pPubKernel, GetName(), guildPubDataName.c_str());
				}
			}
		}
		break;
	case RT_EXCEPT: // 时间区间外
		{
			// 循环保护
			LoopBeginCheck(au);

			for ( ; iter != m_GuildTimer.end(); ++iter)
			{
				LoopDoCheck(au);
				GuildTimerCallback_t& guildTime = *iter;
				if (guildTime < minTime || maxTime < guildTime)
				{
					guildTime.m_CallbackFunc(pPubKernel, GetName(), guildPubDataName.c_str());
				}
			}
		}
		break;
	case RT_ALL: // 调用所有回调   
	default:
		{
			// 循环保护
			LoopBeginCheck(av);

			for ( ; iter != m_GuildTimer.end(); ++iter)
			{
				LoopDoCheck(av);
				GuildTimerCallback_t& guildTime = *iter;
				guildTime.m_CallbackFunc(pPubKernel, GetName(), guildPubDataName.c_str());
			}
		}
		break;   
	}
}

// 获取帮会数据
IPubData* PubGuildManager::GetGuildPubData(IPubKernel* pPubKernel, const wchar_t* guildname)
{
	// 获取公会的pubspace
	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(PUBSPACE_GUILD);
	if (pPubSpace == NULL)
	{
		return NULL;
	}

	// 获取帮会数据
	int nServerId = pPubKernel->GetServerId();
	std::wstring wsGuildDataName = std::wstring(guildname) + StringUtil::IntAsWideStr(nServerId);
	return pPubSpace->GetPubData(wsGuildDataName.c_str());
}

// 帮会定时器设置
void PubGuildManager::ResetGuildTimer(IPubKernel* pPubKernel, const port_date_time_t& now)
{
	if (!m_GuildTimer.empty())
	{
		std::wstring guildPubDataName = (std::wstring(GUILD_PUB_DATA) + m_ServerId).c_str();
		if (pPubKernel->FindTimer(GUILD_TIMER_NAME, GetName(), guildPubDataName.c_str()))
		{
			pPubKernel->RemoveTimer(GUILD_TIMER_NAME, GetName(), guildPubDataName.c_str());
		}

		GuildTimerCallback_t currentTimer(now.nHour, now.nMinute);
		GuildTimerCallback_t *guildTimer = NULL;
		int deltaSeconds = 0;
		GuildTimerCallbackVector::iterator iter(m_GuildTimer.begin());
		// 循环保护
		LoopBeginCheck(cy);
		for ( ; iter != m_GuildTimer.end(); ++iter)
		{
			LoopDoCheck(cy);
			if (currentTimer < *iter)
			{
				guildTimer = &(*iter);
				break;
			}
		}
		if (guildTimer == NULL)
		{
			guildTimer = &m_GuildTimer[0];
			deltaSeconds += DAY_HOURS * HOUR_SECONDS;
		}
		deltaSeconds += ((*guildTimer - currentTimer) - now.nSecond);
		int deltaTimer = 0;
		if (deltaSeconds > HOUR_SECONDS)
		{
			deltaTimer = HOUR_SECONDS;
		}
		else if (deltaSeconds > TEN_MINUTE_SECOND)
		{
			deltaTimer = TEN_MINUTE_SECOND;
		}
		else if (deltaSeconds > MINUTE_SECOND)
		{
			deltaTimer = MINUTE_SECOND;
		}
		else
		{
			deltaTimer = deltaSeconds + 1;
		}

		pPubKernel->AddTimer(GUILD_TIMER_NAME, PubGuildManager::GuildTimerCallback, 
			GetName(), guildPubDataName.c_str(), deltaTimer*1000, 1);
	}
}


bool PubGuildManager::CallBackAllSub(std::function<bool(GuildBase*)> func)
{
	for (auto it : m_guildSubManage)
	{
		if (!func(it)){
			return false;
		}
	}
	return true;
}

bool PubGuildManager::CallBackAllGuild(std::function<bool(IPubData*)> func)
{
	
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL){
		return false;
	}
	for (int row = 0; row < pGuildList->GetRows(); row++)
	{
		const wchar_t*guildName = pGuildList->QueryWideStr(row, GUILD_LIST_REC_COL_NAME);

		std::wstring guildDataName = guildName + m_ServerId;
		IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
		if (pGuildData != NULL)
		{
			if (!func(pGuildData))
			{
				return false;
			}
		}
	}

	return true;

}

//商店重置
void PubGuildManager::ShopResetTimerCb(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name)
{

	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
	if (pPubSpace == NULL){
		return;
	}

	IPubData* pPubData = pPubSpace->GetPubData(data_name);
	if (pPubData == NULL){
		return;
	}

	IRecord* pGuildList = pPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL){
		return;
	}
	IRecord* shopBuyRec = pPubData->GetRecord(GUILD_SHOP_BUY_REC);
	if (shopBuyRec != NULL)
	{
		shopBuyRec->ClearRow();
	}
	IRecord* memberBuyRec = pPubData->GetRecord(GUILD_MEMBER_BUY_REC);
	if (memberBuyRec != NULL)
	{
		memberBuyRec->ClearRow();
	}
	
}




#define GUILD_DNLEVEL_TIMER 3600*1000
#define GUILD_DNLEVEL_TERM  3600*24*7


// 处理场景服务器的消息
int PubGuildManager::OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
									 int scene_id, const IVarList& msg)
{
	if (m_pPubSpace == NULL || m_pGuildPubData == NULL)
	{
		return 0;
	}

	const wchar_t* guildName = msg.WideStrVal(1);
	int msgId = msg.IntVal(2);

	
	CallBackAllSub([pPubKernel, source_id, scene_id, &msg](GuildBase*pBase)->bool{
		if (pBase != NULL)
		{
			 pBase->OnPublicMessage(pPubKernel, source_id, scene_id, msg);
		}
		return true;
	});


	switch (msgId)
	{
		case SP_GUILD_MSG_BUY_ITEM:// 请求购买公会商店物品，扣除个人贡献值
		{
			OnBuyShopItem(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		default:
			OnFilterMessage(pPubKernel, guildName, msgId, source_id, scene_id, msg);
			break;
	}

	return 0;
}
bool PubGuildManager::OnPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args)
{
	return true;
}
///////////////////////////////////////////////////////////////////////////
// 处理场景服务器消息函数
///////////////////////////////////////////////////////////////////////////
bool PubGuildManager::OnFilterMessage(IPubKernel* pPubKernel, const wchar_t* guildname, int msgid, int source_id, int scene_id, const IVarList& args)
{
	return false;
}



void PubGuildManager::AddGuildSub(GuildBase* pBase)
{
	if (pBase == NULL){
		return;
	}

	m_guildSubManage.push_back(pBase);
}

bool PubGuildManager::InitTimer(IPubKernel* pPubKernel)
{
	const char *path = pPubKernel->GetResourcePath();
	if (!LoadGuildTimer(path, m_GuildTimer))
	{
		return false;
	}
	// 将定时器升序排序
	std::sort(m_GuildTimer.begin(), m_GuildTimer.end());
	// 设置定时器回调函数
	GuildTimerCallbackVector::iterator iter(m_GuildTimer.begin());
	// 循环保护
	LoopBeginCheck(aq);
	for (; iter != m_GuildTimer.end(); ++iter)
	{
		LoopDoCheck(aq);
		if (iter->m_CallbackName == GUILD_PUB_RESET)
		{
			iter->m_CallbackFunc = GuildPubReset;
		}
		else if (iter->m_CallbackName == GUILD_BUILDING_MAINTAIN)
		{
		//	iter->m_CallbackFunc = GuildBuildingManage::TimingMaintain;

		}

	}

	return true;
}

// 请求购买公会商品，扣除个人贡献度
int PubGuildManager::OnBuyShopItem(IPubKernel* pPubKernel, const wchar_t* guildName, 
								   int sourceId, int sceneId, const IVarList& args)
{
	const char* playerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	int index = args.IntVal(5);
	const char* itemId = args.StringVal(6);
	int buyNum = args.IntVal(7);
	int addGuildCapital= args.IntVal(8);
	int shopBuyTotal = args.IntVal(9);
	int memberBuyTotal = args.IntVal(10);
	int consume = 0;
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}
	int memberRow = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
	if (memberRow < 0)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}
	int curDonate = GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL);
	if (curDonate < consume)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}

	// 检查剩余道具数量是否足够
	IRecord *shopBuyRecord = m_pGuildPubData->GetRecord(GUILD_SHOP_BUY_REC);
	if (shopBuyRecord == NULL)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}
	int shopBuyCount = 0;
	std::wstring shopBuyId = guildDataName + StringUtil::StringAsWideStr(itemId) + L"_" + StringUtil::IntAsWideStr(index);
	int shopBuyRow = shopBuyRecord->FindWideStr(GUILD_SHOP_BUY_REC_COL_ITEMID, shopBuyId.c_str());
	if (shopBuyRow >= 0)
	{
		shopBuyCount = shopBuyRecord->QueryInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT);
	}
	if ((shopBuyTotal - shopBuyCount) < buyNum)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}

	// 检查成员购买额度是否足够
	IRecord *memberBuyRecord = m_pGuildPubData->GetRecord(GUILD_MEMBER_BUY_REC);
	if (memberBuyRecord == NULL)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}
	int memberBuyCount = 0;
	std::string memberBuyId = std::string(playerNameUid) + std::string(itemId) + "_" + StringUtil::IntAsString(index);
	int memberBuyRow = memberBuyRecord->FindString(GUILD_MEMBER_BUY_REC_COL_UID_ITEMID, memberBuyId.c_str());
	if (memberBuyRow >= 0)
	{
		memberBuyCount = memberBuyRecord->QueryInt(memberBuyRow, GUILD_MEMBER_BUY_REC_COL_BUY_COUNT);
	}
	if ((memberBuyTotal - memberBuyCount) < buyNum)
	{
		SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
		return 0;
	}

	// 设置购买结果
	if (shopBuyRow >= 0)
	{
		if (!shopBuyRecord->SetInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT, shopBuyCount+buyNum))
		{
			SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
			return 0;
		}
	}
	else 
	{
		if (shopBuyRecord->AddRowValue(-1, CVarList() << shopBuyId << buyNum) < 0)
		{
			SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
			return 0;
		}
	}

	if (memberBuyRow >= 0)
	{
		if (!memberBuyRecord->SetInt(memberBuyRow, GUILD_MEMBER_BUY_REC_COL_BUY_COUNT, memberBuyCount+buyNum))
		{
			shopBuyRecord->SetInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT, shopBuyCount);
			SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
			return 0;
		}
	}
	else 
	{
		if (memberBuyRecord->AddRowValue(-1, CVarList() << memberBuyId << buyNum) < 0)
		{
			shopBuyRecord->SetInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT, shopBuyCount);
			SendBuyItemResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, index, itemId, buyNum, consume);
			return 0;
		}
	}

	if (addGuildCapital != 0)
	{
		GuildNumManage::m_pInstance->AddGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, addGuildCapital, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_SHOP);
	}

	// 发送结果
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_BUY_ITEM << GUILD_MSG_REQ_SUCC 
		<< playerName << index << itemId << buyNum << consume << (shopBuyTotal-shopBuyCount-buyNum) 
		<< (memberBuyTotal-memberBuyCount-buyNum) << (curDonate-consume);
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);

	return 0;
}
// 通知购买商品扣除个人贡献值结果
void PubGuildManager::SendBuyItemResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
											const wchar_t* guildName, const wchar_t* playerName,
											int result, int index, const char* itemId, int itemNum, int consume)
{
// 	CVarList msg;
// 	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_BUY_ITEM << result
// 		<< playerName << index << itemId << itemNum << consume << 0 << 0;
// 	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}
//定时器触发回调
int PubGuildManager::GuildTimerCallback(IPubKernel* pPubKernel, const char* space_name,
	const wchar_t* data_name, int time)
{

	port_date_time_t now;
	Port_GetLocalTime(&now);
	GuildTimerCallback_t currentTimer(now.nHour, now.nMinute);
	//int deltaSeconds = 0;
	GuildTimerCallbackVector::const_iterator iter(PubGuildManager::m_pInstance->m_GuildTimer.begin());
	// 循环保护
	LoopBeginCheck(aw);
	for (; iter != PubGuildManager::m_pInstance->m_GuildTimer.end(); ++iter)
	{
		LoopDoCheck(aw);
		if (currentTimer == *iter && iter->m_CallbackFunc)
		{
			iter->m_CallbackFunc(pPubKernel, space_name, data_name);
		}
	}

	PubGuildManager::m_pInstance->ResetGuildTimer(pPubKernel, now);
	return 0;
}

void PubGuildManager::GuildPubReset(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name)
{
	PubGuildManager::ShopResetTimerCb(pPubKernel, space_name,data_name);
	GuildBaseManager::FireNumResetTimerCb(pPubKernel, space_name, data_name);
	GuildBaseManager::m_pInstance->OnZeroClock(pPubKernel);
	//GuildBaseManager::DancingGirlReset(pPubKernel, space_name, data_name);
	//GuildBaseManager::AutoImpeachTimer(pPubKernel, space_name, data_name);
	GuildNumManage::m_pInstance->GuildNumDailyReset(pPubKernel);
}





