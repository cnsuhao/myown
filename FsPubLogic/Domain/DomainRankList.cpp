//---------------------------------------------------------
//文件名:       DomainRankList.cpp
//内  容:       排行榜数据
//说  明:       用来存储，管理各种排行表, 排行榜通过名字来区分
//          
//创建日期:      2014年11月04日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#include "DomainRankList.h"

#include <string>
#include <time.h>
#include <algorithm>

#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/RankingDefine.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/SnsDefine.h"
//#include "FsGame/Define/OffLineDefine.h"
#include "utils/string_util.h"
//#include "FsGame/Define/NationDefine.h"
//#include "FsGame/Define/FrontierSceneDefine.h"

#define SUCCESS_RET_CODE 0
#define FAILED_RET_CODE  -1

#define RANK_LIST_DEFINE_FILE       "ini/SystemFunction/RankList/rank_list_record.xml"
#define RANK_LIST_COLUMN_FILE       "ini/SystemFunction/RankList/rank_list_column.xml"
#define RANK_LIST_COLUMN_TYPE_FILE  "ini/SystemFunction/RankList/rank_list_column_type.xml"
#define LEVEL_LOCK_CONFIG          "ini/SystemFunction/RankList/level_lock_config.xml"

// 排行榜属性表
#define INFO_TABLE              "RankListTable"
#define INFO_TABLE_COLUMN_COUNT 3
#define INFO_TABLE_MAX_ROW      100

#define SECONDS_OF_ONE_DAY      86400 // 一天(24hr)的秒数

#define VALUE_TYPE_IMMEDIATE         0 // 立即数
#define VALUE_TYPE_EXP               1 // 表达式

#define UPDATE_STRATEGY_DEFAULT      0 // 默认策略
#define UPDATE_STRATEGY_GREAT_THAN   1 // 大于时更新 
#define UPDATE_STRATEGY_LESS_THAN    2 // 小于时更新

#define RANK_PLAYER_COUNT  10		         // 每次处理的玩家数

DomainRankList* DomainRankList::m_pDomainRankList = NULL;

DomainRankList::RecordDefineMap DomainRankList::m_RecordDefine;
DomainRankList::ColumnDefineMap DomainRankList::m_ColumnDefine;  
DomainRankList::ColumnTypeMap DomainRankList::m_ColumnType;

DomainRankList::DomainRankList() : IDomainData(L"")
{
}

DomainRankList::~DomainRankList()
{
    m_RecordDefine.clear();
}

// 清空排行榜的定时器
int DomainRankList::ClearRankListTimer(IPubKernel *pPubKernel, const char *space_name,
                              const wchar_t *data_name, int time)
{
    IPubSpace *pPubSpace = pPubKernel->GetPubSpace(space_name);
    if (pPubSpace == NULL)
    {
        return 0;
    }
    IPubData *pDomainData = pPubSpace->GetPubData(data_name);
    if (pDomainData == NULL)
    {
        return 0;
    }

    if (m_pDomainRankList == NULL)
    {
        return 0;
    }

    std::string current_date = GetDateOfToday();

    RecordDefineMap::iterator it = m_RecordDefine.begin();
    RecordDefineMap::iterator it_end = m_RecordDefine.end();
    
    LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(a);
        
        bool do_clear = false;
        RecordDefine &rec_def = it->second;

        // 取得上次清空日期 和 下次清空日期
        std::string last_clear_date;
        std::string next_clear_date;
        m_pDomainRankList->GetClearDate(pDomainData, rec_def.rec_name,
            last_clear_date, next_clear_date);
        
        CVarList clear_msg;
        clear_msg << rec_def.rec_name.c_str();

        switch (rec_def.clear_strategy)
        {
        case RANKING_CLEAR_ON_DAY:
            {
                if (current_date != last_clear_date)
                {
                    std::string next_clear_date_new = GetDateOfNextDay();
                    clear_msg << current_date;
                    clear_msg << next_clear_date_new.c_str();
                    do_clear = true;
                }
            }
            break; 
        case RANKING_CLEAR_ON_WEEK:
            {
                if (current_date != last_clear_date)
                {
                    // 检查下次清理日期
                    if (CompareDate(next_clear_date, current_date) <=0)
                    {
                        std::string next_clear_date_new = GetDateOfNextMonday();
                        clear_msg << current_date;
                        clear_msg << next_clear_date_new.c_str();
                        do_clear = true;
                    }
                }
            }
            break;
        case RANKING_CLEAR_ON_MONTH:
            {
                if (current_date != last_clear_date)
                {
                    // 检查下次清理日期
                    if (CompareDate(next_clear_date, current_date) <=0)
                    {
                        std::string next_clear_date_new = GetDateOfNextMonth();
                        clear_msg << current_date;
                        clear_msg << next_clear_date_new.c_str();
                        do_clear = true;
                    }
                }
            }
            break;
        default:
            break;
        }
        if (do_clear)
        {
            m_pDomainRankList->ClearRankList(pPubKernel, pPubSpace, clear_msg);
        }
    }

    return 0;
}

IPubData* DomainRankList::GetRankListData(IPubKernel* pPubKernel)
{
	IPubData* pData = NULL;

	wchar_t wstr[256];
	int server_id = pPubKernel->GetServerId();
	SWPRINTF_S(wstr, L"Domain_RankingList_%d", server_id);
	IPubSpace* pSpace = pPubKernel->GetPubSpace(PUBSPACE_DOMAIN);
	Assert(NULL != pSpace);
	if (NULL != pSpace)
	{
		pData = pSpace->GetPubData(wstr);
	}

	return pData;
}

const std::wstring & DomainRankList::GetDomainName(IPubKernel * pPubKernel)
{
    if (m_domainName.empty())
    {
        wchar_t wstr[256];
        int server_id = pPubKernel->GetServerId();
		 SWPRINTF_S(wstr, L"Domain_RankingList_%d", server_id);
        m_domainName = wstr;
    }

    return m_domainName;
}

int DomainRankList::OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
    m_pDomainRankList = this;
    
    return OnLoad(pPubKernel, pPubSpace);
}

int DomainRankList::OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
    m_pDomainRankList = this;

    // 载入表的定义配置
    LoadResource(pPubKernel, pPubSpace);

    // 初始化排行榜
    InitAllRankList(pPubKernel, pPubSpace);

	const wchar_t* name = GetDomainName(pPubKernel).c_str();
	if (StringUtil::CharIsNull(name))
	{
		return 0;
	}

	// 添加检测心跳
	if (!pPubKernel->FindTimer("RankListClear", pPubSpace->GetSpaceName(), name))
	{
		// 添加清理排行榜的定时器
		int timer_interval = 1000 * 60 * 1; // 1min 的间隔
		int timer_count = 0; // 不限次数

		pPubKernel->AddTimer("RankListClear", DomainRankList::ClearRankListTimer,
			pPubSpace->GetSpaceName(), name,
			timer_interval, timer_count);
	}

    return 0;
}

/// 处理来自场景服务器的消息
int DomainRankList::OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
                             int source_id, int scene_id, const IVarList& msg)
{
    /*
       消息格式:
           [PUBSPACE][DOMAIN_NAME]
           [msg_id][sub_msg_id]
     */
    IPubData * pDomainData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());
    if (pDomainData == NULL)
    {
        return 1;
    }

    int msgid = msg.IntVal(3);
    switch (msgid)
    {
    case RANKING_SUB_MSG_QUERY:
        {
            OnQuery(pPubKernel, pPubSpace, source_id, scene_id, msg);
        }
        break;
    case RANKING_SUB_MSG_QUERY_TOP_N:
        {
            // 此消息被遗弃
        }
        break;
    case RANKING_SUB_MSG_DELETE_ENTRY:
        {
            OnDeleteEntry(pPubKernel, pPubSpace, source_id, scene_id, msg);
        }
        break;
    case RANKING_SUB_MSG_UPDATE_ENTRY:
        {
            OnUpdateEntry(pPubKernel, pPubSpace, source_id, scene_id, msg);
        }
        break;
    case RANKING_SUB_MSG_CLEAR:
        {
            OnClear(pPubKernel, pPubSpace, source_id, scene_id, msg); 
        }
        break;
    case RANKING_SUB_MSG_DATE:
        {
             OnQueryRecordDate(pPubKernel, pPubSpace, source_id, scene_id, msg); 
        }
        break;
    case RANKING_SUB_MSG_SWAP:
        {
            OnSwapEntry(pPubKernel, pPubSpace, source_id, scene_id, msg); 
        }
        break;
    }

    return 1;
}

// 查询名次, 根据第一条件查询出结果，然后查询第二条件在此结果中的排名
int DomainRankList::Query(IPubKernel* pPubKernel, IPubSpace* pPubSpace, IVarList &out_result, 
              const IVarList& msg, int msg_start_index)
{
    /*
        msg 格式：

        [rank_list_name]
        [start_row][row_count_wanted]
        [sub_column_count]            // 第二条件的列数
        [column1_name][column1_value] // 第二条件
        [column2_name][column2_value] 
        ...
        [columnX_name][columnx_value] // 第一条件
        ...

        out_result 格式

        [rank_list_name]
        [start_row][row_count_wanted]
        [result_row_count][column_count_per_row]
        [seq_no_all + seq_no_query + row_data...]
        ...
    */
    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return -1;
    }

    int msg_index = msg_start_index;
    const char *record_name = msg.StringVal(msg_index);
    int start_row = msg.IntVal(++msg_index);
    int row_count_wanted = msg.IntVal(++msg_index);

    if (start_row < 1 || row_count_wanted < 0)
    {
        return -1;
    }

    RecordDefine *rec_def = GetRecordDefine(record_name);
    if (rec_def == NULL)
    {
        return -1;
    }

    if (rec_def->output_columns_count == 0)
    {
        return -1;
    }

    IRecord * record = pDomainData->GetRecord(record_name);
    if (record == NULL)
    {
        return -1;
    }


    ++msg_index; // 移动到 [sub_column_count]
    int sub_column_count = msg.IntVal(msg_index); // 第二条件的数量
    if (sub_column_count < 0)
    {
        return -1;
    }

    // 获取查询条件的列
    ++msg_index; // 移动到 [column1_name]

    // 获取第二条件
    std::vector<ColumnVar> sub_colunms_to_check;
    if (!GetColumnsFromArgs(&sub_colunms_to_check, msg, msg_index, sub_column_count, rec_def))
    {
        return -1;
    }
    msg_index += 2*sub_column_count; // 移动到第一条件 [columnX_name]

    // 第一条件
    std::vector<ColumnVar> colunms_to_check;
    int column_count_to_check = (int(msg.GetCount()) - msg_index)/2;
    if (!GetColumnsFromArgs(&colunms_to_check, msg, msg_index, column_count_to_check, rec_def))
    {
        return -1;
    }

    CVarList matched_rows;
    int result_row_count = 0;
    int result_column_count = 0;
    int search_ret = ReadRowOfOutputCols(matched_rows, &result_row_count, &result_column_count, record,
        &colunms_to_check, &sub_colunms_to_check, start_row, row_count_wanted, rec_def);
    if (search_ret == 0)
    {
        out_result << record_name;
        out_result << start_row;
        out_result << row_count_wanted;
        out_result << result_row_count;
        out_result << result_column_count;
        out_result << matched_rows;
    }

    return 0;
}

