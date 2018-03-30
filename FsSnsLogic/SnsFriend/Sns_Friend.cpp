//--------------------------------------------------------------------
// 文件名:      SnsFriend.h
// 内  容:      好友消息存储
// 说  明:
// 创建日期:    2014年4月12日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#include "Sns_Friend.h"

#include "server/ISnsData.h"
#include "public/VarList.h"
#include "../../utils/string_util.h"
#include "../../FsGame/Define/SnsDefine.h"
#include "../../FsGame/Define/FriendDefine.h"

SnsFriend::SnsFriend(): ISnsLogic(SNS_FRIEND_SPACE, 0)
{
	// TODO 
}

SnsFriend::~SnsFriend()
{
	// TODO 
}

// 逻辑对象被创建
int SnsFriend::OnModuleCreate(ISnsKernel* pKernel, const IVarList& args)
{	
	return 1;
}

// SNS服务器所有的SNS数据已经加载完成准备就绪(在若干次OnSnsLoad之后)
int SnsFriend::OnReady(ISnsKernel* pKernel, const IVarList& args)
{
	return 1;
}

// SNS数据在内存中创建
int SnsFriend::OnCreate(ISnsKernel* pKernel, const char * uid, const IVarList& args)
{
	CreateFriendRecords(pKernel, uid);

	return 1;
}

int SnsFriend::OnLoad(ISnsKernel* pKernel, const char* uid, const IVarList& args)
{
	CreateFriendRecords(pKernel, uid);

	return 1;
}

//收到来自member的消息
int SnsFriend::OnMessage(ISnsKernel* pKernel, int distribute_id,int server_id, 
						   int memeber_id,int scene_id, const char* uid, const IVarList& args)
{
	// 消息格式
	//
	//    ["sns"]["SnsFriend"][msg_id][sub_msg_id]...
	//
	// 回应消息 
	//
	//    如果有回应消息, 则通过 ISnsKernel::SendToScenePlayer() 发给玩家

	if (pKernel == NULL)
	{
		return 1;
	}

	if (args.GetCount() == 0)
	{
		return 0;
	}

	int msg_id = args.IntVal(2);

    switch (msg_id)
    {
    case SNS_MSG_FRIEND_APPLY: // 好友申请
		FriendApply(pKernel, args);
        break;
	case SNS_MSG_FRIEND_DELETE: // 好友解除
		FriendDelete(pKernel, args);
		break;
    case SNS_MSG_FRIEND_RESULT: // 申请结果
		ApplyResult(pKernel, args);
        break;
    case SNS_MSG_FRIEND_QUERY_FRIEND_APPLY: // 查询申请
		QueryApply(pKernel, uid, server_id, memeber_id, scene_id, args);
        break;
	case SNS_MSG_FRIEND_QUERY_FRIEND_DELETE: // 删除申请
		QueryDelete(pKernel, uid, server_id, memeber_id, scene_id, args);
		break;
    case SNS_MSG_FRIEND_QUERY_FRIEND_RESULT: // 查询结果
		QueryResult(pKernel, uid, server_id, memeber_id, scene_id, args);
        break;
	case SNS_MSG_FRIEND_GIFT_GIVE: // 离线送花
		FriendGiftGive(pKernel, uid, server_id, memeber_id, scene_id, args);
        break;   
	case SNS_MSG_FRIEND_GIFT_GET: // 离线获取送花
		FriendGiftGet(pKernel, uid, server_id, memeber_id, scene_id, args);
		break;
	case SNS_MSG_FRIEND_PUSH_ADD_INTIMACY://离线增加亲密度
		AddIntimacy(pKernel,uid, args);
		break;
	case SNS_MSG_FRIEND_ADD_INTIMACY_GET://获取增加亲密度
		QueryAddIntimacy(pKernel, uid, server_id, memeber_id, scene_id, args);
		break;
	case SNS_MSG_FRIEND_ADD_DEL_SWORN://删除结义对象
	{
		AddDelSworn(pKernel, uid, server_id, memeber_id, scene_id, args);
	}break;
	case SNS_MSG_FRIEND_QUERY_DEL_SWORN:
	{
		QueryDelSworn(pKernel, uid, server_id, memeber_id, scene_id, args);
	
	}break;
	case SNS_MSG_FRIEND_ADD_ENEMY:
	{
		AddEnemy(pKernel, uid, server_id, memeber_id, scene_id, args);
	}break;
	case SNS_MSG_FRIEND_QUERY_ENEMY:
	{
		QueryEnemy(pKernel, uid, server_id, memeber_id, scene_id, args);
	};
	default:
        break;
    }

    return 1;
}

