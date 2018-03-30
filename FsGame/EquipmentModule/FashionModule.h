//---------------------------------------------------------
//文件名:       FashionModule.h
//内  容:       时装模块
//说  明:               
//创建日期:      2017年02月21日
//创建人:        tongzt 
//修改人:    
//---------------------------------------------------------

#ifndef FSGAME_FASHION_MODULE_H_
#define FSGAME_FASHION_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FashionDefine.h"


class FashionModule : public ILogicModule
{
public:

    virtual bool Init(IKernel* pKernel);

    virtual bool Shut(IKernel *pKernel);

    bool LoadResource(IKernel *pKernel);

private:
	// 加载时装配置
	bool LoadFashionConfig(IKernel* pKernel);

	// 取得时装配置
	const FashionCfg* GetFashionCfg(const int id);

	// 可否解锁
	bool CanUnlock(IKernel* pKernel, const PERSISTID& self, 
		const int fashion_id);

	// 前置是否已解锁
	bool IsPreUnlock(IKernel* pKernel, const PERSISTID& self, 
		const int pre_fashion_id);

public:
	// 解锁
	void Unlock(IKernel* pKernel, const PERSISTID& self,
		const int fashion_id);

	// 穿
	void PutOn(IKernel* pKernel, const PERSISTID& self,
		const int fashion_id);

	// 脱
	void TakeOff(IKernel* pKernel, const PERSISTID& self,
		const int fashion_id);

	// 是否适用
	bool IsSuit(int limit, const int v);

	// 找出适合玩家的时装id
	const int FindRightFashion(IKernel* pKernel, const PERSISTID& self,
		const IVarList& fashion_list);

	// 是否可使用物品解锁
	bool CanUse(IKernel* pKernel, const PERSISTID& self,
		const IVarList& fashion_list);

	// GM锁定时装
	void GMLock(IKernel* pKernel, const PERSISTID& self,
		const int fashion_id);

	// 无条件解锁指定时装
	bool GmUnlock(IKernel* pKernel, const PERSISTID& self,
		const int fashion_id, const int valid_day, const int log_type);

	// 一键解锁时装
	bool OnekeyUnlock(IKernel* pKernel, const PERSISTID& self, 
		const int log_type, const char *fashion_str, const int valid_day = 0, const char *delims = ",");

	// 一键锁定时装
	void OnekeyLock(IKernel* pKernel, const PERSISTID& self,
		const char *fashion_str, const char *delims = ",");

	// 时效检查
	void ValidTimeCheck(IKernel* pKernel, const PERSISTID& self);

private:
    // 玩家数据恢复完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);
 
    // 来自玩家的消息
	static int OnCustomMsg(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args); 

	// 时效性检查
	static int HB_ValidTime(IKernel* pKernel, const PERSISTID& self,
		int slice);

public:
	// 重载
	static void ReloadConfig(IKernel* pKernel);

public:
    static FashionModule *m_pFashionModule;
    
private:
	FashionCfgVec m_FashionCfgVec;

}; // End of class

#endif