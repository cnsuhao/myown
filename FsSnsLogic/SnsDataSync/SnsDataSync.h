//---------------------------------------------------------
//文件名:       SnsDataSync.cpp
//内  容:       离线玩家的数据
//说  明:       
//          
//创建日期:      2014年12月24日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef FSSNS_SNS_DATA_SYNC_H_
#define FSSNS_SNS_DATA_SYNC_H_

#include "server/ISnsKernel.h"
#include "server/ISnsCallee.h"
#include "server/IRecord.h"
#include "../ISnsLogic.h"
#include <map>
#include <vector>

#define SNS_SECTION_RECORD_MAX_COL      10 // 表最大10列

#define SNS_DATA_RETURN_IF_NOT_IN_MEM   0  // 不在内存中直接返回
#define SNS_DATA_RETURN_IF_NOT_IN_DB    1  // 不在数据中直接返回
#define SNS_DATA_CREATE_IF_NOT_FOUND    2  // 找不到时尝试创建

class SnsDataSync : public ISnsLogic 
{
public:
    SnsDataSync();
    ~SnsDataSync();

    // 逻辑对象被创建
    int OnModuleCreate(ISnsKernel* pKernel, const IVarList& args);

    // SNS服务器所有的SNS数据已经加载完成准备就绪(在若干次OnSnsLoad之后)
    int OnReady(ISnsKernel* pKernel, const IVarList& args);

    // SNS数据已在内存中创建
    int OnCreate(ISnsKernel* pKernel, const char* uid, const IVarList& args);

    // SNS数据已经加载到内存中
    int OnLoad(ISnsKernel* pKernel, const char* uid, const IVarList& args);

    //收到来自member的消息
    int OnMessage(ISnsKernel* pKernel, int distribute_id, int server_id,
        int member_id, int scene_id, const char* uid, const IVarList& args);

    struct ElementDef
    {
		ElementDef() : name(""), type(0), section(""){}
        std::string name;
        int type;
        std::string section;
    };

    struct SectionDef
    {
		SectionDef() : name(""), sec_type(0), record_col_count(0), record_row_count(0)
		{
			memset(record_col, 0, sizeof(record_col));
		}
        std::string name;
        int sec_type;
        int record_col_count;
        int record_col[SNS_SECTION_RECORD_MAX_COL];
        int record_row_count;
    };

	// 获取Element的定义
	const ElementDef * GetElementDef(const std::string &attr_name,
		const std::string &section_name);
private:
    // 保存玩家数据
    bool SavePlayerData(ISnsKernel* pKernel, int distribute_id, int server_id,
        int member_id, int scene_id, const IVarList& args);

    // 保存玩家数据
    bool InnerSavePlayerData(ISnsKernel* pKernel, const IVarList& args,
        int sns_flag);
    
    // 查询玩家数据    
    bool InnerQueryPlayerData(ISnsKernel* pKernel, const IVarList& args,
        int sns_flag);

	// 查询玩家数据
    bool QueryPlayerData(ISnsKernel* pKernel, int distribute_id, int server_id,
        int member_id, int scene_id, const IVarList& args);

    // 创建玩家条目
    bool CreatePlayerEntry(ISnsKernel* pKernel, int distribute_id, int server_id,
        int member_id, int scene_id, const IVarList& args);

    // 获取SNS数据句柄
    ISnsData * InnerGetSnsDataHandle(ISnsKernel* pKernel, const char *uid,
        const IVarList& args, int op_flag, int sns_flag);

    // 保存段的数据
    bool SaveSectionData(ISnsData *sns_data, const IVarList &args, int args_index);
    
    // 保存属性数据
    bool SaveAttrSectionData(ISnsData *sns_data, const IVarList &args, int args_index);

    // 保存表格数据
    bool SaveRecordSectionData(ISnsData *sns_data, const IVarList &args, int args_index);

    // 查询section数据
    bool QuerySectionData(ISnsKernel *pKernel, ISnsData *sns_data,
        const std::string &section_name, IVarList &result);

    // 查询属性数据
    bool QueryAttrSectionData(ISnsKernel *pKernel, ISnsData *sns_data,
        const std::string &section_name, IVarList &result);

    // 查询表格数据
    bool QueryRecordSectionData(ISnsKernel *pKernel, ISnsData *sns_data,
        const std::string &section_name, IVarList &result);

    // 重置表格, 
    IRecord * ResetRecord(ISnsData *sns_data, const std::string &section_name);

	// 获取Section的类型, 返回-1表示找不到
	int GetSectionType(const std::string &type_name);

	// 获取section的定义
	const SectionDef *GetSectionDef(const std::string &section_name);

    //////////////////////////////////////////////////////////////////////////
    // 配置相关的操作
    //////////////////////////////////////////////////////////////////////////
    // 获取section的成员
    const std::vector<ElementDef> *GetSectionElements(const std::string &section_name);
    
    // 获取变量的id, 返回-1表示找不到
    int GetVarID(const std::string &section_name);

    bool LoadResource(ISnsKernel *pKernel);
    bool LoadVarDef(ISnsKernel *pKernel);
    bool LoadSectionType(ISnsKernel *pKernel);
    bool LoadSectionDef(ISnsKernel *pKernel);
    bool LoadElementsDef(ISnsKernel *pKernel);
private:
    typedef std::map<std::string, int> VarDefMap;
    typedef std::map<std::string, int> SectionTypeMap;
    typedef std::map<std::string, SectionDef> SectionDefMap;
    typedef std::map<std::string, std::vector<ElementDef> > SectionElementsMap;

    VarDefMap m_VarDef; 
    SectionTypeMap m_SectionType;
    SectionDefMap m_SectionDef;
    SectionElementsMap m_SectionElements;

public:
	static SnsDataSync *m_pSnsDataSync;
};

#endif