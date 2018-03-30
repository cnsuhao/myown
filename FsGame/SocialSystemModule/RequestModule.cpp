#include "FsGame/SocialSystemModule/RequestModule.h"
#include "utils/custom_func.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "FsGame/Define/RequestDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/TeamDefine.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "FsGame/SocialSystemModule/TeamModule.h"
#include "public/VarList.h"

#include <ctime>
#include "Define/Fields.h"

//请求持续时间:秒
#define MAX_REQUEST_LAST_TIME 180

RequestModule* RequestModule::m_pRequestModule = NULL;
CoolDownModule* RequestModule::m_pCoolDownModule = NULL;

RequestModule::RequestModule()
{
    m_vecRequestCallBack.resize(REQUESTTYPE_COUNT);
    m_vecBeRequestCallBack.resize(REQUESTTYPE_COUNT);
    m_vecRequestSucCallBack.resize(REQUESTTYPE_COUNT);
    m_vecRequestParaCallBack.resize(REQUESTTYPE_COUNT);
    m_vecAnswerParaCallBack.resize(REQUESTTYPE_COUNT);
    m_vecRequestRltCallBack.resize(REQUESTTYPE_COUNT);
}

//初始化
bool RequestModule::Init(IKernel* pKernel)
{
    m_pRequestModule = this;

    m_pCoolDownModule = (CoolDownModule*)pKernel->GetLogicModule("CoolDownModule");
	Assert(m_pCoolDownModule != NULL && m_pRequestModule != NULL);
	pKernel->AddEventCallback("player", "OnReady", OnReady);

    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_REQUEST, RequestModule::OnCustomRequest);
    pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_REQUEST_ANSWER, RequestModule::OnCustomRequestAnswer);

    pKernel->AddIntCommandHook("player", COMMAND_BEREQUEST, RequestModule::OnCommandBeRequest);
    pKernel->AddIntCommandHook("player", COMMAND_BEREQUEST_ECHO, RequestModule::OnCommandBeRequestEcho);
    pKernel->AddIntCommandHook("player", COMMAND_REQUEST_ANSWER, RequestModule::OnCommandRequestAnswer);
    pKernel->AddIntCommandHook("player", COMMAND_GET_ANSWER_PARA, RequestModule::OnCommandGetAnswerPara);
    pKernel->AddIntCommandHook("player", COMMAND_RET_ANSWER_PARA, RequestModule::OnCommandRetAnswerPara);
    pKernel->AddIntCommandHook("player", COMMAND_GET_REQUEST_PARA, RequestModule::OnCommandGetRequestPara);
    pKernel->AddIntCommandHook("player", COMMAND_RET_REQUEST_PARA, RequestModule::OnCommandRetRequestPara);

    //添加心跳
    DECL_HEARTBEAT(RequestModule::HB_CheckRequest);
    return true;
}

//释放
bool RequestModule::Shut(IKernel* pKernel)
{
    return true;
}

