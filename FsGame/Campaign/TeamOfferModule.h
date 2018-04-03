//--------------------------------------------------------------------
// 文件名:		TeamOfferModule.h
// 内  容:		组队悬赏模块
// 说  明:		
// 创建日期:		2017年01月13日
// 创建人:		tongzt
// 修改人:		 
//--------------------------------------------------------------------
#ifndef _TEAM_OFFER_MODULE_H_
#define _TEAM_OFFER_MODULE_H_

#include "Fsgame/Define/header.h"
#include "TeamOfferDefine.h"

class TeamOfferModule : public ILogicModule
{
public:
	// 初始化
	bool Init(IKernel* pKernel);

	// 关闭
	bool Shut(IKernel* pKernel);

	// 加载资源
	bool LoadRes(IKernel* pKernel);

private:
	// 加载队长宝箱配置
	bool LoadCaptainBox(IKernel* pKernel);

	// 加载奖励规则
	bool LoadAwardRule(IKernel* pKernel);

private:
	// 取得队长宝箱
	const int GetCaptainBox(const int offer_num);

	// 取得奖励规则
	const OfferAwardRule* GetAwardRule(const int offer_num, const int level);
	
private:
	// 可否接取悬赏任务
	bool CanAcceptOfferTask(IKernel* pKernel, const PERSISTID &self);

	// 队员可否进行悬赏活动
	bool CanTeammemberDo(IKernel* pKernel, const PERSISTID &self);

	// 队员可否进入悬赏分组
	bool CanTeammemberEntry(IKernel* pKernel, const PERSISTID &self, 
		const int task_id);

	// 是否可发起组队悬赏
	bool CanLaunch(IKernel* pKernel, const PERSISTID &self, 
		bool check_entry_npc = true);

	// 悬赏场景验证
	bool IsVaildOfferScene(IKernel* pKernel, const PERSISTID &self, 
		const int task_id, const int scene_id);

	// 计算队伍平均等级
	const int GetAverageLvl(IKernel* pKernel, const PERSISTID &self);

public:
	// 玩家准备就绪
	void PlayerReady(IKernel* pKernel, const PERSISTID &self, 
		const IVarList& args);

	// 玩家上线
	void PlayerOnline(IKernel* pKernel, const PERSISTID &self);

	// 发起悬赏
	void Launch(IKernel* pKernel, const PERSISTID &self,
		bool check_entry_npc = true);

	// 队长发起任务同步
	bool SyncTask(IKernel* pKernel, const PERSISTID &self, 
		const IVarList &args);

	// 队员同步任务
	bool MemberSyncTask(IKernel* pKernel, const PERSISTID &self,
		const IVarList &args);

	// 删除悬赏任务
	void DeleteTask(IKernel* pKernel, const PERSISTID &self);

	// 队长发起进入副本
	void EntryGroup(IKernel* pKernel, const PERSISTID &self, 
		const int team_id);

	// 队员跟随进入
	void FollowEntry(IKernel* pKernel, const PERSISTID &self);

	// 退出分组
	void LeaveGroup(IKernel* pKernel, const PERSISTID &self);

	// 组队悬赏结果处理
	void OnResult(IKernel* pKernel, const IVarList& args);

	// 处理组队悬赏结果
	void ProcResult(IKernel* pKernel, const PERSISTID &creator,
		const IVarList& args);

	// 加入队伍
	void AddTeam(IKernel* pKernel, const PERSISTID &self);

	// 退出队伍
	void QuitTeam(IKernel* pKernel, const PERSISTID &self, 
		const int old_team_id);

	// 继续发起下一个悬赏
	void ContinueNext(IKernel* pKernel, const PERSISTID &self);

	// 奖励处理
	void RewardTeamOffer(IKernel* pKernel, const PERSISTID &self, 
		const int pass_time);

	// 发送消息到公共区间
	void SendS2PMsg(IKernel* pKernel, const IVarList& s2p_sub_msg);

	// 取得公共区数据
	IPubData* GetPubData(IKernel* pKernel);

	// 重新加载组队悬赏活动配置
	static void ReloadTeamofferConfig(IKernel* pKernel);

private:
	// 客户端消息
	static int OnCustomMsg(IKernel* pKernel, const PERSISTID &self, 
		const PERSISTID & sender, const IVarList & args);

	// 组队悬赏Npc被杀回调
	static int OnOfferNpcBeKilled(IKernel* pKernel, const PERSISTID& offer_npc,
		const PERSISTID& killer, const IVarList& args);

	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 玩家离开场景
	static int OnPlayerLeaveScene(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

	// 进入场景
	static int OnPlayerEntry(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	// 客户端准备就绪
	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 下线
	static int OnStore(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 玩家内部消息
	static int OnCommandMsg(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 刷怪器内部消息
	static int OnCreatorCommandMsg(IKernel *pKernel, const PERSISTID &creator,
		const PERSISTID &sender, const IVarList &args);

	// 任务进度
	static int OnCommandTaskProc(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 队伍变化回调
	static int C_OnTeamChange(IKernel* pKernel, const PERSISTID& self, 
		const char *property, const IVar& old);

	// 组队悬赏结束心跳
	static int HB_TeamOfferEnd(IKernel* pKernel, const PERSISTID& creator,
		int slice);

	// 组队悬赏副本控制心跳
	static int HB_Contrl(IKernel* pKernel, const PERSISTID& creator,
		int slice);

	// 处理公共数据服务器下传的消息
	static int OnPublicMessage(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static const std::wstring & GetDomainName(IKernel * pKernel);

private:
	CaptainBoxVec m_CaptainBoxVec;			// 队长宝箱
	OfferAwardRuleVec m_OfferAwardRuleVec;	// 个人奖励总规则

	static std::wstring m_domainName;

public:
	static TeamOfferModule *m_pTeamOfferModule;
};

#endif	// _TEAM_OFFER_MODULE_H_
