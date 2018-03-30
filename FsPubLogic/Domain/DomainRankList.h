//---------------------------------------------------------
//文件名:       DomainRankList.h
//内  容:       排行榜数据
//说  明:       用来存储各种排行表
//              - 每个表必须定义一个索引列
//              - 基本属性见本类私有类型 RecordDefine.
//              
//          
//创建日期:      2014年11月04日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef PUB_DOMAIN_DOMAIN_RANK_LIST_H
#define PUB_DOMAIN_DOMAIN_RANK_LIST_H

#include "IDomainData.h"
#include "FsGame/Define/PubDefine.h"

#include <string>
#include <map>
#include <vector>

#define RANK_LIST_MAX_COLUMN_COUNT         50   // 一个排行榜最大的列数
#define RANK_LIST_MAX_COLUMN_NAME_LENGTH   128  // 列名允许的最大字数
#define RANK_LIST_MAX_ASSOCIATED           5    // 每个排行榜最多可以关联的排行榜数
#define RANK_LIST_MAX_COLUMN_SIZE          256  // 排行榜的列允许的最大长度

class DomainRankList : public IDomainData
{
public: 
    DomainRankList();

    virtual ~DomainRankList();

public:

    enum OldDataStrategy
    {
        DISCARD_OLD_DATA    = 0, // 抛弃老数据
        KEEP_OLD_DATA       = 1, // 保留老数据
    };

    // 清空排行榜的定时器
    static int ClearRankListTimer(IPubKernel *pPubKernel, const char *space_name,
        const wchar_t *data_name, int time);
	// 外部获取排行榜数据
	static IPubData* GetRankListData(IPubKernel* pPubKernel);

    // 公共数据区名
    virtual const std::wstring & GetDomainName(IPubKernel* pPubKernel);

    //是否保存到数据库
    virtual bool GetSave(){return true;}

    virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    /// 处理来自场景服务器的消息
    virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 根据@msg条件查询, 结果返回到@out_result   
    // 函数返回 -1 表示操作失败
    //
    // @args 的格式：
    //     [请求者自定义ID]
    //     [排行榜的名字][起始行号][期望返回的行数]
    //     [列1名][列1值][列2名][列2值]...
    //
    // 等效于 SQL 语句:
    //     SELELCT TOP [期望返回的行数] FROM [排行榜的名字]
    //     WHERE 
    //       [列1名]=[列1值] AND [列2名]=[列2值] ....
    //   
    // 返回格式：
    // 
    //     [请求者自定义ID][排行榜的名字][起始行数][期望返回的行数]
    //     [查询结果的行数][查询结果的的列数]
    //     [全表排名][在本次查询条件中的排名][列名][列值]...
    //      ...
    int Query(IPubKernel* pPubKernel, IPubSpace* pPubSpace, IVarList &out_result, 
        const IVarList& msg, int msg_start_index=0);

    // 更新(插入)条目, 返回更新的行数, 返回-1表示出现错误
    // @msg 的格式
    //
    //      [请求者自定义ID]
    //      [排行榜的名字][如果条目不存在的处理方式][搜索列的名子][搜索列的值][列1的名字][列1的值]...
    //      [如果条目不存在的处理方式]: 设置为 0表示不做任何处理，1表示不存在就插入
    //
    // 例如：["level_rank_list"][0]["player_uid"]["1234"]["player_level"][3]["player_experience"][6]
    //  等效于 SQL 语句:
    //     UPDATE level_rank_list
    //     SET player_level=3, player_experience=6
    //     WHERE player_uid="1234"
    //
    int UpdateEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg,
        int msg_start_index=0);

    // 删除条目, 返回删除的行数, 返回-1表示出现错误
    // @msg 的格式
    //
    //      [排行榜的名字][主键名][主键值]
    // 
    // 等效于SQL 语句: 
    //    DELETE FROM [排行榜的名字] WHERE [主键名]=[主键值]
    int DeleteEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg,
        int msg_start_index=0);

    // 交换两行数据
    // @msg 的格式
    //
    //      [请求者自定义ID]
    //      [排行榜的名字][A条件列的名字][A的值][B条件列的名字][B的值]
    //      
    // 此操作将会交换 A 和 B 的位置
    ////  例如：["level_rank_list"]["player_uid"]["1234"]["player_uid"]["456"]
    //      表示吧 uid=1234 的行 和 uid=456的行互换
    //
    bool SwapEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg,
        int msg_start_index=0);

	// 清空排行榜-前处理, 0表示成功, 返回-1表示出现错误
	// @msg 的格式
	//
	//      [请求者自定义ID]
	//      [排行榜的名字]
	// 等效于 SQL 语句： TRUNCATE TABLE [排行榜的名字]
	//int BeforeClearRankList(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg);

    // 清空排行榜, 0表示成功, 返回-1表示出现错误
    // @msg 的格式
    //
    //      [请求者自定义ID]
    //      [排行榜的名字]
    // 等效于 SQL 语句： TRUNCATE TABLE [排行榜的名字]
    int ClearRankList(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg);

    // 查询@column_name 在排行榜@rank_list_name 中的索引
    // 找不到时返回-1
    int GetColumnIndex(const std::string &column_name, const std::string &rank_list_name);

    // 查询@column_name 在排行榜@rank_list_name 中的索引
    // 找不到时返回-1
    int GetColumnIndexInOutput(const std::string &column_name, const std::string &rank_list_name);

	// 根据等级和国家找到符合条件的一组玩家
	bool FindPlayerUidByLevelNation(IPubKernel* pPubKernel, IPubSpace * pPubSpace, int nLevel, int nNation,
										 IVarList& outData, const std::vector<std::string>& vRecUid);
										 
