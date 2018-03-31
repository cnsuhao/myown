#include "GuildBaseManager.h"
#include "FsGame\Define\GuildDefine.h"
#include "utils\record.h"
#include "GuildManager.h"
#include "utils\string_util.h"
#include "FsGame\SocialSystemModule\GuildLoadResource.h"
#include "FsGame\Define\GameDefine.h"
#include <time.h>
#include "utils\custom_func.h"
#include "GuildNumManage.h"
#include "utils\util_func.h"
#include <list>
#include "FsGame\Define\CommandDefine.h"
#include "..\Domain\EnvirValue.h"
#include "..\FsBindLogic.h"
#include "FsGame\Define\RankingDefine.h"
#include "..\Domain\DomainRankList.h"
#include "FsGame\Define\SnsDefine.h"
#include "FsGame\SocialSystemModule\GuildHelper.h"
//#include "FsGame\Helper\GuildHelper.h"

GuildBaseManager* GuildBaseManager::m_pInstance = NULL;
int GuildBaseManager::m_autoImpeachMaxTime = 0;
int GuildBaseManager::m_memOffLineTimes = 0;
int GuildBaseManager::m_stationGroup = 0;

bool GuildBaseManager::Init(IPubKernel* pPubKernel)
{
	if (!InitPubGuildData(pPubKernel))
	{
		return false;
	}

	// 排序表重置
	m_GuildSortHelperSet.clear();
	CheckAndRefreshSortRecord();


	IRecord* pGuildListRec = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildListRec == NULL)
	{
		return 0;
	}
	int rows = pGuildListRec->GetRows();
	int64_t offlineTime = ::time(NULL);
	// 循环保护
	LoopBeginCheck(ao);
	for (int i = 0; i < rows; i++)
	{
		LoopDoCheck(ao);
		// 清空在线状态
		const wchar_t *guildName = pGuildListRec->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
		IRecord* memberList = GetGuildDataRecOpt(guildName, GUILD_MEMBER_REC);
		if (memberList == NULL)
		{
			continue;
		}
		int memberCount = memberList->GetRows();
		// 循环保护
		LoopBeginCheck(ap);
		for (int idx = 0; idx < memberCount; idx++)
		{
			LoopDoCheck(ap);
			int online = memberList->QueryInt(idx, GUILD_MEMBER_REC_COL_ONLINE);
			if (online == ONLINE)
			{
				memberList->SetInt(idx, GUILD_MEMBER_REC_COL_ONLINE, OFFLINE);
				memberList->SetInt64(idx, GUILD_MEMBER_REC_COL_OUTLINE_DATE, offlineTime);
			}
		}
	}




	if (!LoadResource(pPubKernel))
	{
		return false;
	}

	return true;
}

bool GuildBaseManager::InitPubGuildData(IPubKernel* pPubKernel)
{
	bool ret = false;
	{
		// 帮会列表列定义
		CVarList cols;
		cols << VTYPE_WIDESTR
			<< VTYPE_STRING
			<< VTYPE_WIDESTR
			<< VTYPE_INT
			<< VTYPE_WIDESTR
			<< VTYPE_INT64
			<< VTYPE_INT
			<< VTYPE_INT64
			<< VTYPE_WIDESTR
			<< VTYPE_INT;

		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_LIST_REC, cols, CVarList(), GUILD_LIST_REC_ROW_MAX, GUILD_LIST_REC_COL_NAME);
		if (!ret){
			return false;
		}
	}
	
	{
		// 如果还没有申请者列表表格，则创建
		CVarList cols;
		cols << VTYPE_STRING
			<< VTYPE_WIDESTR
			<< VTYPE_WIDESTR;

		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_APPLY_REC, cols, CVarList(), GUILD_APPLY_LIST_ROW_MAX);
		if (!ret){
			return false;
		}
	}
	
	{
		// 如果还没有下线玩家加入公会表格，则创建
		CVarList cols;
		cols << VTYPE_STRING
			<< VTYPE_WIDESTR
			<< VTYPE_WIDESTR
			<< VTYPE_INT64;
		
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_OUTLINE_JOIN_REC, cols, CVarList(), GUILD_OUTLINE_JOIN_ROW_MAX);
		if (!ret){
			return false;
		}
	}
	
	
	{
		// 如果公会排序表不存在，则创建
		CVarList cols;
		cols << VTYPE_WIDESTR;
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_SORT_REC, cols, CVarList(), GUILD_SORT_REC_ROW_MAX, GUILD_SORT_REC_COL_NAME,false);
		if (!ret){
			return false;
		}
	}

	
	
	{
		// 公会商店道具购买计数表
		CVarList cols;
		cols << VTYPE_WIDESTR << VTYPE_INT;
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_SHOP_BUY_REC, cols, CVarList(), GUILD_SHOP_BUY_REC_ROW_MAX);
		if (!ret){
			return false;
		}
	}
	
	
	{
		// 公会成员购买道具统计表
		CVarList cols;
		cols << VTYPE_STRING << VTYPE_INT;
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_MEMBER_BUY_REC, cols, CVarList(), GUILD_MEMBER_BUY_REC_ROW_MAX);
		if (!ret){
			return false;
		}
	}

	{//组织标识
		CVarList cols;
		cols << VTYPE_WIDESTR << VTYPE_INT << VTYPE_WIDESTR;
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_SYSMBOL_REC, cols, CVarList(), GUILD_LIST_REC_ROW_MAX);
		if (!ret){
			return false;
		}
	}

	{//组织设置信息
		CVarList cols;
		cols << VTYPE_WIDESTR
			<< VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT
			<< VTYPE_INT;
		ret = _create_pub_record_safe(m_pGuildPubData, GUILD_SET_REC, cols, CVarList(), GUILD_LIST_REC_ROW_MAX);
		if (!ret){
			return false;
		}

	}

	// 初始化帮会场景分组资源表
	InitGuildStationGroupIDRec();

	// 关闭服务器时间
	if (!m_pGuildPubData->FindAttr("CloseYear"))
	{
		m_pGuildPubData->AddAttr("CloseYear", VTYPE_INT);
	}
	if (!m_pGuildPubData->FindAttr("CloseMonth"))
	{
		m_pGuildPubData->AddAttr("CloseMonth", VTYPE_INT);
	}
	if (!m_pGuildPubData->FindAttr("CloseDay"))
	{
		m_pGuildPubData->AddAttr("CloseDay", VTYPE_INT);
	}
	if (!m_pGuildPubData->FindAttr("CloseHour"))
	{
		m_pGuildPubData->AddAttr("CloseHour", VTYPE_INT);
	}
	if (!m_pGuildPubData->FindAttr("CloseMinute"))
	{
		m_pGuildPubData->AddAttr("CloseMinute", VTYPE_INT);
	}
	if (!m_pGuildPubData->FindAttr("CloseSecond"))
	{
		m_pGuildPubData->AddAttr("CloseSecond", VTYPE_INT);
	}
	pPubKernel->SavePubSpace(GetName());
	return true;
}

// 初始化帮会场景分组资源表
int GuildBaseManager::InitGuildStationGroupIDRec()
{
	//1、创建帮会场景分组资源表
	if (!m_pGuildPubData->FindRecord(GUILD_STATION_GROUP_REC))
	{
		CVarList cols;
		cols << VTYPE_WIDESTR
			<< VTYPE_INT;

		if (!m_pGuildPubData->AddRecord(GUILD_STATION_GROUP_REC,
			GUILD_LIST_REC_ROW_MAX, GUILD_STATION_GROUP_REC_COL_MAX, cols))
		{
			return false;
		}
		m_pGuildPubData->SetRecordSave(GUILD_STATION_GROUP_REC, false);
	}

	//2、初始化帮会场景分组资源数据
	InitGuildStationGroupIDData();

	return 0;
}

// 初始化帮会场景分组资源数据
int GuildBaseManager::InitGuildStationGroupIDData()
{
	//1、获取公会列表
	IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList == NULL)
	{
		return 0;
	}

	//2、获取公会场景分组号记录表
	IRecord* guildStationList = m_pGuildPubData->GetRecord(GUILD_STATION_GROUP_REC);
	if (guildStationList == NULL)
	{
		return 0;
	}
	
	//3、清空重置帮会分组数据
	guildStationList->ClearRow();

	//4、重新分配帮会分组ID
	int guildCount = guildList->GetRows();
	LoopBeginCheck(a);
	for (int i = 0; i < guildCount; ++i)
	{
		LoopDoCheck(a);
		const wchar_t* guildName = guildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
		if (StringUtil::CharIsNull(guildName))
		{
			continue;
		}
		//这里有问题在组织解散的时候
		CVarList var;
		var << guildName << (10 + ++m_stationGroup);  // 帮会名 << 分组号(从10开始)
		guildStationList->AddRowValue(-1, var);
	}

	return 0;
}

bool GuildBaseManager::LoadResource(IPubKernel* pPubKernel)
{
	if (!LoadGuildUpLevelConfig(pPubKernel->GetResourcePath(), m_GuildUpLevelConfig))
	{
		return false;
	}

// 	if (!LoadGuldImpeach(pPubKernel))
// 	{
// 		return false;
// 	}

	return true;
}


// 重新整理公会排序表
void GuildBaseManager::CheckAndRefreshSortRecord()
{
	// 帮会排序表
	IRecord* pGuildSortRecord = m_pGuildPubData->GetRecord(GUILD_SORT_REC);
	if (pGuildSortRecord == NULL)
	{
		return;
	}

	// 帮会列表
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return;
	}



	// 清空排序容器
	m_GuildSortHelperSet.clear();

	// 向容器中插入数据，来进行排序
	GuildSortHelper_t tempHelper;
	int memberCount = pGuildList->GetRows();
	// 循环保护
	LoopBeginCheck(ar);
	for (int i = 0; i < memberCount; ++i)
	{
		LoopDoCheck(ar);
		if (!DumpGuildSortHelper(pGuildList, i, &tempHelper))
		{
			const wchar_t* wstrGuildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
			extend_warning_pub(LOG_ERROR, "CheckAndRefreshSortRecord error DumpGuildSortHelper failed GuildName:%s",
				(wstrGuildName != NULL ? StringUtil::WideStrAsString(wstrGuildName).c_str() : ""));
			continue;
		}

		m_GuildSortHelperSet.insert(tempHelper);
	}

	// 清空排序表
	pGuildSortRecord->ClearRow();

	// 填充排序表数据
	// 循环保护
	LoopBeginCheck(as);
	CVarList varList;
	for (GuildSortHelperSetIter iter = m_GuildSortHelperSet.begin();
		iter != m_GuildSortHelperSet.end(); ++iter)
	{
		LoopDoCheck(as);
		varList.Clear();
		varList << iter->guildName;
		pGuildSortRecord->AddRowValue(-1, varList);
	}
}




void GuildBaseManager::FireNumResetTimerCb(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name)
{
	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
	if (pPubSpace == NULL)
	{
		return;
	}
	IPubData* guildPubData = PubGuildManager::m_pInstance->m_pGuildPubData;
	if (guildPubData == NULL)
	{
		return;
	}
	IRecord* pGuildList = guildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return;
	}

	// 将所有公会的踢人计数重置
	int nRows = pGuildList->GetRows();
	LoopBeginCheck(a);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(a);
		std::wstring wstrGuildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
		//公会频道
		std::wstring guildDataName = wstrGuildName + util_int_as_widestr(pPubKernel->GetServerId());
		IPubData *pPubData = pPubSpace->GetPubData(guildDataName.c_str());
		if (NULL == pPubData)
		{
			continue;;
		}
		// 将踢人次数重置
		if (pPubData->FindAttr("FireNum"))
		{
			pPubData->SetAttrInt("FireNum", 0);
		}
	}
}

// 舞姬重置
void GuildBaseManager::DancingGirlReset(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name)
{

	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
	if (pPubSpace == NULL)
	{
		return;
	}

	// 帮会列表
	IRecord* pGuildSetList = PubGuildManager::m_pInstance->m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetList == NULL)
	{
		return;
	}

	// 遍历所有帮会
	int nRows = pGuildSetList->GetRows();
	for (int i = 0; i < nRows; ++i)
	{
		std::wstring guildName = pGuildSetList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
		int girlSate = pGuildSetList->QueryInt(i, GUILD_SET_REC_COL_DANCING_GIRL);
		if (girlSate == 0)
		{
			GuildNumManage::m_pInstance->DecJianKangDu(guildName.c_str(), GUILD_NUM_CHANGE_NOT_OPEN_GUILD_ACTIVITY);
		}

		pGuildSetList->SetInt(i, GUILD_SET_REC_COL_DANCING_GIRL, 0);
	}


}





