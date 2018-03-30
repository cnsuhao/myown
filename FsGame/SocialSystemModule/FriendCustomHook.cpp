//--------------------------------------------------------------------
// 文件名:		FriendCustomHook.cpp
// 内  容:		好友系统-处理客户端消息
// 说  明:		
// 创建日期:	2014年10月28日
// 整理日期:	2014年10月28日
// 创建人:		  
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "FriendModule.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/FriendDefine.h"
#include "FsGame/Define/FriendMsgDefine.h"
#include "FsGame/Define/SnsDefine.h"
#include "FsGame/Define/SnsDataDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/PlayerBaseDefine.h"

#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/SnsDataModule.h"
//#include "FsGame/InterActiveModule/TeamModule.h"
//#include "FsGame/InterActiveModule/PartnerModule.h"
#include <vector>
#include <algorithm>
#include "SystemFunctionModule/CapitalModule.h"
#include "CommonModule/EnvirValueModule.h"
//#include "RevengeArenaModule.h"
#include "Define/Fields.h"
#include "CommonModule/ContainerModule.h"
#include "Interface/FightInterface.h"
//#include "TradeModule/VipModule.h"
//#include "Define/VipDefine.h"

// 客户端消息处理
int FriendModule::OnCustomMessage(IKernel* pKernel, const PERSISTID& self,
								  const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	if (args.GetCount() < 2)
	{
		return 0;
	}

	int subMsg = args.IntVal(1);
	switch(subMsg)
	{
	case CS_FRIEND_REQUEST_ADD_FRIEND://请求添加好友
		{
			m_pFriendModule->OnCustomAddFriend(pKernel, self, args);
		}
		break;
	case CS_FRIEND_CONFIRM_ADD_FRIEND://确认添加好友
		{
			m_pFriendModule->OnCustomAcceptAddFriend(pKernel, self, args);
		}
		break;
	case CS_FRIEND_REQUEST_DELETE_FRIEND:// 请求删除好友
		{
			m_pFriendModule->OnCustomDeleteFriend(pKernel, self, args);
		}
		break;
	case CS_FRIEND_DELETE_APPLY:// 删除（忽略）好友申请
		{
			m_pFriendModule->OnCustomDeleteFriendApply(pKernel, self, args);
		}
		break;
	case CS_FRIEND_REFRESH_FRIEND_RMD:// 刷新推荐好友 
		{
			m_pFriendModule->OnCustomRefreshFriendRmd(pKernel, self, args);
		}
		break;
	case CS_FRIEND_INFO_UPDATE:// 好友信息更新
		{
			m_pFriendModule->OnCustomFriendInfoUpdate(pKernel, self);
		}
		break;
	case CS_FRIEND_DELETE_ENEMY:		//删除仇人
		{
			m_pFriendModule->OnCustomDeleteEnemy(pKernel, self, args);
		}
		break;
	case CS_FRIEND_DELETE_ALL_ENEMY:	//删除所有仇人
		{
			m_pFriendModule->OnCustomDeleteAllEnemy(pKernel, self, args);
		}
		break;
	case CS_FRIEND_ALL_NEARBY:	//请求附近玩家
		{
			m_pFriendModule->OnCustomRequestNearby(pKernel, self, args);
		}
		break;
	case CS_FRIEND_SEND_GIFT:
	{
		if (args.GetCount() >= 4)
		{			
			m_pFriendModule->OnCustomSendGift(pKernel, self, args.StringVal(2), args.IntVal(3));
		}
		else
		{
			extend_warning(LOG_ERROR, "[FriendModule::OnCustomMessage]send gift param error.");
		}
	}
	break;
	case CS_FRIEND_CONFIRM_ALL_APPLY:
	{
		bool bAccept = args.IntVal(2) != 0;
		m_pFriendModule->OnCustomConfirmAllApply(pKernel, self, bAccept);
	}
	case CS_FRIEND_ADD_BLACKLIST: // 添加黑名单
		{
			if (args.GetCount() >= 3)
			{
				m_pFriendModule->OnCustomAddBlackList(pKernel, self, args.WideStrVal(2) );
			}
			else
			{
				extend_warning(LOG_ERROR, "[FriendModule::OnCustomMessage]add black list param error.");
			}
		}
		break;
	case CS_FRIEND_DELETE_BLACKLIST: // 删除黑名单
		{
			if (args.GetCount() >= 3)
			{
				m_pFriendModule->OnCustomDeleteBlackList(pKernel, self, args.WideStrVal(2));
			}
			else
			{
				extend_warning(LOG_ERROR, "[FriendModule::OnCustomMessage]delete black list param error.");
			}
		}
		break;
	case CS_FRIEND_DELETE_ALL_BLACKLIST: // 删除所有黑名单
		{
			m_pFriendModule->OnCustomClearBlackList( pKernel, self );
		}
		break;
	case CS_FRIEND_APPLAY_SWORN:// 结义申请
	{
		m_pFriendModule->OnCustomApplaySworn(pKernel, self, args);
	}break;
	case CS_FRIEND_APPLAY_SWORN_REPLY://结义申请回复
	{
		m_pFriendModule->OnCustomApplaySwornReply(pKernel, self, args);
	}break;
	case CS_FRIEND_SEEK_AID:// 寻求援助
	{
		m_pFriendModule->OnCustomSeekAid(pKernel,self,args);
	}break;
	case CS_FRIEND_CHANGE_FRIEND_TITILE:// 修改好友称谓
	{
		m_pFriendModule->OnCustomChangeFriendTitle(pKernel,self,args);
	}break;
	case CS_FRIEND_DEL_SWORN:// 删除结义
	{
		m_pFriendModule->OnCustomDelSworn(pKernel,self,args);
	}break;
	case CS_FRIEND_AID: //援助
	{
		m_pFriendModule->OnCustomAid(pKernel,self,args);
	}break;
	case CS_FRIEND_INTIMACY_AWARD:
	{
		m_pFriendModule->OnCustomIntimacyAwards(pKernel, self);
	}break;
	case CS_FRIEND_ADD_FRIEND_BY_ONE_KEY:
	{
		m_pFriendModule->OnCustomAddFriendByOneKey(pKernel, self, args);
	}break;
	default:
		break;
	}

	return 0;
}

