//------------------------------------------------------------------------------
// 文件名:      BufferModuleFuns.cpp
// 内  容:      可是表信息处理、buffer事件处理。
// 说  明:
// 创建日期:    2014年11月4日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "BufferModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/Interface/PropertyInterface.h"
#include "../SystemFunctionModule/StaticDataQueryModule.h"
#include "../Define/StaticDataDefine.h"
#include "utils/string_util.h"
//#include "BufferDecayLogic.h"
#include "../SystemFunctionModule/CoolDownModule.h"
#include "Define/Fields.h"

//添加某个BUFFER
bool BufferModule::InterAddBuffer(IKernel* pKernel, const PERSISTID& self,
								  const PERSISTID& sender, const PERSISTID& buffer)
{
	//校验保护
	if (!pKernel->Exists(sender))
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pBufferObj)
	{
		return false;
	}

	//获取BUFFER容器
	PERSISTID container = GetBufferContainer(pKernel, self);
	if (!pKernel->Exists(container))
	{
		::extend_warning(LOG_WARNING,
			"[BufferModule::InterAddBuffer] the role '%d-%d' BufferContainer is not exists.",
			self.nIdent, self.nSerial);
		return false;
	}

	const char* szBufferConfigID = pKernel->GetConfig(buffer);
	if (StringUtil::CharIsNull(szBufferConfigID))
	{
		return false;
	}

	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szBufferConfigID);
	if (NULL == pBuffBaseData)
	{
		return false;
	}

	bool bDeadClear = pBuffBaseData->GetDeadClear() > 0 ? true : false;
	//对象已死亡，且此BUFFER属死亡后清除类型，不能添加
	int nDead = 0;
	FPropertyInstance->GetDead(pSelfObj,nDead);
	if (nDead > 0 && bDeadClear)
	{
		return false;
	}

	// 免疫控制类buff处理
	if(pSelfObj->QueryInt("ImmunoControlBuff") > 0 && BUFFAFFECT_CONTROL == pBuffBaseData->GetAffectType())
	{
		return false;
	}

	// 侠客释放的buff,相当于主人释放的
	PERSISTID attacker = get_pet_master(pKernel, sender);
	//===以下不是瞬时的BUFFER，有持续时间的
	// 初始化buffer数据
	if (!InitBufferData(pKernel, self, attacker, buffer))
	{
		return false;
	}

	// 处理替换
	if (!DoBufferReplace(pKernel, self, container, buffer))
	{
		return false;
	}

	//存放到BUFFER容器中
	if (!pKernel->Place(buffer, container))
	{
		::extend_warning(LOG_TRACE, 
			"[BufferModule::InterAddBuffer] BUFFER %s cant place to BufferContainer.", 
			pKernel->GetConfig(buffer));
		return false;
	}

	// 只有不同玩家之间加buff才需要衰减
// 	if (TYPE_PLAYER == pSelfObj->GetClassType() && TYPE_PLAYER == pKernel->Type(attacker) && self != attacker)
// 	{
// 		// 处理buff时间衰减
// 		int nRealTime = pBufferObj->QueryInt(FIELD_PROP_LIFE_TIME);
// 		if(BufferDecayLogicSingleton::Instance()->ComputeBuffRealTime(pKernel, self, pBufferObj, pBuffBaseData, nRealTime) && nRealTime > 0)
// 		{
// 			// 设置Buffer的生命时间
// 			BPropertyInstance->SetLifeTime(pBufferObj, nRealTime);
// 		}
// 
// 		// 设置Buffer的生命时间
// 		BPropertyInstance->SetLifeTime(pBufferObj, nRealTime);
// 	}
	
	// 设置对象的buff效果
	ChangeBuffDisplay(pKernel, self, szBufferConfigID);

	//激活
	BufferModule::ActiveBuffer(pKernel, self, buffer);

	return true;
}

// 初始化buffer属性数据
bool BufferModule::InitBufferData(IKernel* pKernel, const PERSISTID& self, 
								  const PERSISTID& sender, const PERSISTID& buffer)
{
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (NULL == pSenderObj)
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pBufferObj)
	{
		return false;
	}

	if (pBufferObj->FindAttr("Skill"))
	{
		PERSISTID curSkill;
		FPropertyInstance->GetCurSkill(pSenderObj,curSkill);
		if (pKernel->Exists(curSkill))
		{
			BPropertyInstance->SetSkill(pBufferObj,curSkill);
		}
	}
	BPropertyInstance->SetSender(pBufferObj,sender);
	BPropertyInstance->SetCounter(pBufferObj,0);
	BPropertyInstance->SetTimer(pBufferObj,0);//正式添加buffer对象
	BPropertyInstance->SetAddTime(pBufferObj,::util_get_time_64());//添加的时间

	return true;
}

