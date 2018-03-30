
//--------------------------------------------------------------------
// 文件名:      FormulaPropIndex.cpp
// 内  容:      
// 说  明:      
// 创建日期:    2015年7月28日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#include "FormulaPropIndex.h"
#include "FormulaPropIndexDefine.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"

TStringPod<char, int>  FormulaPropIndex::s_prop_to_index;
TArrayPod<const char*, 1> FormulaPropIndex::s_index_to_prop;

#define REG_PROP_INDEX(prop)        \
    s_prop_to_index.Add(#prop, FORMULA_PROP_##prop);\
    s_index_to_prop[FORMULA_PROP_##prop] = #prop

void FormulaPropIndex::RegisterPropIndex()
{
    s_index_to_prop.resize(FORMULA_PROP_MAX, ""); // 先设置容器大小，防止push_back元素的顺序依赖枚举值定义顺序

	// fight property
	REG_PROP_INDEX(PhysAttack);
	REG_PROP_INDEX(MagicAttack);
	REG_PROP_INDEX(PhysDefend);
	REG_PROP_INDEX(MagicDefend);
	REG_PROP_INDEX(Hit);
	REG_PROP_INDEX(Miss);
	REG_PROP_INDEX(Crit);
	REG_PROP_INDEX(Toug);
	REG_PROP_INDEX(DecHurtRate);
	REG_PROP_INDEX(PhysDefend);
	REG_PROP_INDEX(MagicDefend);
	REG_PROP_INDEX(PhysDefendRate);
	REG_PROP_INDEX(MagicDefendRate);
	REG_PROP_INDEX(SkillAttackAdd);
	REG_PROP_INDEX(SkillDamageRate);
	REG_PROP_INDEX(CritRate);
	REG_PROP_INDEX(TougRate);
	REG_PROP_INDEX(AddHurtRate);

	// fight formula
	REG_PROP_INDEX(BasePhysAttackRate);
	REG_PROP_INDEX(BasePhysAttackDamage);
	REG_PROP_INDEX(BaseMagicAttackRate);
	REG_PROP_INDEX(BaseMagicAttackDamage);
	REG_PROP_INDEX(TrueCritRate);
	REG_PROP_INDEX(CritDamageRate);
	REG_PROP_INDEX(CritPhysDamage);
	REG_PROP_INDEX(CritMagicDamage);
	REG_PROP_INDEX(TrueHitRate);

	REG_PROP_INDEX(P101); 
	REG_PROP_INDEX(P102);
	REG_PROP_INDEX(P103);
	REG_PROP_INDEX(P104);
};

short FormulaPropIndex::ToPropIndex(const char* prop)
{
    if (StringUtil::CharIsNull(prop))
    {
        return -1;
    }

    TStringPod<char, int>::const_iterator it = s_prop_to_index.Find(prop);
    if (it == s_prop_to_index.End())
    {
        return -1;
    }
    return (short)it.GetData();
}

const char* FormulaPropIndex::ToPropName(int prop_index)
{
    if (prop_index < 0 || prop_index >= FORMULA_PROP_MAX)
    {
        return "";
    }
    if ((int)s_index_to_prop.size() <= prop_index)
    {
        ::extend_warning(LOG_ERROR,
            "[FormulaPropIndex::ToPropName] check function RegisterPropIndex.prop_index:%d", prop_index);
        return "";
    }
    return s_index_to_prop[prop_index];
}
