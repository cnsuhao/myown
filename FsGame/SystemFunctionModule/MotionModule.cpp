//--------------------------------------------------------------------
// 文件名:      MotionModule.cpp
// 内  容:      移动
// 创建日期:    2014-10-15
// 创建人:      liumf
// 修改日期:	2015-5-6
// 修改人:		 
// 修改内容:	重构
//    :        
//--------------------------------------------------------------------
#include "MotionModule.h"
#include "utils/util_func.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/geometry_func.h"
#include "utils/string_util.h"
#include <ctime>
#include "atltypes.h"
#include "server/LoopCheck.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/SwitchManagerModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "utils/custom_func.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
//#include "FsGame/Define/PlayerSysInfoDefine.h"
#include "FsGame/SystemFunctionModule/PlayerBaseModule.h"
#include "FsGame/CommonModule/PathFindingModule.h"
#include "FsGame/CommonModule/LogModule.h"
//#include "FsGame/Define/OffLineDefine.h"
#include "../CommonModule/EnvirValueModule.h"

#include "utils/cache/CacheHelper.hpp"

#define MAX_SKILL_SPEED 40.0f

// 此文件中用到的定义，其他文件不可以使用
// 所以不要放入头文件中
enum MotionState
{
	MOTION_STATE_STOP		= 0,	// 停止移动
	MOTION_STATE_WAIT		= 1,	// 等待移动
	MOTION_STATE_MOVING		= 2,	// 正在移动	
};

// 等待障碍物移开时间
const int WAIT_MOTION_TIME = 500;

// 绕开障碍物所走过的格子表
const char* const ROUND_PATH_LIST_RECORD = "RoundPathListRec";
enum
{
	ROUND_PATH_LIST_RECORD_COL_grid = 0,	// 格子索引	
	ROUND_PATH_LIST_RECORD_COL_count = 1,	// 避让的次数
};
// 记录所走过的格子数量
const int ROUND_PATH_COUNT = 5;
// 尝试绕开障碍物的最大次数
const int ROUND_MAX_COUNT = 5;
// 绕开障碍物总次数,达到此次数后判定无法到达
const int ROUND_TOTAL_COUNT_MAX = 50;

// 动态寻路路径表
const char* const FIND_ROUND_PATH_RECORD = "FindRoundPathRec";
enum
{
	FIND_ROUND_PATH_RECORD_COL_grid = 0, // 格子索引
	FIND_ROUND_PATH_RECORD_COL_count = 1, // 获取次数
};
const int FIND_ROUND_PATH_MAX_COUNT = 10; // 寻路表中的寻路点最大被获取的次数

// 当前寻路到达不了的点
const char* const CANT_ARRIVE_GRID_LIST_RECORD = "CantArriveGridListRec";
enum
{
	CANT_ARRIVE_GRID_LIST_RECORD_COL_grid = 0, // 格子索引
};

// 移动校正监测心跳时间
const int CHECK_DIST_TOTAL_TIME = 60000;

// 检测客户端异常数据
double MotionModule::m_distTotalWarning = 0.0f;		// 移动校正警告值
double MotionModule::m_distTotalMax = 0.0f;			// 移动校正最大值
int MotionModule::m_heartBeatWarning = 0;			// 心跳警告值
int MotionModule::m_heartBeatMax = 0;				// 心跳最大值
int MotionModule::m_cantLoginTime = 0;				// 不允许登录时间
bool MotionModule::m_bCheckPlayerState = true;		// 是否检测玩家状态

MotionModule::MapCallBack MotionModule::m_mapCallBack;
MotionModule * MotionModule::m_pMotionModule = NULL;

// 输出服务器的层信息
int nx_floor_info(void* state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_floor_info, 1);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_floor_info, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	if (!pKernel->Exists(self))
	{
		return 0;
	}

	len_t x = pKernel->GetPosiX(self);
	len_t z = pKernel->GetPosiZ(self);
	len_t y = pKernel->GetPosiY(self);

	/// \brief获得格子行数
	int nRows = (int)pKernel->GetPathGridRows();
	/// \brief获得格子列数
	int nCols = (int)pKernel->GetPathGridCols();

	::CustomSysInfo(pKernel, self, 0,
		CVarList() << (std::string("position: ") + util_double_as_string(x) + " " + util_double_as_string(y).c_str() + " " + util_double_as_string(z).c_str()));

	::CustomSysInfo(pKernel, self, 0,
		CVarList() << "FloorIndex|CanMove|CanStand|FloorHeight|FloorSpace");

	::CustomSysInfo(pKernel, self, 0,
		CVarList() << (std::string("total rows: ") + util_int_as_string(nRows).c_str() + std::string("total cols: ") + util_int_as_string(nCols).c_str()));

	// 遍历层
	int count = pKernel->GetFloorCount(x, z);

	LoopBeginCheck(a);
	for (int i = count - 1; i >= 0; --i)
	{
		LoopDoCheck(a);
		if (pKernel->GetFloorExists(x, z, i))
		{
			len_t height = pKernel->GetFloorHeight(x, z, i);
			len_t space = pKernel->GetFloorSpace(x, z, i);
			bool can_move = pKernel->GetFloorCanMove(x, z, i);
			bool can_stand = pKernel->GetFloorCanStand(x, z, i);

			std::string result;

			result = StringUtil::Int64AsString(i);
			result += "|";
			result += can_move ? "true" : "false";
			result += "|";
			result += can_stand ? "true" : "false";
			result += "|";
			result += util_double_as_string(height);
			result += "|";
			result += util_double_as_string(space);

			// 输出层信息
			::CustomSysInfo(pKernel, self, 0, CVarList() << result.c_str());
		}
	}

	return 0;
}

// 移动到目标点
inline int nx_motion_point(void *state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	if (pKernel == NULL)
	{
		return 1;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_motion_point, 3);

	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_motion_point, 1);
	CHECK_ARG_FLOAT(state, nx_motion_point, 2);
	CHECK_ARG_FLOAT(state, nx_motion_point, 3);
	PERSISTID player = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float z = pKernel->LuaToFloat(state, 3);

	// 玩家对象
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return 1;
	}

	pPlayerObj->SetInt("MotionState", MOTION_STATE_STOP);
	MotionModule::m_pMotionModule->MotionToPoint(pKernel, player, x, z);

	return 1;
}

// 设置检测心跳
// 参数 1 int 检测心跳警告值 -1表示关闭检测
// 参数 2 int 检测心跳最大值 超过踢下线
// 参数 3 double 检测移动校正值警告值
// 参数 4 double 检测移动校正值最大值 超过踢下线
inline int nx_check_player_state(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	// 心跳警告值
	int heartWarning = args.IntVal(0);
	if (heartWarning < 0) 
	{
		MotionModule::m_bCheckPlayerState = false;
		return 0;
	}
	
	MotionModule::m_bCheckPlayerState = true;
	MotionModule::m_heartBeatWarning = heartWarning;

	// 心跳最大值
	int heartMax = args.IntVal(1);
	if (heartMax > 0)
	{
		MotionModule::m_heartBeatMax = heartMax;
	}

	// 移动校正值警告值
	double distWarning = args.DoubleVal(2);
	if (distWarning > 0)
	{
		MotionModule::m_distTotalWarning = distWarning;
	}

	// 移动校正值最大值
	double distMax = args.DoubleVal(3);
	if (distMax > 0)
	{
		MotionModule::m_distTotalMax = distMax;
	}
	
	return 0;
}

// 初始化
bool MotionModule::Init(IKernel* pKernel)
{
	m_pMotionModule = this;
	
	Assert(m_pMotionModule != NULL);

	pKernel->AddEventCallback("player", "OnRequestMove", OnPlayerRequestMove);
	pKernel->AddEventCallback("player", "OnMotion", OnMotion);
	pKernel->AddEventCallback("NormalNpc", "OnMotion", OnMotion);
	pKernel->AddEventCallback("player", "OnRecover", OnRecover, -1);
	pKernel->AddEventCallback("player", "OnContinue", OnRecover, -1);
	pKernel->AddEventCallback("NormalNpc","OnEntry", OnEntry, 999);

	// 客户端消息
	//pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_HEART_BEAT, OnCustomHeartBeat);

	DECL_CRITICAL(MotionModule::C_OnSpeedChanged);
	DECL_CRITICAL(MotionModule::C_OnCantMoveChanged);

	DECL_HEARTBEAT(MotionModule::HB_WaitMotion); 
	DECL_HEARTBEAT(MotionModule::HB_MotionEnd); 
// 	DECL_HEARTBEAT(MotionModule::HB_UpdatePlayerState);
// 	DECL_HEARTBEAT(MotionModule::HB_CheckMotionBreak);

	///////////////////////////////////////////////////////////////////////
// #ifndef FSROOMLOGIC_EXPORTS
// 	DECL_HEARTBEAT(MotionModule::HB_TryMotion);
// #endif // FSROOMLOGIC_EXPORTS
	//////////////////////////////////////////////////////////////////////

	// 读取玩家位置校正警告值和最大值
	m_distTotalWarning = static_cast<double>(EnvirValueModule::EnvirQueryInt(ENV_VALUE_CHECK_DIST_TOTAL_WARNING));
	m_distTotalMax = static_cast<double>(EnvirValueModule::EnvirQueryInt(ENV_VALUE_CHECK_DIST_TOTAL_MAX));
	m_heartBeatWarning = EnvirValueModule::EnvirQueryInt(ENV_VALUE_HEART_BEAT_WARNING);
	m_heartBeatMax = EnvirValueModule::EnvirQueryInt(ENV_VALUE_HEART_BEAT_MAX);
	//m_cantLoginTime = EnvirValueModule::EnvirQueryInt(ENV_VALUE_CANT_LOGIN_TIME_OVER_DIST_TOTAL);
	m_bCheckPlayerState = true;

	DECL_LUA_EXT(nx_motion_point);
	DECL_LUA_EXT(nx_floor_info);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_DISPATCH_GM_FUNCTION(nx_check_player_state);
#endif // FSROOMLOGIC_EXPORTS

	return true;
}

