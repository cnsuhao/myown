//------------------------------------------------------------------------------
// 文件名:      PVPRecAttacker.h
// 内  容:		pvp攻击记录
// 说  明:
// 创建日期:    2017年10月17日
// 创建人:          
//------------------------------------------------------------------------------

#ifndef __PVP_REC_ATTACKER_H__
#define __PVP_REC_ATTACKER_H__

#include "Fsgame/Define/header.h"
#include "utils/singleton.h"

class PVPRecAttacker
{
public:
    bool Init(IKernel* pKernel);
public:
    //获取助攻对象列表 killer为空,返回全部攻击者,不为空会剔除掉
    bool GetAttackerList(IKernel* pKernel, IVarList& outAttackerList, const PERSISTID& self, const PERSISTID& killer);
private:
	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应被伤害
	static int OnCommandBeDamage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//战斗状态发生改变
	static int C_OnFightStateChanged(IKernel *pKernel, const PERSISTID &self, const char *property, const IVar &old);
}; // end of class PVPRecAttacker


typedef HPSingleton<PVPRecAttacker> PVPRecAttackerSingleton;

#endif // __FIGHTDATAACCESS_H__

