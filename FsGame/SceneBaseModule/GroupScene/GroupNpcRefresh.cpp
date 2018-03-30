//--------------------------------------------------------------------
// 文件名:      GroupNpcRefresh.cpp
// 内  容:      分组场景副本怪物刷新规则
// 说  明:
// 创建日期:    2015年8月4日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "FsGame/SceneBaseModule/GroupScene/GroupNpcRefresh.h"
#include "FsGame/Define/GroupSceneDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/SceretSceneDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include <time.h>

#include "FsGame/NpcBaseModule/NpcCreatorModule.h"
#include "FsGame/SceneBaseModule/GroupScene/GroupSceneBase.h"
#include "FsGame/SceneBaseModule/SecretSceneModule.h"
#include "FsGame/SceneBaseModule/TeamSceneModule.h"
#include "FsGame/NpcBaseModule/ai/AIDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/Interface/FightInterface.h"


const char* const GROUP_NPC_REFRESH_CONFIG = "ini/Scene/secretscene/group_npc_refresh.xml";

GroupNpcRefresh*    GroupNpcRefresh::m_pGroupNpcRefresh = NULL;
NpcCreatorModule*   GroupNpcRefresh::m_pNpcCreatorModule = NULL;

#ifndef FSROOMLOGIC_EXPORTS
SecretSceneModule*  GroupNpcRefresh::m_pSecretSceneModule = NULL;
//TeamSceneModule*    GroupNpcRefresh::m_pTeamSceneModule = NULL;
#endif // FSROOMLOGIC_EXPORTS

std::vector<GROUP_SCENE_REFRESH_FUNC> GroupNpcRefresh::v_funcList;

//重新读取资源
int nx_reload_npc_refresh_resource(void *state)
{
    IKernel *pKernel = LuaExtModule::GetKernel(state);

    GroupNpcRefresh::m_pGroupNpcRefresh->LoadResource(pKernel);
    return 1;
}

bool GroupNpcRefresh::Init(IKernel* pKernel)
{
    m_pGroupNpcRefresh = this;
    m_pNpcCreatorModule = (NpcCreatorModule*)pKernel->GetLogicModule("NpcCreatorModule");

	Assert( m_pGroupNpcRefresh != NULL &&
		m_pNpcCreatorModule != NULL);

#ifndef FSROOMLOGIC_EXPORTS
    m_pSecretSceneModule = (SecretSceneModule*)pKernel->GetLogicModule("SecretSceneModule");
    m_pTeamSceneModule = (TeamSceneModule*)pKernel->GetLogicModule("TeamSceneModule");
	Assert(m_pSecretSceneModule != NULL &&
		m_pTeamSceneModule != NULL);
#endif // FSROOMLOGIC_EXPORTS

    pKernel->AddIntCommandHook("SecretNpc", COMMAND_BEKILL, GroupNpcRefresh::RecordNpc);
    pKernel->AddIntCommandHook("SectorNpc", COMMAND_BEKILL, GroupNpcRefresh::RecordNpc);
    pKernel->AddIntCommandHook("EliteNpc", COMMAND_BEKILL, GroupNpcRefresh::RecordNpc);
    pKernel->AddIntCommandHook("player", COMMAND_SECRET_SCENE_ATTACK_BOSS, GroupNpcRefresh::AttackBoss);

    LoadResource(pKernel);

    v_funcList.resize(GROUP_SCENE_MAX, NULL);
    RegisterRefreshRule(GROUP_SCENE_SECRET_COMMON, CommonSecretRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_TREASURE, TreasureSecretRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_TEAM, TeamActivityRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_PET, PetSecretRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_MONEY, MoneySecretRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_ITEM, ItemSecretRule);
    RegisterRefreshRule(GROUP_SCENE_SECRET_ELITE, CommonSecretRule);
	RegisterRefreshRule(GROUP_SCENE_SECRET_GUARD, CommonSecretRule);
	 
	DECL_HEARTBEAT(GroupNpcRefresh::HB_ClearGroupNpc);
    DECL_LUA_EXT(nx_reload_npc_refresh_resource);

    return true;
}
bool GroupNpcRefresh::Shut(IKernel* pKernel)
{
    m_mapGroupNpcConfig.clear();
    v_funcList.clear();
    return true;
}

