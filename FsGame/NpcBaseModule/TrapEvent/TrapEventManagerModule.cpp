//--------------------------------------------------------------------
// 文件名:       TrapEventManager.cpp
// 内  容:       陷阱触发事件包管理
// 说  明:
// 创建日期:    2015年07月13日
// 创建人:		  
//    :       
//--------------------------------------------------------------------
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventManagerModule.h"
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventBase.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "../../Define/CommandDefine.h"
#include "../../Define/GameDefine.h"
#include "../../Define/ClientCustomDefine.h"
#include "../../Define/ServerCustomDefine.h"
#include "utils/custom_func.h"

TrapEventManagerModule* TrapEventManagerModule::m_pInstance = NULL;

// 初始化
bool TrapEventManagerModule::Init(IKernel* pKernel)
{
    m_pInstance = this;


    LoopBeginCheck(a);
	for (int i = 0;i < SPRINGEVENT_MAX;++i)
	{
        LoopDoCheck(a);

		m_pEventContainer[i] = NULL;
	}

    InitEventExecutes(pKernel);


    LoopBeginCheck(b);
	for (int i = 0;i < SPRINGEVENT_MAX;++i)
	{
        LoopDoCheck(b);

		TrapEventBase* pEvent = m_pEventContainer[i];
		if (NULL != pEvent)
		{
			pEvent->OnInit(pKernel);
		}
	}

    return true;
}

// 关闭
bool TrapEventManagerModule::Shut(IKernel* pKernel)
{

    LoopBeginCheck(c);
	for (int i = 0;i < SPRINGEVENT_MAX;++i)
	{
        LoopDoCheck(c);

		TrapEventBase* pEvent = m_pEventContainer[i];
		if (NULL != pEvent)
		{
			SAFE_DELETE(pEvent);
		}
	}
    return true;
}

// 事件注册
bool TrapEventManagerModule::RegistTrapEvent(const SpringEvent func_id)
{
	if (func_id < SPRINGEVENT_INVALID || func_id >= SPRINGEVENT_MAX)
	{
		return false;
	}

	TrapEventBase* pEventBase = TrapEventBase::CreateTrapEvent(func_id);
	if (NULL == pEventBase)
	{
		return false;
	}

	m_pEventContainer[func_id] = pEventBase;

	return true;
}

// 注册各个不同技能事件函数
bool TrapEventManagerModule::InitEventExecutes(IKernel* pKernel)
{
	// 通用

    LoopBeginCheck(d);
	for (int i = SPRINGEVENT_INVALID + 1;i < SPRINGEVENT_MAX;++i)
	{
        LoopDoCheck(d);

		RegistTrapEvent((SpringEvent)i);
	}
    return true;
}

// 处理单个执行操作
bool TrapEventManagerModule::ExecuteOperate(IKernel* pKernel, 
									  const PERSISTID& self,
									  const PERSISTID& object,
									  const SpringEvent funcID)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || !pKernel->Exists(object))
	{
		return false;
	}

	if (funcID <= SPRINGEVENT_INVALID || funcID >= SPRINGEVENT_MAX)
	{
		return false;
	}

	if (NULL == m_pEventContainer[funcID])
	{
		return false;
	}

	//参数设定
	CVarList arg_list;
	arg_list << pSelfObj->QueryString("EventValue")
		     << pSelfObj->QueryFloat("EventRange")
		     << pSelfObj->QueryInt("EventCount");

	//执行事件
	m_pEventContainer[funcID]->Execute(pKernel, self, object, arg_list);

	return true;
}