int GuildBaseManager::OnPublicMessage(IPubKernel* pPubKernel, int source_id, int scene_id, const IVarList& msg)
{

	if (m_pPubSpace == NULL || m_pGuildPubData == NULL)
	{
		return 0;
	}


	const wchar_t* guildName = msg.WideStrVal(1);
	int msgId = msg.IntVal(2);


	switch (msgId)
	{
		case SP_GUILD_MSG_CREATE:// 请求创建公会
		{
			OnCreateGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_APPLY_JOIN:// 请求加入公会
		{
			OnApplyJoinGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_START_ONE_KEY_APPLY_JOIN://一键申请加入
		{
			OnOneKeyApplyJoinGuild(pPubKernel, source_id, scene_id, msg);
		}break;
		case SP_GUILD_MSG_CANCEL_APPLY_JOIN:// 取消加入公会请求
		{
			OnCancelApplyJoinGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_ACCEPT_JOIN:// 接受加入公会申请的请求
		{
			OnAcceptJoinGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_REFUSE_JOIN:// 拒绝加入公会申请
		{
			OnRefuseJoinGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_MEMBER_ONLINE_STATE:  // 公会成员上下线
		{
			OnMemberOnlineStateChange(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_DELETE_JOIN_RECORD:   // 成员不在线时被加入公会
		{
			OnDeleteOutlineJoinRecord(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;

		case SP_GUILD_MSG_UPDATE_PROPS: //更新成员属性
		{
			OnUpdateProps(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_UPDATE_APPLY_PROPS: //更新申请人属性
		{
			OnUpdateApplyProps(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case  SP_GUILD_MSG_UPDATE_APPLY_ONLINE_STATE:
		{
			OnUpdateApplyOnline(pPubKernel, guildName, source_id, scene_id, msg);
		}break;
		case SP_GUILD_MSG_CHANGE_NOTICE:// 修改公告
		{
			OnChangeNotice(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_CHANGE_DECLARATION:// 修改宣言
		{
			OnChangeDeclaration(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_QUIT:// 退出公会
		{
			OnQuit(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_FIRE:// 踢出公会
		{
			OnFire(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_DISMISS:// 解散公会
		{
			OnDismiss(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_DONATE: //捐赠
		{
			OnDonate(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_PROMOTION:
		case SP_GUILD_MSG_DEMOTION:  //职位变动
		{
			OnPromotion(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		
		case SP_GUILD_MSG_CAPTAIN_TRANSFER: //移交帮主
		{
			OnCaptainTransfer(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		case SP_GUILD_MSG_CHAT_INFO:
		{
			OnSaveGuidChannelInfo(pPubKernel, guildName, source_id, scene_id, msg);
		}
		break;
		
		case SP_GUILD_RECORD_GUILD_LOG: // 记录帮会日志
		{
			int type = msg.IntVal(3);
			std::wstring wstrLogArgs = msg.WideStrVal(4);
			RecordGuildLog(guildName, type, wstrLogArgs.c_str());
		}
		break;
// 		case SP_GUILD_START_IMPEACH:    //开始弹劾
// 		{
// 			OnStartImpeach(pPubKernel, guildName, source_id, scene_id, msg);
// 		}
// 		break;
// 		case SP_GUILD_START_IMPEACH_UNDO:    //撤销弹劾
// 		{
// 			OnUndoImpeach(pPubKernel, guildName, msg.StringVal(3));
// 		}
// 		break;
// 		case SP_GUILD_START_IMPEACH_RESPOND:    //响应弹劾
// 		{
// 			OnRespondImpeach(pPubKernel, guildName, source_id, scene_id, msg);
// 		}
// 		break;
		case SP_GUILD_SET_AUTO_AGREE_STATE: //设置帮会自动加入
		{
			int autoAgree = msg.IntVal(3);
			int minLevel = msg.IntVal(4);
			int maxLevel = msg.IntVal(5);
			int ability = msg.IntVal(6);
			IRecord * pGuildSetRc = m_pGuildPubData->GetRecord(GUILD_SET_REC);
			if (pGuildSetRc == NULL){
				return false;
			}
			int guildRow = pGuildSetRc->FindWideStr(GUILD_SET_REC_COL_GUILD_NAME, guildName);
			if (guildRow >= 0)
			{
				pGuildSetRc->SetInt(guildRow, GUILD_SET_REC_COL_AUTO_AGREE, autoAgree ? 1 : 0);
				pGuildSetRc->SetInt(guildRow, GUILD_SET_REC_COL_AUTO_AGREE_MIN_LEVEL, minLevel);
				pGuildSetRc->SetInt(guildRow, GUILD_SET_REC_COL_AUTO_AGREE_MAX_LEVEL, maxLevel);
				pGuildSetRc->SetInt(guildRow, GUILD_SET_REC_COL_AUTO_AGREE_ABILITY, ability);
			}

		}break;
// 		case SP_GUILD_SET_DINE_DANCING_STATE://设置帮会舞姬状态
// 		{
// 			IRecord* pGuildSetList = m_pGuildPubData->GetRecord(GUILD_SET_REC);
// 			if (pGuildSetList == NULL)
// 			{
// 				return 0;
// 			}
// 			int guildRow = pGuildSetList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
// 			if (guildRow < 0)
// 			{
// 				return 0;
// 			}
// 
// 			pGuildSetList->SetInt(guildRow, GUILD_SET_REC_COL_DANCING_GIRL, 1);
// 
// 		}break;
		

		case SP_GUILD_TIGGER_TIMER:
		{
			PubGuildManager::m_pInstance->UpdateGuildTimer(pPubKernel);
		}break;
// 		case SP_GUILD_SET_DANCING_GIRL_AUTO_OPEN_TIME://设置组织舞姬定时开启时间
// 		{
// 			OnSetDancingGirlAutoOpenTime(pPubKernel, guildName, msg.IntVal(3));
// 
// 		}break;
// 		case SP_GUILD_CHANGE_SHORT_NAME://修改简称
// 		{
// 			OnSetGuildShortName(pPubKernel, guildName,source_id,scene_id,msg);
// 		}break;
// 
// 		case SP_GUILD_ADD_GUID_DEVOTE_MAX:	    //增加组织个人帮贡最大值
// 		{
// 			AddMemberGuildDevoteMax(guildName, msg.WideStrVal(3), msg.IntVal(4));
// 		}break;
		case SP_GUILD_AGREE_INVITE_JOIN_GUILD:
		{
			OnInviteAgreeJoinGuild(pPubKernel, guildName, source_id, scene_id, msg);
		}break;
// 		case SP_GUILD_ADD_PETITION:
// 		{
// 			OnPetition(pPubKernel, guildName, source_id, scene_id, msg);
// 		}break;
// 		case SP_GUILD_UPDATE_TEACH_NUM:
// 		{
// 			OnUpdateTeachNum(pPubKernel, guildName, source_id, scene_id, msg, 3);
// 		}break;
		default:
			break;
	}
	return true;
}

void GuildBaseManager::OnPetition(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{

	int petitionType = args.IntVal(3);
	const  wchar_t* playerName = args.WideStrVal(4);
	if (StringUtil::CharIsNull(playerName) || StringUtil::CharIsNull(guildName)){
		return;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return;
	}

	IRecord* pGuildPettionRec = guildData->GetRecord(GUILD_PETITION_REC);
	if (pGuildPettionRec == NULL){
		return;
	}
	
	for (int row = 0; row < pGuildPettionRec->GetRows(); ++row)
	{
		int typeRow = pGuildPettionRec->QueryInt(row, GUILD_PETITION_REC_COL_TYPE);
		if (typeRow != petitionType){
			continue;
		}

		const wchar_t* playerNameRow = pGuildPettionRec->QueryWideStr(row, GUILD_PETITION_REC_COL_NAME);
		if (wcscmp(playerName, playerNameRow) == 0){
			return;
		}
	}

	pGuildPettionRec->AddRowValue(-1, CVarList() << playerName << petitionType);
}

int GuildBaseManager::OnCreateGuild(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const char* pPlayerUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	int sex = args.IntVal(5);
	int level = args.IntVal(6);
	int career = args.IntVal(7);
	int ability = args.IntVal(8);
	int nation = args.IntVal(9);
	int guildIndentifying = args.IntVal(10);
	const wchar_t *guildShortName = args.WideStrVal(11);
	int vip = args.IntVal(12);



	if (!CreateGuild(pPubKernel, guildName, nation, guildIndentifying, guildShortName)){
		SendCreateGuildResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
	};


	// 添加公会私有数据区，如果原来存在返回错误
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL)
	{
		SendCreateGuildResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
		return 0;
	}
	IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList == NULL){
		return false;
	}
	int row = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0){
		return false;
	}

	guildList->SetString(row,GUILD_LIST_REC_COL_CAPTAIN_UID, pPlayerUid);
	guildList->SetWideStr(row,GUILD_LIST_REC_COL_CAPTAIN, playerName);
		// 公会成员表添加数据
	CVarList rowValue;
	int64_t curDate = ::time(NULL);
	rowValue << pPlayerUid << playerName << sex << GUILD_POSITION_CAPTAIN << level
		<< career << ability << 0 << curDate << ONLINE << 0<<vip << 0 << curDate << 0 << 0;
	if (guildData->AddRecordRowValue(GUILD_MEMBER_REC, -1, rowValue) < 0)
	{
		m_pPubSpace->RemovePubData(guildDataName.c_str());
		SendCreateGuildResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	// 删除之前的申请记录
	DelAllApplyRecord(playerName);

	SendCreateGuildResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_SUCC);
	
	// 添加新的帮会场景分组资源数据
	
	pPubKernel->SavePubSpace(GetName());

	RefreshGuildFightAbility(pPubKernel, guildName);
	return 0;
}


bool GuildBaseManager::CreateGuild(IPubKernel*pPubKernel, const wchar_t*guildName, int nation, int guildIndentifying, const wchar_t*guildShortName)
{
	IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList == NULL){
		return false;
	}
	IRecord *pGuildSymbol = m_pGuildPubData->GetRecord(GUILD_SYSMBOL_REC);
	if (pGuildSymbol == NULL){
		return false;
	}

	int row = pGuildSymbol->FindWideStr(GUILD_SYSMBOL_REC_COL_SHORT_NAME, guildShortName);
	if (row >= 0){
		return false;
	}



	 row = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row >= 0){
		return false;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData != NULL){
		return false;
	}


	if (!m_pPubSpace->AddPubData(guildDataName.c_str(), true)){
		return false;
	}

	guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL)
	{
		return false;
	}
	

	if (!AddGuildTable( guildData )){
		return false;
	}

	bool ret = PubGuildManager::m_pInstance->CallBackAllSub([guildData](GuildBase*pBase)->bool{
		if (pBase != NULL)
		{
			return pBase->CreateGuildInit(guildData);
		}
		return false;
	});

	if (!ret)
	{
		m_pPubSpace->RemovePubData(guildDataName.c_str());
		return false;
	}
	// 把公会写入公会列表中
	if (!GuildBaseManager::m_pInstance->AddGuildToGuildList(guildName, "", L"", 0, nation))
	{
		m_pPubSpace->RemovePubData(guildDataName.c_str());
		return false;
	}


	IRecord *pGuildSetRec = m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetRec != NULL)
	{
		int row = pGuildSetRec->FindWideStr(GUILD_SET_REC_COL_GUILD_NAME, guildName);
		if (row >= 0)
		{
			pGuildSetRec->RemoveRow(row);
		}
		pGuildSetRec->AddRowValue(-1, CVarList() << guildName << 0 << 0 << 0 << 0 << 0 << 75600 << 0);
	}
	//添加组织标识
	pGuildSymbol->AddRowValue(-1, CVarList() << guildName << guildIndentifying << guildShortName);

	GuildSortHelper_t guildSortHelperNew;
	if (DumpGuildSortHelper(guildList, row, &guildSortHelperNew))
	{
		// 因为是新建公会，必然排倒数第一
		SortGuild(NULL, &guildSortHelperNew);
	}

	AddGuildStationGroupIDData(guildName);
	return  true;
		
}

bool GuildBaseManager::Patch(IPubData* guildData)
{
	// 修正公会日志数据
	PatchRecord( guildData );
	return true;
}

void GuildBaseManager::PatchRecord(IPubData* guildData)
{
	if (NULL == guildData)
	{
		return;
	}

	// 日志表
	IRecord* pLogRecord = guildData->GetRecord(GUILD_LOG_REC);
	if (pLogRecord == NULL)
	{
		return;
	}

	// 查看表是否满
	int nRows_ = pLogRecord->GetRows();
	int nMax_ = GUILD_LOG_REC_MAX_ROW;

	if (nRows_ > nMax_)
	{
		int nRowCount_ = nRows_ - nMax_;
		for (int i = 0; i < nRowCount_; ++i)
		{
			pLogRecord->RemoveRow(0);
		}
	}

	CVarList cols;
	int nCols = pLogRecord->GetCols();
	for (int i = 0; i < nCols; ++i )
	{
		cols << pLogRecord->GetColType(i);
	}
	change_pub_record(guildData, GUILD_LOG_REC, cols, CVarList(), GUILD_LOG_REC_MAX_ROW);

	AddGuildTable(guildData);
}

// 处理申请加入公会的消息
int GuildBaseManager::OnApplyJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{
	const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	int playerSex = args.IntVal(5);
	int playerLevel = args.IntVal(6);
	int ability = args.IntVal(7);
	int career = args.IntVal(8);
	int vipLevel = args.IntVal(9);
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_APPLY_JOIN, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	// 加入公会的申请者表中
	if (!AddPlayerToApplyTable(pGuildData, pPlayerNameUid, playerName, playerSex, playerLevel, ability, career,vipLevel))
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_APPLY_JOIN, GUILD_MSG_REQ_FAIL);
		return 0;
	}
	//通知
	SendToGuildMemApplyNew(pPubKernel, pGuildData, sourceId, sceneId);


	// 记录玩家的申请记录
	if (!AddRecordToApplyListTable(pPlayerNameUid, playerName, guildName))
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_APPLY_JOIN, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	// 通知申请成功消息
	SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_APPLY_JOIN, GUILD_MSG_REQ_SUCC);

	AutoAgreeJoinGuild(pPubKernel, guildName, playerName, sourceId, sceneId);
	return 0;
}


int GuildBaseManager::OnOneKeyApplyJoinGuild(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args)
{
	const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	int playerSex = args.IntVal(5);
	int playerLevel = args.IntVal(6);
	int ability = args.IntVal(7);
	int career = args.IntVal(8);
	int vipLv = args.IntVal(9);
	int count = args.IntVal(10);
	int maxCount = args.GetCount();
	std::wstring guildList;

	LoopBeginCheck(a);
	for (int i = 0; i < count; i++)
	{
		LoopDoCheck(a);
		int index = 10 + i;
		if (index >= maxCount)
		{
			break;
		}
		const wchar_t *guildName = args.WideStrVal(index);
		if (StringUtil::CharIsNull(guildName))
		{
			continue;
		}
		std::wstring guildDataName = guildName + m_ServerId;
		IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
		if (pGuildData == NULL)
		{
			continue;
		}

		// 加入公会的申请者表中
		if (!AddPlayerToApplyTable(pGuildData, pPlayerNameUid, playerName, playerSex, playerLevel, ability, career,vipLv))
		{
			continue;
		}

		// 记录玩家的申请记录
		if (!AddRecordToApplyListTable(pPlayerNameUid, playerName, guildName))
		{
			continue;
		}

		if (AutoAgreeJoinGuild(pPubKernel, guildName, playerName, sourceId, sceneId))
		{
			break;
		}


		guildList = guildList + guildName + L";";
	}
	// 通知申请成功消息
	SendRequestResult(pPubKernel, sourceId, sceneId, guildList.c_str(), playerName, PS_GUILD_MSG_APPLY_JOIN, GUILD_MSG_REQ_SUCC);
	return 0;
}

// 处理取消申请加入公会的消息
int GuildBaseManager::OnCancelApplyJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);

	bool rc = DelGuildApplyRecord(guildName, playerName);
	SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName,
		PS_GUILD_MSG_CANCEL_APPLY, (rc ? GUILD_MSG_REQ_SUCC : GUILD_MSG_REQ_FAIL));

	return 0;
}

// 处理接受加入公会申请的消息
int GuildBaseManager::OnAcceptJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{
	const char* pApplyNameUid = args.StringVal(3);
	const wchar_t* applyName = args.WideStrVal(4);
	const wchar_t* playerName = args.WideStrVal(5);
	int curLevel = args.IntVal(6);
	int maxMember = args.IntVal(7);
	int applyOnline = args.IntVal(8);

	if (!CanJoinGuild(guildName, maxMember))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, OFFLINE, GUILD_MSG_REQ_FAIL);
		//CustomSysInfoByName(pPubKernel, playerName, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_15, CVarList());
		return 0;
	}

	int sex = 0;
	int level = 0;
	int fightAbility = 0;
	int career = 0;
	int vipLv = 0;
	if (!GetApplyInfo(guildName, applyName, sex, career, level, fightAbility,vipLv))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	// 删除申请者申请记录
	if (!DelAllApplyRecord(applyName))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperOld))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	// 把申请者加入公会中
	if (!AddApplyJoinGuild(guildName, pApplyNameUid, applyName, sex, career, level, fightAbility, applyOnline,vipLv))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			playerName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}
	else
	{
		// 加入成功，记录日志
		RecordGuildLog(guildName, GUILD_LOG_TYPE_JOIN, applyName);
	}
	// 如果申请者不在线，则加到表格保存，等玩家上线设置其加入公会
	if (applyOnline == OFFLINE)
	{
		AddOutlineJoinGuild(guildName, pApplyNameUid, applyName);
	}

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_ACCEPT_JOIN
		<< guildName << playerName << applyName << applyOnline << GUILD_MSG_REQ_SUCC;
	// 帮会新会员数据
	msg << GUILD_POSITION_MEMBER << sex << level << career << 0 << 0 << fightAbility << 0 << vipLv;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
	RefreshGuildFightAbility(pPubKernel, guildName);
	// 公会排序
	GuildSortHelper_t guildSortHelperNew;
	if (DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperNew))
	{
		SortGuild(&guildSortHelperOld, &guildSortHelperNew);
	}

	return 0;
}

int GuildBaseManager::OnInviteAgreeJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{
	int index = 2;
	const char* pApplyNameUid = args.StringVal(++index);
	const wchar_t* applyName = args.WideStrVal(++index);
	int sex = args.IntVal(++index);
	int level = args.IntVal(++index);
	int fightAbility = args.IntVal(++index);
	int career = args.IntVal(++index);
	int vipLv = args.IntVal(++index);
	int maxMember = args.IntVal(++index);
	int applyOnline = true;

	if (!CanJoinGuild(guildName, maxMember))
	{
		CustomSysInfoByName(pPubKernel, applyName, SYSTEM_INFO_ID_17601, CVarList());
		return 0;
	}

	// 删除申请者申请记录
	if (!DelAllApplyRecord(applyName))
	{
		return 0;
	}

	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return 0;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return 0;
	}

	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperOld))
	{
		return 0;
	}

	// 把申请者加入公会中
	if (!AddApplyJoinGuild(guildName, pApplyNameUid, applyName, sex, career, level, fightAbility, applyOnline, vipLv))
	{
		SendAcceptJoinGuildResult(pPubKernel, sourceId, sceneId, guildName,
			applyName, applyName, applyOnline, GUILD_MSG_REQ_FAIL);
		return 0;
	}
	else
	{
		// 加入成功，记录日志
		RecordGuildLog(guildName, GUILD_LOG_TYPE_JOIN, applyName);
	}

	// 如果申请者不在线，则加到表格保存，等玩家上线设置其加入公会
	if (applyOnline == OFFLINE)
	{
		AddOutlineJoinGuild(guildName, pApplyNameUid, applyName);
	}

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_ACCEPT_JOIN
		<< guildName << applyName << applyName << applyOnline << GUILD_MSG_REQ_SUCC;
	// 帮会新会员数据
	msg << GUILD_POSITION_MEMBER << sex << level << career << 0 << 0 << fightAbility << 0 << vipLv;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
	RefreshGuildFightAbility(pPubKernel, guildName);
	// 公会排序
	GuildSortHelper_t guildSortHelperNew;
	if (DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperNew))
	{
		SortGuild(&guildSortHelperOld, &guildSortHelperNew);
	}

	return 0;
}

// 处理拒绝加入公会申请的消息
int GuildBaseManager::OnRefuseJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* applyName = args.WideStrVal(3);
	const wchar_t* playerName = args.WideStrVal(4);

	DelGuildApplyRecord(guildName, applyName);
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_REFUSE_JOIN
		<< playerName << applyName << guildName << GUILD_MSG_REQ_SUCC;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);

	return 0;
}
// 处理公会成员离线状态变化的消息
int GuildBaseManager::OnMemberOnlineStateChange(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	int onlineState = args.IntVal(3);
	const wchar_t* playerName = args.WideStrVal(4);

	int64_t curDate = ::time(NULL);
	if (onlineState == ONLINE)
	{
		SetGuildPubData(guildName, GUILD_LIST_REC, GUILD_LIST_REC_COL_NAME,
			GUILD_LIST_REC_COL_MEMBER_LOGIN_TIME, CVarList() << curDate);
	}
	else
	{
		SetGuildData(guildName, playerName, GUILD_MEMBER_REC,
			GUILD_MEMBER_REC_COL_NAME, GUILD_MEMBER_REC_COL_OUTLINE_DATE, CVarList() << curDate);
	}

	SetGuildData(guildName, playerName, GUILD_MEMBER_REC,
		GUILD_MEMBER_REC_COL_NAME, GUILD_MEMBER_REC_COL_ONLINE, CVarList() << onlineState);

	return 0;
}

// 处理删除离线玩家加入公会记录表格的一行
int GuildBaseManager::OnDeleteOutlineJoinRecord(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* playerName = args.WideStrVal(3);
	IRecord* pOutlineJoin = m_pGuildPubData->GetRecord(GUILD_OUTLINE_JOIN_REC);
	if (pOutlineJoin == NULL)
	{
		return 0;
	}

	int row = pOutlineJoin->FindWideStr(GUILD_OUTLINE_JOIN_REC_COL_PLAYER_NAME, playerName);
	if (row >= 0)
	{
		pOutlineJoin->RemoveRow(row);
	}

	return 0;
}


// 处理修改公告
int GuildBaseManager::OnChangeNotice(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	const wchar_t* text = args.WideStrVal(5);

	int spend = EnvirValue::EnvirQueryInt(ENV_VALUE_CHANGE_ANNOUNCEMENT);
	if (!GuildNumManage::m_pInstance->CanDecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, spend))
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_CHANGE_NOTICE, GUILD_MSG_REQ_FAIL);
		return 0;
	}


	CVarList notice;
	notice << text;
	if (!SetGuildPubData(guildName, GUILD_LIST_REC,
		GUILD_LIST_REC_COL_NAME, GUILD_LIST_REC_COL_ANNOUNCEMENT, notice))
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_MSG_CHANGE_NOTICE, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	GuildNumManage::m_pInstance->DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, spend, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_GUILD_NOTIFY, playerName);

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_CHANGE_NOTICE
		<< guildName << playerName << GUILD_MSG_REQ_SUCC << text;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);

	return 0;
}



