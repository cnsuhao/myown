//--------------------------------------------------------------------
// 文件名:		ItemBaseModule.h
// 内  容:		物品基础数据
// 说  明:		
// 创建日期:	2014年10月17日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------
#ifndef _ItemBaseModule_H
#define _ItemBaseModule_H

#include "Fsgame/Define/header.h"
#include "Fsgame/Define/FunctionEventDefine.h"

class EquipmentModule;
// #ifndef FSROOMLOGIC_EXPORTS
// class BianstoneModule;
// #endif

class ItemBaseModule: public ILogicModule
{
public:
	virtual bool Init(IKernel* pKernel);
	virtual bool Shut(IKernel* pKernel);

	// 加载物品配置
	bool LoadItemPathConfig(IKernel* pKernel);
	
	//物品排序
	static bool ItemSort(IKernel * pKernel, CVarList args, CVarList &returnList);
private:
	static int OnCreateClass(IKernel* pKernel, int index);

	static int OnCreate(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	static int OnLoad(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	static int OnRecover(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	//属性回调
	static int C_OnAmountChanged(IKernel * pKernel, const PERSISTID & self, const char* property, const IVar & old);

public:
	//创建物品, @equip_prop_percent 表示装备随机属性范围，取值范围是[0, 100], 默认值是-1, 表示不指定范围
	PERSISTID CreateItem(IKernel * pKernel, const PERSISTID & container, const char * config, int count,
							EmFunctionEventId nSrcFromId = FUNCTION_EVENT_ID_SYS, bool bIsGain = false,
							int equip_prop_percent = -1);

	// 重新加载物品基础配置
	static void ReloadItemBaseConfig(IKernel* pKernel);

	static ItemBaseModule * m_pItemBaseModule;

private:

	static EquipmentModule * m_pEquipmentModule;
// #ifndef FSROOMLOGIC_EXPORTS
// 	static BianstoneModule* m_pBianstoneModule;
// #endif
};

#endif // _ItemBaseModule_H

