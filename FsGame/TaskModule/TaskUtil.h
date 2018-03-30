// -------------------------------------------
// 文件名称： TaskUtil.h
// 文件说明： 任务工具类
// 创建日期： 2018/02/26
// 创 建 人：  tzt  
// -------------------------------------------
	
#ifndef _TaskUtil_H_
#define _TaskUtil_H_

#include "FsGame/Define/header.h"
#include "TaskDefine.h"
#include "Data/TaskStructDefine.h"


class TaskUtil
{
public:
	// 初始化
	bool Init(IKernel *pKernel);

	// 查询任务状态
	const TaskStatus QueryTaskStatus(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 前置任务是否已提交
	bool IsPrevTaskSubmit(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 任务是否已开启
	bool IsStart(IKernel *pKernel, const PERSISTID &self,
		const int task_type);

	// 任务是否有次数
	bool IsHaveNumOfType(IKernel *pKernel, const PERSISTID &self,
		const int task_type);

	// 当前是否有指定类型的任务
	bool IsHaveOfType(IKernel *pKernel, const PERSISTID &self,
		const int task_type);

	// 当前是否有指定规则的任务
	bool IsHaveOfRule(IKernel *pKernel, const PERSISTID &self,
		const int task_rule);

	// 随机指定任务类型的任务
	const int RandomTask(IKernel *pKernel, const PERSISTID &self,
		const int task_type);

	// 记录任务完成次数
	void RecordTaskNum(IKernel *pKernel, const PERSISTID &self,
		const int task_type);

	// 接取后置任务
	void AcceptPostTask(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 当前主线任务安全检查
	void MainTaskSafeCheck(IKernel *pKernel, const PERSISTID &self);

	// 任务规则自检测
	void SelfCheck(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 任务自动切场景
	void AutoSwitchScene(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 自动播放主线任务cg
	void AutoPlayTaskCG(IKernel *pKernel, const PERSISTID &self);

	// 创建任务副本
	bool CreateGroupClone(IKernel *pKernel, const PERSISTID &self,
		const int task_id);

	// 清理任务副本
	void CleanGroupClone(IKernel *pKernel, const PERSISTID &self, 
		const int task_id);

	// 更新任务
	void UpdateTask(IKernel *pKernel, const PERSISTID &self,
		const IVarList &args);

	// 提交任务
	void SubmitTask(IKernel *pKernel, const PERSISTID &self,
		const IVarList &args);

	// 设置自动寻路临时状态
	void SetAutoPathFind(IKernel *pKernel, const PERSISTID &self, 
		const IVarList &args);

	// 更新杀怪任务
	void UpdateKillNpc(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &npc);

	// 转换CoupleElementList为string
	bool ConvertCEL2Str(const CoupleElementList& src_list, std::string& target_str, 
		bool zero_value = false, const char *del1 = ":", const char *del2 = ",");

	// 转换string为CoupleElementList
	bool ConvertStr2CEL(const std::string& src_str, CoupleElementList& target_list,
		const char *del1 = ":", const char *del2 = ",");

	// 任务场景验证
	bool IsTargetScene(IKernel *pKernel, const NumList& scene_list,
		const int cur_scene = 0);

	// 坐标验证
	bool IsTargetPosition(IKernel *pKernel, const PositionList& position_list,
		const Position& position, const float distance = COMMON_VERIFY_DISTANCE);

private:
	// 延迟更新击杀Npc任务心跳
	static int HB_DelayUpdateKillNpc(IKernel* pKernel, const PERSISTID& self, int slice);
};

typedef HPSingleton<TaskUtil> TaskUtilS;
#endif
