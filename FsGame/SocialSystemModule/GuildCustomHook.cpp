//--------------------------------------------------------------------
// 文件名:		GuildCustomHook.cpp
// 内  容:		公会系统-处理客户端消息
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include <time.h>
#include "GuildModule.h"
#include "../Define/GuildMsgDefine.h"
#include "../Define/PubDefine.h"
#include "../Define/ServerCustomDefine.h"
#include "../Define/GameDefine.h"
#include "../Define/CommandDefine.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/CommonModule/VirtualRecModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/Define/StaticDataDefine.h"
#include <algorithm>
#include "Server/LoopCheck.h"

#include "FsGame/CommonModule/EnvirValueModule.h"
#include "utils/EnvirDefine.h"
#include "CommonModule/LogModule.h"
#include "FriendModule.h"
#include "SkillModule/SkillUpgradeModule.h"
#include "SkillModule/SkillToSprite.h"
#include "Define/Skilldefine.h"
#include "Define/Fields.h"
#include "CommonModule/FunctionEventModule.h"
#include "ItemModule/ToolItem/ToolItemModule.h"
#include "SystemFunctionModule/RewardModule.h"
#include "Define/SnsDefine.h"
#include "CommonModule/ActionMutex.h"
#include "Define/SystemInfo_Item.h"
#include "SystemFunctionModule/ActivateFunctionModule.h"

// 发送帮会成员列表，一次发送成员个数
const int SEND_MEMBER_COUNT = 10;

// 客户端消息处理
int GuildModule::OnCustomMessage(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	if (m_pGuildModule == NULL)
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int subMsg = args.IntVal(1);
	switch (subMsg)
	{
		case CS_GUILD_CREATE:// 创建公会
		{
			m_pGuildModule->OnCustomCreateGuild(pKernel, self, args);
		}
		break;
		case CS_GUILD_GUILD_LIST:// 请求公会列表
		{
			m_pGuildModule->OnCustomGetGuildList(pKernel, self, args);
		}
		break;
		case CS_GUILD_SEARCH:// 搜索公会
		{
			m_pGuildModule->OnCustomFindGuild(pKernel, self, args);
		}
		break;
		case CS_GUILD_APPLY_JOIN:// 请求加入公会
		{
			if (m_pGuildModule->OnCustomApplyJoinGuild(pKernel, self, args) == 0)
			{
				// 通知申请失败
				CVarList msg;
				msg << 0
					<< 0
					<< args.WideStrVal(2)
					<< GUILD_MSG_REQ_FAIL;
				m_pGuildModule->OnCommandApplyJoinGuildResult(pKernel, self, msg);
			}
		}
		break;
		case CS_GUILD_ONE_KEY_APPLY_JOIN://一键申请加入公会
		{
			m_pGuildModule->OnCustomOneKeyApplyJoinGuild(pKernel, self);


		}break;
		case CS_GUILD_CANCEL_APPLY_JOIN:// 请求取消加入公会申请
		{
			m_pGuildModule->OnCustomCancelApplyJoinGuild(pKernel, self, args);
		}
		break;
		case CS_GUILD_ACCEPT_APPLY:// 接受加入公会的申请
		{
			// 校验公会活动期间是否允许操作
			/*if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}
*/
			if (m_pGuildModule->OnCustomAcceptJoinGuild(pKernel, self, args) == 0)
			{
				CVarList msg;
				msg << 0
					<< 0
					<< GUILD_MSG_REQ_FAIL
					<< pSelfObj->QueryWideStr("GuildName")
					<< 0
					<< args.WideStrVal(2);

				m_pGuildModule->OnCommandAcceptResult(pKernel, self, msg);
			}
		}
		break;
		case CS_GUILD_REFUSE_APPLY:// 拒绝加入公会的申请
		{
			if (m_pGuildModule->OnCustomRefuseJoinGuild(pKernel, self, args) == 0)
			{
				CVarList msg;
				msg << 0
					<< 0
					<< GUILD_MSG_REQ_FAIL
					<< args.WideStrVal(2);

				m_pGuildModule->OnCommandRefuseJoin(pKernel, self, msg);
			}
		}
		break;
		case CS_GUILD_GUILD_INFO:// 请求公会信息
		{
			m_pGuildModule->OnCustomGetGuildInfo(pKernel, self);
		}
		break;
		case CS_GUILD_MEMBER_LIST:// 请求成员列表
		{
			m_pGuildModule->OnCustomGetGuildMemberList(pKernel, self, args);
		}
		break;
		case CS_GUILD_APPLY_LIST:// 请求申请者列表
		{
			m_pGuildModule->OnCustomGetGuildApplyList(pKernel, self, args);
		}
		break;
		case CS_GUILD_NOTICE:// 请求修改公告
		{
			m_pGuildModule->OnCustomChangeNotice(pKernel, self, args);
		}
		break;
		case CS_GUILD_DECLARATION:// 请求修改宣言
		{
			m_pGuildModule->OnCustomChangeDeclaration(pKernel, self, args);
		}
		break;
		case CS_GUILD_QUIT:// 请求退出公会
		{
			// 校验公会活动期间是否允许操作
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			//弹劾中不可执行该操作
// 			if (GuildImpeach::IsImpeachIng(pKernel, self) && pSelfObj->QueryInt("GuildPosition") == GUILD_POSITION_CAPTAIN)
// 			{
// 				CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_48, CVarList());
// 				return 0;
// 			}

			m_pGuildModule->OnCustomQuit(pKernel, self, args);
		}
		break;
		case CS_GUILD_FIRE:// 请求踢出公会
		{
			//// 校验公会活动期间是否允许操作
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			//弹劾中不可执行该操作
// 			if (GuildImpeach::IsImpeachIng(pKernel, self))
// 			{
// 				CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_48, CVarList());
// 				return 0;
// 			}

			m_pGuildModule->OnCustomFire(pKernel, self, args);
		}
		break;
		case CS_GUILD_DISMISS:// 请求解散公会
		{
			// 校验公会活动期间是否允许操作
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			//弹劾中不可执行该操作
// 			if (GuildImpeach::IsImpeachIng(pKernel, self))
// 			{
// 				CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_48, CVarList());
// 				return 0;
// 			}

			m_pGuildModule->OnCustomDismiss(pKernel, self, args);
		}
		break;
		case CS_GUILD_DONATE: //捐献
		{
			m_pGuildModule->OnCustomDonate(pKernel, self, args);
		}
		break;
		case CS_GUILD_PROMOTION: //升职
		{
			// 校验公会活动期间是否允许操作
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			m_pGuildModule->OnCustomPromotion(pKernel, self, args, CS_GUILD_PROMOTION);
		}
		break;
		case CS_GUILD_DEMOTION: //降职
		{
			// 校验公会活动期间是否允许操作
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			m_pGuildModule->OnCustomPromotion(pKernel, self, args, CS_GUILD_DEMOTION);
		}
		break;
		case CS_GUILD_SHOP_LIST:// 请求公会商店列表
		{
			m_pGuildModule->OnCustomGetShopList(pKernel, self, args);
		}
		break;
		case CS_GUILD_BUY_ITEM:// 请求购买商店商品
		{
			m_pGuildModule->OnCustomBuyItem(pKernel, self, args);
		}
		break;
		case CS_GUILD_CAPTAIN_TRANSFER: //转移帮主
		{
			//弹劾中不可执行该操作
// 			if (GuildImpeach::IsImpeachIng(pKernel, self))
// 			{
// 				CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_48, CVarList());
// 				return 0;
// 			}
			if (!m_pGuildModule->CheckGuildActivity(pKernel, self))
			{
				return 0;
			}

			m_pGuildModule->OnCustomCaptainTransfer(pKernel, self, args);
		}
		break;
		case CS_GUILD_JOIN_CD:// 请求加入公会CD时间
		{
			m_pGuildModule->OnCustomGetJoinCD(pKernel, self, args);
		}
		break;
		case CS_GUILD_LOG: // 请求帮会日志
		{
			m_pGuildModule->OnCustomGetLog(pKernel, self, args);
		}
		break;
		case CS_GUILD_LOG_DONATE: // 请求帮会捐献日志
		{
			m_pGuildModule->OnCustomGetDonateLog(pKernel, self, args);
		}
		break;

		case CS_GUILD_AUTO_AGREE://自动同意加入组织
		{
			m_pGuildModule->OnCustomSetAutoAgree(pKernel, self, args);
		}break;
		case CS_GUILD_UNLOCK_SKILL://解锁公会技能
		{
			m_pGuildModule->OnCustomUnLockSkill(pKernel, self, args);
		}break;
		case CS_GUILD_GET_SKILL_LIST://获取公会技能列表
		{
			IGameObj *pSelfObj = pKernel->GetGameObj(self);
			if (pSelfObj == NULL){
				break;
			}

			const wchar_t * guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
			if (StringUtil::CharIsNull(guildName)){
				break;
			}

			CVarList msg;
			msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_SKILL_LIST;
			m_pGuildModule->OnCustomGetSkillList(pKernel, guildName, msg);
			pKernel->Custom(self, msg);
		}break;
		case CS_GUILD_SKILL_LEVEL_UP://技能升级
		{
			m_pGuildModule->OnCustomSkillLevelUp(pKernel, self, args);
		}break;
		
		case CS_GUILD_BUILDING_OPERATE_LV:
		{
			m_pGuildModule->OnGuildBuildingOperateLv(pKernel, self, args);
		}break;
		case CS_GUILD_NUM_BUY:
		{
			m_pGuildModule->OnGuildNumBuy(pKernel, self, args);
		}break;
		case CS_GUILD_CHANGE_SHORT_NAME:
		{
			m_pGuildModule->OnCustomChangeGuildShortName(pKernel, self, args);
		}break;
		case CS_GUILD_START_OPERATE_SKILL:
		{
			m_pGuildModule->OnCustomGuildSkillLvOperate(pKernel, self, args);
		}break;
		case CS_GUILD_GET_JIAN_KANG_DU_GIF:
		{
			m_pGuildModule->OnCustomGetJianKangDuGif(pKernel, self);

		}break;
		case CS_GUILD_GET_GUILD_NUM:
		{
			const wchar_t*guildName = args.WideStrVal(2);
			m_pGuildModule->OnCustomGuildNumInfo(pKernel, self, guildName);
		}break;
		case CS_GUILD_GET_GUILD_SYSMBOL:
		{
			const wchar_t*guildName = args.WideStrVal(2);
			m_pGuildModule->OnCustomGuildSysmbol(pKernel, self, guildName);
		}break;
		case CS_GUILD_GET_GUILD_BUILDING:
		{
			m_pGuildModule->OnCustomGuildBuilding(pKernel, self);
		}break;
		case CS_GUILD_GET_GUILD_SET_INFO:
		{
			m_pGuildModule->OnCustomGuildSetInfo(pKernel,self);
		}break;
		case CS_GUILD_USE_RARE_TREASURE:{
			int rareTreasureType = args.IntVal(2);
			int useNum = args.IntVal(3);
			m_pGuildModule->OnCustomUseRareTreasure(pKernel, self, rareTreasureType, useNum);
		}break;
		case CS_GUILD_GET_RARE_TREASURE_LIST:{
			m_pGuildModule->OnCustomGetRareTreasureList(pKernel, self);
		}break;
		case CS_GUILD_GET_NUM_BUY_RECORD:{
			m_pGuildModule->OnCustomGetNumBuyRecord(pKernel, self);
		}break;
		case CS_GUILD_GET_BONUS_INFO:{

			m_pGuildModule->OnCustomGetBonusInfo(pKernel,self);
		}break;
		case CS_GUILD_GET_WELFARE_INFO:{
			m_pGuildModule->OnCustomGetGuildWelfare(pKernel, self);
		}break;
		case CS_GUILD_GIVE_OUT_BONUS:{

			m_pGuildModule->OnCustomGiveOutBonus(pKernel, self,args);
		}break;
		case CS_GUILD_INVITE_JOIN_GUILD:{
			m_pGuildModule->OnCustomInviteJoinGuild(pKernel, self, args);
		}break;
		case CS_GUILD_INVITE_JOIN_GUILD_RSP:{
			m_pGuildModule->OnCustomAgreeInviteJoinGuild(pKernel, self, args);
		}break;
		case CS_GUILD_ADD_PETITION:{
			m_pGuildModule->OnCustomAddPetition(pKernel, self, args);
		}break;
		case CS_GUILD_GET_PETITION_LIST:
		{
			m_pGuildModule->OnCustomGetPetition(pKernel, self, args);
		}break;
		case CS_GUILD_GET_OTHER_GUILD_INFO:
		{
			m_pGuildModule->OnCustomGetOtherGuildInfo(pKernel,self,args);
		}break;
// 		case CS_GUILD_TEACH_LIST:
// 		{
// 			// 获取在线可传功列表
// 			m_pGuildModule->OnCustomTeachList(pKernel, self, args, 2);
// 		}break;
// 		case CS_GUILD_TEACH_REQ:
// 		{			
// 			// 请求对方传功 args: nick[widestr] type[1:req 2:push]
// 			m_pGuildModule->OnCustomTeachReq(pKernel, self, args, 2);
// 		}
// 		break;
// 		case CS_GUILD_TEACH_ACK:
// 		{
// 			// 回应给对方请求 args: nick[widestr] type[1:req 2:push] agree[1: 同意 0: 拒绝]
// 			m_pGuildModule->OnCustomTeachAck(pKernel, self, args, 2);
// 		}break;
// 		case CS_GUILD_TEACH_READY:
// 		{
// 			// 客户端告诉服务器准备好了 args: none
// 			m_pGuildModule->OnCustomTeachReady(pKernel, self, args, 2);
// 		}
	default:
		break;
	}

	return 0;
}