//加载配置
bool GroupNpcRefresh::LoadResource(IKernel* pKernel)
{
    m_mapGroupNpcConfig.clear();
    std::string xmlPath = pKernel->GetResourcePath();
    xmlPath += GROUP_NPC_REFRESH_CONFIG;

    CXmlFile xml(xmlPath.c_str());
    if (!xml.LoadFromFile())
    {
        extend_warning(LOG_WARNING, "[GroupNpcRefresh::npc_scene] failed");
        return false;
    }

    size_t nSecCount = xml.GetSectionCount();
    LoopBeginCheck(a);
    for (size_t i = 0; i < nSecCount; ++i)
    {
        LoopDoCheck(a);
        const char* section = xml.GetSectionByIndex(i);
        if (StringUtil::CharIsNull(section))
        {
            continue;
        }

        int id = atoi(section);
        int refreshbosstype = xml.ReadInteger(section, "CreateBossType", 0);
        int ikillamount = xml.ReadInteger(section, "KillAmount", 0);
        const char* bossConfigID = xml.ReadString(section, "Bossid", "");
        const char* bossPosi = xml.ReadString(section, "BossPos", "");
        
        GroupNpcInfo groupNpcInfo;
        groupNpcInfo.sceneID = id;
        groupNpcInfo.refreshBossType = refreshbosstype;
        groupNpcInfo.killAmount = ikillamount;
        groupNpcInfo.bossConfigID = bossConfigID;
        groupNpcInfo.bossPosi = bossPosi;

        
        m_mapGroupNpcConfig.insert(std::make_pair(id, groupNpcInfo));

    }
    return true;
}

//注册刷新规则
bool GroupNpcRefresh::RegisterRefreshRule(GROUP_SCENE_TYPE sceneType, GROUP_SCENE_REFRESH_FUNC func)
{
    if (sceneType <= GROUP_SCENE_NOT_SECRET || sceneType >= GROUP_SCENE_MAX)
    {
        return false;
    }
    if (func == NULL)
    {
        return false;
    }

    v_funcList[sceneType] = func;
    return true;
}