private:

    // 表的结构定义
    struct RecordDefine
    {
		RecordDefine() : rec_name(""), max_row(0), columns_count(0), pk_column(0), output_columns_count(0),
			sort_columns_count(0), use_asc(0), columns_to_reset_count(0), clear_strategy(0), clear_award(0),
			associated_update_strategy(0), associated_rank_list_count(0), bAwardByNation(true)
		{
			memset(columns_type, 0, sizeof(columns_type));
			memset(output_columns, 0, sizeof(output_columns));
			memset(sort_columns, 0, sizeof(sort_columns));
			memset(columns_to_reset, 0, sizeof(columns_to_reset));
		}
        std::string rec_name;
        int max_row;
        int columns_count; // 总列数

        int pk_column;     // 主键列

        int columns_type[RANK_LIST_MAX_COLUMN_COUNT];         // 列的类型数组
        std::string columns_name[RANK_LIST_MAX_COLUMN_COUNT]; // 列名数组

        int output_columns_count;                            // 输出的列数
        int output_columns[RANK_LIST_MAX_COLUMN_COUNT];      // 输出的列

        int sort_columns_count;                               // 参与排序的列数
        int sort_columns[RANK_LIST_MAX_COLUMN_COUNT];         // 参与排序的列
        
        int use_asc; // 是否升序排列

        int columns_to_reset_count;                                // 要重设的列数
        int columns_to_reset[RANK_LIST_MAX_COLUMN_COUNT]; // 服务器启动时值重设的列

        int clear_strategy;   // 清空策略
		int clear_award;      // 清空是否有奖励
		bool bAwardByNation;  // 是否根据国家排名发奖励

        int associated_update_strategy;                  // 关联排行榜的更新策略
        int associated_rank_list_count;
        std::string associated_rank_list[RANK_LIST_MAX_ASSOCIATED];

    };

    struct ColumnDefine 
    {
		ColumnDefine() : column_name(""), column_type(0), init_value_in_str(""), bIsNeedInit(false){}
        std::string column_name;         // 列名
        int column_type;                 // 列类型
        std::string init_value_in_str;   // 字符串表示的初始值
		bool bIsNeedInit;				 // 是否需要初始值
    };

    // 表的列变量
    struct ColumnVar
    {
        int column_index;
        
        int value_type; // 值类型: 立即数还是表达式

        // 立即数
        union
        {
            int int_value;
            int64_t int64_value;
            float float_value;
            double double_value;
            char str_value[RANK_LIST_MAX_COLUMN_SIZE];
            wchar_t wstr_value[RANK_LIST_MAX_COLUMN_SIZE];
        };

        // 表达式
        char expression[RANK_LIST_MAX_COLUMN_SIZE]; // 值的表达式
        ColumnVar() : value_type(0),int_value(1)
		{
			memset(expression, 0, RANK_LIST_MAX_COLUMN_SIZE * sizeof(char));
		}
    };



