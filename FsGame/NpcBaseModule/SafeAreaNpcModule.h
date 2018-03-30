//--------------------------------------------------------------------
// 文件名:		SafeAreaNpcModule.h
// 内  容:		安全区NPC
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2015年03月30日
// 创建人:		  ( )
// 修改人:        ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __SafeAreaNpc_H__
#define __SafeAreaNpc_H__

#include "Fsgame/Define/header.h"
#include "utils/shape_collision.h"

class AsynCtrlModule;
class SafeAreaNpcModule: public ILogicModule  
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);
	// 释放
	virtual bool Shut(IKernel* pKernel);

public:
	// 触发回调
	static int OnSpring(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 结束触发回调
	static int OnEndSpring(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 安全区NPC进入场景
	static int OnEntry( IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args );

	// 安全区NPC销毁
	static int OnDestroy(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家离开场景
	static int OnLeaveScene(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家进入场景
	static int OnPlayerEnterScene(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应分组变化 
	static int OnCommandChangeGroup(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
private:
	struct RectangularSafeArea 
	{
		int				nAreaId;			// 安全区id
		int				nSceneId;			// 所在场景id
		int				nNation;			// 所属阵营
		int				nSafeRule;			// 安全区策略
		Rectangle2D		kRectangle;			// 矩形
	};

	typedef std::vector<RectangularSafeArea> RectangularSafeAreaVec;

	enum 
	{
		 NOT_IN_RECT_SAFE_AREA = 0,		// 没在矩形安全区
	};

	// 安全区类型
// 	enum SafeType
// 	{
// 		CANT_BE_ATTACK,					// 不能被攻击
// 		AVOID_PVP,						// 不能进行PVP
// 	};

	//更新进入安全区状态
	void UpdateEntryState(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule);

	//更新离开安全区状态
	void UpdateLeaveState(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule);

	// 是否保护(国家验证)
	bool IsSafe(IKernel* pKernel, const PERSISTID& player, int nNation, int nSafeRule, const int type);

    //响应客户端文言提示消息
	void ResponseClientMsg(IKernel* pKernel, const PERSISTID& player, const int info_id);

	// 读取矩形安全区配置
	bool LoadRectanularSafeAreaConfig(IKernel* pKernel);

	// 解析坐标
	bool ParsePos(FmVec2& outPos, const char* strPos);

	// 是否需要检测矩形安全区 
	bool NeedCheckRectangularSafeArea(IKernel* pKernel);

	// 检测是否进入离开矩形安全区
	static int HB_CheckRectangularSafeArea(IKernel* pKernel, const PERSISTID& self, int slice);

	// 检测是否在矩形安全区内 	 int 安全区id
	void CheckEnterRectangularSafeArea(IKernel* pKernel, const PERSISTID& self);

	// 检测是否在指定的安全区内
	bool CheckInRectSafeArea(IKernel* pKernel, const PERSISTID& self, const Rectangle2D& rect);

	// 根据id查询安全区数据
	const RectangularSafeArea* QueryRectangularSafeAreaInfo(int nIndex);
public:
	static SafeAreaNpcModule * m_pSafeAreaNpc;

private:
	static AsynCtrlModule * m_pAsynCtrlModule;
	RectangularSafeAreaVec			m_vecRectangularSafeArea;			//安全区数据
};

#endif // __SafeAreaNpc_H__