// 添加好友请求
void FriendModule::OnCustomAddFriend(IKernel *pKernel, const PERSISTID & self, const IVarList &args)
{
	if (args.GetCount() < 4)
	{
		return;
	}

	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	const wchar_t* pSelfName = pSelfObj->QueryWideStr("Name");
	const wchar_t* pTargetName = args.WideStrVal(2);
    ApplyType applyType = (ApplyType)args.IntVal(3);//申请类型，好友
	const char * pTargetUid	= pKernel->SeekRoleUid(pTargetName);

	if (StringUtil::CharIsNull(pSelfName) ||
		StringUtil::CharIsNull(pTargetName) ||
		wcscmp(pSelfName, pTargetName) == 0)
	{
		return;
	}

    if (applyType == APPLY_TYPE_FRIEND)
    {
        // 是否满足好友条件
        if (!m_pFriendModule->IsCanAddFriend(pKernel, self, pTargetName, true))
        {
            return;
        }
    }

	const char * pSelfUid	= pKernel->SeekRoleUid(pSelfName);
	int level	= pSelfObj->QueryInt("Level");
	int fightcap= pSelfObj->QueryInt("BattleAbility");
	int job		= pSelfObj->QueryInt("Job");
	int sex		= pSelfObj->QueryInt("Sex");
	int vipLv   = pSelfObj->QueryInt("VipLevel");
	const wchar_t *guild_name = pSelfObj->QueryWideStr("GuildName");

	if (StringUtil::CharIsNull(pSelfUid) ||
		StringUtil::CharIsNull(pTargetUid))
	{
		return;
	}

	// 把此玩家加入已邀请的表中
	m_pFriendModule->SetPlayerInvited(pKernel, self, pTargetUid);
	if (applyType == APPLY_TYPE_FRIEND)
	{
		if (m_pFriendModule->DeleteRecommend(pKernel, self, pTargetName) > 0)
		{
			// 添加完推荐的好友时再增加一个
			m_pFriendModule->AddOneRecommend(pKernel, self, args);
		}
	}


	//判断玩家是否在线
	if (pKernel->GetPlayerScene(pTargetName) <= 0)
	{
		// 目标不在线，发送消息给SNS服务器，记录邀请
		pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
			pKernel->GetSceneId(), pTargetUid,
			CVarList() << SPACE_SNS
			<< SNS_FRIEND_SPACE
			<< SNS_MSG_FRIEND_APPLY 
			<< pSelfUid
			<< pSelfName
			<< pTargetUid
			<< pTargetName
			<< level
			<< job
			<< sex
			<< fightcap
			<< guild_name
            << applyType
			<< vipLv);
	}
	else
	{
		//玩家请求好友未处理项
        SSFrienMsg subMsg = SS_FRIEND_APPLY;
		CVarList var;
		var << COMMAND_FRIEND_MSG 
			<< subMsg 
			<< pSelfUid 
			<< pSelfName
			<< level
			<< job
			<< sex
			<< fightcap
			<< guild_name
            << applyType
			<< vipLv;
		pKernel->CommandByName( pTargetName, var);
	}

	// 通知客户端，邀请已发出，请等待
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17101, CVarList());
}

// 确认好友添加
void FriendModule::OnCustomAcceptAddFriend(IKernel *pKernel, const PERSISTID &self, const IVarList &args)
{
	if (args.GetCount() < 4)
	{
		return;
	}

	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	const char * pTargetUid = args.StringVal(2);
    int applyType = args.IntVal(3);
	if (StringUtil::CharIsNull(pTargetUid))
	{
		return;
	}

	// 自身姓名,对方姓名
	const wchar_t* pTargetName = pKernel->SeekRoleName(pTargetUid);
	const wchar_t* pSelfName = pSelfObj->QueryWideStr("Name");

	if (StringUtil::CharIsNull(pSelfName) ||
		StringUtil::CharIsNull(pTargetName) ||
		wcscmp(pSelfName, pTargetName) == 0)
	{
		return;
	}

	int applyRow = m_pFriendModule->FindApplyRow(pKernel, self, pTargetUid, (ApplyType)applyType);
	if (applyRow == -1)
	{
		//不在我的申请表里
		return;
	}

	// 表格指针
	IRecord* pRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	IRecord* pApplyRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_APPLY_REC);
	if (NULL == pRec || NULL == pApplyRec)
	{
		return;
	}

	this->OnAcceptFriend(pKernel, pSelfObj, applyRow, pApplyRec, pRec);
}

// 申请删除（忽略）邀请好友未处理项
void FriendModule::OnCustomDeleteFriendApply(IKernel *pKernel, const PERSISTID &self, const IVarList &args)
{
	const char * pTargetUid = args.StringVal(2);
    int applyType = args.IntVal(3);
	if (StringUtil::CharIsNull(pTargetUid))
	{
		return;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	int applyRow = m_pFriendModule->FindApplyRow(pKernel, self, pTargetUid, (ApplyType)applyType);
	if (applyRow == -1)
	{
		//不在我的申请表里
		return;
	}

	IRecord* pApplyRec = pSelf->GetRecord(FIELD_RECORD_FRIEND_APPLY_REC);
	if ( NULL == pApplyRec)
	{
		return;
	}

	this->OnRefuseFriend(pKernel, pSelf, applyRow, pApplyRec);
}

// 删除好友
void FriendModule::OnCustomDeleteFriend(IKernel *pKernel, const PERSISTID &self, const IVarList &args)
{
	const wchar_t * pTargetName = args.WideStrVal(2);
	DeleteFriend(pKernel, self, pTargetName);
}

//请求刷新推荐好友
void FriendModule::OnCustomRefreshFriendRmd(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 好友推荐表, 好友推荐表筛选表
	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	IRecord * pApplyRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_APPLY_REC);
	IRecord * pRecommendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_REC);
    IRecord * pRmdFriendMidRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_MID_REC);
	//IRecord * pFriendInvitaionSendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_INVITATION_SEND_REC); // 已经发出的邀请
	IRecord * pBlackRec = pSelfObj->GetRecord(FIELD_RECORD_BLACKLIST_REC); // 黑名单
	if (NULL == pFriendRec ||
		NULL == pRecommendRec || 
		NULL == pRmdFriendMidRec || 
		NULL == pApplyRec ||
		//NULL == pFriendInvitaionSendRec ||
		NULL == pBlackRec)
	{
		return;
	}

	// 清推荐中转表和推荐表
	pRecommendRec->ClearRow();
	pRmdFriendMidRec->ClearRow();
	//pFriendInvitaionSendRec->ClearRow();

	const wchar_t * pSelfName = pSelfObj->QueryWideStr("Name");
	const char * pSelfUid = pSelfObj->QueryString("Uid");
	int nLevel = pSelfObj->QueryInt("Level");

	if (StringUtil::CharIsNull(pSelfName) || 
		StringUtil::CharIsNull(pSelfUid))
	{
		return;
	}
	
	const wchar_t * pTargetName = args.WideStrVal(2);
	if (!StringUtil::CharIsNull(pTargetName))
	{
		// 不能查询自己
		if (::wcscmp(pSelfName, pTargetName) == 0)
		{
			return;
		}

		if (CheckPlayerOnline(pKernel, pTargetName))
		{
			//精确查找
			FindPlayerToRecommend(pKernel, self, args);
		}
		else
		{
			if(!CheckPlayerExist(pKernel, pTargetName))
			{
				return;
			}

			if(SnsDataModule::m_pSnsDataModule)
			{

				CVarList query_args;
				query_args << SNS_DATA_REQUEST_FORM_FRIEND_FIND;
				query_args << pTargetName;
				query_args << 1;
				query_args << "PlayerAttribute";
				SnsDataModule::OnQueryPlayerData(pKernel,self,PERSISTID(),query_args);

			}
		}
		
		return;
	}

	IRecord * pOnlinePlayerRec = m_pFriendModule->GetPubOnlineRec(pKernel);
	if (NULL == pOnlinePlayerRec)
	{
		return;
	}

	CVarList var;
	int nRow = pOnlinePlayerRec->GetRows();

	int nLevelDiff = EnvirValueModule::EnvirQueryInt(ENV_VALUE_FRIEND_RECOMM_LEVEL_DIFF);
	int nMyLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);

    LoopBeginCheck(check_a);
	for (int i = 0; i < nRow; i++)
	{
        LoopDoCheck(check_a);
		const wchar_t * pPlayerName = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_NAME);
		if (StringUtil::CharIsNull(pPlayerName))
		{
			continue;
		}

      

		// 等级相差太大
// 		int nPlayerLevel = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_LEVEL);
// 		if (nPlayerLevel < EnvirValueModule::EnvirQueryInt(ENV_VALUE_FRINED_FUNCTION_OPEN_LV)){
// 			continue;
// 		}


