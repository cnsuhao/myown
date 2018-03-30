//---------------------------------------------------------
//文件名:       SnsDataModule.cpp
//内  容:       离线的玩家数据
//说  明:       
//          
//创建日期:      2014年12月24日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#include "SnsDataModule.h"

#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "public/VarList.h"

//#include "FsGame/Define/NationOfficialDefine.h"
#include "FsGame/Define/SnsDataDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
//#include "FsGame/Define/EquipDefine.h"
//#include "FsGame/Define/RideDefine.h"
#include "FsGame/Define/Fields.h"

//#include "FsGame/SocialSystemModule/PetModule.h"

#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/ReLoadConfigModule.h"

#define CONFIG_FILE_VAR_DEF        "ini/SystemFunction/SNS/var_def.xml"
#define CONFIG_FILE_ELEMENT_DEF    "ini/SystemFunction/SNS/element_def.xml"

#define SECTION_PLAYER_ATTRIBUTE           "PlayerAttribute"
#define SECTION_PLAYER_EQUIPMENT           "PlayerEquipment"
#define SECTION_PLAYER_SKILL               "PlayerSkill"
#define SECTION_PLAYER_RIDE                "PlayerRide"
#define SECTION_PLAYER_PET                 "PlayerPet"
#define SECTION_PLAYER_PASSIVE_SKILL       "PlayerPassiveSkill"
#define SECTION_PLAYER_EQUIPMENT_BAPTISE   "PlayerEquipmentBaptise"
#define SECTION_WING_EQUIPMENT             "WingEquipment"

SnsDataModule::VarDefMap SnsDataModule::m_VarDef;
SnsDataModule::SectionElementsMap SnsDataModule::m_SectionElements;
SnsDataModule* SnsDataModule::m_pSnsDataModule = NULL;

//测试使用的GM命令
// 
int nx_player_data(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_player_data, 2);

    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_player_data, 1);
    CHECK_ARG_STRING(state, nx_player_data, 2);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
    const char *target_player_name = pKernel->LuaToString(state, 2);
    const wchar_t *wtarget_player_name = pKernel->LuaToWideStr(state, 2);

    std::wstring target_name = util_string_as_widestr(target_player_name);

    CVarList args;
    args << 0 << 0;
    args << target_name.c_str();

    bool ret = SnsDataModule::OnQueryPlayerData(pKernel, player, PERSISTID(), args);
    
    int code = (ret ? 1 : 0);

    return code;
}

int nx_reload_sns_data_config(void* state)
{
	// 获取核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 判断
	bool result = false;
	if (NULL != SnsDataModule::m_pSnsDataModule)
	{
		result = SnsDataModule::m_pSnsDataModule->LoadResource(pKernel);
	}	
	return result == true ? 1 :0;
}

bool SnsDataModule::Init(IKernel *pKernel)
{
    // 事件
	pKernel->AddEventCallback("player", "OnStore", OnPlayerStoreEvent, 10000);

	// 内部消息
	pKernel->AddIntCommandHook("player", COMMAND_SNS_DATA, OnPlayerCommandMsg);

	// 客户端消息
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_SNS_DATA, OnPlayerCustomMsg);
    
    // 来自公共服的消息
    pKernel->AddEventCallback("scene", "OnPublicMessage", OnPublicMessage);

    pKernel->AddEventCallback("player", "OnEntry", OnPlayerEntry);

	m_pSnsDataModule = this;

    LoadResource(pKernel);

    DECL_LUA_EXT(nx_player_data);
	DECL_LUA_EXT(nx_reload_sns_data_config);

	RELOAD_CONFIG_REG("SnsDataConfig", SnsDataModule::ReloadConfig);
	return true;
}

bool SnsDataModule::Shut(IKernel *pKernel)
{
	return true;
}

// 把一个玩家的数据立刻写入SNS
bool SnsDataModule::SaveSnsData(IKernel* pKernel, const PERSISTID &self)
{
    CVarList offlien_data;
    offlien_data << COMMAND_SNS_DATA;
    offlien_data << SNS_DATA_SAVE_PLAYER_DATA;
    offlien_data << pKernel->SeekRoleUid(pKernel->QueryWideStr(self, "Name"));
    offlien_data << 8; // 8段数据: 属性, 装备, 技能, 坐骑, 宠物, 被动技能, 装备洗练值,翅膀属性,

    ReadPlayerAttr(pKernel, self, offlien_data);
//    ReadPlayerEquipment(pKernel, self, offlien_data);
    ReadPlayerSkill(pKernel, self, offlien_data);
//     ReadPlayerRide(pKernel, self, offlien_data);
//     ReadPlayerPet(pKernel, self, offlien_data);
//     ReadPlayerPassiveSkill(pKernel, self, offlien_data);
//	ReadPlayerEquipmentBaptise(pKernel, self, offlien_data);

    SendMsgToSnsServer(pKernel, self, offlien_data);
    
    return true;
}

//玩家数据加载完毕
int SnsDataModule::OnPlayerEntry(IKernel *pKernel, const PERSISTID &player,
                           const PERSISTID &sender, const IVarList &args)
{
    // 此操作是为了在SNS上面创建此玩家
    
    CVarList offlien_data;
    offlien_data << COMMAND_SNS_DATA;
    offlien_data << SNS_DATA_CREATE_PLAYER_ENTRY;
    offlien_data << pKernel->SeekRoleUid(pKernel->QueryWideStr(player, "Name"));
    offlien_data << 0; // 0段数据
    
    SendMsgToSnsServer(pKernel, player, offlien_data);
    
    return 0;
}

