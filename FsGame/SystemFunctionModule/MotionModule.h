//--------------------------------------------------------------------
// 文件名:      MotionModule.h
// 内  容:      移动
// 创建日期:    2014-10-15
// 创建人:      liumf
//    :        
//--------------------------------------------------------------------
#ifndef _MOTION_MODULE_H_
#define _MOTION_MODULE_H_

#include "Fsgame/Define/header.h"
#include <map>
#include "Fsgame/SystemFunctionModule/MotionHelper.h"

// 寻路移动结果
enum MotionResult
{
	MOTION_RESULT_NONE				= 0,	// 没有结果
	MOTION_RESULT_SUCCEED			= 1,	// 移动寻路成功
	MOTION_RESULT_SELF_INEXIST		= 2,	// 自身不存在
	MOTION_RESULT_CANT_FIND_GRID	= 3,	// 找不到落脚点(目标附近没有可使用点可以站)
	MOTION_RESULT_CANT_ARRIVE		= 4,	// 不可到达
	MOTION_RESULT_CANT_MOVE			= 5,	// 当前不可移动
	MOTION_RESULT_CANT_STAND		= 6,	// 目标点不可站立
	MOTION_RESULT_STOP				= 7,	// 停止移动
	MOTION_RESULT_FAILED			= 8,	// 调用引擎移动失败
	MOTION_RESULT_SELF_CLOSE_IN		= 9,	// 自身被包围 无法移动
	MOTION_RESULT_BREAK				= 10,	// 寻路意外中断
};

