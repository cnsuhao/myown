//--------------------------------------------------------------------
// 文件名:      skill_event_execute_util.h
// 内  容:      技能效果的功能函数
// 说  明:
// 创建日期:    2014年10月30日
// 创建人:       
//    :       
//--------------------------------------------------------------------

#include "FsGame/Define/SkillDefine.h"
#include "utils/geometry_func.h"
#include "utils/math/vector.h"
#include "FsGame/SkillModule/inner/SkillEventExecuteUtil.h"
#include "../../Interface/PropertyInterface.h"
#include "../Impl/Skillaccess.h"
#include "../../Define/FightPropertyDefine.h"
#include "../Impl/TargetFilter.h"
#include "../FightModule.h"
#include "utils/custom_func.h"
#include "utils/util_func.h"
#include "utils/math/fm_fixedpoint.h"
#include "../Impl/SkillDefineDataStruct.h"
#include "../Impl/SkillDataQueryModule.h"
#include "../../NpcBaseModule/AI/AIDefine.h"
#include "../Impl/SkillFlow.h"
#include "../../Interface/FightInterface.h"
#include "Define/Classes.h"
#include "Define/Fields.h"

#define TRACE_LINE_WALK_STEP 4.0f

namespace SkillEventExecuteUtil
{
	bool LocateObject(IKernel* pKernel, IGameObj* pGameTarget, float fPosX, float fPosY, float fPosZ, float fOrient)
	{
		if (pGameTarget)
		{
			if (fabs(pGameTarget->GetPosiX() - fPosX) < 0.1f
				&& fabs(pGameTarget->GetPosiZ() - fPosZ) < 0.1f)
			{
				return false;
			}

			bool res = pKernel->MoveTo(pGameTarget->GetObjectId(), fPosX, fPosY, fPosZ, fOrient);
			return res;
		}
		return false;
	}


	bool MotionObject(IKernel* pKernel, IGameObj* pGameTarget, float fMoveSpeed, float fDestX, float fDestZ, int nMode)
	{
		if (pGameTarget)
		{
			if (fabs(pGameTarget->GetPosiX() - fDestX) < 0.1f
				&& fabs(pGameTarget->GetPosiZ() - fDestZ) < 0.1f)
			{
				return false;
			}

			bool res = false;
			if (MOTION_MODE_HITBACK == nMode)
			{
				res = pKernel->MotionNoRotate(pGameTarget->GetObjectId(), fMoveSpeed, fDestX, fDestZ, nMode);
			}
			else
			{
				res = pKernel->Motion(pGameTarget->GetObjectId(), fMoveSpeed, PI2, fDestX, fDestZ, nMode);
			}

			// motion玩家时,玩家坐标不变,locate下
			if (pGameTarget->GetClassType() == TYPE_PLAYER)
			{
				float fDestY = pKernel->GetMapHeight(fDestX, fDestZ);
				pKernel->Locate(pGameTarget->GetObjectId(), fDestX, fDestY, fDestZ, pGameTarget->GetOrient());
			}
			return res;
		}
		return false;
	}

	// 寻找对象附近的一个坐标位置 
	bool SearchObjectAroundPos(IKernel* pKernel, IGameObj* pGameSelf, float& fOutX, float& fOutY, float& fOutZ)
	{
		// 获得目标的坐标
		float fTargetX = pGameSelf->GetPosiX();
		float fTargetZ = pGameSelf->GetPosiZ();

		// 获得目标格子索引
		int nTargetGridIndex = pKernel->GetGridIndexByPos(fTargetX, fTargetZ);

		if (nTargetGridIndex <= 0)
		{
			return false;
		}

		// 获得周围可用的格子信息
		CVarList GridResult;
		pKernel->GetAroundGridIndex(nTargetGridIndex, GridResult);

		if (GridResult.GetCount() == 0)
		{
			return false;
		}

		// 随机一个目标附近的空格子
		int nMoveGridIndex = GridResult.IntVal(util_random_int((int)GridResult.GetCount()));

		// 获取该格子的坐标
		pKernel->GetPosByIndex(nMoveGridIndex, fOutX, fOutZ);

		fOutY = pKernel->GetMapHeight(fOutX, fOutZ);
		return true;
	}