// 好友SNS表结果定义
void SnsFriend::CreateFriendRecords(ISnsKernel* pKernel, const char *uid)
{
    ISnsData *pSnsData = pKernel->GetSnsData(uid);
    if (NULL == pSnsData)
    {
        return;
    }
	
     CVarList coltypes0, coltypes1, coltypes2;

	 // 好友申请表
	 coltypes0 << VTYPE_STRING       // sender uid
		 << VTYPE_WIDESTR      // sender name
		 << VTYPE_STRING       // receiver uid
		 << VTYPE_WIDESTR      // receiver name
		 << VTYPE_INT          // level
		 << VTYPE_INT          // 国籍
		 << VTYPE_INT          // 职业
		 << VTYPE_INT          // 性别
		 << VTYPE_INT       // 战斗力
		 << VTYPE_WIDESTR	// 帮会
		 << VTYPE_INT     //申请类型 0-好友 
		 << VTYPE_INT;     // vip 


	// 申请结果表
	coltypes1 << VTYPE_STRING       // sender uid
              << VTYPE_WIDESTR      // sender name
              << VTYPE_STRING       // receiver uid
              << VTYPE_WIDESTR      // receiver name
              << VTYPE_INT          // level
			  << VTYPE_INT          // 国籍
			  << VTYPE_INT          // 职业
			  << VTYPE_INT          // 性别
              << VTYPE_INT          // 战斗力
			  << VTYPE_WIDESTR	// 帮会
              << VTYPE_INT;         // 结果


	// 解除好友关系表
	coltypes2 << VTYPE_STRING       // sender uid
			  << VTYPE_WIDESTR      // sender name
			  << VTYPE_STRING       // receiver uid
			  << VTYPE_WIDESTR;     // receiver name

	// 礼物表
	CVarList colGifts;
	colGifts << VTYPE_STRING		// sender uid
			<< VTYPE_INT;			// gift count
	//增加亲密度 
	CVarList colIntimacy;
	colIntimacy << VTYPE_STRING		//sender uid
		<< VTYPE_INT;				// 亲密度值	
	// 删除结义目标
	CVarList delSworn;
	delSworn << VTYPE_WIDESTR;

	//敌人表
	CVarList enemy;
	enemy << VTYPE_STRING
		<< VTYPE_WIDESTR
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_INT
		<< VTYPE_WIDESTR
		<< VTYPE_STRING
		<< VTYPE_INT
		<< VTYPE_INT64;

    _create_record(pKernel, pSnsData, SNS_FRIEND_APPLY_RECORD,  coltypes0, SNS_FRIEND_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_RESULT_RECORD, coltypes1, SNS_FRIEND_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_DELETE_RECORD, coltypes2, SNS_FRIEND_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_GIFT_RECORD, colGifts, SNS_FRIEND_GIFT_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_ADD_INITMACY, colIntimacy, SNS_FRIEND_GIFT_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_DEL_SWORN, delSworn, SNS_FRIEND_DEL_SWORN_MAXROW);
	_create_record(pKernel, pSnsData, SNS_FRIEND_ADD_ENEMY, enemy, SNS_FRIEND_ENEMY_MAXROW);
}

// 创建表格
void SnsFriend::_create_record(ISnsKernel *pKernel, ISnsData *pSnsData, const char *recName, const IVarList &coltype, int rows)
{
    if (pSnsData->FindRecord(recName))
    {
        // 旧数据表列数不对，删除旧表
        if (pSnsData->GetRecordCols(recName) != (int)coltype.GetCount())
        {
            pSnsData->RemoveRecord(recName);
            pSnsData->AddRecord(recName, rows, (int)coltype.GetCount(), coltype);
        }
    }
    else
    {
        pSnsData->AddRecord(recName, rows, (int)coltype.GetCount(), coltype);
    }
}

