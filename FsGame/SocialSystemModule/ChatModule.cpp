//----------------------------------------------------------
// 文件名:      ChatModule.cpp
// 内  容:      聊天模块
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#include "ChatModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/CoolDownDefine.h"
#include "FsGame/SystemFunctionModule/ChannelModule.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/CommonModule/SnsDataModule.h"
#include "FsGame/Define/SnsDataDefine.h"
#include "utils/XmlFile.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "public/CoreFile.h"
#include "time.h"

#ifndef FSROOMLOGIC_EXPORTS
#include "CommonModule/ReLoadConfigModule.h"
#endif // FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/FunctionEventModule.h"
#include "Define/Classes.h"
#include "Interface/FightInterface.h"
#include "Define/Fields.h"
#include "Define/TeamDefine.h"

// 模块定义
ChatModule* ChatModule::m_pChatModule = NULL;
ContainerModule* ChatModule::m_pContainerModule = NULL;
CoolDownModule* ChatModule::m_pCDModule = NULL;

#ifndef FSROOMLOGIC_EXPORTS
SnsDataModule* ChatModule::m_pSnsDataModule = NULL;
ChannelModule* ChatModule::m_pChannelModule = NULL;
LogModule* ChatModule::m_pLogModule = NULL;
#endif // FSROOMLOGIC_EXPORTS

#define CHAT_CONFIG_URL "ini/SocialSystem/Chat/ChatRule.xml"
//加载配置
int nx_reload_itemid_time__config( void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 如果指针存在，表明模块已创建
	if (NULL != ChatModule::m_pChatModule)
	{
		ChatModule::m_pChatModule->LoadResource(pKernel);
	}

	return 0;
}

//加载配置
int nx_reload_itemid_badwords__config( void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 如果指针存在，表明模块已创建
	if (NULL != ChatModule::m_pChatModule)
	{
		ChatModule::m_pChatModule->LoadCheckWords(pKernel);
	}

	return 0;
}

// 初始化
bool ChatModule::Init(IKernel* pKernel)
{
	m_pChatModule = this;
	m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pCDModule = (CoolDownModule *)pKernel->GetLogicModule("CoolDownModule");

	Assert(m_pChatModule != NULL
		&& m_pContainerModule != NULL
		&& m_pCDModule != NULL );

#ifndef FSROOMLOGIC_EXPORTS
	m_pSnsDataModule = dynamic_cast<SnsDataModule*>(pKernel->GetLogicModule("SnsDataModule"));
	m_pChannelModule = (ChannelModule*)pKernel->GetLogicModule("ChannelModule");
	//m_pGuildModule = (GuildModule*)pKernel->GetLogicModule("GuildModule");
	m_pLogModule = (LogModule*)pKernel->GetLogicModule("LogModule");

	Assert(m_pSnsDataModule != NULL
		&& m_pChannelModule != NULL
/*		&& m_pGuildModule != NULL*/
		&& m_pLogModule != NULL);
#endif // FSROOMLOGIC_EXPORTS

	pKernel->AddEventCallback("player", "OnRecover", ChatModule::OnRecover, 1);
	pKernel->AddEventCallback("player", "OnContinue", ChatModule::OnContinue);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CHAT, ChatModule::OnCustomChat);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CHAT_QUERY, ChatModule::OnCustomQuery);

	pKernel->AddIntCommandHook("player", COMMAND_CHAT, ChatModule::OnCommandChat);

	DECL_HEARTBEAT(ChatModule::H_Silence);

	LoadResource(pKernel);

	LoadCheckWords(pKernel);

	DECL_LUA_EXT(nx_reload_itemid_time__config);
	DECL_LUA_EXT(nx_reload_itemid_badwords__config);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("ChatConfig", ReloadResource);
#endif

	return true;
}

// 释放
bool ChatModule::Shut(IKernel* pKernel)
{
	ReleaseRes();
	return true;
}
// 释放资源信息
bool ChatModule::ReleaseRes()
{
	// 释放m_chatNationParam
	m_chatParam.clear();

	// 释放m_chatCheckWords
	m_chatCheckWords.clear();

	m_chatSaveChannel.clear();
	return true;
}

// 设置指定玩家禁言
bool ChatModule::SetSilence(IKernel* pKernel, const PERSISTID& self, int silenceTime)
{
	// 禁言时间不能为负数，单位为分钟
	if (silenceTime <= 0)
	{
		return false;
	}

	if (!pKernel->Exists(self))
	{
		return false;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return false;
	}

	// 只操作玩家
	if (pPlayer->GetClassType() != TYPE_PLAYER)
	{
		return false;
	}

	// 禁言中,若本来已禁言，则删除心跳重新设置禁言时间
	if (pKernel->FindHeartBeat(self, "ChatModule::H_Silence"))
	{
		pKernel->RemoveHeartBeat(self, "ChatModule::H_Silence");
	}
	pKernel->AddHeartBeat(self, "ChatModule::H_Silence", 10 * 1000);
	// 时间转化为毫秒
	pPlayer->SetInt("SilenceTick", silenceTime * 60000);

	return true;
}

//国家相关配置文件
bool ChatModule::LoadResource(IKernel* pKernel)
{
	std::string pathName = pKernel->GetResourcePath();
	//配置文件路径
	pathName += CHAT_CONFIG_URL;

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		//日志
		return false;
	}

	int iSectionCount = (int)xmlfile.GetSectionCount();
	std::string strSectionName = "";
	//std::string nationParam = "";
	LoopBeginCheck(d)
	for (int i = 0;i < iSectionCount;i++)
	{
		LoopDoCheck(d)
		strSectionName = xmlfile.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(strSectionName.c_str()))
		{
			return 0;
		}

		int num = ::atoi(strSectionName.c_str());
		if (num < 0)
		{
			continue;
		}

		if (num == 1)
		{
			std::string strParam = xmlfile.ReadString(strSectionName.c_str(), "Param","");
			if (StringUtil::CharIsNull(strParam.c_str()))
			{
				return 0;
			}

			CVarList paramList;
			util_split_string(paramList, strParam, ",");

			int numCount = (int)paramList.GetCount();
			LoopBeginCheck(a)
			for (int j = 0;j < numCount;j++)
			{
				LoopDoCheck(a)
				m_chatParam.push_back(paramList.StringVal(j));
			}
		}
		if(num == 2)
		{
			std::string strParam = xmlfile.ReadString(strSectionName.c_str(), "Param", "");
			if (StringUtil::CharIsNull(strParam.c_str()))
			{
				return 0;
			}
			CVarList paramList;
			util_split_string(paramList, strParam, ",");
			int nCnt = (int)paramList.GetCount();

			LoopBeginCheck(b)
			for (int j = 0; j < nCnt; j++)
			{
				LoopDoCheck(b)
				int nChannel = atoi(paramList.StringVal(j));
				m_chatSaveChannel.push_back(nChannel);
			}
		}
	}
	return 0;

}


