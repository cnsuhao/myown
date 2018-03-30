//--------------------------------------------------------------------
// 文件名:      AIROnDeadBase.h
// 内  容:      死亡规则
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once
#include "FsGame/NpcBaseModule/AI/Rule/AIRuleBase.h"

class AIROnDeadBase : public AIRuleBase
{
public:
	AIROnDeadBase(void);

	//执行该规则
	virtual int DoRule(IKernel * pKernel,const PERSISTID & self,const PERSISTID & sender,const IVarList & args, AITemplateBase &templateWrap);
protected:
	// 伤害数据
	struct DamageData 
	{
		PERSISTID	player;			// 玩家对象号
		int			nDamageVal;		// 伤害值

		bool operator < (const DamageData& ch) const
		{
			return nDamageVal > ch.nDamageVal;
		}
	};

	typedef std::vector<DamageData> DamageDataVec;

    //取得NPC死亡归属,还没有死亡前取得的值会是错的
    int GetNpcAscription(IKernel * pKernel, const PERSISTID & self, const PERSISTID & sender, IVarList &varKillers);

	//获取NPC死亡伤害前n位
	int GetNpcAscriptionFront(IKernel * pKernel, const PERSISTID & self, IVarList &varKillers);

    //通知任务模块相关逻辑
    void InformTask(IKernel *pKernel, const PERSISTID &self, const PERSISTID &killer);
};