// 		if (std::abs(nMyLevel - nPlayerLevel) > nLevelDiff)
// 		{
// 			continue;
// 		}

		//判断是否是自己
		if (::wcscmp(pSelfName, pPlayerName) == 0)
		{
			continue;
		}

		//判定是否好友
		if (pFriendRec->FindWideStr(COLUMN_FRIEND_REC_NAME, pPlayerName) >= 0)
		{
			continue;
		}

		//是否已经在申请表里了
		if (pApplyRec->FindWideStr(COLUMN_FRIEND_APPLY_REC_NAME, pPlayerName) >= 0)
		{
			continue;
		}

		// 是不在黑名单中
		if (pBlackRec->FindWideStr(COLUMN_BLACKLIST_REC_NAME, pPlayerName) >= 0)
		{
			continue;
		}

		var.Clear();
		const char* pszUid_ = pOnlinePlayerRec->QueryString(i, PUB_COL_PLAYER_UID);
		const wchar_t* pszName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_NAME);
		int nLevel_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_LEVEL);
		int nJob_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_JOB);
		int nSex_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_SEX);
		int nPower_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_BATTLE_ABLITITY);
		const wchar_t* pszGuildName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_GUILD_NAME);
		int nVip_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_VIP);

		var << pszUid_ << pszName_ << nLevel_ << nJob_ << nSex_ << nPower_ << pszGuildName_ << nVip_;
		pRmdFriendMidRec->AddRowValue(-1, var);

		if (pRmdFriendMidRec->GetRows() >= pRmdFriendMidRec->GetRowMax())
		{
			break;
		}
	}

	int nMidRow = pRmdFriendMidRec->GetRows();
	int nMaxRow = pRecommendRec->GetRowMax();

	// 符合条件的人数小于推荐表总人数
	if (nMidRow <= nMaxRow)
	{
		CVarList varList;
        LoopBeginCheck(check_b);
		for (int i = 0; i < nMidRow; i++)
		{
            LoopDoCheck(check_b);
			var.Clear();
			pRmdFriendMidRec->QueryRowValue(i, varList);

			pRecommendRec->AddRowValue(-1, varList);
		}
	}
	else
	{
		// 取nMaxRow个随机不重复的表行索引
		CVarList varList;
		std::vector<int> iVector;
		GetNoRepeatRandomNumber(nMidRow, nMaxRow, iVector);
        LoopBeginCheck(check_c);
		for(std::vector<int>::iterator tempIt = iVector.begin(); tempIt !=iVector.end() ; ++tempIt)
		{
            LoopDoCheck(check_c);
			varList.Clear();
			pRmdFriendMidRec->QueryRowValue(*tempIt, varList);

			pRecommendRec->AddRowValue(-1, varList);
		}
	}
}

// 刷新推荐表
void FriendModule::FindPlayerToRecommend(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 好友推荐表, 好友推荐表筛选表， 在线玩家表
	IRecord * pRecommendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_REC);
	IRecord * pRmdFriendMidRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_MID_REC);
	IRecord * pOnlinePlayerRec = m_pFriendModule->GetPubOnlineRec(pKernel);
	if (NULL == pRecommendRec || 
		NULL == pRmdFriendMidRec ||
		NULL == pOnlinePlayerRec)
	{
		return;
	}

	pRecommendRec->ClearRow();
	pRmdFriendMidRec->ClearRow();

	//只有自己
	if (pOnlinePlayerRec->GetRows() < 2)
	{
		return;
	}

	//目标名字
	const wchar_t * wcTargetName = args.WideStrVal(2);
	if (StringUtil::CharIsNull(wcTargetName))
	{
		return;
	}

	CVarList var;
	int nRow = pOnlinePlayerRec->GetRows();
    LoopBeginCheck(check_d);
	for (int i = 0; i < nRow; i++)
	{
        LoopDoCheck(check_d);
		const wchar_t * pPlayerName = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_NAME);
		if (StringUtil::CharIsNull(pPlayerName))
		{
			continue;
		}
       

		//判断是否是目标玩家
		if (::wcscmp(wcTargetName, pPlayerName) == 0)
		{
            if (pRecommendRec->GetRows() >= pRecommendRec->GetRowMax())
            {
                break;
            }
			var.Clear();
			const char* pszUid_ = pOnlinePlayerRec->QueryString(i, PUB_COL_PLAYER_UID);
			const wchar_t* pszName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_NAME);
			int nLevel_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_LEVEL);
			int nJob_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_JOB);
			int nSex_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_SEX);
			int nPower_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_BATTLE_ABLITITY);
			const wchar_t* pszGuildName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_GUILD_NAME);
			int nVip_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_VIP);
			
			var << pszUid_ << pszName_ << nLevel_ << nJob_ << nSex_ << nPower_ << pszGuildName_ << nVip_;
			pRecommendRec->AddRowValue(-1, var);

			continue;
		}

		// 模糊查找
		if (::wcsstr(pPlayerName, wcTargetName) != NULL)
		{
            if (pRmdFriendMidRec->GetRows() >= pRmdFriendMidRec->GetRowMax())
            {
                break;
            }
			var.Clear();

			const char* pszUid_ = pOnlinePlayerRec->QueryString(i, PUB_COL_PLAYER_UID);
			const wchar_t* pszName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_NAME);
			int nLevel_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_LEVEL);
			int nJob_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_JOB);
			int nSex_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_SEX);
			int nPower_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_BATTLE_ABLITITY);
			const wchar_t* pszGuildName_ = pOnlinePlayerRec->QueryWideStr(i, PUB_COL_PLAYER_GUILD_NAME);
			int nVip_ = pOnlinePlayerRec->QueryInt(i, PUB_COL_PLAYER_VIP);

			var << pszUid_ << pszName_ << nLevel_ << nJob_ << nSex_ << nPower_ << pszGuildName_ << nVip_;

			pRmdFriendMidRec->AddRowValue(-1, var);

			continue;
		}
	}

	if (pRecommendRec->GetRows() < pRecommendRec->GetRowMax() &&
		pRmdFriendMidRec->GetRows() > 0)
	{
        LoopBeginCheck(check_e);
		for (int i = 0; i < pRmdFriendMidRec->GetRows(); ++i)
		{
            LoopDoCheck(check_e);
            if (pRecommendRec->GetRows() >= pRecommendRec->GetRowMax())
            {
                break;
            }
			var.Clear();
			pRmdFriendMidRec->QueryRowValue(i, var);

			pRecommendRec->AddRowValue(-1, var);
		}
	}
}

// 随机生成一定范围成的不重复的整数
void FriendModule::GetNoRepeatRandomNumber(const int nMax, const int tempNumber, std::vector<int> & iVector)
{
	//除数不能是0
	if (nMax == 0)
	{
		return;
	}
	std::vector<int>::iterator tempIt;
    LoopBeginCheck(check_f);
	for(int i = 0; i < tempNumber;)
	{
        LoopDoCheck(check_f);
		int num = rand() % nMax;
        LoopBeginCheck(check_g);
		for(tempIt = iVector.begin(); tempIt !=iVector.end() ; ++tempIt)
		{
            LoopDoCheck(check_g);
			if(*tempIt == num)
			{
				break;
			}
		}
		if(tempIt !=iVector.end())
		{
			continue;
		}
		iVector.push_back(num);
		++i;
	}
}

