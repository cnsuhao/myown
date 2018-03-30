//--------------------------------------------------------------------
// 文件名:		Domain_Team.h
// 内 容:		    队伍管理系统
// 说 明:		
// 创建日期:	2014年11月19日
// 创建人:		 
//    :	    
//--------------------------------------------------------------------

#ifndef _Domain_Team_h_
#define _Domain_Team_h_

#include "IDomainData.h"
#include "public/IVarList.h"
#include "public/IVar.h"
#include <string>
#include <list>
class Domain_Team : public IDomainData
{
public:
    Domain_Team() : IDomainData(L"")
    {
    }

    virtual ~Domain_Team()
    {
    }

public:
    //是否保存到数据库
    virtual bool GetSave(){return false;}

	// 公共数据区名
	virtual const std::wstring & GetDomainName(IPubKernel* pPubKernel);

    virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace * pPubSpace);

    virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace * pPubSpace);

    /// \brief 接收到场景服务器的消息
    /// \param source_id 发消息的服务器标识
    /// \param msg 消息内容
    virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

	// 取得指定队伍ID的成员列表
	static void GetTeamMemsByTeamID(IPubKernel* pPubKernel, IPubSpace * pPubSpace,
		const int team_id, IVarList& mem_list);

private:
    //创建队伍, 返回队伍号，-1失败
    int CreateTeam(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        const wchar_t * captain_name, const wchar_t * member_name,const IVarList &args);

    //删除队伍
    bool DestroyTeam(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        int nTeamID, const wchar_t * player_name);

    //加入队伍
    bool JoinTeam(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        int nTeamID, const wchar_t * player_name,int clone_id);
	bool JoinTeam(IPubKernel *pPubKernel,IRecord *pMainRec, IPubData * pTeamData, const wchar_t * playerName, int teamRow);
    //退出队伍(reason: 0自己离开，1被队长开除 2下线)
    bool QuitTeam(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        int nTeamID, const wchar_t * player_name, int reason,int64_t lastleavetime = 0,int needNewListen = 0);
	//玩家下线
	bool OnStore(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, const wchar_t * player_name);

    //更新成员属性
    bool UpdataMemberInfo(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        int nTeamID, const wchar_t * player_name, int col, const IVar & value);

    //更新队伍属性
    bool UpdataTeamInfo(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id, 
        int nTeamID, const wchar_t * player_name, int col, const IVar & value);

    //判断是否是队伍中的玩家
    bool CheckIsTeamMember(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id,
        const int team_id,const wchar_t* player_name);

    //设置队友职位
    bool SetTeamMember(IPubKernel* pPubKernel, IPubData * pTeamData, int source_id, int scene_id,
        int nTeamID,int nCol, int value, const wchar_t* name);
	//玩家加入匹配
	void PlayerAddMatch(IPubKernel* pPubKernel, const IVarList &value);
	//玩家离开匹配
	void PlayerLeaveMatch(IPubKernel* pPubKernel, const IVarList &value);
	//自动匹配
	 static int  Team_Auto_Match(IPubKernel *pPubKernel, const char *space_name, const wchar_t *data_name, int time);
	 //发送匹配中的队伍信息
	 void SendMatchTeamInfo(IPubKernel* pPubKernel, IPubData * pTeamData,const wchar_t*name);

private:
    //分配新的队伍号，返回-1失败
    int GetTeamID();

    //分配队伍的唯一ID
    std::string GetUniqueTeamID();

    //获得队伍成员表名称
    std::string GetTeamMemberRecName(int nTeamID);
	struct Match_Player_Info
	{
		std::wstring playerName;
		int playerLevel;
		int index; // 目标
	};
	static Domain_Team* _this_team;
	static std::list<Match_Player_Info> match_apply;
	

};

#endif //_Domain_Team_h_