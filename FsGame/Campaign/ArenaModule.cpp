//---------------------------------------------------------
//文件名:       ArenaModule.cpp
//内  容:       竞技场
//说  明:       
//          
//创建日期:      2015年06月25日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#include "ArenaModule.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "public/VarList.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"

#include "FsGame/Define/ArenaModuleDefine.h"
#include "FsGame/SystemFunctionModule/RankListModule.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
//#include "FsGame/SystemFunctionModule/CopyOffLineModule.h"
#include "FsGame/CommonModule/LandPosModule.h"
#include "FsGame/SystemFunctionModule/RewardModule.h"
#include "FsGame/SystemFunctionModule/ResetTimerModule.h"
#include "FsGame/CommonModule/SnsDataModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/CommonModule/ActionMutex.h"

#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/CapitalDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/FightNpcDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/PlayerBaseDefine.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"

#include <sstream>
#include <algorithm>
#include "../Define/CoolDownDefine.h"
#include "../SystemFunctionModule/CoolDownModule.h"
#include "CommonModule/ReLoadConfigModule.h"
#include "Define/SnsDefine.h"
#include "Define/RankingDefine.h"
#include "utils/exptree.h"
#include "system/WinFileSys.h"

// 候选人数组大小上限
#define ARENA_CANDIDATE_MAX_SIZE   500

// 待选挑战者数组大小上限
#define ARENA_CHALLENGEE_MAX_SIZE  100

#define ARENA_CAMP_DEFAULT      0 // 没有阵营
#define ARENA_CAMP_CHALLENGER   1 // 挑战者阵营
#define ARENA_CAMP_CHALLENGEE   2 // 被挑战者阵营

#define ARENA_NO_RANK           -1 // 玩家没有上榜

#define TEXT_ARENA_RANK_COL_UID     "player_uid"
#define TEXT_ARENA_RANK_COL_SN   "player_arena_sn"
#define TEXT_ARENA_RANK_COL_NATION  "player_nation"

// 是否第一次挑战竞技场
#define TEXT_ARENA_TIMES                  "ArenaTimes"
// 竞技场的机会的刷新时间
#define TEXT_ARENA_CHANCE_RESET_DATE      "ArenaChanceResetDate"
// 竞技场的机会数
#define TEXT_ARENA_CHALLENGE_CHANCE       "ArenaChance"
// 已经使用的机会数
#define TEXT_ARENA_CHALLENGE_CHANCE_USED  "ArenaChanceUsed"
// 竞技场的对手信息
#define TEXT_ARENA_CHALLENGEE_INFO  "ArenaChallengeeInfo"
// 竞技场的对手
#define TEXT_ARENA_CHALLENGEE_OBJ   "ArenaChallengeeObj"
// 竞技场的标识
#define TEXT_ARENA_FLAG             "ArenaFlag"
// 新晋升的竞技场排行榜
//#define TEXT_ARENA_NEW_RANK_LIST    "ArenaNewRankList"
// 玩家当前的竞技场排行榜
#define TEXT_ARENA_CUR_RANK_LIST    "ArenaCurvRankList"
// 对手的名字
#define TEXT_ARENA_CHALLENGEE_NAME  "ArenaChallengeeName"
// 候选人列表
#define TEXT_ARENA_CANDIDATE_LIST   "ArenaCandidateList"
// 挑战开始时间
#define TEXT_ARENA_CHALLENGE_START_TIME "ChallengeStartTime"
#define TEXT_ARENA_CHALLENGE_END_TIME "ChallengeEndTime"
//历史最好排名
#define TEXT_HIGHEST_ARENA_RANK         "HighestArenaRank"
// 排名提升奖励
#define TEXT_RANK_UP_REWARD             "RankUpReward"
// 竞技场场次编号
#define TEXT_ARENA_SN					"ArenaSN"

// 配置文件
#define ARENA_CONFIG_FILE           "ini/Campaign/Arena/ArenaConfig.xml"
#define ARENA_CANDIDATE_RULE_FILE   "ini/Campaign/Arena/CandidateRule.xml"
#define ARENA_CHALLENGE_REWARD_FILE "ini/Campaign/Arena/ChallengeReward.xml"
#define ARENA_RANK_RWARD_FILE       "ini/Campaign/Arena/RankReward.xml.xml"
#define ARENA_RANK_LIST_FILE        "ini/Campaign/Arena/RankList.xml"
#define ARENA_PROMOTION_REWARD_FILE "ini/Campaign/Arena/PromotionReward.xml"
#define ARENA_RANK_UP_REWARD_FILE   "ini/Campaign/Arena/RankUpReward.xml"

ArenaModule* ArenaModule::m_pArenaModule = NULL;
RankListModule * ArenaModule::m_pRankListModule = NULL;
CapitalModule * ArenaModule::m_pCapitalModule = NULL;
LevelModule * ArenaModule::m_pLevelModuel = NULL;
CopyOffLineModule * ArenaModule::m_pCopyOfflineModule = NULL;
LandPosModule * ArenaModule::m_pLandPosModule = NULL;
RewardModule * ArenaModule::m_pRewardModule = NULL;
VipModule * ArenaModule::m_pVipModule = NULL;
DynamicActModule * ArenaModule::m_pDynamicActModule = NULL;
LogModule * ArenaModule::m_pLogModule = NULL;

ArenaModule::ArenaConfig ArenaModule::m_ArenaConfig;
std::vector<ArenaModule::CandidateRule> ArenaModule::m_CandidateRule;      // 竞技场候选人规则
std::vector<ArenaModule::ChallengeReward> ArenaModule::m_ChanllengeReward; // 竞技场挑战奖励
ArenaModule::RankRewardMap ArenaModule::m_RankReward;                      // 竞技场排名奖励
std::vector<ArenaModule::ArenaRankListConfig> ArenaModule::m_ArenaRankList;      // 竞技场的排行榜
ArenaModule::PromotionRewardMap ArenaModule::m_PromotionReward;             // 晋级奖励
ArenaModule::RankUpRewardMap ArenaModule::m_RankUpReward;               // 名次提升奖励

std::wstring ArenaModule::m_domainName = L"";

int nx_reload_arena(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == ArenaModule::m_pArenaModule)
    {
        return 0;
    }

    ArenaModule::m_pArenaModule->LoadResource(pKernel);

    return 1;
}

int nx_challenge(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == ArenaModule::m_pArenaModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_challenge, 1);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_challenge, 1);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);

	if (!pKernel->FindData(player, TEXT_ARENA_CANDIDATE_LIST))
	{
		return 0;
	}

	std::wstring wstr_name_list = pKernel->QueryDataWideStr(player, TEXT_ARENA_CANDIDATE_LIST);

	CVarList temp_name_list;
	util_split_wstring(temp_name_list, wstr_name_list, L"$");
    
    // 取第一个人进行挑战
	const wchar_t *challengee_name = temp_name_list.WideStrVal(0);
    
	ArenaModule::m_pArenaModule->OnChallengeSomeone(pKernel, player, PERSISTID(), CVarList() << CLIENT_CUSTOMMSG_ARENA_MSG << ARENA_C2S_CHALLENGE_SOMEONE << challengee_name);

    return 1;
}

// 获取挑战候选人
int nx_get_candidate(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == ArenaModule::m_pArenaModule)
	{
		return 0;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_candidate, 1);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_candidate, 1);

	// 获取参数
	PERSISTID player = pKernel->LuaToObject(state, 1);

	// 获取可挑战的候选人
	ArenaModule::m_pArenaModule->OnGetChallengeCandidate(pKernel, player, PERSISTID(), CVarList());

	return 1;
}

int nx_arena_reward(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == ArenaModule::m_pArenaModule)
    {
        return 0;
    }
    
    ArenaModule::m_pArenaModule->RewardByArenaRank(pKernel);

    return 1;
}

// 重置玩家竞技场状态
int nx_reset_arena_state(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == ArenaModule::m_pArenaModule)
	{
		return 0;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_reset_arena_state, 1);

	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_reset_arena_state, 1);

	// 获取参数
	const wchar_t* strName = pKernel->LuaToWideStr(state, 1);

	ArenaModule::m_pArenaModule->ResetArenaStateByGm(pKernel, strName, UNLOCK_ARENA_STATE);

	return 1;
}

// 锁定玩家竞技场状态
int nx_lock_arena_state(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == ArenaModule::m_pArenaModule)
	{
		return 0;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_lock_arena_state, 1);

	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_lock_arena_state, 1);

	// 获取参数
	const wchar_t* strName = pKernel->LuaToWideStr(state, 1);

	ArenaModule::m_pArenaModule->ResetArenaStateByGm(pKernel, strName, LOCK_ARENA_STATE);

	return 1;
}

//清理竞技场重复数据
int nx_clear_arena_repeatinfo(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	if (NULL == ArenaModule::m_pArenaModule)
	{
		return 0;
	}

	ArenaModule::m_pArenaModule->PlayerRanklistClean(pKernel);

	return 1;
}

int nx_import_arena_list(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_import_arena_list, 2);

	// 检查参数类型
	CHECK_ARG_INT(state, nx_import_arena_list, 1);
	CHECK_ARG_INT(state, nx_import_arena_list, 2);

	int nStartRow = pKernel->LuaToInt(state, 1);
	int nEndRow = pKernel->LuaToInt(state, 2);

	return 1;
}

// 初始化函数
bool ArenaModule::Init(IKernel* pKernel)
{
	m_pArenaModule = this;	
    m_pRankListModule = (RankListModule *)pKernel->GetLogicModule("RankListModule");
    m_pCapitalModule = (CapitalModule *)pKernel->GetLogicModule("CapitalModule");
    m_pLevelModuel = (LevelModule *)pKernel->GetLogicModule("LevelModule");
    m_pCopyOfflineModule = (CopyOffLineModule *)pKernel->GetLogicModule("CopyOffLineModule");
    m_pLandPosModule = (LandPosModule *)pKernel->GetLogicModule("LandPosModule");
    m_pRewardModule = (RewardModule *)pKernel->GetLogicModule("RewardModule");
    m_pVipModule = (VipModule *)pKernel->GetLogicModule("VipModule");
	m_pDynamicActModule = (DynamicActModule *)pKernel->GetLogicModule("DynamicActModule");

    Assert(m_pRankListModule && m_pCapitalModule && m_pLevelModuel && m_pCopyOfflineModule
           && m_pLandPosModule && m_pRewardModule && m_pVipModule && m_pDynamicActModule);
	
	pKernel->AddEventCallback("player", "OnRecover", ArenaModule::OnRecover);
    pKernel->AddEventCallback("player", "OnAfterEntryScene", ArenaModule::OnAfterEntryScene);
    pKernel->AddEventCallback("player", "OnReady", ArenaModule::OnReady);
    pKernel->AddEventCallback("player", "OnLeaveScene", ArenaModule::OnLeaveScene);
	pKernel->AddEventCallback("player", "OnContinue", ArenaModule::OnPlayerContinue);
    // 来自公共服的消息
    pKernel->AddEventCallback("scene", "OnPublicMessage", ArenaModule::OnPublicMessage);

	// 接受SNS服务器消息
	pKernel->AddEventCallback("scene", "OnSnsMessage", ArenaModule::OnSnsMessage);

	// 来自公共服的消息
	pKernel->AddEventCallback("scene", "OnCreate", ArenaModule::OnCreateScene);
    
	// 客户端消息回调
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_ARENA_MSG, ArenaModule::OnCustomMessage);
	
    // 购买机会
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_BUY_INFO, ArenaModule::OnBuyArenaChance);
	
    pKernel->AddIntCommandHook("player", COMMAND_ARENA_PLAYER, ArenaModule::OnPlayerCommand);

    // 玩家OnContinue 后的 Ready
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_CONTINUE_ON_READY, ArenaModule::OnContinueReady);
    
    pKernel->AddIntCommandHook("player", COMMAND_KILLTARGET, ArenaModule::OnKillObject);
    pKernel->AddIntCommandHook("player", COMMAND_BEKILL, ArenaModule::OnBeKilled);

    // 玩家等级变化 
    pKernel->AddIntCommandHook("player", COMMAND_LEVELUP_CHANGE, OnPlayerLevel);

	LoadResource(pKernel);
	
	DECL_LUA_EXT(nx_reload_arena);
    DECL_LUA_EXT(nx_challenge);
    DECL_LUA_EXT(nx_arena_reward);
	DECL_LUA_EXT(nx_reset_arena_state);
	DECL_LUA_EXT(nx_lock_arena_state);
	DECL_LUA_EXT(nx_clear_arena_repeatinfo);
	DECL_LUA_EXT(nx_get_candidate);
	DECL_LUA_EXT(nx_import_arena_list);

	//玩家VIP等级变化回调
	DECL_CRITICAL(ArenaModule::C_OnVIPLevelChanged);
	DECL_RESET_TIMER(RESET_ARENA_CHANCE_TIMES, ArenaModule::ResetArenaChanceTimer);
    DECL_HEARTBEAT(ArenaModule::HB_BeforeFight);
    DECL_HEARTBEAT(ArenaModule::HB_RoundCheck);
    DECL_HEARTBEAT(ArenaModule::HB_CloseWait);
    DECL_HEARTBEAT(ArenaModule::HB_LeaveScene);
    
	RELOAD_CONFIG_REG("ArenaConfig", ArenaModule::ReloadConfig);
	return true;
}

// 资源释放函数
bool ArenaModule::Shut(IKernel* pKernel)
{
	return true;
}

// 加载资源
bool ArenaModule::LoadResource(IKernel* pKernel)
{
    LoadArenaConfig(pKernel, ARENA_CONFIG_FILE);
    
    LoadCandidateConfig(pKernel, ARENA_CANDIDATE_RULE_FILE);
    
    LoadChallengeRewardConfig(pKernel, ARENA_CHALLENGE_REWARD_FILE);
    
    LoadRankRewardConfig(pKernel, ARENA_RANK_RWARD_FILE);
    
    LoadRankListConfig(pKernel, ARENA_RANK_LIST_FILE);
    
    LoadPromotionReward(pKernel, ARENA_PROMOTION_REWARD_FILE);
    
    LoadRankUpReward(pKernel, ARENA_RANK_UP_REWARD_FILE);
    
	return true;
}