// 好友申请
void SnsFriend::FriendApply(ISnsKernel *pKernel, const IVarList &msg)
{
    const char * senderUID    = msg.StringVal(3);
    const char * receiverUID  = msg.StringVal(5);
    int applyType = msg.IntVal(13);

	if (StringUtil::CharIsNull(senderUID) || 
		StringUtil::CharIsNull(receiverUID))
	{
		return;
	}

    ISnsData *pSnsData = pKernel->GetSnsData(receiverUID);
    if (pSnsData == NULL)
    {
        return;
    }
    IRecord* friendApplyRec = pSnsData->GetRecord(SNS_FRIEND_APPLY_RECORD);
    if (NULL == friendApplyRec)
    {
        return;
    }

    bool bExist = false;
    int rows = friendApplyRec->GetRows();
    LoopBeginCheck(a);
    for (int i = 0; i < rows; ++i)
    {
        LoopDoCheck(a);
        const char *senderUIDinRec = friendApplyRec->QueryString(i, SNS_FRIEND_APPLY_COL_SENDER_UID);
        const char *receiverUIDinRec = friendApplyRec->QueryString(i, SNS_FRIEND_APPLY_COL_RECEIVER_UID);
        int applyTypeinRec = friendApplyRec->QueryInt(i, SNS_FRIEND_APPLY_COL_TYPE);

        if (strcmp(senderUID, senderUIDinRec) == 0 && 
			strcmp(receiverUID, receiverUIDinRec) == 0 &&
            applyType == applyTypeinRec)
        {
            bExist = true;
            break;
        }
    }

    if (!bExist)
    {
        CVarList rowVal;
        rowVal.Append(msg, 3, 12);

		if (friendApplyRec->GetRows() >= SNS_FRIEND_MAXROW)
		{
            friendApplyRec->RemoveRow(0);
		}

        friendApplyRec->AddRowValue(-1, rowVal);
    }
}

// 申请解除好友关系
void SnsFriend::FriendDelete(ISnsKernel *pKernel, const IVarList &msg)
{
	const char      *senderUID      = msg.StringVal(3);
	const wchar_t   *senderName     = msg.WideStrVal(4);
	const char      *receiverUID    = msg.StringVal(5);
	const wchar_t	*receiverName   = msg.WideStrVal(6);

	if (StringUtil::CharIsNull(senderUID) || 
		StringUtil::CharIsNull(senderName) || 
		StringUtil::CharIsNull(receiverUID) || 
		StringUtil::CharIsNull(receiverName))
	{
		return;
	}

	ISnsData *pSnsData = pKernel->GetSnsData(receiverUID);
	if (pSnsData == NULL)
	{
		return;
	}
    IRecord* friendDeleteRec = pSnsData->GetRecord(SNS_FRIEND_DELETE_RECORD);
    if (NULL == friendDeleteRec)
    {
        return;
    }

	const char * senderUIDinRec = NULL;
	const char * receiverUIDinRec = NULL;

	bool bExist = false;
	int rows = friendDeleteRec->GetRows();
    LoopBeginCheck(b);
	for (int i = 0; i < rows; ++i)
	{
        LoopDoCheck(b);
		senderUIDinRec = friendDeleteRec->QueryString(i, SNS_FRIEND_DELETE_COL_SENDER_UID);
		receiverUIDinRec = friendDeleteRec->QueryString(i, SNS_FRIEND_DELETE_COL_RECEIVER_UID);

		if (strcmp(senderUID, senderUIDinRec) == 0 && 
			strcmp(receiverUID, receiverUIDinRec) == 0)
		{
			bExist = true;
			break;
		}
	}

	if (!bExist)
	{
		CVarList rowVal;
		rowVal.Append(msg, 3, 4);

		if (friendDeleteRec->GetRows() >= SNS_FRIEND_MAXROW)
		{
            friendDeleteRec->RemoveRow(0);
		}

        friendDeleteRec->AddRowValue(-1, rowVal);
	}
}

// 申请结果
void SnsFriend::ApplyResult(ISnsKernel *pKernel, const IVarList &msg)
{
	const char *senderUID = msg.StringVal(3);
	const char *recverUID = msg.StringVal(5);
    int applyType = msg.IntVal(14);

	ISnsData *pSnsData = pKernel->GetSnsData(recverUID);
	if (NULL == pSnsData)
	{
		return;
	}
    IRecord* friendResultRec = pSnsData->GetRecord(SNS_FRIEND_RESULT_RECORD);
    if (NULL == friendResultRec)
    {
        return;
    }
  /*  IRecord* partnerResultRec = pSnsData->GetRecord(SNS_PARTNER_RESULT_RECORD);
    if (NULL == partnerResultRec)
    {
        return;
    }*/
    bool bExist = false;
    if (applyType == 0)
    {
        //好友
        int rows = friendResultRec->GetRows();
        LoopBeginCheck(c);
        for (int i=0; i<rows; i++)
        {
            LoopDoCheck(c);
            const char *sender_uid = friendResultRec->QueryString(i, SNS_FRIEND_RESULT_COL_SENDER_UID);
            const char *receiver_uid = friendResultRec->QueryString(i, SNS_FRIEND_RESULT_COL_RECEIVER_UID);

            if (strcmp(senderUID, sender_uid) == 0 && 
                strcmp(recverUID, receiver_uid) == 0)
            {
                bExist = true;
                break;
            }
        }
        if (!bExist)
        {
            CVarList rowVal;
            rowVal.Append(msg, 3, 12);

            if (friendResultRec->GetRows() >= SNS_FRIEND_MAXROW)
            {
                friendResultRec->RemoveRow(0);
            }

            friendResultRec->AddRowValue(-1, rowVal);

        }
    }
	
}