// player 的 OnStore 事件
int SnsDataModule::OnPlayerStoreEvent(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args)
{
    // 把离线玩家的数据保存到SNS服务器
    
    int reason = args.IntVal(0);

    // 仅当玩家离开游戏的时候进行存储
    if (reason == STORE_EXIT || reason == STORE_EDIT /*|| STORE_SWITCH == reason*/)
    {
        SaveSnsData(pKernel, self);
    }

    return 0;
}

// Player 的Command消息处理函数
int SnsDataModule::OnPlayerCommandMsg(IKernel* pKernel, const PERSISTID& self, 
                                      const PERSISTID& sender, const IVarList& args)
{
    // 消息格式
    //
    //    [command_id][sub_msg_id][logic_id]....
    //
    // 回应消息 
    //
    //    N/A

    int sub_msg_id = args.IntVal(1);

    switch (sub_msg_id)
    {
    case SNS_DATA_QUERY_PLAYER_DATA:
        {            
            // 来自SNS服务器的查询结果

            // 消息格式: 
            //    [msg_id][sub_msg_id][logic_id][requester_uid][target_uid]
            //    [section_count]
            //    ["PlayerAttribute"][row][col][...]
            //    ["PlayerEquipment"][row][col][...]

            int args_size = (int)args.GetCount();
            int logic_id = args.IntVal(2);
            const char *request_uid = args.StringVal(3);
            
            // 核心消息
            int core_msg_size = args_size - 4; //core_msg 从 target_uid 开始
            int core_msg_index = 4; // core_msg 从 target_uid 开始
            
            const wchar_t *request_name = pKernel->SeekRoleName(request_uid);
            if (request_uid == NULL || request_name == NULL)
            {
                return 0;
            }
            
            #if defined(_DEBUG)
                const char *target_uid = args.StringVal(3);
                const wchar_t *target_name = pKernel->SeekRoleName(target_uid);
            #endif

            CVarList core_msg;
            core_msg << COMMAND_SNS_DATA;
            core_msg << SNS_DATA_HERE_IS_DATA_YOU_WANT;
            core_msg << logic_id;
            core_msg.Append(args, core_msg_index, core_msg_size);

            pKernel->CommandByName(request_name, core_msg);
        }
        break;
    case SNS_DATA_SHOW_ME_YOUR_DATA:
        {
            OnCommandRequestData(pKernel, self, args);
        }
        break;
    case SNS_DATA_HERE_IS_DATA_YOU_WANT:
        {
            // 消息格式: [command_id][sub_msg_id][logic_id][targer_uid]
            //          [section_count][section1_name][row][col]...[sectionX_name][row][col]...

            int logic_id = args.IntVal(2);
            
            // 只处理来自排行榜的请求
            if (logic_id == SNS_DATA_REQUEST_FORM_RANK_LIST_PLAYER ||
				logic_id == SNS_DATA_REQUEST_FORM_RANK_LIST_PET ||
				logic_id == SNS_DATA_REQUEST_FORM_RANK_LIST_RIDE ||
				logic_id == SNS_DATA_REQUEST_FORM_RANK_LIST_WING ||
				logic_id == SNS_DATA_REQUEST_FORM_TEAM_PLAYER_LIST)
            {
                int core_msg_index = 4; 
                int core_msg_size = ((int)args.GetCount()) - 4;

                const char *target_uid = args.StringVal(3);
                const wchar_t *target_name = pKernel->SeekRoleName(target_uid);

                #if defined(_DEBUG)
                    const wchar_t *request_name = pKernel->QueryWideStr(self, "Name");
                #endif    

                CVarList player_data;
				player_data << logic_id;
                player_data << target_name;
                player_data.Append(args, core_msg_index, core_msg_size);

                OnCommandGotData(pKernel, self, player_data);
            }
			// 处理来自聊天的请求
			if (logic_id == SNS_DATA_REQUEST_FORM_CHAT)
			{
				// 处理查询结果
				OnCommandGotChatData(pKernel, self, args);
			}
			
        }
        break;
    default:
        break;
    }

    return 0;
}

// Player 的客户端消息处理函数
int SnsDataModule::OnPlayerCustomMsg(IKernel* pKernel, const PERSISTID& self,
                                     const PERSISTID& sender, const IVarList& args)
{
    // 消息格式
    //
    //     [msg_id][sub_msg_id]...
    //
    // 回应消息 
    // 
    //     通过 SERVER_CUSTOMMSG_SNS_DATA 自定义消息返回


    ClientRequestRankData(pKernel, self, sender, args);
    return 0;
}

// 客户端请求排行榜内玩家的数据
int SnsDataModule::ClientRequestRankData(IKernel* pKernel, const PERSISTID& self,
									     const PERSISTID& sender, const IVarList& args)
{
	int sub_msg = args.IntVal(1);
	CVarList query_args;
	query_args << sub_msg;
	query_args << args.WideStrVal(2);

	switch (sub_msg)
	{
	case SNS_DATA_REQUEST_FORM_RANK_LIST_PLAYER:
		{
			query_args << 3;
			query_args << SECTION_PLAYER_ATTRIBUTE;
			query_args << SECTION_PLAYER_EQUIPMENT;
			query_args << SECTION_PLAYER_EQUIPMENT_BAPTISE;
		}
		break;
	case SNS_DATA_REQUEST_FORM_RANK_LIST_PET:
		{
			query_args << 1;
			query_args << SECTION_PLAYER_PET;
		}
		break;
	case SNS_DATA_REQUEST_FORM_RANK_LIST_RIDE:
		{
			query_args << 1;
			query_args << SECTION_PLAYER_RIDE;
		}
		break;
// 	case SNS_DATA_REQUEST_FORM_RANK_LIST_WING:
// 		{
// 			query_args << 2;
// 			query_args << SECTION_PLAYER_ATTRIBUTE;
// 			query_args << SECTION_WING_EQUIPMENT;
// 		}
// 		break;
	case SNS_DATA_REQUEST_FORM_TEAM_PLAYER_LIST:
	{
			query_args << 2
			<< SECTION_PLAYER_ATTRIBUTE
			<< SECTION_PLAYER_EQUIPMENT;
	}break;
	default:
		{
			return 0;
		}
		break;
	}

	OnQueryPlayerData(pKernel, self, sender, query_args);

	return 0;
}

