//--------------------------------------------------------------------
// 文件名:      FriendModule.h
// 内  容:      好友模块
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __FriendSystem_H__
#define __FriendSystem_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include "FsGame/Define/FriendDefine.h"

#define CONFIG_FRIEND_REFRESH_FRIEND_RMD_CD 30*1000
#define INTIMACY_AWARD "mail_intimacy_award" 
class RankListModule;
//class PartnerModule;
class CapitalModule;
class RevengeArenaModule;
class TeamModule;

class FriendModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);
	// 释放
	virtual bool Shut(IKernel* pKernel);
	// 加载配置 
	bool LoadConfig(IKernel* pKernel);
	
	static void ReloadConfig(IKernel* pKernel);

	//好友数量加成
	bool LoadTeamFriendConfig(IKernel*pKernel);
	//好友亲密度加成
	bool LoadTeamFirendIntimacy(IKernel*pKernel);
	//
	bool LoadFirendIntimacyAward(IKernel*pKernel);
public:
	// 玩家上线
	static int OnRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// 玩家下线
	static int OnStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// 玩家准备就绪
	static int OnReady(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	//接收sns消息
	static int OnSnsMessage(IKernel * pKernel, const PERSISTID & self,const PERSISTID & sender, const IVarList & args);

	// 是否为仇人
	static bool IsEnemy(IKernel* pKernel, const PERSISTID& self, const char* strUid);

	// 客户端消息
	static int OnCustomMessage(IKernel * pKernel, const PERSISTID & self,const PERSISTID & target, const IVarList & args);
	// 内部消息
	static int OnCommandMessage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// sns内部消息
	static int OnPlayerSnsCommand(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// 被杀回调
	static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self,	const PERSISTID& sender, const IVarList& args);
	// 杀死目标回调
	//static int OnCommandKillTarget(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// 距离比较
	static bool CompareDistance(const NearbyRecommend& first, const NearbyRecommend& second);
    //等级变化回调
    static int OnCommandLevelChange(IKernel* pKernel, const PERSISTID& self,	const PERSISTID& sender, const IVarList& args);
	//组队增加亲密度
	//static int OnCommandTeamAddIntimacy(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	
private:
	// 添加好友请求
	void OnCustomAddFriend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 确认好友添加
	void OnCustomAcceptAddFriend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 申请删创造（忽略）邀请好友未处理项
	void OnCustomDeleteFriendApply(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 删除好友
	void OnCustomDeleteFriend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 请求刷新推荐好友
	void OnCustomRefreshFriendRmd(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 更新好友信息
	void OnCustomFriendInfoUpdate(IKernel* pKernel, const PERSISTID& self);
	// 添加仇人
	void OnCustomAddOnceEnemy(IKernel * pKernel, const PERSISTID & self, const PERSISTID& sender);
	// 删除仇人
	void OnCustomDeleteEnemy(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 删除所有仇人
	void OnCustomDeleteAllEnemy(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 请求附近玩家
	void OnCustomRequestNearby(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 查找玩家到推荐中
	void FindPlayerToRecommend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 发消息给公服，请求更新好友信息
	void OnCustomQueryFriendInfo(IKernel* pKernel, const PERSISTID& self);
	// 向好友送好
	void OnCustomSendGift(IKernel* pKernel, const PERSISTID& self, const char* targetUid, int nGiftId);
	// 添加黑名单
	void OnCustomAddBlackList(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName);
	// 删除黑名单
	void OnCustomDeleteBlackList(IKernel* pKernel, const PERSISTID& self, const wchar_t* targetName);
	// 清除黑名单
	void OnCustomClearBlackList(IKernel* pKernel, const PERSISTID& self);
	// 一键处理所有好友申请
	void OnCustomConfirmAllApply(IKernel* pKernel, const PERSISTID& self, bool bAccept);
	// 结拜申请
	void OnCustomApplaySworn(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	//结义申请回复
	void OnCustomApplaySwornReply(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	
	// 寻求援助
	void OnCustomSeekAid(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	// 修改好友称谓
	void OnCustomChangeFriendTitle(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	//删除结义
	void OnCustomDelSworn(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	//援助
	void OnCustomAid(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	//领取亲密度奖励
	void OnCustomIntimacyAwards(IKernel*pKernel, const PERSISTID& self);
	//一键添加好友
	void OnCustomAddFriendByOneKey(IKernel*pKernel, const PERSISTID& self, const IVarList &args);
	
	//增加敌人
	void AddOnceEnemy(IKernel*pKernel, const PERSISTID& self, const IVarList & args);
	//结义好友增加敌人
	void SwornFriendAddOnceEnemy(IKernel*pKernel, const PERSISTID&self, const IVarList & args);
	// 更新好友的在线状态
	void OnCommandOnlineUpdate(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 删除好友
	void OnCommandRemoveFriend(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 好友申请添加
	void OnCommandAddFriendApply(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 好友等级更新
	void OnCommandFriendLevelUpdate(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 好友战斗力更新
	void OnCommandFriendBattleAbility(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 好友帮会更新
	void OnCommandFriendGuildName(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
    //添加好友反馈
    void OnCommandAddFriendResult(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 好友花数量更新
	void OnCommandFriendFlowerChanged(IKernel * pKernel, const PERSISTID & self, const char* friendUid, int nSumFlower, int nWeekFlower );
	// 增加自己和好友亲密度
	void OnCommandAddFriendMutualIntiMacy(IKernel*pKernel,const PERSISTID&self,const char* firendUid,int addIntimacy);
	
	// 增加亲密度
	void OnCommandAddFriendIntiMacy(IKernel*pKernel,const PERSISTID&self,const char* firendUid,int addIntimacy,bool isAddDaily = false);

	// 取得sns查询数据
	void OnCommandGotSnsData(IKernel * pKernel, const PERSISTID & self, const IVarList & args,IVarList &core_msg);
		
	//好友队伍id
	void OnCommandUpdateFriendTeamID(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	//申请结义
	void OnCommandApplaySworn(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	//返回对方是否原因结义
	void OnCommandApplaySwornRsp(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	//结义成功
	void OnCommandSwornSuccess(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	//删除结义对象
	void OnCommandDelSworn(IKernel * pKernel, const PERSISTID & self, const IVarList & args);

	bool IsCanSworn(IKernel*pKernel, const PERSISTID& self, const wchar_t* targetName);
	//
	void OnCommandAidGetPosReq(IKernel* pKernel, const PERSISTID & self, const IVarList & args);
	void OnCommandAidGetPosRsp(IKernel* pKernel, const PERSISTID & self, const IVarList & args);
	//void OnCommandSetNation(IKernel* pKernel, const PERSISTID & self, const IVarList & args);
	void OnCommandUpdatetVip(IKernel* pKernel, const PERSISTID & self, const IVarList & args);



	// 刷新好友加成
	static int OnCommandRefreshFriendGain(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	


	// 删除推荐玩家
	int DeleteRecommend(IKernel * pKernel, const PERSISTID & self, const wchar_t* pTargetName );
	// 添加一个推荐玩家
	void AddOneRecommend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 更新一个好友
	void UpdateOneFriend(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 更新仇家信息
	void UpdateOneEnmey(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 更新黑名单信息
	void UpdateBlackPlayer(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 查找一个玩家
	void FindOnePlayer(IKernel * pKernel, const PERSISTID & self, const IVarList & args);

    // 设置玩家为已邀请
    void SetPlayerInvited(IKernel * pKernel, const PERSISTID & self, const char *role_uid);  
    // 查看此玩家是否已经被邀请 bRemove: 如果存在删除
    bool TestRemovePlayerInvited(IKernel * pKernel, const PERSISTID & self, const char *role_uid, bool bRemove = false );
	//获得结义结拜人数
	int GetSwornNum(IKernel*pKernel, const PERSISTID & self,int sex);
	//vip升级推荐好友
	void RecommendFriendByVipLv(IKernel*pKernel,const PERSISTID & self);
	// 玩家升级
	void RecommendFriendByPlayerLv(IKernel*pKernel, const PERSISTID & self);
	// 加入组织
	//void RecommendFriendByAddGuild(IKernel*pKernel, const PERSISTID & self);
	//是否可以成为好友
	bool CanBeFirend(IKernel*pKernel, const PERSISTID & self,const wchar_t * selfName ,const wchar_t*targetName,int targetLv);
	
		
	//获得当前类型增加多少亲密度
	int GetTeamAddIntimacyValue(int type);
	//每日增加亲密度上限
	int GetDailyAddIntimacyLimit();


	//--------------------------Sns服务器发来的消息处理函数-------------------------------
	// 查询“好友申请”的回复
	static int OnSnsQuestFriendApplyRPL(IKernel* pKernel, const IVarList& args);
	// 查询“解除关系”的回复
	static int OnSnsQuestFriendDeleteRPL(IKernel* pKernel, const IVarList& args);
	// 查询“好友申请结果”的回复
	static int OnSnsQuestFriendApplyResultRPL(IKernel* pKernel, const IVarList& args);
	// 获取“好友送花”的回复
	static void OnSnsQuestFriendGiftPush(IKernel* pKernel, const IVarList& args);
	// 获取增加好友亲密度
	static void OnSnsAddFriendIntimacyPush(IKernel* pKernel, const IVarList& args);
	// 删除结义对象
	static void OnSnsDelSworn(IKernel* pKernel, const IVarList& args);
	// 增加敌人
	static void OnSnsAddEnemy(IKernel*pKernel,const IVarList& args);

public:
	// 战斗力变化回调
	static int C_OnDCBattleAbilityChange(IKernel * pKernel, const PERSISTID & self, const char * property, const IVar & old);
	// 帮会名变化回调
	//static int C_OnDCGuildNameChange(IKernel * pKernel, const PERSISTID & self, const char * property, const IVar & old);
	//队伍id变化回调
	static int C_OnDCTeamIDChange(IKernel*pKernel, const PERSISTID & self, const char * property, const IVar & old);

	// VIP升级回调
	//static int C_OnVIPLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
private:
	// 通知好友刷新花数量
	void OnPlayerGiftChanged(IKernel* pKernel, IGameObj* pSelf, int nMySumFlower, int nMyWeekFlower);
	// 降低仇恨值
	void OnDecEnmeyHaterT(IKernel* pKernel, IGameObj* pSelf, const char* pEnemyUid, int nHatarT);
	//刷新队伍好友增益
	//bool RefreshTeamFriendGain(IKernel*pKernel, const PERSISTID& self);
public:
	// 玩家打包
	bool PackPlayerInfo(IKernel* pKernel, IVarList& var, const wchar_t* pTargetName);
	
	void GetNoRepeatRandomNumber(const int nMax, const int Number, std::vector<int> & iVector);

	// 申请添加邀请好友未处理项
	int AddFriendApply(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 申请删除（忽略）邀请好友未处理项
	int DeleteFriendApply(IKernel * pKernel, const PERSISTID & self, const IVarList & args);
	// 删除好友
	void DeleteFriend(IKernel * pKernel, const PERSISTID & self, const wchar_t * targetName);

	// 是否能添加好友
	bool IsCanAddFriend(IKernel * pKernel, const PERSISTID & self, const wchar_t * targetName, bool bShowFailedTip = false, bool bIgnoreFull = false);

	// 好友列表是否已满
	bool IsFullFriend(IKernel * pKernel, const PERSISTID & palyer);
	// targetName是否是自己的好友
	bool IsFriend(IKernel * pKernel, const PERSISTID & self, const wchar_t * targetName);
	// targetName是否在自已的仇人里
	bool IsInEnemyList(IKernel * pKernel, const PERSISTID & self, const wchar_t * targetName);
	// targetName是否在自已的黑名单里
	bool IsInBlackList(IKernel * pKernel, const PERSISTID & self, const wchar_t * targetName);

	// 根据玩家名字判断玩家是否存在
	bool CheckPlayerExist(IKernel* pKernel, const wchar_t* name);

	// 根据玩家名字判断玩家是否在线（先判断是否存在）
	bool CheckPlayerOnline(IKernel* pKernel, const wchar_t* name);
    //获得申请表中所在行数
    int FindApplyRow(IKernel * pKernel, const PERSISTID & self, const char* targetUid, ApplyType applyType);
public:
    // 获取公共服在线玩家表
    IRecord * GetPubOnlineRec(IKernel * pKernel);

private:
	// 成功赠送花
	void OnSendFlower(IKernel* pKernel, IGameObj* pSelf, const char* pszTargetUid, int nFlowers);
	// 成功收到花
	void OnRecvFlower(IKernel* pKernel, IGameObj* pSelf, const char* pszSenderUid, int nFlowers);

	// 增加亲密度处理 返回亲密度等级
	int IncFriendIntimacy(IKernel* pKernel, IGameObj* pSelf, IRecord* pRecord, int nRowIndex, int nExp, bool isAddDaily = false);
	// 增加好友亲密度
	void IncTeamFriendIntimacy(IKernel* pKernel, IGameObj* pSelf, const CVarList& friendList, int nExp);
	//能否增加亲密度
	bool IsCanAddIntimacy(IKernel*pKernel,IGameObj* pSelf,const char* friendUid);
	bool IsCanAddIntimacy(int intimacyLevelNow, int intimacyLevelMax,int intimacyExpNow);

	//获得亲密度升级所需经验
	int GetIntimacyUpNeedExp(int intimacyLevel);
private: 
	// 清除花心跳
	static int ResetWeekFlowerNum(IKernel* pKernel, const PERSISTID& self, int slice);
	// 每日重置
	static int ResetDaily(IKernel* pKernel, const PERSISTID& self, int slice);
	// 清除周收花数
	void ResetFriedFlower(IKernel* pKernel, IGameObj* pSelf);
	// 接受好友申请
	bool OnAcceptFriend(IKernel* pKernel, IGameObj* pSelf, int nApplyIndex, IRecord* pRecApply, IRecord* pRecFriends);
	// 拒绝好友申请
	void OnRefuseFriend(IKernel* pKernel, IGameObj* pSelf, int nApplyIndex, IRecord* pRecApply);
	// 获取公共空间名字
	static const std::wstring& GetDomainName(IKernel * pKernel);
	//bool IsCanSworn(IKernel*pKernel, const PERSISTID& self, const wchar_t* targetName);

	static std::wstring m_domainName;
public:
	// 本模块指针
	static FriendModule * m_pFriendModule; 
private:
	// 排行榜模块
	static RankListModule * m_pRankListModule; 
   
	//金币模块
	static CapitalModule* m_pCapitalModule;
	//生死擂
	static RevengeArenaModule* m_pRevengeArenaModule;
	// 组队
	static TeamModule* m_pTeamModule;

	//组队好友增益
	std::map<int, std::string> m_teamFriendGain;
	//组队亲密度加成那个
	std::map<int, std::string> m_teamFriendIntimacy;
	//亲密度奖励
	std::map<int, std::string> m_teamIntimacyAward;
};

#endif
