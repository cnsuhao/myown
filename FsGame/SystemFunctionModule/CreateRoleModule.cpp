//--------------------------------------------------------------------
// 文件名:		CreateRoleModule.cpp
//--------------------------------------------------------------------

#include "CreateRoleModule.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "FsGame/Define/GameDefine.h"
//#include "FsGame/Define/NationDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ContainerDefine.h"
#include "FsGame/SkillModule/SkillModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/SystemFunctionModule/ToolBoxModule.h"
#include "FsGame/CommonModule/LevelModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/SystemFunctionModule/MotionModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
//#include "FsGame/ItemModule/EquipmentModule.h"
#include "FsGame/ItemModule/ItemBaseModule.h"
//#include "FsGame/SocialSystemModule/RideModule.h"
//#include "FsGame/Define/RideDefine.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/SystemFunctionModule/PlayerBaseModule.h"
#include "utils/string_util.h"
#include <time.h>
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Define/PlayerBaseDefine.h"
// #include "../SocialSystemModule/OffLineModule.h"
// #include "../Define/OffLineDefine.h"
//#include "FsGame/SceneBaseModule/SecretSceneModule.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/custom_func.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "server/KnlConst.h"
//#include "FsGame/SocialSystemModule/PetConfigManage.h"

#include <sstream>
#include "../CommonModule/EnvirValueModule.h"

//#include "SceneBaseModule/GroupScene/GroupSceneBase.h"
#include "../CommonModule/LuaScriptModule.h"
#include "RewardModule.h"
#include "EquipmentModule/EquipDefine.h"


#define SKILL_CONTAINER_CAPACITY     512//技能容器容量
#define BUFF_CONTAINER_CAPACITY      128//BUFF容器容量
const time_t ESCAPE_GAP_TIME = 600;    //脱离卡死的间隔时间10分钟

#define PLAYER_RESOURCE    "ini/SystemFunction/Createrole/ResourceConfig.xml"
#define PLAYER_CREATE	   "ini/SystemFunction/Createrole/CreateRoleInfo.xml"

#define ARENA_ROBOT_EQUIP_FILE "ini/SystemFunction/Createrole/ArenaRobotEquip.xml"


MotionModule *		CreateRoleModule::m_pMotionModule = NULL;
CreateRoleModule *	CreateRoleModule::m_pCreateRoleModule = NULL;
ContainerModule *	CreateRoleModule::m_pContainerModule = NULL;
LevelModule *		CreateRoleModule::m_pLevelModule = NULL;
//NationModule *		CreateRoleModule::m_pNationModule = NULL;
//TreasureModule*		CreateRoleModule::m_pTreasureModule = NULL;
StaticDataQueryModule *	CreateRoleModule::m_pStaticDataQueryModule = NULL;
EquipmentModule *	CreateRoleModule::m_pEquipmentModule = NULL;
ItemBaseModule *	CreateRoleModule::m_pItemBaseModule = NULL;
//RideModule *		CreateRoleModule::m_pRideModule	= NULL;
CapitalModule *		CreateRoleModule::m_pCapitalModule = NULL;
//SecretSceneModule * CreateRoleModule::m_pSecretSceneModule = NULL;

// 重加载创建角色配置文件
int CreateRoleModule::nx_reload_create_role_config(void* state)
{
	// 或者核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);


	if (!m_pCreateRoleModule->LoadSaveRoleInfo(pKernel))
	{
		return 0;
	}

	return 1;
}

