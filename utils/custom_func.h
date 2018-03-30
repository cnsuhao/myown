#ifndef _GAME_CUSTOM_FUNC_H
#define _GAME_CUSTOM_FUNC_H

#include "../public/IVarList.h"
#include "../server/IPubKernel.h"
#include "FsGame/Define/SystemInfo_Campaign.h"
#include "FsGame/Define/SystemInfo_Common.h"
#include "FsGame/Define/SystemInfo_Equipment.h"
#include "FsGame/Define/SystemInfo_Item.h"
#include "FsGame/Define/SystemInfo_Npc.h"
#include "FsGame/Define/SystemInfo_OPActivity.h"
#include "FsGame/Define/SystemInfo_Scene.h"
#include "FsGame/Define/SystemInfo_Skill.h"
#include "FsGame/Define/SystemInfo_Social.h"
#include "FsGame/Define/SystemInfo_SysFunction.h"
#include "FsGame/Define/SystemInfo_Task.h"

#ifdef FSROOMLOGIC_EXPORTS
#include "server/IRoomKernel.h"
#else
#include "server/IKernel.h"
#endif


// 发送给self玩家
bool CustomSysInfo(IKernel * pKernel, const PERSISTID & self, const int info_id, const IVarList & para_list);

// 发送给名字为name的玩家（场景服用）
bool CustomSysInfoByName(IKernel * pKernel, const wchar_t * name, const int info_id, const IVarList & para_list);

// 发送给名字为name的玩家（公共服用）
bool CustomSysInfoByName(IPubKernel * pKernel, const wchar_t * name, const int info_id, const IVarList & para_list);

// 发送给obj玩家视野范围内的玩家
bool CustomSysInfoByKen(IKernel * pKernel, const PERSISTID & obj, const int info_id, const IVarList & para_list);

// 发送给scene场景内的玩家（如果sceneid<=0 则发送到全世界）
bool CustomSysInfoByScene(IKernel * pKernel, int scene, const int info_id, const IVarList & para_list);

// 发送给group分组内的玩家
bool CustomSysInfoByGroup(IKernel * pKernel, int group, const int info_id, const IVarList & para_list);

// 发送给channel频道的玩家
bool CustomSysInfoByChannel(IKernel * pKernel, int channel, const int info_id, const IVarList & para_list);

// 发送给所有玩家（场景服用）
bool CustomSysInfoBroadcast(IKernel * pKernel, const int info_id, const IVarList & para_list);

// 发送给所有玩家（公共服用）
bool CustomSysInfoBroadcast(IPubKernel * pPubKernel, const int info_id, const IVarList & para_list);


//通知某对象周围玩家播放一次特效
bool CustomEffectByKen(IKernel* pKernel, 
	const char* effect, 
	const PERSISTID& self, 
	const PERSISTID& target, 
	float x = 0.0f, 
	float y = 0.0f, 
	float z = 0.0f);


// 聊天
bool CustomSpeech(IKernel * pKernel, const PERSISTID & self, const char * fast_string_id, const IVarList & para_list);



// 效果
bool CustomDisplayStage(IKernel* pKernel,
						const PERSISTID& self,
						const PERSISTID& target,
						const int nCustomType, // CustomDisplayType
						//const int eEffectType, // ESkillStageEffectType
						const IVarList& params,
						const bool IsSendAll); // 是否范围发送

bool CustomSysInfoGMBroadcast(IKernel* pKernel, const int info_id, const IVarList& para_list);

// 发送消息(不包括范围发送)
bool SafeCustom(IKernel* pKernel, const PERSISTID& self, const IVarList& msg);

// 范围发送消息
bool SafeCustomByKen(IKernel* pKernel, const PERSISTID& self, const IVarList& msg);
#endif // _GAME_CUSTOM_FUNC_H