// 释放
bool MotionModule::Shut(IKernel* pKernel)
{
	return true;
}

// 进入场景
int MotionModule::OnEntry(IKernel* pKernel, const PERSISTID& self, 
						  const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 刷新速度
	m_pMotionModule->UpdateMoveSpeed(pKernel, self);

	// 添加属性回调
	ADD_CRITICAL(pKernel, self, "CantMove", "MotionModule::C_OnCantMoveChanged");
	ADD_CRITICAL(pKernel, self, "RunSpeedAdd", "MotionModule::C_OnSpeedChanged");
	ADD_CRITICAL(pKernel, self, "RunSpeed", "MotionModule::C_OnSpeedChanged");
	return 0;
}

// 玩家上线
int MotionModule::OnRecover(IKernel* pKernel, const PERSISTID& self, 
							const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	float fCheckSyncRange	= EnvirValueModule::EnvirQueryFloat(ENV_VALUE_CHECK_SYNC_RANGE);
	float fRunSpeed			= EnvirValueModule::EnvirQueryFloat(ENV_VALUE_DEFAULT_RUN_SPEED);

	//对象同步允许偏移
	pSelfObj->SetFloat("CheckSyncRange", fCheckSyncRange);

	// 设置行跑速度
	if (pSelfObj->FindAttr("RunSpeed"))
	{
		const float speed = pSelfObj->QueryFloat("RunSpeed");
		if (::util_float_equal_zero(speed))
		{
			pSelfObj->SetFloat("RunSpeed", fRunSpeed);
		}
	}

	// 设置转身速度
	if (pSelfObj->FindAttr("RollSpeed"))
	{
		const float speed = pSelfObj->QueryFloat("RollSpeed");
		if (::util_float_equal_zero(speed))
		{
			pSelfObj->SetFloat("RollSpeed", PI2);
		}
	}

	// 刷新速度
	m_pMotionModule->UpdateMoveSpeed(pKernel, self);  

	// 添加属性回调
	ADD_CRITICAL(pKernel, self, "CantMove", "MotionModule::C_OnCantMoveChanged");
	ADD_CRITICAL(pKernel, self, "RunSpeedAdd", "MotionModule::C_OnSpeedChanged");
	ADD_CRITICAL(pKernel, self, "RunSpeed", "MotionModule::C_OnSpeedChanged");

	// 去掉移动外挂检测 [3/28/2016 liumf]
// 	if (pSelfObj->QueryInt("Online") == PLAYER_ONLINE)
// 	{
// 		// 添加监测移动校正心跳
// 		ADD_HEART_BEAT(pKernel, self, "MotionModule::HB_UpdatePlayerState", CHECK_DIST_TOTAL_TIME);
// 	}

	return 0;
}

//请求移动
int MotionModule::OnPlayerRequestMove(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 移动模式
	int mode = args.IntVal(0);
	if (pSelfObj->QueryInt("Dead") > 0)
	{
		return 0;
	}

	int nResult = 0;
	switch (mode)
	{
	case MMODE_STOP:
		{
			nResult = OnRequestMoveStop(pKernel, self, args);
		}
		break;
	case MMODE_MOTION:
	case MMODE_FLY:
	case MMODE_JUMPTO:
		{
			nResult = OnRequestMoveMotion(pKernel, self, mode, args);
		}
		break;
	}

	return nResult;
}

// 请求停止
int MotionModule::OnRequestMoveStop(IKernel* pKernel, const PERSISTID& self, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	len_t x = args.FloatVal(2);
	len_t y = args.FloatVal(3);
	len_t z = args.FloatVal(4);
	len_t orient = args.FloatVal(5);
	//len_t y = pKernel->GetMapHeight(x, z);
	len_t server_move_speed = pSelfObj->QueryFloat("MoveSpeed");
	bool bCheck = false;
	if (pKernel->Type(self) == TYPE_PLAYER)
	{
		//pKernel->MoveTo(self, x, y, z, orient);
		//pKernel->Stop(self);
		// 高度值不小于地表层的高度
//  		float fGroundY = GetWalkHeight(pKernel, x, z, 0);
//  		y = __max(y, fGroundY);

		bCheck = pKernel->CheckStop(self, server_move_speed, x, y, z, orient);
#ifndef FSROOMLOGIC_EXPORTS
//#ifdef _DEBUG
		if (!bCheck)
		{
			// 临时保护 玩家停在地面以下 强制拉上来
			float fPosX = pSelfObj->GetPosiX();
			float fPosY = pSelfObj->GetPosiY();
			float fPosZ = pSelfObj->GetPosiZ();
			float fMapHeight = pKernel->GetMapHeight(fPosX, fPosZ);
			if (fPosY < fMapHeight)
			{
				pKernel->MoveTo(self, fPosX, fMapHeight, fPosZ, pSelfObj->GetOrient());
			}
			/*if (pKernel->GetServerId() == 7440005)
			{
			char strInfo[128] = { 0 };
			SPRINTF_S(strInfo, "停止结果：%d 坐标 : x=%f, y=%f, z=%f clienty=%f", bCheck ? 1 : 0, pSelfObj->GetPosiX(), pSelfObj->GetPosiY(), pSelfObj->GetPosiZ(), y);
			::CustomSysInfo(pKernel, self, TIPSTYPE_GMINFO_MESSAGE, "0", CVarList() << StringUtil::StringAsWideStr(strInfo));
			}*/
		}
//#endif
#endif
	}

	return 0;
}

