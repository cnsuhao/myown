//--------------------------------------------------------------------
// 文件名:      ModifyPackModule.h
// 内  容:      管理各种修正包和目标修正表的维护
// 说  明:
// 创建日期:    2014年11月1日
// 创建人:      
// 修改人:      
//    :       
//--------------------------------------------------------------------
#ifndef _ModifyPackModule_h
#define _ModifyPackModule_h

#include "Fsgame/Define/header.h"
#include "../Define/ModifyPackDefine.h"

class EquipModifyPackModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);

    //清理
    virtual bool Shut(IKernel* pKernel);
private:

    //回调函数 玩家加载数据完成
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//穿装备
	static int OnEquipAfterAdd(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	//脱装备
    static int OnEquipRemove(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);
	
	//强化等级改变回调
	static int OnEquipStrLevelUp(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& equip, const IVarList& args);

	//强化套装改变回调
	static int OnEquipStrSuit(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//洗炼属性发生变化
	static int OnEquipBaptisePropChange(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& equip, const IVarList& args);

	//附加随机属性变化回调
	static int OnEquipAddModifyChange(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& equip, const IVarList& args);

	//宝石镶嵌or升级or拆除
	static int OnEquipJewelChange(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& equip, const IVarList& args);      

	// 等级变化回调
	static int OnDCLevelChange(IKernel* pKernel, const PERSISTID &self, const PERSISTID & sender, const IVarList & args);

    // 时装变化回调
    static int OnFashionChanged(IKernel* pKernel, const PERSISTID &self, const PERSISTID & sender, const IVarList & args);

	// 刷新战斗力
 	static int RefreshEquipAbility(IKernel* pKernel, const PERSISTID& equip, 
 		const PERSISTID& self, const IVarList& args);

private:

    // 初始化修正表
    void InitPropModifyRec(IKernel* pKernel, const PERSISTID& self);

	/// \brief 装备特殊属性刷新方式
	bool UpdateEquipModifyObjProp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, MapPropValue& mapPropValue);
	
	/// \brief 删除装备特殊属性的影响
	bool RemoveEquipModifyObjProp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, MapPropValue& mapPropValue);

	/// \brief 强化属性修改
	bool UpdateEquipStrPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, int oldProp, int newProp);

	/// \brief 洗炼属性修改
	bool UpdateEquipBaptisePack(IKernel* pKernel, const PERSISTID& self, IVarList& args, const PERSISTID& equip);

	/// \brief 附加随机属性修改
	bool UpdateEquipAddModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, const char * oldProp, const char *  newProp);

	/// \brief 更新宝石属性
	void UpdateEquipJewelPack(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    /// \brief 更新灵魂外观属性
    void UpdateSoulAppearancePack(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

    /// 时装带来的属性变化
    void UpdateFashionPack(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 对信息存储表的操作
	void HandleSaveRec(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, 
		const std::string& strPropName, const float fValue);

public:

    // 本模块指针
    static EquipModifyPackModule*        m_pInstance;
};

#endif