// 移动回调函数
typedef int (__cdecl* MOTION_CALL_BACK_FUNC)(IKernel* pKernel, const PERSISTID& self, int);
#define REGISTER_MOTION_CALLBACK(back) \
{ \
	MotionModule* pMotionModule = dynamic_cast<MotionModule*>(pKernel->GetLogicModule("MotionModule")); \
	if (pMotionModule != NULL) \
	{ \
		pMotionModule->RegisterMotionCallBackFunction(#back, back); \
	} \
}

/////////////////////////////////////////////////////////////////
// #ifndef FSROOMLOGIC_EXPORTS
// #include <vector>
// using namespace std;
// #include "FsGame/SocialSystemModule/BattleTeamModule.h"
// #endif
/////////////////////////////////////////////////////////////////

enum MotionResult;
class CPoint;
class MotionModule: public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);
private:

	static int OnEntry(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 上线
	static int OnRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 移动回调
	static int OnMotion(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 等待障碍物移开
	static int HB_WaitMotion(IKernel* pKernel, const PERSISTID& self, int time);

	// 间隔一帧执行
	static int HB_MotionEnd(IKernel* pKernel, const PERSISTID& self, int time);

	// 监听速度变化
	static int C_OnSpeedChanged(IKernel* pKernel, const PERSISTID& self,
		const char* property, const IVar& old);

	// 可否移动类型变化
	static int C_OnCantMoveChanged(IKernel* pKernel, const PERSISTID& self,
		const char* property, const IVar& old);

	//请求移动
	static int OnPlayerRequestMove(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 响应请求移动停止
	static int OnRequestMoveStop(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应请求移动
	static int OnRequestMoveMotion(IKernel* pKernel, const PERSISTID& self, int nMMode, const IVarList& args);

	// 踢下线
	static int ForceOffline(IKernel* pKernel, const PERSISTID& self);

	// 客户端消息处理
// 	static int OnCustomHeartBeat(IKernel* pKernel, const PERSISTID& self,
// 		const PERSISTID& sender, const IVarList& args);
public:

	// 注册移动回调函数
	bool RegisterMotionCallBackFunction(const char* strFunc, MOTION_CALL_BACK_FUNC pFunc);


 	//************************************
 	// Method:    MotionToObject
 	// FullName:  MotionModule::MotionToObject
 	// Access:    public 
 	// Returns:   bool
 	// Qualifier: 向目标对象移动
 	// Parameter: IKernel * pKernel 核心类
 	// Parameter: const PERSISTID & self 自身
 	// Parameter: const PERSISTID & target 目标
 	// Parameter: float minDistance 停留最小距离 离目标
 	// Parameter: float maxDistance 停留最大距离 离目标
 	// Parameter: const char * strCallBackFunc 回调函数
 	// Parameter: MotionMethod method 寻路方式
 	//************************************
 	bool MotionToObject(IKernel* pKernel, const PERSISTID& self, 
 		const PERSISTID& target, float minDistance = 0.0f, 
		float maxDistance = 0.0f, const char* strCallBackFunc = "",
		MotionMethod method = MOTION_METHOD_ALL);

	//************************************
	// Method:    MotionToPoint
	// FullName:  MotionModule::MotionToPoint
	// Access:    public 
	// Returns:   bool
	// Qualifier: //向目标点移动
	// Parameter: IKernel * pKernel	核心类	
	// Parameter: const PERSISTID & self 自身
	// Parameter: float targetX 目标点x坐标
	// Parameter: float targetZ 目标点z坐标
	// Parameter: float minDistance 停留最小距离 离目标点
	// Parameter: float maxDistance 停留最大距离 离目标点
	// Parameter: const char * strCallBackFunc 回调函数
	// Parameter: MotionMethod method 寻路方式
	//************************************
	bool MotionToPoint(IKernel* pKernel, const PERSISTID& self,
		float targetX, float targetZ, float minDistance = 0.0f, 
		float maxDistance = 0.0f, const char* strCallBackFunc = "",
		MotionMethod method = MOTION_METHOD_ALL);

private:
	// 向目标点移动
	// bForecastCollision 提前预测是否会发生碰撞
	bool ContinueMotion(IKernel* pKernel, const PERSISTID& self, bool bForecastCollision = true);
	// 直线到达格子
	MotionResult MotionLineToGrid(IKernel* pKernel, const PERSISTID& self, int gridIndex);
	// 移动成功(可能到达临时目标点，可能已到达终点，通过继续移动函数进一步检测)
	bool MotionSucceed(IKernel* pKernel, const PERSISTID& self);
	// 避让成功
	bool RoundBarrierSucceed(IKernel* pKernel, const PERSISTID& self);
	// 碰撞处理
	MotionResult DealCollision(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender);
	// 绕过障碍物
	MotionResult RoundBarrier(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender);
	// 无法到达目标点后的处理
	MotionResult DealCantArriveTarget(IKernel* pKernel, const PERSISTID& self);
	// 寻路结束
	// 只允许在三个接口中调用 MotionToPoint ContinueMotion MotionStop
	// 目的是保证寻路必然有寻路结果 否则很容易在一些安全类判断中忽略而导致寻路中断又没有寻路结果
	bool MotionEnd(IKernel * pKernel, const PERSISTID & self, MotionResult result);
	bool MotionEnd(IKernel * pKernel, const PERSISTID & self);

	// 重置寻路状态参数
	// 返回重置前的结果
	int ResetMotionState(IKernel * pKernel, const PERSISTID & self);

	// 取当前所需到达的方格
	int GetNeedArriveGrid(IKernel * pKernel, const PERSISTID & self);
	// 寻找离目标最近的可使用方格
	int GetTargetNearestEnableGrid(IKernel * pKernel, const PERSISTID & self);
	// 在所有象限内查找离目标最近的有效点
	int GetNearestEnableGridInAllQuadrant(IKernel * pKernel,
		const PERSISTID & self,
		float targetX, float targetZ,
		float minDistance, float maxDistance);
	// 获取象限内离目标最近的有效方格
	int GetNearestEnableGridInQuadrant(IKernel * pKernel, 
		const PERSISTID & self,
		CPoint& vectorPoint, CPoint& targetPoint, 
		int length, float targetX, float targetZ,
		float minDistance, float maxDistance,int rowTotal);


	// 寻找自身附近可行走方格
	int GetSelfEnableGrid(IKernel * pKernel, const PERSISTID & self);
	// 获取自身附近方格优先级表
	bool GetSelfGridList(IKernel * pKernel, const PERSISTID & self, int targetGrid, IVarList& gridList, int step = 1);
	// 检测自身附近最优行走点是否可行走
	bool NeedDealCollision(IKernel * pKernel, const PERSISTID & self, int targetGrid, PERSISTID& collision);

	// 寻路,找一条完整路径
	bool FindPathToGrid(IKernel* pKernel, const PERSISTID& self, int targetGrid);
	// 动态寻路，通过A*寻路
	bool FindPathToGridDynamic(IKernel* pKernel, const PERSISTID& self, int targetGrid);
	// 静态寻路，通过场景配置路径寻路
	bool FindPathToGridStatic(IKernel* pKernel, const PERSISTID& self, float targetX, float targetZ);
	// 获取动态寻路的路经点
	int GetPathGrid_loops(IKernel* pKernel, const PERSISTID& self);

	// 测试是否能够直线到达格子(只考虑场景碰撞)
	bool WalkLineEnabled(IKernel* pKernel, const PERSISTID& self, float targetX, float targetZ);

	// 能否站立
	bool CanStand(IKernel* pKernel, float x, float z, IVarList& heightList);
	bool CanStandFast(IKernel* pKernel, float x, float z);
	// 能否移动
	bool CanMove(IKernel * pKernel, const PERSISTID & target);

	// 刷新速度
	void UpdateMoveSpeed(IKernel* pKernel, const PERSISTID& self);
public:
	static MotionModule* m_pMotionModule;

private:
	typedef std::map<std::string, MOTION_CALL_BACK_FUNC> MapCallBack;
	static MapCallBack m_mapCallBack;

public:
	static double m_distTotalWarning;	// 玩家位置误差校正警告值
	static double m_distTotalMax;		// 玩家位置误差校正最大值
	static int m_heartBeatWarning;		// 心跳警告值
	static int m_heartBeatMax;			// 心跳最大值
	static int m_cantLoginTime;			// 强制下线后 不能登录的时间
	static bool m_bCheckPlayerState;	// 是否检测玩家状态异常

#ifndef FSROOMLOGIC_EXPORTS
private:// 国战碰撞
	int OnMotionBattleTeam(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
#endif

public: // 外部使用
	
	//************************************
	// Method:    GetGridCanStandPoint
	// Access:    public static 
	// Returns:   int 格子索引
	// Qualifier: 查找目标点附近的可使用坐标
	// Parameter: IKernel * pKernel 核心类
	// Parameter: const PERSISTID & self 自身
	// Parameter: float destX	目标点x坐标
	// Parameter: float destZ	目标点z坐标
	// Parameter: float & nearX 目标点附近可使用x坐标
	// Parameter: float & nearZ 目标点附近可使用z坐标
	//************************************
	static int GetEnableGridNearTarget(IKernel* pKernel, const PERSISTID& self, float destX, float destZ, float& nearX, float& nearZ);

	//方格是否可用
	static bool GridEnabled(IKernel * pKernel, const int iIndex, int groupId);

	// 方格是否可用 
	// 相对于self 是否可用
	// self所占方格数不同，格子的占用情况不同
	// iIndex 当前占用格子
	// bSpace 是否查看逻辑占格
	static bool GridEnabled(IKernel * pKernel,const PERSISTID& self, const int iIndex, bool bSpace = true); 

	// 相互间的间隔不允许占用
	static bool IsGridSpace(IKernel* pKernel, const PERSISTID& self, const IVarList& gridList);

	// 停止移动
	static bool MotionStop(IKernel* pKernel, const PERSISTID& self);
};

#endif // _PLAYER_BASE_MODULE_H_
