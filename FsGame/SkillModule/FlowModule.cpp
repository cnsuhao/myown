//--------------------------------------------------------------------
// 文件名:       Server\FsRoomLogic\skill_module\FlowModule.cpp
// 内  容:        流程管理系统
// 说  明:
// 创建日期:    2008/01/28
// 创建人:       冯红文
//    :       
//--------------------------------------------------------------------

#include "FsGame/skillmodule/FlowModule.h"

#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "FsGame/define/CommandDefine.h"
#include "FsGame/define/ServerCustomDefine.h"
#include "FsGame/Define/FlowCategoryDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "public/VarList.h"
#include <cassert>



// 技能命中心跳检测误差 ms
#define SKILL_HIT_TIMER_OFFSET  50 

// 最小心跳时间
#define MIN_SKILL_HIT_HEART_TIME 300

FlowModule* FlowModule::m_pInstance = NULL;

//初始化
bool FlowModule::Init(IKernel* pKernel)
{
    m_pInstance = this;

    pKernel->AddEventCallback("player", "OnLeaveScene", FlowModule::OnCommandBreakFlow);
    pKernel->AddIntCommandHook("player", COMMAND_BEKILL, FlowModule::OnCommandBreakFlow);
    pKernel->AddIntCommandHook("NormalNpc", COMMAND_BEKILL, FlowModule::OnCommandBreakFlow);

    //心跳声明
    DECL_HEARTBEAT(FlowModule::H_Flow_Prepare);
    DECL_HEARTBEAT(FlowModule::H_Flow_Lead);
	DECL_HEARTBEAT(FlowModule::H_Flow_Hit);

    return true;
}

//释放
bool FlowModule::Shut(IKernel* pKernel)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////


//
int FlowModule::OnRequestMove(IKernel* pKernel, const PERSISTID& self,
                              const PERSISTID& sender, const IVarList& args)
{
    // 移动模式
    int mode = args.IntVal(0);
    if (mode != MMODE_STOP)
    {
        m_pInstance->BreakAnyFlow(pKernel, self);
    }

    return 0;
}

//被杀后相应---打断流程
int FlowModule::OnCommandBreakFlow(IKernel* pKernel, const PERSISTID& self,
                                   const PERSISTID& sender, const IVarList& args)
{
    m_pInstance->BreakAnyFlow(pKernel, self);
    return 0;
}

//流程准备心跳
int FlowModule::H_Flow_Prepare(IKernel* pKernel, const PERSISTID& self, int slice)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return 0;
    }
    //流程分类
    int iFlowCategory = m_pInstance->QueryCategory(pKernel, self);

    //调用回调函数
    FLOW_RESPONSE_FUNC preparefunc = (FLOW_RESPONSE_FUNC)m_pInstance->GetPrepareFunc(pKernel, self);
    if (preparefunc != NULL)
    {
        if (!preparefunc(pKernel, self))
        {
            m_pInstance->BreakFlow(pKernel, self, iFlowCategory);
            return 0;
        }
    }

    int flowtype = pSelfObj->QueryInt(FIGHTER_FLOW_TYPE);

    switch (flowtype)
    {
        case FLOW_TYPE_DEFAULT:
            //普通流程
            m_pInstance->BeginHit(pKernel, self);
            break;
        case FLOW_TYPE_LEAD:
            //引导流程
            m_pInstance->BeginLead(pKernel, self);
            break;
        default:
            break;
    }

    return 1;
}

//流程开始引导心跳
int FlowModule::H_Flow_Lead(IKernel* pKernel, const PERSISTID& self, int slice)
{
    //调用回调函数
    FLOW_RESPONSE_FUNC leadfunc = (FLOW_RESPONSE_FUNC)m_pInstance->GetLeadFunc(pKernel, self);
    if (leadfunc != NULL)
    {
        if (!leadfunc(pKernel, self))
        {
            int iFlowCategory = m_pInstance->QueryCategory(pKernel, self);
            m_pInstance->BreakFlow(pKernel, self, iFlowCategory);
            return 0;
        }
    }

    H_Flow_Hit(pKernel, self, 0);
    return 1;
}

