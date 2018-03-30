//--------------------------------------------------------------------
// 文件名:      AIFunction.h
// 内  容:      AI系统行为处理
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef _AIFUNCTION_H_
#define _AIFUNCTION_H_

#include "Fsgame/Define/header.h"
#include "FsGame/SystemFunctionModule/MotionModule.h"
#include "AISystem.h"

class AISystem;
class AIFunction
{
public:
	static bool Init(IKernel* pKernel);

	static bool Shut(IKernel* pKernel);
public:
	//移动到目标点（每次移动清除一条数据 FindPathPointList）
	static int StartFindPathMove(IKernel* pKernel, const PERSISTID& self);

	//跑向一个目标点(通过寻路生成FindPathPointList)
	static int StartMoveToPoint(IKernel* pKernel, const PERSISTID& self, float posX, float posZ);

	// 追逐目标
	static int	StartMoveToObject(IKernel * pKernel, const PERSISTID & self, const PERSISTID& target, 
								  float minDistance, float maxDistance);

	//巡逻移动到下一个目标点(直接PatrolPointRec寻路移动)
	// bNeedDynaicFindPath是否需要动态寻路
	static bool StartPatrolToPoint(IKernel* pKernel, const PERSISTID& self, float posX, float posZ, bool bNeedDynaicFindPath = true);

	//NPC移动响应
	static int MotionCallBack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//初始化技能
	static int InitSkill(IKernel* pKernel, const PERSISTID & self, const char* skillStr);

	//生成怪物的行动路径点
	static bool CreateNpcPath(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    //计算当前回出生点（FindPathPointList）路程总时间(单位是毫秒)
    static int CountFindPathPointListTime(IKernel *pKernel, const PERSISTID &self);

	// 移动回调
	static int MotionCallBackFunction(IKernel* pKernel, const PERSISTID& self, int result);
private:
	static AISystem * m_pAISystem;
};
#endif