// 处理修改宣言
int GuildBaseManager::OnChangeDeclaration(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* playerName = args.WideStrVal(4);
	const wchar_t* text = args.WideStrVal(5);
	CVarList decal;
	decal << text;
	if (!SetGuildPubData(guildName, GUILD_LIST_REC,
		GUILD_LIST_REC_COL_NAME, GUILD_LIST_REC_COL_DECLARATION, decal))
	{
		SendRequestResult(pPubKernel, sourceId, sceneId, guildName, playerName, PS_GUILD_CHANGE_DECLARATION, GUILD_MSG_REQ_FAIL);
		return 0;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_CHANGE_DECLARATION
		<< guildName << playerName << GUILD_MSG_REQ_SUCC << text;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);

	return 0;
}
// 处理退出公会请求
int GuildBaseManager::OnQuit(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);

	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}
	const wchar_t *captainName = pGuildList->QueryWideStr(guildRow, GUILD_LIST_REC_COL_CAPTAIN);
	if (StringUtil::CharIsNull(captainName))
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}

	// 检查玩家是否在公会成员表格中
	int row = 0;
	IRecord* guildMemberRecord = GetGuildDataOpt(guildName, playerName, GUILD_MEMBER_REC, GUILD_MEMBER_REC_COL_NAME, row);
	if (guildMemberRecord == NULL)
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}
	int memberCount = guildMemberRecord->GetRows();

	// 公会排序
	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperOld))
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}

	// 如果是公会会长退出公会，当公会只有一个人时解散公会，否则会长移交
	if (wcscmp(playerName, captainName) == 0)
	{
		if (memberCount <= 1)
		{
			int rc = DismissGuild(pPubKernel, guildName, sourceId, sceneId, playerName);
			SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, rc, captainName);
			return 0;
		}
		else
		{
			// 找帮贡最高的会员作为新会长
			GuildMemberSortHelperSet sortHelper;
			// 循环保护
			LoopBeginCheck(ba);
			for (int i = 0; i < memberCount; i++)
			{
				LoopDoCheck(ba);
				int position = guildMemberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_POSITION);
				if (position == GUILD_POSITION_CAPTAIN)
				{
					continue;
				}
				const wchar_t *memberName = guildMemberRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
				int contrib = guildMemberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_DEVOTE);
				int rowlevel = guildMemberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_LEVEL);
				int ability = guildMemberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_FIGHT_ABILITY);
				sortHelper.insert(GuildMemberSortHelper_t(memberName, position, contrib, rowlevel, ability));
			}
			GuildMemberSortHelperSet::iterator iter(sortHelper.begin());
			const wchar_t *newCaptain = iter->memberName.c_str();
			if (TransferCaptain(guildName, playerName, newCaptain) == GUILD_MSG_REQ_FAIL)
			{
				SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
				return 0;
			}
		}
	}

	if (!guildMemberRecord->RemoveRow(row))
	{
		SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, L"");
		return 0;
	}
	else
	{
		// 离开公会成功，记录日志
		RecordGuildLog(guildName, GUILD_LOG_TYPE_LEAVE, playerName);

		// 帮会战数据响应玩家退帮会
		//GVGBattleGameSingleton::Instance()->OnPlayerExitGuild(pPubKernel, guildName, playerName);
	}

	// 移除请愿记录
	RemovePetition(pPubKernel, guildName, playerName);

	captainName = pGuildList->QueryWideStr(guildRow, GUILD_LIST_REC_COL_CAPTAIN);
	SendQuitResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_SUCC, captainName);
	RefreshGuildFightAbility(pPubKernel, guildName);
	// 公会排序
	GuildSortHelper_t guildSortHelperNew;
	if (DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperNew))
	{
		SortGuild(&guildSortHelperOld, &guildSortHelperNew);
	}
	//OnUndoImpeach(pPubKernel, guildName, pPubKernel->SeekRoleUid(playerName));
	return 0;
}
// 处理踢出公会请求
int GuildBaseManager::OnFire(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	const char* pMemberNameUid = args.StringVal(5);
	const wchar_t* memberName = args.WideStrVal(6);

	int onLine = OFFLINE;

	CVarList captainName;
	captainName << L"";
	if (!GetGuildPubData(guildName, GUILD_LIST_REC,
		GUILD_LIST_REC_COL_NAME, GUILD_LIST_REC_COL_CAPTAIN, captainName))
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}

	// 会长不能被踢出公会
	if (wcscmp(memberName, captainName.WideStrVal(0)) == 0)
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}

	// 检查玩家是否在公会成员表格中
	int row = 0;
	IRecord* pRecord = GetGuildDataOpt(guildName, memberName, GUILD_MEMBER_REC, GUILD_MEMBER_REC_COL_NAME, row);
	if (pRecord == NULL)
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}

	// 在删除成员前先获取成员的部分信息
	CVarList onLineState;
	onLineState << int(OFFLINE);
	if (GetGuildData(guildName, memberName, GUILD_MEMBER_REC,
		GUILD_MEMBER_REC_COL_NAME, GUILD_MEMBER_REC_COL_ONLINE, onLineState))
	{
		onLine = onLineState.IntVal(0);
	}

	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}

	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperOld))
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}

	// 删除成员
	if (!pRecord->RemoveRow(row))
	{
		SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, onLine, L"");
		return 0;
	}
	else
	{
		std::wstring wstrlogArgs = std::wstring(playerName) + L"," + std::wstring(memberName);
		// 踢人成功 记录日志
		RecordGuildLog(guildName, GUILD_LOG_TYPE_BE_LEAVE, wstrlogArgs.c_str());
		// 帮会战数据响应玩家退帮会
		//GVGBattleGameSingleton::Instance()->OnPlayerExitGuild(pPubKernel, guildName, playerName);
	}
	// 移除请愿记录
	RemovePetition(pPubKernel, guildName, memberName);


	SendFireResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_SUCC, onLine, memberName);
	// 更新公会的踢人次数
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 增加踢人次数属性
	if (!pGuildData->FindAttr("FireNum"))
	{
		pGuildData->AddAttr("FireNum", VTYPE_INT);
		pGuildData->SetAttrSave("FireNum", true);
	}

	// 更新踢人次数
	if (pGuildData->FindAttr("FireNum"))
	{
		int nFireNum = pGuildData->QueryAttrInt("FireNum");
		pGuildData->SetAttrInt("FireNum", nFireNum + 1);
	}
	RefreshGuildFightAbility(pPubKernel, guildName);
	CheckAndRefreshSortRecord();
	return 0;
}


// 处理解散公会请求
int GuildBaseManager::OnDismiss(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char* pPlayerNameUid = args.StringVal(3);
	const wchar_t* playerName = args.WideStrVal(4);

	CVarList captainName;
	captainName << L"";
	if (!GetGuildPubData(guildName, GUILD_LIST_REC,
		GUILD_LIST_REC_COL_NAME, GUILD_LIST_REC_COL_CAPTAIN, captainName))
	{
		return 0;
	}

	// 只有会长才可以解散公会
	if (wcscmp(playerName, captainName.WideStrVal(0)) != 0)
	{
		return 0;
	}

	DismissGuild(pPubKernel, guildName, sourceId, sceneId, playerName);

	return 0;
}
//捐赠
int GuildBaseManager::OnDonate(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)

{
	const wchar_t *playerName = args.WideStrVal(3);
	const char *playerUid = args.StringVal(4);
	int guildCaptial = args.IntVal(5);
	int donateValue = args.IntVal(6);
	int addSelfGuildCurrency = args.IntVal(7);
	// 更新玩家自己的帮贡
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}

	// 策划要求去除捐献最大值判断， 超出在AddGuildNumValue会自动取最大值 [12/1/2017 lihailuo]
	if (!GuildNumManage::m_pInstance->CanAddGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL))
	{

		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}




	IRecord* guildMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (guildMemberRecord == NULL)
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}
	int memberRow = guildMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
	if (memberRow < 0)
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}

	IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList == NULL)
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}
	int guildRow = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}

	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(guildList, guildRow, &guildSortHelperOld))
	{
		SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL, CVarList() << donateValue << addSelfGuildCurrency);
		return 0;
	}
	
	GuildNumManage::m_pInstance->AddGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, guildCaptial, GUILD_NUM_CHANGE_DONATE, playerName);

	
	
	int maxDevoteSilver =  guildMemberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_DEVOTE_SILVER);
	maxDevoteSilver += donateValue;
	guildMemberRecord->SetInt(memberRow, GUILD_MEMBER_REC_COL_DEVOTE_SILVER, maxDevoteSilver);

	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_NUM_CHANGE
		<< playerName << GUILD_NUM_TYPE::GUILD_NUM_CAPITAL << GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL);
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);

	SendGuildDonate(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_SUCC, CVarList() << donateValue << addSelfGuildCurrency);


	return 0;
}



// 处理公会任命
int GuildBaseManager::OnPromotion(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{
	int msgId = args.IntVal(2);
	const wchar_t* selfName = args.WideStrVal(3);
	const wchar_t* memberName = args.WideStrVal(4);
	int selfPosition = args.IntVal(5);
	int oldPosition = args.IntVal(6);
	int newPosition = args.IntVal(7);

	// 返回的结果
	CVarList rmsg;
	rmsg << PUBSPACE_GUILD << GUILD_LOGIC;
	switch (msgId)
	{
		case SP_GUILD_MSG_PROMOTION:
			rmsg << PS_GUILD_MSG_PROMOTION;
			break;
		case SP_GUILD_MSG_DEMOTION:
			rmsg << PS_GUILD_MSG_DEMOTION;
			break;
		default:
			return 0;
	}
	rmsg << guildName << selfName;

	// 获取公会表公共区数据
	std::wstring guildPubDataName = std::wstring(GUILD_PUB_DATA) + m_ServerId;
	IPubData* pubData = m_pPubSpace->GetPubData(guildPubDataName.c_str());
	if (NULL == pubData)
	{
		return 0;
	}

	// 验证是否在同一帮会
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
		pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);
		return 0;
	}
	IRecord* guildMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (guildMemberRecord == NULL)
	{
		rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
		pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);
		return 0;
	}
	int selfRow = guildMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, selfName);
	int memberRow = guildMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, memberName);
	if (selfRow < 0 || memberRow < 0)
	{
		rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
		pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);
		return 0;
	}

	// 排除两个服务器之间同步时间差异的问题
	int pubSelfPosition = guildMemberRecord->QueryInt(selfRow, GUILD_MEMBER_REC_COL_POSITION);
	int pubOldPosition = guildMemberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);
	if (selfPosition != pubSelfPosition || oldPosition != pubOldPosition)
	{
		rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
		pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);
		return 0;
	}

	// 公会是否存在
	IRecord *guildListRecord = pubData->GetRecord(GUILD_LIST_REC);
	if (guildListRecord == NULL)
	{
		return 0;
	}
	int guildRow = guildListRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return 0;
	}

	
	
	int xiangFangLevel = 1; //GuildBuildingManage::m_pInstance->GetBuildingLevel(guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);

	if (xiangFangLevel >=(int) m_GuildUpLevelConfig.size())
	{
		return 0;
	}

	// 帮会等级配置
	const GuildUpLevelConfig_t& upLevelConfig = m_GuildUpLevelConfig[xiangFangLevel];

	// 人数是否满
	switch (newPosition)
	{
		case GUILD_POSITION_ASSISTANT_CAPTAIN:
		{
			//m_GuildUpLevelConfig
			if (IsPositionFull(guildMemberRecord, newPosition, upLevelConfig.m_DeputyCaptainNum))
			{
				rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
				pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);

				// 提示审批失败
				CustomSysInfoByName(pPubKernel, selfName, SYSTEM_INFO_ID_17602, CVarList());
				return 0;
			}
		}
		break;
		case GUILD_POSITION_GENERAL:
		{
			if (IsPositionFull(guildMemberRecord, newPosition, upLevelConfig.m_GeneralNum))
			{
				rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
				pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);

				// 提示审批失败
				CustomSysInfoByName(pPubKernel, selfName, SYSTEM_INFO_ID_17602, CVarList());
				return 0;
			}
		}
		break;
		case GUILD_POSITION_ELDER:
		{
			if (IsPositionFull(guildMemberRecord, newPosition, upLevelConfig.m_Elder))
			{
				rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
				pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);

				// 提示审批失败
				CustomSysInfoByName(pPubKernel, selfName, SYSTEM_INFO_ID_17602, CVarList());
				return 0;
			}
		}
		break;
		case GUILD_POSITION_MEMBER:
			break;
		default:
			rmsg << GUILD_MSG_REQ_FAIL << memberName << 0;
			pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);
			return 0;
	}

	guildMemberRecord->SetInt(memberRow, GUILD_MEMBER_REC_COL_POSITION, newPosition);

	// 职位变动 记录日志
	std::wstring wstrlogArgs = std::wstring(selfName) + L"," + std::wstring(memberName) + L"," + std::wstring(GUILD_LEVEL) + StringUtil::IntAsWideStr(newPosition);
	if (msgId == SP_GUILD_MSG_PROMOTION)
	{
		RecordGuildLog(guildName, GUILD_LOG_TYPE_PROMOTION_NEW, wstrlogArgs.c_str());
	}
	else if (msgId == SP_GUILD_MSG_DEMOTION)
	{
		RecordGuildLog(guildName, GUILD_LOG_TYPE_DEMOTION_NEW, wstrlogArgs.c_str());
	}

	rmsg << GUILD_MSG_REQ_SUCC << memberName << newPosition;
	pPubKernel->SendPublicMessage(sourceId, sceneId, rmsg);

	return 0;
}