// 查询申请数据
void SnsFriend::QueryApply(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
    if (pSnsData == NULL)
    {
        return;
    }
    IRecord* friendApplyRec = pSnsData->GetRecord(SNS_FRIEND_APPLY_RECORD);
    if (NULL == friendApplyRec)
    {
        return;
    }

	const char *	sender_uid = NULL;
	const wchar_t * sender_name = NULL;
	int level = 0;
	int job = 0;
	int sex = 0;
	int fightcap = 0;
	const wchar_t *guild_name = NULL;
    int applyType = 0;
	int vip = 0;
	const PERSISTID sender = msg.ObjectVal(4);
    LoopBeginCheck(d);
	int nRows = friendApplyRec->GetRows();
	for (int i = 0; i < nRows; ++i)
	{
        LoopDoCheck(d);
		sender_uid	= friendApplyRec->QueryString(	i,  SNS_FRIEND_APPLY_COL_SENDER_UID);
		sender_name	= friendApplyRec->QueryWideStr(	i,  SNS_FRIEND_APPLY_COL_SENDER_NAME);
		level		= friendApplyRec->QueryInt(		i,  SNS_FRIEND_APPLY_COL_LEVEL);
		job			= friendApplyRec->QueryInt(		i,  SNS_FRIEND_APPLY_COL_JOB);
		sex			= friendApplyRec->QueryInt(		i,  SNS_FRIEND_APPLY_COL_SEX);
		fightcap	= friendApplyRec->QueryInt(		i,  SNS_FRIEND_APPLY_COL_FIGHT_CAPACITY);
		guild_name	= friendApplyRec->QueryWideStr(	i,  SNS_FRIEND_APPLY_COL_GUILD_NAME);
        applyType   = friendApplyRec->QueryInt(		i,  SNS_FRIEND_APPLY_COL_TYPE);
		vip			= friendApplyRec->QueryInt(i, SNS_FRIEND_APPLY_COL_VIP_LEVEL);

        pKernel->SendToSceneMessage(server_id, memeber_id, scene_id, 
                                    CVarList() << SNS_MSG_FRIEND_PUSH_APPLY
                                               << sender
                                               << sender_uid
                                               << sender_name
                                               << level
											   << job
											   << sex
                                               << fightcap
											   << guild_name
                                               << applyType
											   << vip);

	}
    pSnsData->ClearRecordRow(SNS_FRIEND_APPLY_RECORD);
}

// 查询删除申请数据
void SnsFriend::QueryDelete(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (NULL == pSnsData)
	{
		return;
	}
    IRecord* friendDeleteRec = pSnsData->GetRecord(SNS_FRIEND_DELETE_RECORD);
    if (NULL == friendDeleteRec)
    {
        return;
    }

	const PERSISTID sender = msg.ObjectVal(4);
    LoopBeginCheck(e);
	int nRows = friendDeleteRec->GetRows();
	for (int i = 0; i < nRows; ++i)
	{
        LoopDoCheck(e);
		const char      *sender_uid = friendDeleteRec->QueryString(i, SNS_FRIEND_DELETE_COL_SENDER_UID);
		const wchar_t   *sender_name = friendDeleteRec->QueryWideStr(i, SNS_FRIEND_DELETE_COL_SENDER_NAME);

		pKernel->SendToSceneMessage(server_id, memeber_id, scene_id, 
									CVarList() << SNS_MSG_FRIEND_PUSH_DELETE
									<< sender
									<< sender_uid
									<< sender_name);
	}
	pSnsData->ClearRecordRow(SNS_FRIEND_DELETE_RECORD);
}

