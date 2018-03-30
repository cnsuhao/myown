//--------------------------------------------------------------------
// 文件名:		AIBattleNpc.cpp
// 内  容:		国战NPC处理
// 说  明:		
// 创建日期:	2015年03月17日
// 创建人:		  
//    :	   
//--------------------------------------------------------------------
#include "AISystem.h"

#define BATTLE_TEAM_ROW 2
#define BATTLE_TEAM_COL 2

#define BATTLE_TEAM_NUM 4


// 创建小队
bool AISystem::CreateBattleTeam(IKernel* pKernel, const PERSISTID& self, int iNation, int nTeamId, int iLevel)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 获取队伍配置
	const BattleTeam* pTeamData = m_pAISystem->QueryTeamConfig(nTeamId);
	//获取对应NPC数据
	int iSceneID = pKernel->GetSceneId();
	//国战NPC配置
	std::map<int, BattleNpcStruct>::iterator it = m_pAISystem->m_mapBattleNpcInfo.find(iSceneID);
	if (it == m_pAISystem->m_mapBattleNpcInfo.end() || NULL == pTeamData)
	{
		return false;
	}

	//先玩家身上获取对应的序列号
	const char * TeamName = ("BattleTeamCampRec" + util_int_as_string(pTeamData->nCamp)).c_str();
	IRecord* TeamRec = pSelfObj->GetRecord(TeamName);
	if (TeamRec == NULL)
	{
		return false;
	}

	//添加行数据
	int iAddRow = TeamRec->AddRow(-1);
	if (iAddRow < 0)
	{
		return false;
	}

	//获取对应的阵营数据
	std::map<int, std::map<int, BattlePos>>::iterator it2 = it->second.m_MapBattleNpc.find(pTeamData->nCamp);
	if (it2 == it->second.m_MapBattleNpc.end())
	{
		TeamRec->RemoveRow(iAddRow);
		return false;
	}

	//获取对应序列的坐标数据
	std::map<int, BattlePos>::iterator it3 = it2->second.find(pTeamData->nBornPosIndex);
	if (it3 == it2->second.end())
	{
		TeamRec->RemoveRow(iAddRow);
		return false;
	}

	//分组
	int iGroupId = -1; 
	if (pSelfObj->GetClassType() == TYPE_PLAYER)
	{
		iGroupId = pSelfObj->QueryInt("GroupID");
	}

	//创建队长 //国家，分组，等级
	CVarList args;
	args << CREATE_TYPE_BATTLELEAD_NPC << pTeamData->nMoveType << self << iAddRow << pTeamData->nCamp << pTeamData->nLifeTime;
	//新的中心点坐标
	float PosX = it3->second.fPosX;
	float PosZ = it3->second.fPosZ;
	int iGridIndex = pKernel->GetGridIndexByPos(PosX, PosZ);
	pKernel->GetPosByIndex(iGridIndex, PosX, PosZ);
	
	float fPosY = pKernel->GetMapHeight(PosX, PosZ);
	//先规整化队长坐标
	const PERSISTID LeadNpc = pKernel->CreateObjectArgs("", it->second.BattleLeadNpc.c_str(), 0, PosX, fPosY, PosZ, it3->second.fOrient, args);
	if (!pKernel->Exists(LeadNpc))
	{
		TeamRec->RemoveRow(iAddRow);
		return false;
	}
	IGameObj* pNpc = pKernel->GetGameObj(LeadNpc);
	if (NULL == pNpc)
	{
		return false;
	}

	//设置阵营
 	pNpc->SetInt("Camp", pTeamData->nCamp);

	//国家
	pNpc->SetInt("Nation", iNation);
	//分组
	pNpc->SetInt("GroupID", iGroupId);
	//等级
	pNpc->SetInt("Level", iLevel);

	//设置其行走路径点
	AISystem::m_pAISystem->SetBattleNpcPath(pKernel, LeadNpc, pTeamData->nCamp);
	//进行行走
    AISystem::m_pAISystem->GetAITemlate(pKernel, LeadNpc).BeginPatrol(pKernel, LeadNpc);

	int team_num = BATTLE_TEAM_NUM;

	// 获取队伍队形
	const BattleTeamFormation *formation = m_pAISystem->QueryTeamFormation(pKernel->GetSceneId());
	if (NULL != formation)
	{
		const int team_row = formation->row;
		const int team_col = formation->column;
		team_num = team_row * team_col;
	}

	//创建其他子对象NPC
    LoopBeginCheck(a);
	for (int i = 1; i <= team_num; ++i)
	{
        LoopDoCheck(a);
		args.Clear();
		args << CREATE_TYPE_BATTLE_NPC << pTeamData->nMoveType << LeadNpc << i << iGroupId /*<< self*/;

		//计算其他对象的偏移位置
		float OtherPosX = PosX;
		float OtherPosZ = PosZ;
		m_pAISystem->GetOtherNpcPos(pKernel, PosX, PosZ, i, OtherPosX, OtherPosZ);

		//创建其他玩家NPC
		PERSISTID npc = pKernel->CreateObjectArgs("", it->second.BattleNpc.c_str(), 0, OtherPosX, 
			pKernel->GetMapHeight(OtherPosX, OtherPosZ), OtherPosZ, it3->second.fOrient, args);

		pNpc = pKernel->GetGameObj(npc);
		if (NULL == pNpc)
		{
			continue;
		}
		//设置阵营
 		pNpc->SetInt("Camp", pTeamData->nCamp);

		//国家
		pNpc->SetInt("Nation", iNation);

		//等级
		pNpc->SetInt("Level", iLevel);

#ifndef FSROOMLOGIC_EXPORTS
		//AISystem::m_pCopyOffLineModule->CopyOffLineData(pKernel, npc);
#endif // FSROOMLOGIC_EXPORTS
	}
	return true;
}