// 更新(插入)条目
int DomainRankList::UpdateEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                                const IVarList& msg, int msg_start_index)
{
    /*
        msg 格式：

            [record_name][insert_if_not_exist]
            [search_column][search_column_value]
            [column1_name][column1_value]
            [column2_name][column2_value]
            ...

        out_result 格式

            N/A
    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return -1;
    }

    int msg_index = msg_start_index;
    const char *record_name = msg.StringVal(msg_index);
    int do_insert = msg.IntVal(++msg_index);

    RecordDefine *rec_def = GetRecordDefine(record_name);
    if (rec_def == NULL)
    {
        // 没有这个表的结构定义
        return -1;
    }

    IRecord *record = pDomainData->GetRecord(record_name);
    if (record == NULL)
    {
        // 表不存在
        return -1;
    }

    // 获取主键
    ++msg_index; // 移动到 [search_column]
    ColumnVar search_column;
    if (!GetColumnFromArgs(&search_column, msg, msg_index, rec_def))
    {
        return -1;
    }
    msg_index += 2; // 移动到 [column1_name]

    // 获取要更新的列

    int column_count_to_check = (int(msg.GetCount()) - msg_index)/2;
    std::vector<ColumnVar> colunms_to_update;
    colunms_to_update.push_back(search_column);

    if (!GetColumnsFromArgs(&colunms_to_update, msg, msg_index, column_count_to_check, rec_def))
    {
        return -1;
    }

    // 查找符合条件的行
    std::vector<int> found_rows;
    int count_updated = 0;
    
    FindRowsByColumn(record, &search_column, rec_def, &found_rows);

    if (found_rows.size() > 0)
    {
        // 更新找到的行
        
        std::vector<int>::iterator it_row = found_rows.begin();
        std::vector<int>::iterator it_row_end = found_rows.end();
        
        LoopBeginCheck(a);
        for (; it_row != it_row_end; ++it_row)
        {
            LoopDoCheck(a);
            
            int temp_row_id = *it_row;
            UpdateRow(record, temp_row_id, &colunms_to_update, rec_def, rec_def->associated_update_strategy);
            ++count_updated;
        }
    }
    else
    {
        // 没有找匹配的行
        
        if (do_insert)
        {
            InsertRow(record, &colunms_to_update, rec_def);
            ++count_updated;
        }
    }

    // 更新关联表
    LoopBeginCheck(b);
    for (int i=0; i<rec_def->associated_rank_list_count; ++i)
    {
        LoopDoCheck(b);
        
        IRecord *associated_record = pDomainData->GetRecord((rec_def->associated_rank_list[i]).c_str());
        if (associated_record == NULL)
        {
            continue;
        }

        std::vector<int> aso_found_rows;
        FindRowsByColumn(associated_record, &search_column, rec_def, &aso_found_rows);

        if (aso_found_rows.size() > 0)
        {
            // 更新找到的行

            std::vector<int>::iterator it_aso_row = aso_found_rows.begin();
            std::vector<int>::iterator it_aso_row_end = aso_found_rows.end();
            
            LoopBeginCheck(c);
            for (; it_aso_row != it_aso_row_end; ++it_aso_row)
            {
                LoopDoCheck(c);
                
                int temp_row_id = *it_aso_row;
                UpdateRow(associated_record, temp_row_id, &colunms_to_update, rec_def, rec_def->associated_update_strategy);
                ++count_updated;
            }
        }
        else
        {
            // 没有找匹配的行

            if (do_insert)
            {
                InsertRow(associated_record, &colunms_to_update, rec_def);
                ++count_updated;
            }
        }
    }

    return count_updated;
}

// 删除条目
int DomainRankList::DeleteEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                                const IVarList& msg, int msg_start_index)
{
    /*
        msg 格式：

            [record_name]
            [primary_key_column][primary_key_value]

        out_result 格式

            N/A
    */
    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return -1;
    }

    int msg_index = msg_start_index;
    const char *record_name = msg.StringVal(msg_index);

    RecordDefine *rec_def = GetRecordDefine(record_name);
    if (rec_def == NULL)
    {
        // 没有这个表的结构定义
        return -1;
    }

    IRecord *record = pDomainData->GetRecord(record_name);
    if (record == NULL)
    {
        // 表不存在
        return -1;
    }

    ++msg_index; // 移动到[primary_key_column]
    ColumnVar pk_column;
    if (!GetColumnFromArgs(&pk_column, msg, msg_index, rec_def))
    {
        return -1;
    }
    msg_index += 2;

    int count_deleted = DeleteRow(record, &pk_column, rec_def);

    return count_deleted;
}

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
bool DomainRankList::SwapEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace, const IVarList& msg,
                 int msg_start_index)
{
    /*
        msg 格式：

            [record_name]
            [primary_key_column of A][primary_key_value of A]
            [primary_key_column of B][primary_key_value of B]

        out_result 格式

            N/A
    */
    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return false;
    }

    int msg_index = msg_start_index;
    const char *record_name = msg.StringVal(msg_index);

    RecordDefine *rec_def = GetRecordDefine(record_name);
    if (rec_def == NULL)
    {
        // 没有这个表的结构定义
        return false;
    }

    IRecord *record = pDomainData->GetRecord(record_name);
    if (record == NULL)
    {
        // 表不存在
        return false;
    }

    ++msg_index; // 移动到[primary_key_column of A]
    ColumnVar pk_column_of_A;
    if (!GetColumnFromArgs(&pk_column_of_A, msg, msg_index, rec_def))
    {
        return false;
    }
    ++msg_index; // 跳过[column_value]
    
    ++msg_index; // 移动到[primary_key_column of B]
    ColumnVar pk_column_of_B;
    if (!GetColumnFromArgs(&pk_column_of_B, msg, msg_index, rec_def))
    {
        return false;
    }
    msg_index += 2;

    // 找到A所在行
    std::vector<int> found_A;
    FindRowsByColumn(record, &pk_column_of_A, rec_def, &found_A);
    if (found_A.size() == 0)
    {
        return false;
    }
    
    int row_id_of_A = found_A.at(0);
    CVarList entry_of_A;
    record->QueryRowValue(row_id_of_A, entry_of_A);
    
    // 找到B所在行
    std::vector<int> found_B;
    FindRowsByColumn(record, &pk_column_of_B, rec_def, &found_B);
    if (found_B.size() == 0)
    {
        return false;
    }
    
    int row_id_of_B = found_B.at(0);
    CVarList entry_of_B;
    record->QueryRowValue(row_id_of_B, entry_of_B);
    
    // 交换
    record->SetRowValue(row_id_of_A, entry_of_B);
    record->SetRowValue(row_id_of_B, entry_of_A);

    return true;
}

// 清空排行榜
int DomainRankList::ClearRankList(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                                  const IVarList& msg)
{
    /*
        msg 格式：

            [rank_list_name]
            或
            [rank_list_name][last_clear_date][next_clear_date]
    */
    int msg_size = (int)msg.GetCount();

    std::string record_name = msg.StringVal(0);
    std::string last_clear_date = "";
    std::string next_clear_date = "";
    if (msg_size == 3)
    {
        last_clear_date = msg.StringVal(1);
        next_clear_date = msg.StringVal(2);
    }

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return -1;
    }

    RecordDefine *rec_def = GetRecordDefine(record_name.c_str());
    if (rec_def == NULL)
    {
        // 没有这个表的结构定义
        return -1;
    }

    IRecord *record = pDomainData->GetRecord(rec_def->rec_name.c_str());
    if (record == NULL)
    {
        // 表不存在
        return -1;
    }

    record->ClearRow();

	if(rec_def->clear_strategy != RANKING_CLEAR_ON_REQUEST)
	{
		// 更新此排行榜的更新日期
		ResetClearDate(pDomainData, rec_def->rec_name, last_clear_date, next_clear_date);
	}

    CVarList clear_msg;
    clear_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    clear_msg << RANKING_SUB_MSG_CLEAR_PLANNED;
    clear_msg << rec_def->rec_name.c_str();
    clear_msg << last_clear_date.c_str();
    clear_msg << next_clear_date.c_str();

    // 通知场景服务器, 排行榜已按计划清空
    // 0 表示发给第一个场景
    pPubKernel->SendPublicMessage(0, 0, clear_msg);

    return 0;
}

// 查询@column_name 在排行榜@rank_list_name 中的索引
// 找不到时返回-1
int DomainRankList::GetColumnIndex(const std::string &column_name,
                                   const std::string &rank_list_name)
{
    const RecordDefine * rec_def = GetRecordDefine(rank_list_name.c_str());
    if (rec_def == NULL)
    {
        return -1;
    }

    return GetColumnIndex(column_name, rec_def);
}

// 查询@column_name 在排行榜@rank_list_name 中的索引
// 找不到时返回-1
int DomainRankList::GetColumnIndexInOutput(const std::string &column_name,
                                           const std::string &rank_list_name)
{
    const RecordDefine * rec_def = GetRecordDefine(rank_list_name.c_str());
    if (rec_def == NULL)
    {
        return -1;
    }

    int index = GetColumnIndex(column_name, rec_def);

    LoopBeginCheck(a);
    for (int i=0; i<rec_def->output_columns_count; i++)
    {
        LoopDoCheck(a);
        
        if (index == rec_def->output_columns[i])
        {
            return i;
        }
    }

    return -1;
}

// 根据等级和国家找到符合条件的一组玩家
bool DomainRankList::FindPlayerUidByLevelNation(IPubKernel* pPubKernel, IPubSpace * pPubSpace, int nLevel
												, int nNation, IVarList& outData, const std::vector<std::string>& vRecUid)
{
	outData.Clear();

	// 临时等级变量
	int tempLevel = nLevel;

	// 存放符合条件的玩家uid
	std::vector<const char*> vFitPlayerUid;

	// 域名空间
	const std::wstring &domain_name = GetDomainName(pPubKernel);
	IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());
	if (pDomainData == NULL)
	{
		return false;
	}

	LoopBeginCheck(a)
	while( vFitPlayerUid.empty() && tempLevel > 0 )
	{
		LoopDoCheck(a)
		std::string strRec = GetDifLvlRankListName(tempLevel);

		// 找到排行榜
		IRecord* pRec = pDomainData->GetRecord(strRec.c_str());
		if (NULL == pRec)
		{
			return false;
		}

		int nCount = pRec->GetCols();
		// 找到国家相同的一组玩家数据
		int nRows = pRec->GetRows();

		int nNationColumn = GetColumnIndex("player_nation", strRec);
		int nOfflineTypeColum = GetColumnIndex("player_online_type", strRec);

		LoopBeginCheck(b);
		for (int i = 0;i < nRows;++i)
		{
			LoopDoCheck(b);

			int nCurNation = pRec->QueryInt(i, nNationColumn);
			//int nCurOnline = pRec->QueryInt(i, nOfflineTypeColum);
			if (nCurNation == nNation)
			{
				// 第1列为玩家uid
				const char* strUid = pRec->QueryString(i, 0);
				// 判断下是否为拷贝的玩家数据
				std::vector<std::string>::const_iterator iter = find(vRecUid.begin(), vRecUid.end(), std::string(strUid));
				if (iter == vRecUid.end())
				{
					vFitPlayerUid.push_back(strUid);
				}
			}
		}

		// 根据榜单规则降低等级
		tempLevel = CompatiblePlayerLevel(tempLevel);
	}

	if (vFitPlayerUid.empty())
	{
		return false;
	}

	// 随机一个符合条件玩家数据
	int nRandomInex = util_random_int((int)vFitPlayerUid.size());
	const char* strFind = vFitPlayerUid[nRandomInex];
	outData << strFind;

	return true;
}

// 给定参数，查询排行榜
int DomainRankList::OnQuery(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                            int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [rank_list_name]
            [start_row][row_count_wanted]
            [column1_name][column1_value]
            [column2_name][column2_value]
            ...

        回应消息

            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [rank_list_name]
            [start_row][row_count_wanted]
            [result_row_count][column_count_per_row]
            [seq_no_all + seq_no_query + row_data...]
            ...

    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    int msg_index = 4; // 从[request_from] 开始
    int request_from = msg.IntVal(msg_index);
    PERSISTID requester = msg.ObjectVal(++msg_index);
    int user_defined_id = msg.IntVal(++msg_index);
    ++msg_index; // 移动到[rank_list_name]

    CVarList query_result;
    int query_ret = Query(pPubKernel, pPubSpace, query_result, msg, msg_index);
    if (query_ret == -1)
    {
        return 0;
    }

    // 回应消息
    CVarList response_msg;
    response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    response_msg << RANKING_SUB_MSG_QUERY;
    response_msg << request_from;
    response_msg << requester;      // 请求此数据的对象
    response_msg << user_defined_id;
    response_msg << query_result;

    // 回复消息
    pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}

