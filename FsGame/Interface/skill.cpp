//------------------------------------------------------------------------------
// 文件名:      skill.cpp
// 内  容:      技能模块公共接口实现
// 说  明:
// 创建日期:    2013年2月22日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------
#include "FsGame/Interface/FightInterface.h"
#include "FsGame/SkillModule/SkillModule.h"
#include "FsGame/SkillModule/SkillToSprite.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/SkillModule/Impl/SkillFlow.h"
#include "FsGame/SkillModule/Impl/SkillAccess.h"
#include "FsGame/SkillModule/impl/SkillDataQueryModule.h"
#include "FsGame/SkillModule/impl/SkillConfigData.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
//#include "FsGame/Interface/aiinterface.h"
//#include "define/npcpropertydefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "../Define/staticdatadefine.h"
#include "../SkillModule/Impl/SkillDefineDataStruct.h"
//#include "../SkillModule/PassiveSkillSystem.h"
#include "utils/string_util.h"
//#include "FsGame/SkillModule/FlySkillModule.h"
#include "Define/CommandDefine.h"
//#include "Define/BattleAbilityDefine.h"
//#include "SkillModule/Impl/PVPRecAttacker.h"
#include "Define/Classes.h"
#include "Define/BattleAbilityDefine.h"
#include "SkillModule/PassiveSkillSystem.h"
#include "SkillModule/PVPRecAttacker.h"

const char* const NPC_PROPERTY_NORMAL_ATTACK_ID = "NormalAttackID";	//临时定义


bool FightInterface::Contains(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID)
{
    if (!pKernel->Exists(self))
    {
        return false;
    }

    if (NULL == szSkillConfigID || StringUtil::CharIsNull(szSkillConfigID))
    {
        return false;
    }

    return SKillToSpriteSingleton::Instance()->Contains(pKernel, self, szSkillConfigID);
}

// 获得某个技能
PERSISTID FightInterface::FindSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID)
{
    return SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, szSkillConfigID);
}

PERSISTID FightInterface::AddSkill(IKernel* pKernel,
	const PERSISTID& self,
	const char* szSkillConfigID)
{
	if (!pKernel->Exists(self))
	{
		return PERSISTID();
	}

	if (NULL == szSkillConfigID || StringUtil::CharIsNull(szSkillConfigID))
	{
		return PERSISTID();
	}

	if (!SKillToSpriteSingleton::Instance()->Contains(pKernel, self, szSkillConfigID))
	{
		if (!SKillToSpriteSingleton::Instance()->AddSkill(pKernel, self, szSkillConfigID))
		{
			return PERSISTID();
		}
	}

	return SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, szSkillConfigID);
}

int FightInterface::UseSkill(IKernel* pKernel,
                             const PERSISTID& self,
                             const char* szSkillConfigID,
                             const IVarList& args)
{
    IGameObj* pGameObj = pKernel->GetGameObj(self);
    if (NULL == pGameObj)
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

	if (NULL == szSkillConfigID || StringUtil::CharIsNull(szSkillConfigID))
    {
        return USESKILL_RESULT_ERR_CONFIG_DATA;
    }

    PERSISTID skill = FindSkill(pKernel, self, szSkillConfigID);
    if (!pKernel->Exists(skill))
    {
        return USESKILL_RESULT_ERR_NOSKILL;
    }

    return SkillModule::m_pInstance->UseSkill(pKernel, self, skill, args);
}


int FightInterface::UseSkill(IKernel* pKernel,
                             const PERSISTID& self,
                             const PERSISTID& skill,
                             const IVarList& args)
{
    if (!pKernel->Exists(self))
    {
        return USESKILL_RESULT_ERR_OTHER;
    }

    return SkillModule::m_pInstance->UseSkill(pKernel, self, skill, args);
}

