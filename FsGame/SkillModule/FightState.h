//--------------------------------------------------------------------
// 文件名:		FightState.h
// 内  容:		战斗状态
// 说  明:		
//				
// 创建日期:	2014年06月27日
// 整理日期:	2014年06月27日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __FightState_H__
#define __FightState_H__

#include "Fsgame/Define/header.h"

class FightState : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

public:
	/// \brief 就绪
	static int OnReady(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

private:
	// 更新玩家战斗状态
	static int OnCommandUpdateFightState(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

public:
	//重置战斗状态
	static int HB_ResetFightState(IKernel * pKernel, const PERSISTID & self,
		int slice);

private:
	static FightState* m_pFightState;
};

#endif //__FightState_H__
