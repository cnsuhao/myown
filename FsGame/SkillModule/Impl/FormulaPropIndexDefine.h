
//--------------------------------------------------------------------
// 文件名:      FormulaPropIndexDefine.h
// 内  容:      
// 说  明:      
// 创建日期:    2015年7月28日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef __FORMULA_PROP_INDEX_DEFINE_H__
#define __FORMULA_PROP_INDEX_DEFINE_H__


enum EFormulaPropType
{
	// fight property
	FORMULA_PROP_PhysAttack,
	FORMULA_PROP_MagicAttack,
    FORMULA_PROP_Hit,
    FORMULA_PROP_Miss,
    FORMULA_PROP_Crit,
    FORMULA_PROP_Toug,
	FORMULA_PROP_PhysDefend,
	FORMULA_PROP_MagicDefend,
	FORMULA_PROP_PhysDefendRate,
	FORMULA_PROP_MagicDefendRate,
	FORMULA_PROP_DecHurtRate,
	FORMULA_PROP_SkillAttackAdd,
	FORMULA_PROP_SkillDamageRate,
	FORMULA_PROP_CritRate,
	FORMULA_PROP_TougRate,
	FORMULA_PROP_AddHurtRate,

    // fight formula
	FORMULA_PROP_BasePhysAttackRate,
	FORMULA_PROP_BasePhysAttackDamage,
	FORMULA_PROP_BaseMagicAttackRate,
	FORMULA_PROP_BaseMagicAttackDamage,
	FORMULA_PROP_TrueCritRate,
	FORMULA_PROP_CritDamageRate,
	FORMULA_PROP_CritPhysDamage,
	FORMULA_PROP_CritMagicDamage,
	FORMULA_PROP_TrueHitRate,

	FORMULA_PROP_P101,
	FORMULA_PROP_P102,
	FORMULA_PROP_P103,
	FORMULA_PROP_P104,
// 	FORMULA_PROP_S5,
// 	FORMULA_PROP_S6,
// 	FORMULA_PROP_S7,
// 	FORMULA_PROP_S8,
// 	FORMULA_PROP_S9,
// 	FORMULA_PROP_S10,
// 	FORMULA_PROP_S11,
// 	FORMULA_PROP_S12,
// 	FORMULA_PROP_S13,
// 	FORMULA_PROP_S14,
// 	FORMULA_PROP_S15,
// 	FORMULA_PROP_S16,
// 	FORMULA_PROP_S17,
// 	FORMULA_PROP_S18,
// 	FORMULA_PROP_S19,
// 	FORMULA_PROP_S20,

    FORMULA_PROP_MAX,
};

#endif // __FORMULA_PROP_INDEX_DEFINE_H__
