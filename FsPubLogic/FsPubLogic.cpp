//--------------------------------------------------------------------
// 文件名:		FsPubLogic.cpp
// 内  容:		公共数据服务器逻辑模块
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "FsPubLogic.h"
#include "Domain/DomainManager.h"
//#include "Guild/GuildManager.h"
#include "public/VarList.h"
#include "public/Inlines.h"
#include "../../utils/util_func.h"
#include "../../FsGame/Define/PubDefine.h"
#include "FsBindLogic.h"
#include <windows.h>

extern IPubKernel* g_pPubKernel;

int64_t g_nMaxCirculateCount = 50010;
pub_callee * pub_callee::pInstance;
void SetMaxCirculateCount(int count)
{
	if (g_nMaxCirculateCount != count)
	{
		g_nMaxCirculateCount = count;
	}
}

// 定义公共数据逻辑DLL
DECLARE_PUBLIC_LOGIC_DLL(pub_callee);

std::vector<IPubLogic*> g_vecLogic;

// 公共数据回调接口

//构造
pub_callee::pub_callee()
{
	IPubLogic* pLogic = DomainManager::Instance();
	if (pLogic)
	{
		g_vecLogic.push_back(pLogic);
	}
	
// 	pLogic = PubGuildManager::Instance();
// 	if (pLogic)
// 	{
// 		g_vecLogic.push_back(pLogic);
// 	}

	m_bServerReady = false;
	m_nMaxPlayers = 0;
	m_nPlayerCount = 0;
	m_nOnlineCount = 0;
	m_nOfflineCount = 0;
	m_nQueueCount = 0;
	m_fGetOfflineTimer = 0.0f;
	pInstance = this;
}

//析构
pub_callee::~pub_callee()
{
	LoopBeginCheck(a)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(a)
		delete g_vecLogic[i];
	}

	g_vecLogic.clear();
}

// 公共数据服务器启动
int pub_callee::OnPublicStart(IPubKernel* pPubKernel)
{ 
	// 初始化核心指针
	g_pPubKernel = pPubKernel;

	FsBindLogic::Instance()->Init(pPubKernel);

	LoopBeginCheck(b)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(b)
		g_vecLogic[i]->OnPublicStart(pPubKernel);
	}
	return 0; 
}

/// \brief 服务器关闭通知
int pub_callee::OnServerClose(IPubKernel* pPubKernel)
{
	FsBindLogic::Instance()->Close(pPubKernel);
	LoopBeginCheck(c)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(c)
		g_vecLogic[i]->OnServerClose(pPubKernel);
	}

	// 清除核心指针
	g_pPubKernel = NULL;
 
	return 0;
}

bool pub_callee::SendPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& msg)
{
	LoopBeginCheck(c)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(c)
		g_vecLogic[i]->OnPublicCommand(pPubKernel, cmd_id, msg);
	}
	return true;
}

// 公共数据项加载完成
int pub_callee::OnPubDataCreate(IPubKernel* pPubKernel, 
								const char* space_name, const wchar_t* data_name)
{
	return 0;
}

// 公共数据项加载完成
int pub_callee::OnPubDataLoaded(IPubKernel* pPubKernel, 
								const char* space_name, const wchar_t* data_name) 
{
	LoopBeginCheck(d)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(d)
		if (0 == strcmp(space_name, g_vecLogic[i]->GetName()))
		{
			g_vecLogic[i]->OnPubDataLoaded(pPubKernel, data_name);

			break;
		}
	}
	return 0; 
}

// 公共数据空间中的所有项加载完成
int pub_callee::OnPubSpaceLoaded(IPubKernel* pPubKernel, 
								 const char* space_name) 
{ 
	LoopBeginCheck(e)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(e)
		if (0 == strcmp(space_name, g_vecLogic[i]->GetName()))
		{
			g_vecLogic[i]->OnPubSpaceLoaded(pPubKernel);

			break;
		}
	}

	return 0; 
}

// 接收到场景服务器的消息
int pub_callee::OnPublicMessage(IPubKernel* pPubKernel, int source_id, 
								int scene_id, const IVarList& msg) 
{
	const char * space_name = msg.StringVal(0);
	int msgid = msg.IntVal(2);

	if (msgid >= PUBDATA_MSG_ID_MIN && msgid <= PUBDATA_MSG_ID_MAX)
	{
		//通用消息
		return ProcessCommonMessage(pPubKernel, source_id, scene_id, msg);
	}
	if (msgid == SP_SERVICE_READY_PUBLIC)
	{
		FsBindLogic::Instance()->OnPublicRegist(pPubKernel, source_id, scene_id, msg);
		return 0;
	}

	LoopBeginCheck(f)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(f)
		if (0 == strcmp(space_name, g_vecLogic[i]->GetName()))
		{
			g_vecLogic[i]->OnPublicMessage(pPubKernel, source_id, scene_id, msg);

			break;
		}
	}
	return 0; 
}

