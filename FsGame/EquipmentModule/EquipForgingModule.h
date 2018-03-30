//--------------------------------------------------------------------
// 文件名:      EquipForging.h
// 内  容:      装备锻造
// 说  明:
// 创建日期:    2017年04月19日
// 创建人:       tongzt
// 修改人:		
//    :       
//--------------------------------------------------------------------
#ifndef FSGAME_ITEMBASE_EQUIP_FORGING_MODULE_H_
#define FSGAME_ITEMBASE_EQUIP_FORGING_MODULE_H_

#include "Fsgame/Define/header.h"
#include "EquipForgingDefine.h"

class EquipForgingModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

    // 加载资源
    bool LoadResource(IKernel* pKernel);

	// 锻造
	void Forging(IKernel *pKernel, const PERSISTID &self, 
		const char *drawing);

	// 重新加载锻造装备配置
	static void ReloadForgingConfig(IKernel* pKernel);
    
private:   
    // 锻造设置
    bool LoadForgingRule(IKernel* pKernel);
		
	// 取得锻造规则
	const ForgingRule* GetForgingRule(const char *drawing);

	// 随机产出品质
	const int RandomOutputColor(const OutputColorWeightVec& output_list, const int sum_weight);

	// 锻造消息
	static int OnCustomEquipmentForging(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
	
public:

	static EquipForgingModule * m_pInstance;
	
private: 
    ForgingRuleVec m_ForgingRuleVec;
};

#endif 
