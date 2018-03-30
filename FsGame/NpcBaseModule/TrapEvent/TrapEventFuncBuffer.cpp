//--------------------------------------------------------------------
// 文件名:      TrapEventFuncBuffer.h
// 内  容:      陷阱触发Buff事件逻辑
// 说  明:		
// 创建日期:    2015年07月13日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventManagerModule.h"
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventFuncBuffer.h"

#include "FsGame/Define/BufferDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"

#include "FsGame/Interface/FightInterface.h"

//单人BUFF
//SPRINGEVENT_SINGLE_BUFF = 3, 
bool TrapEventSingleBuff::Execute(IKernel* pKernel, const PERSISTID& self,
								  const PERSISTID& target, const IVarList& vEventParams)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(target))
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//判定玩家是否存在
	IGameObj * pTargetObj = pKernel->GetGameObj(target);
	if (pTargetObj == NULL)
	{
		return false;
	}

	//获取表数据
	IRecord * SelfRecord = pSelfObj->GetRecord("SpringInfoRec");
	if (SelfRecord == NULL)
	{
		return false;
	}

	//表数据满则不做以下流程处理
	if( SelfRecord->GetRows() >= SelfRecord->GetRowMax() )
	{
		return false;
	}

	//获取陷阱归属人
	PERSISTID owner = pSelfObj->QueryObject("Master");
	PERSISTID sender = self;
	if (pKernel->Exists(owner))
	{
		sender = owner;
	}

	//获取信息
	const char * BuffID = vEventParams.StringVal(0);

	//查看是否已经增加过此目标
	int iRow = SelfRecord->FindObject(0, target);
	if (iRow >= 0)
	{
		return false;
	}
    int delayTime = 0;
    if (pSelfObj->FindAttr("DelayTime"))
    {
        delayTime = pSelfObj->QueryInt("DelayTime");
    }
    
    if (delayTime > 0)
    {
        ADD_COUNT_BEAT(pKernel, self, "TrapNpcModule::HB_Delay_Spring", delayTime, 1);
    }
    else
    {
        //添加玩家BUFF
        FightInterfaceInstance->AddBuffer(pKernel, target, sender, BuffID);

        //将其加入触发者列表
        CVarList arg_list;
        arg_list << target << BuffID;
        SelfRecord->AddRowValue(-1, arg_list);
    }
	return true;
}

// 初始化
bool TrapEventMutiBuff::OnInit(IKernel* pKernel)
{
	DECL_HEARTBEAT(TrapEventMutiBuff::HB_Delay_Spring);

	return true;
}
// 延迟心跳
int TrapEventMutiBuff::HB_Delay_Spring(IKernel* pKernel, const PERSISTID& self, int silce)
{
	OnAddBuff(pKernel, self);
	return 0;
}

//群体BUFF
//SPRINGEVENT_MUTI_BUFF = 4, 
bool TrapEventMutiBuff::Execute(IKernel* pKernel, const PERSISTID& self,
								const PERSISTID& target, const IVarList& vEventParams)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	//判定玩家是否存在
	IGameObj * pTargetObj = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || pTargetObj == NULL)
	{
		return 0;
	}

	int delayTime = 0;
	if (pSelfObj->FindAttr("DelayTime"))
	{
		delayTime = pSelfObj->QueryInt("DelayTime");
	}

	if (delayTime > 0)
	{
		ADD_COUNT_BEAT(pKernel, self, "TrapEventMutiBuff::HB_Delay_Spring", delayTime, 1);
	}
	else
	{
		OnAddBuff(pKernel, self);
	}
	return true;
}

void TrapEventMutiBuff::OnAddBuff(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	//获取表数据
	IRecord * SelfRecord = pSelfObj->GetRecord("SpringInfoRec");
	if (SelfRecord == NULL)
	{
		return;
	}

	//获取陷阱归属人
	PERSISTID owner = pSelfObj->QueryObject("Master");
	PERSISTID sender = self;
	if (pKernel->Exists(owner))
	{
		sender = owner;
	}

	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (pSenderObj == NULL)
	{
		return;
	}

	//获取信息
	const char * BuffID = pSelfObj->QueryString("EventValue");
	float iEventRange = pSelfObj->QueryFloat("EventRange");
	if (iEventRange <= 0)
	{
		return;
	}

	CVarList CResult;
	pKernel->GetAroundList(self, iEventRange, 0, 0, CResult);
	int nCount = static_cast<int>(CResult.GetCount());

	//遍历设置安全区内账户对应的属性
	LoopBeginCheck(a);
	for (int i = 0; i < nCount; ++i)
	{
		LoopDoCheck(a);
		//验证对象有效性(范围陷阱可能在塔或者友方Npc附近)
		PERSISTID obj = CResult.ObjectVal(i);
		IGameObj* pObj = pKernel->GetGameObj(obj);
		if (pObj == NULL)
		{
			continue;
		}

		if (!pKernel->Exists(obj) || pSenderObj->QueryInt(FIELD_PROP_CAMP) == pObj->QueryInt(FIELD_PROP_CAMP) || pObj->QueryInt(FIELD_PROP_CANT_BE_ATTACK) > 0)
		{
			continue;
		}

		//查看是否已经增加过此目标
		int iRow = SelfRecord->FindObject(0, obj);
		if (iRow >= 0)
		{
			return;
		}
		//添加玩家BUFF
		FightInterfaceInstance->AddBuffer(pKernel, obj, sender, BuffID);

		//将其加入触发者列表
		CVarList arg_list;
		arg_list << obj << BuffID;
		SelfRecord->AddRowValue(-1, arg_list);
	}
}