//注册请求回调
bool RequestModule::RegisterRequestCallBack(REQUESTTYPE type,
                                            REQUEST_CAN_REQUEST_CALLBACK can_request_func,
                                            REQUEST_CAN_BEREQUEST_CALLBACK can_berequest_func,
                                            REQUEST_REQUEST_SUCCEED_CALLBACK request_suc_func,
                                            REQUEST_PARA_CALL_BACK req_para_func,
                                            REQUEST_PARA_CALL_BACK ans_para_func,
                                            REQUEST_RESULT_CALLBACK request_result_func)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return false;
    }

    //能否发送请求的回调函数
    bool bexists = false;
    std::list<REQUEST_CAN_REQUEST_CALLBACK>::iterator it = m_vecRequestCallBack[type].begin();
	LoopBeginCheck(a);
    for (; it != m_vecRequestCallBack[type].end(); ++it)
    {
		LoopDoCheck(a);
        if (*it == can_request_func)
        {
            bexists = true;
            break;
        }
    }

    if (!bexists)
    {
        m_vecRequestCallBack[type].push_back(can_request_func);
    }

    //能否接受请求的回调函数
    bexists = false;
    std::list<REQUEST_CAN_BEREQUEST_CALLBACK>::iterator it1 = m_vecBeRequestCallBack[type].begin();
	LoopBeginCheck(b);
    for (; it1 != m_vecBeRequestCallBack[type].end(); ++it1)
    {
		LoopDoCheck(b);
        if (*it1 == can_berequest_func)
        {
            bexists = true;
            break;
        }
    }

    if (!bexists)
    {
        m_vecBeRequestCallBack[type].push_back(can_berequest_func);
    }


    //请求成功后的回调函数
    bexists = false;
    std::list<REQUEST_REQUEST_SUCCEED_CALLBACK>::iterator it2 = m_vecRequestSucCallBack[type].begin();
	LoopBeginCheck(c);
    for (; it2 != m_vecRequestSucCallBack[type].end(); ++it2)
    {
		LoopDoCheck(c);
        if (*it2 == request_suc_func)
        {
            bexists = true;
            break;
        }
    }

    if (!bexists)
    {
        m_vecRequestSucCallBack[type].push_back(request_suc_func);
    }

    if (req_para_func != NULL)
    {
        //获取参数的回调
        bexists = false;
        std::list<REQUEST_PARA_CALL_BACK>::iterator it4 = m_vecRequestParaCallBack[type].begin();
		LoopBeginCheck(d);
        for (; it4 != m_vecRequestParaCallBack[type].end(); ++it4)
        {
			LoopDoCheck(d);
            if (*it4 == req_para_func)
            {
                bexists = true;
                break;
            }
        }

        if (!bexists)
        {
            m_vecRequestParaCallBack[type].push_back(req_para_func);
        }
    }

    if (ans_para_func != NULL)
    {
        //获取参数的回调
        bexists = false;
        std::list<REQUEST_PARA_CALL_BACK>::iterator it3 = m_vecAnswerParaCallBack[type].begin();
		LoopBeginCheck(e);
        for (; it3 != m_vecAnswerParaCallBack[type].end(); ++it3)
        {
			LoopDoCheck(e);
            if (*it3 == ans_para_func)
            {
                bexists = true;
                break;
            }
        }

        if (!bexists)
        {
            m_vecAnswerParaCallBack[type].push_back(ans_para_func);
        }
    }
    if (request_result_func != NULL)
    {
        //获取参数的回调
        bexists = false;
        std::list<REQUEST_RESULT_CALLBACK>::iterator it5 = m_vecRequestRltCallBack[type].begin();
		LoopBeginCheck(f);
        for (; it5 != m_vecRequestRltCallBack[type].end(); ++it5)
        {
			LoopDoCheck(f);
            if (*it5 == request_result_func)
            {
                bexists = true;
                break;
            }
        }																									    

        if (!bexists)
        {
            m_vecRequestRltCallBack[type].push_back(request_result_func);
        }
    }

    return true;
}

// 进入场景就绪
int RequestModule::OnReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord *pReqRec = pSelfObj->GetRecord(REQUEST_REC);
	if (NULL == pReqRec)
	{
		return 0;
	}
	int rows = pReqRec->GetRows();
	// 存在请求信息，则添加请求倒计时心跳
	if (rows > 0 && !pKernel->FindHeartBeat(self, "RequestModule::HB_CheckRequest"))
	{
		pKernel->AddHeartBeat(self, "RequestModule::HB_CheckRequest", 1000);
		//重置请求倒计时
		LoopBeginCheck(g);
		for (int i = 0; i < rows; ++i)
		{
			LoopDoCheck(g);
			pReqRec->SetInt(i, REQUEST_REC_TIME, MAX_REQUEST_LAST_TIME);
		}
	}
	return 0;
}