// 竞技场配置
bool ArenaModule::LoadArenaConfig(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //
    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile()) 
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    if (sec_count == 0)
    {
        return false;
    }
    
    // 依次读取配置信息
    const char *sec = sec_list.StringVal(0);

    m_ArenaConfig.open_level        = xml.ReadInteger(sec, "OpenLevel", 0);
    m_ArenaConfig.buy_chance_limit      = xml.ReadInteger(sec, "BuyChanceLimit", 0);
    m_ArenaConfig.chosen_count      = xml.ReadInteger(sec, "ChosenCount", 0);
    m_ArenaConfig.free_chance       = xml.ReadInteger(sec, "FreeChance", 0);
	m_ArenaConfig.challenge_cd		= xml.ReadInteger(sec, "ChallengeCD", 0) * 1000;
	m_ArenaConfig.challenge_clear_cd_fee = xml.ReadInteger(sec, "ClearCDFee", 10);
    std::string chance_fee          = xml.ReadString(sec, "ChanceFee", "");
    m_ArenaConfig.scene_id          = xml.ReadInteger(sec, "SceneID", 701);
    m_ArenaConfig.npc_config        = xml.ReadString(sec, "NpcConfig", "Monster_50002");
    std::string challenger_born     = xml.ReadString(sec, "ChallengerBorn", "");
    std::string challengee_born     = xml.ReadString(sec, "ChallengeeBorn", "");
    m_ArenaConfig.count_down        = xml.ReadInteger(sec, "Countdown", 0);
    m_ArenaConfig.round_time        = xml.ReadInteger(sec, "RoundTime", 0);
    m_ArenaConfig.wait_before_close = xml.ReadInteger(sec, "WaitBeforeClose", 0);
    m_ArenaConfig.rank_reward_name  = xml.ReadString(sec, "RankReward", "");
    m_ArenaConfig.challenger_win    = xml.ReadString(sec, "ChallengerWin", "");
    m_ArenaConfig.challenger_failed = xml.ReadString(sec, "ChallengerFailed", "");
    m_ArenaConfig.challengee_win    = xml.ReadString(sec, "ChallengeeWin", "");
    m_ArenaConfig.challengee_failed = xml.ReadString(sec, "ChallengeeFailed", "");
	m_ArenaConfig.nBroadRank		= xml.ReadInteger(sec, "BroadRank", 5);

    // @chance_fee 的格式: fee,fee,...fee
    bool parse_success = ParseFee(chance_fee, m_ArenaConfig.chance_fee, sizeof(m_ArenaConfig.chance_fee)/sizeof(int));
    if (!parse_success)
    {
        extend_warning(LOG_ERROR, "%s [ChanceFee] error", file_name);
    }
    
    // 解析坐标, 坐标的格式： x,y,z,o
    parse_success = ParseCoordinate(challenger_born, m_ArenaConfig.challenger_born);
    if (!parse_success)
    {
        extend_warning(LOG_ERROR, "%s [ChallengerBorn] error", file_name);
    }

    parse_success = ParseCoordinate(challengee_born, m_ArenaConfig.challengee_born);
    if (!parse_success)
    {
        extend_warning(LOG_ERROR, "%s [ChallengeeBorn] error", file_name);
    }   

    return true;
}

// 候选人规则
bool ArenaModule::LoadCandidateConfig(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //
    
    m_CandidateRule.clear();
    
    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

	LoopBeginCheck(a)
    for (int i=0; i<sec_count; ++i)
    {
		LoopDoCheck(a)
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        CandidateRule the_rule;
        the_rule.lower_rank      = xml.ReadInteger(sec, "LowerRank", 0);
        the_rule.upper_rank      = xml.ReadInteger(sec, "UpperRank", 0);
        the_rule.candidate_count = xml.ReadInteger(sec, "CandidateCount", 0);
        
        m_CandidateRule.push_back(the_rule);
    }

    return true;
}

// 挑战奖励设置
bool ArenaModule::LoadChallengeRewardConfig(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //
    
    m_ChanllengeReward.clear();
    
    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(b)
    for (int i=0; i<sec_count; ++i)
    {
        LoopDoCheck(b)
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        ChallengeReward the_reward;
        the_reward.lower_level      = xml.ReadInteger(sec, "LowerLevel", 0);
        the_reward.upper_level      = xml.ReadInteger(sec, "UpperLevel", 0);
		the_reward.winner_award.str_exp = xml.ReadString(sec, "WinnerExp", "");
        the_reward.winner_award.str_capital = xml.ReadString(sec, "WinnerCapital", "");
        the_reward.winner_award.str_item    = xml.ReadString(sec, "WinnerItem", "");
		the_reward.loser_award.str_exp = xml.ReadString(sec, "LoserExp", "");
        the_reward.loser_award.str_capital = xml.ReadString(sec, "LoserCapital", "");
        the_reward.loser_award.str_item    = xml.ReadString(sec, "LoserItem", "");

        m_ChanllengeReward.push_back(the_reward);
    }
    
    return true;
}

// 排名奖励设置
bool ArenaModule::LoadRankRewardConfig(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //
    
    m_RankReward.clear();
    
    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(c)
    for (int i=0; i<sec_count; ++i)
    {
        LoopDoCheck(c)
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        RankReward the_reward;
        the_reward.rank_list_name    = xml.ReadString(sec, "RankList", "");
        the_reward.lower_rank        = xml.ReadInteger(sec, "LowerRank", 0);
        the_reward.upper_rank        = xml.ReadInteger(sec, "UpperRank", 0);
        //the_reward.award.exp         = xml.ReadInteger(sec, "Exp", 0);
        the_reward.award.str_capital = xml.ReadString(sec, "Capital", "");
        the_reward.award.str_item    = xml.ReadString(sec, "Item", "");

        RankRewardMap::iterator it_find = m_RankReward.find(the_reward.rank_list_name);
        if (it_find == m_RankReward.end())
        {
            std::vector<RankReward> the_reward_list;
            the_reward_list.push_back(the_reward);
            
            m_RankReward.insert(std::make_pair(the_reward.rank_list_name, the_reward_list));
        }
        else
        {
            std::vector<RankReward> &the_reward_list = it_find->second;
            the_reward_list.push_back(the_reward);
        }
    }

    return true;
}


// 载入排行榜设置
bool ArenaModule::LoadRankListConfig(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //
    
    m_ArenaRankList.clear();
    
    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(d)
    for (int i=0; i<sec_count; ++i)
    {
        LoopDoCheck(d)
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        ArenaRankListConfig the_def;
        the_def.lower_level       = xml.ReadInteger(sec, "LowerLevel", 0);
        the_def.upper_level       = xml.ReadInteger(sec, "UpperLevel", 0);
        the_def.name              = xml.ReadString(sec, "Name", "");
        the_def.level             = xml.ReadInteger(sec, "Level", 0);
        the_def.need_notify       = xml.ReadInteger(sec, "NeedNotify", 0);

        m_ArenaRankList.push_back(the_def);
    }

    return true;
}

// 载入晋级奖励配置
bool ArenaModule::LoadPromotionReward(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //

    m_PromotionReward.clear();

    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(e)
    for (int i=0; i<sec_count; ++i)
    {
        LoopDoCheck(e)
        
        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        PromotionReward the_reward;
        the_reward.rank_list_name    = xml.ReadString(sec, "RankListName", "");
        the_reward.reward_name       = xml.ReadString(sec, "RewardName", "");
        the_reward.capital           = xml.ReadString(sec, "Capital", "");
        the_reward.item              = xml.ReadString(sec, "Item", "");

        m_PromotionReward.insert(std::make_pair(the_reward.rank_list_name, the_reward));
    }

    return true;
}


// 名次提升奖励
bool ArenaModule::LoadRankUpReward(IKernel* pKernel, const char *file_name)
{
    // 配置文件的格式：
    //
    //  <Property   ID="" ... />
    //

    m_RankUpReward.clear();

    std::string the_config_file = pKernel->GetResourcePath();
    the_config_file.append(file_name);

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int sec_count = (int)sec_list.GetCount();

    LoopBeginCheck(e)
    for (int i=0; i<sec_count; ++i)
    {
        LoopDoCheck(e)

        // 依次读取配置信息
        const char *sec = sec_list.StringVal(i);

        RankUpReward the_reward;
        the_reward.rank_list_name   = xml.ReadString(sec, "RankListName", "");
        the_reward.lower_rank       = xml.ReadInteger(sec, "LowerRank", 0);
        the_reward.upper_rank       = xml.ReadInteger(sec, "UpperRank", 0);
		the_reward.award.str_capital = xml.ReadString(sec, "CapitalReward", "");
		the_reward.award.str_item = xml.ReadString(sec, "ItemReward", "");

        RankUpRewardMap::iterator it = m_RankUpReward.find(the_reward.rank_list_name);
        
        if (it == m_RankUpReward.end())
        {
            std::vector<RankUpReward> reward_list;
            reward_list.push_back(the_reward);
            
            m_RankUpReward.insert(std::make_pair(the_reward.rank_list_name, reward_list));
        }
        else
        {
            std::vector<RankUpReward> &reward_list = it->second;
            reward_list.push_back(the_reward);
        }
    }

    return true;
}

// 获取domain的名字
const std::wstring& ArenaModule::GetDomainName(IKernel * pKernel)
{
    if (m_domainName.empty())
    {
        wchar_t wstr[256];
        const int server_id = pKernel->GetServerId();
        SWPRINTF_S(wstr, L"Domain_Arena_%d", server_id);

        m_domainName = wstr;
    }

    return m_domainName;
}

// 获取玩家的名次, 返回-1表示没有入排行榜
bool ArenaModule::GetArenaRank(IKernel* pKernel, const PERSISTID& player, CVarList &result)
{
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }
    
    int role_level = player_obj->QueryInt("Level");
    
    const ArenaRankListConfig *the_config = NULL;
    std::vector<ArenaRankListConfig>::const_iterator it = m_ArenaRankList.begin();
    std::vector<ArenaRankListConfig>::const_iterator it_end = m_ArenaRankList.end();

    LoopBeginCheck(f)
    for (; it!=it_end; ++it)
	{
	    LoopDoCheck(f)
        const ArenaRankListConfig &temp_config = *it;

        if (role_level >= temp_config.lower_level && role_level <= temp_config.upper_level)
        {
            the_config = &temp_config;
            break;
        }
    }
    
    if (the_config == NULL)
    {
        return false;
    }

    int rank = -1;  
    IRecord *rank_rec = GetArenaRankListRecord(pKernel, the_config->name.c_str());
    if (rank_rec != NULL)
    {
        const char *role_uid = player_obj->QueryString("Uid");
        int row = rank_rec->FindString(ARENA_RANK_COL_UID, role_uid);

        if (row == -1)
        {
            rank = -1;
        }
        else
        {
            rank = row + 1; // @row 从0开始，@rank_no 从1开始 
        }
    }
    
    result << rank << the_config->lower_level << the_config->upper_level;
    
    return true;
}


// GM 的重置命令, 仅仅用于GM
void ArenaModule::GMReset(IKernel* pKernel, const PERSISTID& player)
{
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return;
    }
    
    player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE, m_ArenaConfig.free_chance);
    player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE_USED, 0);
}

int ArenaModule::OnRecover(IKernel* pKernel, const PERSISTID& self,
                           const PERSISTID& sender, const IVarList& args)
{
    //pKernel->AddCritical(self, "Level", "ArenaModule::OnPlayerLevel");
    
   REGIST_RESET_TIMER(pKernel, self, RESET_ARENA_CHANCE_TIMES);
    
    int player_level = pKernel->QueryInt(self, "Level");
    
    if (player_level < m_ArenaConfig.open_level)
    {
        return 0;    
    }
    
    ResetArenaChance(pKernel, self);

// 	IGameObj* pPlayer = pKernel->GetGameObj(self);
// 	if (NULL == pPlayer)
// 	{
// 		return 0;
// 	}
// 
// 	// 不在榜情况竞技场属性（处理合服后数据保护）  等有合服功能时,加在OnReady bFirst的处理中
// 	CVarList arena_rank;
// 	GetArenaRank(pKernel, self, arena_rank);
// 	int find_rank = arena_rank.IntVal(0); 
// 	if (find_rank == -1)
// 	{		
// 		int nLevel = pPlayer->QueryInt("Level");
// 		// 合服后,竞技场排行榜数据被清空,玩家有可能不再排行榜中，尝试把玩家存入排行榜
// 		const char * rank_name = GetRankListName(pKernel, nLevel);
// 		if (StringUtil::CharIsNull(rank_name))
// 		{
// 			return 0;
// 		}
// 		const wchar_t* player_name = pPlayer->QueryWideStr("Name");
// 		const char* player_uid = pPlayer->QueryString("Uid");
// 		CVarList rank_list_row;
// 		if (!PrepareDataForArenaRankList(pKernel, self, rank_name, rank_list_row))
// 		{
// 			return 0;
// 		}
// 		CVarList the_msg;
// 		the_msg << ARENA_PUB_SUB_MSG_ADD_TO_RANKLIST;
// 		the_msg << player_name << player_uid << rank_name << rank_list_row;
// 
// 		SendArenaMsgToPublic(pKernel, the_msg);
// 	}

	// VIPLevel回调
	if (!pKernel->FindCritical(self, "VipLevel", "ArenaModule::C_OnVIPLevelChanged"))
	{
		pKernel->AddCritical(self, "VipLevel", "ArenaModule::C_OnVIPLevelChanged");
	}

	return 0;
}

//进入场景
int ArenaModule::OnAfterEntryScene(IKernel * pKernel, const PERSISTID & self,
                             const PERSISTID & sender, const IVarList & args)
{
#if defined(CREATE_ROBOT) 
	OnCreateRobot(pKernel, self, sender, args);
#endif
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }

    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

	ActionMutexModule::NoteAction(pKernel, self, ACTION_ARENA);
    
    // 确保是挑战逻辑
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    if (arena_flag != ARENA_FLAG_ENTERING)
    {
        return 0;
    }
    
    // 记录下玩家使用了一次竞技场机会
    int chance_used = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE_USED);
    ++chance_used;
    player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE_USED, chance_used);

    // 对手信息
    std::string challengee_info_str = player_obj->QueryString(TEXT_ARENA_CHALLENGEE_INFO);
    
    CVarList challengee_info;
    util_split_string(challengee_info, challengee_info_str, ",");
    
    const char *challengee_uid = challengee_info.StringVal(0);
    int challengee_nation = atoi(challengee_info.StringVal(1));
    
    // 分组场景设置
    int scene_group_id = pKernel->NewGroupId();
    player_obj->SetInt("GroupID" , scene_group_id);
    
    // 设挑战者阵营
    player_obj->SetInt( "Camp", ARENA_CAMP_CHALLENGER);
    
    // 冻结玩家
    FreezePlayer(pKernel, self);    
    
    // 创建对手NPC, 并附带分组ID和数据来源(从哪个玩家拷贝数据)
    CVarList npc_args;
    npc_args << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << scene_group_id
             << "CopyFrom" << challengee_uid << "Nation" << challengee_nation
             << "Camp" << ARENA_CAMP_CHALLENGEE;
    
    len_t y = pKernel->GetMapHeight(m_ArenaConfig.challenger_born.x, 
        m_ArenaConfig.challenger_born.z);
        
    PERSISTID challengee_npc = pKernel->CreateObjectArgs("", m_ArenaConfig.npc_config.c_str(), 0,
        m_ArenaConfig.challengee_born.x, y,
        m_ArenaConfig.challengee_born.z,
        m_ArenaConfig.challengee_born.o, npc_args);
        
    if (pKernel->Exists(challengee_npc))
    {
        IGameObj *challengee_obj = pKernel->GetGameObj(challengee_npc);
        if (challengee_obj != NULL)
        {
            // 设置被挑战者的属性并冻结被挑战玩家
            challengee_obj->SetInt("PurposeType", ARENA_BATTLE_NPC);
            challengee_obj->SetInt("Level", player_obj->QueryInt("Level")); // 设置NPC的初始等级为玩家等级
            challengee_obj->SetInt("Nation", challengee_nation);
        }
        
        FreezePlayer(pKernel, challengee_npc);
    }
    
    // 如果对手创建失败, 则给玩家一个空的对象，然后立刻结束竞技场战斗
    
    // 设定准备就绪
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_READY);
    
    // 记录下战斗对象
    player_obj->SetObject(TEXT_ARENA_CHALLENGEE_OBJ, challengee_npc);
    
    // 把玩家HP恢复到100%
    player_obj->SetInt64("HP", pKernel->QueryInt64(self, "MaxHP"));
        
    return 0;
}

 // 就绪
