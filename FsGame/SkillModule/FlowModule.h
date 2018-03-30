//--------------------------------------------------------------------
// 文件名:      Server\FsRoomLogic\skill_module\FlowModule.h
// 内  容:      流程管理系统
// 说  明:      所有的流程都遵循以下的状态：
//              开始|----准备----|----命中----|结束
// 创建日期:    2008/01/28
// 创建人:      冯红文
//    :       
//--------------------------------------------------------------------

#ifndef __FlowModule_h__
#define __FlowModule_h__

#include "Fsgame/Define/header.h"

#include "FsGame/Define/FlowCategoryDefine.h"
#include <map>
#include <string>

//流程阶段结束响应函数定义
typedef bool (__cdecl* FLOW_RESPONSE_FUNC)(IKernel* pKernel, const PERSISTID& self);

class FlowModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);
    //释放
    virtual bool Shut(IKernel* pKernel);

public:
    /**
    @brief        注册一个流程分类
    @param        参数
    [IN]category:流程分类名称
    [IN]beforepreparefunc:流程准备阶段开始时调用的函数
    [IN]preparefunc:流程准备阶段时间结束时调用的函数
    [IN]beforeleadfunc:流程引导阶段开始时调用的函数
    [IN]leadfunc:每次引导结束时调用的函数
    [IN]beforehitfunc:流程命中阶段开始时调用的函数
    [IN]hitfunc:命中时间结束时调用的函数
    [IN]breakfunc:流程被打断时调用的函数
    [IN]pausefunc:流程暂停结束时调用的函数
    [IN]finishfunc:流程结束时调用的函数
    @remarks
    @return       注册成功返回true,失败返回false
    */
    bool RegisterFlow(int iFlowCategory,
                      FLOW_RESPONSE_FUNC beforepreparefunc,
                      FLOW_RESPONSE_FUNC preparefunc,
                      FLOW_RESPONSE_FUNC beforeleadfunc,
                      FLOW_RESPONSE_FUNC leadfunc,
                      FLOW_RESPONSE_FUNC hitfunc,
                      FLOW_RESPONSE_FUNC breakfunc,
                      //FLOW_RESPONSE_FUNC pausefunc,
                      FLOW_RESPONSE_FUNC finishfunc);

    /**
    @brief        开始一个普通的流程
    @param        参数
    [IN]iFlowCategory:流程分类
    [IN]preparetime:准备时间，单位为ms
    [IN]hittime:命中时间，单位为ms
    @remarks      注意:该流程分类需要已经被注册，每个对象只能同时拥有一个流程
    @return       成功开始一个流程，返回true，失败返回false
    */
    bool BeginFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory,
                   int preparetime, int hittime, const char* flowtextid = "");

    /**
    @brief        开始一个普通的流程
    @param        参数
    [IN]iFlowCategory:流程分类
    [IN]preparetime:准备时间，单位为ms
    [IN]hittimes:命中次数
    [IN]hittime:与命中次数相同的时间间隔
    @remarks      注意:该流程分类需要已经被注册，每个对象只能同时拥有一个流程
    @return       成功开始一个流程，返回true，失败返回false
    */
    bool BeginFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory,
                   int preparetime, const IVarList& hittime, const char* flowtextid = "");

    /**
    @brief        开始一个引导流程
    @param        参数
    [IN]iFlowCategory:流程分类
    [IN]preparetime:准备时间，单位为ms
    [IN]leadlifetime:引导持续时间，单位为ms
    [IN]leadseptime:引导间隔时间，单位为ms
    [IN]hittime:命中时间，单位为ms
    @remarks      注意:该流程分类需要已经被注册，每个对象只能同时拥一个流程
    @return       成功开始一个流程，返回true，失败返回false
    */
    bool BeginLeadFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory,
                       int preparetime, int leadlifetime, int leadseptime, int hittime);

    // 结束蓄力
    bool EndStorage(IKernel* pKernel, const PERSISTID& self);

    /**
    @brief        强制结束当前流程
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool BreakFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        强制结束任意当前滚程
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool BreakAnyFlow(IKernel* pKernel, const PERSISTID& self);

    /**
    @brief        暂停当前流程(只有在准备期中的流程可以暂停)
    @param        参数
    [IN]说明
    [IN]bbreak:为true时表示暂停时间到后终止流程
    @remarks
    @return
    */
    bool PauseFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory, int slice, bool bbreak = false);

    /**
    @brief        继续当前流程
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool ResumeFlow(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        当前是否处于一个流程过程中
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool IsFlowing(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        是否在一个任意流程中
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool IsAnyFlowing(IKernel* pKernel, const PERSISTID& self);

    // 释放在蓄力中
    bool IsStoraging(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        是否在一个流程的准备期中
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool IsPreparing(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        是否在一个流程的引导期中
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool IsLeading(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        是否在一个流程的命中期中
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    bool IsHiting(IKernel* pKernel, const PERSISTID& self, int iFlowCategory);

    /**
    @brief        获得当前流程的流程分类名称
    @param        参数
    [IN]说明
    @remarks
    @return
    */
    int QueryCategory(IKernel* pKernel, const PERSISTID& self);

