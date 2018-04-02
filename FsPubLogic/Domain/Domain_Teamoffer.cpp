//----------------------------------------------------------
// 文件名:      Domain_Teamoffer.h
// 内  容:      组队缉盗系统
// 说  明:
// 创建日期:    2018年04月02日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#include "Domain_Teamoffer.h"
#include "../../utils/util_func.h"
#include "FsGame/Define/PubDefine.h"
#include "utils/record.h"
#include "FsGame/Define/CommandDefine.h"
#include "Domain_Team.h"
#include "FsGame/Campaign/TeamOfferDefine.h"

Domain_Teamoffer* Domain_Teamoffer::m_pDomain_Teamoffer = NULL;

const std::wstring & Domain_Teamoffer::GetDomainName(IPubKernel * pPubKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pPubKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Teamoffer_%d", server_id);
		m_domainName = wstr;
	}
	return m_domainName; 
}

int Domain_Teamoffer::OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
	return OnLoad(pPubKernel, pPubSpace);

}

int Domain_Teamoffer::OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
	m_pDomain_Teamoffer = this;
	const std::wstring & name = GetDomainName(pPubKernel);
	IPubData * pPubData = pPubSpace->GetPubData(name.c_str());

	if (NULL == pPubData)
	{
		return 1;
	}

	// 组队悬赏表
	CreateTeamOfferRec(pPubKernel, pPubData);

	return 1;
}


int Domain_Teamoffer::OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace, 
	int source_id, int scene_id, const IVarList& msg)
{
	IPubData * pPubData = pPubSpace->GetPubData(GetName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return 0;
	}

	int msgid = msg.IntVal(2);
	if (SP_DOMAIN_MSG_TEAMOFFER != msgid)
	{
		return 0;
	}

	// 操作类型
	const int opt_type = msg.IntVal(3);

	switch (opt_type)
	{
	case S2P_TEAMOFFER_SUB_MSG_LAUNCH:	// 发起一次组队悬赏
		OnLaunch(pPubKernel, pPubData, msg, source_id, scene_id);
		break;
	case S2P_TEAMOFFER_SUB_MSG_STOP:	// 终止一次组队悬赏
		OnStop(pPubKernel, pPubData, msg, source_id, scene_id);
		break;
	case S2P_TEAMOFFER_SUB_MSG_START:	// 组队悬赏开始通知
		OnStart(pPubKernel, pPubSpace, msg, source_id, scene_id);
		break;
	case S2P_TEAMOFFER_SUB_MSG_CHECK:	// 组队悬赏有效验证
		OnCheck(pPubKernel, pPubSpace, msg, source_id, scene_id);
		break;
	case S2P_TEAMOFFER_SUB_MSG_RESULT:	// 组队悬赏结果
		OnResult(pPubKernel, pPubSpace, msg, source_id, scene_id);
		break;
	case S2P_TEAMOFFER_SUB_MSG_RESET:	// 重置一次组队悬赏
		OnReset(pPubKernel, pPubSpace, msg, source_id, scene_id);
		break;
	default:
		break;
	}

	return 1;
}

// 组队悬赏建表
void Domain_Teamoffer::CreateTeamOfferRec(IPubKernel* pKernel, IPubData* pPubData)
{
	if (NULL == pPubData)
	{
		return;
	}

	if (pPubData->FindRecord(TEAMOFFER_REC))
	{
		pPubData->RemoveRecord(TEAMOFFER_REC);
	}

	// 组队悬赏记录表
	CVarList col_types;
	col_types << VTYPE_INT 
			  << VTYPE_INT 
			  << VTYPE_INT 
			  << VTYPE_INT
			  << VTYPE_OBJECT
			  << VTYPE_INT64;

	_create_record(pKernel, pPubData, TEAMOFFER_REC, 10000, col_types, true);
}

void Domain_Teamoffer::OnLaunch(IPubKernel* pPubKernel, IPubData* pPubData,
	const IVarList& msg, int source_id, int scene_id)
{
	if (NULL == pPubData)
	{
		return;
	}

	const wchar_t *capt_name = msg.WideStrVal(4);
	const int team_id = msg.IntVal(5);
	const int task_id = msg.IntVal(6);

	// 加入
	CVarList row_value;
	row_value.Append(msg, 5, msg.GetCount() - 5);
	row_value << util_get_time_64();

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		exist_row = pRec->AddRowValue(-1, row_value);		
	}

	if (exist_row < 0)
	{
		return;
	}
		
	// 发起
	if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) == TEAMOFFER_STATE_NONE)
	{
		pRec->SetRowValue(exist_row, row_value);

		// 通知队伍成员同步悬赏任务
		CVarList p2s_sub_msg;
		p2s_sub_msg << P2S_TEAMOFFER_SUB_MSG_SYNC_TASK
					<< capt_name
					<< team_id
					<< task_id;
		SendP2SMsg(pPubKernel, source_id, scene_id, p2s_sub_msg);
	}
}

// 取消发起组队悬赏
void Domain_Teamoffer::OnStop(IPubKernel* pPubKernel, IPubData* pPubData,
	const IVarList& msg, int source_id, int scene_id)
{
	if (NULL == pPubData)
	{
		return;
	}

	const int team_id = msg.IntVal(4);
	const int task_id = msg.IntVal(5);
	const int group_id = msg.IntVal(6);

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	// 验证任务
	if (task_id != 0 && pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_TASK_ID) != task_id)
	{
		return;
	}

	// 验证分组
	if (group_id != pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_GROUP_ID))
	{
		return;
	}

	pRec->RemoveRow(exist_row);
}