//记录野怪数量
int GroupNpcRefresh::RecordNpc(IKernel* pKernel,const PERSISTID& npc, const PERSISTID& player, const IVarList& args)
{
    IGameObj* pPlayerObj = pKernel->GetGameObj(player);
    IGameObj* pNpcObj = pKernel->GetGameObj(npc);
    if (pPlayerObj == NULL || pNpcObj == NULL)
    {
        return 0;
    }
    IGameObj* pSceneObj = pKernel->GetSceneObj();
    if (NULL == pSceneObj)
    {
        return 0;
    }
    int sceneID = pKernel->GetSceneId();

    int playerGroupID = pPlayerObj->QueryInt("GroupID");
    int npcGroupID = pNpcObj->QueryInt("GroupID");
    const char* script = pKernel->GetScript(npc);

    if (playerGroupID <= 0 || playerGroupID != npcGroupID)
    {
        return 0;
    }
    //记录杀死NPC信息
    m_pNpcCreatorModule->RecordKillNpc(pKernel, npc);

    IRecord* record = pSceneObj->GetRecord( FIELD_RECORD_ALL_GROUP_IDREC );
    if ( record == NULL)
    {
        return 0;
    }
    int row = record->FindInt(COLUMN_ALL_GROUP_IDREC_ID, playerGroupID);
    if ( row < 0 )
    {
        return 0;
    }

    time_t inTime = record->QueryInt64(row, COLUMN_ALL_GROUP_IDREC_ENTRY_TIME);//进入副本时间
    int killComm = record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_KILL_NPC_NUM );//杀死普通怪数量
    int killElite = record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_KILL_ELITE_NUM );//杀死精英怪数量
    int killBoss = record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_KILL_BOSS_NUM );
    int openBox = record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_OPEN_BOX_NUM );//开宝箱数量
    if ( killComm < 0 || killElite < 0 || killBoss < 0 || openBox < 0)
    {
        return 0;
    }
    int sceneType = GroupSceneBase::GetSceneType(pKernel, sceneID);

    int npcType = pNpcObj->QueryInt("NpcType");
    if (npcType == NPC_TYPE_ELITE)
    {
        //杀死精英怪
        record->SetInt( row, COLUMN_ALL_GROUP_IDREC_KILL_ELITE_NUM, killElite + 1 );
    }
    else if (npcType == NPC_TYPE_ORDINARY)
    {
        //杀死普通怪
        record->SetInt( row, COLUMN_ALL_GROUP_IDREC_KILL_NPC_NUM, killComm + 1 );
    }
    else if (npcType == NPC_TYPE_BOSS)
    {
        KillBoss(pKernel, player, npc, sceneID);

        record->SetInt(row, COLUMN_ALL_GROUP_IDREC_KILL_BOSS_NUM, killBoss + 1);
       /* if (sceneType == GROUP_SCENE_SECRET_COMMON 
			|| sceneType == GROUP_SCENE_SECRET_ELITE 
			||sceneType == GROUP_SCENE_SECRET_GUARD 
			||sceneType == GROUP_SCENE_SECRET_SNEAK_ON)
        {*/
            //关卡秘境杀死BOSS算结束
            record->SetInt64(row, COLUMN_ALL_GROUP_IDREC_END_TIME, ::time(NULL));
        //}

    }
    else if (npcType == NPC_TYPE_BOX)
    {
        //宝箱
        record->SetInt(row, COLUMN_ALL_GROUP_IDREC_OPEN_BOX_NUM, openBox + 1);
    }

    //PERSISTID boss = record->QueryObject(row, COLUMN_ALL_GROUP_IDREC_BOSS);


		killComm = record->QueryInt(row, COLUMN_ALL_GROUP_IDREC_KILL_NPC_NUM);//杀死普通怪数量
		killElite = record->QueryInt(row, COLUMN_ALL_GROUP_IDREC_KILL_ELITE_NUM);//杀死精英怪数量
		killBoss = record->QueryInt(row, COLUMN_ALL_GROUP_IDREC_KILL_BOSS_NUM);


#ifndef FSROOMLOGIC_EXPORTS
    //更新关卡秘境进行信息
    m_pSecretSceneModule->UpdateScreenAction(pKernel, player, npc);

		m_pNpcCreatorModule->upDateWheelNpc(pKernel, sceneID, playerGroupID, killComm + killElite + killBoss);
#endif // FSROOMLOGIC_EXPORTS

    if (v_funcList[sceneType] != NULL)
    {
        v_funcList[sceneType](pKernel, player, CVarList() << record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_KILL_NPC_NUM ) 
                                                          << record->QueryInt( row, COLUMN_ALL_GROUP_IDREC_KILL_ELITE_NUM ));
    }
    
    return 0;
}

//刷新BOSS
bool GroupNpcRefresh::RefreshBoss(IKernel * pKernel ,const PERSISTID& self, int sceneID, int killComm, int killElite)
{
    bool isQuitIng = pKernel->FindHeartBeat(self, "SecretSceneModule::HB_Delay_Open_End");
    int iRefreshType = m_pGroupNpcRefresh->RefreshBossType(sceneID);
    if (iRefreshType >= 0 && !isQuitIng)
    {
        bool bCanRefresh = false;
        if (iRefreshType == REFRESH_BOSS_TYPE_AMOUNT)
        {
            std::map<int, GroupNpcInfo>::iterator iter = m_pGroupNpcRefresh->m_mapGroupNpcConfig.find(sceneID);
            if (iter != m_pGroupNpcRefresh->m_mapGroupNpcConfig.end())
            {
                if (killComm == iter->second.killAmount)
                {
                    bCanRefresh = true;
                }
            }
        }
        else if (iRefreshType == REFRESH_BOSS_TYPE_ELITE)
        {
			IGameObj* pSceneObj = pKernel->GetSceneObj();
			if (pSceneObj != NULL)
			{
				int eliteNum = pSceneObj->QueryInt("EliteNum");
				if (killElite == eliteNum)
				{
					bCanRefresh = true;
				}
			}
        }

        if (bCanRefresh)
        {
            m_pGroupNpcRefresh->CreateBoss(pKernel, self);
        }
    }
    return true;
}