//计算其他成员的偏移位置
bool AISystem::GetOtherNpcPos(IKernel* pKernel, float PosX, float PosZ, int iIndex, float& OtherPosX, float& OtherPosZ)
{
	int team_num = BATTLE_TEAM_NUM;
	int team_row = BATTLE_TEAM_ROW;
	int team_col = BATTLE_TEAM_COL;

	// 获取队伍队形
	const BattleTeamFormation *formation = m_pAISystem->QueryTeamFormation(pKernel->GetSceneId());
	if (NULL != formation)
	{
		team_row = formation->row;
		team_col = formation->column;
		team_num = team_row * team_col;
	}

	if (iIndex < 1 || iIndex > team_num)
	{
		return false;
	}
	//获取对应的中心点坐标
	int iGridIndex = pKernel->GetGridIndexByPos(PosX, PosZ);
	//查看对应的行偏移量
	int iDiffRow = (iIndex - 1) % team_row - 1;
	//列偏移
	int iDiffCol = (iIndex - 1) / team_col - 1;

	//查看格子对应的数量
	int iTotalRows = (int)pKernel->GetPathGridRows();
	int iTotalCols = (int)pKernel->GetPathGridCols();
	int iOtherGridIndex = iGridIndex + iDiffRow*team_row + iDiffCol*iTotalRows*team_col;
	if (iOtherGridIndex < 0 || iOtherGridIndex > iTotalRows*iTotalCols)
	{
		return false;
	}

	pKernel->GetPosByIndex(iOtherGridIndex, OtherPosX, OtherPosZ);
	return true;
}

//设置国战行走路径
bool AISystem::SetBattleNpcPath(IKernel* pKernel, const PERSISTID& self, int nCamp)
{
	//获取对象
	IGameObj* selfObj = pKernel->GetGameObj(self);
	if (selfObj == NULL)
	{
		return false;
	}
	
	//查看行走策略
	int iMoveType = selfObj->QueryDataInt("MoveType");
	//获取对应的行路信息
	int iSceneID = pKernel->GetSceneId();
	std::map<int, MoveTypeDef>::iterator it = m_pAISystem->m_mapMoveTypeInfo.find(iSceneID);
	if (it == m_pAISystem->m_mapMoveTypeInfo.end())
	{
		return false;
	}

	//获取阵营数据
	MoveTypeDef::iterator it2 = it->second.find(nCamp);
	if (it2 == it->second.end())
	{
		return false;
	}

	//获取行走策略
	std::map<int, std::vector<MoveTypePos>>::iterator it3 = it2->second.find(iMoveType);
	if (it3 == it2->second.end())
	{
		return false;
	}

	CVarList arg_list;
	//进行路径点的重新添加
	int iCount = (int)it3->second.size();
	if (iCount > 0)
	{
		//攻/守 出发点->从目标点
		len_t dx = it3->second[iCount - 1].fPosX;
		len_t dz = it3->second[iCount - 1].fPosZ;

		len_t fX = pKernel->GetPosiX(self);
		len_t fZ = pKernel->GetPosiZ(self);
		len_t fDis = util_dot_distance(fX, fZ, dx, dz);
		len_t fDis1;
        LoopBeginCheck(b);
		for( int i = 0; i < iCount; ++i )
		{
            LoopDoCheck(b);
			fDis1 = util_dot_distance( dx, dz, it3->second[i].fPosX, it3->second[i].fPosZ );
			if( fDis > fDis1 )
			{
				arg_list << it3->second[i].fPosX
					<< pKernel->GetMapHeight(it3->second[i].fPosX, it3->second[i].fPosZ) //GetFirstWalkHeight(pKernel, it3->second[i].fPosX, it3->second[i].fPosZ)
					<< it3->second[i].fPosZ;
			}
			else
			{
				continue;
			}
		}
	}
	AIFunction::CreateNpcPath(pKernel, self, arg_list);
	return true;
}