// 初始化
bool CreateRoleModule::Init(IKernel* pKernel)
{
	m_pCreateRoleModule = this;
    m_pMotionModule		= (MotionModule*)pKernel->GetLogicModule("MotionModule");
    //m_pEquipmentModule	= (EquipmentModule *)pKernel->GetLogicModule("EquipmentModule");
    m_pContainerModule	= (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pLevelModule		= (LevelModule*)pKernel->GetLogicModule("LevelModule");
	m_pStaticDataQueryModule	= (StaticDataQueryModule*)pKernel->GetLogicModule("StaticDataQueryModule");
	m_pItemBaseModule   = (ItemBaseModule*)pKernel->GetLogicModule("ItemBaseModule");
	m_pCapitalModule	= (CapitalModule*)pKernel->GetLogicModule("CapitalModule");

	Assert(m_pCreateRoleModule != NULL && 
		m_pMotionModule != NULL && 
		m_pContainerModule != NULL && 
		m_pLevelModule != NULL && 
		m_pStaticDataQueryModule != NULL && 
/*		m_pEquipmentModule != NULL &&*/
		m_pItemBaseModule != NULL &&
		m_pCapitalModule != NULL );

	// 添加回调
	pKernel->AddLogicClass("role", TYPE_PLAYER, "player");

	pKernel->AddEventCallback("role", "OnCreateRole", OnCreateRole);
	// 编辑离线玩家数据回调
	pKernel->AddEventCallback("player", "OnRecover", CreateRoleModule::OnFirstRecover, INT_MIN);
	pKernel->AddEventCallback("player", "OnStore", CreateRoleModule::OnStore);

	pKernel->AddIntCommandHook("player", COMMAND_SAVE_ROLE_INFO, OnSaveRoleInfo);
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_ESCAPE_LOCK, CreateRoleModule::EscapeFromLock);

//	pKernel->AddEventCallback(CLASS_NAME_TEMPORARY_BAG_BOX, "OnCreate", OnTemporayBoxCreate);

	LoadSaveRoleInfo(pKernel);

    #if defined(CREATE_ROBOT)
        LoadArenaRobotInfo(pKernel);
    #endif

	DECL_LUA_EXT(nx_reload_create_role_config);
	return true;
}

// 释放
bool CreateRoleModule::Shut(IKernel* pKernel)
{
	m_vSaveRoleProps.clear();
	return true;
}

// 竞技场的机器人配置
bool CreateRoleModule::LoadArenaRobotInfo(IKernel* pKernel)
{
    // 文件格式：
    //  <ID="", Job="", Level="", Equipment="", StrLevel="", Jewel="", Soul="" />
    //
    std::string file_path = pKernel->GetResourcePath();
    file_path.append(ARENA_ROBOT_EQUIP_FILE);
    
    CXmlFile xml(file_path.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = file_path;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }
    
    CVarList sec_list;
    xml.GetSectionList(sec_list);

    int count = (int) sec_list.GetCount();
	LoopBeginCheck(h)
    for (int i = 0; i < count; i++)
    {
		LoopDoCheck(h)
        const char *section = sec_list.StringVal(i);
        
        ArenaRobotEquipment equip;
        equip.role_job = xml.ReadInteger(section, "Job", 0);
        equip.role_level = xml.ReadInteger(section, "Level", 0);
        equip.equip = xml.ReadString(section, "Equipment", "");
        equip.equip_strlevel = xml.ReadString(section, "StrLevel", "");
        equip.equip_jewel = xml.ReadString(section, "Jewel", "");
        equip.equip_soul = xml.ReadString(section, "Soul", "");
        
        if (equip.role_job ==0 || equip.role_level ==0)
        {
            continue;
        }
        
        std::stringstream ss;
        ss << equip.role_job << "_" << equip.role_level;
        
        ArenaEquipMap::iterator it_find = m_ArenaRobotEquipment.find(ss.str());
        if (it_find != m_ArenaRobotEquipment.end())
        {
            it_find->second.push_back(equip);
        }
        else
        {
            std::vector<ArenaRobotEquipment> temp_vec;
            temp_vec.push_back(equip);
            
            m_ArenaRobotEquipment.insert(std::make_pair(ss.str(), temp_vec));
        }
    }
    
    return true;
}