// 更新成员属性
int GuildBaseManager::OnUpdateProps(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* playerName = args.WideStrVal(3);
	int level = args.IntVal(4);
	int ability = args.IntVal(5);
	int vipLevel = args.IntVal(6);

	// 如果是会长，需要更新公会列表中会长等级
	CVarList captainName;
	captainName << L"";
	if (!GetGuildPubData(guildName, GUILD_LIST_REC,
		GUILD_LIST_REC_COL_NAME, GUILD_LIST_REC_COL_CAPTAIN, captainName))
	{
		return 0;
	}
	if (wcscmp(playerName, captainName.WideStrVal(0)) == 0)
	{
		SetGuildPubData(guildName, GUILD_LIST_REC, GUILD_LIST_REC_COL_NAME,
			GUILD_LIST_REC_COL_CAPTAIN_LEVEL, CVarList() << level);
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 更新公会成员列表
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList != NULL)
	{
		int row = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
		if (row >= 0)
		{

			int levelRow = pGuildMemberList->QueryInt(row, GUILD_MEMBER_REC_COL_LEVEL);
			if (levelRow < level)
			{
				//等级有变动，查看当前等级是不是10倍，记日志
				if (level % 10 == 0)
				{
					SendGuildLog(guildName, GUILD_LOG_MEMBER_EVENT,
						CVarList() << GUILD_LOG_MEMBER_EVENT_PLAYER_LV_UP
						<<playerName<<level);
				}
			}

			pGuildMemberList->SetInt(row, GUILD_MEMBER_REC_COL_LEVEL, level);
			pGuildMemberList->SetInt(row, GUILD_MEMBER_REC_COL_FIGHT_ABILITY, ability);
			pGuildMemberList->SetInt(row, GUILD_MEMBER_REC_COL_VIP_LEVEL, vipLevel);
		}
	}
	RefreshGuildFightAbility(pPubKernel, guildName);
	CheckAndRefreshSortRecord();
	return 0;
}

// 更新申请人属性
int GuildBaseManager::OnUpdateApplyProps(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* playerName = args.WideStrVal(3);
	const wchar_t* guildNames = args.WideStrVal(4);
	int level = args.IntVal(5);
	int ability = args.IntVal(6);
	int vipLv = args.IntVal(7);

	CVarList guildList;
	::util_split_wstring(guildList, guildNames, L";");

	int guildCount = (int)guildList.GetCount();
	// 循环保护
	LoopBeginCheck(bd);
	for (int i = 0; i < guildCount; ++i)
	{
		LoopDoCheck(bd);
		const wchar_t* changeGuildName = guildList.WideStrVal(i);
		std::wstring guildDataName = changeGuildName + m_ServerId;
		IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
		if (pGuildData == NULL)
		{
			return 0;
		}

		IRecord* pApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
		if (pApplyList != NULL)
		{
			int row = pApplyList->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, playerName);
			if (row >= 0)
			{
				pApplyList->SetInt(row, GUILD_JOIN_REC_COL_LEVEL, level);
				pApplyList->SetInt(row, GUILD_JOIN_REC_COL_FIGHT_ABILITY, ability);
				pApplyList->SetInt(row, GUILD_JOIN_REC_COL_VIP, vipLv);
			}
		}
	}

	return 0;
}

void GuildBaseManager::OnUpdateApplyOnline(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{
	const wchar_t* playerName = args.WideStrVal(3);
	const wchar_t* guildNames = args.WideStrVal(4);
	int nOnline_ = args.IntVal(5);

	CVarList guildList;
	::util_split_wstring(guildList, guildNames, L";");

	int guildCount = (int)guildList.GetCount();
	// 循环保护
	LoopBeginCheck(bd);
	for (int i = 0; i < guildCount; ++i)
	{
		LoopDoCheck(bd);
		const wchar_t* changeGuildName = guildList.WideStrVal(i);
		std::wstring guildDataName = changeGuildName + m_ServerId;
		IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
		if (pGuildData == NULL)
		{
			continue;
		}

		IRecord* pApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
		if (pApplyList != NULL)
		{
			int row = pApplyList->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, playerName);
			if (row >= 0)
			{
				pApplyList->SetInt(row, GUILD_JOIN_REC_COL_ONLINE, nOnline_);
			}
		}
	}
}

//解散工会
int GuildBaseManager::DismissGuild(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const wchar_t *playerName)
{
	// 删除公会列表中的记录项
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	// 首先dump出公会信息，用于删除排序表
	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(pGuildList, guildRow, &guildSortHelperOld))
	{
		extend_warning_pub(LOG_ERROR, "[%s][%d] DumpGuildSortHelper failed", __FILE__, __LINE__);
		return 0;
	}

	// 删除申请者记录及公会的申请者列表
	if (!DelGuildAllApplyRecord(guildName, true))
	{
		return GUILD_MSG_REQ_FAIL;
	}

	// 删除成员列表
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	int rowCount = pGuildMemberList->GetRows();
	if (rowCount <= 0)
	{
		return GUILD_MSG_REQ_FAIL;
	}

	// 循环保护
	LoopBeginCheck(be);
	for (int row = 0; row < rowCount; ++row)
	{
		LoopDoCheck(be);
		const char* pMemberNameUid = pGuildMemberList->QueryString(row, GUILD_MEMBER_REC_COL_NAME_UID);
		const wchar_t* memberName = pGuildMemberList->QueryWideStr(row, GUILD_MEMBER_REC_COL_NAME);
		int onLine = pGuildMemberList->QueryInt(row, GUILD_MEMBER_REC_COL_ONLINE);
		// 通知成员公会解散
		if (sourceId >= 0 && sceneId >= 0)
		{
			SendGuildGone(pPubKernel, sourceId, sceneId, guildName, playerName, onLine, memberName);
		}
	}

	pGuildMemberList->ClearRow();
	pGuildData->RemoveRecord(GUILD_MEMBER_REC);



	IRecord *pGuildSymbol = m_pGuildPubData->GetRecord(GUILD_SYSMBOL_REC);
	if (pGuildSymbol != NULL){
		int row = pGuildSymbol->FindWideStr(GUILD_SYSMBOL_REC_COL_GUILD_NAME, guildName);
		if (row >= 0)
		{
			pGuildSymbol->RemoveRow(row);
		}
	}

	//删除竞拍请愿
	IRecord * target_petition = pGuildData->GetRecord(C_REC_GUILD_PETITION_AUCTION);
	if (target_petition != NULL)
	{
		pGuildData->RemoveRecord(C_REC_GUILD_PETITION_AUCTION);
	}

	// 删除公会私有数据空间
	m_pPubSpace->RemovePubData(guildDataName.c_str());

	// 删除公会列表中的记录项
	pGuildList->RemoveRow(guildRow);

	// 删除排序相关数据
	SortGuild(&guildSortHelperOld, NULL);

	if (sourceId >= 0 && sceneId >= 0)
	{
		CVarList msg;
		msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_DISMISS << guildName;
		pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
	}

	IRecord* pGuildSetList = m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetList != NULL)
	{
		int row = pGuildSetList->FindWideStr(GUILD_SET_REC_COL_GUILD_NAME, guildName);
		if (row >= 0)
		{
			pGuildSetList->RemoveRow(row);
		}
	}

	// 更新公会战斗力排行榜
	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	// 清除公会战力和公会等级排行榜中的数据
	CVarList update_msg;
	update_msg																												 
		<< GUILD_FIGHT_POWER_RANK_LIST
		<< "guild_name" << guildName;

	DomainRankList::m_pDomainRankList->DeleteEntry(pPubKernel, pPubSpace, update_msg);

	update_msg.Clear();
	update_msg
		<< GUILD_LEVEL_RANK_LIST
		<< "guild_name" << guildName;

	DomainRankList::m_pDomainRankList->DeleteEntry(pPubKernel, pPubSpace, update_msg);


	//解散工会，有依赖处理,需要通知相关业务关联。
	FsBindLogic::Instance()->SendPublicCommand(pPubKernel, E_PUB_CMD_GUILD_DISMISS, CVarList() << guildName);

	pPubKernel->SendPublicMessage(pPubKernel->GetServerId(), 1,
		CVarList() << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_DISMISS_PUBLIC << guildName);
	return GUILD_MSG_REQ_SUCC;
}



//帮主转移
int GuildBaseManager::OnCaptainTransfer(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	//const char *playerUid = args.StringVal(3);
	const wchar_t *oldCaptain = args.WideStrVal(4);
	const wchar_t *newCaptain = args.WideStrVal(5);

	if (TransferCaptain(guildName, oldCaptain, newCaptain) <= 0)
	{
		SendCaptainResult(pPubKernel, sourceId, sceneId, oldCaptain, GUILD_MSG_REQ_FAIL, newCaptain, GUILD_POSITION_CAPTAIN, guildName);
	}
	else
	{
		SendCaptainResult(pPubKernel, sourceId, sceneId, oldCaptain, GUILD_MSG_REQ_SUCC, newCaptain, GUILD_POSITION_MEMBER, guildName);
		FsBindLogic::Instance()->SendPublicCommand(pPubKernel, E_PUB_CMD_GUILD_CAPTAIN_CHANGE, CVarList() << guildName << newCaptain);
	}

	return 0;
}



//移交帮主
int GuildBaseManager::TransferCaptain(const wchar_t* guildName, const wchar_t *oldCaptain, const wchar_t *newCaptain)
{
	// 检查帮会列表
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_CAPTAIN, oldCaptain);
	if (guildRow < 0)
	{
		return GUILD_MSG_REQ_FAIL;
	}

	// 检查帮会成员列表
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return GUILD_MSG_REQ_FAIL;
	}
	int oldCaptainRow = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, oldCaptain);
	int newCaptainRow = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, newCaptain);
	if (oldCaptainRow < 0 || newCaptainRow < 0)
	{
		return GUILD_MSG_REQ_FAIL;
	}

	// 首先更新帮会列表内容
	const char *newCaptainUid = pGuildMemberList->QueryString(newCaptainRow, GUILD_MEMBER_REC_COL_NAME_UID);
	int newCaptainLevel = pGuildMemberList->QueryInt(newCaptainRow, GUILD_MEMBER_REC_COL_LEVEL);
	pGuildList->SetString(guildRow, GUILD_LIST_REC_COL_CAPTAIN_UID, newCaptainUid);
	pGuildList->SetWideStr(guildRow, GUILD_LIST_REC_COL_CAPTAIN, newCaptain);
	pGuildList->SetInt(guildRow, GUILD_LIST_REC_COL_CAPTAIN_LEVEL, newCaptainLevel);

	// 设置帮会成员职务
	pGuildMemberList->SetInt(oldCaptainRow, GUILD_MEMBER_REC_COL_POSITION, GUILD_POSITION_MEMBER);
	pGuildMemberList->SetInt(newCaptainRow, GUILD_MEMBER_REC_COL_POSITION, GUILD_POSITION_CAPTAIN);

	// 职位变动记录日志
	std::wstring wstrlogArgs = std::wstring(L"@system") + L"," + std::wstring(oldCaptain) + L"," + std::wstring(GUILD_LEVEL) + StringUtil::IntAsWideStr(GUILD_POSITION_MEMBER);
	RecordGuildLog(guildName, GUILD_LOG_TYPE_DEMOTION_NEW, wstrlogArgs.c_str());
	wstrlogArgs.clear();
	wstrlogArgs = std::wstring(L"@system") + L"," + std::wstring(newCaptain) + L"," + std::wstring(GUILD_LEVEL) + StringUtil::IntAsWideStr(GUILD_POSITION_CAPTAIN);
	RecordGuildLog(guildName, GUILD_LOG_TYPE_PROMOTION_NEW, wstrlogArgs.c_str());

	return GUILD_MSG_REQ_SUCC;
}





// 保存聊天消息
int GuildBaseManager::OnSaveGuidChannelInfo(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return 0;
	}

	IRecord *pRec = pGuildData->GetRecord(GUILD_CHAT_HISTORY_REC);
	if (NULL == pRec)
	{
		return 0;
	}
	//超过最大值
	int nRows = pRec->GetRows();
	if (nRows >= GUILD_CHAT_HISTORY_ROW_MAX)
	{
		pRec->RemoveRow(0);
	}

	const wchar_t *pwName = args.WideStrVal(4);
	const wchar_t *pContent = args.WideStrVal(5);
	const char *resource = args.StringVal(6);
	const int nNationpost = args.IntVal(7);
	const int nVipLevel = args.IntVal(8);
	const int post = args.IntVal(9);
	const int64_t time = args.Int64Val(10);
	const int level = args.IntVal(11);

	CVarList var;
	var << pwName << pContent << guildName << resource << nNationpost << nVipLevel << post << time << level;
	pRec->AddRowValue(-1, var);
	return 1;
}
void GuildBaseManager::OnSetDancingGirlAutoOpenTime(IPubKernel* pPubKernel, const wchar_t*guildName, int time)
{
	// 帮会列表
	IRecord* pGuildSetList = m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetList == NULL)
	{
		return;
	}

	int row = pGuildSetList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0){
		return;
	}

	pGuildSetList->SetInt(row, GUILD_SET_REC_COL_AUTO_OPEN_DANCING, time);


}

void GuildBaseManager::OnSetGuildShortName(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
{

	const wchar_t* guildShortName = args.WideStrVal(3);
	const wchar_t* playerName = args.WideStrVal(4);
	int spend = 0;// EnvirValue::EnvirQueryInt(ENV_VALUE_CHANGE_SHORT_NAME);
	bool ret = GuildNumManage::m_pInstance->CanDecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, spend);

	if (!ret)
	{
		SendChangeGuildShortNameResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
		return;
	}

	IRecord *pGuildSymbol = m_pGuildPubData->GetRecord(GUILD_SYSMBOL_REC);
	if (pGuildSymbol == NULL)
	{
		SendChangeGuildShortNameResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
		return ;
	}

	int shortNameRow = pGuildSymbol->FindWideStr(GUILD_SYSMBOL_REC_COL_SHORT_NAME, guildShortName);
	if (shortNameRow >= 0)
	{
		SendChangeGuildShortNameResult(pPubKernel, sourceId, sceneId, guildName, playerName, GUILD_MSG_REQ_FAIL);
		return ;
	}
	int row = pGuildSymbol->FindWideStr(GUILD_SYSMBOL_REC_COL_GUILD_NAME, guildName);
	if (row < 0)
	{
		return;
	}
	
	pGuildSymbol->SetWideStr(row, GUILD_SYSMBOL_REC_COL_SHORT_NAME, guildShortName);
	GuildNumManage::m_pInstance->DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, spend, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_CHANGE_SHORT_NAME, playerName);
	SendChangeGuildShortNameResult(pPubKernel, sourceId, sceneId, guildName,playerName, GUILD_MSG_REQ_SUCC);

}



void GuildBaseManager::OnUpdateTeachNum(IPubKernel* pPubKernel, const wchar_t*guildName, int sourceId, int sceneId, const IVarList& args, int nOffset)
{
	// 格式: domain[string] guildName[widestr] msgid[int] uid[string] type[EmTeachType] num[int](改变的值)
	const char* pszUid = args.StringVal(nOffset++);
	EmTeachType eType = (EmTeachType)args.IntVal(nOffset++);
	int nChanged = args.IntVal(nOffset);
	if ( StringUtil::CharIsNull(pszUid) )
	{
		::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnUpdateTeachNum] player uid is null");
		return;
	}

	if (nChanged == 0)
	{
		::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnUpdateTeachNum] not changed value( type:%d value:%d)", eType, nChanged);
		return;
	}

	IPubData* pGuildData = FindGuildData(guildName);
	if (pGuildData == NULL)
	{
		::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnUpdateTeachNum] not found guild pub data(guildName:%s)",
			NULL == guildName ? "" : StringUtil::WideStrAsString(guildName));
		return;
	}

	IRecord *pRec = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (NULL == pRec)
	{
		::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnUpdateTeachNum] not found guild member record");
		return;
	}

	// 更新数据
	int nRow = pRec->FindString(GUILD_MEMBER_REC_COL_NAME_UID, pszUid);
	if (nRow == -1)
	{
		::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnUpdateTeachNum] not found member(uid:%s guild:%s)");
	}
	else
	{
		int nColIndex = GUILD_MEMBER_REC_COL_TEACH_NUM;
		if (eType == EM_TEACH_RECIVER)
		{
			nColIndex = GUILD_MEMBER_REC_COL_BE_TEACH_NUM;
		}
		int nNum = pRec->QueryInt(nRow, nColIndex);
		nNum += nChanged;
		pRec->SetInt(nRow, nColIndex, nNum);
	}
}

///////////////////////////////////////////////////////////////////////////
// 回复场景服务器消息函数
///////////////////////////////////////////////////////////////////////////
// 发送创建公会的结果给场景服务器
void GuildBaseManager::SendCreateGuildResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int result)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_CREATE <<
		guildName << playerName << result;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}
// 发送接受申请加入公会的结果给场景服务器
void GuildBaseManager::SendAcceptJoinGuildResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	const wchar_t* applyName, int onLine, int result)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_ACCEPT_JOIN
		<< guildName << playerName << applyName << onLine << result;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}



