//--------------------------------------------------------------------
// 文件名:      EquipmentModule.h
// 内  容:      装备模块
// 说  明:		
// 创建日期:    2014年10月24日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef EquipmentModule_h__
#define EquipmentModule_h__


#include "Fsgame/Define/header.h"
#include "EquipDefine.h"

#include <string>
#include <map>
#include <vector>

class DropModule;

class EquipmentModule : public ILogicModule
{
public:

	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 释放
	virtual bool Shut(IKernel* pKernel);

public:

    // 装备的灵魂状态
    struct SoulState
    {
    
        SoulState() : equip_all_set(false), soul_all_set(false), soul_level(0), soul_class(0)
        {}
    
        bool equip_all_set; // 是否穿带了所有装备
        bool soul_all_set;  // 是否已镶满灵魂
        int soul_level;     // 所有灵魂中的最低等级
        int soul_class;     // 所有灵魂中的最低阶 (一阶灵魂...N阶灵魂)
    };
    
    // 简单的装备信息
    struct EquipInfo
    {
        std::string config_id;
        int color;
        EquipInfo():config_id(""),
                    color(0)
        {

        }
    };

	// 玩家上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 玩家准备就绪
	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
		
	// 装备栏类创建时的回调
	static int OnEquipBoxClassCreate(IKernel* pKernel, int index);

	//装备栏的回调
	static int OnEquipBoxCreate(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	static int OnEquipBoxAfterAdd(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	static int OnEquipBoxRemove(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	static int OnEquipBoxNoAdd(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& item, const IVarList& args);

	static int OnEquipBoxNoRemove(IKernel* pKernel, const PERSISTID& equipbox,
		const PERSISTID& sender, const IVarList& args);

	static int GetEquipCount( IKernel* pKernel, const PERSISTID& self );
	static int GetEquipCountByColor( IKernel* pKernel, const PERSISTID& self, int nColor );
	static int GetEquipCountByGreaterColor( IKernel* pKernel, const PERSISTID& self, int nMinColor );

    // 检查角色身上的已镶嵌灵魂状态
    static bool CheckSoul(IKernel* pKernel, const PERSISTID& self,
        SoulState *soul_state);
        
    // 获取角色身上的武器信息, 返回NULL 表示此位置没有装备
    static const char* GetEquipConfig(IKernel* pKernel, const PERSISTID& self, int equip_index);

    // 获取角色身上的武器信息
    static bool GetEquipInfo(IKernel* pKernel, const PERSISTID& self, int equip_index, EquipInfo *info);

	//能否放入装备栏（检测灵魂祭坛中是否有相同的装备）
	//static bool CanPutIntoEquip(IKernel* pKernel,const PERSISTID& self,const PERSISTID& item);
public:
	//判断一件装备是否可装备到玩家的装备栏的指定位置
	EQUIPCHECK_RESULT CanEquip(IKernel* pKernel, const PERSISTID& self, const PERSISTID& item, int pos);
	//检查装备是否可以被更换
	EQUIPCHECK_RESULT CanChangeEquip(IKernel* pKernel, const PERSISTID& owner, const PERSISTID& equip);

	//获得某个格子的装备类型
	std::string GetEquipType(int index);
	//获得某个类型对应的格子
	int GetEquipIndex(const char * szType);
	// 是否匹配
	bool IsMatch(int classlimit, int condition);

	//刷新可视装备
	bool RefreshVisible(IKernel* pKernel, const PERSISTID& self);

	// 载入配置文件
    void LoadResource(IKernel* pKernel);

	// 释放配置文件
	void ReleaseResource(IKernel* pKernel);

	//报告装备错误
	void ReportEquipError(IKernel* pKernel, const PERSISTID& equip, EQUIPCHECK_RESULT result);

	// 创建装备, @equip_prop_percent 表示取随机属性范围，取值范围是[0, 100], 默认值是-1, 表示不指定范围
	bool CreateEquip(IKernel * pKernel, const PERSISTID & container, const char * config, int count, int equip_prop_percent = -1);
    
    // 穿上翅膀
    void OnPutonEquipment(IKernel * pKernel, const PERSISTID &owner, const PERSISTID & equipment);
    
    // 脱下翅膀
    void OnTakeoffEquipment(IKernel * pKernel, const PERSISTID &owner, const PERSISTID & equipment);

    // 获得装备随机属性, @equip_prop_percent 表示取随机属性范围，取值范围是[0, 100], 默认值是-1, 表示不指定范围
    int SetRandEquipProp(IKernel* pKernel, const PERSISTID & self, 
        const PERSISTID & equip, int color_level, int prop_percent = -1);

    // 获得附加属性随机值
    int GetRandAddProp(int minValue, int maxValue, int prop_percent = -1);

	// 重新加载装备配置
	static void ReloadEquipConfig(IKernel* pKernel);
     
public:

	static EquipmentModule* m_pEquipmentModule;
    static DropModule * m_pDropModule;

	std::map<int, std::string> m_EquipIndexName;
	std::map<std::string, int> m_EquipNameIndex;
};

#endif // EquipmentModule_h__