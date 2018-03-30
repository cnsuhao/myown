//--------------------------------------------------------------------
// 文件名:		GuildHelper.cpp
// 内  容:		guild helper
// 说  明:		
// 创建日期:	2017年08月03日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#include "GuildHelper.h"
#include "utils/util_func.h"
#include "utils/string_util.h"
#include "FsGame/Define/PubDefine.h"


const wchar_t c_helper_space_split_str[] = L"_";

GuildHelper::GuildHelper()
{
	//
}
GuildHelper::~GuildHelper()
{
	//
}
bool GuildHelper::InitPubData(IKernel* pKernel)
{
	//0. for server id
	GuildHelper::GetServerId(::util_int64_as_widestr((int64_t)pKernel->GetServerId()).c_str());

	//1. PUBSPACE_GUILD
	IPubSpace* pPubSpace = GuildHelper::GetSpaceGuild(pKernel->GetPubSpace(PUBSPACE_GUILD));
	Assert(pPubSpace != NULL);
	
	std::wstring dataName = GUILD_PUB_DATA + GuildHelper::GetServerId();
	IPubData* pubData = GuildHelper::SpaceGuild_PubData(pPubSpace->GetPubData(dataName.c_str()));
	Assert(pubData != NULL);

	//2. switch pub
	pPubSpace = GuildHelper::GetSpaceDomain(pKernel->GetPubSpace(PUBSPACE_DOMAIN));
	Assert(pPubSpace != NULL);
	return true;
}
bool GuildHelper::InitPubDataEx(IPubKernel* pPubKernel)
{
	//0. for server id
	GuildHelper::GetServerId(::util_int64_as_widestr((int64_t)pPubKernel->GetServerId()).c_str());

	//1. PUBSPACE_GUILD
	IPubSpace* pPubSpace = GuildHelper::GetSpaceGuild(pPubKernel->GetPubSpace(PUBSPACE_GUILD));
	Assert(pPubSpace != NULL);
	std::wstring dataName = GUILD_PUB_DATA + GuildHelper::GetServerId();
	IPubData* pubData = GuildHelper::SpaceGuild_PubData(pPubSpace->GetPubData(dataName.c_str()));
	Assert(pubData != NULL);

	//2. switch pub
	pPubSpace = GuildHelper::GetSpaceDomain(pPubKernel->GetPubSpace(PUBSPACE_DOMAIN));
	Assert(pPubSpace != NULL);
	return true;
}
const std::wstring& GuildHelper::GetServerId(const wchar_t* server_id)
{
	static std::wstring g_server_id;
	if (server_id != NULL)
	{
		g_server_id = server_id;
	}
	if (StringUtil::CharIsNull(g_server_id.c_str()))
	{
		Assert(0);
	}
	return g_server_id;
}
IPubSpace* GuildHelper::GetSpaceDomain(IPubSpace* pubSpace)
{
	static IPubSpace* g_space_domain;
	if (pubSpace != NULL)
	{
		g_space_domain = pubSpace;
	}
	if (g_space_domain == NULL)
	{
		Assert(0);
	}
	return g_space_domain;
}
IPubSpace* GuildHelper::GetSpaceGuild(IPubSpace* pubSpace)
{
	static IPubSpace* g_space_guild;
	if (pubSpace != NULL)
	{
		g_space_guild = pubSpace;
	}
	if (g_space_guild == NULL)
	{
		Assert(0);
	}
	return g_space_guild;
}
const wchar_t* GuildHelper::Parse_SpaceNameById(const wchar_t* space_name, int server_id, bool has_split_string)
{
	static std::wstring parse_space_name;
	parse_space_name = space_name;
	if (has_split_string)
	{
		parse_space_name += c_helper_space_split_str;
	}
	parse_space_name += ::util_int64_as_widestr((int64_t)server_id).c_str();
	return parse_space_name.c_str();
}
const wchar_t* GuildHelper::Parse_SpaceName(const wchar_t* space_name, bool has_server_id, bool has_split_string)
{
	static std::wstring parse_space_name_ex;
	if (!has_server_id)
	{
		return space_name;
	}
	parse_space_name_ex = space_name;
	if (has_split_string)
	{
		parse_space_name_ex += c_helper_space_split_str;
	}
	parse_space_name_ex += GuildHelper::GetServerId();
	return parse_space_name_ex.c_str();
}
IPubData* GuildHelper::SpaceGuild_PubData(IPubData* pubData)
{
	static IPubData* g_guild_pub_data;
	if (pubData != NULL)
	{
		g_guild_pub_data = pubData;
	}
	if (g_guild_pub_data == NULL)
	{
		Assert(0);
	}
	return g_guild_pub_data;
}
bool GuildHelper::SpaceGuild_Data(const wchar_t* data_name, IPubData*& guild_data, bool has_server_id)
{
	guild_data = NULL;
	if (StringUtil::CharIsNull(data_name))
	{
		return false;
	}
	if (has_server_id)
	{
		std::wstring data_name_new = data_name + GuildHelper::GetServerId();
		guild_data = GuildHelper::GetSpaceGuild()->GetPubData(data_name_new.c_str());
	}
	else
	{
		guild_data = GuildHelper::GetSpaceGuild()->GetPubData(data_name);
	}
	
	if (guild_data == NULL)
	{
		return false;
	}
	return true;
}
bool GuildHelper::SpaceDomain_Data(const wchar_t* data_name, IPubData*& domain_data, bool has_server_id)
{
	domain_data = NULL;
	if (StringUtil::CharIsNull(data_name))
	{
		return false;
	}
	if (has_server_id)
	{
		std::wstring data_name_new = data_name + GuildHelper::GetServerId();
		domain_data = GuildHelper::GetSpaceDomain()->GetPubData(data_name_new.c_str());
	}
	else
	{
		domain_data = GuildHelper::GetSpaceDomain()->GetPubData(data_name);
	}

	if (domain_data == NULL)
	{
		return false;
	}
	return true;
}
bool GuildHelper::GetGuildIdent(const wchar_t* guildName, int& target_ident, const wchar_t*& target_alias)
{
	target_ident = 0;
	target_alias = L"";
	IPubData* guild_pub = GuildHelper::SpaceGuild_PubData();
	IRecord * target_record = guild_pub->GetRecord(GUILD_SYSMBOL_REC);
	if (target_record == NULL)
	{
		return false;
	}
	int row = target_record->FindWideStr(GUILD_SYSMBOL_REC_COL_GUILD_NAME, guildName);
	if (row < 0)
	{
		return false;
	}
	target_ident = target_record->QueryInt(row, GUILD_SYSMBOL_REC_COL_IDENTIFYING);
	target_alias = target_record->QueryWideStr(row, GUILD_SYSMBOL_REC_COL_SHORT_NAME);
	return true;
}
bool GuildHelper::GetGuildCaptain(IPubKernel* pPubKernel, const wchar_t* guild_name, std::wstring& guild_captain)
{
	guild_captain = L"";
	if (StringUtil::CharIsNull(guild_name))
	{
		return false;
	}
	//查询工会会长
	IPubData* target_guild = NULL;
	IRecord * target_member = NULL;
	if (!GuildHelper::SpaceGuild_Data(guild_name, target_guild, true))
	{
		return false;
	}
	target_member = target_guild->GetRecord(GUILD_MEMBER_REC);
	if (target_member == NULL)
	{
		return false;
	}
	int target_index = target_member->FindInt(GUILD_MEMBER_REC_COL_POSITION, GUILD_POSITION_CAPTAIN);
	if (target_index < 0)
	{
		return false;
	}
	guild_captain = target_member->QueryWideStr(target_index, GUILD_MEMBER_REC_COL_NAME);
	return true;
}
bool GuildHelper::GetGuildListRec(const wchar_t* guildName, IRecord*& p_record, int& p_row)
{
	p_record = NULL;
	p_row = -1;
	IPubData* guild_pub = GuildHelper::SpaceGuild_PubData();
	p_record = guild_pub->GetRecord(GUILD_LIST_REC);
	if (p_record == NULL)
	{
		return false;
	}
	p_row = p_record->FindWideStr(GUILD_LIST_REC_COL_NAME, guildName);
	if (p_row < 0)
	{
		return false;
	}
	return true;
}


