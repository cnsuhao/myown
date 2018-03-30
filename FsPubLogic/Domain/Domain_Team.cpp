#include "Domain_Team.h"
#include "public/VarList.h"
#include "public/Var.h"
#include "utils/util_func.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/TeamDefine.h"
#include "FsGame/Define/SnsDefine.h"
#include <vector>
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
Domain_Team* Domain_Team::_this_team = NULL;
std::list<Domain_Team::Match_Player_Info> Domain_Team::match_apply;
int Domain_Team::OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{

    return OnLoad(pPubKernel, pPubSpace);
}


int Domain_Team::OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace)
{
    IPubData *pTeamData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());
    if (pTeamData == NULL)
        return 1;

    //删除所有表格
    CVarList rec_list;
    pTeamData->GetRecordList(rec_list);
	LoopBeginCheck(a);
    for (int i = 0; i < (int)rec_list.GetCount(); ++i)
	{
		LoopDoCheck(a);
        pTeamData->RemoveRecord(rec_list.StringVal(i));
	}
	
	pPubKernel->AddTimer("Team_Auto_Match", Domain_Team::Team_Auto_Match, pPubSpace->GetSpaceName(), GetDomainName(pPubKernel).c_str(),5000, 0);
	_this_team = this;
    return 1;
}

int Domain_Team::OnMessage(IPubKernel* pPubKernel, IPubSpace *pPubSpace,
                           int source_id, int scene_id, const IVarList& msg)
{
    IPubData *pTeamData = pPubSpace->GetPubData(GetDomainName(pPubKernel).c_str());
    if (pTeamData == NULL)
        return 1;

    int msgid = msg.IntVal(2);
    switch (msgid)
    {
    case SP_DOMAIN_MSG_TEAM_ON_RECOVER:
        {
            //当玩家重新登陆的时候验证是否在队伍中
            const int nTeamID = msg.IntVal(3);
            const wchar_t *player_name =msg.WideStrVal(4);

            //如果玩家重登入后信息有效
            if (CheckIsTeamMember(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name))
            {
				CVarList pub_msg;
				pub_msg << PUBSPACE_DOMAIN 
							   << GetDomainName(pPubKernel).c_str()
							   << PS_DOMAIN_SERVERMSG_TEAM_SET_PLAYERONRECOVER 
							   << player_name 
							   <<nTeamID;
                pPubKernel->SendPublicMessage(source_id, scene_id, pub_msg);
            }
            else
            {
				CVarList pub_msg;
				pub_msg << PUBSPACE_DOMAIN 
							   << GetDomainName(pPubKernel).c_str()
							   << PS_DOMAIN_SERVERMSG_TEAM_SET_PLAYERONRECOVER
							   << player_name 
							   <<0;
                pPubKernel->SendPublicMessage(source_id, scene_id, pub_msg);
            }
        }
        break;
    case SP_DOMAIN_MSG_TEAM_CREATE:
        //创建队伍
        {
            //队长
            const wchar_t *captain_name = msg.WideStrVal(3);
            const wchar_t *member_name = msg.WideStrVal(4);
			
            //创建队伍
            CreateTeam(pPubKernel, pTeamData, source_id, scene_id, captain_name, member_name, msg);
        }
        break;
    case SP_DOMAIN_MSG_TEAM_DESTROY:
        //解散队伍
        {
            int nTeamID = msg.IntVal(3);
            const wchar_t *player_name = msg.WideStrVal(4);
            //解散队伍
            DestroyTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name);
        }
        break;
    case SP_DOMAIN_MSG_TEAM_JOIN:
        //成员加入队伍
        {
            int nTeamID = msg.IntVal(3);
            const wchar_t *player_name = msg.WideStrVal(4);
			int clone_id = 0;
			if (msg.GetCount() >5)
			{
				clone_id = msg.IntVal(5);
			}
 
            //加入队伍
            JoinTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name, clone_id);
        }
        break;
    case SP_DOMAIN_MSG_TEAM_LEAVE:
        //成员离开队伍
        {
            int nTeamID = msg.IntVal(3);
            const wchar_t * player_name = msg.WideStrVal(4);
            int reason = msg.IntVal(5);
            //如果是掉线
            int64_t LastOffLineTime = 0;
            int needNewListen = 0;
            if(LEAVE_TYPE_OFFLINE == reason)
            {
                //取得下线的时间
                LastOffLineTime = msg.Int64Val(6);
                needNewListen = msg.IntVal(7);
            }

            //退出队伍
            QuitTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name, reason, LastOffLineTime, needNewListen);
        }
        break;
    case SP_DOMAIN_MSG_TEAM_UPDATE_MEMBER:
        //更新队伍成员信息
        {
            int nTeamID = msg.IntVal(3);
            const wchar_t * player_name = msg.WideStrVal(4);
			LoopBeginCheck(b);
            for (int i = 0; i < ((int)msg.GetCount() - 5) / 2; ++i)
            {
				LoopDoCheck(b);
                //成员属性
                int col = msg.IntVal(i * 2 + 5);
                CVar value;
                switch(msg.GetType(i * 2 + 6))
                {
                case VTYPE_INT:
                    value.SetInt(msg.IntVal(i * 2 + 6));
                    break;
                case VTYPE_INT64:
                    value.SetInt64(msg.Int64Val(i * 2 + 6));
                    break;
                case VTYPE_FLOAT:
                    value.SetFloat(msg.FloatVal(i * 2 + 6));
                    break;
                case VTYPE_DOUBLE:
                    value.SetDouble(msg.DoubleVal(i * 2 + 6));
                    break;
                case VTYPE_STRING:
                    value.SetString(msg.StringVal(i * 2 + 6));
                    break;
                case VTYPE_WIDESTR:
                    value.SetWideStr(msg.WideStrVal(i * 2 + 6));
                    break;
                }

                //更新成员信息
                UpdataMemberInfo(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name, col, value);
            }
        }
        break;
    case SP_DOMAIN_MSG_TEAM_UPDATE_INFO:
        //更新队伍信息
        {
            int nTeamID = msg.IntVal(3);
            const wchar_t *player_name = msg.WideStrVal(4);
			LoopBeginCheck(c);
            for (int i = 0; i < ((int)msg.GetCount() - 5) / 2; i++)
            {
				LoopDoCheck(c);
                //成员属性
                int col = msg.IntVal(i * 2 + 5);
                CVar value;
                switch(msg.GetType(i * 2 + 6))
                {
                case VTYPE_INT:
                    value.SetInt(msg.IntVal(i * 2 + 6));
                    break;
                case VTYPE_INT64:
                    value.SetInt64(msg.Int64Val(i * 2 + 6));
                    break;
                case VTYPE_FLOAT:
                    value.SetFloat(msg.FloatVal(i * 2 + 6));
                    break;
                case VTYPE_DOUBLE:
                    value.SetDouble(msg.DoubleVal(i * 2 + 6));
                    break;
                case VTYPE_STRING:
                    value.SetString(msg.StringVal(i * 2 + 6));
                    break;
                case VTYPE_WIDESTR:
                    value.SetWideStr(msg.WideStrVal(i * 2 + 6));
                    break;
                }

                //更新队伍信息
                UpdataTeamInfo(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name, col, value);
            }
        }
        break;
	// 设置成员位置
    case SP_DOMAIN_MSG_TEAM_SET_MEMBER:
        {
            int nTeamID = msg.IntVal(3);
            int col = msg.IntVal(4);
            int value = msg.IntVal(5);
            const wchar_t* name = msg.WideStrVal(6);
            SetTeamMember(pPubKernel, pTeamData, source_id, scene_id, nTeamID, col, value, name);
        }
        break;
	//加入匹配列表
	case SP_DOMAIN_MSG_TEAM_ADD_AUTO_MATCH_LIST:
	{
			int type = msg.IntVal(3);
			if (type == TEAM_AUTO_MATCH_ON)
			{
				PlayerAddMatch(pPubKernel,msg);
			}
			else
			{
				PlayerLeaveMatch(pPubKernel, msg);
			}


		/*	int nTeamID = msg.IntVal(3);
            int col = msg.IntVal(4);
            int value = msg.IntVal(5);
            const wchar_t* name = msg.WideStrVal(6);
			UpdataTeamInfo(pPubKernel, pTeamData, source_id, scene_id, nTeamID, name, col, CVar(VTYPE_INT, value));	*/

	}break;
	//获取匹配中队伍信息
	case SP_DOMAIN_MSG_TEAM_MATCH_INFO:
	{
		const wchar_t *player_name = msg.WideStrVal(3);
		SendMatchTeamInfo(pPubKernel, pTeamData, player_name);

	}break;
	case SP_DOMAIN_MSG_TEAM_PLAYER_STORE:
	{
		const wchar_t *player_name = msg.WideStrVal(3);
		OnStore(pPubKernel, pTeamData, source_id, scene_id, player_name);
	}break;
    default:
        break;
    }
    return 1;
}