// 更新好友信息
void FriendModule::OnCustomFriendInfoUpdate(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return;
    }
	IRecord* pRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (NULL == pRec)
	{
		return;
	}

	int iMaxRows = pRec->GetRows();

	IRecord * pOnlinePlayerRec = m_pFriendModule->GetPubOnlineRec(pKernel);
	if (NULL == pOnlinePlayerRec)
	{
		return;
	}

	int pPlayerLevel = 0;
	int nPlayerJob = 0;
	int nPlayerSex = 0;
	int nPlayerAbility = 0;
	int teamID = 0;
	int vip = 0;
	const wchar_t * pPlayerName = NULL;
	const wchar_t * pGuildName = NULL;

    LoopBeginCheck(check_h);
	for(int nRow = 0; nRow < iMaxRows; ++nRow)
	{
        LoopDoCheck(check_h);
		const char * pPlayerUID = pRec->QueryString(nRow, COLUMN_FRIEND_REC_UID);
		int nRowsIndex = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pPlayerUID);
		if (nRowsIndex != -1)
		{
			pPlayerLevel = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_LEVEL);
			pRec->SetInt( nRow, COLUMN_FRIEND_REC_LEVEL, pPlayerLevel);
			nPlayerJob = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_JOB);
			pRec->SetInt( nRow, COLUMN_FRIEND_REC_JOB, nPlayerJob);
			nPlayerSex = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_SEX);
			pRec->SetInt( nRow, COLUMN_FRIEND_REC_SEX, nPlayerSex);
			pPlayerName = pOnlinePlayerRec->QueryWideStr(nRowsIndex, PUB_COL_PLAYER_NAME);
			pRec->SetWideStr( nRow, COLUMN_FRIEND_REC_NAME, pPlayerName);
			nPlayerAbility = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_BATTLE_ABLITITY);
			pRec->SetInt( nRow, COLUMN_FRIEND_REC_POWER, nPlayerAbility);
			pGuildName = pOnlinePlayerRec->QueryWideStr(nRowsIndex, PUB_COL_PLAYER_GUILD_NAME);
			pRec->SetWideStr( nRow, COLUMN_FRIEND_REC_GUILD_NAME, pGuildName);
			teamID = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_TEAM_ID);
			pRec->SetInt(nRow, COLUMN_FRIEND_REC_TEAM_ID, teamID);
			vip = pOnlinePlayerRec->QueryInt(nRowsIndex, PUB_COL_PLAYER_VIP);
			pRec->SetInt(nRow, COLUMN_FRIEND_REC_VIP_LEVEL, vip);
		}
	}
}

// 添加仇人
void FriendModule::OnCustomAddOnceEnemy(IKernel * pKernel, const PERSISTID & self, const PERSISTID& sender)
{
	// 自己不是玩家
	if (pKernel->Type(self) != TYPE_PLAYER)
	{
		return;
	}
    if (!pKernel->Exists(sender))
    {
        return;
    }

	PERSISTID attObj = sender;

	if (pKernel->Type(sender) == TYPE_NPC)
	{
		// 取NPC主人ID
		attObj = get_sceneobj_master(pKernel, sender);

		if (!pKernel->Exists(attObj))
		{
			return;
		}
	}

	// 不是被玩家杀死
	if (pKernel->Type(attObj) != TYPE_PLAYER)
	{
		return;
	}

	if (self == attObj)
	{
		return;
	}

	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	IGameObj * pEnemyObj = pKernel->GetGameObj(attObj);

	if (NULL == pSelfObj || NULL == pEnemyObj)
	{
		return;
	}

	const char * attUid = pEnemyObj->QueryString("Uid");

	if (StringUtil::CharIsNull(attUid))
	{
		return;
	}

	// 是否已经是仇人
	IRecord * pEnemyRecord = pSelfObj->GetRecord(FIELD_RECORD_ENEMY_REC);
	if (NULL == pEnemyRecord)
	{
		return;
	}

	int nEnemyRow = pEnemyRecord->FindString(COLUMN_ENEMY_REC_UID, attUid);
	int nHaterT = EnvirValueModule::EnvirQueryInt( ENV_VALUE_PLAYER_KILL_HATREDT );


	CVarList row_list;
	row_list << attUid
		<< pEnemyObj->QueryWideStr("Name")
		<< pEnemyObj->QueryInt("Level")
		<< pEnemyObj->QueryInt("Job")
		<< pEnemyObj->QueryInt("Sex")
		<< pEnemyObj->QueryInt("BattleAbility")
		<< pEnemyObj->QueryWideStr("GuildName")
		<< pEnemyObj->QueryString(FIELD_PROP_ACCOUNT)
		<< ONLINE
		<< nHaterT		// 仇恨值
		<< ::time(NULL)
		<< pEnemyObj->QueryInt(FIELD_PROP_VIP_LEVEL);


	CVarList info;
	info.Append(row_list, 0, 9);
	info << nHaterT << ::time(NULL) << pEnemyObj->QueryInt(FIELD_PROP_VIP_LEVEL);;

	SwornFriendAddOnceEnemy(pKernel, self, info);


	if (nEnemyRow > -1)
	{

		// 是仇人 更新仇恨值
		int nNowHaterT = pEnemyRecord->QueryInt(nEnemyRow, COLUMN_ENEMY_REC_HATRED);
		nNowHaterT += nHaterT;
		pEnemyRecord->SetInt(nEnemyRow, COLUMN_ENEMY_REC_HATRED, nNowHaterT);
		pEnemyRecord->SetInt64(nEnemyRow, COLUMN_ENEMY_REC_BE_KILL_TIME, ::time(NULL));
		return;
	}
	else
	{

		if (pEnemyRecord->GetRows() >= pEnemyRecord->GetRowMax())
		{
			//  周栋说不删的 满了删除第一条
			//pEnemyRecord->RemoveRow(0);
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17118, CVarList());
			return;
		}


		pEnemyRecord->AddRowValue(-1, row_list);
	}
	


	
	


}

// 删除仇人
void FriendModule::OnCustomDeleteEnemy(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
	const wchar_t * pTargetName = args.WideStrVal(2);

	if (StringUtil::CharIsNull(pTargetName))
	{
		return;
	}
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return;
    }

	IRecord* pRec = pSelfObj->GetRecord(FIELD_RECORD_ENEMY_REC);
	if (NULL == pRec)
	{
		return;
	}

	int nRowIndex = pRec->FindWideStr(COLUMN_ENEMY_REC_NAME, pTargetName);
	if (nRowIndex == -1)
	{
		return;
	}

	pRec->RemoveRow(nRowIndex);
}

// 删除所有仇人
void FriendModule::OnCustomDeleteAllEnemy(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return;
    }
	IRecord* pRec = pSelfObj->GetRecord(FIELD_RECORD_ENEMY_REC);
	if (NULL == pRec)
	{
		return;
	}

	pRec->ClearRow();
}

