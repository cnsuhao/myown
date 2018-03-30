//--------------------------------------------------------------------
// 文件名:      SkillEventFuncCommon.h
// 内  容:      技能通用事件逻辑
// 说  明:		
// 创建日期:    2014年11月25日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "FsGame/SkillModule/inner/SkillEventManager.h"
#include "FsGame/SkillModule/inner/SkillEventFuncCommon.h"

#include "FsGame/Define/CommandDefine.h"

#include "FsGame/Interface/FightInterface.h"
#include "FsGame/SkillModule/FightModule.h"
#include "FsGame/SkillModule/impl/TargetFilter.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/SkillModule/inner/SkillEventExecuteUtil.h"
#include "FsGame/SkillModule/impl/FightDataAccess.h"
#include "FsGame/SkillModule/impl/SkillFlow.h"
#include "FsGame/Interface/PropertyInterface.h" 
#include "FsGame/Define/BufferDefine.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/exptree.h"
#include "FsGame/SkillModule/Impl/SkillDataQueryModule.h"
#include "../../NpcBaseModule/AI/AIDefine.h"
#include "../Impl/SkillStage.h"
#include "../../Define/Classes.h"

//添加BUFF
//EVENT_EXEC_ADDBUFFER = 1,

// 运行技能事件
// 1    :添加BUFFER
// 参数1: 添加的BUFF的ConfigID
bool SkillEventAddBuffer::Execute(IKernel* pKernel, const IVarList& vEventParams, const PERSISTID& self,
	const PERSISTID& skill, const int64_t uuid, const int iEventType,
	const IVarList& params, IVarList& outMsgParams)
{
	IGameObj* pGameObj = pKernel->GetGameObj(skill);
	if (!pKernel->Exists(self) || NULL == pGameObj)
	{
		return false;
	}
	// 检测参数数量
	size_t count = vEventParams.GetCount();
	CHECK_PARAM_COUNT(count, 3);

	const char* strBufferID		= vEventParams.StringVal(1);
	const char* strTimeRatio	= vEventParams.StringVal(2);
	const char* strRate			= vEventParams.StringVal(3);

	// 默认概率为100%
	int nAddBuffRate = BASE_RATE_NUM;
	if (!StringUtil::CharIsNull(strRate))
	{
		nAddBuffRate = convert_int(strRate, 100);
	}

	if (StringUtil::CharIsNull(strBufferID))
	{
		return false;
	}

	PERSISTID target = self;
	
	if (params.GetCount() >= 1 && VTYPE_OBJECT == params.GetType(0))
	{
		target = params.ObjectVal(0);
	}
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (NULL == pTargetObj)
	{
		return false;
	}
#ifdef FSROOMLOGIC_EXPORTS
	// 帮会战建筑不会被技能附加buff
	if (pTargetObj->QueryInt("GVG_IsTower") > 0)
	{
		return false;
	}
#endif 
	// 固定伤害的npc不能被技能加buff
	if (pTargetObj->QueryInt("FixBeDamaged") > 0)
	{
		return false;
	}
	// 计算加buff的概率
	int nRandom = ::util_random_int(BASE_RATE_NUM);
	if (nRandom >= nAddBuffRate)
	{
		return false;
	}

	float fTimeRatio = GetBufferTimeRatio(pKernel, target, strTimeRatio);

	const char* strLifeTime = pKernel->GetConfigProperty(strBufferID, "LifeTime");
	int nLifeTime = StringUtil::StringAsInt(strLifeTime);

	float fBufferTime = (float)nLifeTime * fTimeRatio;

	// 时间为0,就不用加了
	if (util_float_equal_zero(fBufferTime))
	{
		return false;
	}

	// 添加引起buffer时间改变的接口
	if (!FightInterfaceInstance->AddBuffer(pKernel, target, self, strBufferID, (int)fBufferTime))
	{
		return false;
	}

	return true;
}

// 找到当前buff的时间比率
const float SkillEventAddBuffer::GetBufferTimeRatio(IKernel* pKernel, const PERSISTID& self, const char* strBuffTimeRatio)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 1.0f;
	}

	// 没有buff时间区分
	if (StringUtil::CharIsNull(strBuffTimeRatio))
	{
		return 1.0f;
	}

	CVarList args;
	StringUtil::SplitString(args, strBuffTimeRatio, ",");
	int nCount = (int)args.GetCount();
	// 有区分的玩家的时间默认为1,按不同类型获取比率
	float fRatio = 1.0f;
	if (MAX_BUFF_TYPE_NUM != nCount)
	{
		return fRatio; 
	}

	int nClassType = pSelfObj->GetClassType();
	if (TYPE_NPC == nClassType && pSelfObj->FindAttr("AITemplate"))
	{
		int nTemplate = pSelfObj->QueryInt("AITemplate");
		int nBuffIndex = BOSS_ADD_BUFF;
		switch (nTemplate)
		{
		case AI_TEMP_BOSS:
			nBuffIndex = BOSS_ADD_BUFF;
			break;
		case AI_TEMP_NORMAL_FIGHT:
			nBuffIndex = NORMAL_NPC_ADD_BUFF;
			break;
		}
		fRatio = args.FloatVal(nBuffIndex);
	}

	return fRatio;
}