//流程开始打击心跳
int FlowModule::H_Flow_Hit(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	int iFlowCategory = m_pInstance->QueryCategory(pKernel, self);
	if (iFlowCategory <= FLOW_CATEGORY_NONE)
	{
		::extend_warning(LOG_WARNING,
			"[FlowModule::H_Flow_Hit] role '%d-%d' FlowCategory is empty.",
			self.nIdent, self.nSerial);
		return 0;
	}

	//调用回调函数
	FLOW_RESPONSE_FUNC hitfunc = m_pInstance->GetHitFunc(pKernel, self);
	if (hitfunc != NULL)
	{
		if (!hitfunc(pKernel, self))
		{
			::extend_warning(LOG_TRACE,
				"[FlowModule::H_Flow_Hit] self '%d-%d' callback func return false break flow.",
				self.nIdent, self.nSerial);

			m_pInstance->BreakFlow(pKernel, self, iFlowCategory);
			return 0;
		}

		// 在执行上一步回调后如果self不在了, 则直接返回
		if (!pKernel->Exists(self))
		{
			return 0;
		}
	}

	int flowtype = pSelfObj->QueryInt("FlowType");

	//命中次数到0了，结束流程
	int iHitTimes = pSelfObj->QueryInt(FIGHTER_FLOW_HIT_TIMES);
	iHitTimes -= 1;
	pSelfObj->SetInt(FIGHTER_FLOW_HIT_TIMES, iHitTimes);
	if (iHitTimes <= 0)
	{
		//命中次数到0了，结束流程
		m_pInstance->Finish(pKernel, self);
	}
	else
	{
		//还需要继续命中流程
		if (FLOW_TYPE_DEFAULT == flowtype)//普通流程
		{
			m_pInstance->BeginHit(pKernel, self);
		}
	}

    return 1;
}

//注册一个流程分类
bool FlowModule::RegisterFlow(int iFlowCategory,
                              FLOW_RESPONSE_FUNC beforepreparefunc, FLOW_RESPONSE_FUNC preparefunc,
                              FLOW_RESPONSE_FUNC beforeleadfunc, FLOW_RESPONSE_FUNC leadfunc, FLOW_RESPONSE_FUNC hitfunc,
                              FLOW_RESPONSE_FUNC breakfunc/*, FLOW_RESPONSE_FUNC pausefunc*/, FLOW_RESPONSE_FUNC finishfunc)
{
    std::map<int, FlowModule::S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it != m_mapflowfunc.end())
    {
        return false;
    }

    FlowModule::S_FLOW_FUNC stemp;
    stemp.beforepreparefunc = beforepreparefunc;
    stemp.preparefunc = preparefunc;
    stemp.beforeleadfunc = beforeleadfunc;
    stemp.leadfunc = leadfunc;
    stemp.hitfunc = hitfunc;
    stemp.breakfunc = breakfunc;
    stemp.finishfunc = finishfunc;

    m_mapflowfunc.insert(std::make_pair(iFlowCategory, stemp));

    return true;
}

//开始一个流程
bool FlowModule::BeginFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory,
                           int preparetime, int hittime, const char* flowtextid)
{
    return BeginFlow(pKernel, self, iFlowCategory, preparetime, CVarList() << hittime, flowtextid);
}