//清除某个BUFFER
bool BufferModule::InterRemoveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer, int iRemoveReason)
{
	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pBufferObj || NULL == pSelf)
	{
		return false;
	}

	//恢复属性
	int nBufferActive=0;
	BPropertyInstance->GetActive(pBufferObj,nBufferActive);
	if (nBufferActive == 1)
	{
		BufferModule::DeactiveBuffer(pKernel, self, buffer);
	}
	// DeactiveBuffer()的调用可能触发事件导致buffer会被提前删除
	if (pKernel->Exists(buffer))
	{
		pKernel->DestroySelf(buffer);
	}

	// 更新buff显示效果
	ChangeBuffDisplay(pKernel, self, "");
	return true;
}

//瞬发状态的效果
int BufferModule::InstantActiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer)
{
	// 处理击活事件
	m_pInstance->BufferEventEffect(pKernel, BUFFEVENT_ACTIVE, self, self, buffer);

	return 0;
}

//启动状态的效果
bool BufferModule::ActiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pBufferObj)
	{
		return false;
	}

	const char* szBufferConfigID = pKernel->GetConfig(buffer);
	if (StringUtil::CharIsNull(szBufferConfigID))
	{
		return false;
	}

	BPropertyInstance->SetActive(pBufferObj, 1);

	// 向添加buff的对象发消息
	//pKernel->Command(buffer, self, CVarList() << COMMAND_ADD_BUFFER_TO_OBJECT);

	// 处理击活事件
	m_pInstance->BufferEventEffect(pKernel, BUFFEVENT_ACTIVE, self, self, buffer);

	// buffer对象可能在执行事件后被删除，在此需要验证buffer对象是否合法
	if (!pKernel->Exists(buffer))
	{
		return true;
	}

	//添加计时的BUFFER
	if (!pKernel->FindHeartBeat(buffer, "BufferModule::H_BufferUpdate"))
	{
		pKernel->AddHeartBeat(buffer, "BufferModule::H_BufferUpdate", BUFFER_HEART_INTERVAL);
	}

	return true;
}

//关闭状态的效果
bool BufferModule::DeactiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(buffer))
	{
		return false;
	}

	const char* szBufferConfigID = pKernel->GetConfig(buffer);
	if (StringUtil::CharIsNull(szBufferConfigID))
	{
		return false;
	}

	//添加计时的BUFFER
	if (pKernel->Exists(buffer))
	{
		if (pKernel->FindHeartBeat(buffer, "BufferModule::H_BufferUpdate"))
		{
			pKernel->RemoveHeartBeat(buffer, "BufferModule::H_BufferUpdate");
		}
	}

	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pBufferObj)
	{
		return false;
	}
	
	int nBufferActive = 0;
	BPropertyInstance->GetActive(pBufferObj,nBufferActive);
	if (nBufferActive == 1)
	{
		BPropertyInstance->SetActive(pBufferObj,0);
		m_pInstance->BufferEventEffect(pKernel, BUFFEVENT_DEACTIVE, self, self, buffer);
	}

	return true;
}

//计时或计次时的效果
bool BufferModule::TimerBuffer(IKernel* pKernel, const PERSISTID& self, 
							   const PERSISTID& eventsender, const PERSISTID& buffer)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(buffer))
	{
		return false;
	}

	// 处理计时心跳
	m_pInstance->BufferEventEffect(pKernel, BUFFEVENT_TIMER, self, eventsender, buffer);
	//伤害完成后，BUFFER本身可能已经被删除
	if (!pKernel->Exists(buffer))
	{
		return false;
	}

	return true;
}

