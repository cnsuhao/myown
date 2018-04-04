//--------------------------------------------------------------------
// 文件名:      Domain_Boss.h
// 内  容:      Boss记录表
// 说  明:		
// 创建日期:    2018年03月30日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "Domain_Boss.h"
#include "FsGame\Define\FightNpcDefine.h"
#include "utils\record.h"
#include "FsGame\Define\PubDefine.h"	
#include "utils\util_func.h"

Domain_Boss::Domain_Boss() : IDomainData(L"", 0)
{
	
}

const std::wstring & Domain_Boss::GetDomainName(IPubKernel * pPubKernel)
{
	//常规检查
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pPubKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Boss_%d", server_id);

		m_domainName = wstr;
	}

	return m_domainName;
}


bool Domain_Boss::GetSave()
{
	return true;
}

int Domain_Boss::OnCreate(IPubKernel * pPubKernel, IPubSpace * pPubSpace)
{
	//常规检查
	const std::wstring & name = GetDomainName(pPubKernel);
	IPubData * pPubData = pPubSpace->GetPubData(name.c_str());

	if (NULL == pPubData)
	{
		return 1;
	}

	//场景BOSS信息表
	CVarList col_types;
	col_types << VTYPE_STRING //boss编号
		<< VTYPE_INT64//BOSS死亡时间
		<< VTYPE_INT	// boss所在场景
		<< VTYPE_WIDESTR;// 杀死boss的玩家
	_create_record(pPubKernel, pPubData, SCENE_BOSS_TOTAL_REC, 0, col_types, false);

	return 1;
}

int Domain_Boss::OnLoad(IPubKernel * pPubKernel, IPubSpace * pPubSpace)
{
	return OnCreate(pPubKernel, pPubSpace);
}

///  \brief 接收到场景服务器的消息
///  \param source_id 发消息的服务器标识
///  \param msg 消息内容
int Domain_Boss::OnMessage(IPubKernel * pPubKernel, IPubSpace * pPubSpace, int source_id, int scene_id, const IVarList & msg)
{
	//常规检查
	const std::wstring & name = GetDomainName(pPubKernel);
	IPubData * pPubData = pPubSpace->GetPubData(name.c_str());
	if (NULL == pPubData)
	{
		return 1;
	}

	//常规检查
	const int msg_type = msg.IntVal(2);
	if (msg_type != SP_DOMAIN_MSG_SCENE_BOSS_STATE)
	{
		return 1;
	}

	const char* strBossConfig = msg.StringVal(3);
	const int nState = msg.IntVal(4);
	const int nSceneId = msg.IntVal(5);
	const wchar_t* wsKiller = msg.WideStrVal(6);

	IRecord* pRec = pPubData->GetRecord(SCENE_BOSS_TOTAL_REC);
	if (NULL == pRec)
	{
		return false;
	}
	int nRowIndex = pRec->FindString(SCENE_BOSS_INFO_CONFIG, strBossConfig);
	// 第一次刷新boss
	if (-1 == nRowIndex)
	{
		pRec->AddRowValue(-1, CVarList() << strBossConfig << (int64_t)nState << nSceneId << wsKiller);
	}
	else
	{
		if (BS_BOSS_DEAD == nState)
		{
			pRec->SetInt64(nRowIndex, SCENE_BOSS_DEAD_TIME, ::util_get_utc_time());
			pRec->SetWideStr(nRowIndex, SCENE_BOSS_INFO_KILLER, wsKiller);
		}
		else
		{
			pRec->SetInt64(nRowIndex, SCENE_BOSS_DEAD_TIME, (int64_t)nState);
			pRec->SetWideStr(nRowIndex, SCENE_BOSS_INFO_KILLER, L"");
		}
	}
	return true;
}