private:

    // 根据列名查询排行榜
    int OnQuery(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 条目有更新, 刷新排行榜
    int OnUpdateEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 删除排行榜的条目
    int OnDeleteEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);
    
    // 清空整个排行榜
    int OnClear(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 查询排行榜的数据日期
    int OnQueryRecordDate(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 交换排行榜中的两行
    int OnSwapEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 从@the_record中查找符合条件@the_column的一行数据, 返回行索引, 没找到返回-1
    int FindRowByColumn(IRecord *the_record, const ColumnVar *the_column,
        const RecordDefine *rec_def);
        
    // 从@the_record中查找符合条件@the_column的所有行索引
    void FindRowsByColumn(IRecord *the_record, const ColumnVar *the_column,
        const RecordDefine *rec_def, std::vector<int> *row_id_list);

    // 从@record中取出第@row_id行的数据，放入@dest中, 
    // 由@rec_def.output_columns 来决定放入哪些列
    int ReadRowOfOutputCols(IVarList &dest, IRecord *record, int row_id,
        const RecordDefine *rec_def);

    // 根据 @columns查询结果， 从结果的第@start_row开始获取@row_count_wanted条数据, 放入@out_list
    // @row_count 记录返回的行数, @coulumn_count 记录返回的列数
    // 返回数据格式是: [seq_no_all][seq_no_query][column1]...
    int ReadRowOfOutputCols(IVarList &out_list, int *row_count, int *coulumn_count,
        IRecord *record, const std::vector<ColumnVar> *columns,
        int start_row, int row_count_wanted, const RecordDefine *rec_def);

    // 根据 @columns查询结果， 从结果的第@start_row开始获取@row_count_wanted条数据, 然后再
    // 结果中检索符合条件@sub_column 的条目放入@out_list
    // @row_count 记录返回的行数, @coulumn_count 记录返回的列数
    // 返回数据格式是: [seq_no_all][seq_no_query][column1]...
    int ReadRowOfOutputCols(IVarList &out_list, int *row_count, int *coulumn_count,
        IRecord *record, const std::vector<ColumnVar> *columns, const std::vector<ColumnVar> *sub_column,
        int start_row, int row_count_wanted, const RecordDefine *rec_def);

    // 根据@column 查看列是否匹配
    bool IsRowMatch(int row_id, IRecord *record, const std::vector<ColumnVar> *columns,
        const RecordDefine *rec_def);

    // 从@record中取出第@row_id行的数据，放入@dest中
    int ReadRow(IVarList &dest, IRecord *record, int row_id, const RecordDefine *rec_def);

    // 从@record中取出第@row_id行的数据，放入@row_data中, 
    int ReadRow(std::vector<ColumnVar> *row_data, IRecord *record, int row_id,
        const RecordDefine *rec_def);

    // 插入一行数据
    int InsertRow(IRecord *the_record, const std::vector<ColumnVar> *columns,
        const RecordDefine *rec_def);

    // 更新一行数据
    int UpdateRow(IRecord *the_record, int row_id, const std::vector<ColumnVar> *columns,
        const RecordDefine *rec_def, int update_strategy = 0);

    // 删除一行数据
    int DeleteRow(IRecord *the_record, const ColumnVar *pk_column, const RecordDefine *rec_def);

    // 从@args中取出一个列
    bool GetColumnFromArgs(ColumnVar *the_column, const IVarList &args,
        int args_index, const RecordDefine *rec_def);

    // 从@args中取出多个列
    bool GetColumnsFromArgs(std::vector<ColumnVar> *columns, const IVarList &args,
        int args_index, int column_count, const RecordDefine *rec_def);

    // 读取配置
    bool LoadResource(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 列类型配置
    bool LoadColunmTypeConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 列名配置
    bool LoadColunmNameConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 排行榜配置
    bool LoadRankListConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 初始化排行榜
    void InitAllRankList(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 根据表的名字获取表的定义
    RecordDefine * GetRecordDefine(const char *rec_name);

    // 根据列名获取列名定义
    const ColumnDefine * GetColumnDefine(const std::string &column_name);

    // 根据字符表示的类型@type_name, 获取整数表示的类型
    // 找不到时返回-1
    int GetColumnIntType(std::string &type_name);

public: 

    // 根据列名获取列类型
    // 找不到时返回-1
    int GetColumnType(std::string &column_name);

private:

    // 获取列@column_name在表@rec_def中的索引
    // 找不到时返回-1
    int GetColumnIndex(const std::string &column_name, const RecordDefine *rec_def);

    // 读取@the_column的值, 放入@dest
    int ReadColumnVar(CVarList &dest, const ColumnVar *the_column,
        const RecordDefine *rec_def);

    // 从@row读取第@column_index列的值, 放入@the_column
    int GetColumnValue(ColumnVar *the_column, const CVarList &row,
        int column_index, const RecordDefine *rec_def);

    // 设置@the_record　第@row_id行的指定列@the_column的值
    int SetColunmOfRow(IRecord *the_record, int row_id, const ColumnVar *the_column,
        const RecordDefine *rec_def, int update_strategy = 0);

    // 设置@row_data指定列@the_column的值
    int SetColunmOfRow(CVarList &row_data, const ColumnVar *the_column,
        const RecordDefine *rec_def);

    // 初始化表@rec_def一行的值, 放入@row_data
    int InitRowData(CVarList &row_data, const RecordDefine *rec_def);

    // 比较两行数据@row1 和 @row2的优先级
    int CompareRowData(const CVarList &row1, const CVarList &row2,
        const RecordDefine *rec_def);

    // 比较@column1和@column2的优先级
    int CompareColumnData(const ColumnVar *column1, const ColumnVar *column2,
        const RecordDefine *rec_def);

    //  辅助函数

    // 把逗号分隔的字符串保存到数组
    // 例如: "a,b,c" 保存到 {"a", "b", "c"}
    bool SaveStrToArray(const char *str_src, std::string *str_array, int array_length,
        int *actual_size);

    // 安全的字符串拷贝函数
    int StrSafeCopy(char *dest, size_t dest_size, const char *src);

    // 安全的字符串拷贝函数
    int WstrSafeCopy(wchar_t *dest, size_t dest_size, const wchar_t *src);

    // 在@record的第@row_id行插入一行数据@row_data
    int InsertToRecord(const CVarList& row_data, int row_id, IRecord *record, const RecordDefine *rec_def);

    // 重设RankList的某些列
    int ResetRankListColumn(IRecord *record, const RecordDefine *rec_def);

    // 对排行榜的辅助表的操作
    // 排行榜的辅助表, 用来记录排行榜的上次清空时间和下次清空时间
    bool CreateInfoTable(IPubData * pPubData);
    bool AddToInfoTable(IPubData * pPubData, const std::string &entry_name);
    bool ResetClearDate(IPubData * pPubData, const std::string &entry_name, 
        const std::string &last_clear_date, const std::string &next_clear_date);
    bool GetClearDate(IPubData * pPubData, const std::string &entry_name,
        std::string &last_clear_date, std::string &next_clear_date);

    // 比较两个日期, 如果
    // date1 晚于 date2 返回大于0的数, date1 早于 date2 返回小于0的数, date1 等于 date2 返回0
    // date 的格式是: yyyy-dd-mm
    static int CompareDate(const std::string &date1, const std::string &date2);

    // 获取今天的日期
    static std::string GetDateOfToday();
    // 获取明天的日期
    static std::string GetDateOfNextDay();
    // 获取下周一的日期
    static std::string  GetDateOfNextMonday();
    // 获取下个月一号的日期
    static std::string  GetDateOfNextMonth();

    // 解析右表达式, 右表达式是对数字进行的求职操作
    // 右表达式的格式为： (+)n 或者 (-)n
    bool ParseRightExp(ColumnVar *the_column, int column_type, const char *exp);

	// 根据玩家等级取得排行榜等级区分表名
	static std::string GetDifLvlRankListName(int player_lvl);

	// 兼容玩家等级(根据GetDifLvlRankListName规则,向低等级兼容,获取兼容等级)
	static int CompatiblePlayerLevel(int player_lvl);

	// 重建排行榜, @old_data 现有排行榜数据的处理策略
    bool RecreateRankList(IPubData * pDomainData, const RecordDefine &rec_def, OldDataStrategy old_data = DISCARD_OLD_DATA);


public:
    static DomainRankList* m_pDomainRankList;

private:
    typedef std::map<std::string, RecordDefine> RecordDefineMap;
    typedef std::map<std::string, ColumnDefine> ColumnDefineMap;
    //typedef std::map<std::string, int> ColumnNameMap; // 例如：<player_uid, 6>
    typedef std::map<std::string, int> ColumnTypeMap; // 例如：<"int", 2>

    static RecordDefineMap m_RecordDefine;
    static ColumnDefineMap m_ColumnDefine;
    //static ColumnNameMap m_ColumnName;
    static ColumnTypeMap m_ColumnType;

	friend class Domain_Offline;
};

#endif