//向名称为target_name发送请求类型为type的请求
bool RequestModule::Request(IKernel* pKernel, const PERSISTID& self,
                            const wchar_t* target_name, REQUESTTYPE type)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
    const wchar_t* source_name = pSelfObj->QueryWideStr("Name");
    if (wcscmp(source_name, target_name) == 0)
    {
        // 不能对自已请求
		return false;
    }

    //职业
    int profession = pSelfObj->QueryInt("Job");
    /*int sex = pSelfObj->QueryInt("Sex");
    if (profession > 3 || profession < 1 
		|| sex > 2 || sex < 1)
    {
        return false;
    }
    int pics[3][2] = {{1, 2}, {3, 4}, {5, 6}};

    int pic = pics[profession - 1][sex - 1];*/
    //战斗力
    int battle = pSelfObj->QueryInt("BattleAbility");
	
	int param = 0;
	//添加邀请参数，用于响应后判断是否同一请求
	//if (REQUESTTYPE_INVITE_SINGLEPLAYER == type)
	//{
		param = pSelfObj->QueryInt("TeamID");
	//}
    CVarList var_list;
    if (pKernel->GetPlayerScene(target_name) > 0)
    {
        var_list << COMMAND_GET_ANSWER_PARA << (int)type << source_name << param 
			<< profession << battle << pSelfObj->QueryInt("Nation") << pSelfObj->QueryInt("Level") << pSelfObj->QueryInt("Sex");
        pKernel->CommandByName(target_name, var_list);
    }
    else
    {
        //[{@0:名称}]处于离线状态, 请求失败
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17305, CVarList() << target_name);
    }

    return true;
}

int RequestModule::OnCustomRequest(IKernel* pKernel, const PERSISTID& self,
                                   const PERSISTID& sender, const IVarList& args)
{
    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
	if (requesttype <= REQUESTTYPE_NONE
		|| requesttype >= REQUESTTYPE_COUNT)
	{
		return 0;
	}
    const wchar_t* targetname = args.WideStrVal(2);

    m_pRequestModule->Request(pKernel, self, targetname, requesttype);

    return 0;
}

int RequestModule::OnCustomRequestAnswer(IKernel* pKernel, const PERSISTID& self,
                                         const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

    //被请求者客户端发送过来的对请求的回答信息
    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
	if (requesttype <= REQUESTTYPE_NONE
		|| requesttype >= REQUESTTYPE_COUNT)
	{
		return 0;
	}

    //被请求者
    const wchar_t* srcname = args.WideStrVal(2);
    int result = args.IntVal(3);

    //再次请求得到srcname的数据(暂时只为组队请求修改此流程)
    CVarList msg;
    msg << COMMAND_GET_REQUEST_PARA 
		   << requesttype
           << pSelfObj->QueryWideStr("Name") 
		   << result;

    if (pKernel->GetPlayerScene(srcname) > 0)
    {
        pKernel->CommandByName(srcname, msg);
    }
    else
    {
        //对方处于离线状态, {@1:请求}请求失败
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17305, CVarList() << srcname);
    }

    return 0;
}

//能够响应其他玩家发送的请求，命令格式：usgined cmdid, int requesttype, widestr srcname
int RequestModule::OnCommandBeRequest(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
	const wchar_t* srcname = args.WideStrVal(2);
	int nSrcTeamId = args.IntVal(3);
	int nSrcLv = args.IntVal(4);
	int nSrcBA = args.IntVal(5);
	int nSrcSex = args.IntVal(6);
	int nSrcJob = args.IntVal(7);

	//可否被请求，由各模块处理逻辑
	CVarList src_paras;
	src_paras << REQUEST_STATE_REQ;
	src_paras.Append(args, 3, args.GetCount() - 3);
	if (!m_pRequestModule->CanBeRequest(pKernel, self, requesttype, srcname, src_paras))
	{
		IRecord *pReqRec = pSelfObj->GetRecord(REQUEST_REC);
		if (pReqRec != NULL)
		{
			int rows = pReqRec->GetRows();
			LoopBeginCheck(h);
			for (int i = rows - 1; i >= 0; --i)
			{
				LoopDoCheck(h);
				const wchar_t* name = pReqRec->QueryWideStr(i, REQUEST_REC_NAME);
				int reqtype = pReqRec->QueryInt(i, REQUEST_REC_TYPE);
				if (wcscmp(srcname, name) == 0 && reqtype == requesttype)
				{
					//删除此玩家此种请求
					pReqRec->RemoveRow(i);
				}
			}
		}
		//失败，发送回执给请求者
		CVarList msg;
		msg << COMMAND_BEREQUEST_ECHO
			<< requesttype
			<< pSelfObj->QueryWideStr("Name")
			<< 0;
		pKernel->CommandByName(srcname, msg);
		return 0;
	}
	
    
    //成功，向客户端发送请求
    if (requesttype == REQUESTTYPE_JOIN_SINGLEPLAYER || requesttype == REQUESTTYPE_INVITE_SINGLEPLAYER)
    {
        pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_TEAM << CLIENT_CUSTOMMSG_TEAM_REFRESH);
		pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_TEAM << CLIENT_CUSTOMMSG_TEAM_REQ_ADD_TEAM 
			<< requesttype << srcname << nSrcLv << nSrcBA << nSrcSex << nSrcJob);
    }

    //发送回执给请求者
    CVarList msg;
    msg << COMMAND_BEREQUEST_ECHO 
		   << requesttype 
		   << pSelfObj->QueryWideStr("Name") 
		   << 1;
    pKernel->CommandByName(srcname, msg);
    return 0;
}

