//--------------------------------------------------------------------
// 文件名:		SnsManager.cpp
// 内  容:		数据服务器逻辑模块:数据的管理
// 说  明:		sns通用管理类
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#include "SnsManager.h"
#include "../FsGame/Define/SnsDefine.h"
#include "../../server/ISnsData.h"
#include "SnsDataSync/SnsDataSync.h"

SnsManager* SnsManager::m_pInstance = NULL;

SnsManager::SnsManager()
{
	is_sns_ready_ = false;
	LoopBeginCheck(a)
	for (HashMapTable::iterator it = m_mapSnsLogicList.begin(); it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(a)
		delete it->second;
		it->second = NULL;
	}

	m_mapSnsLogicList.clear();
}

SnsManager::~SnsManager()
{
	LoopBeginCheck(b)
	for (HashMapTable::iterator it = m_mapSnsLogicList.begin(); it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(b)
		delete it->second;
		it->second = NULL;
	}

	m_mapSnsLogicList.clear();
}

ISnsCallee* SnsManager::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = NEW SnsManager;
	}

	return m_pInstance;
}

const char * SnsManager::GetSnsName()
{
	return SPACE_SNS;
}

// \brief Sns服务器已启动，此时逻辑模块已加载完成
// \param pKernel 核心指针，下同
int SnsManager::OnServerStarted(ISnsKernel* pSnsKernel)
{
	ISnsLogic* pSnsLogic = NULL;

// 	pSnsLogic = NEW OffLineDelay();
// 	m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;

    pSnsLogic = NEW SnsDataSync();
    m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;

// 	pSnsLogic = NEW SnsFriend();
// 	m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;

// 	pSnsLogic = NEW SnsArena();
// 	m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;
// 
// 	pSnsLogic = NEW SnsAuction();
// 	m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;
// 	pSnsLogic = NEW SnsBlessing();
// 	m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;
	
//     pSnsLogic = NEW SnsPartner();
//     m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;

//     pSnsLogic = NEW SnsGuildPresent();
//     m_mapSnsLogicList[pSnsLogic->GetSnsName(pSnsKernel)] = pSnsLogic;

	return 1;
}

// \brief Sns逻辑对象被创建
// \param pKernel 核心指针，下同
// \param args 无意义
int SnsManager::OnSnsModuleCreate(ISnsKernel* pSnsKernel, const IVarList& args)
{
	LoopBeginCheck(c)
	for (HashMapTable::iterator it = m_mapSnsLogicList.begin(); it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(c)
		it->second->OnModuleCreate(pSnsKernel, args);
	}

	return 1;
}

// \brief Sns服务器所以的Sns数据已经加载完成准备就绪(在若次OnSnsLoad之后)
// \param pKernel 核心指针，下同
// \param args 无意义
int SnsManager::OnServerReady(ISnsKernel* pSnsKernel, const IVarList& args)
{
	LoopBeginCheck(d)
	for (HashMapTable::iterator it = m_mapSnsLogicList.begin(); it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(d)
		it->second->OnReady(pSnsKernel, args);
	}
	is_sns_ready_ = true;
	return 1;
}

// \brief sns数据在内存中创建
// \param args
// \uid  sns uid //sns数据关键字
// \args  
int SnsManager::OnSnsDataCreate(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args)
{
	HashMapTable::iterator it = m_mapSnsLogicList.begin();
	LoopBeginCheck(e)
	for (; it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(e)
		it->second->OnCreate(pSnsKernel, uid, args);
	}

	return 1;
}

// \brief 加载sns数据
// \uid  sns uid //sns数据关键字
// \args  
int SnsManager::OnSnsDataLoad(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args)
{
	HashMapTable::iterator it = m_mapSnsLogicList.begin();
	LoopBeginCheck(f)
	for (; it != m_mapSnsLogicList.end(); ++it)
	{
		LoopDoCheck(f)
		it->second->OnLoad(pSnsKernel, uid, args);
	}

	return 1;
}

