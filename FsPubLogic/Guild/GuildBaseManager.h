#ifndef __GUILD_BASE_INFO_H__
#define __GUILD_BASE_INFO_H__
#include "GuildBase.h"


/*
公会排序说明：
1、GUILD_LIST_REC : 公会列表本身是无序的
2、GuildSortHelperSet : 公会表加载完成之后，将所有公会排序到此set中
3、GUILD_SORT_REC : 将排好序的set中的公会放到sort记录中
4、每当公会排序相关属性变化时，需要在set中重新排序，并将变化部分同步到sort记录中，
这一步的代价是很小的，具体参见代码
5、这里以空间换取时间，同时排序相关代码也较之前有所简化

6、其他的选择：可以去掉sort记录表，从而所有需要公会排序的功能都放到public服务器进行处理
*/
// 用于公会排序的辅助结构
struct GuildSortHelper_t
{
	GuildSortHelper_t() : guildName(L""), guildLevel(0), curMember(0), randDate(0), fightAbiliey(0) {}
	GuildSortHelper_t(const wchar_t* name, int nation, int lvl, int member, __int64 t, int ability)
		: guildName(name), guildLevel(lvl), curMember(member), randDate(t), fightAbiliey(ability) {}

	bool operator == (const GuildSortHelper_t &other) const
	{
		return guildName == other.guildName &&
			guildLevel == other.guildLevel &&
			curMember == other.curMember;
	}
	bool operator >= (const GuildSortHelper_t &other) const
	{
		if (fightAbiliey == other.fightAbiliey)
		{
			if (guildLevel == other.guildLevel)
			{
				if (curMember == other.curMember)
				{
					return randDate < other.randDate;   // 时间小的排在前面
				}
				else
				{
					return curMember > other.curMember;
				}
			}
			else
			{
				return guildLevel > other.guildLevel;
			}
		}
		else{
			return fightAbiliey > other.fightAbiliey;
		}
		return false;
	}
	std::wstring guildName;
	int guildLevel;
	int curMember;
	__int64 randDate;
	int fightAbiliey;
};

typedef std::set<GuildSortHelper_t, std::greater_equal<GuildSortHelper_t> > GuildSortHelperSet;
typedef GuildSortHelperSet::iterator GuildSortHelperSetIter;










class GuildBaseManager :public GuildBase
{
public:
	GuildBaseManager():GuildBase(){
		m_pInstance = this;
	}
	~GuildBaseManager()
	{
		m_pInstance = NULL;
	}

	// 给公会排序
	bool DumpGuildSortHelper(IRecord *guildRec, int row, GuildSortHelper_t *helper);
	void SortGuild(const GuildSortHelper_t *helperOld, const GuildSortHelper_t *helperNew);
	//增加组织成员捐助最大值	
	void AddMemberGuildDevoteMax(const wchar_t *guildName ,const wchar_t*playerName,int value);
	bool CreateGuild(IPubKernel*pPubKernel, const wchar_t*guildName, int nation, int guildIndentifying, const wchar_t*guildShortName);

	//代码补丁接口 每个组织都会调用一遍
	virtual bool Patch(IPubData* guildData);
private:
	void PatchRecord(IPubData* guildData);
private:
	virtual bool Init(IPubKernel* pPubKernel);
	bool  InitPubGuildData(IPubKernel* pPubKernel);
	// 初始化帮会场景分组资源表
	int InitGuildStationGroupIDRec();

	// 初始化帮会场景分组资源数据
	int InitGuildStationGroupIDData();
	// 加载配置
	bool LoadResource(IPubKernel* pPubKernel);
	// 重新整理公会排序表
	void CheckAndRefreshSortRecord();

	/********************/



	

	/****************************/


	///////////////////////////////////////////////////////////////////////////
	// 处理场景服务器消息函数
	///////////////////////////////////////////////////////////////////////////

	int OnPublicMessage(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg);

