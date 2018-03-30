#ifndef __GUILD_NUM_MANAGE_H__
#define __GUILD_NUM_MANAGE_H__
#include "GuildBase.h"
#include "FsGame\Define\GuildDefine.h"

class GuildNumManage:public GuildBase
{

public:
	GuildNumManage(){
		m_pInstance = this;
	}
	bool Init(IPubKernel* pPubKernel);
	bool CreateGuildInit(IPubData* guildData);
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg);
	bool LoadResConfig(IPubKernel*pPubKernel);
	
	//增加组织数值
	bool AddGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int addValue, GUILD_NUM_CHANGE desc, const wchar_t* playerName = NULL);
	//获得当前组织数值
	int GetGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType);
	//能够添加组织数值
	int CanAddGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType);
	//扣除组织数值
	bool DecGuildNumValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int decValue, GUILD_NUM_CHANGE desc, const wchar_t* playerName = NULL);
	//增加锁定数值
	bool AddGuildNumLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int lockValue);
	//获得锁定数值
	int GetGuildNumLockValue( const wchar_t* guildName, GUILD_NUM_TYPE guildNumType);
	//是否可以扣除
	 bool CanDecGuildNumValue( const wchar_t* guildName, GUILD_NUM_TYPE guildNumType,int value);
	//扣除锁定数值
	bool DecGuildNumLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int decValue);
	//解锁锁定数值
	bool GuildUnLockValue(const wchar_t* guildName, GUILD_NUM_TYPE guildNumType, int unLockValue);
	//获得类型最大限制数值
	int GetGuildNumUpperLimit(GUILD_NUM_TYPE type,const wchar_t* guildName);
	//组织建筑上限加成
	int GetGuildBuildingAditionUpperLimit(GUILD_NUM_TYPE type,const wchar_t*guildName);
	//每日重置
	int GuildNumDailyReset(IPubKernel* pPubKernel);
	//扣除健康度
	void DecJianKangDu(const wchar_t*guildName, int ret);

public:
	static GuildNumManage* m_pInstance;

private:



	enum
	{
		GUILD_NUM_BUY_SUCCES,// 购买成功
		GUILD_NUM_BUY_GUILD_CAPITAL_NO_ENOUGH,//钱不足
		GUILD_NUM_BUY_TIMES_NO_ENOUGH , //次数不足
		GUILD_NUM_BUY_REACH_MAX, //到达数值最大值
	};
	//健康度礼包
	void OnGetJianKangDuGif(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args);
		

		//购买组织数值
	void OnBuyGuildNum(IPubKernel* pPubKernel, int sourceId, int sceneId, const IVarList& args);

	//组织数值初始化
	bool LoadGuildNumInitConfig(IPubKernel*pPubKernel);
	//组织建筑提升组织数值上限
	bool LoadGuildBuildingAdditoinUpperLimit(IPubKernel*pPubKernel);
	//组织数值购买配置
	bool LoadGuildNumBuyCostConfig(IPubKernel*pPubKernel);
	//加载健康度配置
	bool LoadGuildJianKangDuConfig(IPubKernel*pPubKernel);
	//增加个人贡献组织资金
	void AddPersoinalGainCapital(IPubData*guildData, const wchar_t*playerName, int value);
	
	void SendGuildNumBuyResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		const wchar_t* playerName,int guildNum, int result,int value = 0);

	struct GuildNumInit{
		int m_initNum{0};
		int m_upperLimit{0};
	};
	std::map<int,GuildNumInit> m_initGuildNumCofig;

	struct GuildBuildingAddition{
		int m_GuidBuingType{ -1 };
		std::map<int, int> m_addition;
	};

	//组织数值类型，组织建筑类型，建筑等级
	std::map<int, GuildBuildingAddition> m_guildBuildingAddition;
	struct GuildNumBuyCost{
		int m_buyMaxTimes{0};
		int m_buyValue{0};
		int m_recoverValue{0};
		std::vector<int> m_buyCost;

	};

	//<组织数值类型
	std::map<int, GuildNumBuyCost> m_buyGuildNumConfig;

	//组织健康度
	struct GuildJianKangDuGif{
		int m_lower_limit;
		int m_upper_limit;
		std::string m_award;
	};
	struct GuildJianKangDu{
		int m_dailyRecoverValue{ 0 }; //每日恢复
		int m_needGuildCapital{ 0 };      //需要资金
		int m_maintainFailed{ 0 };      //维护失败扣除健康度
		int m_guildActivityFialed{ 0 }; //组织活动未开启扣除健康度
		int m_guildRecoverFailed{ 0 };   //恢复失败扣除健康度
		int m_extraAwardNeedGainCapital{0}; //每日获取额外礼包需要的组织贡献
		std::string m_extraAward;		//额外礼包

		std::vector<GuildJianKangDuGif> m_gif;
	};

	GuildJianKangDu m_guildJianKangDuConfig;


};






#endif