void ChatModule::ReloadResource(IKernel* pKernel)
{
	ChatModule::m_pChatModule->LoadCheckWords(pKernel);
}

//读取聊天相关定义文件
bool ChatModule::LoadCheckWords(IKernel* pKernel)
{
	m_chatCheckWords.clear();

	std::string path = pKernel->GetResourcePath();
	path += "filter.txt";

	FILE *file = core_file::fopen(path.c_str(), "rb");
	if (file == NULL)
	{
		return false;
	}

	core_file::fseek( file, 0, SEEK_END );
	long lengths = core_file::ftell( file );
	core_file::fseek(file, 0, SEEK_SET);

	char* strContent = NEW char[lengths +1 ];
	memset(strContent, 0, lengths +1 );

	size_t result = core_file::fread(strContent, sizeof(char),lengths, file );
	if (result == 0)
	{
		delete[] strContent;
		strContent = 0; 

		return false;
	}

	strContent[lengths] = '\0';
	core_file::fclose(file);

	CVarList items; StringUtil::SplitString(items, strContent, "\n");
	LoopBeginCheck(a)
	for (size_t i=0; i<items.GetCount(); i++)
	{
		LoopDoCheck(a)
		std::string word = items.StringVal(i);
		word = StringUtil::Trim(word, 10); // 去掉回车
		word = StringUtil::Trim(word, 13); // 去掉换行
		word = StringUtil::Trim(word, 32); // 去掉空格

		m_chatCheckWords.push_back(::util_string_as_widestr(word.c_str()));
	}

	m_strWord = ::util_string_as_widestr("*");
	return true;

}

// 检查是否有脏字，忽略Url
bool ChatModule::CheckBadWordsIgnoreUrl(std::wstring& strInfo)
{
    // 不过滤链接中的内容
    // 例如: ....[url=1,7440003-001-1427438407-0006,麒麟战袍][u][[e8c936]麒麟战袍[-]][-][/url]...
    // 上面 [url= ... [/url] 之间的内容不过滤
    
    std::wstring url_begin_with = L"[url=";
    std::wstring url_end_with = L"[/url]";
    
    bool has_bad_words = false;
    std::string::size_type read_index = 0;
    std::wstring result_str;
    
    LoopBeginCheck(a)
    while (true)
    {
        LoopDoCheck(a)
        
        std::string::size_type start_pos = strInfo.find(url_begin_with, read_index);
        if (start_pos == std::string::npos)
        {
            // 木有起始符, 因此木有url
            std::wstring temp_str = strInfo.substr(read_index);
            
            has_bad_words = CheckBadWords(temp_str) ? true : has_bad_words;
            result_str.append(temp_str);
            
            break;
        }
        
        // 找到了起始符，接着查找结束符
        std::string::size_type end_pos = strInfo.find(url_end_with, start_pos);
        if (end_pos == std::string::npos)
        {
            // 木有结尾符, 因此木有url
            std::wstring temp_str = strInfo.substr(read_index);

            has_bad_words = CheckBadWords(temp_str) ? true : has_bad_words;
            result_str.append(temp_str);
            
            break;
        }
        
        // 找到一个url
        
        // 过滤非url内容
        if (start_pos != read_index)
        {
            std::wstring temp_str = strInfo.substr(read_index, start_pos - read_index);

            has_bad_words = CheckBadWords(temp_str) ? true : has_bad_words;
            result_str.append(temp_str);
        }
        
        // 取出url
        result_str.append(strInfo.substr(start_pos, end_pos-start_pos + url_end_with.length()));
        
        read_index = end_pos + url_end_with.length();
        
        if (read_index >= strInfo.length())
        {
            break;
        }
    }
    
    strInfo = result_str;
    
    return has_bad_words;
}

// 检查是否有脏字
bool ChatModule::CheckBadWords(std::wstring& strInfo)
{ 
	LoopBeginCheck(b)
	for (size_t i = 0; i < m_chatCheckWords.size(); i ++)
	{
		LoopDoCheck(b)
		std::wstring strTemp = m_chatCheckWords[i];
		if (strTemp.empty())
		{
			continue;
		}

		int nIndex = int(strInfo.find(strTemp));
		if (nIndex != -1)//找到非法词
		{
			std::wstring strSetWord = L"";
			//替换和谐词
			LoopBeginCheck(c)
			for (size_t j = 0; j < strTemp.length(); j++)
			{
				LoopDoCheck(c)
				strSetWord += m_strWord;
			}
			strInfo.replace(nIndex, strTemp.length(), strSetWord);
			return true;
		}
	}

	return false;
}

//玩家上线
int ChatModule::OnRecover(IKernel* pKernel, const PERSISTID& self, 
						  const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	if (pPlayer->QueryInt("SilenceTick") > 0)
	{
		//禁言中
		pKernel->AddHeartBeat(self, "ChatModule::H_Silence", 10 * 1000);
	}

	return 0;
}

//顶号
int ChatModule::OnContinue(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	if (pPlayer->QueryInt("SilenceTick") > 0)
	{
		if (pKernel->FindHeartBeat(self, "ChatModule::H_Silence"))
		{
			pKernel->RemoveHeartBeat(self, "ChatModule::H_Silence");
		}
		pKernel->AddHeartBeat(self, "ChatModule::H_Silence", 10 * 1000);
	}

	return 0;

}

// 聊天相关查询
int ChatModule::OnCustomQuery(IKernel* pKernel, const PERSISTID& self, 
						 const PERSISTID& sender, const IVarList& args)
{

	// 消息格式
	//
	//     [msg_id][sub_msg_id]...
	//

	if (!pKernel->Exists(self))
	{
		return 0;
	}

	
	int sub_msg_id = args.IntVal(1);
	switch (sub_msg_id)
	{
	case SNS_DATA_REQUEST_FORM_CHAT:
		{
#ifndef FSROOMLOGIC_EXPORTS
			// args 的格式
			//      [msg_id][sub_msg_id][target_name]
			const wchar_t *target_player_name = args.WideStrVal(2);
			if (StringUtil::CharIsNull(target_player_name))
			{
				return 0;
			}
			CVarList query_args;
			query_args << SNS_DATA_REQUEST_FORM_CHAT;
			query_args << target_player_name;
			query_args << 1;
			query_args << "PlayerAttribute";
			m_pSnsDataModule->OnQueryPlayerData(pKernel,self,sender,query_args);
#endif // FSROOMLOGIC_EXPORTS
		}

		break;
	}

	return 0;
}