// \brief 收到消息
// \param args
// \distribute_id 分区id
// \server_id  服务器编号
// \memeber_id  场景服务器编号
// \scene_id  发送场景的id
// \uid  sns uid //sns数据关键字
// \args  member发送的消息体
int SnsManager::OnMemberMessage(ISnsKernel* pSnsKernel, int distribute_id,int server_id,
									int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * pSnsName = args.StringVal(0);
	const wchar_t* pModuleName = args.WideStrVal(1);
	int msgid = args.IntVal(2);

	if (std::strcmp(pSnsName, SPACE_SNS) != 0)
	{
		return 0;
	}

	if (msgid >= SNSDATA_MSG_ID_MIN && msgid <= SNSDATA_MSG_ID_MAX)
	{
		if (ProcessCommonMessage(pSnsKernel, distribute_id, server_id, memeber_id, scene_id, uid, args))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	ISnsLogic * pModuleNameIt = FindSnsLogic(pModuleName);
	if (pModuleNameIt != NULL)
	{
		pModuleNameIt->OnMessage(pSnsKernel, distribute_id, server_id, memeber_id, scene_id, uid, args);
	}
	else
	{
		LoopBeginCheck(g)
		for (HashMapTable::iterator it = m_mapSnsLogicList.begin(); it != m_mapSnsLogicList.end(); ++it)
		{
			LoopDoCheck(g)
			it->second->OnMessage(pSnsKernel, distribute_id, server_id, memeber_id, scene_id, uid, args);
		}
	}

	return 1;
}

/// \brief sns服务器关闭通知
int SnsManager::OnServerClose(ISnsKernel* pSnsKernel)
{
	is_sns_ready_ = false;
	pSnsKernel->SaveSnsAll();

	return 1;
}

//查找模块
ISnsLogic * SnsManager::FindSnsLogic(const wchar_t * module_name)
{
	HashMapTable::iterator it = m_mapSnsLogicList.find(module_name);
	if (it != m_mapSnsLogicList.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

// 处理通用消息
bool SnsManager::ProcessCommonMessage(ISnsKernel* pSnsKernel, int distribute_id,int server_id,
									  int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	int msgid = args.IntVal(2);

	if (msgid < SNSDATA_MSG_ID_MIN || msgid > SNSDATA_MSG_ID_MAX)
	{
		//不是通用消息
		return false;
	}

	if ( pSnsKernel == NULL )
	{
		return false;
	}


	ISnsData * pSnsData = pSnsKernel->GetSnsData(uid);
	if ( pSnsData == NULL )
	{
		return false;
	}

	switch (msgid)
	{
	case SNSDATA_MSG_SET_PROP:
		return SetPubDataProp(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_INC_PROP:
		return IncPubDataProp(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_CLEAR_REC:
		return ClearPubDataRec(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_REMOVE_REC_ROW:
		return RemovePubDataRecRow(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_ADD_REC_ROW_VALUE:
		return AddPubDataRecRowValue(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_SET_REC_VALUE:
		return SetPubDataRecValue(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_INC_REC_VALUE:
		return IncPubDataRecValue(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_REMOVE_REC_ROW_BYKEY:
		return RemovePubDataRecRowByKey(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_SET_REC_VALUE_BYKEY:
		return SetPubDataRecValueByKey(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_INC_REC_VALUE_BYKEY:
		return IncPubDataRecValueByKey(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_UPDATE_CONFIG:
		return UpdateConfig(pSnsKernel, pSnsData, distribute_id, server_id, memeber_id, scene_id, uid, args);
	case SNSDATA_MSG_SET_MAX_CIRCULATE_COUNT:
		{
			int maxcount = args.IntVal(3);
			SetMaxCirculateCount(maxcount);
			return true;
		}
	default:
		break;
	}

	return false;
}

//修改属性，格式：string snsspace, wstring snsdata, int msgid, string prop, var value, ...;
bool SnsManager::SetPubDataProp(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
								int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * prop = args.StringVal(3);

	switch (args.GetType(4))
	{
	case VTYPE_INT:
		return pSnsData->SetAttrInt(prop, args.IntVal(4));
		break;
	case VTYPE_INT64:
		return pSnsData->SetAttrInt64(prop, args.Int64Val(4));
		break;
	case VTYPE_FLOAT:
		return pSnsData->SetAttrFloat(prop, args.FloatVal(4));
		break;
	case VTYPE_DOUBLE:
		return pSnsData->SetAttrDouble(prop, args.DoubleVal(4));
		break;
	case VTYPE_STRING:
		return pSnsData->SetAttrString(prop, args.StringVal(4));
		break;
	case VTYPE_WIDESTR:
		return pSnsData->SetAttrWideStr(prop, args.WideStrVal(4));
		break;
	case VTYPE_OBJECT:
		return pSnsData->SetAttrObject(prop, args.ObjectVal(4));
		break;
	default:
		return false;
		break;
	}
}

//增加属性，格式：string snsspace, wstring snsdata, int msgid, string prop, int value, ...;
bool SnsManager::IncPubDataProp(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
								int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * prop = args.StringVal(3);
	switch (args.GetType(4))
	{
	case VTYPE_INT:
		return pSnsData->SetAttrInt(prop, 
			pSnsData->QueryAttrInt(prop) + args.IntVal(4));
	case VTYPE_INT64:
		return pSnsData->SetAttrInt64(prop, 
			pSnsData->QueryAttrInt64(prop) + args.Int64Val(4));
	case VTYPE_FLOAT:
		return pSnsData->SetAttrFloat(prop, 
			pSnsData->QueryAttrFloat(prop) + args.FloatVal(4));
	case VTYPE_DOUBLE:
		return pSnsData->SetAttrDouble(prop, 
			pSnsData->QueryAttrDouble(prop) + args.DoubleVal(4));
	default:
		return false;
	}
}

//清除某个表的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, ...;
bool SnsManager::ClearPubDataRec(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
								 int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	return pSnsData->ClearRecordRow(rec);
}

//删除某个表的某行数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, ...;
bool SnsManager::RemovePubDataRecRow(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
									 int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int row = args.IntVal(4);

	return pSnsData->RemoveRecordRow(rec, row);
}

//添加某个表的一行内容，命令格式：string snsspace, wstring snsdata, int msgid, string rec, ...;
bool SnsManager::AddPubDataRecRowValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
									   int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	CVarList values;
	values.Append(args, 4, args.GetCount() - 4);

	return pSnsData->AddRecordRowValue(rec, -1, values) >= 0;
}

//设置某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, int col, var value, ...;
bool SnsManager::SetPubDataRecValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
									int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int row = args.IntVal(4);
	int col = args.IntVal(5);

	switch (args.GetType(6))
	{
	case VTYPE_INT:
		return pSnsData->SetRecordInt(rec, row, col, args.IntVal(6));
		break;
	case VTYPE_INT64:
		return pSnsData->SetRecordInt64(rec, row, col, args.Int64Val(6));
		break;
	case VTYPE_FLOAT:
		return pSnsData->SetRecordFloat(rec, row, col, args.FloatVal(6));
		break;
	case VTYPE_DOUBLE:
		return pSnsData->SetRecordDouble(rec, row, col, args.DoubleVal(6));
		break;
	case VTYPE_STRING:
		return pSnsData->SetRecordString(rec, row, col, args.StringVal(6));
		break;
	case VTYPE_WIDESTR:
		return pSnsData->SetRecordWideStr(rec, row, col, args.WideStrVal(6));
		break;
	case VTYPE_OBJECT:
		return pSnsData->SetRecordObject(rec, row, col, args.ObjectVal(6));
		break;
	default:
		return false;
		break;
	}
}

//增加某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, int col, var value, ...;
bool SnsManager::IncPubDataRecValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
									int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int row = args.IntVal(4);
	int col = args.IntVal(5);

	switch (args.GetType(6))
	{
	case VTYPE_INT:
		return pSnsData->SetRecordInt(rec, row, col, 
			pSnsData->QueryRecordInt(rec, row, col) + args.IntVal(6));
	case VTYPE_INT64:
		return pSnsData->SetRecordInt64(rec, row, col, 
			pSnsData->QueryRecordInt64(rec, row, col) + args.Int64Val(6));
	case VTYPE_FLOAT:
		return pSnsData->SetRecordFloat(rec, row, col, 
			pSnsData->QueryRecordFloat(rec, row, col) + args.FloatVal(6));
	case VTYPE_DOUBLE:
		return pSnsData->SetRecordDouble(rec, row, col, 
			pSnsData->QueryRecordDouble(rec, row, col) + args.DoubleVal(6));
	default:
		return false;
	}
}

//根据关键值删除某个表的某行数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue;
bool SnsManager::RemovePubDataRecRowByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
										  int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int keycol = args.IntVal(4);

	int row = -1;
	switch (args.GetType(5))
	{
	case VTYPE_INT:
		row = pSnsData->FindRecordInt(rec, keycol, args.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pSnsData->FindRecordInt64(rec, keycol, args.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pSnsData->FindRecordFloat(rec, keycol, args.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pSnsData->FindRecordDouble(rec, keycol, args.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pSnsData->FindRecordString(rec, keycol, args.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pSnsData->FindRecordWideStr(rec, keycol, args.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pSnsData->FindRecordObject(rec, keycol, args.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	return pSnsData->RemoveRecordRow(rec, row);
}

//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
bool SnsManager::SetPubDataRecValueByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
										 int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int keycol = args.IntVal(4);

	int row = -1;
	switch (args.GetType(5))
	{
	case VTYPE_INT:
		row = pSnsData->FindRecordInt(rec, keycol, args.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pSnsData->FindRecordInt64(rec, keycol, args.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pSnsData->FindRecordFloat(rec, keycol, args.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pSnsData->FindRecordDouble(rec, keycol, args.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pSnsData->FindRecordString(rec, keycol, args.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pSnsData->FindRecordWideStr(rec, keycol, args.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pSnsData->FindRecordObject(rec, keycol, args.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	int col = args.IntVal(6);
	switch (args.GetType(7))
	{
	case VTYPE_INT:
		return pSnsData->SetRecordInt(rec, row, col, args.IntVal(7));
		break;
	case VTYPE_INT64:
		return pSnsData->SetRecordInt64(rec, row, col, args.Int64Val(7));
		break;
	case VTYPE_FLOAT:
		return pSnsData->SetRecordFloat(rec, row, col, args.FloatVal(7));
		break;
	case VTYPE_DOUBLE:
		return pSnsData->SetRecordDouble(rec, row, col, args.DoubleVal(7));
		break;
	case VTYPE_STRING:
		return pSnsData->SetRecordString(rec, row, col, args.StringVal(7));
		break;
	case VTYPE_WIDESTR:
		return pSnsData->SetRecordWideStr(rec, row, col, args.WideStrVal(7));
		break;
	case VTYPE_OBJECT:
		return pSnsData->SetRecordObject(rec, row, col, args.ObjectVal(7));
		break;
	default:
		return false;
		break;
	}
}

//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
bool SnsManager::IncPubDataRecValueByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
										 int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * rec = args.StringVal(3);
	int keycol = args.IntVal(4);

	int row = -1;
	switch (args.GetType(5))
	{
	case VTYPE_INT:
		row = pSnsData->FindRecordInt(rec, keycol, args.IntVal(5));
		break;
	case VTYPE_INT64:
		row = pSnsData->FindRecordInt64(rec, keycol, args.Int64Val(5));
		break;
	case VTYPE_FLOAT:
		row = pSnsData->FindRecordFloat(rec, keycol, args.FloatVal(5));
		break;
	case VTYPE_DOUBLE:
		row = pSnsData->FindRecordDouble(rec, keycol, args.DoubleVal(5));
		break;
	case VTYPE_STRING:
		row = pSnsData->FindRecordString(rec, keycol, args.StringVal(5));
		break;
	case VTYPE_WIDESTR:
		row = pSnsData->FindRecordWideStr(rec, keycol, args.WideStrVal(5));
		break;
	case VTYPE_OBJECT:
		row = pSnsData->FindRecordObject(rec, keycol, args.ObjectVal(5));
		break;
	default:
		return false;
		break;
	}

	if (row < 0)
		return false;

	int col = args.IntVal(6);

	switch (args.GetType(7))
	{
	case VTYPE_INT:
		return pSnsData->SetRecordInt(rec, row, col, 
			pSnsData->QueryRecordInt(rec, row, col) + args.IntVal(7));
	case VTYPE_INT64:
		return pSnsData->SetRecordInt64(rec, row, col, 
			pSnsData->QueryRecordInt64(rec, row, col) + args.Int64Val(7));
	case VTYPE_FLOAT:
		return pSnsData->SetRecordFloat(rec, row, col, 
			pSnsData->QueryRecordFloat(rec, row, col) + args.FloatVal(7));
	case VTYPE_DOUBLE:
		return pSnsData->SetRecordDouble(rec, row, col, 
			pSnsData->QueryRecordDouble(rec, row, col) + args.DoubleVal(7));
	default:
		return false;
	}
}

//更新配置，命令格式：string snsspace, wstring snsdata, int msgid
bool SnsManager::UpdateConfig(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,	 
							  int memeber_id,int scene_id, const char* uid,const IVarList& args)
{
	const char * pSnsName = args.StringVal(0);
	const wchar_t* pModuleName = args.WideStrVal(1);

	if (0 != std::strcmp(pSnsName, SPACE_SNS))
	{
		return false;
	}

	ISnsLogic * pModuleNameIt = FindSnsLogic(pModuleName);
	if (NULL != pModuleNameIt)
	{
		return pModuleNameIt->OnUpdateConfig(pSnsKernel, distribute_id, server_id, memeber_id, scene_id, uid, args);
	}

	return false;
}
