//--------------------------------------------------------------------
// 文件名:      EquipStrengthenModule.h
// 内  容:      装备强化
// 说  明:
// 创建日期:    2014年11月1日
// 创建人:        
// 修改人:		
//    :       
//--------------------------------------------------------------------
#ifndef FSGAME_ITEMBASE_EQUIP_STRENGTHEN_MODULE_H_
#define FSGAME_ITEMBASE_EQUIP_STRENGTHEN_MODULE_H_

#include "Fsgame/Define/header.h"
#include "EquipStrengthenDefine.h"

class EquipStrengthenModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);

    //释放
    virtual bool Shut(IKernel* pKernel);

	// 加载资源
	bool LoadRes(IKernel* pKernel);

	//加载消耗资源
	bool LoadConsumeRes(IKernel* pKernel);

	// 加载强化限制配置
	bool LoadStrLimitRes(IKernel* pKernel);

	// 加载强化套装配置
	bool LoadStrSuitRes(IKernel* pKernel);

	// 加载强化外观配置
	bool LoadStrApperRes(IKernel* pKernel);

	// 释放配置文件
	bool ReleaseResource(IKernel* pKernel);

private:
	// 强化套装检查
	void SuitCheck(IKernel* pKernel, const PERSISTID& self);

	// 取得强化消耗
	bool GetStrConsume(const int str_level, Consume_Vec& capitals, Consume_Vec& items);

	// 取得强化限制等级
	const int GetLimitStrLevel(IKernel* pKernel, const PERSISTID& self, int equip_index);

	// 取得最高强化等级
	const int QueryMaxStrenLvl(IKernel* pKernel, const PERSISTID& self);

	// 取得最低强化等级
	const int GetMinStrenLvl(IKernel* pKernel, const PERSISTID& self);

	// 取得套装激发的属性包
	const int GetSuitActivePkg(const int min_str_lvl);

public:
    //一键强化
    int AutoStrengthenEquip(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// player请求执行强化
	int ExecuteStrengthenEquip(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 取得指定部位的强化等级
	const int GetStrLvlByPos(IKernel* pKernel, const PERSISTID& self, 
		const int target_pos);

	// 取得强化外观
	const int GetStrApperance(const char *equip_id, const int str_lvl);
	
private:
	// 客户端消息
	static int OnCustomEquipStrengthen(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 重新加载装备强化配置
	static void ReloadStrengthenConfig(IKernel* pKernel);

public:

	// 实体类单例指针
	static EquipStrengthenModule * m_pInstance;

	// 强化等级限制
	StrengthenLimitVec m_StrengthenLimitVec;

	// 强化消耗
	StrenthenConsumeVec m_StrenthenConsumeVec;

	// 强化套装
	StrengthenSuitList m_StrengthenSuitList;

	// 强化外观
	StrengthenApperanceVec m_StrengthenApperanceVec;
};

#endif 