//响应其他玩家发送的请求的回复(COMMAND_BEREQUEST可能会被异步处理，通过此命令判断是否正确的向对方发送请求的逻辑)
int RequestModule::OnCommandBeRequestEcho(IKernel* pKernel, const PERSISTID& self,
                                          const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* targetname = args.WideStrVal(2);
    int result = args.IntVal(3);

    if(result == -1)
    {
        //[{@0:名称}]拒绝接受此请求，请求信息发送失败
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17303, CVarList() << targetname);
    }

    //结果返回
    m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, targetname, result);

    return 0;
}

//请求的结果返回result：3发送请求 2同意 1等待 0失败 -1屏蔽请求 -2拒绝
void RequestModule::RequestResultReturn(IKernel* pKernel, const PERSISTID& self,
                                        REQUESTTYPE type, const wchar_t* targetname, int result)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return ;
    }


    std::list <REQUEST_RESULT_CALLBACK>::const_iterator it = m_vecRequestRltCallBack[type].begin();
	LoopBeginCheck(i);
    for (; it != m_vecRequestRltCallBack[type].end(); ++it)
    {
		LoopDoCheck(i);
        if (!((*it)(pKernel, self, type, targetname, result)))
        {
            return ;
        }
    }

    return ;
}


//被请求玩家回复请求结果
int RequestModule::OnCommandRequestAnswer(IKernel* pKernel, const PERSISTID& self,
                                          const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* targetname = args.WideStrVal(2);
    int result = args.IntVal(3);
    CVarList paras;
    paras << REQUEST_STATE_ANS;
    paras.Append(args, 4, args.GetCount() - 4);

    if (requesttype > REQUESTTYPE_NONE
		&& requesttype < REQUESTTYPE_COUNT)
    {
        if (result == 0)
        {
            //[{@0:名称}]拒绝了你的{@1:请求}请求
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17302, CVarList() << targetname << "");

            //结果返回
            m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, targetname, -2);

        }
        else if (result == 2)
        {
            //你对[{@0:名称}]的{@1:请求}请求已超时...
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17304, CVarList() << targetname << "");

            //结果返回
            m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, targetname, -2);

        }
        else
        {
            if (!m_pRequestModule->CanRequest(pKernel, self, requesttype, targetname, paras))
            {
                //结果返回
                m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, targetname, 0);
                return 0;
            }

            //结果返回
            m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, targetname, 2);

            m_pRequestModule->RequestSucceed(pKernel, self, requesttype, targetname, paras);
        }
    }

    return 0;
}
int RequestModule::OnCommandGetAnswerPara(IKernel* pKernel, const PERSISTID& self,
                                          const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* srcname = args.WideStrVal(2);
    int teamid = args.IntVal(3);
    int pic = args.IntVal(4);//头像
    int battle = args.IntVal(5);//战斗力
	int nation = args.IntVal(6);// 国籍
	int level = args.IntVal(7); // 等级
	int sex = args.IntVal(8);	//性别
	if (!m_pRequestModule->CanBeRequest(pKernel, self, requesttype, srcname, CVarList() << 0 << teamid << nation))
	{
		return 0;
	}

	if (pKernel->GetPlayerScene(pSelfObj->QueryWideStr("Name")) <= 0)
	{
		//[{@0:名称}]处于离线状态, 请求失败
		CustomSysInfoByName(pKernel, srcname, SYSTEM_INFO_ID_17301, CVarList() << pSelfObj->QueryWideStr("Name"));
		 
		 return 0;
	}

    //获得参数
    CVarList paras;
    m_pRequestModule->GetAnswerParas(pKernel, self, requesttype, paras);
	
	IRecord *pReqRec = pSelfObj->GetRecord(REQUEST_REC);
	//判断是否收到该玩家发送的相同请求
	if (pReqRec != NULL)
	{
		int rows = pReqRec->GetRows();
		LoopBeginCheck(j);
		for (int i = 0; i < rows; ++i)
		{
			LoopDoCheck(j);
			const wchar_t* name = pReqRec->QueryWideStr(i, REQUEST_REC_NAME);
			int reqType = pReqRec->QueryInt(i, REQUEST_REC_TYPE);
			if (wcscmp(srcname, name) == 0 && reqType == requesttype)
			{
				// 已向[{@0:name}]发出了请求信息，等待回应。。。。
				CustomSysInfoByName(pKernel, srcname, SYSTEM_INFO_ID_17306, CVarList() << pSelfObj->QueryWideStr("Name"));
				return 0;
			}
		}
		
		CVarList row_value;
		row_value << rows << srcname << requesttype << MAX_REQUEST_LAST_TIME << teamid << pic << battle << level << sex << nation;
		pReqRec->AddRowValue(-1, row_value);

		// 添加请求信息持续心跳
		if (!pKernel->FindHeartBeat(self, "RequestModule::HB_CheckRequest") )
		{
			pKernel->AddHeartBeat(self, "RequestModule::HB_CheckRequest", 1000);
		}
	
	}

    CVarList msg;
    msg << COMMAND_RET_ANSWER_PARA 
		   << requesttype 
		   << pSelfObj->QueryWideStr("Name") 
		   << paras;
    if (!pKernel->CommandByName(srcname, msg))
    {
        return 0;
    }
    return 0;
}

