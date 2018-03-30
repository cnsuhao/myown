//--------------------------------------------------------------------
// 文件名:      SkillModule.h
// 内  容:      技能系统主逻辑
// 说  明:
// 创建日期:    2014年10月23日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#ifndef _SkillModule_H_
#define _SkillModule_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/SkillDefine.h"

class OffLineModule;
class SkillModule : public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 释放
    virtual bool Shut(IKernel* pKernel);

    // 加载skill相关的全部资源
    bool LoadAllResource(IKernel* pKernel);

public:
     
    /**
    *@brief  使用技能接口，技能的作用规则由配置数据决定
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 施法者
    *@param  [in] skill 使用技能对象
    *@param  [in] args 参数列表：可以是包含一个技能作用对象，可以是一个技能作用目标点，也可以是空列表
    *@return int 返回1表示释放成功，其他值代表对应才错误号返回 
    */
    int UseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const IVarList& args);

	// 准备随机使用一个技能
	bool ReadyUseRandomSkill(IKernel* pKernel, const PERSISTID& self, float& fSkillMinDis, float& fSkillMaxDis, const char* strSkillId);
protected:
    // -------------------------事件回调函数------------------------------


    // Npc加载回调
    static int OnNpcLoad(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 客户端就绪
    static int OnReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 上线 回调函数
    static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 技能容器创建回调
    static int OnSkillContainerCreateClass(IKernel* pKernel, int index);

    // 技能从数据库恢复
    static int OnSkillRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 技能加载配置文件完成
    static int OnSkillLoad(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // ------------------------------自定义消息回调-----------------------------
    void DoUseSkillParamProcess(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target);

    // 使用技能
    static int OnCustomUseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 使用技能(客户端通知命中处理)
	static int OnCustomDoHitByClient(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应客户端打断技能请求
	static int OnCustomBreakCurSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
private:

    static bool RegisterSkillContainer(IKernel* pKernel, const PERSISTID& self, const wchar_t* name, int capacity, bool unsave);
    static bool AddSkillViewport(IKernel* pKernel, const PERSISTID& self, int iViewPortID, const wchar_t* name);

    // 刷新某个技能的属性
    bool RefreshSkillConfigProperty(IKernel* pKernel, const PERSISTID& skill);

	// 预加载技能配置数据
	bool PreLoadSkillConfig(IKernel* pKernel);

	// 技能升级回调
	static int C_OnSkillLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);

	// 是否能够使用技能回调   
	static int C_OnCantUseSkillChanged(IKernel* pKernel, const PERSISTID& self,
		const char* property,const IVar& old);

	// 重新加载技能配置
	static void ReloadSkillConfig(IKernel* pKernel);

	// 玩家变身弩车用的技能使用限制
	static bool IsCanUseSkill(IKernel* pKernel, const PERSISTID& self, const char* strSkillId);
public:
    // 刷新所有技能的属性,GM重新加载技能配置用
    bool RefreshAllSkillConfigProperty(IKernel* pKernel, const PERSISTID& self);
public:
    // 本模块指针
    static SkillModule* m_pInstance;
};

#endif
