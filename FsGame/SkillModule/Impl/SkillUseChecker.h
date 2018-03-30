//--------------------------------------------------------------------
// 文件名:      Server\FsGame\SkillModule\impl\skillUseChecker.h
// 内  容:      检查技能使用的限制条件的实现函数全部定义于此
// 说  明:
// 创建日期:	2014年10月31日
// 创建人:		 
//    :       
//--------------------------------------------------------------------

#if !defined __SkillUseChecker_h__
#define __SkillUseChecker_h__

#include "Fsgame/Define/header.h"
#include "utils/math/math.h"
#include "utils/Singleton.h"

class SkillUseChecker
{
public:

    /**
    *@brief  判断是否可以使用技能
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 施法者
    *@param  [in] x,y,z,orient 技能释放时施法者位置和朝向
    *@param  [in] skill 技能对象
    *@param  [in] target 技能作用对象
    *@param  [in] dx,dy,dz 技能作用点位置
    *@return int 返回1表示可以释放，其他值代表对应才错误号返回
    */
    int CanUseSkill(IKernel* pKernel, const PERSISTID& self,
                    float x, float y, float z, float orient,
                    const PERSISTID& skill, const PERSISTID& target,
                    float dx, float dy, float dz);

    // 射程判断
    int CheckCastRange(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill);

    //是否满足关系需求
    bool CheckSkillRelation(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const PERSISTID& target);

    // 判断技能高绩效包对象关系
    bool CheckEventEffectRelation(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const char* szSkillEffectPackID);

private:
    // 特殊的限制技能释放的属性判断
	int HasPropertyLimit(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill);
}; // end of class SkillUseChecker

typedef HPSingleton<SkillUseChecker>  SkillUseCheckerSingleton;


#endif
