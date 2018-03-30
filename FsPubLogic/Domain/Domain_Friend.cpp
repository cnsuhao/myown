//--------------------------------------------------------------------
// 文件名:      Domain_Friend.cpp
// 内  容:      好友域表
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include <algorithm>
#include "Domain_Friend.h"
#include "../../FsGame/Define/PubDefine.h"
#include "../../utils/extend_func.h"
#include "../../FsGame/Define/FriendDefine.h"

Domain_Friend::Domain_Friend() : IDomainData(L"", 0)
{
}

const std::wstring & Domain_Friend::GetDomainName(IPubKernel * pPubKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pPubKernel->GetServerId();
		SWPRINTF_S(wstr, DOMAIN_FRIEND_RECOMMEND_REC, server_id);
		m_domainName = wstr;
	}

	return m_domainName;
}

bool Domain_Friend::GetSave()
{
	return false;
}

int Domain_Friend::OnCreate(IPubKernel * pPubKernel, IPubSpace * pPubSpace)
{
	return OnLoad(pPubKernel, pPubSpace);
}

int Domain_Friend::OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
	IPubData * pFriendData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());

	if (NULL == pFriendData)
	{
		return 1;
	}

	if (!pFriendData->FindRecord(PUB_ONLINE_PLAYER_REC))
	{
		CVarList var;
		var << VTYPE_STRING << VTYPE_WIDESTR << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_INT << VTYPE_WIDESTR << VTYPE_INT << VTYPE_INT;
		pFriendData->AddRecord(PUB_ONLINE_PLAYER_REC, 0, PUB_COL_PLAYER_MAX, var);
	}

	pFriendData->SetRecordSave(PUB_ONLINE_PLAYER_REC, false);

	return 1;
}

int Domain_Friend::OnMessage(IPubKernel * pPubKernel, IPubSpace * pPubSpace, int source_id, int scene_id, const IVarList & msg)
{
	const std::wstring & name = GetDomainName(pPubKernel);
	IPubData * pFriendData = pPubSpace->GetPubData(name.c_str());
	if (!pFriendData)
	{
		return 1;
	}

	IRecord * pOnlinePlayerRec = pFriendData->GetRecord(PUB_ONLINE_PLAYER_REC);
	if (NULL == pOnlinePlayerRec)
	{
		return 1;
	}

	const int msg_type = msg.IntVal(2);
	int nStartIndex = 2;
	switch (msg_type)
	{
	case FRIEND_SEVER_PUB_ADD_ONLINE_PLAYER_INFO: // 添加在线玩家信息
		{
			CVarList valueList;
			const char * uid = msg.StringVal(++nStartIndex);
			const wchar_t * pPlayerName = msg.WideStrVal(++nStartIndex);
			int nLevel = msg.IntVal(++nStartIndex);
			int nJob = msg.IntVal(++nStartIndex);
			int nSex = msg.IntVal(++nStartIndex);
			int nAbility = msg.IntVal(++nStartIndex);
			const wchar_t *pGuildName = msg.WideStrVal(++nStartIndex);
			int teamID = msg.IntVal(++nStartIndex);
			int vip = msg.IntVal(++nStartIndex);
			valueList << uid << pPlayerName << nLevel << nJob << nSex << nAbility << pGuildName << teamID<<vip;
			//如果已经在线
			int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, uid);
			if (nRow >= 0)
			{
				pOnlinePlayerRec->SetRowValue(nRow, valueList);
			}
			else
			{
				pOnlinePlayerRec->AddRowValue(-1, valueList);
			}
		}
		break;
	case FRIEND_SEVER_PUB_DEL_ONLINE_PLAYER_INFO: // 删除在线玩家信息
		{
			const char * pPlayerUid = msg.StringVal(++nStartIndex);
			int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pPlayerUid);
			if (nRow < 0)
			{
				return 1;
			}

			pOnlinePlayerRec->RemoveRow(nRow);
		}
		break;
	case FRIEND_SEVER_PUB_SET_LEVEL_INFO: // 设置等级
		{
			const char * pPlayerUid = msg.StringVal(++nStartIndex);
			int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pPlayerUid);
			if (nRow < 0)
			{
				return 1;
			}

			int nLevel = msg.IntVal(++nStartIndex);
			pOnlinePlayerRec->SetInt(nRow, PUB_COL_PLAYER_LEVEL, nLevel);
		}
		break;
	case FRIEND_SEVER_PUB_SET_BATTLEABILITY_INFO: // 设置战斗力
		{
			const char * pUid = msg.StringVal(++nStartIndex);
			int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pUid);
			if (nRow < 0)
			{
				return 1;
			}

			int nAbility = msg.IntVal(++nStartIndex);
			pOnlinePlayerRec->SetInt(nRow, PUB_COL_PLAYER_BATTLE_ABLITITY, nAbility);
		}
		break;
	case FRIEND_SEVER_PUB_SET_GUILDNAME_INFO: // 设置帮会名
		{
			const char * pUid = msg.StringVal(++nStartIndex);
			int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pUid);
			if (nRow < 0)
			{
				return 1;
			}

			const wchar_t *pGuildName = msg.WideStrVal(++nStartIndex);
			pOnlinePlayerRec->SetWideStr(nRow, PUB_COL_PLAYER_GUILD_NAME, pGuildName);
	}break;
	case FRIEND_SEVER_PUB_SET_TEAM_ID:
	{
		const char * pUid = msg.StringVal(++nStartIndex);
		int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pUid);
		if (nRow < 0)
		{
			return 1;
		}
		int teamID = msg.IntVal(++nStartIndex);
		pOnlinePlayerRec->SetInt(nRow, PUB_COL_PLAYER_TEAM_ID, teamID);
	}break;
	case FRIEND_SEVER_PUB_SET_VIP:
	{
		const char * pUid = msg.StringVal(++nStartIndex);
		int nRow = pOnlinePlayerRec->FindString(PUB_COL_PLAYER_UID, pUid);
		if (nRow < 0)
		{
			return 1;
		}
		int vipLv = msg.IntVal(++nStartIndex);
		pOnlinePlayerRec->SetInt(nRow, PUB_COL_PLAYER_VIP, vipLv);
	}break;
	default:
		break;
	}

	return 1;
}