// 请求附近玩家
void FriendModule::OnCustomRequestNearby(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return;
    }
	IRecord * pNearbyRec = pSelfObj->GetRecord(FIELD_RECORD_NEARBY_RECOMMEND_REC);
	if (NULL == pNearbyRec)
	{
		return;
	}

	pNearbyRec->ClearRow();

	int sceneid = pKernel->GetSceneId();
	CVarList playerlist;

	float fNearRand = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_NEARBY_PLAYER_DISTANCE);
	pKernel->GetAroundList(self, fNearRand, TYPE_PLAYER, pNearbyRec->GetRowMax(), playerlist);

	if (playerlist.GetCount() <= 0)
	{
		return;
	}

	const wchar_t * target_name = NULL;
	PERSISTID playerid;
	IGameObj * player = NULL;
	float fDistance = 0.0f;

	std::vector<NearbyRecommend> vecNearby;

	float fSearchDistance = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_NEARBY_PLAYER_DISTANCE);
	//int nLevelDiff = EnvirValueModule::EnvirQueryInt(ENV_VALUE_FRIEND_RECOMM_LEVEL_DIFF);
	int nMyLevel = pSelfObj->QueryInt( FIELD_PROP_LEVEL );

    LoopBeginCheck(check_i);
	for (int i = 0; i < (int)playerlist.GetCount(); ++i)
	{
        LoopDoCheck(check_i);
		playerid = playerlist.ObjectVal(i);
		if (!pKernel->Exists(playerid))
		{
			continue;
		}

		if (playerid == self)
		{
			continue;
		}

		player = pKernel->GetGameObj(playerid);
		if (NULL == player)
		{
			continue;
		}
        if (player->QueryInt("PrepareRole") == ROLE_FLAG_ROBOT)
        {
            //跳过机器人
            continue;
        }
		
		target_name = player->QueryWideStr("Name");
		if (StringUtil::CharIsNull(target_name))
		{
			continue;
		}

		if (m_pFriendModule->IsFriend(pKernel,self,target_name)/* && TeamModule::IsTeamMate(pKernel,self,target_name)*/)
		{
			continue;
		}
		//距离太远
		fDistance = pKernel->Distance2D(playerid, self);
		if (fDistance > fSearchDistance)
		{
			continue;
		}

		//// 等级相差太大
		//int nPlayerLevel = player->QueryInt( FIELD_PROP_LEVEL );
		//if (std::abs(nMyLevel - nPlayerLevel) > nLevelDiff)
		//{
		//	continue;
		//}

		NearbyRecommend nearby;
		nearby.pid = playerid;
		nearby.distance = fDistance;
		vecNearby.push_back(nearby);

		if ((int)vecNearby.size() >= pNearbyRec->GetRowMax())
		{
			break;
		}
	}

	if (vecNearby.empty())
	{
		return;
	}

	std::sort(vecNearby.begin(), vecNearby.end(), FriendModule::CompareDistance);

    LoopBeginCheck(check_j);
	for (int i = 0; i < (int)vecNearby.size(); ++i)
	{
        LoopDoCheck(check_j);
		player = pKernel->GetGameObj(vecNearby[i].pid);
		if (NULL == player)
		{
			continue;
		}

		CVarList varlist;

		varlist << player->QueryString("Uid")
				<< player->QueryWideStr("Name")
				<< player->QueryInt("Level")
				<< player->QueryInt("Job")
				<< player->QueryInt("Sex")
				<< player->QueryInt("BattleAbility")
				<< player->QueryWideStr("GuildName")
				<< player->QueryInt("TeamID")
				<< player->QueryInt("VipLevel");

		pNearbyRec->AddRowValue(-1, varlist);

		if (pNearbyRec->GetRows() >= pNearbyRec->GetRowMax())
		{
			break;
		}
	}
}

// 距离比较
bool FriendModule::CompareDistance(const NearbyRecommend& first, const NearbyRecommend& second)
{
	return first.distance < second.distance;
}

// 删除推荐玩家
int FriendModule::DeleteRecommend(IKernel * pKernel, const PERSISTID & self, const wchar_t* pTargetName )
{
	if (StringUtil::CharIsNull(pTargetName))
	{
		return 0;
	}
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return 0;
    }

	IRecord* pRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_REC);
	if (NULL == pRec)
	{
		return 0;
	}

	int nRowIndex = pRec->FindWideStr(COLUMN_FRIEND_RECOMMEND_REC_NAME, pTargetName);
	if (nRowIndex == -1)
	{
		return 0;
	}

	pRec->RemoveRow(nRowIndex);

	return 1;
}

// 添加一个推荐玩家
void FriendModule::AddOneRecommend(IKernel * pKernel, const PERSISTID & self, const IVarList & args)
{
	const wchar_t * pTargetName = args.WideStrVal(2);
	const char * TargetUID = pKernel->SeekRoleUid(pTargetName);
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return;
    }

	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	IRecord * pRecommendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_REC);
    IRecord * pRmdFriendMidRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_RECOMMEND_MID_REC);
    IRecord * pFriendInvitationSendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_INVITATION_SEND_REC);
	if (NULL == pRecommendRec || NULL == pRmdFriendMidRec || NULL == pFriendInvitationSendRec || NULL ==pFriendRec)
	{
		return;
	}

	int nMidRow = pRmdFriendMidRec->GetRows();
	int nMaxRow = pRecommendRec->GetRowMax();
	int nCurRow = pRecommendRec->GetRows();

	// 推荐总表为空或当前推荐表满
	if (nMidRow <= 0 || nCurRow >= nMaxRow)
	{
		return;
	}

    LoopBeginCheck(check_k);
	for (int i = 0; i < nMidRow; ++i)
	{
        LoopDoCheck(check_k);
        if (pRecommendRec->GetRows() >= nMaxRow)
        {
            //推荐表满
            break;
        }
		int nRandRow = rand() % nMidRow;

		const char * pPlayerUID = pRmdFriendMidRec->QueryString(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_UID);
		
		if (StringUtil::CharIsNull(pPlayerUID))
		{
			continue;
		}

		// 不再推荐刚清理的玩家
		if (strcmp(pPlayerUID, TargetUID) == 0)
		{
			continue;
		}

        // 过滤已经发送邀请
        int nSendRow = pFriendInvitationSendRec->FindString(COLUMN_FRIEND_INVITATION_SEND_REC_UID, TargetUID);
        if (nSendRow >= 0)
        {
            continue;
        }

		// 在当前推荐表，好友表，黑名单跳过
		int iRow = pRecommendRec->FindString(COLUMN_FRIEND_RECOMMEND_REC_UID, pPlayerUID);
		if (iRow != -1)
		{
			continue;
		}
		iRow = pFriendRec->FindString(COLUMN_FRIEND_REC_UID, pPlayerUID);
		if (iRow != -1)
		{
			continue;
		}

		const wchar_t * pPlayerName = pRmdFriendMidRec->QueryWideStr(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_NAME);
		int nPlayerLevel = pRmdFriendMidRec->QueryInt(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_LEVEL);
		int nPlayerJob = pRmdFriendMidRec->QueryInt(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_JOB);
		int nPlayerSex = pRmdFriendMidRec->QueryInt(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_SEX);
		int nBattleAbility = pRmdFriendMidRec->QueryInt(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_POWER);
		const wchar_t *pGuildName = pRmdFriendMidRec->QueryWideStr(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_GUILD_NAME);
		int nVip_ = pRmdFriendMidRec->QueryInt(nRandRow, COLUMN_FRIEND_RECOMMEND_MID_REC_VIP_LEVEL);

		if (StringUtil::CharIsNull(pPlayerName))
		{
			continue;
		}

		CVarList var;
		var << pPlayerUID
			<< pPlayerName
			<< nPlayerLevel
			<< nPlayerJob
			<< nPlayerSex
			<< nBattleAbility
			<< pGuildName
			<< nVip_;

		pRecommendRec->AddRowValue(-1, var);
	}
}

