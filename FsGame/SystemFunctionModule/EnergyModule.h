//--------------------------------------------------------------------
// 文件名:		EnergyModule.h
// 内  容:		能量模块(HP,MP)
// 说  明:		
//				
// 创建日期:	2018年03月09日
// 整理日期:	
// 创建人:		liumf   
//--------------------------------------------------------------------

#ifndef __ENERGY_MODULE_H__
#define __ENERGY_MODULE_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include "Define/GameDefine.h"

class EnergyModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	virtual bool Shut(IKernel* pKernel){ return true; }

	// 更新HP MP
	static bool UpdateEnergy(IKernel* pKernel, const PERSISTID &self, EnergyType type, int64_t nChangeValue);

	// 是否足够耗蓝
	static bool IsMPEnough(IKernel* pKernel, const PERSISTID &self, int nDecMP);
private:
	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 配置读取
	bool LoadConfig(IKernel* pKernel);

	// 回复hp的心跳
	static int HB_RestoreHP(IKernel * pKernel, const PERSISTID & self, int slice);
						
	// 回复mp的心跳
	static int HB_RestoreMP(IKernel * pKernel, const PERSISTID & self, int slice);

	// 重新加载配置
	static void ReloadConfig(IKernel* pKernel);

	// 更新HP
	void UpdateHP(IKernel* pKernel, IGameObj* pSelfObj, int64_t nChangeValue);

	// 更新MP
	void UpdateMP(IKernel* pKernel, IGameObj* pSelfObj, int64_t nChangeValue);
private:
	static EnergyModule* m_pEnergyModule;
};

#endif //__ENERGY_MODULE_H__
