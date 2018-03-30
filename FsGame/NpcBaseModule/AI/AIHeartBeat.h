//--------------------------------------------------------------------
// 文件名:      AIHeartBeat.h
// 内  容:      AI系统心跳
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "Fsgame/Define/header.h"
class AIHeartBeat
{
public:
	AIHeartBeat(void);
public:
	//战斗心跳
	static int HB_AIFighting(IKernel* pKernel, const PERSISTID& self, int slice);

	//巡逻心跳
	static int HB_AIPatrol(IKernel* pKernel, const PERSISTID& self, int slice);

	// 护送Npc行动检查
	static int HB_AIEndFight(IKernel* pKernel, const PERSISTID& self, int slice);

	//出生心跳
	static int HB_AIBornEnd(IKernel* pKernel, const PERSISTID& self, int slice);

	//转身处理
	static int HB_AIRatate(IKernel* pKernel, const PERSISTID& self, int slice);

	//搜索怪物心跳
	static int HB_AIFind(IKernel* pKernel, const PERSISTID& self, int slice);

	//延时拷贝玩家数据
	static int HB_AIClonePlayer(IKernel* pKernel, const PERSISTID& self, int slice);

    //普通怪回出生点超时保护
    static int HB_BACK_BORN_TIMEOUT(IKernel *pKernel, const PERSISTID &self, int slice);

	// 押送Npc跟随心跳
// 	static int HB_EscortNpcFollow(IKernel *pKernel, const PERSISTID &self, int slice);
// 
// 	// 定位护送Npc心跳
// 	static int HB_LocationConvyNpc(IKernel* pKernel, const PERSISTID& self, int slice);
// 
// 	// 延迟护送Npc移动
// 	static int HB_DelayConvoy(IKernel* pKernel, const PERSISTID& self, int slice);

	// 逃跑结束心跳
	static int HB_EndEscape(IKernel* pKernel, const PERSISTID& self, int slice);

	// 检测逃跑心跳
	static int HB_CheckEscape(IKernel* pKernel, const PERSISTID& self, int slice);
};