//开始一个流程
bool FlowModule::BeginFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory,
                           int preparetime, const IVarList& hittime, const char* flowtextid)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    if (FlowModule::IsAnyFlowing(pKernel, self))
    {
        //提示已存在一个流程，不能同时存在多个流程
        return false;
    }

    if (FlowModule::IsFlowing(pKernel, self, iFlowCategory))
    {
        return false;
    }

    IRecord* pHitRecord = pSelfObj->GetRecord(FIGHTER_FLOW_HITTIME_REC);
    if (NULL == pHitRecord)
    {
        return false;
    }
    pHitRecord->ClearRow();
    int iRecordMaxRow = pHitRecord->GetRowMax();
    int iHitTimesCount = (int)hittime.GetCount();
    int count = iRecordMaxRow < iHitTimesCount ? iRecordMaxRow : iHitTimesCount;

    ::extend_warning(LOG_TRACE,
                     "[FlowModule::BeginFlow] role '%d-%d'.",
                     self.nIdent, self.nSerial);

    pSelfObj->SetInt(FIGHTER_FLOW_PREPARE_TIME, preparetime);
    pSelfObj->SetInt(FIGHTER_FLOW_HIT_TIMES, count);
    pSelfObj->SetInt(FIGHTER_FLOW_LEAD_TIME, 0);
    pSelfObj->SetInt(FIGHTER_FLOW_LEAD_SEP_TIME, 0);
    pSelfObj->SetInt(FIGHTER_FLOW_CATEGORY, iFlowCategory);

	LoopBeginCheck(a)
    for (int i = 0; i < count; i++)
    {
		LoopDoCheck(a)
        int iHitTime = hittime.IntVal(i);
        pHitRecord->AddRowValue(-1, CVarList() << i << iHitTime);
    }

    pSelfObj->SetInt(FIGHTER_FLOW_HITS, 0);//当前命中次数(准备开始命中心跳时，取flow_hittime_rec表中哪一行的时间数值)
    pSelfObj->SetInt(FIGHTER_FLOW_TYPE, FLOW_TYPE_DEFAULT);//普通流程

	BeginPrepare(pKernel,self);
    return true;
}

//开始一个引导流程
bool FlowModule::BeginLeadFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory, int preparetime, int leadlifetime, int leadseptime, int hittime)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

    if (leadseptime == 0)
    {
        return false;
    }

    // 当前同时仅有一个流程存在，因此只要存在，就不能再开始
    if (FlowModule::IsAnyFlowing(pKernel, self))
    {
        //提示已存在一个流程，不能同时存在多个流程
        return false;
    }

    if (FlowModule::IsFlowing(pKernel, self, iFlowCategory))
    {
        return false;
    }

    ::extend_warning(LOG_TRACE,
                     "[FlowModule::BeginLeadFlow] role '%d-%d'.",
                     self.nIdent, self.nSerial);

    IRecord* pHitRecord = pSelfObj->GetRecord(FIGHTER_FLOW_HITTIME_REC);
    if (NULL == pHitRecord)
    {
        return false;
    }

    pHitRecord->ClearRow();
    int iRecordMaxRow = pHitRecord->GetRowMax();
    int hitcount =  leadlifetime / leadseptime;
    int count = iRecordMaxRow < hitcount ? iRecordMaxRow : hitcount;

    // 记录流程信息在自已身上
    pSelfObj->SetInt(FIGHTER_FLOW_PREPARE_TIME, preparetime);
    pSelfObj->SetInt(FIGHTER_FLOW_HIT_TIMES, count);
    pSelfObj->SetInt(FIGHTER_FLOW_LEAD_TIME, leadlifetime);
    pSelfObj->SetInt(FIGHTER_FLOW_LEAD_SEP_TIME, leadseptime);
    pSelfObj->SetInt(FIGHTER_FLOW_CATEGORY, iFlowCategory);

    // 引导流程
	LoopBeginCheck(b)
    for (int i = 0; i < count; i++)
    {   
		LoopDoCheck(b)
        // 引导的流程的命中时间都为0
        pHitRecord->AddRowValue(-1, CVarList() << i << 0);// 去除hittime
    }
    pSelfObj->SetInt(FIGHTER_FLOW_HITS, 0);//当前命中次数(准备开始命中心跳时，取flow_hittime_rec表中哪一行的时间数值)
    pSelfObj->SetInt(FIGHTER_FLOW_TYPE, FLOW_TYPE_LEAD);//引导流程

    //开始准备期
    BeginPrepare(pKernel, self);
    return true;
}

//强制结束当前流程
bool FlowModule::BreakFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    if (!FlowModule::IsFlowing(pKernel, self, iFlowCategory))
    {
        return false;
    }

    //清除心跳
    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Prepare"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Prepare");
    }

    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Lead"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Lead");
    }

    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Hit"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Hit");
    }

    //清除流程标志
    if (pSelfObj->FindAttr(FIGHTER_FLOWING))
    {
        pSelfObj->SetInt(FIGHTER_FLOWING, 0);
    }

    IRecord* pRecord = pSelfObj->GetRecord(FIGHTER_FLOW_MUTIL_HIT_TIMER_REC);
    if (NULL != pRecord)
    {
        pRecord->ClearRow();
    }


    //调用函数
    FLOW_RESPONSE_FUNC breakfunc = FlowModule::GetBreakFunc(pKernel, self);
    if (breakfunc != NULL)
    {
        breakfunc(pKernel, self);
    }
    return true;
}