int ArenaModule::OnReady(IKernel * pKernel, const PERSISTID & self,
                         const PERSISTID & sender, const IVarList & args)
{
	bool firstentry = args.BoolVal(0);
	if (firstentry) // 首次进入,查询离线的竞技场战报
	{
		OnPlayerContinue(pKernel, self, sender, args);
	}
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }

    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 确保是挑战逻辑
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    if (arena_flag != ARENA_FLAG_READY)
    {
        // 把玩家送出竞技场
        GoToCity(pKernel, self);
        
        return 0;
    }
    
    // 设定为战斗中
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_IN_BATTLE);
    player_obj->SetInt("Camp", ARENA_CAMP_CHALLENGER);

    // 开始倒计时
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_BeforeFight");
    pKernel->AddCountBeat(self, "ArenaModule::HB_BeforeFight", m_ArenaConfig.count_down * 1000, 1);

    // 通知客户端倒计时开始
    CVarList msg_for_client;
    msg_for_client << SERVER_CUSTOMMSG_ARENA_MSG;
	msg_for_client << ARENA_S2C_COUNTDOWN_BEFORE_FIGHT;
    msg_for_client << m_ArenaConfig.count_down; // 倒计时

    pKernel->Custom(self, msg_for_client);
    
    return 0;
}

int ArenaModule::OnContinueReady(IKernel * pKernel, const PERSISTID & self,
                           const PERSISTID & sender, const IVarList & args)
{
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }

    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
//     if (arena_flag == ARENA_FLAG_NOT_ARENA)
//     {
//         // 非挑战状态, 把玩家送出竞技场
//         GoToCity(pKernel, self);
//         return 0;
//     }

    if (arena_flag != ARENA_FLAG_END)
    {
        // 此时还没有分出胜负, 强制结束挑战
        player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);
        
        // 移除全部心跳
        DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_BeforeFight");
        DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_RoundCheck");
        DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_CloseWait");
        
        // 结束前离开竞技场, 判定为掉线逃跑
        OnChallengeEnd(pKernel, self, ARENA_RET_CHALLENGE_LOSE, ARENA_RET_ESCAPE);

        // 接下来 把玩家传出竞技场
        WaitThenLeaveArena(pKernel, self);
    }

    return 0;
}

// 玩家离开场景
int ArenaModule::OnLeaveScene(IKernel *pKernel, const PERSISTID &self,
                        const PERSISTID &sender, const IVarList &args)
{ 
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }
  
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

	ActionMutexModule::CleanAction(pKernel, self);

    // 竞技场状态
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    
    // 移除全部定时心跳
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_BeforeFight");
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_CloseWait");
    
    if (arena_flag != ARENA_FLAG_NOT_ARENA && arena_flag != ARENA_FLAG_END)
    {
        // 结束前离开竞技场, 判定为掉线逃跑
        player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);
        OnChallengeEnd(pKernel, self, ARENA_RET_CHALLENGE_LOSE, ARENA_RET_ESCAPE);
    }
    
    int scene_group_id = player_obj->QueryInt("GroupID");
    
    // 解锁挑战双方
    UnlockPlayer(pKernel, self);
    
    // 清除所有挑战状态
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_NOT_ARENA);
    player_obj->SetString(TEXT_ARENA_CHALLENGEE_INFO, "");
    player_obj->SetInt("Camp", ARENA_CAMP_DEFAULT);
    player_obj->SetDataWideStr(TEXT_ARENA_CANDIDATE_LIST, L"");
    player_obj->SetInt("GroupID", -1);
    
    if (scene_group_id > 0)
    {
		pKernel->ClearGroup(scene_group_id);
    }
    
    // 恢复玩家HP
    player_obj->SetInt64("HP", player_obj->QueryInt64("MaxHP"));
    player_obj->SetInt("Dead", 0);

	UnfreezePlayer(pKernel, self);
    
    return 0;
}

int ArenaModule::OnPlayerContinue(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{ 
		return 0;
	}

	const char* strUid = pSelfObj->QueryString(FIELD_PROP_UID);
	pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
		pKernel->GetSceneId(), strUid,
		CVarList() << SPACE_SNS
		<< SNS_ARENA_SPACE
		<< SNS_MSG_ARENA_QUERY_FIGHT_RESULT
		<< self);

	return 0;
}

//接收sns消息
int ArenaModule::OnSnsMessage(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args)
{
	int msgid = args.IntVal(0);
	switch (msgid)
	{
	case SNS_MSG_ARENA_PUSH_FIGHT_RESULT:
		OnSnsQueryFightResult(pKernel, args);
		break;
	}
	return 0;
}

// 查询战报回复
int ArenaModule::OnSnsQueryFightResult(IKernel* pKernel, const IVarList& args)
{
	int nIndex = 1;
	PERSISTID player = args.ObjectVal(nIndex++);
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord* pResultRec = pSelfObj->GetRecord(FIELD_RECORD_ARENA_FIGHT_RESULT);
	if (NULL == pResultRec)
	{
		return 0;
	}
	int nRows = args.IntVal(nIndex++);
	if (nRows > 0)
	{
		// 通知玩家显示红点
		pKernel->Custom(player, CVarList() << SERVER_CUSTOMMSG_ARENA_MSG << ARENA_S2C_FIGHT_RESULT_CHANGE_TIP);
	}
	LoopBeginCheck(g);
	for (int i = 0; i < nRows;++i)
	{
		LoopDoCheck(g);
		// 表满了 移除最后一行
		if (pResultRec->GetRows() == pResultRec->GetRowMax())
		{
			pResultRec->RemoveRow(pResultRec->GetRows() - 1);
		}
		int nResult = args.IntVal(nIndex++);
		const wchar_t* enemy_name = args.WideStrVal(nIndex++);
		int challenger_prev_rank = args.IntVal(nIndex++);
		int challenger_new_rank = args.IntVal(nIndex++);
		int64_t nFightTime = args.Int64Val(nIndex++);

		CVarList newdata;
		newdata << nResult << enemy_name << challenger_prev_rank << challenger_new_rank << nFightTime;
		pResultRec->AddRowValue(0, newdata);
	}
	
	return 0;
}

int ArenaModule::OnCreateRobot(IKernel * pKernel, const PERSISTID & player,
                         const PERSISTID & sender, const IVarList &args)
{
    if (!pKernel->Exists(player))
    {
        return 0;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }
    
//     STORE_TYPES_ENUM save_type = (STORE_TYPES_ENUM)args.IntVal(0);
//     
//     if (save_type == STORE_EXIT)
//     {
        int player_level = player_obj->QueryInt("Level");
        const char* rank_list_name = GetRankListName(pKernel, player_level);
        m_pRankListModule->InsertPlayerToRankList(pKernel, player, rank_list_name);
 //   }
   
    return 0;
}

// 玩家等级变化
int ArenaModule::OnPlayerLevel(IKernel* pKernel, const PERSISTID &self,
    const PERSISTID & sender, const IVarList & args)
{   
#if defined(CREATE_ROBOT)

    return 0;

#endif
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    int old_level = args.IntVal(1);;
    int new_level = player_obj->QueryInt("Level");
    const wchar_t *player_name = player_obj->QueryWideStr("Name");
    const char *player_uid = player_obj->QueryString("Uid");
    
    if (new_level < m_ArenaConfig.open_level)
    {
        // 未达到开启等级
        return 0;
    }
    
    const char * old_rank_name = GetRankListName(pKernel, old_level);
    const char * new_rank_name = GetRankListName(pKernel, new_level);
    
    if (strcmp(old_rank_name, new_rank_name) !=0)
    {
        // 新旧榜不同, 玩家进入了新的排行榜,需要清空历史记录
        player_obj->SetInt(TEXT_HIGHEST_ARENA_RANK, 0);
    }
    
    // 检查玩家是否晋升
    
    if (!StringUtil::CharIsNull(old_rank_name)
        && !StringUtil::CharIsNull(new_rank_name))
    {
        // 新榜和旧榜都不为空
        
        CVarList rank_list_row;
        if (!PrepareDataForArenaRankList(pKernel, self, new_rank_name, rank_list_row))
        {
            return 0;
        }
        
        if (strcmp(old_rank_name, new_rank_name) != 0)
        {
            // 新旧榜不同, 因此玩家是晋升
            //PromotePlayer(pKernel, self, old_rank_name, new_rank_name, rank_list_row);
        }
        else
        {
            // 新旧榜相同, 但是玩家有可能不再排行榜中，尝试把玩家存入排行榜
            CVarList the_msg;
            the_msg << ARENA_PUB_SUB_MSG_ADD_TO_RANKLIST;
            the_msg << player_name << player_uid << new_rank_name << rank_list_row;

            SendArenaMsgToPublic(pKernel, the_msg);
        }
    }
    else if (StringUtil::CharIsNull(old_rank_name)
             && !StringUtil::CharIsNull(new_rank_name))
    {
        // 旧榜为空, 新榜不为空, 因此玩家是第一次开启竞技场功能
        
        ResetArenaChance(pKernel, self);
        
        CVarList rank_list_row;
        if (!PrepareDataForArenaRankList(pKernel, self, new_rank_name, rank_list_row))
        {
            return 0;
        }
        
        //PromotePlayer(pKernel, self, "", new_rank_name, rank_list_row);
    }
    
    return 0;
}

// 来自客户端的消息
int ArenaModule::OnCustomMessage(IKernel * pKernel, const PERSISTID & player,
                    const PERSISTID & sender, const IVarList & args)
{
    // 消息格式
    //  
    //      [CLIENT_CUSTOMMSG_ARENA_MSG][sub_msg_id]...
    //

    if (!pKernel->Exists(player))
    {
        return 0;
    }
    
    int role_level = pKernel->QueryInt(player, "Level");
    if (role_level < m_ArenaConfig.open_level)
    {
        // 未达到竞技场要求等级
        return 0;
    }
    
    if (args.GetCount() < 2)
    {
        return 0;
    }
    
    int sub_msg_id = args.IntVal(1);
    
    switch (sub_msg_id)
    {
	case ARENA_C2S_QUERY_SELF_INFO:
        {
            // 查询玩家的竞技场信息
            OnSelfArenaInfo(pKernel, player, sender, args);
        }
        break;
	case ARENA_C2S_GET_CANDIDATE:
        {
            // 获取可挑战的候选人
            OnGetChallengeCandidate(pKernel, player, sender, args);
        }
        break;
	case ARENA_C2S_CHALLENGE_SOMEONE:
        {
            // 挑战某人
            OnChallengeSomeone(pKernel, player, sender, args);
        }
        break;
	case ARENA_C2S_GIVE_UP:
        {
            // 放弃挑战
            OnGiveUpChallenge(pKernel, player, sender, args);
        }
        break;
	case ARENA_C2S_CHALLENGE_CLEAR_CD:
        {
			OnClearChallengeCD(pKernel, player, sender, args);
        }
        break;
	case ARENA_C2S_REQUEST_FIGHT_RESULT:
        {
			OnRequestFightResult(pKernel, player, sender, args);
        }
        break;
    default:
        break;
    }
    
    return 0;
}

// 来自公服的消息
int ArenaModule::OnPublicMessage(IKernel * pKernel, const PERSISTID & self,
                                 const PERSISTID & sender, const IVarList & args)
{
    //
    // 消息格式
    // [msg_id][sub_msg_id][...]
    
    // public服务器会把竞技场的消息交给第一个场景服处理

    int main_msg_id = args.IntVal(0);
    if (main_msg_id != PS_DOMAIN_MSG_ARENA)
    {
        return 0;
    }

    int sub_msg_id = args.IntVal(1);

    switch (sub_msg_id)
    {
    case ARENA_PUB_SUB_MSG_GIVE_RANK_REWARD:
        {
            // 发放排行榜奖励
            RewardByArenaRank(pKernel);
        }
        break;
    default:
        break;
    }

    return 0;
}

// 发给玩家的command
int ArenaModule::OnPlayerCommand(IKernel * pKernel, const PERSISTID & self,
                                 const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //     [msg_id][sub_msg_id]...
    //
    
    int sub_msg_id = args.IntVal(1);
    
    switch(sub_msg_id)
    {
    case ARENA_PUB_SUB_MSG_CHALLENGE_REQUEST:
        {
            // 对挑战请求的回应
            
            OnPubChallengeRequest(pKernel, self, sender, args);
        }
        break;
    case ARENA_PUB_SUB_MSG_PROMOTE:
        {
            // 对晋级的回应

            OnPubPromote(pKernel, self, sender, args);
        }
        break;
    case ARENA_PUB_SUB_MSG_GET_CANDIDATE:
        {
            // 对获取候选人的回应

            OnPubCandidate(pKernel, self, sender, args);
        }
        break;
    case ARENA_PUB_SUB_MSG_CHALLENGE_END:
        {
            // 对挑战结束的回应
            OnPubChallengeEnd(pKernel, self, sender, args);
        }
        break;
	case ARENA_MSG_ADD_FIGHT_RESULT_DATA:
		{
			// 增加一条战报数据
			OnAddFightResultData(pKernel, self, sender, args);
		}
        break;
    default:
        break;
    }
    
    return 0;
}

// 开始战斗前的倒计时
int ArenaModule::HB_BeforeFight( IKernel* pKernel, const PERSISTID& self, int slice)
{
    // 倒计时结束, 可以开打了
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 记录挑战开始时间
    ADD_DATA(pKernel, self, TEXT_ARENA_CHALLENGE_START_TIME, VTYPE_INT64);
    player_obj->SetDataInt64(TEXT_ARENA_CHALLENGE_START_TIME, ::time(NULL));
    
    // 用来记录挑战结束时间
    ADD_DATA(pKernel, self, TEXT_ARENA_CHALLENGE_END_TIME, VTYPE_INT64);

    // 解冻玩家
    UnfreezePlayer(pKernel, self);
    
    // 设置在战斗中
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_IN_BATTLE);
    
    PERSISTID challengee = player_obj->QueryObject(TEXT_ARENA_CHALLENGEE_OBJ);
    if (pKernel->Exists(challengee))
    {
        UnfreezePlayer(pKernel, challengee);

        // 给被挑战者的NPC对象设定对手
        AISystem::Instance()->GetAITemlate(pKernel, challengee).OnProcess(pKernel, AI_RULE_SPRING, challengee, self, CVarList());
    }
    
    // 战斗时长的计数器
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_RoundCheck");
    pKernel->AddCountBeat(self, "ArenaModule::HB_RoundCheck", m_ArenaConfig.round_time * 1000, 1);
    
    // 通知客户端开打
    CVarList msg_for_client;
    msg_for_client << SERVER_CUSTOMMSG_ARENA_MSG;
	msg_for_client << ARENA_S2C_FIGHT;
    msg_for_client << m_ArenaConfig.round_time;
    pKernel->Custom(self, msg_for_client);
    
    return 0;
}