// 组队悬赏开始
void Domain_Teamoffer::OnStart(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
	const IVarList& msg, int source_id, int scene_id)
{
	IPubData * pPubData = pPubSpace->GetPubData(GetName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return;
	}

	const int team_id = msg.IntVal(4);
	const int task_id = msg.IntVal(5);
	const int group_id = msg.IntVal(6);
	const PERSISTID creator = msg.ObjectVal(7);

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);

	// 队伍成员
	CVarList mem_list;
	Domain_Team::GetTeamMemsByTeamID(pPubKernel, pPubSpace, team_id, mem_list);

	// 发起失败
	if (exist_row < 0)
	{	
		CVarList row_value;
		row_value << team_id
				  << task_id
				  << group_id
				  << TEAMOFFER_STATE_DOING
				  << creator
				  << util_get_time_64();
		pRec->AddRowValue(-1, row_value);
	}
	else
	{
		if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) != TEAMOFFER_STATE_NONE)
		{
			return;
		}

		// 设置进行中
		pRec->SetInt(exist_row, TEAMOFFER_REC_COL_GROUP_ID, group_id);
		pRec->SetInt(exist_row, TEAMOFFER_REC_COL_STATE, TEAMOFFER_STATE_DOING);
		pRec->SetInt64(exist_row, TEAMOFFER_REC_COL_STATE_TIME, util_get_time_64());
		pRec->SetObject(exist_row, TEAMOFFER_REC_COL_CREATOR, creator);
	}
	
	// 通知队伍进入分组副本
	CVarList p2s_sub_msg;
	p2s_sub_msg << P2S_TEAMOFFER_SUB_MSG_ENTRY_GROUP
				<< team_id;

	p2s_sub_msg.Concat(mem_list);

	SendP2SMsg(pPubKernel, source_id, scene_id, p2s_sub_msg);
}

// 组队悬赏有效验证
void Domain_Teamoffer::OnCheck(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
	const IVarList& msg, int source_id, int scene_id)
{
	IPubData * pPubData = pPubSpace->GetPubData(GetName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return;
	}

	const int team_id = msg.IntVal(4);

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	
	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	CVarList mem_list;
	Domain_Team::GetTeamMemsByTeamID(pPubKernel, pPubSpace, team_id, mem_list);
	if (mem_list.IsEmpty())
	{
		pRec->RemoveRow(exist_row);
	}	
}

// 组队悬赏结果
void Domain_Teamoffer::OnResult(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
	const IVarList& msg, int source_id, int scene_id)
{
	IPubData * pPubData = pPubSpace->GetPubData(GetName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return;
	}

	const int result = msg.IntVal(4);
	const int team_id = msg.IntVal(5);

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	if (pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_STATE) != TEAMOFFER_STATE_DOING)
	{
		return;
	}

	// 状态持续时间
	const int64_t now_time = util_get_time_64();
	const int64_t state_time = pRec->QueryInt64(exist_row, TEAMOFFER_REC_COL_STATE_TIME);
	const int continue_time = (int)((now_time - state_time) / 1000);

	pRec->SetInt(exist_row, TEAMOFFER_REC_COL_STATE, TEAMOFFER_STATE_END);
	pRec->SetInt64(exist_row, TEAMOFFER_REC_COL_STATE_TIME, now_time);

	const int task_id = pRec->QueryInt(exist_row, TEAMOFFER_REC_COL_TASK_ID);
	const PERSISTID creator = pRec->QueryObject(exist_row, TEAMOFFER_REC_COL_CREATOR);

	// 队伍成员
	CVarList mem_list;
	Domain_Team::GetTeamMemsByTeamID(pPubKernel, pPubSpace, team_id, mem_list);
	
	// 组队悬赏结束
	CVarList p2s_sub_msg;
	p2s_sub_msg << P2S_TEAMOFFER_SUB_MSG_RESULT
				<< result
				<< team_id
				<< creator
				<< continue_time;

	p2s_sub_msg.Concat(mem_list);

	SendP2SMsg(pPubKernel, source_id, scene_id, p2s_sub_msg);	
}

// 重置一次组队悬赏
void Domain_Teamoffer::OnReset(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
	const IVarList& msg, int source_id, int scene_id)
{
	IPubData * pPubData = pPubSpace->GetPubData(GetName(pPubKernel).c_str());
	if (NULL == pPubData)
	{
		return;
	}

	const int team_id = msg.IntVal(4);

	// 组队悬赏记录表
	IRecord *pRec = pPubData->GetRecord(TEAMOFFER_REC);
	if (NULL == pRec)
	{
		return;
	}

	const int exist_row = pRec->FindInt(TEAMOFFER_REC_COL_TEAM_ID, team_id);
	if (exist_row < 0)
	{
		return;
	}

	pRec->SetInt(exist_row, TEAMOFFER_REC_COL_STATE, TEAMOFFER_STATE_NONE);
	pRec->SetInt(exist_row, TEAMOFFER_REC_COL_GROUP_ID, -1);
	pRec->SetObject(exist_row, TEAMOFFER_REC_COL_CREATOR, PERSISTID());
	pRec->SetInt64(exist_row, TEAMOFFER_REC_COL_STATE_TIME, int64_t(0));
}

// 公共区发送到场景服
void Domain_Teamoffer::SendP2SMsg(IPubKernel* pPubKernel, int source_id,
	int scene_id, const IVarList& sub_p2s_msg)
{
	CVarList p2s_msg;
	p2s_msg << PUBSPACE_DOMAIN
			<< GetDomainName(pPubKernel)
			<< PS_DOMAIN_MSG_TEAMOFFER;
	p2s_msg.Concat(sub_p2s_msg);
	pPubKernel->SendPublicMessage(source_id, scene_id, p2s_msg);
}
