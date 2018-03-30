//--------------------------------------------------------------------
// 文件名:		SceneAttackManage.cpp
// 内  容:		场景可攻击管理器
// 说  明:		
//				
// 创建日期:	2015年05月28日
// 创建人:		 
//    :	    
//--------------------------------------------------------------------

#include <assert.h>
#include "SceneAttackManage.h"
#include "utils/string_util.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/extend_func.h"
#include "Define/Fields.h"
#include "SocialSystemModule/TeamModule.h"
#include "Define/Classes.h"

//判断函数地址表
std::vector<SCENE_PK_TYPE_FUNC> SceneAttackManage::m_funCheckRelationList;

//初始化
void SceneAttackManage::Init(IKernel *pKernel)
{
    m_funCheckRelationList.resize(SCENE_ATTACK_TYPE_MAX, NULL);

	RegisterJudgeFun(SCENE_ATTACK_PUBLIC_SCENE, CheckByPlayerPKModel);
    RegisterJudgeFun(SCENE_ATTACK_CAMP, CheckCampPK);
	//RegisterJudgeFun(SCENE_ATTACK_GUILD, CheckGuildPK);
	RegisterJudgeFun(SCENE_ATTACK_PEACE, CheckPeace);
	RegisterJudgeFun(SCENE_ATTACK_PVE, CheckPVE);
}

//判断一个对象是否可以攻击
bool SceneAttackManage::CheckTargetRelation(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	PERSISTID attacker = get_pet_master(pKernel, self);
																																			  
    SceneAttackType type = (SceneAttackType)pKernel->GetSceneObj()->QueryInt("AttackType");
	if (type < 0 || type >= SCENE_ATTACK_TYPE_MAX )
    {
        return false;
    }

	return m_funCheckRelationList[type](pKernel, attacker, target, nTestRelationType);
}

//注册攻击模式类型判断函数
bool SceneAttackManage::RegisterJudgeFun(SceneAttackType type, SCENE_PK_TYPE_FUNC fun)
{
    if(type < 0 || type >= SCENE_ATTACK_TYPE_MAX)
    {
        return false;
    }
    if(fun == NULL)
    {
        return false;
    }
    m_funCheckRelationList[type] = fun;
    return true;
}

// 自定义阵营关系判断
bool SceneAttackManage::CheckCampPK(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj *pTarget = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || NULL == pTarget)
	{
		return false;
	}

	int nSelfCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	int nTargetCamp = pTarget->QueryInt(FIELD_PROP_CAMP);

	// 阵营值相同为友方,不同为敌方 
	bool bResult = false;
	if (TRT_FRIEND == nTestRelationType)
	{
		bResult = nSelfCamp == nTargetCamp;
	}
	else
	{
		bResult = nSelfCamp != nTargetCamp;
	}

	return bResult;
}

// 自定义公会关系判断
// bool SceneAttackManage::CheckGuildPK(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
// {
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	IGameObj *pTarget = pKernel->GetGameObj(target);
// 	if (pSelfObj == NULL || NULL == pTarget)
// 	{
// 		return false;
// 	}
// 
// 	const wchar_t* wsSelfGuildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	const wchar_t* wsTargetGuildName = pTarget->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 
// 	// 阵营值相同为友方,不同为敌方 
// 	bool bResult = false;
// 	if (TRT_FRIEND == nTestRelationType)
// 	{
// 		bResult = StringUtil::CharIsEqual(wsSelfGuildName, wsTargetGuildName);
// 	}
// 	else
// 	{
// 		bResult = !StringUtil::CharIsEqual(wsSelfGuildName, wsTargetGuildName);;
// 	}
// 
// 	return bResult;
// }

// 和平
bool SceneAttackManage::CheckPeace(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj *pTarget = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || NULL == pTarget)
	{
		return false;
	}

	bool bResult = false;
	if (TRT_FRIEND == nTestRelationType)
	{
		bResult = true;
	}
	else
	{
		bResult = false;;
	}

	return bResult;
}

// 只能PVE
bool SceneAttackManage::CheckPVE(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || NULL == pTargetObj)
	{
		return false;
	}

	int nSelfNation = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	int nTargetNation = pTargetObj->QueryInt(FIELD_PROP_CAMP);
	bool bResult = false;
	// 攻击者为怪
	if (pSelfObj->GetClassType() == TYPE_NPC)
	{
		if (TRT_FRIEND == nTestRelationType)
		{
			bResult = nSelfNation == nTargetNation;
		}
		else
		{
			bResult = nSelfNation != nTargetNation;
		}
	}
	else if (TYPE_PLAYER == pSelfObj->GetClassType())  // 攻击者是玩家
	{
		if (TYPE_NPC == pTargetObj->GetClassType())	// 目标为npc
		{
			if (TRT_FRIEND == nTestRelationType)
			{
				bResult = false;
			}
			else
			{
				bResult = nSelfNation != nTargetNation;
			}
		}
		else if (TYPE_PLAYER == pTargetObj->GetClassType())
		{
			if (TRT_FRIEND == nTestRelationType)
			{
				bResult = true;
			}
			else
			{
				bResult = false;;
			}
		}
	}

	return bResult;
}