int RequestModule::OnCommandRetAnswerPara(IKernel* pKernel, const PERSISTID& self,
                                          const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

    REQUESTTYPE type = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* target_name = args.WideStrVal(2);

    CVarList target_paras;
    target_paras << REQUEST_STATE_REQ;
    target_paras.Append(args, 3, args.GetCount() - 3);

    //能否向对象发送请求
    if (!m_pRequestModule->CanRequest(pKernel, self, type, target_name, target_paras))
    {

		CVarList msg;
		msg << COMMAND_TEAM_MSG << COMMAND_DEL_PLAYER_APPLAY<<type<<pSelfObj->QueryWideStr("Name");
		pKernel->CommandByName(target_name, msg);

        return 0;
    }

    //获得参数
    CVarList paras;
    m_pRequestModule->GetRequestParas(pKernel, self, type, paras);

    //用命令向目标对象发送请求
    CVarList msg;
    msg << COMMAND_BEREQUEST << type << pSelfObj->QueryWideStr(FIELD_PROP_NAME) 
		<< pSelfObj->QueryInt(FIELD_PROP_TEAM_ID)
		<< pSelfObj->QueryInt(FIELD_PROP_LEVEL) 
		<< pSelfObj->QueryInt(FIELD_PROP_BATTLE_ABILITY)
		<< pSelfObj->QueryInt(FIELD_PROP_SEX)
		<< pSelfObj->QueryInt(FIELD_PROP_JOB);
    if (paras.GetCount() > 0)
    {
        msg << paras;
    }
    if (!pKernel->CommandByName(target_name, msg))
    {
        //发送命令失败, 玩家[{@0:名称}]不存在或对方已下线
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17301, CVarList() << target_name);
        return 0;
    }
    //结果返回
    m_pRequestModule->RequestResultReturn(pKernel, self, type, target_name, 3);
    //成功发送请求
	::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17306, CVarList() << target_name);

    return 0;
}
/*再次确认请求，当被请求者回复时，请求者的状态可能已经改变,需重新获取para*/
int RequestModule::OnCommandGetRequestPara(IKernel* pKernel, const PERSISTID& self,
                                           const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* targetname = args.WideStrVal(2);
    int result = args.IntVal(3);

	if (TeamModule::IsFull(pKernel,self))
	{
		//[{@0:name}]队伍已满，加入队伍失败
		CustomSysInfoByName(pKernel, targetname, SYSTEM_INFO_ID_17401, CVarList() << pSelfObj->QueryWideStr("Name"));
		return 0;

	}

    //获得参数
    CVarList paras;
    m_pRequestModule->GetRequestParas(pKernel, self, requesttype, paras);	

    CVarList msg;
    msg << COMMAND_RET_REQUEST_PARA 
		    << requesttype 
		    << pSelfObj->QueryWideStr("Name") 
		    << result 
		    << paras;
    if (!pKernel->CommandByName(targetname, msg))
    {
        return 0;
    }

    return 0;
}