//生成秘境BOSS
void GroupNpcRefresh::CreateBoss(IKernel* pKernel, const PERSISTID& self)
{
#ifndef FSROOMLOGIC_EXPORTS
    if ( !pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
    {
        return ;
    }
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (pSelfObj == NULL)
    {
        return;
    }
    int sceneID = pKernel->GetSceneId();
    std::map<int, GroupNpcInfo>::iterator iter = m_pGroupNpcRefresh->m_mapGroupNpcConfig.find(sceneID);
    if (iter == m_pGroupNpcRefresh->m_mapGroupNpcConfig.end())
    {
        return ;
    }
    GroupNpcInfo& info = iter->second;
    const char* configID = info.bossConfigID.c_str();
    const char* bossPosi = info.bossPosi.c_str();
    if (StringUtil::CharIsNull(configID) || StringUtil::CharIsNull(bossPosi))
    {
        return ;
    }
    CVarList resultList;
    util_split_string(resultList, bossPosi, ",");
    if ((int)resultList.GetCount() != 3)
    {
        return ;
    }
    float x = resultList.FloatVal(0);
    float z = resultList.FloatVal(1);
    float orient = resultList.FloatVal(2);
    float y = pKernel->GetMapHeight(x, z);

    PERSISTID scene = pKernel->GetScene();
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
    if (NULL == pSceneObj)
    {
        return;
    }
    IRecord* group_record = pSceneObj->GetRecord(FIELD_RECORD_ALL_GROUP_IDREC);
    if (group_record == NULL)
    {
        return;
    }
    int groupID = pSelfObj->QueryInt("GroupID");
    if (groupID <= 0)
    {
        return ;
    }
    int row = group_record->FindInt(COLUMN_ALL_GROUP_IDREC_ID, groupID);
    if (row < 0)
    {
        return;
    }

    const PERSISTID boss = pKernel->CreateObjectArgs("", configID, 0, x, y, z, orient, 
                                            CVarList() << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << groupID);
    if (pKernel->Exists(boss))
    {
        group_record->SetObject(row, COLUMN_ALL_GROUP_IDREC_BOSS, boss);
    }
#endif // FSROOMLOGIC_EXPORTS
}

//BOSS被攻击
int GroupNpcRefresh::AttackBoss(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (!pKernel->Exists(self) || pSelfObj == NULL)
    {
        return 0;
    }
    int groupID = pSelfObj->QueryInt("GroupID");
    if (groupID <= 0)
    {
        return 0;
    }
    IGameObj* pSceneObj = pKernel->GetSceneObj();
    if (NULL == pSceneObj)
    {
        return 0;
    }
    IRecord* group_record = pSceneObj->GetRecord(FIELD_RECORD_ALL_GROUP_IDREC);
    if (group_record == NULL)
    {
        return 0;
    }
    int row = group_record->FindInt(COLUMN_ALL_GROUP_IDREC_ID, groupID);
    if (row < 0)
    {
        return 0;
    }
    time_t inTime = group_record->QueryInt64(row, COLUMN_ALL_GROUP_IDREC_ENTRY_TIME);

#ifndef FSROOMLOGIC_EXPORTS
    if (m_pSecretSceneModule->IsOverTime(inTime, pSelfObj))
    {
        return 0;
    }
#endif // FSROOMLOGIC_EXPORTS

    time_t attackBossTime = group_record->QueryInt64(row, COLUMN_ALL_GROUP_IDREC_BOSS_BE_ATTACK_TIME);
    time_t sysTime = ::time(NULL);
    if (attackBossTime < sysTime && attackBossTime != 0)
    {
        return 0;
    }
    group_record->SetInt64(row, COLUMN_ALL_GROUP_IDREC_BOSS_BE_ATTACK_TIME, sysTime);
    return 0;
}

//BOSS被击杀
int GroupNpcRefresh::KillBoss(IKernel* pKernel, const PERSISTID& player, const PERSISTID& boss, int sceneID)
{
    IGameObj* pPlayerObj = pKernel->GetGameObj(player);
    if (pPlayerObj == NULL)
    {
        return 0;
    }

    int sceneType = GroupSceneBase::GetSceneType(pKernel, sceneID);
    if ( sceneType == GROUP_SCENE_NOT_SECRET)
    {
        return 0;
    }
	
	
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (NULL != pSceneObj)
	{
		int groupID = pPlayerObj->QueryInt("GroupID");
		if (groupID > 0)
		{
			IRecord* group_record = pSceneObj->GetRecord(FIELD_RECORD_ALL_GROUP_IDREC);
			if (group_record != NULL)
			{
				int row = group_record->FindInt(COLUMN_ALL_GROUP_IDREC_ID, groupID);
				if (row >= 0)
				{
					time_t inTime = group_record->QueryInt64(row, COLUMN_ALL_GROUP_IDREC_ENTRY_TIME);

#ifndef FSROOMLOGIC_EXPORTS
					if (!m_pSecretSceneModule->IsOverTime(inTime, pPlayerObj))
					{
						time_t attackBossTime = group_record->QueryInt64(row, COLUMN_ALL_GROUP_IDREC_BOSS_BE_ATTACK_TIME);
						if (attackBossTime == 0)
						{
							time_t sysTime = ::time(NULL);
							group_record->SetInt64(row, COLUMN_ALL_GROUP_IDREC_BOSS_BE_ATTACK_TIME, sysTime);
						}
					}
#endif // FSROOMLOGIC_EXPORTS
				}
			}
		}
	}





#ifndef FSROOMLOGIC_EXPORTS
    if (sceneType == GROUP_SCENE_SECRET_TEAM)
    {
        m_pTeamSceneModule->OnBossKill(pKernel, boss, player, CVarList());
    }
    else
    {
        m_pSecretSceneModule->KillBoss(pKernel, player, boss, sceneID);
    }
#endif // FSROOMLOGIC_EXPORTS

    return 0;
}

//刷BOSS机制
int GroupNpcRefresh::RefreshBossType(int sceneID)
{
    std::map<int, GroupNpcInfo>::iterator iter = m_pGroupNpcRefresh->m_mapGroupNpcConfig.find(sceneID);
    if (iter == m_pGroupNpcRefresh->m_mapGroupNpcConfig.end())
    {
        return REFRESH_BOSS_TYPE_WRONG;
    }
    GroupNpcInfo& info = iter->second;

    return info.refreshBossType;
}
//杀怪数标准
int GroupNpcRefresh::GetKillNpcAmount(int sceneID)
{
    std::map<int, GroupNpcInfo>::iterator iter = m_pGroupNpcRefresh->m_mapGroupNpcConfig.find(sceneID);
    if (iter == m_pGroupNpcRefresh->m_mapGroupNpcConfig.end())
    {
        return REFRESH_BOSS_TYPE_WRONG;
    }
    GroupNpcInfo& info = iter->second;

    return info.killAmount;
}

//关卡秘境规则
bool GroupNpcRefresh::CommonSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    int groupID = pSelfObj->QueryInt("GroupID");
    IGameObj* pSceneObj = pKernel->GetSceneObj();
    if (NULL == pSceneObj)
    {
        return false;
    }
    IRecord* groupRec = pSceneObj->GetRecord(FIELD_RECORD_ALL_GROUP_IDREC);
    if (groupRec == NULL)
    {
        return false;
    }
    int row = groupRec->FindInt(COLUMN_ALL_GROUP_IDREC_ID, groupID);
    PERSISTID boss = groupRec->QueryObject(row, COLUMN_ALL_GROUP_IDREC_BOSS);
    int sceneID = pKernel->GetSceneId();
    int killComm = args.IntVal(0);
    int killElite = args.IntVal(1);
    if (boss.IsNull())
    {
        //关卡秘境刷新BOSS
        m_pGroupNpcRefresh->RefreshBoss(pKernel, self, sceneID, killComm, killElite);
    }

#ifndef FSROOMLOGIC_EXPORTS
	int subType = m_pSecretSceneModule->GetSubType(SecretSceneModule::GetSecretIndex(pSelfObj));
    if (subType == SCENE_SUB_TYPE_OPEN)
    {

        m_pSecretSceneModule->QueryNpc(pKernel, self);
    }
#endif // FSROOMLOGIC_EXPORTS

    return true;
}
//试剑亭规则
bool GroupNpcRefresh::TreasureSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
#ifndef FSROOMLOGIC_EXPORTS
    m_pSecretSceneModule->QueryNpc(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
    return true;
}
//组队活动副本规则
bool GroupNpcRefresh::TeamActivityRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    int groupID = pSelfObj->QueryInt("GroupID");
    IGameObj* pSceneObj = pKernel->GetSceneObj();
    if (NULL == pSceneObj)
    {
        return false;
    }
    IRecord* groupRec = pSceneObj->GetRecord(FIELD_RECORD_ALL_GROUP_IDREC);
    if (groupRec == NULL)
    {
        return false;
    }
    int row = groupRec->FindInt(COLUMN_ALL_GROUP_IDREC_ID, groupID);






    PERSISTID boss = groupRec->QueryObject(row, COLUMN_ALL_GROUP_IDREC_BOSS);
    int killComm = args.IntVal(0);
    int killElite = args.IntVal(1);
    if (boss == PERSISTID())
    {
        //关卡秘境刷新BOSS
        m_pGroupNpcRefresh->RefreshBoss(pKernel, self, pKernel->GetSceneId(), killComm, killElite);
    }
    return true;
}
//宠物秘境规则
bool GroupNpcRefresh::PetSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
    return true;
}