// 回合检查的计时器
int ArenaModule::HB_RoundCheck( IKernel* pKernel, const PERSISTID& self, int slice)
{
    // 挑战时间已经用完, 对手没被击杀, 挑战失败
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }
    
    // 设置结束标志 
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);
    
    int result = ARENA_RET_CHALLENGE_LOSE;
    int reason = ARENA_RET_TIME_OUT;

    PERSISTID challengee_obj = player_obj->QueryObject(TEXT_ARENA_CHALLENGEE_OBJ);
    if (!pKernel->Exists(challengee_obj))
    {
        result = ARENA_RET_CHALLENGE_WIN;
        reason = ARENA_RET_ONE_DIE;
    }
    else
    {
        // 超出挑战时间, 判定挑战者失败
        result = ARENA_RET_CHALLENGE_LOSE;
        reason = ARENA_RET_TIME_OUT;
    }
    
    OnChallengeEnd(pKernel, self, result, reason);

    WaitThenLeaveArena(pKernel, self);
    
    return 0;
}

// 延时退出竞技场
int ArenaModule::HB_CloseWait(IKernel* pKernel, const PERSISTID& self, int slice)
{
    // 等待时间结束, 送玩家离开竞技场
    
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_CloseWait");

    LeaveArenaNow(pKernel, self);
    
    return 0;
}


// 离开竞技场场景
int ArenaModule::HB_LeaveScene(IKernel* pKernel, const PERSISTID& self, int slice)
{
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_LeaveScene");
    
    LeaveArenaNow(pKernel, self);
    
    return 0;
}

// 完成一次击杀
int ArenaModule::OnKillObject(IKernel* pKernel, const PERSISTID& self,
                              const PERSISTID& sender, const IVarList& args)
{
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }
    
    //玩家完成一次击杀, 挑战结束
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }
    
    // 检查玩家的竞技场状态
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    if (arena_flag == ARENA_FLAG_END)
    {
        return 0;
    }
    
    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);
    
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_CloseWait");
    
    OnChallengeEnd(pKernel, self, ARENA_RET_CHALLENGE_WIN, ARENA_RET_ONE_DIE);
    
    WaitThenLeaveArena(pKernel, self);
    
    return 0;
}

// 被击杀
int ArenaModule::OnBeKilled(IKernel* pKernel, const PERSISTID& self,
                            const PERSISTID& sender, const IVarList& args)
{
    // 只关心竞技场场景
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }

    //玩家被击杀, 挑战结束
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 检查玩家的竞技场状态
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    if (arena_flag == ARENA_FLAG_END)
    {
        return 0;
    }

    player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);

    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, self, "ArenaModule::HB_CloseWait");
    
    OnChallengeEnd(pKernel, self, ARENA_RET_CHALLENGE_LOSE, ARENA_RET_ONE_DIE);

    WaitThenLeaveArena(pKernel, self);
    
    return 0;
}

// 晋级玩家
// int ArenaModule::PromotePlayer(IKernel* pKernel, const PERSISTID& self,
//                                   const char *current_ranklist_name, const char *new_ranklist_name)
// {
//     IGameObj *player_obj = pKernel->GetGameObj(self);
//     if (player_obj == NULL)
//     {
//         return 0;
//     }
//     
//     const char *player_uid = pKernel->QueryString(self, "Uid");
//     const wchar_t *player_name = pKernel->QueryWideStr(self, "Name");
// 
//     CVarList the_msg;
//     the_msg << ARENA_PUB_SUB_MSG_PROMOTE;
//     the_msg << player_name << player_uid << current_ranklist_name << new_ranklist_name;
// 
//     SendArenaMsgToPublic(pKernel, the_msg);
//     
//     return 0;
// }
// 
// // 晋级玩家
// int ArenaModule::PromotePlayer(IKernel* pKernel, const PERSISTID& self,
//                                const char *current_ranklist_name, const char *new_ranklist_name,
//                                const IVarList &row_data)
// {
//     // 晋级玩家的时候, 用最新的数据插入排行榜
//     IGameObj *player_obj = pKernel->GetGameObj(self);
//     if (player_obj == NULL)
//     {
//         return 0;
//     }
// 
//     const char *player_uid = pKernel->QueryString(self, "Uid");
//     const wchar_t *player_name = pKernel->QueryWideStr(self, "Name");
// 
//     CVarList the_msg;
//     the_msg << ARENA_PUB_SUB_MSG_PROMOTE
//             << player_name 
//             << player_uid 
//             << current_ranklist_name 
//             << new_ranklist_name
//             << row_data;
// 
//     SendArenaMsgToPublic(pKernel, the_msg);
//     
//     return 0;
// }

// 查询自己的竞技场信息
int ArenaModule::OnSelfArenaInfo(IKernel * pKernel, const PERSISTID & player,
                                 const PERSISTID & sender, const IVarList & args)
{
    // args 的格式：
    // 
    //      [901][1]
    //
    // 回应消息：
    //
    //      [1801][1][竞技场挑战次数][竞技场名次][战斗力][战魂][所在竞技场排行榜]
    //      [新晋升的竞技场名字]
    
    CVarList summary; // [竞技场挑战次数][竞技场名次][战斗力][战魂][所在竞技场排行榜]
    GetSelfArenaInfo(pKernel, player, summary);
    
    CVarList out_msg;
    out_msg << SERVER_CUSTOMMSG_ARENA_MSG;
	out_msg << ARENA_S2C_REV_SELF_INFO;
    out_msg << summary;
    //out_msg << pKernel->QueryString(player, TEXT_ARENA_NEW_RANK_LIST);
    
    // 通知过后就清空晋级标识
   // pKernel->SetString(player, TEXT_ARENA_NEW_RANK_LIST, "");
    
    pKernel->Custom(player, out_msg);
    
    return 0;
}

// 获取候选被挑战者
int ArenaModule::OnGetChallengeCandidate(IKernel * pKernel, const PERSISTID & player,
                                         const PERSISTID & sender, const IVarList & args)
{
    // args 的格式：
    // 
    //      [901][3]
    //
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }

    const wchar_t *challenger_name = player_obj->QueryWideStr("Name");
    const char *challenger_uid = player_obj->QueryString("Uid");
    int challenger_level = player_obj->QueryInt("Level");
    
    const char *rank_list_name = GetRankListName(pKernel, challenger_level);
    if (StringUtil::CharIsNull(rank_list_name))
    {
        return 0;
    }

    CVarList challenge_msg;
    challenge_msg << ARENA_PUB_SUB_MSG_GET_CANDIDATE; // 去取候选人
    challenge_msg << challenger_name 
        << challenger_uid 
        << rank_list_name
        << m_ArenaConfig.chosen_count;

    SendArenaMsgToPublic(pKernel, challenge_msg);
    
    return 0;
}

// 请求挑战某人
int ArenaModule::OnChallengeSomeone(IKernel * pKernel, const PERSISTID & player,
                                    const PERSISTID & sender, const IVarList & args)
{
    // args 的格式：
    // 
    //      [901][4][challengee name]
    //
    // 回应消息：
    //
    //      请求成功不返回消息
    //
    //      请求失败的消息:
    //      [1801][4][error_code]
    //
    
		//竞技场开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_ARENA, player))
	{
		return 0;
	}

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }

    const wchar_t *challengee_name = args.WideStrVal(2);
    const char *challengee_uid = pKernel->SeekRoleUid(challengee_name);

    // 检查要挑战的玩家是合乎要求
    bool valid_challengee = false;
    std::wstring wstr_name_list = player_obj->QueryDataWideStr(TEXT_ARENA_CANDIDATE_LIST);

    CVarList temp_name_list;
    util_split_wstring(temp_name_list, wstr_name_list, L"$");
    int name_count = (int) temp_name_list.GetCount();

    LoopBeginCheck(h)
    for (int i=0; i<name_count; ++i)
    {
        LoopDoCheck(h)
        
        const wchar_t *temp_name = temp_name_list.WideStrVal(i);
        if (wcscmp(temp_name, challengee_name) == 0)
        {
            valid_challengee = true;
            break;
        }
    }
    
    if (!valid_challengee)
    {
        CVarList tip_args;
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_1, tip_args);
        return 0;
    }

	// 挑战是否在CD中
	if (CoolDownModule::m_pInstance->IsCoolDown(pKernel, player, COOLDOWN_ARENA_CHALLENGE))
	{
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_2, CVarList());
		return 0;
	}

	if (ActionMutexModule::IsHaveAction(pKernel, player))
	{
		return 0;
	}

    // 记录下挑战对手
    ADD_DATA(pKernel, player, TEXT_ARENA_CHALLENGEE_NAME, VTYPE_WIDESTR);
    player_obj->SetDataWideStr(TEXT_ARENA_CHALLENGEE_NAME, challengee_name);

    bool success = false;
    
    // 检查是否还有挑战机会
    int arena_chance = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE);
    int arena_chance_used = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE_USED);
    if (arena_chance_used < arena_chance)
    {
        success = ChallengeSomeone(pKernel, player, challengee_name);
        
        if (!success)
        {
            CVarList tip_args;
			::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_1, tip_args);
        }
    }
    else
    {
        // 机会不足, 通知客户端去购买
        
        // 已经购买的机会数
        int bought_chance = arena_chance - m_ArenaConfig.free_chance;
        if (bought_chance < 0)
        {
            
            CVarList tip_args;
			::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_1, tip_args);
            return 0;
        }

        // vip 会获得额外的购买次数
        int buy_limit = m_ArenaConfig.buy_chance_limit;
        //buy_limit += m_pVipModule->GetNumber(pKernel, player, BUY_CHALLENGE_TIMES);

        // 需要的资金
        int fee = 0;
        if (buy_limit > bought_chance)
        {
			if (bought_chance >= 0 && bought_chance < ARENA_CHALLENGE_CHANCE_MAX_SIZE)
			{
				// 没有超过购买上限
				fee = m_ArenaConfig.chance_fee[bought_chance];
			}
			else
			{
				fee = 0x7fffffff;
				::extend_warning(LOG_ERROR, "[ArenaModule::OnChallengeSomeone] config error ");
			}
        }
        
        CVarList buy_chance;
        buy_chance << SERVER_CUSTOMMSG_BUY_INFO;
        //buy_chance << VIP_BUY_CHALLENGE;
        buy_chance << fee;
        buy_chance << bought_chance; // 已购买次数
        buy_chance << buy_limit;     // 购买次数限制
        
        pKernel->Custom(player, buy_chance);        
    }
    
    return 0;
}


// 购买竞技场次数
int ArenaModule::OnBuyArenaChance(IKernel * pKernel, const PERSISTID & player,
                            const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //
    //      [msg][sub_msg]
    //

    int sub_msg_id = args.IntVal(1);
    
//     if (sub_msg_id != VIP_BUY_CHALLENGE)
//     {
//         return 0;
//     }
   
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }
    
    int current_chance = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE);
    
    // 已购买的机会 = 当前拥有的机会 - 免费机会
    int bought_chance = current_chance - m_ArenaConfig.free_chance;
    if (bought_chance < 0)
    {
        return 0;
    }
    
    // vip 会获得额外的购买次数
    int buy_limit = m_ArenaConfig.buy_chance_limit;
    //buy_limit += m_pVipModule->GetNumber(pKernel, player, BUY_CHALLENGE_TIMES);
    
    if (bought_chance >= buy_limit)
    {
        // 不可以超过购买上限
        CVarList tip_args;
        ::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_3, tip_args);
        return 0;
    }
    
    if (bought_chance >= ARENA_CHALLENGE_CHANCE_MAX_SIZE)
    {
        // 配置出错
        return 0;
    }
    
    // 需要的元宝数
    int fee = m_ArenaConfig.chance_fee[bought_chance];
	__int64 current_funds = m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_TSILVER);
    if (fee > current_funds)
    {
        // 资金不足
        CVarList tip_args;
        tip_args << fee;
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_4, tip_args);
            
        return 0;
    }
    
    // 扣除资金
	int ret = m_pCapitalModule->DecCapital(pKernel, player, CAPITAL_TSILVER, fee, FUNCTION_EVENT_ID_BUY_ARENA);
    if (ret != DC_SUCCESS)
    {
        // 扣钱失败
        CVarList tip_args;
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_4, tip_args);
        return 0;
    }
    
    // 增加一次机会
    ++current_chance;
    player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE, current_chance);

	// 通知客户端购买成功
	CVarList out_msg;
	out_msg << SERVER_CUSTOMMSG_ARENA_MSG
		<< ARENA_S2C_BUY_CHANCE_SUCCESS
		<< current_chance;

	pKernel->Custom(player, out_msg);

	//LogModule::m_pLogModule->OnArenaGameCost(pKernel, player, LOG_ARENA_BUY_CHALLENGE_TIMES, fee);
    
    // 当机会购买后，直接去竞技场挑战
    
    // 找到缓存的对手
//     if (player_obj->FindData(TEXT_ARENA_CHALLENGEE_NAME))
//     {
//         const wchar_t *challengee_name = player_obj->QueryDataWideStr(TEXT_ARENA_CHALLENGEE_NAME);
//         
//         bool success = ChallengeSomeone(pKernel, player, challengee_name);
// 
//         if (!success)
//         {  
//             CVarList tip_args;
//             ::CustomSysInfo(pKernel, player, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE,
//                 SYS_INFO_ARENA_ERROR, tip_args);
//         }
//     }
    
    return 0;
}

// 放弃挑战
int ArenaModule::OnGiveUpChallenge(IKernel * pKernel, const PERSISTID & player,
                              const PERSISTID & sender, const IVarList & args)
{
    // 只处理竞技场
    if (!IsArenaScene(pKernel))
    {
        return 0;
    }

    if (!pKernel->Exists(player))
    {
        return 0;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 确保是竞技场状态
    int arena_flag = player_obj->QueryInt(TEXT_ARENA_FLAG);
    if (arena_flag == ARENA_FLAG_NOT_ARENA)
    {
        return 0;
    }

    // 检查是否已经战斗结束
    if (arena_flag != ARENA_FLAG_END)
    {
        // 在战斗结束前逃跑
        player_obj->SetInt(TEXT_ARENA_FLAG, ARENA_FLAG_END);
        OnChallengeEnd(pKernel, player, ARENA_RET_CHALLENGE_LOSE, ARENA_RET_ESCAPE);
    }
    
    // 移除等待心跳
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_BeforeFight");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_CloseWait");
    
    // 把玩家送回之前的地点
    WaitThenLeaveArena(pKernel, player);

    return 0;
}