// 服务器每帧调用
int pub_callee::OnFrameExecute(IPubKernel* pPubKernel, int frame_ticks) 
{ 
	// 请求获得主服务器信息
	pPubKernel->RequestServerState();

	m_fGetOfflineTimer += frame_ticks * 0.001f;

	//// 每隔一段时间拉人上线
	//if (m_fGetOfflineTimer >= 120.0f)
	//{
	//	m_fGetOfflineTimer = 0.0f;

	//	if (m_bServerReady)
	//	{
	//		// 获得当前时间
	//		double now = ::util_get_now();

	//		// 时间
	//		int year, month, day, hour, minute, second;

	//		util_decode_date(now, year, month, day);
	//		util_decode_time(now, hour, minute, second);

	//		// 加载
	//		pPubKernel->RequestRoleOffline(30, WORD(year), WORD(month), WORD(day),
	//						WORD(hour), WORD(minute), WORD(second), true);
	//	}
	//}

	return 0; 
}

// 接收到主服务器状态和玩家数量信息
int pub_callee::OnServerState(IPubKernel* pPubKernel,
							  const char* world_state, int max_players, int player_count, 
							  int online_count, int offline_count, int queue_count) 
{ 
	if (0 == stricmp(world_state, "opened"))
	{
		if (!m_bServerReady)
		{
			FsBindLogic::Instance()->Start(pPubKernel);
		}
		m_bServerReady = true;
		m_nMaxPlayers = max_players;
		m_nPlayerCount = player_count;
		m_nOnlineCount = online_count;
		m_nOfflineCount = offline_count;
		m_nQueueCount = queue_count;

		
	}

	return 1; 
}

// 接收到可以加载到游戏的离线角色信息
int pub_callee::OnRoleOffline(IPubKernel* pPubKernel, int count,
							  const IVarList& info) 
{ 
	if (0 == count)
	{
		return 0;
	}

	//int index = 0;

	LoopBeginCheck(g)
	for (int i = 0; i < count; ++i)
	{
		LoopDoCheck(g)
		//// 角色帐号
		//const char* account = info.StringVal(index++);
		//// 角色名
		//const wchar_t* role_name = info.WideStrVal(index++);
		// 重新进入游戏的时间
		//        double resume_time = info.DoubleVal(index++);
		//++index;
		//// 重新进入游戏的相关信息
		//const char* resume_info = info.StringVal(index++);

		//CVarList res;
		//::util_split_string(res, resume_info, ",");

		//if (res.GetCount() != 2)
		//{
		//	continue;
		//}

		//// 在游戏中存在的时间秒数
		//int live_seconds = atoi(res.StringVal(0));

		//if (live_seconds <= 0)
		//{
		//	continue;
		//}

		//#ifdef	NDEBUG   
		// 请求主服务器加载玩家角色到游戏中
		//if (m_nOfflineCount < 300)
		//{
		// 角色帐号
		const char* account = info.StringVal(i);
		// 角色名
		const wchar_t* role_name = info.WideStrVal(i);
			pPubKernel->RequestLoadPlayer(account, role_name, 3000);
		/*}*/
		//#endif
	}

	return 1; 
}

