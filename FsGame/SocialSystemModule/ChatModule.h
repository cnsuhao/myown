//----------------------------------------------------------
// 文件名:      ChatModule.h
// 内  容:      聊天模块
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#ifndef _ChatModule_H_
#define _ChatModule_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include <string>

// 国家频道
//static const char * CHANNEL_TYPE_NATION = "channel_type_nation";

//参数
enum
{
	CHAT_ITEM_ID,					//小喇叭id
	CD_WHISPER_TIME,				//私聊cd
	CD_WORLD_TIME,					//世界聊天cd
	CD_GUILD_TIME,					//帮会cd
	CD_TEAM_TIME,					//组队cd
	CD_WORLD_NEW_TIME,				//小喇叭cd
	CD_SCENE_TIME,					//场景cd
	LIMIT_WORLD_LEVEL,              //世界频道等级限制
};

//聊天类型定义
enum
{
	//私聊
	CHATTYPE_WHISPER = 0,
	//世界
	CHATTYPE_WORLD,
	//系统 
	CHATTYPE_SYSTEM,
	// 帮会
	CHATTYPE_GUILD,
	// 队伍
	CHATTYPE_TEAM,
	// 组队邀请
	CHATTYPE_TEAM_INVITE,
	// 小喇叭
	CHATTYPE_WORLD_NEW,

	CHATTYPE_MAX,
};

//聊天内容的类型定义
enum
{
	//文字聊天
	CHAT_CONTENT_TYPE_WORD = 0,
	//语音聊天
	CHAT_CONTENT_TYPE_VOICE = 1,
};

class ChannelModule;
//class GuildModule;
class LogModule;
class ContainerModule;
class SnsDataModule;
class CoolDownModule;
//class VipModule;

class ChatModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);
public:

	// 设置指定玩家禁言 ，禁言时间单位为分钟
	bool SetSilence(IKernel* pKernel, const PERSISTID& self, int silenceTime);

public:
	// 国家相关配置信息
	bool LoadResource(IKernel* pKernel);
	static void ReloadResource(IKernel* pKernel);
	// 释放资源信息
	bool ReleaseRes();

	//读取聊天相关定义文件
	bool LoadCheckWords(IKernel* pKernel);

    // 检查是否有脏字，忽略Url
    bool CheckBadWordsIgnoreUrl(std::wstring& strInfo);

	// 检查是否有脏字
	bool CheckBadWords(std::wstring& strInfo);

	/// \brief 上线
	static int OnRecover(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 顶替上线
	static int OnContinue(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 发送消息到公会聊天频道
	int ChatToGuild(IKernel* pKernel, const PERSISTID& self, IVarList& args);

	// 发送消息到帮会聊天频道通过帮会名
	int ChatToGuild(IKernel* pKernel, const wchar_t* guildName, IVarList& args);

	// 交互动作信息
//	int InteractToScene(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	//聊天内容为文字
	static int ChatContentTypeByWord(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
private:
	// 自定义响应函数
	static int OnCustomChat(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 聊天相关查询
	static int OnCustomQuery(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 跨场景聊天信息
	static int OnCommandChat(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//聊天内容为语音
	static int ChatContentTypeByVoice(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	//私聊
	static void OnChatChit( IKernel* pKernel, const PERSISTID& self, 
		const wchar_t* targetname, const wchar_t* content);

	// 公聊
	static void OnChatWorld(IKernel* pKernel, const PERSISTID& self, 
		const wchar_t* content);
	// new
	static void OnChatWorldNew(IKernel* pKernel, const PERSISTID& self, 
		const wchar_t* content);

	// 系统
	static void OnChatSystem(IKernel* pKernel, const PERSISTID& self, 
		const IVarList& args);

	// 帮会
// 	static void OnChatGuild(IKernel* pKernel, const PERSISTID& self, 
// 		const wchar_t* content);

	// 队伍
	static void OnChatTeam(IKernel* pKernel, const PERSISTID& self, 
		const wchar_t* content);

	static void OnChatTeamInvite(IKernel* pKernel, const PERSISTID& self, 
		const wchar_t* content);

	//语音私聊
	static void OnChatVoiceChit(IKernel* pKernel, const PERSISTID& self,
		const wchar_t* targetname, const char* msg_id, int msg_time);

	// 语音公聊
	static void OnChatVoiceWorld(IKernel* pKernel, const PERSISTID& self,
		const char* msg_id, int msg_time);

	// 帮会语音
// 	static void OnChatVoiceGuild(IKernel* pKernel, const PERSISTID& self,
// 		const char* msg_id, int msg_time);
// 
// 	// 队伍语音
// 	static void OnChatVoiceTeam(IKernel* pKernel, const PERSISTID& self,
// 		const char* msg_id, int msg_time);

	// 聊天cd判断
	static bool CheckChannelInCD(IKernel* pKernel, const PERSISTID& self,
		const int channel);

	//判断玩家是否在线
	static bool CheckPlayerOnline(IKernel* pKernel, const PERSISTID& self,
		const wchar_t* targetName, bool bTipCustom = true);

	//判断玩家是否在线
	static bool CheckPlayerOnline(IKernel* pKernel, const wchar_t* name);

	//添加消息 字段
	static bool SetChatInfo( IKernel* pKernel, const PERSISTID& self, CVarList& var);

	//密语对象检查
	static bool CheckWhisper(IKernel* pKernel, const PERSISTID& self,
		const wchar_t* targetName);

	//禁言心跳
	static int H_Silence(IKernel* pKernel, const PERSISTID& self, int slice);

    // 玩家通过聊天展示自己的道具(装备，物品等), 从聊天内容中提取道具信息, 
    static int ExtractItemInfo(IKernel* pKernel, const PERSISTID& self,
        const wchar_t *content);

    // 获取展示的装备信息并存放在 @items_info 中
    // @items_info 的存放格式： [unique_id][prop..]...[unique_id][prop...]
    static bool InnerQueryItemInfo(IKernel* pKernel, const PERSISTID& self,
        const IVarList& items_list, CVarList& items_info);
        
    // 保存展示的物品
    static bool SaveShowoffItem(IKernel* pKernel, const PERSISTID& self,
        const CVarList& items_info);

	//保存聊天信息
	static bool SaveChatInfo(IKernel *pKernel, const int channel, 
						const PERSISTID& self, const std::wstring &content, int subType = -1);

public:
	static ChatModule* m_pChatModule;
	std::vector<std::string> m_chatParam;
	std::vector<std::wstring> m_chatCheckWords;
	std::vector<int> m_chatSaveChannel;	//需要保存信息的频道
	std::wstring m_strWord;

private:
	
	static ContainerModule* m_pContainerModule;
	static CoolDownModule* m_pCDModule;		// 冷却模块
	//static VipModule* m_pVipModule;			// vip模块

#ifndef FSROOMLOGIC_EXPORTS
	static SnsDataModule *m_pSnsDataModule;
	static ChannelModule* m_pChannelModule;
	//static GuildModule* m_pGuildModule;
	static LogModule* m_pLogModule;
#endif // FSROOMLOGIC_EXPORTS
};

#endif // _ChatModule_H_
