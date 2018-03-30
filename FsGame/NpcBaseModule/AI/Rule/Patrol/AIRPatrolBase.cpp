//--------------------------------------------------------------------
// 文件名:      AIRPatrolBase.cpp
// 内  容:      巡逻规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIRPatrolBase.h"
#include "FsGame/NpcBaseModule/AI/AISystem.h"
#include "FsGame/NpcBaseModule/AI/AIFunction.h"
#include "utils/util_func.h"
AIRPatrolBase::AIRPatrolBase()
{
	m_ruleType = AI_RULE_PATROL;
	m_subRuleCode = AIRS_PATROL_BASE;
}

int AIRPatrolBase::DoRule(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, const IVarList & args, AITemplateBase &templateWrap)
{
	if (!pKernel->Exists(self))
		return AI_RT_SUCCESS;

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

    IRecord *pPatrolPointRec = pSelf->GetRecord("PatrolPointRec");
    if(pPatrolPointRec == NULL)
    {
        return AI_RT_NULL;
    }

    if(pSelf->QueryInt("Dead") > 0)
    {
        return AI_RT_SUCCESS;
    }

	//巡逻状态才巡逻
 	if (pSelf->QueryInt("CurState") != AI_STATE_PATROL)
	{
		return AI_RT_IGNORE;
	}

    //多加的判断防止不触发
    //检测周围玩家进行一轮触发
	float SpringRange = AISystem::GetSpringRange(pSelf);
    CVarList arg_list;
    CVarList temp;
    pKernel->GetAroundList(self, SpringRange, TYPE_PLAYER, 10, arg_list);
    LoopBeginCheck(a);
    for ( size_t i = 0; i < arg_list.GetCount(); ++i )
    {
        LoopDoCheck(a);
        PERSISTID player = arg_list.ObjectVal(i);
        if ( pKernel->Exists(player) )
        {
            if(templateWrap.OnProcess(pKernel, AI_RULE_SPRING, self, player, temp) == AI_RT_SUCCESS)
            {
                return AI_RT_IGNORE;
            }
        }
    }

    //不能移动处理
    if ( pSelf->QueryInt("CantMove") > 0 )
    {
        return AI_RT_IGNORE;
    }

    //当前还在巡逻走动中
	int sub_state = pSelf->QueryInt("SubState");
	if ( sub_state == AI_PATROL_WALK)
	{
		return AI_RT_IGNORE;
	}
	else if (sub_state == AI_PATROL_WAIT)
	{
		//没有巡逻点
		int rows = pPatrolPointRec->GetRows();
		if (rows < 1)
		{
			return AI_RT_IGNORE;
		}

		int step = pSelf->QueryDataInt("PatrolStep");
		if (step < 0)
		{
			//第一次巡逻
			pSelf->SetDataInt("PatrolStep", 0);
			pSelf->SetDataInt("PatrolRestTime", 0);

			//继续巡逻,PatrolStep++
			return NextPatrol(pKernel, self, templateWrap);	
		}

		unsigned int restTime = (unsigned int)pSelf->QueryDataInt("PatrolRestTime");
		if (restTime >= 0)
		{
			unsigned int newRestTime = (unsigned int)restTime + (unsigned int)args.IntVal(0);
			if (step >= rows)
				step = 0;

			//休息完毕
			if (newRestTime >= (unsigned int)pPatrolPointRec->QueryInt(step, AI_PATROL_REC_POS_TIME))
			{
				//继续巡逻,PatrolStep++
				NextPatrol(pKernel, self, templateWrap);	
				//清除休息标记
				pSelf->SetDataInt("PatrolRestTime", 0);
			}
			else
			{
				//继续休息
				pSelf->SetDataInt("PatrolRestTime", (int)newRestTime);
			}
		}
	}

	return AI_RT_SUCCESS;
}

//巡逻下一点,PatrolStep++
int AIRPatrolBase::NextPatrol(IKernel *pKernel, const PERSISTID &self, const AITemplateBase &templateWrap)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 0;
    }

    IRecord *pPatrolPointRec = pSelf->GetRecord("PatrolPointRec");
    if(pPatrolPointRec == NULL)
    {
        return AI_RT_NULL;
    }

	int rows = pPatrolPointRec->GetRows();
	if (rows < 1)
	{
		return AI_RT_IGNORE;
	}

	//取得当前巡逻点
	int step = pSelf->QueryDataInt("PatrolStep");
	if (step >= rows)
		step = 0;

	//取得巡逻模式
	int mode = pSelf->QueryInt("PatrolMode");
	if (mode == AI_PATROL_MODE_RANDOM)
	{
		//随机模式
		int n = util_random_int(rows);
		//找一个不同于patrolStep点
		if(n == step)
		{
			if(step >= (rows - 1))
				n = 0;
			else
				n ++;
		}
		step = n;
	}
	else if (mode == AI_PATROL_MODE_BACK)
	{
		//往返模式
		//设置返回标记
		if (pSelf->QueryDataInt("PatrolBack") == 0)
		{
			//向前走
			step ++;
			if (step >= rows)
			{
				//倒数第点往回走
				step = __max(rows - 2, 0);
				//设置返回标记
				pSelf->SetDataInt("PatrolBack", 1);	
			}
		}
		else
		{
			//向回走
			step --;
			if (step < 0)
			{
				//第2点开始走
				step = 1;
				//删除返回标记
				pSelf->SetDataInt("PatrolBack", 0);	//
			}
		}
	}
	else if(mode == AI_PATROL_MODE_ROUND)
	{
		//环形走模式
		step ++;
		//从0点开始往回走
		if (step >= rows)
			step = 0;
	}

	if(step < 0 || step >= rows)
		step = 0;

	//向下一个巡逻点走去
	pSelf->SetDataInt("PatrolStep", step);
	float x = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_X);
	float z = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_Z);

	if (AIFunction::StartPatrolToPoint(pKernel, self, x, z, false))
	{
		pSelf->SetInt("SubState", AI_PATROL_WALK);
	}

	return AI_RT_SUCCESS;
}