// 查询申请结果
void SnsFriend::QueryResult(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
    if (pSnsData == NULL)
    {
        return;
    }
    IRecord* friendResultRec = pSnsData->GetRecord(SNS_FRIEND_RESULT_RECORD);
    if (NULL == friendResultRec)
    {
        return;
    }

	const PERSISTID sender = msg.ObjectVal(4);
    LoopBeginCheck(f);
	int nRows = friendResultRec->GetRows();
	for ( int i = 0; i < nRows; ++i )
	{
        LoopDoCheck(f);
		const char      *sender_uid	    = friendResultRec->QueryString(i,  SNS_FRIEND_RESULT_COL_SENDER_UID);
        const wchar_t   *sender_name    = friendResultRec->QueryWideStr(i,  SNS_FRIEND_RESULT_COL_SENDER_NAME);
		int				level	        = friendResultRec->QueryInt(i,  SNS_FRIEND_RESULT_COL_LEVEL);
		int				job				= friendResultRec->QueryInt(i,  SNS_FRIEND_RESULT_COL_JOB);
		int				sex				= friendResultRec->QueryInt(i,  SNS_FRIEND_RESULT_COL_SEX);
		int				fightcap	    = friendResultRec->QueryInt(i,  SNS_FRIEND_RESULT_COL_FIGHT_CAPACITY);
		const wchar_t   *guild_name    = friendResultRec->QueryWideStr(i,  SNS_FRIEND_RESULT_COL_GUILD_NAME);
		int				result	        = friendResultRec->QueryInt(i,  SNS_FRIEND_RESULT_COL_RESULT);

        pKernel->SendToSceneMessage(server_id, memeber_id, scene_id, 
                                    CVarList() << SNS_MSG_FRIEND_PUSH_RESULT
                                               << sender
                                               << sender_uid
                                               << sender_name
                                               << level
											   << job
											   << sex
                                               << fightcap
											   << guild_name
                                               << result);

	}
	pSnsData->ClearRecordRow(SNS_FRIEND_RESULT_RECORD);
}

// 离线送花
void SnsFriend::FriendGiftGive(ISnsKernel *pKernel, const char *uid,
			int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	const char * senderUID = msg.StringVal(3);
	int nNum = msg.IntVal(4);
	if (StringUtil::CharIsNull(uid) ||
		StringUtil::CharIsNull(senderUID))
	{
		return;
	}

	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}
	IRecord* pGiftRec = pSnsData->GetRecord(SNS_FRIEND_GIFT_RECORD);
	if (NULL == pGiftRec)
	{
		return;
	}


	CVarList rowVal;
	rowVal.Append(msg, 3, 2);
	if (pGiftRec->GetRows() >= SNS_FRIEND_GIFT_MAXROW)
	{
		pGiftRec->RemoveRow(0);
	}
	pGiftRec->AddRowValue(-1, rowVal);
}

// 获取离线送花
void SnsFriend::FriendGiftGet(ISnsKernel *pKernel, const char *uid,
			int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}
	IRecord* pGiftRec = pSnsData->GetRecord(SNS_FRIEND_GIFT_RECORD);
	if (NULL == pGiftRec)
	{
		return;
	}

	const PERSISTID sender = msg.ObjectVal(3);
	LoopBeginCheck(f);
	int nRows = pGiftRec->GetRows();
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(f);
		const char      *sender_uid = pGiftRec->QueryString(i, SNS_FRIEND_GIFT_COL_SENDER_UID);
		int				count = pGiftRec->QueryInt(i, SNS_FRIEND_GIFT_COL_GIFT_COUNT);

		pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
			CVarList() << SNS_MSG_FRIEND_GIFT_PUSH
			<< sender
			<< sender_uid
			<< count );

	}
	pSnsData->ClearRecordRow(SNS_FRIEND_GIFT_RECORD);
}

void SnsFriend::AddIntimacy(ISnsKernel *pKernel, const char *uid, const IVarList &msg)
{



	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}
	IRecord* paddInitmacy = pSnsData->GetRecord(SNS_FRIEND_ADD_INITMACY);
	if (NULL == paddInitmacy)
	{
		return;
	}

	const char* friendUid = msg.StringVal(3);
	int addIntimacy = msg.IntVal(4);


	paddInitmacy->AddRowValue(-1, CVarList() << friendUid <<addIntimacy);


}