// 重置挑战CD
int ArenaModule::OnClearChallengeCD(IKernel * pKernel, const PERSISTID & player, const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 已经不在cd中了
	if (!CoolDownModule::m_pInstance->IsCoolDown(pKernel, player, COOLDOWN_ARENA_CHALLENGE))
	{
		return 0; 
	}

	if (!m_pCapitalModule->CanDecCapital(pKernel, player, CAPITAL_TSILVER, m_ArenaConfig.challenge_clear_cd_fee))
	{
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_5, CVarList());
		return 0;
	}

	if (m_pCapitalModule->DecCapital(pKernel, player, CAPITAL_TSILVER, m_ArenaConfig.challenge_clear_cd_fee, FUNCTION_EVENT_ID_CLEAR_ARENA_CD) != DC_SUCCESS)
	{
		return 0;
	}

	CoolDownModule::m_pInstance->EndCoolDown(pKernel, player, COOLDOWN_ARENA_CHALLENGE);

	//LogModule::m_pLogModule->OnArenaGameCost(pKernel, player, LOG_ARENA_CLEAR_CHALLENGE_CD, m_ArenaConfig.challenge_clear_cd_fee);
	return 0;
}

// 响应战报数据请求
int ArenaModule::OnRequestFightResult(IKernel * pKernel, const PERSISTID & player, const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord* pResultRec = pSelfObj->GetRecord(FIELD_RECORD_ARENA_FIGHT_RESULT);
	if (NULL == pResultRec)
	{
		return 0;
	}
	int nRows = pResultRec->GetRows();
	if (nRows == 0) // 没有战报数据
	{
		return 0;
	}
	
	CVarList msg;
	msg << SERVER_CUSTOMMSG_ARENA_MSG << ARENA_S2C_REV_FIGHT_RESULT_DATA << nRows;
	LoopBeginCheck(g);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(g);
		int nResult = pResultRec->QueryInt(i, COLUMN_ARENA_FIGHT_RESULT_RESULT);
		const wchar_t* enemy_name = pResultRec->QueryWideStr(i, COLUMN_ARENA_FIGHT_RESULT_NAME);
		int challenger_prev_rank = pResultRec->QueryInt(i, COLUMN_ARENA_FIGHT_RESULT_OLDRANK);
		int challenger_new_rank = pResultRec->QueryInt(i, COLUMN_ARENA_FIGHT_RESULT_CURRANK);
		int64_t nFightTime = pResultRec->QueryInt64(i, COLUMN_ARENA_FIGHT_RESULT_TIME);

		msg << nResult << enemy_name << challenger_prev_rank << challenger_new_rank << nFightTime;
	}

	pKernel->Custom(player, msg);
	return 0;
}

// 获取玩家的竞技场信息
bool ArenaModule::GetSelfArenaInfo(IKernel * pKernel, const PERSISTID & player, IVarList& summary)
{
    // @summary 的格式
    //    [竞技场今日挑战次数][今日机会数][竞技场的排名(0 表示没有排名)][战斗力][战魂][所在排行榜]
    //
    
    if (!pKernel->Exists(player))
    {
        return false;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }
    
    int arena_chance_used = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE_USED);
    int arena_chance = player_obj->QueryInt(TEXT_ARENA_CHALLENGE_CHANCE);
    int rank_no = 0;
    
    int role_level = player_obj->QueryInt("Level");
    const char* rank_list_name = GetRankListName(pKernel, role_level);
    
    IRecord *rank_rec = GetArenaRankListRecord(pKernel, rank_list_name);
    if (rank_rec != NULL)
    {
        const char *role_uid = player_obj->QueryString("Uid");
        int row = rank_rec->FindString(ARENA_RANK_COL_UID, role_uid);
        
        if (row != -1)
        {
            rank_no = row + 1; // @row 从0开始，@rank_no 从1开始 
        }
    }
    
    int battle_ability = player_obj->QueryInt("BattleAbility");
	__int64 battle_soul = 1;// m_pCapitalModule->GetCapital(pKernel, player, CAPITAL_BATTLE_SOUL);
    
    summary << arena_chance_used << arena_chance << rank_no 
            << battle_ability << battle_soul << rank_list_name;
    
    return true;
}

// 挑战结束
int ArenaModule::OnChallengeEnd(IKernel * pKernel, const PERSISTID &challenger, int result, int reason)
{
    // 竞技场挑战结束
    IGameObj *challenger_obj = pKernel->GetGameObj(challenger);
    if (challenger_obj == NULL)
    {
        return 0;
    }

	//ActionMutexModule::CleanAction(pKernel, challenger);
    
    int challenger_level = challenger_obj->QueryInt("Level");
    const char *rank_list_name = GetRankListName(pKernel, challenger_level);
	const char *arena_sn = challenger_obj->QueryString(TEXT_ARENA_SN);
    
    // 记录下结束时间
    challenger_obj->SetDataInt64(TEXT_ARENA_CHALLENGE_END_TIME, ::time(NULL));
    
    // 不管结果如何，都要更新挑战者的排行榜数据
    CVarList challenger_data;
    PrepareDataForArenaRankList(pKernel, challenger, rank_list_name, challenger_data);
    
    // 同时也要更新挑战者的SNS数据
    SnsDataModule::SaveSnsData(pKernel, challenger);
    
    const wchar_t *challenger_name = challenger_obj->QueryWideStr("Name");
    const char *challenger_uid = challenger_obj->QueryString("Uid");
    
    PERSISTID challengee = challenger_obj->QueryObject(TEXT_ARENA_CHALLENGEE_OBJ);
    const wchar_t *challengee_name = challenger_obj->QueryDataWideStr(TEXT_ARENA_CHALLENGEE_NAME);
    const char *challengee_uid = pKernel->SeekRoleUid(challengee_name);
    
    if (pKernel->Exists(challengee))
    {
        const wchar_t *npc_name = pKernel->QueryWideStr(challengee, "Name");
        if (wcscmp(npc_name, challengee_name) != 0)
        {
            challengee_uid = "";
        }
    }
    else
    {
        challengee_uid = "";
    }
    
    CVarList pub_msg;
    pub_msg << ARENA_PUB_SUB_MSG_CHALLENGE_END
			<< arena_sn
            << challenger_name
            << challenger_uid
            << pKernel->SeekRoleUid(challengee_name)
            << rank_list_name
            << result
            << challenger_data;
    
    SendArenaMsgToPublic(pKernel, pub_msg);

	m_pArenaModule->FreezeAllObject(pKernel, challenger_obj);
    return 0;
}

// 根据竞技结果，发送奖励, @reward_info 存放了奖励结果
bool ArenaModule::RewardByChallenge(IKernel * pKernel, const PERSISTID &challenger, int challenger_prev_rank,
                                    int challenger_new_rank, const wchar_t *challengee_name, int result)
{
	//竞技场开关
	if (!SwitchManagerModule::CheckFunctionEnable(pKernel, SWITCH_FUNCTION_ARENA, challenger))
	{
		return false;
	}

    int challenger_level = pKernel->QueryInt(challenger, "Level");
    
    const char *ranklist_name = GetRankListName(pKernel, challenger_level);
    
    // 奖励配置
    const ChallengeReward *reward_config = GetChallengeReward(challenger_level);
    
    if (reward_config == NULL)
    {											   

        return false;
    }
    
    RewardModule::AwardEx challenger_award;
	challenger_award.srcFunctionId = FUNCTION_EVENT_ID_ARENA;
    
    if (result == ARENA_RET_CHALLENGE_WIN)
    {
        // 挑战者胜利
        // 给挑战者的奖励
		//RewardModule::RewardPlayerById(pKernel, challenger, reward_config)
// 		challenger_award.name = m_ArenaConfig.challenger_win;		 
// 		RewardModule::ParseAddCapitals(reward_config->winner_award.str_capital.c_str(), challenger_award.capitals);
// 		RewardModule::ParseAddItems(reward_config->winner_award.str_item.c_str(), challenger_award.items);
// 
// 		ExpTree exp;
// 		challenger_award.exp = (int64_t)exp.CalculateEvent(pKernel, challenger, PERSISTID(), PERSISTID(), PERSISTID(), reward_config->winner_award.str_exp.c_str());
    }
    else
    {
        // 挑战者失败
        // 给挑战者的奖励
//         challenger_award.name = m_ArenaConfig.challenger_failed;
// 		RewardModule::ParseAddCapitals(reward_config->loser_award.str_capital.c_str(), challenger_award.capitals);
// 		RewardModule::ParseAddItems(reward_config->loser_award.str_item.c_str(), challenger_award.items);
// 
// 		ExpTree exp;
// 		challenger_award.exp = (int64_t)exp.CalculateEvent(pKernel, challenger, PERSISTID(), PERSISTID(), PERSISTID(), reward_config->loser_award.str_exp.c_str());
    }
    
    const wchar_t *challenger_name = pKernel->QueryWideStr(challenger, "Name");

    // 奖励给玩家
    m_pRewardModule->RewardPlayer(pKernel, challenger, &challenger_award, CVarList());
    return true;
}

// 记录双方的战报数据
void ArenaModule::RecordFightResultData(IKernel * pKernel, const PERSISTID &challenger, const wchar_t *challengee_name, int challenger_prev_rank, int challenger_new_rank, int challengee_rank, int nResult)
{
	IGameObj* pChallengerObj = pKernel->GetGameObj(challenger);
	if (NULL == pChallengerObj)
	{
		return;
	}

	// 先记录挑战者的数据
	SaveOneFightResultData(pKernel, challenger, challengee_name, challenger_prev_rank, challenger_new_rank, nResult);

	// 在记录被挑战者的数据
	int nSceneId = pKernel->GetPlayerScene(challengee_name);

	int nFightResult = nResult == ARENA_RET_CHALLENGE_WIN ? ARENA_RET_CHALLENGE_LOSE : ARENA_RET_CHALLENGE_WIN;
	const wchar_t* wsChallengerName = pChallengerObj->QueryWideStr(FIELD_PROP_NAME);
	// 机器人不用记录
	if (IsRobot(pKernel, ARENA_RANK_LIST, challengee_name))
	{
		return;
	}
	
	// 被挑战者的挑战前和挑战后排名
	int nPreRank = 0;
	int nNewRank = 0;
	if (nFightResult == ARENA_RET_CHALLENGE_WIN)
	{
		nPreRank = challengee_rank;
		nNewRank = challengee_rank;
	}
	else
	{
		nPreRank = challenger_new_rank;
		nNewRank = challenger_prev_rank;
	}
	// 不在线记录在sns
	if (nSceneId <= 0)
	{
		const char* targetUid = pKernel->SeekRoleUid(challengee_name);
		pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
			pKernel->GetSceneId(), targetUid,
			CVarList() << SPACE_SNS
			<< SNS_ARENA_SPACE
			<< SNS_MSG_ARENA_SAVE_FIGHT_RESULT
			<< nFightResult
			<< wsChallengerName
			<< nPreRank
			<< nNewRank);
	}
	else
	{
		CVarList msg;
		msg << COMMAND_ARENA_PLAYER << ARENA_MSG_ADD_FIGHT_RESULT_DATA << nFightResult << wsChallengerName << nPreRank << nNewRank;
		pKernel->CommandByName(challengee_name, msg);
	}
}

// 保存一条战报数据
void ArenaModule::SaveOneFightResultData(IKernel * pKernel, const PERSISTID &player, const wchar_t *enemy_name, int challenger_prev_rank, int challenger_new_rank, int nResult)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (NULL == pSelfObj)
	{
		return;
	}

	IRecord* pResultRec = pSelfObj->GetRecord(FIELD_RECORD_ARENA_FIGHT_RESULT);
	if (NULL == pResultRec)
	{
		return;
	}
	// 表满了 移除最后一行
	if (pResultRec->GetRows() == pResultRec->GetRowMax())
	{
		pResultRec->RemoveRow(pResultRec->GetRows() - 1);
	}

	CVarList newdata;
	newdata << nResult << enemy_name << challenger_prev_rank << challenger_new_rank << time(NULL);
	pResultRec->AddRowValue(0, newdata);
}

// Public 服务器对挑战请求的回应
int ArenaModule::OnPubChallengeRequest(IKernel * pKernel, const PERSISTID & self,
                                       const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //     [msg_id][sub_msg_id][result][remark][arena_sn][challengee_uid][challengee_nation]
    //
    
    int result = args.IntVal(2);
    int remark = args.IntVal(3);
    
    if (result == ARENA_CODE_FAILED)
    {
        // public服务器判断结果：不可以进行挑战
        
        switch (remark)
        {
        case ARENA_CODE_ONE_IN_CHALLENE:
            {
                // 一方在挑战中
				
				CVarList tip_args;
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_6, tip_args);
            }
            break;
        case ARENA_CODE_ONE_NOT_FOUND:
            {
                // 在玩家所在的排行榜中找不到对应的人
            
                CVarList tip_args;
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7, tip_args);
            }
            break;
        case ARENA_CODE_CHALLENGEE_LOW_RANK:
            {
                // 对手名次低于自己

                CVarList tip_args;
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_8, tip_args);
            }
            break;
        default:
            {
                // 系统错误
                CVarList tip_args;
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_9, tip_args);
            }
            break;
        }

		//竞技场挑战失败，清空活动互斥属性
		ActionMutexModule::CleanAction(pKernel, self);
            
        return 0;
    }

	const char *arena_sn = args.StringVal(4);
    const char *challengee_uid = args.StringVal(5);
    int challengee_nation = args.IntVal(6);
    
    // 可以进行挑战
    
    char sz_challengee_info[512] = {0};
    SPRINTF_S(sz_challengee_info, "%s,%d", challengee_uid, challengee_nation); 
   
    pKernel->SetString(self, TEXT_ARENA_CHALLENGEE_INFO, sz_challengee_info);
    pKernel->SetInt(self, TEXT_ARENA_FLAG, ARENA_FLAG_ENTERING);
	pKernel->SetString(self, TEXT_ARENA_SN, arena_sn);


	// 开始冷却
	CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, COOLDOWN_ARENA_CHALLENGE, m_ArenaConfig.challenge_cd);

    // 记录下玩家此时的位置
    len_t pos_x = pKernel->GetPosiX(self);
    len_t pos_z = pKernel->GetPosiZ(self);
    len_t pos_y = pKernel->GetMapHeight(pos_x, pos_z);
    int sceneId = pKernel->GetSceneId();

    m_pLandPosModule->SetPlayerLandPosi(pKernel, self, sceneId, pos_x, pos_y, pos_z);	

	float fPosY = pKernel->GetMapHeight(m_ArenaConfig.challenger_born.x, m_ArenaConfig.challenger_born.z);

    // 把玩家送去竞技场
    bool switch_success = AsynCtrlModule::m_pAsynCtrlModule->SwitchLocate(pKernel, self,
        m_ArenaConfig.scene_id,
        m_ArenaConfig.challenger_born.x,
		fPosY,
        m_ArenaConfig.challenger_born.z,
        m_ArenaConfig.challenger_born.o);
        
    if (!switch_success)
    {
        //
        // 传送失败
        //

        pKernel->SetInt(self, TEXT_ARENA_FLAG, ARENA_FLAG_NOT_ARENA);
        
        // 恢复双方状态状态
        UnlockPlayer(pKernel, self);
        
        CVarList tip_args;
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_1, tip_args);
    }
    
    return 0;
}

