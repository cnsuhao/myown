//----------------------------------------------------------
// 文件名:      RideModule.h
// 内  容:      坐骑模块
// 说  明:
// 创建日期:  
// 创建人:      
// 修改人:    
//----------------------------------------------------------

#ifndef __RIDE_MODULE_H__
#define __RIDE_MODULE_H__

#include "Fsgame/Define/header.h"
#include "RideDefine.h"

class RideModule : public ILogicModule
{
typedef std::string String;

public:
    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

	// 坐骑配置文件加载
	bool LoadResource(IKernel *pKernel);

	// 获取激活坐骑
	PERSISTID GetActivedRide(IKernel *pKernel, const PERSISTID &self);

	// 重新加载坐骑配置
	static void ReloadRideConfig(IKernel* pKernel);

private:
	// 阶级配置加载
	bool LoadUpgradeStepRes(IKernel* pKernel);

	// 坐骑皮肤配置文件加载
	bool LoadRideSkinRes(IKernel *pKernel);

	// 坐骑皮肤解锁配置加载
	bool LoadSkinUnlocRes(IKernel *pKernel);

private:
	// 获得坐骑皮肤配置
	const RideSkin* GetRideSkin(const int skin_id);

	// 取得升阶信息
	const RideStepCfg* GetRideStepCfg(const int step);

	// 取得皮肤解锁配置
	const SkinUnlockCfg* GetSkinUnlockCfg(const int id);

	// 可否升阶
	bool CanUpgradeStep(const int step, const int star);

	// 初始化时设置属性包ID
	bool SetRideAddPckID(IKernel* pKernel, const PERSISTID& Player, 
		const PERSISTID& Ride);

	// 取得要修正包中要修正的属性-值
	bool GetPckMapPropValue(IKernel *pKernel, const PERSISTID &self, 
		const UpdatePkgTypes up_pkg_type, const IVarList& prop_names, 
		const IVarList& prop_values, std::map<std::string, double>& map_prop_value);

	// 取得坐骑显示和隐藏时的更新修正包方式
	const UpdatePkgTypes GetRideUpPckType(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& ride);

	// 激活坐骑
	bool ActiveRide(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID &ride);

	// 使用某皮肤
	bool UseDefaultSkin(IKernel *pKernel, const PERSISTID &ride);

	// 前置皮肤是否已解锁
	bool IsUnlockPreSkin(IKernel* pKernel, const PERSISTID& self,
		const int skin_id);

	// 已解锁
	bool IsUnlock(IKernel* pKernel, const PERSISTID& self,
		const int skin_id);

	// 可否切换为升阶皮肤
	bool CanChangeStepSkin(IKernel* pKernel, const PERSISTID& self);

public:

	// 根据类型取得皮肤ID
	const int GetSkinByType(const int skin_type);

	// 根据类型取得皮肤ID
	const int GetSkinByType(IKernel *pKernel, const PERSISTID &self, 
		const int skin_type);

	// 查询当前坐骑阶级
	const int QueryRideStep(IKernel *pKernel, const PERSISTID &self);

	// 新增坐骑
	bool AddRide(IKernel *pKernel, const PERSISTID &self, 
		const char *rideConfig);

	// 新增坐骑皮肤
	bool AddRideSkin(IKernel *pKernel, const PERSISTID &self, 
		const int skin_id);

	// 删除坐骑皮肤
	bool RemoveRideSkin(IKernel *pKernel, const PERSISTID &self,
		const int skin_type);

	// 坐骑激活状态下客户端显示坐骑状态
	bool OnRide(IKernel *pKernel, const PERSISTID &self);

	// 坐骑激活状态下客户端退出坐骑状态
	bool DownRide(IKernel *pKernel, const PERSISTID &self);

	// 可否上坐骑
	bool CanRide(IKernel* pKernel, const PERSISTID& self);

	// 取得骑乘状态
	const int GetRidingState(IKernel* pKernel, const PERSISTID& self);

	// 是否能激活坐骑
	bool CanActiveRide(IKernel* pKernel, const PERSISTID& self, 
		const char *ride_config);


	/************************************************************************/
	/*									         客户端请求处理函数			*/
	/************************************************************************/
	
public:
	// 更换坐骑皮肤
	bool  ChangeRideSkin(IKernel *pKernel, const PERSISTID &self, 
		const int new_skin_id, const int ride_index = 1);

	// 推送坐骑皮肤信息
	bool  PushRideSkinMsg(IKernel *pKernel, const PERSISTID &self, 
		const int ride_index = 1);

	// 上下坐骑操作
	void RideUpDownOperate(IKernel* pKernel, const PERSISTID& self, 
		const int operate_type);

	// 去除新皮肤标识
	void ClearSkinFlag(IKernel* pKernel, const PERSISTID& self, const int skin_id);

	// 解锁坐骑皮肤
	bool UnlockSkin(IKernel *pKernel, const PERSISTID &self,
		const int skin_id, const int ride_index = 1);

	// 坐骑升级
	void UpgradeLevel(IKernel *pKernel, const PERSISTID &self);

	// 坐骑升阶
	void UpgradeStep(IKernel *pKernel, const PERSISTID &self);

	// 玩家上线
	int PlayerOnline(IKernel* pKernel, const PERSISTID& self);


	/************************************************************************/
	/*									         修正包处理函数				*/
	/************************************************************************/

public:
	// 更新坐骑修正包
	void UpdateRideAddPkg(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID& ride, const char *pkg_id, const IVarList& up_msg);

	// 为升级操作更新修正包
	void Upgrade2UpdateAddPkg(IKernel *pKernel, const PERSISTID &self, 
		const PERSISTID& ride);

	// 更新拥有的皮肤修正包
	void UpdateUseSkinPkg(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& ride, const IVarList& up_msg);


	/************************************************************************/
	/*									         回调函数					*/
	/************************************************************************/

private:
    // 玩家上线
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

    // 玩家准备就绪
    static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);

	// 玩家离开阵营战
	static int OnPlayerLeave(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);

    // 装备栏类创建时的回调
    static int OnRideBoxClassCreate(IKernel* pKernel, int index);

    // 装备栏的回调
    static int OnRideBoxCreate(IKernel* pKernel, const PERSISTID& ridebox,
        const PERSISTID& sender, const IVarList& args);

    // 技能释放下马回调
    static int OnCommandPlayerBeginSkill(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

	// 技能释放下马回调
	static int OnCommandPlayerFinishSkill(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	// 被添加debuff时下马
    static int OnCommandAddBuffer(IKernel * pKernel, const PERSISTID & self,
        const PERSISTID & sender, const IVarList & args);

	static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static int OnCommandBeDamage(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	static int OnCustomMessage(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 坐骑相关重置定时器
	static int MountResetTimer(IKernel *pKernel, const PERSISTID &self, 
		int slice);

	// 功能开启 
	static int OnCommandActiveFunc(IKernel *pKernel, const PERSISTID &self,
		const PERSISTID &sender, const IVarList &args);
	
private:
	// 升阶配置
	RideStepCfgList m_RideStepCfgList;

	// 坐骑皮肤配置
	RideSkinMap m_RideSkinMap;

	// 阶级上限
	int m_MaxStep;

	SkinUnlockCfgVec m_SkinUnlockCfgVec;
 

public:
	static RideModule* m_pRideModule;

};

#endif