//创建队伍，返回队伍号，-1失败
int Domain_Team::CreateTeam(IPubKernel* pPubKernel,IPubData *pTeamData, int source_id, int scene_id,
                            const wchar_t *captain_name, const wchar_t *member_name, const IVarList &args)
{
    //队伍主表是否存在
    if (!pTeamData->FindRecord(TEAM_MAIN_REC_NAME))
    {
        CVarList cols;
		cols << VTYPE_INT          //队伍编号
			<< VTYPE_STRING     //队伍成员表名称
			<< VTYPE_WIDESTR    //队长名
			<< VTYPE_WIDESTR    //队伍密码
			<< VTYPE_STRING     //队伍唯一ID
			<< VTYPE_WIDESTR	   //队伍信息
			<< VTYPE_INT		   //队伍状态
			<< VTYPE_STRING	   //队伍挖宝信息
			<< VTYPE_INT     //队伍公开状态
			<< VTYPE_INT	// 自动加入队伍
			<< VTYPE_INT		 //队伍类型
			<< VTYPE_INT        // 自动匹配
			<< VTYPE_INT		//最低等级
			<< VTYPE_INT		//最高等级
			<< VTYPE_INT;      //禁止加入队伍

        Assert(cols.GetCount() == TEAM_MAIN_REC_COL_COUNT);
        pTeamData->AddRecord(TEAM_MAIN_REC_NAME, 0, (int)cols.GetCount(), cols);
    }

	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec)
	{
		return -1;
	}

	{
		int row  = pMainRec->FindWideStr(TEAM_MAIN_REC_COL_TEAM_CAPTAIN, captain_name);
		if (row >= 0)
		{
			return 0;
		}
	}

    //获得新的队伍号
    int newTeamID = GetTeamID();
    if (newTeamID < 0)
        return -1;

    //获得队伍的唯一ID
    std::string strUID = GetUniqueTeamID();

    //获得队伍成员表名称
    std::string team_member_rec_name = GetTeamMemberRecName(newTeamID);

	//是否是公开队伍
	int viewState = TEAM_PUBLIC;
	/*if (wcslen(member_name) > 0)
	{
		viewState = TEAM_PRIVATE;
	}*/

	// 如果玩家在副本里组队
	int clone_id = 0;
	if (args.GetCount() > 5)
	{
		clone_id = args.IntVal(5);
	}




    //添加内容
    CVarList team_values;
    //自由组队创建队伍
	team_values << newTeamID
		<< team_member_rec_name.c_str()
		<< captain_name
		<< L""
		<< strUID.c_str()
		<< L""
		<< 0
		<< ""
		<< viewState
		<< TEAM_AUTO_JOIN_OPEN
		<< clone_id
		<< TEAM_AUTO_MATCH_OFF
		<< 1
		<< 60
		<< 0;

    int row = pMainRec->AddRowValue(-1, team_values);
    if (row < 0)
	{
        return -1;
	}

    if (!pTeamData->FindRecord(team_member_rec_name.c_str()))
    {
        //创建成员表
        CVarList cols;
		cols << VTYPE_STRING		//UID
			<< VTYPE_WIDESTR	//成员名称
			<< VTYPE_INT			//等级
			<< VTYPE_INT			//职业
			<< VTYPE_INT			//性别
			<< VTYPE_INT			//战斗力
			<< VTYPE_INT64		//离线情况
			<< VTYPE_INT			//成员职位      
			<< VTYPE_INT		//所在场景
			<< VTYPE_STRING		//BUFFERS              
			<< VTYPE_INT64         //当前生命值
			<< VTYPE_WIDESTR         // 公会名
			<< VTYPE_INT64         //最大生命值
			<< VTYPE_INT         //	跟随状态
			<< VTYPE_INT        //准备状态
			<< VTYPE_INT       //副本ID
			<< VTYPE_INT     //Online
			<< VTYPE_FLOAT	// x坐标
			<< VTYPE_FLOAT	// z坐标
			<< VTYPE_INT	//能否被踢出队伍
			<< VTYPE_INT	//战斗状态
			<< VTYPE_INT;	// vip
        int max_row = MAX_MEMBER_IN_NORMALTEAM;
        if (!pTeamData->AddRecord(team_member_rec_name.c_str(), max_row, (int)cols.GetCount(), cols))
        {
            //创建成员表失败，删除队伍主表中的内容
            pMainRec->RemoveRow(row);
            return -1;
        }
    }

	// 队伍成员表
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return -1;
	}

	

    //成员表中加入队长信息
    CVarList member_values;
    //默认是分配队长
    member_values << "" << captain_name 
							<< 0 << 0 << 0<< 0 
							<< int64_t(0) <<TYPE_TEAM_CAPTAIN 
							<< 0 << "" 
							<< int64_t(0)/*captian_hp*/ << L""
							<< int64_t(0)  << 0 << 0
							 <<clone_id<< ONLINE
							<< 0.0f << 0.0f
							<<TEAM_IS_KICK_OFF
							<<0<<0;

    row = pTeamMemRec->AddRowValue(-1, member_values);
    if (row < 0)
    {
        pTeamData->RemoveRecord(team_member_rec_name.c_str());
        pMainRec->RemoveRow(row);
        return -1;
    }

    //成员表中加入成员信息
    if (wcslen(member_name) > 0)
    {    
        member_values.Clear();

		member_values <<"" << member_name 
								<< 0 << 0<< 0 << 0 
								<< int64_t(0) <<TYPE_TEAM_PLAYER 
								<< 0 << "" 
								<< int64_t(0)/*captian_hp*/ << L"" 
								<< int64_t(0)  << 0 << 0 
								<< clone_id<< ONLINE
								<< 0.0f << 0.0f
								<< TEAM_IS_KICK_OFF
								<<0<<0;

        row = pTeamMemRec->AddRowValue(-1, member_values);
        if (row < 0)
        {
            pTeamData->RemoveRecord(team_member_rec_name.c_str());
            pMainRec->RemoveRow(row);
            return -1;
        }
    }

    //返回结果
	CVarList pub_msg;
	pub_msg << PUBSPACE_DOMAIN 
				   << GetDomainName(pPubKernel).c_str() 
				   << PS_DOMAIN_SERVERMSG_TEAM_CREATE 
				   << captain_name << member_name << newTeamID;
    pPubKernel->SendPublicMessage(source_id, scene_id, pub_msg);

    return newTeamID;
}

