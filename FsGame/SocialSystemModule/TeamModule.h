//--------------------------------------------------------------------
// 文件名:		TeamModule.h
// 内 容:		    组队模块
// 说 明:		
// 创建日期:	2014年11月21日
// 创建人:		 
//    :	    
//--------------------------------------------------------------------

#ifndef _TEAM_MODULE_H_
#define _TEAM_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/RequestDefine.h"
#include "FsGame/Define/TeamDefine.h"
#include "FsGame/SocialSystemModule/FriendModule.h"
#include <map>
#include <string>
#include <vector>
#include <set>

class RequestModule;
class CloneSceneModule;
class CloneSceneInfoWrap;
class CBattleWarModule;

// 附加玩家距离
#define TEAM_NEARBY_RANGE	20.0f


// 队伍模块
class TeamModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

public:

    // 自己是否在队伍中
    virtual bool IsInTeam(IKernel* pKernel, const PERSISTID& self);

    // 自己是否队长
    virtual bool IsTeamCaptain(IKernel* pKernel, const PERSISTID& self);

    // 是否有拾取物品的权利
    virtual bool HaveRightPickUp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target_obj , const wchar_t* member_name);

    // 获得队友名称列表
    virtual bool GetTeamMemberList(IKernel* pKernel, const PERSISTID& self, IVarList& lst);

	// 获得队员名称列表
	virtual void GetTeamMemberList(IKernel* pKernel, const int team_id, IVarList& lst);

	// 获取用来更新附近队伍表的信息
	virtual bool GetNearByTeamInfo(IKernel* pKernel, PERSISTID playID, IVarList& lst);

    // 获得队长名
    virtual const wchar_t* GetCaptain(IKernel* pKernel, const PERSISTID& self);

    // 获取组队公共数据区指针
    virtual IPubData* GetTeamPubData(IKernel* pKernel);

	// 更新副本ID到公共数据服务器
	void UpdateCloneToDomain(IKernel* pKernel, const PERSISTID& self, int clone_id);

	// 添加队伍成员属性回调
	void AddCriticalCallBack(IKernel* pKernel, const PERSISTID& self);

	// 清除队伍成员属性回调
	void RemoveCriticalCallBack(IKernel* pKernel, const PERSISTID& self);

	//任务激活功能
	static int OnCommandTask(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 获取domain的名字
	static const std::wstring& GetDomainName(IKernel * pKernel);

	// 对方是否自己队友
	static bool IsTeamMate(IKernel* pKernel, const PERSISTID& self, const wchar_t* target_name);

	//能否被踢出队伍
	static bool IsBeKickTeam(IKernel* pKernel, const PERSISTID& self, const wchar_t* target_name);
	// 队伍是否已满
	static bool IsFull(IKernel* pKernel, const PERSISTID& obj);
	//是否是自动加入队伍
	static bool IsAutoJoin(IKernel*pKernel, int teamID);
	//达到队伍等级
	bool IsAchieveTeamLevel(IKernel*pKernel, const PERSISTID& self,int teamID);

	//设置当前玩家能否被踢
	void SetTeamMemKick(IKernel*pKernel, const PERSISTID& self,int kickState);

	//获取队伍目标类型
	static int GetTeamObjType(IKernel*pKernel, int teamID);
	//根据类型获得id
	int GetTeamIndexByType(int type);
	//根据objID 获取type
	int GetTeamIndexByObjectID(int objID);
	//自动更改队伍目标
	void AutoChangeTeamIndex(IKernel*pKernel, const PERSISTID& self,int index);
	//设置是否能加入队伍TEAM_JOIN_TEAM_ON, //能够加入队伍 TEAM_JOIN_TEAM_OFF, //不能够加入队伍
	void SetJoinTeamState(IKernel* pKernel, const PERSISTID& self, int state);

	// 更新准备状态
	void UpdateReadyState(IKernel* pKernel, const PERSISTID& self, int ready_state);

	void CustomMessageToTeamMember(IKernel*pKernel, const PERSISTID& self, const IVarList&msg,bool customSelf = false);
	//是否是活动场景
	bool IsActivityScene(IKernel*pKernel);
private:

    // 玩家上线
    static int OnRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 离开游戏
	static int OnStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 玩家准备
    static int OnReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 顶号
	static int OnContinue(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 公共数据服务器下传的消息
    static int OnPublicMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 处理组队请求
    static bool CanRequestJoinTeam(IKernel* pKernel, const PERSISTID& self, REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);

    static bool CanBeRequestJoinTeam(IKernel* pKernel, const PERSISTID& self, REQUESTTYPE type, const wchar_t* srcname, const IVarList& paras);

    static bool RequestJoinTeamSuccess(IKernel* pKernel, const PERSISTID& self, REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);

    static bool GetRequestParas(IKernel* pKernel, const PERSISTID& self, REQUESTTYPE type, IVarList& paras);

    static bool RequestJoinTeamResult(IKernel* pKernel, const PERSISTID& self, REQUESTTYPE type, const wchar_t* targetname, int result);

    // 组队客户端消息
    static int OnCustomMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 组队服务端消息
	static int OnCommandMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 创建队伍
    static int OnCustomCreateTeam(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 踢出队伍
    static int OnCustomKickOut(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 退队
    static int OnCustomLeave(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //解散队伍
    static int OnCustomDestroy(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 移交队长
    static int OnCustomChangeCaptain(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 附近队伍
	static int OnCustomNearByTeam(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 更新队员位置
	static int OnCustomUpdateMemberPosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 距离比较
	static bool CompareDistance(const NearbyTeamRecommend& first, const NearbyTeamRecommend& second);

    //清除入队申请
    static int OnCustomClearApply(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    //获取申请列表
    static int OnCustomApplyList(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//设置队伍公开属性
	static int OnCustomSetViewState(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//设置队伍自动加入属性
	static int OnCustomSetAutoJoin(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//获取队伍信息
	static int OnCustomGetTeamInfo(IKernel* pKernel, const PERSISTID& self);
	//设置队伍目标
	static int OnCustomSetTeamObject(IKernel* pKernel, const PERSISTID& self, int index);
	//设置队伍等级限制
	static int OnCustomSetTeamLimitLevel(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	//发送队伍列表
	static void OnCustomSendTeamList(IKernel*pKernel, const PERSISTID& self, int objID);
	//当前队伍正在匹配状态
	bool IsAutoMatch(IKernel*pKernel, const PERSISTID& self);
	//去队长附近
	static void OnCustomTransferCapTainSite(IKernel*pKernel, const PERSISTID& self);
	//召集
	static void OnCustomConvene(IKernel*pKernel, const PERSISTID& self);
	//设置跟随
	static void OnCustomSetFollow(IKernel*pKernel, const PERSISTID& self,int state);
	//获取推荐组队列表
	static void OnCustomRequestRecommandList(IKernel*pKernel, const PERSISTID& self);
	
private:
    // 队伍成员上线
    static int OnCommandTeamMemberRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 队伍成员变化
    static int OnCommandTeamMemberChange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 队伍创建结果
    static int OnCommandCreateTeamResult(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 加入队伍
    static int OnCommandJoinTeam(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 添加成员
    static int OnCommandAddMember(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 删除成员
    static int OnCommandRemoveMember(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //清空队伍
    static int OnCommandClearTeam(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 更新成员信息
    static int OnCommandUpdateMemberInfo(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 更新队伍信息
    static int OnCommandUpdateTeamInfo(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 清除附近队伍列表信息
	static int OnCommandClearNearbyTeam(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 更新队伍成员位置
	static int OnCommandUpdateMemberPosition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// 获取玩家坐标
	static void OnCommandGetTargetPos(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//传送道目标附近
	static void OnCommandTransferToTarget(IKernel*pKernel, const PERSISTID& self, const IVarList& args);

private:
	// 等级提升 
	static int OnCommandLevelUp(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

    // 血量回调
    static int C_OnHPChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

    // 最大血量回调
    static int C_OnMaxHPChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

    // 公会名变化
    static int C_OnGuildNameChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

	// 职业变化回调
	static int C_OnJobChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

	// 战斗力变化回调
	static int C_OnHighestBattleAbilityChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
	// 战斗战斗状态回调
	static int C_OnFightState(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
	// 更新队员位置心跳
	static int H_UpdateMemberPos(IKernel* pKernel, const PERSISTID& self, int slice);

	// VIP升级回调
	static int C_OnVIPLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

private:

    // 复制整个队伍信息到角色属性中
    void RefreshAllTeamInfo(IKernel* pKernel, const PERSISTID& self, int team_id);

    // 清除角色属性中所有队伍信息
    void ClearTeamInfo(IKernel* pKernel, const PERSISTID& self);

    // 更新队伍主表信息到角色属性中
    void RefreshTeamMainInfo(IKernel* pKernel, const PERSISTID& self, int col);

    // 更新队伍成员信息到角色属性中
    void RefreshTeamMemberInfo(IKernel* pKernel, const PERSISTID& self, const wchar_t* memeber_name, int col);

    // 添加成员
    void AddTeamMember(IKernel* pKernel, const PERSISTID& self, const wchar_t* member_name);

    // 删除成员
    void RemoveTeamMember(IKernel* pKernel, const PERSISTID& self, const wchar_t* member_name);

    // 更新角色属性到公共数据服务器
    void UpdatePropToDomain(IKernel* pKernel, const PERSISTID& self, const IVarList& prop_list);

    // 自己上线处理
    void OnSelfRecover(IKernel* pKernel, const PERSISTID& self);

	//返回玩家结果处理
	static void OnCommandCheckConditionRsp(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	//检查请求
	static bool OnCommandCheckConditionReq(IKernel* pKernel, const PERSISTID& self, int reqType, int index,const wchar_t*ResultinfromName);
	//队伍成员条件是否满足
	static int OnCommandCheckCondition(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	//当前队伍是否允许加入
	bool CanJoinTeam(IKernel*pKernel,int teamID);
	//自动匹配
	static void AutoMatch(IKernel*pKernel, const PERSISTID &self, int autoMatch, int objIndex = 0);
public:
	//获得队长名
	const wchar_t *GetTeamCaptain(IKernel*pKernel,int teamID );

	//增加能够匹配的规则
	static bool AddCheckTeamRule(int typeID, CheckTeamCondition rule);

	//检查是否满足入队条件
	static bool DealCheckTeamCondition(IKernel*pKernel, const PERSISTID& self, int index, const wchar_t*ResultinfromName );
	bool LoadRes(IKernel*pKernel);
	bool LoadTeamRule(IKernel*pKernel);
	bool LoadActivitySccene(IKernel*pKernel);

	static bool IsTeamValid(IKernel*pKernel, const PERSISTID& self, int objIndex);
	//玩家是否满足队伍等级
	 static int GetObjMinLimitLevel( int objIndex);
	 //重置队伍限制等级
	 static void ResetTeamLimitLevel(IKernel*pKernel, const PERSISTID& self,  int objIndex);
	//获得队伍索引的目标id
	int GetTeamIndexObjID(int index);
	//检查玩家返回结果
	static bool AddCheckTeamResult(int typeID, CheckTeamResult rule);
	//获取队伍目标索引
	static int GetTeamObjIndex(IKernel *pKernel, int teamID);
	static int GetTeamObjID(IKernel*pKernel, int teamID);
	//根据创建一个类型队伍
	bool CreateTeamByType(IKernel*pKernel, const PERSISTID& self, int teamType);

	static void ReloadConfig(IKernel*pKernel);

public:
	static void LeaveTeam(IKernel* pKernel, const PERSISTID& player);

public:
    // 模块指针
    static TeamModule* m_pTeamModule;
    static RequestModule* m_pRequestModule;
	static FriendModule* m_pFriendModule;
	// 公共数据名称
	static std::wstring m_domainName; 

	struct TeamRule
	{
		TeamRule()
		{
			m_teamType = 0;
			m_objectID = 0;
			m_limitLevel = 0;
		};

		struct TeamWeekTime
		{
			int weekDay; //周几
			int daySec; //每日秒数
			TeamWeekTime() :weekDay(0), daySec(0)
			{}
		};

		int m_teamType;
		int m_objectID; //目标id 例如如果是秘籍就是场景id
		int m_limitLevel;
		
		std::string m_finishAward;
		TeamWeekTime m_openTime;
		TeamWeekTime m_endTime;

	};


	//检查匹配
	static std::map<int, CheckTeamCondition> m_check_team_condition;
	//检查队伍结果
	static std::map<int, CheckTeamResult> m_check_team_result;

	static std::map<int, TeamRule> m_TeamRule;
	//活动场景	
	std::set<int> m_activityScene;

};

#endif //_TEAM_MODULE_H_