//天将铜钱规则
bool GroupNpcRefresh::MoneySecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
#ifndef FSROOMLOGIC_EXPORTS
    m_pSecretSceneModule->QueryNpc(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
    return true;
}
//夺命剑州规则
bool GroupNpcRefresh::ItemSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
#ifndef FSROOMLOGIC_EXPORTS
    m_pSecretSceneModule->QueryNpc(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS
    return true;
}


int GroupNpcRefresh::HB_ClearGroupNpc(IKernel* pKernel, const PERSISTID& self, int silce)
{

	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (pSelf == NULL)
	{
		return 0;
	}
	int groupID = pSelf->QueryInt(FIELD_PROP_GROUPID);
	if (groupID <= 0)
	{
		return 0;
	}
	
	CVarList res;
	pKernel->GroupChildList(groupID, TYPE_NPC, res);
	LoopBeginCheck(a);
	for (size_t i = 0; i < res.GetCount(); i++){
		LoopDoCheck(a);
		PERSISTID& obj = res.ObjectVal(i);
		if (pKernel->Exists(obj))
		{
			pKernel->DestroySelf(obj);
		}
	}
	return  0;

}

//停止当前分组内obj行为
void GroupNpcRefresh::StopGroupObj(IKernel *pKernel, int groupID, int waitTime)
{

	int sceneID = pKernel->GetSceneId();
	int sceneType = GroupSceneBase::GetSceneType(pKernel, sceneID);
	if (sceneType == GROUP_SCENE_NOT_SECRET){
		return;
	}

	CVarList control_list;
	control_list << CB_CANT_MOVE
		<< CB_CANT_USE_SKILL
		<< CB_CANT_BE_ATTACK
		<< CB_CANT_ATTACK;

		CVarList res;
		if (groupID <= 0)
		{
			return;
		}

		pKernel->GroupChildList(groupID,TYPE_NPC, res);
		LoopBeginCheck(a);
		for (size_t i = 0; i < res.GetCount(); i++){
			LoopDoCheck(a);
			PERSISTID& obj = res.ObjectVal(i);
			IGameObj* pObj = pKernel->GetGameObj(obj);
			if (NULL == pObj)
			{
				continue;;
			}
			if (!pObj->FindAttr(FIELD_PROP_NPC_TYPE))
			{
				continue;
			}
			int npcType = pObj->QueryInt(FIELD_PROP_NPC_TYPE);
			switch (npcType)
			{
				case NPC_TYPE_ORDINARY:
				case NPC_TYPE_ELITE:
				case NPC_TYPE_BOSS:
				case NPC_TYPE_CONVOY:
				{
						FightInterfaceInstance->AddControlBuffer(pKernel, obj, obj, control_list, waitTime);
				}break;
			};
		}
		res.Clear();
		pKernel->GroupChildList(groupID, TYPE_PLAYER, res);

		LoopBeginCheck(b);
		for (size_t i = 0; i < res.GetCount(); i++){
			LoopDoCheck(b);
			PERSISTID& obj = res.ObjectVal(i);
				FightInterfaceInstance->AddControlBuffer(pKernel, obj, obj, control_list, waitTime);

			};


}