/**
*@brief  使用技能(暂时只提供NPC使用)
*@param  [IN] pKernel 引擎内核指针
*@param  [IN] self 技能施放者
*@return bool 寻找是否成功
*/
bool FightInterface::ReadyUseRandomSkill(IKernel* pKernel, const PERSISTID& self, float& fSkillMinDis, float& fSkillMaxDis, const char* strSkillId)
{
	fSkillMinDis = 0.0f;
	fSkillMaxDis = 0.0f;
	if (!pKernel->Exists(self))
	{
		return false;
	}

	return SkillModule::m_pInstance->ReadyUseRandomSkill(pKernel, self, fSkillMinDis, fSkillMaxDis, strSkillId);
}

// 打断技能
bool FightInterface::BreakSkill(IKernel* pKernel,
                                const PERSISTID& self,
                                const PERSISTID& breaker,
                                bool bForce,
                                int iBreakReason)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(breaker))
    {
        return false;
    }

    extend_warning(LOG_TRACE,
        "[FightInterface::BreakSkill] self '%d-%d' breaker%d-%d",
        self.nIdent, self.nSerial, breaker.nIdent, breaker.nSerial);

    return SkillFlowSingleton::Instance()->BreakSkill(pKernel, self, breaker, bForce, iBreakReason);
}

//	强制一个技能冷却时间的开始
bool FightInterface::ForceBeginCoolDown(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID)
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || StringUtil::CharIsNull(szSkillConfigID))
	{
		return false;
	}
    PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, szSkillConfigID);
    IGameObj* pSkillObj = pKernel->GetGameObj(skill);
    if (NULL == pSkillObj)
    {
        return false; 
    }

    //开始进行冷却处理
    int iPubCoolTime = 0;
    int iCoolDownTime = 0;
    int iCoolCategory = 0;
    if (!SkillAccessSingleton::Instance()->GetCoolDownTime(pKernel, self, skill, iPubCoolTime, iCoolDownTime, iCoolCategory))
    {
        return false;
    }
    
	CoolDownModule::m_pInstance->BeginCoolDown(pKernel, self, iCoolCategory, iCoolDownTime);
	
	return true;
}

// 判断是否在正在使用技能
int FightInterface::IsSkillUseBusy(IKernel* pKernel, const PERSISTID& self)
{
    if (!pKernel->Exists(self))
    {
        return USESKILL_RESULT_NO_OBJECT;
    }

    return SkillFlowSingleton::Instance()->IsSkillUseBusy(pKernel, self);
}

// 指定技能是否正在冷却
bool FightInterface::IsCoolDown(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
    if (!pKernel->Exists(self) || !pKernel->Exists(skill))
    {
        return false;
    }

    return SkillAccessSingleton::Instance()->IsCoolDown(pKernel, self, skill);
}

bool FightInterface::IsCoolDown(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID)
{
    PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, szSkillConfigID);
    if (!pKernel->Exists(self) || !pKernel->Exists(skill))
    {
        return false;
    }

    return SkillAccessSingleton::Instance()->IsCoolDown(pKernel, self, skill);
}

/*!
* @brief	计算本次技能产生的仇恨值
* @param	[in] pKernel 引擎内核指针
* @param	[in] skill 技能
* @param	[in] nDamValue 伤害值
* @return	int 返回增加的仇恨值
*/
int FightInterface::ComputeHateValue(IKernel* pKernel, const PERSISTID& skill, int nDamValue)
{
	if (!pKernel->Exists(skill))
	{
		return 0;
	}

	return SkillAccessSingleton::Instance()->ComputeHateValue(pKernel, skill, nDamValue);
}

/*!
 * @brief	获取当前使用技能的操控类型 
 * @param	[in] pKernel 引擎内核指针
 * @param	[in] self 施法者
 * @return	int 返回值见USOperationState
 */
// int FightInterface::GetCurUSOperationState(IKernel* pKernel, const PERSISTID& self)
// {
// 	return SkillAccessSingleton::Instance()->GetCurUSOperationState(pKernel, self);
// }
/*!
 * @brief	更新宠物或人物的技能数据及效果
 * @param	self 对象号(可能是宠物也可能是玩家)
 * @param	strSkillId 技能id
 * @param	nSkillLevel 技能等级
 * @param	nSkillSrc  技能来源(被动技能用到)
 * @return	bool
 */