// 接到客户端的聊天消息
int ChatModule::OnCustomChat(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
  	if (!pKernel->Exists(self))
	{
		return 0;
	}
	int chat_type = args.IntVal(1);
	//频道非法保护
	if (chat_type < 0)
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int silencetick = pPlayer->QueryInt("SilenceTick");
	if (silencetick > 0)
	{
		//在禁言中
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17001, CVarList());
		return 0;
	}

	//频道处于cd中
	if (CheckChannelInCD(pKernel, self, chat_type))
	{
		return 0;
	}

	if (args.GetCount() < 2)
	{
		return 0;
	}

	//聊天内容的类型 0表示文字聊天， 1表示语音聊天
	int content_type = args.IntVal(2);
	switch (content_type)
	{
		//文字聊天
	case CHAT_CONTENT_TYPE_WORD:
	{
		ChatContentTypeByWord(pKernel, self, args);
	}
	break;
	//语音聊天
	case CHAT_CONTENT_TYPE_VOICE:
	{
		ChatContentTypeByVoice(pKernel, self, args);
	}
	break;
	default:
		return 0;
	}

	return 0;
}

//聊天信息
int ChatModule::OnCommandChat(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{

	CVarList argList;
	int count = (int)args.GetCount();

	if (count < 3)
	{
		return 0;
	}
	const wchar_t *speak_player_name = args.WideStrVal(4);

	argList.Append(args, 1, count - 1);
	if(pKernel->Custom(self, argList)==0)
	{
		//发送聊天消息失败
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17002, CVarList());
		return 0;
	}
	return 0;

}


//聊天内容为文字
int ChatModule::ChatContentTypeByWord(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int chat_type = args.IntVal(1);

	//聊天内容
	const wchar_t* content = args.WideStrVal(3);
	if (StringUtil::CharIsNull(content))
	{
		return 0;
	}

	// 提取聊天内容中的展示的道具信息
	ExtractItemInfo(pKernel, self, content);

	std::wstring word_content = content;
	//有脏字，则记录聊天日志
	if (m_pChatModule->CheckBadWordsIgnoreUrl(word_content))
	{
#ifndef FSROOMLOGIC_EXPORTS
		// 记录日志
		std::wstring log_info = L"Unlawful Word : ";
		const wchar_t* name_self = pPlayer->QueryWideStr("Name");
		if (NULL == name_self)
		{
			return 0;
		}

		log_info = log_info + name_self + L" Chat Content: " + content;
		//m_pLogModule->SavePlayerLog(pKernel, self, LOG_PLAYER_CHAT, log_info.c_str());
#endif // FSROOMLOGIC_EXPORTS
	}

	if (StringUtil::CharIsNull(word_content.c_str()))
	{
		return 0;
	}

	const wchar_t* pTarget = NULL;
	// 转换临时用
	std::wstring strTmpTarget_;
	switch (chat_type)
	{
		//私聊
	case CHATTYPE_WHISPER:
	{
		pTarget = args.WideStrVal(4);
		if (StringUtil::CharIsNull(pTarget))
		{
			return 0;
		}
		m_pChatModule->OnChatChit(pKernel, self, pTarget, word_content.c_str());

	}
	break;
	//世界
	case CHATTYPE_WORLD:
	{
#ifndef FSROOMLOGIC_EXPORTS
		m_pChatModule->OnChatWorld(pKernel, self, word_content.c_str());

#endif // FSROOMLOGIC_EXPORTS
	}
	break;
	//系统
	case CHATTYPE_SYSTEM:
	{
		m_pChatModule->OnChatSystem(pKernel, self, args);
	}
	break;
	// 帮会
// 	case CHATTYPE_GUILD:
// 	{
// #ifndef FSROOMLOGIC_EXPORTS
// 		pTarget = pKernel->QueryWideStr(self, FIELD_PROP_GUILD_NAME);
// 		m_pChatModule->OnChatGuild(pKernel, self, word_content.c_str());
// #endif // FSROOMLOGIC_EXPORTS
// 	}
// 	break;
	// 队伍
	case CHATTYPE_TEAM:
	{
		int nTeamId_ = pKernel->QueryInt(self, FIELD_PROP_TEAM_ID);
		strTmpTarget_ = StringUtil::IntAsWideStr(nTeamId_);
		pTarget = strTmpTarget_.c_str();
		m_pChatModule->OnChatTeam(pKernel, self, word_content.c_str());
	}
	break;
	case CHATTYPE_TEAM_INVITE:
	{
		m_pChatModule->OnChatTeamInvite(pKernel, self, word_content.c_str());
	}break;

	// 小喇叭
	case CHATTYPE_WORLD_NEW:
	{
		// 取得背包
		PERSISTID tool_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
		if (!pKernel->Exists(tool_box))
		{
			return 0;
		}

		std::string &strItemId = m_pChatModule->m_chatParam[CHAT_ITEM_ID];
		int nRemoved = 0;
		if (FunctionEventModule::GetItemBindPriorUse(FUNCTION_EVENT_ID_USE_ITEM))
		{
			nRemoved = ContainerModule::RemoveItemsBindPrior(pKernel, tool_box, strItemId.c_str(), 1, FUNCTION_EVENT_ID_USE_ITEM, true);
		}
		else
		{
			nRemoved = ContainerModule::RemoveItems(pKernel, tool_box, strItemId.c_str(), 1, FUNCTION_EVENT_ID_USE_ITEM, true);
		}

		if (nRemoved < 1)
		{
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17003, CVarList());
			return 0;
		}
// 		int after = ContainerModule::GetItemCount(pKernel, tool_box, strItemId.c_str());
// 		pKernel->Command(self, self, CVarList() << COMMAND_SEVEN_DAY_OBJ << COMMAND_SEVEN_DAY_OBJ_USE_SPEAKER );
#ifndef FSROOMLOGIC_EXPORTS
		// 通知成就模块更新喇叭使用成就
// 		AchievementModule::m_pAchievementModule->UpdateAchievement(pKernel, self,
// 			SUBTYPE_BASIC_BRUSH_TRUMPET, CVarList() << 1);

		m_pChatModule->OnChatWorldNew(pKernel, self, word_content.c_str());
#endif // FSROOMLOGIC_EXPORTS
	}
		break;

	default:
		return 0;
	}

	// 写入成功的记录聊天日志