	// 一定范围内的敌人受到技能伤害 
	void DamgeRangeMultiEnemy(IKernel* pKernel, const PERSISTID& skill, IGameObj* pGameSelf, IGameObj* pTarget,
		float fRadio, float fValue, bool bIncludeSelf, EventDamageType type, int nMaxHurtNum, IVarList& outResultObjs)
	{
		if (NULL == pGameSelf)
		{
			return;
		}
		// 查询目标周围的所有对象
		PERSISTID self = pGameSelf->GetObjectId();
		PERSISTID center = NULL == pTarget ? pGameSelf->GetObjectId() : pTarget->GetObjectId();

		pKernel->GetAroundList(center, fRadio, 0, 0, outResultObjs);
		if (bIncludeSelf)
		{
			outResultObjs.AddObject(center);
		}

		// 获取技能的数据
		int nHitTargetRelationType, nTargetClassType;
		SkillAccessSingleton::Instance()->GetSkillHitTargetRelation(pKernel, skill, nHitTargetRelationType, nTargetClassType);

		int64_t nSkillUID = pGameSelf->QueryInt64(FIGHTER_CUR_SKILL_UUID);

		int nCurHitNum = 0;
		LoopBeginCheck(a)
		for (size_t i = 0; i < outResultObjs.GetCount(); ++i)
		{
			LoopDoCheck(a)
				// nMaxHurtNum为0表示不限制人数
			if (nMaxHurtNum != 0 && nCurHitNum >= nMaxHurtNum)
			{
				break;
			}
			PERSISTID target = outResultObjs.ObjectVal(i);
			IGameObj* pSearchTargetObj = pKernel->GetGameObj(target);
			if (NULL == pSearchTargetObj)
			{
				continue;
			}
			// 目标是否死亡
			int nDead = 0;
			if (FPropertyInstance->GetDead(pSearchTargetObj, nDead) && nDead > 0)
			{
				continue;
			}
			// 找到敌方并扣血 
			if (USESKILL_RESULT_SUCCEED == TargetFilter::SatisfyRelation(pKernel, self, target, nHitTargetRelationType, nTargetClassType))
			{
				if (SKILL_EVENT_DAMAGE == type)
				{
					FightModule::m_pInstance->SkillEventDamageTarget(pKernel, self, target, skill, nSkillUID, fValue);
				}
				else if (VALUE_EVENT_DAMAGE == type)
				{
					FightModule::m_pInstance->EventDamageTarget(pKernel, self, target, skill, nSkillUID, true, (int)fValue);
				}
				else if (USE_SKILL_DAMAGE == type)
				{
					FightModule::m_pInstance->SkillDamageTarget(pKernel, self, target, skill, nSkillUID);
				}
				else if (PERCENT_DAMAGE == type)
				{
#ifdef FSROOMLOGIC_EXPORTS
					// 帮会战的建筑和boss不会受到百分比伤害的影响
					if (1 == pSearchTargetObj->QueryInt("GVG_IsTower") || 0 == strcmp(pSearchTargetObj->GetScript(), "GVGBoss"))
					{
						continue;
					}
#endif
					int64_t nTarMaxHP = pSearchTargetObj->QueryInt64("MaxHP");
					float fDamVal = (float)nTarMaxHP * fValue;
					FightModule::m_pInstance->EventDamageTarget(pKernel, self, target, skill, nSkillUID, true, (int)fDamVal);
				}
				++nCurHitNum;
			}
		}
	}