// 来自public服务器的消息
int SnsDataModule::OnPublicMessage(IKernel * pKernel, const PERSISTID & self,
                                   const PERSISTID & sender, const IVarList & args)
{
    // 只让1号场景服务器处理
    int scene_id = pKernel->GetSceneId();
    if (scene_id != 1)
    {
        return 0;
    }
    
    int msg_id = args.IntVal(0);
    
//     switch (msg_id)
//     {
//     case COMMAND_NATION_OFFICIAL_MODULE:
//         OnNationPostChanged(pKernel, args);
//         break;
//     default:
//         break;
//    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 功能函数
//////////////////////////////////////////////////////////////////////////

// 发消息给SNS服务器
bool SnsDataModule::SendMsgToSnsServer(IKernel* pKernel, const PERSISTID& self,
                         const IVarList &msg)
{
    const char *uid = pKernel->SeekRoleUid(pKernel->QueryWideStr(self, "Name"));
    return SendMsgToSnsServer(pKernel, uid, msg);
}

bool SnsDataModule::SendMsgToSnsServer(IKernel* pKernel, const char* uid,
                               const IVarList &msg)
{
    CVarList wrapper_msg;
    wrapper_msg << "sns" << L"SnsDataSync";
    wrapper_msg << msg;

    bool ret = pKernel->SendSnsMessage(pKernel->GetDistrictId(),
        pKernel->GetServerId(),
        pKernel->GetMemberId(),
        pKernel->GetSceneId(),
        uid,
        wrapper_msg);

    return ret;
}

// 读取玩家的离线数据
bool SnsDataModule::ReadPlayerAttr(IKernel* pKernel, const PERSISTID& self,
                           IVarList &result)
{
    // 玩家属性共两列: 属性名,属性值
    int col_count = 2; // name,value 两列
    std::string section_name = SECTION_PLAYER_ATTRIBUTE;

    CVarList temp_result;

    const std::vector<ElementDef> * attr_list = GetSectionElements(section_name);
    if (attr_list != NULL)
    {
        std::vector<ElementDef>::const_iterator it = attr_list->begin();
        std::vector<ElementDef>::const_iterator it_end = attr_list->end();
        
        LoopBeginCheck(a);
        for (; it!=it_end; ++it)
        {
            LoopDoCheck(a);
            
            const ElementDef &element = *it;

            if (!pKernel->Find(self, element.name.c_str()))
            {
                continue;
            }

            switch (element.type)
            {
            case VTYPE_INT:
                {
                    int attr_val = pKernel->QueryInt(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            case VTYPE_INT64:
                {
                    int64_t attr_val = pKernel->QueryInt64(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            case VTYPE_FLOAT:
                {
                    float attr_val = pKernel->QueryFloat(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            case VTYPE_DOUBLE:
                {
                    double attr_val = pKernel->QueryDouble(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            case VTYPE_STRING:
                {
                    const char *attr_val = pKernel->QueryString(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            case VTYPE_WIDESTR:
                {
                    const wchar_t *attr_val = pKernel->QueryWideStr(self, element.name.c_str());
                    temp_result << element.name.c_str();
                    temp_result << attr_val;
                }
                break;
            default:
                break; 
            }
        }
    }

    result << section_name.c_str();
    result << ((int)temp_result.GetCount())/col_count;
    result << col_count; // 两列: 属性名, 属性值
    result << temp_result;

    return true;
}

// 读取玩家装备信息
// bool SnsDataModule::ReadPlayerEquipment(IKernel* pKernel, const PERSISTID& self,
//                                 IVarList &result)
// {
//     // 装备信息包括7列: 装备ConfigID, 品质, 附魔包, 修正包, 强化等级, 宝石, 灵魂
//     int col_count = 7;
//     std::string section_name = SECTION_PLAYER_EQUIPMENT;
//     CVarList temp_result;
// 
//     // 取得装备栏
//     PERSISTID equipment_box = pKernel->GetChild(self, L"EquipBox");
//     if (!pKernel->Exists(equipment_box))
//     {
//         result << section_name.c_str();
//         result << 0; 
//         result << col_count;
//         result << temp_result;
// 
//         return false;
//     }
// 
//     // 取得强化表
//     IRecord * pStrengthenRec = pKernel->GetRecord(self, EQUIP_STRHENTHEN_REC);
//     if (NULL == pStrengthenRec)
//     {
//         result << section_name.c_str();
//         result << 0; 
//         result << col_count;
//         result << temp_result;
// 
//         return false;
//     }
// 
//     // 遍历装备表, 读取每个栏位的装备, 宝石, 灵魂
//     LoopBeginCheck(a);
//     for (int i = EQUIP_POS_WEAPON; i < EQUIP_MAX_PLACEHOLDER; ++i)
//     {
//         LoopDoCheck(a);
//         
//         PERSISTID equip = pKernel->GetItem(equipment_box, i);
// 
//         // 读取装备信息
//         if (pKernel->Exists(equip))
//         {
// #if defined(_DEBUG)
//             const wchar_t *equipment_name = pKernel->QueryWideStr(equip, "Name");
// #endif
//             temp_result << pKernel->QueryString(equip, "Config");
//             temp_result << pKernel->QueryInt(equip, "ColorLevel");
//             temp_result << pKernel->QueryInt(equip, "AddPackage");
//             temp_result << pKernel->QueryString(equip, "ModifyAddPackage");
//         }
//         else
//         {
//             temp_result << "";
//             temp_result << 0;
//             temp_result << 0;
//             temp_result << "";
//         }
// 
//         // 从强化表中获取对应栏位的装备的强化等级
// 
//         int row_id = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, i);
//         if (row_id == -1)
//         {
//             temp_result << 0; // 强化等级
//             temp_result << ""; // 栏位镶嵌的宝石
//             temp_result << ""; // 栏位镶嵌的灵魂
// 
//             continue;
//         }
// 
//         // 强化等级
//         int iEquipStrLv = pStrengthenRec->QueryInt(row_id, EQUIP_STRHENTHEN_LEVEL);
// 
//         // 从强化表中获取对应栏位的宝石
//         fast_string jewel = "";
//         
//         LoopBeginCheck(b);
//         for (int m = EQUIP_STRHENTHEN_JEWEL1; m <= EQUIP_STRHENTHEN_JEWEL4; ++m)
//         {
//             LoopDoCheck(b);
//             
//             jewel += pStrengthenRec->QueryString(row_id, m);
//             jewel += ",";
//         }
// 
//         // 从强化表中获取对应栏位的灵魂
//         fast_string soul = "";
//         soul += pStrengthenRec->QueryString(row_id, EQUIP_STRHENTHEN_SOUL);
// 
//         temp_result << iEquipStrLv;
//         temp_result << jewel;
//         temp_result << soul;
//     }
// 
//     result << section_name.c_str();
//     result << ((int)temp_result.GetCount())/col_count; 
//     result << col_count;
//     result << temp_result;
// 
//     return true;
// }

// 读取玩家技能信息
bool SnsDataModule::ReadPlayerSkill(IKernel* pKernel, const PERSISTID& self,
    IVarList &result)
{
    // 技能包括2列: 技能ID, 技能等级
    int col_count = 2;
    std::string section_name = SECTION_PLAYER_SKILL;
    CVarList temp_result;

    // 取得技能栏
    PERSISTID skill_box = pKernel->GetChild(self, L"SkillContainer");
    if (!pKernel->Exists(skill_box))
    {
        result << section_name.c_str();
        result << 0; 
        result << col_count;
        result << temp_result;

        return false;
    }
    
    IGameObj *skill_box_obj = pKernel->GetGameObj(skill_box);
    if (skill_box_obj == NULL)
    {
        result << section_name.c_str();
        result << 0; 
        result << col_count;
        result << temp_result;

        return false;
    }
    
    CVarList skill_list;
    
    int box_capacity = skill_box_obj->GetCapacity();
    
    LoopBeginCheck(a);
    for (int i=1; i<=box_capacity; ++i)
    {
        LoopDoCheck(a);
        
        IGameObj *temp_skill = skill_box_obj->GetChildByIndex(i);
        if (temp_skill != NULL)
        {
            skill_list << temp_skill->GetConfig();
            skill_list << temp_skill->QueryInt("Level");
        }
    }
    
    result << section_name.c_str();
    result << (int)skill_list.GetCount()/col_count; 
    result << col_count;
    result << skill_list;

    return true;
}

// 读取玩家坐骑信息
// bool SnsDataModule::ReadPlayerRide(IKernel* pKernel, const PERSISTID& self,
//                                    IVarList &result)
// {
//     // 坐骑包括7列: 
//     // 坐骑id，使用中皮肤id，坐骑阶级、星级
//     int col_count = 4;
//     std::string section_name = SECTION_PLAYER_RIDE;
//     CVarList the_ride_info;
// 
//     // 取得目标容器
//     PERSISTID the_box = pKernel->GetChild(self, L"RideBox");
//     if (!pKernel->Exists(the_box))
//     {
//         result << section_name.c_str();
//         result << 0; 
//         result << col_count;
//         result << the_ride_info;
// 
//         return false;
//     }
// 
//     IGameObj *the_box_obj = pKernel->GetGameObj(the_box);
//     if (the_box_obj == NULL)
//     {
//         result << section_name.c_str();
//         result << 0; 
//         result << col_count;
//         result << the_ride_info;
// 
//         return false;
//     }
// 
//     // 找到已激活坐骑，第一位就是坐骑
// 	IGameObj *pRide = the_box_obj->GetChildByIndex(1);
// 	if (NULL == pRide)
// 	{
// 		result << section_name.c_str();
// 		result << 0;
// 		result << col_count;
// 		result << the_ride_info;
// 		return false;
// 	}
// 
// 	// 取得使用中的皮肤
// 	IRecord *pSkinRec = pRide->GetRecord(FIELD_RECORD_RIDE_SKIN_REC);
// 	if (NULL == pSkinRec)
// 	{
// 		result << section_name.c_str();
// 		result << 0;
// 		result << col_count;
// 		result << the_ride_info;
// 
// 		return false;
// 	}
// 
// 	the_ride_info << pRide->GetConfig();
// 
// 	// 使用中的皮肤
// 	const int inuse_row = pSkinRec->FindInt(COLUMN_RIDE_SKIN_REC_STATE, SKIN_USE_STATE_INUSE);
// 	if (inuse_row < 0)
// 	{
// 		the_ride_info << pRide->QueryString(FIELD_PROP_DEFAULT_SKIN);
// 	}
// 	else
// 	{
// 		the_ride_info << pSkinRec->QueryString(inuse_row, COLUMN_RIDE_SKIN_REC_ID);
// 	}
// 
// 	// 坐骑
// 	the_ride_info << pRide->QueryInt(FIELD_PROP_RIDE_STEP);
// 	the_ride_info << pRide->QueryInt(FIELD_PROP_RIDE_STAR);
// 
//     result << section_name.c_str();
//     result << (int)the_ride_info.GetCount()/col_count; 
//     result << col_count;
//     result << the_ride_info;
//     
//     return true;
// }
// 
// // 读取玩家宠物信息
// bool SnsDataModule::ReadPlayerPet(IKernel* pKernel, const PERSISTID& self,
//                           IVarList &result)
// {
// 
//     // 技能包括6列: 
//     // 宠物ID, 等级, 星级, 阶级, 出站状态, 技能
// 
//     int col_count = 8;
//     std::string section_name = SECTION_PLAYER_PET;
//     CVarList the_pet_info;
// 
//     // 获取所有出战的NPC
//     CVarList pet_list;
//     PetModule::GetPetAll(pKernel, self, pet_list);
//     
//     int pet_count = (int) pet_list.GetCount();
// 
//     LoopBeginCheck(a);
//     for (int i=0; i<pet_count; ++i)
//     {
//         LoopDoCheck(a);
//         
//         PERSISTID temp_pet = pet_list.ObjectVal(i);
//         
//         if (!pKernel->Exists(temp_pet))
//         {
//             continue;
//         }
//         
//         IGameObj *temp_pet_obj = pKernel->GetGameObj(temp_pet);
//         if (temp_pet_obj == NULL)
//         {
//             continue;
//         }
//         
//         // 基本属性
//         the_pet_info << temp_pet_obj->GetConfig();
//         the_pet_info << temp_pet_obj->QueryInt("Level");
//         the_pet_info << temp_pet_obj->QueryInt("StarLevel");
//         the_pet_info << temp_pet_obj->QueryInt("StepLevel");
//         the_pet_info << temp_pet_obj->QueryInt("Work");
// 		the_pet_info << temp_pet_obj->QueryInt(FIELD_PROP_PET_SCORE);
//         
//         // 技能列表
//         // 技能的格式为： config_id:level,config_id:level,...config_id:level
//         std::string pet_skill_list;
//         
//         IGameObj *pet_skill_box_obj = temp_pet_obj->GetChild(L"SkillContainer");
//         if (pet_skill_box_obj != NULL)
//         { 
//             int the_box_capacity = pet_skill_box_obj->GetCapacity();
// 
//             LoopBeginCheck(b);
//             for (int i=1; i<=the_box_capacity; ++i)
//             {
//                 LoopDoCheck(b);
//                 
//                 IGameObj *temp_skill_obj = pet_skill_box_obj->GetChildByIndex(i); //pKernel->GetItem(container, i);
//                 if (temp_skill_obj == NULL)
//                 {
//                     continue;
//                 }
// 
//                 pet_skill_list.append(temp_skill_obj->GetConfig());
//                 pet_skill_list.append(":");
//                 pet_skill_list.append(util_int_as_string(temp_skill_obj->QueryInt("Level")).c_str());
//                 pet_skill_list.append(",");
//             }
//         }
// 
//         the_pet_info << pet_skill_list.c_str(); 
// 
// 		// 天赋列表
// 		std::string talent_str = "";
// 		IRecord *pRec = temp_pet_obj->GetRecord(FIELD_RECORD_PET_TALENT_REC);
// 		if (NULL != pRec)
// 		{
// 			LoopBeginCheck(b);
// 			for (int row = 0; row < pRec->GetRows(); ++row)
// 			{
// 				LoopDoCheck(b);
// 				const char *talent_id = pRec->QueryString(row, COLUMN_PET_TALENT_REC_TALENT_ID);
// 				if (StringUtil::CharIsNull(talent_id))
// 				{
// 					continue;
// 				}
// 
// 				if (!talent_str.empty())
// 				{
// 					talent_str.append(",");
// 
// 				}
// 
// 				talent_str.append(talent_id);
// 			}
// 		}
// 		the_pet_info << talent_str.c_str();
//         
//     }// End of 遍历宠物容器
// 
//     result << section_name.c_str();
//     result << (int)the_pet_info.GetCount()/col_count; 
//     result << col_count;
//     result << the_pet_info;
// 
//     return true;
// }
// 
// // 读取玩家被动技能信息
// bool SnsDataModule::ReadPlayerPassiveSkill(IKernel* pKernel, const PERSISTID& self,
//                                            IVarList &result)
// {
//     std::string section_name = SECTION_PLAYER_PASSIVE_SKILL;
// 
//     // 取得技能栏
//     IRecord *skill_rec = pKernel->GetRecord(self, "passive_skill_rec");
// 	if (skill_rec == NULL)
// 	{
// 		return false;
// 	}
// 	// 被动技能包括3列: 技能ID, 等级, 来源
// 	int col_count = skill_rec->GetCols();
// 	if (col_count == 0)
// 	{
// 		return false;
// 	}
//  
//     CVarList skill_list;
//     CVarList temp_row;
// 
//     int skill_count = skill_rec->GetRows();
//     
//     LoopBeginCheck(a);
//     for (int i=0; i<skill_count; ++i)
//     {
//         LoopDoCheck(a);
//         
//         temp_row.Clear();
//         
//         skill_rec->QueryRowValue(i, temp_row);
//         
//         skill_list << temp_row;
//     }
// 
//     result << section_name.c_str();
//     result << (int)skill_list.GetCount()/col_count; 
//     result << col_count;
//     result << skill_list;
// 
//     return true;
// }

// 读取玩家洗练属性
// bool SnsDataModule::ReadPlayerEquipmentBaptise(IKernel* pKernel, const PERSISTID& self,
// 											   IVarList &result)
// {
// 	// 装备洗练信息包括9列: 
// 	// 装备栏位索引, 属性1洗炼值, 属性2洗炼值, 属性3洗炼值, 属性4洗炼值, 属性1是否锁定, 属性2是否锁定, 属性3是否锁定, 属性4是否锁定
// 	int col_count = 9;
// 	std::string section_name = SECTION_PLAYER_EQUIPMENT_BAPTISE;
// 	CVarList temp_result;
// 
// 	// 取得装备栏
// 	PERSISTID equipment_box = pKernel->GetChild(self, L"EquipBox");
// 	if (!pKernel->Exists(equipment_box))
// 	{
// 		result << section_name.c_str();
// 		result << 0; 
// 		result << col_count;
// 		result << temp_result;
// 
// 		return false;
// 	}
// 
// 	// 取得洗练表
// 	IRecord * pBaptiseRec = pKernel->GetRecord(self, EQUIP_BAPTISE_REC);
// 	if (NULL == pBaptiseRec)
// 	{
// 		result << section_name.c_str();
// 		result << 0; 
// 		result << col_count;
// 		result << temp_result;
// 
// 		return false;
// 	}
// 
// 	// 遍历装备表, 从洗练表内取出对应的数据
// 	LoopBeginCheck(a);
// 	for (int i = EQUIP_POS_WEAPON; i < EQUIP_MAX_PLACEHOLDER; ++i)
// 	{
// 		LoopDoCheck(a);
// 
// 		PERSISTID equip = pKernel->GetItem(equipment_box, i);
// 
// 		// 读取装备信息
// 		if (pKernel->Exists(equip))
// 		{
// #if defined(_DEBUG)
// 			const wchar_t *equipment_name = pKernel->QueryWideStr(equip, "Name");
// #endif
// 			// 从洗练表内取出对应的洗练数据
// 
// 			int row_id = pBaptiseRec->FindInt(0, i);
// 			if (row_id == -1)
// 			{
// 				temp_result << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
// 				continue;
// 			}
// 
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_INDEX);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_ONE);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_TWO);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_THREE);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_FOUR);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_ONE_LOCK_STATE);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_PROP_TWO_LOCK_STATE);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_RPOP_THREE_LOCK_STATE);
// 			temp_result << pBaptiseRec->QueryInt(row_id, EQUIP_BAPTISE_RPOP_FOUR_LOCK_STATE);
// 		}
// 		else
// 		{
// 			temp_result << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
// 		}
// 	}
// 
// 	result << section_name.c_str();
// 	result << ((int)temp_result.GetCount())/col_count; 
// 	result << col_count;
// 	result << temp_result;
// 
// 	return true;
// }

// 玩家查询离线数据
bool SnsDataModule::OnQueryPlayerData(IKernel* pKernel, const PERSISTID& self, 
    const PERSISTID& sender, const IVarList& args)
{
    // 消息格式
    //
    //      [logic_id][target_user_name]
    //      [section_count][section1_name][section2_name]...
    //
    // 回应消息 
    //
    //    通过 COMMAND_SNS_DATA 通知查询结果
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if( pSelfObj == NULL )
	{
		return false;
	}

    int logic_id = args.IntVal(0);
    const wchar_t *target_player_name = args.WideStrVal(1);
    int section_count = args.IntVal(2);
    int section_index = 3;
    
    if (section_count <= 0)
    {
        return false;
    }
    
    const char *targer_uid = pKernel->SeekRoleUid(target_player_name);
       
    if (target_player_name == NULL || targer_uid == NULL)
    {
        return false;
    }

    if (strlen(targer_uid) == 0)
    {
        return false;
    }

    // 请求者
    const char *requester_uid = pKernel->SeekRoleUid(pSelfObj->QueryWideStr("Name"));

    if(pKernel->GetPlayerScene(target_player_name) > 0)
    {
        
        // 目标玩家在线
        CVarList request_msg;
        request_msg << COMMAND_SNS_DATA;
        request_msg << SNS_DATA_SHOW_ME_YOUR_DATA;
        request_msg << logic_id;
        request_msg << requester_uid;
        request_msg << section_count;
        
        LoopBeginCheck(a);
        for (int i=0; i<section_count; ++i)
        {
            LoopDoCheck(a);
            
            request_msg << args.StringVal(section_index + i);
        }

        pKernel->CommandByName(target_player_name, request_msg);

        return true;
    }
    else
    {

        // 目标玩家不在线

        CVarList query_msg;
        query_msg << COMMAND_SNS_DATA;
        query_msg << SNS_DATA_QUERY_PLAYER_DATA;
        query_msg << logic_id;
        query_msg << requester_uid;
        query_msg << targer_uid;
        
        query_msg << section_count;

        LoopBeginCheck(b);
        for (int i=0; i<section_count; ++i)
        {
            LoopDoCheck(b);
            
            query_msg << args.StringVal(section_index + i);
        }
        
        bool sns_success = SendMsgToSnsServer(pKernel, self, query_msg);
        
        return sns_success;
    }
}


// 把玩家属性存储到SNS
// args 的格式: [name][value]...[name][value]
// name 是字符串类型, 可用的name配置在SnsData.xml 的 element_def 页中
bool SnsDataModule::SavePlayerAttr(IKernel* pKernel, const PERSISTID& player, const IVarList& args)
{
    if (!pKernel->Exists(player))
    {
        return false;
    }

    int attr_count = (int)(args.GetCount()/2);
    
    CVarList sns_data;
    sns_data << COMMAND_SNS_DATA;
    sns_data << SNS_DATA_SAVE_PLAYER_DATA;
    sns_data << pKernel->SeekRoleUid(pKernel->QueryWideStr(player, "Name"));
    
    sns_data << 1; // 仅仅只有一段数据：玩家属性
    sns_data << SECTION_PLAYER_ATTRIBUTE; // 段名
    sns_data << attr_count; // 属性个数
    sns_data << 2;  // 每个属性占据两列(name + value)
    sns_data << args;

    SendMsgToSnsServer(pKernel, player, sns_data);
    
    return true;
}

// 查询聊天玩家相关数据
bool SnsDataModule::OnCommandGotChatData(IKernel* pKernel, const PERSISTID& self,const IVarList& args)
{
	// args格式: [command_id][sub_msg_id][logic_id][targer_uid]
	//          [section_count][section1_name][row][col]...[sectionX_name][row][col]...

	if (!pKernel->Exists(self))
	{
		return false;
	}

	const char *target_uid = args.StringVal(3);
	const wchar_t *target_name = pKernel->SeekRoleName(target_uid);

	#if defined(_DEBUG)
	const wchar_t *request_name = pKernel->QueryWideStr(self, "Name");
	#endif    

	int index = 6;
	int row = args.IntVal(index++); 
	int col = args.IntVal(index++);

	CVarList player_data;
	CVarList result_data;
	// 取出玩家属性数据
	player_data.Append(args, index, row*col);
	
	const int propLen = 5;
	//属性名，注意顺序
	std::string strPropName[propLen] = 
	{
		"Level", "Job", "Sex","GuildName"
	};
	// 按顺序取
	
	LoopBeginCheck(a);
	for (unsigned int i = 0; i < strPropName->size(); ++i)
	{
	    LoopDoCheck(a);
	    
	    LoopBeginCheck(b);
		for (unsigned int counter = 0; counter < args.GetCount();++counter)
		{
		    LoopDoCheck(b);
		    
			if (strcmp(strPropName[i].c_str(),args.StringVal(counter)) == 0)
			{
				int data_type = pKernel->GetType(self,args.StringVal(counter));
				switch (data_type)
				{
				case VTYPE_INT:
					{
						result_data << args.IntVal(++counter);
					}
					break;
				case VTYPE_INT64:
					{
						result_data << args.Int64Val(++counter);
					}
					break;
				case VTYPE_FLOAT:
					{
						result_data << args.FloatVal(++counter);
					}
					break;
				case VTYPE_DOUBLE:
					{
						result_data << args.DoubleVal(++counter);
					}
					break;
				case VTYPE_STRING:
					{
						result_data << args.StringVal(++counter);
					}
					break;
				case VTYPE_WIDESTR:
					{
						result_data << args.WideStrVal(++counter);
					}
					break;
				default:
					break; 
				}
			}

		}
	}

	CVarList client_msg;
	client_msg << SERVER_CUSTOMMSG_CHAT_QUERY_RST;
	client_msg << SNS_DATA_REQUEST_FORM_CHAT;
	client_msg << target_uid <<target_name;
	client_msg << result_data;

	pKernel->Custom(self, client_msg);

	return true;

	
}

// 请求数据
bool SnsDataModule::OnCommandRequestData(IKernel* pKernel, const PERSISTID& self, 
                                        const IVarList& args)
{
    // args 格式：
    //      [msg_id][sub_msg_id][logic_id]
    //      [requester_uid][section_count][section1_name]...
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if( pSelfObj == NULL )
	{
		return false;
	}

    int logic_id = args.IntVal(2);
    const char* requester_uid = args.StringVal(3);
    int section_count = args.IntVal(4);
    int section_index = 5;
    
    const wchar_t *requester_name = pKernel->SeekRoleName(requester_uid);
    if (requester_name == NULL)
    {
        return false;
    }

    CVarList player_data;
    player_data << COMMAND_SNS_DATA;
    player_data << SNS_DATA_HERE_IS_DATA_YOU_WANT;
    player_data << logic_id;
    player_data << pKernel->SeekRoleUid(pSelfObj->QueryWideStr("Name"));
    player_data << section_count;
    
    LoopBeginCheck(a);
    for (int i=0; i<section_count; ++i)
    {
        LoopDoCheck(a);
        
        const char* section_name = args.StringVal(section_index +i);
        
        if (strcmp(section_name, SNS_DATA_SECTION_ATTR) == 0)
        {
            ReadPlayerAttr(pKernel, self, player_data);
        }
        else if (strcmp(section_name, SNS_DATA_SECTION_EQUIPMENT) == 0)
        {
//            ReadPlayerEquipment(pKernel, self, player_data);
        }
        else if (strcmp(section_name, SNS_DATA_SECTION_SKILL) == 0)
        {
            ReadPlayerSkill(pKernel, self, player_data);
        }
// 		else if (strcmp(section_name, SNS_DATA_SECTION_PLAYER_PET) == 0)
// 		{
// 			ReadPlayerPet(pKernel, self, player_data);
// 		}
// 		else if (strcmp(section_name, SNS_DATA_SECTION_PLAYER_RIDE) == 0)
// 		{
// 			ReadPlayerRide(pKernel, self, player_data);
// 		}
// 		else if (strcmp(section_name, SNS_DATA_SECTION_EQUIPMENT_BAPTISE) == 0)
// 		{
// 			ReadPlayerEquipmentBaptise(pKernel, self, player_data);
// 		}
        else
        {
            // 没有此段定义，那就标记为０行０列
            player_data << section_name << 0 << 0; 
        }
    }

    pKernel->CommandByName(requester_name, player_data);

    return true;
}

// 得到数据
bool SnsDataModule::OnCommandGotData(IKernel* pKernel, const PERSISTID& self,
                                     const IVarList& args)
{
	// args 格式：
	//      [logic_id]
	//      [target_name]
	//      [section_count]
	//      ["PlayerAttribute"][row][col][...]
	//      ["PlayerEquipment"][row][col][...]
	//      ["PlayerPet"][row][col][...]
	//      ["PlayerRide"][row][col][...]

	int args_count = (int) args.GetCount();
	if (args_count == 0)
	{
		return false;
	}

	int logic_id = args.IntVal(0);

	// 去除第一位标签
	CVarList tempArgs;
	tempArgs.Append(args, 1, args_count - 1);
	int tempArgs_count = (int)tempArgs.GetCount();

	// 当数据量过大时, 把一条消息拆分成多个, 每个消息最多100个字段
	int fragment_size = 100;

	int fragment_count = (tempArgs_count + fragment_size - 1)/fragment_size;
	int temp_count = fragment_size;

	CVarList client_msg;
	LoopBeginCheck(a);
	for (int i=0; i<fragment_count; ++i)
	{
	    LoopDoCheck(a);

		client_msg.Clear();

		client_msg << SERVER_CUSTOMMSG_SNS_DATA;
		client_msg << logic_id;
		client_msg << fragment_count;
		client_msg << i;

		temp_count = fragment_size;
		if ((i*fragment_size + fragment_size) >= tempArgs_count)
		{
			temp_count = fragment_size - ((i*fragment_size + fragment_size) - tempArgs_count);
		}

		client_msg.Append(tempArgs, i*fragment_size, temp_count);

#if defined(_DEBUG)
		int len = (int)client_msg.GetCount();
#endif    
		pKernel->Custom(self, client_msg);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 配置相关的函数
//////////////////////////////////////////////////////////////////////////

// 获取section的子项
const std::vector<SnsDataModule::ElementDef> * SnsDataModule::GetSectionElements(
    const std::string &sec_name)
{
    SectionElementsMap::const_iterator it = m_SectionElements.find(sec_name);

    if (it == m_SectionElements.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

// 获取变量的id, 错误返回-1 
int SnsDataModule::GetVarID(const std::string &var_name)
{
    VarDefMap::const_iterator it = m_VarDef.find(var_name);

    if (it == m_VarDef.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}

// 载入配置文件
bool SnsDataModule::LoadResource(IKernel *pKernel)
{
    if (!LoadVarDef(pKernel))
    {
        return false;
    }

    if (!LoadElementsDef(pKernel))
    {
        return false;
    }

    return true;
}

// 载入配置文件-变量定义
bool SnsDataModule::LoadVarDef(IKernel *pKernel)
{
    //
    //    <Property ID="2" TypeName="int"/>
    //

    std::string res_path = pKernel->GetResourcePath();

    std::string config_file = res_path;
    config_file.append(CONFIG_FILE_VAR_DEF);

    CXmlFile xml(config_file.c_str());
    if (!xml.LoadFromFile())
    {
        ::extend_warning(LOG_ERROR, "%s does not exists.", config_file.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a);
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a);
        
        const char *xml_section = sec_list.StringVal(i);
        std::string type_id = xml_section;
        std::string type_name = xml.ReadString(xml_section, "TypeName", "");

        if (type_id.length() == 0 || type_name.length() == 0)
        {                
            ::extend_warning(LOG_ERROR, "%s: invalid var define.", config_file.c_str());

            return false;
        }

        int int_id = atoi(type_id.c_str());

        m_VarDef.insert(VarDefMap::value_type(type_name, int_id));
    }

    return true;
}

// 载入配置文件-section 子项定义
bool SnsDataModule::LoadElementsDef(IKernel *pKernel)
{
    //
    //    <Property ID="Name" Type="wstr" Section="PlayerAttribute"/>
    //

    std::string res_path = pKernel->GetResourcePath();

    std::string config_file = res_path;
    config_file.append(CONFIG_FILE_ELEMENT_DEF);

    CXmlFile xml(config_file.c_str());
    if (!xml.LoadFromFile())
    {
        ::extend_warning(LOG_ERROR, "%s does not exists.", config_file.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a);
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a);
        
        const char *xml_section = sec_list.StringVal(i);
        std::string elem_name = xml_section;
        std::string elem_type = xml.ReadString(xml_section, "Type", "");
        std::string elem_section = xml.ReadString(xml_section, "Section", "");

        if (elem_type.length() == 0 || elem_section.length() == 0)
        {                
            ::extend_warning(LOG_ERROR, "%s: invalid element define.", config_file.c_str());

            return false;
        }

        int type_id = GetVarID(elem_type);
        if (type_id == -1)
        {
            ::extend_warning(LOG_ERROR, "%s: invalid element define.", config_file.c_str());
            return false;
        }

        ElementDef element;
        element.name = elem_name;
        element.type = type_id;
        element.section = elem_section;

        SectionElementsMap::iterator it = m_SectionElements.find(element.section);
        if (it == m_SectionElements.end())
        {
            std::vector<ElementDef> element_list;
            element_list.push_back(element);

            m_SectionElements.insert(SectionElementsMap::value_type(element.section, element_list));
        }
        else
        {
            std::vector<ElementDef> &element_list = it->second;
            element_list.push_back(element);
        }
    }

    return true;
}

void SnsDataModule::ReloadConfig(IKernel* pKernel)
{
	m_pSnsDataModule->LoadResource(pKernel);
}