#ifndef FSROOMLOGIC_EXPORTS
	//LogModule::m_pLogModule->OnTextChat(pKernel, self, chat_type, pTarget, content);
#endif
	return 0;
}

//聊天内容为语音
int ChatModule::ChatContentTypeByVoice(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int chat_type = args.IntVal(1);

	//聊天内容为语音
	const char* msg_id = args.StringVal(3);

	//语音时长
	const int msg_time = args.IntVal(4);

	switch (chat_type)
	{
	//私聊
	case CHATTYPE_WHISPER:
	{
		const wchar_t* targetName = args.WideStrVal(5);
		if (StringUtil::CharIsNull(targetName))
		{
			return 0;
		}
		m_pChatModule->OnChatVoiceChit(pKernel, self, targetName, msg_id, msg_time);
	}
	break;
	//世界
	case CHATTYPE_WORLD:
	{
#ifndef FSROOMLOGIC_EXPORTS
		m_pChatModule->OnChatVoiceWorld(pKernel, self, msg_id, msg_time);
#endif // FSROOMLOGIC_EXPORTS
	}
	break;
	// 帮会
// 	case CHATTYPE_GUILD:
// 	{
// #ifndef FSROOMLOGIC_EXPORTS
// 		m_pChatModule->OnChatVoiceGuild(pKernel, self, msg_id, msg_time);
// #endif // FSROOMLOGIC_EXPORTS
// 	}
// 	break;
// 	// 队伍
// 	case CHATTYPE_TEAM:
// 	{
// #ifndef FSROOMLOGIC_EXPORTS		   
// 		m_pChatModule->OnChatVoiceTeam(pKernel, self, msg_id, msg_time);
// #endif // FSROOMLOGIC_EXPORTS
// 	}
// 	break;
	}
	return 0;
}


//私聊
void ChatModule::OnChatChit( IKernel* pKernel, const PERSISTID& self,  const wchar_t* targetname, const wchar_t* content )
{
	if (!CheckWhisper(pKernel, self, targetname))
	{	
		return;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	const wchar_t* srcName = pPlayer->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int iNation = pPlayer->QueryInt("Nation");

	CVarList var ;
	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_WHISPER << CHAT_CONTENT_TYPE_WORD << content << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self,  var))
	{
		var << iNation;

		pKernel->CommandByName(targetname, var);

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_WHISPER_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_WHISPER, atoi(strSC.c_str()) * 1000);

	}
	
	return;
}

// 公聊
void ChatModule::OnChatWorld(IKernel* pKernel, const PERSISTID& self,	 const wchar_t* content)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	//频道聊天等级限制
	int player_level = pPlayer->QueryInt("Level");
	int limit_nation_level = atoi(m_pChatModule->m_chatParam[LIMIT_WORLD_LEVEL].c_str());
	if(player_level < limit_nation_level)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17004, CVarList());
		return;
	}

	int64_t now = ::util_get_time_64();

	const wchar_t* srcName = pPlayer->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int iNation = pPlayer->QueryInt("Nation");

	CVarList var;

	var << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_WORLD << CHAT_CONTENT_TYPE_WORD << content << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		var << iNation;

#ifndef FSROOMLOGIC_EXPORTS
		pKernel->CustomByWorld(var);
#endif // FSROOMLOGIC_EXPORTS

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_WORLD_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_WORLD, atoi(strSC.c_str()) * 1000);
	}

	//保存聊天内容
	SaveChatInfo(pKernel, CHATTYPE_WORLD, self, content);

	return;
}

//new
void ChatModule::OnChatWorldNew(IKernel* pKernel, const PERSISTID& self, const wchar_t* content)
{
	if (!pKernel->Exists(self))
	{
		return;
	}
	
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	const wchar_t* srcName = pPlayer->QueryWideStr("Name");
	const int nNation = pPlayer->QueryInt("Nation");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	CVarList var;

	var << SERVER_CUSTOMMSG_SPEECH 
		<< CHATTYPE_WORLD_NEW
		<< CHAT_CONTENT_TYPE_WORD
		<< content 
		<< srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		var << nNation;		//国家

#ifndef FSROOMLOGIC_EXPORTS
		pKernel->CustomByWorld(var);
#endif // FSROOMLOGIC_EXPORTS

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_WORLD_NEW_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_WORLD_NEW, atoi(strSC.c_str()) * 1000);
	}
}


//系统
void ChatModule::OnChatSystem( IKernel* pKernel, const PERSISTID& self,  const IVarList& args )
{
	std::wstring word_content = L"";

	if (args.GetCount() < 2)
	{
		return;
	}
	CVarList var ;
	var << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_SYSTEM << CHAT_CONTENT_TYPE_WORD;
	word_content = util_utf8_as_widestr(args.StringVal(2));	
	if (StringUtil::CharIsNull(word_content.c_str()))
	{
		return;
	}
	var.AddWideStr(word_content.c_str());

#ifndef FSROOMLOGIC_EXPORTS
	pKernel->CustomByWorld(var);
#endif // FSROOMLOGIC_EXPORTS

	return;
}

// 发送消息到帮会聊天频道
// int ChatModule::ChatToGuild(IKernel* pKernel, const wchar_t* guildName, IVarList& args)
// {
// #ifndef FSROOMLOGIC_EXPORTS
// 	// 获取公会名
// 	if (StringUtil::CharIsNull(guildName))
// 	{
// 		return 0;
// 	}
// 
// 	// 获取成员表格
// 	int row = 0;
// 	IRecord* pRecord = m_pGuildModule->GetMemberRecord(pKernel, guildName);
// 	if (pRecord == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	// 通知每个帮会成员
// 	LoopBeginCheck(d);
// 	for (int r=0; r < pRecord->GetRows(); r++)
// 	{
// 		LoopDoCheck(d);
// 		const wchar_t* name = pRecord->QueryWideStr(r, GUILD_MEMBER_REC_COL_NAME);
// 		if (StringUtil::CharIsNull(name))
// 		{
// 			continue;
// 		}
// 
// 		bool online = CheckPlayerOnline(pKernel, name);
// 		if (online)
// 		{
// 			pKernel->CommandByName(name, args);
// 		}
// 	}
// #endif// FSROOMLOGIC_EXPORTS
// 	return 1;
// }