//强制结束任意当前滚程
bool FlowModule::BreakAnyFlow(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = QueryCategory(pKernel, self);
    return BreakFlow(pKernel, self, iFlowCategory);
}

//是否在某一个特定的流程中
bool FlowModule::IsFlowing(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    int iCurCategory = QueryCategory(pKernel, self);

    if (iCurCategory == iFlowCategory)
    {
        IGameObj* pSelfObj = pKernel->GetGameObj(self);
        if (NULL == pSelfObj)
        {
            return false;
        }

        if (pSelfObj->QueryInt(FIGHTER_FLOWING) > 0)
        {
            return true;
        }
    }

    return false;
}

//是否在一个流程中
bool FlowModule::IsAnyFlowing(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }
    if (pSelfObj->QueryInt(FIGHTER_FLOWING) > 0)
    {
        return true;
    }

    return false;
}

// 释放在蓄力中
bool FlowModule::IsStoraging(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    int iCurCategory = QueryCategory(pKernel, self);
    if (iCurCategory == iFlowCategory)
    {
        return pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Storage");
    }

    return false;
}

//是否在一个流程的准备期中
bool FlowModule::IsPreparing(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    int iCurCategory = QueryCategory(pKernel, self);

    if (iCurCategory == iFlowCategory)
    {
        return pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Prepare");
    }

    return false;
}

//是否在一个流程的引导期中
bool FlowModule::IsLeading(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    int iCurCategory = QueryCategory(pKernel, self);

    if (iCurCategory == iFlowCategory)
    {
        return pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Lead");
    }

    return false;
}

//是否在一个流程的命中期中
bool FlowModule::IsHiting(IKernel* pKernel, const PERSISTID& self, int iFlowCategory)
{
    int iCurCategory = QueryCategory(pKernel, self);

    if (iCurCategory == iFlowCategory)
    {
        if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Hit"))
        {
            return true;
        }
    }

    return false;
}

//获得流程分类
int FlowModule::QueryCategory(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return -1;
    }
    return pSelfObj->QueryInt(FIGHTER_FLOW_CATEGORY);
}
//////////////////////////////////////////////////////////////////////////
//开始准备期
void FlowModule::BeginPrepare(IKernel* pKernel, const PERSISTID& self, const char* flowtextid)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return ;
    }

	int64_t now = ::util_get_time_64();
	//开始的时间
	pSelfObj->SetInt64(FIGHTER_FLOW_BEGIN_TIME, now);

	//流程开始中
	if (pSelfObj->FindAttr(FIGHTER_FLOWING))
	{
		pSelfObj->SetInt(FIGHTER_FLOWING, 1);
	}

    //有无准备时间
    int preparetime = pSelfObj->QueryInt(FIGHTER_FLOW_PREPARE_TIME);
    if (preparetime > 0)
    {
        FLOW_RESPONSE_FUNC beforepreparefunc = m_pInstance->GetBeforePrepareFunc(pKernel, self);
        if (beforepreparefunc != NULL)
        {
            beforepreparefunc(pKernel, self);
        }

        // 开启定时器
        if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Prepare"))
        {
            pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Prepare");
        }
        pKernel->AddCountBeat(self, "FlowModule::H_Flow_Prepare", preparetime, 1);
    }
    else
    {
        //直接准备结束
        FlowModule::H_Flow_Prepare(pKernel, self, 0);
    }
}

//开始引导期
void FlowModule::BeginLead(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return ;
	}
	int leadlifetime = pSelfObj->QueryInt("FlowLeadTime");
	int leadseptime = pSelfObj->QueryInt("FlowLeadSepTime");
	if (leadseptime == 0)
	{
		return;
	}

	if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Lead"))
		pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Lead");

	FLOW_RESPONSE_FUNC beforeleadfunc = m_pInstance->GetBeforeLeadFunc(pKernel, self);
	if (beforeleadfunc != NULL)
	{
		beforeleadfunc(pKernel, self);
	}

	// 如果无引导生命周期
	if (leadlifetime == -1)
	{
		pKernel->AddHeartBeat(self, "FlowModule::H_Flow_Lead", leadseptime);
	}
	else
	{
		int count = leadlifetime / leadseptime;
		pKernel->AddCountBeat(self, "FlowModule::H_Flow_Lead", leadseptime, count);
	}
}

