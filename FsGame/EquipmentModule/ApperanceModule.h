//--------------------------------------------------------------------
// 文件名:      ApperanceModule.h
// 内  容:      外观模块
// 说  明:		
// 创建日期:    2018年03月14日
// 创建人:      tzt      
//--------------------------------------------------------------------
#ifndef ApperanceModule_h__
#define ApperanceModule_h__

#include "Fsgame/Define/header.h"
#include "EquipDefine.h"
#include "ApperanceDefine.h"

class ApperanceModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

private:
	// 加载外观优先级配置
	bool LoadApperancePriRes(IKernel* pKernel);

	// 加载外观穿戴属性包配置
	bool LoadApperancePkgRes(IKernel* pKernel);

	// 取得外观显示优先级
	const int GetApperancePri(const int src);

	// 取得外观穿戴属性包
	const int GetApperancePkg(const int apperance_id);

	// 改变外观
	void Change(IKernel *pKernel, const PERSISTID &self,
		const EQUIP_POS equip_pos);

public:
    // 增加一个外观
	void Add(IKernel *pKernel, const PERSISTID &self, 
		const EQUIP_POS equip_pos, const ApperanceSource src, const int apperance_id);

	// 根据装备部位和来源删除一个外观
	void Remove(IKernel *pKernel, const PERSISTID &self,
		const EQUIP_POS equip_pos, const ApperanceSource src);

public:
	// 根据性别取得装备外观
	const int GetEquipApperance(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &equip);

	// 根据性别取得装备时装
	const int GetEquipFashion(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &equip);

	// 取得指定位置的外观ID
	const int GetApperanceByEquipPos(IKernel *pKernel, const PERSISTID &self,
		const EQUIP_POS equip_pos);

	// 取得指定部位外观属性
	const char *GetApperanceProp(const EQUIP_POS equip_pos);

	// 初始化穿戴属性包
	void InitPackage(IKernel *pKernel, const PERSISTID &self);

private:
	// 穿装备
	static int OnEquipBoxAfterAdd(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	// 脱装备
	static int OnEquipBoxRemove(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	// 获得物品
	static int OnItemBoxAfterAdd(IKernel* pKernel, const PERSISTID& item_box,
		const PERSISTID& sender, const IVarList& args);

	// 强化等级变化
	static int OnStrLvlChanged(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

	// 玩家数据恢复完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

private:
	ApperancePriVec m_ApperancePriVec;
	ApperancePkgVec m_ApperancePkgVec;

public:
	static ApperanceModule *m_pThis;
};

#endif // ApperanceModule_h__