// 发送消息到帮会聊天频道
// int ChatModule::ChatToGuild(IKernel* pKernel, const PERSISTID& self, IVarList& args)
// {
// #ifndef FSROOMLOGIC_EXPORTS
// 	if (!pKernel->Exists(self))
// 	{
// 		return 0;
// 	}
// 
// 	// 玩家对象
// 	IGameObj *pPlayer = pKernel->GetGameObj(self);
// 	if (NULL == pPlayer)
// 	{
// 		return 0;
// 	}
// 
// 	// 获取公会名
// 	const wchar_t* guildName = pPlayer->QueryWideStr("GuildName");
// 	if (StringUtil::CharIsNull(guildName))
// 	{
// 		return 0;
// 	}
// 
// 	// 获取成员表格
// 	int row = 0;
// 	IRecord* pRecord = m_pGuildModule->GetMemberRecord(pKernel, self, row);
// 	if (pRecord == NULL)
// 	{
// 		return 0;
// 	}
// 	
// 	// 通知每个帮会成员
// 	LoopBeginCheck(d)
// 	for (int r=0; r < pRecord->GetRows(); r++)
// 	{
// 		LoopDoCheck(d)
// 		const wchar_t* name = pRecord->QueryWideStr(r, GUILD_MEMBER_REC_COL_NAME);
// 		if (StringUtil::CharIsNull(name))
// 		{
// 			continue;
// 		}
// 
// 		bool online = CheckPlayerOnline(pKernel, self, name, false);
// 		if (online)
// 		{
// 			pKernel->CommandByName(name, args);
// 		}
// 	}
// #endif // FSROOMLOGIC_EXPORTS
// 	return 1;
// }

//帮会 
// void ChatModule::OnChatGuild(IKernel* pKernel, const PERSISTID& self, const wchar_t* content)
// {
// #ifndef FSROOMLOGIC_EXPORTS
// 	IGameObj* playerObj = pKernel->GetGameObj(self);
// 	if (NULL == playerObj)
// 	{
// 		return;
// 	}
// 
// 	const wchar_t* guildName = playerObj->QueryWideStr("GuildName");
// 
// 	const wchar_t* srcName = playerObj->QueryWideStr("Name");
// 
// 	const char* uid = pKernel->SeekRoleUid(srcName);
// 
// 	if (guildName==NULL || srcName == NULL || uid == NULL)
// 	{
// 		return;
// 	}
// 
// 	if (wcslen(guildName) <= 0)
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_NOT_IN_GUILD, CVarList());
// 		return;
// 	}
// 
// 	bool isGuildExist = m_pGuildModule->IsGuildExist(pKernel, guildName);
// 	if (!isGuildExist)
// 	{
// 		//公会不存在
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_GUILD_NOT_EXIST, CVarList());
// 		return;
// 	}
// 
// 	int row = 0;
// 	IRecord* pRecord = m_pGuildModule->GetMemberRecord(pKernel, self, row);
// 
// 	if (pRecord == NULL)
// 	{
// 		// 公会不存在
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_GUILD_NOT_EXIST, CVarList());
// 		return;
// 	}
// 
// 	int srcRow = pRecord->FindString(GUILD_MEMBER_REC_COL_NAME_UID, uid);
// 	if (srcRow < 0)
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_NOT_IN_GUILD, CVarList());
// 		return;
// 	}
// 
// 	int srcPost = pRecord->QueryInt(srcRow, GUILD_MEMBER_REC_COL_POSITION);
// 
// 	CVarList var;
// 	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_GUILD << CHAT_CONTENT_TYPE_WORD << content << srcName;
// 	if (m_pChatModule->SetChatInfo(pKernel, self, var))
// 	{
// 		//加入帮会职务
// 		var.AddInt(srcPost);
// 
// 		// 通知每个帮会成员
// 		LoopBeginCheck(e)
// 		for (int r=0; r < pRecord->GetRows(); r++)
// 		{
// 			LoopDoCheck(e)
// 			const wchar_t* name = pRecord->QueryWideStr(r, GUILD_MEMBER_REC_COL_NAME);
// 			if (StringUtil::CharIsNull(name))
// 			{
// 				continue;
// 			}
// 
// 			bool online = CheckPlayerOnline(pKernel, self, name, false);
// 			if (online)
// 			{
// 				pKernel->CommandByName(name, var);
// 			}
// 		}
// 
// 		// 添加cd表纪录
// 		std::string strSC = m_pChatModule->m_chatNationParam[CD_GUILD_TIME];
// 		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_GUILD, atoi(strSC.c_str()) * 1000);
// 
// 		//保存聊天内容
// 		SaveChatInfo(pKernel, CHATTYPE_GUILD, self, content, srcPost);
// 	}
// #endif // FSROOMLOGIC_EXPORTS
// 	return;
// }

// 交互动作信息
// int ChatModule::InteractToScene(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
// {
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
// 	if (NULL == pSelfObj || NULL == pSkillObj)
// 	{
// 		return 0;
// 	}
// 
// 	// 是否为交互技能
// 	const char* strSkillId = pSkillObj->GetConfig();
// 	int nSkillType = FightInterfaceInstance->GetSkillTypeByConfig(pKernel, strSkillId);
// 	if (nSkillType != INTERACT_SKILL_TYPE)
// 	{
// 		return 0;
// 	}
// 
// 	std::wstring wsSkillId = StringUtil::StringAsWideStr(strSkillId);
// 	const wchar_t* srcName = pSelfObj->QueryWideStr(FIELD_PROP_NAME);
// 	int nSceneId = pKernel->GetSceneId();
// 
// 	CVarList msg;
// 	msg << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_INTERACT << CHAT_CONTENT_TYPE_WORD << wsSkillId << srcName;
// 	pKernel->CustomByScene(nSceneId, msg);
// 	return 0;
// }