	//请愿
	void OnPetition(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	
	// 处理创建公会的消息
	int OnCreateGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	
	// 处理申请加入公会的消息
	int OnApplyJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 一键申请加入公会消息
	int OnOneKeyApplyJoinGuild(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args);

	// 处理取消申请加入公会的消息
	int OnCancelApplyJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理接受加入公会申请的消息
	int OnAcceptJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	// 邀请同意加入组织
	int OnInviteAgreeJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理拒绝加入公会申请的消息
	int OnRefuseJoinGuild(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理公会成员离线状态变化的消息
	int OnMemberOnlineStateChange(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理删除离线玩家加入公会记录表格的一行
	int OnDeleteOutlineJoinRecord(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理修改公告
	int OnChangeNotice(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理修改宣言
	int OnChangeDeclaration(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理退出公会请求
	int OnQuit(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理踢出公会请求
	int OnFire(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理解散公会请求
	int OnDismiss(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理公会捐献请求
	int OnDonate(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 处理公会任命
	int OnPromotion(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	// 更新成员属性
	int OnUpdateProps(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 更新申请人属性
	int OnUpdateApplyProps(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	// 更新申请人在线状态
	void OnUpdateApplyOnline(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	//解散工会
	int DismissGuild(IPubKernel* pPubKernel, const wchar_t* guildName,
		int sourceId, int sceneId, const wchar_t *playerName);

	// 请求帮主移交
	int OnCaptainTransfer(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	// 会长移交
	int TransferCaptain(const wchar_t* guildName, const wchar_t *oldCaptain, const wchar_t *newCaptain);
	
	// 保存聊天消息
	int OnSaveGuidChannelInfo(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	//设置舞姬自动开启时间
	void OnSetDancingGirlAutoOpenTime(IPubKernel* pPubKernel, const wchar_t*guildName, int time);
	//设置公会简称
	void OnSetGuildShortName(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);

	// 更新传功数据
	void OnUpdateTeachNum(IPubKernel* pPubKernel, const wchar_t*guildName, int sourceId, int sceneId, const IVarList& args, int nOffset);

	
	///////////////////////////////////////////////////////////////////////////
	// 回复场景服务器消息函数
	///////////////////////////////////////////////////////////////////////////
	// 发送创建公会的结果给场景服务器
	void SendCreateGuildResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int result);

	// 发送接受申请加入公会的结果给场景服务器
	void SendAcceptJoinGuildResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName,
		const wchar_t* applyName, int onLine, int result);

	// 发送处理请求的结果
	void SendRequestResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int msgId, int result);

	// 发送退出公会回复
	void SendQuitResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int result, const wchar_t* captainName);

	// 发送踢出公会回复
	void SendFireResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int result, int onLine, const wchar_t* memberName);

	// 通知成员公会解散
	void SendGuildGone(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int onLine, const wchar_t* memberName);

	// 公会贡献结果
	void SendGuildDonate(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* guildName, const wchar_t* playerName, int result,const IVarList & args);

	

	// 通知帮主移交结果
	void SendCaptainResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* oldCaptain, int result, const wchar_t* newCaptain, int oldCaptainPosition, const wchar_t* guildName);

	void SendChangeGuildShortNameResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
	const wchar_t* guildName,	 const wchar_t* playerName, int result);


	///////////////////////////////////////////////////////////////////////////
	// 功能函数
	///////////////////////////////////////////////////////////////////////////
	
	// 添加公会到公会列表中
	bool AddGuildToGuildList(const wchar_t* guildName, const char* playerNameUid,
		const wchar_t* playerName, int playerLevel, int nation);

	// 添加公会私有数据表格
	bool AddGuildTable( IPubData* pGuildData);

	// 添加申请人到申请玩家列表表格中
	bool AddPlayerToApplyTable(IPubData* pGuildData, const char* pPlayerNameUid, const wchar_t* playerName,
		int playerSex, int playerLevel, int ability, int career,int vipLv);
	//通知公会程又有新的申请
	void SendToGuildMemApplyNew(IPubKernel* pPubKernel, IPubData* pGuildData, int sourceId, int sceneId);
	// 添加一条记录到申请者列表表格
	bool AddRecordToApplyListTable(const char *playerUid, const wchar_t* playerName, const wchar_t* guildName);

	// 删除申请玩家列表中的记录
	bool DelPlayerFromApplyTable(IPubData* pGuildData, const wchar_t* playerName);

	// 删除申请者列表中申请公会项记录
	bool DelRecordFromApplyListTable(const wchar_t* playerName, const wchar_t* guildName);

	// 检查是否还可以加入公会
	bool CanJoinGuild(const wchar_t* guildName,  int maxMember);

	// 获取申请人的信息
	bool GetApplyInfo(const wchar_t* guildName,
		const wchar_t* applyName, int &sex, int& career, int& level, int& fight,int& vipLv);

	// 删除申请者的全部申请记录
	bool DelAllApplyRecord(const wchar_t* applyName);

	// 删除申请者在一个公会中的申请记录
	bool DelGuildApplyRecord(const wchar_t* guildName, const wchar_t* applyName);

	// 删除一个公会全部的申请记录
	bool DelGuildAllApplyRecord(const wchar_t* guildName, bool needDeleteRecord);

	// 添加申请者加入公会表格
	bool AddApplyJoinGuild(const wchar_t* guildName, const char* pApplyNameUid, const wchar_t* applyName,
		int sex, int career, int level, int fight, int onLine,int vipLv);

	// 修改公会列表记录中公会当前人数
	bool ChangeGuildCurMember(const wchar_t* guildName);

	// 添加离线申请者加入公会记录
	bool AddOutlineJoinGuild(const wchar_t* guildName, const char* applyUid, const wchar_t* applyName);

	// 获取表格操作接口及玩家记录所在行
	IRecord* GetGuildDataOpt(const wchar_t* guildName, const wchar_t* playerName,
		const char* tableName, int playNameCol, int& row);

	// 获取表格操作接口
	IRecord* GetGuildDataRecOpt(const wchar_t* guildName, const char* tableName);

	// 向公会的私有数据表格中写入数据
	bool SetGuildData(const wchar_t* guildName, const wchar_t* playerName,
		const char* tableName, int playNameCol, int col, const IVarList& value);

	// 从公会的私有数据表格中获取数据
	bool GetGuildData(const wchar_t* guildName, const wchar_t* playerName,
		const char* tableName, int playNameCol, int col, CVarList& value);

	// 向公会的公共数据表格中写入数据
	bool SetGuildPubData(const wchar_t* guildName, const char* tableName,
		int guildNameCol, int col, const IVarList& value);

	// 从公会的公共数据表格中获取数据
	bool GetGuildPubData(const wchar_t* guildName, const char* tableName,
		int guildNameCol, int col, CVarList& value);

	// 添加离线玩家到离线玩家退出公会表格
	//bool AddOutLineQuitGuild(const wchar_t* guildName, const char* pMemberNameUid, const wchar_t* memberName);
	

	// 统计各个角色的数量是否超过最大值
	bool IsPositionFull(IRecord* memberList, int position, int maxNumber);

	//添加公会经验
	int OnAddGuildDefendExp(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
	
	// 添加新的帮会场景分组资源数据
	int AddGuildStationGroupIDData(const wchar_t* guildName);
	// 将正整数随机分为n份 每份至少为1
	bool RandomSplitNumber(int value, int num, IVarList& list);
	//将普通帮众值设置为新定义的值
	bool UpdateMemberPositionVal(IPubData* pPubGuildData);

	//刷新公会战斗力
	void RefreshGuildFightAbility(IPubKernel * pPubKernel, const wchar_t* guildName);
	//自动同一加入公会
	bool AutoAgreeJoinGuild(IPubKernel *pPubKernel, const wchar_t* guildName, const wchar_t* playerName, int sourceId, int sceneId);
public:
	//日志宽字符
	void GetGuildLogWstring(const IVarList& args,std::wstring& logWsting);
	//给所有组织添加日志
	void SendAllGuildLog(int type, const IVarList& args);
	//给同阵营添加日志
	void SendSameNationGuildLog( int nation, int type, const IVarList& args);
	//添加组织日志
	void SendGuildLog(const wchar_t *guildName, int type, const IVarList& args);
	//static void AutoImpeachTimer(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);
	static void DancingGirlReset(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);
	// 踢人次数重置
	static void FireNumResetTimerCb(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name);

	//清除请愿
	void ClearPetition(IPubKernel* pPubKernel, const wchar_t* guildName, int type);
	// 移除请愿玩家
	void RemovePetition(IPubKernel* pPubKernel, const wchar_t* guildName, const wchar_t* pszName);

	// 获得公会数据
	IPubData* FindGuildData(const wchar_t* guildName) const;
public:
	// 记录帮会日志
	int RecordGuildLog(const wchar_t* guildName, int type, const wchar_t* args);
private:
	///////////////////////////////////////////////////////////////////////////
	//弹劾 
	///////////////////////////////////////////////////////////////////////////	
	// 加载配置
// 	bool LoadGuldImpeach(IPubKernel* pPubKernel);
// 	//开始弹劾
// 	int OnStartImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
// 	//撤消弹劾
// 	int OnUndoImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, const char* pSelfUid);
// 	//响应弹劾
// 	int OnRespondImpeach(IPubKernel* pPubKernel, const wchar_t* guildName, int sourceId, int sceneId, const IVarList& args);
// 	//结算弹劾
// 	bool SettleImpeach(IPubKernel* pPubKernel, const wchar_t* data_name);
// 	//自动弹劾
// 	static void AutoImpeach(IPubKernel* pPubKernel, const char* space_name, const wchar_t* guildName, const wchar_t* capName);
// //开始弹劾完成检查定时器
// 	void StartImpeachTimer(IPubKernel* pPubKernel, const wchar_t* guildName);
// 	//弹劾完成检查定时器回调
// 	static int OnImpeachExamine(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time);
	
	public:
	static GuildBaseManager* m_pInstance;


	public:
		void OnZeroClock( IPubKernel* pPubKernel );
	private:

	GuildUpLevelConfigVector m_GuildUpLevelConfig;  // 公会等级配置信息
	GuildSortHelperSet m_GuildSortHelperSet;    // 用于公会排序

	static int m_autoImpeachMaxTime; // 自动弹劾帮主最大的时间
	static int m_memOffLineTimes;	 //自动弹劾时,筛选组织其他成员离线时间 
	static int m_stationGroup;  //驻地场景分组
};

#endif