//---------------------------------------------------------
//文件名:       SnsDataModule.h
//内  容:       离线的玩家数据
//说  明:       
//          
//创建日期:      2014年12月24日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef FSGAME_SNS_DATA_MODULE_H
#define FSGAME_SNS_DATA_MODULE_H

#include "Fsgame/Define/header.h"

#include <map>
#include <vector>

class SnsDataModule : public ILogicModule
{
public:

	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

    static bool QuerySnsData(IKernel* pKernel, const IVarList& args);

    // 把一个玩家的数据立刻写入SNS
    static bool SaveSnsData(IKernel* pKernel, const PERSISTID &player);
    
private:

    struct ElementDef 
    {
		ElementDef(): name(""), type(0), section(""){}
        std::string name;
        int type;
        std::string section;
    };

    //////////////////////////////////////////////////////////////////////////
    // 回调函数
    //////////////////////////////////////////////////////////////////////////

    //玩家数据加载完毕
    static int OnPlayerEntry(IKernel *pKernel, const PERSISTID &player,
        const PERSISTID &sender, const IVarList &args);

    // player 的 OnStore 事件
	static int OnPlayerStoreEvent(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // Player 的Command消息回调
    static int OnPlayerCommandMsg(IKernel* pKernel, const PERSISTID& self, 
        const PERSISTID& sender, const IVarList& args);

    // Player 的客户端消息处理函数
    static int OnPlayerCustomMsg(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 来自public服务器的消息
    static int OnPublicMessage(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    //////////////////////////////////////////////////////////////////////////
    // 功能函数
    //////////////////////////////////////////////////////////////////////////

    // 发消息给SNS服务器
    static bool SendMsgToSnsServer(IKernel* pKernel, const PERSISTID& self,
        const IVarList &msg);

    static bool SendMsgToSnsServer(IKernel* pKernel, const char* uid,
        const IVarList &msg);
        
    // 读取玩家的离线数据
    static bool ReadPlayerAttr(IKernel* pKernel, const PERSISTID& self,
        IVarList &result);

    // 读取玩家装备信息
//     static bool ReadPlayerEquipment(IKernel* pKernel, const PERSISTID& self,
//         IVarList &result);
    
    // 读取玩家技能信息
    static bool ReadPlayerSkill(IKernel* pKernel, const PERSISTID& self,
        IVarList &result);
        
    // 读取玩家坐骑信息
//     static bool ReadPlayerRide(IKernel* pKernel, const PERSISTID& self,
//         IVarList &result);
//         
//     // 读取玩家宠物信息
//     static bool ReadPlayerPet(IKernel* pKernel, const PERSISTID& self,
//         IVarList &result);

    // 读取玩家被动技能信息
//     static bool ReadPlayerPassiveSkill(IKernel* pKernel, const PERSISTID& self,
//         IVarList &result);

	// 读取玩家洗练属性
// 	static bool ReadPlayerEquipmentBaptise(IKernel* pKernel, const PERSISTID& self, 
// 		IVarList &result);
public:
    // 查询玩家数据
    static bool OnQueryPlayerData(IKernel* pKernel, const PERSISTID& self, 
        const PERSISTID& sender, const IVarList& args);
    
    // 把玩家属性存储到SNS
    // args 的格式: [name][value]...[name][value]
    // name 是字符串类型, 可用的name配置在SnsData.xml 的 element_def 页中
    static bool SavePlayerAttr(IKernel* pKernel, const PERSISTID& player, const IVarList& args); 

	// 载入配置文件
	bool LoadResource(IKernel *pKernel);
	

private:
    // 请求数据
    static bool OnCommandRequestData(IKernel* pKernel, const PERSISTID& self, 
        const IVarList& args);

    // 得到数据
    static bool OnCommandGotData(IKernel* pKernel, const PERSISTID& self,
        const IVarList& args);

	// 查询聊天玩家相关数据
	static bool OnCommandGotChatData(IKernel* pKernel, const PERSISTID& self,
		const IVarList& args);

    //////////////////////////////////////////////////////////////////////////
    // 配置相关的函数
    //////////////////////////////////////////////////////////////////////////

    // 获取section的子项
    static const std::vector<SnsDataModule::ElementDef> * GetSectionElements(const std::string &sec_name);

    // 获取变量的id, 错误返回-1 
    int GetVarID(const std::string &var_name);

    // 载入配置文件-变量定义
    bool LoadVarDef(IKernel *pKernel);

    // 载入配置文件-section 子项定义
    bool LoadElementsDef(IKernel *pKernel);

    // 客户端请求排行榜内玩家的数据
	static int ClientRequestRankData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static void ReloadConfig(IKernel* pKernel);
public:

    typedef std::map<std::string, int> VarDefMap;
    typedef std::map<std::string, std::vector<ElementDef> > SectionElementsMap;

    static VarDefMap m_VarDef;
    static SectionElementsMap m_SectionElements;

    static SnsDataModule* m_pSnsDataModule;
};
#endif