//PK模式逻辑
bool SceneAttackManage::CheckByPlayerPKModel(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (pSelfObj == NULL || NULL == pTargetObj)
	{
		return false;
	}

	int nSelfNation = pSelfObj->QueryInt(FIELD_PROP_CAMP);
	int nTargetNation = pTargetObj->QueryInt(FIELD_PROP_CAMP);
	bool bResult = false;
	// 攻击者为怪
	if (pSelfObj->GetClassType() == TYPE_NPC)
	{
		if (TRT_FRIEND == nTestRelationType)
		{
			bResult = nSelfNation == nTargetNation;
		}
		else
		{
			bResult = nSelfNation != nTargetNation;
		}
	}
	else if (TYPE_PLAYER == pSelfObj->GetClassType())  // 攻击者是玩家
	{
		if (TYPE_NPC == pTargetObj->GetClassType())	// 目标为npc
		{
			if (TRT_FRIEND == nTestRelationType)
			{
				bResult = false;
			}
			else
			{
				bResult = nSelfNation != nTargetNation;
			}
		}
		else if (TYPE_PLAYER == pTargetObj->GetClassType())	 // 目标为玩家
		{
			// 在公共场景中, 切分组进入PK模式的处理
			int nSelfCamp = pSelfObj->QueryInt(FIELD_PROP_CAMP);
			int nTargetCamp = pTargetObj->QueryInt(FIELD_PROP_CAMP);
			if (nSelfCamp > 0 && nTargetCamp > 0 && pSelfObj->QueryInt(FIELD_PROP_GROUP_ID) == pTargetObj->QueryInt(FIELD_PROP_GROUP_ID))
			{
				return CheckCampPK(pKernel, self, target, nTestRelationType);
			}

			if (TRT_FRIEND == nTestRelationType)
			{
				bResult = IsFriendlyInPublicScene(pKernel, pSelfObj, pTargetObj);
			}
			else
			{
				bResult = IsEnemyInPublicScene(pKernel, pSelfObj, pTargetObj);
			}
		}
	}

	return bResult;
}

// 是否为友方
bool SceneAttackManage::IsFriendlyInPublicScene(IKernel *pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}

	// 自己为友方
	if (pSelfObj == pTargetObj)
	{
		return true;
	}

	// 只有队友为友方
	const wchar_t* wsTargetName = pTargetObj->QueryWideStr(FIELD_PROP_NAME);
	bool bResult = TeamModule::IsTeamMate(pKernel, pSelfObj->GetObjectId(), wsTargetName);

	return bResult;
}

// 是否为敌方
bool SceneAttackManage::IsEnemyInPublicScene(IKernel *pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}
	
	// 队友肯定不是敌方,自己为友方
	const wchar_t* wsTargetName = pTargetObj->QueryWideStr(FIELD_PROP_NAME);
	if (TeamModule::IsTeamMate(pKernel, pSelfObj->GetObjectId(), wsTargetName) || pSelfObj == pTargetObj)
	{
		return false;
	}
	
	bool bResult = false;

	int nPKModel = pSelfObj->QueryInt(FIELD_PROP_PKMODEL);
	switch (nPKModel)
	{
	case PT_PEACE:
		bResult = false;
		break;
	case PT_ATT_CRIME:
		bResult = IsEnemyInAttCrimeModel(pSelfObj, pTargetObj);
		break;
	case PT_GUILD:
		//bResult = IsEnemyInGuildModel(pSelfObj, pTargetObj);
		break;
	case PT_KILL_ALL:
		bResult = true;
		break;
	}

	return bResult;
}

// 和平模式判定
bool SceneAttackManage::IsEnemyInAttCrimeModel(IGameObj* pSelfObj, IGameObj* pTargetObj)
{
	if (NULL == pSelfObj || NULL == pTargetObj)
	{
		return false;
	}
 
	bool bResult = false;
	int nTargetRedState = pTargetObj->QueryInt(FIELD_PROP_CRIME_STATE);
	// 目标是红名,和平模式下就能打
	if (nTargetRedState > 0)
	{
		bResult = true;
	}

	return bResult;
}

// 公会模式下的判定
// bool SceneAttackManage::IsEnemyInGuildModel(IGameObj* pSelfObj, IGameObj* pTargetObj)
// {																																												  
// 	if (NULL == pSelfObj || NULL == pTargetObj)
// 	{
// 		return false;
// 	}
// 
// 	const wchar_t* wsSelfGuildName = pSelfObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	const wchar_t* wsTargetGuildName = pTargetObj->QueryWideStr(FIELD_PROP_GUILD_NAME);
// 	if (StringUtil::CharIsNull(wsSelfGuildName) /*|| StringUtil::CharIsNull(wsTargetGuildName)*/)
// 	{
// 		return false;
// 	}
// 
// 	bool bResult = wcscmp(wsSelfGuildName, wsTargetGuildName) != 0;
// 	
// 	return bResult;
// }