// 处理创建公会的请求
int GuildModule::OnCustomCreateGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// obtain parameters
	const wchar_t* guildName = args.WideStrVal(2);
    int guildIndentifying = args.IntVal(3);
	const wchar_t* guildShortName = args.WideStrVal(4);
	

	// check condifion
	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (pPlayer == NULL)
	{
		return 0;
	}
	if (StringUtil::CharIsNull(guildShortName)){
		return 0;
	}


	//公会创建功能开启与否
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_CREATE, self))
	{
		return 0;
	}

	int64_t quitGuildDate = pPlayer->QueryInt64("QuitGuildDate");
	if (quitGuildDate > 0)
	{
		int passMinutes = GetPassMinutes(quitGuildDate);
		if (passMinutes < EnvirValueModule::EnvirQueryInt(ENV_VALUE_ADD_GUILD_CD))
		{
			// 处于冷却时间，无法创建公会,包括退出公会的情况
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17623, CVarList());
			return 0;
		}
	}

	if (!CheckCanCreateGuild(pKernel, self, guildName))
	{
		return 0;
	}
	if (wcslen(guildShortName)>1){
		return 0;
	}

	if (GuildShortNameExist(pKernel, guildShortName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17618, CVarList());
		return 0;
	}
	


	const wchar_t* playerName = pPlayer->QueryWideStr("Name");
	const char* pPlayerUid = pKernel->SeekRoleUid(playerName);
	int sex = pPlayer->QueryInt("Sex");
	int career = pPlayer->QueryInt("Job");
	int ability = pPlayer->QueryInt("BattleAbility");
	int nation = pPlayer->QueryInt("Nation");
	int vipLv = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

	// 减铜币，如果创建失败的话在结果处理函数中再加钱
	int nCostGold = EnvirValueModule::EnvirQueryInt(ENV_VALUE_CREATE_GUILD_GOLD);
	int rc = m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_GOLD, (__int64)nCostGold, FUNCTION_EVENT_ID_GUILD_CREATE);
	if (rc != DC_SUCCESS)
	{
		return 0;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD << guildName << SP_GUILD_MSG_CREATE
		<< pPlayerUid << playerName << sex << pPlayer->QueryInt("Level") 
		<< career << ability << nation 
		<< guildIndentifying << guildShortName << vipLv;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 处理获取公会列表请求, 每次取REQUEST_GUILD_LIST_PRE_COUNT条公会记录
int GuildModule::OnCustomGetGuildList(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//翻页数
	int index = args.IntVal(2);
	if (index < 0)
	{
		return 0;
	}

	if (index == 0)
	{
		pSelfObj->SetInt("GuildListBeginRow", 0);
	}

	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (pPlayer == NULL)
	{
		return 0;
	}

	// 检测玩家等级是否符合限制
	if (ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_GUILD_FUNCTION))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17604, CVarList());
		return false;
	}

	IRecord* pGuildListRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	IRecord* pGuildSortRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SORT_REC);
	if (pGuildListRecord == NULL || pGuildSortRecord == NULL)
	{
		return 0;
	}

	int rowTotal = pGuildSortRecord->GetRows();
	int rowStart = pSelfObj->QueryInt("GuildListBeginRow");
	if (rowStart >= rowTotal)
	{
		m_pVirtualRecModule->SendVirtualRecord(pKernel, self, GUILD_LIST_VIRTUALREC_NAME, CVarList());
		return 0;
	}

	CVarList msg;
	int getRowCount = 1;    // 记录取了几行公会记录
	int rankStart = index * REQUEST_GUILD_LIST_PRE_COUNT;
	int nation = pSelfObj->QueryInt("Nation");
	// 循环保护
	LoopBeginCheck(dh);
	for (int i = rowStart; i < rowTotal; i++)
	{
		LoopDoCheck(dh);
		if (getRowCount > REQUEST_GUILD_LIST_PRE_COUNT)
		{
			pSelfObj->SetInt("GuildListBeginRow", i);
			break;
		}

		const wchar_t *guildName = pGuildSortRecord->QueryWideStr(i, GUILD_SORT_REC_COL_NAME);
		if (StringUtil::CharIsNull(guildName))
		{
			continue;
		}

		SerialGuildInfo(pKernel, self, msg, rankStart+1, guildName);

		getRowCount++;
		rankStart++;
	}
	m_pVirtualRecModule->SendVirtualRecord(pKernel, self, GUILD_LIST_VIRTUALREC_NAME, msg);

	return 0;
}

// 处理搜索公会请求
int GuildModule::OnCustomFindGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	const wchar_t* searchName = args.WideStrVal(2);
	if (!IsWidStrValid(searchName))
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (pPlayer == NULL)
	{
		return 0;
	}

	// 检测玩家等级是否符合限制
	if (ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_GUILD_FUNCTION))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17604, CVarList());
		return false;
	}

	IRecord* pGuildListRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SORT_REC);
	if (pGuildListRecord == NULL)
	{
		return 0;
	}

	int iGuildListRow = pGuildListRecord->GetRows();
	if (iGuildListRow <= 0)
	{
		return 0;
	}

	typedef std::vector<std::pair<int,std::wstring> > GuildNameVec;
	typedef GuildNameVec::iterator GuildNameVecIter;

	int rankVal = 0;
	int guildCount = 0;
	GuildNameVec guildNameVector;

	// 循环保护
	LoopBeginCheck(di);
	for (int i = 0; i < iGuildListRow; i++)
	{
		LoopDoCheck(di);
		rankVal++;

		const wchar_t* pGuildName = pGuildListRecord->QueryWideStr(i, GUILD_SORT_REC_COL_NAME);
		if(!::wcsstr(pGuildName, searchName))
		{
			continue;
		}

		guildNameVector.push_back(std::make_pair(rankVal, std::wstring(pGuildName)));
		guildCount++;
		if (guildCount >= REQUEST_GUILD_LIST_PRE_COUNT)
		{
			break;
		}
	}

	int number = 0;
	CVarList varGuild;
	// 循环保护
	LoopBeginCheck(dj);
	for(GuildNameVecIter iter = guildNameVector.begin(); iter != guildNameVector.end() ; ++iter)
	{
		LoopDoCheck(dj);
		if (SerialGuildInfo(pKernel, self, varGuild, iter->first, iter->second.c_str()))
		{
			number++;
		}
	}

	CVarList rmsg;
	rmsg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_SEARCH_RESULT << number << varGuild;
	pKernel->Custom(self, rmsg);

	return 0;
}

// 处理请求表格数据
int GuildModule::OnCustomGetTableData(IKernel* pKernel, const PERSISTID& self, int msgType)
{
	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD;

	std::string tableName;
	switch(msgType)
	{
	case CS_GUILD_MEMBER_LIST:
		{
			tableName = GUILD_MEMBER_REC;
			msg << SC_GUILD_MEMBER_LIST;
		}
		break;
	case CS_GUILD_APPLY_LIST:
		{
			tableName = GUILD_JOIN_REC;
			msg << SC_GUILD_APPLY_LIST;
		}
		break;
	default:
		return 0;
	}

	IPubData* pGuildPubData = GetPubData(pKernel, GUILD_PUB_DATA);
	if (pGuildPubData == NULL)
	{
		return 0;
	}

	IRecord* pRecord = pGuildPubData->GetRecord(tableName.c_str());
	if (pRecord == NULL)
	{
		// 公共数据区未查到表，在公会数据区查询
		const wchar_t* guildName = GetPlayerGuildName(pKernel, self);
		IPubData* pGuildData = GetPubData(pKernel, guildName);
		if (pGuildData == NULL)
		{
			return 0;
		}

		pRecord = pGuildData->GetRecord(tableName.c_str());
		if (pRecord == NULL)
		{
			return 0;
		}
	}

	int rows = pRecord->GetRows();
	// 循环保护
	LoopBeginCheck(dk);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(dk);
		CVarList rowValue;
		pRecord->QueryRowValue(i, rowValue);      
		msg << rowValue;
	}

	pKernel->Custom(self, msg);

	return 0;
}

