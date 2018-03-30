//--------------------------------------------------------------------
// 文件名:      skill_event_execute_util.h
// 内  容:      技能效果的功能函数
// 说  明:
// 创建日期:    2014年10月30日
// 创建人:       
//    :       
//--------------------------------------------------------------------

#ifndef __SKILLEVENTEXECUTEUTIL_H__
#define __SKILLEVENTEXECUTEUTIL_H__

#include "Fsgame/Define/header.h"
#include "../../Define/FightDefine.h"

namespace SkillEventExecuteUtil
{
	enum
	{
		ALL_TARGET_TYPE = 0,		// 所有类型
	};

	// 事件伤害类型
	enum EventDamageType
	{
		SKILL_EVENT_DAMAGE,			// 与技能公式相关的伤害
		VALUE_EVENT_DAMAGE,			// 固定值的伤害
		USE_SKILL_DAMAGE,			// 使用技能伤害
		PERCENT_DAMAGE				// 按百分比伤害
	};

	// 治疗类型
	enum EventCureCal
	{
		EVENT_VALUE_RESTORE,		// 固定值回复血量
		EVENT_PERCENT_RESTORE,		// 按百分比回复血量
	};

	// 位置数据
	struct PosInfo
	{
		PosInfo() : fPosX(0.0f), fPosZ(0.0f){}
		float	fPosX;		// x坐标
		float	fPosZ;		// z坐标
	};

	//对象定位，内部区分玩家或NPC
	bool LocateObject(IKernel* pKernel, IGameObj* pGameTarget, float fPosX, float fPosY, float fPosZ, float fOrient);

	//对象移动，内部区分玩家或NPC
	bool MotionObject(IKernel* pKernel, IGameObj* pGameTarget, float fMoveSpeed, float fDestX, float fDestZ, int nMode = 0);

	// 寻找对象附近的一个坐标位置 
	bool SearchObjectAroundPos(IKernel* pKernel, IGameObj* pGameSelf, float& fOutX, float& fOutY, float& fOutZ);

	// 一定范围内的敌人受到技能伤害 nMaxHurtNum 0 表示不限人数 (type为VALUE_EVENT_DAMAGE时 不能再伤害回调中调用此函数)
	void DamgeRangeMultiEnemy(IKernel* pKernel, const PERSISTID& skill, IGameObj* pGameSelf, IGameObj* pTarget,
		float fRadio, float fValue, bool bIncludeSelf, EventDamageType type, int nMaxHurtNum, IVarList& outResultObjs);

	// 统计攻击敌人个数
	int	StatDamageEnemyNum(IKernel* pKernel, IGameObj* pGameSelf, const PERSISTID& skill, float fRadio, int nMaxHurtNum);

	// 为一个对象加回复HP
	int RestoreHPForObject(IKernel* pKernel, IGameObj* pSelfObj, float fVal, EventCureCal type, float fAddRate = 0.0f);

	// 通过技能为对象回复HP object可能是技能或buff
	//int RestoreHPForObjectBySkill(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& skiller, const PERSISTID& object, float fVal, EventCureCal type);

	/*!
	* @brief	随机在周围环形范围内找一个可站立点
	* @param	int 随机次数n(如果随机n次还没找到点,则寻找失败)
	* @param	float fMinRadios, float fMaxRadios 内外圆半径
	* @param	float& fOutX, float& fOutY 返回的坐标值
	* @return	bool 是否查找成功
	*/
	bool RandomFindAroundPos(IKernel* pKernel, IGameObj* pSelfObj, int nRandomNum, float fMinRadios, float fMaxRadios, float& fOutX, float& fOutY, float& fOutZ);

	/*!
	* @brief	沿着固定朝向距离找到位置
	* @param	fDistance 距离
	* @param	fOrient 方向
	* @param	float& fOutX, float& fOutY, float& fOutZ 返回的位置坐标
	* @return	void
	*/
	bool FindPosByDisAndOrient(IKernel* pKernel, IGameObj* pSelfObj, float fDistance, float fOrient, float& fOutX, float& fOutY, float& fOutZ);

	// 考虑地形障碍，获得下一个位置
	void GetFinalPosByOrient(IKernel* pKernel, float fSrcX, float fSrcY, float fSrcZ, float fOrient, float fDistance,
		float& fFinalX, float& fFinalY, float& fFinalZ);

	// 通过buff对象查找包id
	const char* QueryBuffPackageId(IKernel* pKernel, const PERSISTID& buff);

	// 创建陷阱npc对象
	PERSISTID CreateTrapNpcObj(IKernel* pKernel, IGameObj* pSelfObj, const char* strNpcId, float fPosX, float fPosY, float fPosZ);

	// 根据范围统计陷阱攻击目标
	bool FindTargetList(IKernel* pKernel, IVarList& outTargetList, IGameObj* pSelfObj,
		const PERSISTID& master, const PERSISTID& skill, int nHitIndex, bool bUseSkillRange, float fSkillRadius = 0.0f);

	// 是否相同的命中次数
//	bool IsSameHitIndex(IKernel* pKernel, IGameObj* pSelfObj, IGameObj* pSkillObj, int nHitIndex);

	// 是否为玩家类型
	bool IsPlayerType(IGameObj* pSelfObj);

	// 检测目标类型是否符合
	bool CheckTargetType(IGameObj* pTargetObj, int nCheckType);
};

#endif // __SKILLEVENTEXECUTEUTIL_H__