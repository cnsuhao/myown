//--------------------------------------------------------------------
// 文件名:      AILoadResource.cpp
// 内  容:      AI系统加载资源
// 说  明:
// 创建日期:    2015年3月16日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AISystem.h"
#include "AIDefine.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "utils/math/fm_fixedpoint.h"
#include <algorithm>
#include "utils/string_util.h"
#include "FsGame/Interface/FightInterface.h"

//-------------------------------------加载配置-----------------------------------------//
//巡逻路径配置
bool AISystem::LoadPatrolPath(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += PATROL_PATH_CONFIG;

	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapPatrolInfo.clear();//路径点
		std::vector<PatrolPos> v_step;
        LoopBeginCheck(c);
		while (pNode != NULL)
		{
            LoopDoCheck(c);
			const char* npcId = QueryXmlAttr(pNode,"ID");
			xml_node<>* step = pNode->first_node("Step");
			v_step.clear();
			int id = 0;
            LoopBeginCheck(d);
			while (step != NULL)
			{
                LoopDoCheck(d);
				PatrolPos point;
				point.PosX = convert_float(QueryXmlAttr(step,"x"));
				point.PosZ = convert_float(QueryXmlAttr(step,"z"));
				point.iRestTime = convert_int(QueryXmlAttr(step,"resettime"));

				v_step.push_back(point);
				step = step->next_sibling("Step");
			}

			m_pAISystem->m_mapPatrolInfo.insert(make_pair(npcId,v_step));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch(parse_error& e)
	{
		::extend_warning(pKernel, "AISystem::LoadPatrolPath: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

//护送路径配置
bool AISystem::LoadConvoyPath(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += CONVOY_PATH_CONFIG;

	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapConvoyStep.clear();//护送NPC路径
		std::vector<Point> v_step;
		LoopBeginCheck(a);
		while (pNode != NULL)
		{
			LoopDoCheck(a);
			const char* npcId = QueryXmlAttr(pNode, "ID");
			int maxStep = convert_int(QueryXmlAttr(pNode, "MaxStep"));

			xml_node<>* step = pNode->first_node("Step");
			v_step.clear();
			LoopBeginCheck(b);
			while (step != NULL)
			{
				LoopDoCheck(b);
				float x = convert_float(QueryXmlAttr(step, "x"));
				float z = convert_float(QueryXmlAttr(step, "z"));
				float y = convert_float(QueryXmlAttr(step, "y"));

				const Point point(x, y, z);
				v_step.push_back(point);
				step = step->next_sibling("Step");
			}

			m_pAISystem->m_mapConvoyStep.insert(make_pair(npcId, v_step));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch (parse_error& e)
	{
		::extend_warning(pKernel, "[Error]CreateStaticNpcFromXml: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

//拦截怪物配置加载
bool AISystem::LoadInterceptInfo(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += CONVOY_INTERCEPT_CONFIG;
	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapIntercept.clear();//拦截怪信息
		std::vector<Intercept> v_intecept;
        LoopBeginCheck(e);
		while (pNode != NULL)
		{
            LoopDoCheck(e);
			const char* npcId = QueryXmlAttr(pNode,"ID");
			xml_node<>* step = pNode->first_node("Step");
			v_intecept.clear();
            LoopBeginCheck(f);
			while (step != NULL)
			{
                LoopDoCheck(f);
				//拦截怪物
				Intercept interceptNpc;
				int id = convert_int(QueryXmlAttr(step, "step"));
				const char* intercept = QueryXmlAttr(step,"intercept");
				int num = convert_int(QueryXmlAttr(step,"num"));
				float monsterX = convert_float(QueryXmlAttr(step,"monsterX"));
				float monsterZ = convert_float(QueryXmlAttr(step,"monsterZ"));
				interceptNpc.npcId = intercept;
				interceptNpc.num = num;
				interceptNpc.id = id;
				interceptNpc.monsterX = monsterX;
				interceptNpc.monsterZ = monsterZ;
				v_intecept.push_back(interceptNpc);
				step = step->next_sibling("Step");
			}

			m_pAISystem->m_mapIntercept.insert(make_pair(npcId, v_intecept));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch(parse_error& e)
	{
		::extend_warning(pKernel, "[Error]AISystem::LoadInterceptInfo: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

//怪物技能加载
bool AISystem::LoadNPCSkill(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += NPC_SKILL_CONFIG;
	CXmlFile xml(res_path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = res_path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}
	m_pAISystem->m_mapSkillConfig.clear();
	size_t count = xml.GetSectionCount();
	std::vector<NpcSkill> m_vec;
	int tempGroup = 0;
    LoopBeginCheck(g);
	for (size_t i=0;i<count;i++)
	{
        LoopDoCheck(g);
		const char * skillID = xml.GetSectionByIndex(i);

		int max = xml.ReadInteger(skillID, "MaxWeighting", 0);
		int add = xml.ReadInteger(skillID, "AddWeighting", 0);
		int prior = xml.ReadInteger(skillID, "Prior", 0);
		int condition = xml.ReadInteger(skillID, "Condition", 0);
        int time = xml.ReadInteger(skillID, "Time", 0);
        int initWeighting = xml.ReadInteger(skillID, "initWeighting", 0);

		NpcSkill npcSkill;
		npcSkill.skillID = skillID;
		npcSkill.maxWeighting = max;
		npcSkill.addWeighting = add;
		npcSkill.prior = prior;
		npcSkill.condition = condition;
        npcSkill.time = time;
        npcSkill.initWeighting = initWeighting;
		m_pAISystem->m_mapSkillConfig.insert(std::make_pair(skillID,npcSkill));
	}
	return true;
}

//怪物技能包
bool AISystem::LoadNpcSkillPackage(IKernel *pKernel)
{
    std::string path = pKernel->GetResourcePath();
    path += NPC_SKILL_PACKAGE_CONFIG;

    char* pfilebuff = GetFileBuff(path.c_str());
    if (pfilebuff == NULL)
    {
        extend_warning(LOG_ERROR, "[AISystem::LoadNpcSkillPackage] failed");
        return false;
    }

    try
    {
        xml_document<> doc;
        doc.parse<rapidxml::parse_default>(pfilebuff);
        xml_node<>* pNodeRoot = doc.first_node("Object");
        if (pNodeRoot == NULL)
        {
            delete[] pfilebuff;
            return false;
        }
        xml_node<>* pNode = pNodeRoot->first_node("Property");
        if (pNode == NULL)
        {
            delete[] pfilebuff;
            return false;
        }

        LoopBeginCheck(h);
        while (pNode != NULL)
        {
            LoopDoCheck(h);
            NpcSkillPackage npcSkillPackage;
            std::string id = QueryXmlAttr(pNode, "ID", "");
            npcSkillPackage.id = id.c_str();
            npcSkillPackage.normalSkill = QueryXmlAttr(pNode, "NormalSkill", "");
			npcSkillPackage.bornskill = QueryXmlAttr(pNode, "BornSkill", "");
            npcSkillPackage.randCount = convert_int(QueryXmlAttr(pNode, "Num"));
            
            CVarList skillList;
            if(npcSkillPackage.randCount  > 0)
            {
                util_split_string(skillList, QueryXmlAttr(pNode, "RandomSkillLib", ""), ",");
                size_t count = skillList.GetCount();
                LoopBeginCheck(i);
                for(size_t i = 0; i < count; ++i)
                {
                    LoopDoCheck(i);
                    npcSkillPackage.skillVec.push_back(skillList.StringVal(i));
                }
            }
            m_pAISystem->m_mapSkillPackage.insert(std::make_pair(id, npcSkillPackage));

            pNode = pNode->next_sibling("Property");
        }

        delete[] pfilebuff;
        return true;
    }
    catch(parse_error& e)
    {
        delete[] pfilebuff;
        ::extend_warning(pKernel, e.what());
        return false;
    }


    return true;
}

//国战NPC配置
bool AISystem::LoadBattleNpcBornIndex(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += BATTLE_NPC_CONFIG;

	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapBattleNpcInfo.clear();//路径点
        LoopBeginCheck(j);
		while (pNode != NULL)
		{
            LoopDoCheck(j);
			int iSceneID = convert_int(QueryXmlAttr(pNode, "ID"));
			xml_node<>* pNodeCreator = pNode->first_node("Creator");
			if (NULL == pNodeCreator)
			{
				continue;
			}
		
			BattleNpcStruct TempBattleStruct;
			TempBattleStruct.BattleLeadNpc = QueryXmlAttr(pNodeCreator,"BattleLeadNpc");
			TempBattleStruct.BattleNpc = QueryXmlAttr(pNodeCreator,"LeadNpc");
			xml_node<>* Item = pNodeCreator->first_node("Item");
			std::map<int, std::map<int, BattlePos>> mTempCampBattlePos;
            LoopBeginCheck(k);
			while (Item != NULL)
			{
                LoopDoCheck(k);
				int iCamp = convert_int(QueryXmlAttr(Item, "Camp"));
				xml_node<>* pPosition = Item->first_node("Position");
				map<int, BattlePos> mTempBattlePos;
                LoopBeginCheck(l);
				while(pPosition != NULL)
				{
                    LoopDoCheck(l);
					BattlePos tempPos;
					tempPos.fPosX = convert_float(QueryXmlAttr(pPosition,"x"));
					tempPos.fPosZ = convert_float(QueryXmlAttr(pPosition,"z"));
					tempPos.fOrient = convert_float(QueryXmlAttr(pPosition,"orient"));

					int iIndex = convert_int(QueryXmlAttr(pPosition,"Index"));
					mTempBattlePos.insert(make_pair(iIndex, tempPos));
					pPosition = pPosition->next_sibling("Position");
				}
				mTempCampBattlePos.insert(make_pair(iCamp, mTempBattlePos));
				Item = Item->next_sibling("Item");
			}
			TempBattleStruct.m_MapBattleNpc = mTempCampBattlePos;
			m_pAISystem->m_mapBattleNpcInfo.insert(make_pair(iSceneID, TempBattleStruct));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch(parse_error& e)
	{
		::extend_warning(pKernel, "AISystem::LoadBattleNpc: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

//行走路径配置
bool AISystem::LoadBattleNpcMoveType(IKernel* pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += BATTLE_NPC_MOVETYPE_CONFIG;

	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapMoveTypeInfo.clear();//路径点
        LoopBeginCheck(m);
		while (pNode != NULL)
		{
            LoopDoCheck(m);
			int iSceneID = convert_int(QueryXmlAttr(pNode, "ID"));
			xml_node<>* Item = pNode->first_node("Item");
			MoveTypeDef TempMoveTypeDef;
            LoopBeginCheck(n);
			while(Item != NULL)
			{
                LoopDoCheck(n);
				int iCamp = convert_int(QueryXmlAttr(Item, "Camp"));
				xml_node<>* pNodeCreator = Item->first_node("Creator");
				std::map<int, std::vector<MoveTypePos>> TempMoveTypeStruct;
                LoopBeginCheck(o);
				while(pNodeCreator != NULL)
				{
                    LoopDoCheck(o);
					int iType = convert_int(QueryXmlAttr(pNodeCreator, "Type"));
					xml_node<>* Step = pNodeCreator->first_node("Step");
					vector<MoveTypePos> mTempMoveType;
                    LoopBeginCheck(p);
					while(Step != NULL)
					{
                        LoopDoCheck(p);
						MoveTypePos tempPos;
						tempPos.fPosX = convert_float(QueryXmlAttr(Step,"x"));
						tempPos.fPosZ = convert_float(QueryXmlAttr(Step,"z"));
						mTempMoveType.push_back(tempPos);
						Step = Step->next_sibling("Step");
					}
					TempMoveTypeStruct.insert(make_pair(iType, mTempMoveType));
					pNodeCreator = pNodeCreator->next_sibling("Creator");
				}
				TempMoveTypeDef.insert(make_pair(iCamp, TempMoveTypeStruct));
				Item = Item->next_sibling("Item");
			}
			m_pAISystem->m_mapMoveTypeInfo.insert(make_pair(iSceneID, TempMoveTypeDef));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch(parse_error& e)
	{
		::extend_warning(pKernel, "AISystem::LoadBattleNpcMoveType: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

//读取创建小队配置
bool AISystem::LoadBattleNpcTeam(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += BATTLE_TEAM_CONFIG;

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	m_pAISystem->m_vecBattleTeamConfig.clear();

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_pAISystem->m_vecBattleTeamConfig.reserve(iSectionCount);

	LoopBeginCheck(f);
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(f);
		const char* section = xmlfile.GetSectionByIndex(i);

		BattleTeam data;
		data.nTeamId	= StringUtil::StringAsInt(section);
		data.nCamp		= xmlfile.ReadInteger(section, "Camp", 1);
		data.nMoveType	= xmlfile.ReadInteger(section, "MoveType", 1);
		data.nBornPosIndex = xmlfile.ReadInteger(section, "BornPosIndex", 1);
		data.nLifeTime	= xmlfile.ReadInteger(section, "LifeTime", 0) * 1000;

		m_pAISystem->m_vecBattleTeamConfig.push_back(data);
	}
	return true;
}


//读取小队队形配置
bool AISystem::LoadBattleTeamFormation(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += BATTLE_TEAM_FORMATION_CONFIG;

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	m_pAISystem->m_BattleTeamFormationVec.clear();

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_pAISystem->m_BattleTeamFormationVec.reserve(iSectionCount);

	LoopBeginCheck(ff);
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(ff);
		const char* section = xmlfile.GetSectionByIndex(i);
		const int scene_id = xmlfile.ReadInteger(section, "SceneID", 0);

		BattleTeamFormation formation(scene_id);
		formation.row = xmlfile.ReadInteger(section, "Row", 1);
		formation.column = xmlfile.ReadInteger(section, "Column", 1);


		m_pAISystem->m_BattleTeamFormationVec.push_back(formation);
	}
	return true;
}

//神秘宝箱配置
bool AISystem::LoadBoxConfig(IKernel *pKernel)
{
	std::string res_path = pKernel->GetResourcePath();
	res_path += BOX_WEIGHT_CONFIG;

	try
	{
		char* pfilebuff = GetFileBuff(res_path.c_str());

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
		m_pAISystem->m_mapBoxInfo.clear();	//神秘宝箱掉落
		BoxWeight box;
        LoopBeginCheck(p);
		while (pNode != NULL)
		{
            LoopDoCheck(p);
			int weight_sum = 0;
			box.configId = QueryXmlAttr(pNode,"ID");

			xml_node<>* Drop = pNode->first_node("Drop");
            LoopBeginCheck(q);
			while (Drop != NULL)
			{
                LoopDoCheck(q);
				DropWeight	dropNote;
				dropNote.iType = convert_int(QueryXmlAttr(Drop,"DropType"));
				dropNote.DropId = QueryXmlAttr(Drop,"ConfigID");
				int Weight = convert_int(QueryXmlAttr(Drop,"Weight"), 1);
				dropNote.iWeightBegin = weight_sum;
				weight_sum += Weight;
				dropNote.iWeightEnd = weight_sum;

				box.vec_drop.push_back(dropNote);

				Drop = Drop->next_sibling("Drop");
			}
			box.WeightSum = weight_sum;

			m_pAISystem->m_mapBoxInfo.insert(make_pair(box.configId, box));
			pNode = pNode->next_sibling("Property");
		}

		delete[] pfilebuff;
	}
	catch(parse_error& e)
	{
		::extend_warning(pKernel, "[Error]LoadBoxConfig: load xml error:");
		::extend_warning(pKernel, res_path.c_str());
		::extend_warning(pKernel, e.what());
		return false;
	}
	return true;
}

// 等级属性包配置
bool AISystem::LoadLevelPackage(IKernel *pKernel)
{
	// 配置文件路径
	std::string s_path_name = pKernel->GetResourcePath();
	s_path_name += NPC_LEVEL_PACKAGE_FILE;

	CXmlFile xmlfile(s_path_name.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string s_info = "Not Exist " + s_path_name;
		::extend_warning(LOG_ERROR, s_info.c_str());
		return false;
	}

	m_pAISystem->m_NpcLevelPackageVec.clear();

	const int n_sec_count = (int)xmlfile.GetSectionCount();
	m_pAISystem->m_NpcLevelPackageVec.reserve(n_sec_count);

	LoopBeginCheck(a);
	for (int i = 0; i < n_sec_count; i++)
	{
		LoopDoCheck(a);
		const char *p_sec = xmlfile.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(p_sec))
		{
			continue;
		}

		NpcLevelPackage cfg;
		cfg.s_config_id = xmlfile.ReadString(p_sec, "ConfigID", "");
		cfg.n_lower_level = xmlfile.ReadInteger(p_sec, "LowerLevel", 0);
		cfg.n_upper_level = xmlfile.ReadInteger(p_sec, "UpperLevel", 0);
		cfg.n_package_id = xmlfile.ReadInteger(p_sec, "PackageID", 0);

		m_pAISystem->m_NpcLevelPackageVec.push_back(cfg);
	}

	return true;
}

//-------------------------------------查询配置-----------------------------------------//


// 获取护送NPC路径信息
bool AISystem::GetConvoyPoint(IKernel* pKernel, const PERSISTID& self, const char* npcId)
{

	std::map<std::string, std::vector<Point> >::iterator iter = m_pAISystem->m_mapConvoyStep.find(npcId);
	if (iter == m_pAISystem->m_mapConvoyStep.end())
	{
		return false;
	}

	std::vector<Point> stepVec = (*iter).second;
	if (stepVec.empty())
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	IRecord* pPatrolRecord = pSelfObj->GetRecord("PatrolPointRec");
	if (pPatrolRecord == NULL)
	{
		return false;
	}

	//清除表数据
	pPatrolRecord->ClearRow();

	LoopBeginCheck(u);
	for (int i = 0; i < (int)stepVec.size(); i++)
	{
		LoopDoCheck(u);
		float pointX = stepVec[i].x;
		float pointY = stepVec[i].y;
		float pointZ = stepVec[i].z;

		CVarList var;
		var << pointX << pointZ << 1000;
		pPatrolRecord->AddRowValue(-1, var);

	}

	return true;
}

// 获取截路NPC信息
bool AISystem::GetInterceptNpc(IKernel* pKernel,const PERSISTID& self,const char* npcId,int step,CVarList& args)
{
	std::map<std::string,std::vector<Intercept>>::iterator iter = m_pAISystem->m_mapIntercept.find(npcId);
	if (iter == m_pAISystem->m_mapIntercept.end())
	{
		return false;
	}
	std::vector<Intercept> stepVec = (*iter).second;
	if (stepVec.empty())
	{
		return false;
	}
    LoopBeginCheck(v);
	for (int i = 0; i < (int)stepVec.size(); i++)
	{
        LoopDoCheck(v);
		int tempId = stepVec[i].id; 
		if (tempId == step)
		{
			std::string interceptNpc = stepVec[i].npcId;
			int num = stepVec[i].num;
			float monsterX = stepVec[i].monsterX;
			float monsterZ = stepVec[i].monsterZ;
			args << interceptNpc << num << monsterX << monsterZ;
		}
	}

	return true;
}

// 获取寻路NPC
bool AISystem::GetPatrolNpcPoint(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	IRecord* pPatrolRecord = pSelfObj->GetRecord("PatrolPointRec");
	if (pPatrolRecord == NULL)
	{
		return false;
	}

	pPatrolRecord->ClearRow();

	const char * npcID = pKernel->GetConfig(self);
	std::map<std::string,std::vector<PatrolPos>>::iterator iter = m_pAISystem->m_mapPatrolInfo.find(npcID);
	if (iter == m_pAISystem->m_mapPatrolInfo.end())
	{
		return false;
	}
	std::vector<PatrolPos>& stepVec = (*iter).second;
	if (stepVec.empty())
	{
		return false;
	}
    LoopBeginCheck(w);
	for (int i = 0; i < (int)stepVec.size(); i++)
	{
        LoopDoCheck(w);
		float PosX = stepVec[i].PosX;
		float PosZ = stepVec[i].PosZ;
		int stop_time = stepVec[i].iRestTime;
		CVarList args;
		args << PosX << PosZ << stop_time;
		pPatrolRecord->AddRowValue(-1, args);
	}

	return true;
}

bool AISystem::GetSkillConfig(IKernel* pKernel, const PERSISTID& self, const char* skillID, NpcSkill& npcSkill)
{
    std::map<std::string,NpcSkill>::iterator iter = m_pAISystem->m_mapSkillConfig.find(skillID);
    if (iter == m_pAISystem->m_mapSkillConfig.end())
    {
        return false;
    }
    npcSkill = iter->second;
    return true;
}

// 获取怪物技能
bool AISystem::InsertNpcSkill(IKernel* pKernel,const PERSISTID& self,const char* skillStr)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return false;
    }

	CVarList skillList;

    std::map<std::string, NpcSkillPackage>::iterator pakIter = m_pAISystem->m_mapSkillPackage.find(skillStr);
    if(pakIter == m_pAISystem->m_mapSkillPackage.end())
    {
        return false;
    }
    NpcSkillPackage &skillPackage = pakIter->second;

	// 处理出生后立即使用的技能
	if (!StringUtil::CharIsNull(skillPackage.bornskill.c_str()))
	{
		PERSISTID skill = FightInterfaceInstance->AddSkill(pKernel, self, skillPackage.bornskill.c_str());
		if (!skill.IsNull())
		{
			FightInterfaceInstance->UseSkill(pKernel, self, skill, CVarList() << self);
		}
	}

    skillList << skillPackage.normalSkill.c_str();
    if(skillPackage.randCount > 0 && int(skillPackage.skillVec.size()) >= skillPackage.randCount)
    {
        bool ownSkills_b = pSelf->FindAttr("OwnSkills");
        std::string ownSkills = "";
        //打乱
        random_shuffle(skillPackage.skillVec.begin(), skillPackage.skillVec.end());
        LoopBeginCheck(x);
        for(int i = 0; i < skillPackage.randCount; ++i)
        {
            LoopDoCheck(x);
            skillList << skillPackage.skillVec[i].c_str();
            if(ownSkills_b)
            {
                ownSkills += skillPackage.skillVec[i].c_str();
                ownSkills += ",";
            }
        }
        if(ownSkills_b)
        {
            //拥有的技能，给客户端看（普通技能除外）
            pSelf->SetString("OwnSkills", ownSkills.c_str());
        }
    }

	IRecord *npcSkillRec = pSelf->GetRecord(NPC_SKILL_REC);
    if(npcSkillRec == NULL)
    {
        return false;
    }
    LoopBeginCheck(y);
	for (int i=0;i<(int)skillList.GetCount();i++)
	{
        LoopDoCheck(y);
		std::string skillID = skillList.StringVal(i);

		std::map<std::string,NpcSkill>::iterator iter = m_pAISystem->m_mapSkillConfig.find(skillID);
		if (iter != m_pAISystem->m_mapSkillConfig.end())
		{
			NpcSkill npcSkill = (*iter).second;

			CVarList list;
			list << npcSkill.skillID << npcSkill.initWeighting;
            npcSkillRec->AddRowValue(-1, list);
		}
	}
	return true;
}

//获得神秘宝箱掉落
bool AISystem::GetBoxDeadDrop(const std::string &configId, DropWeight &drop)
{
	MAP_BOXWEIGHT::iterator iter = m_pAISystem->m_mapBoxInfo.find(configId);
	if (iter == m_pAISystem->m_mapBoxInfo.end())
	{
		return false;
	}
	//获取随机数
	BoxWeight &box = iter->second;
	int nRandom = util_random_int(box.WeightSum);

	//遍历符合条件区间对应掉落
	VEC_DROPWEIGHT::iterator iter2 = box.vec_drop.begin();
	VEC_DROPWEIGHT::iterator iter_end = box.vec_drop.end();
    LoopBeginCheck(z);
	for (; iter2 != iter_end; ++iter2)
	{
        LoopDoCheck(z);
		if (iter2->iWeightBegin <= nRandom
			&& iter2->iWeightEnd > nRandom)
		{
			drop.iType = iter2->iType;		//类型
			drop.DropId = iter2->DropId;	//配置id
		}
	}

	return true;
}

//在目标点范围内寻找一个随机点
bool AISystem::RandomFindAroundPos(float& fOutX, float& fOutZ)
{
	//2、 随机一个距离
	float fRadios =  util_random_float(AI_PARTOL_RANDOM_SPACE);

	//3、 随机角度
	float fOutO = util_random_float(PI2);
	float fDeltaX = fRadios * FMSIN(fOutO);
	float fDeltaZ = fRadios * FMCOS(fOutO);

	//4、 计算最终的随机坐标
	fOutX = fOutX + fDeltaX;
	fOutZ = fOutZ + fDeltaZ;

	return true;
}

// 查询小队配置
const BattleTeam* AISystem::QueryTeamConfig(int nTeamId)
{
	const BattleTeam* pFind = NULL;
	int nSize = (int)m_vecBattleTeamConfig.size();
	LoopBeginCheck(r);
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(r);
		const BattleTeam* pData = &m_vecBattleTeamConfig[i];
		if (pData != NULL && pData->nTeamId == nTeamId)
		{
			pFind = pData;
			break;
		}
	}

	return pFind;
}

// 查询小队队形
const BattleTeamFormation* AISystem::QueryTeamFormation(const int scene_id)
{
	BattleTeamFormation temp(scene_id);
	BattleTeamFormationVec::const_iterator find_it = find(
		m_BattleTeamFormationVec.begin(),
		m_BattleTeamFormationVec.end(),
		temp);
	if (m_BattleTeamFormationVec.end() == find_it)
	{
		return NULL;
	}

	return &(*find_it);
}