// 处理申请加入公会的请求
int GuildModule::OnCustomApplyJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	const wchar_t* guildName = args.WideStrVal(2);
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}

	IGameObj * pPlayer = pKernel->GetGameObj(self); 
	if(pPlayer == NULL)
	{
		return 0;
	}

	//申请加入帮会开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_JOIN, self))
	{
		return 0;
	}

	const wchar_t* playerName = pPlayer->QueryWideStr("Name");
	const wchar_t* selfGuildName = pPlayer->QueryWideStr("GuildName");
	if (IsWidStrValid(selfGuildName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17605, CVarList());
		return 0;
	}

	// 检查是否处于CD中
	int64_t quitGuildDate = pPlayer->QueryInt64("QuitGuildDate");
	if (quitGuildDate > 0)
	{
		int passMinutes = GetPassMinutes(quitGuildDate);
		if (passMinutes < EnvirValueModule::EnvirQueryInt(ENV_VALUE_ADD_GUILD_CD))
		{
			// 处于冷却时间，无法申请加入公会
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17623, CVarList());
			return 0;
		}
	}

	// 检测玩家等级是否符合限制
	if (ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_GUILD_FUNCTION))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17604, CVarList());
		return 0;
	}

	// 查找申请者列表是否有记录
	IRecord* applyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_APPLY_REC);
	if (applyRecord == NULL)
	{
		return 0;
	}
	int row = applyRecord->FindWideStr(GUILD_APPLY_REC_COL_NAME, playerName);
	if (row >= 0)
	{
		// 该玩家存在申请记录
		std::wstring applyGuildString = applyRecord->QueryWideStr(row, GUILD_APPLY_REC_COL_GUILD);
		CVarList applyGuildList;
		::util_split_wstring(applyGuildList, applyGuildString, L";");

		// 达到申请数限制
		int guildCount = (int)applyGuildList.GetCount();
		if (guildCount >= GUILD_APPLY_MAX_NUM)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17619, CVarList());
			return 0;
		}

		// 循环保护
		LoopBeginCheck(dl);
		for (int i = 0; i < guildCount; ++i)
		{
			LoopDoCheck(dl);
			// 已经申请过该公会
			if (wcscmp(guildName, applyGuildList.WideStrVal(i)) == 0)
			{
				CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17620, CVarList());
				return 0;
			}
		}
	}

	// 检查公会的申请列表是否已满
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}
	IRecord* joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (joinRecord == NULL)
	{
		return 0;
	}
	if (joinRecord->GetRows() >= GUILD_JOIN_REC_ROW_MAX)
	{
		//该公会申请列表已满
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17621, CVarList());
		return 0;
	}

	// 取公会等级
	IRecord* guildListRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC); 
	if (guildListRecord == NULL)
	{
		return 0;
	}
	row = guildListRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0)
	{
		return 0;
	}
	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);

	// 根据公会等级获取公会最大成员数量
	GuildUpLevelConfig_t *upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
	if (upLevelConfig == NULL)
	{
		return 0;
	}
	int maxMember = upLevelConfig->m_MaxMember;

	// 取公会当前人员数量
	IRecord* memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberRecord == NULL)
	{
		return 0;
	}
	int curMember  = memberRecord->GetRows();

	// 判断公会是否满员
	if (curMember >= maxMember)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17622, CVarList());
		return 0;
	}

	IRecord* pGuildListRec = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (pGuildListRec == NULL)
	{
		return 0;
	}
	int guildListRow = pGuildListRec->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildListRow < 0)
	{
		return 0;
	}

	const char* pPlayerUid = pKernel->SeekRoleUid(playerName);
	int playerSex   = pPlayer->QueryInt("Sex");
	int playerLevel = pPlayer->QueryInt("Level");
	int ability = pPlayer->QueryInt("BattleAbility");
	int career = pPlayer->QueryInt("Job");
	int vipLevel = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);
	CVarList msg;
	msg << PUBSPACE_GUILD << guildName << SP_GUILD_MSG_APPLY_JOIN << pPlayerUid << playerName 
		<< playerSex << playerLevel << ability << career << vipLevel;
	pKernel->SendPublicMessage(msg);

	return 1;
}

int GuildModule::OnCustomOneKeyApplyJoinGuild(IKernel* pKernel, const PERSISTID& self)
{

	IGameObj * pPlayer = pKernel->GetGameObj(self);
	if (pPlayer == NULL)
	{
		return 0;
	}
	//申请加入帮会开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_JOIN, self))
	{
		return 0;
	}

	// 检查是否处于CD中
	int64_t quitGuildDate = pPlayer->QueryInt64("QuitGuildDate");
	if (quitGuildDate > 0)
	{
		int passMinutes = GetPassMinutes(quitGuildDate);
		if (passMinutes < EnvirValueModule::EnvirQueryInt(ENV_VALUE_ADD_GUILD_CD))
		{
			// 处于冷却时间，无法申请加入公会
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17623, CVarList());
			return 0;
		}
	}

	int64_t OneKeyJoinTime = pPlayer->QueryInt64("OneKeyJoinTime");
	if (OneKeyJoinTime >0)
	{
		int passMinutes = GetPassMinutes(OneKeyJoinTime);
		if (passMinutes < GUILD_ONE_KEY_JOIN_CD)
		{
			// 处于冷却时间，无法申请加入公会
			return 0;
		}
		
	}
	pPlayer->SetInt64("OneKeyJoinTime", (int64_t)::time(NULL));
	const wchar_t* playerName = pPlayer->QueryWideStr("Name");
	const wchar_t* selfGuildName = pPlayer->QueryWideStr("GuildName");
	if (IsWidStrValid(selfGuildName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17605, CVarList());
		return 0;
	}
	// 检测玩家等级是否符合限制
	if (ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_GUILD_FUNCTION))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17604, CVarList());
		return false;
	}
	
	// 查找申请者列表是否有记录
	IRecord* applyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_APPLY_REC);
	if (applyRecord == NULL)
	{
		return 0;
	}

	int sendApplyCountMax = 10;
	int applayRow = applyRecord->FindWideStr(GUILD_APPLY_REC_COL_NAME, playerName);
	std::wstring applyGuildString;
	if (applayRow >= 0)
	{
		// 该玩家存在申请记录
		applyGuildString = applyRecord->QueryWideStr(applayRow, GUILD_APPLY_REC_COL_GUILD);
		CVarList applyGuildList;
		::util_split_wstring(applyGuildList, applyGuildString, L";");

		// 达到申请数限制
		int guildCount = (int)applyGuildList.GetCount();
		if (guildCount >= GUILD_APPLY_MAX_NUM)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17619, CVarList());
			return 0;
		}
		else
		{
			sendApplyCountMax = GUILD_APPLY_MAX_NUM - guildCount;
		}
	}

	IRecord* pGuildSortRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SORT_REC);
	if ( pGuildSortRecord == NULL)
	{
		return 0;
	}

	int rowTotal = pGuildSortRecord->GetRows();

	if (rowTotal <= 0)
	{
		return 0;
	}
	
	const char* pPlayerUid = pKernel->SeekRoleUid(playerName);
	int playerSex = pPlayer->QueryInt("Sex");
	int playerLevel = pPlayer->QueryInt("Level");
	int ability = pPlayer->QueryInt("BattleAbility");
	int career = pPlayer->QueryInt("Job");
	int vipLv = pPlayer->QueryInt(FIELD_PROP_VIP_LEVEL);

	IRecord* pGuildListRec = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (pGuildListRec == NULL)
	{
		return 0;
	}

	// 取公会等级
	IRecord* guildListRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (guildListRecord == NULL)
	{
		return 0;
	}
	//发送申请的数量
	int sendApplyCount = 0;

	CVarList guildList;

	// 循环保护
	LoopBeginCheck(dh);
	for (int i = 0; i < rowTotal; i++)
	{
		LoopDoCheck(dh);
		const wchar_t *guildName = pGuildSortRecord->QueryWideStr(i, GUILD_SORT_REC_COL_NAME);
		if (StringUtil::CharIsNull(guildName))
		{
			continue;
		}

		if (applyGuildString.find(guildName) != std::string::npos)
		{
			continue;
		}
		

		// 检查公会的申请列表是否已满
		IPubData* pGuildData = GetPubData(pKernel, guildName);
		if (pGuildData == NULL)
		{
			continue;
		}
		IRecord* joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
		if (joinRecord == NULL)
		{
			continue;
		}
		if (joinRecord->GetRows() >= GUILD_JOIN_REC_ROW_MAX)
		{
			continue;
		}

		int row = guildListRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
		if (row < 0)
		{
			continue;
		}
		
		int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
		// 根据公会等级获取公会最大成员数量
		GuildUpLevelConfig_t *upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
		if (upLevelConfig == NULL)
		{
			continue;
		}
		int maxMember = upLevelConfig->m_MaxMember;

		// 取公会当前人员数量
		IRecord* memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
		if (memberRecord == NULL)
		{
			continue;
		}
		int curMember = memberRecord->GetRows();

		// 判断公会是否满员
		if (curMember >= maxMember)
		{
			continue;
		}

		int guildListRow = pGuildListRec->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
		if (guildListRow < 0)
		{
			continue;
		}

		if (sendApplyCount >= sendApplyCountMax)
		{
			break;
		}

		guildList << guildName;

		sendApplyCount++;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD <<"" << SP_GUILD_START_ONE_KEY_APPLY_JOIN << pPlayerUid << playerName
		<< playerSex << playerLevel << ability << career << vipLv<< guildList.GetCount() << guildList;
	pKernel->SendPublicMessage(msg);
	return 0;
}




// 处理取消申请加入公会的请求
int GuildModule::OnCustomCancelApplyJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* guildName = args.WideStrVal(2);
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}

	IRecord* applyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_APPLY_REC);
	if (applyRecord == NULL)
	{
		return 0;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* playerUid = pKernel->SeekRoleUid(playerName);
	int row = applyRecord->FindString(GUILD_APPLY_REC_COL_NAME_UID, playerUid);
	if (row < 0)
	{
		// 已经不存在申请记录 直接通知客户端取消成功
		m_pGuildModule->OnCommandCancelApplyJoinGuildResult(pKernel, self, CVarList() << 0 << 0 << guildName << GUILD_MSG_REQ_SUCC);
		return 0;
	}

	std::wstring applyGuildString = applyRecord->QueryWideStr(row, GUILD_APPLY_REC_COL_GUILD);
	CVarList applyGuildList;
	::util_split_wstring(applyGuildList, applyGuildString, L";");
	int guildCount = (int)applyGuildList.GetCount();

	// 循环保护
	LoopBeginCheck(dm);
	for (int i = 0; i < guildCount; ++i)
	{
		LoopDoCheck(dm);
		if (wcscmp(guildName, applyGuildList.WideStrVal(i)) == 0)
		{
			CVarList msg;
			msg << PUBSPACE_GUILD << guildName << SP_GUILD_MSG_CANCEL_APPLY_JOIN << playerUid << playerName;
			pKernel->SendPublicMessage(msg);

			break;
		}
	}

	return 0;
}

// 处理获取公会信息的请求
int GuildModule::OnCustomGetGuildInfo(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 只能获取自己的公会信息
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}

	// 检测玩家等级是否符合限制
	if (ActivateFunctionModule::CheckActivateFunction(pKernel, self, AFM_GUILD_FUNCTION))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17604, CVarList());
		return false;
	}

	// 获取公共空间
	IPubData* pPubData = GetPubData(pKernel, GUILD_PUB_DATA);
	if (pPubData == NULL)
	{
		return 0;
	}

	// 获取公会列表
	IRecord* guildListRecord = pPubData->GetRecord(GUILD_LIST_REC);
	if (guildListRecord == NULL)
	{
		return 0;
	}

	// 查找玩家所在公会
	int row = guildListRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0)
	{
		// 玩家身上的公会记录已经不存在
		pSelfObj->SetWideStr("GuildName", L"");
		pSelfObj->SetInt("GuildPosition", GUILD_POSITION_NONE);
		//LogModule::m_pLogModule->OnGuildQuit(pKernel, self);
		return 0;
	}

	// 获取排名表
	IRecord* pSortRecord = pPubData->GetRecord(GUILD_SORT_REC);
	if (pSortRecord == NULL)
	{
		return 0;
	}

	// 查找公会排名
	int rank = 0;
	int tempNationId = 0;
	std::wstring tempName = L"";
	int sortCount = pSortRecord->GetRows();
	// 循环保护
	LoopBeginCheck(dn);
	for (int i = 0; i < sortCount; ++i)
	{
		LoopDoCheck(dn);
		++rank;
		tempName = pSortRecord->QueryWideStr(i, GUILD_SORT_REC_COL_NAME);
		if (tempName == guildName)
		{
			break;
		}
	}

	// 获取帮会公共空间
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 获取成员列表
	IRecord* memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberRecord == NULL)
	{
		return 0;
	}

	const wchar_t *playerName = pSelfObj->QueryWideStr("Name");
	int memberRow = memberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
	if (memberRow < 0)
	{
		return 0;
	}

	IRecord* joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (joinRecord == NULL)
	{
		return 0;
	}

	const wchar_t *captainName = guildListRecord->QueryWideStr(row, GUILD_LIST_REC_COL_CAPTAIN);
	int guildJuYiTingLevel = GetGuildLevel(pKernel, guildName);
	int curMember  = memberRecord->GetRows();
	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	GuildUpLevelConfig_t *upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
	if (upLevelConfig == NULL)
	{
		return 0;
	}
	int maxMember   = upLevelConfig->m_MaxMember;
	const wchar_t* notice = guildListRecord->QueryWideStr(row, GUILD_LIST_REC_COL_ANNOUNCEMENT);
	// 帮会宣言
	const wchar_t* decal = guildListRecord->QueryWideStr(row, GUILD_LIST_REC_COL_DECLARATION);
	int maxContrib  = memberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_DEVOTE);
	int applyNumber = joinRecord->GetRows();
	int selfPosition= memberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);
	int ability = guildListRecord->QueryInt(row, GUILD_LIST_REC_COL_FIGHT_ABILITY);
	int guildCapital = GetGuildNumValue(pKernel,guildName,GUILD_NUM_TYPE::GUILD_NUM_CAPITAL);
	int devoteSilver = memberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_DEVOTE_SILVER);
	int64_t xiBingFuTime = pGuildData->QueryAttrInt64(STOPPING_WAR_TIME);

	// 获取当天踢人数
	IPubData* pGuildPubdata_ = GetPubData(pKernel, guildName);
	int nFireNum_ = 0;
	if (NULL != pGuildPubdata_)
	{
		nFireNum_ = pGuildPubdata_->QueryAttrInt("FireNum");
	}

	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_GUILD_INFO << guildName << captainName << guildJuYiTingLevel
		<< curMember << maxMember << notice << maxContrib
		<< applyNumber << selfPosition << rank << decal << ability << devoteSilver << xiBingFuTime << nFireNum_;
	pKernel->Custom(self, msg);


	OnCustomGuildNumInfo(pKernel,self,guildName);
	OnCustomGuildSysmbol(pKernel,self,guildName);
	OnCustomGuildBuilding(pKernel, self);
	return 0;
}