/*再次确认被请求*/
int RequestModule::OnCommandRetRequestPara(IKernel* pKernel, const PERSISTID& self,
                                           const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

    REQUESTTYPE requesttype = (REQUESTTYPE)args.IntVal(1);
    const wchar_t* srcname = args.WideStrVal(2);
    int result = args.IntVal(3);

    CVarList src_paras;
    src_paras << REQUEST_STATE_ANS;
    src_paras.Append(args, 4, args.GetCount() - 4);
    if (result == 1)
    {
        if (!m_pRequestModule->CanBeRequest(pKernel, self, requesttype, srcname, src_paras))
        {
            //失败
            m_pRequestModule->RequestResultReturn(pKernel, self, requesttype, srcname, 0);
            return 0;
        }
    }
	
	//判断消息是否已经失效
	if (m_pRequestModule->IsRequestInvalid(pKernel, self, srcname, requesttype, src_paras))
	{
		return 0;
	}

    //获得参数
    CVarList paras;
    m_pRequestModule->GetAnswerParas(pKernel, self, requesttype, paras);
	
    CVarList msg;
    msg << COMMAND_REQUEST_ANSWER 
		   << requesttype 
		   << pSelfObj->QueryWideStr("Name") 
		   << result 
		   << paras;
    if (!pKernel->CommandByName(srcname, msg))
    {
        return 0;
    }

    return 0;
}

//能否发送请求(参数与OnCustomRequest相同)
bool RequestModule::CanRequest(IKernel* pKernel, const PERSISTID& self,
                               REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return false;
    }

    std::list <REQUEST_CAN_REQUEST_CALLBACK>::const_iterator it = m_vecRequestCallBack[type].begin();
	LoopBeginCheck(k);
    for (; it != m_vecRequestCallBack[type].end(); ++it)
    {
		LoopDoCheck(k);
        if (!((*it)(pKernel, self, type, targetname, paras)))
        {
            return false;
        }
    }

    return true;
}

//能否接受请求(参数与OnCommandBeRequest相同)
bool RequestModule::CanBeRequest(IKernel* pKernel, const PERSISTID& self,
                                 REQUESTTYPE type, const wchar_t* srcname, const IVarList& paras)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return false;
    }

    std::list <REQUEST_CAN_BEREQUEST_CALLBACK>::const_iterator it = m_vecBeRequestCallBack[type].begin();
	LoopBeginCheck(l);
    for (; it != m_vecBeRequestCallBack[type].end(); ++it)
    {
		LoopDoCheck(l);
        if (!((*it)(pKernel, self, type, srcname, paras)))
        {
            return false;
        }
    }

    return true;
}

//请求成功
void RequestModule::RequestSucceed(IKernel* pKernel, const PERSISTID& self,
                                   REQUESTTYPE type, const wchar_t* targetname, const IVarList& paras)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return;
    }

    std::list <REQUEST_REQUEST_SUCCEED_CALLBACK>::const_iterator it = m_vecRequestSucCallBack[type].begin();
	LoopBeginCheck(m);
    for (; it != m_vecRequestSucCallBack[type].end(); ++it)
    {
		LoopDoCheck(m);
        if (!((*it)(pKernel, self, type, targetname, paras)))
        {
            return;
        }
    }

    return;
}

//获得参数
void RequestModule::GetRequestParas(IKernel* pKernel, const PERSISTID& self,
                                    REQUESTTYPE type, IVarList& paras)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return;
    }
    std::list <REQUEST_PARA_CALL_BACK>::const_iterator it = m_pRequestModule->m_vecRequestParaCallBack[type].begin();
	LoopBeginCheck(n);
    for (; it != m_pRequestModule->m_vecRequestParaCallBack[type].end(); ++it)
    {
		LoopDoCheck(n);
        if (!((*it)(pKernel, self, type, paras)))
        {
            return;
        }
    }
    return;
}

