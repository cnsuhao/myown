//--------------------------------------------------------------------
// 文件名:		GuildModule.cpp
// 内  容:		公会系统
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __GuildModule_H__
#define __GuildModule_H__

#include "Fsgame/Define/header.h"
#include "../Define/GuildDefine.h"

class CapitalModule;
class VirtualRecModule;
class ItemBaseModule;
class ChatModule;
class SystemMailModule;

class GuildModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);
    // 关闭
    virtual bool Shut(IKernel* pKernel);

    // 公会是否已经存在
    bool IsGuildExist(IKernel* pKernel, const wchar_t* guildName);
    // 获取公会的数据项
    IPubData* GetPubData(IKernel* pKernel, const wchar_t* name);
    // 获取公会公有数据记录
    IRecord* GetPubRecord(IKernel* pKernel, const wchar_t* pubDataName, const char* recName);
    // 获得玩家所在公会名称
    const wchar_t* GetPlayerGuildName(IKernel* pKernel, const PERSISTID& self);
    // 获取公会表格记录
    IRecord* GetGuildRecord(IKernel* pKernel, const PERSISTID& self, int& row);
	// 获取公会表格记录
	IRecord* GetGuildRecord(IKernel* pKernel, const wchar_t* guildName, int& row);
    // 获取成员表格记录
    IRecord* GetMemberRecord(IKernel* pKernel, const PERSISTID& self, int& row);
	// 获取成员表格记录
    IRecord* GetMemberRecord(IKernel* pKernel, const wchar_t* guildName);
    // 获得帮会帮主UID
    const char* GetGuildCaptainUid(IKernel* pKernel, const wchar_t *guildName);
	// 加载资源
	bool LoadResource(IKernel* pKernel);
	// 获取帮会等级
	int GetGuildLevel(IKernel* pKernel, const wchar_t* guildName);
	// 检测玩家权限是否满足
	bool CheckPositionPriv(IKernel* pKernel, const PERSISTID & self, GuildPriv iPrivType);

	// 刷新审核者公会的加入申请提示
	static bool UpdateNewApplyToAuditor(IKernel* pKernel, const PERSISTID& player);
	// 检查玩家是否是公会会长
	bool CheckIsCaptain(IKernel* pKernel, const PERSISTID& self);

	// 准备就绪
	static int OnReady(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);
	//如果有公会申请通知
	void InformGuildApply(IKernel*pKernel, const PERSISTID & self);
	

	void CustomMessageToGuildMember(IKernel*pKernel, const wchar_t* guildName, const IVarList& msg);
	void CommandToGuildMember(IKernel*pKernel, const wchar_t* guildName, const IVarList& msg);

	void SendGuildAward(IKernel* pKernel, const std::string &award, const wchar_t * guildName);

	void CustomSysInfoByGuild(IKernel * pKernel, const wchar_t* guildName, int tips_type,
		const char * fast_string_id, const IVarList & para_list);
	// 获得组织数值
	int GetGuildNumValue(IKernel*pKernel, const wchar_t* guildName, GUILD_NUM_TYPE guildNumType);
	//组织短名是否存在
	bool GuildShortNameExist(IKernel*pKernel,const wchar_t* shortName);

	//获得建筑等级
	int GuildBuildingLevel(IKernel *pKernel,const wchar_t* guildName, GUILD_BUILDING_TYPE buildType);
	//组织模块奖励
	void RewardGuildValue(IKernel *pKernel, const wchar_t*guildName, GUILD_NUM_AWARD type, int addValue, GUILD_NUM_CHANGE res, const wchar_t*playerName = NULL);
	//增加组织建筑经验
	void AddGuildBuildingExp(IKernel *pKernel,const wchar_t*guildName,int addValue);
	//增加组织数值
	void AddGuildNumValue(IKernel *pKernel, const wchar_t*guildName, GUILD_NUM_TYPE type, int value, GUILD_NUM_CHANGE res, const wchar_t*playerName = NULL);
	//增加技能研究经验
	void AddGuildSkillExp(IKernel *pKernel,const wchar_t*guildName,int addValue);
	//增加组织日志
	void AddGuildLog(IKernel*pKernel, const PERSISTID& self,int type, const IVarList& args);
	// 获得玩家所在公会会长的名字
	const wchar_t* GetPlayerGuildMasterName(IKernel* pKernel, const PERSISTID& self);

	// 扣除组织数值
	static bool DecGuildValue(IKernel* pKernel, const GUILD_NUM_TYPE type, 
		const wchar_t *guild_name, const int value, const GUILD_NUM_CHANGE src);