//替换相关逻辑
bool BufferModule::DoBufferReplace(IKernel* pKernel, const PERSISTID& self, const PERSISTID& container, const PERSISTID& buffer)
{
	if (!pKernel->Exists(self) ||!pKernel->Exists(container) ||  !pKernel->Exists(buffer))
	{
		return false;
	}

	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(pKernel->GetConfig(buffer));
	
	// 获取替换信息
	int iReplaceCategory = 0 ,iReplaceType = 0;
	if (NULL != pBuffBaseData)
	{
		iReplaceCategory = pBuffBaseData->GetReplaceCategory();
		iReplaceType = pBuffBaseData->GetReplaceType();
	}
	// 迭代所有BUFFER
	unsigned int it;
	PERSISTID temp_buffer = pKernel->GetFirst(container, it);
	LoopBeginCheck(c)
	while (pKernel->Exists(temp_buffer))
	{
		LoopDoCheck(c)
		IGameObj* pTmpBufferObj = pKernel->GetGameObj(temp_buffer);
		// 获取已存在的BUFF分类
		int tempReplaceCategory = 0;
		const BuffBaseData* pBuffMainData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(pKernel->GetConfig(temp_buffer));
		if (NULL == pTmpBufferObj || NULL == pBuffMainData)
		{
			temp_buffer = pKernel->GetNext(container, it);
			// 直接跳过 
			continue;
		}

		tempReplaceCategory = pBuffMainData->GetReplaceCategory();
		// 只需要检查分类号相同的buffer
		if (tempReplaceCategory == iReplaceCategory)
		{ 
			// 直接替换
			if (BUFFER_REPLACE_DIRECT == iReplaceType)
			{
				// 删除现有的同类buff
				BufferModule::InterRemoveBuffer(pKernel, self, temp_buffer, BUFFER_REMOVE_REPLACE);
			}
			else if(BUFFER_REPLACE_REFRESH_TIME == iReplaceType) // 刷新buffer时间
			{
				BPropertyInstance->SetCounter(pTmpBufferObj,0);
				BPropertyInstance->SetTimer(pTmpBufferObj,0);
				BPropertyInstance->SetAddTime(pTmpBufferObj,::util_get_time_64());
				return false;
			}
			else if (BUFFER_REPLACE_NOT_REPLACE == iReplaceType)
			{
				return false;
			}
		} 

		temp_buffer = pKernel->GetNext(container, it);

	} // end of while (...
	return true;
}

//获得BUFFER的剩余时间，单位为ms
int BufferModule::GetBufferLeaveTime(int timeType, int lifeTime, int exitTime, time_t addTime)
{
	int leave_time = 0;
	switch (timeType)
	{
	case BUFFER_TIMETYPE_ONLINE:
		{
			//只计算在线时间
			leave_time = lifeTime - exitTime;
			break;
		}
	case BUFFER_TIMETYPE_ALLTIME:
		{
			//计算全部时间
			time_t now = ::util_get_time_64();
			leave_time = int(lifeTime - (now - addTime));
			break;
		}
	default:
		break;
	}
	return leave_time;
}

//处理事件
void BufferModule::BufferEventEffect(IKernel* pKernel,
									 const EBufferEventType type,
									 const PERSISTID& self,
									 const PERSISTID& sender,
									 const PERSISTID& buffer,
									 const IVarList& params)
{
	if (!pKernel->Exists(self))
	{
		return ;
	}
	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pBufferObj)
	{
		return ;
	}

	const char* szBufferConfigID = pKernel->GetConfig(buffer);
	if (StringUtil::CharIsNull(szBufferConfigID))
	{
		return ;
	}

	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szBufferConfigID);
	if (NULL == pBuffBaseData)
	{
		return;
	}

	//效果包
	const StringArray& effectid = pBuffBaseData->GetBuffEffect();
	EEventExecType func_id = EVENT_EXEC_INVALID;//事件编号(EEventExecType)
	int nEventType = 0;//事件类型（eBufferEventType）

	size_t nEffectCount = effectid.GetCount();
	LoopBeginCheck(r);
	for(size_t i = 0; i < nEffectCount; ++i)
	{
		LoopDoCheck(r);
		//事件类型，目标类型，事件编号
		const BuffEffectData* pBuffEffectData = SkillDataQueryModule::m_pInstance->GetBuffEffectProp(effectid.StringVal(i));
		if (NULL == pBuffEffectData)
		{
			continue;
		}
		nEventType = pBuffEffectData->GetEventType();
		if (nEventType != type)
		{
			continue;
		}
		func_id = (EEventExecType)pBuffEffectData->GetFuncNo();
		PERSISTID target = sender;
		CVarList msgParams;
		CVarList configparams;
		pBuffEffectData->GetParams().ConvertToVarList(configparams);
		SkillEventManager::m_pInstance->ExecuteOperate(pKernel,
			func_id,      // 功能操作编号
			configparams,      // 功能参数
			self, buffer, 0, type, CVarList() << target << params, msgParams);
	}

    return ;
}

