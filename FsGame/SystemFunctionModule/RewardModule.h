//---------------------------------------------------------
//文件名:       RewardModule.h
//内  容:       发放奖励给玩家
//说  明:       
//          
//创建日期:      2015年03月30日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef REWARD_MODULE_H_
#define REWARD_MODULE_H_

#include "Fsgame/Define/header.h"
#include "Fsgame/Define/GameDefine.h"
#include "public/IVarList.h"
#include "FsGame/Define/RewardDefine.h"
#include <map>
#include <vector>
#include "Fsgame/Define/FunctionEventDefine.h"

class ToolBoxModule;
class ContainerModule;
class CapitalModule;
class SystemMailModule;
class LevelModule;

class RewardModule : public ILogicModule
{
public:

	virtual bool Init(IKernel* pKernel);

	virtual bool Shut(IKernel* pKernel);

public:
	// 奖励数据
	struct AwardData 
	{
		std::string strType;	// 名字
		int			nAddVal;	// 增加的值
	};

	typedef std::vector<AwardData> AwardVec;

	struct AwardEx
	{
		AwardEx() : name(""), srcFunctionId(EmFunctionEventId(-1)) {}

		//int				nAwardId;	// 奖励id
		std::string		name;    // 奖励的名字，要跟邮件中配置(ini/mail/mail_config)的保持一致
		// 用于邮件发放奖励
		std::string		strCapitals;	
		std::string		strItems;

		// 直接发放奖励
		AwardVec		capitals;
		AwardVec		items;

		// ***注意：经验不会通过邮件发给玩家
		// 当玩家不在线的时候，经验会被丢弃
		AwardVec		exps;

		// NOTE: 所有的经验 物品 货币来源都统一使用	
		EmFunctionEventId srcFunctionId;		// 奖励来源功能ID(对应(EmFunctionEventId))
	};

	typedef std::map<int, AwardEx*> AwardMap;
public:
	// 尝试发奖励发给玩家
	bool TryRewardPlayer(IKernel *pKernel, const PERSISTID &player, int nAwardId, int operation_on_full);

    // 发奖励发给玩家
    // 当 @operation_on_full = REWARD_MAIL_ON_FULL 的时候, 会通过使用预定义的邮件发给玩家.
    // 如果邮件模板中有参数，则通过@mail_param 传送
	bool RewardPlayerById(IKernel *pKernel, const PERSISTID &player, 
		int nAwardId, int operation_on_full = REWARD_MAIL_ON_FULL, const IVarList &mail_param = CVarList());

	// 通过角色名发放奖励, 直接通过邮件发给角色
	// 其中, @award.name 是邮件模板的名字, 如果邮件模板中有参数，则通过@mail_param 传送
	//bool RewardByRoleName(IKernel *pKernel, const wchar_t *role_name, int nAwardId, const IVarList &mail_param = CVarList());

    // 通过邮件发奖励给玩家
	bool RewardViaMail(IKernel *pKernel, const wchar_t *role_name, int nAwardId, const IVarList &mail_param, const wchar_t *sender_name = L"");


	// 用于生成动态奖励,然后发放给玩家

	// 将奖励物品串解析为奖励数据
	// 格式：  "CapitalCopper:10,CapitalGold:10,CapitalSmelt:10,CapitalPublicOffice:10,CapitalBattleSoul:10,CapitalBlackShop:10,CapitalGuild:10"
	static bool GenerateAwardData(const std::string& strAward, RewardModule::AwardEx& award);

	// 发奖励发给玩家
	// 当 @operation_on_full = REWARD_MAIL_ON_FULL 的时候, 会通过使用预定义的邮件发给玩家.
	// 其中, @award.name 是邮件模板的名字, 如果邮件模板中有参数，则通过@mail_param 传送
	bool RewardPlayer(IKernel *pKernel, const PERSISTID &player, const AwardEx *award, const IVarList &mail_param, int operation_on_full = REWARD_MAIL_ON_FULL);

	//展示物品
	static void ShowItem(IKernel*pKernel,const PERSISTID &player,const std::string& awardStr);
private:
	// 读取全部奖励配置
	bool LoadAllRewardConfig(IKernel *pKernel);

	// 读取配置
	bool LoadRewardConfig(IKernel *pKernel, const char* strPath);

    // 通过邮件发送奖励
	bool InnerRewardViaMail(IKernel *pKernel, const wchar_t *role_name, const AwardEx* award, const IVarList &mail_param, const wchar_t *sender_name = L"");
    
    // 直接奖励给玩家
    bool InnerRewardDirectly(IKernel *pKernel, const PERSISTID &player, const AwardEx *award);

    // 经验可以直接加给玩家
	bool AddExp(IKernel *pKernel, const PERSISTID &player, const AwardVec& exp_list, int exp_from);

    // 资金可以直接加给玩家, @capital 的格式：capital_name:数量,capital_name:数量,
	bool AddCapital(IKernel *pKernel, const PERSISTID &player, const AwardVec& capital_list, EmFunctionEventId capitalEventID);
    
    // 是否可以保存到背包中
	bool CanPutInToolBox(IKernel *pKernel, const PERSISTID &player, const AwardVec& items_list, int nBindState);
    
    // 放入背包中
	bool PutInToolBox(IKernel *pKernel, const PERSISTID &player, const AwardVec& item_list, EmFunctionEventId itemEventID, int nBindState);

	// 将奖励物品串解析为奖励数据
	// 格式：  "CapitalCopper:10,CapitalGold:10,CapitalSmelt:10,CapitalPublicOffice:10,CapitalBattleSoul:10,CapitalBlackShop:10,CapitalGuild:10"
	static void ParseAddAwardData(const char* strAwardData, AwardVec& vecAward);

	// 增加一项奖励 nAwardType (对应AwardType)
	static void ParseAddOneAwardData(const char* strAward, int nAwardType, AwardEx& award);

	// 查询奖励数据
	const AwardEx* RewardModule::QueryAwardById(int nAwardId);

	static bool ItemsToString(const AwardVec &items_list, std::string& strItems, const char* pszSp1 = ",", const char* pszSp2 = ":");
	static bool CapitalsToString(const AwardVec &capital_list, std::string& strCapitals, const char* pszSp1 = ",", const char* pszSp2 = ":");
public:
	// 将物品串中其他职业的物品排除
	// 格式：  "ConfigID:数量,ConfigID:数量"
// 	static bool FilterAwardByJob(IKernel* pKernel, const PERSISTID& self, AwardVec& items);
// 	static bool FilterAwardByJob(IKernel* pKernel, int job, AwardVec& items);

public:

	static RewardModule *m_pRewardInstance;
    
private:
	AwardMap		m_mapAllAwardData;		// 所有奖励数据

	static ContainerModule *m_pContainerModule;
	static CapitalModule *m_pCapitalModule;
	static SystemMailModule *m_pMailModule;
    static LevelModule *m_pLevelModule;
};

#endif