void GroupNpcRefresh::StartGroupObj(IKernel *pKernel, int groupID,int bufferTimeType)
{

	int sceneID = pKernel->GetSceneId();
	int sceneType = GroupSceneBase::GetSceneType(pKernel, sceneID);
	if (sceneType == GROUP_SCENE_NOT_SECRET){
		return;
	}

	CVarList control_list;
	control_list << CB_CANT_MOVE
		<< CB_CANT_USE_SKILL
		<< CB_CANT_BE_ATTACK
		<< CB_CANT_ATTACK;

	if (groupID <= 0)
	{
		return;
	}

	CVarList res;
	pKernel->GroupChildList(groupID, TYPE_NPC, res);
	LoopBeginCheck(a);
	for (size_t i = 0; i < res.GetCount(); i++){
		LoopDoCheck(a);
		PERSISTID& obj = res.ObjectVal(i);
		IGameObj* pObj = pKernel->GetGameObj(obj);
		if (NULL == pObj)
		{
			continue;;
		}
		if (!pObj->FindAttr(FIELD_PROP_NPC_TYPE))
		{
			continue;
		}

		int npcType = pObj->QueryInt(FIELD_PROP_NPC_TYPE);
		switch (npcType)
		{
			case NPC_TYPE_ORDINARY:
			case NPC_TYPE_ELITE:
			case NPC_TYPE_BOSS:
			case NPC_TYPE_CONVOY:
			{
				FightInterfaceInstance->RemoveControlBuffer(pKernel, obj, control_list, bufferTimeType);
			}
			break;
		};
	}



	res.Clear();
	pKernel->GroupChildList(groupID, TYPE_PLAYER, res);

	LoopBeginCheck(b);
	for (size_t i = 0; i < res.GetCount(); i++){
		LoopDoCheck(b);
		PERSISTID& obj = res.ObjectVal(i);
		FightInterfaceInstance->RemoveControlBuffer(pKernel, obj,  control_list, bufferTimeType);

	};




}

void GroupNpcRefresh::DelayClearGroupNpc(IKernel*pKernel, int groupID)
{
	PERSISTID curScene = pKernel->GetScene();
	IGameObj* pSceneObj = pKernel->GetGameObj(curScene);
	if (NULL == pSceneObj)
	{
		return ;
	}
	PERSISTID npc = pKernel->Create(curScene, CLASS_NAME_DISCARD_HELPER);
	IGameObj* pNpcObj = pKernel->GetGameObj(npc);
	if (!pKernel->Exists(npc) || pNpcObj == NULL)
	{
		return ;
	}
	pNpcObj->SetInt("GroupID", groupID);
	ADD_COUNT_BEAT(pKernel,npc,"GroupNpcRefresh::HB_ClearGroupNpc",3000,1);
	NpcCreatorModule::m_pNpcCreatorModule->CleanSceneCfgName(pKernel, groupID);
}