	// 统计攻击敌人个数
	int	StatDamageEnemyNum(IKernel* pKernel, IGameObj* pGameSelf, const PERSISTID& skill, float fRadio, int nMaxHurtNum)
	{
		if (NULL == pGameSelf || !pKernel->Exists(skill))
		{
			return 0;
		}
		PERSISTID self = pGameSelf->GetObjectId();
		CVarList outResultObjs;
		pKernel->GetAroundList(self, fRadio, 0, 0, outResultObjs);

		// 获取技能的数据
		int nHitTargetRelationType, nTargetClassType;
		SkillAccessSingleton::Instance()->GetSkillHitTargetRelation(pKernel, skill, nHitTargetRelationType, nTargetClassType);
		// 攻击敌人个数
		int nCurHitNum = 0;
		LoopBeginCheck(b)
		for (size_t i = 0; i < outResultObjs.GetCount(); ++i)
		{
			LoopDoCheck(b)
				// nMaxHurtNum为0表示不限制人数
			if (nMaxHurtNum != 0 && nCurHitNum >= nMaxHurtNum)
			{
				break;
			}
			PERSISTID target = outResultObjs.ObjectVal(i);
			IGameObj* pTarget = pKernel->GetGameObj(target);
			if (NULL == pTarget)
			{
				continue;
			}
			// 目标是否死亡
			int nDead = 0;
			if (FPropertyInstance->GetDead(pTarget, nDead) && nDead > 0)
			{
				continue;
			}
			// 找到敌方并扣血 
			if (USESKILL_RESULT_SUCCEED == TargetFilter::SatisfyRelation(pKernel, self, target, nHitTargetRelationType, nTargetClassType))
			{
				++nCurHitNum;
			}
		}

		return nCurHitNum;
	}
	// 为一个对象加回复HP
	int RestoreHPForObject(IKernel* pKernel, IGameObj* pSelfObj, float fVal, EventCureCal type, float fAddRate)
	{
		// 真实加血值
		int64_t nRealAdd = 0;
		if (NULL == pSelfObj)
		{
			return 0;
		}
		// 得到玩家当前的hp和maxhp
		int64_t nMaxValue = pSelfObj->QueryInt64("MaxHP");
		int64_t nCurValue = pSelfObj->QueryInt64("HP");
		// 得到准备加血的数值
		int64_t nValue = 0;
		if (EVENT_VALUE_RESTORE == type)
		{
			nValue = (int64_t)fVal;
		}
		else
		{
			nValue = (int64_t)((float)nMaxValue * fVal);
		}

		if (fAddRate > 0.0f)
		{
			nValue = (int64_t)((float)nValue * (1 + fAddRate));
		}

		// HP回复减弱属性处理
		// 	int nWeakRate = pSelfObj->QueryInt("WeakRestoreHPRate");
		// 	float fWeakRate = 0.0f;
		// 	if (nWeakRate > 0)
		// 	{
		// 		fWeakRate = (float)nWeakRate / 100.0f;
		// 	}
		// 	nValue = (int64_t)((float)nValue * (1 - fWeakRate)); 

		// 计算真实加血值和加血后的数值
		if (nCurValue + nValue > nMaxValue)
		{
			nValue = nMaxValue;
			nRealAdd = nMaxValue - nCurValue;
		}
		else if (nCurValue + nValue < 0)
		{
			nValue = 0;
			nRealAdd = 0;
		}
		else
		{
			nRealAdd = nValue;
			nValue = nCurValue + nValue;
		}
		// 设置当前HP
		pSelfObj->SetInt64("HP", nValue);
		return (int)nRealAdd;
	}

	// 通过技能为对象回复HP
	// int RestoreHPForObjectBySkill(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skiller, const PERSISTID& object, float fVal, EventCureCal type)
	// {
	// 	IGameObj* pObj = pKernel->GetGameObj(object);
	// 	IGameObj* pSkillerObj = pKernel->GetGameObj(skiller);
	// 	if (NULL == pSelfObj || NULL == pObj || NULL == pSkillerObj)
	// 	{
	// 		return 0;
	// 	}
	// 	// HP恢复附加率有效的判定
	// 	bool bAddReHP = false;
	// 	// 除了药品技能外的治疗技能
	// 	if (0 == strcmp(pObj->GetScript(), AMBER_SKILL_NORMAL) && 0 != strcmp(pObj->GetConfig(), RESTORE_HP_SKILL_ID))
	// 	{
	// 		bAddReHP = true;
	// 	}
	// 	// 或者由技能产生的buff
	// 	else if (0 == strcmp(pObj->GetScript(), AMBER_BUFFER))
	// 	{
	// 		PERSISTID skill = pObj->QueryObject("Skill");
	// 
	// 		if (pKernel->Exists(skill) && 0 == strcmp(pKernel->GetScript(skill), AMBER_SKILL_NORMAL))
	// 		{
	// 			bAddReHP = true;
	// 		}
	// 	}
	//  
	// 	float fAddRate = 0.0f;
	// 	if (bAddReHP)
	// 	{
	// 		int nReHPAddRate = 0;
	// 		if (pKernel->Exists(skiller))
	// 		{
	// 			nReHPAddRate = pSkillerObj->QueryInt("ReHPAddRate");
	// 		}
	// 		fAddRate = (float)nReHPAddRate / 10000.0f;
	// 	}
	// 	
	// 	return RestoreHPForObject(pKernel, pSelfObj, fVal, type, fAddRate);
	// }

