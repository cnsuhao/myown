//--------------------------------------------------------------------
// 文件名:		FsBindLogic.cpp
// 内  容:		处理逻辑，非业务。
// 说  明:		
// 创建日期:	2017年08月02日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#include "FsBindLogic.h"
#include "FsPubLogic.h"
#include "../../FsGame/Define/PubCmdDefine.h"
#include "../../FsGame/Define/PubDefine.h"
//#include "../../FsGame/Define//GuildDefine.h"
#include "../utils/util_func.h"
#include "../utils/string_util.h"
//#include "Domain/DomainActivity.h"
//#include "FsGame/SystemFunctionModule/ContainerHelper.hpp"
#include "utils/IniFile.h"
FsBindLogic::FsBindLogic()
{
	bind_pub_data_name_ = L"";
}
FsBindLogic::~FsBindLogic()
{
	//
}
bool FsBindLogic::Init(IPubKernel* pPubKernel)
{
	for (int index = 0; index < MAX_MEMBER_NUM; index++)
	{
		member_list_[index].id_ = index;
		member_list_[index].scene_ = 0;
		member_list_[index].is_open_ = false;
	}

	std::string strGameIni(pPubKernel->GetResourcePath());
	strGameIni.append("game.ini");
	CIniFile iniFile(strGameIni.c_str());
	if (!iniFile.LoadFromFile())
	{
		Assert(false);
		::extend_warning(LOG_ERROR, "open game.ini(path:%s) failed", strGameIni.c_str());
		return false;
	} 
	int nLogLevel = iniFile.ReadInteger("game_info", "log_level", 0);
	if (nLogLevel > 0)
	{
		// 设置日志等级
		extend_log_level(NULL, (LogLevelEnum)nLogLevel, "");
	}
	return true;
}
bool FsBindLogic::Start(IPubKernel* pPubKernel)
{
	//通知服务准备就绪
	SendToMemberAll(pPubKernel, CVarList() << PS_SERVICE_READY_PUBLIC);
	//发送 准备就绪。
	SendPublicCommand(pPubKernel, E_PUB_CMD_READY, CVarList());
	return true;
}
bool FsBindLogic::Close(IPubKernel* pPubKernel)
{
	return true;
}
bool FsBindLogic::OnPublicRegist(IPubKernel* pPubKernel, int source_id, int scene_id, const IVarList& args)
{
	if (source_id<0 || source_id >= MAX_MEMBER_NUM)
	{
		Assert(0);
		return false;
	}
	member_list_[source_id].is_open_ = true;
	member_list_[source_id].scene_ = scene_id;
	return true;
}
bool FsBindLogic::SendToMemberMaster(IPubKernel* pPubKernel, const IVarList& args)
{
	if (!member_list_[0].is_open_)
	{
		Assert(0);
		return false;
	}
	return pPubKernel->SendPublicMessage(0, member_list_[0].scene_, args);
}
bool FsBindLogic::SendToMemberAll(IPubKernel* pPubKernel, const IVarList& args)
{
	for (int index = 0; index < MAX_MEMBER_NUM;index++)
	{
		if (member_list_[index].is_open_)
		{
			pPubKernel->SendPublicMessage(index, member_list_[index].scene_, args);
		}
	}
	return true;
}
bool FsBindLogic::SendPublicCommand(IPubKernel* pPubKernel, int cmd_id, const IVarList& args)
{
	return pub_callee::pInstance->SendPublicCommand(pPubKernel, cmd_id, args);
}
// bool FsBindLogic::GetActivityNode(int id, IActivityNode*& p_dc)
// {
// 	return DomainActivity::Instance()->GetActivityNode(id, p_dc);
// }