bool util_text_format_nation(int i_nation, ICacheString& dst_string)
{
	dst_string.Clear();
	//if (i_nation == NATION_JIANGHU || i_nation == NATION_COURT)
	if (i_nation == 1 || i_nation == 2)
	{
		dst_string.SetFormat("Nation%d", i_nation); // 【江湖】 已有
		return true;
	}
#ifdef _DEBUG
	Assert(0);
#endif
	return false;
}
//Town10    【葬剑谷】
//Town11    【金陵】
//Town12    【凌风阁】
//Town20    【天一城】
//Town21    【灵犀岛】
//Town22    【苍琅山】
bool util_text_format_town_name(int i_town, ICacheString& dst_string)
{
	dst_string.Clear();
	if (i_town == 10 || i_town == 11 || i_town == 12 || i_town == 20 || i_town == 21 || i_town == 22)
	{
		dst_string.SetFormat("Town%d", i_town); //天一城
		return true;
	}
#ifdef _DEBUG
	Assert(0);
#endif
	return false;
}
bool util_text_format_town_nation(int i_town, ICacheString& dst_string)
{
	return util_text_format_nation((int)(i_town / 10), dst_string);
}
//TownTitle10  【总舵】
//TownTitle11  【分舵】
//TownTitle12  【分舵】
//TownTitle20  【总舵】
//TownTitle21  【分舵】
//TownTitle22  【分舵】
bool util_text_format_town_title(int i_town, ICacheString& dst_string)
{
	dst_string.Clear();
	if (i_town == 10 || i_town == 11 || i_town == 12 || i_town == 20 || i_town == 21 || i_town == 22)
	{
		dst_string.SetFormat("TownTitle%d", i_town); //【总舵】
		return true;
	}
#ifdef _DEBUG
	Assert(0);
#endif
	return false;
}
//TownPosition10 【锦衣卫总指挥使】 
//TownPosition11 【锦衣卫副指挥使】
//TownPosition12 【锦衣卫副指挥使】
//TownPosition20 【总盟主】 
//TownPosition21 【副盟主】
//TownPosition22 【副盟主】
bool util_text_format_town_position(int i_town, ICacheString& dst_string)
{
	dst_string.Clear();
	if (i_town == 10 || i_town == 11 || i_town == 12 || i_town == 20 || i_town == 21 || i_town == 22)
	{
		dst_string.SetFormat("TownPosition%d", i_town); //锦衣卫指挥使
		return true;
	}
#ifdef _DEBUG
	Assert(0);
#endif
	return false;
}