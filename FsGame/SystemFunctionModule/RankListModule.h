//--------------------------------------------------------------------
// 文件名:      RankListModule
// 内  容:      所有排行榜的管理类
// 说  明:      排行榜管理类,排行榜的所有消息通信、属性回调都放到此类中(暂时先考虑玩家排行榜)
// 创建日期:    2014-10-15
//
// 创建人:      liumf
//    :        
//--------------------------------------------------------------------

#ifndef FSGAME_GENERAL_RANK_LIST_MODULE_H
#define FSGAME_GENERAL_RANK_LIST_MODULE_H

#include "Fsgame/Define/header.h"

#include <map>
#include <vector>

#define RANK_LIST_MAX_COLUMN_COUNT 50 //一个排行榜最大的列数
#define REQUEST_FROM_SERVER 0 // 消息来自服务器
#define REQUEST_FROM_CLIENT 1 // 消息来自客户端

class RewardModule;
class RankListModule : public ILogicModule
{
public:

	virtual bool Init(IKernel * pKernel);

	virtual bool Shut(IKernel * pKernel);

    // 载入资源配置
    bool LoadResource(IKernel * pKernel);
    
    // 排行榜的定义
    bool LoadRankListDef(IKernel * pKernel, const char *file);
    bool LoadRankListColumnDef(IKernel * pKernel, const char *file);
    bool LoadTypeNameDef(IKernel * pKernel, const char *file);
	// 排行榜奖励配置
	bool LoadRankListAwardDef(IKernel * pKernel, const char *file);

public:

    // 获取domain的名字
    static const std::wstring& GetDomainName(IKernel * pKernel);

	// 获得排行榜公共数据
	static IPubData* GetPubData(IKernel* pKernel);

    // 根据@args条件查询, 通过回调返回结果. @requester将会在回应的消息中原样返回
    // 函数返回 -1 表示操作失败 
    // 查询结果会以COMMAND_RANK_LIST 消息发给 @requester, 详情见 RankListModule::OnPublicMessage();
    //
    // @args 的格式
    //    [请求者自定义ID]
    //    [排行榜的名字][起始行号][期望返回的行数]
    //    [列1名][列1值][列2名][列2值]...
    //
    // 等效于 SQL 语句:
    //  SELELCT TOP [期望返回的行数] FROM [排行榜的名字]
    //  WHERE 
    //   [列1名]=[列1值] AND [列2名]=[列2值] ....
    //
    static int Query(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args, int request_from = REQUEST_FROM_SERVER);

    // 更新排行榜的一条记录, request将会在回应的消息中原样返回
    // 函数返回 -1 表示操作失败
    // [如果不存在是否插入]: 0 表示不插入， 1表示插入
    // @args 的格式：
    //    
    //    [排行榜的名字][如果不存在是否插入][搜索列的名字][搜索列的值]
    //    [要更新的列名][要更新列值]...
    //
    // 例如：["level_rank_list"][0]["player_uid"]["1234"]["player_level"][3]["player_experience"][6]
    //  等效于 SQL 语句:
    //     UPDATE level_rank_list
    //     SET player_level=3, player_experience=6
    //     WHERE player_uid="1234"
    //
    static int UpdateEntry(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args, int request_from = REQUEST_FROM_SERVER);

    // 删除排行榜的一条记录, request将会在回应的消息中原样返回
    // 函数返回 -1 表示操作失败
    // @args 的格式：
    //     [请求者自定义ID]
    //     [排行榜的名字][索引列的名字][索引列的值]
    // 
    //  等效于 SQL 语句:
    //     DELETE FROM [排行榜的名字]
    //     WHERE [索引列的名字]=[索引列的值]
    //
    static int DeleteEntry(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args, int request_from = REQUEST_FROM_SERVER);

    // 清空排行榜的全部记录
    // 函数返回 -1 表示操作失败, request将会在回应的消息中原样返回
    // @args 的格式：
    //     [请求者自定义ID]
    //     [排行榜的名字]
    //  
    //  等效于 SQL 语句:
    //     TRUNCATE TABLE [排行榜的名字]
    static int ClearRankList(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args, int request_from = REQUEST_FROM_SERVER);