//删除队伍
bool Domain_Team::DestroyTeam(IPubKernel* pPubKernel,IPubData *pTeamData, int source_id, int scene_id,
                              int nTeamID, const wchar_t *player_name)
{
	IRecord* pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (pMainRec == NULL)
	{
		return false;
	}

    int row = pMainRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
        //未找到此队伍
        return false;

	std::wstring captain = pMainRec->QueryWideStr(row, TEAM_MAIN_REC_COL_TEAM_CAPTAIN);
   
	if (wcscmp(captain.c_str(), player_name) != 0)
        //不是队长，不能解散
        return false;

    //删除队伍主表中的内容
    pMainRec->RemoveRow(row);

    //向所有成员返回结果
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);
	IRecord* pMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pMemRec)
	{
		return  false;
	}
    int rows = pMemRec->GetRows();
	CVarList var;
	LoopBeginCheck(d);
    for (int i = 0; i < rows; ++i)
    {
		LoopDoCheck(d);
        const wchar_t *member_name = pMemRec->QueryWideStr(i, TEAM_REC_COL_NAME);
		var.Clear();
		var  << PUBSPACE_DOMAIN 
			   << GetDomainName(pPubKernel).c_str() 
			   << PS_DOMAIN_SERVERMSG_TEAM_DESTROY 
			   << member_name;
        pPubKernel->SendPublicMessage(source_id, scene_id, var);
    }

    //删除成员表
    pTeamData->RemoveRecord(team_member_rec_name.c_str());

    return true;
}

