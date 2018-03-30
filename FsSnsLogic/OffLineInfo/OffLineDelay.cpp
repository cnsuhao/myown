//----------------------------------------------------------
// 文件名:      OffLineDelay.cpp
// 内  容:      离线拉人
// 说  明:
// 创建日期:    2014年11月21日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#include "OffLineDelay.h"
#include "../../server/ISnsCallee.h"
#include "../../server/SnsLogicDll.h"
#include "../../public/VarList.h"
#include "../../public/Inlines.h"
#include "../../FsGame/Define/SnsDefine.h"
#include "../../utils/util_func.h"
#include "../../server/ISnsData.h"
#include "../../system/WinEncoding.h"
#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "FsGame/Define/OffLineDefine.h"
#include "FsGame/Define/SnsDataDefine.h"
#include "../SnsDataSync/SnsDataSync.h"


OffLineDelay::OffLineDelay(): ISnsLogic(L"OffLineDelay", 0)
{
}

int OffLineDelay::OnModuleCreate(ISnsKernel* pKernel, const IVarList& args)
{	
	return 1;
}

int OffLineDelay::OnReady(ISnsKernel* pKernel, const IVarList& args)
{
	LoadDataInfoConfig(pKernel);
	//注册心跳
	pKernel->DeclareHeartBeat("Offline_delay_heart", OffLineDelay::Offline_delay_heart);

	return 1;
}

// \brief sns数据在内存中创建
// \param args
// \uid  sns uid //sns数据关键字
// \args  
int OffLineDelay::OnCreate(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args)
{
	return 1;
}

int OffLineDelay::OnLoad(ISnsKernel* pKernel, const char* uid, const IVarList& args)
{
	return 1;
}

//推送心跳
int OffLineDelay::Offline_delay_heart(ISnsKernel * pKernel, 
					  const char* uid, int time)
{
	ISnsData * pSnsData = pKernel->GetSnsData(uid);
	if ( NULL == pSnsData )
	{
		return 0;
	}

	const char * serverid = pSnsData->QueryAttrString("ServerIdString");

	CVarList args;

	util_split_string(args, serverid, ",");

	if (args.GetCount() < 3)
	{
		return 0;
	}

	const int server_id = args.IntVal(0);
	const int memeber_id = args.IntVal(1);
	const int scene_id = args.IntVal(2); 

	const char* account = pSnsData->QueryAttrString("PlayerAccount");
	const wchar_t* name = pSnsData->QueryAttrWideStr("PlayerName");
	const char* playerTime = pSnsData->QueryAttrString("PlayerTime");
	
	pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
		        CVarList() << SNS_MSG_PUSH_PLAYER_OFFLINE << account << name << playerTime);

	return 1;
}


// 响应sns拉取离线玩家消息
int OffLineDelay::OnPushPlayerOffline(ISnsKernel* pKernel, const char* uid, const IVarList& prev_args)
{
	if ( prev_args.GetCount() == 0 )
	{
		return 0;
	}

	const int server_id = prev_args.IntVal(0);
	const int memeber_id = prev_args.IntVal(1);
	const int scene_id = prev_args.IntVal(2);

	const char* account = prev_args.StringVal(3);
	const wchar_t* name = prev_args.WideStrVal(4);
	const char* time = prev_args.StringVal(5);

	//断线保护时间 
	const int64_t protectTime = prev_args.Int64Val(6);// 单位:秒

	if (protectTime <= 0 || StringUtil::CharIsNull(uid))
	{
		return 0;
	}

	char buffId[256] = {0};

	SPRINTF_S(buffId, "%d,%d,%d", server_id, memeber_id, scene_id);

	ISnsData * pSnsData = pKernel->GetSnsData(uid);
	if ( NULL == pSnsData )
	{
		return 0;
	}

	//设置服务器id
	if ( !pSnsData->FindAttr("ServerIdString") )
	{
		pSnsData->AddAttr( "ServerIdString", VTYPE_STRING, true );
	}
	pSnsData->SetAttrString( "ServerIdString", buffId );

	//账号
	if ( !pSnsData->FindAttr("PlayerAccount") )
	{
		pSnsData->AddAttr( "PlayerAccount", VTYPE_STRING, true );
	}
	pSnsData->SetAttrString( "PlayerAccount", account );

	//名称
	if ( !pSnsData->FindAttr("PlayerName") )
	{
		pSnsData->AddAttr( "PlayerName", VTYPE_WIDESTR, true );
	}
	pSnsData->SetAttrWideStr( "PlayerName", name );

	//时间
	if ( !pSnsData->FindAttr("PlayerTime") )
	{
		pSnsData->AddAttr( "PlayerTime", VTYPE_STRING, true );
	}
	pSnsData->SetAttrString( "PlayerTime", time );

	//取消对应心跳
	if (pKernel->FindHeartBeat(uid, "Offline_delay_heart") )
	{
		pKernel->RemoveHeartBeat(uid, "Offline_delay_heart");
	}
	pKernel->AddCountBeat(uid, "Offline_delay_heart", (int)protectTime * 1000, 1);

	pKernel->SaveSns(uid);
	return 0;
}

