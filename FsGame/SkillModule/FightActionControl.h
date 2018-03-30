//--------------------------------------------------------------------
// 文件名:		FightActionControl.h
// 内  容:		战斗行为状态控制
// 说  明:		
//				
// 创建日期:	2016年04月06日
// 整理日期:	2016年04月06日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __FightActionControl_H__
#define __FightActionControl_H__

#include "Fsgame/Define/header.h"
#include "utils/Singleton.h"
#include <vector>

// 目前3种行为 浮空 > 击退 > 正常 行为优先级高的 覆盖优先级低的 反之则不能覆盖

class FightActionControl
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 改变战斗行为状态
	bool ChangeState(IKernel* pKernel, const PERSISTID& self, int nState, int nStiffTime = 0);

	// 获取当前的行为状态
	int GetFightActionState(IKernel* pKernel, const PERSISTID& self);

	// 重置为正常状态
	void ResetNormalState(IKernel* pKernel, const PERSISTID& self);
private:
	// 移除硬直状态
	static int H_RemoveActionState(IKernel* pKernel,const PERSISTID& self,int slice);
private:
};	

typedef HPSingleton<FightActionControl> FightActionControlSingleton;
#endif //__FightState_H__
