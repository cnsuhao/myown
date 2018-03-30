//------------------------------------------------------------------------------
// 文件名:      fight_data_access.h
// 内  容:      战斗数据获取
// 说  明:
// 创建日期:    2013年2月16日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __FIGHTDATAACCESS_H__
#define __FIGHTDATAACCESS_H__

#include "Fsgame/Define/header.h"
#include "FsGame/Define/FightDefine.h"
#include "FsGame/define/staticdatadefine.h"
#include "utils/singleton.h"
#include "utils/cau.h"
#include "SkillDefineDataStruct.h"
#include <cmath>
#include <algorithm>


class FightDataAccess
{
public:
    bool Init(IKernel* pKernel);

public:
    //获取攻击命中类型
    EHIT_TYPE GetHitType(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& src);

	// 获得公式计算的数据
	bool GetFormulaValue(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target, const PERSISTID& skill, int iFormulaIndex, float& fValue);
private:
	// 响应结算伤害前,计算技能公式系数
	static int OnCommandBeforeDamge(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
}; // end of class FightDataAccess


typedef HPSingleton<FightDataAccess> FightDataAccessSingleton;

#endif // __FIGHTDATAACCESS_H__