// 创建角色
int CreateRoleModule::OnCreateRole(IKernel* pKernel, const PERSISTID& self,
								   const PERSISTID& sender, const IVarList& args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	// 性别 1男 2女
	int job = args.IntVal(0);
	int sex = args.IntVal(1);
	int level = 1;
	
#if defined (CREATE_ROBOT)

    level = args.IntVal(2);
	int nNation = args.IntVal(3);

#endif

	// 检测性别是否合法
	if (!IsValidSex(sex))
	{
		// 不合法，赋予男性
		sex = SEX_MAN;
	}

	// 检测职业是否合法
	if (!IsValidJob(job))
	{
		// 不合法，赋予第一个职业
		job = PLAYER_JOB_MIN + 1;
	}

	//职业
	pPlayer->SetInt("Job",job);
	//性别
	pPlayer->SetInt("Sex",sex);

	pPlayer->SetInt("Level", level);
	
#if defined (CREATE_ROBOT)
    pPlayer->SetInt("PrepareRole", ROLE_FLAG_ROBOT);
	pPlayer->SetInt(FIELD_PROP_NATION, nNation);
#else
    pPlayer->SetInt("PrepareRole", ROLE_FLAG_NORMAL_PLAYER);

#endif
    
	//设置抽宝箱标记
//     pPlayer->SetInt("CopperFirst", 1);
//     pPlayer->SetInt("SliverFirst", 1);
	pPlayer->SetInt64(FIELD_PROP_ROLE_CREATE_TIME, ::time(NULL));
	int64_t playerHP = pPlayer->QueryInt64("MaxHP");
	pPlayer->SetInt64("HP", playerHP);

	//格子占用个数，是否穿透 
	pPlayer->SetInt("PathGrid", TYPE_PLAYER_PATH_GRID);

	//出生点
	const char * point = EnvirValueModule::EnvirQueryString(ENV_VALUE_GUIDE_SCENE_BRON_POS);
	pKernel->SetLandPoint(self,point);
	len_t self_x,self_y,self_z,self_orient;
	int sceneID;
	pKernel->GetSwitchLocation(point,sceneID,self_x,self_y,self_z,self_orient);
	pKernel->SetLandPosi(self, sceneID, self_x, self_y, self_z, self_orient);
	
	// 设置当前的最新版本号
	if (PLAYER_VERSION_MAX > 1)
	{
		pKernel->SetInt(self, "PlayerVersion", PLAYER_VERSION_MAX - 1);
	}

	// 返回0表示创建成功
	return 0;
}

// 一定要保证第一次调用
int CreateRoleModule::OnFirstRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	pSelf->SetInt(FIELD_PROP_GROUP_ID, -1);

	// 判断状态
	int nState = pSelf->QueryInt(FIELD_PROP_BORN_STATE);
	if (nState == EM_PLAYER_STATE_BRON)
	{
		LogModule::m_pLogModule->OnCreateRole(pKernel, self);

		// 初始化数据
		//初始化信息 玩家创建时的基本信息全部在初始化中处理
		m_pCreateRoleModule->RoleInit(pKernel, self);
		//保存信息
		m_pCreateRoleModule->OnSaveRoleInfo(pKernel, self, sender, args);

		// 玩家出生时的物品
		if (NULL != RewardModule::m_pRewardInstance)
		{
			int nAwardId = EnvirValueModule::EnvirQueryInt(ENV_VALUE_PLAYER_CREATE_REWARD);
			RewardModule::m_pRewardInstance->RewardPlayerById(pKernel, self, nAwardId);
		}

		// 设置当前的最新版本号
		if (PLAYER_VERSION_MAX > 1)
		{
			pSelf->SetInt("PlayerVersion", PLAYER_VERSION_MAX - 1);
		}

		// 立即保存
		pSelf->SetInt(FIELD_PROP_BORN_STATE, EM_PLAYER_STATE_INIT);

		pKernel->SavePlayerData(self);

		// 刚创建进入游戏
		pKernel->Command(self, self, CVarList() << COMMAND_MSG_FIRST_ENTER);
	}

	// 处理玩家版本号逻辑
	PlayerBaseModule::HandlePlayerVersion(pKernel, self, CVarList());

	// patch
	if (LuaScriptModule::m_pLuaScriptModule)
	{
		CVarList results;

		CVarList luaargs;
		luaargs << self;
		LuaScriptModule::m_pLuaScriptModule->RunLuaScript(pKernel, "patch.lua", "main", luaargs, 1, &results);
		const char* pszResult_ = results.StringVal(0);
		if ( strcmp(pszResult_, "ok") != 0 )
		{
			extend_warning(LOG_ERROR, "[CreateRoleModule::OnFirstRecover] patch data error(role id:%s)", pKernel->QueryString(self, FIELD_PROP_UID));
		}
	}
	return 0;
}

// 角色离线
int CreateRoleModule::OnStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	int reason = args.IntVal(0);
	if (reason == STORE_EXIT)
	{
		//保存信息
		pKernel->Command(self, self, CVarList() << COMMAND_SAVE_ROLE_INFO);
	}

	return 0;
}