int GuildModule::OnCustomGuildNumInfo(IKernel*pKernel, const PERSISTID& self, const wchar_t * guildName)
{


	IRecord *pGuildNumRec = GetPubRecord(pKernel, guildName,GUILD_NUM_REC);
	if (pGuildNumRec == NULL){
		return 0;
	}

	CVarList info;

	
	int rowMax = pGuildNumRec->GetRows();

	for (int i = 0; i < rowMax; i++)
	{

		CVarList var;
		pGuildNumRec->QueryRowValue(i, var);
		info << var;
	}

	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_GUILD_NUM_INFO << info;
	pKernel->Custom(self, msg);
	return 0;
}




int GuildModule::OnCustomGuildSysmbol(IKernel*pKernel,const PERSISTID& self, const wchar_t * guildName)
{
	

	IRecord *pGuildSysmbolRec = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SYSMBOL_REC) ;
	if (pGuildSysmbolRec == NULL){
		return 0;
	}
	int row = pGuildSysmbolRec->FindWideStr(GUILD_SYSMBOL_REC_COL_GUILD_NAME, guildName);
	if (row < 0){
		return 0;
	}

	int identifying = pGuildSysmbolRec->QueryInt(row, GUILD_SYSMBOL_REC_COL_IDENTIFYING);
	const wchar_t* shortName = pGuildSysmbolRec->QueryWideStr(row, GUILD_SYSMBOL_REC_COL_SHORT_NAME);
	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_GUILD_SYSMBOL << identifying << shortName;
	pKernel->Custom(self, msg);

	return 0;
}

int GuildModule::OnCustomGuildBuilding(IKernel*pKernel, const PERSISTID& self)
{
		const wchar_t* guildName = pKernel->QueryWideStr(self,FIELD_PROP_GUILD_NAME);
		IRecord *pGuildBuildingRec = GetPubRecord(pKernel, guildName, GUILD_BUILDING_REC); 
		if (pGuildBuildingRec == NULL){
			return 0;
		}
		

		int rowMax = pGuildBuildingRec->GetRows();
		CVarList info;
		for (int i = 0; i < rowMax; i++)
		{

			CVarList var;
			pGuildBuildingRec->QueryRowValue(i, var);
			info << var;
		}


		CVarList msg;
		msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_BUILDING_INFO << info;
		pKernel->Custom(self, msg);
		return 0;
}

int GuildModule::OnCustomGuildSetInfo(IKernel*pKernel, const PERSISTID& self)
{
	const wchar_t* guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);

	IRecord *pGuildSetRec = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SET_REC);
	if (pGuildSetRec == NULL){
		return 0;
	}


	int row = pGuildSetRec->FindWideStr(GUILD_SET_REC_COL_GUILD_NAME,guildName);
	if (row < 0){
		return 0;
	}
	

	CVarList var;
	pGuildSetRec->QueryRowValue(row, var);


	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_SET_INFO << var;
	pKernel->Custom(self, msg);

	return 0;
}




// 处理接受加入公会的请求
int GuildModule::OnCustomAcceptJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* applyName = args.WideStrVal(2);
	if (!IsWidStrValid(applyName))
	{
		return 0;
	}

	const wchar_t *guildName = pSelfObj->QueryWideStr("GuildName");
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}

	if (!CheckCanOptJoinGuild(pKernel, self, applyName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17624, CVarList());
		return 0;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);
	const char* pApplyNameUid = pKernel->SeekRoleUid(applyName);

	// 获得公会等级
	IRecord* pGuildList = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (pGuildList == NULL)
	{
		return 0;
	}
	int guildRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return 0;
	}
	int guildJuYiTingLevel = GetGuildLevel(pKernel, guildName);
	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	// 获得公会最大成员数量
	GuildUpLevelConfig_t* upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
	if (upLevelConfig == NULL)
	{
		return 0;
	}

	// 申请者是否在线 
	int applyOnline = (pKernel->GetPlayerScene(applyName) > 0) ? ONLINE : OFFLINE;

	CVarList msg;
	msg << PUBSPACE_GUILD << guildName 
		<< SP_GUILD_MSG_ACCEPT_JOIN << pApplyNameUid << applyName
		<< playerName << guildJuYiTingLevel << upLevelConfig->m_MaxMember << applyOnline;

	pKernel->SendPublicMessage(msg);


	// 帮会活动进行中则消息通知
// 	if( applyOnline == PLAYER_ONLINE && GuildDefendModule::m_pGuildDefendModule->IsGuildDefendRunning(pKernel, guildName) )
// 	{
// 		pKernel->CustomByName(applyName, CVarList() << SERVER_CUSTOMMSG_GUILD_ACTIVITY 
// 			                                         << SC_GUILD_DEFEND_RUNNING
// 		                                	         << 1);
// 	}


	return 1;
}

// 处理拒绝加入公会的请求
int GuildModule::OnCustomRefuseJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* applyName = args.WideStrVal(2);
	if (!IsWidStrValid(applyName))
	{
		return 0;
	}

	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}

	if (!CheckCanOptJoinGuild(pKernel, self, applyName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17624, CVarList());
		return 0;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");

	CVarList msg;
	msg << PUBSPACE_GUILD << guildName << SP_GUILD_MSG_REFUSE_JOIN << applyName << playerName;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 处理请求成员列表
int GuildModule::OnCustomGetGuildMemberList(IKernel *pKernel, const PERSISTID &self, const IVarList &args)
{
	const wchar_t* guildName = GetPlayerGuildName(pKernel, self);
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}

	IRecord *memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberRecord == NULL)
	{
		return 0;
	}

	CVarList msg;
	CVarList memberArray;
	//int64_t curTime = ::time(NULL);
	int rows = memberRecord->GetRows();
	int curBath = 0; // 当前批次
	// 除0保护
	if (SEND_MEMBER_COUNT == 0)
	{
		return 0;
	}

	IRecord * pOnlinePlayerRec = FriendModule::m_pFriendModule->GetPubOnlineRec(pKernel);
	if (NULL == pOnlinePlayerRec)
	{
		return 0;
	}


	int totalBath = rows % SEND_MEMBER_COUNT != 0 ?
		((rows / SEND_MEMBER_COUNT) + 1) : rows / SEND_MEMBER_COUNT;// 总批次

	// 循环保护
	LoopBeginCheck(do);
	for (int i = 0, j = 0; i < rows;)
	{
		LoopDoCheck(do);
		memberArray.Clear();
		// 循环保护
		LoopBeginCheck(dp);
		for(j = 0; j < SEND_MEMBER_COUNT && i < rows; ++i, ++j)
		{
			LoopDoCheck(dp);



			int guildPosition = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_POSITION);
			const wchar_t *name = memberRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
			int sex     = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_SEX);
			int level   = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_LEVEL);
			int career  = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_CAREER);
			int maxContrib = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_DEVOTE);
			int fight   = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_FIGHT_ABILITY);	// 战斗力
			int online  = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_ONLINE);	// 0不在线，1在线
			int64_t lastOfflineTime = memberRecord->QueryInt64(i, GUILD_MEMBER_REC_COL_OUTLINE_DATE);
			int offlinehours = GetPassMinutes(lastOfflineTime)/60; // 如果不在线，表示离线时间，以小时为单位
			int vipLv = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_VIP_LEVEL);
			int teamID = 0;

			const char * pPlayerUID = memberRecord->QueryString(i, GUILD_MEMBER_REC_COL_NAME_UID);
			int nRowsIndex = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pPlayerUID);
			if (nRowsIndex >= 0)
			{
				teamID = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_TEAM_ID);
			}



			memberArray << guildPosition << name << sex << level << career 
				<< maxContrib << fight << online << offlinehours << teamID << vipLv;
		}

		msg.Clear();
		msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_MEMBER_LIST << totalBath << curBath++ << j;
		pKernel->Custom2(self, msg, memberArray);
	}

	return 0;
}

// 处理请求申请入帮列表
int GuildModule::OnCustomGetGuildApplyList(IKernel *pKernel, const PERSISTID &self, const IVarList &args)
{
	// 检查是否有权限
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_RATIFY))
	{
		return 0;
	}
	const wchar_t* guildName = GetPlayerGuildName(pKernel, self);
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}

	IRecord *joinRecord = pGuildData->GetRecord(GUILD_JOIN_REC);
	if (joinRecord == NULL)
	{
		return 0;
	}

	CVarList applyArray;
	int rows = joinRecord->GetRows();
	// 循环保护
	LoopBeginCheck(dq);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(dq);
		const wchar_t *name = joinRecord->QueryWideStr(i, GUILD_JOIN_REC_COL_PLAYER_NAME);	// 成员名字
		int sex     = joinRecord->QueryInt(i, GUILD_JOIN_REC_COL_SEX);	// 0女，1男
		int level   = joinRecord->QueryInt(i, GUILD_JOIN_REC_COL_LEVEL);
		int career  = joinRecord->QueryInt(i, GUILD_JOIN_REC_COL_CAREER);
		int fight   = joinRecord->QueryInt(i, GUILD_JOIN_REC_COL_FIGHT_ABILITY);	// 战斗力
		int online_ = joinRecord->QueryInt(i, GUILD_JOIN_REC_COL_ONLINE);
		applyArray << name << sex << level << career << fight << online_;
	}

	CVarList msg;
	msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_APPLY_LIST << rows;
	pKernel->Custom2(self, msg, applyArray);

	return 0;
}