private:
    // 玩家上线
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    // 玩家下线
    static int OnPlayerDisconnect(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    // 玩家等级变化
    static int OnLevelChange(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args);
	// 战斗力变化回调
	static int OnBattleAbilityChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

	 //组织职位
	static int C_OnGuildPositionChange(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

	//vip等级
	static int C_OnPlayerdVipLv(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
    ///////////////////////////////////////////////////////////////////////////
    // 功能函数
    ///////////////////////////////////////////////////////////////////////////
    // 设置不在线玩家加入公会的情况
    int SetOutLineJoinGuild(IKernel* pKernel, const PERSISTID& self, const wchar_t* playerName);
  
    // 向公共服务器发送玩家在线状态变化消息
    void SendOnlineStateToPub(IKernel* pKernel, const PERSISTID& self, int onlineState);
    // 向公共服务器发送玩家属性变化的消息（等级、战斗力、竞技场排名）
    void SendPropsChangeToPub(IKernel* pKernel, const PERSISTID& self);
    
    // 检测wchar_t是否有效
    bool IsWidStrValid(const wchar_t* widStr);
    // 检测能否创建公会
    bool CheckCanCreateGuild(IKernel* pKernel, const PERSISTID& self, const wchar_t* guildName);
    // 检查是否可以进行审批是否加入公会的操作
    bool CheckCanOptJoinGuild(IKernel* pKernel, const PERSISTID& self, const wchar_t* applyName);
  
    // 检查是否可以退出公会
    bool CheckCanQuit(IKernel* pKernel, const PERSISTID& self);
    // 检查是否可以踢出成员
    bool CheckCanFire(IKernel* pKernel, const PERSISTID& self, const wchar_t* member);
    // 检查是否可以解散公会
    bool CheckCanDismiss(IKernel* pKernel, const PERSISTID& self);
    // 设置玩家离开公会
    bool SetLeaveGuild(IKernel* pKernel, const PERSISTID& self, int64_t leaveDate,bool isSetQuitGuildTime =false);
	// 检测职位是否具有权限
	bool CheckPositionPriv(IKernel* pKernel, int position, GuildPriv iPrivType);
    // 序列化帮会信息，用于发送客户端显示的帮会列表
    bool SerialGuildInfo(IKernel *pKernel, const PERSISTID& self, CVarList &varGuild, int randVal, const wchar_t *guildName);
	// 获得帮会升级配置信息
    GuildUpLevelConfig_t *GetGuildUpLevelConfig(int lvl);
	// 获得职位对应商店购买比列
    int GetGuildShopMemberLimit(int guildPosition, int rawLimit);
	// 获得帮会职位配置信息
    GuildPositionConfig_t *GetGuildPositionConfig(int position);
	// 检查踢人次数
	bool CheckFireLimit(IKernel* pKernel, const PERSISTID& self);
	//获取组织技能升级花费
	int GetGuildSkillLevelUpSpend(int skillID,int skillLevel);
	//刷新组织符号
	void RefreshGuildSysmbol(IKernel*pKernel, const PERSISTID& self);
	//组织每日功能重置
	static int ResetDaily(IKernel* pKernel, const PERSISTID& self, int slice);
public:
	///////////////////////////////////////////////////////////////////////////
	// 客户端消息回调
	static int OnCustomMessage(IKernel* pKernel, const PERSISTID& self, 
        const PERSISTID& sender, const IVarList& args);
	// 广播客户端消息
	static void BroadCastGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& broadcast);
	static void BroadCastGuild(IKernel* pKernel, const wchar_t* pwszGuildName, const IVarList& broadcast);
private:
    // 处理创建公会的请求
    int OnCustomCreateGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理获取公会列表请求
    int OnCustomGetGuildList(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理搜索公会请求
    int OnCustomFindGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理请求表格数据
    int OnCustomGetTableData(IKernel* pKernel, const PERSISTID& self, int msgType);
    // 处理申请加入公会的请求
    int OnCustomApplyJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 处理一键申请加入公会请求
	int OnCustomOneKeyApplyJoinGuild(IKernel* pKernel, const PERSISTID& self);

    // 处理取消申请加入公会的请求
    int OnCustomCancelApplyJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 处理获取公会信息的请求
	int OnCustomGetGuildInfo(IKernel* pKernel, const PERSISTID& self);
	// 发送组织数值请求
	int OnCustomGuildNumInfo(IKernel*pKernel, const PERSISTID& self,const wchar_t*guildName);
	
	
	//获得公会徽章信息
	int OnCustomGuildSysmbol(IKernel*pKernel, const PERSISTID& self, const wchar_t * guildName);
	//组织建筑信息
	int OnCustomGuildBuilding(IKernel*pKernel, const PERSISTID& self);
	// 组织设置信息
	int OnCustomGuildSetInfo(IKernel*pKernel, const PERSISTID& self);
	// 使用秘宝
	int OnCustomUseRareTreasure(IKernel*pKernel, const PERSISTID& self,int rareTreasureType,int num);
	//获得秘宝列表
	int OnCustomGetRareTreasureList(IKernel*pKernel, const PERSISTID& self);
	//获得组织数值购买次数
	int OnCustomGetNumBuyRecord(IKernel*pKernel, const PERSISTID& self);
	
	// 处理接受加入公会的请求
	int OnCustomAcceptJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    // 处理拒绝加入公会的请求
    int OnCustomRefuseJoinGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    // 处理请求成员列表
    int OnCustomGetGuildMemberList(IKernel *pKernel, const PERSISTID &self, const IVarList &args);

    // 处理请求申请入帮列表
    int OnCustomGetGuildApplyList(IKernel *pKernel, const PERSISTID &self, const IVarList &args);

    // 处理修改公告请求
    int OnCustomChangeNotice(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 处理修改宣言请求
	int OnCustomChangeDeclaration(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求退出公会
    int OnCustomQuit(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求踢出公会
    int OnCustomFire(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求解散公会
    int OnCustomDismiss(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求捐献
    int OnCustomDonate(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求升职
    int OnCustomPromotion(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int type);
    // 请求公会商店列表
    int OnCustomGetShopList(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求加入公会的CD
    int OnCustomGetJoinCD(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 请求购买商店商品
    int OnCustomBuyItem(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 请求移交帮主之位
    int OnCustomCaptainTransfer(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 请求帮会日志
	int OnCustomGetLog(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	// 请求捐献日志
	int OnCustomGetDonateLog(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	
	//自动同意
	void OnCustomSetAutoAgree(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	//解锁公会技能 
	void OnCustomUnLockSkill(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	//获得公会技能列表
	void OnCustomGetSkillList(IKernel*pKernel,const wchar_t*guildName,CVarList&var);
	//技能升级
	void OnCustomSkillLevelUp(IKernel*pKernel, const PERSISTID& self,const IVarList& args);
	//组织技能升级
	void GuildSkillLevelUp(IKernel*pKernel, const PERSISTID & self,int level , int newLevel);
	//组织建设升级
	void OnGuildBuildingOperateLv(IKernel*pKernel,const PERSISTID& self,const IVarList& args);
	//组织数值购买
	void OnGuildNumBuy(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//修改组织简称
	void OnCustomChangeGuildShortName(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//领取健康度礼包
	void OnCustomGetJianKangDuGif(IKernel*pKernel,const PERSISTID& self);
	//开始研究技能
	void OnCustomGuildSkillLvOperate(IKernel*pKernel, const PERSISTID& self, const IVarList& args);

	//获取分红信息
	int OnCustomGetBonusInfo(IKernel*pKernel, const PERSISTID& self);
	//获取组织资金
	int OnCustomGetGuildWelfare(IKernel*pKernel, const PERSISTID& self);
	//分红
	int OnCustomGiveOutBonus(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//邀请加入组织
	int OnCustomInviteJoinGuild(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//同意邀请加入组织
	int OnCustomAgreeInviteJoinGuild(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//添加请愿
	void OnCustomAddPetition(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//获取请愿
	void OnCustomGetPetition(IKernel*pKernel, const PERSISTID& self, const IVarList& args);
	//获得其他组织信息
	void OnCustomGetOtherGuildInfo(IKernel*pKernel, const PERSISTID& self, const IVarList& args);

	// 获取在线可传功列表
	void OnCustomTeachList(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	// 请求对方传功 args: nick[widestr] type[1:req 2:push]
	void OnCustomTeachReq(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	// 回应给对方请求 args: nick[widestr] type[1:req 2:push] agree[1: 同意 0: 拒绝]
	void OnCustomTeachAck(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	// // 客户端告诉服务器准备好了 args: none
	void OnCustomTeachReady(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);


    ///////////////////////////////////////////////////////////////////////////
    // 处理场景服内部消息
    ///////////////////////////////////////////////////////////////////////////
    // 内部命令回调
    static int OnCommandGuild(IKernel* pKernel, const PERSISTID& self, 
        const PERSISTID& sender, const IVarList& args);
	// 玩家被杀死组织广播
	static int OnCommandBeKilled(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args);
	// 帮会等级改变
	static int OnGuildLevelChange( IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args );
    // 处理创建结果消息
    int OnCommandCreateGuildResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理申请加入公会结果消息
    int OnCommandApplyJoinGuildResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理取消申请加入公会结果消息
    int OnCommandCancelApplyJoinGuildResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理审批者接受申请结果的消息
    int OnCommandAcceptResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理申请者加入公会结果的消息
    int OnCommandApplyJoinResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理审批者成功拒绝了申请的消息
    int OnCommandRefuseJoin(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理退出公会结果
    int OnCommandQuitResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理解散公会结果
    int OnCommandDismissResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理离开公会
    int OnCommandLeaveGuild(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理公会捐献
    int OnCommandDonateResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    // 处理购买商品扣除贡献值结果消息
    int OnCommandBuyItemResult(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 日常活动
	static int OnCommandDailyActivity(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//----------------------------------------------------------------------------------
	// 传功
	void OnCommandTeachRequest(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	void OnCommandTeachAck(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	void OnCommandTeachFin(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);
	void OnCommandTeachFailed(IKernel* pKernel, const PERSISTID& self, const IVarList& args, int nOffset);

    ///////////////////////////////////////////////////////////////////////////
    // 处理公共服务器消息
    ///////////////////////////////////////////////////////////////////////////
    // 处理公共数据服务器下传的消息
    static int OnPublicMessage(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);


private:
	// 职位发生变动，刷新申请提示
	int UpdateNewApplyByPosition(IKernel* pKernel, const PERSISTID& player);
	// 刷新所有审核者公会的加入申请提示
	int UpdateNewApplyToAllAuditor(IKernel* pKernel, const wchar_t* guildName);
	// 获取公会的申请记录数
	int GetGuildApplyCount(IKernel* pKernel, const wchar_t* guildName);
	
	// 刷新帮会职位
	void UpdateGuildInfo(IKernel* pKernel, const PERSISTID& self);

    //校验是否公会活动期间
	bool CheckGuildActivity(IKernel* pKernel, const PERSISTID& self);

    // 刷新帮会等级
	void UpdateGuildLevelToPlayer(IKernel* pKernel, const PERSISTID& self);
	//刷新组织技能
	void RefreshGuildSkill(IKernel*pKernel, const PERSISTID& self);
	
	//获得商店物品信息
	const GuildShopConfigItem_t* GetShopItemInfo(int itemIndex);
	////是否是有效的公会名
	//bool IsVaildGuildName(const wchar_t*guildName);
	static void ReloadConfig(IKernel* pKernel);

	// 传功上线处理
	void OnTeahHandleClientReady(IKernel* pKernel, const PERSISTID& self);
	// 同步传功次数
	void SyncTeachCount(IKernel* pKernel, const PERSISTID& self);
	// 发送同步传功消息
	void SendToClientTeachCount(IKernel* pKernel, const PERSISTID& self, int nTeachCount, int nReciveCount) const;
	EmTeachResult CheckTeachAble(IKernel* pKernel, const PERSISTID& self, const wchar_t* pszName, EmTeachMethod method, bool bActive);
	// 调换结果
	EmTeachResult SwapResult(EmTeachResult orig);
	// 清除传功数据
	void CleanTeach(IKernel* pKernel, IGameObj* player);
	// 准备传功
	bool PrepareTeach(IKernel* pKernel, const PERSISTID& player, const wchar_t* pszTarget, EmTeachType type);
	// 检查是否进入了
	void CheckTeachPrepare(IKernel* pKernel, const PERSISTID& player);
	// 检查是否准备好了
	void CheckTeachReady(IKernel* pKernel, const PERSISTID& player);
	// 传功心跳
	static int H_TeachAutoRun(IKernel* pKernel, const PERSISTID& self, int slice);

public:
    static GuildModule* m_pGuildModule;
private:
    static CapitalModule  *m_pCapitalModule;            // 资金模块
    static ItemBaseModule *m_pItemBaseModule;           // 道具模块
    static VirtualRecModule* m_pVirtualRecModule;       // 虚拟表模块
	static ChatModule* m_pChatModule;					// 聊天模块
	static SystemMailModule* m_pMailModule;				// 邮件模块

    GuildCreateConfig_t m_GuildCreateConfig;        // 创建公会配置信息
    GuildUpLevelConfigVector m_GuildUpLevelConfig;  // 公会等级配置信息
    GuildDonateConfigVector  m_GuildDonateConfig;   // 公会捐献配置信息
    GuildPositionConfigVector m_GuildPositionConfig;// 公会职位配置
	GuildShopConfigVector m_GuildShopConfig;        // 公会商店
	GuildSkillConfigMap		m_guildSkill;				//组织技能
	GuildSkillLevelUpSpendMap  m_guildSkillLevelUpCost; //  组织技能升级花费
	
	
	GUILDPREPAREINFO m_guildPrepareInfo;
};

#endif //__GuildModule_H__