//获得参数
void RequestModule::GetAnswerParas(IKernel* pKernel, const PERSISTID& self,
                                   REQUESTTYPE type, IVarList& paras)
{
    if (type <= REQUESTTYPE_NONE
		|| type >= REQUESTTYPE_COUNT)
    {
        return;
    }

    std::list <REQUEST_PARA_CALL_BACK>::const_iterator it = m_pRequestModule->m_vecAnswerParaCallBack[type].begin();
	LoopBeginCheck(o);
    for (; it != m_pRequestModule->m_vecAnswerParaCallBack[type].end(); ++it)
    {
		LoopDoCheck(o);
        if (!((*it)(pKernel, self, type, paras)))
        {
            return;
        }
    }

    return;
}

int RequestModule::HB_CheckRequest(IKernel* pKernel, const PERSISTID& self, int time)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord *pReqRec = pSelfObj->GetRecord(REQUEST_REC);
	if (NULL == pReqRec)
	{
		return 0;
	}
    int count = pReqRec->GetRows();
	LoopBeginCheck(p);
    for (int i = count - 1; i >= 0; --i)
    {
		LoopDoCheck(p);
        int lastTime = pReqRec->QueryInt(i, REQUEST_REC_TIME);
        if (lastTime >= 1)
        {
            //请求持续时间减1秒
            pReqRec->SetInt(i, REQUEST_REC_TIME, --lastTime);
        }
        else if (lastTime < 1)
        {
           const wchar_t* srcname = pReqRec->QueryWideStr(i, REQUEST_REC_NAME);
            int requesttype = pReqRec->QueryInt(i, REQUEST_REC_TYPE);
            const wchar_t* self_name = pSelfObj->QueryWideStr("Name");

			//删除此条请求
			pReqRec->RemoveRow(i);
			
			//向玩家发送请求过时
			CVarList msg;
			msg << COMMAND_GET_REQUEST_PARA 
				   << requesttype
				   << self_name 
				   << 2;
            pKernel->CommandByName(srcname, msg);
        }
    }

    //处理完所有的请求信息，删除心跳
    if (count == 0 && pKernel->FindHeartBeat(self, "RequestModule::HB_CheckRequest"))
    {
        pKernel->RemoveHeartBeat(self, "RequestModule::HB_CheckRequest");
    }
	 
    return 0;
}

//请求状态是否相同
bool RequestModule::IsRequestInvalid(IKernel* pKernel, const PERSISTID& self, const wchar_t* srcname, int request_type, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	bool bInValid = true;

	IRecord *pReqRec = pSelfObj->GetRecord(REQUEST_REC);
	//删除请求表中此玩家的请求
	if (pReqRec != NULL)
	{
		int param = 0;
		int type = -1;
		int rows = pReqRec->GetRows();
		LoopBeginCheck(q);
		for (int i = rows - 1; i >= 0; --i)
		{
			LoopDoCheck(q);
			const wchar_t* name = pReqRec->QueryWideStr(i, REQUEST_REC_NAME);
			type = pReqRec->QueryInt(i, REQUEST_REC_TYPE);
			param = pReqRec->QueryInt(i, REQUEST_REC_STATE);
			if (wcscmp(srcname, name) == 0 && type == request_type)
			{
				//删除此玩家此种请求
				pReqRec->RemoveRow(i);
				//消息未失效
				bInValid = false;
				break;
			}
		}

		//判断消息是否已经失效
		switch (type)
		{
			//邀请组队
			case REQUESTTYPE_INVITE_SINGLEPLAYER:
			{
				int teamID = args.IntVal(1);
				int request = args.IntVal(6);
				int is_captain = args.IntVal(7);
				//队伍ID不等，当前准备状态不可邀请，无邀请权限
				if (teamID != param|| 
					(teamID > 0 && (request != TEAM_REQUEST_YES && is_captain == 0)))
				{
					bInValid = true;
				}
			}
			break;
		}
		//未找到当前消息或者请求者发送请求时的状态与接收时的状态不同
		if (bInValid)
		{
			//通知目标玩家请求已失效
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_17305, CVarList());
			return true;
		}
	}
	return bInValid;
}