// 处理修改公告请求
int GuildModule::OnCustomChangeNotice(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* text = args.WideStrVal(2);

	if (!IsWidStrValid(text) || wcslen(text) > GUILD_NOTICE_MAX_LENGTH)
	{
		return 0;
	}

	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_ANNOUNCEMENT))
	{
		// 无权发布公告
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17625, CVarList());
		return 0;
	}

	// 检测是否包含非法字符
	if (!pKernel->CheckName(text))
	{
		// 提示包含非法字符
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17626, CVarList());
		return 0;
	}




	const wchar_t* playerName  = pSelfObj->QueryWideStr("Name");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);
	const wchar_t* guildName   = pSelfObj->QueryWideStr("GuildName");
	if (GetGuildNumValue(pKernel,guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL) < EnvirValueModule::EnvirQueryInt(ENV_VALUE_CHANGE_ANNOUNCEMENT))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17627, CVarList());
		return 0;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName 
		<< SP_GUILD_MSG_CHANGE_NOTICE 
		<< pPlayerNameUid
		<< playerName
		<< text;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 处理修改宣言
int GuildModule::OnCustomChangeDeclaration(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 获取玩家对象
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 获取宣言
	const wchar_t* text = args.WideStrVal(2);

	// 宣言合法检测
	if (!IsWidStrValid(text) || wcslen(text) > GUILD_DECLARATION_MAX_LENGTH)
	{
		return 0;
	}

	// 发布权限检测
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_ANNOUNCEMENT))
	{
		// 无权发布公告
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17625, CVarList());
		return 0;
	}

	// 检测是否包含非法字符
	if (!pKernel->CheckName(text))
	{
		// 提示包含非法字符
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17628, CVarList());
		return 0;
	}

	// 扣除元宝
// 	int nGold_ = EnvirValueModule::EnvirQueryInt(ENV_VALUE_GUILD_DECLAREATION_MONDIY_COST);
// 	if (nGold_ > 0)
// 	{
// 		if (DC_SUCCESS != 
// 			CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_SILVER, nGold_, FUNCTION_EVENT_ID_GUILD_DECLAR_MONDIFY))
// 		{
// 			// 资金不足
// 			CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_7, CVarList());
// 			return 0;
// 		}
// 	}
// 	else
// 	{
// 		// 未配置直接返回资金不足
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_7, CVarList());
// 		return 0;
// 	}

	const wchar_t* playerName  = pSelfObj->QueryWideStr("Name");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);
	const wchar_t* guildName   = pSelfObj->QueryWideStr("GuildName");

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName 
		<< SP_GUILD_CHANGE_DECLARATION 
		<< pPlayerNameUid
		<< playerName
		<< text;
	pKernel->SendPublicMessage(msg);

	return 0;
}


// 请求退出公会
int GuildModule::OnCustomQuit(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}

	// 请求退出公会功能开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_LEAVE, self))
	{
		return 0;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName
		<< SP_GUILD_MSG_QUIT
		<< pPlayerNameUid
		<< playerName;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求踢出公会
int GuildModule::OnCustomFire(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t* memberName = args.WideStrVal(2);
	if (!CheckCanFire(pKernel, self, memberName))
	{
		return 0;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const wchar_t* guildName  = pSelfObj->QueryWideStr("GuildName");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);
	const char* pMemberNameUid = pKernel->SeekRoleUid(memberName);

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName
		<< SP_GUILD_MSG_FIRE
		<< pPlayerNameUid
		<< playerName
		<< pMemberNameUid
		<< memberName;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求解散公会
int GuildModule::OnCustomDismiss(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (!CheckCanDismiss(pKernel, self))
	{
		return false;
	}

	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* pPlayerNameUid = pKernel->SeekRoleUid(playerName);

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< pSelfObj->QueryWideStr("GuildName") 
		<< SP_GUILD_MSG_DISMISS
		<< pPlayerNameUid
		<< playerName;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求公会捐献
int GuildModule::OnCustomDonate(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int donateValue = args.IntVal(2);

	const wchar_t *guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}


	int donateTime = pSelfObj->QueryInt("GuildDonateTimes");
	if (donateTime > 0){ return 0; }
	const wchar_t *playerName = pSelfObj->QueryWideStr("Name");
	const char* playerUid = pKernel->SeekRoleUid(playerName);
	//请求公会捐献功能开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_DONATE, self))
	{
		return 0;
	}

	// 获得帮会贡献信息

	GuildDonateConfigVector::iterator iter(m_GuildDonateConfig.begin());
	// 循环保护

	GuildDonateConfig_t *config = &(*iter);

	if (config == NULL)
	{
		return 0;
	}
	if (config->m_CapitalNumber > donateValue){
		return 0;
	}

	


	//个人组织资金	
	int addSelfGuildCurrency = (int)(donateValue*config->m_RewardPlayerDonate / (float)config->m_CapitalNumber);
	//组织资金
	int addGuildCapital = (int)(donateValue*config->m_GuildDonate / (float)config->m_CapitalNumber);


// 	if (!m_pCapitalModule->IsDailyCanAddCapital(pKernel, self, CAPITAL_GUILD_CURRENCY))
// 	{
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
// 			GUILD_CUSTOM_STRING_106, CVarList());
// 		return 0;
// 	}



// 	__int64 capitalSilver = m_pCapitalModule->GetCapital(pKernel, self, CAPITAL_TRADE_SILVER);
// 	if (capitalSilver < donateValue)
// 	{
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
// 			GUILD_CUSTOM_STRING_96, CVarList());
// 		return 0;
// 	}
// 
// 
// 	if (!m_pCapitalModule->CanIncCapital(pKernel, self, CAPITAL_GUILD_CURRENCY, addSelfGuildCurrency))
// 	{
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
// 			GUILD_CUSTOM_STRING_106, CVarList());
// 		return 0;
// 	}
// 
// 
// 
// 	int rc = m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_TRADE_SILVER,
// 		(__int64)donateValue, FUNCTION_EVENT_ID_GUILD_DONATE);
// 	if (rc != DC_SUCCESS)
// 	{
// 		return 0;
// 	}
// 
// 	m_pCapitalModule->IncCapital(pKernel, self, CAPITAL_GUILD_CURRENCY, addSelfGuildCurrency, FUNCTION_EVENT_ID_GUILD_DONATE);

	// 发送消息给公共服务器
	CVarList msg;
	msg << PUBSPACE_GUILD
		<< guildName
		<< SP_GUILD_MSG_DONATE
		<< playerName
		<< playerUid
		<< addGuildCapital
		<< donateValue
		<< addSelfGuildCurrency;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求公会职位变更
int GuildModule::OnCustomPromotion(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int type)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t *memberName = args.WideStrVal(2);
	int newPosition = args.IntVal(3);
	const wchar_t *guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}

	// 公会是否存在
	IRecord *guildListRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (guildListRecord == NULL)
	{
		return 0;
	}
	int guildRow = guildListRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return 0;
	}

	// 是否公会成员
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}
	IRecord *memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberRecord == NULL)
	{
		return 0;
	}
	const wchar_t *selfName = pSelfObj->QueryWideStr("Name");
	int selfRow = memberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, selfName);
	if (selfRow < 0)
	{
		return 0;
	}
	int memberRow = memberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, memberName);
	if (memberRow < 0)
	{
		return 0;
	}

	int selfPosition = memberRecord->QueryInt(selfRow, GUILD_MEMBER_REC_COL_POSITION);
	int oldPosition = memberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);
	if (0 > oldPosition || oldPosition >= GUILD_POSITION_COUNT)
	{
		return 0;
	}

	
	int pubMsgId = 0;
	
	switch (type)
	{
	case CS_GUILD_PROMOTION:
		//newPosition = oldPosition-1;
		pubMsgId = SP_GUILD_MSG_PROMOTION;
		
		break;
	case CS_GUILD_DEMOTION:
		//newPosition = oldPosition+1;
		pubMsgId = SP_GUILD_MSG_DEMOTION;
		
		break;
	default:
		return 0;
	}

	if (0 > newPosition || newPosition >= GUILD_POSITION_COUNT)
	{
		return 0;
	}

	// 检查是否有帮会任命的权限
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_SET_POSITION))
	{
		// 无权任命
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17629, CVarList());
		return 0;
	}

	// 不能任命比自己职位高的成员
	if (selfPosition >= oldPosition)
	{
		return 0;
	}
	// 不能任命比自己职位高的职位
	if (selfPosition >= newPosition)
	{
		return 0;
	}

	// 发送公共服务器进行处理
	CVarList msg;
	msg << PUBSPACE_GUILD
		<< guildName
		<< pubMsgId
		<< selfName
		<< memberName
		<< selfPosition
		<< oldPosition
		<< newPosition;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求公会商店列表
int GuildModule::OnCustomGetShopList(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}


	// 请求购买帮会商店功能开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_SHOP, self))
	{
		return 0;
	}

	// 找到帮会当前等级
	const wchar_t *guildName = pSelfObj->QueryWideStr("GuildName");
	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* playerUid = pKernel->SeekRoleUid(playerName);
	if (StringUtil::CharIsNull(guildName) 
		|| StringUtil::CharIsNull(playerName)
		|| StringUtil::CharIsNull(playerUid))
	{
		return 0;
	}
	IRecord *guildList = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
	if (guildList == NULL)
	{
		return 0;
	}
	int guildRow = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (guildRow < 0)
	{
		return 0;
	}
	int shangdianLevel = GuildBuildingLevel(pKernel, guildName,GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_SHANG_DIAN);

	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	GuildUpLevelConfig_t *upLevelCfg = &m_GuildUpLevelConfig[xiangFangLevel];
    if(upLevelCfg == NULL)
    {
        return 0;
    }

	IPubData *pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}
	IRecord *shopBuyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SHOP_BUY_REC);
	if (shopBuyRecord == NULL)
	{
		return 0;
	}
	IRecord *memberBuyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_MEMBER_BUY_REC);
	if (memberBuyRecord == NULL)
	{
		return 0;
	}
	IRecord *memberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberList == NULL)
	{
		return 0;
	}
	int memberRow = memberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
	if (memberRow < 0)
	{
		return 0;
	}
	int position = memberList->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);


	// 首先发送已经解锁的道具
	int shopItemCount = 0;
	CVarList shopItemList;
	GuildShopConfigVector::iterator iter(m_GuildShopConfig.begin());
	// 循环保护
	LoopBeginCheck(ds);
	for ( ; iter != m_GuildShopConfig.end(); ++iter)
	{
		LoopDoCheck(ds);
		if (iter->m_UnlockLevel > shangdianLevel)
		{
			break;
		}

		int guildTotal = (int)iter->m_GuildTotal;
		std::wstring itemName = guildName + ::util_int64_as_widestr(pKernel->GetServerId()) 
			+ StringUtil::StringAsWideStr(iter->m_ItemId.c_str()) + L"_" + StringUtil::IntAsWideStr(iter->m_Index);
		int shopBuyRow = shopBuyRecord->FindWideStr(GUILD_SHOP_BUY_REC_COL_ITEMID, itemName.c_str());
		if (shopBuyRow >= 0)
		{
			int count = shopBuyRecord->QueryInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT);
			guildTotal -= count;
		}

		int memberLimit = GetGuildShopMemberLimit(position, iter->m_MemberLimit);
		std::string memberBuyName = std::string(playerUid) + iter->m_ItemId + "_" + StringUtil::IntAsString(iter->m_Index);
		int memberBuyRow = memberBuyRecord->FindString(GUILD_MEMBER_BUY_REC_COL_UID_ITEMID, memberBuyName.c_str());
		if (memberBuyRow >= 0)
		{
			int count = memberBuyRecord->QueryInt(memberBuyRow, GUILD_MEMBER_BUY_REC_COL_BUY_COUNT);
			memberLimit -= count;
		}

		shopItemList << iter->m_Index << iter->m_ItemId<< (std::max)(guildTotal, 0) << (std::max)(memberLimit, 0) ;
		shopItemCount++;
	}


	CVarList rmsg;
	rmsg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_SHOP_LIST << shopItemCount;
	pKernel->Custom2(self, rmsg, shopItemList);
	return 0;
}