// 处理通用消息
bool pub_callee::ProcessCommonMessage(IPubKernel* pPubKernel, int source_id, 
									  int scene_id, const IVarList& msg)
{
	const char * pubspace = msg.StringVal(0);
	const wchar_t * pubdata = msg.WideStrVal(1);
	int msgid = msg.IntVal(2);

	if (msgid < PUBDATA_MSG_ID_MIN 
		|| msgid > PUBDATA_MSG_ID_MAX)
	{
		//不是通用消息
		return false;
	}

	IPubSpace * pPubSpace = pPubKernel->GetPubSpace(pubspace);
	if (NULL == pPubSpace)
	{
		return false;
	}

	IPubData * pPubData = pPubSpace->GetPubData(pubdata);
	if (NULL == pPubData)
	{
		return false;
	}

	switch (msgid)
	{
	case PUBDATA_MSG_SET_PROP:
		{
			return SetPubDataProp(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_INC_PROP:
		{
			return IncPubDataProp(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_ADD_REC:
		{
			return AddPubDataRec(pPubKernel, pPubData, source_id, scene_id, msg);
		}

		break;
	case PUBDATA_MSG_CLEAR_REC:
		{
			return ClearPubDataRec(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_REMOVE_REC_ROW:
		{
			return RemovePubDataRecRow(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_ADD_REC_ROW_VALUE:
		{
			return AddPubDataRecRowValue(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_SET_REC_VALUE:
		{
			return SetPubDataRecValue(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_INC_REC_VALUE:
		{
			return IncPubDataRecValue(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_REMOVE_REC_ROW_BYKEY:
		{
			return RemovePubDataRecRowByKey(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_SET_REC_VALUE_BYKEY:
		{
			return SetPubDataRecValueByKey(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_INC_REC_VALUE_BYKEY:
		{
			return IncPubDataRecValueByKey(pPubKernel, pPubData, source_id, scene_id, msg);
		}
		
		break;
	case PUBDATA_MSG_UPDATE_CONFIG:
		{
			return UpdateConfig(pPubKernel, pPubData, source_id, scene_id, msg);
		}

		break;
	case PUBDATA_MSG_MAX_CIRCULATE_COUNT:
		{
			int maxcount = msg.IntVal(3);
			SetMaxCirculateCount(maxcount);
			return true;
		}

		break;
	default:
		break;
	}

	return false;
}

//修改属性，格式：string pubspace, wstring pubdata, int msgid, string prop, var value, ...;
bool pub_callee::SetPubDataProp(IPubKernel* pPubKernel, 
								IPubData* pPubData, 
								int source_id, 
								int scene_id,
								const IVarList& msg)
{
	const char * prop = msg.StringVal(3);

	switch (msg.GetType(4))
	{
	case VTYPE_INT:
		{
			return pPubData->SetAttrInt(prop, msg.IntVal(4));
		}
		
		break;
	case VTYPE_INT64:
		{
			return pPubData->SetAttrInt64(prop, msg.Int64Val(4));
		}
		
		break;
	case VTYPE_FLOAT:
		{
			return pPubData->SetAttrFloat(prop, msg.FloatVal(4));
		}
		
		break;
	case VTYPE_DOUBLE:
		{
			return pPubData->SetAttrDouble(prop, msg.DoubleVal(4));
		}
		
		break;
	case VTYPE_STRING:
		{
			return pPubData->SetAttrString(prop, msg.StringVal(4));
		}
		
		break;
	case VTYPE_WIDESTR:
		{
			return pPubData->SetAttrWideStr(prop, msg.WideStrVal(4));
		}
		
		break;
	case VTYPE_OBJECT:
		{
			return pPubData->SetAttrObject(prop, msg.ObjectVal(4));
		}
		
		break;
	default:
		return false;
		break;
	}
}

//增加属性，格式：string pubspace, wstring pubdata, int msgid, string prop, int value, ...;
bool pub_callee::IncPubDataProp(IPubKernel* pPubKernel,
								IPubData* pPubData, 
								int source_id, 
								int scene_id, 
								const IVarList& msg)
{
	const char * prop = msg.StringVal(3);
	switch (msg.GetType(4))
	{
	case VTYPE_INT:
		return pPubData->SetAttrInt(prop, 
			pPubData->QueryAttrInt(prop) + msg.IntVal(4));
	case VTYPE_INT64:
		return pPubData->SetAttrInt64(prop, 
			pPubData->QueryAttrInt64(prop) + msg.Int64Val(4));
	case VTYPE_FLOAT:
		return pPubData->SetAttrFloat(prop, 
			pPubData->QueryAttrFloat(prop) + msg.FloatVal(4));
	case VTYPE_DOUBLE:
		return pPubData->SetAttrDouble(prop, 
			pPubData->QueryAttrDouble(prop) + msg.DoubleVal(4));
	default:
		return false;
	}
}

//增加某个表，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int maxrow, int cols, ...;
bool pub_callee::AddPubDataRec(IPubKernel* pPubKernel, 
							   IPubData* pPubData, 
							   int source_id, 
							   int scene_id, 
							   const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	const int max_row = msg.IntVal(4);
	const int cols = msg.IntVal(5);
	CVarList col_types;
	col_types.Append(msg, 6, msg.GetCount() - 6);
	if (pPubData->FindRecord(rec))
	{
		pPubData->RemoveRecord(rec);
	}

	return pPubData->AddRecord(rec, max_row, cols, col_types);
}

//清除某个表的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
bool pub_callee::ClearPubDataRec(IPubKernel* pPubKernel,
								 IPubData* pPubData, 
								 int source_id,
								 int scene_id, 
								 const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	return pPubData->ClearRecordRow(rec);
}

//删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, ...;
bool pub_callee::RemovePubDataRecRow(IPubKernel* pPubKernel, 
									 IPubData* pPubData, 
									 int source_id,
									 int scene_id, 
									 const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int row = msg.IntVal(4);

	const int iCurRows = pPubData->GetRecordRows(rec);
	if (row >= iCurRows)
	{
		return false;
	}

	return pPubData->RemoveRecordRow(rec, row);
}

//添加某个表的一行内容，命令格式：string pubspace, wstring pubdata, int msgid, string rec, ...;
bool pub_callee::AddPubDataRecRowValue(IPubKernel* pPubKernel, 
									   IPubData* pPubData, 
									   int source_id, 
									   int scene_id,
									   const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	CVarList values;
	values.Append(msg, 4, msg.GetCount() - 4);

	return pPubData->AddRecordRowValue(rec, -1, values) >= 0;
}

//设置某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
bool pub_callee::SetPubDataRecValue(IPubKernel* pPubKernel, 
									IPubData* pPubData, 
									int source_id, 
									int scene_id, 
									const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int row = msg.IntVal(4);
	int col = msg.IntVal(5);

	const int iCurRows = pPubData->GetRecordRows(rec);
	if (row >= iCurRows)
	{
		return false;
	}

	switch (msg.GetType(6))
	{
	case VTYPE_INT:
		return pPubData->SetRecordInt(rec, row, col, msg.IntVal(6));
		break;
	case VTYPE_INT64:
		return pPubData->SetRecordInt64(rec, row, col, msg.Int64Val(6));
		break;
	case VTYPE_FLOAT:
		return pPubData->SetRecordFloat(rec, row, col, msg.FloatVal(6));
		break;
	case VTYPE_DOUBLE:
		return pPubData->SetRecordDouble(rec, row, col, msg.DoubleVal(6));
		break;
	case VTYPE_STRING:
		return pPubData->SetRecordString(rec, row, col, msg.StringVal(6));
		break;
	case VTYPE_WIDESTR:
		return pPubData->SetRecordWideStr(rec, row, col, msg.WideStrVal(6));
		break;
	case VTYPE_OBJECT:
		return pPubData->SetRecordObject(rec, row, col, msg.ObjectVal(6));
		break;
	default:
		return false;
		break;
	}
}

//增加某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int row, int col, var value, ...;
bool pub_callee::IncPubDataRecValue(IPubKernel* pPubKernel, 
									IPubData* pPubData,
									int source_id, 
									int scene_id, 
									const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int row = msg.IntVal(4);
	int col = msg.IntVal(5);

	const int iCurRows = pPubData->GetRecordRows(rec);
	if (row >= iCurRows)
	{
		return false;
	}

	switch (msg.GetType(6))
	{
	case VTYPE_INT:
		return pPubData->SetRecordInt(rec, row, col, 
			pPubData->QueryRecordInt(rec, row, col) + msg.IntVal(6));
	case VTYPE_INT64:
		return pPubData->SetRecordInt64(rec, row, col, 
			pPubData->QueryRecordInt64(rec, row, col) + msg.Int64Val(6));
	case VTYPE_FLOAT:
		return pPubData->SetRecordFloat(rec, row, col, 
			pPubData->QueryRecordFloat(rec, row, col) + msg.FloatVal(6));
	case VTYPE_DOUBLE:
		return pPubData->SetRecordDouble(rec, row, col, 
			pPubData->QueryRecordDouble(rec, row, col) + msg.DoubleVal(6));
	default:
		return false;
	}
}

//根据关键值删除某个表的某行数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue;
bool pub_callee::RemovePubDataRecRowByKey(IPubKernel* pPubKernel, 
										  IPubData* pPubData, 
										  int source_id,
										  int scene_id, 
										  const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int keycol = msg.IntVal(4);

	int row = -1;
	switch (msg.GetType(5))
	{
	case VTYPE_INT:
		row = pPubData->FindRecordInt(rec, keycol, msg.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pPubData->FindRecordInt64(rec, keycol, msg.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pPubData->FindRecordFloat(rec, keycol, msg.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pPubData->FindRecordDouble(rec, keycol, msg.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pPubData->FindRecordString(rec, keycol, msg.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pPubData->FindRecordWideStr(rec, keycol, msg.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pPubData->FindRecordObject(rec, keycol, msg.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	return pPubData->RemoveRecordRow(rec, row);
}

//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
bool pub_callee::SetPubDataRecValueByKey(IPubKernel* pPubKernel, 
										 IPubData* pPubData, 
										 int source_id, 
										 int scene_id, 
										 const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int keycol = msg.IntVal(4);

	int row = -1;
	switch (msg.GetType(5))
	{
	case VTYPE_INT:
		row = pPubData->FindRecordInt(rec, keycol, msg.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pPubData->FindRecordInt64(rec, keycol, msg.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pPubData->FindRecordFloat(rec, keycol, msg.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pPubData->FindRecordDouble(rec, keycol, msg.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pPubData->FindRecordString(rec, keycol, msg.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pPubData->FindRecordWideStr(rec, keycol, msg.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pPubData->FindRecordObject(rec, keycol, msg.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	int col = msg.IntVal(6);
	switch (msg.GetType(7))
	{
	case VTYPE_INT:
		return pPubData->SetRecordInt(rec, row, col, msg.IntVal(7));
		break;
	case VTYPE_INT64:
		return pPubData->SetRecordInt64(rec, row, col, msg.Int64Val(7));
		break;
	case VTYPE_FLOAT:
		return pPubData->SetRecordFloat(rec, row, col, msg.FloatVal(7));
		break;
	case VTYPE_DOUBLE:
		return pPubData->SetRecordDouble(rec, row, col, msg.DoubleVal(7));
		break;
	case VTYPE_STRING:
		return pPubData->SetRecordString(rec, row, col, msg.StringVal(7));
		break;
	case VTYPE_WIDESTR:
		return pPubData->SetRecordWideStr(rec, row, col, msg.WideStrVal(7));
		break;
	case VTYPE_OBJECT:
		return pPubData->SetRecordObject(rec, row, col, msg.ObjectVal(7));
		break;
	default:
		return false;
		break;
	}
}

//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string pubspace, wstring pubdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
bool pub_callee::IncPubDataRecValueByKey(IPubKernel* pPubKernel, 
										 IPubData* pPubData,
										 int source_id,
										 int scene_id, 
										 const IVarList& msg)
{
	const char * rec = msg.StringVal(3);
	int keycol = msg.IntVal(4);

	int row = -1;
	switch (msg.GetType(5))
	{
	case VTYPE_INT:
		row = pPubData->FindRecordInt(rec, keycol, msg.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pPubData->FindRecordInt64(rec, keycol, msg.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pPubData->FindRecordFloat(rec, keycol, msg.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pPubData->FindRecordDouble(rec, keycol, msg.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pPubData->FindRecordString(rec, keycol, msg.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pPubData->FindRecordWideStr(rec, keycol, msg.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pPubData->FindRecordObject(rec, keycol, msg.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	int col = msg.IntVal(6);

	switch (msg.GetType(7))
	{
	case VTYPE_INT:
		return pPubData->SetRecordInt(rec, row, col, 
			pPubData->QueryRecordInt(rec, row, col) + msg.IntVal(7));
	case VTYPE_INT64:
		return pPubData->SetRecordInt64(rec, row, col, 
			pPubData->QueryRecordInt64(rec, row, col) + msg.Int64Val(7));
	case VTYPE_FLOAT:
		return pPubData->SetRecordFloat(rec, row, col, 
			pPubData->QueryRecordFloat(rec, row, col) + msg.FloatVal(7));
	case VTYPE_DOUBLE:
		return pPubData->SetRecordDouble(rec, row, col, 
			pPubData->QueryRecordDouble(rec, row, col) + msg.DoubleVal(7));
	default:
		return false;
	}
}

//更新配置，命令格式：string pubspace, wstring pubdata, int msgid
bool pub_callee::UpdateConfig(IPubKernel* pPubKernel, 			 
							  IPubData* pPubData,		 
							  int source_id,	 
							  int scene_id, 		 
							  const IVarList& msg)
{
	const char * space_name = msg.StringVal(0);

	LoopBeginCheck(h)
	for (size_t i = 0; i < g_vecLogic.size(); i++)
	{
		LoopDoCheck(h)
		if (0 == strcmp(space_name, g_vecLogic[i]->GetName()))
		{
			return g_vecLogic[i]->OnUpateConfig(pPubKernel, source_id, scene_id, msg);
		}
	}

	return false;
}
