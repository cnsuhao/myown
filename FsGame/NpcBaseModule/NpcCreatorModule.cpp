//--------------------------------------------------------------------
// 文件名:      NpcCreatorModule.cpp
// 内  容:      NPC创建
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "FsGame/NpcBaseModule/NpcCreatorModule.h"
//#include "FsGame/Define/SceretSceneDefine.h"
#include "FsGame/Define/GameDefine.h"
#include <time.h>
#include <math.h>
#include <io.h>
#include <list>
#include <algorithm>
#include "utils/XmlFile.h"
#include "utils/string_util.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
//#include "FsGame/SceneBaseModule/GroupScene/GroupSceneBase.h"
//#include "FsGame/Define/BranchSceneDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/custom_func.h"
#include "FsGame/Define/FightDefine.h"
#include "FsGame/Interface/FightInterface.h"

#define NPC_POOL "ini/npc/npc_config/npc_pool.xml"
#define TASK_CREATOR "ini/npc/npc_config/TaskCreator.xml"
#define GUILD_INBREAK "ini/npc/npc_config/guild_inbreak.xml"

// 数据声明
NpcCreatorModule* NpcCreatorModule::m_pNpcCreatorModule = NULL;

// 初始化
bool NpcCreatorModule::Init(IKernel* pKernel)
{
	// 模块指针
	m_pNpcCreatorModule = this;

	// 注册回调
	pKernel->AddClassCallback("RandomNpcCreator", "OnCreateClass",
		OnRandomNpcCreatorCreateClass);

	pKernel->AddClassCallback("RandomNpcCreatorItem", "OnCreateClass",
		OnRandomNpcCreatorItemCreateClass);

	pKernel->AddEventCallback("RandomNpcCreator", "OnSpring",
		OnRandomNpcCreatorSpring);

	pKernel->AddEventCallback("RandomNpcCreator", "OnEndSpring",
		OnRandomNpcCreatorEndSpring);

	pKernel->AddEventCallback("scene", "OnCreate", OnSceneCreate);

	pKernel->AddEventCallback("npc", "OnDestroy", OnNpcDestroy);

// 	pKernel->AddEventCallback("FunctionNpc", "OnSpring", OnNpcSpring);
// 
// 	pKernel->AddEventCallback("FunctionNpc", "OnEndSpring", OnNpcEndSpring);
	// 心跳注册
	DECL_HEARTBEAT(NpcCreatorModule::H_Refresh);
	DECL_HEARTBEAT(NpcCreatorModule::H_EndRefresh);
	DECL_HEARTBEAT(NpcCreatorModule::H_DelayCreateStaticNpc);
	DECL_HEARTBEAT(NpcCreatorModule::H_DelayCreateRandomNpc);
	DECL_HEARTBEAT(NpcCreatorModule::H_GroupSceneCreateStaticNpc);
	DECL_HEARTBEAT(NpcCreatorModule::H_GroupSceneCreateRandomNpc);
	//DECL_HEARTBEAT(NpcCreatorModule::H_GroupCreateNpcByTime);

    DECL_CRITICAL(NpcCreatorModule::C_OnCurStepChanged);
	

	// 载入资源
	extend_warning(pKernel, "Load NpcCreatorModule resource begin...");

	LoadNpcInfo(pKernel);
    LoadNpcPool(pKernel);
	
	if (!LoadTaskCreatorRes(pKernel))
	{
		Assert(false);
		return false;
	}
	//LoadGuildInbreak(pKernel);

	extend_warning(pKernel, "Load NpcCreatorModule resource end.");

	srand((unsigned int)time(NULL));
	return true;
}

void NpcCreatorModule::ClearXMLData()
{
    LoopBeginCheck(check_a);
	for(auto mapIter = m_clone_npc_info.begin(); mapIter != m_clone_npc_info.end() ;++mapIter )
	{
        LoopDoCheck(check_a);
        LoopBeginCheck(check_b);
		for(int iVec = 0; iVec < (int)mapIter->second.size();iVec++)
		{
            LoopDoCheck(check_b);
			if( mapIter->second[iVec] )
				delete  mapIter->second[iVec];
		}
	}

	
  

    LoopBeginCheck(check_e);
    for(std::vector<char*>::iterator vecfileiter = m_vecbuff.begin(); vecfileiter != m_vecbuff.end() ;++vecfileiter )
    {
        LoopDoCheck(check_e);
        delete *vecfileiter;
    }
}

void NpcCreatorModule::ParseCreateNpcByTime(const std::string& sceneCfgName, const std::vector<xml_document<>*>& tempDocVec)
{
	
	int iDocCount = (int)tempDocVec.size();
	LoopBeginCheck(a);
	for (int fileIndex = 0; fileIndex < iDocCount; fileIndex++)
	{
		LoopDoCheck(a);
		xml_node<>* pNodeRoot = tempDocVec[fileIndex]->first_node("object");
		if (pNodeRoot == NULL)
			continue;
		// 创建随机对象生成器
		xml_node<>* pNode = pNodeRoot->first_node("createByTime");

		if (pNode != NULL)
		{

			xml_node<>* pCreator = pNode->first_node("creator");

			std::vector<RefreshNpcRule> npcInfo;
			LoopBeginCheck(check_l1);
			while (pCreator != NULL){
				LoopDoCheck(check_l1);
				int waitTime = convert_int(QueryXmlAttr(pCreator, "waitTime"), 0);
				int refreshTime = convert_int(QueryXmlAttr(pCreator, "refreshtime"), 0);
				int totalStep = convert_int(QueryXmlAttr(pCreator, "totalstep"), 0);

				RefreshNpcRule m_GuardNpc;

				m_GuardNpc._totalStep = totalStep;
				m_GuardNpc._refreshTime = refreshTime;
				m_GuardNpc._waitTime = waitTime;
				xml_node<>* pItem = pCreator->first_node("item");

				if (pItem == NULL) {
					continue;
				}

				LoopBeginCheck(check_l2);
				while (pItem != NULL)
				{
					LoopDoCheck(check_l2);
					// 坐标
					float x = convert_float(QueryXmlAttr(pItem, "x"));
					float y = convert_float(QueryXmlAttr(pItem, "y"));
					float z = convert_float(QueryXmlAttr(pItem, "z"));
					int amount = convert_int(QueryXmlAttr(pItem, "amount"));
					// NPC编号
					const char* npc_id = QueryXmlAttr(pItem, "id");

					float orient = convert_float(QueryXmlAttr(pItem, "ay"));

					m_GuardNpc._npcInfo.push_back(NpcInfo(npc_id, x, y, z, orient, amount));

					pItem = pItem->next_sibling("item");
				}

				npcInfo.push_back(m_GuardNpc);
				pCreator = pCreator->next_sibling("creator");

			};
			m_refresh_npc_by_time.insert(std::make_pair(sceneCfgName, npcInfo));
		}

	}


}

void NpcCreatorModule::ParseWheelNpc(const std::string& sceneCfgName, const std::vector<xml_document<>*>& tempDocVec)
{
	int iDocCount = (int)tempDocVec.size();
	LoopBeginCheck(a);
	for (int fileIndex = 0; fileIndex < iDocCount; fileIndex++)
	{
		LoopDoCheck(a);
		xml_node<>* pNodeRoot = tempDocVec[fileIndex]->first_node("object");
		if (pNodeRoot == NULL)
			continue;
		// 创建随机对象生成器
		xml_node<>* pNode = pNodeRoot->first_node("wheelcreator");
		LoopBeginCheck(check_l1);
		while (pNode != NULL)
		{
				LoopBeginCheck(check_l1);
				//key 杀怪数量
				std::map<int ,std::vector<WheelNpc> > wheelNpcGroup;
				xml_node<>* pNpcGroup = pNode->first_node("stepcreate");
				while (pNpcGroup != NULL)
				{
					std::vector<WheelNpc> wheelNpc;
					xml_node<>* pItem = pNpcGroup->first_node("item");
					LoopBeginCheck(check_l2);
					while (pItem != NULL)
					{
						LoopDoCheck(check_l2);

						// 坐标
						float x = convert_float(QueryXmlAttr(pItem, "x"));
						float y = convert_float(QueryXmlAttr(pItem, "y"));
						float z = convert_float(QueryXmlAttr(pItem, "z"));
						int amount = convert_int(QueryXmlAttr(pItem, "amount"));
						int killNum = convert_int(QueryXmlAttr(pItem, "needKillNum"));

						// NPC编号
						const char* npc_id = QueryXmlAttr(pItem, "id");

						float orient = convert_float(QueryXmlAttr(pItem, "ay"));

						wheelNpc.push_back(WheelNpc(npc_id, x, y, z, orient, amount, killNum));

						pItem = pItem->next_sibling("item");
					}
						
					int groupNeedKillNum = convert_int(QueryXmlAttr(pNpcGroup, "needKillNum"));

					if (!wheelNpc.empty())
					{
						wheelNpcGroup.insert(make_pair(groupNeedKillNum,wheelNpc));
					}
					pNpcGroup = pNpcGroup->next_sibling("stepcreate");
				}
				if (!wheelNpcGroup.empty())
				{
					m_wheel_npc.insert(std::make_pair(sceneCfgName, wheelNpcGroup));
				}
				pNode = pNode->next_sibling("wheelcreator");
			};
		}

}



// 关闭
bool NpcCreatorModule::Shut(IKernel* pKernel)
{
    ClearXMLData();
    m_mapNpcPool.clear();
    v_vecSceneNpc.clear();
	m_TaskCreatorMap.clear();
	return true;
}

// 随机对象生成器
int NpcCreatorModule::OnRandomNpcCreatorCreateClass(IKernel* pKernel, int index)
{
	createclass_from_xml(pKernel, index, "struct\\npc\\creator\\randomnpc_creator.xml");
	return 0;
}

// 随机对象生成器的子项
int NpcCreatorModule::OnRandomNpcCreatorItemCreateClass(IKernel* pKernel, int index)
{
	createclass_from_xml(pKernel, index, "struct\\npc\\creator\\randomnpc_creatoritem.xml");

	return 0;
}

// 触发
int NpcCreatorModule::OnRandomNpcCreatorSpring(IKernel* pKernel, const PERSISTID& creator, 
											   const PERSISTID& sender, const IVarList& args)
{
    const char* script = pKernel->GetScript(sender);
	// 保护
	if (pKernel->Type(sender) != TYPE_PLAYER && strcmp(script, "ConvoyNpc") != 0)
	{
        //玩家和护送NPC都可以触发
		return 0;
	}

	//不是通过触发来生成NPC的类型
    IGameObj* pCreator = pKernel->GetGameObj(creator);
    if (pCreator == NULL)
    {
        return 0;
    }
    if (pCreator->QueryInt("CreateType") != 0)
    {
        return 0;
    }

    //关卡秘境玩家行进步骤
    IGameObj* pPlayer = pKernel->GetGameObj(sender);
    if (pPlayer == NULL)
    {
        return 0;
    }
    if (pPlayer->FindAttr("Action"))
    {
        int action = pPlayer->QueryInt("Action");
        int sceneAction = pCreator->QueryInt("SceneAction");
        if (sceneAction > 0 && action != sceneAction)
        {
            //是关卡秘境且玩家没有行进到此触发器，不触发
            return 0;
        }
    }

	// 记录当前进行触发范围的数据
	int new_count = pCreator->QueryInt("SpringCount") + 1;

	if (new_count < 1)
	{
		new_count = 1;
	}

	pCreator->SetInt("SpringCount", new_count);

	if (new_count >= 1)
	{
		// 开始刷新
		BeginRefresh(pKernel, creator);

        if (pCreator->QueryInt("TotalStep") > 1)
        {
            ADD_CRITICAL(pKernel, creator, "CurStep", "NpcCreatorModule::C_OnCurStepChanged");
        }
	}
		
	return 0;
}

// 结束触发
int NpcCreatorModule::OnRandomNpcCreatorEndSpring(IKernel* pKernel, const PERSISTID& creator, 
												  const PERSISTID& sender, const IVarList& args)
{
	// 保护
	if (pKernel->Type(sender) != TYPE_PLAYER)
	{
		return 0;
	}
    IGameObj* pCreator = pKernel->GetGameObj(creator);
    if (pCreator == NULL)
    {
        return 0;
    }

    int isDestroy = pCreator->QueryInt("IsDestroy");
    if (isDestroy == 1)
    {
        return 0;
    }

	// 记录当前还在触发范围的数据
	int new_count = pCreator->QueryInt("SpringCount") - 1;

	if (new_count < 0)
	{
		new_count = 0;
	}

	pCreator->SetInt("SpringCount", new_count);

	if (new_count == 0)
	{
		// 停止刷新
		EndRefresh(pKernel, creator);
	}

	return 0;
}

// int NpcCreatorModule::OnNpcSpring(IKernel* pKernel, const PERSISTID& creator, const PERSISTID& sender, const IVarList& args)
// {
// 	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
// 	if (NULL == pSenderObj || pSenderObj->GetClassType() != TYPE_PLAYER)
// 	{
// 		return 0;
// 	}
// 
// 	pKernel->AddVisualPriority(sender, creator, true);
// 	return 0;
// }
// 
// int NpcCreatorModule::OnNpcEndSpring(IKernel* pKernel, const PERSISTID& creator, const PERSISTID& sender, const IVarList& args)
// {
// 	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
// 	if (NULL == pSenderObj || pSenderObj->GetClassType() != TYPE_PLAYER)
// 	{
// 		return 0;
// 	}
// 
// 	pKernel->RemoveVisualPriority(sender, creator);
// 	return 0;
// }

// 创建静态NPC和触发器
int NpcCreatorModule::OnSceneCreate(IKernel* pKernel, const PERSISTID& scene, 
									const PERSISTID& sender, const IVarList& args)
{
	// 如果是副本 则先不放npc进去
	if (pKernel->GetSceneClass() == 2)
	{
		return 0;
	}

	// 得到场景NPC创建配置文件
	fast_string creator_file;
	fast_string scene_config = pKernel->GetConfig(scene);

	// 实际文件夹
	size_t n_pos = scene_config.find_last_of('\\');

	scene_config = scene_config.substr(n_pos + 1, scene_config.length() - n_pos - 1);
	auto it = m_pNpcCreatorModule->m_group_npc_info.find(scene_config.c_str());
	if (it == m_pNpcCreatorModule->m_group_npc_info.end()){
		return 0;
	}
		// 遍历文件夹下所有文件
	for (auto its:it->second)
	{
		m_pNpcCreatorModule->CreateStaticNpc(pKernel, scene, its.m_createStaticGroup);
		m_pNpcCreatorModule->CreateRandomNpc(pKernel, scene, its.m_createRandGroup);
	}

    //秘境场景在创建时记录场景内NPC数量
    m_pNpcCreatorModule->GetSceneNpcNum(pKernel, scene);

	return 0;
}

