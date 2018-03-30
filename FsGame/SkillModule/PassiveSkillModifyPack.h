//--------------------------------------------------------------------
// 文件名:      PassiveSkillModifyPack.h
// 内  容:      被动技能属性包逻辑
// 说  明:
// 创建日期:    2016年3月3日
// 创建人:		 刘明飞
//    :       
//--------------------------------------------------------------------

#ifndef _PASSIVESKILLMODIFYPACK_H_
#define _PASSIVESKILLMODIFYPACK_H_

#include "Fsgame/Define/header.h"
#include "Fsgame/Define/ModifyPackDefine.h"
#include "utils/Singleton.h"

class PassiveSkillModifyPack
{
public:
    /**
    @brief  初始化
    @param  pKernel [IN] 引擎核心指针
    @return 返回初始化结果
    */
   bool Init(IKernel* pKernel);
private:
	// 更新被动技能影响人物的属性
	static int AddPassiveSkillModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 移除被动技能影响人物的属性
	static int RemovePassiveSkillModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
};

typedef HPSingleton<PassiveSkillModifyPack> PassiveSkillModifyPackSingleton;

#endif