// 发送处理请求的结果
void GuildBaseManager::SendRequestResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int msgId, int result)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << msgId << guildName << playerName << result;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}
// 发送退出公会回复
void GuildBaseManager::SendQuitResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int result, const wchar_t* captainName)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_QUIT
		<< guildName << playerName << result << captainName;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}


// 发送踢出公会回复
void GuildBaseManager::SendFireResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int result, int onLine, const wchar_t* memberName)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_FIRE
		<< guildName << playerName << result << onLine << memberName;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}


// 通知成员公会解散
void GuildBaseManager::SendGuildGone(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int onLine, const wchar_t* memberName)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_GUILD_GONE
		<< guildName << playerName << onLine << memberName;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}


// 通知捐赠
void GuildBaseManager::SendGuildDonate(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName, const wchar_t* playerName,
	int result, const IVarList & args)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_DONATE
		<< guildName << playerName << result << args;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}

// 帮主转移通知
void GuildBaseManager::SendCaptainResult(IPubKernel* pPubKernel, int sourceId, int sceneId, const wchar_t* oldCaptain,
	int result, const wchar_t* newCaptain, int oldCaptainPosition, const wchar_t* guildName)

{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_CAPTAIN_TRANSFER
		<< result << oldCaptain << newCaptain << oldCaptainPosition << guildName;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}




void GuildBaseManager::SendChangeGuildShortNameResult(IPubKernel* pPubKernel, int sourceId, int sceneId, const wchar_t* guildName, const wchar_t* playerName, int result)
{
	CVarList msg;
	msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_CHANGE_GUILD_SHORT_NAME
		<< result<<guildName<<playerName;
	pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
}

///////////////////////////////////////////////////////////////////////////
// 功能函数
///////////////////////////////////////////////////////////////////////////






// 添加公会到公会列表中
bool GuildBaseManager::AddGuildToGuildList(const wchar_t* guildName, const char* playerNameUid, const wchar_t* playerName,
	int playerLevel, int nation)
{
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return false;
	}

	if (pGuildList->GetRows() >= GUILD_LIST_REC_ROW_MAX)
	{
		return false;
	}

	// 列表中不应存在公会记录
	int row = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row >= 0)
	{
		return false;
	}


	// 添加一行记录-公会信息
	CVarList rows_value;
	int64_t curDate = ::time(NULL);
	rows_value << guildName << playerNameUid << playerName << playerLevel
		<< L"" << curDate << nation << curDate << L"" << (int)0 ;
	row = pGuildList->AddRowValue(-1, rows_value);
	return (row >= 0);
}




// 添加公会私有数据表格
bool GuildBaseManager::AddGuildTable( IPubData* pGuildData)
{
	if (NULL == pGuildData)
	{
		return false;
	}

	// 帮会公共数据不用添加帮会的数据
	std::wstring guildPubDataName = std::wstring(GUILD_PUB_DATA) + m_ServerId;
	if (wcscmp(pGuildData->GetName(), guildPubDataName.c_str()) == 0)
	{
		return false;
	}



	IRecord* pMemRec = pGuildData->GetRecord(GUILD_MEMBER_REC);
	CVarList cols;
	cols << VTYPE_STRING << VTYPE_WIDESTR << VTYPE_INT << VTYPE_INT
		<< VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT
		<< VTYPE_INT64 << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT64 << VTYPE_INT << VTYPE_INT;
	// 公会成员表
	if (NULL == pMemRec)
	{

		if (!pGuildData->AddRecord(GUILD_MEMBER_REC, GUILD_MEMBER_REC_ROW_MAX, GUILD_MEMBER_REC_COL_COUNT, cols))
		{
			return false;
		}
		pGuildData->SetRecordSave(GUILD_MEMBER_REC, true);
	}
	else if (check_record_changed(pMemRec, cols, GUILD_MEMBER_REC_ROW_MAX))
	{
		// 重新构造表
		change_pub_record(pGuildData, GUILD_MEMBER_REC, cols, CVarList(), GUILD_MEMBER_REC_ROW_MAX, GUILD_MEMBER_REC_COL_NAME_UID);
	}

	// 申请加入本公会的玩家列表
	cols.Clear();
	cols << VTYPE_STRING << VTYPE_WIDESTR << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT;
	if (!_create_pub_record_safe(pGuildData, GUILD_JOIN_REC, cols, CVarList(), GUILD_JOIN_REC_ROW_MAX) )
	{
		return false;
	}

	// 公会聊天记录表
	cols.Clear();
	cols << VTYPE_WIDESTR << VTYPE_WIDESTR << VTYPE_WIDESTR << VTYPE_STRING
		<< VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT64 << VTYPE_INT;

	// 校验表列是否发生变化
	int nCols = (int)cols.GetCount();
	if (pGuildData->FindRecord(GUILD_CHAT_HISTORY_REC))
	{
		if (pGuildData->GetRecordCols(GUILD_CHAT_HISTORY_REC) != nCols)
		{
			pGuildData->RemoveRecord(GUILD_CHAT_HISTORY_REC);
			pGuildData->AddRecord(GUILD_CHAT_HISTORY_REC, GUILD_CHAT_HISTORY_ROW_MAX, nCols, cols);
		}
	}
	else
	{
		if (!pGuildData->AddRecord(GUILD_CHAT_HISTORY_REC, GUILD_CHAT_HISTORY_ROW_MAX, nCols, cols))
		{
			return false;
		}
	}

	// 公会日志表
	if (!pGuildData->FindRecord(GUILD_LOG_REC))
	{
		CVarList cols;
		cols << VTYPE_INT << VTYPE_INT64 << VTYPE_WIDESTR;
		if (!pGuildData->AddRecord(GUILD_LOG_REC, GUILD_LOG_REC_MAX_ROW, GUILD_LOG_COL_TOTAL, cols))
		{
			return false;
		}
		pGuildData->SetRecordSave(GUILD_LOG_REC, true);
	}

	//弹劾表
// 	if (!pGuildData->FindRecord(GUILD_IMPEACH_MEMBER_REC))
// 	{
// 		CVarList cols;
// 		cols << VTYPE_STRING << VTYPE_WIDESTR << VTYPE_INT << VTYPE_INT << VTYPE_INT64;
// 		if (!pGuildData->AddRecord(GUILD_IMPEACH_MEMBER_REC, 0, int(cols.GetCount()), cols))
// 		{
// 			return false;
// 		}
// 		pGuildData->SetRecordSave(GUILD_IMPEACH_MEMBER_REC, true);
// 	}

	//工会请愿表：竞拍，临时表 [生存周期：竞拍时间段内]
	if (!pGuildData->FindRecord(C_REC_GUILD_PETITION_AUCTION))
	{
		CVarList cols;
		cols << VTYPE_WIDESTR ;
		if (!pGuildData->AddRecord(C_REC_GUILD_PETITION_AUCTION, 0, int(cols.GetCount()), cols))
		{
			return false;
		}
		pGuildData->SetRecordSave(C_REC_GUILD_PETITION_AUCTION, false);
	}

	//投票收集最长时间公共数据属性名
// 	if (!pGuildData->FindAttr(GUILD_IMPEACH_VOTE_TIME))
// 	{
// 		pGuildData->AddAttr(GUILD_IMPEACH_VOTE_TIME, VTYPE_INT64);
// 		pGuildData->SetAttrSave(GUILD_IMPEACH_VOTE_TIME, true);
// 	}
// 	//弹劾成功需求相应人数百分小数
// 	if (!pGuildData->FindAttr(GUILD_IMPEACH_ANSWER_NUM))
// 	{
// 		pGuildData->AddAttr(GUILD_IMPEACH_ANSWER_NUM, VTYPE_INT);
// 		pGuildData->SetAttrSave(GUILD_IMPEACH_ANSWER_NUM, true);
// 	}
// 	//能弹劾的帮主离线时间
// 	if (!pGuildData->FindAttr(GUILD_IMPEACH_OFFLINE_TIME))
// 	{
// 		pGuildData->AddAttr(GUILD_IMPEACH_OFFLINE_TIME, VTYPE_INT64);
// 		pGuildData->SetAttrSave(GUILD_IMPEACH_OFFLINE_TIME, true);
// 	}

	// 帮会表数据版本号属性 
	if (!pGuildData->FindAttr(GUILD_PUB_DATA_VERSION_ID))
	{
		// 将成员的职位值更新
		UpdateMemberPositionVal(pGuildData);

		pGuildData->AddAttr(GUILD_PUB_DATA_VERSION_ID, VTYPE_INT);
		pGuildData->SetAttrSave(GUILD_PUB_DATA_VERSION_ID, true);
		pGuildData->SetAttrInt(GUILD_PUB_DATA_VERSION_ID, GUILD_DATA_VERSION_ID);
	}
	// 增加踢人次数属性
	if (!pGuildData->FindAttr("FireNum"))
	{
		pGuildData->AddAttr("FireNum", VTYPE_INT);
		pGuildData->SetAttrSave("FireNum", true);
	}
	//请愿	
	if (!pGuildData->FindRecord(GUILD_PETITION_REC))
	{
		CVarList cols;
		cols << VTYPE_WIDESTR << VTYPE_INT;
		if (!pGuildData->AddRecord(GUILD_PETITION_REC, 0, int(cols.GetCount()), cols))
		{
			return false;
		}
		pGuildData->SetRecordSave(GUILD_PETITION_REC, true);
	}

	


	// 增加帮战的数据
	//GVGBattleGameSingleton::Instance()->AddGuildGVGData(pGuildData);
	return true;
}






// 添加申请人到申请玩家列表表格中
bool GuildBaseManager::AddPlayerToApplyTable(IPubData* pGuildData, const char* pPlayerNameUid, const wchar_t* playerName,
	int playerSex, int playerLevel, int ability, int career, int vipLv)
{
	IRecord* pApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (pApplyList == NULL)
	{
		return false;
	}

	int row = pApplyList->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, playerName);
	if (row >= 0)
	{
		// 已记录该玩家的申请，无需重复记录
		return false;
	}

	// 申请表已满
	if (pApplyList->GetRows() >= GUILD_JOIN_REC_ROW_MAX)
	{
		pApplyList->RemoveRow(0);
	}

	// 添加一行记录-公会信息
	CVarList rowValue;
	rowValue << pPlayerNameUid << playerName << playerSex << playerLevel << career << ability<<vipLv << ONLINE;
	row = pApplyList->AddRowValue(-1, rowValue);

	return (row >= 0);
}








void GuildBaseManager::SendToGuildMemApplyNew(IPubKernel* pPubKernel, IPubData* pGuildData, int sourceId, int sceneId)
{

	if (pGuildData == NULL || pPubKernel){
		return;
	}

	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return;
	}
	int rowCount = pGuildMemberList->GetRows();
	if (rowCount <= 0)
	{
		return;
	}

	// 循环保护
	LoopBeginCheck(be);
	for (int row = 0; row < rowCount; ++row)
	{
		LoopDoCheck(be);
		const char* pMemberNameUid = pGuildMemberList->QueryString(row, GUILD_MEMBER_REC_COL_NAME_UID);
		const wchar_t* memberName = pGuildMemberList->QueryWideStr(row, GUILD_MEMBER_REC_COL_NAME);
		int onLine = pGuildMemberList->QueryInt(row, GUILD_MEMBER_REC_COL_ONLINE);
		CVarList msg;
		msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_NEW_APPLY << memberName;
		pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
	}

}







// 添加一条记录到申请者列表表格
bool GuildBaseManager::AddRecordToApplyListTable(const char *playerUid, const wchar_t* playerName, const wchar_t* guildName)
{
	IRecord* pApplyList = m_pGuildPubData->GetRecord(GUILD_APPLY_REC);
	if (pApplyList == NULL)
	{
		return false;
	}

	int row = pApplyList->FindWideStr(GUILD_APPLY_REC_COL_NAME, playerName);
	if (row >= 0)
	{
		// 已存在该玩家的申请公会记录，更新后返回
		std::wstring guildNames;
		guildNames = pApplyList->QueryWideStr(row, GUILD_APPLY_REC_COL_GUILD);
		guildNames += L";";
		guildNames += guildName;
		pApplyList->SetWideStr(row, GUILD_APPLY_REC_COL_GUILD, guildNames.c_str());
		return true;
	}

	// 如果不存在，需要新添加，先判断数量是否达到上限
	if (pApplyList->GetRows() >= GUILD_APPLY_LIST_ROW_MAX)
	{
		// 如果达到上限，先删除最早的一条记录
		const wchar_t* applyName = pApplyList->QueryWideStr(0, GUILD_APPLY_REC_COL_NAME);
		if (applyName == NULL)
		{
			return false;
		}
		if (!DelAllApplyRecord(applyName))
		{
			return false;
		}
	}

	CVarList rowValue;
	rowValue << playerUid << playerName << guildName;
	row = pApplyList->AddRowValue(-1, rowValue);

	return (row >= 0);
}
// 删除申请玩家列表中的记录
bool GuildBaseManager::DelPlayerFromApplyTable(IPubData* pGuildData, const wchar_t* playerName)
{
	IRecord* pApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (pApplyList == NULL)
	{
		return false;
	}

	int row = pApplyList->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, playerName);
	if (row >= 0)
	{
		pApplyList->RemoveRow(row);
	}

	return true;
}


// 删除申请者列表中申请公会项记录
bool GuildBaseManager::DelRecordFromApplyListTable(const wchar_t* playerName, const wchar_t* guildName)
{
	IRecord* pApplyList = m_pGuildPubData->GetRecord(GUILD_APPLY_REC);
	if (pApplyList == NULL)
	{
		return false;
	}

	int row = pApplyList->FindWideStr(GUILD_APPLY_REC_COL_NAME, playerName);
	if (row >= 0)
	{
		std::wstring guildNames;
		CVarList newGuildList;

		guildNames = pApplyList->QueryWideStr(row, GUILD_APPLY_REC_COL_GUILD);
		CVarList guildList;
		::util_split_wstring(guildList, guildNames, L";");
		int guildCount = (int)guildList.GetCount();
		// 循环保护
		LoopBeginCheck(bg);
		for (int i = 0; i < guildCount; ++i)
		{
			LoopDoCheck(bg);
			if (wcscmp(guildName, guildList.WideStrVal(i)) == 0)
			{
				continue;
			}
			newGuildList.AddWideStr(guildList.WideStrVal(i));
		}

		int newGuildCount = (int)newGuildList.GetCount();
		if (newGuildCount <= 0)
		{
			pApplyList->RemoveRow(row);
		}
		else
		{
			std::wstring newGuildNames;
			// 循环保护
			LoopBeginCheck(bh);
			for (int i = 0; i < newGuildCount; ++i)
			{
				LoopDoCheck(bh);
				newGuildNames += newGuildList.WideStrVal(i);
				if (i != newGuildCount - 1)
				{
					newGuildNames += L";";
				}
			}
			pApplyList->SetWideStr(row, GUILD_APPLY_REC_COL_GUILD, newGuildNames.c_str());
		}
	}

	return true;
}
// 检查是否还可以加入公会
bool GuildBaseManager::CanJoinGuild(const wchar_t* guildName, int maxMember)
{
	IRecord* pGuildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return false;
	}

	int row = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0)
	{
		return false;
	}
	

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return false;
	}

	int curMember = pGuildMemberList->GetRows();
	return (curMember < maxMember);
}
	



// 获取申请人的信息
bool GuildBaseManager::GetApplyInfo(const wchar_t* guildName,
	const wchar_t* applyName, int &sex, int& career, int& level, int& fight, int& vipLv)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}
	IRecord* joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (joinRecord == NULL)
	{
		return false;
	}
	int row = joinRecord->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, applyName);
	if (row < 0)
	{
		return false;
	}

	career = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_CAREER);
	sex = joinRecord->QueryInt(row, GUILD_MEMBER_REC_COL_SEX);
	level = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_LEVEL);
	fight = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_FIGHT_ABILITY);
	vipLv = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_VIP);

	return true;
}