//队伍
void ChatModule::OnChatTeam(IKernel* pKernel, const PERSISTID& self, const wchar_t* content)
{
	// 
	IGameObj* playerObj = pKernel->GetGameObj(self);
	if (NULL == playerObj)
	{

		return;
	}
	IRecord* pRecord = playerObj->GetRecord(TEAM_REC_NAME);
	if (NULL == pRecord)
	{
		return;
	}
	const wchar_t* srcName = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int count = pRecord->GetRows();
	if (count <= 0)
	{
		// 不在队伍中
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17009, CVarList());
		return;
	}

	CVarList var;
	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_TEAM << CHAT_CONTENT_TYPE_WORD << content << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		LoopBeginCheck(f)
		for( int r = 0; r < count; r++ )
		{
			LoopDoCheck(f)
			const char* uid = pRecord->QueryString(r, TEAM_REC_COL_UID);
			if (NULL == uid)
			{
				continue;
			}
			const wchar_t* name = pKernel->SeekRoleName(uid);
			if (NULL == name)
			{
				continue;
			}

			pKernel->CommandByName(name, var);
		}

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_TEAM_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_TEAM, atoi(strSC.c_str()) * 1000);
	}
}


void ChatModule::OnChatTeamInvite(IKernel* pKernel, const PERSISTID& self, const wchar_t* content)
{
	IGameObj* playerObj = pKernel->GetGameObj(self);
	if (NULL == playerObj)
	{
		return;
	}
	IRecord* pRecord = playerObj->GetRecord(TEAM_REC_NAME);
	if (NULL == pRecord)
	{
		return;
	}
	const wchar_t* srcName = playerObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int iNation = playerObj->QueryInt("Nation");

	CVarList var;

	var << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_TEAM_INVITE << CHAT_CONTENT_TYPE_WORD << content << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		var << iNation;
#ifndef FSROOMLOGIC_EXPORTS
		pKernel->CustomByWorld(var);
#endif
	}
}

//语音私聊
void ChatModule::OnChatVoiceChit(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetname, const char* msg_id, int msg_time)
{
	if (!CheckWhisper(pKernel, self, targetname))
	{
		return;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	const wchar_t* srcName = pPlayer->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int iNation = pPlayer->QueryInt("Nation");

	CVarList var;
	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_WHISPER << CHAT_CONTENT_TYPE_VOICE << msg_id << msg_time << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		var << iNation;

		pKernel->CommandByName(targetname, var);

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_WHISPER_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_WHISPER, atoi(strSC.c_str()) * 1000);

	}

	return;
}

// 语音公聊
void ChatModule::OnChatVoiceWorld(IKernel* pKernel, const PERSISTID& self, const char* msg_id, int msg_time)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	//频道聊天等级限制
	int player_level = pPlayer->QueryInt("Level");
	int limit_nation_level = atoi(m_pChatModule->m_chatParam[LIMIT_WORLD_LEVEL].c_str());
	if (player_level < limit_nation_level)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17004, CVarList());
		return;
	}

	int64_t now = ::util_get_time_64();

	const wchar_t* srcName = pPlayer->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcName))
	{
		return;
	}

	int iNation = pPlayer->QueryInt("Nation");

	CVarList var;

	var << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_WORLD << CHAT_CONTENT_TYPE_VOICE << msg_id << msg_time << srcName;
	if (m_pChatModule->SetChatInfo(pKernel, self, var))
	{
		var << iNation;

#ifndef FSROOMLOGIC_EXPORTS
		pKernel->CustomByWorld(var);
#endif // FSROOMLOGIC_EXPORTS

		// 添加cd表纪录
		std::string strSC = m_pChatModule->m_chatParam[CD_WORLD_TIME];
		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_WORLD, atoi(strSC.c_str()) * 1000);
	}

	return;
}


//帮会语音聊天 
// void ChatModule::OnChatVoiceGuild(IKernel* pKernel, const PERSISTID& self, const char* msg_id, int msg_time)
// {
// #ifndef FSROOMLOGIC_EXPORTS
// 	IGameObj* playerObj = pKernel->GetGameObj(self);
// 	if (NULL == playerObj)
// 	{
// 		return;
// 	}
// 
// 	const wchar_t* guildName = playerObj->QueryWideStr("GuildName");
// 
// 	const wchar_t* srcName = playerObj->QueryWideStr("Name");
// 
// 	const char* uid = pKernel->SeekRoleUid(srcName);
// 
// 	if (guildName == NULL || srcName == NULL || uid == NULL)
// 	{
// 		return;
// 	}
// 
// 	if (wcslen(guildName) <= 0)
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_NOT_IN_GUILD, CVarList());
// 		return;
// 	}
// 
// 	bool isGuildExist = m_pGuildModule->IsGuildExist(pKernel, guildName);
// 	if (!isGuildExist)
// 	{
// 		//公会不存在
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_GUILD_NOT_EXIST, CVarList());
// 		return;
// 	}
// 
// 	int row = 0;
// 	IRecord* pRecord = m_pGuildModule->GetMemberRecord(pKernel, self, row);
// 
// 	if (pRecord == NULL)
// 	{
// 		// 公会不存在
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_GUILD_NOT_EXIST, CVarList());
// 		return;
// 	}
// 
// 	int srcRow = pRecord->FindString(GUILD_MEMBER_REC_COL_NAME_UID, uid);
// 	if (srcRow < 0)
// 	{
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_NOT_IN_GUILD, CVarList());
// 		return;
// 	}
// 
// 	int srcPost = pRecord->QueryInt(srcRow, GUILD_MEMBER_REC_COL_POSITION);
// 
// 	CVarList var;
// 	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_GUILD << CHAT_CONTENT_TYPE_VOICE << msg_id << msg_time << srcName;
// 	if (m_pChatModule->SetChatInfo(pKernel, self, var))
// 	{
// 		//加入帮会职务
// 		var.AddInt(srcPost);
// 
// 		// 通知每个帮会成员
// 		LoopBeginCheck(e)
// 		for (int r = 0; r < pRecord->GetRows(); r++)
// 		{
// 			LoopDoCheck(e)
// 				const wchar_t* name = pRecord->QueryWideStr(r, GUILD_MEMBER_REC_COL_NAME);
// 			if (StringUtil::CharIsNull(name))
// 			{
// 				continue;
// 			}
// 
// 			bool online = CheckPlayerOnline(pKernel, self, name, false);
// 			if (online)
// 			{
// 				pKernel->CommandByName(name, var);
// 			}
// 		}
// 
// 		// 添加cd表纪录
// 		std::string strSC = m_pChatModule->m_chatNationParam[CD_GUILD_TIME];
// 		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_GUILD, atoi(strSC.c_str()) * 1000);
// 	}
// #endif // FSROOMLOGIC_EXPORTS
// 	return;
// }

