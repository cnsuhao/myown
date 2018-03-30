//----------------------------------------------------------
// 文件名:      PropertyInterface.h
// 内  容:      技能、buffer、通用属性查询的接口
// 创建人:       
// 创建日期:    2014年10月
//    :        
//----------------------------------------------------------
#ifndef FS_ROOM_PROPERTY_INTERFACE_H
#define FS_ROOM_PROPERTY_INTERFACE_H

#include "Fsgame/Define/header.h"
#include "utils/singleton.h"
#include "DeclareProperty.h"
class SkillPropertyInterface
{
	// 技能属性
	DECLARE_PROPERTY_INT(CurHits);
	DECLARE_PROPERTY_INT(HurtCount);
	DECLARE_PROPERTY_STRING(Config);
	
};
typedef HPSingleton<SkillPropertyInterface> SkillPropertyInterfaceSingleton;
#define SPropertyInstance SkillPropertyInterfaceSingleton::Instance()

class BufferPropertyInterface
{ 
#pragma region //Buffer属性
	DECLARE_PROPERTY_STRING(Config);
	DECLARE_PROPERTY_INT(LifeTime);
	DECLARE_PROPERTY_INT(Counter);
	DECLARE_PROPERTY_INT(Active);
	DECLARE_PROPERTY_INT(Timer);
	DECLARE_PROPERTY_INT64(AddTime);
	DECLARE_PROPERTY_OBJECT(Skill);
	DECLARE_PROPERTY_OBJECT(Sender);
#pragma endregion
};

typedef HPSingleton<BufferPropertyInterface> BufferPropertyInterfaceSingleton;
#define BPropertyInstance BufferPropertyInterfaceSingleton::Instance()

class FightPropertyInterface
{
#pragma region //战斗结算属性接口
    DECLARE_PROPERTY_INT64(HP);
    DECLARE_PROPERTY_INT64(MaxHP);
#pragma  endregion

#pragma region //技能流程属性接口
	DECLARE_PROPERTY_INT(CurSkillType);
	DECLARE_PROPERTY_INT(UseBeforeMoveSelfPos);
	DECLARE_PROPERTY_INT(Dead);
	DECLARE_PROPERTY_INT(Flowing);
	DECLARE_PROPERTY_INT(FlowCategory);
	DECLARE_PROPERTY_INT(FlowPrepareTime);
	DECLARE_PROPERTY_INT(FlowLeadTime);
	DECLARE_PROPERTY_INT(FlowLeadSepTime);
	DECLARE_PROPERTY_INT(FlowType);
	DECLARE_PROPERTY_INT(FlowHitTimes);
	DECLARE_PROPERTY_INT(FlowHits);
	DECLARE_PROPERTY_INT(SpeedRatio);

	DECLARE_PROPERTY_FLOAT(CurSkillTargetX);
	DECLARE_PROPERTY_FLOAT(CurSkillTargetY);
	DECLARE_PROPERTY_FLOAT(CurSkillTargetZ);
	DECLARE_PROPERTY_FLOAT(BeforeMoveSelfPosX);
	DECLARE_PROPERTY_FLOAT(BeforeMoveSelfPosY);
	DECLARE_PROPERTY_FLOAT(BeforeMoveSelfPosZ);
	DECLARE_PROPERTY_FLOAT(BeforeMoveSelfPosO);

	DECLARE_PROPERTY_INT64(CurSkillUUID);
	DECLARE_PROPERTY_INT64(FlowBeginTime);
	DECLARE_PROPERTY_INT64(FlowLeadStartTime);

	DECLARE_PROPERTY_OBJECT(CurSkillTarget)
	DECLARE_PROPERTY_OBJECT(CurSkill);
#pragma  endregion

#pragma region //控制类
	DECLARE_PROPERTY_INT(CantMove);
	DECLARE_PROPERTY_INT(CantUseSkill);
	DECLARE_PROPERTY_INT(CantAttack);
	DECLARE_PROPERTY_INT(CantBeAttack);
	DECLARE_PROPERTY_INT(ImmunoControlBuff);
	DECLARE_PROPERTY_INT(AbsorbDamageRate);
	DECLARE_PROPERTY_INT(Silent);
	DECLARE_PROPERTY_INT(FightActionState);
#pragma endregion //控制类

#pragma region // 霸体属性
	DECLARE_PROPERTY_INT(GaintVal);
	DECLARE_PROPERTY_INT(MaxGaintVal);
	DECLARE_PROPERTY_FLOAT(GaintRestoreRate);
#pragma endregion // 霸体属性
public:
	//属性查询
	bool Query(IGameObj* pGameObj,const char* name);
};

typedef HPSingleton<FightPropertyInterface> FightPropertyInterfaceSingleton;
#define FPropertyInstance FightPropertyInterfaceSingleton::Instance()
#endif