// Public 服务器对玩家晋升的回应
int ArenaModule::OnPubPromote(IKernel * pKernel, const PERSISTID & self,
                        const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //     [msg_id][sub_msg_id][result][rank_list_name][名次]
    //
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    int result = args.IntVal(2);

    if (result == ARENA_CODE_FAILED)
    {
        // 失败
        return 0;
    }
    
    std::string new_rank_list_name = args.StringVal(3);
    int new_rank = args.IntVal(4);

    std::string prev_rank_list_name = player_obj->QueryString(TEXT_ARENA_CUR_RANK_LIST);
    
    bool is_promotion = false;
    
    // 判断是否晋级
    if (prev_rank_list_name != new_rank_list_name)
    {
        is_promotion = true;
    }

    // 记录历史
    player_obj->SetString(TEXT_ARENA_CUR_RANK_LIST, new_rank_list_name.c_str());
    return 0;
}

// Public 服务器对候选人的回应
int ArenaModule::OnPubCandidate(IKernel * pKernel, const PERSISTID & self,
                                const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //    [msg_id][sub_msg_id][candidate_count][candidate_attr_count]
    //    [candidate1_rank][.. candidate1 row ..]... [candidateN_rank][.. candidateN row ..]
    //
    IGameObj *player_obj = pKernel->GetGameObj(self);
    if (player_obj == NULL)
    {
        return 0;
    }

    // 提取出候选人
    CVarList candidate;
    candidate.Append(args, 2, args.GetCount() - 2);
    
    CVarList out_msg;
    out_msg << SERVER_CUSTOMMSG_ARENA_MSG;
	out_msg << ARENA_S2C_REV_CANDIDATE_INFO;
    out_msg << candidate; 
    
    pKernel->Custom(self, out_msg);

	int nSize = out_msg.GetCount();
    //
    // 读取候选人信息，缓存起来
    //

    int player_level = player_obj->QueryInt("Level");
    const char *rank_list_name = GetRankListName(pKernel, player_level);

    int attr_count = candidate.IntVal(1);
    int name_col = RankListModule::m_pRankListModule->GetColumnIndexByName("player_name", rank_list_name);

    int temp_index = 2;
    int candidate_count = candidate.IntVal(0);
    std::wstring candidate_name_list;

    LoopBeginCheck(g)
    for (int i=0; i<candidate_count; ++i)
    {
        LoopDoCheck(g)

        const wchar_t *temp_name = candidate.WideStrVal(temp_index + 1 + name_col);

        candidate_name_list.append(temp_name);
        candidate_name_list.append(L"$");

        temp_index += attr_count;
    }

    // 缓存候选人
    ADD_DATA(pKernel, self, TEXT_ARENA_CANDIDATE_LIST, VTYPE_WIDESTR);
    player_obj->SetDataWideStr(TEXT_ARENA_CANDIDATE_LIST, candidate_name_list.c_str());

    return 0;
}

// Public 服务器对挑战结束的回应
int ArenaModule::OnPubChallengeEnd(IKernel * pKernel, const PERSISTID & self,
                                   const PERSISTID & sender, const IVarList & args)
{
    // args 的格式
    //    [msg_id][sub_msg_id][result_code][op_code][之前的排名][现在的排名]
    //
    
    // 竞技场挑战结束
    IGameObj *challenger_obj = pKernel->GetGameObj(self);
    if (challenger_obj == NULL)
    {
        return 0;
    }

	const wchar_t* challenger_name = challenger_obj->QueryWideStr(FIELD_PROP_NAME);
    int result_code = args.IntVal(2);
    int op_code = args.IntVal(3);
    int challenger_prev_rank = args.IntVal(4);
    int challenger_new_rank = args.IntVal(5);

	// 挑战者失败时,取下被挑战者的名次,用于记录战况
	int challengee_rank = 0;
	if (result_code == ARENA_RET_CHALLENGE_LOSE)
	{
		challengee_rank = args.IntVal(6);
	}
    
    int challenger_level = challenger_obj->QueryInt("Level");
    const wchar_t *challengee_name = challenger_obj->QueryDataWideStr(TEXT_ARENA_CHALLENGEE_NAME);

    // 发出名次变化通知
    if (result_code == ARENA_RET_CHALLENGE_WIN)
    {
		// 是否满足全服通告
		if (challenger_new_rank < m_pArenaModule->m_ArenaConfig.nBroadRank)
		{
			::CustomSysInfoBroadcast(pKernel, SYSTEM_INFO_ID_10, CVarList() << challenger_name << challengee_name << challenger_new_rank);
		}
    }

    // 给予竞技场奖励
	RewardByChallenge(pKernel, self, challenger_prev_rank, challenger_new_rank, challengee_name, result_code);

	// 记录战报数据
	m_pArenaModule->RecordFightResultData(pKernel, self, challengee_name, challenger_prev_rank, challenger_new_rank, challengee_rank, result_code);

    // 挑战日志
//     PVPLog the_log;
//     the_log.beChallenge = challengee_name;
//     the_log.result = result_code;
//     the_log.startTime = challenger_obj->QueryDataInt64(TEXT_ARENA_CHALLENGE_START_TIME);
//     the_log.endTime = challenger_obj->QueryDataInt64(TEXT_ARENA_CHALLENGE_END_TIME);
//     the_log.beforeRank = challenger_prev_rank;
//     the_log.afterRank = challenger_new_rank;
// 
//     m_pLogModule->SavePVPLog(pKernel, self, the_log);

    // 挑战者获得的奖励
    CVarList challenger_reward_info;
    const ChallengeReward *reward_config = GetChallengeReward(challenger_level);
    //int actionResult = LOG_GAME_ACTION_FAIL;
    if (reward_config != NULL)
    {
        if (result_code == ARENA_RET_CHALLENGE_WIN)
        {
           // actionResult = LOG_GAME_ACTION_SUCCESS;
			ExpTree exp;
			int nAddExp = (int)exp.CalculateEvent(pKernel, self, PERSISTID(), PERSISTID(), PERSISTID(), reward_config->winner_award.str_exp.c_str());

			challenger_reward_info << nAddExp
				<< reward_config->winner_award.str_capital;
        }
        else
        {
			ExpTree exp;
			int nAddExp = (int)exp.CalculateEvent(pKernel, self, PERSISTID(), PERSISTID(), PERSISTID(), reward_config->loser_award.str_exp.c_str());

			challenger_reward_info << nAddExp
				<< reward_config->loser_award.str_capital;
        }
    }
    else
    {
        challenger_reward_info << 0 << "" << "";
    }

    // 玩法日志-竞技场
//     GamePlayerActionLog log;
//     log.actionType     =    LOG_GAME_ACTION_ARENA;
//     log.actionState    =    actionResult;	
//     LogModule::m_pLogModule->SaveGameActionLog(pKernel, self, log);  

    // 之前的最佳排名
    int prev_best_rank = challenger_obj->QueryInt(TEXT_HIGHEST_ARENA_RANK);
    
    // 是否突破最好排名
    bool is_new_best = IsBestRank(pKernel, self, challenger_prev_rank, challenger_new_rank);
    
    // 突破最佳排名获得的奖励
    std::string capital_reward;
	std::string item_reward;
    
	bool bSendReward = false;
    if (is_new_best)
    {
        std::string the_rank_list_name = GetRankListName(pKernel, challenger_obj->QueryInt("Level"));
		bSendReward = GetRankUpReward(capital_reward, item_reward, the_rank_list_name, challenger_new_rank, prev_best_rank);

		// 名次突破历史最好排名
		challenger_obj->SetInt(TEXT_HIGHEST_ARENA_RANK, challenger_new_rank);
    }

    // 发给挑战者的消息
    CVarList msg_for_client;
    msg_for_client << SERVER_CUSTOMMSG_ARENA_MSG;
	msg_for_client << ARENA_S2C_CHALLENGE_END;
    msg_for_client << result_code;
    msg_for_client << challenger_reward_info; // 奖励内容
    msg_for_client << challenger_prev_rank; // 原来的排名
    msg_for_client << challenger_new_rank; // 新排名				   
    msg_for_client << prev_best_rank; // 历史最好排名
    
	if (bSendReward)
    {
		msg_for_client << capital_reward << item_reward;	 // 名次提升奖励
		RewardByRankUp(pKernel, self, capital_reward, item_reward, challenger_new_rank);
    }
    
	pKernel->Custom(self, msg_for_client);
	//LogModule::m_pLogModule->OnArenaGameRank(pKernel, self, challenger_prev_rank, challenger_new_rank);
    return 0;
}

// 增加一条战报数据
int ArenaModule::OnAddFightResultData(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	//msg << COMMAND_ARENA_PLAYER << ARENA_MSG_ADD_FIGHT_RESULT_DATA << nFightResult << wsChallengerName << challenger_new_rank << challenger_prev_rank;
	int nResult = args.IntVal(2);
	const wchar_t* wsName = args.WideStrVal(3);
	int nOldRank = args.IntVal(4);
	int nCurRank = args.IntVal(5);

	int online = pSelfObj->QueryInt(FIELD_PROP_ONLINE);
	// 玩家离线,将数据保存sns
	if (online == OFFLINE)
	{
		const char* strUid = pSelfObj->QueryString(FIELD_PROP_UID);
		pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(),
			pKernel->GetSceneId(), strUid,
			CVarList() << SPACE_SNS
			<< SNS_ARENA_SPACE
			<< SNS_MSG_ARENA_SAVE_FIGHT_RESULT
			<< nResult
			<< wsName
			<< nOldRank
			<< nCurRank);
	}
	else
	{
		m_pArenaModule->SaveOneFightResultData(pKernel, self, wsName, nOldRank, nCurRank, nResult);
		// 通知玩家显示红点
		pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_ARENA_MSG << ARENA_S2C_FIGHT_RESULT_CHANGE_TIP);
	}
	return 0;
}

// VIP升级回调
int ArenaModule::C_OnVIPLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old)
{
	return 0;
}

// 挑战某人
bool ArenaModule::ChallengeSomeone(IKernel * pKernel, const PERSISTID & player,
                                   const wchar_t* challengee_name)
{
    if (!pKernel->Exists(player))
    {
        return false;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }
    
    // 找到玩家所在排行榜
    int player_level = player_obj->QueryInt("Level");
    const char *rank_list_name = GetRankListName(pKernel, player_level);
    
    if (StringUtil::CharIsNull(rank_list_name))
    {
        CVarList tip_args;
		::CustomSysInfo(pKernel, player, SYSTEM_INFO_ID_1, tip_args);
            
        return false;
    }

    const wchar_t *challenger_name = player_obj->QueryWideStr("Name");
    const char *challenger_uid = player_obj->QueryString("Uid");
    const char *challengee_uid = pKernel->SeekRoleUid(challengee_name);

    if (strcmp(challenger_uid, challengee_uid) == 0)
    {
        // 不可以挑战自己
        return false;
    }																				  

    CVarList challenge_msg;
    challenge_msg << ARENA_PUB_SUB_MSG_CHALLENGE_REQUEST; // 挑战申请
    challenge_msg << challenger_name << rank_list_name << challenger_uid << challengee_uid;

    SendArenaMsgToPublic(pKernel, challenge_msg);

    return true;
}

// 发放竞技场排名奖励
bool ArenaModule::RewardByArenaRank(IKernel *pKernel)
{
    // 清理玩家排行, 玩家有可能在多个榜中存有数据，只保留最高的榜

    // 遍历排行榜
    std::vector<ArenaRankListConfig>::const_iterator it = m_ArenaRankList.begin();
    std::vector<ArenaRankListConfig>::const_iterator it_end = m_ArenaRankList.end();

    // 遍历排行榜
    LoopBeginCheck(k)
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(k)
        
        const ArenaRankListConfig &the_rank_list_config = *it;

        // 根据排行榜的名字, 找到合适的奖励表
        const std::vector<RankReward> *reward_list =GetRankReward(the_rank_list_config.name);
        if (reward_list == NULL)
        {
            continue;
        }
        
        IRecord *rec = GetArenaRankListRecord(pKernel, the_rank_list_config.name.c_str());
        if (rec == NULL)
        {
            continue;
        }
        
        int rec_row_count = rec->GetRows();
        if (rec_row_count == 0)
        {
            continue;
        }
        
        // 奖励表中的每个项目都是指定了名次范围
        std::vector<RankReward>::const_iterator reward_it = reward_list->begin();
        std::vector<RankReward>::const_iterator reward_it_end = reward_list->end();
        
        LoopBeginCheck(l)
        for (; reward_it != reward_it_end; ++reward_it)
        {
            LoopDoCheck(l)
            
            const RankReward &the_reward = *reward_it;
            
			int start_row = the_reward.upper_rank - 1; // 因为名次从1开始，而表数据从0开始, 故而需要 -1
			int end_row = the_reward.lower_rank - 1;
            
            if (start_row > end_row)
            {
                continue;
            }
            
            // 确保索引不超出范围
            if (start_row >= rec_row_count)
            {
                continue;
            }
            
            if (end_row >= rec_row_count)
            {
                end_row = rec_row_count - 1;
            }

// 			RewardModule::AwardEx the_award;
// 			the_award.name = m_ArenaConfig.rank_reward_name;
// 			RewardModule::ParseAddCapitals(the_reward.award.str_capital.c_str(), the_award.capitals);
// 			RewardModule::ParseAddItems(the_reward.award.str_item.c_str(), the_award.items);
// 			the_award.srcFunctionId = FUNCTION_EVENT_ID_ARENA_REWARD;
// 			//the_award.exp = the_reward.award.exp * nMulriple;
// 			// 当前竞技场奖励翻倍
// 			if (nMulriple > 1)
// 			{
// 				for (RewardModule::CapitalsList::iterator itr = the_award.capitals.begin(); itr != the_award.capitals.end(); ++itr)
// 				{
// 					itr->second *= nMulriple;
// 				}
// 
// 				for (RewardModule::ItemsList::iterator itr = the_award.items.begin(); itr != the_award.items.end(); ++itr)
// 				{
// 					itr->nAmount *= nMulriple;
// 				}
// 			}            
            CVarList mail_param;
            
            int account_col = m_pRankListModule->GetColumnIndexByName("player_account", the_rank_list_config.name.c_str());
            int role_name_col = m_pRankListModule->GetColumnIndexByName("player_name", the_rank_list_config.name.c_str());
            int role_type_col = m_pRankListModule->GetColumnIndexByName("player_role_flag", the_rank_list_config.name.c_str());

            // 根据此项目对应的名次, 找到对应的玩家
            LoopBeginCheck(o)
            for (int row=start_row; row<=end_row; ++row)
            {
                LoopDoCheck(o)
                
                mail_param.Clear();
                
                CVarList row_data;
                rec->QueryRowValue(row, row_data);
                
                 
                std::string player_account = row_data.StringVal(account_col);
                std::wstring player_name = row_data.WideStrVal(role_name_col);
                int role_type = row_data.IntVal(role_type_col);
                
                mail_param << row + 1; // 把行号转为名次
                
                // 排除机器人
                if (role_type == ROLE_FLAG_ROBOT)
                {
                    continue;
                }
                
                // 通过邮件发奖励给玩家
               // m_pRewardModule->RewardViaMail(pKernel, player_name.c_str(), &the_award, mail_param);
            }
        }
    }

    return true;
}