private:

    static int OnRequestMove(IKernel* pKernel, const PERSISTID& self,
                             const PERSISTID& sender, const IVarList& args);

    static int OnCommandBreakFlow(IKernel* pKernel, const PERSISTID& self,
                                  const PERSISTID& sender, const IVarList& args);

    //流程相关心跳
    static int H_Flow_Prepare(IKernel* pKernel, const PERSISTID& self, int slice);
    static int H_Flow_Lead(IKernel* pKernel, const PERSISTID& self, int slice);
    static int H_Flow_Hit(IKernel* pKernel, const PERSISTID& self, int slice);

private:
    //开始准备期
    void BeginPrepare(IKernel* pKernel, const PERSISTID& self, const char* flowtextid = "");
    //开始引导期
    void BeginLead(IKernel* pKernel, const PERSISTID& self);
    //开始命中期
    void BeginHit(IKernel* pKernel, const PERSISTID& self);
    //流程结束
    void Finish(IKernel* pKernel, const PERSISTID& self);

    //获得流程准备函数（如果配置了准备心跳，则在心跳开始前执行）
    FLOW_RESPONSE_FUNC GetBeforePrepareFunc(IKernel* pKernel, const PERSISTID& self);
    //获得流程准备函数（准备阶段结束执行）
    FLOW_RESPONSE_FUNC GetPrepareFunc(IKernel* pKernel, const PERSISTID& self);
    //获得流程引导函数(进入引导阶段后，立即执行）
    FLOW_RESPONSE_FUNC GetBeforeLeadFunc(IKernel* pKernel, const PERSISTID& self);
    //获得流程引导函数（引导阶段每次心跳结束前执行）
    FLOW_RESPONSE_FUNC GetLeadFunc(IKernel* pKernel, const PERSISTID& self);
    //流程命中阶段每次心跳结束前执行的函数
    FLOW_RESPONSE_FUNC GetHitFunc(IKernel* pKernel, const PERSISTID& self);
    //获得流程打断函数
    FLOW_RESPONSE_FUNC GetBreakFunc(IKernel* pKernel, const PERSISTID& self);
    //获得流程完成函数
    FLOW_RESPONSE_FUNC GetFinishFunc(IKernel* pKernel, const PERSISTID& self);

public:
    static FlowModule* m_pInstance; //为上述四个静态函数使用的指针
private:
    struct S_FLOW_FUNC
    {
        FLOW_RESPONSE_FUNC beforepreparefunc;
        FLOW_RESPONSE_FUNC preparefunc;//准备
        FLOW_RESPONSE_FUNC beforeleadfunc;
        FLOW_RESPONSE_FUNC leadfunc;   //引导
        FLOW_RESPONSE_FUNC hitfunc;    //打击
        FLOW_RESPONSE_FUNC breakfunc;  //打断
        FLOW_RESPONSE_FUNC finishfunc; //完成
    };

    //某个流程的函数定义
    std::map<int, S_FLOW_FUNC> m_mapflowfunc;
};
#endif