bool FightInterface::UpdateSkillData(IKernel* pKernel, const PERSISTID &self, const char* strSkillId, int nSkillLevel, int nSkillSrc)
{
	if (!pKernel->Exists(self) || NULL == strSkillId || StringUtil::CharIsNull(strSkillId) || nSkillLevel <= 0)
	{
		return false;
	}

	int nSkillType = GetSkillTypeByConfig(pKernel, strSkillId);
	if (USE_SKILL_TYPE == nSkillType)
	{
		PERSISTID skill = FindSkill(pKernel, self, strSkillId);
		if (skill.IsNull())
		{
			skill = AddSkill(pKernel, self, strSkillId);
		}

		IGameObj* pSkillObj = pKernel->GetGameObj(skill);
		if (pSkillObj != NULL)
		{
			pSkillObj->SetInt("Level", nSkillLevel);
		}

		if (pKernel->Type(self) == TYPE_PLAYER)
		{
			// 战斗力更新
			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_SKILL_TYPE);
		}
	}
	else if(PASSIVE_SKILL_TYPE == nSkillType)
	{
		int nSkillId = StringUtil::StringAsInt(strSkillId);
		PassiveSkillSystemSingleton::Instance()->UpdatePassiveSkill(pKernel, self, nSkillId, nSkillLevel, nSkillSrc);

		if (pKernel->Type(self) == TYPE_PLAYER)
		{
			// 战斗力更新
			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PASSIVE_SKILL_TYPE);
		}
	}

	return true;
}

/*!
 * @brief	通过技能id,判定是否为玩家的普通技能 
 * @param	[in] pKernel 引擎内核指针
 * @param	[in] self 施法者
 * @param	[in] strSkillId 技能id
 * @return	int 返回值见USOperationState
 */
// bool FightInterface::IsPlayerNormalAttackSkill(IKernel* pKernel, const PERSISTID& self, const char* strSkillId)
// {
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	if (NULL == pSelfObj || NULL == strSkillId || StringUtil::CharIsNull(strSkillId))
// 	{
// 		return false;
// 	}
// 
// 	return SkillAccessSingleton::Instance()->IsPlayerNormalAttackSkill(pKernel, pSelfObj, strSkillId);
// }

/*!
 * @brief	将所有技能等级设置为最低级 
 * @param	[in] pKernel 引擎内核指针
 * @param	[in] self 施法者
 * @return	void 
 */
// void FightInterface::SetSkillLevelMinLevel(IKernel* pKernel, const PERSISTID& self)
// {
// 	if(!pKernel->Exists(self))
// 	{
// 		return;
// 	}
// 
// 	SKillToSpriteSingleton::Instance()->SetSkillLevelMinLevel(pKernel, self);
// }

/*!
 * @brief	将所有技能等级设置为最低级 
 * @param	[in] pKernel 引擎内核指针
 * @param	[in] self 施法者
 * @param	[in] strSkill 技能名字
 * @return	int 剩余的CD秒数 -1 获取失败 
 */
// int FightInterface::GetSkillLeftCDSeconds(IKernel* pKernel, const PERSISTID& self, const char* strSkill)
// {
// 	if(!pKernel->Exists(self))
// 	{
// 		return -1;
// 	}
// 
// 	return SkillAccessSingleton::Instance()->GetSkillLeftCD(pKernel, self, strSkill);
// }

/*!
 * @brief	获取技能的总CD 
 * @param	[in] pKernel 引擎内核指针
 * @param	[in] self 施法者
 * @param	[in] strSkill 技能名字
 * @return	int 总CD秒数 -1 获取失败 
 */
int FightInterface::QuerySkillCD(IKernel* pKernel, const PERSISTID& self, const char* strSkill)
{
	if(!pKernel->Exists(self))
	{
		return -1;
	}

	// 找到技能对象
	PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, strSkill);
	if (skill.IsNull())
	{
		return -1;
	}

	//开始进行冷却处理
	int iPubCoolTime = 0;
	int iCoolDownTime = 0;
	int iCoolCategory = 0;//分类冷却
	SkillAccessSingleton::Instance()->GetCoolDownTime(pKernel, self, skill, iPubCoolTime, iCoolDownTime, iCoolCategory);

	return iCoolDownTime;
}

