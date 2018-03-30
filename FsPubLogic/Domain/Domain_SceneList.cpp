//--------------------------------------------------------------------
// 文件名:      Domain_SceneList.cpp
// 内  容:      所有被创建的场景列表
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#include "Domain_SceneList.h"
#include "../../public/VarList.h"
#include "../../FsGame/Define/PubDefine.h"
#include "../../FsGame//Define/SceneListDefine.h"

const char * scene_list_rec_name = "scene_list_rec";
enum 
{
	SCENE_LIST_COL_scene_id,
	SCENE_LIST_COL_config_id,
	SCENE_LIST_COL_member_id,
};

int Domain_SceneList::OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{	
	return OnLoad(pPubKernel, pPubSpace);
}

int Domain_SceneList::OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
	if (NULL == pPubKernel || NULL == pPubSpace)
	{
		return 1;
	}

	IPubData * pSceneListData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());
	if (pSceneListData == NULL)
		return 1;

	if (!pSceneListData->FindRecord(scene_list_rec_name))
	{
		CreateTable(pSceneListData, scene_list_rec_name);
	}

	pSceneListData->ClearRecordRow(scene_list_rec_name);

	// 场景服务器对应的场景表
	if (!pSceneListData->FindRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC))
	{
		CVarList cols;
		cols << VTYPE_INT << VTYPE_INT;
		pSceneListData->AddRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC, 
			SERVER_MEMBER_FIRST_LOAD_SCENE_REC_MAX_ROW, 
			SERVER_MEMBER_FIRST_LOAD_SCENE_COL_TOTAL, 
			cols);
	}

	pSceneListData->ClearRecordRow(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);

	return 1;
}

/// \brief 接收到场景服务器的消息
/// \param source_id 发消息的服务器标识
/// \param msg 消息内容
int Domain_SceneList::OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
								int source_id, int scene_id, const IVarList& msg)
{
	IPubData * pSceneListData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());
	if (pSceneListData == NULL)
		return 1;

	int msgid = msg.IntVal(2);
	switch (msgid)
	{
	case SP_DOMAIN_MSG_SCENELIST_ADD_SCENE:
		{
			int sceneId = msg.IntVal(3);
			int memberId = msg.IntVal(5);

			CVarList value;
			value << sceneId << msg.StringVal(4) << memberId;

			pSceneListData->AddRecordRowValue(scene_list_rec_name, -1, value);

			IRecord* pMemberSceneRec = pSceneListData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
			if (pMemberSceneRec != NULL)
			{
				int row = pMemberSceneRec->FindInt(SERVER_MEMBER_FIRST_LOAD_SCENE_COL_member_id, memberId);
				if (row < 0)
				{
					// memberId 只对应一个场景
					pMemberSceneRec->AddRowValue(-1, CVarList() << memberId << sceneId);
				}
			}
		}
		break;
	case SP_DOMAIN_MSG_SCENELIST_DEL_SCENE:
		{
			int remove_scene_id = msg.IntVal(3);

			// 场景列表
			IRecord* pSceneListRec = pSceneListData->GetRecord(scene_list_rec_name);
			if (pSceneListRec == NULL)
			{
				break;
			}

			// 删除场景记录
			int row = pSceneListRec->FindInt(SCENE_LIST_COL_scene_id, remove_scene_id);
			if (row >= 0)
			{
				pSceneListData->RemoveRecordRow(scene_list_rec_name, row);
			}
			
			// member对应的场景表
			IRecord* pMemberSceneRec = pSceneListData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
			if (pMemberSceneRec == NULL)
			{
				break;
			}
			
			// 查找记录
			row = pMemberSceneRec->FindInt(SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id, remove_scene_id);
			if (row < 0)
			{
				// 无记录
				break;
			}

			// 查找被删除的场景对应的member Id
			int memberId = pMemberSceneRec->QueryInt(row, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_member_id);

			// 查找此member对应的其他场景
			int memberRow = pSceneListRec->FindInt(SCENE_LIST_COL_member_id, memberId);
			if (memberRow < 0)
			{
				// 无其他场景 删除此记录
				pMemberSceneRec->RemoveRow(row);
			}
			else
			{
				// 查询表中的场景ID
				int sceneId = pSceneListRec->QueryInt(memberRow, SCENE_LIST_COL_scene_id);

				// 修改member对应的场景表
				pSceneListRec->SetInt(row, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id, sceneId);
			}
		
		}
		break;
	}
	return 1;
}

void Domain_SceneList::CreateTable(IPubData * pSceneListData, const char * table_name)
{
	if (pSceneListData == NULL)
	{
		return;
	}

	if (!pSceneListData->FindRecord(table_name))
	{
		CVarList coltypes;
		coltypes << VTYPE_INT << VTYPE_STRING << VTYPE_INT;

		pSceneListData->AddRecord(table_name, 4096, (int)coltypes.GetCount(), coltypes);
	}	
}