// 请求加入公会的CD
int GuildModule::OnCustomGetJoinCD(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int leftMinutes = 0;
	int64_t quitGuildDate = pSelfObj->QueryInt64("QuitGuildDate");
	if (quitGuildDate > 0)
	{
		int passMinutes = GetPassMinutes(quitGuildDate);
		leftMinutes = EnvirValueModule::EnvirQueryInt(ENV_VALUE_ADD_GUILD_CD) - passMinutes;
		// 为了弥补给让客户端倒计时计时器比较快的问题，如果有CD时间并且在60分钟内，再多加10分钟
		//leftMinutes = (leftMinutes > 0 && leftMinutes < 60) ? (leftMinutes+10) : leftMinutes;
	}

	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_JOIN_CD << leftMinutes);

	return 0;
}

// 请求购买商店商品
int GuildModule::OnCustomBuyItem(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int index = args.IntVal(2);
	int itemNum = args.IntVal(3);
	if (itemNum <= 0) 
	{
		return 0;
	}

	

	// 请求购买帮会商店功能开关是否开启
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_GUILD_SHOP, self))
	{
		return 0;
	}

	const wchar_t *guildName = pSelfObj->QueryWideStr("GuildName");
	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* playerUid = pKernel->SeekRoleUid(playerName);
	if (StringUtil::CharIsNull(guildName)
		|| StringUtil::CharIsNull(playerName)
		|| StringUtil::CharIsNull(playerUid))
	{
		return 0;
	}

	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}
	// 检查是否帮会一员
	IRecord *memberList = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberList == NULL)
	{
		return 0;
	}
	int memberRow = memberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
	if (memberRow < 0)
	{
		return 0;
	}
	int position = memberList->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);
	

	const GuildShopConfigItem_t *shopItem = GetShopItemInfo(index);
	if (shopItem == NULL)
	{
		return 0;
	}
	const char* itemID = shopItem->m_ItemId.c_str();

	int shangDianLv = GuildBuildingLevel(pKernel,guildName,GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_SHANG_DIAN);

	// 是否解锁
	if (shopItem->m_UnlockLevel > shangDianLv)
	{
		return 0;
	}


	////商店购买物品还没有扣钱
	int memberBuyTotal = GetGuildShopMemberLimit(position, shopItem->m_MemberLimit);
	int shopBuyedTotal = shopItem->m_GuildTotal;

	//// 检查剩余道具数量是否足够
	IRecord *shopBuyRecord = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_SHOP_BUY_REC);
	if (shopBuyRecord == NULL)
	{
		return 0;
	}
	int shopBuyCount = 0;
	std::wstring guildDataName = guildName + ::util_int64_as_widestr(pKernel->GetServerId());
	std::wstring shopBuyId = guildDataName + StringUtil::StringAsWideStr(itemID) + L"_" + StringUtil::IntAsWideStr(index);
	int shopBuyRow = shopBuyRecord->FindWideStr(GUILD_SHOP_BUY_REC_COL_ITEMID, shopBuyId.c_str());
	if (shopBuyRow >= 0)
	{
		shopBuyCount = shopBuyRecord->QueryInt(shopBuyRow, GUILD_SHOP_BUY_REC_COL_BUY_COUNT);
	}
	if ((shopBuyedTotal - shopBuyCount) < itemNum)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17630, CVarList());
		return 0;
	}

	////花费够不够 
	for (auto it : shopItem->m_decCapital)
	{
		if (!CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, it.first, it.second*itemNum))
		{
			//CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_21, CVarList());
			return 0;
		}
	}


	//// 背包格是否足够
	const PERSISTID toolbox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(toolbox))
	{
		return 0;
	}
	int nBindState = FunctionEventModule::GetItemBindState(FUNCTION_EVENT_ID_GUILD_SHOP_BUY);
	int putCnt = ContainerModule::TryAddItem(pKernel, toolbox, itemID, itemNum, nBindState);
	if (putCnt < itemNum)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7004, CVarList());
		return 0;
	}
	//扣除

	for (auto it : shopItem->m_decCapital)
	{
		if (!CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, it.first, it.second*itemNum,FUNCTION_EVENT_ID_GUILD_SHOP_BUY ))
		{
			return 0;
		}
	}
	


	//// 请求购买商品
	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName
		<< SP_GUILD_MSG_BUY_ITEM 
		<< playerUid
		<< playerName
		<< index
		<< itemID
		<< itemNum
		<< shopItem->m_addGuildCapital * itemNum
		<< shopBuyedTotal
		<< memberBuyTotal;

	pKernel->SendPublicMessage(msg);

	return 0;
}


// 请求移交帮主之位
int GuildModule::OnCustomCaptainTransfer(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	const wchar_t *newCaptainName = args.WideStrVal(2);
	if (StringUtil::CharIsNull(newCaptainName))
	{
		return 0;
	}

	const wchar_t *selfName = pSelfObj->QueryWideStr("Name");
	if (!CheckIsCaptain(pKernel, self)) 
	{
		return 0;
	}
	const char* playerUid = pKernel->SeekRoleUid(selfName);

	// 检查newCaptainName是否本帮会成员
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return 0;
	}
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}
	IRecord* pMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pMemberRecord == NULL)
	{
		return 0;
	}
	int memberRow = pMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, newCaptainName);
	if (memberRow < 0)
	{
		return 0;
	}
	int position = pMemberRecord->QueryInt(memberRow, GUILD_MEMBER_REC_COL_POSITION);
	if (position == GUILD_POSITION_CAPTAIN)
	{
		return 0;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD 
		<< guildName
		<< SP_GUILD_MSG_CAPTAIN_TRANSFER 
		<< playerUid
		<< selfName
		<< newCaptainName;
	pKernel->SendPublicMessage(msg);

	return 0;
}

// 请求帮会日志
int GuildModule::OnCustomGetLog(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 获取玩家对象
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 帮会名称
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}

	// 获取帮会公共空间
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 获取帮会日志表
	IRecord* pLogRecord = pGuildData->GetRecord(GUILD_LOG_REC);
	if (pLogRecord == NULL)
	{
		return 0;
	}
	
	// 开始行数
	
	int type = args.IntVal(2);
	int count = pLogRecord->GetRows();
	// 数据
	CVarList list;
	// 循环保护
	LoopBeginCheck(a);
	// 取数据
	for (int i = 0; i < count; ++i)
	{
		LoopDoCheck(a);
		int typeRow = pLogRecord->QueryInt(i, GUILD_LOG_COL_type);
		if (type != -1 && typeRow != type){
			continue;
		}

		int64_t time = pLogRecord->QueryInt64(i, GUILD_LOG_COL_time);
		const wchar_t* args = pLogRecord->QueryWideStr(i, GUILD_LOG_COL_args);
		if (args == NULL)
		{
			args = L"";
		}
		list << typeRow << time << args;
	}

	// 发数据给客户端
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_LOG << count << list);

	return 0;
}

// 请求捐献日志
int GuildModule::OnCustomGetDonateLog(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 获取玩家对象
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 帮会名称
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}

	// 获取帮会公共空间
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return 0;
	}

	// 获取帮会日志表
	IRecord* pLogRecord = pGuildData->GetRecord(GUILD_LOG_REC);
	if (pLogRecord == NULL)
	{
		return 0;
	}

	// 总行数
	int count = pLogRecord->GetRows();
	// 捐献数据条数
	int donateCount = 0;
	// 数据
	CVarList list;
	// 循环保护
	LoopBeginCheck(b);
	for (int i = count - 1; i >= 0; --i)
	{
		LoopDoCheck(b);

		// 检测数据类型
		int type = pLogRecord->QueryInt(i, GUILD_LOG_COL_type);
		int64_t time = pLogRecord->QueryInt64(i, GUILD_LOG_COL_time);
		const wchar_t* args = pLogRecord->QueryWideStr(i, GUILD_LOG_COL_args);
		if (args == NULL)
		{
			args = L"";
		}

		// 填充数据
		list << type << time << args;

		// 判断是否取完
		if (++donateCount > GUILD_LOG_DONATE_COUNT)
		{
			break;
		}
	}

	// 发数据给客户端
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_LOG_DONATE << donateCount << list);

	return 0;
}



//校验是否公会活动期间
bool GuildModule::CheckGuildActivity(IKernel* pKernel, const PERSISTID& self)
{
	// 帮会守护活动
	//if (GuildDefendModule::m_pGuildDefendModule->IsInGuildDefendTime() 
	//	|| GuildDineModule::IsDineTime(pKernel))		// 帮会聚餐活动
	//{
	//	CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_34, CVarList());
	//	return false;
	//}

	//工会成员退出检测是否合法,包含退出，踢人,解散公会
// 	if (!MiddleModule::Instance()->CheckGuildMemberQuit(pKernel,self))
// 	{
// 		return false;
// 	}
	return true;
}


void GuildModule::OnCustomSetAutoAgree(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return ;
	}
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_AUTO_AGREE))
	{
		// 无权审批
		return ;
	}
	//检查自己是否是帮会成员
	const wchar_t *selfName = pSelfObj->QueryWideStr("Name");
	const char* playerUid = pKernel->SeekRoleUid(selfName);
	const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
	if (!IsWidStrValid(guildName))
	{
		return ;
	}
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return ;
	}
	IRecord* pMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pMemberRecord == NULL)
	{
		return ;
	}
	int memberRow = pMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, selfName);
	if (memberRow < 0)
	{
		return;
	}
	

	CVarList msg;
	msg << PUBSPACE_GUILD
		<< guildName
		<< SP_GUILD_SET_AUTO_AGREE_STATE
		<<args.IntVal(2)
		<<args.IntVal(3)
		<<args.IntVal(4)
		<<args.IntVal(5);
	pKernel->SendPublicMessage(msg);

}
void GuildModule::OnCustomUnLockSkill(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_UNLOCK_SKILL))
	{

		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17631, CVarList());
		return;
	}

	const wchar_t * guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName))
	{
		return;
	}
	const wchar_t* playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);

	int skillID = args.IntVal(2);


	auto it = m_guildSkill.find(skillID);
	if (it == m_guildSkill.end())
	{
		return;
	}

	int row;
	IRecord* pGuildRecord = m_pGuildModule->GetGuildRecord(pKernel, guildName, row);
	if (pGuildRecord == NULL){
		return;
	}
	if (it->second.m_autoUnlock){
		return;
	}

	int shuYuanLv = GuildBuildingLevel(pKernel,guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_SHU_YUAN);
	if (it->second.m_unlockShuYuanLevel > shuYuanLv){
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17632, CVarList());
		return;
	}

	int guildCapital = GetGuildNumValue(pKernel,guildName,GUILD_NUM_TYPE::GUILD_NUM_CAPITAL);
	if (guildCapital < it->second.m_unlockCost)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17633, CVarList());
		return;
	}

	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return;
	}

	IRecord* pGuildPubSkillRecord = pGuildData->GetRecord(GUILD_PUB_SKILL_REC);
	if (pGuildPubSkillRecord == NULL)
	{
		return;
	}

	int skillRow = pGuildPubSkillRecord->FindInt(GUILD_PUB_SKILL_REC_COL_SKILLID, skillID);
	if (skillRow >= 0)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17634, CVarList());
		return;
	}

	CVarList msg;
	msg << PUBSPACE_GUILD
		<< guildName
		<< SP_GUILD_UNLOCK_SKILL
		<< skillID
		<< playerName;
	pKernel->SendPublicMessage(msg);

}


void GuildModule::OnCustomGetSkillList(IKernel*pKernel, const wchar_t*guildName, CVarList&var)
{
	
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return;
	}

	IRecord* pSkillRecord = pGuildData->GetRecord(GUILD_PUB_SKILL_REC);
	if (pSkillRecord == NULL)
	{
		return;
	}

	for (int row = 0; row < pSkillRecord->GetRows(); row++)
	{
		CVarList info;
		pSkillRecord->QueryRowValue(row, info);
		var << info;
	}
		

}

