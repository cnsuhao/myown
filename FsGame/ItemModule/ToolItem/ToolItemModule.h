//---------------------------------------------------------
//文件名:       ToolItemModule.h
//内  容:       可被使用的道具类物品
//说  明:       
//          
//创建日期:      2014年11月24日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef FSGAME_ITEM_TOOL_ITEM_MODULE_H
#define FSGAME_ITEM_TOOL_ITEM_MODULE_H

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ToolItemDefine.h"
#include <vector>
#include <map>

class DropModule;
class CapitalModule;
class FlowModule;
class ContainerModule;
class CoolDownModule;
class EquipmentModule;
class BufferModule;
class LogModule;
class ItemEffectBase;

class ToolItemModule: public ILogicModule
{
public:

    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

public:
	// 判断使用物品是否在cd中
	bool IsItemCoolDown(IKernel* pKernel, const PERSISTID& self, const PERSISTID& item);

    // 使用物品, 例如使用恢复生命的道具
    bool UseItem(IKernel* pKernel, const PERSISTID& user,
        const PERSISTID& sender,  const PERSISTID& item, int nCount );

	// 稀有物品提示
	// strFrom来源
	int TipsSpecialItem(IKernel* pKernel, const PERSISTID& self, 
		const char* itemConfigID, const int color, const EmFunctionEventId src);

	// 物品是否配匹查询规
	static bool IsMatchState(IKernel* pKernel, const PERSISTID& item, int nMatch);

	// 物品是否支持绑定
	static bool IsBindAble(IKernel* pKernel, const char* pszItem);
	static bool IsBindAble(IKernel* pKernel, const PERSISTID& item);
	// 物品是否支持绑定(支持装备类物品，nColor: 当pszItem是装备类物品时才需要，表示品质)
	static bool IsBindAble(IKernel* pKernel, const char* pszItem, int nColor);

	// 是否为可装备的物品
	static bool IsCanWearItem(IKernel* pKernel, const char* strItemConfig);
private:
    //////////////////////////////////////////////////////////////////////////
    // 物品使用效果
    //////////////////////////////////////////////////////////////////////////

    // 使用物品, 返回消耗的物品数量
    int InnerUseItem(IKernel *pKernel, const PERSISTID &user, const PERSISTID &item, int request_amount);

	// 展示获取的物品
	static void ShowGetItem(IKernel *pKernel, const PERSISTID &user, const IVarList& showItems);

    //////////////////////////////////////////////////////////////////////////
    // 辅助函数
    //////////////////////////////////////////////////////////////////////////
    // 道具可否使用
    bool IsItemUsable(IKernel* pKernel, const PERSISTID& user,
        const PERSISTID& item, int amount);

    // 消耗物品, 返回实际消耗的数量
    int ConsumItem(IKernel* pKernel, const PERSISTID& item, int request_amount); 

    //////////////////////////////////////////////////////////////////////////
    // 资源配置
    //////////////////////////////////////////////////////////////////////////

    // 获得使用包的条目
    const std::vector<ItemEffectParam> *GetEffectEntry(const std::string &package_name);

    bool GetEffectID(std::string &the_name, int *the_id);
public:
    //获取道具效果信息
    bool GetItemEffectInfo(IKernel* pKernel, const char* itemConfig, int effectType, CVarList& args);
public:
    // 载入资源
    bool LoadResource(IKernel *pKernel);

    // 物品使用效果的分类
    bool LoadEffectTypeConfig(IKernel *pKernel);

    // 物品使用效果包
    bool LoadEffectPackage(IKernel *pKernel);

	// 重新加载道具配置
	static void ReloadToolItemConfig(IKernel* pKernel);
public:

    static ToolItemModule* m_pToolItemModule;

private:
    typedef std::map<std::string, int> EffectTypeMap;
    typedef std::map<std::string, std::vector<ItemEffectParam> > EffectParamMap;

    EffectTypeMap m_EffectType;
    EffectParamMap m_EffectParam;

	ItemEffectBase* m_pArrItemEffect[MAX_ITEM_EFFECT_NUM];		// 物品使用效果

    static DropModule *m_pDropModule;
    static FlowModule *m_pFlowModule;
    static CapitalModule *m_pCapitalModule;
    static ContainerModule *m_pContainerModule;
    static CoolDownModule *m_pCoolDownModule;
    static EquipmentModule *m_pEquipmentModule;
    static BufferModule *m_pBufferModule;
    static LogModule *m_pLogModule;
};

#endif // _ToolItemModule_H

