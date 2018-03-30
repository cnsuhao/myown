//----------------------------------------------------------
// 文件名:      SystemMailModule.h
// 内  容:      邮件系统
// 说  明:
// 创建人:		  
// 修改人:        ( )
// 创建日期:    
//    :       
//----------------------------------------------------------

#ifndef FSGAME_SocialSystemModule_SYSTEM_MAIL_MODULE_H_
#define FSGAME_SocialSystemModule_SYSTEM_MAIL_MODULE_H_

#include <map>
#include "Fsgame/Define/header.h"
#include "FsGame/Define/FunctionEventDefine.h"
#include <vector>
class CapitalModule;

// 预先配置的邮件
struct ConfigMailItem
{
	ConfigMailItem(): sender(L""), mail_title(L""), mail_content(L""), items(""){}
	std::wstring		sender;
	std::wstring        mail_title;
	std::wstring        mail_content;
	std::string			items;
};

typedef std::map<std::string, ConfigMailItem> ConfigMailMap;

// 邮件系统设置
struct SystemMailConfig
{
	SystemMailConfig(): mail_appendix_life(0), mail_no_appendix_life(0){}
	int mail_appendix_life;		// 有附件的邮件存在时间(单位是秒)
	int mail_no_appendix_life;	// 无附件的邮件存在时间(单位是秒)


	void LoadConfig(IKernel* pKernel);
};

enum 
{
	AUTO_SEND_MAIL_BY_TASK,
	AUTO_SEND_MAIL_BY_LEVEL,
	AUTO_SEND_MAIL_MAX,
};


struct AutoSendMail
{
	int m_conditionValue{0};
	std::string m_mailName;
};