	/*!
	* @brief	随机在周围环形范围内找一个可站立点
	* @param	int 随机次数n(如果随机n次还没找到点,则寻找失败)
	* @param	float fMinRadios, float fMaxRadios, 内外圆半径及角度
	* @param	float& fOutX, float& fOutY 返回的坐标值
	* @return	bool 是否查找成功
	*/
	bool RandomFindAroundPos(IKernel* pKernel, IGameObj* pSelfObj, int nRandomNum, float fMinRadios, float fMaxRadios, float& fOutX, float& fOutY, float& fOutZ)
	{
		bool bFind = false;
		if (NULL == pSelfObj || nRandomNum <= 0)
		{
			return false;
		}
		// 随机一个距离
		float fRadios = fMinRadios + util_random_float(fMaxRadios - fMinRadios);
		LoopBeginCheck(c)
		for (int i = 0; i < nRandomNum; ++i)
		{
			LoopDoCheck(c)
				// 随机角度
				float fTmpAnagle = util_random_float(PI);
			if (FindPosByDisAndOrient(pKernel, pSelfObj, fRadios, fTmpAnagle, fOutX, fOutY, fOutZ))
			{
				bFind = true;
				break;
			}
		}

		return bFind;
	}

	/*!
	* @brief	沿着固定朝向距离找到位置
	* @param	fDistance 距离
	* @param	fOrient 方向
	* @param	float& fOutX, float& fOutY, float& fOutZ 返回的位置坐标
	* @return	void
	*/
	bool FindPosByDisAndOrient(IKernel* pKernel, IGameObj* pSelfObj, float fDistance, float fOrient, float& fOutX, float& fOutY, float& fOutZ)
	{
		bool bFind = false;
		if (NULL == pSelfObj)
		{
			return false;
		}

		fOutX = 0.0f;
		fOutY = 0.0f;
		fOutZ = 0.0f;

		float fDeltaX = fDistance * FMSIN(fOrient);
		float fDeltaZ = fDistance * FMCOS(fOrient);
		// 计算最终的随机坐标
		float fTmpX = pSelfObj->GetPosiX() + fDeltaX;
		float fTmpZ = pSelfObj->GetPosiZ() + fDeltaZ;
		// 获取此点的格子索引
		int nGridIndex = pKernel->GetGridIndexByPos(fTmpX, fTmpZ);
		if (-1 == nGridIndex)
		{
			return false;
		}

		// 判定格子是否可用
		bool bCanWalk = false;
		bool bLogicbalk = false;
		bool bObjectoccupy = false;
		int groupId = pSelfObj->QueryInt("GroupID");
		pKernel->QueryPathGridStatus(nGridIndex, bCanWalk, bLogicbalk, bObjectoccupy, groupId);
		if (bCanWalk)
		{
			fOutX = fTmpX;
			fOutZ = fTmpZ;
			fOutY = pKernel->GetMapHeight(fOutX, fOutZ);
			bFind = true;
		}

		return bFind;
	}

	// 考虑地形障碍，获得下一个位置
	void GetFinalPosByOrient(IKernel* pKernel, float fSrcX, float fSrcY, float fSrcZ, float fOrient, float fDistance,
		float& fFinalX, float& fFinalY, float& fFinalZ)
	{
		//移动方向的单位向量
		FmVec2 vNormal = FmVec2(::sin(fOrient), ::cos(fOrient));

		//计算目标点
		float fDestX = fSrcX + vNormal.x * fDistance;
		float fDestZ = fSrcZ + vNormal.y * fDistance;

		//获取最终点
		float fNewX = 0.0f, fNewY = 0.0f, fNewZ = 0.0f;
		if (!pKernel->TraceLineWalk(TRACE_LINE_WALK_STEP, fSrcX, fSrcY, fSrcZ, fDestX, fDestZ, fNewX, fNewY, fNewZ))
		{
			fFinalX = fNewX;
			fFinalY = fNewY;
			fFinalZ = fNewZ;
		}
		else
		{
			fFinalX = fDestX;
			fFinalZ = fDestZ;
			fFinalY = pKernel->GetMapHeight(fFinalX, fFinalZ);
		}
	}

