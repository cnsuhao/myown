//------------------------------------------------------------------
// 文 件 名:    ActionMutex.h
// 内    容:    玩家行为互斥模块
// 说    明:
// 创 建 人:      
// 创建日期:
//    :        
//-------------------------------------------------------------------
#ifndef _ACTION_MUTEX_H__
#define _ACTION_MUTEX_H__

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ActionMutexDefine.h"

class ActionMutexModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);
    // 关闭
    virtual bool Shut(IKernel* pKernel);

	/*!
	* @brief	是否已有行为
	* @param int 行为类型
	* @return  true已有行为，false无行为
	*/
	static bool IsHaveAction(IKernel * pKernel, const PERSISTID & self, bool show_sysinfo = true);

	/*!
	* @brief	记录行为
	* @param int 行为类型
	*/
	static void NoteAction(IKernel * pKernel, const PERSISTID & self,
		const int action);

	/*!
	* @brief	清除行为
	*/
	static void CleanAction(IKernel * pKernel, const PERSISTID & self);

	/*!
	* @brief	当前是否为指定活动
	* @param int 行为类型
	*/
	static bool IsThisAction(IKernel * pKernel, const PERSISTID & self,
		const int action);

	/*!
	* @brief	行为是否合法
	* @param int 行为类型
	* @return  true合法，false不合法
	*/
	static bool IsValidAction(const int action);

	// 清除行为消息
	static int OnCommandCleanAction(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & var);
};

#endif // _ACTION_MUTEX_H__