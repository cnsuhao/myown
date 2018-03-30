//--------------------------------------------------------------------
// 文件名:      AITemplateBase.h
// 内  容:      NPC基础模板
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef _AI_TEMPLATE_BASE_H_
#define _AI_TEMPLATE_BASE_H_
#include "Fsgame/Define/header.h"
#include "FsGame/NpcBaseModule/AI/AIDefine.h"
#include <vector>

//NPC主状态类型
enum 
{
	AI_STATE_BORN = 0, //出生
	AI_STATE_PATROL,   //巡逻
	AI_STATE_FIGHT,    //战斗
	AI_STATE_BACK,     //往回走
	AI_STATE_DIED,     //死亡
    AI_STATE_FOLLOW,   //跟随
    AI_RECUPERATE,     //休养
	AI_ESCAPE,		   //逃跑
};

//NPC 巡逻的子状态
enum
{
	AI_PATROL_WAIT = 0,		// 休息状态
	AI_PATROL_WALK,			// 走动状态
	AI_PATROL_STOP,			// 暂停状态
};

class AITemplateBase
{
public:
	AITemplateBase(void);
	virtual ~AITemplateBase();
public:

	//获得模版类型
	const AI_TEMPLATE_TYPE GetTemplateType();

	//注册事件
	bool RegistEventRule(AI_RULE_TYPE nEvent,int nRule);

public:

    //初始化
    virtual bool Init(IKernel* pKernel);

	//基本行为结束的消息处理
	virtual int	OnBasicActionMsg(IKernel* pKernel, const PERSISTID& self, int msg, const IVarList& args = CVarList());

	//出生
	virtual int OnBorn(IKernel * pKernel,const PERSISTID & self, const PERSISTID & sender, const IVarList & args);

	//模版处理控制
	virtual int OnProcess(IKernel* pKernel, AI_RULE_TYPE processType, const PERSISTID& self, const PERSISTID& sender = PERSISTID(), const IVarList& args = CVarList());

    //巡逻
    virtual int BeginPatrol(IKernel * pKernel, const PERSISTID & sel) const;

public:

	//结束巡逻
	int EndPatrol(IKernel * pKernel , const PERSISTID & self) const;

	//改变AI状态
	int ChangeState(IKernel * pKernel, const PERSISTID & self, int state) const;

	//获取子规则类型
	int GetSubAIRule(int nRuleType,const PERSISTID & self) const;

	//产生随机的巡逻点
	void InitRandomPatrolPath(IKernel * pKernel, const PERSISTID & self, bool doStop = true) const;

	// 怪物创建初始化属性
	void InitPropAfterBorn(IKernel* pKernel, const PERSISTID& self) const;

	// 添加技能
	void InitSkillAfterBorn(IKernel * pKernel, const PERSISTID& self) const;

    //NPC出生公告（现在用于有国家的BOSS）
    void BornNotice(IKernel *pKernel, const PERSISTID &self) const;


protected:

    //当前模板类型编号
	AI_TEMPLATE_TYPE m_templateType;

	//事件规则对应表
	std::vector< int > m_EventRuleVct;
};

#endif // _AI__TEMPLATE_BASE_H_