//加入队伍
bool Domain_Team::JoinTeam(IPubKernel* pPubKernel,IPubData *pTeamData, int source_id, int scene_id,
                           int nTeamID, const wchar_t *player_name, int clone_id)
{
	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec)
	{
		return false;
	}
    int row = pMainRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
	{
        //未找到此队伍
        return false;
	}

	// 成员表
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return false;
	}
	//加入者
    int row_col = pTeamMemRec->FindWideStr(TEAM_REC_COL_NAME, player_name);
    if (row_col >= 0)
	{
        //已在队伍中
        return false;
	}
    
    CVarList values;
	values << "" << player_name
		<< 0 << 0 << 0 << 0 << 0
		<< int64_t(0) << TYPE_TEAM_PLAYER
		<< "" << 0 << ""
		<< int64_t(0)/*captian_hp*/ << L""
		<< int64_t(0) << 0 << 0
		<< clone_id << ONLINE
		<< 0.0f << 0.0f
		<< TEAM_IS_KICK_OFF
		<< 0 //战斗状态
		<< 0;//vip

    //添加到表中
    int memberrow = pTeamMemRec->AddRowValue(-1, values);
    if (memberrow < 0)
        return false;
	CVarList var;
   
	

	
	/*var << COMMAND_TEAM_MSG
		<< PS_DOMAIN_SERVERMSG_TEAM_JOIN
		<< player_name << nTeamID;
*/

	var << PUBSPACE_DOMAIN
		<< GetDomainName(pPubKernel).c_str()
		<< PS_DOMAIN_SERVERMSG_TEAM_JOIN
		<<  player_name << nTeamID;
	pPubKernel->SendPublicMessage(source_id, scene_id, var);



	//pPubKernel->CommandByName(player_name, var);




	//向所有成员返回结果
	int rows = pTeamMemRec->GetRows();
	LoopBeginCheck(e);
    for (int i = 0; i < rows; ++i)
    {
		LoopDoCheck(e);
        const wchar_t *member_name = pTeamMemRec->QueryWideStr(i, TEAM_REC_COL_NAME);

		var.Clear();
		var << PUBSPACE_DOMAIN 
			  << GetDomainName(pPubKernel).c_str() 
			  << PS_DOMAIN_SERVERMSG_TEAM_ADD_MEMBER
			  << member_name << player_name << nTeamID;
        pPubKernel->SendPublicMessage(source_id, scene_id,var);
    }
		
	int matchState = pMainRec->QueryInt(row, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH);
	if (matchState == TEAM_AUTO_MATCH_ON)
	{//如果当前队伍正在匹配中，发送匹配已满消息
		if (rows >= pTeamMemRec->GetRowMax())
		{
			//匹配已满关闭自动匹配
			pMainRec->SetInt(row, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH, TEAM_AUTO_MATCH_OFF);
		}
	}
    return true;
}


bool Domain_Team::JoinTeam(IPubKernel *pPubKernel, IRecord *pMainRec, IPubData * pTeamData, const wchar_t * playerName, int teamRow)
{
	if (pPubKernel == NULL||pMainRec == NULL || pTeamData == NULL || playerName == NULL)
	{
		return false;
	}

	int nTeamID = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_ID);
	std::string team_member_rec_name = pMainRec->QueryString(teamRow, TEAM_MAIN_REC_COL_TEAM_MEMBER_REC_NAME);
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return false;
	}


	CVarList var;

	//加入者
	int row_col = pTeamMemRec->FindWideStr(TEAM_REC_COL_NAME, playerName);
	if (row_col >= 0)
	{
		return false;
	}
	CVarList values;
	values << "" << playerName
		<< 0 << 0 << 0 << 0 << 0
		<< int64_t(0) << TYPE_TEAM_PLAYER
		<< "" << 0 << ""
		<< int64_t(0)/*captian_hp*/ << L""
		<< int64_t(0) << 0 << 0
		<< 0 << ONLINE
		<< 0.0f << 0.0f
		<< TEAM_IS_KICK_OFF
		<< 0//战斗状态;
		<< 0;
	//添加到表中
	int memberrow = pTeamMemRec->AddRowValue(-1, values);
	if (memberrow < 0){
		return false;
	}

		var.Clear();
		var << COMMAND_TEAM_MSG
			<< COMMAND_TEAM_TEAM_JOIN
		    << playerName << nTeamID;
		pPubKernel->CommandByName(playerName, var);



	//向所有成员返回结果
	int rows = pTeamMemRec->GetRows();
	LoopBeginCheck(e);
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(e);
		const wchar_t *member_name = pTeamMemRec->QueryWideStr(i, TEAM_REC_COL_NAME);
		var.Clear();
		var << COMMAND_TEAM_MSG
			<< COMMAND_TEAM_TEAM_ADD_MUMBER
			<< member_name << playerName << nTeamID;
		pPubKernel->CommandByName(playerName, var);
	}

	if (rows >= pTeamMemRec->GetRowMax()){
		//匹配已满关闭自动匹配
		pMainRec->SetInt(teamRow, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH, TEAM_AUTO_MATCH_OFF);
	}
	return true;

}

