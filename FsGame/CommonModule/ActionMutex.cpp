//------------------------------------------------------------------
// 文 件 名:    ActionMutex.cpp
// 内    容:    玩家行为互斥模块
// 说    明:
// 创 建 人:      
// 创建日期:    
//    :        
//-------------------------------------------------------------------

#include "ActionMutex.h"

#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/custom_func.h"

// 清除行为
inline int reset_action_mutex(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, reset_action_mutex, 1);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, reset_action_mutex, 1);

	// 获取参数
	PERSISTID player = pKernel->LuaToObject(state, 1);   
	IGameObj* pSelfObj = pKernel->GetGameObj(player);
	if (pSelfObj == NULL)
	{
		pKernel->LuaPushBool(state, false);
		return 0;
	}

	pKernel->LuaPushBool(state, pSelfObj->SetInt(FIELD_PROP_CUR_ACTION, ACTION_NONE));

	return 0;
}

// 初始化
bool ActionMutexModule::Init( IKernel* pKernel )
{
	// 清除行为消息
	pKernel->AddIntCommandHook("player", COMMAND_RESET_ACTION_MUTEX, OnCommandCleanAction);

	DECL_LUA_EXT(reset_action_mutex);
	
	return true;
}

// 关闭
bool ActionMutexModule::Shut( IKernel* pKernel )
{
	return true;
}

/*!
* @brief	是否已有行为
* @param int 行为类型
* @return  true已有行为，false无行为
*/
bool ActionMutexModule::IsHaveAction(IKernel * pKernel, const PERSISTID & self, bool show_sysinfo/* = true*/)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	// 当前行为
	const int cur_action = pSelfObj->QueryInt(FIELD_PROP_CUR_ACTION);

	// 无行为
	if (cur_action == ACTION_NONE)
	{
		return false;
	}

	// 系统提示：#{cur_action}进行中，不可进行其他操作
	if (show_sysinfo)
	{
		// 拼接key
		//char key[128] = { 0 };
		//SPRINTF_S(key, "%s_%d", STR_ACTION_MUTEX_SYSINFO_PROMPT, cur_action);

		int system_id = SYSTEM_INFO_ID_3001 - 1;
		system_id += cur_action;
		::CustomSysInfo(pKernel, self, system_id, CVarList());
	}

	return true;
}

/*!
* @brief	记录行为
* @param int 行为类型
*/
void ActionMutexModule::NoteAction(IKernel * pKernel, const PERSISTID & self, 
	const int action)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	// 不合法
	if (!IsValidAction(action))
	{
		return;
	}

	pSelfObj->SetInt(FIELD_PROP_CUR_ACTION, action);
}

/*!
* @brief	清除行为
*/
void ActionMutexModule::CleanAction(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return;
	}

	if (pSelfObj->QueryInt(FIELD_PROP_CUR_ACTION) != ACTION_NONE)
	{
		pSelfObj->SetInt(FIELD_PROP_CUR_ACTION, ACTION_NONE);
	}
}

/*!
* @brief	当前是否为指定活动
* @param int 行为类型
*/
bool ActionMutexModule::IsThisAction(IKernel * pKernel, const PERSISTID & self, 
	const int action)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	if (pSelfObj->GetClassType() != TYPE_PLAYER)
	{
		return false;
	}

	// 不合法
	if (!IsValidAction(action))
	{
		return false;
	}

	if (action == pSelfObj->QueryInt(FIELD_PROP_CUR_ACTION))
	{
		return true;
	}

	return false;
}

/*!
* @brief	行为是否合法
* @param int 行为类型
* @return  true合法，false不合法
*/
bool ActionMutexModule::IsValidAction(const int action)
{
	return (action > ACTION_NONE && action < ACTION_MAX);
}

// 清除行为消息
int ActionMutexModule::OnCommandCleanAction( IKernel * pKernel, const PERSISTID & self, 
	const PERSISTID & sender, const IVarList & var )
{
	// 清除行为
	CleanAction(pKernel, self);

	return 0;
}