int ArenaModule::ResetArenaChanceTimer(IKernel* pKernel, const PERSISTID& self, int slice)
{
    ResetArenaChance(pKernel, self);
    return 0;
}


// 发放晋级奖励
bool ArenaModule::RewardByPromotion(IKernel* pKernel, const PERSISTID& self, const char *rank_list_name)
{
    const PromotionReward *reward = GetPromotionReward(rank_list_name);
    
    if (reward == NULL)
    {
        return false;
    }
    
    const wchar_t *player_name = pKernel->QueryWideStr(self, "Name");
    
//     RewardModule::AwardEx the_award;
//     the_award.name = reward->reward_name;
// 	the_award.srcFunctionId = FUNCTION_EVENT_ID_ARENA_REWARD;
// 	RewardModule::ParseAddCapitals(reward->capital.c_str(), the_award.capitals);
// 	RewardModule::ParseAddItems(reward->item.c_str(), the_award.items);
//     
//     CVarList mail_param;
//     m_pRewardModule->RewardViaMail(pKernel, player_name, &the_award, mail_param);

    return true;
}

// 解锁挑战选手
bool ArenaModule::UnlockPlayer(IKernel * pKernel, const PERSISTID & player)
{
    if (!pKernel->Exists(player))
    {																												  
        return false;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }

    int level = player_obj->QueryInt("Level");																							    
    const char *rank_list_name = GetRankListName(pKernel, level);

	std::string arena_sn = player_obj->QueryString(TEXT_ARENA_SN);
    
	if (arena_sn.length() == 0)
	{
		return true;
	}
    
    CVarList challenge_msg;
    challenge_msg << ARENA_PUB_SUB_MSG_UNLOCK_PLAYER; // 解锁玩家
	challenge_msg << arena_sn.c_str() << rank_list_name ;

    SendArenaMsgToPublic(pKernel, challenge_msg);

    return true;
}

// 冻结玩家
void ArenaModule::FreezePlayer(IKernel * pKernel, const PERSISTID & player)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (NULL == pPlayerObj)
	{
		return;
	}
	pPlayerObj->SetInt("CantBeAttack", 1);
	pPlayerObj->SetInt("CantAttack", 1);
	pPlayerObj->SetInt("CantMove", 1);
	pPlayerObj->SetInt("CantUseSkill", 1);
}

// 解冻玩家
void ArenaModule::UnfreezePlayer(IKernel * pKernel, const PERSISTID & player)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (NULL == pPlayerObj)
	{
		return;
	}
	pPlayerObj->SetInt("CantBeAttack", 0);
	pPlayerObj->SetInt("CantAttack", 0);
	pPlayerObj->SetInt("CantMove", 0);
	pPlayerObj->SetInt("CantUseSkill", 0);
}

// 等待一段时间然后离开竞技场
void ArenaModule::WaitThenLeaveArena(IKernel * pKernel, const PERSISTID & player)
{
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_BeforeFight");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_CloseWait");
    
    // 延时退出
    pKernel->AddCountBeat(player, "ArenaModule::HB_CloseWait", m_ArenaConfig.wait_before_close * 1000, 1);

}

// 立刻离开竞技场
void ArenaModule::LeaveArenaNow(IKernel * pKernel, const PERSISTID & player)
{
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_BeforeFight");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_RoundCheck");
    DELETE_HEART_BEAT(pKernel, player, "ArenaModule::HB_CloseWait");
    
    // 立刻离开竞技场
    if (!m_pLandPosModule->PlayerReturnLandPosi(pKernel, player))
    {
        // 退出失败, 2秒后重试
        pKernel->AddCountBeat(player, "ArenaModule::HB_LeaveScene", 2 * 1000, 1);
    }
}

// 此玩家是否可以被挑战
bool ArenaModule::IsValidChallengee(const char *challenger_uid,
                              CVarList &challengee_info, const char *rank_list_name)
{
    int uid_col = m_pRankListModule->GetColumnIndexByName(TEXT_ARENA_RANK_COL_UID, rank_list_name);
    int sn_col = m_pRankListModule->GetColumnIndexByName(TEXT_ARENA_RANK_COL_SN, rank_list_name);
    
	if (uid_col == -1 || sn_col == -1)
	{
		return false;
	}

    const char *temp_challengee_uid = challengee_info.StringVal(uid_col);
	std::string challengee_arena_sn = challengee_info.StringVal(sn_col);

    if (strcmp(temp_challengee_uid, challenger_uid) == 0)
    {
        // 不能挑战自己
        return false;
    }

    if (challengee_arena_sn.length() > 0)
    {
        // 此人已经在竞技场中
        return false;
    }
    
    return true;
}

// 重置竞技场次数
bool ArenaModule::ResetArenaChance(IKernel * pKernel, const PERSISTID & player)
{
    // 检查玩家的竞技场机会是否需要更新
    
    if (!pKernel->Exists(player))
    {
        return false;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }
    
    int player_level = player_obj->QueryInt("Level");

    if (player_level < m_ArenaConfig.open_level)
    {
        return 0;    
    }

    char date_of_today[128] = {0};
    util_convert_date_to_string(date_of_today, sizeof(date_of_today));
    
    const char *last_reset_date = player_obj->QueryString(TEXT_ARENA_CHANCE_RESET_DATE);
    
    // 重置日期不是今天, 需要进行重置
    if (strcmp(date_of_today, last_reset_date) != 0)
    {
        player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE, m_ArenaConfig.free_chance);
        player_obj->SetInt(TEXT_ARENA_CHALLENGE_CHANCE_USED, 0);
        player_obj->SetString(TEXT_ARENA_CHANCE_RESET_DATE, date_of_today);
    }
    
    return true;
}

// 是否竞技场场景
bool ArenaModule::IsArenaScene(IKernel * pKernel)
{
    PERSISTID scene = pKernel->GetScene();
    const char *scene_script = pKernel->GetScript(scene);

    if (strcmp(scene_script, "ArenaScene") == 0)
    {
        return true;
    }
    
    return false;
}


// gm命令解锁自己的竞技场状态
bool ArenaModule::ResetArenaStateByGm(IKernel * pKernel, const wchar_t* wsName, int nLockState)
{
	if (StringUtil::CharIsNull(wsName))
	{
		return false;
	}

	PERSISTID player = pKernel->FindPlayer(wsName);
	IGameObj* player_obj = pKernel->GetGameObj(player);
	if (NULL == player_obj)
	{
		return false;
	}

	int level = player_obj->QueryInt("Level");
	const char *rank_list_name = GetRankListName(pKernel, level);
	const char* strUid = pKernel->SeekRoleUid(wsName);

	CVarList challenge_msg;
	challenge_msg << ARENA_PUB_SUB_MSG_GM_RESET_ARENA_STATE; // 解锁玩家
	challenge_msg << rank_list_name;
	challenge_msg << strUid;
	challenge_msg << nLockState;

	SendArenaMsgToPublic(pKernel, challenge_msg);
	return true;
}

// 送玩家去皇城
bool ArenaModule::GoToCity(IKernel * pKernel, const PERSISTID & player)
{
    if (!pKernel->Exists(player))
    {
        return false;
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return false;
    }

	int main_scene_id = 1001;//GetNationMainSceneId(player_nation);	   // 以后走配置

    pKernel->SwitchBorn(player, main_scene_id);
    
    return true;
}

bool ArenaModule::IsRobot(IKernel * pKernel, const char *ranklist_name, const wchar_t *role_name)
{
    IRecord *rec = GetArenaRankListRecord(pKernel, ranklist_name);
    if (rec == NULL)
    {
        return true;
    }
    
    const char *role_uid = pKernel->SeekRoleUid(role_name);
    
    int uid_col = m_pRankListModule->GetColumnIndexByName("player_uid", ranklist_name);
    int role_type_col = m_pRankListModule->GetColumnIndexByName("player_role_flag", ranklist_name);
    
    int row_id = rec->FindString(uid_col, role_uid);
    if (row_id == -1)
    {
        return true;
    }
    
    CVarList row_info;
    rec->QueryRowValue(row_id, row_info);
    
    int role_flag = row_info.IntVal(role_type_col);
    if (role_flag != ROLE_FLAG_ROBOT)
    {
        return false;
    }
    
    return true;
}

// 发送最佳名次突破奖励
int ArenaModule::RewardByRankUp(IKernel *pKernel, const PERSISTID & player, const std::string &capital_reward, const std::string& item_reward, int new_rank)
{ 
    IGameObj *player_obj = pKernel->GetGameObj(player);
    if (player_obj == NULL)
    {
        return 0;
    }
    
    // 组装邮件，发给用户
//     RewardModule::AwardEx award;
// 	award.srcFunctionId = FUNCTION_EVENT_ID_ARENA_REWARD;
//     award.name = TEXT_RANK_UP_REWARD;
// 	RewardModule::ParseAddItems(item_reward.c_str(), award.items);
// 	RewardModule::ParseAddCapitals(capital_reward.c_str(), award.capitals);
//     
//     CVarList mail_param;
//     mail_param << new_rank;
//     
//     const wchar_t *player_name = player_obj->QueryWideStr("Name");
//     
//     m_pRewardModule->RewardViaMail(pKernel, player_name, &award, mail_param);

    return 0;
}

// 取得候选人规则
const ArenaModule::CandidateRule* ArenaModule::GetCandidateRule(int player_rank)
{
    std::vector<CandidateRule>::const_iterator it = m_CandidateRule.begin();
    std::vector<CandidateRule>::const_iterator it_end = m_CandidateRule.end();
    
    LoopBeginCheck(q)
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(q)
        
        const CandidateRule& the_rule = *it;
        
        if (player_rank == ARENA_NO_RANK)
        {
            // 当玩家没有
        }
        else
        {
			if (player_rank >= the_rule.upper_rank
				&& player_rank <= the_rule.lower_rank)
            {
                return &the_rule;
            }
        }
        

    }
    
    return NULL;
}

// 取得挑战奖励设置
const ArenaModule::ChallengeReward* ArenaModule::GetChallengeReward(int player_level)
{
    std::vector<ChallengeReward>::const_iterator it = m_ChanllengeReward.begin();
    std::vector<ChallengeReward>::const_iterator it_end = m_ChanllengeReward.end();

    LoopBeginCheck(r)
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(r)
        
        const ChallengeReward& the_reward = *it;

        if (player_level >= the_reward.lower_level
            && player_level <= the_reward.upper_level)
        {
            return &the_reward;
        }
    }

    return NULL;
}

// 取得排名奖励设置
const std::vector<ArenaModule::RankReward>* ArenaModule::GetRankReward(const std::string &rank_list_name)
{
    RankRewardMap::const_iterator it_find = m_RankReward.find(rank_list_name);
    if (it_find != m_RankReward.end())
    {
        return &(it_find->second);
    }
    
    return NULL;
}

// 根据排行榜的名字获得晋级奖励
const ArenaModule::PromotionReward* ArenaModule::GetPromotionReward(const std::string &rank_list_name)
{
    PromotionRewardMap::const_iterator it_find = m_PromotionReward.find(rank_list_name);
    if (it_find != m_PromotionReward.end())
    {
        return &(it_find->second);
    }

    return NULL;
}

// 取得竞技场排行榜的表
IRecord* ArenaModule::GetArenaRankListRecord(IKernel * pKernel, const char* rank_list_name)
{
    // 获取公共区间的数据
    if (!pKernel->FindPubSpace(PUBSPACE_DOMAIN))
    {
        return NULL;
    }

    IPubSpace * pub_space = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
    if (pub_space == NULL)
    {
        return NULL;
    }

    wchar_t domain_name[256] = {0};
    SWPRINTF_S(domain_name, L"Domain_RankingList_%d", pKernel->GetServerId());

    IPubData * pub_data = pub_space->GetPubData(domain_name);
    if (pub_data == NULL)
    {
        return NULL;
    }

    IRecord * the_rec = pub_data->GetRecord(rank_list_name);
    return the_rec;
}

// 根据等级取得对应的竞技场排行榜
const char* ArenaModule::GetRankListName(IKernel * pKernel, int level)
{
    std::vector<ArenaRankListConfig>::const_iterator it = m_ArenaRankList.begin();											 
    std::vector<ArenaRankListConfig>::const_iterator it_end = m_ArenaRankList.end();
    
    LoopBeginCheck(s)
    for (; it!=it_end; ++it)
    {
        LoopDoCheck(s)
        
        const ArenaRankListConfig &the_list = *it;
        
        if (level >= the_list.lower_level && level <= the_list.upper_level)
        {
#if defined(CREATE_ROBOT)

            if (level < 15)
            {
                return "";
            }

#endif

            return the_list.name.c_str();
        }
    }
    
    return "";
}


// 解析费用
bool ArenaModule::ParseFee(std::string &str_fee, int *fee_array, int fee_array_size)
{
    // @str_fee 的格式: 10,20,....N
    
    // 初始化@fee_array
    
    memset(fee_array, 0, sizeof(int) * fee_array_size);
    
    CVarList split_result;
    util_split_string(split_result, str_fee, ",");
    
    int fee_count = (int) split_result.GetCount();
    if (fee_count > fee_array_size)
    {
        fee_count = fee_array_size;
    }
    
    if (fee_count == 0)
    {
        return true;
    }
    
    LoopBeginCheck(t)
    for (int i=0; i<fee_count; ++i)
    {
        LoopDoCheck(t)
        
        fee_array[i] = atoi(split_result.StringVal(i));
    }
    
    LoopBeginCheck(u)
    for (int i=0; i<(fee_array_size - fee_count); ++i)
    {
        LoopDoCheck(u)
    
        fee_array[fee_count+i] = fee_array[fee_count-1];
    }
    
    return true;
}

// 解析坐标
bool ArenaModule::ParseCoordinate(const std::string &str_coordinate, Coordinate& coordinate)
{
    // @str_coordinate 的格式: x,y,z,o
    
    CVarList split_result;
    util_split_string(split_result, str_coordinate, ",");
    
    int list_size = (int) split_result.GetCount();
    if (list_size != 3)
    {
        return false;
    }

    coordinate.x = (len_t) atof(split_result.StringVal(0));
    coordinate.z = (len_t) atof(split_result.StringVal(1));
    coordinate.o = (len_t) atof(split_result.StringVal(2));
    
    return true;
}

// 发竞技场的消息到public服务器
void ArenaModule::SendArenaMsgToPublic(IKernel * pKernel, const IVarList& msg)
{
    CVarList pub_msg;
    pub_msg << PUBSPACE_DOMAIN;
    pub_msg << GetDomainName(pKernel).c_str();
    pub_msg << SP_DOMAIN_MSG_ARENA;
    pub_msg << msg;

    pKernel->SendPublicMessage(pub_msg);
}