//更新成员属性
bool Domain_Team::UpdataMemberInfo(IPubKernel* pPubKernel,IPubData *pTeamData, int source_id, int scene_id, 
                                   int nTeamID, const wchar_t *player_name, int col, const IVar & value)
{
	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec)
	{
		return false;
	}
    int row = pMainRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
        //未找到此队伍
        return false;

	// 成员表
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return false;
	}
    row = pTeamMemRec->FindWideStr(TEAM_REC_COL_NAME, player_name);
    if (row < 0)
	{
        //不在队伍中
        return false;
	}

    //第1列不允许修改
    if (col == 1 || col >= pTeamMemRec->GetCols())
        return false;

    if (pTeamMemRec->GetColType(col) != value.GetType())
        //数据类型不对
        return false;

    switch(pTeamMemRec->GetColType(col))
    {
    case VTYPE_INT:
        pTeamMemRec->SetInt(row, col, value.IntVal());
        break;
    case VTYPE_INT64:
        pTeamMemRec->SetInt64(row, col, value.Int64Val());
        break;
    case VTYPE_FLOAT:
        pTeamMemRec->SetFloat(row, col, value.FloatVal());
        break;
    case VTYPE_DOUBLE:
        pTeamMemRec->SetDouble(row, col, value.DoubleVal());
        break;
    case VTYPE_STRING:
        pTeamMemRec->SetString(row, col, value.StringVal());
        break;
    case VTYPE_WIDESTR:
        pTeamMemRec->SetWideStr(row, col, value.WideStrVal());
        break;
    }

    //通知所有成员
    int rows = pTeamMemRec->GetRows();
	CVarList var;
	LoopBeginCheck(f);
    for (int i = 0; i < rows; ++i)
     {
		LoopDoCheck(f);
        const wchar_t *member_name = pTeamMemRec->QueryWideStr(i, TEAM_REC_COL_NAME);
		var.Clear();
		var << PUBSPACE_DOMAIN 
			  << GetDomainName(pPubKernel).c_str() 
			  << PS_DOMAIN_SERVERMSG_TEAM_UPDATE_MEMBER 
			  << member_name << player_name << col;
        pPubKernel->SendPublicMessage(source_id, scene_id, var);
    }

    return true;
}

//更新队伍属性
bool Domain_Team::UpdataTeamInfo(IPubKernel* pPubKernel,IPubData * pTeamData, int source_id, int scene_id,
                                 int nTeamID, const wchar_t * player_name, int col, const IVar & value)
{
	IRecord* teamRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == teamRec)
    {
        return false;
    }
	int row = teamRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
	{
        //未找到此队伍
        return false;
	}

    //第0列不允许修改
	if (col <= 0 || col >= teamRec->GetCols())
	{
        return false;
	}

	if (teamRec->GetColType(col) != value.GetType())
	{
        //数据类型不对
        return false;
	}

    switch(teamRec->GetColType(col))
    {
    case VTYPE_INT:
		teamRec->SetInt(row,col,value.IntVal());
        break;
    case VTYPE_INT64:
		teamRec->SetInt64(row,col,value.Int64Val());
        break;
    case VTYPE_FLOAT:
		teamRec->SetFloat(row,col,value.FloatVal());
        break;
    case VTYPE_DOUBLE:
		teamRec->SetDouble(row,col,value.DoubleVal());
        break;
    case VTYPE_STRING:
		teamRec->SetString(row,col,value.StringVal());
        break;
    case VTYPE_WIDESTR:
		teamRec->SetWideStr(row,col,value.WideStrVal());
        break;
    }

    //通知所有成员
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);
	IRecord* memRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == memRec)
	{
		return false;
	}
	int rows = memRec->GetRows();
	CVarList var;
	LoopBeginCheck(g);
    for (int i = 0; i < rows; ++i)
    {
		LoopDoCheck(g);
		const wchar_t *member_name = memRec->QueryWideStr(i, TEAM_REC_COL_NAME);
		var.Clear();
		var << PUBSPACE_DOMAIN 
			  << GetDomainName(pPubKernel).c_str() 
			  << PS_DOMAIN_SERVERMSG_TEAM_UPDATE_INFO 
			  << member_name << col << nTeamID;
        pPubKernel->SendPublicMessage(source_id, scene_id, var);
    }	

    return true;
}