//域服务器 收到消息
int OffLineDelay::OnMessage(ISnsKernel* pKernel, int distribute_id,int server_id, 
										int memeber_id,int scene_id, const char* uid, const IVarList& args)
{
	int imsg = args.IntVal(2);
	switch (imsg)
	{
		// 创建
	case SNS_MSG_PUSH_PLAYER_OFFLINE:
		{
			if ( args.GetCount() < 7 )
			{
				return 1;
			}
			const char* account = args.StringVal(3);
			const wchar_t* name = args.WideStrVal(4);
			const char* time = args.StringVal(5);
			const int64_t protectTime = args.Int64Val(6);


			CVarList  arglist;
			arglist << server_id << memeber_id << scene_id << account << name << time << protectTime;

			OnPushPlayerOffline( pKernel, uid, arglist);
		}
		break;
		// 查询某个玩家的数据
	case SNS_MSG_QUERY_PLAYER_FIGHT_DATA:
		{
			if ( args.GetCount() < 4 )
			{
				return 1;
			}
			// 需要玩家数据的npc对象
			PERSISTID npc = args.ObjectVal(3);

			// 生成所要查询的玩家数据
			CVarList playerData;
			ISnsData * pSnsData = pKernel->GetSnsData(uid);
			if (NULL == pSnsData)
			{
				// 发送数据
				pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
					CVarList() << SNS_MSG_QUERY_PLAYER_FIGHT_DATA << npc << playerData);
				return 1;
			}

			GenerateQueryData(pSnsData, playerData);
			// 发送数据
			pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
				CVarList() << SNS_MSG_QUERY_PLAYER_FIGHT_DATA << npc << playerData);
		}
		break;
	case SNS_MSG_QUERY_PLAYER_PROPERTY:
		{
			if ( args.GetCount() < 5 )
			{
				return 1;
			}
			CVarList dst_msg;
			int cmd_id = args.IntVal(3);
			const char* attr_names = args.StringVal(4);
			if (StringUtil::CharIsNull(attr_names))
			{
				return 1;
			}
			LoadPlayerPropertys(pKernel, attr_names, args, dst_msg);
			// 发送数据
			pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
				CVarList() << SNS_MSG_QUERY_PLAYER_PROPERTY << cmd_id << dst_msg);
		}
		break;
		// 查询某个玩家的数据
	case SNS_MSG_CANCEL_PUSH_OFFLINE_PLAYER:
		{
			//取消对应心跳
			if (pKernel->FindHeartBeat(uid, "Offline_delay_heart") )
			{
				pKernel->RemoveHeartBeat(uid, "Offline_delay_heart");
			}
		}
		break;
	default:
		break;
	}

	return 1;
}

// 读取数据信息配置
bool OffLineDelay::LoadDataInfoConfig(ISnsKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/OffLine/MirrorPorperty.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_vDataInfo.reserve(iSectionCount);
	LoopBeginCheck(a)
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(a)
		const char* strSectionName = xmlfile.GetSectionByIndex(i);
		if (0 == i)	// 第一行为其他配置
		{
			continue;
		}
		MirDataInfo info;
		info.nType		= xmlfile.ReadInteger(strSectionName, "Type", 0);
		info.strName	= xmlfile.ReadString(strSectionName, "DataName", "");
		if (DATA_CONTAINER_TYPE == info.nType || DATA_RECORD_TYPE == info.nType)
		{
			info.strSnsTable = xmlfile.ReadString(strSectionName, "SnsTable", "");
		}
		m_vDataInfo.push_back(info);
	}
	return true;
}

// 生成查询数据
void OffLineDelay::GenerateQueryData(ISnsData* pSnsData, IVarList& outData)
{
	if ( NULL == pSnsData )
	{
		return;
	}
	outData.Clear();
	int nSize = (int)m_vDataInfo.size();
	LoopBeginCheck(b)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(b)
		MirDataInfo& info = m_vDataInfo[i];
		// 填充属性数据
		if (DATA_PROPERTY_TYPE == info.nType)
		{
			FillPropertyData(pSnsData, info.strName.c_str(), outData);
		}
		else
		{
			// 填充容器数据,容器数据以表的形式存储
			FillContainerOrRecData(pSnsData, info.strName.c_str(), info.strSnsTable.c_str(), outData);
		}
	}
}

// 获取某个属性数据
void OffLineDelay::FillPropertyData(ISnsData* pSnsData, const char* strProName, IVarList& outData)
{
	if (NULL == pSnsData)
	{
		return;
	}
	if(!pSnsData->FindAttr(strProName))
	{
		outData << NOT_VALID_VALUE;
	}
	else 
	{
		const SnsDataSync::ElementDef* pDataType = SnsDataSync::m_pSnsDataSync->GetElementDef(strProName, "PlayerAttribute");
		if (NULL == pDataType)
		{
			return;
		}
		switch(pDataType->type)
		{
		case VTYPE_INT:
			outData << pSnsData->QueryAttrInt(strProName);
			break;
		case VTYPE_INT64:
			{
				// hp maxhp这两个属性由int改为int64,为了使数据库中的int兼容int64所做的处理
				int64_t nValue = 0;
				if (pSnsData->GetAttrType(strProName) == VTYPE_INT)
				{
					nValue = (int64_t)pSnsData->QueryAttrInt(strProName);
				}
				else
				{
					nValue = pSnsData->QueryAttrInt64(strProName);
				}
				outData << nValue;
			}
			break;
		case VTYPE_FLOAT:
			outData << pSnsData->QueryAttrFloat(strProName);
			break;
		case VTYPE_STRING:
			outData << pSnsData->QueryAttrString(strProName);
			break;
		case VTYPE_WIDESTR:
			outData << pSnsData->QueryAttrWideStr(strProName);
			break;
		}
	}
}

