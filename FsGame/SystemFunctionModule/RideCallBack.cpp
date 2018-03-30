//--------------------------------------------------------------------
// 文件名:		RideCallBack.cpp
// 内  容:		坐骑回调函数
// 说  明:		
// 创建日期:	2014年12月32日
// 创建人:		
// 修改人:		 tongzt
//--------------------------------------------------------------------

#include "RideModule.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "FsGame/SkillModule/Impl/SkillFlow.h"
#include "../SkillModule/Impl/SkillDataQueryModule.h"
#include "../Define/BufferDefine.h"

#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "../Define/Fields.h"
#include "../CommonModule/SwitchManagerModule.h"


/*!
* @brief	玩家上线
* @param	pKernel 核心指针
* @param	self    自己
* @param	sender
* @param	args    参数
* @return	bool
*/
int RideModule::OnPlayerRecover(IKernel* pKernel,
	const PERSISTID& self,
	const PERSISTID& sender,
	const IVarList& args)
{
	return m_pRideModule->PlayerOnline(pKernel, self);
}

/*!
* @brief	玩家进入游戏
* @param	pKernel 核心指针
* @param	self    自己
* @param	sender
* @param	args    参数
* @return	bool
*/
int RideModule::OnPlayerReady(IKernel* pKernel,
	const PERSISTID& self,
	const PERSISTID& sender,
	const IVarList& args)
{
	// 判断
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 在线玩家(online_flag == 1)
	int online_flag = pSelf->QueryInt("Online");

	if (!pKernel->FindViewport(self, VIEWPORT_RIDE) && online_flag == 1)
	{
		PERSISTID box = pKernel->GetChild(self, L"RideBox");
		if (pKernel->Exists(box))
		{
			pKernel->AddViewport(self, VIEWPORT_RIDE, box);
		}
		else
		{
			::extend_warning(pKernel, /*LOG_WARNING, */"[RideModule::OnPlayerReady]: RideBox not Exists");
		}
	}

	return 0;
}

/*!
* @brief	坐骑背包创建
* @param	pKernel 核心指针
* @param	self    自己
* @param	sender
* @param	args    参数
* @return	bool
*/
int RideModule::OnRideBoxCreate(IKernel* pKernel,
	const PERSISTID& ridebox,
	const PERSISTID& sender,
	const IVarList& args)
{
	// 判断
	IGameObj* pRidebox = pKernel->GetGameObj(ridebox);
	if (NULL == pRidebox)
	{
		return 0;
	}

	// 设置名字
	pRidebox->SetWideStr("Name", L"RideBox");

	return 0;
}

/*!
* @brief	坐骑背包Class创建回调
* @param	pKernel 核心指针
* @param	index   索引
* @return	int
*/
int RideModule::OnRideBoxClassCreate(IKernel* pKernel,
	int index)
{
	// 设置属性保存
	pKernel->SetSaving(index, "Name", true);

	return 0;
}

/*!
* @brief	释放技能开始回调
* @param	pKernel 核心指针
* @param	self   自己
* @param	sender 技能target
* @param	args   参数
* @return	int
*/
int RideModule::OnCommandPlayerBeginSkill(IKernel * pKernel,
	const PERSISTID & self,
	const PERSISTID & sender,
	const IVarList & args)
{
	// 判断
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 获取技能对象
	PERSISTID skill = args.ObjectVal(1);
	if (!pKernel->Exists(skill))
	{
		return 0;
	}

	// 回复HP技能不能打断骑马
	const char* strSkill = pKernel->GetConfig(skill);
	if (StringUtil::CharIsNull(strSkill) || strcmp(strSkill, RESTORE_HP_SKILL_ID) == 0)
	{
		return 0;
	}

	// 隐藏坐骑
	RideModule::m_pRideModule->DownRide(pKernel, self);

	return 0;
}

/*!
* @brief	释放技能完成回调
* @param	pKernel 核心指针
* @param	self   自己
* @param	sender 技能target
* @param	args   参数
* @return	int
*/
int RideModule::OnCommandPlayerFinishSkill(IKernel * pKernel,
	const PERSISTID & self,
	const PERSISTID & sender,
	const IVarList & args)
{
	// 判断
	IGameObj* pSender = pKernel->GetGameObj(sender);
	if (NULL == pSender)
	{
		return 0;
	}
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	// 获取技能
	PERSISTID skill = args.ObjectVal(1);
	if (!pKernel->Exists(skill))
	{
		return 0;
	}

	// 回复HP技能不能打断骑马
	const char* strSkill = pKernel->GetConfig(skill);
	if (StringUtil::CharIsNull(strSkill) || strcmp(strSkill, RESTORE_HP_SKILL_ID) == 0)
	{
		return 0;
	}

	// 判断攻击的NPC类型
	const char* npc_script = pSender->GetScript();

	int iType = 0;
	if (strcmp(npc_script, "player") != 0)
	{
		iType = pSender->QueryInt("NpcType");
	}

	if (strcmp(npc_script, "PetNpc") == 0 || strcmp(npc_script, "player") == 0 || iType == 3)
	{
		RideModule::m_pRideModule->DownRide(pKernel, self);
	}

	return 0;
}

