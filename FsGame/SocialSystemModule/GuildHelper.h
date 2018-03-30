//--------------------------------------------------------------------
// 文件名:		GuildHelper.h
// 内  容:		guild helper
// 说  明:		
// 创建日期:	2017年08月03日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#ifndef __HELPER_GUILD_H__
#define	__HELPER_GUILD_H__
#include "Fsgame/Define/header.h"
#include "server/IPubKernel.h"
#include "FsGame/Define/GuildDefine.h"
#include "utils/cache/ICacheString.h"
class GuildHelper
{
public:
	GuildHelper();
	~GuildHelper();
public:
	//初始化公共数据
	static bool InitPubData(IKernel* pKernel);
	static bool InitPubDataEx(IPubKernel* pPubKernel);
public:
	// (inc)for server id
	static const std::wstring& GetServerId(const wchar_t* server_id = NULL);
	// (inc)for space
	static IPubSpace* GetSpaceDomain(IPubSpace* pubSpace = NULL);
	static IPubSpace* GetSpaceGuild(IPubSpace* pubSpace = NULL);
public:
	// 解析域名空间
	static const wchar_t* Parse_SpaceNameById(const wchar_t* space_name, int server_id, bool has_split_string = true);
	static const wchar_t* Parse_SpaceName(const wchar_t* space_name, bool has_server_id = true,bool has_split_string=true);
public:
	//(inc)工会公共数据
	static IPubData* SpaceGuild_PubData(IPubData* pubData = NULL);
	//(inc)查找guild数据
	static bool SpaceGuild_Data(const wchar_t* data_name, IPubData*& guild_data,bool has_server_id=false);
public:
	//(inc)查找domain数据
	static bool SpaceDomain_Data(const wchar_t* data_name, IPubData*& domain_data, bool has_server_id = false);
public:
	//for 其他具体的业务，逻辑表操作
	//工会：返回工会图标，简称
	static bool GetGuildIdent(const wchar_t* guildName, int& target_ident, const wchar_t*& target_alias);
	//工会： 查找工会会长
	static bool GetGuildCaptain(IPubKernel* pPubKernel, const wchar_t* guild_name, std::wstring& guild_captain);
	//工会：返回公会所在：公会列表,行号
	static bool GetGuildListRec(const wchar_t* guildName, IRecord*& p_record, int& p_row);
};

//客户端文言 【国际化管理】
//cs 参考： public static string TranslateNation(string value)
//format(客户端文言): 阵营
bool util_text_format_nation(int i_nation, ICacheString& dst_string);
//format(客户端文言): 组织城池名
bool util_text_format_town_name(int i_town, ICacheString& dst_string);
//format(客户端文言): 组织城池阵营
bool util_text_format_town_nation(int i_town, ICacheString& dst_string);
//format(客户端文言): 组织城池标题：总舵，分舵
bool util_text_format_town_title(int i_town, ICacheString& dst_string);
//format(客户端文言): 组织城池官职
bool util_text_format_town_position(int i_town, ICacheString& dst_string);

#endif