// 删除申请者的全部申请记录
bool GuildBaseManager::DelAllApplyRecord(const wchar_t* applyName)
{
	IRecord* pApplyList = m_pGuildPubData->GetRecord(GUILD_APPLY_REC);
	if (pApplyList == NULL)
	{
		return false;
	}

	int row = pApplyList->FindWideStr(GUILD_APPLY_REC_COL_NAME, applyName);
	if (row < 0)
	{
		return true;
	}

	std::wstring guildNames = pApplyList->QueryWideStr(row, GUILD_APPLY_REC_COL_GUILD);
	CVarList guildList;
	::util_split_wstring(guildList, guildNames, L";");

	int guildCount = (int)guildList.GetCount();
	// 循环保护
	LoopBeginCheck(bi);
	for (int i = 0; i < guildCount; ++i)
	{
		LoopDoCheck(bi);
		// 删除公会的申请者列表中的申请者记录
		std::wstring guildDataName = guildList.WideStrVal(i) + m_ServerId;
		IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
		if (pGuildData == NULL)
		{
			continue;
		}
		IRecord* pGuildApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
		if (pGuildApplyList == NULL)
		{
			continue;
		}
		int guildRow = pGuildApplyList->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, applyName);
		if (guildRow < 0)
		{
			continue;
		}

		if (!pGuildApplyList->RemoveRow(guildRow))
		{
			continue;
		}
	}

	// 删除公共的申请者列表中的申请者记录
	return pApplyList->RemoveRow(row);
}



// 删除申请者在一个公会中的申请记录
bool GuildBaseManager::DelGuildApplyRecord(const wchar_t* guildName, const wchar_t* applyName)
{
	// 删除公会申请者表格的记录
	DelRecordFromApplyListTable(applyName, guildName);

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		// 不存在公会 直接取消成功
		return true;
	}

	// 删除申请者表格中申请该公会的记录
	DelPlayerFromApplyTable(pGuildData, applyName);

	return true;
}


// 删除一个公会全部的申请记录
bool GuildBaseManager::DelGuildAllApplyRecord(const wchar_t* guildName, bool needDeleteRecord)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}

	IRecord* pGuildApplyList = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (pGuildApplyList == NULL)
	{
		return false;
	}

	int row = pGuildApplyList->GetRows();
	if (row <= 0)
	{
		// 如果没有申请者，则不需要处理
		return true;
	}

	// 循环保护
	LoopBeginCheck(bj);
	for (int i = 0; i < row; ++i)
	{
		LoopDoCheck(bj);
		const wchar_t* applyName = pGuildApplyList->QueryWideStr(i, GUILD_JOIN_REC_COL_PLAYER_NAME);
		if (applyName != NULL)
		{
			DelRecordFromApplyListTable(applyName, guildName);
		}
	}

	pGuildApplyList->ClearRow();

	if (needDeleteRecord)
	{
		pGuildData->RemoveRecord(GUILD_JOIN_REC);
	}

	return true;
}



// 添加申请者加入公会表格
bool GuildBaseManager::AddApplyJoinGuild(const wchar_t* guildName, const char* pApplyNameUid, const wchar_t* applyName,
	int sex, int career, int level, int fight, int onLine,int  vipLv)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return false;
	}

	int row = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, applyName);
	if (row >= 0)
	{
		return true;
	}

	CVarList rowValue;
	int64_t curDate = ::time(NULL);
	rowValue << pApplyNameUid << applyName << sex << GUILD_POSITION_MEMBER << level << career << fight << 0
		<< curDate << onLine <<0<<vipLv << 0 << curDate << 0 << 0;
	row = pGuildMemberList->AddRowValue(-1, rowValue);

	return (row >= 0);
}
// 添加离线申请者加入公会记录
bool GuildBaseManager::AddOutlineJoinGuild(const wchar_t* guildName, const char* applyUid, const wchar_t* applyName)
{
	IRecord* pOutlineJoin = m_pGuildPubData->GetRecord(GUILD_OUTLINE_JOIN_REC);
	if (pOutlineJoin == NULL)
	{
		return false;
	}

	int row = pOutlineJoin->FindWideStr(GUILD_OUTLINE_JOIN_REC_COL_PLAYER_NAME, applyName);
	if (row >= 0)
	{
		return false;
	}

	if (pOutlineJoin->GetRows() >= GUILD_OUTLINE_JOIN_ROW_MAX)
	{
		return false;
	}

	CVarList rowValue;
	rowValue << applyUid << applyName << guildName << ::time(NULL);
	row = pOutlineJoin->AddRowValue(-1, rowValue);

	return (row >= 0);
}
// 获取表格操作接口及玩家记录所在行
IRecord* GuildBaseManager::GetGuildDataOpt(const wchar_t* guildName, const wchar_t* playerName,
	const char* tableName, int playNameCol, int& row)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return NULL;
	}
	IRecord* pRecord = pGuildData->GetRecord(tableName);
	if (pRecord == NULL)
	{
		return NULL;
	}

	row = pRecord->FindWideStr(playNameCol, playerName);
	if (row < 0)
	{
		return NULL;
	}

	return pRecord;
}



// 获取表格操作接口
IRecord* GuildBaseManager::GetGuildDataRecOpt(const wchar_t* guildName, const char* tableName)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return NULL;
	}
	IRecord* pRecord = pGuildData->GetRecord(tableName);
	if (pRecord == NULL)
	{
		return NULL;
	}

	return pRecord;
}


// 向公会的私有数据表格中写入数据
bool GuildBaseManager::SetGuildData(const wchar_t* guildName, const wchar_t* playerName,
	const char* tableName, int playNameCol, int col, const IVarList& value)
{
	if (guildName == NULL || tableName == NULL || value.GetCount() != 1)
	{
		return false;
	}

	int row = 0;
	IRecord* pRecord = GetGuildDataOpt(guildName, playerName, tableName, playNameCol, row);
	if (pRecord == NULL)
	{
		return false;
	}

	switch (value.GetType(0))
	{
		case VTYPE_INT:
		{
			pRecord->SetInt(row, col, value.IntVal(0));
		}
		break;
		case VTYPE_INT64:
		{
			pRecord->SetInt64(row, col, value.Int64Val(0));
		}
		break;
		default:
			return false;
	}

	return true;
}


// 从公会的私有数据表格中获取数据
bool GuildBaseManager::GetGuildData(const wchar_t* guildName, const wchar_t* playerName,
	const char* tableName, int playNameCol, int col, CVarList& value)
{
	if (guildName == NULL || tableName == NULL || value.GetCount() != 1)
	{
		return false;
	}

	int row = 0;
	IRecord* pRecord = GetGuildDataOpt(guildName, playerName, tableName, playNameCol, row);
	if (pRecord == NULL)
	{
		return false;
	}

	switch (value.GetType(0))
	{
		case VTYPE_INT:
		{
			value.SetInt(0, pRecord->QueryInt(row, col));
		}
		break;
		case VTYPE_INT64:
		{
			value.SetInt64(0, pRecord->QueryInt64(row, col));
		}
		break;
		default:
			return false;
	}

	return true;
}




// 向公会的公共数据表格中写入数据
bool GuildBaseManager::SetGuildPubData(const wchar_t* guildName, const char* tableName,
	int guildNameCol, int col, const IVarList& value)
{
	if (guildName == NULL || tableName == NULL || value.GetCount() != 1)
	{
		return false;
	}

	IRecord* pRecord = m_pGuildPubData->GetRecord(tableName);
	if (pRecord == NULL)
	{
		return false;
	}

	int row = pRecord->FindWideStr(guildNameCol, guildName);
	if (row < 0)
	{
		return false;
	}

	switch (value.GetType(0))
	{
		case VTYPE_INT:
		{
			pRecord->SetInt(row, col, value.IntVal(0));
		}
		break;
		case VTYPE_INT64:
		{
			pRecord->SetInt64(row, col, value.Int64Val(0));
		}
		break;
		case VTYPE_WIDESTR:
		{
			pRecord->SetWideStr(row, col, value.WideStrVal(0));
		}
		break;
		default:
			return false;
	}

	return true;
}


// 从公会的公共数据表格中获取数据
bool GuildBaseManager::GetGuildPubData(const wchar_t* guildName, const char* tableName,
	int guildNameCol, int col, CVarList& value)
{
	if (guildName == NULL || tableName == NULL || value.GetCount() != 1)
	{
		return false;
	}

	IRecord* pRecord = m_pGuildPubData->GetRecord(tableName);
	if (pRecord == NULL)
	{
		return false;
	}

	int row = pRecord->FindWideStr(guildNameCol, guildName);
	if (row < 0)
	{
		return false;
	}

	switch (value.GetType(0))
	{
		case VTYPE_INT:
		{
			value.SetInt(0, pRecord->QueryInt(row, col));
		}
		break;
		case VTYPE_INT64:
		{
			value.SetInt64(0, pRecord->QueryInt64(row, col));
		}
		break;
		case VTYPE_WIDESTR:
		{
			value.SetWideStr(0, pRecord->QueryWideStr(row, col));
		}
		break;
		default:
			return false;
	}

	return true;
}

// 给公会排序数据缩影
bool GuildBaseManager::DumpGuildSortHelper(IRecord *guildRec, int row, GuildSortHelper_t *helper)
{
	if (guildRec == NULL || helper == NULL || 0 > row)
	{
		return false;
	}

	int rows = guildRec->GetRows();
	if (row > rows)
	{
		return false;
	}
	helper->guildName = guildRec->QueryWideStr(row, GUILD_LIST_REC_COL_NAME);
	if (helper->guildName.empty())
	{
		return false;
	}
	helper->guildLevel = 1;// GuildBuildingManage::m_pInstance->GetBuildingLevel(helper->guildName.c_str(), GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_JU_YI_TING);;
	helper->fightAbiliey = guildRec->QueryInt(row, GUILD_LIST_REC_COL_FIGHT_ABILITY);
	std::wstring guildDataName = helper->guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return false;
	}
	helper->curMember = pGuildMemberList->GetRows();
	helper->randDate = guildRec->QueryInt64(row, GUILD_LIST_REC_COL_RANK_DATE);

	return true;
}


// 工会排序
void GuildBaseManager::SortGuild(const GuildSortHelper_t *helperOld, const GuildSortHelper_t *helperNew)
{
	IRecord* guildSortRecord = m_pGuildPubData->GetRecord(GUILD_SORT_REC);
	if (guildSortRecord == NULL)
	{
		return;
	}

	if (helperOld != NULL && helperNew != NULL && *helperOld == *helperNew)
	{
		return;
	}
	// 首先删除sort record中的老记录
	if (helperOld != NULL)
	{
		m_GuildSortHelperSet.erase(*helperOld);
		int row = guildSortRecord->FindWideStr(GUILD_SORT_REC_COL_NAME, helperOld->guildName.c_str());
		if (row >= 0)
		{
			guildSortRecord->RemoveRow(row);
		}
	}

	// 然后插入新记录
	if (helperNew != NULL)
	{
		// 插入数据
		std::pair<GuildSortHelperSetIter, bool> pairRet = m_GuildSortHelperSet.insert(*helperNew);

		// 插入成功
		if (pairRet.second)
		{
			// 找到插入位置的后继结点
			GuildSortHelperSetIter iter = ++pairRet.first;

			int insertRow = -1;
			if (iter != m_GuildSortHelperSet.end())
			{
				// 查找需要插入的位置
				const wchar_t *succeedGuildName = iter->guildName.c_str();
				int succeedRow = guildSortRecord->FindWideStr(GUILD_SORT_REC_COL_NAME, succeedGuildName);
				if (succeedRow >= 0)
				{
					insertRow = succeedRow;
				}
			}

			CVarList varList;
			varList << helperNew->guildName;
			guildSortRecord->AddRowValue(insertRow, varList);
		}
	}

	// 校验排序表
	CheckAndRefreshSortRecord();
}


void GuildBaseManager::AddMemberGuildDevoteMax(const wchar_t *guildName, const wchar_t*playerName, int value)
{
	if (StringUtil::CharIsNull(guildName) || StringUtil::CharIsNull(playerName))
	{
		return;
	}
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL){
		return ;
	}

	IRecord* guildMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (guildMemberRecord == NULL)
	{
		return;
	}

	int row = guildMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME,playerName);
	if (row < 0){
		return  ;
	}

	int maxDonate = guildMemberRecord->QueryInt(row, GUILD_MEMBER_REC_COL_DEVOTE);
	maxDonate += value;
	guildMemberRecord->SetInt(row, GUILD_MEMBER_REC_COL_DEVOTE,maxDonate);

}


// 职位是否已满
bool GuildBaseManager::IsPositionFull(IRecord* memberList, int position, int maxNumber)
{
	if (memberList == NULL)
	{
		return true;
	}
	if (position >= GUILD_POSITION_COUNT)
	{
		return true;
	}
	int posCount = 0;
	int rowCount = memberList->GetRows();
	// 循环保护
	LoopBeginCheck(bk);
	for (int i = 0; i < rowCount; i++)
	{
		LoopDoCheck(bk);
		int memberPosition = memberList->QueryInt(i, GUILD_MEMBER_REC_COL_POSITION);
		if (position == memberPosition)
		{
			posCount++;
			if (posCount >= maxNumber)
			{
				return true;
			}
		}
	}
	return posCount >= maxNumber;
}






//添加公会经验
int GuildBaseManager::OnAddGuildDefendExp(IPubKernel* pPubKernel, const wchar_t* guildName,
	int sourceId, int sceneId, const IVarList& args)
{
	////1、奖励个人贡献
	//int rewardGuildExp = args.IntVal(3);

	////2、获取公会列表
	//IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	//if (guildList == NULL)
	//{
	//	return 0;
	//}
	//int guildRow = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	//if (guildRow < 0)
	//{
	//	return 0;
	//}

	//GuildSortHelper_t guildSortHelperOld;
	//if (!DumpGuildSortHelper(guildList, guildRow, &guildSortHelperOld))
	//{
	//	return 0;
	//}

	////4、更新公会经验值，并进行自动升级处理
	//int oldGuildLevel = guildList->QueryInt(guildRow, GUILD_LIST_REC_COL_LEVEL);
	//int guildLevel = oldGuildLevel;
	//int guildExp = guildList->QueryInt(guildRow, GUILD_LIST_REC_COL_CUR_EXP);
	//int guildCurExp = guildExp + rewardGuildExp;
	////int guildMaxExp = 0;
	//// 循环保护
	//LoopBeginCheck(bc);
	//while (true)
	//{
	//	LoopDoCheck(bc);
	//	if (guildLevel >= (int)m_GuildUpLevelConfig.size() || guildLevel < 0)
	//	{
	//		break;
	//	}
	//	GuildUpLevelConfig_t *upLevelConfig = &m_GuildUpLevelConfig[guildLevel];
	//	if (guildCurExp >= upLevelConfig->m_LevelUpExp)
	//	{
	//		if (guildLevel < (int)m_GuildUpLevelConfig.size() - 1)
	//		{
	//			guildLevel++;
	//			guildCurExp -= upLevelConfig->m_LevelUpExp;
	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}

	//guildList->SetInt(guildRow, GUILD_LIST_REC_COL_LEVEL, guildLevel);
	//guildList->SetInt(guildRow, GUILD_LIST_REC_COL_CUR_EXP, guildCurExp);

	//// 公会升级 记录日志
	//if (guildLevel > oldGuildLevel)
	//{
	//	std::wstring wstrlogArgs = std::wstring(guildName) + L"," + StringUtil::IntAsWideStr(guildLevel);
	//	RecordGuildLog(guildName, GUILD_LOG_TYPE_UP_LEVEL, wstrlogArgs.c_str());

	//	// 通知帮会成员 帮会升级
	//	CommandGuildLevelChange(pPubKernel, guildName, guildLevel, true);
	//}

	//// 公会排序
	//GuildSortHelper_t guildSortHelperNew;
	//if (DumpGuildSortHelper(guildList, guildRow, &guildSortHelperNew))
	//{
	//	SortGuild(&guildSortHelperOld, &guildSortHelperNew);
	//}

	return 0;
}






// 添加新的帮会场景分组资源数据
int GuildBaseManager::AddGuildStationGroupIDData(const wchar_t* guildName)
{
	//1、非空判断
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}
	
	//2、获取公会场景分组号记录表
	IRecord* guildStationList = m_pGuildPubData->GetRecord(GUILD_STATION_GROUP_REC);
	if (guildStationList == NULL)
	{
		return 0;
	}

	//3、获取是否已经有记录
	int row = guildStationList->FindWideStr(GUILD_STATION_GROUP_REC_COL_GUILD_NAME, guildName);
	if (row < 0)
	{
		CVarList var;
		var << guildName << (10 + ++m_stationGroup);  // 帮会名 << 分组号(从10开始)
		guildStationList->AddRowValue(-1, var);
	}

	return 0;
}


