#ifndef _SCENEBASE_MODULE_H_
#define _SCENEBASE_MODULE_H_

#include "Fsgame/Define/header.h"

#define DOMAIN_SCENELIST	L"Domain_SceneList"

class SceneBaseModule: public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);
public:
	//属性
	static int OnCreateClass(IKernel* pKernel, int index);
	
	//场景创建
	static int OnCreate(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	//场景销毁
	static int OnDestroy(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	//进入场景
	static int OnEntry(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	//离开场景
	static int OnLeave(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 将所有玩家都踢出当前场景(恢复到进入场景)
	static void KickAllPlayerOutScene( IKernel* pKernel, int nGroup );
	// 销毁所有非玩家相关的对象
	static void ClearNonPlayerInScene(IKernel* pKernel, int nGroup);
};

#endif // _SCENEBASE_MODULE_H_