//给竞技场的机器人穿上装备
// int CreateRoleModule::SetRobotEquip(IKernel* pKernel,const PERSISTID& self)
// {
// 	// 玩家对象
// 	IGameObj *pPlayer = pKernel->GetGameObj(self);
// 	if (NULL == pPlayer)
// 	{
// 		return 0;
// 	}
// 
//     int role_job = pPlayer->QueryInt("Job");
//     int role_level = pPlayer->QueryInt("Level");
//     
//     // 装备栏
//     PERSISTID equip_box = pKernel->GetChild(self, L"EquipBox");
//     if (!pKernel->Exists(equip_box))
//     {
//         return 0;
//     }
//     
//     // 强化表
//     IRecord * strengthen_record = pPlayer->GetRecord(EQUIP_STRHENTHEN_REC);
//     if (NULL == strengthen_record)
//     {
//         return 0;
//     }
//     
//     // 先找到合适的装备
//     const ArenaRobotEquipment *robot_equip = NULL;
// 	LoopBeginCheck(m)
//     for (int i=role_level; i>0; --i)
//     {
// 		LoopDoCheck(m)
//         std::stringstream ss;
//         ss << role_job << "_" << i;
//         std::string key = ss.str();
//         
//         ArenaEquipMap::iterator it_find = m_ArenaRobotEquipment.find(key);
//         if (it_find == m_ArenaRobotEquipment.end())
//         {
//             continue;
//         }
//         
//         // 找到了配置的装备信息
//         int config_count = (int)it_find->second.size();
//         
//         if (config_count == 0)
//         {
//             // 木有装备配置信息
//             return 0;
//         }
//         
//         if (config_count > 1)
//         {
//             // 取得一个随机数, 用来选择一个配置
//             int idx = util_random_int(config_count);
//             robot_equip = &((it_find->second)[idx]);
//         }
//         else
//         {
//             robot_equip = &(it_find->second.front());
//         }
//         
//         break;
//     }
//     
//     if (robot_equip == NULL)
//     {
//         // 此等级的职业木有装备
//         return 0;
//     }
//     
//     // 解析装备列表, 各个装备直接用分号分隔
//     CVarList equip_list;
//     util_split_string(equip_list, robot_equip->equip, ";");
//     int equip_config_count = (int)equip_list.GetCount();
// 	LoopBeginCheck(n)
//     for (int i=0; i<equip_config_count; ++i)
//     {
// 		LoopDoCheck(n)
//         std::string one_config = equip_list.StringVal(i);
//         
//         // @one_equip 的格式：装备位置,装备ID,装备品质
//     
//         CVarList one_config_detail;
//         util_split_string(one_config_detail, one_config, ",");
//         if (one_config_detail.GetCount() != 3)
//         {
//             continue;
//         }
//         
//         CreateEquip(pKernel, self, equip_box, one_config_detail);
//     }
//     
//     // 强化等级
//     CVarList str_level_list;
//     util_split_string(str_level_list, robot_equip->equip_strlevel, ";");
//     int str_level_config_count = (int)str_level_list.GetCount();
// 	LoopBeginCheck(o)
//     for (int i=0; i<str_level_config_count; ++i)
//     {
// 		LoopDoCheck(o)
//         std::string one_config = str_level_list.StringVal(i);
// 
//         // @one_level 的格式：装备位置,等级
// 
//         CVarList one_config_detail;
//         util_split_string(one_config_detail, one_config, ",");
//         if (one_config_detail.GetCount() != 2)
//         {
//             continue;
//         }
// 
//         int pos = atoi(one_config_detail.StringVal(0));
//         int level = atoi(one_config_detail.StringVal(1));
//         
//         // 找到装备
//         PERSISTID the_equip = pKernel->GetItem(equip_box, pos);
//         if (!pKernel->Exists(the_equip))
//         {
//             continue;
//         }
//         
//         // 找到强化表中的位置
//         int row = strengthen_record->FindInt(EQUIP_STRHENTHEN_INDEX, pos);
//         if (row == -1)
//         {
//             continue;
//         }
//         
//         strengthen_record->SetInt(row, EQUIP_STRHENTHEN_LEVEL, level);
//         
//         // 发出强化通知
//         pKernel->Command(the_equip, self, CVarList() << COMMAND_EQUIP_STRLEVELUP << 0 << level);
//     }    
//     
//     // 宝石
//     CVarList str_jewel_list;
//     util_split_string(str_jewel_list, robot_equip->equip_jewel, ";");
//     int jewel_config_count = (int)str_jewel_list.GetCount();
// 	LoopBeginCheck(p)
//     for (int i=0; i<jewel_config_count; ++i)
//     {
// 		LoopDoCheck(p)
//         std::string one_config = str_jewel_list.StringVal(i);
// 
//         // @one_level 的格式：装备位置,宝石1,宝石2,宝石3,宝石4
// 
//         CVarList one_config_detail;
//         util_split_string(one_config_detail, one_config, ",");
//         if (one_config_detail.GetCount() != 5)
//         {
//             continue;
//         }
// 
//         int pos = atoi(one_config_detail.StringVal(0));
//         const char *jewel1 = one_config_detail.StringVal(1);
//         const char *jewel2 = one_config_detail.StringVal(2);
//         const char *jewel3 = one_config_detail.StringVal(3);
//         const char *jewel4 = one_config_detail.StringVal(4);
// 
//         // 找到强化表中的位置
//         int row = strengthen_record->FindInt(EQUIP_STRHENTHEN_INDEX, pos);
//         if (row == -1)
//         {
//             continue;
//         }
// 
//         strengthen_record->SetString(row, EQUIP_STRHENTHEN_JEWEL1, jewel1);
//         strengthen_record->SetString(row, EQUIP_STRHENTHEN_JEWEL2, jewel2);
//         strengthen_record->SetString(row, EQUIP_STRHENTHEN_JEWEL3, jewel3);
//         strengthen_record->SetString(row, EQUIP_STRHENTHEN_JEWEL4, jewel4);
//         
//         // 发出宝石镶嵌通知
//         pKernel->Command(self, self, CVarList() << COMMAND_EQUIPMENT_JEWEL << pos << EQUIP_STRPROP_OPTION_ADD << jewel1);
//         pKernel->Command(self, self, CVarList() << COMMAND_EQUIPMENT_JEWEL << pos << EQUIP_STRPROP_OPTION_ADD << jewel2);
//         pKernel->Command(self, self, CVarList() << COMMAND_EQUIPMENT_JEWEL << pos << EQUIP_STRPROP_OPTION_ADD << jewel3);
//         pKernel->Command(self, self, CVarList() << COMMAND_EQUIPMENT_JEWEL << pos << EQUIP_STRPROP_OPTION_ADD << jewel4);
//     }
//     
//     // 灵魂
//     CVarList str_soul_list;
//     util_split_string(str_soul_list, robot_equip->equip_soul, ";");
//     int soul_config_count = (int)str_soul_list.GetCount();
// 	LoopBeginCheck(r)
//     for (int i=0; i<soul_config_count; ++i)
//     {
// 		LoopBeginCheck(r)
//         std::string one_config = str_soul_list.StringVal(i);
// 
//         // @one_level 的格式：装备位置,灵魂
// 
//         CVarList one_config_detail;
//         util_split_string(one_config_detail, one_config, ",");
//         if (one_config_detail.GetCount() != 2)
//         {
//             continue;
//         }
// 
//         int pos = atoi(one_config_detail.StringVal(0));
//         const char *soul = one_config_detail.StringVal(1);
// 
//         // 找到强化表中的位置
//         int row = strengthen_record->FindInt(EQUIP_STRHENTHEN_INDEX, pos);
//         if (row == -1)
//         {
//             continue;
//         }
// 
//         strengthen_record->SetString(row, EQUIP_STRHENTHEN_SOUL, soul);
//         
//         // 发出器魂通知
//         CVarList add_soul_args;
//         add_soul_args << COMMAND_EQUIPMENT_SOUL << pos << EQUIP_STRPROP_OPTION_ADD << soul;
//         pKernel->Command(self, self, add_soul_args );
//     }
// 
//     return 0;
// }


