#include "EnergyModule.h"
#include "Define/GameDefine.h"
#include "CommonModule/EnvirValueModule.h"
#include <time.h>

EnergyModule* EnergyModule::m_pEnergyModule = NULL;

// 初始化
bool EnergyModule::Init(IKernel* pKernel)
{
	m_pEnergyModule = this;
	Assert(NULL != m_pEnergyModule);

	pKernel->AddEventCallback("player", "OnRecover", OnPlayerRecover);
	pKernel->AddEventCallback("player", "OnReady", OnPlayerReady);
	pKernel->AddEventCallback("player", "OnStore", OnPlayerStore);

	DECL_HEARTBEAT(EnergyModule::HB_RestoreHP);
	DECL_HEARTBEAT(EnergyModule::HB_RestoreMP);
	return true;
}

// 更新HP MP
bool EnergyModule::UpdateEnergy(IKernel* pKernel, const PERSISTID &self, EnergyType type, int64_t nChangeValue)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || 0 == nChangeValue || type <= ENERGY_NONE || type >= ENERGY_MAX)
	{
		return false;
	}
	// 死亡时不能改变
	int nDead = pSelfObj->QueryInt(FIELD_PROP_DEAD);
	if (nDead > 0)
	{
		return false;
	}

	if (ENERGY_HP == type)
	{
		m_pEnergyModule->UpdateHP(pKernel, pSelfObj, nChangeValue);
	}
	else
	{
		m_pEnergyModule->UpdateMP(pKernel, pSelfObj, nChangeValue);
	}
	return true;
}

// 是否足够耗蓝
bool EnergyModule::IsMPEnough(IKernel* pKernel, const PERSISTID &self, int nDecMP)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{																						    
		return false;
	}
	// 只有玩家用技能耗蓝
	if (pSelfObj->GetClassType() != TYPE_PLAYER)
	{
		return true;
	}

	int64_t nCurMP = pSelfObj->QueryInt64(FIELD_PROP_MP);
	return nCurMP - nDecMP > 0;
}

/// \brief 上线
int EnergyModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSeflObj = pKernel->GetGameObj(self);
	if (NULL == pSeflObj)
	{
		return 0;
	}

	//重置HP时间
	int64_t resetTime = static_cast<int64_t>(EnvirValueModule::EnvirQueryInt(ENV_VALUE_RESET_MAX_HP_TIME));
	int64_t logoutTime = pSeflObj->QueryInt64(FIELD_PROP_ROLE_LOGOUT_TIME);

	//血量数据
	int64_t maxHP = pSeflObj->QueryInt64(FIELD_PROP_MAX_HP);
	int64_t iLastHP = pSeflObj->QueryInt64(FIELD_PROP_TEMP_HP);

	int64_t maxMP = pSeflObj->QueryInt64(FIELD_PROP_MAX_MP);
	int64_t iLastMP = pSeflObj->QueryInt64(FIELD_PROP_TEMP_MP);

	//时间判断是否重置HP
	if (::time(NULL) - logoutTime >= resetTime)
	{
		pSeflObj->SetInt64(FIELD_PROP_HP, maxHP);
		pSeflObj->SetInt64(FIELD_PROP_MP, maxMP);
	}
	else
	{
		pSeflObj->SetInt64(FIELD_PROP_HP, iLastHP > maxHP ? maxHP : iLastHP);
		pSeflObj->SetInt64(FIELD_PROP_MP, iLastMP > maxMP ? maxMP : iLastMP);
	}
	return 0;
}

int EnergyModule::OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	int nRestoreTime = EnvirValueModule::EnvirQueryInt(ENV_VALUE_HP_RESTORE_TIME) * 1000;
	ADD_HEART_BEAT(pKernel, self, "EnergyModule::HB_RestoreHP", nRestoreTime);

	nRestoreTime = EnvirValueModule::EnvirQueryInt(ENV_VALUE_MP_RESTORE_TIME) * 1000;
	ADD_HEART_BEAT(pKernel, self, "EnergyModule::HB_RestoreMP", nRestoreTime);
	return 0;
}