#ifndef FSROOMLOGIC_EXPORTS
	/*!
	* @brief	是否在轻功中
	* @param	[in] pKernel 引擎内核指针
	* @param	[in] self 施法者
	* @return	bool
	*/
// 	bool FightInterface::IsInFlySkill(IKernel* pKernel, const PERSISTID& self)
// 	{
// 		return FlySkillModule::m_pInstance->IsInFlySkill(pKernel, self);
// 	}
#endif

/*!
 * @brief	移除宠物或人物的技能
 * @param	self 对象号(可能是宠物也可能是玩家)
 * @param	strSkillId 技能id
 * @return	bool
 */
bool FightInterface::RemoveSkillData(IKernel* pKernel, const PERSISTID &self, const char* strSkillId)
{
	if (!pKernel->Exists(self) || NULL == strSkillId || StringUtil::CharIsNull(strSkillId))
	{
		return false;
	}
	bool bRet = false;
	int nSkillType = GetSkillTypeByConfig(pKernel, strSkillId);
	if (USE_SKILL_TYPE == nSkillType)
	{
		bRet = RemoveSkill(pKernel, self, strSkillId);

		if (pKernel->Type(self) == TYPE_PLAYER)
		{
			// 战斗力更新
//			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_SKILL_TYPE);
		}
	}
	else if(PASSIVE_SKILL_TYPE == nSkillType)
	{
		int nSkillId = StringUtil::StringAsInt(strSkillId);
// 		bRet = PassiveSkillSystemSingleton::Instance()->RemovePassiveSkill(pKernel, self, nSkillId);
// 
// 		if (pKernel->Type(self) == TYPE_PLAYER)
// 		{
// 			// 战斗力更新
// 			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
// 			pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PASSIVE_SKILL_TYPE);
// 		}
	}	

	return bRet;
}

/*!
 * @brief	通过技能名判定技能类型
 * @param	
 * @param
 * @return	int 技能类型对应SkillType
 */
int FightInterface::GetSkillTypeByConfig(IKernel* pKernel, const char* strSkillId)
{
	int nSkillType = NONE_SKILL_TYPE;
	do 
	{
		if (NULL == strSkillId || StringUtil::CharIsNull(strSkillId))
		{
			break;
		}
		const SkillBaseData* pBaseData = SkillDataQueryModule::m_pInstance->GetSkillBase(strSkillId);
		if (NULL == pBaseData)
		{
			break;
		}
		const char* strScript = pBaseData->Getscript();
		// 主动技能的处理
		if (strcmp(strScript, AMBER_SKILL_NORMAL) == 0)
		{
			nSkillType = USE_SKILL_TYPE;
		}
// 		else if (strcmp(strScript, CLASS_NAME_SKILL_INTERACT) == 0)
// 		{
// 			nSkillType = INTERACT_SKILL_TYPE;
// 		}
		else
		{
			nSkillType = PASSIVE_SKILL_TYPE;
		}
	} while (false);
	
	return nSkillType;
}

bool FightInterface::RemoveSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSkillObj)
	{
		return false;
	}
	bool ret = SKillToSpriteSingleton::Instance()->RemoveSkill(pKernel, self, skill);

	return ret;
}


bool FightInterface::RemoveSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	if (NULL == szSkillConfigID || StringUtil::CharIsNull(szSkillConfigID))
	{
		return false;
	}

	PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, szSkillConfigID);
	if (skill.IsNull())
	{
		return false;
	}
	bool ret = SKillToSpriteSingleton::Instance()->RemoveSkill(pKernel, self, skill);

	return ret;
}

/*!
* @brief	获取击杀玩家助攻列表
* @param	outAttackerList 玩家助攻列表
* @param	self 被击杀目标
* @param	killer 击杀者(为空时,返回的列表中带有击杀者)
* @return	bool 是否查询成功
*/
bool FightInterface::GetAttackerList(IKernel* pKernel, IVarList& outAttackerList, const PERSISTID& self, const PERSISTID& killer)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}
	return PVPRecAttackerSingleton::Instance()->GetAttackerList(pKernel, outAttackerList, self, killer);
}