bool Domain_Team::CheckIsTeamMember(IPubKernel* pPubKernel, IPubData *pTeamData, int source_id, int scene_id,
                                    const int team_id, const wchar_t* Name)
{
	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec)
	{
		return false;
	}
    //查找是否有该队伍
    int row = pMainRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, team_id);
    if (row < 0)
	{
        //未找到此队伍
        return false;
	}

    //获得队伍成员表名称
    std::string team_member_rec_name = GetTeamMemberRecName(team_id);
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return false;
	}
    //获得队长名
    const wchar_t* captain_name = pMainRec->QueryWideStr(row, TEAM_MAIN_REC_COL_TEAM_CAPTAIN);

    //获得该人在队伍中的位置
    int member_row = pTeamMemRec->FindWideStr(TEAM_REC_COL_NAME, Name);

    if (member_row < 0)
	{
        //不在队伍中
        return false;
	}

    //查找是否有队友在线，若没有则将队长权限移交给自己
    int rows = pTeamMemRec->GetRows();
    bool isOneInTeam = false;
	LoopBeginCheck(h);
    for (int i = 0; i < rows; ++ i)
    {
		LoopDoCheck(h);
        int64_t mblastleavetime = pTeamMemRec->QueryInt64(i, TEAM_REC_COL_ISOFFLINE);
        if (mblastleavetime == 0)
        {
            isOneInTeam = true;
            break;
        }
    }

    //立即将自己设置为上线状态，防止移交队长过程中，另一队友上线，产生冲突
    UpdataMemberInfo(pPubKernel, pTeamData, source_id, scene_id, team_id, Name, 
			TEAM_REC_COL_ISOFFLINE, CVar(VTYPE_INT64, int64_t(0)));

    if (!isOneInTeam)
    {
        //更新队伍信息
        UpdataTeamInfo(pPubKernel, pTeamData, source_id, scene_id, team_id, captain_name, 
				TEAM_MAIN_REC_COL_TEAM_CAPTAIN, CVar(VTYPE_WIDESTR, Name));
    }
    
    return true;
}

//设置队友职位
bool Domain_Team::SetTeamMember(IPubKernel* pPubKernel, IPubData *pTeamData, int source_id, int scene_id,
                   int nTeamID, int nCol, int value, const wchar_t* name)
{
	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec)
	{
		return false;
	}
    int row = pMainRec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
	{
        //未找到此队伍
        return false;
	}

    //取得队员所在的队伍
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);

	IRecord* mem_rec = pTeamData->GetRecord(team_member_rec_name.c_str());

	if(NULL == mem_rec)
	{
		return false;
	}

	int member_row = mem_rec->FindWideStr(TEAM_REC_COL_NAME, name);
    if (member_row < 0)
        //不在队伍中
        return false;

    // 如果不是设置位置和职位的返回
    if (nCol == TEAM_REC_COL_TEAMWORK)
    {
		mem_rec->SetInt(member_row,nCol, value);
		int member_rows = mem_rec->GetRows();
		CVarList var;
        //通知所有人
		LoopBeginCheck(j);
        for (int i = 0; i < member_rows; ++i)
        {
			LoopDoCheck(j);
			const wchar_t *member_name = mem_rec->QueryWideStr(i, TEAM_REC_COL_NAME);
            if (wcscmp(member_name, name) != 0)
            {
				mem_rec->SetInt(i,nCol,TYPE_TEAM_PLAYER);
            }
			var.Clear();
			var << PUBSPACE_DOMAIN 
				  << GetDomainName(pPubKernel).c_str() 
				  << PS_DOMAIN_SERVERMSG_TEAM_SET_MEMBERS_WORK 
				  << member_name << name ;
            pPubKernel->SendPublicMessage(source_id, scene_id, var);
        }
    }
    
    return true;
}

void Domain_Team::PlayerAddMatch(IPubKernel* pPubKernel, const IVarList &value)
{

	const  wchar_t* name = value.WideStrVal(4);
	if (name == NULL)
	{
		return;
	}
	int level  = value.IntVal(5);
	int index = value.IntVal(6);
	Match_Player_Info playerInfo;
	playerInfo.index = index;
	playerInfo.playerLevel = level;
	playerInfo.playerName = name;

	std::list<Match_Player_Info>::iterator it = match_apply.begin();
	LoopBeginCheck(a);
	for (; it != match_apply.end(); ++it)
	{
		LoopDoCheck(a);
		if ( it->playerName.compare(name) == 0)
		{
			match_apply.erase(it);
			break;
		}
		
		
	}

	match_apply.push_back(playerInfo);
}

void Domain_Team::PlayerLeaveMatch(IPubKernel* pPubKernel, const IVarList &value)
{
	const  wchar_t* name = value.WideStrVal(4);
	if (name == NULL)
	{
		return;
	}

	std::list<Match_Player_Info>::iterator it = match_apply.begin();
	LoopBeginCheck(a);
	for (; it != match_apply.end();)
	{
		LoopDoCheck(a);
		if (it->playerName.compare(name) == 0)
		{
			it = match_apply.erase(it);
		}
		else
		{
			++it;
		}
	}


}

int Domain_Team::Team_Auto_Match(IPubKernel *pPubKernel, const char *space_name, const wchar_t *data_name, int time)
{
	if (match_apply.empty()){
		return 0;
	}

	IPubSpace* pPubSpace = pPubKernel->GetPubSpace(space_name);
	if (pPubSpace == NULL){
		return 0 ;
	}

	IPubData *pTeamData = pPubSpace->GetPubData(_this_team->GetDomainName(pPubKernel).c_str());
	if (pTeamData == NULL){
		return 0;
	}
	
	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec){
		return 0;
	}

	int teamRowMax = pMainRec->GetRows();
	LoopBeginCheck(a);
	for (int teamRow = 0; teamRow < teamRowMax; teamRow++)
	{
		LoopDoCheck(a);
		int autoMatch = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH);
		if (autoMatch == TEAM_AUTO_MATCH_ON)
		{
			int minLevel = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_LIMIT_MIN_LEVEL);
			int maxLevel = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_LIMIT_MAX_LEVEL);
			int index = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_OBJECT_INDEX);

			std::list<Match_Player_Info>::iterator it = match_apply.begin();
			LoopBeginCheck(b);
			for (; it != match_apply.end();)
			{
				LoopDoCheck(b);
				if (it->index == index)
				{
					if (it->playerLevel >= minLevel && it->playerLevel <= maxLevel)
					{
						const wchar_t*playerName = it->playerName.c_str();
						if (_this_team->JoinTeam(pPubKernel,pMainRec, pTeamData, playerName, teamRow))
						{
							it = match_apply.erase(it);
							int matchState = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH);
							if (matchState == TEAM_AUTO_MATCH_OFF)
							{
								break;
							}
							continue;
						}
					}
				}
				++it;
			}
		}
	}
	return 0;
}