// 系统邮件模块
class SystemMailModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);
    //释放
    virtual bool Shut(IKernel* pKernel);

	static const std::wstring & GetDomainName(IKernel * pKernel);
    // 玩家进入游戏
    static int OnRecover(IKernel* pKernel, const PERSISTID& self,
						const PERSISTID& sender, const IVarList& args);

    // 玩家下线
    static int OnDisconnect(IKernel* pKernel, const PERSISTID& self,
						const PERSISTID& sender, const IVarList& args);

    //玩家准备
    static int OnReady(IKernel* pKernel, const PERSISTID& self, 
						const PERSISTID& sender, const IVarList& args);

	// 得到预先配置的邮件
	static const ConfigMailItem& GetConfigMail(IKernel* pKernel, const std::string& mail_id);

	// 拼凑mail的字符串
	static const std::wstring ComposeMailContent(IKernel* pKernel, const wchar_t* content, const IVarList &var);

	// 发送系统邮件给某个角色
    // source发送者（不同的系统名，可以为空）
    // name 接收者名字
    // title，content邮件标题及正文
    // addin_param附加物品列表格式: 资金名:数量,...;道具ID:道具数量,...
    // 例如:  CapitalCopper:100,CapitalGold:50;Item001:10,Equip002:2
	static bool SendMailToPlayer(IKernel* pKernel, const wchar_t* source, const wchar_t* name, 
				const wchar_t* title, const wchar_t* content, const std::string& addin_param, 
				EmFunctionEventId nMailFrom = FUNCTION_EVENT_ID_SYS);
	// 根据邮件模板发送系统邮件
	// name		接收者名字
	// mailid	邮件模板
	// param	模板参数
	static bool SendMailToPlayer(IKernel* pKernel, const char* mailid, const wchar_t* name, const IVarList& param, 
				const std::string& addin_param = std::string(),
				EmFunctionEventId nMailFrom = FUNCTION_EVENT_ID_SYS);

	// 发送系统邮件给某个账号
    // source发送者（不同的系统名，可以为空）
    // name 接收者名字
    // title，content邮件标题及正文
    // addin_param附加物品列表格式: 资金名:数量,...;道具ID:道具数量,...
    // 例如:  CapitalCopper:100,CapitalGold:50;Item001:10,Equip002:2
	static bool SendMailToAccount(IKernel* pKernel, const wchar_t* source, const char* account, 
								const wchar_t* title, const wchar_t* content, const std::string& addin_param,
								EmFunctionEventId nMailFrom = FUNCTION_EVENT_ID_SYS);


	// 客户端消息处理函数
	static int OnCustomClientMessage(IKernel* pKernel, const PERSISTID& self,
								const PERSISTID& sender, const IVarList& args);
	
    // 邮件数量
    static int OnCustomMailCount(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
        
    // 查询邮件列表
    static int OnCustomRetrieve(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
	
	//读取邮件
	static int OnCustomReadMail(IKernel* pKernel, const PERSISTID& self,
								const PERSISTID& sender, const IVarList& args);
	// 删除邮件
	static int OnCustomDeleteMail(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
        
    // 删除全部邮件
    static int OnCustomDeleteAll(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
        
	// 提取附件
	static int OnCustomPickMail(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 提取全部附件
    static int OnCustomPickAll(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 提取邮件
    static int InnerPickMail(IKernel* pKernel, const PERSISTID& self, const PERSISTID& toolbox, const IVarList& args);

	static bool ClearAllMail(IKernel* pKernel, const PERSISTID& self);
	static int SendTestMail(IKernel* pKernel, const PERSISTID& self,std::string &mailID);

	static void LoadResource(IKernel* pKernel);
	static bool LoadAutoSendMailCfg(IKernel *pKernel);

	// 发送广播全局邮件
	static bool SendGlobalMail(IKernel* pKernel, 
								const wchar_t* pwszSender, 
								const wchar_t* pwszTitle, 
								const wchar_t* pwszContent, 
								const char* pszAttachment, 
								int nFrom, 
								int64_t nOriMailId = 0,
								int64_t nCreateTime = 0,
								int64_t nExperidTime = 0 );
	static void RecvGlobalMail(IKernel* pKernel, const PERSISTID& player);

	//任务激活功能
	//static int OnCommandTask(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//等级激活功能
	//static int OnCommandLevel(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//
	//static void SystemGlobalMail(IKernel*pKernel, const char* mailName, const IVarList& args, int nNation, int lifeTime = 604800);
	//处理全局
	//static int OnPlayerRecoreGlobaleMail(IKernel*pKernel, const PERSISTID& self);

	// 内部命令回调
// 	static int OnCommandGuild(IKernel* pKernel, const PERSISTID& self,
// 		const PERSISTID& sender, const IVarList& args);


private:
	static IRecord *GetMailRecord(IKernel* pKernel, const PERSISTID& self);
		
	static int OnQueryLetter(IKernel* pKernel, const PERSISTID& self,const PERSISTID& sender, const IVarList& args);
	static int OnLookLetter(IKernel* pKernel, const PERSISTID& self,const PERSISTID& sender, const IVarList& args);
	
	//收到消息查询信件消息
	static int OnQuerySelfLetter(IKernel* pKernel, const PERSISTID& self,const PERSISTID& sender, const IVarList& args);

	//检查信箱的心跳
	static int H_Check_Letter(IKernel* pKernel, const PERSISTID& self, int slice);
	// 此模块计时器
	static int HB_TimeTick(IKernel* pKernel, const PERSISTID& self, int time);

	static int64_t _dayToSeconds(int day);
    static void ParseAppendix(const std::wstring& appendix, std::string &capital, std::string &item);

	static bool SetObjProperty(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    // 检查未读邮件
    static void CheckUnreadMail(IKernel* pKernel, const PERSISTID& self);
private:
	static CapitalModule* m_CapitalModuel;

	static ConfigMailMap m_ConfigMails;

	static SystemMailConfig m_SysMailConfig;

	static std::map<int, std::vector<AutoSendMail> > m_autoSendMailConfig;

	static std::wstring m_domainName;
};

#endif //