int CreateRoleModule::RoleInit(IKernel* pKernel,const PERSISTID& self)
{
	_initRoleInfoContainers(pKernel, self); //初始化容器
	_initRoleStrRecord(pKernel, self);			//初始化表
	return 0;
}

void CreateRoleModule::_initRoleInfoContainers(IKernel* pKernel, const PERSISTID& self)
{
	// buff容器
	_initRoleInfoContainer(pKernel, self, wszBufferContainerName, "BufferContainer", BUFF_CONTAINER_CAPACITY);

	// 技能容器
	_initRoleInfoContainer(pKernel, self, SKILL_CONTAINER_NAME, "SkillContainer", SKILL_CONTAINER_CAPACITY);

	// 装备容器
	_initRoleInfoContainer(pKernel, self, EQUIP_BOX_NAME_WSTR, EQUIP_BOX_NAME, EQUIP_POS_STR_MAX);

    //道具类背包
    int itemCapacity = ToolBoxModule::GetItemBoxCapacity();
    _initRoleInfoContainer(pKernel, self, ITEM_BOX_NAME_WSTR, ITEM_BOX_NAME, itemCapacity);

    //实力类背包
//     int powerCapacity = ToolBoxModule::GetPowerBoxCapacity();
//     _initRoleInfoContainer(pKernel, self, POWER_BOX_NAME_WSTR, POWER_BOX_NAME, powerCapacity);
// 
//     //符文背包
//     int badgeCapacity = ToolBoxModule::GetBadgeBoxCapacity();
//     _initRoleInfoContainer(pKernel, self, BADGE_BOX_NAME_WSTR, BADGE_BOX_NAME, badgeCapacity);
// 
//     //碎片背包
//     int chipCapacity = ToolBoxModule::GetChipBoxCapacity();
//     _initRoleInfoContainer(pKernel, self, CHIP_BOX_NAME_WSTR, CHIP_BOX_NAME, chipCapacity);

	//挂机背包
	//_initRoleInfoContainer(pKernel, self, OFFLINE_BOX, "OffLineBox", 0);

	//坐骑栏
	//_initRoleInfoContainer(pKernel, self, L"RideBox", "RideBox", MAX_RIDE_NUMBER);

	// 宠物栏
	//_initRoleInfoContainer(pKernel, self, L"PetBox", "PetBox", PetConfigManage::GetMaxPetNum());

	//临时背包 
// 	int temporay_cap = ToolBoxModule::GetTemporaryBoxCapacity();
// 	_initRoleInfoContainer(pKernel, self, TEMPORARY_BAG_BOX_NAME_WSTR, CLASS_NAME_TEMPORARY_BAG_BOX, temporay_cap);

	// 印记容器
	//_initRoleInfoContainer(pKernel, self, MARKER_BOX_NAME_WSTR, MARKER_BOX_NAME, MARK_TYPE_END - 1);
}