    // 查询排行榜数据的日期(数据的刷新日期)
    // args的格式: [排行榜的个数][排行榜1]...[排行榜n]
    static int QueryRankListDate(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args);

	// 根据玩家等级取得排行榜等级区分表名
	static std::string GetDifLvlRankListName(int player_lvl);

    // 查询 @column_name 在排行榜@rank_list_name中的列号.(列号从0开始)
    // 返回 -1 表示没有此列或者没有此排行榜
    static int GetColumnIndexByName(const char *column_name, const char *rank_list_name);

    // 为排行榜准备数据
    static bool PrepareDataForRankList(IKernel* pKernel, const PERSISTID& player, 
        const char *rank_list_name, IVarList &the_data);

    // 尝试把玩家插入排行榜, 此接口尽可能的填充对应排行的字段
    static int InsertPlayerToRankList(IKernel* pKernel, const PERSISTID& player, 
        const char *rank_list_name);
        
    // 删除排行榜里玩家记录
    static int DeleteFromRankList(IKernel* pKernel, const PERSISTID& player, 
        const char *rank_list_name);

	// 删除排行榜里玩家记录
	static int DeleteFromRankList(IKernel* pKernel, const char* playerUid, 
		const char *rank_list_name);

    // 交换排行榜中两行数据
    // @args 的格式：
    //     [请求者自定义ID]
    //     [排行榜的名字][A搜索列的名字][A搜索列的值][B搜索列的名字][B搜索列的值]
    //      
    //  例如：["level_rank_list"]["player_uid"]["1234"]["player_uid"]["456"]
    //      表示吧 uid=1234 的行 和 uid=456的行互换
    //
    static int SwapEntry(IKernel * pKernel, const PERSISTID &requester,
        const IVarList & args, int request_from = REQUEST_FROM_SERVER);
private:

    //玩家数据加载完毕
    static int OnPlayerRecover(IKernel *pKernel, const PERSISTID &player,
        const PERSISTID &sender, const IVarList &args);

    // 玩家数据保存
    static int OnStore(IKernel * pKernel, const PERSISTID &player,
        const PERSISTID & sender, const IVarList &args);

    // 自定义的回调函数

	// 来自公服的消息
	static int OnPublicMessage(IKernel * pKernel, const PERSISTID & self,
	    const PERSISTID & sender, const IVarList & args);