void GuildModule::OnCustomSkillLevelUp(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}
	const wchar_t * guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName))
	{
		return;
	}
	const wchar_t* playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
	const int skillID = args.IntVal(2);
	auto  it = m_guildSkill.find(skillID);
	if (it == m_guildSkill.end())
	{
		return;
	}


	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return;
	}

	IRecord* pGuildPubSkillRecord = pGuildData->GetRecord(GUILD_PUB_SKILL_REC);
	if (pGuildPubSkillRecord == NULL)
	{
		return;
	}

	int skillRow = pGuildPubSkillRecord->FindInt(GUILD_PUB_SKILL_REC_COL_SKILLID, skillID);
	if (skillRow < 0)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17635, CVarList());
		return;
	}
	int skillLimitLv = pGuildPubSkillRecord->QueryInt(skillRow, GUILD_PUB_SKILL_REC_COL_LEVEL);


	// 找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return;
	}
	// 没找到技能
	int nRows = pPassiveSkillRec->FindInt(PS_COL_SKILL_ID, skillID);
	if (-1 == nRows)
	{
		return;
	}


	// 默认为只升一级
	int nOldSkillLevel = pPassiveSkillRec->QueryInt(nRows, PS_COL_SKILL_LEVEL);

	if (nOldSkillLevel >= skillLimitLv){
		return;
	}

	int nNewSkillLevel = nOldSkillLevel + 1;
	auto its = m_guildSkillLevelUpCost.find(it->second.m_skillType);

	if (its == m_guildSkillLevelUpCost.end()){
		return;
	}

	auto itt = its->second.find(nOldSkillLevel);
	if (itt == its->second.end()){
		return;
	}
	// 货币测试
	for (auto itf : itt->second.m_vCurrency)
	{
		if (!CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, itf.first, itf.second) )
		{
			//::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_82, CVarList());
			return;
		}
	}

	// 查找背包
	PERSISTID itemBox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(itemBox))
	{
		return;
	}
	// 物品是否足够
	if (!ContainerModule::m_pContainerModule->TryRemoveItems(pKernel, itemBox, itt->second.m_vItems))
	{
		return;
	}


	// 玩家等级
	int player_level = pSelfObj->QueryInt("Level");
	
	// 心法等级不能高于限制
	if (nNewSkillLevel > player_level)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17636, CVarList());
		return;
	}

	// 扣除货币
	for (auto itf : itt->second.m_vCurrency)
	{
		CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, itf.first, itf.second, FUNCTION_EVENT_ID_GUILD_SKILL);
	}
	// 扣除物品
	ContainerModule::m_pContainerModule->RemoveItems(pKernel, itemBox, itt->second.m_vItems, NULL, FUNCTION_EVENT_ID_GUILD_SKILL);

	m_pGuildModule->GuildSkillLevelUp(pKernel, self, skillID, nNewSkillLevel);

	/*CVarList msg;
	msg << PUBSPACE_GUILD
		<< guildName
		<< SP_GUILD_SKILL_LEVEL_UP
		<< skillID
		<< nCostDonate
		<< playerName
		<< nNewSkillLevel;
	pKernel->SendPublicMessage(msg);*/
}





void GuildModule::OnGuildBuildingOperateLv(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	//权限检查
	if (!m_pGuildModule->CheckPositionPriv(pKernel, self, GUILD_PRIV_BUILDING_LEVEL_UP)){
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return ;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	const wchar_t * guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName)){
		return;
	}

	const wchar_t *playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
	int buildingType = args.IntVal(2);
	int operate = args.IntVal(3);
	int useMiBao = args.IntVal(4);
	CVarList pubMsg;
	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_BUILDING_LVUP_OPERATE << operate << playerName << buildingType << useMiBao;
	pKernel->SendPublicMessage(pubMsg);



}

void GuildModule::AddGuildBuildingExp(IKernel *pKernel, const wchar_t*guildName, int addValue)
{

	if (StringUtil::CharIsNull(guildName)){
		return;
	}


	CVarList pubMsg;
	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_BUILDING_LEVEL_ADD_EXP << addValue;
	pKernel->SendPublicMessage(pubMsg);

}

void GuildModule::OnGuildNumBuy(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	//权限检查
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_GUILD_NUM_BUY)){
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	const wchar_t * guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName)){
		return;
	}

	const wchar_t *playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
	int guildNumType = args.IntVal(2);


	CVarList pubMsg;
	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_NUM_BUY << playerName << guildNumType;
	pKernel->SendPublicMessage(pubMsg);

}

void GuildModule::OnCustomChangeGuildShortName(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{

	const wchar_t *shortName = args.WideStrVal(2);
	const wchar_t *guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName))
	{
		return;
	}
	const wchar_t* playerName = pKernel->QueryWideStr(self, FIELD_PROP_NAME);

	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_CHANGE_SHORT_NAME))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return;
	}

	if (GuildShortNameExist(pKernel, shortName))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return ;
	}

// 	if (GetGuildNumValue(pKernel, guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL) < EnvirValueModule::EnvirQueryInt(ENV_VALUE_CHANGE_SHORT_NAME))
// 	{
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_103, CVarList());
// 		return;
// 	}
// 
// 
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_CHANGE_SHORT_NAME << shortName << playerName;
// 	pKernel->SendPublicMessage(pubMsg);
}

void GuildModule::OnCustomGetJianKangDuGif(IKernel*pKernel, const PERSISTID& self)
{
	IGameObj*pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}


// 	const wchar_t *playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
// 	const wchar_t *guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	int guildDailyGif = pSelfObj->QueryInt(FIELD_PROP_GUILD_DAILY_GIF);
// 	if (guildDailyGif > 0){
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_105, CVarList());
// 		return;
// 	}
// 	
// 
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_GET_JIAN_KANG_DU_GIF << playerName;
// 	pKernel->SendPublicMessage(pubMsg);
}

void GuildModule::OnCustomGuildSkillLvOperate(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	int skillID = args.IntVal(2);
	int operate = args.IntVal(3);
	const wchar_t *playerName = pKernel->QueryWideStr(self, FIELD_PROP_NAME);
	const wchar_t *guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_START_STUDY_SKILL))
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return;
	}

	IRecord* pGuildPubSkillData = GetPubRecord(pKernel, guildName, GUILD_PUB_SKILL_REC);
	if (pGuildPubSkillData == NULL)
	{
		return;
	}
	int row = -1;
	if (operate == GUILD_OPERATE_START_LVUP){
		 row = pGuildPubSkillData->FindInt(GUILD_PUB_SKILL_REC_COL_STUDY_STATE, GUILD_PUB_SKILL_STUDY_STATE_STUDYING);
		if (row >= 0)
		{
			CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17638, CVarList());
			return;
		}
	}

	row = pGuildPubSkillData->FindInt(GUILD_PUB_SKILL_REC_COL_SKILLID,skillID);
	if (row < 0)
	{
		return;
	}



	CVarList pubMsg;
	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_SKILL_LVUP_OPERATE << operate << skillID << playerName;
	pKernel->SendPublicMessage(pubMsg);



}
int GuildModule::OnCustomUseRareTreasure(IKernel*pKernel, const PERSISTID& self, int rareTreasureType, int useNum)
{
	if (useNum == 0){
		return 0;
	}
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return 0;
	}
	
	

	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_USE_RARE_TREASURE)){
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17637, CVarList());
		return 0 ;
	}
	const wchar_t*guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	

// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_USE_RARE_TREASURE << pSelfObj->QueryWideStr(FIELD_PROP_NAME) << rareTreasureType << useNum;
// 	pKernel->SendPublicMessage(pubMsg);


	return 0;
}

int GuildModule::OnCustomGetRareTreasureList(IKernel*pKernel, const PERSISTID& self)
{
	const wchar_t *guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);

	if (StringUtil::CharIsNull(guildName)){
		return 0;
	}


	IRecord *pGuildRareTreasure = GetPubRecord(pKernel, guildName, GUILD_RARE_TREASURE_REC);
	if (pGuildRareTreasure == NULL)
	{
		return 0;
	}
	int count = pGuildRareTreasure->GetRows();
	CVarList var;
	for (int i = 0; i < count; i++)
	{
		CVarList info;
		pGuildRareTreasure->QueryRowValue(i, info);
		var << info;
	}

	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_RARE_TREASURE_LIST << count << var);

	return 0;
}


int GuildModule::OnCustomGetNumBuyRecord(IKernel*pKernel, const PERSISTID& self)
{
	const wchar_t* guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName)){
		return 0;
	}

	IRecord * pGuildNumBuyRec = GetPubRecord(pKernel, guildName, GUILD_NUM_BUY_REC);
	if (pGuildNumBuyRec == NULL){
		return 0;
	}
	CVarList var;
	int buyTimes = 0;
	int count = pGuildNumBuyRec->GetRows();
	for (int i = 0; i < count; i++)
	{
		CVarList info;
		pGuildNumBuyRec->QueryRowValue(i,info);
		var << info;
	}

	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_NUM_BUY_RECORD << count << var);


	return 0;
}
int GuildModule::OnCustomGetBonusInfo(IKernel*pKernel, const PERSISTID& self)
{
	const wchar_t*guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName)){ 
		return 0; 
	}

	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL){
		return 0;
	}


	int bonusValue = pGuildData->QueryAttrInt(GUILD_BONUS_VALUE);
	int bonusLockValue = pGuildData->QueryAttrInt(GUILD_BONUS_LOCK_VALUE);

	auto bonusTime = pGuildData->GetRecord(GUILD_BONUS_REC);
	if (NULL == bonusTime)
	{
		return 0;
	}

	CVarList bonusInfo;
	for (int i = 0; i < bonusTime->GetRows(); i++)
	{
		CVarList info;
		bonusTime->QueryRowValue(i, info);
		bonusInfo << info;
	}


	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_BONUS_INFO << bonusValue << bonusLockValue
		<< bonusTime->GetRows() << bonusInfo);


	return 0;
}



int GuildModule::OnCustomGetGuildWelfare(IKernel*pKernel, const PERSISTID& self)
{
	const wchar_t*guildName = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(guildName)){
		return 0;
	}
	const wchar_t*playerName = pKernel->QueryWideStr(self, FIELD_PROP_NAME);
	IPubData* pGuildPubData = GetPubData(pKernel, GUILD_PUB_DATA);
	if (pGuildPubData == NULL){
		return 0;
	}

	

	IPubData* pGuilData = GetPubData(pKernel, guildName);
	if (pGuilData == NULL){
		return 0;
	}


	int dailyGetCapital = pGuilData->QueryAttrInt(DAILY_GET_GUILD_CAPITAL);
	int dailyGainCapital = 0;
	IRecord* pGainCapital = pGuilData->GetRecord(GUILD_GAIN_CAPITAL_REC);
	if (pGainCapital != NULL)
	{
		int row = pGainCapital->FindWideStr(GUILD_GAIN_CAPITAL_REC_COL_NAME, playerName);
		if (row >= 0)
		{
			dailyGainCapital = pGainCapital->QueryInt(row, GUILD_GAIN_CAPITAL_REC_COL_VALUE);
		}
	}

	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_WELFARE_INFO << dailyGetCapital << dailyGainCapital);
	return 0;
}


int GuildModule::OnCustomGiveOutBonus(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	
	IGameObj*pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return 0;
	}

		
