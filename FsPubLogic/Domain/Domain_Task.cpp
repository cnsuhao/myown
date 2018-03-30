//--------------------------------------------------------------------
// 文件名:		Domain_Task.cpp
// 内  容:		任务信息域
// 说  明:		
// 创建日期:	2016年07月01日
// 创建人:		  tongzt
// 修改人:        
//--------------------------------------------------------------------

#include "Domain_Task.h"
#include "../../FsGame/Define/PubDefine.h"

#include "../../utils/extend_func.h"
#include "../../utils/record.h"
#include "../../utils/util_func.h"
#include "../../utils/string_util.h"
#include <time.h>
#include "../../FsGame/TaskModule/TaskDefine.h"


// 初始化相关
Domain_Task * Domain_Task::m_pDomain_Task = NULL;

// 构造
Domain_Task::Domain_Task() : IDomainData(L"", 0)
{
	m_pDomain_Task = this;
}

// 取域名
const std::wstring & Domain_Task::GetDomainName(IPubKernel *pPubKernel)
{
	// 常规检查
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pPubKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Task_%d", server_id);

		m_domainName = wstr;
	}

	return m_domainName;
}

// 创建
int Domain_Task::OnCreate(IPubKernel *pPubKernel, IPubSpace *pPubSpace)
{
	return OnLoad(pPubKernel, pPubSpace);
}

// 加载
int Domain_Task::OnLoad(IPubKernel*pPubKernel, IPubSpace *pPubSpace)
{
	// 常规检查
	const std::wstring & domain_name = GetDomainName(pPubKernel);
	IPubData * pPubData = pPubSpace->GetPubData(domain_name.c_str());

	if (NULL == pPubData)
	{
		return 1;
	}

	//// 刺探Npc统计表
	//CVarList col_types;
	//col_types  << VTYPE_STRING
	//				 << VTYPE_INT;
	//CreateRecord(pPubKernel, pPubData, SpyNpcDamageRec, col_types,
	//	SPY_NPC_DAMAGE_REC_COL_MAX, SPY_NPC_DAMAGE_REC_MAX_ROWS, CVarList(), SPY_NPC_DAMAGE_REC_COL_ID);

	// 最新一次重置日期
	if (!pPubData->FindAttr("ResetTaskDate"))
	{
		pPubData->AddAttr("ResetTaskDate", VTYPE_DOUBLE);
		pPubData->SetAttrSave("ResetTaskDate", true);
		pPubData->SetAttrDouble("ResetTaskDate", util_get_date());
	}

	// 添加定时器
	pPubKernel->AddTimer("ResetTaskTimer", Domain_Task::ResetTaskTimer, 
		pPubSpace->GetSpaceName(), GetDomainName(pPubKernel).c_str(), 5 * 1000, 0);

	return 1;
}

// 消息
int Domain_Task::OnMessage(IPubKernel *pPubKernel, IPubSpace *pPubSpace,
								 int source_id,  int scene_id, const IVarList & msg)
{
	// 常规检查
	const std::wstring & name = GetDomainName(pPubKernel);
	IPubData * pPubData = pPubSpace->GetPubData(name.c_str());
	if (NULL == pPubData)
	{
		return 1;
	}

	// 参数检查
	if (msg.GetCount() < 4)
	{
		return 1;
	}

	//常规检查
	const int msg_type = msg.IntVal(2);
	const int sub_msg = msg.IntVal(3);
// 	if (msg_type != SP_DOMAIN_MSG_TASK)
// 	{
// 		return 1;
// 	}

	//CVarList args;
	//args.Append(msg, 4, msg.GetCount() - 4);

	//switch (sub_msg)
	//{
	//case S2P_SUBMSG_SPY_NPC_DAMAGE_INC:	// 刺探Npc伤害值递增
	//	IncSpyNpcDamage(pPubKernel, pPubSpace, args);
	//	break;
	//default:
	//	break;
	//}

	return 1;
}

// 建表
void Domain_Task::CreateRecord(IPubKernel *pPubKernel, IPubData *pPubData,
									 const char *rec_name, IVarList& cols_type, const int cols, 
									 const int max_row, IVarList& new_col_val, const int rec_key, bool is_save /*= true*/)
{
	if (NULL == pPubKernel 
		|| NULL == pPubData)
	{
		return;
	}

	if (StringUtil::CharIsNull(rec_name))
	{
		return;
	}

	// 取表
	IRecord* pRecord = pPubData->GetRecord(rec_name);
	if (pRecord == NULL)		
	{												
		pPubData->AddRecord(rec_name, max_row, cols, cols_type);
		pPubData->SetRecordSave(rec_name, is_save);
	}
	else if (check_record_changed(pRecord, cols_type, max_row))
	{
		// 重新构造表
		change_pub_record(pPubData, rec_name, cols_type, new_col_val, max_row, rec_key);
	}
}

// 任务定时器
int Domain_Task::ResetTaskTimer(IPubKernel *pPubKernel, const char *space_name, 
	const wchar_t *data_name, int time)
{
	IPubSpace *pPubSpace = pPubKernel->GetPubSpace(space_name);
	if (NULL == pPubSpace)
	{
		return 0;
	}

	// 数据区间
	IPubData * pPubData = pPubSpace->GetPubData(m_pDomain_Task->GetDomainName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return 0;
	}

	// 重置时间计算
	const double last_reset_date = pPubData->QueryAttrDouble("ResetTaskDate");
	const double cur_date = util_get_date();
	const double diff_day = util_get_time_diff(cur_date, last_reset_date);
	if (diff_day < 1 && last_reset_date != 0.0f)	// 未跨天
	{
		return 0;
	}

	// 当前时间
	time_t date = ::time(NULL);
	tm* cur_time = ::localtime(&date);
	if (NULL == cur_time)
	{
		return 0;
	}

	// 凌晨5点重置
	if (cur_time->tm_hour < DOMAIN_TASK_RESET_TIME)
	{
		return 0;
	}

	// 重设重置时间
	pPubData->SetAttrDouble("ResetTaskDate", cur_date);

	return 0;
}