// 发消息给sns服，请求更新好友信息
void FriendModule::OnCustomQueryFriendInfo(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (!pKernel->Exists(self) || NULL == pSelfObj)
	{
		return;
	}

	// 请求更新好友表中所有好友的信息
	IRecord* rec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (rec != NULL)
	{

		LoopBeginCheck(check_l);
		for (int i = 0; i < rec->GetRows(); i++)
		{
			LoopDoCheck(check_l);
			std::wstring friend_name = rec->QueryWideStr(i, COLUMN_FRIEND_REC_NAME);

			if (friend_name.empty())
			{
				continue;
			}

			if (SnsDataModule::m_pSnsDataModule)
			{
				CVarList query_args;
				query_args << SNS_DATA_REQUEST_FORM_FRIEND_UPDATE;
				query_args << friend_name.c_str();
				query_args << 1;
				query_args << "PlayerAttribute";
				SnsDataModule::OnQueryPlayerData(pKernel, self, PERSISTID(), query_args);
			}

			bool online = CheckPlayerOnline(pKernel, friend_name.c_str());
			int online_flag = online ? 1 : 0;
			rec->SetInt(i, COLUMN_FRIEND_REC_ONLINE, online_flag);
		}
	}

	// 请求更新仇人表中所有好友的信息
	IRecord* recEnmey = pSelfObj->GetRecord(FIELD_RECORD_ENEMY_REC);
	if (recEnmey != NULL)
	{

		LoopBeginCheck(check_l);
		for (int i = 0; i < recEnmey->GetRows(); i++)
		{
			LoopDoCheck(check_l);
			std::wstring enmey_name = recEnmey->QueryWideStr(i, COLUMN_ENEMY_REC_NAME);

			if (enmey_name.empty())
			{
				continue;
			}

			if (SnsDataModule::m_pSnsDataModule)
			{
				CVarList query_args;
				query_args << SNS_DATA_REQUEST_FORM_FRIEND_ENEMY;
				query_args << enmey_name.c_str();
				query_args << 1;
				query_args << "PlayerAttribute";
				SnsDataModule::OnQueryPlayerData(pKernel, self, PERSISTID(), query_args);
			}

			bool online = CheckPlayerOnline(pKernel, enmey_name.c_str());
			int online_flag = online ? 1 : 0;
			recEnmey->SetInt(i, COLUMN_ENEMY_REC_ONLINE, online_flag);
		}
	}

	return;
}

void FriendModule::OnCustomSendGift(IKernel* pKernel, const PERSISTID& self, const char* targetUid, int nGiftId)
{
	if (StringUtil::CharIsNull(targetUid))
	{
		return;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	const wchar_t* pwstrTargetName = pKernel->SeekRoleName(targetUid);
	if (StringUtil::CharIsNull( pwstrTargetName) )
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17006, CVarList() << pwstrTargetName);
		return;
	}
	const wchar_t* pwstrMyName = pSelf->QueryWideStr(FIELD_PROP_NAME);
	if (StringUtil::CharIsNull(pwstrMyName))
	{
		extend_warning(LOG_ERROR, "[FriendModule::OnCustomSendGift] logic error, player name is null.");
		return;
	}
	const char* pstrMyUid = pSelf->QueryString(FIELD_PROP_UID);
	if (StringUtil::CharIsNull(pstrMyUid))
	{
		extend_warning(LOG_ERROR, "[FriendModule::OnCustomSendGift] logic error, player uid is null.");
		return;
	}

	// 1. 是否是好友
	IRecord* pFriendRec = pKernel->GetRecord(self, FIELD_RECORD_FRIEND_REC);
	if (NULL == pFriendRec)
	{
		return;
	}
	int nFriendIndex = pFriendRec->FindString(COLUMN_FRIEND_REC_UID, targetUid);
	if ( nFriendIndex == -1 )
	{
		CVarList args;
		args << pwstrTargetName;
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17102, args);
		return;
	}

	if (!IsCanAddIntimacy(pKernel,pSelf, targetUid))
	{
		
		return;
	}

	// 2. 判断送花id是否存在
	ConfigFriendGiftItem* pGift = Configure<ConfigFriendGiftItem>::Find((size_t)nGiftId);
	if ( NULL == pGift )
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17119, CVarList());
		return;
	}


	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box)){
		return;
	}

	if (!ContainerModule::m_pContainerModule->TryRemoveItems(pKernel, item_box, pGift->GetItemID().c_str(), 1)){
		return;
	}

	if (!ContainerModule::m_pContainerModule->RemoveItems(pKernel, item_box, pGift->GetItemID().c_str(), 1, FUNCTION_EVENT_ID_PRESENT_GIFT)){
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17119, CVarList());
		return; 
	}





	int nFlowers = (int)pGift->GetValue();
	if (nFlowers < 1)
	{
		extend_warning(LOG_ERROR, "[FriendModule::OnCustomSendGift] gift id(%d) config error, value lower zero.", nGiftId);
		nFlowers = 1;
	}

	// 4. 开始送花
	int nScene = pKernel->GetPlayerScene(pwstrTargetName);
	if ( nScene > 0 )
	{
		// 4.1 处理送花
		CVarList args;
		args << COMMAND_FRIEND_MSG << SS_FRIEND_SEND_GIFT << pstrMyUid << nFlowers;
		pKernel->CommandByName(pwstrTargetName, args);
	}
	else
	{
		// 4.1 不在线发往sns服务器保存起来
		pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
			pKernel->GetSceneId(), targetUid,
			CVarList() << SPACE_SNS
			<< SNS_FRIEND_SPACE
			<< SNS_MSG_FRIEND_GIFT_GIVE
			<< pstrMyUid
			<< nFlowers);
	}

	// 5. 送花成功处理
	OnSendFlower(pKernel, pSelf, targetUid, nFlowers);
}

// 添加黑名单
void FriendModule::OnCustomAddBlackList(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName)
{
	if (StringUtil::CharIsNull(targetName))
	{
		return;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	const char* pszTargetUid = pKernel->SeekRoleUid(targetName);
	if (StringUtil::CharIsNull(pszTargetUid))
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17006, CVarList() << targetName);
		return;
	}

	IRecord* pBlackRec = pKernel->GetRecord(self, FIELD_RECORD_BLACKLIST_REC);
	if (NULL == pBlackRec)
	{
		return;
	}

	if (pBlackRec->GetRows() >= pBlackRec->GetRowMax())
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17121, CVarList());
		return;
	}

	int nRowIndex = pBlackRec->FindString(COLUMN_BLACKLIST_REC_UID, pszTargetUid);
	if (nRowIndex != -1)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17120, CVarList());
		return;
	}

	//if (IsFriend(pKernel, self, targetName))
	//{
	//	::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, FRI_FRIEND_IS_FRIEND, CVarList());
	//	return;
	//}

	CVarList row;
	row << pszTargetUid
		<< targetName
		<< 0
		<< 0
		<< 0;
	pBlackRec->AddRowValue(-1, row);

	//删除申请表
	CVarList var;
	var << pszTargetUid << APPLY_TYPE_FRIEND;
	m_pFriendModule->DeleteFriendApply(pKernel, self, var);
	// 如果是好友则删除
	m_pFriendModule->DeleteFriend(pKernel, self, targetName);


	// 从sns上获取其它信息
	if (SnsDataModule::m_pSnsDataModule)
	{
		CVarList query_args;
		query_args << SNS_DATA_REQUEST_FORM_FRIEND_BLACK;
		query_args << targetName;
		query_args << 1;
		query_args << "PlayerAttribute";
		SnsDataModule::OnQueryPlayerData(pKernel, self, self, query_args);
	}

	// [8/17/2016 lihl] 在黑名单中从仇人列表中删除(周栋要求)
	IRecord * pEnmeyRec = pSelf->GetRecord(FIELD_RECORD_ENEMY_REC);
	if (NULL == pEnmeyRec)
	{
		return;
	}
	int nRow = pEnmeyRec->FindWideStr(COLUMN_ENEMY_REC_NAME, targetName);
	if (nRow != -1)
	{
		pEnmeyRec->RemoveRow(nRow);
	}
}

