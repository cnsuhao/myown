//--------------------------------------------------------------------
// 文件名:      LevelModule.h
// 内  容:      经验与等级相关模块
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef FSGAME_COMMON_LEVEL_MODULE_H_
#define FSGAME_COMMON_LEVEL_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/LevelDefine.h"
#include <vector>

class LevelModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);

	//释放
	virtual bool Shut(IKernel* pKernel);

	// 客户端请求等级相关消息
	static int  OnCustomMsgLevelLock(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
public:
	/// \brief 添加角色的经验函数
	/// \param pKernel 功能接口指针
	/// \param self 对象
	/// \param type 经验添加类型
	/// \param exp 添加经验值
	bool AddExp(IKernel* pKernel, const PERSISTID& self, int nSrcFrom, int64_t exp);

	/// \brief 获取角色升级所需经验函数
	/// \param pKernel 功能接口指针
	/// \param self 对象
	/// \param level 等级
	int64_t GetLevelUpgradeExp(IKernel* pKernel, const PERSISTID& self, int level);

	// 读取配置信息
	bool LoadResource(IKernel* pKernel);

	// 取得角色等级上限值
	int GetPlayerMaxLevel(IKernel* pKernel);
	/// \brief 尝试增加经验值
	/// \param pKernel 功能接口指针
	/// \param self 对象
	/// \param exp 添加经验值
	static bool TryAddExp(IKernel* pKernel, const PERSISTID& self, int64_t exp);
private:
	//------------------------------回调----------------------------------
	//等级改变回调
	static int C_OnLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property,
		const IVar& old);

	//刷新升级所需经验
	static int OnRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
private:
	// 升级
	bool LevelUp(IKernel* pKernel, const PERSISTID& self, int newlevel, int64_t curExp);

	// 刷新等级变化后的属性
	bool RefreshLevelUp(IKernel* pKernel, const PERSISTID& obj, const int old_level, int curLevel);

    // 玩家升级信息记录
	void SaveLevelUpInfo(IKernel* pKernel, const PERSISTID& self, const int oldLevel, const int newLevel);

	static void ReloadConfig(IKernel* pKernel);
public:
	static LevelModule* m_pLevelModule;

private:         
	//角色最高级别
	int m_nMaxLevel;

	//各级升级所需经验
	std::vector<int64_t> m_vecExpToUpgrade;
};

#endif // FSGAME_COMMON_LEVEL_MODULE_H_