	// 通过buff对象查找包id
	const char* QueryBuffPackageId(IKernel* pKernel, const PERSISTID& buff)
	{
		const char* strBasePackageID = "";
		do
		{
			if (!pKernel->Exists(buff))
			{
				break;
			}
			const char* szFromID = pKernel->GetConfig(buff);
			const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szFromID);
			if (pBuffBaseData == NULL)
			{
				break;
			}

			//获取buff包的id
			strBasePackageID = pBuffBaseData->GetBasePackage();
		} while (0);

		return strBasePackageID;
	}

	// 创建陷阱npc对象
	PERSISTID CreateTrapNpcObj(IKernel* pKernel, IGameObj* pSelfObj, const char* strNpcId, float fPosX, float fPosY, float fPosZ)
	{
		if (NULL == pSelfObj || StringUtil::CharIsNull(strNpcId))
		{
			return PERSISTID();
		}

		CVarList args;
		args << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << pSelfObj->QueryInt("GroupID") << "Master" << pSelfObj->GetObjectId();

		PERSISTID npc = pKernel->CreateObjectArgs("", strNpcId, 0, fPosX, fPosY, fPosZ, pSelfObj->GetOrient(), args);
		return npc;
	}

	// 根据范围统计陷阱攻击目标
	bool FindTargetList(IKernel* pKernel, IVarList& outTargetList, IGameObj* pSelfObj,
		const PERSISTID& master, const PERSISTID& skill, int nHitIndex, bool bUseSkillRange, float fSkillRadius)
	{
		if (NULL == pSelfObj || !pKernel->Exists(master) || !pKernel->Exists(skill) || nHitIndex < 0)
		{
			return false;
		}

		outTargetList.Clear();

		// 不用技能范围
		if (!bUseSkillRange)
		{
			// 查询目标周围的所有对象
			CVarList resultObj;
			pKernel->GetAroundList(pSelfObj->GetObjectId(), fSkillRadius, TYPE_PLAYER | TYPE_NPC, 0, outTargetList);
		}
		else
		{

			float fPosX = pSelfObj->GetPosiX();
			float fPosY = pSelfObj->GetPosiY();
			float fPosZ = pSelfObj->GetPosiZ();

			// 命中次数从1计数
			++nHitIndex;

			//统计技能影响的对象
			SkillFlowSingleton::Instance()->GetHitRangeTargets(pKernel, master, skill, fPosX, fPosY, fPosZ, nHitIndex, outTargetList);
		}
		return true;
	}

	// 是否相同的命中次数
	bool IsSameHitIndex(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj, int nHitIndex)
	{
		if (NULL == pSkillObj || NULL == pSelfObj)
		{
			return false;
		}

		// 配置为0时,不验证索引值
		if (0 == nHitIndex)
		{
			return true;
		}

		bool bSame = false;
		int nMaxHitNum = SkillDataQueryModule::m_pInstance->GetMaxHitNum(pKernel, pSkillObj);
		if (nHitIndex > nMaxHitNum)
		{
			nHitIndex = nMaxHitNum;
		}
		// 转为从0计数
		--nHitIndex;
		// 只有在指定命中时,才会拉住敌方
		int nCurHitIndex = SkillAccessSingleton::Instance()->GetHitIndex(pKernel, pSelfObj, pSkillObj);

		if (nCurHitIndex == nHitIndex)
		{
			bSame = true;;
		}

		return bSame;
	}

	// 是否为玩家类型
	bool IsPlayerType(IGameObj* pSelfObj)
	{
		if (pSelfObj == NULL)
		{
			return false;
		}

		return pSelfObj->GetClassType() == TYPE_PLAYER || pSelfObj->GetClassType() == TYPE_NPC && strcmp(pSelfObj->GetScript(), "BattleNpc") == 0;
	}

	// 检测目标类型是否符合
	bool CheckTargetType(IGameObj* pTargetObj, int nCheckType)
	{
		if (NULL == pTargetObj)
		{
			return false;
		}

		// 目标类型检测,nAddTargetType为ALL_TARGET_TYPE不用检测
		if (ALL_TARGET_TYPE == nCheckType)
		{
			return true;
		}

		bool bCheckResult = false;
		int nTargetType = pTargetObj->GetClassType();
		if (TYPE_NPC == nCheckType)
		{
			bCheckResult = nTargetType == nCheckType;
		}
		else if (TYPE_PLAYER == nCheckType)
		{
			bCheckResult = SkillEventExecuteUtil::IsPlayerType(pTargetObj);
		}

		return bCheckResult;
	}
};