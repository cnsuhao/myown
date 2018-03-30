//--------------------------------------------------------------------
// 文件名:      SkillToSprite.h
// 内  容:      NPC及玩家的技能的添加，删除和查询
// 说  明:
// 创建人:       
// 创建日期:    2014年10月24日
//    :       
//--------------------------------------------------------------------
#ifndef FSGAME_SKILLMODULE_SKILL_TO_SPRITE_H_
#define FSGAME_SKILLMODULE_SKILL_TO_SPRITE_H_

#include "Fsgame/Define/header.h"
#include "utils/Singleton.h"


class SkillToSprite
{
public:
    // 获取对象的技能容器对象
    PERSISTID GetSkillContainer(IKernel* pKernel, const PERSISTID& self);

    /// 是否已有技能
    bool Contains(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

    /// 清空技能容器中的技能
    bool ClearAllSkill(IKernel* pKernel, const PERSISTID& self);
   
	/// \brief 查找某角色身上是否有指定的技能
    /// \param pKernel
    /// \param self 添加技能的npc或玩家
    /// \param szSkillConfigID 要查找的技能配置id
    PERSISTID FindSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

    // 添加/删除技能
    bool AddSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);
    bool AddSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);
    bool RemoveSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	// 随机找到一个未冷却的技能
	const PERSISTID FindRandomSkillNotInCD(IKernel* pKernel, const PERSISTID& self, float& fSkillDis);

	// 判定技能是否合法,合法的话取出技能的距离
	bool IsSkillValid(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, float& fSkillDis);

	// 将对象的技能等级都设置为1级
//	void SetSkillLevelMinLevel(IKernel* pKernel, const PERSISTID& self);
private:

    /// 向技能容器中指定位置添加技能
    bool AddSkillToContainer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, int pos = 0);

    /// 删除指定技能容器中的指定技能
    bool RemoveSkillFromContainer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	// 随机找到一个技能id
	const PERSISTID RandomFindSkillId(IKernel* pKernel, const PERSISTID& self);
};

typedef HPSingleton<SkillToSprite> SKillToSpriteSingleton;

#endif // FSGAME_SKILLMODULE_SKILL_TO_SPRITE_H_