void GuildBaseManager::RefreshGuildFightAbility(IPubKernel * pPubKernel, const wchar_t* guildName)
{
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return;
	}
	int abilityAll = 0;
	// 更新公会成员列表
	IRecord* pGuildMemberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList != NULL)
	{

		int rowSize = pGuildMemberList->GetRows();
		LoopBeginCheck(a);
		for (int size = 0; size < rowSize; size++)
		{
			LoopDoCheck(a);
			abilityAll += pGuildMemberList->QueryInt(size, GUILD_MEMBER_REC_COL_FIGHT_ABILITY);
		}
	}

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
	guildList->SetInt(guildRow, GUILD_LIST_REC_COL_FIGHT_ABILITY, abilityAll);

	// 更新公会战斗力排行榜
	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace)
	{
		return;
	}

	
	CVarList update_msg;
				    
	int nBoom = GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_FANRONGDU);
	int nLevel = 1;// GuildBuildingManage::m_pInstance->GetBuildingLevel(guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_SHU_YUAN);

	int target_ident = 0;
	const wchar_t* target_alias = L"";
	GuildHelper::GetGuildIdent(guildName, target_ident, target_alias);

	update_msg << RESERVED_NUMBER
		<< GUILD_FIGHT_POWER_RANK_LIST
		<< INSERT_IF_NOT_EXIST
		<< "guild_name" << guildName
		<< "guild_fight_power" << abilityAll
		<< "guild_identifying" << target_ident
		<< "guild_alias" << target_alias;
	
	DomainRankList::m_pDomainRankList->UpdateEntry(pPubKernel, pPubSpace, update_msg, 1);
}

bool GuildBaseManager::AutoAgreeJoinGuild(IPubKernel *pPubKernel, const wchar_t* guildName, const wchar_t* applyName, int sourceId, int sceneId)
{
	IRecord* guildList = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (guildList == NULL)
	{
		return false;
	}
	int guildRow = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return false;
	}

	IRecord * pGuildSetRc = m_pGuildPubData->GetRecord(GUILD_SET_REC);
	if (pGuildSetRc == NULL){
		return false;
	}
	int guildSetRow = pGuildSetRc->FindWideStr(GUILD_SET_REC_COL_GUILD_NAME, guildName);
	if (guildSetRow < 0)
	{
		return false;
	}


	int autoAgree = pGuildSetRc->QueryInt(guildSetRow, GUILD_SET_REC_COL_AUTO_AGREE);

	if (autoAgree != GUILD_AUTO_AGREE_ON){
		return false;
	}

	int minLevel = pGuildSetRc->QueryInt(guildSetRow, GUILD_SET_REC_COL_AUTO_AGREE_MIN_LEVEL);
	int maxLevel = pGuildSetRc->QueryInt(guildSetRow, GUILD_SET_REC_COL_AUTO_AGREE_MAX_LEVEL);
	int abilityLimit = pGuildSetRc->QueryInt(guildSetRow, GUILD_SET_REC_COL_AUTO_AGREE_ABILITY);

	int curLevel = 1;// GuildBuildingManage::m_pInstance->GetBuildingLevel(guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	GuildUpLevelConfigVector::iterator it = m_GuildUpLevelConfig.begin();
	int maxMember = 0;
	for (; it != m_GuildUpLevelConfig.end(); it++)
	{
		if (it->m_Level == curLevel)
		{
			maxMember = it->m_MaxMember;
		}

	}
	if (!CanJoinGuild(guildName, maxMember))
	{
		return false;
	}

	GuildSortHelper_t guildSortHelperOld;
	if (!DumpGuildSortHelper(guildList, guildRow, &guildSortHelperOld))
	{
		return false;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return false;
	}

	

	IRecord* joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (joinRecord == NULL)
	{
		return false;
	}

	int row = joinRecord->FindWideStr(GUILD_JOIN_REC_COL_PLAYER_NAME, applyName);
	if (row < 0)
	{
		return false;
	}

	const char* pApplyNameUid = joinRecord->QueryString(row, GUILD_JOIN_REC_COL_PLAYER_NAME_UID);
	int	career = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_CAREER);
	int sex = joinRecord->QueryInt(row, GUILD_MEMBER_REC_COL_SEX);
	int level = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_LEVEL);
	int fightAbility = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_FIGHT_ABILITY);
	int vipLv = joinRecord->QueryInt(row, GUILD_JOIN_REC_COL_VIP);
	if (minLevel <= level &&level <= maxLevel && fightAbility >= abilityLimit)
	{
		int applyOnline = ONLINE;
		// 把申请者加入公会中
		if (!AddApplyJoinGuild(guildName, pApplyNameUid, applyName, sex, career, level, fightAbility, applyOnline, vipLv))
		{
			return false;
		}
		else
		{
			// 加入成功，记录日志
			RecordGuildLog(guildName, GUILD_LOG_TYPE_JOIN, applyName);
		}

		// 删除申请者申请记录
		if (!DelAllApplyRecord(applyName))
		{
			return false;
		}

		// 如果申请者不在线，则加到表格保存，等玩家上线设置其加入公会
		if (applyOnline == OFFLINE)
		{
			AddOutlineJoinGuild(guildName, pApplyNameUid, applyName);
		}

		CVarList msg;
		msg << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_MSG_ACCEPT_JOIN
			<< guildName << applyName << applyName << applyOnline << GUILD_MSG_REQ_SUCC;
		msg << GUILD_POSITION_MEMBER << sex << level << career << 0 << 0 << fightAbility << 0;
		pPubKernel->SendPublicMessage(sourceId, sceneId, msg);
		RefreshGuildFightAbility(pPubKernel, guildName);
		// 公会排序
		GuildSortHelper_t guildSortHelperNew;
		if (DumpGuildSortHelper(guildList, guildRow, &guildSortHelperNew))
		{
			SortGuild(&guildSortHelperOld, &guildSortHelperNew);
		}
		return true;
	}
	return false;
}



void GuildBaseManager::GetGuildLogWstring(const IVarList& args, std::wstring& logWsting)
{

	for (size_t index = 0; index < args.GetCount(); ++index)
	{
		std::wstring data;
		switch (args.GetType(index))
		{

			case VTYPE_BOOL:
				data = StringUtil::IntAsWideStr(args.BoolVal(index));
				break;
			case VTYPE_INT:
				data = StringUtil::IntAsWideStr(args.IntVal(index));
				break;
			case VTYPE_INT64:
				data = StringUtil::StringAsWideStr(StringUtil::Int64AsString(args.Int64Val(index)).c_str());
				break;
			case VTYPE_FLOAT:
				data = StringUtil::FloatAsWideStr(args.FloatVal(index));
				break;
			case VTYPE_DOUBLE:

				data = StringUtil::StringAsWideStr(StringUtil::DoubleAsString(args.DoubleVal(index)).c_str());
				break;
			case VTYPE_STRING:
				data = StringUtil::StringAsWideStr(args.StringVal(index));
				break;
			case VTYPE_WIDESTR:
				data = args.WideStrVal(index);
				break;

				break;
			default:
				continue;
		}

		logWsting += data + L",";

	}




}

void GuildBaseManager::SendAllGuildLog( int type, const IVarList& args)
{
	std::wstring log;
	GetGuildLogWstring(args,log);

	if (StringUtil::CharIsNull(log.c_str()))
	{
		return;
	}
	IRecord* pGuildListRec = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildListRec == NULL)
	{
		return ;
	}


	for (int row = 0; row < pGuildListRec->GetRows(); row++)
	{
		const wchar_t*guildName = pGuildListRec->QueryWideStr(row, GUILD_LIST_REC_COL_NAME);
		RecordGuildLog(guildName,type,log.c_str());
	}




}

void GuildBaseManager::SendSameNationGuildLog( int nation, int type, const IVarList& args)
{
	std::wstring log;
	GetGuildLogWstring(args, log);
	if (StringUtil::CharIsNull(log.c_str()))
	{
		return;
	}

	IRecord* pGuildListRec = m_pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildListRec == NULL)
	{
		return;
	}


	for (int row = 0; row < pGuildListRec->GetRows(); row++)
	{

		const wchar_t*guildName = pGuildListRec->QueryWideStr(row, GUILD_LIST_REC_COL_NAME);
		RecordGuildLog(guildName, type, log.c_str());
	}


}

void GuildBaseManager::SendGuildLog( const wchar_t *guildName, int type, const IVarList& args)
{
	std::wstring log;
	GetGuildLogWstring(args, log);
	if (StringUtil::CharIsNull(log.c_str())){
		return;
	}
	RecordGuildLog(guildName, type, log.c_str());
}

int GuildBaseManager::RecordGuildLog(const wchar_t* guildName, int type, const wchar_t* args)
{
	// 帮会公共空间
	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 日志表
	IRecord* pLogRecord = pGuildData->GetRecord(GUILD_LOG_REC);
	if (pLogRecord == NULL)
	{
		return 0;
	}

	// 查看表是否满
	if (pLogRecord->GetRows() >= pLogRecord->GetRowMax())
	{
		// 表已满，删除第一行数据
		pLogRecord->RemoveRow(0);
	}

	// 获取时间戳
	int64_t time = ::time(NULL);
	// 添加新数据
	pLogRecord->AddRowValue(-1, CVarList() << type << time << args);

	return 0;
}

