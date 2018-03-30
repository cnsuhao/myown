//--------------------------------------------------------------------
// 文件名:      AIBasicAction.cpp
// 内  容:      AI系统行为处理
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIFunction.h"
#include "AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include "FsGame/Interface/FightInterface.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/geometry_func.h"
#include "../../CommonModule/PathFindingModule.h"

#include "utils/cache/CacheHelper.hpp"
const char* const FIND_PATH_POINT_LIST_RECORD = "FindPathPointList";

AISystem * AIFunction::m_pAISystem = NULL;

bool AIFunction::Init(IKernel * pKernel)
{
	m_pAISystem = (AISystem*)pKernel->GetLogicModule("AISystem");
	Assert(m_pAISystem!=NULL);

	// 注册移动回调
	REGISTER_MOTION_CALLBACK(AIFunction::MotionCallBackFunction);
	return true;
}

bool AIFunction::Shut(IKernel* pKernel)
{
	return true;
}

//巡逻移动到下一个目标点(直接 PatrolPointRec寻路移动)
bool AIFunction::StartPatrolToPoint(IKernel* pKernel, const PERSISTID& self, float posX, float posZ, bool bNeedDynaicFindPath/* = true*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	if (pSelfObj->QueryInt("Dead") > 0)
	{
		return false;
	}
	
	// 寻路方式
	MotionMethod method = bNeedDynaicFindPath ? MOTION_METHOD_ALL : MOTION_METHOD_NORMAL_STATIC;

	// 进行寻路
	return MotionModule::m_pMotionModule->MotionToPoint(pKernel, self, posX, posZ, 0.0f, 0.0f, "AIFunction::MotionCallBackFunction", method);
}

//跑向一个目标点(通过寻路)
int	AIFunction::StartMoveToPoint(IKernel * pKernel , const PERSISTID & self , float toPosX, float toPosZ)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return AI_RT_IGNORE;
    }

    if(pSelf->QueryInt("Dead") > 0)
    {
        return AI_RT_IGNORE;
    }

	if ( pSelf->QueryInt("CantMove") > 0 )
	{
		return AI_RT_IGNORE;
	}
	
	//清空表
	IRecord* pRecord = pSelf->GetRecord(FIND_PATH_POINT_LIST_RECORD);
	if (pRecord == NULL)
	{
		return AI_RT_IGNORE;
	}
	pRecord->ClearRow();

	//调用寻找路接口 
	CVarList FindPathResult;
	MontionPos pos_sef;
	MontionPos pos_target;
	MotionHelper::ParseMotionPosEx(pKernel, pos_sef, pSelf->GetPosiX(), pSelf->GetPosiZ());
	MotionHelper::ParseMotionPosEx(pKernel, pos_target, toPosX, toPosZ);

	//执行寻路
	len_t newX, newY, newZ;

	//路径点
	CVarList pathList;
	if (pKernel->TraceLineWalk(1, pos_sef.pos_x_, pos_sef.pos_y_, pos_sef.pos_z_, toPosX, toPosZ, newX, newY, newZ))
	{
		//短距离直接可以走
		FindPathResult << toPosX;
		FindPathResult << toPosZ;
	}
	else
	{
		int point_count = 0;
		if (MotionHelper::FindPathToTargets_Static(pKernel, self, pos_sef, pos_target, point_count, pathList))
		{
			if ( point_count > 0 )
			{
				LoopBeginCheck(a);
				for (int j = 1; j < point_count; ++j)
				{
					LoopDoCheck(a);
					float temp_x = pathList.FloatVal(j*3);
					float temp_z = pathList.FloatVal(j*3+2);
					int tempIndex = pKernel->GetGridIndexByPos(temp_x,temp_z);
					if (tempIndex == pos_sef.grid_)
					{
						continue;
					}
					FindPathResult << temp_x << temp_z;
				}
			}
		}
	}

	int count = (int)FindPathResult.GetCount();
	//寻路失败
	if ((int)count <= 1)
	{
		return AI_RT_FAIL;
	}

    IRecord *findPaihPointRec = pSelf->GetRecord(FIND_PATH_POINT_LIST_RECORD);
    if(findPaihPointRec == NULL)
    {
        return AI_RT_FAIL;
    }
    LoopBeginCheck(b);
	for (int nPoint = 0; nPoint < count; nPoint += 2)
	{
        LoopDoCheck(b);
		int row = findPaihPointRec->AddRow(-1);
		if (row < 0)
			return AI_RT_FAIL;
		findPaihPointRec->SetFloat(row, AI_FINDPATH_POS_X, FindPathResult.FloatVal(nPoint));
		findPaihPointRec->SetFloat(row, AI_FINDPATH_POS_Z, FindPathResult.FloatVal(nPoint+1));
	}

	//从第一点个开始
	if(StartFindPathMove(pKernel, self) == AI_RT_FAIL)
	{
		return AI_RT_FAIL;
	}

	return AI_RT_SUCCESS;
}

// 追逐目标
int	AIFunction::StartMoveToObject(IKernel * pKernel , const PERSISTID & self, const PERSISTID& target, 
								  float minDistance, float maxDistance)
{
	if (!pKernel->Exists(self))
	{
		return AI_RT_FAIL;
	}

	if (!MotionModule::m_pMotionModule->MotionToObject(pKernel, self, target, minDistance, maxDistance, "AIFunction::MotionCallBackFunction"))
	{
		return AI_RT_FAIL;
	}

	return AI_RT_SUCCESS;
}

// 移动回调
int AIFunction::MotionCallBackFunction(IKernel* pKernel, const PERSISTID& self, int result)
{
	MotionCallBack(pKernel, self, PERSISTID(), CVarList() << result);

	return 0;
}

//跑动状态的OnMotion
int AIFunction::MotionCallBack(IKernel * pKernel, const PERSISTID & self, const PERSISTID& sender, const IVarList& args)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

    if(!pSelf->FindAttr("AITemplate"))
    {
        return 0;
    }

	int result = (int)MOTION_RESULT_SUCCEED;

	if (args.GetCount() > 0 && args.GetType(0) == VTYPE_INT)
	{
		result = args.IntVal(0);
	}

	// 路径移动成功，继续下一个路径点
	if (result == MOTION_RESULT_SUCCEED)
	{
		IRecord* pRecord = pSelf->GetRecord(FIND_PATH_POINT_LIST_RECORD);
		if (pRecord != NULL && pRecord->GetRows() > 0)
		{
			pRecord->RemoveRow(0);
			
			// 表中存在没有走的路径
			if (pRecord->GetRows() > 0)
			{
				return AIFunction::StartFindPathMove(pKernel, self);
			}
		}
	}

	// 存在攻击目标,立即转向目标
	if (result == MOTION_RESULT_SUCCEED || result == MOTION_RESULT_STOP)
	{
		PERSISTID aitarget = pSelf->QueryObject("AITargetObejct");
		if (pKernel->Exists(aitarget))
		{
			//转向
			float angle = pKernel->Angle(self, aitarget);
			float dstX = pKernel->GetPosiX(aitarget);
			float dstZ = pKernel->GetPosiZ(aitarget);
			float dorient = util_dot_angle(pSelf->GetPosiX(), pSelf->GetPosiZ(), dstX, dstZ);
			if (fabs(angle) > 0.01f)
			{
				pKernel->Locate(self, pSelf->GetPosiX(), pSelf->GetPosiY(), pSelf->GetPosiZ(), dorient);
			}
		}
	}

	return AISystem::OnActionEnd(pKernel, self, result);
}

//移动到目标点(提前保证寻路表中存在数据）
int	AIFunction::StartFindPathMove(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    IRecord *findPaihPointRec = pSelfObj->GetRecord(FIND_PATH_POINT_LIST_RECORD);
    if(findPaihPointRec == NULL)
    {
        return AI_RT_FAIL;
    }

	//获得第一个行动点
	len_t posX = findPaihPointRec->QueryFloat(0, AI_FINDPATH_POS_X);
	len_t posZ = findPaihPointRec->QueryFloat(0, AI_FINDPATH_POS_Z);
	
	if (!MotionModule::m_pMotionModule->MotionToPoint(pKernel, self, posX, posZ, 0.0f, 0.0f, "AIFunction::MotionCallBackFunction"))
	{
		return AI_RT_FAIL;
	}

	return AI_RT_SUCCESS;
}


int AIFunction::InitSkill(IKernel * pKernel,const PERSISTID & self,const char* skillStr)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//插入数据
	m_pAISystem->InsertNpcSkill(pKernel,self,skillStr);

	//初始化技能
    IRecord *pSkillRec = pSelfObj->GetRecord("boss_skill_rec");
    if(pSkillRec == NULL)
    {
        return 0;
    }
	int rows = pSkillRec->GetRows();

    LoopBeginCheck(c);
	for (int i = 0;i < rows;i ++)
	{
        LoopDoCheck(c);
		const char* skillId = pSkillRec->QueryString(i, 0);

		FightInterfaceInstance->UpdateSkillData(pKernel,self,skillId, 1);
	}

	return rows;
}

//生成怪物路径点
bool AIFunction::CreateNpcPath(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 获取寻路表
    IRecord *pPatrolPointRec = pSelfObj->GetRecord("PatrolPointRec");
    if(pPatrolPointRec == NULL)
    {
        return false;
    }

	// 清空表
	pPatrolPointRec->ClearRow();

	// 成功的话将路径点加入表格中
	CVarList rowVal;
	len_t PosX,PosZ;
	int iLastGridIndex = -2;
    LoopBeginCheck(f);
	int args_num = (int)args.GetCount();
	for (int i = 0; i < args_num; i += 3)
	{
        LoopDoCheck(f);
		//转换成格子坐标
		PosX = args.FloatVal(i);
		PosZ = args.FloatVal(i + 2);
		int iGridIndex = pKernel->GetGridIndexByPos( PosX, PosZ );
		pKernel->GetPosByIndex( iGridIndex, PosX, PosZ );

		// 随机一个巡逻点的停留时间(ms) 5-10秒
		int stop_time = 1000 * (5 + util_random_int(5));
		rowVal.Clear();
		rowVal << PosX << PosZ << stop_time;
		if (iLastGridIndex != iGridIndex)
		{
			pPatrolPointRec->AddRowValue(-1, rowVal);
		}

		iLastGridIndex = iGridIndex;
	}
	return true;
}

//计算当前回出生点（FindPathPointList）路程总时间(单位是毫秒)
int AIFunction::CountFindPathPointListTime(IKernel *pKernel, const PERSISTID &self)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

    float time = 0;
    float speed = pSelf->QueryFloat("RunSpeed");
    if(speed <= 0)
    {
        return 0;
    }
    float startX = pSelf->GetPosiX();
    float startZ = pSelf->GetPosiZ();

    IRecord *pFindPathPointRec = pSelf->GetRecord(FIND_PATH_POINT_LIST_RECORD);
    int length = pFindPathPointRec->GetRows();
    if(length > 0)
    {
        float tempX = 0;
        float tempZ = 0;
        float findLength = 0;
        LoopBeginCheck(g);
        for(int i = 0; i < length; ++i)
        {
            LoopDoCheck(g);
            tempX = pFindPathPointRec->QueryFloat(i, AI_FINDPATH_POS_X);
            tempZ = pFindPathPointRec->QueryFloat(i, AI_FINDPATH_POS_Z);
            findLength += util_dot_distance(startX, startZ, tempX, tempZ);
            startX = tempX;
            startZ = tempZ;
        }
        time = findLength / speed;
    }
    else
    {
        float bornX = pSelf->QueryFloat("BornX");
        float bornZ = pSelf->QueryFloat("BornZ");
        time = util_dot_distance(startX, startZ, bornX, bornZ) / speed;
    }
   
    //4舍5入
    return int((time * 1000) + 0.5);
}