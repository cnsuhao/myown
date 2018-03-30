//--------------------------------------------------------------------
// 文件名:      JewelModule.h
// 内  容:      宝石模块
// 说  明:		
// 创建日期:    2014年12月5日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef JewelModule_h__
#define JewelModule_h__

#include "Fsgame/Define/header.h"
#include "JewelDefine.h"

class ContainerModule;
class CapitalModule;
class AsynCtrlModule;

// 精简过的装备模块
class JewelModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

	// 加载资源
	bool LoadResource(IKernel* pKernel);

	// 加载宝石索引映射配置
	bool LoadJewelIndexCfg(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

public:
 
	// 宝石相关消息
    static int OnCustomJewel(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

	// 获取玩家身上大于等于某个等级的宝石数量(运营活动用)
	static int GetJewelsByLevel(IKernel* pKernel, const PERSISTID& self, const int level);

	// 重新加载宝石配置
	static void ReloadJewelConfig(IKernel* pKernel);

public:

	// 宝石镶嵌
	int ExecuteJewelInlay(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 摘下宝石
	int DismantleJewel(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 合成宝石
	int ComposeJewel(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 摘下
	bool Dismantle(IKernel* pKernel, const PERSISTID& self, 
		const int equip_idx, const int jewel_idx);

private:
	//获得镶嵌了的宝石数量
	int GetExecuteJewelNum(IKernel*pKernel, const PERSISTID& self);

	// 是否有相似宝石
	bool JewelHaveSame(IKernel* pKernel, const PERSISTID& self, int equiprow, int jewelindex, int newType);

	// 指定部位是否可以使用指定类型宝石
	bool CanUseJewel(const int jewel_type, const int equip_pos);

	// 根据索引号取宝石ID
	const char* GetJewelID(const int index);

	// 统计合成材料
	bool CountMateria(IKernel* pKernel, const IVarList &res, 
		const int target_level, JewelComposeConsume &compose_material, bool buy = false);

	// 扣除宝石
	 bool DecJewels(IKernel *pKernel, const PERSISTID &self,
		const JewelMap &jewels, const int log_id);

private:

    static ContainerModule* m_pContainerModule;
    static CapitalModule* m_pCapitalModule;
    static AsynCtrlModule* m_pAsynCtrlModule;

	JewelUsePosVec m_JewelUsePosVec;
	JewelIndexMap m_JewelIndexMap;

public:
    static JewelModule* m_pInstance;
    

};
#endif // JewelModule_h__