// 删除黑名单
void FriendModule::OnCustomDeleteBlackList(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName)
{
	if (StringUtil::CharIsNull(targetName))
	{
		return;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	IRecord* pBlackRec = pKernel->GetRecord(self, FIELD_RECORD_BLACKLIST_REC);
	if (NULL == pBlackRec)
	{
		return;
	}

	int nRowIndex = pBlackRec->FindWideStr(COLUMN_BLACKLIST_REC_NAME, targetName);
	if (nRowIndex == -1)
	{
		return;
	}
	pBlackRec->RemoveRow(nRowIndex);
}

// 清除黑名单
void FriendModule::OnCustomClearBlackList(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	IRecord* pBlackRec = pKernel->GetRecord(self, FIELD_RECORD_BLACKLIST_REC);
	if (NULL == pBlackRec)
	{
		return;
	}

	pBlackRec->ClearRow();
}

// 申请确认操作
void FriendModule::OnCustomConfirmAllApply( IKernel* pKernel, const PERSISTID& self, bool bAccept )
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf) return;

	IRecord* pRecApply = pSelf->GetRecord(FIELD_RECORD_FRIEND_APPLY_REC);
	if (NULL == pRecApply) return;

	if ( bAccept )
	{
		IRecord* pRecFirds = pSelf->GetRecord(FIELD_RECORD_FRIEND_REC);
		if (NULL == pRecFirds) return;

		for (int i = pRecApply->GetRows() - 1; i >= 0; --i)
		{
			this->OnAcceptFriend(pKernel, pSelf, i, pRecApply, pRecFirds);
		}
	}
	else
	{
		for (int i = pRecApply->GetRows() - 1; i >= 0; --i)
		{
			this->OnRefuseFriend(pKernel, pSelf, i, pRecApply);
		}
	}
}

void FriendModule::OnCustomApplaySworn(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const char* targetUid = args.StringVal(2);
	
	IGameObj*pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	
	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box))
	{
		return ;
	}

	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (NULL == pFriendRec)
	{
		return ;
	}

	const wchar_t *targetName = pKernel->SeekRoleName(targetUid);
	if (!IsCanSworn(pKernel, self, targetName))
	{
		return;
	}


	int nRow = pFriendRec->FindString(COLUMN_FRIEND_REC_UID, targetUid);
	if (nRow < 0)
	{
		return;
	}
	int friendSex = pFriendRec->QueryInt(nRow, COLUMN_FRIEND_REC_SEX);
	int selfSex = pSelfObj->QueryInt(FIELD_PROP_SEX);
	

	std::string spendItemID;

	if (friendSex == pSelfObj->QueryInt(FIELD_PROP_SEX))
	{
		spendItemID = EnvirValueModule::EnvirQueryString(ENV_VALUE_SWORN_NEED_ITEM1);
	}
	else
	{
		spendItemID = EnvirValueModule::EnvirQueryString(ENV_VALUE_SWORN_NEED_ITEM2);
	}

	if (!ContainerModule::m_pContainerModule->TryRemoveItems(pKernel, item_box, spendItemID.c_str(), 1))
	{
		return;
	}

	

	IRecord *pFriendApplaySworn = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_APPLAY_SWORN_REC);
	if (pFriendApplaySworn == NULL){
		return;
	}
	int row = pFriendApplaySworn->FindWideStr(COLUMN_FRIEND_APPLAY_SWORN_REC_NAME, targetName);
	if (row < 0)
	{
		pFriendApplaySworn->AddRowValue(-1, CVarList() << targetName);
	}

	pKernel->CommandByName(targetName, CVarList() << COMMAND_FRIEND_MSG << SS_FRIEND_APPLAY_SWORN << pSelfObj->QueryWideStr(FIELD_PROP_NAME) << selfSex);

}

void FriendModule::OnCustomApplaySwornReply(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const wchar_t* targetName = args.WideStrVal(2);
	int result = args.IntVal(3);

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}
	if (!IsCanSworn(pKernel, self, targetName))
	{
		return;
	}

	pKernel->CommandByName(targetName, CVarList() << COMMAND_FRIEND_MSG << SS_FRIEND_APPLAY_SWORN_RSP << result<<pSelfObj->QueryWideStr(FIELD_PROP_NAME)<<pSelfObj->QueryInt(FIELD_PROP_SEX));

}

void FriendModule::OnCustomSeekAid(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const wchar_t* targetName = args.WideStrVal(2);
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	const wchar_t* pSelfName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
	if (pSelfObj == NULL){
		return;
	}
	    // 结义召唤cd
	int seekAidcdCdTimes = EnvirValueModule::EnvirQueryInt(ENV_VALUE_SWORN_SKI_AID_CD);

	int sceneID = pKernel->GetSceneId();
	if (StringUtil::CharIsNull(targetName))
	{

		IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
		if (NULL == pFriendRec){
			return;
		}
		bool isSuccess = false;
		for (int row = 0; row < pFriendRec->GetRows(); row++)
		{
			int relation = pFriendRec->QueryInt(row,COLUMN_FRIEND_REC_RELATION);
			if (relation == FRIEND_RELATION_SWORN)
			{
				int onLine = pFriendRec->QueryInt(row, COLUMN_FRIEND_REC_ONLINE);
				if (onLine == ONLINE)
				{

					int64_t seekAidTime = pFriendRec->QueryInt64(row, COLUMN_FRIEND_REC_SEEK_SWORN_AID_TIME);
					if (seekAidTime > ::time(NULL) && seekAidTime != 0){
						continue;
					}

					const wchar_t* friendName = pFriendRec->QueryWideStr(row, COLUMN_FRIEND_REC_NAME);
					pKernel->CustomByName(friendName, CVarList() << SERVER_CUSTOMMSG_FRIEND << S2C_SEEK_AID << pSelfName << sceneID);
					pFriendRec->SetInt64(row, COLUMN_FRIEND_REC_SEEK_SWORN_AID_TIME, ::time(NULL) + seekAidcdCdTimes);
					isSuccess = true;

				}
			}
		}

		if (isSuccess)
		{
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17122, CVarList());
		}
	}
	else
	{
		IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
		if (NULL == pFriendRec){
			return;
		}
		int row = pFriendRec->FindWideStr(COLUMN_FRIEND_REC_NAME, targetName);
		if (row < 0){
			return;
		}
		int relation = pFriendRec->QueryInt(row, COLUMN_FRIEND_REC_RELATION);
		if (relation != FRIEND_RELATION_SWORN){
			return;
		}
		int onLine = pFriendRec->QueryInt(row, COLUMN_FRIEND_REC_ONLINE);
		if (onLine == ONLINE)
		{
			int64_t seekAidTime = pFriendRec->QueryInt64(row, COLUMN_FRIEND_REC_SEEK_SWORN_AID_TIME);
			if (seekAidTime  > ::time(NULL) && seekAidTime != 0){
				return;
			}
			const wchar_t* friendName = pFriendRec->QueryWideStr(row, COLUMN_FRIEND_REC_NAME);
			pKernel->CustomByName(friendName, CVarList() << SERVER_CUSTOMMSG_FRIEND << S2C_SEEK_AID << pSelfName << sceneID);
			pFriendRec->SetInt64(row, COLUMN_FRIEND_REC_SEEK_SWORN_AID_TIME, ::time(NULL) + seekAidcdCdTimes);
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17122, CVarList());
		}
	}
}