// 请求移动
int MotionModule::OnRequestMoveMotion(IKernel* pKernel, const PERSISTID& self, int nMMode, const IVarList& args)
{
	if (nMMode < MMODE_MOTION || nMMode > MMODE_LOCATE)
	{
		return 0;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 玩家不能移动
	if (pSelfObj->QueryInt(FIELD_PROP_CANT_MOVE) > 0)
	{
		return 0;
	}

	len_t sx = args.FloatVal(2);
	len_t sz = args.FloatVal(3);
	len_t dx = args.FloatVal(4);
	len_t dz = args.FloatVal(5);
	len_t syclient = args.FloatVal(6);

	// 移动消息最后一位会多一个空字符串的参数
	len_t move_speed = pSelfObj->QueryFloat("MoveSpeed");
// 	int nCount = (int)args.GetCount();
// 	// 释放某些技能时移动速度会瞬间提高
// 	if (nCount > 8)
// 	{
// 		move_speed = args.FloatVal(7);
// 
// 		move_speed = __min(MAX_SKILL_SPEED, move_speed);
// 	}

	float sy = pSelfObj->GetPosiY();
	len_t rotate_speed = pSelfObj->QueryFloat("RollSpeed");
	if (pKernel->Type(self) == TYPE_PLAYER)
	{
		bool bCheck = false;
		if (MMODE_MOTION == nMMode)
		{
			bCheck = pKernel->CheckMotion(self, move_speed, rotate_speed, sx, sy, sz, dx, dz, 0);
		}
		else if (MMODE_FLY == nMMode)
		{
			pKernel->CheckFly(self, move_speed, rotate_speed, sx, sy, sz, dx, syclient, dz);
		}
		else if (MMODE_JUMPTO ==  nMMode)
		{
			pKernel->CheckJumpTo(self, move_speed, rotate_speed, sx, sy, sz, dx, dz);
		}
		
		if (!bCheck)
		{
			char strInfo[128] = {0};
			SPRINTF_S(strInfo, "OnRequestMoveMotion mode = %d move_speed = %f", nMMode, move_speed);
			::extend_warning(LOG_INFO, strInfo);
		}
		pKernel->Command(self, self, CVarList() << COMMAND_PLAYER_MOTION << sx << syclient << sz << dx << dz);
	}

	return 0;
}

// 移动回调
int MotionModule::OnMotion(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	if (m_pMotionModule == NULL)
	{
		return 0;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}
	int target_result = args.IntVal(1);
	int target_type   = pSelfObj->GetClassType();
	int target_state  = pSelfObj->QueryInt("MotionState");

	int target_grid_pos = pKernel->GetGridIndexByPos(pKernel->GetPosiX(self), pKernel->GetPosiZ(self));
	int target_grid_dst = pKernel->GetGridIndexByPos(pSelfObj->QueryFloat("TargetX"), pSelfObj->QueryFloat("TargetZ"));

	
	if (target_type == TYPE_PLAYER && target_result != MRESULT_SUCCEED)
	{
#ifndef FSROOMLOGIC_EXPORTS
//#ifdef _DEBUG
		/*if (pKernel->GetServerId() == 7440005)
		{
		char strInfo[128] = { 0 };
		SPRINTF_S(strInfo, "当前坐标是运动结果：%d x=%f, y=%f, z=%f", target_result, pSelfObj->GetPosiX(), pSelfObj->GetPosiY(), pSelfObj->GetPosiZ());
		::CustomSysInfo(pKernel, self, TIPSTYPE_GMINFO_MESSAGE, "0", CVarList() << StringUtil::StringAsWideStr(strInfo));
		}*/
//#endif
#endif
	}

	// 没有进行寻路
	if (target_state == MOTION_STATE_STOP)
	{
		return 0;
	}
	
	switch (target_result)
	{
	case MRESULT_SUCCEED:           // 成功到达
		m_pMotionModule->MotionSucceed(pKernel, self);
		break;
	case MRESULT_COLLIDE_SCENE:     // 碰到场景
	case MRESULT_COLLIDE_OBJECT:    // 碰到对象
	case MRESULT_COLLIDE_PATH_WALK:
	case MRESULT_COLLIDE_PATH_LOGIC:
	case MRESULT_COLLIDE_PATH_OBJECT:
		{
			MotionResult result = m_pMotionModule->DealCollision(pKernel, self, sender);
			if (result != MOTION_RESULT_SUCCEED)
			{
				// 躲避障碍物失败 直接寻路结束
				m_pMotionModule->MotionEnd(pKernel, self, result);
			}
		}
		
		break;
	}

	return 0;
}

// 注册移动回调函数
bool MotionModule::RegisterMotionCallBackFunction(const char* strFunc, MOTION_CALL_BACK_FUNC pFunc)
{
	MapCallBack::iterator iter = m_mapCallBack.find(strFunc);
	if (iter != m_mapCallBack.end()) 
	{
		// 已经被注册了
		Assert(0);
		return false;
	}

	m_mapCallBack.insert(std::make_pair(strFunc, pFunc));

	return true;
}

//向目标对象移动
bool MotionModule::MotionToObject(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
								  float minDistance/* = 0.0f*/, float maxDistance/* = 0.0f*/, 
								  const char* strCallBackFunc/* = ""*/,
								  MotionMethod method/* = MOTION_METHOD_ALL*/)
{
	// 检测目标是否存在
	if (!pKernel->Exists(self) || !pKernel->Exists(target))   
	{
		return false;
	}

	float x = pKernel->GetPosiX(target);
	float z = pKernel->GetPosiZ(target);

	return MotionToPoint(pKernel, self, x, z, minDistance, maxDistance, strCallBackFunc, method);
}

//向目标点移动
bool MotionModule::MotionToPoint(IKernel* pKernel, const PERSISTID& self, float targetX, float targetZ,
								 float minDistance/* = 0.0f*/, float maxDistance/* = 0.0f*/, 
								 const char* strCallBackFunc/* = ""*/,
								 MotionMethod method/* = MOTION_METHOD_ALL*/)
{
	// 检测目标是否存在 
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 判断是否正在寻路
	int motioning = pSelfObj->QueryInt("MotionState");
	if (motioning != MOTION_STATE_STOP)
	{
		// 检测目标是否一致
		float oldTargetX = pSelfObj->QueryFloat("TargetX");
		float oldTargetZ = pSelfObj->QueryFloat("TargetZ");
		if (util_float_equal(targetX, oldTargetX) 
			&& util_float_equal(targetZ, oldTargetZ))
		{
			// 正在寻路，目标一致, 则直接返回寻路成功，继续正在寻路的逻辑
			return true;
		}
	}

	// 重置状态
	ResetMotionState(pKernel, self);

	// 保存目标点
	pSelfObj->SetFloat("TargetX", targetX);
	pSelfObj->SetFloat("TargetZ", targetZ);

	// 大小检测
	if (minDistance < 0.0f)
	{
		minDistance = 0.0f;
	}
	if (maxDistance < 0.0f)
	{
		maxDistance = 0.0f;
	}
	if (maxDistance < minDistance)
	{
		// 互换值 保证逻辑正确
		float temp = maxDistance;
		maxDistance = minDistance;
		minDistance = temp;
	}
  
	// 离目标最小距离
	pSelfObj->SetFloat("PathDistanceMin", minDistance);

	// 离目标最大距离
	pSelfObj->SetFloat("PathDistanceMax", maxDistance);
	
	// 设置回调函数
	pSelfObj->SetString("CallBackFunction", strCallBackFunc);

	// 设置寻路方式
	pSelfObj->SetInt("MotionMethod", method);

	// 场景阻挡，调用场景寻路，找条行走路径
	int nearestGrid = GetTargetNearestEnableGrid(pKernel, self);
	if (nearestGrid > 0)
	{
		float nearTargetX = 0.0f;
		float nearTargetZ = 0.0f;
		pKernel->GetPosByIndex(nearestGrid, nearTargetX, nearTargetZ);
		if (!WalkLineEnabled(pKernel, self, nearTargetX, nearTargetZ))
		{
			FindPathToGridStatic(pKernel, self, nearTargetX, nearTargetZ);
		}
	}

	// 向目标移动
	ContinueMotion(pKernel, self);

	return true;
}

// 向目标点移动
// bForecastCollision 提前预测是否会发生碰撞
bool MotionModule::ContinueMotion(IKernel* pKernel, const PERSISTID& self, bool bForecastCollision/*= true*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		Assert(0);
		return false;
	}

	// 检测目标在有效距离内
	float selfX = pKernel->GetPosiX(self);
	float selfZ = pKernel->GetPosiZ(self);
	float targetX = pSelfObj->QueryFloat("TargetX");
	float targetZ = pSelfObj->QueryFloat("TargetZ");
	float minDistance = pSelfObj->QueryFloat("PathDistanceMin");
	float maxDistance = pSelfObj->QueryFloat("PathDistanceMax");
	float distance = util_dot_distance(selfX, selfZ, targetX, targetZ);

	if (distance >= minDistance && distance <= maxDistance)
	{
		// 在有效距离内，成功到达目标点，结束寻路
		MotionEnd(pKernel, self, MOTION_RESULT_SUCCEED);
		return true;
	}
	else
	{
		// 距离小于一个格子的距离
		float len = pKernel->GetPathGridSize();
		if (distance <= len)
		{
			// 在有效距离内，成功到达目标点，结束寻路
			MotionEnd(pKernel, self, MOTION_RESULT_SUCCEED);
			return true;
		}
	}

	// 获取需要到达的点
	int targetGrid = GetNeedArriveGrid(pKernel, self);
	if (targetGrid <= 0)
	{
		// 目标附近找不到落脚点则直接去目标点
		// 目标点所在格子
		targetGrid = pKernel->GetGridIndexByPos(targetX, targetZ);
	}

	// 当前已经到达目标最近的点,寻路成功
	int curGrid = pKernel->GetGridIndexByPos(selfX, selfZ);
	if (curGrid == targetGrid)
	{
		// 成功到达目标点，结束寻路
		MotionEnd(pKernel, self, MOTION_RESULT_SUCCEED);
		return true;
	}

	// 朝向目标第一个格子不可行走，则主动避让
	PERSISTID collision;
	if (bForecastCollision && NeedDealCollision(pKernel, self, targetGrid, collision))      
	{
		// 尝试绕过障碍物
		if (DealCollision(pKernel, self, collision) == MOTION_RESULT_SUCCEED)
		{
			// 成功，则不走原来的目标点，先避让
			//::extend_warning(LOG_ERROR, "Go Ahead RoundBarrier %d", self.nSerial);
			return true;
		}
		else
		{
			// 尝试绕过障碍物失败 则不进行避让，继续直线到达目标点
			// 逻辑在下面
		}
	}

	// 直线到达目标格子
	MotionResult result = MotionLineToGrid(pKernel, self, targetGrid);
	if (result != MOTION_RESULT_SUCCEED)
	{
		MotionEnd(pKernel, self, result);
		return false;
	}

	return true;
}

// 直线到达格子
MotionResult MotionModule::MotionLineToGrid(IKernel* pKernel, const PERSISTID& self, int gridIndex)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return MOTION_RESULT_FAILED;
	}

	if (gridIndex <= 0)
	{
		return MOTION_RESULT_CANT_FIND_GRID;
	}

	float x = 0;
	float z = 0;
	pKernel->GetPosByIndex(gridIndex, x, z);

	// 能否移动
	if (!CanMove(pKernel, self))
	{
		return MOTION_RESULT_CANT_MOVE;
	}

	// 设置移动状态
	pSelfObj->SetInt("MotionState", MOTION_STATE_MOVING);

	// 获取速度
	float moveSpeed = pSelfObj->QueryFloat("MoveSpeed");
	if (moveSpeed <= 0.0f)
	{
		return MOTION_RESULT_FAILED;
	}
	if (!pKernel->Motion(self, moveSpeed, PI2, x, z))
	{
		//extend_warning(LOG_ERROR, "[%s][%d] Motion error", __FILE__, __LINE__);
		return MOTION_RESULT_FAILED;
	}

	return MOTION_RESULT_SUCCEED;
}

// 移动成功(可能到达临时目标点，可能已到达终点，通过继续移动函数检测)
bool MotionModule::MotionSucceed(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 避开障碍物成功
	int gridIndex = pSelfObj->QueryInt("RoundGrid");
	if (gridIndex > 0)
	{	
		RoundBarrierSucceed(pKernel, self);
	}

	// 继续向目标移动
	return ContinueMotion(pKernel, self);
}

// 避让成功
bool MotionModule::RoundBarrierSucceed(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	int gridIndex = pSelfObj->QueryInt("RoundGrid");
	if (gridIndex <= 0)
	{
		return false;
	}

	// 记录避让时所在点
	IRecord* pRecord = pSelfObj->GetRecord(ROUND_PATH_LIST_RECORD);
	if (pRecord)
	{
		// 避让次数
		int roundCount = 0;

		// 删除已经存在的记录
		int row = pRecord->FindInt(ROUND_PATH_LIST_RECORD_COL_grid, gridIndex);
		if (row >= 0)
		{
			roundCount = pRecord->QueryInt(row, ROUND_PATH_LIST_RECORD_COL_count);
			pRecord->RemoveRow(row);
		}

		// 数量超过最大数，删除第一行数据
		int rowCount = pRecord->GetRows();
		if (rowCount >= ROUND_PATH_COUNT)
		{
			// 删除最早的数据
			pRecord->RemoveRow(0);
		}

		// 新增数据
		pRecord->AddRowValue(-1, CVarList() << gridIndex << ++roundCount);

		//extend_warning(LOG_WARNING, "Round Record Count %d %d %d", self.nSerial, gridIndex, roundCount);

		// 同一点避让次数过多，直接判定无法到达
		if (roundCount > ROUND_MAX_COUNT)
		{
			pSelfObj->SetInt("RoundTotalCount", ROUND_TOTAL_COUNT_MAX);
		}
	}

	// 避让成功，重置该点（重置避让状态）
	pSelfObj->SetInt("RoundGrid", 0);

	return true;
}

// 碰撞处理
MotionResult MotionModule::DealCollision(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender)
{	
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return MOTION_RESULT_SELF_INEXIST;
	}

	// 被撞对象
	int senderMotionState = MOTION_STATE_STOP;
	float senderSpeed = 0.0f;
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (pSenderObj != NULL)
	{
		senderMotionState = pSenderObj->FindAttr("MotionState") ? pSenderObj->QueryInt("MotionState") : MOTION_STATE_STOP;
		senderSpeed = pSenderObj->FindAttr("MoveSpeed") ? pSenderObj->QueryFloat("MoveSpeed") : 0.0f;
	}
	else
	{
		return MOTION_RESULT_SELF_INEXIST;
	}

	// 是否需要短暂等待
	bool bWait = pSenderObj->FindAttr("MotionState")												// 被撞目标存在此属性
		&& senderMotionState == MOTION_STATE_MOVING													// 被撞目标正在移动
		&& (pSelfObj->QueryInt64("WaitTarget") != sender.nData64									// 上次碰撞不是此对象
		|| (!util_float_equal(pSelfObj->QueryFloat("WaitTargetX"), pKernel->GetPosiX(sender))		// 或者是此对象
		&& !util_float_equal(pSelfObj->QueryFloat("WaitTargetZ"), pKernel->GetPosiZ(sender))))		// 但是对象的坐标有改变
		&& pSelfObj->QueryFloat("MoveSpeed") <= senderSpeed;										// 速度不大于被撞目标

	// 需要等待对方先离开
	if (bWait)
	{
		// 记录状态
		pSelfObj->SetInt("MotionState", MOTION_STATE_WAIT);
		// 停止移动
		pKernel->Stop(self);
		// 记录等待的对象
		pSelfObj->SetInt64("WaitTarget", sender.nData64);
		// 记录等待对象当前坐标点
		pSelfObj->SetFloat("WaitTargetX", pKernel->GetPosiX(sender));
		pSelfObj->SetFloat("WaitTargetZ", pKernel->GetPosiZ(sender));
		
		ADD_HEART_BEAT(pKernel, self, "MotionModule::HB_WaitMotion", WAIT_MOTION_TIME);

		return MOTION_RESULT_SUCCEED;
	}

	// 检测避让次数
	if (pSelfObj->QueryInt("RoundTotalCount") < ROUND_TOTAL_COUNT_MAX)
	{
		// 尝试绕过障碍物
		return RoundBarrier(pKernel, self, sender);
	}

	// 避让次数过多，直接调用引擎的寻路（万不得已，只能走此逻辑）
	// 检测是否允许调用
	int method = pSelfObj->QueryInt("MotionMethod");
	if ((MOTION_METHOD_DYNAMIC & method) == 0 
		&& (MOTION_METHOD_STATIC & method) == 0)
	{
		// 不允许调用，返回寻路失败
		return MOTION_RESULT_SELF_CLOSE_IN;
	}

	return DealCantArriveTarget(pKernel, self);
}

// 等待障碍物移开
int MotionModule::HB_WaitMotion(IKernel* pKernel, const PERSISTID& self, int time)
{
	if (!pKernel->Exists(self))
	{
		Assert(0);
		return false;
	}

	DELETE_HEART_BEAT(pKernel, self, "MotionModule::HB_WaitMotion");

	m_pMotionModule->ContinueMotion(pKernel, self); 

	return 0;
}

// 绕过障碍物
MotionResult MotionModule::RoundBarrier(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return MOTION_RESULT_FAILED;
	}

	// 寻找自身附近可走的点
	int gridIndex = GetSelfEnableGrid(pKernel, self);
	if (gridIndex <= 0)
	{
		return MOTION_RESULT_SELF_CLOSE_IN;
	}

	// 当前所在的格子
	int curGridIndex = pKernel->GetGridIndexByPos(pKernel->GetPosiX(self), pKernel->GetPosiZ(self));

	if (curGridIndex <= 0)
	{
		extend_warning(LOG_ERROR, "[%s][%d] RoundBarrier error", __FILE__, __LINE__);
		return MOTION_RESULT_SELF_INEXIST;
	}
	
	// 移动
	MotionResult ret = MotionLineToGrid(pKernel, self, gridIndex);
	if (ret != MOTION_RESULT_SUCCEED)
	{
		return ret;
	}

	// 记录避让时所在点
	pSelfObj->SetInt("RoundGrid", curGridIndex);
	// 记录避让次数
	int tempCount =  pSelfObj->QueryInt("RoundTotalCount");
	pSelfObj->SetInt("RoundTotalCount", tempCount + 1);

	return MOTION_RESULT_SUCCEED;
}

// 无法到达目标点后的处理
MotionResult MotionModule::DealCantArriveTarget(IKernel* pKernel, const PERSISTID& self)
{
	// 如果自身被包围 则直接返回寻路失败
	if (GetSelfEnableGrid(pKernel, self) <= 0)
	{
		//::extend_warning(LOG_ERROR, "npc can't move because of no path. config:%s SceneId:%d, x:%f z:%f", 
		//	pKernel->GetConfig(self), 
		//	pKernel->GetSceneId(), 
		//	pKernel->GetPosiX(self), 
		//	pKernel->GetPosiZ(self));  
		return MOTION_RESULT_CANT_ARRIVE;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return MOTION_RESULT_SELF_INEXIST;
	}

	LoopBeginCheck(a);
	do
	{
		LoopDoCheck(a);

		// 清空寻路表
		IRecord* pFindRoundPathRecord = pSelfObj->GetRecord(FIND_ROUND_PATH_RECORD);
		if (pFindRoundPathRecord != NULL)
		{
			pFindRoundPathRecord->ClearRow();
		}

		// 清空避让表
		IRecord* pRoundPathListRecord = pSelfObj->GetRecord(ROUND_PATH_LIST_RECORD);
		if (pRoundPathListRecord != NULL)
		{
			pRoundPathListRecord->ClearRow();
		}
		
		// 清空避让次数
		pSelfObj->SetInt("RoundTotalCount", 0);

		// 目标附近点
		int nearestGrid = GetTargetNearestEnableGrid(pKernel, self);
		if (nearestGrid <= 0)
		{
			// 尝试很多次后 找不到落脚点就直接寻路失败
			return MOTION_RESULT_CANT_FIND_GRID;
		}

		// 调用寻路
		if (!FindPathToGrid(pKernel, self, nearestGrid))
		{
			IRecord* pRecord = pSelfObj->GetRecord(CANT_ARRIVE_GRID_LIST_RECORD);
			if (pRecord != NULL && pRecord->GetRows() < pRecord->GetRowMax())
			{
				// 寻路失败，表明该点无法到达。记录到表中
				pRecord->AddRowValue(-1, CVarList() << nearestGrid);
			}
			else
			{
				// 出现异常，直接返回寻路失败
				return MOTION_RESULT_CANT_ARRIVE;
			}

			break;
		}
	} while (0);

	// 继续寻路
	ContinueMotion(pKernel, self, false);

	return MOTION_RESULT_SUCCEED;
}

// 寻路结束
// 只允许在三个接口中调用 MotionToPoint ContinueMotion MotionStop
// 目的是保证寻路必然有寻路结果 否则很容易在一些安全类判断中忽略而导致寻路中断又没有寻路结果
bool MotionModule::MotionEnd(IKernel * pKernel, const PERSISTID & self, MotionResult result)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 停止移动
	pKernel->Stop(self);

	pSelfObj->SetInt("MotionResult", result);

	// 间隔一帧执行
	ADD_HEART_BEAT(pKernel, self, "MotionModule::HB_MotionEnd", WAIT_MOTION_TIME);
	return true;
}

// 间隔一帧执行
int MotionModule::HB_MotionEnd(IKernel* pKernel, const PERSISTID& self, int time)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	DELETE_HEART_BEAT(pKernel, self, "MotionModule::HB_MotionEnd");

	m_pMotionModule->MotionEnd(pKernel, self);

	return 0;
}

// 发送寻路结果
bool MotionModule::MotionEnd(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 重置状态参数
	int result = ResetMotionState(pKernel, self);
	if (result == MOTION_RESULT_NONE)
	{
		// 没有结果 可能被新的寻路重置掉，不用发送结果
		return true;
	}

	// 发送结果
	const char* callBack = pSelfObj->QueryString("CallBackFunction");
	if (StringUtil::CharIsNull(callBack))
	{
		// 不需要回调
		return true;
	}

	// 查找回调函数并执行
	MapCallBack::iterator iter = m_mapCallBack.find(callBack);
	if (iter != m_mapCallBack.end())
	{
		MOTION_CALL_BACK_FUNC callBackFunc = iter->second;
		if (callBackFunc != NULL)
		{
			callBackFunc(pKernel, self, result);			
		}
	}

	return true;
}

// 重置寻路状态参数
// 返回重置前的结果
int MotionModule::ResetMotionState(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 重置状态
	pSelfObj->SetInt("MotionState", MOTION_STATE_STOP);
	pSelfObj->SetInt("RoundGrid", 0);
	pSelfObj->SetInt64("WaitTarget", 0);
	pSelfObj->SetInt("NearTargetGrid", 0);
	pSelfObj->SetInt("RoundTotalCount", 0);
	pSelfObj->SetInt("MotionMethod", static_cast<int>(MOTION_METHOD_ALL));
	pSelfObj->SetFloat("WaitTargetX", 0.0f);
	pSelfObj->SetFloat("WaitTargetZ", 0.0f);
	pSelfObj->SetFloat("SelfLastX", 0.0f);
	pSelfObj->SetFloat("SelfLastZ", 0.0f);

	// 清空表
	IRecord* pRecord = pSelfObj->GetRecord(ROUND_PATH_LIST_RECORD);
	if (pRecord != NULL)
	{
		pRecord->ClearRow();
	}
	pRecord = pSelfObj->GetRecord(FIND_ROUND_PATH_RECORD);
	if (pRecord != NULL)
	{
		pRecord->ClearRow();
	}
	pRecord = pSelfObj->GetRecord(CANT_ARRIVE_GRID_LIST_RECORD);
	if (pRecord != NULL)
	{
		pRecord->ClearRow();
	}

	// 删除心跳
// 	DELETE_HEART_BEAT(pKernel, self, "MotionModule::HB_WaitMotion");
// 	DELETE_HEART_BEAT(pKernel, self, "MotionModule::HB_CheckMotionBreak");

	int result = pSelfObj->QueryInt("MotionResult");
 
	pSelfObj->SetInt("MotionResult", MOTION_RESULT_NONE);

	return result;
}

// 取当前所需到达的方格
int MotionModule::GetNeedArriveGrid(IKernel * pKernel, const PERSISTID & self)
{
	// 优先走路径点
	//LOOPS_WORKTIME_BEGIN();
	int targetGrid = GetPathGrid_loops(pKernel, self);
	if (targetGrid <= 0)
	{
		// 离目标最近的有效格子
		int nearestGrid = GetTargetNearestEnableGrid(pKernel, self);
		if (nearestGrid <= 0)
		{
			// 目标附近的点全部被占用，无法靠近目标，寻路失败
			return 0;
		}

		targetGrid = nearestGrid;
	}

	return targetGrid;
}

// 寻找目标附近最优可使用方格
// 最优：离目标近，角度小
int MotionModule::GetTargetNearestEnableGrid(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 取目标点附近点
	int nearTargetGrid = pSelfObj->QueryInt("NearTargetGrid");

	// 取不可到达路径表
	IRecord* pCantArriveRecord = pSelfObj->GetRecord(CANT_ARRIVE_GRID_LIST_RECORD);
	if (pCantArriveRecord == NULL)
	{
		return 0;
	}

	// 目标点可用
	if (GridEnabled(pKernel, self, nearTargetGrid))
	{
		// 目标点可以到达
		if (pCantArriveRecord->FindInt(
			CANT_ARRIVE_GRID_LIST_RECORD_COL_grid, 
			nearTargetGrid) < 0)
		{
			return nearTargetGrid;
		}

	}

	// 目标点
	float targetX = pSelfObj->QueryFloat("TargetX");
	float targetZ = pSelfObj->QueryFloat("TargetZ");

	// 最小距离
	float minDistance = pSelfObj->QueryFloat("PathDistanceMin");    
	float maxDistance = pSelfObj->QueryFloat("PathDistanceMax");    

	// 目标点所在格子
	int index = pKernel->GetGridIndexByPos(targetX, targetZ);

	// 每个格子占用
	float len = pKernel->GetPathGridSize();
	if (util_float_equal_zero(len))
	{
		return 0;
	}

	// 距离换算成格子数
	int grildLen = static_cast<int>(maxDistance / len);
	if (grildLen <= 0)
	{
		if (GridEnabled(pKernel, self, index))
		{
			// 目标点可以到达
			if (pCantArriveRecord->FindInt(
				CANT_ARRIVE_GRID_LIST_RECORD_COL_grid, 
				index) < 0)
			{
				pSelfObj->SetInt("NearTargetGrid", index);
				return index;
			}
		}

		return 0;
	}

	// 查找所有象限
	return GetNearestEnableGridInAllQuadrant(pKernel, self, targetX, targetZ, minDistance, maxDistance);
}

// 在所有象限内查找离目标最近的有效点
int MotionModule::GetNearestEnableGridInAllQuadrant(IKernel * pKernel,
													const PERSISTID & self,
													float targetX, float targetZ,
													float minDistance, float maxDistance)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 自己所在点
	float selfX = pKernel->GetPosiX(self);
	float selfZ = pKernel->GetPosiZ(self);  

	// 地图总格子的行列数
	int iRowTotal = (int)pKernel->GetPathGridRows();
	if (iRowTotal <= 0)
	{
		return 0;
	}

	// 目标点所在格子
	int index = pKernel->GetGridIndexByPos(targetX, targetZ);

	// 目标点
	CPoint pathPoint;
	pathPoint.x = (index - 1) % iRowTotal + 1;
	pathPoint.y = (index - 1) / iRowTotal;

	// 原点所在格子
	int selfIndex = pKernel->GetGridIndexByPos(selfX, selfZ);

	// 原点
	CPoint selfPoint;
	selfPoint.x = (selfIndex - 1) % iRowTotal + 1;
	selfPoint.y = (selfIndex - 1) / iRowTotal;

	// 原点到目标点的向量
	CPoint vectorPoint;
	vectorPoint.x = selfPoint.x - pathPoint.x;
	vectorPoint.y = selfPoint.y - pathPoint.y;

	// 每个格子占用
	float len = pKernel->GetPathGridSize();
	if (util_float_equal_zero(len))
	{
		return 0;
	}

	// 距离换算成格子数
	int grildLen = static_cast<int>(maxDistance / len);

	// 如果与目标正好在坐标轴上，则先遍历正方向的象限
	if (vectorPoint.x == 0)
	{
		vectorPoint.x = 1;
	}
	if (vectorPoint.y == 0)
	{
		vectorPoint.y = 1;
	}

	CPoint tempVectorPoint = vectorPoint;
	int nearIndex = 0;

	// 遍历四个象限内的方格
	// 先遍历所在象限，再遍历离得近的象限
	// 循环保护
	LoopBeginCheck(el);
	for (int i = 0; i < 4; ++i)
	{
		LoopDoCheck(el);
		switch (i)
		{
		case 0: // 第一个象限
			//NULL;
			break;
		case 1:	// 第二个象限
			if (abs(vectorPoint.x) < abs(vectorPoint.y))
			{
				tempVectorPoint.x = 0 - vectorPoint.x;
			}
			else
			{
				tempVectorPoint.y = 0 - vectorPoint.y;
			}
			break;
		case 2:	// 第三个个象限
			if (abs(vectorPoint.x) > abs(vectorPoint.y))
			{
				tempVectorPoint.x = 0 - vectorPoint.x;
			} 
			else
			{
				tempVectorPoint.y = 0 - vectorPoint.y;
			}
			break;
		case 3:	// 第四个象限
			tempVectorPoint.x = 0 - vectorPoint.x;
			tempVectorPoint.y = 0 - vectorPoint.y;
			break;
		default:
			//NULL;
			break;
		}

		// 取向量起点相对于终点所在象限内最优格子索引
		nearIndex = GetNearestEnableGridInQuadrant(pKernel, 
			self,
			tempVectorPoint, 
			pathPoint, 
			grildLen, 
			targetX, targetZ, 
			minDistance, maxDistance, 
			iRowTotal);

		if (nearIndex > 0)
		{
			//extend_warning(LOG_WARNING, "FindTargetNearGrid index %d targetIdx %d i %d", nearIndex, index, i);
			pSelfObj->SetInt("NearTargetGrid", nearIndex);
			return nearIndex;
		}
	}

	return 0;
}

// 获取象限内离目标最近的有效方格
int MotionModule::GetNearestEnableGridInQuadrant(IKernel * pKernel,
												 const PERSISTID & self,
												 CPoint& vectorPoint, 
												 CPoint& targetPoint, 
												 int length, 
												 float targetX, float targetZ,
												 float minDistance, float maxDistance,
												 int rowTotal)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 目标点
	if (vectorPoint.x == 0 || vectorPoint.y == 0 || length <= 0)
	{
		return 0;
	}

	// 取不可到达路径表
	IRecord* pCantArriveRecord = pSelfObj->GetRecord(CANT_ARRIVE_GRID_LIST_RECORD);
	if (pCantArriveRecord == NULL)
	{
		return 0;
	}


	// 行数
	int rowTotalX = 1;
	int rowTotalY = rowTotal;

	// 颠倒遍历优先次序
	if (abs(vectorPoint.x) < abs(vectorPoint.y))
	{
		int temp = 0;
		temp = vectorPoint.x;
		vectorPoint.x = vectorPoint.y;
		vectorPoint.y = temp;

		temp = targetPoint.x;
		targetPoint.x = targetPoint.y;
		targetPoint.y = temp;

		rowTotalX = rowTotal;
		rowTotalY = 1;
	}

	// 绝对值向量
	CPoint absPoint;
	absPoint.x = abs(vectorPoint.x);
	absPoint.y = abs(vectorPoint.y);
	if (absPoint.x == 0 || absPoint.y == 0)
	{
		return 0;
	}

	// 单位向量
	CPoint unitPoint;
	unitPoint.x = vectorPoint.x / absPoint.x;
	unitPoint.y = vectorPoint.y / absPoint.y;

	// 附近点离目标距离
	float tempTargetDistance = 0.0f;
	// 附近点索引
	int curIndex = 0;
	// 遍历点
	CPoint point = targetPoint;

	// j的遍历次数
	int jCount = 0;
	// 循环保护
	LoopBeginCheck(em);
	for (int i = 1; i < length + 1; ++i)
	{
		LoopDoCheck(em);
		point.x = targetPoint.x +  unitPoint.x * i;

		jCount = i * 2 + 1;
		// 循环保护
		LoopBeginCheck(en);
		for (int j = 0; j < jCount; ++j)
		{
			LoopDoCheck(en);
			if (j <= i)
			{
				point.y = targetPoint.y + unitPoint.y * j;
			}
			else
			{
				point.x = targetPoint.x - unitPoint.x * (jCount - j);
			}

			// 转换为格子索引
			curIndex = point.x * rowTotalX + point.y * rowTotalY;

			// 检测格子不可用
			if (!GridEnabled(pKernel, self, curIndex))
			{
				continue;
			}

			// 格子不可以到达
			if (!(pCantArriveRecord->FindInt(
				CANT_ARRIVE_GRID_LIST_RECORD_COL_grid, 
				curIndex) < 0))
			{
				continue;
			}

			// 计算格子距离
			float x = 0.0f;
			float z = 0.0f;
			pKernel->GetPosByIndex(curIndex, x, z);

			// 该点不能站立
			//if (!CanStand(pKernel, x, z, CVarList()))
			if (!CanStandFast(pKernel, x, z))
			{
				continue;
			}

			// 附近点到目标点距离
			tempTargetDistance = util_dot_distance(x, z, targetX, targetZ);

			// 在有效距离内
			if (tempTargetDistance >= minDistance 
				&& tempTargetDistance < maxDistance)
			{
				//extend_warning(LOG_WARNING, "FindTargetNearGridInQuadrant i %d j %d", i, j);
				return curIndex;
			}
		}
	}

	return 0;
}

// 寻找自身走向目标附近最优可行走方格
int MotionModule::GetSelfEnableGrid(IKernel * pKernel, const PERSISTID & self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	//目标点
	int pathIndex = GetNeedArriveGrid(pKernel, self);
	if (pathIndex <= 0)
	{
		// 目标附近找不到落脚点则直接去目标点
		// 目标点
		float targetX = pSelfObj->QueryFloat("TargetX");
		float targetZ = pSelfObj->QueryFloat("TargetZ"); 

		// 目标点所在格子
		pathIndex = pKernel->GetGridIndexByPos(targetX, targetZ);
	}

	CVarList varIndex;
	if (!GetSelfGridList(pKernel, self, pathIndex, varIndex))
	{
		return 0;
	}

	// 所走过的格子表
	IRecord* pRecord = pSelfObj->GetRecord(ROUND_PATH_LIST_RECORD);

	// 记录最优回头路，在无路可走的时候，走
	int firstEnableGridIndex = 0;

	int idex = 0;
	// 循环保护
	LoopBeginCheck(eo);
	for (size_t i = 0; i < varIndex.GetCount(); ++i)
	{
		LoopDoCheck(eo);
		idex = varIndex.IntVal(i);

		// 返回优先级最高的格子坐标
		if (m_pMotionModule->GridEnabled(pKernel, self, idex, false))
		{
			float x = 0.0f;
			float z = 0.0f;
			pKernel->GetPosByIndex(idex, x, z);
			// 该点能否站立
			//if (!CanStand(pKernel, x, z, CVarList()))
			if (!CanStandFast(pKernel, x, z ))
			{
				continue;
			}

			// 可否直线到达
			if (!WalkLineEnabled(pKernel, self, x, z))
			{
				continue;
			}

			if (firstEnableGridIndex == 0)
			{
				firstEnableGridIndex = idex;
			}

			// 不走回头路
			if (pRecord == NULL || pRecord->FindInt(ROUND_PATH_LIST_RECORD_COL_grid, idex) < 0)
			{
				return idex;
			}
		}
	}

	// 无路可走，走回头路，或者告知无路走
	return firstEnableGridIndex;
}

// 获取自身走向目标附近方格优先级表
// targetGrid 目标点
// varList 返回列表
// step 步伐
bool MotionModule::GetSelfGridList(IKernel * pKernel, const PERSISTID & self, int targetGrid, IVarList& varList, int step/* = 1*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 取占用的格子数
	int iGrid = pSelfObj->QueryInt("PathGrid");
	// 当前所在格子索引
	int indexGrid = pKernel->GetGridIndexByPos(pKernel->GetPosiX(self), pKernel->GetPosiZ(self));
	if (iGrid <=0)
	{
		varList << indexGrid;
		return true;
	}
	// 取地图总格子的行数
	int iRowTotal = (int)pKernel->GetPathGridRows();
	if (iRowTotal <= 0)
	{
		return false;
	}

	CPoint curPoint;
	curPoint.x = (indexGrid - 1) % iRowTotal + 1;
	curPoint.y = (indexGrid - 1) / iRowTotal;

	CPoint pathPoint;
	pathPoint.x = (targetGrid - 1) % iRowTotal + 1;
	pathPoint.y = (targetGrid - 1) / iRowTotal;

	CPoint tempPoint = curPoint - pathPoint;
	if (tempPoint.x == 0 && tempPoint.y == 0)
	{
		return false;
	}

	// 算法，只沿着横坐标和纵坐标的方向移动
	// 优先级 朝向目标 优先距离长的方向 背离目标 优先距离短的方向
	if (tempPoint.x == 0)
	{
		tempPoint.x = tempPoint.y;
	}
	else if (tempPoint.y == 0)
	{
		tempPoint.y = tempPoint.x;
	}

	int absTempX = abs(tempPoint.x);
	int absTempY = abs(tempPoint.y);
	if (absTempX == 0 || absTempY == 0)
	{
		return false;
	}
	
	if (absTempY > absTempX)
	{
		varList << (indexGrid - (tempPoint.y / absTempY) * iGrid * iRowTotal * step)
				<< (indexGrid - (tempPoint.x / absTempX) * iGrid * step)
				<< (indexGrid + (tempPoint.x / absTempX) * iGrid * step)
				<< (indexGrid + (tempPoint.y / absTempY) * iGrid * iRowTotal * step);
	}
	else
	{
		varList << (indexGrid - (tempPoint.x / absTempX) * iGrid * step)
				<< (indexGrid - (tempPoint.y / absTempY) * iGrid * iRowTotal * step)
				<< (indexGrid + (tempPoint.y / absTempY) * iGrid * iRowTotal * step)
				<< (indexGrid + (tempPoint.x / absTempX) * iGrid * step);
	}
	
	return true;
}

// 需要提前处理碰撞，检测自身走向目标的附近最优行走点是否可行走(接近于直线走向目标的第一个格子)
bool MotionModule::NeedDealCollision(IKernel * pKernel, const PERSISTID & self, int targetGrid, PERSISTID& collision)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	CVarList varIndex;
	if (!GetSelfGridList(pKernel, self, targetGrid, varIndex))
	{
		return false;
	}

	// 没有获取到行走点
	if (varIndex.IsEmpty())
	{
		return false;
	}

	// 获取最优行走点
	int gridIndex = varIndex.IntVal(0);

	// 是否被对象占用
	if (!GridEnabled(pKernel, self, gridIndex, false))
	{
		CVarList ret;
		int groupId = pSelfObj->QueryInt("GroupID");
		pKernel->GetPathGridBalker(gridIndex, ret, groupId);
		if (!ret.IsEmpty() && ret.GetType(0) == VTYPE_OBJECT)
		{
			collision = ret.ObjectVal(0);
			return true;
		}
	}

	// 所走过的格子表
	IRecord* pRecord = pSelfObj->GetRecord(ROUND_PATH_LIST_RECORD);
	if (pRecord == NULL)
	{
		return false;
	}

	// 回头路
	if (pRecord->FindInt(ROUND_PATH_LIST_RECORD_COL_grid, gridIndex) >= 0)
	{
		return true;
	}

	return false;
}

// 寻路，通过引擎找一条行走路径
bool MotionModule::FindPathToGrid(IKernel* pKernel, const PERSISTID& self, int targetGrid)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 检测是否允许调用
	int method = pSelfObj->QueryInt("MotionMethod");
	if ((MOTION_METHOD_DYNAMIC & method) == 0 
		&& (MOTION_METHOD_STATIC & method) == 0)
	{
		// 不允许调用，返回
		return false;
	}

	float targetX = 0.0f;
	float targetZ = 0.0f;
	pKernel->GetPosByIndex(targetGrid, targetX, targetZ);

	bool ret = false;
	LoopBeginCheck(a);
	do
	{
		LoopDoCheck(a);
		// 无场景阻挡
		if (WalkLineEnabled(pKernel, self, targetX, targetZ))
		{
			// 优先动态寻路
			if (!FindPathToGridDynamic(pKernel, self, targetGrid))
			{
				// 动态寻路失败，则静态寻路
				ret = FindPathToGridStatic(pKernel, self, targetX, targetZ);
			}

			break;
		}

		// 有场景阻挡，优先静态寻路
		if (!FindPathToGridStatic(pKernel, self, targetX, targetZ))
		{
			// 静态寻路失败，则动态寻路
			ret = FindPathToGridDynamic(pKernel, self, targetGrid);
		}

	} while (0);
	
	return ret;
}

// 动态寻路，通过A*寻路
bool MotionModule::FindPathToGridDynamic(IKernel* pKernel, const PERSISTID& self, int targetGrid)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 检测是否允许调用
	int method = pSelfObj->QueryInt("MotionMethod");
	if ((MOTION_METHOD_DYNAMIC & method) == 0)
	{
		// 不允许调用，返回
		return false;
	}
	// 坐标点
	MontionPos pos_sef;
	MontionPos pos_target;
	MotionHelper::ParseMotionPosAll(pKernel, pos_sef, pKernel->GetPosiX(self), pKernel->GetPosiZ(self), pKernel->GetPosiY(self));
	MotionHelper::ParseMotionPos(pKernel, pos_target, targetGrid);

	// 路径表
	CVarList pathList;
	if (!MotionHelper::FindPathToTargets_Dynamic(pKernel, self, pos_sef, pos_target, pathList))
	{
		return false;
	}
	// 路径个数
	int count = (int)pathList.GetCount();
	if (count == 0)
	{
		return false;
	}

	// 获取路径表
	IRecord* pRecord = pSelfObj->GetRecord(FIND_ROUND_PATH_RECORD);
	if (pRecord == NULL)
	{
		return false;
	}

	// 先清空表
	pRecord->ClearRow();

	// 放入路径表中
	// 通过A*寻路，则倒序取值
	float tempX = 0.0f;
	float tempZ = 0.0f;
	int tempGrid = 0;

	float newX = 0.0f;
	float newY = 0.0f;
	float newZ = 0.0f;
	float tempSrcX = pos_sef.pos_x_;
	float tempSrcZ = pos_sef.pos_z_;
	float walkStep = pSelfObj->QueryFloat("WalkStep");

	// 循环保护
	LoopBeginCheck(ep);
	for (int i = count - 1; i >= 0; --i)
	{
		LoopDoCheck(ep);
		// 获取格子索引
		tempGrid = pathList.IntVal(i);

		// 获取坐标
		pKernel->GetPosByIndex(tempGrid, tempX, tempZ);

		// 点是否可以站立
		//if (!CanStand(pKernel, tempX, tempZ, CVarList()))
		if (!CanStandFast(pKernel, tempX, tempZ ))
		{
			return false;
		}

		// 判断是否可以直线到达
		if (!pKernel->TraceLineWalk(walkStep, tempSrcX, 
			pKernel->GetMapHeight(tempSrcX, tempSrcZ), 
			tempSrcZ, tempX, tempZ, newX, newY, newZ))
		{
			return false;
		}

		tempSrcX = tempX;
		tempSrcZ = tempZ;

		pRecord->AddRowValue(-1, CVarList() << tempGrid << 0);
	}

	return true;
}

// 静态寻路，通过场景配置路径寻路
bool MotionModule::FindPathToGridStatic(IKernel* pKernel, const PERSISTID& self, float targetX, float targetZ)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 检测是否允许调用
	int method = pSelfObj->QueryInt("MotionMethod");
	if ((MOTION_METHOD_STATIC & method) == 0)
	{
		// 不允许调用，返回
		return false;
	}

	// 路径表
	CVarList pathList;

	// 坐标点
	MontionPos pos_sef;
	MontionPos pos_target;
	MotionHelper::ParseMotionPosAll(pKernel, pos_sef, pKernel->GetPosiX(self), pKernel->GetPosiZ(self), pKernel->GetPosiY(self));
	MotionHelper::ParseMotionPosEx(pKernel, pos_target, targetX, targetZ);

	float newX = 0.0f;
	float newY = 0.0f;
	float newZ = 0.0f;
	float tempSrcX = pos_sef.pos_x_;
	float tempSrcZ = pos_sef.pos_z_;
	float walkStep = pSelfObj->QueryFloat("WalkStep");

	int pointCount = 0;
	CVarList tempPathList;

	if (!MotionHelper::FindPathToTargets_Static(pKernel, self, pos_sef, pos_target, pointCount, tempPathList))
	{
		return false;
	}
	if ( pointCount > 0 )
	{
		// 循环保护
		LoopBeginCheck(eq);
		for (int i = 0; i < pointCount; ++i)
		{
			LoopDoCheck(eq);
			float tempX = tempPathList.FloatVal(i*3);
			float tempZ = tempPathList.FloatVal(i*3+2);
			int tempIndex = pKernel->GetGridIndexByPos(tempX, tempZ);
			if (tempIndex == pos_sef.grid_)
			{
				continue;
			}
			// 点是否可以行走
			if (!pKernel->TraceLineWalk(walkStep, tempSrcX, pKernel->GetMapHeight(tempSrcX, tempSrcZ), tempSrcZ, tempX, tempZ, newX, newY, newZ))
			{
//#if _DEBUG // 在需要的时候开启，别清理掉
//				extend_warning(LOG_ERROR, "[%s][%d] StaticPath error srcX:%f, srcZ:%f, pathX:%f pathZ:%f can't walk line SceneId:%d SceneConfig:%s",
//					__FILE__, __LINE__, tempSrcX, tempSrcZ, tempX, tempZ, pKernel->GetSceneId(), pKernel->GetSceneConfig(pKernel->GetSceneId()));
//#endif
				return false;
			}
			// 点是否可以站立
			//if (!CanStand(pKernel, tempX, tempZ, CVarList()))
			if (!CanStandFast(pKernel, tempX, tempZ ))
			{
//#if _DEBUG // 在需要的时候开启，别清理掉
//				extend_warning(LOG_ERROR, "[%s][%d] StaticPath error selfX:%f, selfZ:%f, pathX:%f pathZ:%f can't Stand SceneId:%d SceneConfig:%s",
//					__FILE__, __LINE__, selfX, selfZ, tempX, tempZ, pKernel->GetSceneId(), pKernel->GetSceneConfig(pKernel->GetSceneId()));
//#endif
				return false;
			}

			tempSrcX = tempX;
			tempSrcZ = tempZ;

			pathList << tempIndex;
		}
	}

	// 路径个数
	int count = (int)pathList.GetCount();
	if (count == 0)
	{
		return false;
	}

	// 获取路径表
	IRecord* pRecord = pSelfObj->GetRecord(FIND_ROUND_PATH_RECORD);
	if (pRecord == NULL)
	{
		return false;
	}

	// 先清空表
	pRecord->ClearRow();

	// 放入路径表中
	// 循环保护
	LoopBeginCheck(er);
	for (int i = 0; i < count; ++i)
	{
		LoopDoCheck(er);
		pRecord->AddRowValue(-1, CVarList() << pathList.IntVal(i) << 0);
	}

	return true;
}

// 获取动态寻路的路经点
int MotionModule::GetPathGrid_loops(IKernel* pKernel, const PERSISTID& self)
{
	//LOOPS_WORKTIME_CHECK(GetPathGrid);
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 获取表
	IRecord* pRecord = pSelfObj->GetRecord(FIND_ROUND_PATH_RECORD);
	if (pRecord == NULL)
	{
		return 0;
	}
	
	// 检测表示否为空
	if (pRecord->GetRows() <= 0)
	{
		return 0;
	}


	// 超过访问次数
	int findCount = pRecord->QueryInt(0, FIND_ROUND_PATH_RECORD_COL_count);
	if (findCount > FIND_ROUND_PATH_MAX_COUNT)
	{
		pRecord->ClearRow();
		return 0;
	}

	// 获取第一个路径点
	int firstGrid = pRecord->QueryInt(0, FIND_ROUND_PATH_RECORD_COL_grid);

	// 记录访问次数
	pRecord->SetInt(0, FIND_ROUND_PATH_RECORD_COL_count, ++findCount);

	// 自身占用格子数
	int gridSize = pSelfObj->QueryInt("PathGrid");

	// 地图总行数
	int iTotalRow = (int)pKernel->GetPathGridRows();
	if (iTotalRow <= 0)
	{
		return 0;
	}

	// 当前格子索引
	int curGrid = pKernel->GetGridIndexByPos(pKernel->GetPosiX(self), pKernel->GetPosiZ(self));

	// 到达目标点，删除当前路径点，获取下一个路径点
	if (GridEnabled(pKernel, self, firstGrid, false) )
	{
		// 目标点可用，必须到达目标点
		if (curGrid == firstGrid)
		{
			pRecord->RemoveRow(0);
			return GetPathGrid_loops(pKernel, self);
		}
	}
	else
	{
		// 目标点不可用，到达周围即可

		// 当前格子坐标
		size_t curX = (curGrid - 1) % iTotalRow + 1;
		size_t curY = (curGrid - 1) / iTotalRow;

		// 路径点格子坐标
		size_t firstX = (firstGrid - 1) % iTotalRow + 1;
		size_t firstY = (firstGrid - 1) / iTotalRow;


		// 已经到达当前点或者到达周围，删除之
		if (curX >= (firstX - gridSize)
			&& curX <= firstX + gridSize
			&& curY >= firstY - gridSize
			&& curY <= firstY + gridSize)
		{
			pRecord->RemoveRow(0);
			return GetPathGrid_loops(pKernel, self);
		}

	}

	
	return firstGrid;

}

// 测试是否能够直线到达(只考虑场景碰撞)
bool MotionModule::WalkLineEnabled(IKernel* pKernel, const PERSISTID& self, float targetX, float targetZ)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 坐标点
	float selfX = pKernel->GetPosiX(self);
	float selfY = pKernel->GetPosiY(self);
	float selfZ = pKernel->GetPosiZ(self);
	float newX = 0.0f;
	float newY = 0.0f;
	float newZ = 0.0f;

	len_t walkStep = pSelfObj->QueryFloat("WalkStep");

	return pKernel->TraceLineWalk(walkStep, selfX, selfY, selfZ, targetX, targetZ, newX, newY, newZ);
}

// 该点能否站立
bool MotionModule::CanStand(IKernel* pKernel, float x, float z, IVarList& heightList)
{
	// 清理列表
	heightList.Clear();

	bool bCanMove = false;

	// 遍历所有层数
	int count = pKernel->GetFloorCount(x, z);

	// 循环保护
	LoopBeginCheck(es);
	for (int i = 0; i < count; ++i)
	{
		LoopDoCheck(es);
		if (pKernel->GetFloorCanStand(x, z, i))
		{
			// 可站立
			bCanMove = true;

			// 高度列表
			heightList << pKernel->GetFloorHeight(x, z, i);
		}
	}

	// 返回
	return bCanMove;
}
bool MotionModule::CanStandFast(IKernel* pKernel, float x, float z)
{
	bool bCanMove = false;
	// 遍历所有层数
	int count = pKernel->GetFloorCount(x, z);
	// 循环保护
	LoopBeginCheck(es);
	for (int i = 0; i < count; ++i)
	{
		LoopDoCheck(es);
		if (pKernel->GetFloorCanStand(x, z, i))
		{
			// 可站立
			bCanMove = true;
			break;
		}
	}
	// 返回
	return bCanMove;
}

// 能否移动
bool MotionModule::CanMove(IKernel * pKernel, const PERSISTID & target)
{
	IGameObj* pTargetObj = pKernel->GetGameObj(target);
	if (pTargetObj == NULL)
	{
		return false;
	}

	if(pTargetObj->QueryInt("Dead") > 0)
	{
		return false;
	}

	if (pTargetObj->QueryInt("CantMove") > 0)
	{
		return false;
	}

	return true;
}

// 监听不能跑动属性
int MotionModule::C_OnCantMoveChanged(IKernel* pKernel, const PERSISTID& self,
								   const char* property, const IVar& old)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	// 不可行走
	if (pSelfObj->QueryInt("CantMove") >= MOVE_NO)
	{
		m_pMotionModule->MotionStop(pKernel, self);
/////////////////////////////////////////////////////////////
// #ifndef FSROOMLOGIC_EXPORTS
// 		m_pBattleTeamModule->Stop(pKernel, self);
// #endif // FSROOMLOGIC_EXPORTS
/////////////////////////////////////////////////////////////
	}

	return 0;
}

// 监听速度变化
int MotionModule::C_OnSpeedChanged(IKernel* pKernel, const PERSISTID& self,
								   const char* property, const IVar& old)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	m_pMotionModule->UpdateMoveSpeed(pKernel, self);

	return 0;
}

// 刷新速度
void MotionModule::UpdateMoveSpeed(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	float speed = pSelfObj->QueryFloat("RunSpeed") + pSelfObj->QueryFloat("RunSpeedAdd");

	pSelfObj->SetFloat("MoveSpeed", speed);

	// 如果正在寻路 则重新寻路以应用最新的速度
	int motionState = pSelfObj->QueryInt("MotionState");
	if (motionState != MOTION_STATE_STOP)
	{
		// 目标点
		float targetX = pSelfObj->QueryFloat("TargetX");
		float targetZ = pSelfObj->QueryFloat("TargetZ");

		// 离目标最小距离
		float minDistance = pSelfObj->QueryFloat("PathDistanceMin");

		// 离目标最大距离
		float maxDistance = pSelfObj->QueryFloat("PathDistanceMax");

		// 回调函数
		const char* strCallBackFunc = pSelfObj->QueryString("CallBackFunction");
		if (strCallBackFunc == NULL)
		{
			strCallBackFunc = "";
		}

		// 设置寻路方式
		MotionMethod method = static_cast<MotionMethod>(pSelfObj->QueryInt("MotionMethod"));

		// 重置寻路状态
		m_pMotionModule->ResetMotionState(pKernel, self);

		// 先停止移动（此处直接调用引擎停止移动）
		// 因为改变速度后需要先停掉之前的寻路 重新开始寻路才能应用原来的速度
		// 因为此处是重新寻路，所以不用返回寻路结果, 因此不用调用MotionStop接口
		pKernel->Stop(self);

		// 再重新寻路
		m_pMotionModule->MotionToPoint(pKernel, self, targetX, targetZ, 
			minDistance, maxDistance, strCallBackFunc, method);
	}
}

// 停止移动
bool MotionModule::MotionStop(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 即将被销毁 不需要停止移动
	if(pSelfObj->FindAttr("LifeTime") && pSelfObj->QueryInt("LifeTime") > 0)
	{
		return false;
	}

// #ifndef FSROOMLOGIC_EXPORTS
// 	m_pBattleTeamModule->Stop(pKernel, self);
// #endif // FSROOMLOGIC_EXPORTS

	if (m_pMotionModule != NULL 
		&& pSelfObj->QueryInt("MotionState") != MOTION_STATE_STOP)
	{
		// 正在移动，则结束
		return m_pMotionModule->MotionEnd(pKernel, self, MOTION_RESULT_STOP);
	}

	return pKernel->Stop(self);
}