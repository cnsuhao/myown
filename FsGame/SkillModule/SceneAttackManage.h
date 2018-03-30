//--------------------------------------------------------------------
// 文件名:		SceneAttackManage.h
// 内  容:		场景可攻击管理器
// 说  明:		
//				
// 创建日期:	2015年05月28日
// 创建人:		 
//    :	    
//--------------------------------------------------------------------
#ifndef __SceneAttackManage_H__
#define __SceneAttackManage_H__

#include <vector>
#include "Fsgame/Define/header.h"
#include "FsGame/Define/PKModelDefine.h"

class TeamModule;

//场景攻击模式类型判断函数
typedef bool(__cdecl* SCENE_PK_TYPE_FUNC)(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

class SceneAttackManage
{
public:
    ~SceneAttackManage();

    //初始化
    static void Init(IKernel *pKernel);

    // 验证目标关系类型
	static bool CheckTargetRelation(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);
private:
    // 私有
    SceneAttackManage();

    // 私有的拷贝构造函数
    SceneAttackManage(const SceneAttackManage &a);

    // 私有的赋值函数
    SceneAttackManage & operator =(const SceneAttackManage &a);

    //注册攻击模式类型判断函数
    static bool RegisterJudgeFun(SceneAttackType type, SCENE_PK_TYPE_FUNC fun);

//---------------------------------------------------------------------------------------------------
    // 自定义阵营关系判断
	static bool CheckCampPK(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

	// 自定义公会关系判断
	//static bool CheckGuildPK(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

	// 和平
	static bool CheckPeace(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

	// 只能PVE
	static bool CheckPVE(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

	//PK模式逻辑
	static bool CheckByPlayerPKModel(IKernel *pKernel, const PERSISTID &self, const PERSISTID &target, TestRelationType nTestRelationType);

	// 公共场景判断关系的接口
	// 是否为友方
	static bool IsFriendlyInPublicScene(IKernel *pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj);

	// 是否为敌方
	static bool IsEnemyInPublicScene(IKernel *pKernel, IGameObj* pSelfObj, IGameObj* pTargetObj);

	// 以下判定,都是在目标是玩家并且不是队友的情况下
	// 和平模式判定
	static bool IsEnemyInAttCrimeModel(IGameObj* pSelfObj, IGameObj* pTargetObj);

	// 公会模式下的判定																																												 
	//static bool IsEnemyInGuildModel(IGameObj* pSelfObj, IGameObj* pTargetObj);
//--------------------------------------------------------------------------------------------------    
private:
	// 模块指针
	static TeamModule* m_pTeamModule;

    //判断函数地址表
    static std::vector<SCENE_PK_TYPE_FUNC> m_funCheckRelationList;
};


#endif //__SceneAttackManage_H__