// 队伍语音
// void ChatModule::OnChatVoiceTeam(IKernel* pKernel, const PERSISTID& self, const char* msg_id, int msg_time)
// {
// 	IGameObj* playerObj = pKernel->GetGameObj(self);
// 	if (NULL == playerObj)
// 	{
// 		return;
// 	}
// 	IRecord* pRecord = playerObj->GetRecord(TEAM_REC_NAME);
// 	if (NULL == pRecord)
// 	{
// 		return;
// 	}
// 	const wchar_t* srcName = playerObj->QueryWideStr("Name");
// 	if (StringUtil::CharIsNull(srcName))
// 	{
// 		return;
// 	}
// 
// 	int count = pRecord->GetRows();
// 	if (count <= 0)
// 	{
// 		// 不在队伍中
// 		::CustomSysInfo(pKernel, self, TIPSTYPE_NORMAL_CHAT_MESSAGE, STR_CHAT_SYSINFO_NOT_IN_TEAM, CVarList());
// 		return;
// 	}
// 
// 	CVarList var;
// 	var << COMMAND_CHAT << SERVER_CUSTOMMSG_SPEECH << CHATTYPE_TEAM << CHAT_CONTENT_TYPE_VOICE << msg_id << msg_time << srcName;
// 	if (m_pChatModule->SetChatInfo(pKernel, self, var))
// 	{
// 		LoopBeginCheck(f);
// 		for (int r = 0; r < count; r++)
// 		{
// 			LoopDoCheck(f);
// 			const char* uid = pRecord->QueryString(r, TEAM_REC_COL_UID);
// 			if (NULL == uid)
// 			{
// 				continue;
// 			}
// 			const wchar_t* name = pKernel->SeekRoleName(uid);
// 			if (NULL == name)
// 			{
// 				continue;
// 			}
// 
// 			pKernel->CommandByName(name, var);
// 		}
// 
// 		// 添加cd表纪录
// 		std::string strSC = m_pChatModule->m_chatNationParam[CD_TEAM_TIME];
// 		m_pCDModule->BeginCoolDown(pKernel, self, COOLDOWN_CHAT_TEAM, atoi(strSC.c_str()) * 1000);
// 	}
// }

// 聊天cd判断
bool ChatModule::CheckChannelInCD(IKernel* pKernel, const PERSISTID& self, const int channel)
{
	//系统没有cd
	if (channel == CHATTYPE_SYSTEM)
	{
		return false;
	}

	IGameObj* player = pKernel->GetGameObj(self);
	if (NULL == player)
	{
		return true;
	}
	
	// 冷却类型
	int nCoolDownType = 0;

	switch(channel)
	{
	case CHATTYPE_WHISPER:
			nCoolDownType = COOLDOWN_CHAT_WHISPER;
		break;
	case CHATTYPE_WORLD:
			nCoolDownType = COOLDOWN_CHAT_WORLD;
		break;
	case CHATTYPE_GUILD:
			nCoolDownType = COOLDOWN_CHAT_GUILD;
		break;
// 	case CHATTYPE_TEAM:
// 			nCoolDownType = COOLDOWN_CHAT_TEAM;
// 		break;
	case CHATTYPE_WORLD_NEW:
			nCoolDownType = COOLDOWN_CHAT_WORLD_NEW;
		break;
	default:
		break;
	}

	// 不在CD中
	int64_t cdTime = m_pCDModule->GetMillisecRemain(pKernel, self, nCoolDownType);
	if (cdTime <= 0)
	{
		return false;
	}

	// 冷却中，剩余时间{@0:数}秒
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17005, CVarList() << cdTime / 1000);
	return true;
}

//判断玩家是否在线
bool ChatModule::CheckPlayerOnline(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName, bool bTipCustom/*= true*/)
{
	//判断该玩家是否存在
	if (bTipCustom && strnlen(pKernel->SeekRoleUid(targetName), 50) == 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17006, CVarList() << targetName);
		return false;
	}

	//判断玩家是否在线
	if (bTipCustom && pKernel->GetPlayerScene(targetName) <= 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17007, CVarList() << targetName);
		return false;
	}

	return true;
}

// 根据玩家名字判断玩家是否在线（先判断是否存在）
bool ChatModule::CheckPlayerOnline(IKernel* pKernel, const wchar_t* targetName)
{
	//判断该玩家是否存在
	if (strnlen(pKernel->SeekRoleUid(targetName), 50) == 0)
	{
		return false;
	}

	//判断玩家是否在线
	if (pKernel->GetPlayerScene(targetName) <= 0)
	{
		return false;
	}

	return true;
}

//添加消息 字段
bool ChatModule::SetChatInfo( IKernel* pKernel, const PERSISTID& self, CVarList& var )
{

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return false;
	}
	int level = pPlayer->QueryInt("Level");
	// 帮会
	const wchar_t* guildName = pPlayer->QueryWideStr("GuildName");
	// 性别
	int nSex = pPlayer->QueryInt(FIELD_PROP_SEX);
	// 职业
	int nJob = pPlayer->QueryInt(FIELD_PROP_JOB);
	// vip等级
	const int nVipLevel = pPlayer->QueryInt("VipLevel");
	var << level << guildName << nSex << nJob << nVipLevel;

	// 添加当前时间
	int64_t now = ::time(NULL);
	var << now;

	return true;

}

//密语对象检查
bool ChatModule::CheckWhisper(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	if (wcslen(targetName) == 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17006, CVarList());
		return false;
	}

	const wchar_t* srcname = pSelfObj->QueryWideStr("Name");
	if (StringUtil::CharIsNull(srcname))
	{
		return false;
	}

	if (wcscmp(srcname, targetName) == 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17008, CVarList());
		return false;
	}
	//判断该玩家是否存在
	if (strnlen(pKernel->SeekRoleUid(targetName), 50) == 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17006, CVarList() << targetName);
		return false;
	}

	//判断玩家是否在线
	if (pKernel->GetPlayerScene(targetName) <= 0)
	{
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17007, CVarList() << targetName);
		return false;
	}


	return true;
}

//禁言心跳
int ChatModule::H_Silence(IKernel* pKernel, const PERSISTID& self, int slice)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int silencetick = pPlayer->QueryInt("SilenceTick");
	int newtick = silencetick - slice;
	if (newtick <= 0)
	{
		//禁言结束
		pPlayer->SetInt("SilenceTick", 0);
		pKernel->RemoveHeartBeat(self, "ChatModule::H_Silence");

		return 0;
	}

	//继续中
	pPlayer->SetInt("SilenceTick", newtick);
	return 0;
}