// 条目有更新, 刷新排行榜
int DomainRankList::OnUpdateEntry(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
                                 int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [record_name][insert_if_not_exist]
            [primary_column][primary_column_value]
            [column1_name][column1_value]
            [column2_name][column2_value]
            ...

        回应消息

            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [record_name]
            [primary_column][primary_column_value]
            [updated_columns_count]
            
    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    int msg_index = 4; // 从[request_from] 开始
    int request_from = msg.IntVal(msg_index);
    PERSISTID requester = msg.ObjectVal(++msg_index);
    int user_defined_id =msg.IntVal(++msg_index);
    const char *record_name = msg.StringVal(++msg_index);
    
    int updated_count = UpdateEntry(pPubKernel, pPubSpace, msg, msg_index);
    if (updated_count == -1)
    {
        return 0;
    }
   
    CVarList response_msg;
    response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    response_msg << RANKING_SUB_MSG_UPDATE_ENTRY;
    response_msg << request_from;
    response_msg << requester;
    response_msg << user_defined_id;
    response_msg << record_name;
    response_msg.Append(msg, 9, 2); //请求参数的主键[column_name][column_value]
    response_msg << updated_count;     // 更新的行数

    pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}

// 删除排行榜的条目
int DomainRankList::OnDeleteEntry(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
                                 int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [record_name]
            [primary_key_column][primary_key_value]

        回应消息

            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [record_name]
            [primary_column][primary_column_value]
            [updated_columns_count]
    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    int msg_index = 4; // 从[request_from] 开始
    int request_from = msg.IntVal(msg_index);
    PERSISTID requester = msg.ObjectVal(++msg_index);
    int user_defined_id = msg.IntVal(++msg_index);
    const char *record_name = msg.StringVal(++msg_index);

    int deleted_count = DeleteEntry(pPubKernel, pPubSpace, msg, msg_index);
    if (deleted_count == -1)
    {
        return 0;
    }

    CVarList response_msg;
    response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    response_msg << RANKING_SUB_MSG_DELETE_ENTRY;
    response_msg << request_from;
    response_msg << requester;
    response_msg << user_defined_id;
    response_msg << record_name;
    response_msg.Append(msg, 8, 2); //请求参数的主键[column_name][column_value]
    response_msg << deleted_count; // 删除的行数

    pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}


// 清空整个排行榜
int DomainRankList::OnClear(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
            int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]
            
            [request_from]
            [request_obj]

            [user_defined_id]
            [record_name]

        回应消息

            [msg_id][sub_msg_id]
            
            [request_from]
            [request_obj]

            [user_defined_id]
            [record_name]
            [clear_ret] 0 表示成功 1 表示失败

    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    int msg_index = 4; // 从[request_from] 开始
    int request_from = msg.IntVal(msg_index);
    PERSISTID requester = msg.ObjectVal(++msg_index);
    int user_defined_id = msg.IntVal(++msg_index);
    const char *record_name = msg.StringVal(++msg_index);

    CVarList clear_msg;
    clear_msg << record_name;
    int clear_ret = ClearRankList(pPubKernel, pPubSpace, clear_msg);
    if (clear_ret == -1)
    {
        return 0;
    }

    CVarList response_msg;
    response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    response_msg << RANKING_SUB_MSG_CLEAR;
    response_msg << request_from;
    response_msg << requester;
    response_msg << user_defined_id;
    response_msg << record_name;
    response_msg << SUCCESS_RET_CODE;

    pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}

// 查询排行榜的数据日期
int DomainRankList::OnQueryRecordDate(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]

            [request_obj]

            [record_name 1]
            ...
            [record_name n]

        回应消息

            [msg_id][sub_msg_id]

            [request_obj]

            [record_name 1][last_clear_date][next_clear_date]
            ...
            [record_name n][last_clear_date][next_clear_date]
    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    // 找到辅助表
    IRecord *info_record = pDomainData->GetRecord(INFO_TABLE);
    if (info_record == NULL)
    {
        return 0;
    }

    int msg_size = (int)msg.GetCount();
    PERSISTID requester = msg.ObjectVal(4);
    int record_count = msg_size - 5;
    int record_index = 5;

    CVarList record_date;

    LoopBeginCheck(a);
    for (int i=0; i<record_count; ++i)
    {
        LoopDoCheck(a);
        
        const char *record_name = msg.StringVal(record_index + i);

        int row_id = info_record->FindString(0, record_name);
        if (row_id != -1)
        {
            CVarList temp_row;
            if (info_record->QueryRowValue(row_id, temp_row))
            {
                record_date << temp_row;
            }
        }
    }

    CVarList response_msg;
    response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    response_msg << RANKING_SUB_MSG_DATE;
    response_msg << requester;
    response_msg << record_date;

    pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}

// 交换排行榜中的两行
int DomainRankList::OnSwapEntry(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
                int source_id, int scene_id, const IVarList& msg)
{
    /*
        请求消息：

            [PUBSPACE][DOMAIN_NAME]
            [msg_id][sub_msg_id]

            [request_from]
            [requester_obj]

            [user_defined_id]
            [record_name]
            [primary_column_of_A][primary_column_value]
            [primary_column_of_B][primary_column_value]


            
    */

    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());

    if (pDomainData == NULL)
    {
        return 0;
    }

    int msg_index = 4; // 从[request_from] 开始
    int request_from = msg.IntVal(msg_index);
    PERSISTID requester = msg.ObjectVal(++msg_index);
    int user_defined_id =msg.IntVal(++msg_index);
    const char *record_name = msg.StringVal(++msg_index);
    
    SwapEntry(pPubKernel, pPubSpace, msg, msg_index);
    //if (updated_count == -1)
    //{
    //    return 0;
    //}
   
    //CVarList response_msg;
    //response_msg << SP_DOMAIN_MSG_GENERAL_RANK_LIST;
    //response_msg << RANKING_SUB_MSG_SWAP;
    //response_msg << request_from;
    //response_msg << requester;
    //response_msg << user_defined_id;
    //response_msg << record_name;
    //response_msg.Append(msg, 9, 2); //请求参数的主键[column_name][column_value]
    //response_msg << updated_count;     // 更新的行数

    //pPubKernel->SendPublicMessage(source_id, scene_id, response_msg);

    return 0;
}

// 初始化排行榜
void DomainRankList::InitAllRankList(IPubKernel* pPubKernel, IPubSpace* pPubSpace)
{
    const std::wstring &domain_name = GetDomainName(pPubKernel);
    IPubData * pDomainData = pPubSpace->GetPubData(domain_name.c_str());
    if (pDomainData == NULL)
    {
        return ;
    }

    // 创建辅助表
    CreateInfoTable(pDomainData);

    RecordDefineMap::iterator it = m_RecordDefine.begin();
    RecordDefineMap::iterator it_end = m_RecordDefine.end();
    
    LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(a);
        
        RecordDefine &rec_def = it->second;

        bool create_new = false;
        OldDataStrategy old_data_strategy = KEEP_OLD_DATA;

        if (!pDomainData->FindRecord(rec_def.rec_name.c_str()))
        {
            // 排行榜不存在
            create_new = true;
        }
        else
        {
            IRecord *rec = pDomainData->GetRecord(rec_def.rec_name.c_str());
            if (rec != NULL)
            {
                // 查看列数是否相同
                if (!create_new)
                {
                    if (rec_def.columns_count != rec->GetCols() || rec_def.max_row > rec->GetRowMax())
                    { 
                        create_new = true;
                    }
                    
                    if (rec_def.columns_count < rec->GetCols())
                    {
                        // 新榜的列数减少了，不能保存老数据
                        old_data_strategy = DISCARD_OLD_DATA;
                    }
                }
            
                // 查看每一列的定义是否相同
                if (!create_new)
                {
                    // 列数相同, 查看列类型定义是否相同
                    
                    LoopBeginCheck(b);
                    for (int i=0; i<rec_def.columns_count; i++)
                    {
                        LoopDoCheck(b);
                        
                        if (rec->GetColType(i) != rec_def.columns_type[i])
                        {
                            create_new = true;
                            old_data_strategy = DISCARD_OLD_DATA;
                            break;
                        }
                    }
                }

            }// record != NULL

        } // End of 表检查

        if (create_new)
        {
            bool created = RecreateRankList(pDomainData, rec_def, old_data_strategy);

            if (created)
            {
                // 设置索引列(主键)
                pDomainData->SetRecordKey(rec_def.rec_name.c_str(), rec_def.pk_column);
                pDomainData->SetRecordSave(rec_def.rec_name.c_str(), true);
            }
            else
            {
                std::string err_msg = rec_def.rec_name;
                err_msg.append(" Recreate failed.");
				::extend_warning_pub(LOG_ERROR, err_msg.c_str());
            }
        }
        else
        {
            // 排行榜的表已经存在，设置必要的列的初始值
            if (rec_def.columns_to_reset_count > 0)
            {
                IRecord *record = pDomainData->GetRecord(rec_def.rec_name.c_str());
                if (record != NULL)
                {
                    ResetRankListColumn(record, &rec_def);
                }
            }
        }
    }// End of 排行榜循环

    return;
}

// 根据列查找一行数据, 返回行ID, 没找到返回-1
int DomainRankList::FindRowByColumn(IRecord *the_record,
                                   const ColumnVar *the_column,
                                   const RecordDefine *rec_def)
{
    int row_index = -1;
    int column_type = rec_def->columns_type[the_column->column_index];
    
    switch (column_type)
    {
    case VTYPE_INT:
        row_index = the_record->FindInt(the_column->column_index, the_column->int_value);
        break;
    case VTYPE_INT64:
        row_index = the_record->FindInt64(the_column->column_index, the_column->int64_value);
        break;
    case VTYPE_FLOAT:
        row_index = the_record->FindFloat(the_column->column_index, the_column->float_value);
        break;
    case VTYPE_DOUBLE:
        row_index = the_record->FindDouble(the_column->column_index, the_column->double_value);
        break;
    case VTYPE_STRING:
        row_index = the_record->FindString(the_column->column_index, the_column->str_value);
        break;
    case VTYPE_WIDESTR:
        row_index = the_record->FindWideStr(the_column->column_index, the_column->wstr_value);
        break;
    default:
        break;
    }

    return row_index;
}