void FriendModule::OnCustomChangeFriendTitle(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const wchar_t* targetName = args.WideStrVal(2);
	const wchar_t *desc = args.WideStrVal(3);
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (NULL == pFriendRec)
	{
		return;
	}

	int nRow = pFriendRec->FindWideStr(COLUMN_FRIEND_REC_NAME, targetName);
	if (nRow < 0){
		return;
	}
	pFriendRec->SetWideStr(nRow, COLUMN_FRIEND_REC_TITLE, desc);

}

void FriendModule::OnCustomDelSworn(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const wchar_t* swornName = args.WideStrVal(2);
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}
	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (NULL == pFriendRec){
		return;
	}

	int nRow = pFriendRec->FindWideStr(COLUMN_FRIEND_REC_NAME, swornName);
	if (nRow < 0){
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17123, CVarList() << swornName);
		return;
	}

	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box)){
		return;
	}

	int spend = EnvirValueModule::EnvirQueryInt(ENV_VALUE_DEL_SWORN_NEEN_COPPER);

	if (!CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, CAPITAL_GOLD, spend))
	{
		return;
	}

	int relation = pFriendRec->QueryInt(nRow, COLUMN_FRIEND_REC_RELATION);
	if (relation != FRIEND_RELATION_SWORN)
	{
		return;
	}

	int friendSex = pFriendRec->QueryInt(nRow, COLUMN_FRIEND_REC_SEX);

	CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, CAPITAL_GOLD, spend, FUNCTION_EVENT_ID_FRIEND);

	int intimacyLevel = EnvirValueModule::EnvirQueryInt(ENV_VALUE_NOMAL_FRIEND_INTIMACY_MAX);
	pFriendRec->SetInt(nRow, COLUMN_FRIEND_REC_RELATION, FRIEND_RELATION_NOMAL);
	pFriendRec->SetInt(nRow, COLUMN_FRIEND_REC_INTIMACY_LEVEL, intimacyLevel);

	if (pKernel->GetPlayerScene(swornName) > 0)
	{
		pKernel->CommandByName(swornName, CVarList() << COMMAND_FRIEND_MSG << SS_FRIEND_DEL_SWORN << pSelfObj->QueryWideStr(FIELD_PROP_NAME));
	}
	else
	{
		const char* swornUid = pKernel->SeekRoleUid(swornName);
		pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
			pKernel->GetSceneId(), swornUid,
			CVarList() << SPACE_SNS
			<< SNS_FRIEND_SPACE
			<< SNS_MSG_FRIEND_ADD_DEL_SWORN
			<< pSelfObj->QueryWideStr(FIELD_PROP_NAME));
	}
	bool  isSameSex = friendSex == pSelfObj->QueryInt(FIELD_PROP_SEX) ? true : false;
	if (isSameSex)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17115, CVarList() << swornName);
	}
	else
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17116, CVarList() << swornName);
	}
	LogModule::m_pLogModule->OnSworn(pKernel, self, swornName, isSameSex, FRIEND_SOWRN_DEL);
}


void FriendModule::OnCustomAid(IKernel*pKernel, const PERSISTID& self, const IVarList & args)
{
	const wchar_t* targetName = args.WideStrVal(2);

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	IRecord * pFriendRec = pSelfObj->GetRecord(FIELD_RECORD_FRIEND_REC);
	if (NULL == pFriendRec){
		return;
	}

	int nRow = pFriendRec->FindWideStr(COLUMN_FRIEND_REC_NAME, targetName);
	if (nRow < 0){
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17123, CVarList() << targetName);
		return;
	}

	int relation = pFriendRec->QueryInt(nRow, COLUMN_FRIEND_REC_RELATION);
	if (relation != FRIEND_RELATION_SWORN)
	{
		return;
	}

	if (pKernel->GetPlayerScene(targetName) <= 0){
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17007, CVarList() << targetName);
	}
	int intimacyLevel = pFriendRec->QueryInt(nRow,COLUMN_FRIEND_REC_INTIMACY_LEVEL);

	ConfigSwornBuff* pUpgradeNext = Configure<ConfigSwornBuff>::Find(intimacyLevel);
	if (pUpgradeNext != NULL)
	{
		FightInterfaceInstance->AddBuffer(pKernel, self, self, pUpgradeNext->GetBuffID().c_str());
	}
	pKernel->CommandByName(targetName, CVarList() << COMMAND_FRIEND_MSG << SS_FRIEND_AID_TARGET_POS_REQ << pSelfObj->QueryWideStr(FIELD_PROP_NAME));
}

void FriendModule::OnCustomAddFriendByOneKey(IKernel*pKernel, const PERSISTID& self, const IVarList &args)
{

	IGameObj * pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj){
		return;
	}

	const wchar_t* pSelfName = pSelfObj->QueryWideStr("Name");
	const char * pSelfUid = pKernel->SeekRoleUid(pSelfName);
	int level = pSelfObj->QueryInt("Level");
	int fightcap = pSelfObj->QueryInt("BattleAbility");
	int job = pSelfObj->QueryInt("Job");
	int sex = pSelfObj->QueryInt("Sex");
	int vipLv = pSelfObj->QueryInt("VipLevel");
	const wchar_t *guild_name = pSelfObj->QueryWideStr("GuildName");

	if (StringUtil::CharIsNull(pSelfUid))
	{
		return;
	}
	if (StringUtil::CharIsNull(pSelfName))
	{
		return;
	}

	int applyType = APPLY_TYPE_FRIEND;
	for (size_t index = 2; index < args.GetCount(); index++)
	{

		const wchar_t* pTargetName = args.WideStrVal(2);
		const char * pTargetUid = pKernel->SeekRoleUid(pTargetName);
		if (StringUtil::CharIsNull(pTargetUid) || StringUtil::CharIsNull(pTargetUid) || wcscmp(pSelfName, pTargetName) == 0){
			continue;
		}
		// 是否满足好友条件
		if (!m_pFriendModule->CanBeFirend(pKernel, self, pSelfName ,pTargetName, 100)){
			continue;
		}

		// 把此玩家加入已邀请的表中
		m_pFriendModule->SetPlayerInvited(pKernel, self, pTargetUid);
		// 添加完推荐的好友时再增加一个
		if (m_pFriendModule->DeleteRecommend(pKernel, self, pTargetName) > 0)
		{
			m_pFriendModule->AddOneRecommend(pKernel, self, args);
		}

		//判断玩家是否在线
		if (pKernel->GetPlayerScene(pTargetName) <= 0)
		{
			// 目标不在线，发送消息给SNS服务器，记录邀请
			pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
				pKernel->GetSceneId(), pTargetUid,
				CVarList() << SPACE_SNS
				<< SNS_FRIEND_SPACE
				<< SNS_MSG_FRIEND_APPLY
				<< pSelfUid
				<< pSelfName
				<< pTargetUid
				<< pTargetName
				<< level
				<< job
				<< sex
				<< fightcap
				<< guild_name
				<< applyType
				<< vipLv);
		}
		else
		{
			//玩家请求好友未处理项
			SSFrienMsg subMsg = SS_FRIEND_APPLY;
			CVarList var;
			var << COMMAND_FRIEND_MSG
				<< subMsg
				<< pSelfUid
				<< pSelfName
				<< level
				<< job
				<< sex
				<< fightcap
				<< guild_name
				<< applyType
				<< vipLv;
			pKernel->CommandByName(pTargetName, var);
		}
	}


	// 通知客户端，邀请已发出，请等待
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17101, CVarList());
}

