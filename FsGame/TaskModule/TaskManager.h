// -------------------------------------------
// 文件名称： TaskManager.h
// 文件说明： 任务管理模块
// 创建日期： 2018/02/24
// 创 建 人：  tzt  
// -------------------------------------------
	
#ifndef _TaskManager_H_
#define _TaskManager_H_

#include "FsGame/Define/header.h"
#include "Template/TaskTemplate.h"
#include "Rule/TaskBaseRule.h"

class TaskManager : public ILogicModule
{
public:
	// 初始化
    virtual bool Init(IKernel *pKernel);

	// 关闭
    virtual bool Shut(IKernel *pKernel);

    // 加载资源文件
    bool LoadRes(IKernel *pKernel);

private:
	// 注册任务模板
	bool RegistTemplate(IKernel *pKernel, TaskTemplate *pTemplate);

	// 初始化任务模板
	bool InitTemplates(IKernel *pKernel);

	// 创建规则类
	TaskBaseRule *CreateRule(const TaskRules rule);

	// 初始化任务规则
	bool InitRules(IKernel *pKernel);

public:
	// 取得任务模板
	TaskTemplate* GetTemplate(const TaskTypes task_type);

	// 取得任务规则
	TaskBaseRule* GetRule(const TaskRules task_rule);

	// 为所有任务注册定时器
	void RegistResetTimer(IKernel *pKernel, const PERSISTID &self);

	// 获取任务domain
	static const std::wstring& GetDomainName(IKernel * pKernel);

	// 重载配置函数
#define DefineReloadFunc(funcname) \
	static void Reload##funcname(IKernel *pKernel)

	DefineReloadFunc(Task_All);		// 所有任务重载
	DefineReloadFunc(Task_Main);	// 主线任务重载
	DefineReloadFunc(Task_Branch);	// 支线任务重载
	DefineReloadFunc(Task_Daily);	// 日常任务重载

private:
	// 任务动作
	static int OnCommandTaskAction(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 客户端准备就绪
	static int OnReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 任务command消息
	static int OnCommandTask(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 客户端任务消息
	static int OnCustomTask(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 玩家从数据库恢复数据完成（玩家上线）
	static int OnPlayerRecover(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

	// 玩家移动
	static int OnCommandMotion(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &sender, const IVarList &args);

	// 等级提升 
	static int OnCommandLevelUp(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 玩家离开场景
	static int OnLeaveScence(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 击杀Npc
	static int OnKillNpc(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 击杀玩家
	static int OnKillPlayer(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// CG播放结束消息
	static int OnCommandCGDone(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

private:
	// 任务模板函数
	TaskTemplate* m_pTemplates[TASK_TYPE_TOTAL];

	// 任务规则函数
	TaskBaseRule* m_pRules[TASK_RULE_TOTAL];

public:
	// 公共数据名称
	static std::wstring m_domainName;
	static TaskManager *m_pThis;
};

//typedef HPSingleton<TaskManager> TaskManagerSingleton;
#endif