// 	int giveOutValue = args.IntVal(3);
// 	int giveOutValueMax = EnvirValueModule::EnvirQueryInt(ENV_VALUE_GIVE_OUT_BONUS_MAX);
// 	if (giveOutValue > giveOutValueMax)
// 	{
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_112, CVarList());
// 		return 0;
// 	}
// 
// 
// 
// 
// 	const wchar_t*guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	const wchar_t*playerName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
// 	if (!CheckPositionPriv(pKernel, self, GUILD_PRIV_GIVE_OUT_BONUS)){
// 		CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, GUILD_CUSTOM_STRING_109, CVarList());
// 		return 0;
// 	}
// 
// 	CVarList info;
// 	info.Append(args, 2, args.GetCount() - 2);
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD << guildName << SP_GUILD_GIVE_OUT_BONUS << playerName << info;
// 	pKernel->SendPublicMessage(pubMsg);
	return 0;
}

int GuildModule::OnCustomInviteJoinGuild(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//确定玩家帮会
	const wchar_t * guildName = pSelfObj->QueryWideStr("GuildName");
	if (StringUtil::CharIsNull(guildName))
	{
		return 0;
	}

	int position = pSelfObj->QueryInt("GuildPosition");
	if (position != GUILD_POSITION_CAPTAIN){
		return 0;
	}

	int guildJuYiTingLevel = GetGuildLevel(pKernel, guildName);
	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	// 获得公会最大成员数量
	GuildUpLevelConfig_t* upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
	if (upLevelConfig == NULL)
	{
		return 0;
	}

	IRecord* pGuildMemberList = GetPubRecord(pKernel,guildName, GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return 0;
	}

	int curMember = pGuildMemberList->GetRows();
	

	if (curMember >= upLevelConfig->m_MaxMember)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17639, CVarList());
		return 0;
	}

	const wchar_t* inviteName = args.WideStrVal(2);
	if (StringUtil::CharIsNull(inviteName))
	{
		return 0;
	}

	if (pKernel->GetPlayerScene(inviteName) <= 0)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17640, CVarList());
		return 0;
	}

	pKernel->CustomByName(inviteName, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_INVITE_JOIN_GUILD<<pSelfObj->QueryWideStr(FIELD_PROP_NAME)<<guildName);
	pKernel->CommandByName(inviteName, CVarList() << COMMAND_GUILD_BY_NAME << SS_GUILD_INVITE_JOIN_GUILD << guildName);
	return 0;
}

int GuildModule::OnCustomAgreeInviteJoinGuild(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return 0;
	}
	
	const wchar_t*guildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (!StringUtil::CharIsNull(guildName)){
		return 0;
	}

	if (!pSelfObj->FindData(GUILD_INVITE_GUILD_NAME))
	{
		return 0 ;
	}

	const wchar_t* inviteGuildName	=  pSelfObj->QueryDataWideStr(GUILD_INVITE_GUILD_NAME);
	
	int guildJuYiTingLevel = GetGuildLevel(pKernel, inviteGuildName);
	int xiangFangLevel = GuildBuildingLevel(pKernel, inviteGuildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
	// 获得公会最大成员数量
	GuildUpLevelConfig_t* upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
	if (upLevelConfig == NULL)
	{
		return 0;
	}

	IRecord* pGuildMemberList = GetPubRecord(pKernel, inviteGuildName, GUILD_MEMBER_REC);
	if (pGuildMemberList == NULL)
	{
		return 0;
	}

	int curMember = pGuildMemberList->GetRows();


	if (curMember >= upLevelConfig->m_MaxMember)
	{
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17622, CVarList());
	}

	int findRow = pGuildMemberList->FindWideStr(GUILD_MEMBER_REC_COL_NAME, inviteGuildName);
	if (findRow >= 0)
	{
		return 0;
	}


	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	const char* pPlayerUid = pKernel->SeekRoleUid(playerName);
	int playerSex = pSelfObj->QueryInt("Sex");
	int playerLevel = pSelfObj->QueryInt("Level");
	int ability = pSelfObj->QueryInt("BattleAbility");
	int career = pSelfObj->QueryInt("Job");
	int vipLevel = pSelfObj->QueryInt(FIELD_PROP_VIP_LEVEL);
	CVarList info;
	info << pPlayerUid << playerName
		<< playerSex << playerLevel << ability << career << vipLevel << upLevelConfig->m_MaxMember;
	CVarList pubMsg;
	pubMsg << PUBSPACE_GUILD << inviteGuildName << SP_GUILD_AGREE_INVITE_JOIN_GUILD << info;
	pKernel->SendPublicMessage(pubMsg);


	return 0;
}


void GuildModule::OnCustomAddPetition(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
// 	int type = args.IntVal(2);
// 	IGameObj*pSelfObj = pKernel->GetGameObj(self);
// 	if (pSelfObj == NULL){
// 		return;
// 	}
// 
// 	const wchar_t *pGuildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	if (StringUtil::CharIsNull(pGuildName)){
// 		return;
// 	}
// 
// 	const wchar_t* pSelfName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
// 	CVarList pubMsg;
// 	pubMsg << PUBSPACE_GUILD << pGuildName << SP_GUILD_ADD_PETITION << type << pSelfName;
// 	pKernel->SendPublicMessage(pubMsg);

}

void GuildModule::OnCustomGetPetition(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	int type = args.IntVal(2);

	IGameObj*pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	const wchar_t *pGuildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
	if (StringUtil::CharIsNull(pGuildName)){
		return;
	}

	IRecord* pGuildMemberList = GetPubRecord(pKernel, pGuildName, GUILD_PETITION_REC);
	if (pGuildMemberList == NULL)
	{
		return;
	}
	int count = 0;
	CVarList info;
	for (int row = 0; row < pGuildMemberList->GetRows(); row++)
	{
		
		int typeRow = pGuildMemberList->QueryInt(row, GUILD_PETITION_REC_COL_TYPE);
		if (type !=-1&&type != typeRow){ 
			continue; 
		}
		++count;
		const wchar_t* playerNameRow = pGuildMemberList->QueryWideStr(row, GUILD_PETITION_REC_COL_NAME);
		info << type << playerNameRow;
	}
	
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_GET_PETTION_LIST << count << info);
}
void GuildModule::OnCustomGetOtherGuildInfo(IKernel*pKernel, const PERSISTID& self, const IVarList& args)
{
	const wchar_t* guildName = args.WideStrVal(2);
	if (StringUtil::CharIsNull(guildName)){
		return;
	}

	int fanrongdu = GetGuildNumValue(pKernel, guildName, GUILD_NUM_FANRONGDU);
	int guildCapital = GetGuildNumValue(pKernel, guildName, GUILD_NUM_CAPITAL);
	int guildXingdongli = GetGuildNumValue(pKernel, guildName, GUILD_NUM_XINGDONGLI);
	int guildJiankandu = GetGuildNumValue(pKernel, guildName, GUILD_NUM_JIANKANDU);

	// 获取帮会公共空间
	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL){
		return;
	}

	// 获取成员列表
	IRecord* memberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (memberRecord == NULL){
		return;
	}


	int playerNum = memberRecord->GetRows();
	
	const wchar_t* captainName = L"";
	const wchar_t* assistantName = L"";

	for (int i = 0; i < playerNum; i++)
	{
		int position = memberRecord->QueryInt(i, GUILD_MEMBER_REC_COL_POSITION);
		if (position == GUILD_POSITION_CAPTAIN)
		{
			captainName = memberRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
		}
		else if (position == GUILD_POSITION_ASSISTANT_CAPTAIN)
		{
			assistantName = memberRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
		}

	}



	IRecord *pGuildBuildingRec = GetPubRecord(pKernel, guildName, GUILD_BUILDING_REC);
	if (pGuildBuildingRec == NULL){
		return ;
	}


	int buildingRowMax = pGuildBuildingRec->GetRows();
	CVarList buildingInfo;
	for (int i = 0; i < buildingRowMax; i++)
	{

		int buildingType = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_TYPE);
		int buildingLv = pGuildBuildingRec->QueryInt(i, GUILD_BUILDING_REC_COL_LEVEL);
		CVarList var;
		var << buildingType << buildingLv;
		buildingInfo << var;
	}

	
// 	IRecord *pbattle = m_pGuildModule->GetPubRecord(pKernel, GUILD_PUB_DATA, C_REC_GUILD_BATTLE_TOWN);
// 	if (pbattle == NULL)
// 	{
// 		return;
// 	}
// 	int ownerID = 0;
// 	int battleRow = pbattle->FindWideStr(E_FIELD_BATTLE_TOWN_WCHAR_OCCUPIER,guildName);
// 	if (battleRow >= 0)
// 	{
// 		ownerID = pbattle->QueryInt(battleRow, E_FIELD_BATTLE_TOWN_INT32_TOWN);
// 	}
// 
// 	IRecord* pGuildFlagRec = pGuildData->GetRecord(GUILD_DOMAIN_FLAG_REC);
// 	if (NULL == pGuildFlagRec)
// 	{
// 		return;
// 	}
// 
// 	int guildFlagRowMax = pGuildFlagRec->GetRows() ;
// 	CVarList flagInfo;
// 	for (int i = 0; i < guildFlagRowMax; i++)
// 	{
// 		int nFlagType = pGuildFlagRec->QueryInt(i, GUILD_FLAG_COL_FLAG_TYPE);
// 		int nFlagId = pGuildFlagRec->QueryInt(i, GUILD_FLAG_COL_FLAG_ID);
// 		flagInfo<<nFlagType
// 			<< nFlagId;
// 	}
// 
// 	IRecord *pGuildList = m_pGuildModule->GetPubRecord(pKernel, GUILD_PUB_DATA,GUILD_LIST_REC);
// 	if (pGuildList == NULL)
// 	{
// 		return;
// 	}
// 
// 	int guildListRow = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
// 	if (guildListRow < 0){
// 		return;
// 	}
// 
// 	auto declaration =  pGuildList->QueryWideStr(guildListRow, GUILD_LIST_REC_COL_DECLARATION);
// 	
// 	int nation = pGuildList->QueryInt(guildListRow, GUILD_LIST_REC_COL_NATION);
// 
// 	int xiangFangLevel = GuildBuildingLevel(pKernel, guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_XIANG_FANG);
// 	GuildUpLevelConfig_t *upLevelConfig = GetGuildUpLevelConfig(xiangFangLevel);
// 	if (upLevelConfig == NULL)
// 	{
// 		return ;
// 	}
// 
// 	int maxMember = upLevelConfig->m_MaxMember;
// 	int power = pGuildList->QueryInt(guildListRow, GUILD_LIST_REC_COL_FIGHT_ABILITY);
// 
// 	// 获取公共空间
// 	IPubData* pPubData = GetPubData(pKernel, GUILD_PUB_DATA);
// 	if (NULL == pPubData)
// 	{
// 		return;
// 	}
// 
// 	IRecord* pSortRecord = pPubData->GetRecord(GUILD_SORT_REC);
// 	if (NULL == pSortRecord)
// 	{
// 		return;
// 	}
// 
// 	const int rank_row = pSortRecord->FindWideStr(GUILD_SORT_REC_COL_NAME, guildName);	
// 
// 	CVarList msg;
// 	msg << SERVER_CUSTOMMSG_GUILD
// 		<< SC_GUILD_OTHER_GUILD_INFO
// 		<< guildName
// 		<< nation
// 		<< fanrongdu
// 		<< guildCapital
// 		<< playerNum
// 		<< captainName
// 		<< assistantName
// 		<< declaration
// 		<< maxMember
// 		<< ownerID
// 		<< guildFlagRowMax
// 		<< flagInfo
// 		<< buildingRowMax
// 		<< buildingInfo
// 		<< power
// 		<< rank_row
// 		<< guildXingdongli
// 		<< guildJiankandu;
// 
// 	pKernel->Custom(self, msg);

}