// 从@the_record中查找符合条件@the_column的所有行索引
void DomainRankList::FindRowsByColumn(IRecord *the_record, const ColumnVar *the_column,
                     const RecordDefine *rec_def, std::vector<int> *row_id_list)
{
    int row_index = -1;
    int column_type = rec_def->columns_type[the_column->column_index];

    // 最大行号
    int max_row_id = the_record->GetRows() - 1;

    switch (column_type)
    {
    case VTYPE_INT:
        {
            row_index = the_record->FindInt(the_column->column_index, the_column->int_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindInt(the_column->column_index, the_column->int_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case VTYPE_INT64:
        {
            row_index = the_record->FindInt64(the_column->column_index, the_column->int64_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindInt64(the_column->column_index, the_column->int64_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case VTYPE_FLOAT:
        {
            row_index = the_record->FindFloat(the_column->column_index, the_column->float_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindFloat(the_column->column_index, the_column->float_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case VTYPE_DOUBLE:
        {
            row_index = the_record->FindDouble(the_column->column_index, the_column->double_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindDouble(the_column->column_index, the_column->double_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case VTYPE_STRING:
        {
            row_index = the_record->FindString(the_column->column_index, the_column->str_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindString(the_column->column_index, the_column->str_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case VTYPE_WIDESTR:
        {
            row_index = the_record->FindWideStr(the_column->column_index, the_column->wstr_value);
            
            LoopBeginCheck(a)
            while (row_index != -1)
            {
                LoopDoCheck(a)
                
                row_id_list->push_back(row_index);

                if (row_index != max_row_id)
                {
                    row_index = the_record->FindWideStr(the_column->column_index, the_column->wstr_value, row_index+1);
                }
                else
                {
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}

// 从@record中取出第@row_id行的数据，放入@dest中, 
// 由@rec_def.output_columns 来决定放入哪些列 
int DomainRankList::ReadRowOfOutputCols(IVarList &dest, IRecord *record, int row_id,
                           const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->output_columns_count; i++)
    {
        LoopDoCheck(a);
        
        int column_index = rec_def->output_columns[i];

        switch (rec_def->columns_type[column_index])
        {
        case VTYPE_INT:
            dest << record->QueryInt(row_id, column_index);
            break;
        case VTYPE_INT64:
            dest << record->QueryInt64(row_id, column_index);
            break;
        case VTYPE_FLOAT:
            dest << record->QueryFloat(row_id, column_index);
            break;
        case VTYPE_DOUBLE:
            dest << record->QueryDouble(row_id, column_index);
            break;
        case VTYPE_STRING:
            dest << record->QueryString(row_id, column_index);
            break;
        case VTYPE_WIDESTR:
            dest << record->QueryWideStr(row_id, column_index);
            break;
        default:
            return -1;
        }
    }

    return 0;
}

// 根据 @columns查询结果， 从结果的第@start_row开始获取@row_count_wanted条数据, 放入@out_list
// @row_count 记录返回的行数, @coulumn_count 记录返回的列数
int DomainRankList::ReadRowOfOutputCols(IVarList &out_list, int *row_count, int *coulumn_count,
                        IRecord *record, const std::vector<ColumnVar> *columns,
                        int start_row, int row_count_wanted, const RecordDefine *rec_def)
{
    int read_ret = 0;
    int current_row_count = record->GetRows();
    if (row_count_wanted == 0)
    {
        row_count_wanted = current_row_count;
    }

    std::vector<int> matched_row;
    int matched_row_index = 0;
    int saved_count = 0;

    // 查找符合条件的row的索引
    
    LoopBeginCheck(a);
    for (int i=0; i<current_row_count; i++)
    {
        LoopDoCheck(a);
        
        if (!IsRowMatch(i, record, columns, rec_def))
        {
            continue;
        }
        
        // 匹配行数增加
        matched_row_index ++;

        if (matched_row_index < start_row)
        {
            // 匹配行数小于要求起点
            continue;
        }

        // 达到要求的起点@start_row, 读取一行数据
        out_list << i+1; // i 从零开始计数, 需要+1转换
        out_list << matched_row_index;
        read_ret = ReadRowOfOutputCols(out_list, record, i, rec_def);
        if (read_ret == -1)
        {
            return -1;
        }

        saved_count++;

        if (saved_count == row_count_wanted)
        {
            break;
        }
    }

    *row_count = saved_count;
    *coulumn_count = rec_def->output_columns_count + 2; // 因为增加了 seq_no_all 和 seq_no_query 两列

    return 0;
}

// 根据 @columns查询结果， 从结果的第@start_row开始获取@row_count_wanted条数据, 然后再
// 结果中检索符合条件@sub_column 的条目, 放入@out_list
// @row_count 记录返回的行数, @coulumn_count 记录返回的列数
// 返回数据格式是: [seq_no_all][seq_no_query][column1]...
int DomainRankList::ReadRowOfOutputCols(IVarList &out_list, int *row_count, int *coulumn_count,
                        IRecord *record, const std::vector<ColumnVar> *columns, 
                        const std::vector<ColumnVar> *sub_columns,
                        int start_row, int row_count_wanted, const RecordDefine *rec_def)
{
    int read_ret = 0;
    int current_row_count = record->GetRows();
    if (row_count_wanted == 0)
    {
        row_count_wanted = current_row_count;
    }

    std::vector<int> matched_row;
    int matched_row_index = 0;
    int saved_count = 0;

    // 查找符合条件的row的索引
    
    LoopBeginCheck(a);
    for (int i=0; i<current_row_count; i++)
    {
        LoopDoCheck(a);
        
        if (!IsRowMatch(i, record, columns, rec_def))
        {
            continue;
        }

        // 匹配行数增加
        matched_row_index ++;

        if (matched_row_index < start_row)
        {
            // 匹配行数小于要求起点
            continue;
        }

        // 检查是否符合第二条件
        if (!IsRowMatch(i, record, sub_columns, rec_def))
        {
            continue;
        }

        // 达到要求的起点@start_row, 读取一行数据
        out_list << i+1; // i 从零开始计数, 需要+1转换
        out_list << matched_row_index;
        read_ret = ReadRowOfOutputCols(out_list, record, i, rec_def);
        if (read_ret == -1)
        {
            return -1;
        }

        saved_count++;

        if (saved_count == row_count_wanted)
        {
            break;
        }
    }

    *row_count = saved_count;
    *coulumn_count = rec_def->output_columns_count + 2; // 因为增加了 seq_no_all 和 seq_no_query 两列

    return 0;
}

// 根据@column 查看列是否匹配
bool DomainRankList::IsRowMatch(int row_id, IRecord *record,
                                const std::vector<ColumnVar> *columns,
                                const RecordDefine *rec_def)
{
    std::vector<ColumnVar>::const_iterator it = columns->begin();
    std::vector<ColumnVar>::const_iterator it_end = columns->end();

    LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(a);
        
        const ColumnVar &column_var = *it;
        int column_index = column_var.column_index;
        int column_type = rec_def->columns_type[column_index];

        switch (column_type)
        {
        case VTYPE_INT:
            {
                int val = record->QueryInt(row_id, column_index);

                if (column_var.int_value != val)
                {
                    return false;
                }
            }
            break;
        case VTYPE_INT64:
            {
                int64_t val = record->QueryInt64(row_id, column_index);

                if (column_var.int64_value != val)
                {
                    return false;
                }
            }
            break;
        case VTYPE_FLOAT:
            {
                float val = record->QueryFloat(row_id, column_index);

                if (column_var.float_value != val)
                {
                    return false;
                }
            }
            break;
        case VTYPE_DOUBLE:
            {
                double val = record->QueryDouble(row_id, column_index);

                if (column_var.double_value != val)
                {
                    return false;
                }
            }
            break;
        case VTYPE_STRING:
            {
                const char *val = record->QueryString(row_id, column_index);

                if (strcmp(val, column_var.str_value) != 0)
                {
                    return false;
                }
            }
            break;
        case VTYPE_WIDESTR:
            {
                const wchar_t *val = record->QueryWideStr(row_id, column_index);

                if (wcscmp(val, column_var.wstr_value) != 0)
                {
                    return false;
                }
            }
            break;
        default:
            return false;
        }
    }

    return true;
}

// 从@record中取出第@row_id行的数据，放入@dest中
int DomainRankList::ReadRow(IVarList &dest, IRecord *record, int row_id,
                            const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->columns_count; i++)
    {
        LoopDoCheck(a);
        
        switch (rec_def->columns_type[i])
        {
        case VTYPE_INT:
            dest << record->QueryInt(row_id, i);
            break;
        case VTYPE_INT64:
            dest << record->QueryInt64(row_id, i);
            break;
        case VTYPE_FLOAT:
            dest << record->QueryFloat(row_id, i);
            break;
        case VTYPE_DOUBLE:
            dest << record->QueryDouble(row_id, i);
            break;
        case VTYPE_STRING:
            dest << record->QueryString(row_id, i);
            break;
        case VTYPE_WIDESTR:
            dest << record->QueryWideStr(row_id, i);
            break;
        default:
            dest << "";
        }
    }

    return 0;
}

// 从@record中取出第@row_id行的数据，放入@row_data中, 
int DomainRankList::ReadRow(std::vector<ColumnVar> *row_data, IRecord *record, int row_id,
            const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->columns_count; i++)
    {
        LoopDoCheck(a);
        
        ColumnVar column;        
        column.column_index = i;

        int x = sizeof(column);
        ColumnVar * p = &column;

        switch (rec_def->columns_type[i])
        {
        case VTYPE_INT:
            column.int_value = record->QueryInt(row_id, i);
            break;
        case VTYPE_INT64:
            column.int64_value = record->QueryInt64(row_id, i);
            break;
        case VTYPE_FLOAT:
            column.float_value = record->QueryFloat(row_id, i);
            break;
        case VTYPE_DOUBLE:
            column.double_value = record->QueryDouble(row_id, i);
            break;
        case VTYPE_STRING:
            {
                const char *src = record->QueryString(row_id, i);
                StrSafeCopy(column.str_value, sizeof(column.str_value), src);
            }
            break;
        case VTYPE_WIDESTR:
            {
                const wchar_t *src = record->QueryWideStr(row_id, i);
                WstrSafeCopy(column.wstr_value, sizeof(column.wstr_value), src);
            }
            break;
        default:
            break;
        }

        row_data->push_back(column);
    }

    return 0;
}

// 插入一行数据
int DomainRankList::InsertRow(IRecord *the_record,
                             const std::vector<ColumnVar> *columns,
                             const RecordDefine *rec_def)
{
    // 初始化一行数据
    CVarList row_data;
    InitRowData(row_data, rec_def);

    // 设置此行数据
    std::vector<ColumnVar>::const_iterator it = columns->begin();
    std::vector<ColumnVar>::const_iterator it_end = columns->end();
    
    LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(a);
        
        const ColumnVar &column = *it;
        SetColunmOfRow(row_data, &column, rec_def);
    }
    
    // 检查是否需要排序
    if (rec_def->sort_columns_count == 0)
    {
        // 不需要排序的排行榜, 直接插入到最后
        return InsertToRecord(row_data, -1, the_record, rec_def);
    }

    // TODO 改进搜索算法
    // 寻找合适的插入位置
    int row_count = the_record->GetRows();
    
    LoopBeginCheck(b);
    for (int i=0; i<row_count; i++)
    {
        LoopDoCheck(b);
        
        CVarList temp_row;
        ReadRow(temp_row, the_record, i, rec_def);

        int compare_ret = CompareRowData(row_data, temp_row, rec_def);

        if (rec_def->use_asc)
        {
            // 升序排列
            // 找到一个优先级高的行,然后插入在它前面
            if (compare_ret < 0)
            {
                int row_id = (i-1) ? -1 : 0;
                return InsertToRecord(row_data, row_id, the_record, rec_def);
            }
        }
        else
        {
            // 降序排列 
            // 找到一个优先级低的行,然后插入在它前面
            if (compare_ret > 0)
            {
                int row_id = (i-1)==-1 ? 0 : i;
                return InsertToRecord(row_data, row_id, the_record, rec_def);
            }
        }
    }

    // 在表尾插入
    return InsertToRecord(row_data, -1, the_record, rec_def);
}

// 更新一行数据
int DomainRankList::UpdateRow(IRecord *the_record,
                             int row_id,
                             const std::vector<ColumnVar> *columns,
                             const RecordDefine *rec_def,
                             int update_strategy)
{
    // 记录是否需要重新排序
    bool need_resort = false;

    std::vector<ColumnVar>::const_iterator it = columns->begin();
    std::vector<ColumnVar>::const_iterator it_end = columns->end();
    
	std::vector<ColumnVar> old_row_data;
	ReadRow(&old_row_data, the_record, row_id, rec_def);

    LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(a);
        
        const ColumnVar &column = *it;
        SetColunmOfRow(the_record, row_id, &column, rec_def, update_strategy);

        if (!need_resort)
        {
            // 查看要更新的列是否包含了排序列
            
            LoopBeginCheck(b);
            for (int i=0; i<rec_def->sort_columns_count; i++)
            {
                LoopDoCheck(b);
                
                if (column.column_index == rec_def->sort_columns[i])
                {
					// SetColunmOfRow函数执行后column的值应该有变动,这里重新读取一下本条记录
					std::vector<ColumnVar> temp_column;
					ReadRow(&temp_column, the_record, row_id, rec_def);
					
					// 如果排序列有变动,则重新排序
					if (static_cast<int>(temp_column.size()) > column.column_index 
						&& static_cast<int>(old_row_data.size()) > column.column_index)
					{
						int cmp_ret = CompareColumnData(&temp_column[column.column_index], &old_row_data[column.column_index], rec_def);
						if (cmp_ret != 0)
						{
							need_resort = true;

							break;
						}
					}
                }
            }
        }
    }

    if (need_resort)	
    {   
        // 先删除，然后添加
        std::vector<ColumnVar> row_data;
        ReadRow(&row_data, the_record, row_id, rec_def);
        the_record->RemoveRow(row_id);
        InsertRow(the_record, &row_data, rec_def);
    }

    return 1;
}

// 删除一行数据
int DomainRankList::DeleteRow(IRecord *the_record,
                             const ColumnVar *pk_column,
                             const RecordDefine *rec_def)
{
    int row_id = FindRowByColumn(the_record, pk_column, rec_def);

    if (row_id == -1)
    {
        return 0;
    }

    if (the_record->RemoveRow(row_id))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// 从@args中取出一个列
bool DomainRankList::GetColumnFromArgs(ColumnVar *the_column,
                                     const IVarList &args,
                                     int args_index,
                                     const RecordDefine *rec_def)
{
    std::string column_name = args.StringVal(args_index);
    args_index++;

    int column_index = GetColumnIndex(column_name, rec_def);
    if (column_index == -1)
    {
        return false;
    }

    the_column->column_index = column_index;

    switch (rec_def->columns_type[column_index])
    {
    case VTYPE_INT:
        {
            if (args.GetType(args_index) == VTYPE_STRING)
            {
                const char *src = args.StringVal(args_index);
                StrSafeCopy(the_column->expression, sizeof(the_column->expression), src);
                the_column->value_type = VALUE_TYPE_EXP;
            }
            else
            {
                the_column->int_value = args.IntVal(args_index);
                the_column->value_type = VALUE_TYPE_IMMEDIATE;
            }
        }
        break;
    case VTYPE_INT64:
        {
            if (args.GetType(args_index) == VTYPE_STRING)
            {
                const char *src = args.StringVal(args_index);
                StrSafeCopy(the_column->expression, sizeof(the_column->expression), src);
                the_column->value_type = VALUE_TYPE_EXP;
            }
            else
            {
                the_column->int64_value = args.Int64Val(args_index);
                the_column->value_type = VALUE_TYPE_IMMEDIATE;
            }
        }
        break;
    case VTYPE_FLOAT:
        {

            if (args.GetType(args_index) == VTYPE_STRING)
            {
                const char *src = args.StringVal(args_index);
                StrSafeCopy(the_column->expression, sizeof(the_column->expression), src);
                the_column->value_type = VALUE_TYPE_EXP;
            }
            else
            {
                the_column->float_value = args.FloatVal(args_index);
                the_column->value_type = VALUE_TYPE_IMMEDIATE;
            }
        }
        break;
    case VTYPE_DOUBLE:
        {

            if (args.GetType(args_index) == VTYPE_STRING)
            {
                const char *src = args.StringVal(args_index);
                StrSafeCopy(the_column->expression, sizeof(the_column->expression), src);
                the_column->value_type = VALUE_TYPE_EXP;
            }
            else
            {
                the_column->double_value = args.DoubleVal(args_index);
                the_column->value_type = VALUE_TYPE_IMMEDIATE;
            }
        }
        break;
    case VTYPE_STRING:
        {
            const char *src = args.StringVal(args_index);
            StrSafeCopy(the_column->str_value, sizeof(the_column->str_value), src);
            the_column->value_type = VALUE_TYPE_IMMEDIATE;
        }
        break;
    case VTYPE_WIDESTR:
        {
            const wchar_t *src = args.WideStrVal(args_index);
            WstrSafeCopy(the_column->wstr_value, sizeof(the_column->wstr_value), src);
            the_column->value_type = VALUE_TYPE_IMMEDIATE;
        }
        break;
    default:
        return false;
        break;
    }

    return true;
}

// 从@args中取出多个列
bool DomainRankList::GetColumnsFromArgs(std::vector<ColumnVar> *columns,
                                     const IVarList &args,
                                     int args_index,
                                     int column_count,
                                     const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<column_count; i++)
    {
        LoopDoCheck(a);
        
        ColumnVar the_column;
        if (!GetColumnFromArgs(&the_column, args, args_index, rec_def))
        {
            return false;
        }
        args_index += 2; // 跳过 [column_name][column_value]

        columns->push_back(the_column);
    }
    
    return true;
}

// 读取配置
bool DomainRankList::LoadResource(IPubKernel* pPubKernel, IPubSpace* pPubSpace)
{

    if (!LoadColunmTypeConfig(pPubKernel, pPubSpace))
    {
        return false;
    }
    

    if (!LoadColunmNameConfig(pPubKernel, pPubSpace))
    {
        return false;
    }

    if (!LoadRankListConfig(pPubKernel, pPubSpace))
    {
        return false;
    }

    return true;
}

// 列类型配置
bool DomainRankList::LoadColunmTypeConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace)
{
    /*
        <Property ID="int" Type="2"/>
    */

    std::string res_path = pPubKernel->GetResourcePath();

    std::string config_file = res_path;
    config_file.append(RANK_LIST_COLUMN_TYPE_FILE);

    CXmlFile xml(config_file.c_str());
    if (!xml.LoadFromFile())
    {
		::extend_warning_pub(LOG_ERROR, "%s does not exists.", config_file.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a);
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a);
        
        std::string type_name = sec_list.StringVal(i);
        int type_value = xml.ReadInteger(type_name.c_str(), "Type", -1);

        if (type_name.length() == 0)
        {                
			::extend_warning_pub(LOG_ERROR, "%s: empty column type found.", config_file.c_str());

            return false;
        }

        if (type_value == -1)
        {
			::extend_warning_pub(LOG_ERROR, "%s:invalid column type value found. column is %s",
                config_file.c_str(), type_name.c_str());

            return false;
        }

        m_ColumnType.insert(ColumnTypeMap::value_type(type_name, type_value));
    }

    return true;
}

// 列名配置
bool DomainRankList::LoadColunmNameConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace)
{
    /*
        <Property ID="player_uid" ValueType="str" InitValue="" /> 
    */
    std::string res_path = pPubKernel->GetResourcePath();

    std::string config_file = res_path;
    config_file.append(RANK_LIST_COLUMN_FILE);

    CXmlFile xml(config_file.c_str());
    if (!xml.LoadFromFile())
    {
		::extend_warning_pub(LOG_ERROR, "%s does not exists.", config_file.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a);
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a);
        
        std::string column_name = sec_list.StringVal(i);
        std::string column_str_type = xml.ReadString(column_name.c_str(), "ValueType", "");
        std::string column_init_value = xml.ReadString(column_name.c_str(), "InitValue", "");
		bool IsNeedInit = xml.ReadInteger(column_name.c_str(), "IsNeedInit", 0) == 1;

        if (column_name.length() == 0)
        {                
			::extend_warning_pub(LOG_ERROR, "%s: empty column name found.", config_file.c_str());

            return false;
        }

        if (column_name.length() > RANK_LIST_MAX_COLUMN_NAME_LENGTH)
        {
			::extend_warning_pub(LOG_ERROR, "%s: Column name[%s] too long. max length is  %d",
                config_file.c_str(), column_name.c_str(), RANK_LIST_MAX_COLUMN_NAME_LENGTH);

            return false;
        }

        int the_type = GetColumnIntType(column_str_type);
        if (the_type == -1)
        {
			::extend_warning_pub(LOG_ERROR, "%s: invalid column type(%s).",
                config_file.c_str(), column_str_type.c_str());

            return false;
        }

        ColumnDefine col_def;
        col_def.column_name = column_name;
        col_def.column_type = the_type;
        col_def.init_value_in_str = column_init_value;
		col_def.bIsNeedInit = IsNeedInit;

        m_ColumnDefine.insert(ColumnDefineMap::value_type(col_def.column_name, col_def));
        //m_ColumnName.insert(ColumnNameMap::value_type(column_name, the_type));
    }

    return true;
}

// 排行榜配置
bool DomainRankList::LoadRankListConfig(IPubKernel* pPubKernel, IPubSpace* pPubSpace)
{
    /*
    <Property ID="player_level_rank_list" 
                MaxRows="1000" 
                ColumnsName="player_uid,player_name,player_career,player_nation,player_level,player_experience" 
                OutputColumns="player_name,player_career,player_nation,player_level,player_experience" 
                PrimaryKey="player_uid" 
                SortColumns="player_level,player_experience" 
                SortOrder="desc" /> 
    */
    std::string res_path = pPubKernel->GetResourcePath();

    std::string rank_list_config_file = res_path;
    rank_list_config_file.append(RANK_LIST_DEFINE_FILE);

    CXmlFile xml(rank_list_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = rank_list_config_file;
        err_msg.append(" does not exists.");
		::extend_warning_pub(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a);
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a);
        
		bool ignore = false;

        const char *rank_list_name = sec_list.StringVal(i);
        int associated_update_stragegy = xml.ReadInteger(rank_list_name, "AssociatedRankListStrategy", 0);
        const char *associated_rank_list = xml.ReadString(rank_list_name, "AssociatedRankList", "");
        int max_row_count = xml.ReadInteger(rank_list_name, "MaxRows", 1000);
        const char *columns_name = xml.ReadString(rank_list_name, "ColumnsName", "");
        const char *output_columns = xml.ReadString(rank_list_name, "OutputColumns", "");
        const char *primary_key = xml.ReadString(rank_list_name, "PrimaryKey", "");
        const char *sort_columns = xml.ReadString(rank_list_name, "SortColumns", "");
        const char *sort_order = xml.ReadString(rank_list_name, "SortOrder", "");
        const char *columns_to_reset = xml.ReadString(rank_list_name, "ColumnsToResetOnStart", "");
        int clear_strategy = xml.ReadInteger(rank_list_name, "ClearStrategy", 0);
		int clear_award = xml.ReadInteger(rank_list_name, "ClearAward", 0);
		int awardByNation = xml.ReadInteger(rank_list_name, "AwardByNation", 1);

        if (rank_list_name == NULL || columns_name == NULL
            || output_columns == NULL || primary_key== NULL
            || sort_columns == NULL || sort_order == NULL
            || columns_to_reset == NULL)
        {
            continue;
        }

        if (strlen(columns_name) == 0)
        {
			::extend_warning_pub(LOG_ERROR, "%s has no columns.", rank_list_name);

            continue;
        }

        RecordDefine rec_def;
        rec_def.rec_name = rank_list_name;
        rec_def.max_row = max_row_count;

        rec_def.associated_rank_list_count = 0;
        rec_def.columns_count = 0;
        rec_def.output_columns_count = 0;
        rec_def.sort_columns_count = 0;
        rec_def.columns_to_reset_count = 0;
        
        rec_def.associated_update_strategy = associated_update_stragegy;

        // 提取关联表
        SaveStrToArray(associated_rank_list, rec_def.associated_rank_list, RANK_LIST_MAX_ASSOCIATED,
            &(rec_def.associated_rank_list_count));

        // 提取全部列
        SaveStrToArray(columns_name, rec_def.columns_name, RANK_LIST_MAX_COLUMN_COUNT,
            &(rec_def.columns_count));

        ignore = false;
        
        LoopBeginCheck(b);
        for (int j=0; j<rec_def.columns_count; j++)
        {
            LoopDoCheck(b);
            
            rec_def.columns_type[j] = GetColumnType(rec_def.columns_name[j]);

            if (rec_def.columns_type[j] == -1)
            {
				::extend_warning_pub(LOG_ERROR, "%s: invalid column name %s.",
                    rank_list_name, (rec_def.columns_name[j]).c_str());

                ignore = true;
                break;
            }
        }
        
        if (ignore)
        {
            continue;
        }

        //提取主键列
        std::string str_pk = primary_key;
        rec_def.pk_column = GetColumnIndex(str_pk, &rec_def);
        if (rec_def.pk_column == -1)
        {
			::extend_warning_pub(LOG_ERROR, "%s: invalid primary column name %s.",
                rank_list_name, primary_key);

            continue;
        }

        // 提取输出的列
        std::string output_columns_array[RANK_LIST_MAX_COLUMN_COUNT];
        SaveStrToArray(output_columns, output_columns_array, RANK_LIST_MAX_COLUMN_COUNT,
            &(rec_def.output_columns_count));
            
        ignore = false;
        
        LoopBeginCheck(c);
        for (int j=0; j<rec_def.output_columns_count; j++)
        {
            LoopDoCheck(c);
            
            rec_def.output_columns[j] = GetColumnIndex(output_columns_array[j], &rec_def);

            if (rec_def.output_columns[j] == -1)
            {
				::extend_warning_pub(LOG_ERROR, "%s: invalid output column name %s.",
                    rank_list_name, (output_columns_array[j]).c_str());
                
                ignore = true;
                break;
            }
        }
        
        if (ignore)
        {
            continue;
        }

        // 提取排序的列
        std::string sort_column_array[RANK_LIST_MAX_COLUMN_COUNT];
        SaveStrToArray(sort_columns, sort_column_array, RANK_LIST_MAX_COLUMN_COUNT,
            &(rec_def.sort_columns_count));

        ignore = false;
        
        LoopBeginCheck(d);
        for (int j=0; j<rec_def.sort_columns_count; j++)
        {
            LoopDoCheck(d);
            
            rec_def.sort_columns[j] = GetColumnIndex(sort_column_array[j], &rec_def);

            if (rec_def.sort_columns[j] == -1)
            {
				::extend_warning_pub(LOG_ERROR, "%s: invalid sort column name %s.",
                    rank_list_name, (sort_column_array[j]).c_str());

                ignore = true;
                continue;
            }
        }
        
        if (ignore)
        {
            continue;
        }

        // 升序/降序
        if (strcmp(sort_order, "desc") == 0)
        {
            rec_def.use_asc = false;
        }
        else
        {
            rec_def.use_asc = true;
        }

        // 启动时重设的列
        std::string columns_to_reset_array[RANK_LIST_MAX_COLUMN_COUNT];        
        SaveStrToArray(columns_to_reset, columns_to_reset_array, RANK_LIST_MAX_COLUMN_COUNT,
            &(rec_def.columns_to_reset_count));

        ignore = false;
        
        LoopBeginCheck(e);
        for (int j=0; j<rec_def.columns_to_reset_count; j++)
        {
            LoopDoCheck(e);
            
            rec_def.columns_to_reset[j] = GetColumnIndex(columns_to_reset_array[j], &rec_def);

            if (rec_def.columns_to_reset[j] == -1)
            {
				::extend_warning_pub(LOG_ERROR, "%s: invalid reset column name %s.",
                    rank_list_name, (columns_to_reset_array[j]).c_str());

                ignore = true;
                break;
            }
        }
        
        if (ignore)
        {
            continue;
        }

        // 清空策略
        rec_def.clear_strategy = clear_strategy;
		rec_def.clear_award = clear_award;
		rec_def.bAwardByNation = awardByNation != 0;

        m_RecordDefine.insert(RecordDefineMap::value_type(rec_def.rec_name, rec_def));

    }

    return true;
}

// 根据表的名字获取表的定义
DomainRankList::RecordDefine * DomainRankList::GetRecordDefine(const char *rec_name)
{
    std::string search_key = rec_name;
    RecordDefineMap::iterator it = m_RecordDefine.find(search_key);
    if (it == m_RecordDefine.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

// 根据列名获取列名定义
const DomainRankList::ColumnDefine * DomainRankList::GetColumnDefine(const std::string &column_name)
{
    ColumnDefineMap::const_iterator it = m_ColumnDefine.find(column_name);
    if (it == m_ColumnDefine.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

// 根据字符表示的类型@type_name, 获取整数表示的类型
// 找不到时返回-1
int DomainRankList::GetColumnIntType(std::string &type_name)
{
    ColumnTypeMap::iterator it = m_ColumnType.find(type_name);
    if (it == m_ColumnType.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}

// 根据列名获取列类型
// 找不到时返回-1
int DomainRankList::GetColumnType(std::string &column_name)
{
    ColumnDefineMap::const_iterator it = m_ColumnDefine.find(column_name);
    if (it == m_ColumnDefine.end())
    {
        return -1;
    }
    else
    {
        const ColumnDefine &col_def = it->second;
        return col_def.column_type;
    }

    //ColumnNameMap::iterator it = m_ColumnName.find(column_name);
    //if (it == m_ColumnName.end())
    //{
    //    return -1;
    //}
    //else
    //{
    //    return it->second;
    //}
}


// 获取列@column_name在表@rec_def中的索引
// 找不到时返回-1
int DomainRankList::GetColumnIndex(const std::string &column_name, const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->columns_count; i++)
    {
        LoopDoCheck(a);
        
        if (column_name == rec_def->columns_name[i])
        {
            return i;
        }
    }

    return -1;
}

// 读取@the_column的值, 放入@dest
int DomainRankList::ReadColumnVar(CVarList &dest, const ColumnVar *the_column, const RecordDefine *rec_def)
{
    int column_type = rec_def->columns_type[the_column->column_index];

    switch (column_type)
    {
    case VTYPE_INT:
        dest << the_column->int_value;
        break;
    case VTYPE_INT64:
        dest << the_column->int64_value;
        break;
    case VTYPE_FLOAT:
        dest << the_column->float_value;
        break;
    case VTYPE_DOUBLE:
        dest << the_column->double_value;
        break;
    case VTYPE_STRING:
        dest << the_column->str_value;
        break;
    case VTYPE_WIDESTR:
        dest << the_column->wstr_value;
        break;
    default:
        dest << 0;
        break;
    }

    return 0;
}

// 设置@row_data指定列@the_column的值
int DomainRankList::SetColunmOfRow(CVarList &row_data, const ColumnVar *the_column,
                                  const RecordDefine *rec_def)
{
    int column_type = rec_def->columns_type[the_column->column_index];

    switch (column_type)
    {
    case VTYPE_INT:
		{
			// 操作表达式
			if (the_column->value_type == VALUE_TYPE_EXP)
			{
				// 解析表达式
				ColumnVar temp_column;
				temp_column.int_value = 0;

				ParseRightExp(&temp_column, VTYPE_INT, the_column->expression);

				row_data.SetInt(the_column->column_index, temp_column.int_value);
			}
			else
			{
				row_data.SetInt(the_column->column_index, the_column->int_value);
			}
		}
        break;
    case VTYPE_INT64:
		{
			// 操作表达式
			if (the_column->value_type == VALUE_TYPE_EXP)
			{
				// 解析表达式
				ColumnVar temp_column;
				temp_column.int64_value = static_cast<int64_t>(0);

				ParseRightExp(&temp_column, VTYPE_INT, the_column->expression);

				row_data.SetInt64(the_column->column_index, temp_column.int64_value);
			}
			else
			{
				row_data.SetInt64(the_column->column_index, the_column->int64_value);
			}
		}
        
        break;
    case VTYPE_FLOAT:
		{
			// 操作表达式
			if (the_column->value_type == VALUE_TYPE_EXP)
			{
				// 解析表达式
				ColumnVar temp_column;
				temp_column.float_value = 0.0f;

				ParseRightExp(&temp_column, VTYPE_INT, the_column->expression);

				row_data.SetFloat(the_column->column_index, temp_column.float_value);
			}
			else
			{
				row_data.SetFloat(the_column->column_index, the_column->float_value);
			}
		}
        break;
    case VTYPE_DOUBLE:
		{
			// 操作表达式
			if (the_column->value_type == VALUE_TYPE_EXP)
			{
				// 解析表达式
				ColumnVar temp_column;
				temp_column.double_value = 0.0f;

				ParseRightExp(&temp_column, VTYPE_INT, the_column->expression);

				row_data.SetDouble(the_column->column_index, temp_column.double_value);
			}
			else
			{
				row_data.SetDouble(the_column->column_index, the_column->double_value);
			}
		}
        break;
    case VTYPE_STRING:
        row_data.SetString(the_column->column_index,  the_column->str_value);
        break;
    case VTYPE_WIDESTR:
        row_data.SetWideStr(the_column->column_index, the_column->wstr_value);
        break;
    default:
        break;
    }

    return 0;
}

// 设置@the_record　第@row_id行的指定列@the_column的值
int DomainRankList::SetColunmOfRow(IRecord *the_record, int row_id, const ColumnVar *the_column,
                   const RecordDefine *rec_def, int update_strategy)
{
    int column_type = rec_def->columns_type[the_column->column_index];

    switch (column_type)
    {
    case VTYPE_INT:
        {
            int cur_val = the_record->QueryInt(row_id, the_column->column_index);
            
            if (the_column->value_type == VALUE_TYPE_EXP)
            {
                // 解析表达式
                ColumnVar temp_column;
                temp_column.int_value = cur_val;
                
                ParseRightExp(&temp_column, VTYPE_INT, the_column->expression);
                
                the_record->SetInt(row_id, the_column->column_index, temp_column.int_value);
            }
            else
            {
                // 根据更新策略来决定是否进行更新
                switch (update_strategy)
                {
                case UPDATE_STRATEGY_GREAT_THAN:
                {
                    if (the_column->int_value > cur_val)
                    {
                        the_record->SetInt(row_id, the_column->column_index, the_column->int_value); 
                    }
                }
                break;
                case UPDATE_STRATEGY_LESS_THAN:
                    {
                        if (the_column->int_value < cur_val)
                        {
                            the_record->SetInt(row_id, the_column->column_index, the_column->int_value); 
                        }
                    }
                    break;
                default:
                    the_record->SetInt(row_id, the_column->column_index, the_column->int_value); 
                }
            }
            
        }
        break;
    case VTYPE_INT64:
        {
            int64_t cur_val = the_record->QueryInt64(row_id, the_column->column_index);
            if (the_column->value_type == VALUE_TYPE_EXP)
            {
                // 解析表达式
                ColumnVar temp_column;
                temp_column.int64_value = cur_val;

                ParseRightExp(&temp_column, VTYPE_INT64, the_column->expression);

                the_record->SetInt64(row_id, the_column->column_index, temp_column.int64_value);
            }
            else
            {  
                // 根据更新策略来决定是否进行更新
                switch (update_strategy)
                {
                case UPDATE_STRATEGY_GREAT_THAN:
                    {
                        if (the_column->int64_value > cur_val)
                        {
                            the_record->SetInt64(row_id, the_column->column_index, the_column->int64_value); 
                        }
                    }
                    break;
                case UPDATE_STRATEGY_LESS_THAN:
                    {
                        if (the_column->int64_value < cur_val)
                        {
                            the_record->SetInt64(row_id, the_column->column_index, the_column->int64_value); 
                        }
                    }
                    break;
                default:
                    the_record->SetInt64(row_id, the_column->column_index, the_column->int64_value); 
                }
            }
        }
        break;
    case VTYPE_FLOAT:
        {
            float cur_val = the_record->QueryFloat(row_id, the_column->column_index);

            if (the_column->value_type == VALUE_TYPE_EXP)
            {
                // 解析表达式
                ColumnVar temp_column;
                temp_column.float_value = cur_val;

                ParseRightExp(&temp_column, VTYPE_FLOAT, the_column->expression);
                
                the_record->SetFloat(row_id, the_column->column_index, temp_column.float_value);
            }
            else
            {
                // 根据更新策略来决定是否进行更新
                switch (update_strategy)
                {
                case UPDATE_STRATEGY_GREAT_THAN:
                    {
                        if (the_column->float_value > cur_val)
                        {
                            the_record->SetFloat(row_id, the_column->column_index, the_column->float_value);
                        }
                    }
                    break;
                case UPDATE_STRATEGY_LESS_THAN:
                    {
                        if (the_column->float_value < cur_val)
                        {
                            the_record->SetFloat(row_id, the_column->column_index, the_column->float_value);
                        }
                    }
                    break;
                default:
                    the_record->SetFloat(row_id, the_column->column_index, the_column->float_value);
                }
            }
        }
        break;
    case VTYPE_DOUBLE:
        {
            double cur_val = the_record->QueryDouble(row_id, the_column->column_index);

            if (the_column->value_type == VALUE_TYPE_EXP)
            {
                // 解析表达式
                ColumnVar temp_column;
                temp_column.double_value = cur_val;

                ParseRightExp(&temp_column, VTYPE_DOUBLE, the_column->expression);

                the_record->SetDouble(row_id, the_column->column_index, temp_column.double_value);
            }
            else
            {
                // 根据更新策略来决定是否进行更新
                switch (update_strategy)
                {
                case UPDATE_STRATEGY_GREAT_THAN:
                    {
                        if (the_column->double_value > cur_val)
                        {
                            the_record->SetDouble(row_id, the_column->column_index, the_column->double_value);
                        }
                    }
                    break;
                case UPDATE_STRATEGY_LESS_THAN:
                    {
                        if (the_column->double_value < cur_val)
                        {
                            the_record->SetDouble(row_id, the_column->column_index, the_column->double_value);
                        }
                    }
                    break;
                default:
                    the_record->SetDouble(row_id, the_column->column_index, the_column->double_value);
                }
            }            
        }
        break;
    case VTYPE_STRING:
        the_record->SetString(row_id, the_column->column_index, the_column->str_value);
        break;
    case VTYPE_WIDESTR:
        the_record->SetWideStr(row_id, the_column->column_index, the_column->wstr_value);
        break;
    default:
        break;
    }

    return 0;
}

// 初始化表@rec_def一行的值, 放入@row_data
int DomainRankList::InitRowData(CVarList &row_data, const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->columns_count; i++)
    {
        LoopDoCheck(a);
        
        switch (rec_def->columns_type[i])
        {
        case VTYPE_INT:
            {
                int v = 0;
                row_data << v;
            }
            break;
        case VTYPE_INT64:
            {
                int64_t v = 0;
                row_data << v;
            }
            break;
        case VTYPE_FLOAT:
            row_data << 0.0F;
            break;
        case VTYPE_DOUBLE:
            {
                double v = 0.0;
                row_data << v;
            }
            break;
        case VTYPE_STRING:
            row_data << "";
            break;
        case VTYPE_WIDESTR:
            row_data << L"";
            break;
        default:
            break; 
        }
    }

    return 0;
}

// 比较两行数据@row1 和 @row2的优先级
int DomainRankList::CompareRowData(const CVarList &row1, const CVarList &row2,
                                  const RecordDefine *rec_def)
{
    LoopBeginCheck(a);
    for (int i=0; i<rec_def->sort_columns_count; i++)
    {
        LoopDoCheck(a);
        
        int column_index = rec_def->sort_columns[i];
        
        ColumnVar col1;
        ColumnVar col2;
        GetColumnValue(&col1, row1, column_index, rec_def);
        GetColumnValue(&col2, row2, column_index, rec_def);

        int cmp_ret = CompareColumnData(&col1, &col2, rec_def);

        if (cmp_ret != 0)
        {
            return cmp_ret;
        }
    }

    return 0;
}

// 从@row读取第@column_index列的值, 放入@the_column
int DomainRankList::GetColumnValue(ColumnVar *the_column, const CVarList &row,
                                  int column_index, const RecordDefine *rec_def)
{
    the_column->column_index = column_index;
    
    switch (rec_def->columns_type[column_index])
    {
    case VTYPE_INT:
        the_column->int_value = row.IntVal(column_index);
        break;
    case VTYPE_INT64:
        the_column->int64_value = row.Int64Val(column_index);
        break;
    case VTYPE_FLOAT:
        the_column->float_value = row.FloatVal(column_index);
        break;
    case VTYPE_DOUBLE:
        the_column->double_value = row.DoubleVal(column_index);
        break;
    case VTYPE_STRING:
        {
            const char *src = row.StringVal(column_index);
            StrSafeCopy(the_column->str_value, sizeof(the_column->str_value), src);
        }
        break;
    case VTYPE_WIDESTR:
        {
            const wchar_t *src = row.WideStrVal(column_index);
            WstrSafeCopy(the_column->wstr_value, sizeof(the_column->wstr_value), src);
        }
        break;
    default:
        
        break; 
    }

    return 0;
}

// 比较@column1和@column2的优先级
int DomainRankList::CompareColumnData(const ColumnVar *column1,
                                     const ColumnVar *column2,
                                     const RecordDefine *rec_def)
{
    if (column1->column_index != column2->column_index)
    {
        return 0;
    }

    int ret = 0;

    switch (rec_def->columns_type[column1->column_index])
    {
    case VTYPE_INT:
        ret = column1->int_value - column2->int_value;
        break;
    case VTYPE_INT64:
        ret = (int)(column1->int64_value - column2->int64_value);
        break;
    case VTYPE_FLOAT:
        ret = ((column1->float_value - column2->float_value) > 0 ? 1 : -1);
        break;
    case VTYPE_DOUBLE:
        ret = ((column1->double_value - column2->double_value) > 0 ? 1 : -1);
        break;
    case VTYPE_STRING:
        ret = (int)(strlen(column1->str_value) - strlen(column2->str_value));
        break;
    case VTYPE_WIDESTR:
        ret = (int)(wcslen(column1->wstr_value) - wcslen(column2->wstr_value));
        break;
    default:
        ret = 0;
        break; 
    }

    return ret;
}

// 把逗号分隔的字符串保存到数组
// 例如: "a,b,c" 保存到 {"a", "b", "c"}
bool DomainRankList::SaveStrToArray(const char *str_src,
                                  std::string *str_array,
                                  int array_length,
                                  int *actual_size)
{
    CVarList parse_result;
    util_split_string(parse_result, str_src, ",");

    int result_count = (int)parse_result.GetCount();
    if (result_count > array_length)
    {
        return false;
    }

    LoopBeginCheck(a);
    for (int i = 0; i<result_count; ++i)
    {
        LoopDoCheck(a);
        
        str_array[i] = parse_result.StringVal(i);
    }

    *actual_size = result_count;

    return true;
}


int DomainRankList::StrSafeCopy(char *dest, size_t dest_size, const char *src)
{
    memset(dest, 0, dest_size);
    size_t src_size = strlen(src);

    if (src_size >= dest_size)
    {
        memcpy(dest, src, dest_size-sizeof(char));
    }
    else
    {
        memcpy(dest, src, src_size);
    }

    return 0;
}

int DomainRankList::WstrSafeCopy(wchar_t *dest, size_t dest_size, const wchar_t *src)
{
    memset(dest, 0, dest_size);

    size_t src_size = wcslen(src) * sizeof(wchar_t);

    if (src_size >= dest_size)
    {
        memcpy(dest, src, dest_size-sizeof(wchar_t));
    }
    else
    {
        memcpy(dest, src, src_size);
    }

    return 0;
}

// 插入一行数据
int DomainRankList::InsertToRecord(const CVarList& row_data, int row_id, 
                              IRecord *record, const RecordDefine *rec_def)
{
    int row_count = record->GetRows();
    if (row_count == rec_def->max_row)
    {
         // 当前行数已经达到最大行数

        if (row_id == -1)
        {
            //不可以在最后插入
            return 0;
        }
        else
        {
            //插入前要删除一行
            record->RemoveRow(-1);
        }

    }

    if (row_id >= row_count)
    {
        if (record->AddRowValue(-1, row_data) > 0)
        {
            return 1;
        }
    }
    else
    {
        int new_row_id = record->AddRowValue(row_id, row_data);
        if (new_row_id >= 0)
        {
            return 1;
        }
    }

    return 0;
}

// 重设RankList的某些列
int DomainRankList::ResetRankListColumn(IRecord *record, const RecordDefine *rec_def)
{
    int row_count = record->GetRows();

    LoopBeginCheck(a);
    for (int row=0; row<row_count; row++)
    {
        LoopDoCheck(a);
        
        LoopBeginCheck(b);
        for (int j=0; j<rec_def->columns_to_reset_count; j++)
        {
            LoopDoCheck(b);
            
            int column_index = rec_def->columns_to_reset[j];
            const ColumnDefine * col_def = GetColumnDefine(rec_def->columns_name[column_index]);
            if (col_def == NULL)
            {
                continue;
            }

			if (!col_def->bIsNeedInit)
			{
				continue;
			}

            switch (col_def->column_type)
            {
            case VTYPE_INT:
                {
                    int val = atoi(col_def->init_value_in_str.c_str());
                    record->SetInt(row, column_index, val);
                }
                break;
            case VTYPE_INT64:
                {
                    __int64 val = util_string_as_int64(col_def->init_value_in_str);
                    record->SetInt64(row, column_index, val);
                }
                break;
            case VTYPE_FLOAT:
                {
                    float val = (float)atof(col_def->init_value_in_str.c_str());
                    record->SetFloat(row, column_index, val);
                }
                break;
            case VTYPE_DOUBLE:
                {
                    double val = atof(col_def->init_value_in_str.c_str());
                    record->SetDouble(row, column_index, val);
                }
                break;
            case VTYPE_STRING:
                record->SetString(row, column_index, col_def->init_value_in_str.c_str());
                break;
            case VTYPE_WIDESTR:
                {
                    std::wstring val = util_string_as_widestr(col_def->init_value_in_str.c_str());
                    record->SetWideStr(row, column_index, val.c_str());
                }
                break;
            default:
                break; 
            }
        }
    }

    return 0;
}

// 对排行榜的辅助表的操作
// 排行榜的辅助表, 用来记录排行榜的上次清空时间和下次清空时间
bool DomainRankList::CreateInfoTable(IPubData * pPubData)
{
    bool do_create = false;
    if (pPubData->FindRecord(INFO_TABLE))
    {
        IRecord *record = pPubData->GetRecord(INFO_TABLE);
        if (record == NULL)
        {
            return false;
        }

        // 列数不同
        if (record->GetCols() != INFO_TABLE_COLUMN_COUNT)
        {
            pPubData->RemoveRecord(INFO_TABLE);
            do_create = true;
        }
    }
    else
    {
        do_create = true;
    }

    if (do_create)
    {
        CVarList column;
        column << VTYPE_STRING << VTYPE_STRING << VTYPE_STRING;

        return pPubData->AddRecord(INFO_TABLE, INFO_TABLE_MAX_ROW,
            INFO_TABLE_COLUMN_COUNT, column);
    }

    return true;
}

bool DomainRankList::AddToInfoTable(IPubData * pPubData, const std::string &entry_name)
{
    IRecord *rec = pPubData->GetRecord(INFO_TABLE);
    if (rec == NULL)
    {
        return false;
    }

    int row_id = rec->FindString(0, entry_name.c_str());
    if (row_id == -1)
    {
        CVarList entry;
        entry << entry_name.c_str() << "" << "";
        rec->AddRowValue(row_id, entry);
    }

    return true;
}

bool DomainRankList::ResetClearDate(IPubData * pPubData, const std::string &entry_name, 
                                    const std::string &last_clear_date,
                                    const std::string &next_clear_date)
{
    IRecord *rec = pPubData->GetRecord(INFO_TABLE);
    if (rec == NULL)
    {
        return false;
    }

    int row_id = rec->FindString(0, entry_name.c_str());
    if (row_id == -1)
    {
        CVarList entry;
        entry << entry_name.c_str();
        entry << last_clear_date.c_str();
        entry << next_clear_date.c_str();

        rec->AddRowValue(row_id, entry);
    }
    else
    {
        rec->SetString(row_id, 1, last_clear_date.c_str());
        rec->SetString(row_id, 2, next_clear_date.c_str());
    }

    return true;
}

bool DomainRankList::GetClearDate(IPubData * pPubData, const std::string &entry_name,
                                  std::string &last_clear_date, std::string &next_clear_date)
{
    IRecord *rec = pPubData->GetRecord(INFO_TABLE);
    if (rec == NULL)
    {
        return false;
    }

    int row_id = rec->FindString(0, entry_name.c_str());
    if (row_id == -1)
    {
        return false;
    }
    else
    {
        last_clear_date.append(rec->QueryString(row_id, 1));
        next_clear_date.append(rec->QueryString(row_id, 2));
    }

    return true;
}

// 比较两个日期, 如果
// date1 晚于 date2 返回大于0的数, date1 早于 date2 返回小于0的数, date1 等于 date2 返回0
// date 的格式是: yyyy-dd-mm
int DomainRankList::CompareDate(const std::string &date1, const std::string &date2)
{
    if (date1.length() == 0)
    {
        return -1;
    }

    if (date2.length() == 0)
    {
        return 1;
    }

    CVarList date1_list;
    util_split_string(date1_list, date1, "-");
    
    if (date1_list.GetCount() != 3)
    {
        return -1;
    }

    CVarList date2_list;
    util_split_string(date2_list, date2, "-");

    if (date1_list.GetCount() != 3)
    {
        return 1;
    }

    int date1_year = atoi(date1_list.StringVal(0));
    int date2_year = atoi(date2_list.StringVal(0));

    if (date1_year != date2_year)
    {
        return date1_year - date2_year;
    }

    int date1_month = atoi(date1_list.StringVal(1));
    int date2_month = atoi(date2_list.StringVal(1));
    
    if (date1_month != date2_month)
    {
        return date1_month - date2_month;
    }

    int date1_day = atoi(date1_list.StringVal(2));
    int date2_day = atoi(date2_list.StringVal(2));

    if (date1_day != date2_day)
    {
        return date1_day - date2_day;
    }

    return 0;
}

// 获取今天的日期
std::string DomainRankList::GetDateOfToday()
{
    __time64_t current = _time64(NULL);

    tm * local = _localtime64(&current);

    char str_date[256] = {0};
    SPRINTF_S(str_date, "%d-%02d-%02d", local->tm_year+1900,
        local->tm_mon+1, local->tm_mday);

    std::string temp_ret = str_date;
    return temp_ret;
}

// 获取明天的日期
std::string DomainRankList::GetDateOfNextDay()
{
    __time64_t current = _time64(NULL);
    __time64_t tomorow = current + SECONDS_OF_ONE_DAY; // 加一天

    tm * local_tomorrow = _localtime64(&tomorow);

    char str_date[256] = {0};
    SPRINTF_S(str_date, "%d-%02d-%02d", local_tomorrow->tm_year+1900,
        local_tomorrow->tm_mon+1, local_tomorrow->tm_mday);

    std::string temp_ret = str_date;
    return temp_ret;
}

// 获取下周一的日期
std::string DomainRankList::GetDateOfNextMonday()
{
    __time64_t current = _time64(NULL);
    tm * local = _localtime64(&current);

    int week_day = local->tm_wday; // 0-6
    if (week_day == 0)
    {
        week_day = 7; //周日
    }
    int day_offset = 7 - week_day + 1;

    __time64_t next_monday = current + day_offset * SECONDS_OF_ONE_DAY;
    tm * local_next_monday = _localtime64(&next_monday);

    char str_date[256] = {0};
    SPRINTF_S(str_date, "%d-%02d-%02d", local_next_monday->tm_year+1900,
        local_next_monday->tm_mon+1, local_next_monday->tm_mday);

    std::string temp_ret = str_date;
    return temp_ret;
}

// 获取下个月一号的日期
std::string DomainRankList::GetDateOfNextMonth()
{
    __time64_t current = _time64(NULL);
    tm * local = _localtime64(&current);

    int year = local->tm_year + 1900;
    int month = local->tm_mon + 1; //tm_mon 的范围是 0-11
    int day_of_month = 1;

    if (month == 12)
    {
        // 本月是最后一个月, 下个月就是明年的 1.1
        year += 1;
        month = 1;
    }
    else
    {
        month += 1;
    }

    char str_date[256] = {0};
    SPRINTF_S(str_date, "%d-%02d-%02d", year, month, day_of_month);

    std::string temp_ret = str_date;
    return temp_ret;
}

// 解析右表达式, 右表达式是对数字进行的求职操作
// 右表达式的格式为： (+)n 或者 (-)n
bool DomainRankList::ParseRightExp(ColumnVar *the_column, int column_type, const char *exp)
{
    int exp_len = (int) strlen(exp);
    if (exp_len < 4)
    {
        return false;
    }

    // 获取操作符
    const char the_operator = *(exp+1);

    // 获取操作数
    const char *the_operand = exp + 3;

    bool ret = false;
    switch (column_type)
    {
    case VTYPE_INT:
        {
            int val = atoi(the_operand);
            switch(the_operator)
            {
            case '+':
                {
                    the_column->int_value += val;
                    ret = true;
                }
                break;
            case '-':
                {
                    the_column->int_value -= val;
                    ret = true;
                }
                break;
            default:
                break;
            }
        }
        break;
    case VTYPE_INT64:
        {
            __int64 val = _atoi64(the_operand);
            switch(the_operator)
            {
            case '+':
                {
                    the_column->int64_value += val;
                    ret = true;
                }
                break;
            case '-':
                {
                    the_column->int64_value -= val;
                    ret = true;
                }
                break;
            default:
                break;
            }
        }
        break;
    case VTYPE_FLOAT:
        {
            float val = (float)atof(the_operand);
            switch(the_operator)
            {
            case '+':
                {
                    the_column->float_value += val;
                    ret = true;
                }
                break;
            case '-':
                {
                    the_column->float_value -= val;
                    ret = true;
                }
                break;
            default:
                break;
            }
        }
        break;
    case VTYPE_DOUBLE:
        {
            double val = atof(the_operand);
            switch(the_operator)
            {
            case '+':
                {
                    the_column->double_value += val;
                    ret = true;
                }
                break;
            case '-':
                {
                    the_column->double_value -= val;
                    ret = true;
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return ret;
}

// 根据玩家等级取得排行榜等级区分表名
std::string DomainRankList::GetDifLvlRankListName(int player_lvl)
{
	int left = 0;
	int right = 0;
	if (player_lvl < 30)
	{
		left = (player_lvl/10)*10 == 0 ? 1 : (player_lvl/10)*10;
		right = ((player_lvl + 10)/10)*10;
	}
	else
	{
		if (player_lvl %10 < 5)
		{
			left = (player_lvl/10)*10;
			right = (player_lvl/10)*10 + 5;
		}
		else
		{
			left = (player_lvl/10)*10 + 5;
			right = ((player_lvl + 5)/10)*10;
		}
	}

	//拼接表名
	char rank_list_name_lvl[256] = {0};
	SPRINTF_S(rank_list_name_lvl, "%s_%d_%d", PLAYER_LEVEL_RANK_LIST,left,right);
	//rank_list_name = std::string(rank_list_name_lvl);
	return rank_list_name_lvl;
}

// 兼容玩家等级(根据GetDifLvlRankListName规则,向低等级兼容,获取兼容等级)
int DomainRankList::CompatiblePlayerLevel(int player_lvl)
{
	// 小于30级,每10级一个等级段
	if( player_lvl < 30 )
	{
		player_lvl -= 10;
	}
	// 大于30级,每5级一个等级段
	else
	{
		player_lvl -= 5;
	}

	return player_lvl;
};

// 重建排行榜, @old_data 现有排行榜数据的处理策略
bool DomainRankList::RecreateRankList(IPubData * pDomainData, const RecordDefine &rec_def,
    OldDataStrategy old_data)
{   
    // 新榜的列定义
    CVarList rank_list_columns;

    LoopBeginCheck(a);
    for (int i=0; i<rec_def.columns_count; i++)
    {
        LoopDoCheck(a);

        rank_list_columns << rec_def.columns_type[i];
    }
    
    if (old_data == DISCARD_OLD_DATA)
    {
        // 直接创建新排行榜, 丢弃老数据
        
        pDomainData->RemoveRecord(rec_def.rec_name.c_str());
        
        bool created = pDomainData->AddRecord(rec_def.rec_name.c_str(), rec_def.max_row,
            int(rank_list_columns.GetCount()), rank_list_columns);

        if (created)
        {
            // 设置索引列(主键)
            pDomainData->SetRecordKey(rec_def.rec_name.c_str(), rec_def.pk_column);
            pDomainData->SetRecordSave(rec_def.rec_name.c_str(), true);
        }
        
        return true;
    }
    
    //
    // 需要把现有排行榜的数据移动到新的排行榜
    //
    
    bool has_old_record = true;
    
    // 现有的表
    IRecord *old_record = pDomainData->GetRecord(rec_def.rec_name.c_str());
    if (old_record == NULL)
    {
        has_old_record = false;
        
        bool created = pDomainData->AddRecord(rec_def.rec_name.c_str(), rec_def.max_row,
            int(rank_list_columns.GetCount()), rank_list_columns);

        if (created)
        {
            // 设置索引列(主键)
            pDomainData->SetRecordKey(rec_def.rec_name.c_str(), rec_def.pk_column);
            pDomainData->SetRecordSave(rec_def.rec_name.c_str(), true);
        }
        
        return created;
    }
    
    int old_record_column_count = old_record->GetCols();
    
    CVarList new_fields;
    
    LoopBeginCheck(b);
    for (int i=old_record_column_count; i<rec_def.columns_count; ++i)
    {
        LoopDoCheck(b);

        switch(rec_def.columns_type[i])
        {
        case VTYPE_INT:
            new_fields << 0;
            break;
        case VTYPE_INT64:
            new_fields << (int64_t)0;
            break;
        case VTYPE_FLOAT:
            new_fields << 0.0f;
            break;
        case VTYPE_DOUBLE:
            new_fields << 0.0;
            break;
        case VTYPE_STRING:
            new_fields << "";
            break;
        case VTYPE_WIDESTR:
            new_fields << L"";
            break;
        default:
            return false;
        }   
    }
       
    // 创建临时表, 用来保存老数据
    std::string temp_rank_list_name = rec_def.rec_name;
    temp_rank_list_name.append("temp");

    bool created = pDomainData->AddRecord(temp_rank_list_name.c_str(), rec_def.max_row,
        int(rank_list_columns.GetCount()), rank_list_columns);
    if (!created)
    {
        return false;
    }
    
    IRecord *temp_record = pDomainData->GetRecord(temp_rank_list_name.c_str());
    if (temp_record == NULL)
    {
        return false;
    }
    
    CVarList temp_rows;
    int old_data_count = old_record->GetRows();
    
    LoopBeginCheck(c);
    for (int i=0; i<old_data_count; ++i)
    {
        LoopDoCheck(c);
        
        temp_rows.Clear();
        
        old_record->QueryRowValue(i, temp_rows);
        temp_rows << new_fields;
        
        temp_record->AddRowValue(-1, temp_rows);
    }
    
    // 删除旧表
    pDomainData->RemoveRecord(rec_def.rec_name.c_str());
    
    // 创建新表
    created = pDomainData->AddRecord(rec_def.rec_name.c_str(), rec_def.max_row,
        int(rank_list_columns.GetCount()), rank_list_columns);
    if (!created)
    {
        pDomainData->RemoveRecord(temp_rank_list_name.c_str());
        return false;
    }
    
    IRecord *new_record = pDomainData->GetRecord(rec_def.rec_name.c_str());
    if (new_record == NULL)
    {
        pDomainData->RemoveRecord(temp_rank_list_name.c_str());
        return false; 
    }
    
    // 从临时表中复制数据到新表
    int temp_data_count = temp_record->GetRows();
    
    LoopBeginCheck(d);
    for (int i=0; i<temp_data_count; ++i)
    {
        LoopDoCheck(d);
        
        temp_rows.Clear();
        
        temp_record->QueryRowValue(i, temp_rows);
        new_record->AddRowValue(-1, temp_rows);
        
    }
    
    // 删除临时表
    pDomainData->RemoveRecord(temp_rank_list_name.c_str());
    
    return true;
}
