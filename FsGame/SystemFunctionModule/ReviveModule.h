//--------------------------------------------------------------------
// 文件名:		ReviveModule.h
// 内  容:		死亡重生
// 说  明:		
//				
// 创建日期:	2018年03月13日
// 整理日期:	
// 创建人:		liumf   
//--------------------------------------------------------------------

#ifndef __REVIVE_MODULE_H__
#define __REVIVE_MODULE_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include "Define/GameDefine.h"
#include <map>

class ReviveModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	virtual bool Shut(IKernel* pKernel){ return true; }
private:
	// 复活面板配置
	struct RevivePanelConfig_s
	{
		RevivePanelConfig_s() :m_sceneID(0),
			m_revivePanel("")
		{}
		int m_sceneID;                    // 原型场景ID
		std::string m_revivePanel;        // 复活面板
	};

	typedef std::map< int, RevivePanelConfig_s > RevivePanelConfigMap; // <原型场景ID, 复活面板配置>

	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//请求复活消息处理 
	static int OnCustomRevive(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& msg);

	// 相应请求复活
	void OnCustomRequestRevive(IKernel* pKernel, const PERSISTID& self, const IVarList& msg);

	// 玩家死亡
	static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 加载配置
	bool LoadConfig(IKernel* pKernel);

	// 重新加载
	static void ReLoadConfig(IKernel* pKernel);

	// 加载复活面板配置
	bool LoadRevivePanelConfig(IKernel* pKernel);

	//应场景复活配置
	RevivePanelConfig_s* GetRevivePanelConfig(IKernel* pKernel);

	// 原地复活
	void LocalRevive(IKernel* pKernel, const PERSISTID& self);

	// 就近复活
	void NearbyRevive(IKernel* pKernel, const PERSISTID& self);

	// 城中复活
	void CityRevive(IKernel* pKernel, const PERSISTID& self);

	// 复活玩家
	void PlayerRevive(IKernel* pKernel, const PERSISTID& self);

	// 校验复活类型
	bool CheckReviveType(IKernel* pKernel, const int reviveType);
private:
	static ReviveModule* m_pReviveModule;

	RevivePanelConfigMap  m_mapRevivePanelConfig;    //复活面板配置
};

#endif //__ENERGY_MODULE_H__
