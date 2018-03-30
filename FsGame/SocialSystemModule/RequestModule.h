/***********************************************************************
 玩家之间的交互请求系统定义
***********************************************************************/
#ifndef __RequestModule_h__
#define __RequestModule_h__

#include "Fsgame/Define/header.h"
#include "FsGame/Define/RequestDefine.h"
#include <vector>
#include <list>

class CoolDownModule;


//响应请求回调的函数
//能否发送请求的回调
typedef bool (_cdecl* REQUEST_CAN_REQUEST_CALLBACK)(IKernel* pKernel, const PERSISTID& self,
                                                    REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);

//能否被请求的回调
typedef bool (_cdecl* REQUEST_CAN_BEREQUEST_CALLBACK)(IKernel* pKernel, const PERSISTID& self,
                                                      REQUESTTYPE type, const wchar_t* srcname, const IVarList& paras);

//客户端同意请求后的回调
typedef bool (_cdecl* REQUEST_REQUEST_SUCCEED_CALLBACK)(IKernel* pKernel, const PERSISTID& self,
                                                        REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);


//请求者向被请求者发送命令前需要在命令中附加的参数的回调
typedef bool (_cdecl* REQUEST_PARA_CALL_BACK)(IKernel* pKernel, const PERSISTID& self,
                                              REQUESTTYPE type, IVarList& paras);

//请求的结果回调函数
typedef bool (_cdecl* REQUEST_RESULT_CALLBACK)(IKernel* pKernel, const PERSISTID& self,
                                               REQUESTTYPE type, const wchar_t* targetname, int result);

class RequestModule : public ILogicModule
{
public:
    RequestModule();

public:
    //初始化
    virtual bool Init(IKernel* pKernel);
    //释放
    virtual bool Shut(IKernel* pKernel);

private:
	// 进入场景就绪
	static int OnReady(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
public:
    //注册请求回调
    //type:请求类型
    //can_request_func:能否发出请求的回调
    //can_berequest_func:能否响应请求的回调
    //request_suc_func:请求成功后的回调
    //pPropList:属性列表，在canbe_request回调的参数中会收到请求角色的这些属性
    virtual bool RegisterRequestCallBack(REQUESTTYPE type,
                                         REQUEST_CAN_REQUEST_CALLBACK can_request_func,
                                         REQUEST_CAN_BEREQUEST_CALLBACK can_berequest_func,
                                         REQUEST_REQUEST_SUCCEED_CALLBACK request_suc_func,
                                         REQUEST_PARA_CALL_BACK req_para_func = NULL,
                                         REQUEST_PARA_CALL_BACK ans_para_func = NULL,
                                         REQUEST_RESULT_CALLBACK request_result_func = NULL);

    //向名称为target_name发送请求类型为type的请求
    virtual bool Request(IKernel* pKernel, const PERSISTID& self,
                         const wchar_t* target_name, REQUESTTYPE type);

    static int OnCustomRequest(IKernel* pKernel, const PERSISTID& self,
                               const PERSISTID& sender, const IVarList& args);

    static int OnCustomRequestAnswer(IKernel* pKernel, const PERSISTID& self,
                                     const PERSISTID& sender, const IVarList& args);

    //能够响应其他玩家发送的请求，命令格式：usgined cmdid, int request_type, widestr srcname
    static int OnCommandBeRequest(IKernel* pKernel, const PERSISTID& self,
                                  const PERSISTID& sender, const IVarList& args);

    //响应其他玩家发送的请求的回复(COMMAND_BEREQUST可能会被异步处理，通过此命令判断是否正确的向对方发送请求的逻辑)
    static int OnCommandBeRequestEcho(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args);

    //被请求玩家回复请求结果
    static int OnCommandRequestAnswer(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args);


    static int OnCommandGetAnswerPara(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args);

    static int OnCommandRetAnswerPara(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args);

    /*再次确认请求，当被请求者回复时，请求者的状态可能已经改变*/
    static int OnCommandGetRequestPara(IKernel* pKernel, const PERSISTID& self,
                                       const PERSISTID& sender, const IVarList& args);

    /*再次确认被请求*/
    static int OnCommandRetRequestPara(IKernel* pKernel, const PERSISTID& self,
                                       const PERSISTID& sender, const IVarList& args);

    // 检查请求是否过期
    static int HB_CheckRequest(IKernel* pKernel, const PERSISTID& self,
                               int time);
protected:
	//请求是否失效
	bool IsRequestInvalid(IKernel* pKernel, const PERSISTID& self, const wchar_t* srcname, int request_type, const IVarList& args);

    //能否发送请求(参数与OnCustomRequest相同)
    bool CanRequest(IKernel* pKernel, const PERSISTID& self,
                    REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);

    //能否接受请求(参数与OnCommandBeRequest相同)
    bool CanBeRequest(IKernel* pKernel, const PERSISTID& self,
                      REQUESTTYPE type, const wchar_t* srcname, const IVarList& paras);

    //请求成功
    void RequestSucceed(IKernel* pKernel, const PERSISTID& self,
                        REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras);

    //获得参数
    void GetRequestParas(IKernel* pKernel, const PERSISTID& self,
                         REQUESTTYPE type, IVarList& paras);
    //获得参数
    void GetAnswerParas(IKernel* pKernel, const PERSISTID& self,
                        REQUESTTYPE type, IVarList& paras);

    //请求的失败的结果返回 0失败 -1屏蔽请求 -2拒绝
    void RequestResultReturn(IKernel* pKernel, const PERSISTID& self,
                             REQUESTTYPE type, const wchar_t* targetname, int result);

    static CoolDownModule* m_pCoolDownModule;

    std::vector<std::list<REQUEST_CAN_REQUEST_CALLBACK> >   m_vecRequestCallBack;
    std::vector<std::list<REQUEST_CAN_BEREQUEST_CALLBACK> > m_vecBeRequestCallBack;
    std::vector<std::list<REQUEST_REQUEST_SUCCEED_CALLBACK> >   m_vecRequestSucCallBack;
    std::vector<std::list<REQUEST_PARA_CALL_BACK> > m_vecAnswerParaCallBack;
    std::vector<std::list<REQUEST_PARA_CALL_BACK> > m_vecRequestParaCallBack;
    std::vector<std::list<REQUEST_RESULT_CALLBACK> >    m_vecRequestRltCallBack;

public:
    static RequestModule* m_pRequestModule;
};
#endif