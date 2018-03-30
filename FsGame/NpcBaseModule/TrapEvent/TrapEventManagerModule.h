//--------------------------------------------------------------------
// 文件名:      TrapEventManager.h
// 内  容:      陷阱触发事件处理器
// 说  明:
// 创建日期:    2015年07月13日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#ifndef _TrapEventManager_H_
#define _TrapEventManager_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/TrapEventFuncIdDefine.h"

class TrapEventBase;
class TrapEventManagerModule : public ILogicModule
{
public:
    TrapEventManagerModule()  {
		memset(m_pEventContainer, 0, sizeof(m_pEventContainer));
	}
    ~TrapEventManagerModule() {}

public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 清理
    virtual bool Shut(IKernel* pKernel);

public:
    // 加载事件处理函数
    bool InitEventExecutes(IKernel* pKernel);

	// 处理单个执行操作
	bool ExecuteOperate(IKernel* pKernel, const PERSISTID& self, const PERSISTID& object, const SpringEvent funcID);
private:
	// 事件注册
	bool RegistTrapEvent(const SpringEvent func_id);
private:
	TrapEventBase*		m_pEventContainer[SPRINGEVENT_MAX];			// 陷阱触发事件容器
public:
    static TrapEventManagerModule*   m_pInstance;
};

#endif // _SkillEventManager_H_