bool CreateRoleModule::_initRoleInfoContainer(IKernel* pKernel, const PERSISTID& self, const wchar_t* name,
											  const char* script, int capacity, bool needReset)
{
	PERSISTID container = pKernel->GetChild(self, name);
	if (!pKernel->Exists(container))
	{
		container = pKernel->CreateContainer(self, script, capacity);
		if (!pKernel->Exists(container))
		{
			return false;
		}
		pKernel->SetWideStr(container, "Name", name);
	}
	else
	{
		if (needReset)
		{
			unsigned int it;
			PERSISTID element = pKernel->GetFirst(container, it);
			LoopBeginCheck(v)
			while (pKernel->Exists(element))
			{
				LoopDoCheck(v)
				pKernel->Destroy(container, element);
				element = pKernel->GetNext(container, it);
			}
		}
	}
	return true;
}

// 初始化角色宝箱信息
void CreateRoleModule::_initRoleInfoTreasure(IKernel* pKernel, const PERSISTID& self)
{
	if (NULL == pKernel || !pKernel->Exists(self))
	{
		return;
	}
	//m_pTreasureModule->InitPlayerTreasureData(pKernel, self);
}

void CreateRoleModule::_initRoleStrRecord(IKernel* pKernel, const PERSISTID& self)
{
	PERSISTID equbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equbox))
	{
		return;
	}

 	IRecord * pStrengthenRec = pKernel->GetRecord(self, EQUIP_STRHENTHEN_REC);
 	if (NULL == pStrengthenRec)
 	{
 		return;
 	}
 
 	pStrengthenRec->ClearRow();
 
 	LoopBeginCheck(w)
 	for (int i = 0; i < (int)pStrengthenRec->GetRowMax(); ++i)
 	{
 		LoopDoCheck(w)
 		//装备格子索引,强化等级,灵魂,吞噬个数,宝石x4
 		pStrengthenRec->AddRowValue(i, CVarList()<<i+1<<0<<""<<0<<""<<""<<""<<"");
 	}
}

 int CreateRoleModule::OnSaveRoleInfo(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	m_pCreateRoleModule->SaveRoleInfo(pKernel, self);
	return 0;
}