// NPC被删除
int NpcCreatorModule::OnNpcDestroy(IKernel* pKernel, const PERSISTID& npc, 
								   const PERSISTID& sender, const IVarList& args)
{
    IGameObj* pNpc = pKernel->GetGameObj(npc);
    if (pNpc == NULL)
    {
        return 0;
    }
	// 取得生成器
	if (!pNpc->FindData("CreatorItem"))
	{
		return 0;
	}

	PERSISTID creator_item = pNpc->QueryDataObject("CreatorItem");

	if (!pKernel->Exists(creator_item))
	{
		return 0;
	}
    IGameObj* pCreatorItem = pKernel->GetGameObj(creator_item);
    if (pCreatorItem == NULL)
    {
        return 0;
    }

	PERSISTID creator = pKernel->Parent(creator_item);
    IGameObj* pCreatorObj = pKernel->GetGameObj(creator);

	if (!pKernel->Exists(creator) || NULL == pCreatorObj)
	{
		return 0;
	}

	// 释放位置
	int pos_row = pNpc->QueryDataInt("CreatorPosition");
    IRecord* positionRec = pCreatorItem->GetRecord(POSITION_INFO_REC);
    if (positionRec == NULL)
    {
        return 0;
    }

	if (pos_row < positionRec->GetRows())
	{
        positionRec->SetInt(pos_row, POSIINFOREC_COL_USED, 0);
        positionRec->SetInt64(pos_row, POSIINFOREC_COL_TIME, 0);
	}

	// 减少creator_item创建的NPC的当前数量
	int max_amount = pCreatorItem->QueryInt("MaxAmount");
	int cur_amount = pCreatorItem->QueryInt("CurAmount");

	if (cur_amount > 0)
	{
		cur_amount--;
	}

	pCreatorItem->SetInt("CurAmount", cur_amount);

	IRecord* objectRec = pCreatorObj->GetRecord(CREATE_OBJECT_LIST_REC);
    if (objectRec == NULL)
    {
        return 0;
    }
    // 从生成器创建的NPC列表中删除
	int row = objectRec->FindObject(0, npc);

	if (row < 0)
	{
		return 0;
	}

    objectRec->RemoveRow(row);

	// 因为停止刷新被删除的，不做任何处理
	if (!pNpc->FindData("CreatorEndRefresh"))
	{
		// 因为怪物被杀死的，重置当前时间为死亡时间
		if (pos_row < positionRec->GetRows())
		{
            positionRec->SetInt64(pos_row, POSIINFOREC_COL_TIME, util_get_time_64());
		}

		// 通知生成器刷新
		if (cur_amount < max_amount)
		{
			BeginRefresh(pKernel, creator);
		}
	}

	return 0;
}

// 心跳
int NpcCreatorModule::H_Refresh(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	// 保护
	if (!pKernel->Exists(creator))
	{
		return 0;
	}
    IGameObj* pCreator = pKernel->GetGameObj(creator);
    if (pCreator == NULL)
    {
        return 0;
    }

	int nGroupID = pCreator->QueryInt("GroupID");

	// 需要下次心跳再更新
	bool bneedrefresh = false;

	// 下次刷新的时间
	__int64 now = util_get_time_64();

	__int64 min_time = 3600 * 24 * 30;

	CVarList temp;

	//创建类型默认0，1是只创建精英怪，2创建精英怪附带分批小怪
	int iCreateType = pCreator->QueryInt("CreateType");
	// 场景对象
	PERSISTID scene = pKernel->GetScene();
	
	CVarList creator_items;
	pKernel->GetChildList(creator, TYPE_HELPER, creator_items);

	// 遍历
	int creator_item_counts = (int)creator_items.GetCount();

    LoopBeginCheck(check_g);
	for (int i = 0; i < creator_item_counts; i++)
	{
        LoopDoCheck(check_g);
		PERSISTID creator_item = creator_items.ObjectVal(i);
        IGameObj* pCreatorItem = pKernel->GetGameObj(creator_item);
        if (pCreatorItem == NULL)
        {
            continue;
        }
        // 数量比较
        int max_amount = pCreatorItem->QueryInt("MaxAmount");
        int cur_amount = pCreatorItem->QueryInt("CurAmount");

        if (pCreator->QueryInt("TotalStep") > 1)
        {
            //分批次刷新
            int curStep = pCreator->QueryInt("CurStep");//当前波数
            int step = pCreatorItem->QueryInt("Step");//怪物所属波数
            if (curStep != step)
            {
                continue;
            }
            //记录此创建器的怪物信息（创建个数，被杀个数）
            if (!pCreator->FindData("CurNum"))
            {
                pCreator->AddDataInt("CurNum", 0);
            }
            pCreator->SetDataInt("CurNum", max_amount);
            if (!pCreator->FindData("KillNum"))
            {
                pCreator->AddDataInt("KillNum", 0);
            }  
        }

		if (cur_amount >= max_amount)
		{
			continue;
		}

		// 刷新周期
		int refresh_period = pCreatorItem->QueryInt("RefreshPeriod");

		// 需要生成NPC和数量
		const char* config = pCreatorItem->QueryString("NpcID");

        if (StringUtil::CharIsNull(config))
		{
			continue;
		}

		int refresh_count = max_amount - cur_amount;

		// 位置使用的对象
		PERSISTID& pos_obj = creator_item;

		// 迭代计数(记录真正要刷新的数量)
		int real_refresh_count = refresh_count;

		if (real_refresh_count <= 0)
		{
			continue;
		}

		IRecord* pRecord = pCreatorItem->GetRecord(POSITION_INFO_REC);

		if (NULL == pRecord)
		{
			continue;
		}

		// 最大position数
		int max_postion_rows = pRecord->GetRows();

		if (max_postion_rows < max_amount)
		{
			// 配置错误不刷新
			continue;
		}

		// 循环将可用位置加入list
		std::list<int> posList;

        LoopBeginCheck(check_h);
		for (int e = 0; e < max_postion_rows; ++e)
		{
            LoopDoCheck(check_h);
			int bUse = pRecord->QueryInt(e, POSIINFOREC_COL_USED);

			if (bUse <= 0)
			{
				// 判断刷新周期是否达到
				__int64 last_dead_time = pRecord->QueryInt64(e, POSIINFOREC_COL_TIME);
				
				__int64 lapse_time = now - last_dead_time;

				if ( lapse_time < (__int64)refresh_period)
				{
					__int64 next_time = (__int64)refresh_period - (now - last_dead_time);

					if (next_time < min_time)
					{
						min_time = next_time;
					}

					// 刷新时间未到
					bneedrefresh = true;

					// 真正要刷新的数量减1
					real_refresh_count -= 1;

					continue;
				}

				if ( -1 == refresh_period && 0 != last_dead_time )
				{
					// 刷新时间无限长时，只允许刷新一次
					bneedrefresh = false;
					
					real_refresh_count = 0;

					posList.clear();

					break;
				}

				// 达到时间了就记录刷新位置list
				posList.push_back(e);
			}
		}

		// 如果没有真正可以刷的，则不刷新
		if (real_refresh_count <= 0)
		{
			// 是否需要再次调用刷新心跳
			if (bneedrefresh)
			{
				if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_Refresh"))
				{
					pKernel->RemoveHeartBeat(creator, "NpcCreatorModule::H_Refresh");
				}
				
				pKernel->AddCountBeat(creator, "NpcCreatorModule::H_Refresh", (int)min_time, 1);
			}

			return 0;
		}

		// 可用位置数量
		int can_refresh_pos_count = (int)posList.size();
		if (can_refresh_pos_count == 0)
		{
			return 0;
		}

		if (real_refresh_count > can_refresh_pos_count)
		{
			real_refresh_count = can_refresh_pos_count;
		}

		// 刷出的数量
		int r_count = 0;

		PERSISTID RandCreator = pCreator->QueryObject("RandHelper");
		int iRandRow = pCreator->QueryInt("RandRow");
		// 真正需要刷新的数量
        LoopBeginCheck(check_i);
		for (int j = 0; j < real_refresh_count; ++j)
		{
            LoopDoCheck(check_i);
			int row_index = 0;
			if (pKernel->Exists(RandCreator) && creator_item == RandCreator)
			{
				row_index = iRandRow;
			}
			else
			{
				// 随机一个位置
				int r_pos = rand()%can_refresh_pos_count;

				// 取得位置行数
				std::list<int>::iterator it = posList.begin();

                LoopBeginCheck(check_j);
				for (int t = 0; t < r_pos; ++t)
				{
                    LoopDoCheck(check_j);
					++it;
				}

				// 得到行号
				row_index = *it;

				// 删除
				posList.erase(it);
			}
		
			// 创建Npc
			float r = pRecord->QueryFloat(row_index, POSIINFOREC_COL_R);
			float y = pRecord->QueryFloat(row_index, POSIINFOREC_COL_Y);

			float x, z;
			if (r <= 0)
			{
				x = pRecord->QueryFloat(row_index, POSIINFOREC_COL_X);
				z = pRecord->QueryFloat(row_index, POSIINFOREC_COL_Z);
			}
			else
			{
				x = pRecord->QueryFloat(row_index, POSIINFOREC_COL_X) + (util_random_float(2.0f) - 1.0f) * r;
				z = pRecord->QueryFloat(row_index, POSIINFOREC_COL_Z) + (util_random_float(2.0f) - 1.0f) * r;
				y = GetCurFloorHeight(pKernel, x, y, z);
			}

            float o = pRecord->QueryFloat(row_index, POSIINFOREC_COL_O);
			//角度-1随机一个
			if (o < 0)
			{
				o = util_random_float(2*PI);
			}

            std::string newConfig;
            m_pNpcCreatorModule->GetConfigID(pKernel, creator, config, newConfig);
            if (StringUtil::CharIsNull(newConfig.c_str()))
            {
                newConfig = config;
            }
            
			// 带参数创建NPC
			CVarList create_args;
			create_args << CREATE_TYPE_PROPERTY_VALUE
						<< FIELD_PROP_GROUP_ID 
						<< nGroupID;

			// 怪物按等级刷属性包
			if (pCreator->FindData("CreatorLevel"))
			{
				create_args << FIELD_PROP_NPC_PACKAGE_LEVEL
							<< pCreator->QueryDataInt("CreatorLevel");
			}

			PERSISTID npc = pKernel->CreateObjectArgs("", newConfig.c_str(), 0, x, y, z, o, create_args);
			if (pKernel->Exists(npc))
			{
                IGameObj* pNpc = pKernel->GetGameObj(npc);
                if (pNpc == NULL)
                {
                    continue;
                }
				// 设置NPC由哪个生成器生成
                pNpc->AddDataObject("CreatorItem", creator_item);
                pNpc->AddDataInt("CreatorPosition", row_index);
                pNpc->AddDataObject("Creator", creator);

				if(pNpc->FindAttr("BornX"))
				{
					pNpc->SetFloat("BornX", x);
					pNpc->SetFloat("BornZ", z);
				}

				// 添加到本生成器生成的对象列表中
                IRecord* objectRec = pCreator->GetRecord(CREATE_OBJECT_LIST_REC);
                if (objectRec != NULL)
                {
                    objectRec->AddRowValue(-1, CVarList() << npc);
                }
				
				pRecord->SetInt(row_index, POSIINFOREC_COL_USED, 1);

				// 怪物按等级刷属性包
				if (pCreator->FindData("CreatorLevel"))
				{
					pNpc->SetInt(FIELD_PROP_LEVEL, pCreator->QueryDataInt("CreatorLevel"));
				}

				// 计数器
				r_count++;
			}

			// 剩余数量
			can_refresh_pos_count--;

			if (can_refresh_pos_count <= 0)
			{
				break;
			}
		}

		// 当前数量增加
		pCreatorItem->SetInt("CurAmount", cur_amount + r_count);
	}

	// 是否需要再次调用本心跳
	if (bneedrefresh)
	{
		if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_Refresh"))
		{
			pKernel->RemoveHeartBeat(creator, "NpcCreatorModule::H_Refresh");
		}
		
		pKernel->AddCountBeat(creator, "NpcCreatorModule::H_Refresh", (int)min_time, 1);
	}

	return 1;
}

// 结束刷新心跳
int NpcCreatorModule::H_EndRefresh(IKernel* pKernel, const PERSISTID& creator, int slice)
{

	return 0;
    IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
    if (NULL == pCreatorObj)
    {
        return 0;
    }
	int i;

	// 删除存在的对象，但是如果有战斗对象在就不删除
    IRecord* objectRec = pCreatorObj->GetRecord(CREATE_OBJECT_LIST_REC);
    if (objectRec == NULL)
    {
        return 0;
    }
	int rows = objectRec->GetRows();

    LoopBeginCheck(check_k);
	for (i = rows - 1; i >= 0; i--)
	{
        LoopDoCheck(check_k);
        PERSISTID npc = objectRec->QueryObject(i, 0);

		if (!pKernel->Exists(npc))
		{
			continue;
		}
        IGameObj* pNpc = pKernel->GetGameObj(npc);
        if (pNpc == NULL)
        {
            continue;
        }
        // 只有非死亡NPC才置该标志
        int nDead = 0;

        if (pNpc->FindAttr("Dead"))
        {
            nDead = pNpc->QueryInt("Dead");
        }

        if (nDead <= 0)
        {
            PERSISTID creator_item = pNpc->QueryDataObject("CreatorItem");

            // 添加一个标记(onnpcdestroy回调内会做一些特殊的处理)
            pNpc->AddDataInt("CreatorEndRefresh", 0);

            // 删除NPC
            pKernel->DestroySelf(npc);
        }
	}
		
	// 还有NPC没有被删除
	if (objectRec->GetRows() > 0)
	{
		pKernel->SetBeatCount(creator, "NpcCreatorModule::H_EndRefresh", 1);
	}

	return 1;
}

