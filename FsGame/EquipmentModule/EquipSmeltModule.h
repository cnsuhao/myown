//--------------------------------------------------------------------
// 文件名:      EquipSmeltModule.h
// 内  容:      装备熔炼
// 说  明:
// 创建日期:    2014年11月27日
// 创建人:        
// 修改人:		
//    :       
//--------------------------------------------------------------------
#ifndef EquipSmeltModule_h__
#define EquipSmeltModule_h__

#include "Fsgame/Define/header.h"
#include "EquipSmeltDefine.h"

class EquipSmeltModule : public ILogicModule
{

public:
	//初始化
	virtual bool Init(IKernel* pKernel);

    //清理
    virtual bool Shut(IKernel* pKernel);

private:
	// 加载熔炼相关配置
	bool LoadSmeltSetting(IKernel* pKernel, const char* file);

	// 解析并销毁原料, 返回产出熔炼值
	const int ConsumeRawMaterial(IKernel* pKernel, const PERSISTID& self, 
		const IVarList &uid_list);

	// 检查 @item 是否存在于 @items 中
	bool HasItem(const IVarList &items, const PERSISTID &item);

	// 取得输出熔炼值
	const float GetOutputSmeltValue(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID &item);
  
public:
	// 执行熔炼
	int DoSmeltEquip(IKernel* pKernel, const PERSISTID& self, 
		const IVarList& args);

private:
	// 熔炼装备
	static int OnCustomEquipSmelt(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

public:
	//实体指针
	static EquipSmeltModule *m_pEquipSmeltModule;
};
#endif // EquipSmeltModule_h__