    // 发给场景的command消息
    static int OnSceneCommandMessage(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 发给玩家的command消息
    static int OnPlayerCommandMessage(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 客户端消息
    static int OnCustomMessage(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);

    // 根据指定条件查询排行榜
    // [排行榜的名字][查询条件的列名][查询条件的值]...
    static int OnQuery(IKernel * pKernel, const PERSISTID & player,
        const PERSISTID & sender, const IVarList & args);

    // 玩家VIP等级改变
    static int OnPlayerLevel(IKernel* pKernel, const PERSISTID& self,
        const char* property, const IVar& old);
        
    // 玩家等级改变
    static int OnPlayerVipLevel(IKernel* pKernel, const PERSISTID& self,
        const char* property, const IVar& old);

    // 玩家战斗力改变
    static int OnPlayerFightPower(IKernel* pKernel, const PERSISTID& self,
        const char* property, const IVar& old);

	// 竞技场排行榜
// 	static int OnPlayerPVPExp(IKernel* pKernel, const PERSISTID& self,
// 		const char* property, const IVar& old);

    // 玩家完成一次击杀
    static int OnKillObject(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 设置排行榜中玩家的在线状态
    static int SetPlayerOnlineState(IKernel* pKernel, const PERSISTID& player, int on_line);

    // 尝试把玩家数据加入到必要的排行榜中
    static int InsertToMultiRankList(IKernel* pKernel, const PERSISTID& player);

	// 玩家升级回调：删除原排行榜等级区分表的玩家数据
	static int RemovePlayerOfOldDifLvlRankList(IKernel* pKernel, const PERSISTID& player,int oldLevel = 0);


    // 更新玩家的排行榜数据
    static int UpdateRankList(IKernel* pKernel, const PERSISTID& player);
        
private:

    // 来自公共服务器的消息

    // 分发来自公共服的消息
    static int DispatchPublicMessage(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的条目查询结果
    static int OnPublicQueryResult(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的条目更新结果
    static int OnPublicUpdateEntryResult(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的条目删除结果
    static int OnPublicDeleteEntyResult(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的清空结果
    static int OnPublicClearResult(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的清空消息, 排行榜依据计划清空
    static int OnPublicClearPlanned(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

    // 来自公共服务器的消息, 排行榜的刷新日期
    static int OnPublicRankListDate(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

	// 来自公共服务器的清空消息, 排行榜依据计划清空-发送奖励
	static int OnPublicClearAward(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args);

    //////////////////////////////////////////////////////////////////////////
    // 私有的辅助函数
    //////////////////////////////////////////////////////////////////////////

    struct RankListDef
    {
		RankListDef(): name(""), columns_count(0)
		{
		}
        std::string name;
        int columns_count;
        std::string columns[RANK_LIST_MAX_COLUMN_COUNT];
    };
    
    struct RankListColumnDef
    {
		RankListColumnDef(): name(""), prop(""), column_type(""){}
        std::string name; // 排行榜的字段名
        std::string prop; // 字段对应的玩家属性, 空字符串表示没有玩家属性与之对应
        std::string column_type; // 字段类型名 (int, float....)
    };

    // 排行榜定义
    static const RankListDef* GetRankListDef(const char *rank_list_name);
    
    static bool HasColumn(const char *column_name, const RankListDef * rank_list_def);

    
    // 获取字段定义
    static const RankListColumnDef* GetRankListColumnDef(const char *field_name);
    
    // 设置指定字段的值, 并把此字段加入@fields
    static bool SetColumnValue(IKernel *pKernel, const PERSISTID &player,
        const RankListColumnDef *field_def, IVarList & fields);

    // 设置指定字段的值, 不包括字段名
    static bool SetColumnValueWithoutName(IKernel *pKernel, const PERSISTID &player,
        const RankListColumnDef *field_def, IVarList & fields);

    // 把逗号分隔的字符串保存到数组
    // 例如: "a,b,c" 保存到 {"a", "b", "c"}
    static bool SaveStrToArray(const char *str_src, std::string *str_array, int array_length,
        int *actual_size);
        
    // 根据类型名获取类型ID
    static int GetColumnTypeByName(const std::string &column_name);

	// 发送排行榜奖励
	static int RewardRankList(IKernel *pKernel, const IVarList & args);

	// 排行榜奖励配置
	struct RewardConfig_s
	{
		RewardConfig_s(): m_ID(0), m_Rank(""), m_Capital(""), m_Items(""){}
		int m_ID;               // ID
		std::string m_Rank;     // 排名区间
		std::string m_Capital;  // 货币奖励
		std::string m_Items;    // 物品奖励
	};
	struct RankListRewardConfig_s
	{
		RankListRewardConfig_s(): m_RankListName("")
		{
			m_RewardConfigVec.clear();
		}
		std::string m_RankListName;                       // 排行榜名称
		std::vector<RewardConfig_s> m_RewardConfigVec;    // <奖励配置>
	};

	// 排行榜奖励配置
	RankListRewardConfig_s* RankListRewardConfig( const char* rankListName );

	static void ReloadConfig(IKernel *pKernel);
public:

    static RankListModule*   m_pRankListModule;

	static RewardModule*     m_pRewardModule;      // 领奖模块

private:

    typedef std::map<std::string, RankListDef> RankListMap;
    
    typedef std::map<std::string, RankListColumnDef> ColumnMap;
    
    typedef std::map<std::string, int> ColumnTypeMap;

	typedef std::map<std::string, RankListRewardConfig_s> RankListRewardConfigMap;   // <排行榜名称, 奖励集合>

	typedef RankListRewardConfigMap::iterator RankListRewardConfigMap_it;

	static std::wstring m_domainName;  // 公共服务器的Domain数据名称

    static RankListMap m_RankListMap;
    
    static ColumnMap m_ColumnMap;
    
    static ColumnTypeMap m_ColumnType;

	static RankListRewardConfigMap m_RankListRewardConfigMap;    // 排行榜奖励配置
    
};


#endif