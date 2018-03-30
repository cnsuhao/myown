// -------------------------------------------
// 文件名称：	 CommRuleModule.h
// 文件说明： 常用规则处理（定义一些通用并且简单的规则）
// 创建日期： 2018/03/02
// 创 建 人：  tzt
// -------------------------------------------

#ifndef __CommRuleModule_H__
#define __CommRuleModule_H__

#include "CommRuleDefine.h"

class CommRuleModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel *pKernel);

	// 关闭
	virtual bool Shut(IKernel *pKernel);
public:
	// 应用杀怪规则
	static void ApplyKillNpcRule(IKernel *pKernel, const PERSISTID &npc);

	// 应用杀人规则
	static void ApplyKillPlayerRule(IKernel *pKernel, const PERSISTID &deader);

	// 记录攻击玩家
	static void RecordAttackPlayer(IKernel *pKernel, const PERSISTID &sufferer,
		const PERSISTID &damager);

	// 玩家改变分组
	static bool ChangeGroup(IKernel* pKernel, const PERSISTID& self,
		const int change_type, const int new_group = 0);

	// 播放CG
	static void PlayCG(IKernel* pKernel, const PERSISTID& self,
		const char *cg_id, const IVarList &args = CVarList());

	// 解析为消耗列表
	static void ParseConsumeVec(const char *src_str, Consume_Vec& consume_vec, 
		const char *del_first = ",", const char *del_second = ":");

	// 可否消耗物品
	static bool CanDecItems(IKernel *pKernel, const PERSISTID &self,
		const Consume_Vec &items);

	// 扣除物品
	static bool ConsumeItems(IKernel *pKernel, const PERSISTID &self,
		const Consume_Vec &items, const int function_id);

	// 可否消耗资金
	static bool CanDecCapitals(IKernel *pKernel, const PERSISTID &self,
		const Consume_Vec &capitals);

	// 扣除资金
	static bool ConsumeCapitals(IKernel *pKernel, const PERSISTID &self,
		const Consume_Vec &capitals, const int function_id);

	// 是否合法装备部位
	static bool IsValidEquipPos(const int equip_pos);

	// 是否合法职业
	static bool IsValidJob(const int job);

	// 是否合法品质
	static bool IsValidColor(const int color);

private:
	// NPC被杀时回调
	static int OnNpcBeKilled(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 玩家被击杀回调
	static int OnPlayerBeKilled(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 玩家被伤害回调
	static int OnPlayerBeDamaged(IKernel* pKernel, const PERSISTID& self,		  
		const PERSISTID& sender, const IVarList& args);

	// CG播放结束
	static int OnCustomCGDone(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 队伍共享击杀
	void KillNpc_TeamShare(IKernel *pKernel, const PERSISTID &npc);

	// 有效击杀玩家验证
	void KillPlayer_Validate(IKernel *pKernel, const PERSISTID &deader);

	// 记录有效击杀
	bool RecordValidKill(IKernel* pKernel, const PERSISTID& self,
		const char *deader_uid);

public:
	static CommRuleModule *m_pThis;
    
};
//typedef HPSingleton<CommRuleModule> CommRuleS;

#endif