void SnsFriend::QueryAddIntimacy(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{

	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}
	IRecord* pAddInitmacy = pSnsData->GetRecord(SNS_FRIEND_ADD_INITMACY);
	if (NULL == pAddInitmacy)
	{
		return;
	}
	
	int rowMax = pAddInitmacy->GetRows();
	if (rowMax > 0)
	{
		const PERSISTID sender = msg.ObjectVal(3);
		CVarList send;
		for (int i = 0; i < rowMax; i++)
		{
			send << pAddInitmacy->QueryString(i, SNS_FRIEND_ADD_INTIMACY_COL_SENDER_UID)
				<< pAddInitmacy->QueryInt(i, SNS_FRIEND_ADD_INTIMACY_COL_VALUE);
		}

		pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
			CVarList() << SNS_MSG_FRIEND_ADD_INTIMACY_RESULT
			<< sender
			<< rowMax
			<< send);


		pAddInitmacy->ClearRow();
	}




}

void SnsFriend::AddDelSworn(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}

	const wchar_t *targetName = msg.WideStrVal(3);

	IRecord* pDelSworn = pSnsData->GetRecord(SNS_FRIEND_DEL_SWORN);
	if (NULL == pDelSworn)
	{
		return;
	}

	int row = pDelSworn->FindWideStr(SNS_FRIEND_DEL_SWORN_COL_NAME, targetName);
	if (row >= 0)	{
		return;
	}

	pDelSworn->AddRowValue(-1, CVarList()<<targetName );
	
	
	
}

void SnsFriend::QueryDelSworn(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}

	

	IRecord* pDelSworn = pSnsData->GetRecord(SNS_FRIEND_DEL_SWORN);
	if (NULL == pDelSworn)
	{
		return;
	}
	int rowMax = pDelSworn->GetRows();
	if (rowMax>0){
		const PERSISTID sender = msg.ObjectVal(3);
		CVarList send;
		for (int i = 0; i < rowMax; i++)
		{
			send << pDelSworn->QueryString(i, SNS_FRIEND_DEL_SWORN_COL_NAME);
				
		}
		pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
			CVarList() << SNS_MSG_FRIEND_PUSH_DEL_SWORN
			<< sender
			<< rowMax
			<< send);
		pDelSworn->ClearRow();
		return;
	}
	
}

void SnsFriend::AddEnemy(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{
	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}
	const char *pUid = msg.StringVal(3);
	CVarList info;
	info.Append(msg, 3, SNS_FRIEND_ENEMY_COL_MAX);
	int size = info.GetCount();
	if (size != SNS_FRIEND_ENEMY_COL_MAX)
	{
		return;
	}


	IRecord* pEnemy = pSnsData->GetRecord(SNS_FRIEND_ADD_ENEMY);
	if (NULL == pEnemy)
	{
		return;
	}

	int row = pEnemy->FindString(SNS_FRIEND_ENEMY_COL_UID, pUid);
	if (row >= 0){
		pEnemy->SetInt(row,SNS_FRIEND_ENEMY_COL_LEVEL, info.IntVal(2));
		pEnemy->SetInt(row, SNS_FRIEND_ENEMY_COL_ABILITY, info.IntVal(6));
		pEnemy->SetWideStr(row, SNS_FRIEND_ENEMY_COL_GUILDNAME, info.WideStrVal(10));
		pEnemy->SetInt64(row, SNS_FRIEND_ENEMY_COL_TIMES, info.Int64Val(12));
		pEnemy->SetInt(row, SNS_FRIEND_ENEMY_COL_HATER, info.IntVal(13));
		pEnemy->SetInt(row, SNS_FRIEND_ENEMY_COL_VIP_LV, info.IntVal(14));
	}
	else
	{
		pEnemy->AddRowValue(-1, info);
	}
}

void SnsFriend::QueryEnemy(ISnsKernel *pKernel, const char *uid, int server_id, int memeber_id, int scene_id, const IVarList &msg)
{


	ISnsData *pSnsData = pKernel->GetSnsData(uid);
	if (pSnsData == NULL)
	{
		return;
	}



	IRecord* pEnemy = pSnsData->GetRecord(SNS_FRIEND_ADD_ENEMY);
	if (NULL == pEnemy)
	{
		return;
	}
	int rowMax = pEnemy->GetRows();
	if (rowMax > 0)
	{
		const PERSISTID sender = msg.ObjectVal(3);
		CVarList send;
		for (int i = 0; i < rowMax; i++)
		{
			CVarList info;
			pEnemy->QueryRowValue(i, info);
			send << info;

		}
		pKernel->SendToSceneMessage(server_id, memeber_id, scene_id,
			CVarList() << SNS_MSG_FRIEND_PUSH_ENEMY
			<< sender
			<< rowMax
			<< send);
		pEnemy->ClearRow();
		return;
	}




}