/*!
* @brief	处理玩家被怪物攻击死亡后需要下马逻辑
* @param	pKernel 核心指针
* @param	self   死亡对象
* @param	sender 攻击对象
* @param	args
* @return	int
*/
int RideModule::OnCommandBeKill(IKernel* pKernel,
	const PERSISTID& self,
	const PERSISTID& sender,
	const IVarList& args)
{
	IGameObj* playerObj = pKernel->GetGameObj(self);
	if (NULL == playerObj)
	{
		return 0;
	}

	// 人物死亡，下马
	RideModule::m_pRideModule->DownRide(pKernel, self);
	return 0;
}

/*!
* @brief	受到伤害完成回调
* @param	pKernel 核心指针
* @param	self   受到伤害的玩家
* @param	sender 释放者target
* @param	args   参数
* @return	int
*/
int RideModule::OnCommandBeDamage(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	IGameObj* playerObj = pKernel->GetGameObj(self);
	if (NULL == playerObj)
	{
		return 0;
	}

	IGameObj* pSender = pKernel->GetGameObj(sender);
	if (NULL == pSender)
	{
		return 0;
	}

	// 判断攻击的NPC类型
	const char* npc_script = pSender->GetScript();
	int iType = 0;
	if (strcmp(npc_script, "player") != 0)
	{
		iType = pSender->QueryInt("NpcType");
	}
	if (strcmp(npc_script, "PetNpc") == 0 || strcmp(npc_script, "player") == 0 || iType == 3)
	{
		RideModule::m_pRideModule->DownRide(pKernel, self);
	}

	return 0;
}

// 被添加buff时下马
int RideModule::OnCommandAddBuffer(IKernel * pKernel, const PERSISTID & self,
	const PERSISTID & sender, const IVarList & args)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(sender) || pKernel->Type(self) != TYPE_PLAYER)
	{
		return 0;
	}

	// 获取buff的配置数据
	const char* szFromID = pKernel->GetConfig(sender);
	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szFromID);
	if (pBuffBaseData == NULL)
	{
		return false;
	}
	// 如果添加的buff不是增益buff  就下马
	if (pBuffBaseData->GetAffectType() != BUFFAFFECT_BENEFICIAL)
	{
		RideModule::m_pRideModule->DownRide(pKernel, self);
	}

	return 0;
}

/*!
* @brief	处理客户端消息
* @param	pKernel
* @param	self
* @param	sender
* @param	args
* @return	int
*/
int RideModule::OnCustomMessage(IKernel* pKernel,
	const PERSISTID& self,
	const PERSISTID& sender,
	const IVarList& args)
{
	// 保护判断
	if (!pKernel->Exists(self) || m_pRideModule == NULL)
	{
		return 0;
	}

	// 二级消息ID
	int sub_msg = args.IntVal(1);
	switch (sub_msg)
	{
	case CS_RIDE_SKIN_MSG_REQ:
		{
			m_pRideModule->PushRideSkinMsg(pKernel, self, args.IntVal(2));
		}
		break;
	case CS_RIDE_CHANGE_SKIN:
		{
			m_pRideModule->ChangeRideSkin(pKernel, self, args.IntVal(2), args.IntVal(3));
		}
		break;
	case CS_RIDE_UP_DOWN:
		{
			m_pRideModule->RideUpDownOperate(pKernel, self, args.IntVal(2));
		}
		break;
	case CS_RIDE_CLEAR_SKIN_FLAG:
		{
			m_pRideModule->ClearSkinFlag(pKernel, self, args.IntVal(2));
		}
		break;
	case CS_RIDE_SKIN_UNLOCK:
		{
			m_pRideModule->UnlockSkin(pKernel, self, args.IntVal(2));
		}
		break;
	case CS_RIDE_UPGRADE_LEVEL:
		m_pRideModule->UpgradeLevel(pKernel, self);
		break;
	case CS_RIDE_UPGRADE_STEP:
		m_pRideModule->UpgradeStep(pKernel, self);
		break;
	default:
		break;
	}

	return 0;
}