// 玩家通过聊天展示自己的道具(装备，物品等), 从聊天内容中提取道具信息, 
int ChatModule::ExtractItemInfo(IKernel* pKernel, const PERSISTID& self, const wchar_t *content)
{
    // 在 @content 中, 道具信息具有如下特征:
    // 装备：
    //     ... [url=1,7440003-001-1427438407-0006,麒麟战袍][u][[e8c936]麒麟战袍[-]][-][/url]...
    // 普通物品:
    //     ... [url=2,7440003-001-1427438407-0006,麒麟战袍][u][[e8c936]麒麟战袍[-]][-][/url]...
    //
    // 格式说明: [url=类型,物品UID,物品名]...[/url]

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}
    
    std::string str_content = util_widestr_as_string(content).c_str();
    
    CVarList items_list;

    const char *url_str = "url=";
    size_t url_str_len = strlen(url_str);
    
    size_t url_pos = 0;
    url_pos = str_content.find("url=", url_pos);
	LoopBeginCheck(j)
    while (url_pos != std::string::npos)
    {
		LoopDoCheck(j)
        // 解析串: [url=类型,物品UID,物品名]
        
        size_t first_comm = str_content.find(",", url_pos);
        if (first_comm == std::string::npos)
        {
            url_pos = str_content.find("url=", url_pos + url_str_len);
            continue;
        }

        size_t second_comm = str_content.find(",", first_comm + 1);
        if (second_comm == std::string::npos)
        {
            url_pos = str_content.find("url=", url_pos + url_str_len);
            continue;
        }

        size_t third_comm = str_content.find(",", second_comm + 1);
        if (third_comm == std::string::npos)
        {
            url_pos = str_content.find("url=", url_pos + url_str_len);
        }

        // 类型 和 UniqueID
        std::string str_link_type = str_content.substr(url_pos+url_str_len, first_comm-url_pos - url_str_len);
        std::string unique_id = str_content.substr(first_comm+1, second_comm - first_comm -1);
        std::string configID = str_content.substr(second_comm+1, third_comm - second_comm - 1);

        url_pos = str_content.find("url=", url_pos + url_str_len);

        
        int link_type = atoi(str_link_type.c_str());
        items_list << link_type;
        items_list << unique_id;
        items_list << configID;
    }
    
    CVarList items_info;
    items_info << pPlayer->QueryString("Uid");
    items_info << pPlayer->QueryWideStr("Name");
    
    if (items_list.GetCount() > 0)
    {
        InnerQueryItemInfo(pKernel, self, items_list, items_info);
        
        // 发送到public服务器
        
        SaveShowoffItem(pKernel,self, items_info);
    }
    
    return 0;
}

// 获取展示的装备信息
bool ChatModule::InnerQueryItemInfo(IKernel* pKernel, const PERSISTID& self,
                              const IVarList& items_list, CVarList& items_info)
{
//     static const char *prop_list[] = {"ColorLevel", "BasePackage", "AddPackage",
//      "ModifyLevel", "ModifyAddPackage"};

    int item_count = (int)(items_list.GetCount()/3);
    
    // 玩家装备栏
    PERSISTID equipment_box = pKernel->GetChild(self, L"EquipBox");
    if (!pKernel->Exists(equipment_box))
    {
        return false;
    }
    
    // 获取@items_list 里面的物品属性
    int args_index = -1;
	LoopBeginCheck(h)
    for (int i=0; i<item_count; ++i)
    {
		LoopDoCheck(h)
        int item_type = items_list.IntVal(++args_index);
        const char *item_uid = items_list.StringVal(++args_index);
        const char* itemConfig = items_list.StringVal(++args_index);
        
        if (item_type != 1 && item_type != 2 && item_type != 8)
        {
            continue;
        }

        PERSISTID container = ContainerModule::GetBoxContainer(pKernel, self, itemConfig);
        if (container.IsNull())
        {
            continue;
        }
        
        // 现在背包里找
        PERSISTID item = m_pContainerModule->FindItemByUid(pKernel, container, item_uid);

        if (!pKernel->Exists(item))
        {
            // 背包里面没有, 看看是不是在装备栏
            item = m_pContainerModule->FindItemByUid(pKernel, equipment_box, item_uid);

            if (!pKernel->Exists(item))
            {
                // 没有此物品
                continue;
            }
        }
        
        fast_string szInfo;
        if (!(::player_item_to_string(pKernel, item, szInfo)))
        {
            // 获取物品信息失败
            continue;
        }
        
        items_info << item_uid;
        items_info << szInfo.c_str();
        
    }// 物品循环结束
    
    return true;
}

// 保存展示的物品
bool ChatModule::SaveShowoffItem(IKernel* pKernel, const PERSISTID& self,
                                 const CVarList& items_info)
{
#ifndef FSROOMLOGIC_EXPORTS
//     wchar_t public_showoff_name[256] = {0};
//     SWPRINTF_S(public_showoff_name, DOMAIN_SHOWOFF_NAME_FORMAT_STR, pKernel->GetServerId());
// 
//     CVarList pub_msg;
//     pub_msg << PUBSPACE_DOMAIN;
//     pub_msg << public_showoff_name;
//     pub_msg << SP_DOMAIN_MSG_SHOWOFF;
//     pub_msg << SHOWOFF_PUB_SAVE_ITEM;
//     pub_msg << items_info;
// 
//     pKernel->SendPublicMessage(pub_msg);
#endif // FSROOMLOGIC_EXPORTS
    return false;
}

//保存聊天信息
bool ChatModule::SaveChatInfo(IKernel *pKernel, const int channel, 
							  const PERSISTID& self, const std::wstring &content, int subType)
{
#ifndef FSROOMLOGIC_EXPORTS
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	//过滤无需保存的频道
	int nCnt = (int)m_pChatModule->m_chatSaveChannel.size();
	LoopBeginCheck(i)
	for (int i = 0; i < nCnt; i++)
	{
		LoopDoCheck(i)
		if (channel == m_pChatModule->m_chatSaveChannel[i])
		{
			const wchar_t* srcname = pPlayer->QueryWideStr("Name");
			if (NULL == srcname)
			{
				return false;
			}

			//int nNation = pPlayer->QueryInt("Nation");
			
			//需要保存的内容
			CVarList var;
			var << channel
				<< subType
				<< srcname
				<< content;
			SetChatInfo(pKernel, self, var);

			//m_pChannelModule->SaveChannelInfo(pKernel, var);
			return true;
		}
	}
#endif // FSROOMLOGIC_EXPORTS
	return false;
}