// bool GuildBaseManager::LoadGuldImpeach(IPubKernel* pPubKernel)
// {
// 
// 	std::string xmlPath = pPubKernel->GetResourcePath();
// 	xmlPath += GUILD_IMPEACH_XML_PATH;
// 
// 	char* pfilebuff = GetFileBuff(xmlPath.c_str());
// 	if (pfilebuff == NULL)
// 	{
// 		extend_warning_pub(LOG_ERROR, "[GuildImpeach::LoadConfig] failed");
// 		return false;
// 	}
// 
// 	try
// 	{
// 		xml_document<> doc;
// 		doc.parse<rapidxml::parse_default>(pfilebuff);
// 		xml_node<>* pNodeRoot = doc.first_node("Object");
// 		if (pNodeRoot == NULL)
// 		{
// 			delete[] pfilebuff;
// 			return false;
// 		}
// 		xml_node<>* pNode = pNodeRoot->first_node("Property");
// 		if (pNode == NULL)
// 		{
// 			delete[] pfilebuff;
// 			return false;
// 		}
// 
// 
// 		m_autoImpeachMaxTime = convert_int(QueryXmlAttr(pNode, "AutoImpeachMaxTimes"), 0);
// 		m_memOffLineTimes = convert_int(QueryXmlAttr(pNode, "MemOffLineTimes"), 0);
// 		return true;
// 	}
// 	catch (parse_error&)
// 	{
// 		delete[] pfilebuff;
// 		return false;
// 	}
// 
// 	return false;
// }
// 
// 
// //开始弹劾
// int GuildBaseManager::OnStartImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
// {
// 	if (guildName == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	std::wstring guildDataName = guildName + m_ServerId;
// 	IPubData *pData = pPubSpace->GetPubData(guildDataName.c_str());
// 	if (pData == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//如果大于0就代表当前已有进行中的弹劾
// 	if (impeachRec->GetRows() > 0)
// 	{
// 		//通知发起人
// 		pPubKernel->CommandByName(args.WideStrVal(4), CVarList() << COMMAND_GUILD_IMPEACH << PS_GUILD_IMPEACH_START_FAIL);
// 		return 0;
// 	}
// 
// 	CVarList rowData;
// 	rowData << args.StringVal(3)
// 		<< args.WideStrVal(4)
// 		<< args.IntVal(5)
// 		<< args.IntVal(6)
// 		<< args.Int64Val(7);
// 	impeachRec->AddRowValue(-1, rowData);
// 
// 	//弹劾成功需求相应人数，百分比小数
// 	pData->SetAttrInt(GUILD_IMPEACH_ANSWER_NUM, args.IntVal(8));
// 	//能弹劾的帮主离线时间
// 	pData->SetAttrInt64(GUILD_IMPEACH_OFFLINE_TIME, args.Int64Val(9));
// 	//投票收集最长时间
// 	pData->SetAttrInt64(GUILD_IMPEACH_VOTE_TIME, args.Int64Val(10));
// 
// 	RecordGuildLog(guildName, GUILD_LOG_TYPE_IMPEACH_START, args.WideStrVal(4));
// 
// 	StartImpeachTimer(pPubKernel, guildName);
// 
// 	//通知所有帮会成员
// 	IRecord *memberRecord = pData->GetRecord(GUILD_MEMBER_REC);
// 	if (memberRecord == NULL)
// 	{
// 		return 0;
// 	}
// 	int memberRecordLength = memberRecord->GetRows();
// 	LoopBeginCheck(a);
// 	for (int i = 0; i < memberRecordLength; ++i)
// 	{
// 		LoopDoCheck(a);
// 
// 		pPubKernel->CommandByName(memberRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME), CVarList() << COMMAND_GUILD_IMPEACH << PS_GUILD_IMPEACH_START_SUCCEED);
// 	}
// 
// 	return 0;
// }
// 
// //注册弹劾完成检查定时器
// void GuildBaseManager::StartImpeachTimer(IPubKernel* pPubKernel, const wchar_t* guildName)
// {
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return;
// 	}
// 
// 	std::wstring guildDataName = guildName + m_ServerId;
// 	IPubData *pData = pPubSpace->GetPubData(guildDataName.c_str());
// 	if (pData == NULL)
// 	{
// 		return;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return;
// 	}
// 	if (impeachRec->GetRows() <= 0)
// 	{
// 		return;
// 	}
// 
// 	if (!pPubKernel->FindTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), guildDataName.c_str()))
// 	{
// 		pPubKernel->RemoveTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), guildDataName.c_str());
// 	}
// 	pPubKernel->AddTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, OnImpeachExamine, GetName(), guildDataName.c_str(), GUILD_IMPEACH_EXAMINE_TIME, 10000);
// }
// 
// //弹劾完成检查定时器回调
// int GuildBaseManager::OnImpeachExamine(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time)
// {
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(m_pInstance->GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	IPubData *pData = pPubSpace->GetPubData(data_name);
// 	if (pData == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return 0;
// 	}
// 	if (impeachRec->GetRows() <= 0)
// 	{
// 		return 0;
// 	}
// 
// 	//计算剩余时间
// 	int64_t startTime = 0;
// 	int initiatorRow = impeachRec->FindInt(GUILD_IMPEACH_MEMBER_INITIATOR, GUILD_IMPEACH_INITIATOR);
// 	if (initiatorRow != -1)
// 	{
// 		startTime = impeachRec->QueryInt64(initiatorRow, GUILD_IMPEACH_MEMBER_TIME);
// 	}
// 	//剩余时间转成毫秒
// 	int64_t remainTime = (pData->QueryAttrInt64(GUILD_IMPEACH_VOTE_TIME) - (::time(NULL) - startTime)) * 1000;
// 	if (remainTime <= 0)
// 	{
// 		//移除计时器
// 		if (pPubKernel->FindTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, m_pInstance->GetName(), data_name))
// 		{
// 			pPubKernel->RemoveTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, m_pInstance->GetName(), data_name);
// 		}
// 		//结算弹劾
// 		m_pInstance->SettleImpeach(pPubKernel, data_name);
// 	}
// 	else if (remainTime <= GUILD_IMPEACH_EXAMINE_TIME)
// 	{
// 		//更改计时器间隔时间为剩余时间
// 		if (pPubKernel->FindTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, m_pInstance->GetName(), data_name))
// 		{
// 			pPubKernel->RemoveTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, m_pInstance->GetName(), data_name);
// 		}
// 		pPubKernel->AddTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, OnImpeachExamine, m_pInstance->GetName(), data_name, int(remainTime), 2);
// 	}
// 
// 	return 0;
// }


void GuildBaseManager::ClearPetition(IPubKernel* pPubKernel, const wchar_t* guildName, int type)
{
	if (StringUtil::CharIsNull(guildName) ){
		return;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return;
	}

	IRecord* pGuildPettionRec = guildData->GetRecord(GUILD_PETITION_REC);
	if (pGuildPettionRec == NULL){
		return;
	}

	for (int row = pGuildPettionRec->GetRows() - 1; row>=0; --row)
	{
		int typeRow = pGuildPettionRec->QueryInt(row, GUILD_PETITION_REC_COL_TYPE);
		if (typeRow == type){
			continue;
		}
		pGuildPettionRec->RemoveRow(row);
	}

}

void GuildBaseManager::RemovePetition(IPubKernel* pPubKernel, const wchar_t* guildName, const wchar_t* pszName)
{
	if (StringUtil::CharIsNull(guildName)){
		return;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData *guildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	if (guildData == NULL){
		return;
	}

	IRecord* pGuildPettionRec = guildData->GetRecord(GUILD_PETITION_REC);
	if (pGuildPettionRec == NULL){
		return;
	}

	for (int row = pGuildPettionRec->GetRows() - 1; row >= 0; --row)
	{
		const wchar_t* pszPlayer_ = pGuildPettionRec->QueryWideStr(row, GUILD_PETITION_REC_COL_NAME);
		if ( wcscmp( pszName, pszPlayer_ ) != 0 ){
			continue;
		}
		pGuildPettionRec->RemoveRow(row);
	}
}

IPubData* GuildBaseManager::FindGuildData(const wchar_t* guildName) const
{
	if (StringUtil::CharIsNull(guildName))
	{
		return NULL;
	}

	std::wstring guildDataName = guildName + m_ServerId;
	IPubData* pGuildData = m_pPubSpace->GetPubData(guildDataName.c_str());
	return pGuildData;
}

//结算弹劾
// bool GuildBaseManager::SettleImpeach(IPubKernel* pPubKernel, const wchar_t* data_name)
// {
// 	if (data_name == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//移除计时器
// 	if (pPubKernel->FindTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), data_name))
// 	{
// 		pPubKernel->RemoveTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), data_name);
// 	}
// 
// 	//帮会名
// 	std::wstring guildName = data_name;
// 	guildName = guildName.substr(0, guildName.length() - m_ServerId.length());
// 
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return false;
// 	}
// 	IPubData *pData = pPubSpace->GetPubData(data_name);
// 	if (pData == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//发起人
// 	int initiatorRow = impeachRec->FindInt(GUILD_IMPEACH_MEMBER_INITIATOR, GUILD_IMPEACH_INITIATOR);
// 	if (initiatorRow == -1)
// 	{
// 		//出错了,找不到发起人这次弹劾没用了
// 		impeachRec->ClearRow();
// 		return false;
// 	}
// 	std::wstring impeachName = impeachRec->QueryWideStr(initiatorRow, GUILD_IMPEACH_MEMBER_NAME);
// 
// 	//帮会成员表
// 	IRecord *memberRecord = pData->GetRecord(GUILD_MEMBER_REC);
// 	if (memberRecord == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//统计有效票数
// 	int poll = 0;
// 	int impeachRecLength = impeachRec->GetRows();
// 	int memberRecordLength = memberRecord->GetRows();
// 	LoopBeginCheck(d);
// 	for (int i = 0; i < impeachRecLength; ++i)
// 	{
// 		LoopDoCheck(d);
// 		const char *uid = impeachRec->QueryString(i, GUILD_IMPEACH_MEMBER_UID);
// 		if (StringUtil::CharIsNull(uid))
// 		{
// 			continue;
// 		}
// 		if (memberRecord->FindString(GUILD_MEMBER_REC_COL_NAME_UID, uid) != -1)
// 		{
// 			poll += impeachRec->QueryInt(i, GUILD_IMPEACH_MEMBER_NUM);
// 		}
// 	}
// 
// 	//清除这个弹劾相记录数据
// 	impeachRec->ClearRow();
// 
// 	//判断是否满足票数,加1是因为把发起者自已的投票减去
// 	int needPoll = pData->QueryAttrInt(GUILD_IMPEACH_ANSWER_NUM);
// 	if (poll < needPoll)
// 	{
// 		//记录弹劾结果到帮会日志
// 		std::wstring wstrlogArgs = impeachName + L"," + StringUtil::IntAsWideStr(GUILD_IMPEACH_RESULT_VOTE_FAIL);
// 		RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_IMPEACH_FAIL, impeachName.c_str());
// 		//通知场景服务器
// 		pPubKernel->SendPublicMessage(0, 0,
// 			CVarList() << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_IMPEACH_RESULT << guildName.c_str() << impeachName.c_str() << GUILD_IMPEACH_RESULT_VOTE_FAIL);
// 		return false;
// 	}
// 
// 	//检查帮会成员是足够
// 	if (memberRecord->GetRows() < START_IMPEACH_MEMBER_MIN)
// 	{
// 		//记录弹劾结果到帮会日志
// 		std::wstring wstrlogArgs = impeachName + L"," + StringUtil::IntAsWideStr(GUILD_IMPEACH_RESULT_MEMBER_FAIL);
// 		RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_IMPEACH_FAIL, impeachName.c_str());
// 		//通知场景服务器
// 		pPubKernel->SendPublicMessage(0, 0,
// 			CVarList() << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_IMPEACH_RESULT << guildName.c_str() << impeachName.c_str() << GUILD_IMPEACH_RESULT_MEMBER_FAIL);
// 		return false;
// 	}
// 
// 	//帮主所在行
// 	int row = memberRecord->FindInt(GUILD_MEMBER_REC_COL_POSITION, GUILD_POSITION_CAPTAIN);
// 	if (row == -1)
// 	{
// 		return false;
// 	}
// 	const wchar_t *captainName = memberRecord->QueryWideStr(row, GUILD_MEMBER_REC_COL_NAME);
// 	if (captainName == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//移交帮主
// 	if (TransferCaptain(guildName.c_str(), captainName, impeachName.c_str()) != GUILD_MSG_REQ_SUCC)
// 	{
// 		//记录弹劾结果到帮会日志
// 		std::wstring wstrlogArgs = impeachName + L"," + StringUtil::IntAsWideStr(GUILD_IMPEACH_RESULT_FAIL);
// 		RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_IMPEACH_FAIL, impeachName.c_str());
// 		//通知场景服务器
// 		pPubKernel->SendPublicMessage(0, 0,
// 			CVarList() << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_IMPEACH_RESULT << guildName.c_str() << impeachName.c_str() << GUILD_IMPEACH_RESULT_FAIL);
// 		return false;
// 	}
// 
// 	//记录弹劾结果到帮会日志
// 	std::wstring wstrlogArgs = impeachName + L"," + StringUtil::IntAsWideStr(GUILD_IMPEACH_RESULT_SUCCEED);
// 	RecordGuildLog(guildName.c_str(), GUILD_LOG_TYPE_IMPEACH_SUCCEED, impeachName.c_str());
// 
// 	//帮主转移通知
// 	SendCaptainResult(pPubKernel, 0, 0, captainName, GUILD_MSG_REQ_SUCC, impeachName.c_str(), GUILD_POSITION_MEMBER, guildName.c_str());
// 
// 	//通知场景服务器发邮件
// 	pPubKernel->SendPublicMessage(0, 0,
// 		CVarList() << PUBSPACE_GUILD << GUILD_LOGIC << PS_GUILD_IMPEACH_RESULT << guildName.c_str() << impeachName.c_str() << GUILD_IMPEACH_RESULT_SUCCEED);
// 
// 	return true;
// }

//将普通帮众值设置为新定义的值
bool GuildBaseManager::UpdateMemberPositionVal(IPubData* pPubGuildData)
{
	if (NULL == pPubGuildData)
	{
		return false;
	}
	IRecord* pGuildMemberList = pPubGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return false;
	}

	int curMember = pGuildMemberList->GetRows();
	LoopBeginCheck(a);
	for (int i = 0; i < curMember; ++i)
	{
		LoopDoCheck(a);
		int nPosition = pGuildMemberList->QueryInt(i, GUILD_MEMBER_REC_COL_POSITION);
		if (GUILD_POSITION_ELDER == nPosition)
		{
			pGuildMemberList->SetInt(i, GUILD_MEMBER_REC_COL_POSITION, GUILD_POSITION_MEMBER);
		}
	}
	return true;
}



//撤消弹劾
// int GuildBaseManager::OnUndoImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, const char *pSelfUid)
// {
// 	if (guildName == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	std::wstring guildDataName = guildName + m_ServerId;
// 	IPubData *pData = pPubSpace->GetPubData(guildDataName.c_str());
// 	if (pData == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//操作人
// 	if (StringUtil::CharIsNull(pSelfUid))
// 	{
// 		return 0;
// 	}
// 
// 	//当前发起人
// 	int initiatorRow = impeachRec->FindInt(GUILD_IMPEACH_MEMBER_INITIATOR, GUILD_IMPEACH_INITIATOR);
// 	if (initiatorRow == -1)
// 	{
// 		//出错了,找不到发起人这次弹劾没用了
// 		return 0;
// 	}
// 	const char *impeachPlayerUid = impeachRec->QueryString(initiatorRow, GUILD_IMPEACH_MEMBER_UID);
// 
// 	//判断是否是发起人
// 	if (strcmp(pSelfUid, impeachPlayerUid) != 0)
// 	{
// 		return 0;
// 	}
// 
// 	//移除计时器
// 	if (!pPubKernel->FindTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), guildName))
// 	{
// 		pPubKernel->RemoveTimer(GUILD_IMPEACH_EXAMINE_TIME_NAME, GetName(), guildName);
// 	}
// 
// 	impeachRec->ClearRow();
// 
// 	return 0;
// }
// 
// //响应弹劾
// int GuildBaseManager::OnRespondImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args)
// {
// 	if (guildName == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	const char* pSelfUid = args.StringVal(3);
// 	if (StringUtil::CharIsNull(pSelfUid))
// 	{
// 		return 0;
// 	}
// 
// 	const wchar_t *pSelfName = args.WideStrVal(4);
// 	if (StringUtil::CharIsNull(pSelfName))
// 	{
// 		return 0;
// 	}
// 
// 	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(GetName());
// 	if (pPubSpace == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	std::wstring guildDataName = guildName + m_ServerId;
// 	IPubData *pData = pPubSpace->GetPubData(guildDataName.c_str());
// 	if (pData == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	//弹劾成员操作记录表
// 	IRecord *impeachRec = pData->GetRecord(GUILD_IMPEACH_MEMBER_REC);
// 	if (impeachRec == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	if (impeachRec->GetRows() < 1)
// 	{
// 		return 0;
// 	}
// 
// 	int row = impeachRec->FindString(GUILD_IMPEACH_MEMBER_UID, pSelfUid);
// 	if (row == -1)
// 	{
// 		CVarList rowData;
// 		rowData << pSelfUid << pSelfName << GUILD_IMPEACH_RESPOND << 1 << ::time(NULL);
// 		impeachRec->AddRowValue(-1, rowData);
// 	}
// 	else
// 	{
// 		int curPollNum = impeachRec->QueryInt(row, GUILD_IMPEACH_MEMBER_NUM);
// 		impeachRec->SetInt(row, GUILD_IMPEACH_MEMBER_NUM, curPollNum + 1);
// 	}
// 
// 	//帮会成员表
// 	IRecord *memberRecord = pData->GetRecord(GUILD_MEMBER_REC);
// 	if (memberRecord == NULL)
// 	{
// 		return false;
// 	}
// 
// 	//统计有效票数
// 	int poll = 0;
// 	int impeachRecLength = impeachRec->GetRows();
// 	LoopBeginCheck(d);
// 	for (int i = 0; i < impeachRecLength; ++i)
// 	{
// 		LoopDoCheck(d);
// 		const char *uid = impeachRec->QueryString(i, GUILD_IMPEACH_MEMBER_UID);
// 		if (StringUtil::CharIsNull(uid))
// 		{
// 			continue;
// 		}
// 		if (memberRecord->FindString(GUILD_MEMBER_REC_COL_NAME_UID, uid) != -1)
// 		{
// 			poll += impeachRec->QueryInt(i, GUILD_IMPEACH_MEMBER_NUM);
// 		}
// 	}
// 
// 	//判断是否满足票数
// 	int needPoll = pData->QueryAttrInt(GUILD_IMPEACH_ANSWER_NUM);
// 	if (poll >= needPoll)
// 	{
// 		//结算弹劾
// 		m_pInstance->SettleImpeach(pPubKernel, guildDataName.c_str());
// 	}
// 
// 	return 0;
// }
// void GuildBaseManager::AutoImpeach(IPubKernel* pPubKernel, const char* space_name, const wchar_t* guildName, const wchar_t* capName)
// {
// 	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
// 	if (pPubSpace == NULL){
// 		return;
// 	}
// 	std::wstring guildDataName = guildName + StringUtil::IntAsWideStr(pPubKernel->GetServerId());
// 	IPubData* pGuildData = pPubSpace->GetPubData(guildDataName.c_str());
// 
// 	if (pGuildData == NULL){
// 		return;
// 	}
// 
// 	IRecord *memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
// 	if (memberRecord == NULL)
// 	{
// 		return;
// 	}
// 
// 	struct MemberInfo
// 	{
// 		bool operator <(const MemberInfo& other)
// 		{
// 			return devote > other.devote;
// 		}
// 		int row;
// 		int devote{ 0 };
// 		int position{ -1 };
// 	};
// 
// 	int64_t now = ::time(NULL);
// 	std::list<MemberInfo> memList;
// 
// 	int countMax = memberRecord->GetRows();
// 	for (int i = 0; i < countMax; i++)
// 	{
// 		int position = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_CAREER);
// 
// 		if (position == GUILD_POSITION_CAPTAIN){
// 			continue;
// 		}
// 		int onLineState = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_ONLINE);
// 		if (onLineState != 1) //不在线
// 		{
// 			int64_t outLine = memberRecord->QueryInt64(i, GUILD_MEMBER_REC_COL_OUTLINE_DATE);
// 			if (now - outLine > m_memOffLineTimes){
// 				continue;
// 			}
// 		}
// 		int devote = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_DEVOTE);
// 
// 		MemberInfo info;
// 		info.row = i;
// 		info.position = position;
// 		info.devote = devote;
// 		memList.push_back(info);
// 
// 	}
// 
// 	if (memList.empty())
// 	{
// 		return;
// 	}
// 
// 	memList.sort();
// 
// 	for (auto it : memList)
// 	{
// 		if (it.position == GUILD_POSITION_ASSISTANT_CAPTAIN)
// 		{
// 			const wchar_t * name = memberRecord->QueryWideStr(it.row, GUILD_MEMBER_REC_COL_NAME);
// 			m_pInstance->TransferCaptain(guildName, capName, name);
// 			return;
// 		}
// 	}
// 
// 	auto it = memList.begin();
// 	const wchar_t * name = memberRecord->QueryWideStr(it->row, GUILD_MEMBER_REC_COL_NAME);
// 	m_pInstance->TransferCaptain(guildName, capName, name);
// 
// }
// 
// void GuildBaseManager::AutoImpeachTimer(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name)
// {
// 
// 	auto timeNow = ::time(NULL);
// 	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
// 	if (pPubSpace == NULL){
// 		return;
// 	}
// 
// 	// 帮会列表
// 	IRecord* pGuildList = PubGuildManager::m_pInstance->m_pGuildPubData->GetRecord(GUILD_LIST_REC);
// 	if (pGuildList == NULL){
// 		return;
// 	}
// 
// 	// 遍历所有帮会
// 	int nRows = pGuildList->GetRows();
// 	for (int i = 0; i < nRows; ++i)
// 	{
// 		//// 帮会名称
// 		std::wstring wstrGuildName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_NAME);
// 		std::wstring guildDataName = wstrGuildName + StringUtil::IntAsWideStr(pPubKernel->GetServerId());
// 		IPubData* pGuildData = pPubSpace->GetPubData(guildDataName.c_str());
// 
// 		if (pGuildData == NULL){
// 			continue;
// 		}
// 		std::wstring capName = pGuildList->QueryWideStr(i, GUILD_LIST_REC_COL_CAPTAIN);
// 		if (StringUtil::CharIsNull(capName.c_str())){
// 			continue;
// 		}
// 
// 		IRecord *memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
// 		if (memberRecord == NULL)
// 		{
// 			continue;
// 		}
// 
// 		int row = memberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, capName.c_str());
// 		if (row < 0){
// 			continue;
// 		}
// 		int onLineState = memberRecord->QueryInt(row, GUILD_MEMBER_REC_COL_ONLINE);
// 		if (onLineState == 1) //在线
// 		{
// 			continue;
// 		}
// 		int64_t lastOnLineTiem = memberRecord->QueryInt64(row, GUILD_MEMBER_REC_COL_OUTLINE_DATE);
// 		if (timeNow - lastOnLineTiem > m_autoImpeachMaxTime)
// 		{
// 			AutoImpeach(pPubKernel, space_name, wstrGuildName.c_str(), capName.c_str());
// 		}
// 
// 	}
// 
// 
// }


void GuildBaseManager::OnZeroClock(IPubKernel* pPubKernel)
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
			
			// 重置传功次数
			if (guildData == NULL) continue;
				
			IRecord *pRec = guildData->GetRecord(GUILD_MEMBER_REC);
			if (NULL == pRec)
			{
				::extend_warning_pub(LOG_ERROR, "[GuildBaseManager::OnZeroClock] not found guild member record");
				continue;
			}

			int nRows_ = pRec->GetRows();
			for (int k = 0; k < nRows_; ++k)
			{
				pRec->SetInt(k, GUILD_MEMBER_REC_COL_TEACH_NUM, 0);
				pRec->SetInt(k, GUILD_MEMBER_REC_COL_BE_TEACH_NUM, 0);
			}
		}
	}
}