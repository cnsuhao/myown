#include "custom_func.h"
#include "../FsGame/Define/ServerCustomDefine.h"
//#include "../public/VarList.h"
#include "FsGame/Define/SnsDefine.h"
//#include "string_util.h"
//#include "../FsGame/SocialSystemModule/GuildModule.h"
//#include "../FsGame/Define/CommandDefine.h"


// 发送给self玩家
bool CustomSysInfo(IKernel * pKernel, const PERSISTID & self, 
	const int info_id, const IVarList & para_list)
{
	if (!pKernel->Exists(self) 
		|| pKernel->Type(self) != TYPE_PLAYER)
	{
		return false;
	}

	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->Custom(self, system_info);
}

// 发送给名字为name的玩家（场景服用）
bool CustomSysInfoByName(IKernel * pKernel, const wchar_t * name, 
	const int info_id, const IVarList & para_list)
{
	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByName(name, system_info);
}

// 发送给名字为name的玩家（公共服用）
bool CustomSysInfoByName(IPubKernel * pKernel, const wchar_t * name, 
	const int info_id, const IVarList & para_list)
{
	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByName(name, system_info);
}

// 发送给obj玩家视野范围内的玩家
bool CustomSysInfoByKen(IKernel * pKernel, const PERSISTID & obj, 
	const int info_id, const IVarList & para_list)
{
	if (!pKernel->Exists(obj) 
		|| pKernel->Type(obj) != TYPE_PLAYER)
	{
		return false;
	}

	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByKen(obj, system_info);
}

// 发送给scene场景内的玩家（如果sceneid<=0 则发送到全世界）
bool CustomSysInfoByScene(IKernel * pKernel, int scene, 
	const int info_id, const IVarList & para_list)
{
#ifdef FSROOMLOGIC_EXPORTS

	return true;

#else

	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;

	if (scene <= 0)
	{
		return pKernel->CustomByWorld(system_info);
	}
	else
	{
		return pKernel->CustomByScene(scene, system_info);
	}

#endif
}

// 发送给group分组内的玩家
bool CustomSysInfoByGroup(IKernel * pKernel, int group, const int info_id, const IVarList & para_list)
{
	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByGroup(group, system_info);
}

// 发送给channel频道的玩家
bool CustomSysInfoByChannel(IKernel * pKernel, int channel, 
	const int info_id, const IVarList & para_list)
{
	if (channel < 0)
	{
		return false;
	}

#ifdef FSROOMLOGIC_EXPORTS

	return true;

#else

	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByChannel(channel, system_info);

#endif
}

// 发送给所有玩家（场景服用）
bool CustomSysInfoBroadcast(IKernel * pKernel, const int info_id, const IVarList & para_list)
{
#ifdef FSROOMLOGIC_EXPORTS

	return true;

#else

	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pKernel->CustomByWorld(system_info);

#endif

}

// 发送给所有玩家（公共服用）
bool CustomSysInfoBroadcast(IPubKernel * pPubKernel, const int info_id, const IVarList & para_list)
{
	CVarList system_info;
	system_info << SERVER_CUSTOMMSG_SYSINFO
				<< info_id
				<< para_list;
	return pPubKernel->CustomByWorld(system_info);
}

bool CustomSysInfoGMBroadcast(IKernel* pKernel, int tips_type, const wchar_t* info, const IVarList& para_list)
{
#ifdef FSROOMLOGIC_EXPORTS
	return true;
#else
	return pKernel->CustomByWorld(CVarList() << SERVER_CUSTOMMSG_SYSINFO << tips_type << info << para_list);
#endif
}

bool CustomSpeech(IKernel * pKernel, const PERSISTID & self, const char * fast_string_id, const IVarList & para_list)
{
	if ( !pKernel->Exists(self) || pKernel->Type(self) != TYPE_PLAYER)
	{
		return false;
	}
	return pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_SPEECH << fast_string_id << para_list);
}

//通知某对象周围玩家播放一次特效
bool CustomEffectByKen(IKernel* pKernel, const char* effect, const PERSISTID& self, const PERSISTID& target, float x, float y, float z)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}
	if (!effect || !*effect)
	{
		return false;
	}
	return pKernel->CustomByKen(self, CVarList() << SERVER_CUSTOMMSG_EFFECT << self /*<< ONCEEFFECT*/ << effect << target << x << y << z);
}

// 效果
bool CustomDisplayStage(IKernel* pKernel,
						const PERSISTID& self,
						const PERSISTID& target,
						const int nCustomType, // CustomDisplayType
//						const int eEffectType, // ESkillStageEffectType
						const IVarList& params,
						const bool IsSendAll) // 是否范围发送	
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	CVarList msg;
	msg << SERVER_CUSTOMMSG_DISPLAY << nCustomType<< target;

	msg << params;

	if (IsSendAll)
	{
		pKernel->CustomByKen(self, msg);
	}
	else
	{
		if (pKernel->Type(self) == TYPE_PLAYER)
		{
			pKernel->Custom(self, msg);
		}
	}
	return true;
}

// 发送消息(不包括范围发送)
bool SafeCustom(IKernel* pKernel, const PERSISTID& self, const IVarList& msg)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	if (0 == msg.GetCount())
	{
		return false;
	}

	if (pSelfObj->GetClassType() == TYPE_PLAYER && pSelfObj->QueryInt("Online") == ONLINE)
	{
		pKernel->Custom(self, msg);
	}

	return true;
}

// 范围发送消息
bool SafeCustomByKen(IKernel* pKernel, const PERSISTID& self, const IVarList& msg)
{
	if (!pKernel->Exists(self) || 0 == msg.GetCount())
	{
		return false;
	}
	pKernel->CustomByKen(self, msg);

	return true;
}