void Domain_Team::SendMatchTeamInfo(IPubKernel* pPubKernel, IPubData * pTeamData, const wchar_t*name)
{

	IRecord *pMainRec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == pMainRec){
		return ;
	}
	// 自动匹配中的队伍数量
	int autoMatchTeamNum = 0;
	int teamRowMax = pMainRec->GetRows();
	LoopBeginCheck(a);
	for (int teamRow = 0; teamRow < teamRowMax; teamRow++)
	{
		LoopDoCheck(a);
		int autoMatch = pMainRec->QueryInt(teamRow, TEAM_MAIN_REC_COL_TEAM_AUTO_MATCH);
		if (autoMatch == TEAM_AUTO_MATCH_ON)
		{
			autoMatchTeamNum++;
		}
	}

	int waitNumber = match_apply.size();

	CVarList msg;
	msg << SERVER_CUSTOMMSG_TEAM
		<< CLIENT_CUSTOMMSG_TEAM_MATCH_TEAM_INFO
		<< autoMatchTeamNum
		<< waitNumber;
	pPubKernel->CustomByName(name, msg);

}

//分配新的队伍号，返回-1失败
int Domain_Team::GetTeamID()
{
    static int NewTeamNo = 1;
    int nTeamID = NewTeamNo++;

    if (NewTeamNo <= 0)
        NewTeamNo = 1;

    return nTeamID;
}

//分配队伍的唯一ID
std::string Domain_Team::GetUniqueTeamID()
{
    return util_gen_unique_name();
}
//获得队伍成员表名称
std::string Domain_Team::GetTeamMemberRecName(int nTeamID)
{
    char team_member_rec_name[256];
    SPRINTF_S(team_member_rec_name, "team_%d_member_rec", nTeamID);
    return team_member_rec_name;
}