int EnergyModule::OnPlayerStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return 0;
	}

	const int type = args.IntVal(0);
	if (STORE_EXIT == type)
	{
		int64_t nCurHP = pPlayer->QueryInt64(FIELD_PROP_HP);
		pPlayer->SetInt64(FIELD_PROP_TEMP_HP, nCurHP);

		int64_t nCurMP = pPlayer->QueryInt64(FIELD_PROP_MP);
		pPlayer->SetInt64(FIELD_PROP_TEMP_MP, nCurMP);
	}

	return 0;
}

// 配置读取
bool EnergyModule::LoadConfig(IKernel* pKernel)
{
	return true;
}

// 回复hp的心跳
int EnergyModule::HB_RestoreHP(IKernel * pKernel, const PERSISTID & self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	int nFightState = pSelfObj->QueryInt(FIELD_PROP_FIGHT_STATE);
	// 战斗状态不回复HP
	if (nFightState > 0)
	{
		return 0;
	}

	int nAddHP = EnvirValueModule::EnvirQueryInt(ENV_VALUE_RESTORE_HP);
	m_pEnergyModule->UpdateHP(pKernel, pSelfObj, nAddHP);
	return 0;
}

// 回复mp的心跳
int EnergyModule::HB_RestoreMP(IKernel * pKernel, const PERSISTID & self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}
	int nFightState = pSelfObj->QueryInt(FIELD_PROP_FIGHT_STATE);
	int nAddMP = nFightState > 0 ? EnvirValueModule::EnvirQueryInt(ENV_VALUE_FIGHT_RESTORE_MP) : EnvirValueModule::EnvirQueryInt(ENV_VALUE_NORMAL_RESTORE_MP);
	m_pEnergyModule->UpdateMP(pKernel, pSelfObj, nAddMP);
	return 0;
}

// 重新加载配置
void EnergyModule::ReloadConfig(IKernel* pKernel)
{

}

// 更新HP
void EnergyModule::UpdateHP(IKernel* pKernel, IGameObj* pSelfObj, int64_t nChangeValue)
{
	 if (NULL == pSelfObj)
	 {
		 return;
	 }
	 int64_t nLastHP = pSelfObj->QueryInt64(FIELD_PROP_HP);
	 if (nChangeValue > 0)
	 {
		 int64_t nMaxHP = pSelfObj->QueryInt64(FIELD_PROP_MAX_HP);
		 int64_t nCurHP = __min(nLastHP + nChangeValue, nMaxHP);

		 pSelfObj->SetInt64(FIELD_PROP_HP, nCurHP);
	 }
	 else
	 {
		 int64_t nCurHP = __max(nLastHP + nChangeValue, 0);	
		 pSelfObj->SetInt64(FIELD_PROP_HP, nCurHP);
// 		 if (nCurHP == 0)
// 		 {
// 			 pSelfObj->SetInt(FIELD_PROP_DEAD, 1);
// 		 }
	 }
}

// 更新MP
void EnergyModule::UpdateMP(IKernel* pKernel, IGameObj* pSelfObj, int64_t nChangeValue)
{
	// 只有玩家有蓝条
	if (NULL == pSelfObj || pSelfObj->GetClassType() != TYPE_PLAYER)
	{
		return;
	}
	int64_t nLastMP = pSelfObj->QueryInt64(FIELD_PROP_MP);
	if (nChangeValue > 0)
	{
		int64_t nMaxMP = pSelfObj->QueryInt64(FIELD_PROP_MAX_MP);
		int64_t nCurMP = __min(nLastMP + nChangeValue, nMaxMP);

		pSelfObj->SetInt64(FIELD_PROP_MP, nCurMP);
	}
	else
	{
		int64_t nCurMP = __max(nLastMP + nChangeValue, 0);
		pSelfObj->SetInt64(FIELD_PROP_MP, nCurMP);
	}
}