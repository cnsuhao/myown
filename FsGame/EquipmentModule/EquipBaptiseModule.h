//--------------------------------------------------------------------
// 文件名:      EquipBaptiseModule.h
// 内  容:      洗练模块
// 说  明:		
// 创建日期:		2018年03月12日
// 创建人:       tzt      
//--------------------------------------------------------------------

#ifndef _EquipBaptiseModule_h__
#define _EquipBaptiseModule_h__


#include "Fsgame/Define/header.h"
#include "EquipBaptiseDefine.h"

#include <string>
#include <map>
#include <vector>

class EquipBaptiseModule : public ILogicModule
{
public:

	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

	// 配置加载
	bool LoadRes(IKernel* pKernel);

	// 
	bool ReleaseRes(IKernel* pKernel);

private:
	// 加载洗练属性配置
	bool LoadBPRes(IKernel* pKernel);

	// 加载洗练属性取值配置
	bool LoadBPRangeRes(IKernel* pKernel);

	// 加载洗练属性上限配置
	bool LoadBPLimitRes(IKernel* pKernel);

	// 加载洗练消耗配置
	bool LoadBConsumeRes(IKernel* pKernel);

	// 取得洗练属性表
	const String_Vec *GetPropList(const int index, const int job);

	// 取得洗练属性取值
	const int GetPropValue(const int index, const int job, 
		const int baptise_type, const char *prop_name);

	// 取得洗练属性上限值
	const int GetPropLimitValue(const int index, const int job,
		const int str_lvl, const char *prop_name);

	// 取得洗练消耗
	const BaptiseConsume *GetBaptiseConsume(const int baptise_type);

	// 拼接json字符串
	void ConvertToJsonStr(const String_Vec& str_list, const Integer_Vec int_list, 
		std::string& json_str);

	// 纠正洗练值
	void CorrectBaptiseValue(IKernel* pKernel, const PERSISTID& self, 
		const int equip_pos, const char *prop_name, const int str_lvl, int& final_value);

	// 指定部位洗练属性是否全部达到上限
	bool IsAllPropMaxed(IKernel* pKernel, const PERSISTID& self,
		const int equip_pos);

public:
	// 执行一次洗练
	void ExecuteBaptise(IKernel* pKernel, const PERSISTID& self, 
		const IVarList& args);

	// 保存洗练结果
	void SaveResult(IKernel* pKernel, const PERSISTID& self,
		const int equip_pos);

	// 放弃洗练结果
	void GiveupResult(IKernel* pKernel, const PERSISTID& self,
		const int equip_pos);

	// 锁定属性
	void LockProp(IKernel* pKernel, const PERSISTID& self,
		const IVarList& args);

	// 解锁属性
	void UnLockProp(IKernel* pKernel, const PERSISTID& self,
		const IVarList& args);

	// 初始化每个装备部位的洗练信息（上线调用）
	void InitBaptise(IKernel* pKernel, const PERSISTID& self);

	// 取得玩家的槽位的洗练属性列表
	void GetPropNameList(IKernel* pKernel, const PERSISTID& self,
		const int equip_pos, String_Vec& prop_name_list);

private:
	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家准备就绪
	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
		
	// 处理客户端发送消息
	static int OnCustomMsg(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
   
	// 重新加载装备配置
	static void ReloadEquipConfig(IKernel* pKernel);

private:
	BaptisePropVec m_BaptisePropVec;
	BPRangeVec m_BPRangeVec;
	BPLimitValueVec m_BPLimitValueVec;
	BConsumeVec m_BConsumeVec;
     
public:
	static EquipBaptiseModule* m_pEquipBaptiseModule;

};

#endif