//退出队伍(reason: 0自己离开，1被队长开除，2玩家下线)
bool Domain_Team::QuitTeam(IPubKernel* pPubKernel, IPubData *pTeamData, int source_id, int scene_id,
                            int nTeamID, const wchar_t *player_name, int reason , int64_t lastleavetime, int needNewListen)
{
	IRecord* team_rec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if(NULL == team_rec)
	{
		return false;
	}

	int row = team_rec->FindInt(TEAM_MAIN_REC_COL_TEAM_ID, nTeamID);
    if (row < 0)
        //未找到此队伍
        return false;

	const wchar_t *captain = team_rec->QueryWideStr(row, TEAM_MAIN_REC_COL_TEAM_CAPTAIN);
    std::string team_member_rec_name = GetTeamMemberRecName(nTeamID);

	IRecord* mem_rec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if(NULL == mem_rec)
	{
		return false;
	}

	int member_row = mem_rec->FindWideStr(TEAM_REC_COL_NAME, player_name);
    if (member_row < 0)
        //不在队伍中
        return false;
	//取出该人的身份
	int team_work = mem_rec->QueryInt(member_row, TEAM_REC_COL_TEAMWORK);
    //通知所有成员
	int rows = mem_rec->GetRows(); 
	CVarList var;

    //如果是下线
    if(reason == LEAVE_TYPE_OFFLINE)
    {
        // 设置该团员离线
		mem_rec->SetInt64(member_row, TEAM_REC_COL_ISOFFLINE, lastleavetime);
		mem_rec->SetInt(member_row, TEAM_REC_COL_TEAMWORK, TYPE_TEAM_PLAYER);
        int count = 0;
		LoopBeginCheck(k);
        for (int i = 0; i < rows; ++i)
        {
			LoopDoCheck(k);
			int64_t mblastleavetime = mem_rec->QueryInt64(i, TEAM_REC_COL_ISOFFLINE);

            //只通知在线的人
            if (mblastleavetime == 0)
            {
				const wchar_t *member_name =mem_rec->QueryWideStr(i, TEAM_REC_COL_NAME);
				var.Clear();
				var << PUBSPACE_DOMAIN 
					  << GetDomainName(pPubKernel).c_str() 
					  << PS_DOMAIN_SERVERMSG_TEAM_UPDATE_MEMBER 
					  << member_name 
					  << player_name 
					  << TEAM_REC_COL_ISOFFLINE;
                pPubKernel->SendPublicMessage(source_id, scene_id, var);
                count++;
            }
        }

        if (count == 0)
        {
			if (DestroyTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, captain))
			{
				return true;
			}
        }
        //不保留记录
       // return true;
    }
	else
	{
		//成员表中删除自已
		mem_rec->RemoveRow(member_row);

		rows = mem_rec->GetRows();

		//通知所有成员有队员离开
		LoopBeginCheck(l);
		for (int i = 0; i < rows; ++i)
		{
			LoopDoCheck(l);
			const wchar_t * member_name = mem_rec->QueryWideStr(i, TEAM_REC_COL_NAME);
			var.Clear();
			var << PUBSPACE_DOMAIN
				<< GetDomainName(pPubKernel).c_str()
				<< PS_DOMAIN_SERVERMSG_TEAM_LEAVE
				<< member_name << player_name << reason;
			pPubKernel->SendPublicMessage(source_id, scene_id, var);
		}
	}

   

   

	

    if (rows > 0)
    {	
		int next_leader_row = -1;
        //如果离开的是队长，设置下一成员为队长，并设置其职位为队长
        if (team_work == TYPE_TEAM_CAPTAIN)
        {
			LoopBeginCheck(m);
            for (int i = 0; i < rows; ++i)
            {
				LoopDoCheck(m);
				int64_t mblastleavetime = mem_rec->QueryInt64(i, TEAM_REC_COL_ISOFFLINE);

                //掉线的人无权得到队长职位
                if (mblastleavetime > 0)
                    continue;
				
				mem_rec->SetInt(i,TEAM_REC_COL_TEAMWORK,team_work);
                next_leader_row = i;
                break;
            }

            if (next_leader_row >= 0)//找到下任队长
            {
				const wchar_t *next_leader_name = mem_rec->QueryWideStr(next_leader_row, TEAM_REC_COL_NAME);
                
                //设置新队长职位
				mem_rec->SetInt(next_leader_row, TEAM_REC_COL_TEAMWORK, team_work);

                //更新队伍队长
                UpdataTeamInfo(pPubKernel, pTeamData, source_id, scene_id, nTeamID, player_name,
						TEAM_MAIN_REC_COL_TEAM_CAPTAIN, CVar(VTYPE_WIDESTR, next_leader_name));

                //通知所有人
				LoopBeginCheck(n);
                for (int i = 0; i < rows; ++i)
                {
					LoopDoCheck(n);
					const wchar_t *member_name = mem_rec->QueryWideStr(i, TEAM_REC_COL_NAME);
					var.Clear();
					var << PUBSPACE_DOMAIN 
						  << GetDomainName(pPubKernel).c_str() 
						  << PS_DOMAIN_SERVERMSG_TEAM_UPDATE_MEMBER 
						  << member_name << next_leader_name 
						  << TEAM_REC_COL_TEAMWORK;
                    pPubKernel->SendPublicMessage(source_id, scene_id, var);
                }
            }
			else
			{
				//没找到下任队长，设置队伍解散标记
				next_leader_row = -2;
			}
        }        

		//如果队长离队，并且没找到下任队长，就解散队伍
		if (next_leader_row == -2)
		{
			DestroyTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, captain);
		}
    }
    else
    {
        //队伍人数为0，则删除队伍信息
        DestroyTeam(pPubKernel, pTeamData, source_id, scene_id, nTeamID, captain);
    }

	var.Clear();
	var << PUBSPACE_DOMAIN 
		  << GetDomainName(pPubKernel).c_str() 
		  << PS_DOMAIN_SERVERMSG_TEAM_LEAVE 
		  << player_name << player_name << reason;
    //通知自已
    pPubKernel->SendPublicMessage(source_id, scene_id, var);

    return true;
}


bool Domain_Team::OnStore(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, const wchar_t * player_name)
{
	IRecord* team_rec = pTeamData->GetRecord(TEAM_MAIN_REC_NAME);
	if (NULL == team_rec)
	{
		return false;
	}

	std::vector<int> errorTeamGroup;
	for (int i = 0; i < team_rec->GetRows(); i++)
	{
		const wchar_t* capName = team_rec->QueryWideStr(i, TEAM_MAIN_REC_COL_TEAM_CAPTAIN);
		if (wcscmp(player_name, capName) != 0){
			continue;
		}
		//异常,玩家下线了，应该不是队长了，可是还有队伍，直接解散队伍
		int nTeamID = team_rec->QueryInt(i, TEAM_MAIN_REC_COL_TEAM_ID);
		errorTeamGroup.push_back(nTeamID);
	}

	for (auto it : errorTeamGroup)
	{
		DestroyTeam(pPubKernel, pTeamData, source_id, scene_id, it, player_name);
	}

	return true;

}

// 取得指定队伍ID的成员列表
void Domain_Team::GetTeamMemsByTeamID(IPubKernel* pPubKernel, IPubSpace * pPubSpace, 
	const int team_id, IVarList& mem_list)
{
	// 公共数据指针
	IPubData *pTeamData = pPubSpace->GetPubData(_this_team->GetDomainName(pPubKernel).c_str());
	if (pTeamData == NULL)
	{
		return;
	}

	// 取得队伍成员表名
	std::string team_member_rec_name = _this_team->GetTeamMemberRecName(team_id);

	// 遍历队伍成员表
	IRecord *pTeamMemRec = pTeamData->GetRecord(team_member_rec_name.c_str());
	if (NULL == pTeamMemRec)
	{
		return;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < pTeamMemRec->GetRows(); ++i)
	{
		LoopDoCheck(a);
		const wchar_t* member_name = pTeamMemRec->QueryWideStr(i, TEAM_REC_COL_NAME);

		mem_list << member_name;
	}
}

const std::wstring & Domain_Team::GetDomainName(IPubKernel *pPubKernel)
{
	if (m_domainName.empty())
	{
		wchar_t wstr[256];
		int server_id = pPubKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Team_%d", server_id);
		m_domainName = wstr;
	}

	return m_domainName;
}