// 生成显示buff的可视属性BuffDisplayID
bool BufferModule::ChangeBuffDisplay(IKernel* pKernel, const PERSISTID& self, const std::string& szBufferConfigID)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	//获取BUFFER容器
	PERSISTID container = GetBufferContainer(pKernel, self);
	if (!pKernel->Exists(container))
	{
		::extend_warning(LOG_WARNING,
			"[BufferModule::InterAddBuffer] the role '%d-%d' BufferContainer is not exists.",
			self.nIdent, self.nSerial);
		return false;
	}

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}
	// 删除buff的时候
	if (szBufferConfigID.empty())
	{
		unsigned int it = 0;
		fast_string strDisplay = "";
		PERSISTID buffer = pKernel->GetFirst(container, it);
		LoopBeginCheck(d)
		while (pKernel->Exists(buffer))
		{
			LoopDoCheck(d)
			const char* szTmpConfig = pKernel->GetConfig(buffer);
			const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szTmpConfig);
			if (NULL == pBuffBaseData)
			{
				return false;
			}
			// 找到需要显示buff的效果
			const char* strBuffDisplay = pBuffBaseData->GetBuffDisplay();
			if (!StringUtil::CharIsNull(strBuffDisplay))
			{
				strDisplay = strBuffDisplay;
				break;
			}
			
			buffer = pKernel->GetNext(container, it);
		}
		// 没找到显示的buff效果,strDisplay为""
		pSelf->SetString("BuffDisplayID", strDisplay.c_str());
	}
	else
	{
		// 添加buff的时候
		const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szBufferConfigID.c_str());
		if (NULL == pBuffBaseData)
		{
			return false;
		}
		// 是否存在buff效果id
		const char* strBuffDisplay = pBuffBaseData->GetBuffDisplay();
		if (StringUtil::CharIsNull(strBuffDisplay))
		{
			return false;
		}
		pSelf->SetString("BuffDisplayID", strBuffDisplay);
	}
	
	return true;
}

// buff的直接替换处理
bool BufferModule::DoBuffDirectReplace(IKernel* pKernel, const PERSISTID& curbuff, const PERSISTID& oldbuff)
{
	return true;
}

// 按照类型清除buff
int BufferModule::RemoveBuffByClearType( IKernel* pKernel, const PERSISTID &self, int nClearType ) 
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
	unsigned int uit;
	PERSISTID buffercontainer = m_pInstance->GetBufferContainer(pKernel, self);
	if (!pKernel->Exists(buffercontainer))
	{
		::extend_warning(LOG_TRACE,
			"[BufferModule::OnCommandBeKill] the role '%d-%d' not have BufferContainer.",
			self.nIdent, self.nSerial);
		return 0;
	}

	// 移除目标身上死亡后需要移除的BUFFER
	PERSISTID buffer = pKernel->GetFirst(buffercontainer, uit);
	LoopBeginCheck(e)
	while (pKernel->Exists(buffer))
	{
		LoopDoCheck(e)
		const char* szBufferConfigID = pKernel->GetConfig(buffer);
		if (StringUtil::CharIsNull(szBufferConfigID))
		{
			buffer = pKernel->GetNext(buffercontainer, uit);
			continue;
		}

		int nReason = 0;
		if (IsNeedClearBuff(pKernel, szBufferConfigID, nClearType, nReason))
		{
			m_pInstance->InterRemoveBuffer(pKernel, self, buffer, nReason);
		}

		buffer = pKernel->GetNext(buffercontainer, uit);
	}

	return 1;
}

// 是否需要清除buff
bool BufferModule::IsNeedClearBuff(IKernel* pKernel, const char* strBufferConfigID, int nClearType, int& outReason)
{
	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(strBufferConfigID);
	if (NULL == pBuffBaseData)
	{
		return false;
	}
	bool bClear = false;

	switch (nClearType)
	{
	case BUFFER_SWITCH_CLEAR:
		bClear = pBuffBaseData->GetChangeSceneClear() > 0 ? true : false;
		outReason = BUFFER_REMOVE_CLEAR;
		break;
	case BUFFER_OFFLINE_CLEAR:
		bClear = pBuffBaseData->GetOffLineClear() > 0 ? true : false;
		outReason = BUFFER_REMOVE_OFFLINE;
		break;
	case BUFFER_DEAD_CLEAR:
		bClear = pBuffBaseData->GetDeadClear() > 0 ? true : false;
		outReason = BUFFER_REMOVE_DEAD;
		break;
	}
	
	return bClear;
}