// 开始刷新
void NpcCreatorModule::BeginRefresh(IKernel* pKernel, const PERSISTID& creator)
{
	if (!pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_Refresh"))
	{
		pKernel->AddCountBeat(creator, "NpcCreatorModule::H_Refresh",
										500 + ::util_random_int(6) * 100, 1);
	}

	if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_EndRefresh"))
	{
		pKernel->RemoveHeartBeat(creator, "NpcCreatorModule::H_EndRefresh");
	}
}

// 结束刷新
void NpcCreatorModule::EndRefresh(IKernel* pKernel, const PERSISTID& creator)
{
	if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_Refresh"))
	{
		pKernel->RemoveHeartBeat(creator, "NpcCreatorModule::H_Refresh");
	}

	if (!pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_EndRefresh"))
	{
		pKernel->AddCountBeat(creator, "NpcCreatorModule::H_EndRefresh", 60 * 1000, 1);
	}
}

bool NpcCreatorModule::CreateStaticNpc(IKernel* pKernel, const PERSISTID& scene, std::vector<CreateStaticNpcInfo>& staticNpc, int nSubSceneNo, int nGroupId)
{
	int curSceneID = pKernel->GetSceneId();
// 	int nSceneType = GroupSceneBase::GetSceneType(pKernel, curSceneID); 
// 	if (nGroupId == -1 && nSceneType != GROUP_SCENE_NOT_SECRET)
//     {
//         // 秘境场景不创建静态npc
//         return false;
//     }
	LoopBeginCheck(check_l);
	for (auto it : staticNpc)
	{
		LoopDoCheck(check_l);
		// 创建npc
		CreateStaticNpcUnit(pKernel, scene, it, nSubSceneNo, nGroupId);
	}

	return true;  
}

// 根据xml配置创建某个静态NPC
PERSISTID NpcCreatorModule::CreateStaticNpcUnit(IKernel* pKernel, const PERSISTID& scene, 
	CreateStaticNpcInfo&pItem, int nSubSceneNo, int nGroupId)
{
	IGameObj* pSceneObj = pKernel->GetGameObj(scene);
	if ( NULL == pSceneObj)
	{
		return PERSISTID();
	}

	// 场景阵营
	int nSceneNation = 0/*pSceneObj->QueryInt(FIELD_PROP_NATION)*/;
	// 服务端使用角度

	
	if (pItem.m_npcID.empty())
	{
		return PERSISTID();
	}
	int groupID = 0;
	if (nGroupId != -1)
	{
		groupID = nGroupId;
		
	}
	else
	{
		groupID = pItem.m_groupID;
	}

	PERSISTID npc_obj;

	// <!--Nation 1 江湖创建 2 朝廷创建 0都创建-->
	if (0 == pItem.m_nation || nSceneNation == pItem.m_nation)
	{	
		float y1 = GetCurFloorHeight(pKernel, pItem.m_position.m_x, pItem.m_position.m_y, pItem.m_position.m_z);
		// 带参数创建NPC
		npc_obj = pKernel->CreateObjectArgs("", pItem.m_npcID.c_str(), 0, pItem.m_position.m_x, y1, pItem.m_position.m_z, pItem.m_position.m_ay, CVarList() << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << groupID);
// 		if (pKernel->Exists(npc_obj))
// 		{
// 			pKernel->SetInt(npc_obj, "NoSpring", 0);
// 		}
	}

	return npc_obj;
}

// 创建随机NPC
bool NpcCreatorModule::CreateRandomNpc(IKernel* pKernel, const PERSISTID& scene,
	std::vector<CreateSpringNpcInfo>&npcCfg, int nSubSceneNo, int nGroupId)
{
	

    LoopBeginCheck(check_m);
	for (auto it : npcCfg)
	{
		LoopDoCheck(check_m);
		CreateARandomCreator(pKernel, scene, it, nSubSceneNo, nGroupId);

	}
	return true;
}

void NpcCreatorModule::CleanCfg()
{
	ClearXMLData();

	m_clone_npc_info.clear();

	m_group_npc_info.clear();

	m_refresh_npc_by_time.clear();

	m_wheel_npc.clear();

};


//获得目录下文件夹列表
void NpcCreatorModule::GetDirFileList(IKernel* pKernel,const fast_string& npcCfgPath, std::vector<xml_document<>*>& outFileList)
{

	fast_string creator_file = npcCfgPath;
	fast_string file_path = fast_string(pKernel->GetResourcePath()) + "ini\\Scene\\creator\\npc_creator\\";
	file_path = file_path + creator_file + "\\";
	fast_string find_file_path = file_path + "*.xml";
	_finddata_t findfile;
	int nFind = int(_findfirst(find_file_path.c_str(), &findfile));
	if (nFind == -1){
		return;
	}
	// 循环加载
	LoopBeginCheck(check_o);
	do
	{
		LoopDoCheck(check_o);
		// 实际文件名
		creator_file = file_path + findfile.name;

		xml_document<>* doc = new xml_document<>();
		try
		{
			char* pfilebuff = GetFileBuff(creator_file.c_str());
			if (NULL == pfilebuff){
				continue;
			}
			m_vecbuff.push_back(pfilebuff);
			doc->parse<rapidxml::parse_default>(pfilebuff);
		}
		catch (parse_error& e)
		{
			::extend_warning(pKernel, e.what());
			delete doc;
			return;
		}
		// 插入文件到vector
		outFileList.push_back(doc);

	} while (_findnext(nFind, &findfile) == 0);
	_findclose(nFind);
}

void NpcCreatorModule::PraseCloneNpc(IKernel* pKernel, int sceneID, const fast_string& cfgPath)
{
	// 是副本把XML存入map
	

	auto it = m_clone_npc_info.find(sceneID);
	if (it != m_clone_npc_info.end())
	{
		::extend_warning(pKernel, "[Error]NpcCreatorModule::LoadCloneNpcInfo: found same scene:");
		return;
	}

	// 获得ini配置文件路径
	std::vector<xml_document<>*> tempDocVec;
	GetDirFileList(pKernel, cfgPath, tempDocVec);
	if (tempDocVec.empty()){
		return;
	}
	// 插入map
	auto pib = m_clone_npc_info.insert(std::make_pair(sceneID, tempDocVec));
	if (!pib.second)
	{
		::extend_warning(pKernel, "[Error]LoadXmlConfigError: insert error:");
	}

}

void NpcCreatorModule::ParseGroupNpc(IKernel* pKernel, const std::string& npcCfgName)
{
	auto it = m_group_npc_info.find(npcCfgName);
	if (it != m_group_npc_info.end())
	{
		::extend_warning(pKernel, "[Error]NpcCreatorModule::LoadCloneNpcInfo: found same scene:");
		::extend_warning(pKernel, npcCfgName.c_str());
	}
	std::vector<xml_document<>*> tempDocVec;
	GetDirFileList(pKernel, npcCfgName.c_str(), tempDocVec);
	if (tempDocVec.empty()){
		return;
	}

	
	ParseSceneNpc(pKernel,npcCfgName, tempDocVec);
	//解析出按时间刷新的npc
	ParseCreateNpcByTime(npcCfgName, tempDocVec);
	ParseWheelNpc(npcCfgName, tempDocVec);
}


// 载入副本NPC信息
bool NpcCreatorModule::LoadNpcInfo(IKernel* pKernel)
{
	CleanCfg();

	// 获得路径
	fast_string pathname = pKernel->GetResourcePath();
	pathname += "scenes.ini";

	CUtilIni inifile(pathname.c_str());
	if (!inifile.LoadFromFile())
	{
		::extend_warning(pKernel, "[Error]NpcCreatorModule::LoadCloneNpcInfo: No found file:");
		::extend_warning(pKernel, pathname.c_str());
		return false;
	}

	int count = (int)inifile.GetSectionCount();
    LoopBeginCheck(check_n);
	for (int i = 0; i < count; i++)
	{
        LoopDoCheck(check_n);
		const char* pSectName = inifile.GetSectionByIndex(i);
		int nClonable = inifile.ReadInteger(i, "Clonable", 0);
		// 获得ini配置文件路径
		fast_string creator_file = inifile.ReadString(i, "Config", "");
		
		// 是副本把XML存入map
		int sceneid = ::atoi(pSectName);
		if (nClonable == 1)
		{
			PraseCloneNpc(pKernel, sceneid, creator_file);
		}


		size_t n_pos = creator_file.find_last_of('\\');
		fast_string npcBasePath = creator_file.substr(n_pos + 1, creator_file.length() - n_pos - 1);
		int nGrouping = inifile.ReadInteger(i, "Grouping", 0);
		if (nGrouping >= 1)
		{
			for (int i = 0; i < 20; i++)
			{
				std::string npcCfgName;
				if (i == 0)
				{
					npcCfgName = npcBasePath.c_str();
				}
				else
				{
					npcCfgName = npcBasePath.c_str() + std::string("_") + StringUtil::IntAsString(i);
				}
				ParseGroupNpc(pKernel, npcCfgName);
			}
		}
		
	}

	return true;
}

// 加载任务刷怪器配置
bool NpcCreatorModule::LoadTaskCreatorRes(IKernel* pKernel)
{
	m_TaskCreatorMap.clear();

	// 任务Npc配置
	std::string file_path = pKernel->GetResourcePath();
	file_path += TASK_CREATOR;
	try
	{
		char* pfilebuff = GetFileBuff(file_path.c_str());

		if (StringUtil::CharIsNull(pfilebuff))
		{
			std::string err_msg = file_path;
			err_msg.append(" does not exists.");
			::extend_warning(LOG_ERROR, err_msg.c_str());
			return false;
		}

		xml_document<> doc;

		doc.parse<rapidxml::parse_default>(pfilebuff);

		xml_node<>* pNodeRoot = doc.first_node("object");

		if (pNodeRoot == NULL)
		{
			std::string err_msg = file_path;
			err_msg.append(" invalid npc creator file.");
			::extend_warning(LOG_ERROR, err_msg.c_str());
			return false;
		}

		xml_node<>* pNode = pNodeRoot->first_node("randomcreator");
		if (pNode == NULL)
		{
			std::string err_msg = file_path;
			err_msg.append(" invalid npc creator file.");
			::extend_warning(LOG_ERROR, err_msg.c_str());
			return false;
		}

		xml_node<>* pCreator = pNode->first_node("creator");
		// 从当前pCreator处开始创建
		LoopBeginCheck(a);
		while (pCreator != NULL)
		{
			CreateSpringNpcInfo createSpringNpcInfo;

			LoopDoCheck(a);
			// 初始化分组号和分组id
			int nSubSceneNo = -1;
			int nGroupId = -1;

			// 坐标
			createSpringNpcInfo.m_springPosition.m_x = convert_float(QueryXmlAttr(pCreator, "x"));
			createSpringNpcInfo.m_springPosition.m_y = convert_float(QueryXmlAttr(pCreator, "y"));
			createSpringNpcInfo.m_springPosition.m_z = convert_float(QueryXmlAttr(pCreator, "z"));

			// 外环
			createSpringNpcInfo.m_maxScope = convert_float(QueryXmlAttr(pCreator, "MaxScope"), 30);
			//内环
			createSpringNpcInfo.m_minScope = convert_float(QueryXmlAttr(pCreator, "MinScope"), 25);
			// 触发半径
			createSpringNpcInfo.m_springRange = convert_float(QueryXmlAttr(pCreator, "SpringRange"), 50.0f);
			//0-触发后创建NPC，1-进入场景就创建
			createSpringNpcInfo.m_createType = convert_int(QueryXmlAttr(pCreator, "createtype"), 0);

			//怪物的波数
			createSpringNpcInfo.m_totalStep = convert_int(QueryXmlAttr(pCreator, "totalstep"), 0);
			//NPC是否销毁 0-是 1-否
			createSpringNpcInfo.m_isDestroy = convert_int(QueryXmlAttr(pCreator, "IsDestroy", 0)) > 0 ? true : false;
			//关卡秘境行进步骤
			createSpringNpcInfo.m_sceneAction = convert_int(QueryXmlAttr(pCreator, "SceneAction", 0));

			// 生成器编号
			const char *creator_no = QueryXmlAttr(pCreator, "No", "");
			createSpringNpcInfo.m_no = creator_no;

			int totalNpcNum = 0;
			createSpringNpcInfo.m_totalNpcNum = totalNpcNum;
			createSpringNpcInfo.m_totalNpcNum = ParseRandomNpcItem(pKernel, createSpringNpcInfo.m_NpcGroup, pCreator);

			// 组装
			CVarList result;
			util_split_string(result, creator_no, "_");
			if (result.GetCount() > 1)
			{
				const int key = result.IntVal(1);
				m_TaskCreatorMap.insert(std::make_pair(key, createSpringNpcInfo));
			}

			// 得到下一个创建器信息
			pCreator = pCreator->next_sibling("creator");			
		}
		
		delete[] pfilebuff;

	}
	catch (parse_error& e)
	{
		::extend_warning(pKernel, "[Error]NpcCreatorModule::LoadTaskCreatorRes: ");
		::extend_warning(pKernel, file_path.c_str());
		::extend_warning(pKernel, e.what());

		return false;
	}

	return true;
}

// 根据XML创建副本NPC
bool NpcCreatorModule::CreateCloneNpcFromXml(IKernel* pKernel, const PERSISTID& scene)
{
	if (pKernel->GetSceneClass() != 2)
	{
		//不是副本
		return false;
	}

	// 开启静态npc创建心跳
	if (pKernel->FindHeartBeat(scene, "NpcCreatorModule::H_DelayCreateStaticNpc"))
	{
		pKernel->RemoveHeartBeat(scene, "NpcCreatorModule::H_DelayCreateStaticNpc");
	}

	pKernel->AddCountBeat(scene,"NpcCreatorModule::H_DelayCreateStaticNpc", 300, 5);

	// 开始随机创建器创建心跳
	if (pKernel->FindHeartBeat(scene, "NpcCreatorModule::H_DelayCreateRandomNpc"))
	{
		pKernel->RemoveHeartBeat(scene, "NpcCreatorModule::H_DelayCreateRandomNpc");
	}

	pKernel->AddCountBeat(scene,"NpcCreatorModule::H_DelayCreateRandomNpc", 300,5);


	return true;
}

// 为分组场景创建NPC
bool NpcCreatorModule::CreateGroupNpcFromXml(IKernel* pKernel, const PERSISTID& scene,
	int nSubSceneNo, int nGroupId, const char* npcCfgName)
{
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
    if (NULL == pSceneObj)
    {
        return false;
    }
	
	
	AddSceneGroupNpcCfgName(nSubSceneNo, nGroupId, npcCfgName);
	IRecord* staticRec = pSceneObj->GetRecord(GROUP_STATIC_CREATE_FLOW);
    if (staticRec != NULL)
    {
		staticRec->AddRowValue(-1, CVarList() << nSubSceneNo << nGroupId << 0 << 0 );
    }
    IRecord* randomRec = pSceneObj->GetRecord(GROUP_RANDOM_CREATE_FLOW);
    if (randomRec != NULL)
    {
		randomRec->AddRowValue(-1, CVarList() << nSubSceneNo << nGroupId << 0 << 0 );
    }
	
    //静态NPC创建器心跳
    if (pKernel->FindHeartBeat(scene, "NpcCreatorModule::H_GroupSceneCreateStaticNpc"))
    {
        pKernel->RemoveHeartBeat(scene, "NpcCreatorModule::H_GroupSceneCreateStaticNpc");
    }
    pKernel->AddCountBeat(scene,"NpcCreatorModule::H_GroupSceneCreateStaticNpc", 300, 5);
	

	// 开始随机创建器创建心跳
	if (pKernel->FindHeartBeat(scene, "NpcCreatorModule::H_GroupSceneCreateRandomNpc"))
	{
		pKernel->RemoveHeartBeat(scene, "NpcCreatorModule::H_GroupSceneCreateRandomNpc");
	}
	pKernel->AddCountBeat(scene,"NpcCreatorModule::H_GroupSceneCreateRandomNpc", 300, 5);

	CreateRandomNpcByTime(pKernel, scene, nSubSceneNo, nGroupId);

	CreateWheelNpc(pKernel, nSubSceneNo, nGroupId);

	return true;
}


void NpcCreatorModule::AddSceneGroupNpcCfgName(int sceneId, int groupId, const std::string& npcCfgName)
{
	auto it = m_sceneGroupNpcCfgName.find(sceneId);
	if (it != m_sceneGroupNpcCfgName.end())
	{
		auto its = it->second.find(groupId);
		if (its != it->second.end())
		{
			it->second.erase(its);
		}
		it->second[groupId] = npcCfgName;
	}
	else
	{
		m_sceneGroupNpcCfgName[sceneId][groupId] = npcCfgName;
	}

	
};


const char* NpcCreatorModule::GetSceneGroupNpcCfgName(int sceneID, int groupID)
{
	auto it = m_sceneGroupNpcCfgName.find(sceneID);
	if (it == m_sceneGroupNpcCfgName.end()){
		return "";
	}
	auto its = it->second.find(groupID);
	if (its == it->second.end()){
		return "";
	}
	return its->second.c_str();

}
void NpcCreatorModule::CreateWheelNpc(IKernel*pKernel, int sceneId, int groupId, int killNumNow)
{
	const char* npcCfgName = GetSceneGroupNpcCfgName(sceneId, groupId);
	auto it = m_wheel_npc.find(npcCfgName);
	if (it != m_wheel_npc.end())
	{
		auto its = it->second.find(killNumNow);
		if (its != it->second.end())
		{
			std::vector<WheelNpc>::iterator itt = its->second.begin();
			LoopBeginCheck(a);
			for (; itt != its->second.end(); ++itt)
			{
				LoopDoCheck(a);
				float x = itt->_x;
				float y = itt->_y;
				float z = itt->_z;
				int amount = itt->_mount;
				int needKillNum = itt->_needKillNum;
				// 得到实际高度
				y = GetCurFloorHeight(pKernel, x, y, z);
				
				LoopBeginCheck(b);
				for (int i = 0; i < amount; i++)
				{
					LoopDoCheck(b);
					// 带参数创建NPC
					PERSISTID npc_obj = pKernel->CreateObjectArgs("", itt->_id.c_str(), 0, x, y, z, itt->_ay, CVarList() << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << groupId);
					if (!pKernel->Exists(npc_obj))
					{
						continue;
					}

					if (needKillNum != 0)
					{
						IGameObj *pNpc = pKernel->GetGameObj(npc_obj);
						if (pNpc != NULL)
						{
							pNpc->AddDataInt("NeedKillNum", needKillNum);
							FightInterfaceInstance->AddBuffer(pKernel, npc_obj, npc_obj, CONTROL_BUFFID);
						}

					}
				}
			}
		}
	}
}






// 建立creator和其id的对应关系
bool NpcCreatorModule::RegisterRandomCreator(IKernel* pKernel, const PERSISTID& scene, 
											 const char* id, const PERSISTID& creator)
{
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
    if (NULL == pSceneObj)
    {
        return false;
    }
	if (!id || !pKernel->Exists(scene) || !pKernel->Exists(creator))
	{
		return false;
	}
    IRecord* creatorRec = pSceneObj->GetRecord("creator_id_rec");
    if (creatorRec == NULL)
    {
        return false;
    }
	int row = creatorRec->FindString(0, id);
	if (row >= 0)
	{
		return true;
	}

    return creatorRec->AddRowValue(-1, CVarList() << id << creator) >= 0;
}

void NpcCreatorModule::ParseSceneNpc(IKernel* pKernel, std::string sceneCfgName, const std::vector<xml_document<>*>& tempDocVec)
{
	std::vector<CreateSceneNpcInfo> m_crateFileList;
	int iDocCount = (int)tempDocVec.size();
	LoopBeginCheck(a);
	for (int fileIndex = 0; fileIndex < iDocCount; fileIndex++)
	{
		LoopDoCheck(a);
		xml_node<>* pNodeRoot = tempDocVec[fileIndex]->first_node("object");
		if (pNodeRoot == NULL)
			continue;
		CreateSceneNpcInfo CreateSceneNpcInfo;
		// 创建随机对象生成器
		

		ParseRandomNpcCreate(pKernel, CreateSceneNpcInfo.m_createRandGroup, pNodeRoot);
		ParseStaticNpcCreate(pKernel, CreateSceneNpcInfo.m_createStaticGroup, pNodeRoot);


		m_crateFileList.push_back(CreateSceneNpcInfo);
	}

	m_group_npc_info[sceneCfgName] = m_crateFileList;


}

void NpcCreatorModule::ParseRandomNpcCreate(IKernel* pKernel, std::vector<CreateSpringNpcInfo>& springNpcGroup, xml_node<>*pNodeRoot)
{
	if (pNodeRoot == NULL){
		return;
	}

	xml_node<>* pNode = pNodeRoot->first_node("randomcreator");
	if (pNode == NULL){
		return;
	}
	
	 xml_node<>* pCreator = pNode->first_node("creator");
	// 从当前pCreator处开始创建
	LoopBeginCheck(check_z);
	while (pCreator != NULL)
	{
		CreateSpringNpcInfo createSpringNpcInfo;

		LoopDoCheck(check_z);
		// 初始化分组号和分组id
		int nSubSceneNo = -1;
		int nGroupId = -1;

		// 坐标
		float x = convert_float(QueryXmlAttr(pCreator, "x"));
		float y = convert_float(QueryXmlAttr(pCreator, "y"));
		float z = convert_float(QueryXmlAttr(pCreator, "z"));

		// 得到实际高度
		//y = GetCurFloorHeight(pKernel, x, y, z);
		createSpringNpcInfo.m_springPosition.m_x = x;
		createSpringNpcInfo.m_springPosition.m_y = y;
		createSpringNpcInfo.m_springPosition.m_z = z;
		// 外环
		createSpringNpcInfo.m_maxScope = convert_float(QueryXmlAttr(pCreator, "MaxScope"), 30);
		//内环
		createSpringNpcInfo.m_minScope = convert_float(QueryXmlAttr(pCreator, "MinScope"), 25);
		// 触发半径
		createSpringNpcInfo.m_springRange = convert_float(QueryXmlAttr(pCreator, "SpringRange"), 50.0f);
		//0-触发后创建NPC，1-进入场景就创建
		createSpringNpcInfo.m_createType = convert_int(QueryXmlAttr(pCreator, "createtype"), 0);
		//怪物的波数
		createSpringNpcInfo.m_totalStep = convert_int(QueryXmlAttr(pCreator, "totalstep"), 0);

		//NPC是否销毁 0-是 1-否
		createSpringNpcInfo.m_isDestroy =convert_int(QueryXmlAttr(pCreator, "IsDestroy", 0)) > 0 ?true:false;
		//关卡秘境行进步骤
		createSpringNpcInfo.m_sceneAction = convert_int(QueryXmlAttr(pCreator, "SceneAction", 0));
		// 生成器编号
		createSpringNpcInfo.m_no = QueryXmlAttr(pCreator, "No");
		int totalNpcNum = 0;
		createSpringNpcInfo.m_totalNpcNum = totalNpcNum;
		createSpringNpcInfo.m_totalNpcNum =  ParseRandomNpcItem(pKernel, createSpringNpcInfo.m_NpcGroup, pCreator);
		// 得到下一个创建器信息
		pCreator = pCreator->next_sibling("creator");
		springNpcGroup.push_back(createSpringNpcInfo);
	}
	
}


void NpcCreatorModule::ParseStaticNpcCreate(IKernel* pKernel, std::vector<CreateStaticNpcInfo>& staticNpcGroup, xml_node<>*pCreator)
{
	if (pCreator == NULL)
	{
		return;
	}
	
	// 创建静态对象生成器
	xml_node<>* pNode = pCreator->first_node("staticcreator");

	if (NULL != pNode)
	{
		CreateStaticNpcInfo staticNpcInfo;
		xml_node<>* pItem = pNode->first_node("item");

		LoopBeginCheck(check_dd);
		while (pItem != NULL)
		{
			LoopDoCheck(check_dd);
			// NPC编号
			staticNpcInfo.m_npcID = QueryXmlAttr(pItem, "id");
			if (staticNpcInfo.m_npcID.empty()){
				continue;
			}
			NpcPosition positoin;
				// 坐标
			staticNpcInfo.m_position.m_x = convert_float(QueryXmlAttr(pItem, "x"));
			staticNpcInfo.m_position.m_y = convert_float(QueryXmlAttr(pItem, "y"));
			staticNpcInfo.m_position.m_z = convert_float(QueryXmlAttr(pItem, "z"));
			// 模型旋转
			const char* ay = QueryXmlAttr(pItem, "ay", "0.0");
			// 服务端使用角度
			staticNpcInfo.m_position.m_ay = (float)::atof(ay);
			staticNpcInfo.m_groupID = StringUtil::StringAsInt(QueryXmlAttr(pItem, "Group"), -1);


			// 阵营
			staticNpcInfo.m_nation = StringUtil::StringAsInt(QueryXmlAttr(pItem, "Nation"));
			staticNpcGroup.push_back(staticNpcInfo);
			// 得到下一个npc创建信息
			pItem = pItem->next_sibling("item");

		}
	}

}

int NpcCreatorModule::ParseRandomNpcItem(IKernel* pKernel, std::vector<RandomCreator>& createSpringNpcInfoGroup, xml_node<>*pCreator)
{
	if (pCreator == NULL){
		return 0;
	}
	int totalNpcNum = 0;
	xml_node<>* pItem = pCreator->first_node("item");
	LoopBeginCheck(check_q);
	while (pItem != NULL)
	{
		LoopDoCheck(check_q);
		RandomCreator npcGroup;
		// NPC编号
		npcGroup.m_npcID = QueryXmlAttr(pItem, "id");
		// 最大生成的数量
		npcGroup.m_createNum = convert_int(QueryXmlAttr(pItem, "amount"));
		totalNpcNum = totalNpcNum + npcGroup.m_createNum;
		// 刷新时间
		npcGroup.m_refreshTime = convert_int(QueryXmlAttr(pItem, "refreshtime"));
		npcGroup.m_step = convert_int(QueryXmlAttr(pItem, "step"));
		npcGroup.m_limit = convert_int(QueryXmlAttr(pItem, "limit"));
		
		// 位置表数据读取
		ParseRandomNpcPosition(pKernel, npcGroup.m_positionGroup, pItem);
		pItem = pItem->next_sibling("item");
		createSpringNpcInfoGroup.push_back(npcGroup);
	}
	return totalNpcNum;
}

void NpcCreatorModule::ParseRandomNpcPosition(IKernel* pKernel, std::vector<NpcPosition> & positionGroup, xml_node<>*pItem)
{
	if (pItem == NULL){
		return;
	}
	xml_node<>* pPosition = pItem->first_node("position");
	if (pPosition != NULL)
	{
		NpcPosition position;
		// 使用固定位置信息
		LoopBeginCheck(check_r);
		while (pPosition != NULL)
		{
			LoopDoCheck(check_r);
			float x1, y1, z1;

			// 位置
			x1 = convert_float(QueryXmlAttr(pPosition, "x"));
			z1 = convert_float(QueryXmlAttr(pPosition, "z"));
			y1 = convert_float(QueryXmlAttr(pPosition, "y"));
			// 取得最近的层高度
			//y1 = GetCurFloorHeight(pKernel, x1, y1, z1);

			float o1 = convert_float(QueryXmlAttr(pPosition, "ay"));
			position.m_x = x1;
			position.m_y = y1;
			position.m_z = z1;
			position.m_ay = o1;
			pPosition = pPosition->next_sibling("position");
			positionGroup.push_back(position);
		}
	}
}

bool NpcCreatorModule::CreateARandomCreator(IKernel* pKernel, const PERSISTID& scene, CreateSpringNpcInfo&pCreate, int nSubSceneNo, int nGroupId)
{
	
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
    if (pSceneObj == NULL)
    {
        return false;
    }
	
	if (pSceneObj->QueryInt("Grouping") >= 1)
	{
		int sceneID = pKernel->GetSceneId();

// 		int sceneType = GroupSceneBase::GetSceneType(pKernel, sceneID);
// 		if (nGroupId == -1)
// 		{
// 			
//             if ((sceneType != GROUP_SCENE_NOT_SECRET && sceneType != GROUP_SCENE_SECRET_PET) ||
// 				(sceneID >= GUIDE_SCENEID_MIN && sceneID <= GUIDE_SCENEID_MAX))
// 			{
// 				//秘境副本场景，新手引导场景不在此创建NPC创建器
// 				return true;
// 			}
// 		}
	}

	// 创建一个生成器
	PERSISTID creator = pKernel->CreateTo(scene, "RandomNpcCreator", pCreate.m_springPosition.m_x, pCreate.m_springPosition.m_y, pCreate.m_springPosition.m_z, 0.0f);
    IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
    if (pCreatorObj == NULL){
        return false;
    }
	if (pKernel->Exists(creator))
	{
		// 注册生成器
		if (!pCreate.m_no.empty())
		{
			RegisterRandomCreator(pKernel, scene, pCreate.m_no.c_str(), creator);
		}

		// 设置属性
		pCreatorObj->SetFloat("MaxScope", pCreate.m_maxScope);
		pCreatorObj->SetFloat("MinScope", pCreate.m_minScope);
		pCreatorObj->SetFloat("SpringRange", pCreate.m_springRange);

		pCreatorObj->SetInt("NoSpring", 0);
		pCreatorObj->SetInt("Springer", 0);

		pCreatorObj->SetInt("Invisible", 1);
		pCreatorObj->SetInt("CreateType", pCreate.m_createType);
		pCreatorObj->SetInt("TotalStep", pCreate.m_totalStep);
		pCreatorObj->SetInt("IsDestroy", pCreate.m_isDestroy);
		pCreatorObj->SetInt("SceneAction", pCreate.m_sceneAction);
		if (pCreate.m_totalStep > 1)
		{
			pCreatorObj->SetInt("CurStep", 1);
		}
		//创建器上的怪物总数
		int create_amount = 0;

		// 需要随机生成的NPC位置总数量
		int pos_count = 0;

		for (auto it : pCreate.m_NpcGroup)
		{
			PERSISTID creator_item = pKernel->Create(creator, "RandomNpcCreatorItem");
			if (!pKernel->Exists(creator_item)){
				continue;
			}

			IGameObj* pItemObj = pKernel->GetGameObj(creator_item);
			if (pItemObj == NULL){
				continue;
			}

			IRecord* positionRec = pItemObj->GetRecord(POSITION_INFO_REC);
			if (positionRec == NULL){
				continue;
			}

			pItemObj->SetString("NpcID", it.m_npcID.c_str());
			pItemObj->SetInt("MaxAmount", it.m_createNum);
			pItemObj->SetInt("RefreshPeriod",it.m_refreshTime);
			pItemObj->SetInt("Step", it.m_step);
			pItemObj->SetInt("Limit", it.m_limit);
			pItemObj->SetInt("GroupID", nGroupId);

			
			if (!it.m_positionGroup.empty())
			{
				// 使用固定位置信息
				for (auto position : it.m_positionGroup){
					 
					float y1 = GetCurFloorHeight(pKernel, position.m_x, position.m_y, position.m_z);
					positionRec->AddRowValue(-1, CVarList() << position.m_x << y1 << position.m_z << position.m_ay << 0 << (__int64)0 << -1.0f);
				}
			}
			else
			{
				if (it.m_createNum > 0 && pCreate.m_createType == 0)
				{
					LoopBeginCheck(check_s);
					for (int j = 0; j < it.m_createNum; j++)
					{
						LoopDoCheck(check_s);
						float x1, y1, z1, o1;
						float orient = util_random_float(PI2);
						// 随机取周围的点,防止死循环
						LoopBeginCheck(check_t);
						for (int k = 0; k < 8; ++k)
						{
							LoopDoCheck(check_t);
							x1 = pCreate.m_springPosition.m_x +(pCreate.m_maxScope + util_random_float(pCreate.m_maxScope - pCreate.m_minScope)) * ::sinf(orient);
							z1 = pCreate.m_springPosition.m_z +(pCreate.m_maxScope + util_random_float(pCreate.m_maxScope - pCreate.m_minScope)) * ::cosf(orient);
							y1 = pKernel->GetMapHeight(x1, z1);
							if (pKernel->CanWalk(x1, z1))
							{
								break;
							}
						}
						//如果最后随机出来的点还是存在问题
						if (!pKernel->CanWalk(x1, z1))
						{
							x1 = pCreate.m_springPosition.m_x;
							z1 = pCreate.m_springPosition.m_z;
						}

						o1 = PI2 * ::util_random_float(1.0f);
						// 添加到位置表格
						positionRec->AddRowValue(-1,
							CVarList() << x1 << y1 << z1 << o1 << 0 << (__int64)0 << -1.0f);
					}
				}
			}
		
		}

		pCreatorObj->SetInt("MonsterAmount", pCreate.m_totalNpcNum);

		//如果是大于0就先进行随机设定了
		if (pCreate.m_createType > 0)
		{
			PreSetCreateCenterPos(pKernel, creator);
		}

		// 是分组场景的话设置组号
		if (pSceneObj->QueryInt("Grouping") >= 1)
		{
			pCreatorObj->SetInt("GroupID", nGroupId);
		}

		if (pCreate.m_createType > 0)
		{
			BeginRefresh(pKernel, creator);
		}
	}

	return true;
}

// 创建任务刷怪器
const PERSISTID NpcCreatorModule::CreateTaskrCreator(IKernel* pKernel, const PERSISTID& scene,
	const int task_id, const int group_id, const int level)
{
	IGameObj* pSceneObj = pKernel->GetGameObj(scene);
	if (pSceneObj == NULL)
	{
		return PERSISTID();
	}

	// 查找刷怪器
	std::map<int, CreateSpringNpcInfo>::const_iterator find_it = m_TaskCreatorMap.find(task_id);
	if (find_it == m_TaskCreatorMap.end())
	{
		return PERSISTID();
	}

	const CreateSpringNpcInfo &pCreate = find_it->second;
	
	// 创建一个生成器
	PERSISTID creator = pKernel->CreateTo(scene, "RandomNpcCreator", pCreate.m_springPosition.m_x, pCreate.m_springPosition.m_y, pCreate.m_springPosition.m_z, 0.0f);
	IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
	if (pCreatorObj == NULL)
	{
		return PERSISTID();
	}
	
	// 注册生成器
	if (!pCreate.m_no.empty())
	{
		RegisterRandomCreator(pKernel, scene, pCreate.m_no.c_str(), creator);
	}

	// 临时属性
	ADD_DATA_INT(pCreatorObj, "CreatorLevel");
	pCreatorObj->SetDataInt("CreatorLevel", level);

	// 设置属性
	pCreatorObj->SetFloat("MaxScope", pCreate.m_maxScope);
	pCreatorObj->SetFloat("MinScope", pCreate.m_minScope);
	pCreatorObj->SetFloat("SpringRange", pCreate.m_springRange);

	pCreatorObj->SetInt("NoSpring", 0);
	pCreatorObj->SetInt("Springer", 0);

	pCreatorObj->SetInt("Invisible", 1);
	pCreatorObj->SetInt("CreateType", pCreate.m_createType);
	pCreatorObj->SetInt("TotalStep", pCreate.m_totalStep);
	pCreatorObj->SetInt("IsDestroy", pCreate.m_isDestroy);
	pCreatorObj->SetInt("SceneAction", pCreate.m_sceneAction);
	if (pCreate.m_totalStep > 1)
	{
		pCreatorObj->SetInt("CurStep", 1);
	}
	//创建器上的怪物总数
	int create_amount = 0;

	// 需要随机生成的NPC位置总数量
	int pos_count = 0;

	for (auto it : pCreate.m_NpcGroup)
	{
		PERSISTID creator_item = pKernel->Create(creator, "RandomNpcCreatorItem");
		if (!pKernel->Exists(creator_item))
		{
			continue;
		}

		IGameObj* pItemObj = pKernel->GetGameObj(creator_item);
		if (pItemObj == NULL)
		{
			continue;
		}

		IRecord* positionRec = pItemObj->GetRecord(POSITION_INFO_REC);
		if (positionRec == NULL)
		{
			continue;
		}

		pItemObj->SetString("NpcID", it.m_npcID.c_str());
		pItemObj->SetInt("MaxAmount", it.m_createNum);
		pItemObj->SetInt("RefreshPeriod", it.m_refreshTime);
		pItemObj->SetInt("Step", it.m_step);
		pItemObj->SetInt("Limit", it.m_limit);
		pItemObj->SetInt("GroupID", group_id);


		if (!it.m_positionGroup.empty())
		{
			// 使用固定位置信息
			for (auto position : it.m_positionGroup)
			{

				float y1 = GetCurFloorHeight(pKernel, position.m_x, position.m_y, position.m_z);
				positionRec->AddRowValue(-1, CVarList() << position.m_x << y1 << position.m_z << position.m_ay << 0 << (__int64)0 << -1.0f);
			}
		}
		else
		{
			if (it.m_createNum > 0 && pCreate.m_createType == 0)
			{
				LoopBeginCheck(check_s);
				for (int j = 0; j < it.m_createNum; j++)
				{
					LoopDoCheck(check_s);
					float x1, y1, z1, o1;
					float orient = util_random_float(PI2);
					// 随机取周围的点,防止死循环
					LoopBeginCheck(check_t);
					for (int k = 0; k < 8; ++k)
					{
						LoopDoCheck(check_t);
						x1 = pCreate.m_springPosition.m_x + (pCreate.m_maxScope + util_random_float(pCreate.m_maxScope - pCreate.m_minScope)) * ::sinf(orient);
						z1 = pCreate.m_springPosition.m_z + (pCreate.m_maxScope + util_random_float(pCreate.m_maxScope - pCreate.m_minScope)) * ::cosf(orient);
						y1 = pKernel->GetMapHeight(x1, z1);
						if (pKernel->CanWalk(x1, z1))
						{
							break;
						}
					}
					//如果最后随机出来的点还是存在问题
					if (!pKernel->CanWalk(x1, z1))
					{
						x1 = pCreate.m_springPosition.m_x;
						z1 = pCreate.m_springPosition.m_z;
					}

					o1 = PI2 * ::util_random_float(1.0f);
					// 添加到位置表格
					positionRec->AddRowValue(-1,
						CVarList() << x1 << y1 << z1 << o1 << 0 << (__int64)0 << -1.0f);
				}
			}
		}

	}

	pCreatorObj->SetInt("MonsterAmount", pCreate.m_totalNpcNum);

	//如果是大于0就先进行随机设定了
	if (pCreate.m_createType > 0)
	{
		PreSetCreateCenterPos(pKernel, creator);
	}

	// 是分组场景的话设置组号
	if (pSceneObj->QueryInt("Grouping") >= 1)
	{
		pCreatorObj->SetInt("GroupID", group_id);
	}

	if (pCreate.m_createType > 0)
	{
		BeginRefresh(pKernel, creator);
	}

	return creator;
}

// 分步创建静态npc的心跳
int NpcCreatorModule::H_DelayCreateStaticNpc(IKernel* pKernel, const PERSISTID& creator, int slice)
{

	//// 记录函数开始时间
	//unsigned long begin_time = ::util_get_ticks();

	//// 得到心跳宿主对象
	//PERSISTID scene = creator;

	//// 保护
	//if (!pKernel->Exists(scene))
	//{
	//	return 0;
	//}
 //   IGameObj* pSceneObj = pKernel->GetGameObj(scene);
 //   if (pSceneObj == NULL)
 //   {
 //       return 0;
 //   }

	//int iCloneLevel = 0;

	//if (pSceneObj->FindAttr("Level"))
	//{
	//	iCloneLevel = pSceneObj->QueryInt("Level");
	//}

	//std::vector<xml_document<>*>* cfgXml = NULL;

	//if (pKernel->GetSceneClass() == 2)
	//{
 //       //副本场景，使用原型场景的配置信息
	//	// 场景id
	//	int nCloneSceneID = pKernel->GetSceneId();

	//	// 副本原型id
	//	int nPrototypeSceneID = pKernel->GetPrototypeSceneId(nCloneSceneID);
	//
	//	// 得到npc创建配置文件
	//	auto it = m_pNpcCreatorModule->m_clone_npc_info.find(nPrototypeSceneID);

	//	if (it == m_pNpcCreatorModule->m_clone_npc_info.end())
	//	{
	//		return 0;
	//	}
	//	cfgXml = &(it->second);
	//}
	//else
	//{
	//	// 场景id
	//	int nSceneID = pKernel->GetSceneId();
	//	std::string npcCfgName = StringUtil::IntAsString(nSceneID);
	//	// 直接通过分组场景id得到npc配置文件
	//	auto it = m_pNpcCreatorModule->m_group_npc_info.find(npcCfgName);

	//	if (it == m_pNpcCreatorModule->m_group_npc_info.end())
	//	{
	//		return 0;
	//	}
	//	cfgXml = &(it->second);
	//}
	//
	//if (cfgXml == NULL){
	//	return 0;
	//}

	//std::vector<xml_document<>*>& DocVec = *cfgXml;

	//int nCount = int(DocVec.size());

	//int i = 0;

	//// 得到当前正在创建的文件index，如果没有，则认为从0开始
	//if (pSceneObj->FindData("create_static_npc_file_count"))
	//{
	//	i = pSceneObj->QueryDataInt("create_static_npc_file_count");
	//}

	//// 遍历文件
 //   LoopBeginCheck(check_u);
	//for (;i < nCount;)
	//{
 //       LoopDoCheck(check_u);
	//	xml_node<>* pNodeRoot = DocVec[i]->first_node("object");

	//	if (pNodeRoot == NULL)
	//	{
	//		return 0;
	//	}

	//	// 创建静态对象生成器
	//	xml_node<>* pNode = pNodeRoot->first_node("staticcreator");

	//	if (NULL != pNode)
	//	{
	//		xml_node<>*  pItem =  NULL;

	//		int static_npc_index = 0;

	//		if (pSceneObj->FindData("create_static_npc_item_index") )
	//		{	
	//			// 得到当前正在创建的npc的index
	//			static_npc_index = pSceneObj->QueryDataInt("create_static_npc_item_index");

 //               pNode = pNode->first_node("item");
 //               LoopBeginCheck(check_v);
 //               for(int j = 0; j < static_npc_index; ++j)
	//			{
 //                   LoopDoCheck(check_v);
	//				pNode = pNode->next_sibling("item");
	//			}

	//			pItem = pNode;

	//		}
	//		else
	//		{
	//			pItem = pNode->first_node("item");
	//		}

 //           LoopBeginCheck(check_w);
	//		while (pItem != NULL)
	//		{
 //               LoopDoCheck(check_w);
	//			// 初始化分组号和分组id
	//			int nSubSceneNo = -1;
	//			int nGroupId = -1;

	//			if (pSceneObj->FindData("Create_SubSceneNo"))
	//			{
	//				nSubSceneNo = pSceneObj->QueryDataInt("Create_SubSceneNo");
	//			}

	//			if (pSceneObj->FindData("Create_GroupId"))
	//			{
	//				nGroupId = pSceneObj->QueryDataInt("Create_GroupId");
	//			}

	//			// 创建npc
	//			m_pNpcCreatorModule->CreateStaticNpcUnit(pKernel, scene, pItem, nSubSceneNo, nGroupId);

	//			// 得到下一个npc创建信息
	//			pItem = pItem->next_sibling("item");

	//			// npc创建序号递增
	//			static_npc_index++;

	//			// 得到当前系统时间
	//			unsigned long cur_time = ::util_get_ticks();

	//			// 当前函数执行时间超过最大时间，本次心跳结束
	//			if ((cur_time - begin_time) > MAX_HERAT_BEAT_TIME)
	//			{
	//				// 重新设置当前心跳的次数
	//				pKernel->SetBeatCount(scene,"NpcCreatorModule::H_DelayCreateStaticNpc", 5);

	//				// 保存正在创建的npc file编号
	//				if (!pSceneObj->FindData("create_static_npc_file_count"))
	//				{
	//					pSceneObj->AddDataInt("create_static_npc_file_count", 0);
	//				}

	//				pSceneObj->SetDataInt("create_static_npc_file_count", i);

	//				// 保存正在创建的npc item的index
	//				if (!pSceneObj->FindData("create_static_npc_item_index"))
	//				{
	//					pSceneObj->AddDataInt("create_static_npc_item_index", 0);
	//				}

	//				pSceneObj->SetDataInt("create_static_npc_item_index", static_npc_index);	

	//				return 1;
	//			}
	//		}
	//	} 

	//	// 文件No号增加
	//	i++;

	//	// 一个xml中的静态npc配置已经创建完毕，清除npc索引
	//	pSceneObj->RemoveData("create_static_npc_item_index");

	//	// 保存正在创建的npc file编号
	//	if (!pSceneObj->FindData("create_static_npc_file_count"))
	//	{
	//		pSceneObj->AddDataInt("create_static_npc_file_count", 0);
	//	}
	//	pSceneObj->SetDataInt("create_static_npc_file_count", i);
	//}
	//	
	return 1;
}

// 分步创建随机npc的心跳
int NpcCreatorModule::H_DelayCreateRandomNpc(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	//// 记录函数开始时间
	//unsigned long begin_time = ::util_get_ticks();

	//// 得到心跳宿主对象
	//PERSISTID scene = creator;

	//// 保护
	//if (!pKernel->Exists(scene))
	//{
	//	return 0;
	//}
 //   IGameObj* pSceneObj = pKernel->GetGameObj(scene);
 //   if (pSceneObj == NULL)
 //   {
 //       return 0;
 //   }

	//std::vector<xml_document<>*> *cfgXml = NULL;

	//if (pKernel->GetSceneClass() == 2)
	//{
	//	// 副本场景处理

	//	int nCloneSceneID = pKernel->GetSceneId();

	//	int nPrototypeSceneID = pKernel->GetPrototypeSceneId(nCloneSceneID);

	//	auto it = m_pNpcCreatorModule->m_clone_npc_info.find(nPrototypeSceneID);

	//	if (it == m_pNpcCreatorModule->m_clone_npc_info.end())
	//	{
	//		return 0;
	//	}
	//	cfgXml = &(it->second);
	//}
	//else
	//{
	//	// 其他默认为是分组场景
	//	int nSceneID = pKernel->GetSceneId();
	//	std::string npcCfgName = StringUtil::IntAsString(nSceneID);
	//	auto it = m_pNpcCreatorModule->m_group_npc_info.find(npcCfgName);
	//	if (it == m_pNpcCreatorModule->m_group_npc_info.end())
	//	{
	//		return false;
	//	}
	//	cfgXml = &(it->second);
	//}

	//std::vector<xml_document<>*>& DocVec = *cfgXml;

	//int nCount = int(DocVec.size());

	//int i = 0;

	//if (pSceneObj->FindData("create_random_npc_file_count"))
	//{
	//	i = pSceneObj->QueryDataInt("create_random_npc_file_count");
	//}

 //   LoopBeginCheck(check_x);
	//for (; i < nCount;)
	//{
 //       LoopDoCheck(check_x);
	//	extend_warning(pKernel,"NpcCreatorModule::H_DelayCreateRandomNpc");	

	//	xml_node<>* pNodeRoot = DocVec[i]->first_node("object");

	//	if (pNodeRoot == NULL)
	//	{
	//		return 0;
	//	}

	//	// 创建随机对象生成器
	//	xml_node<>* pNode = pNodeRoot->first_node("randomcreator");

	//	if (pNode != NULL)
	//	{
	//		xml_node<>* pCreator =  NULL;

	//		// 当前创建器的index值
	//		int random_creator_index = 0;

	//		if (pSceneObj->FindData("create_random_creator_index") )
	//		{
	//			// 得到当前正在创建的npc的index
	//			random_creator_index = pSceneObj->QueryDataInt("create_random_creator_index");

	//			pNode = pNode->first_node("creator");
 //               LoopBeginCheck(check_y);
 //               for(int j = 0; j < random_creator_index; j++)
	//			{
 //                   LoopDoCheck(check_y);
	//				pNode = pNode->next_sibling("creator");
	//			}
	//			
	//			pCreator = pNode;

	//		}
	//		else
	//		{
	//			pCreator = pNode->first_node("creator");
	//		}

	//		// 从当前pCreator处开始创建
 //           LoopBeginCheck(check_z);
	//		while (pCreator != NULL)
	//		{
 //               LoopDoCheck(check_z);
	//			// 初始化分组号和分组id
	//			int nSubSceneNo = -1;
	//			int nGroupId = -1;

	//			// 创建npc创建器
	//			m_pNpcCreatorModule->CreateARandomCreator(pKernel, scene, pCreator, nSubSceneNo, nGroupId);

	//			// 得到下一个创建器信息
	//			pCreator = pCreator->next_sibling("creator");

	//			// 索引值递增
	//			random_creator_index++;

	//			unsigned long cur_time = ::util_get_ticks();

	//			// 当前函数执行时间超过最大时间，本次心跳结束
	//			if ((cur_time - begin_time) > MAX_HERAT_BEAT_TIME)
	//			{
	//				// 重新设置当前心跳的次数
	//				pKernel->SetBeatCount(scene,"NpcCreatorModule::H_DelayCreateRandomNpc", 5);

	//				// 保存正在创建的file编号
	//				if (!pSceneObj->FindData("create_random_npc_file_count"))
	//				{
	//					pSceneObj->AddDataInt("create_random_npc_file_count", 0);
	//				}

	//				pSceneObj->SetDataInt("create_random_npc_file_count", i);

	//				// 保存正当前位置
	//				if (!pSceneObj->FindData("create_random_creator_index"))
	//				{
	//					pSceneObj->AddDataInt("create_random_creator_index", 0);
	//				}

	//				pSceneObj->SetDataInt("create_random_creator_index", random_creator_index);	

	//				return 1;
	//			}
	//		}
	//	}

	//	// 文件编号增加
	//	i ++;

	//	// 一个文件中的随机创建器已经创建完毕，删除临时数据
	//	pSceneObj->RemoveData("create_random_creator_index");

	//	// 保存正在创建的file编号
	//	if (!pSceneObj->FindData("create_random_npc_file_count"))
	//	{
	//		pSceneObj->AddDataInt("create_random_npc_file_count", 0);
	//	}
	//	pSceneObj->SetDataInt("create_random_npc_file_count", i);
	//}

	return 1;
}

// 分组场景中分步创建静态npc的心跳
int NpcCreatorModule::H_GroupSceneCreateStaticNpc(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	// 记录函数开始时间
	unsigned long begin_time = ::util_get_ticks();

	// 得到心跳宿主对象
	PERSISTID scene = creator;

	// 保护
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
	if (!pKernel->Exists(scene) || NULL == pSceneObj)
	{
		return 0;
	}

	// 取得表指针
	IRecord* pPrecord = pSceneObj->GetRecord(GROUP_STATIC_CREATE_FLOW);

	if (NULL == pPrecord)
	{
		return 0;
	}

	// 场景id
	int nSceneID = pKernel->GetSceneId();
	
	// 直接通过分组场景id得到npc配置文件
	int rows = pPrecord->GetRows();

    LoopBeginCheck(check_aa);
	for (int n = 0; n < rows; n++)
	{
        LoopDoCheck(check_aa);
		int iSubSceneNo = pPrecord->QueryInt(n, CREATE_FLOW_SUB_SCENE_NO);
		int iGroupID = pPrecord->QueryInt(n, CREATE_FLOW_GROUP_ID);
		int iFileIndex = pPrecord->QueryInt(n, CREATE_FLOW_FILE_INDEX);
		int iItemIndex = pPrecord->QueryInt(n, CREATE_FLOW_ITEM_INDEX);
		// iGroupID 非法，跳过
	/*	if (iGroupID <= 0)
		{
			continue;
		}*/
		const char* npcCfgName = m_pNpcCreatorModule->GetSceneGroupNpcCfgName(iSubSceneNo, iGroupID);
		auto it = m_pNpcCreatorModule->m_group_npc_info.find(npcCfgName);
		if (it == m_pNpcCreatorModule->m_group_npc_info.end())
		{
			continue;
		}

		
		auto& fileGroup = it->second;
		// 遍历文件
        LoopBeginCheck(check_bb);
		auto fileInfo = fileGroup.begin();
		std::advance(fileInfo, iFileIndex);
		for (; fileInfo != fileGroup.end(); fileInfo++)
		{
			auto item = (*fileInfo).m_createStaticGroup.begin();
			std::advance(item, iItemIndex);
			for (; item != (*fileInfo).m_createStaticGroup.end(); item++)
			{
					// 初始化分组号和分组id
					int nSubSceneNo = -1;
					int nGroupId = -1;

					// 创建npc
					m_pNpcCreatorModule->CreateStaticNpcUnit(pKernel, scene, *item, iSubSceneNo, iGroupID);

					// npc创建序号递增
					iItemIndex++;

					// 得到当前系统时间
					unsigned long cur_time = ::util_get_ticks();

					// 当前函数执行时间超过最大时间，本次心跳结束
					if ((cur_time - begin_time) > MAX_HERAT_BEAT_TIME)
					{
						// 重新设置当前心跳的次数
						pKernel->SetBeatCount(scene, "NpcCreatorModule::H_GroupSceneCreateStaticNpc", 5);

						// 保存正在创建的npc file编号
						pPrecord->SetInt(n, CREATE_FLOW_FILE_INDEX, iFileIndex);

						// 保存正在创建的npc item的index
						pPrecord->SetInt(n, CREATE_FLOW_ITEM_INDEX, iItemIndex);

						return 1;
					}
				}
			// 文件No号增加
			iFileIndex++;

			// 一个xml中的静态npc配置已经创建完毕，清除npc索引
			pPrecord->SetInt(n, CREATE_FLOW_ITEM_INDEX, 0);
			iItemIndex=0;

			// 保存正在创建的npc file编号
			pPrecord->SetInt(n, CREATE_FLOW_FILE_INDEX, iFileIndex);
		}
	
		pPrecord->SetInt(n, CREATE_FLOW_GROUP_ID, 0);
		pPrecord->SetInt(n, CREATE_FLOW_SUB_SCENE_NO, 0);
	}

	int delete_index = -1;

    LoopBeginCheck(check_ee);
	while ((delete_index = pPrecord->FindInt(CREATE_FLOW_GROUP_ID,0)) != -1)
	{
        LoopDoCheck(check_ee);
		pPrecord->RemoveRow(delete_index);
	}

	return 1;
}

// 分组场景中分步创建random npc的心跳
int NpcCreatorModule::H_GroupSceneCreateRandomNpc(IKernel* pKernel, const PERSISTID& creator, int slice)
{
	// 记录函数开始时间
	unsigned long begin_time = ::util_get_ticks();

	// 得到心跳宿主对象
	PERSISTID scene = creator;

	// 保护
	if (!pKernel->Exists(scene))
	{
		return 0;
	}
    IGameObj* pSceneObj = pKernel->GetGameObj(scene);
    if (pSceneObj == NULL)
    {
        return 0;
    }

	// 取得表指针
	IRecord* pPrecord = pSceneObj->GetRecord(GROUP_RANDOM_CREATE_FLOW);

	if (NULL == pPrecord)
	{
		return 0;
	}

	// 其他默认为是分组场景
	int nSceneID = pKernel->GetSceneId();

	

	int rows = pPrecord->GetRows();

    LoopBeginCheck(check_ff);
	for (int n = 0; n < rows; n++)
	{
		LoopDoCheck(check_ff);
		int iSubSceneNo = pPrecord->QueryInt(n, CREATE_FLOW_SUB_SCENE_NO);
		int iGroupID = pPrecord->QueryInt(n, CREATE_FLOW_GROUP_ID);
		int iFileIndex = pPrecord->QueryInt(n, CREATE_FLOW_FILE_INDEX);
		UINT iCreateIndex = pPrecord->QueryInt(n, CREATE_FLOW_ITEM_INDEX);

		// iGroupID 非法，跳过
		if (iGroupID <= 0)
		{
			continue;
		}
		const char* npcCfgName = m_pNpcCreatorModule->GetSceneGroupNpcCfgName(iSubSceneNo, iGroupID);

		auto it = m_pNpcCreatorModule->m_group_npc_info.find(npcCfgName);
		if (it == m_pNpcCreatorModule->m_group_npc_info.end())
		{
			continue;
		}

		auto& DocVec = it->second;

		int iDocCount = int(DocVec.size());


		// 遍历文件
		LoopBeginCheck(check_gg);
		for (; iFileIndex < iDocCount;)
		{
			LoopDoCheck(check_gg);
			for (; iCreateIndex < DocVec[iFileIndex].m_createRandGroup.size();)
			{
				auto its = DocVec[iFileIndex].m_createRandGroup[iCreateIndex];

				// 创建npc创建器
				m_pNpcCreatorModule->CreateARandomCreator(pKernel, scene, its, iSubSceneNo, iGroupID);

				// 索引值递增
				iCreateIndex++;

				unsigned long cur_time = ::util_get_ticks();

				// 当前函数执行时间超过最大时间，本次心跳结束
				if ((cur_time - begin_time) > MAX_HERAT_BEAT_TIME)
				{
					// 重新设置当前心跳的次数
					pKernel->SetBeatCount(scene, "NpcCreatorModule::H_GroupSceneCreateRandomNpc", 5);

					// 保存正在创建的npc file编号
					pPrecord->SetInt(n, CREATE_FLOW_FILE_INDEX, iFileIndex);

					// 保存正在创建的npc item的index
					pPrecord->SetInt(n, CREATE_FLOW_ITEM_INDEX, iCreateIndex);

					return 1;
				}
			
			}

		// 文件编号增加
		iFileIndex++;

		// 一个文件中的随机创建器已经创建完毕，删除临时数据
		pPrecord->SetInt(n, CREATE_FLOW_ITEM_INDEX, 0);
		iCreateIndex = 0;

		// 保存正在创建的npc file编号
		pPrecord->SetInt(n, CREATE_FLOW_FILE_INDEX, iFileIndex);
	}
		pPrecord->SetInt(n, CREATE_FLOW_GROUP_ID, 0);
		pPrecord->SetInt(n, CREATE_FLOW_SUB_SCENE_NO, 0);
	}

	int delete_index = -1;

    LoopBeginCheck(check_jj);
	while ((delete_index = pPrecord->FindInt(CREATE_FLOW_GROUP_ID,0)) != -1)
	{
        LoopDoCheck(check_jj);
		pPrecord->RemoveRow(delete_index);
	}

	return 1;
}

// int NpcCreatorModule::H_GroupCreateNpcByTime(IKernel*pKernel, const PERSISTID& creator, int slice)
// {
// 	IGameObj* pCreate = pKernel->GetGameObj(creator);
// 	if (pCreate == NULL)
// 	{
// 		return 0;
// 	}
// 	int	littleStepMax = pCreate->QueryInt("LittleStepMax");
// 	int littleStepNow = pCreate->QueryInt("LittlestepNow");
// 	int sceneID = pCreate->QueryInt("SceneId");
// 	int GroupId = pCreate->QueryInt("GroupID");
// 	DWORD GlobalStep = pCreate->QueryInt("GlobalStep");
// 	//是否开始下一组刷新npc
// 	bool IsNextGlobalStep = false;
// 	if (littleStepMax <= littleStepNow)
// 	{
// 		GlobalStep++;
// 		IsNextGlobalStep = true;
// 
// 	}
// 	const char* npcCfgName = m_pNpcCreatorModule->GetSceneGroupNpcCfgName(sceneID, GroupId);
// 	auto it = m_pNpcCreatorModule->m_refresh_npc_by_time.find(npcCfgName);
// 	if (it != m_pNpcCreatorModule->m_refresh_npc_by_time.end())
// 	{
// 		if (GlobalStep < it->second.size())
// 		{
// 			RefreshNpcRule& refreshNpc = it->second[GlobalStep];
// 			
// 
// 			if (IsNextGlobalStep)
// 			{
// 				pCreate->SetInt("LittlestepNow",0);
// 				pCreate->SetInt("GlobalStep", GlobalStep);
// 				pCreate->SetInt("LittleStepMax", refreshNpc._totalStep);
// 				if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_GroupCreateNpcByTime"))
// 				{
// 					DELETE_HEART_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime");
// 				}
// 
// 				ADD_COUNT_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime", refreshNpc._waitTime, 1);
// 				
// 				if (GlobalStep == it->second.size() - 1)
// 				{
// 					CustomSysInfoByGroup(pKernel,
// 					GroupId,
// 					TIPSTYPE_EVENT_CHAT_BROAD_MESSAGE, SECRET_TIPS_LAST_STEP, CVarList());
// 				}
// 				return 0;
// 			}
// 			else
// 			{
// 				littleStepNow++;
// 				pCreate->SetInt("LittlestepNow", littleStepNow);
// 				if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_GroupCreateNpcByTime"))
// 				{
// 					DELETE_HEART_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime");
// 				}
// 
// 				ADD_COUNT_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime", refreshNpc._refreshTime, 1);
// 				if (littleStepNow == 1)
// 				{
// 					if (GlobalStep + 1< it->second.size())
// 					{
// 						RefreshNpcRule& nextStep = it->second[GlobalStep + 1];
// 							CVarList msg;
// 							msg << SERVER_CUSTOMMSG_SECRET_SCENE << SECRET_SCENE_STEP_LIFE_TIMES << littleStepMax*refreshNpc._refreshTime + nextStep._waitTime;
// 						pKernel->CustomByGroup(GroupId, msg);
// 					}
// 					
// 					CVarList msg;
// 					msg << SERVER_CUSTOMMSG_SECRET_SCENE << SECRET_SCENE_LIFE_STEP_TIMES << (int)(it->second.size() - GlobalStep - 1);
// 					pKernel->CustomByGroup(GroupId,msg);
// 				}
// 
// 			}
// 
// 
// 			std::vector<NpcInfo>::iterator itt = refreshNpc._npcInfo.begin();
// 			LoopBeginCheck(a);
// 			for (; itt != refreshNpc._npcInfo.end(); ++itt)
// 			{
// 				LoopDoCheck(a);
// 				// 坐标
// 				float x = itt->_x;
// 				float y = itt->_y;
// 				float z = itt->_z;
// 				int amount = itt->_mount;
// 
// 				// 得到实际高度
// 				y = GetCurFloorHeight(pKernel, x, y, z);
// 
// 				LoopBeginCheck(a);
// 				for (int i = 0; i < amount; i++) {
// 					LoopDoCheck(a);
// 					// 带参数创建NPC
// 					PERSISTID npc_obj = pKernel->CreateObjectArgs("", itt->_id.c_str(), 0, x, y, z, itt->_ay, CVarList() << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << GroupId);
// 					if (!pKernel->Exists(npc_obj))
// 					{
// 						continue;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return true;
// }

int NpcCreatorModule::C_OnCurStepChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old)
{
    BeginRefresh(pKernel, self);
    return 1;
}


void NpcCreatorModule::CleanSceneCfgName(IKernel*pKernel, int groupID)
{
	int sceneID = pKernel->GetSceneId();
	auto it = m_sceneGroupNpcCfgName.find(sceneID);
	if (it != m_sceneGroupNpcCfgName.end())
	{
		auto its = it->second.find(groupID);
		if (its != it->second.end())
		{
			it->second.erase(its);
		}
	}
}

//记录杀死NPC数量
void NpcCreatorModule::RecordKillNpc(IKernel* pKernel, const PERSISTID& npc)
{
    IGameObj* pNpcObj = pKernel->GetGameObj(npc);
    if (pNpcObj == NULL)
    {
        return;
    }
    if (!pNpcObj->FindData("Creator") || !pNpcObj->FindData("CreatorItem"))
    {
        return;
    }
    PERSISTID creator = pNpcObj->QueryDataObject("Creator");
    PERSISTID creator_item = pNpcObj->QueryDataObject("CreatorItem");
    IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
    IGameObj* pItemObj = pKernel->GetGameObj(creator_item);
    if (pCreatorObj == NULL || pItemObj == NULL)
    {
        return;
    }
    if (pKernel->Exists(creator) && pKernel->Exists(creator_item))
    {
        if (pCreatorObj->QueryInt("TotalStep") > 1)
        {
            int totalStep = pCreatorObj->QueryInt("TotalStep");
            int curStep = pCreatorObj->QueryInt("CurStep");

            int cur_num = pCreatorObj->QueryDataInt("CurNum");
            int kill_num = pCreatorObj->QueryDataInt("KillNum");
            int limit_num = pItemObj->QueryInt("Limit");
            int step = pItemObj->QueryInt("Step");
            if (curStep == step)
            {
                kill_num = kill_num + 1;
                pCreatorObj->SetDataInt("KillNum", kill_num);
                if (kill_num >= limit_num && curStep < totalStep && limit_num > 0)
                {
                    pCreatorObj->SetInt("CurStep", curStep + 1);
                    pCreatorObj->SetDataInt("KillNum", 0);
                }
            }
		}
    }
}

bool NpcCreatorModule::LoadNpcPool(IKernel* pKernel)
{
    m_mapNpcPool.clear();
    std::string xmlPath = pKernel->GetResourcePath();
    xmlPath += NPC_POOL;

    try
    {
       char* pfilebuff = GetFileBuff(xmlPath.c_str());

        if (NULL == pfilebuff)
        {
            return false;
        }

        xml_document<> doc;

        doc.parse<rapidxml::parse_default>(pfilebuff);

        xml_node<>* pNodeRoot = doc.first_node("Object");
        if (NULL == pNodeRoot)
        {
            return false;
        }
        xml_node<>* pNode = pNodeRoot->first_node("Property");
        if (NULL == pNode)
        {
            return false;
        }

        LoopBeginCheck(check_kk);
        while (pNode != NULL)
        {
            LoopDoCheck(check_kk);
            const char* id = QueryXmlAttr(pNode, "ID");
            xml_node<>* pItem = pNode->first_node("Item");
            std::vector<NpcPool> vecPool;
            LoopBeginCheck(check_ll);
            while (pItem != NULL)
            {
                LoopDoCheck(check_ll);
                const char* npcID = QueryXmlAttr(pItem, "ConfigID");
                int canSame = convert_int(QueryXmlAttr(pItem, "CanSame"), 0);
                int weight = convert_int(QueryXmlAttr(pItem, "Weight"), 0);

                NpcPool info;
                info.configID = npcID;
                info.isSame = canSame;
                info.weight = weight;
                if (StringUtil::CharIsNull(info.configID.c_str()))
                {
					pItem = pItem->next_sibling("Item");
                    continue;
                }

                vecPool.push_back(info);

                pItem = pItem->next_sibling("Item");

            }

            m_mapNpcPool.insert(make_pair(id, vecPool));

            pNode = pNode->next_sibling("Property");
        }
    }
    catch(parse_error& e)
    {
        ::extend_warning(pKernel, e.what());

        return false;
    }

    return true;
}

int NpcCreatorModule::GetConfigID(IKernel* pKernel, const PERSISTID& pCreator, const char* npcPoolID, std::string& newConfigID)
{
    std::map<std::string,std::vector<NpcPool> >::iterator iter = m_pNpcCreatorModule->m_mapNpcPool.find(npcPoolID);
    if (iter == m_pNpcCreatorModule->m_mapNpcPool.end())
    {
        return 0;
    }
    std::vector<NpcPool> tarVec;
    m_pNpcCreatorModule->GetNpcFromPool(pKernel, pCreator, iter->second, tarVec);
    
    int totalWeight = m_pNpcCreatorModule->CalWeight(tarVec);
    int random = util_random_int(totalWeight);

    int size = (int)tarVec.size();
    LoopBeginCheck(check_mm);
    for (int i = 0; i < size; i++)
    {
        LoopDoCheck(check_mm);
        NpcPool& info = tarVec[i];

        int minWeight = info.minWeight;
        int maxWeight = info.maxWeight;
        if (random >= minWeight && random <= maxWeight)
        {
            newConfigID = info.configID.c_str();
            break;
        }
    }
    
    return 0;
}

//计算总权值
int NpcCreatorModule::CalWeight(std::vector<NpcPool>& vec)
{
    int totalWeight = 0;
    int size = (int)vec.size();
    LoopBeginCheck(check_nn);
    for (int i = 0; i < size; i++)
    {
        LoopDoCheck(check_nn);
        NpcPool& info = vec[i];
        int weight = info.weight;

        info.minWeight = totalWeight;
        
        info.maxWeight = totalWeight + weight - 1;

        totalWeight = totalWeight + weight;
    }

    return totalWeight;
}

void NpcCreatorModule::GetNpcFromPool(IKernel* pKernel, const PERSISTID& pCreator, std::vector<NpcPool> srcVec, std::vector<NpcPool>& tarVec)
{
    IGameObj* pCreatorObj = pKernel->GetGameObj(pCreator);
    if (NULL == pCreatorObj)
    {
        return;
    }
    IRecord* creatRec = pCreatorObj->GetRecord(CREATE_OBJECT_LIST_REC);
    if (NULL == creatRec)
    {
        return;
    }
   
    std::vector<NpcPool>::iterator iter = srcVec.begin();
    LoopBeginCheck(check_oo);
    for ( ;iter != srcVec.end(); ++iter)
    {
        LoopDoCheck(check_oo);
        NpcPool& info = *iter;
        const char* configID = info.configID.c_str();
        if (info.isSame == 1)
        {
            bool isBorn = false;

            int rows = creatRec->GetRows();

            LoopBeginCheck(check_pp);
            for (int i = 0; i < rows; i ++)
            {
                LoopDoCheck(check_pp);
                PERSISTID obj = creatRec->QueryObject(i, 0);
                if (!pKernel->Exists(obj))
                {
                    continue;
                }
                const char* str = pKernel->GetConfig(obj);
                if (strcmp(configID, str) == 0)
                {
                    isBorn = true;
                    break;
                }
                
            }
            if (!isBorn)
            {
                tarVec.push_back(info);
            }
        }
        else
        {
            tarVec.push_back(info);
        }
    }
}

//提前设定随机位置
bool NpcCreatorModule::PreSetCreateCenterPos(IKernel* pKernel, const PERSISTID& creator)
{
	CVarList creator_items;
	pKernel->GetChildList(creator, TYPE_HELPER, creator_items);
    IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
    if (pCreatorObj == NULL)
    {
        return false;
    }

	//获取中心点
	len_t CenterX = 0.0f;
	len_t CenterZ = 0.0f;
	// 遍历
	int creator_item_counts = (int)creator_items.GetCount();
    LoopBeginCheck(check_qq);
	for (int i = 0; i < creator_item_counts; i++)
	{
        LoopDoCheck(check_qq);
		PERSISTID creator_item = creator_items.ObjectVal(i);
        IGameObj* pItemObj = pKernel->GetGameObj(creator_item);
		if (pKernel->Exists(creator_item) && pItemObj != NULL)
		{
            int refreshCount = pItemObj->QueryInt("MaxAmount");
            if (refreshCount > 1)
            {
                break;
            }
            IRecord* positionRec = pItemObj->GetRecord(POSITION_INFO_REC);
            if (positionRec == NULL)
            {
                continue;
            }
			int iPosCount = positionRec->GetRows();
			if (iPosCount > 0)
			{
				//随机一个数据
				int iRand = util_random_int(iPosCount);
				//随机行数
				pCreatorObj->SetInt("RandRow", iRand);
				pCreatorObj->SetObject("RandHelper", creator_item);

				len_t x = positionRec->QueryFloat(iRand, POSIINFOREC_COL_X);
				len_t z = positionRec->QueryFloat(iRand, POSIINFOREC_COL_Z);
				CenterX = x;
				CenterZ = z;
				break;
			}
		}
	}

	len_t MaxScope = pCreatorObj->QueryFloat("MaxScope");
	len_t MinScope = pCreatorObj->QueryFloat("MinScope");
	//针对没有设定位置的创建器重新分配位置
    LoopBeginCheck(check_rr);
	for (int i = 0; i < creator_item_counts; i++)
	{
        LoopDoCheck(check_rr);
		PERSISTID creator_item = creator_items.ObjectVal(i);
        IGameObj* pItemObj = pKernel->GetGameObj(creator_item);
		if (NULL == pItemObj)
		{
			continue;
		}
        IRecord* positionRec = pItemObj->GetRecord(POSITION_INFO_REC);
        if (positionRec == NULL)
        {
            continue;
        }
		// 数量比较
		int max_amount = pItemObj->QueryInt("MaxAmount");
        int iPosCount = positionRec->GetRows();
        if (iPosCount <= 0 && max_amount > 0)
        {
            LoopBeginCheck(check_ss);
            for (int j = 0; j < max_amount; j++)
            {
                LoopDoCheck(check_ss);
                float x1, y1, z1, o1;

                float orient = util_random_float(PI2);
                // 随机取周围的点,防止死循环
                LoopBeginCheck(check_tt);
                for (int k = 0; k < 8; ++k)
                {
                    LoopDoCheck(check_tt);
                    x1 = CenterX + (MinScope + util_random_float(MaxScope - MinScope)) * ::sinf(orient);
                    z1 = CenterZ + (MinScope + util_random_float(MaxScope - MinScope)) * ::cosf(orient);
                    y1 = pKernel->GetMapHeight(x1, z1);
                    if (pKernel->CanWalk(x1, z1))
                    {
                        break;
                    }
                }

                //如果最后随机出来的点还是存在问题
                if (!pKernel->CanWalk(x1, z1))
                {
                    x1 = CenterX;
                    z1 = CenterZ;
                }

                o1 = PI2 * ::util_random_float(1.0f);

                // 添加到位置表格
                positionRec->AddRowValue(-1, 
                    CVarList() << x1 << y1 << z1 << o1 << 0 << (__int64)0 << -1.0f);
            }
        }
	}

	return true;
}

bool NpcCreatorModule::CreateRandomNpcByTime(IKernel*pKernel, const PERSISTID& scene, int sceneId, int groupId)
{
	
	if (!pKernel->Exists(scene))
	{
		return 0;
	}
	IGameObj* pSceneObj = pKernel->GetGameObj(scene);
	if (pSceneObj == NULL)
	{
		return 0;
	}
	const char * npcCfgName = GetSceneGroupNpcCfgName(sceneId, groupId);
	auto it = m_refresh_npc_by_time.find(npcCfgName);
	if (it != m_refresh_npc_by_time.end())
	{
		auto its = it->second.begin();
		if (its != it->second.end())
		{
				
			PERSISTID creator = pKernel->Create(scene, "NpcCreatorByTime");
			IGameObj* pCreatorObj = pKernel->GetGameObj(creator);
			if (pCreatorObj == NULL){
				return false;
			}
			pCreatorObj->SetInt("GlobalStep", 0);
			pCreatorObj->SetInt("LittleStepMax",its->_totalStep);
			pCreatorObj->SetInt("LittlestepNow",0);
			pCreatorObj->SetInt("SceneId",sceneId);
				// 是分组场景的话设置组号
			if (pSceneObj->QueryInt("Grouping") >= 1)
			{
				pCreatorObj->SetInt("GroupID", groupId);

			}
			if (pKernel->FindHeartBeat(creator, "NpcCreatorModule::H_GroupCreateNpcByTime"))
			{
				DELETE_HEART_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime");
			}
			ADD_COUNT_BEAT(pKernel, creator, "NpcCreatorModule::H_GroupCreateNpcByTime", its->_waitTime, 1);

		}


	}


	return true;

}

//获得场景内NPC数量信息（副本分组场景使用）
int NpcCreatorModule::GetSceneNpcNum(IKernel* pKernel, const PERSISTID& scene)
{
 //   IGameObj* pSceneObj = pKernel->GetGameObj(scene);
 //   if (NULL == pSceneObj)
 //   {
 //       return 0;
 //   }
 //   int sceneID = pKernel->GetSceneId();
 //   if (GroupSceneBase::GetSceneType(pKernel, sceneID) == GROUP_SCENE_NULL)
 //   {
 //       return 0;
 //   }
 //   std::vector<int>::iterator iter = 
 //       find(m_pNpcCreatorModule->v_vecSceneNpc.begin(), m_pNpcCreatorModule->v_vecSceneNpc.end(), sceneID);
 //   if (iter != m_pNpcCreatorModule->v_vecSceneNpc.end())
 //   {
 //       //已存在，返回
 //       return 0;
 //   }
	//std::string npcCfgName = StringUtil::IntAsString(sceneID);
	//auto fileit = m_group_npc_info.find(npcCfgName);
 //   if (fileit == m_group_npc_info.end())
 //   {
 //       return 0;
 //   }
 //   int eliteNum = 0;
 //   int bossNum = 0;
 //   int boxNum = 0;
 //   int commonNum = 0;

 //   auto& DocVec = fileit->second;

 //   int iDocCount = int(DocVec.size());

 //   // 遍历文件
 //   LoopBeginCheck(check_uu);
 //   for (int iFileIndex = 0; iFileIndex < iDocCount; ++iFileIndex)
 //   {
 //       LoopDoCheck(check_uu);
 //       xml_node<>* pNodeRoot = DocVec[iFileIndex]->first_node("object");

 //       if (pNodeRoot == NULL)
 //       {
 //           continue;
 //       }

 //       // 创建随机对象生成器
 //       xml_node<>* pNode = pNodeRoot->first_node("randomcreator");

 //       if (pNode == NULL)
 //       {
 //           continue;
 //       }

 //       // 创建器的指针
 //       xml_node<>* pCreator = pNode->first_node("creator");

 //       // 从当前pCreator处开始创建
 //       LoopBeginCheck(check_vv);
 //       while (pCreator != NULL)
 //       {
 //           LoopDoCheck(check_vv);
 //           xml_node<>* pItem = pCreator->first_node("item");
 //           LoopBeginCheck(check_ww);
 //           while (pItem != NULL)
 //           {
 //               LoopDoCheck(check_ww);
 //               // 最大生成的数量
 //               int npc_amount = convert_int(QueryXmlAttr(pItem, "amount"));
 //               int npcType = convert_int(QueryXmlAttr(pItem, "npctype"));
 //               switch(npcType)
 //               {
 //               case CREATE_NPC_TYPE_ELITE:
 //                   eliteNum = eliteNum + npc_amount;
 //                   break;
 //               case CREATE_NPC_TYPE_BOSS:
 //                   bossNum = bossNum + npc_amount;
 //                   break;
 //               case CREATE_NPC_TYPE_BOX:
 //                   boxNum = boxNum + npc_amount;
 //                   break;
 //               default:
 //                   commonNum = commonNum + npc_amount;
 //                   break;
 //               }

 //               pItem = pItem->next_sibling("item");
 //           }

 //           // 得到下一个创建器信息
 //           pCreator = pCreator->next_sibling("creator");
 //       }
 //   }
 //   pSceneObj->SetInt("EliteNum", eliteNum);
 //   pSceneObj->SetInt("BossNum", bossNum);
 //   pSceneObj->SetInt("BoxNum", boxNum);
 //   pSceneObj->SetInt("CommonNum", commonNum);
    
//    m_pNpcCreatorModule->v_vecSceneNpc.push_back(sceneID);
    return 0;
}

// bool NpcCreatorModule::LoadGuildInbreak(IKernel* pKernel)
// {
// 	m_guildInbreak.clear();
// 	// 任务Npc配置
// 	std::string file_path = pKernel->GetResourcePath();
// 	file_path += GUILD_INBREAK;
// 	try
// 	{
// 		char* pfilebuff = GetFileBuff(file_path.c_str());
// 
// 		if (StringUtil::CharIsNull(pfilebuff))
// 		{
// 			std::string err_msg = file_path;
// 			err_msg.append(" does not exists.");
// 			::extend_warning(LOG_ERROR, err_msg.c_str());
// 			return false;
// 		}
// 
// 		xml_document<> doc;
// 
// 		doc.parse<rapidxml::parse_default>(pfilebuff);
// 
// 		xml_node<>* pNodeRoot = doc.first_node("object");
// 
// 		if (pNodeRoot == NULL)
// 		{
// 			std::string err_msg = file_path;
// 			err_msg.append(" invalid npc creator file.");
// 			::extend_warning(LOG_ERROR, err_msg.c_str());
// 			return false;
// 		}
// 
// 		xml_node<>* pNode = pNodeRoot->first_node("randomcreator");
// 		if (pNode == NULL)
// 		{
// 			std::string err_msg = file_path;
// 			err_msg.append(" invalid npc creator file.");
// 			::extend_warning(LOG_ERROR, err_msg.c_str());
// 			return false;
// 		}
// 
// 		xml_node<>* pCreator = pNode->first_node("creator");
// 		// 从当前pCreator处开始创建
// 		LoopBeginCheck(a);
// 		while (pCreator != NULL)
// 		{
// 			CreateSpringNpcInfo createSpringNpcInfo;
// 
// 			LoopDoCheck(a);
// 			// 初始化分组号和分组id
// 			int nSubSceneNo = -1;
// 			int nGroupId = -1;
// 
// 			// 坐标
// 			createSpringNpcInfo.m_springPosition.m_x = convert_float(QueryXmlAttr(pCreator, "x"));
// 			createSpringNpcInfo.m_springPosition.m_y = convert_float(QueryXmlAttr(pCreator, "y"));
// 			createSpringNpcInfo.m_springPosition.m_z = convert_float(QueryXmlAttr(pCreator, "z"));
// 
// 			// 外环
// 			createSpringNpcInfo.m_maxScope = convert_float(QueryXmlAttr(pCreator, "MaxScope"), 30);
// 			//内环
// 			createSpringNpcInfo.m_minScope = convert_float(QueryXmlAttr(pCreator, "MinScope"), 25);
// 			// 触发半径
// 			createSpringNpcInfo.m_springRange = convert_float(QueryXmlAttr(pCreator, "SpringRange"), 50.0f);
// 			//0-触发后创建NPC，1-进入场景就创建
// 			createSpringNpcInfo.m_createType = convert_int(QueryXmlAttr(pCreator, "createtype"), 0);
// 
// 			//怪物的波数
// 			createSpringNpcInfo.m_totalStep = convert_int(QueryXmlAttr(pCreator, "totalstep"), 0);
// 			//NPC是否销毁 0-是 1-否
// 			createSpringNpcInfo.m_isDestroy = convert_int(QueryXmlAttr(pCreator, "IsDestroy", 0)) > 0 ? true : false;
// 			//关卡秘境行进步骤
// 			createSpringNpcInfo.m_sceneAction = convert_int(QueryXmlAttr(pCreator, "SceneAction", 0));
// 
// 			// 生成器编号
// 			const char *creator_no = QueryXmlAttr(pCreator, "No", "");
// 			createSpringNpcInfo.m_no = creator_no;
// 
// 			int totalNpcNum = 0;
// 			createSpringNpcInfo.m_totalNpcNum = totalNpcNum;
// 			createSpringNpcInfo.m_totalNpcNum = ParseRandomNpcItem(pKernel, createSpringNpcInfo.m_NpcGroup, pCreator);
// 
// 			m_guildInbreak.insert(std::make_pair(creator_no, createSpringNpcInfo));
// 
// 			// 得到下一个创建器信息
// 			pCreator = pCreator->next_sibling("creator");
// 		}
// 
// 		delete[] pfilebuff;
// 
// 	}
// 	catch (parse_error& e)
// 	{
// 		::extend_warning(pKernel, "[Error]NpcCreatorModule::LoadTaskCreatorRes: ");
// 		::extend_warning(pKernel, file_path.c_str());
// 		::extend_warning(pKernel, e.what());
// 
// 		return false;
// 	}
// 
// 	return true;
// }


//获得可出生的BOSS
// void NpcCreatorModule::GetBossFromPool(IKernel* pKernel, const char* npcPool, std::string& newConfig)
// {
//     const char* bossConfig = "";
//     IGameObj* pSceneObj = pKernel->GetSceneObj();
//     if (NULL == pSceneObj)
//     {
//         return;
//     }
//     IRecord* record = pSceneObj->GetRecord(SCENE_BOSS_REC);
//     if (NULL == record)
//     {
//         return;
//     }
//     std::map<std::string,std::vector<NpcPool> >::iterator iter = m_mapNpcPool.find(npcPool);
//     if (iter == m_mapNpcPool.end())
//     {
//         return;
//     }
//     std::vector<NpcPool>& npcVec = iter->second;
//     int size = (int)npcVec.size();
//     std::vector<NpcPool> targetVec;
//     LoopBeginCheck(check_xx);
//     for (int i = 0; i < size; ++i)
//     {
//         LoopDoCheck(check_xx);
//         NpcPool& info = npcVec[i];
//         const char* configID = info.configID.c_str();
//         if (info.isSame == 1)
//         {
//             int row = record->FindString(SCENE_BOSS_CONFIG, configID);
//             if (row >= 0)
//             {
//                 continue;
//             }
//             targetVec.push_back(info);
//         }
//         else
//         {
//             targetVec.push_back(info);
//         }
//     }
//     int total = m_pNpcCreatorModule->CalWeight(targetVec);
//     int random = util_random_int(total);
// 
//     int targetSize = (int)targetVec.size();
//     LoopBeginCheck(check_yy);
//     for (int i = 0; i < targetSize; i++)
//     {
//         LoopDoCheck(check_yy);
//         NpcPool& info = targetVec[i];
// 
//         int minWeight = info.minWeight;
//         int maxWeight = info.maxWeight;
//         if (random >= minWeight && random <= maxWeight)
//         {
//             newConfig = info.configID;
//             break;
//         }
//     }
// }
// 
// void NpcCreatorModule::upDateWheelNpc(IKernel*pKernel, int sceneID, int groupID, int killNumNow)
// {
// 
// 	const char* npcCfgName = GetSceneGroupNpcCfgName(sceneID, groupID);
// 	//判断当前场景是否有车轮npc
// 	auto it = m_wheel_npc.find(npcCfgName);
// 	if (it == m_wheel_npc.end()){
// 		return;
// 	}
// 
// 
// 	CVarList res;
// 	pKernel->GroupChildList(groupID, TYPE_NPC, res);
// 	LoopBeginCheck(a);
// 	for (size_t i = 0; i < res.GetCount(); i++)
// 	{
// 		LoopDoCheck(a);
// 		PERSISTID& obj = res.ObjectVal(i);
// 		if (pKernel->Exists(obj))
// 		{
// 			IGameObj * sceneObj = pKernel->GetGameObj(obj);
// 			if (sceneObj->GetClassType() == TYPE_NPC)
// 			{
// 				if (sceneObj->FindData("NeedKillNum"))
// 				{
// 					int needNum = sceneObj->QueryDataInt("NeedKillNum");
// 					if (needNum <= 0){
// 						continue;
// 					}
// 						needNum -= 1;
// 					if (needNum <= 0)
// 					{
// 						FightInterfaceInstance->RemoveBuffer(pKernel, obj, CONTROL_BUFFID);
// 					}
// 					else
// 					{
// 						sceneObj->SetDataInt("NeedKillNum", needNum);
// 					}
// 				}
// 			}
// 		}
// 	}
// 
// 
// 	CreateWheelNpc(pKernel, sceneID, groupID, killNumNow);
// 
// }
