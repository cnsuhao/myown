//--------------------------------------------------------------------
// 文件名:		GuildInterface.cpp
// 内  容:		公会系统-接口函数
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "GuildModule.h"
#include "../Define/PubDefine.h"
#include "../Define/GuildDefine.h"
#include "utils/util_func.h"
#include "utils/string_util.h"
#include "server/LoopCheck.h"

// 公会是否已经存在
bool GuildModule::IsGuildExist(IKernel* pKernel, const wchar_t* guildName)
{
    if (pKernel == NULL || !IsWidStrValid(guildName))
    {
        return false;
    }

    IPubData* pGuildPubData = GetPubData(pKernel, GUILD_PUB_DATA);
    if (pGuildPubData == NULL)
    {
        return false;
    }

    IRecord* pGuildList = pGuildPubData->GetRecord(GUILD_LIST_REC);
    if (pGuildList == NULL)
    {
        return false;
    }

    int row = pGuildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
    return (row >= 0);
}

// 获取公会的数据项
IPubData* GuildModule::GetPubData(IKernel* pKernel, const wchar_t* name)
{
    if (pKernel == NULL || StringUtil::CharIsNull(name))
    {
        return NULL;
    }

    IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_GUILD);
    if (pPubSpace == NULL)
    {
        return NULL;
    }

    std::wstring dataName = name + ::util_int64_as_widestr(pKernel->GetServerId());
    return pPubSpace->GetPubData(dataName.c_str());
}

// 获取公会公有数据记录
IRecord* GuildModule::GetPubRecord(IKernel* pKernel, const wchar_t* pubDataName, const char* recName)
{
    IPubData* pubData = GetPubData(pKernel, pubDataName);
    return (pubData != NULL) ? pubData->GetRecord(recName) : NULL;
}

// 获得玩家所在公会名称
const wchar_t* GuildModule::GetPlayerGuildName(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return L"";
	}

    const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
    if (!IsGuildExist(pKernel, guildName))
    {
        return L"";
    }

    return guildName;
}

// 获取公会表格
IRecord* GuildModule::GetGuildRecord(IKernel* pKernel, const PERSISTID& self, int& row)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return NULL;
	}

    const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
    return GetGuildRecord(pKernel, guildName, row );
}

// 获取公会表格
IRecord* GuildModule::GetGuildRecord(IKernel* pKernel, const wchar_t* guildName, int& row)
{
	if (!IsWidStrValid(guildName))
	{
		return NULL;
	}

	IPubData* pGuildPubData = GetPubData(pKernel, GUILD_PUB_DATA);
	if (pGuildPubData == NULL)
	{
		return NULL;
	}
	IRecord* pGuildRecord = pGuildPubData->GetRecord(GUILD_LIST_REC);
	if (pGuildRecord == NULL)
	{
		return NULL;
	}
	row = pGuildRecord->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (row < 0)
	{
		return NULL;
	}

	return pGuildRecord;
}

// 获取成员表格记录
IRecord* GuildModule::GetMemberRecord(IKernel* pKernel, const PERSISTID& self, int& row)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return NULL;
	}
	const wchar_t* playerName = pSelfObj->QueryWideStr("Name");
	if (!IsWidStrValid(playerName))
	{
		return NULL;
	}

    const wchar_t* guildName = pSelfObj->QueryWideStr("GuildName");
    IRecord* pMemberRecord = GetMemberRecord( pKernel, guildName );
    if (pMemberRecord == NULL)
    {
        return NULL;
    }
    row = pMemberRecord->FindWideStr(GUILD_MEMBER_REC_COL_NAME, playerName);
    if (row < 0)
    {
        return NULL;
    }

    return pMemberRecord;
}

// 获取成员表格记录
IRecord* GuildModule::GetMemberRecord(IKernel* pKernel, const wchar_t* guildName)
{
	if (!IsWidStrValid(guildName))
	{
		return NULL;
	}

	IPubData* pGuildData = GetPubData(pKernel, guildName);
	if (pGuildData == NULL)
	{
		return NULL;
	}
	IRecord* pMemberRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
	if (pMemberRecord == NULL)
	{
		return NULL;
	}

	return pMemberRecord;
}

//获取帮主UID
const char* GuildModule::GetGuildCaptainUid(IKernel* pKernel, const wchar_t *guildName)
{
    if (StringUtil::CharIsNull(guildName))
    {
        return "";
    }
    IRecord* guildList = GetPubRecord(pKernel, GUILD_PUB_DATA, GUILD_LIST_REC);
    if (guildList == NULL)
    {
        return "";
    }
    int row = guildList->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
    if (row < 0)
    {
        return "";
    }
    const char *captainName = guildList->QueryString(row, GUILD_LIST_REC_COL_CAPTAIN_UID);
    return captainName;
}


bool GuildModule::GuildShortNameExist(IKernel*pKernel, const wchar_t* shortName)
{

	IPubData* pGuildPubData = GetPubData(pKernel, GUILD_PUB_DATA);
	if (pGuildPubData == NULL)
	{
		return false;
	}

	IRecord* pGuildSysmbol = pGuildPubData->GetRecord(GUILD_SYSMBOL_REC);
	if (pGuildSysmbol == NULL)
	{
		return false;
	}

	int row = pGuildSysmbol->FindWideStr(GUILD_SYSMBOL_REC_COL_SHORT_NAME, shortName);
	return (row >= 0);
}

