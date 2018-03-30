

#ifndef __AsynCtrlModule_H__
#define __AsynCtrlModule_H__

#include "Fsgame/Define/header.h"
#include <map>

#define ASYNCTRLMODULE_CUSTOM_STRING_1       "ui_form_asyn_ctrl_module_1"   // 进入场景所需等级不足{0:EnterLevel}

// 克隆场景
const static int CLONE_SCENE = 1024; 

// 调用异步的枚举
enum ASYN_TYPE
{
	ASYN_TYPE_SWITCHLOCATE,							// 传送
};

class AsynCtrlModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

	// 进入场景
	static int OnReady(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	//
	static int OnContinue(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

public:
	// 是否等候异步操作的过程中
	bool IsWaiting(IKernel* pKernel, const PERSISTID& self);

	// 开始等候异步操作的过程
	bool BeginWait(IKernel* pKernel, const PERSISTID& self, int asyn_type);

	// 结束等候异步操作的过程
	bool EndWait(IKernel* pKernel, const PERSISTID& self);

	// 是否可切换场景DestSceneID目标场景原场景
	bool CanSwitchLocate(IKernel* pKernel, const wchar_t* selfName, const int DestSceneID);

	// 场景切换
	bool SceneToScene(const int curScene, const int targetScene);

	//*********************请求切场景*****************************
	// 切换场景到指定的场景号和坐标
	//bFlag=true 强行传送
	bool SwitchLocate(IKernel* pKernel, const PERSISTID& self, int scene, 
		float x, float y, float z, float orient, const bool bFlag = false);

	// 切换到场景的出生点
	//bFlag=true 强行传送
	bool SwitchBorn(IKernel* pKernel, const PERSISTID& self, int scene, const bool bFlag = false);

	// 移动到当前场景出生点
	bool MoveToBorn(IKernel* pKernel, const PERSISTID& self, int scene);

private:
	// 异步切换场景的心跳
	static int H_Asyn_SwitchLocate(IKernel* pKernel, const PERSISTID& self, int slice);

	static int H_Asyn_SwitchBorn(IKernel* pKernel, const PERSISTID& self, int slice);

public:
	// 载入资源
	bool LoadResource(IKernel *pKernel);

private:
	// 冲突场景信息key当前场景ID,元素为冲突场景ID
	std::map<int, std::string> m_MapConflictScene;

public:
	static AsynCtrlModule* m_pAsynCtrlModule;

};

#endif	// __AsynCtrlModule_H__