// 排行榜的一行
bool ArenaModule::PrepareDataForArenaRankList(IKernel * pKernel, const PERSISTID & player, 
    const char *rank_list_name, IVarList& row_data)
{
    return m_pRankListModule->PrepareDataForRankList(pKernel, player, rank_list_name, row_data);
}

// 排行榜的通知策略
ArenaModule::NotifyStrategy  ArenaModule::GetRankListNotifyStrategy(std::string &rank_list_name)
{
    std::vector<ArenaRankListConfig>::const_iterator it = m_ArenaRankList.begin();
    std::vector<ArenaRankListConfig>::const_iterator it_end = m_ArenaRankList.end();

    LoopBeginCheck(s)
	for (; it != it_end; ++it)
	{
		LoopDoCheck(s)
		const ArenaRankListConfig &the_list = *it;
		if (the_list.name == rank_list_name)
		{
			return (NotifyStrategy)the_list.need_notify;
		}
	}

	return DO_NOT_NOTIFY;
}

// 发送最佳名次突破奖励, @old_rank = 之前的最佳排名, @new_rank=当前排名
bool ArenaModule::GetRankUpReward(std::string &capital_reward, std::string& item_reward, const std::string &rank_list_name, int new_rank, int old_rank)
{
    RankUpRewardMap::iterator map_it = m_RankUpReward.find(rank_list_name);
    if (map_it == m_RankUpReward.end())
    {
        return false;
    }
    
    std::vector<RankUpReward> &reward_list = map_it->second;
    
    std::vector<RankUpReward>::iterator list_it = reward_list.begin();
    std::vector<RankUpReward>::iterator list_it_end = reward_list.end();
    
	std::vector<RankUpReward>::iterator old_it = list_it_end;
	std::vector<RankUpReward>::iterator new_it = list_it_end;

	// 只有最高排名达到下一个排名段,才会发奖励
	bool bFind = false;
	LoopBeginCheck(a);
    for (; list_it != list_it_end; ++list_it)
    {
		LoopDoCheck(a);
        RankUpReward &the_reward = *list_it;
		if (old_rank >= the_reward.upper_rank && old_rank <= the_reward.lower_rank)
		{
			old_it = list_it;
		}

		if (new_rank >= the_reward.upper_rank && new_rank <= the_reward.lower_rank)
		{
			new_it = list_it;
			capital_reward = the_reward.award.str_capital;
			item_reward = the_reward.award.str_item;
		}	   

		if (old_it != list_it_end && new_it != list_it_end)
		{
			break;
		}
    }

	// 旧排名和新排名都没有奖励
	if (old_it == list_it_end && new_it == list_it_end)
	{
		bFind = false;
	}
	else
	{
		bFind = old_it != new_it;
	}
	return bFind;
}

// 根据名次提升，获取奖励
bool ArenaModule::IsBestRank(IKernel * pKernel, const PERSISTID & player,
                                  int old_rank, int new_rank)
{
	if (new_rank <= 0)
	{
		return false;
	}
    // @old_rank ==0 表示玩家从未上榜
    if (old_rank != 0)
    {
        if (old_rank <= new_rank)
        {
            return false;
        }
    }

    IGameObj *player_obj = pKernel->GetGameObj(player);

    int highest_arena_rank = player_obj->QueryInt(TEXT_HIGHEST_ARENA_RANK);
    if (highest_arena_rank != 0)
    {
        if (new_rank >= highest_arena_rank)
        {
            // 新排名没有达到历史最好排名
            return false;
        }
    }

    return true;
}

// 把 float 转成INT, 向上取整
// int ArenaModule::Float2IntRoundUp(float f)
// {
//     int ret = (int) f;
//     int temp = (int) (f * 10);
//     
//     if (temp % 10 == 0)
//     {
//         return ret;
//     }
//     else
//     {
//         return ret + 1;
//     }
// }

// 清理玩家排行, 玩家有可能在多个榜中存有数据，只保留最高的榜
int ArenaModule::PlayerRanklistClean(IKernel * pKernel)
{

    // 获取所有表的IRecord 指针
    std::vector<RanklistRecInfo> all_rec;

    std::vector<ArenaRankListConfig>::const_iterator it = m_ArenaRankList.begin();
    std::vector<ArenaRankListConfig>::const_iterator it_end = m_ArenaRankList.end();

	LoopBeginCheck(a);
    for (; it!=it_end; ++it)
    {
		LoopDoCheck(a);

        const ArenaRankListConfig &the_rank_list = *it;

        IRecord *rec = GetArenaRankListRecord(pKernel, the_rank_list.name.c_str());
        if (rec == NULL)
        {
            continue;
        }

        int rec_row_count = rec->GetRows();
        if (rec_row_count == 0)
        {
            continue;
        }
        
        RanklistRecInfo rec_info;
        rec_info.name = the_rank_list.name;
        rec_info.level = the_rank_list.level;
        rec_info.rec = rec;
        
        all_rec.push_back(rec_info);
    }
        
    // 对@all_rec进行排序, 按照Level从高到低
    std::sort(all_rec.begin(), all_rec.end(), CompareRanklistLevel);
        
    // 挨个表检查
    std::vector<RanklistRecInfo>::iterator it_all_rec = all_rec.begin();
    std::vector<RanklistRecInfo>::iterator it_all_rec_end = all_rec.end();
    
    LoopBeginCheck(b)
    for (; it_all_rec != it_all_rec_end; ++it_all_rec)
    {
        LoopBeginCheck(b)
        
        const RanklistRecInfo& main_rec = *it_all_rec;
        
        int player_uid_col = m_pRankListModule->GetColumnIndexByName("player_uid", main_rec.name.c_str());

        int rec_row_count = main_rec.rec->GetRows();
        
        CVarList temp_row;
        
        LoopBeginCheck(c)
        for (int i=0; i<rec_row_count; ++i)
        {
            LoopBeginCheck(c)
            
            temp_row.Clear();
            main_rec.rec->QueryRowValue(i, temp_row);
            
            std::string main_uid = temp_row.StringVal(player_uid_col);
            
            // 去低等级的表中清理此UID的数据
            
            std::vector<RanklistRecInfo>::iterator it_check_rec = all_rec.begin();
            std::vector<RanklistRecInfo>::iterator it_check_rec_end = all_rec.end();
            
            LoopBeginCheck(d)
            for (; it_check_rec != it_check_rec_end; ++it_check_rec)
            {
                LoopBeginCheck(d)
                
                const RanklistRecInfo& check_rec = *it_check_rec;
                
                if (check_rec.level >= main_rec.level)
                {
                    continue;
                }
                
                int check_row = check_rec.rec->FindString(player_uid_col, main_uid.c_str());
                if (check_row != -1)
                {
					  RankListModule::DeleteFromRankList(pKernel, main_uid.c_str(), check_rec.name.c_str());
                }
                
            }
            
        }// End of row loop
        
    }//end of all_rec loop
    
    return 0;       
}

// 比较排行榜的等级
bool ArenaModule::CompareRanklistLevel(const RanklistRecInfo &a, const RanklistRecInfo &b)
{
    return a.level > b.level;
}

// 重新加载配置
void ArenaModule::ReloadConfig(IKernel * pKernel)
{
	m_pArenaModule->LoadResource(pKernel);
}

// 冻结分组内所有对象
void ArenaModule::FreezeAllObject(IKernel* pKernel, IGameObj* pSelfObj)
{
	if (NULL == pSelfObj)
	{
		return;
	}

	int nGroupId = pSelfObj->GetGroupId();
	CVarList objectlist;
	pKernel->GroupChildList(nGroupId, 0, objectlist);
	int nCount = (int)objectlist.GetCount();
	LoopBeginCheck(a);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(a);
		PERSISTID object = objectlist.ObjectVal(i);
		FreezePlayer(pKernel, object);
	}
}

// 来自公服的消息
int ArenaModule::OnCreateScene(IKernel * pKernel, const PERSISTID & self,
	const PERSISTID & sender, const IVarList & args)
{
	m_pArenaModule->ExportPubRecData(pKernel, 6000, 7000);

// 	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_30_35", 0, 500, 3035);
// 
// 	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_35_40", 0, 259, 3540);
// 	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_40_45", 0, 100, 4045);
// 	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_45_50", 0, 100, 4550);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_50_55", 0, 99, 5055);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_55_60", 0, 99, 5560);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_60_65", 0, 100, 6065);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_65_70", 0, 100, 6570);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_70_75", 0, 99, 7075);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_75_80", 0, 100, 7580);
//	m_pArenaModule->ExportLevelRankData(pKernel, "player_level_rank_list_80_85", 0, 20, 8085);
	return 0;
}

// 导出公共区表数据
void ArenaModule::ExportPubRecData(IKernel* pKernel, int nStart, int nEnd)
{
	char strFileName[32] = { 0 };
	SPRINTF_S(strFileName, "gm_x_arena_list_%d.lua", nStart);

	std::string path = pKernel->GetResourcePath();
	path += strFileName;

	FILE* fp = Port_FileOpen(path.c_str(), "wb");

	if (NULL == fp)
	{
		return;
	}

//  	char strData[100*1024] = { 0 };
// 	SPRINTF_S(strData, "%s", "local datas_ = {\"pubname\":\"domain\",\"dataname\":\"Domain_RankingList_7440005\",\"recname\":\"abc\",\"data\":{");
	fprintf(fp, "local datas_ = {pubname=\"domain\",dataname=\"Domain_RankingList_100\",recname=\"arena_rank_list\",data={");
	// 公共数据
	IPubData* pPubData = RankListModule::m_pRankListModule->GetPubData(pKernel);
	if (pPubData == NULL)
	{
		return;
	}

	// 商会表
	IRecord* pRecord = pPubData->GetRecord(ARENA_RANK_LIST);
	if (NULL == pRecord)
	{
		return;
	}

	int nRows = pRecord->GetRows();
	if (nStart < 0 || nEnd > nRows)
	{
		return;
	}

	LoopBeginCheck(a);
	for (int i = nStart; i < nEnd; ++i)
	{
		LoopDoCheck(a);
		fprintf(fp, "{");
		//strcat(strData, "{");

		const char* strUid = pRecord->QueryString(i, 0);
		const char* strAccount = pRecord->QueryString(i, 1);
		const wchar_t* wsName = pRecord->QueryWideStr(i, 2);
		std::string name = StringUtil::WideStrAsString(wsName);
		int nJob = pRecord->QueryInt(i, 3);
		int nSex = pRecord->QueryInt(i, 4);
		int nNation = pRecord->QueryInt(i, 5);
		int nPower = pRecord->QueryInt(i, 6);
		int nLevel = pRecord->QueryInt(i, 7);
		int state = pRecord->QueryInt(i, 8);
		const char* cloth = pRecord->QueryString(i, 9);
		const char* weapon = pRecord->QueryString(i, 10);
		const char* wing = pRecord->QueryString(i, 11);
		const char* resource = pRecord->QueryString(i, 12);
		int flag = pRecord->QueryInt(i, 13);
		int sn = pRecord->QueryInt(i, 14);
		int vip = pRecord->QueryInt(i, 15);

		fprintf(fp, "\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",%d,%d,%d", strUid, strAccount, name.c_str(), nJob, 
					nSex, nNation, nPower, nLevel, state, cloth, weapon, wing, resource, flag, sn, vip);

		fprintf(fp, "},\r\n");
// 		char strTmp[128] = { 0 };
// 		SPRINTF_S(strTmp, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%s,%s,%s,%s,%d,%d,%d", strUid, strAccount, name.c_str()
// 			, nJob, nSex, nNation, nPower, nLevel, state, cloth, weapon, wing, resource, flag, sn, vip);
// 
// 		strcat(strData, strTmp);
// 		strcat(strData, "},/r/n");
	}

	

	fprintf(fp, "}\r\n}\r\nreturn datas_");
//	fprintf(fp, "%s", strData);

	fclose(fp);
}

void ArenaModule::ExportLevelRankData(IKernel* pKernel, const char* strRecName, int nStart, int nEnd,int nNo)
{
	char strFileName[32] = { 0 };
	SPRINTF_S(strFileName, "gm_x_level_list_%d.lua", nNo);

	std::string path = pKernel->GetResourcePath();
	path += strFileName;

	FILE* fp = Port_FileOpen(path.c_str(), "wb");

	if (NULL == fp)
	{
		return;
	}

	//  	char strData[100*1024] = { 0 };
	// 	SPRINTF_S(strData, "%s", "local datas_ = {\"pubname\":\"domain\",\"dataname\":\"Domain_RankingList_7440005\",\"recname\":\"abc\",\"data\":{");
	fprintf(fp, "local datas_ = {pubname=\"domain\",dataname=\"Domain_RankingList_100\",recname=\"%s\",data={", strRecName);
	// 公共数据
	IPubData* pPubData = RankListModule::m_pRankListModule->GetPubData(pKernel);
	if (pPubData == NULL)
	{
		return;
	}

	// 商会表
	IRecord* pRecord = pPubData->GetRecord(strRecName);
	if (NULL == pRecord)
	{
		return;
	}

	int nRows = pRecord->GetRows();
// 	if (nStart < 0 || nEnd > nRows)
// 	{
// 		return;
// 	}

	LoopBeginCheck(a);
	for (int i = 0; i < nRows; ++i)
	{
		LoopDoCheck(a);
		fprintf(fp, "{");
		//strcat(strData, "{");
		//player_uid, player_account, player_name, player_career, player_sex, player_nation, player_level, player_online_type, player_online, player_vip_level

		const char* strUid = pRecord->QueryString(i, 0);
		const char* strAccount = pRecord->QueryString(i, 1);
		const wchar_t* wsName = pRecord->QueryWideStr(i, 2);
		std::string name = StringUtil::WideStrAsString(wsName);
		int nJob = pRecord->QueryInt(i, 3);
		int nSex = pRecord->QueryInt(i, 4);
		int nNation = pRecord->QueryInt(i, 5);
		int nLevel = pRecord->QueryInt(i, 6);
		int online_type = pRecord->QueryInt(i, 7);
		int online = pRecord->QueryInt(i, 8);
		int vip = pRecord->QueryInt(i, 9);

		fprintf(fp, "\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,%d", strUid, strAccount, name.c_str(), nJob, nSex, nNation, nLevel, online_type, online, vip);

		fprintf(fp, "},\r\n");
		// 		char strTmp[128] = { 0 };
		// 		SPRINTF_S(strTmp, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%s,%s,%s,%s,%d,%d,%d", strUid, strAccount, name.c_str()
		// 			, nJob, nSex, nNation, nPower, nLevel, state, cloth, weapon, wing, resource, flag, sn, vip);
		// 
		// 		strcat(strData, strTmp);
		// 		strcat(strData, "},/r/n");
	}



	fprintf(fp, "}\r\n}\r\nreturn datas_");
	//	fprintf(fp, "%s", strData);

	fclose(fp);
}