//开始命中期
void FlowModule::BeginHit(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	IRecord* pHitRecord = pSelfObj->GetRecord(FIGHTER_FLOW_HITTIME_REC);
	if (NULL == pHitRecord)
	{
		return;
	}

	int hits = pSelfObj->QueryInt("FlowHits");
	if (hits < 0 || hits >= pHitRecord->GetRows())
	{
		Finish(pKernel, self);
		return;
	}

	//本次命中心跳的时间
	int hittime = pHitRecord->QueryInt(hits, 1);
	if (hittime <= MIN_SKILL_HIT_HEART_TIME && hittime != 0)
	{
		hittime = MIN_SKILL_HIT_HEART_TIME;
	}

	pSelfObj->SetInt("FlowHits", hits + 1);

	if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Hit"))
		pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Hit");

	if (hittime > 0)
	{
		//添加命中心跳
		pKernel->AddCountBeat(self, "FlowModule::H_Flow_Hit", hittime, 1);
	}
	else
	{
		H_Flow_Hit(pKernel, self, 0);
	}
}


//流程结束
void FlowModule::Finish(IKernel* pKernel, const PERSISTID& self)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return ;
    }

    //获取当前流程
    int iFlowCategory = m_pInstance->QueryCategory(pKernel, self);

    if (!FlowModule::IsFlowing(pKernel, self, iFlowCategory))
    {
        return;
    }

    //清除心跳
    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Prepare"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Prepare");
    }

    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Lead"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Lead");
    }

    if (pKernel->FindHeartBeat(self, "FlowModule::H_Flow_Hit"))
    {
        pKernel->RemoveHeartBeat(self, "FlowModule::H_Flow_Hit");
    }

    //清除流程标志
    if (pSelfObj->FindAttr(FIGHTER_FLOWING))
    {
        pSelfObj->SetInt(FIGHTER_FLOWING, 0);
    }

    IRecord* pRecord = pSelfObj->GetRecord(FIGHTER_FLOW_MUTIL_HIT_TIMER_REC);
    if (NULL != pRecord)
    {
        pRecord->ClearRow();
    }

    //调用函数
    FLOW_RESPONSE_FUNC finishfunc = m_pInstance->GetFinishFunc(pKernel, self);
    if (finishfunc != NULL)
    {
        finishfunc(pKernel, self);
    }

    ::extend_warning(LOG_TRACE,
                     "[FlowModule::Finish] role '%d-%d'.",
                     self.nIdent, self.nSerial);

    return;
}

//////////////////////////////////////////////////////////////////////////
FLOW_RESPONSE_FUNC FlowModule::GetBeforePrepareFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);
    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.beforepreparefunc;
}

//获得流程准备函数
FLOW_RESPONSE_FUNC FlowModule::GetPrepareFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);

    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.preparefunc;
}


FLOW_RESPONSE_FUNC FlowModule::GetBeforeLeadFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);
    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.beforeleadfunc;
}

//获得流程引导函数
FLOW_RESPONSE_FUNC FlowModule::GetLeadFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);

    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.leadfunc;
}

//获得流程打击函数
FLOW_RESPONSE_FUNC FlowModule::GetHitFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);
    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.hitfunc;
}

//获得流程完成函数
FLOW_RESPONSE_FUNC FlowModule::GetFinishFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);

    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.finishfunc;
}


//获得流程打断函数
FLOW_RESPONSE_FUNC FlowModule::GetBreakFunc(IKernel* pKernel, const PERSISTID& self)
{
    int iFlowCategory = FlowModule::QueryCategory(pKernel, self);
    std::map<int, S_FLOW_FUNC>::iterator it = m_mapflowfunc.find(iFlowCategory);
    if (it == m_mapflowfunc.end())
    {
        return NULL;
    }

    return (*it).second.breakfunc;
}
//////////////////////////////////////////////////////////////////////////