bool CreateRoleModule::SaveRoleInfo(IKernel* pKernel, const PERSISTID& self)
{
	std::string info = "";

	LoopBeginCheck(x)
	for (int i = 0; i < (int)m_vSaveRoleProps.size(); i++)
	{
		LoopDoCheck(x)
		if (!pKernel->Find(self, m_vSaveRoleProps[i].c_str()))
		{
			info += m_vSaveRoleProps[i].c_str();
			info += ",";
			info += "";
		}
		else
		{
			int nType = pKernel->GetType(self, m_vSaveRoleProps[i].c_str());
			if (nType == VTYPE_INT)
			{
				int nValue = pKernel->QueryInt(self, m_vSaveRoleProps[i].c_str());
				info += m_vSaveRoleProps[i].c_str();
				info += ",";
				info += util_int64_as_string(nValue).c_str();
			}
			else if (nType == VTYPE_STRING)
			{
				std::string strValue = pKernel->QueryString(self, m_vSaveRoleProps[i].c_str());
				info += m_vSaveRoleProps[i].c_str();
				info += ",";
				info += strValue;
			}
			else if (nType == VTYPE_INT64)
			{
				__time64_t nValue64 = pKernel->QueryInt64(self, m_vSaveRoleProps[i].c_str());
				info += m_vSaveRoleProps[i].c_str();
				info += ",";
				info += util_int64_as_string(nValue64).c_str();
			}
			else
			{
				info += m_vSaveRoleProps[i].c_str();
				info += ",";
				info += "";
			}
		}
		info += ";";
	}

	std::wstring roleinfo = util_string_as_widestr(info.c_str());

	if (info.empty())
	{
		::extend_warning(LOG_INFO, "CreateRoleModule role_info is null");
		return false;
	}

	pKernel->SetWideStr(self, "RoleInfo", roleinfo.c_str());

	return true;
}

bool CreateRoleModule::LoadSaveRoleInfo(IKernel* pKernel)
{
	std::string path = pKernel->GetResourcePath();
	path += "ini/SystemFunction/createrole/RoleSaveProps.xml";

	CXmlFile xml(path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string msg = std::string("[Error]ini/SystemFunction/createrole/rolesaveprops.xml No found file : ");
		::extend_warning(LOG_INFO, msg.c_str());
		return false;
	}

	CVarList sec_list;
	xml.GetSectionList(sec_list);
	LoopBeginCheck(y)
	for (size_t i = 0; i < sec_list.GetCount(); i++)
	{
		LoopDoCheck(y)
		std::string section = sec_list.StringVal(i);
		std::string prop = xml.ReadString(section.c_str(), "Prop", "");
		if (!prop.empty())
		{
			m_vSaveRoleProps.push_back(prop);
		}
	}

	return true;
}
//脱离卡死
int CreateRoleModule::EscapeFromLock(IKernel* pKernel,const PERSISTID& self,const PERSISTID& sender,const IVarList& args)
{
	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	int sceneID = pKernel->GetSceneId();
// 	GROUP_SCENE_TYPE type = (GROUP_SCENE_TYPE)GroupSceneBase::GetSceneType(pKernel, sceneID);
// 	if (type != GROUP_SCENE_NOT_SECRET)
// 	{
// 		int subType = m_pSecretSceneModule->GetSubType(SecretSceneModule::GetSecretIndex(pPlayer));
// 		if (SCENE_SUB_TYPE_SCREEN == subType)
// 		{
// 			::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, STR_ESCAPE_FROM_LOCK_1, CVarList());
// 
// 			return 1;
// 		}
//	}
    PERSISTID scene = pKernel->GetScene();

    if (!pPlayer->FindData("EscapeTime"))
    {
        ADD_DATA(pKernel, self, "EscapeTime", VTYPE_INT64);
    }

    time_t lastTime = pPlayer->QueryDataInt64("EscapeTime");
    time_t now = ::time(NULL);
    time_t remainTime = now - lastTime;
    //十分钟冷却时间内不可以使用
    if (remainTime < ESCAPE_GAP_TIME)
    {
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19201, CVarList() << (ESCAPE_GAP_TIME - remainTime));

        return 1;
    }

    len_t x ,y, z, orient;
    pKernel->GetSceneBorn(x, y, z, orient);
    pKernel->MoveTo(self, x, y, z, orient);
    //BattleTeamModule::m_pBattleTeamModule->AddLittleStepGridRec(pKernel, self, x, z);
    pPlayer->SetDataInt64("EscapeTime", now);
    pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_ESCAPE_LOCK << 1);

    return 0;
}