// 获取容器或表数据
void OffLineDelay::FillContainerOrRecData(ISnsData* pSnsData, const char* strProName, const char* strRec, IVarList& outData)
{
	if (NULL == pSnsData)
	{
		return;
	}
	// 填充容器数据,容器数据以表的形式存储
	if (!pSnsData->FindRecord(strRec))
	{
		outData << 0 << 0;
		return;
	}
	
	// 填充每行每列的数据
	int nRows = pSnsData->GetRecordRows(strRec);
	int nCols = pSnsData->GetRecordCols(strRec);
	outData << nRows << nCols;
	LoopBeginCheck(c)
	for (int i = 0;i < nRows;++i)
	{
		LoopDoCheck(c)
		LoopBeginCheck(d) 
		for (int j = 0;j < nCols;++j)
		{
			LoopDoCheck(d)
			int nType = pSnsData->GetRecordColType(strRec, j);
			switch(nType)
			{
			case VTYPE_INT:
				outData << pSnsData->QueryRecordInt(strRec, i, j);
				break;
			case VTYPE_INT64:
				outData << pSnsData->QueryRecordInt64(strRec, i, j);
				break;
			case VTYPE_FLOAT:
				outData << pSnsData->QueryRecordFloat(strRec, i, j);
				break;
			case VTYPE_STRING:
				outData << pSnsData->QueryRecordString(strRec, i, j);
				break;
			case VTYPE_WIDESTR:
				outData << pSnsData->QueryRecordWideStr(strRec, i, j);
				break;
			}
		}
	}
}

void OffLineDelay::LoadPlayerPropertys(ISnsKernel* pKernel, const char* attr_names, const IVarList& req_msg, IVarList& dst_msg)
{
	//查找玩家属性(支持:int,int64,float,double,sting) cmd_id(int32),"attr_name,attr_name",{ident_id(int64),target_uid(char)}..=>{ident_id,"attr_name=attr_value,attr_name=attr_value"}
	dst_msg.Clear();
	CVarList attr_list_name;
	CVarList attr_list_type;
	util_split_string(attr_list_name, attr_names, C_KERNEL_STRIMG_COMMA);

	int n_count = (int)req_msg.GetCount();
	ISnsData * pSnsData = NULL;
	for (int index = 5; index < n_count;index=index+2)
	{
		pSnsData = pKernel->GetSnsData(req_msg.StringVal(index+1));
		if (NULL != pSnsData)
		{
			if (attr_list_type.GetCount() <= 0)
			{
				for (int dd = 0; dd < (int)attr_list_name.GetCount(); dd++)
				{
					attr_list_type.AddInt(pSnsData->GetAttrType(attr_list_name.StringVal(dd)));
				}
			}
			LoadPlayerPropertys_Detal(pSnsData, attr_list_name, attr_list_type, req_msg.Int64Val(index), dst_msg);
		}
	}

}
void OffLineDelay::LoadPlayerPropertys_Detal(ISnsData * pSnsData, const IVarList& attr_list_name, const IVarList& attr_list_type, int64_t ident_id, IVarList& dst_msg)
{
	int  attr_type = 0;
	const char* attr_name = NULL;
	temp_string_.Clear();
	
	for (int index = 0; index < (int)attr_list_name.GetCount(); index++)
	{
		attr_name = attr_list_name.StringVal(index);
		attr_type = attr_list_type.IntVal(index);
		switch (attr_type)
		{
		case VTYPE_INT:
			temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%d", attr_name, pSnsData->QueryAttrInt(attr_name));
			break;
		case VTYPE_INT64:
			temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%lld", attr_name, pSnsData->QueryAttrInt64(attr_name));
			break;
		case VTYPE_FLOAT:
			temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%f", attr_name, pSnsData->QueryAttrFloat(attr_name));
			break;
		case VTYPE_DOUBLE:
			temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%f", attr_name, pSnsData->QueryAttrFloat(attr_name));
			break;
		case VTYPE_STRING:
			temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%s", attr_name, pSnsData->QueryAttrString(attr_name));
			break;
		case VTYPE_WIDESTR:
			//temp_string_.AppendSplitFormat(C_KERNEL_STRIMG_COMMA, "%s=%s", attr_name, pSnsData->QueryAttrWideStr(attr_name));
			break;
		default:
			break;
		}
	}
	dst_msg << ident_id << temp_string_.GetString();
}