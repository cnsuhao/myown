//--------------------------------------------------------------------
// 文件名:      SkillModule.h
// 内  容:      技能系统主逻辑
// 说  明:
// 创建日期:    2014年10月23日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#include "Skillmodule.h"

#include "FSGame/define/CommandDefine.h"
#include "FSGame/define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "Impl/SkillDataQueryModule.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "../Define/GameDefine.h"
#include "FsGame/Define/FightPropertyDefine.h"
#include "utils/string_buffer.h"
#include "SkillToSprite.h"
#include "FsGame/SystemFunctionModule/CoolDownModule.h"
#include "../Define/ViewDefine.h"
#include "utils/string_util.h"
#include "Impl/SkillFlow.h"
#include "Impl/Skillaccess.h"
#include "Impl/SkillStage.h"
#include "utils/custom_func.h"
#include "public/Converts.h"
#include "../Interface/PropertyInterface.h"
#include "../Define/SnsDefine.h"
#include "../Define/Skilldefine.h"
//#include "../TradeModule/VipModule.h"
#include "../CommonModule/EnvirValueModule.h"
//#include "../SocialSystemModule/RideModule.h"
#include "FightModule.h"
#include "../Define/Fields.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif

extern void declare_skill_lua_ext(IKernel* pKernel);

SkillModule* SkillModule::m_pInstance = NULL;

//初始化
bool SkillModule::Init(IKernel* pKernel)
{
	m_pInstance = this;

	// 初始化技能流
	if (!SkillFlowSingleton::Instance()->Init(pKernel) ||
		!SkillAccessSingleton::Instance()->Init(pKernel))
	{
		return false;
	}

	pKernel->AddEventCallback("NormalNPC", "OnLoad", SkillModule::OnNpcLoad);

	pKernel->AddEventCallback("player", "OnReady", SkillModule::OnReady);
	pKernel->AddEventCallback("player", "OnContinue", SkillModule::OnReady);
	pKernel->AddEventCallback("player", "OnRecover", SkillModule::OnPlayerRecover, -1);
	pKernel->AddEventCallback("scene", "OnQueryGift", SkillModule::OnPlayerRecover, -1);

	pKernel->AddEventCallback("SkillNormal", "OnRecover", SkillModule::OnSkillRecover, -1);
	pKernel->AddEventCallback("SkillNormal", "OnLoad", SkillModule::OnSkillLoad, -1);

	pKernel->AddClassCallback("SkillContainer", "OnCreateClass", SkillModule::OnSkillContainerCreateClass);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_USE_SKILL, SkillModule::OnCustomUseSkill);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_USE_SKILL_BY_CLIENT, SkillModule::OnCustomDoHitByClient);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_BREAK_SKILL_FINISH, SkillModule::OnCustomBreakCurSkill);

	//属性回调
	DECL_CRITICAL(SkillModule::C_OnSkillLevelChanged);

	// CantUseSkill 回调
	DECL_CRITICAL(SkillModule::C_OnCantUseSkillChanged); 

	declare_skill_lua_ext(pKernel);

	// 技能预加载数据
	PreLoadSkillConfig(pKernel);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("SkillConfig", SkillModule::ReloadSkillConfig);
#endif
	return true;
}

//释放
bool SkillModule::Shut(IKernel* pKernel)
{
	return true;
}

// 加载全部资源
bool SkillModule::LoadAllResource(IKernel* pKernel)
{
	// 加载技能,buffer等静态配置数据
	if (!SkillDataQueryModule::m_pInstance->LoadResource(pKernel))
	{
		return false;
	}
	return true;
}

// ----------------------------使用技能-------------------------------------

//在指定位置直接使用技能， 技能的使用目标由规则产生
//针对某个对象使用的技能
int SkillModule::UseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return USESKILL_RESULT_ERR_OTHER;
	}

	if (SkillFlowSingleton::Instance()->IsSkillUseBusy(pKernel, self) != USESKILL_RESULT_SUCCEED)
	{
		return USESKILL_RESULT_ERR_OTHERSKILL_ISHITING;
	}

	PERSISTID useskill;
	// 技能为空,则找下是否有准备待释放的技能
	if (!skill.IsNull())
	{
		useskill = skill;
	}
	else
	{
		if (pSelfObj->FindData(READY_RANDOM_SKILL))
		{
			useskill = pSelfObj->QueryDataObject(READY_RANDOM_SKILL);
		}
	}

	//技能使用对象
	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;
	float dorient = 0.0f;
	PERSISTID target = PERSISTID();

	size_t count = args.GetCount();
	if (count == 1 && args.GetType(0) == VTYPE_OBJECT)
	{
		// 单攻技能target是目标的对象号,群攻技能是自己的对象号
		target = args.ObjectVal(0);
		IGameObj* pTargetObj = pKernel->GetGameObj(target);
		if (NULL != pTargetObj)
		{
			if (pTargetObj->QueryInt("Dead") > 0)
			{
				return 0;
			}
			pKernel->GetLocation(target, dx, dy, dz, dorient);
		}
	}
	else if(2 == count)
	{
		dx = args.FloatVal(0);
		dz = args.FloatVal(1);
		dy = pKernel->GetMapHeight(dx, dz);
	}

	if (!SkillAccessSingleton::Instance()->GetSkillTargetAndPos(pKernel, self, useskill, target, dx, dy, dz, dorient))
	{
		// 技能目标位置修正失败
		// TODO
		if (pSelfObj->FindData(READY_RANDOM_SKILL))
		{
			// 使用成功清除准备技能
			pSelfObj->SetDataObject(READY_RANDOM_SKILL, PERSISTID());
		}
		return USESKILL_RESULT_ERR_OTHER;
	}

	// 获取技能使用者的位置
	float x, y, z, orient;
	pKernel->GetLocation(self, x, y, z, orient);

	int iUseSkillRet = SkillFlowSingleton::Instance()->UseSkill(pKernel, self, x, y, z, useskill, target, dx, dy, dz, dorient);

	if (pSelfObj->FindData(READY_RANDOM_SKILL))
	{
		// 使用成功清除准备技能
		pSelfObj->SetDataObject(READY_RANDOM_SKILL, PERSISTID());
	}
	if (1 != iUseSkillRet) 
	{
		IGameObj* pSkillObj = pKernel->GetGameObj(useskill);
		if (NULL == pSkillObj)
		{
			return 0;
		}
		const char* strSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);
		::extend_warning(LOG_TRACE, "[SkillModule::UseSkill] free skill %s use error.",
			strSkillConfigID);
	}

	return iUseSkillRet;
}
//////////////////////////////////////////////////////////////////////////
//刷新所有技能的属性
bool SkillModule::RefreshAllSkillConfigProperty(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self))
	{
		return false;
	}

	PERSISTID skillcontainer = SKillToSpriteSingleton::Instance()->GetSkillContainer(pKernel, self);
	if (!pKernel->Exists(skillcontainer))
	{
		return false;
	}

	unsigned int it;
	PERSISTID skill = pKernel->GetFirst(skillcontainer, it);

	LoopBeginCheck(c)
	while (pKernel->Exists(skill))
	{
		LoopDoCheck(c)
		RefreshSkillConfigProperty(pKernel, skill);

		skill = pKernel->GetNext(skillcontainer, it);
	}

	return true;
}

//刷新某个技能的属性
bool SkillModule::RefreshSkillConfigProperty(IKernel* pKernel, const PERSISTID& skill)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSkillObj)
	{
		return false;
	}

	const char* szSkillConfigID = pSkillObj->QueryString(SKILL_CONFIGID);

	const SkillBaseData* pSkillBase = SkillDataQueryModule::m_pInstance->GetSkillBase(szSkillConfigID);
	if (NULL != pSkillBase)
	{
		pSkillBase->SetPropData(pKernel, skill);
	}

	SkillDataQueryModule::m_pInstance->OnSkillPropChange(pKernel, skill, PERSISTID());
	return true;
}


// -------------------------事件回调函数------------------------------
bool SkillModule::AddSkillViewport(IKernel* pKernel, const PERSISTID& self, int iViewPortID, const wchar_t* name)
{
	
	if (!pKernel->Exists(self) || NULL == name)
	{
		return false;
	}
	if (!pKernel->FindViewport(self, iViewPortID))
	{
		PERSISTID skillcontainer = pKernel->GetChild(self, name);
		//创建Skill容器
		if (pKernel->Exists(skillcontainer))
		{
			pKernel->AddViewport(self, iViewPortID, skillcontainer);
		}
	}

	return true;
}

bool SkillModule::PreLoadSkillConfig(IKernel* pKernel)
{
	CVarList pathList;
	if (!SkillDataQueryModule::m_pInstance->GetSkillConfigPath("SkillNew", pathList))
	{
		return false;
	}

	// 处理需要预先加载的配置文件
	fast_string strFile;
	LoopBeginCheck(a)
	for (size_t i = 0; i < pathList.GetCount(); ++i)
	{
		LoopDoCheck(a)
		strFile = "ini/";
		strFile += pathList.StringVal(i);
		pKernel->PreloadConfig(strFile.c_str());
	}
	return true;
}

// 技能升级回调
int SkillModule::C_OnSkillLevelChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old)
{
	SkillDataQueryModule::m_pInstance->OnSkillPropChange(pKernel, self, PERSISTID());
	return 0;
}

// 是否能够使用技能回调   
int SkillModule::C_OnCantUseSkillChanged(IKernel* pKernel, const PERSISTID& self,
								   const char* property,const IVar& old)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int nStatus = pSelfObj->QueryInt("CantUseSkill");
	// 不能使用技能
	if (1 == nStatus)
	{
		pKernel->Command(self, self, CVarList() << COMMAND_CANT_USE_SKILL_BREAK_SKILL);
	}
	return 0;
}

// 重新加载技能配置
void SkillModule::ReloadSkillConfig(IKernel* pKernel)
{
	// 加载技能
	if (!SkillModule::m_pInstance->LoadAllResource(pKernel))
	{
		return;
	}

	// buff衰减的配置
//	BufferDecayLogicSingleton::Instance()->LoadConfig(pKernel);
}

// 玩家变身弩车用的技能使用限制
bool SkillModule::IsCanUseSkill(IKernel* pKernel, const PERSISTID& self, const char* strSkillId)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
// 	int nFrontierTransState = pSelfObj->QueryInt(FIELD_PROP_FRONTIER_TRANS_STATE);
// 	if (nFrontierTransState != FTS_BALLISTA_STATE)
// 	{
// 		return true;
// 	}

// 	const char* strBallistSkill = FrontierModule::ms_cfgBase.GetBallisterSkillID().c_str();
// 	if (strcmp(strBallistSkill, strSkillId) != 0)
// 	{
// 		return false;
// 	}

	return true;
}

//客户端就绪
int SkillModule::OnReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    int online_flag = pSelfObj->QueryInt("Online");
    if (online_flag == ONLINE)
    { 
        // 仅仅对在线玩家设置视图
        AddSkillViewport(pKernel, self, VIEWPORT_SKILL, SKILL_CONTAINER_NAME);
    }

	return 0;
}

bool SkillModule::RegisterSkillContainer(IKernel* pKernel, const PERSISTID& self, const wchar_t* name, int capacity, bool unsave)
{
	if (NULL == name || capacity <= 0)
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	IGameObj* pContainerObj = pSelfObj->GetChild(name);
	if (NULL == pContainerObj)
	{
		PERSISTID skillcontainer = pKernel->CreateContainer(self, StringUtil::WideStrAsString(SKILL_CONTAINER_NAME).c_str(), capacity);
		pContainerObj = pKernel->GetGameObj(skillcontainer);
		if (NULL == pContainerObj)
		{
			return false;
		}
		pContainerObj->SetWideStr("Name", name);
		//不需要保存
		pKernel->SetUnsave(skillcontainer, unsave);
	}

	return true;
}

//对象加载配置文件完成
int SkillModule::OnNpcLoad(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	RegisterSkillContainer(pKernel, self, SKILL_CONTAINER_NAME, MAX_NPC_SKILL_CAPACITY, false);

	return 0;
}

int SkillModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
								 const PERSISTID& sender, const IVarList& args)
{
	// CantUseSkill回调
	if (!pKernel->FindCritical(self,"CantUseSkill","SkillModule::C_OnCantUseSkillChanged"))
	{
		pKernel->AddCritical(self, "CantUseSkill", "SkillModule::C_OnCantUseSkillChanged");
	}


	RegisterSkillContainer(pKernel, self, SKILL_CONTAINER_NAME, MAX_PLAYER_SKILL_CAPACITY, false);
	return 0;
}

//技能从数据库恢复
int SkillModule::OnSkillRecover(IKernel* pKernel, const PERSISTID& skill,
								const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (NULL == pSkillObj || NULL == pSenderObj)
	{
		return 0; 
	}

	const char* szSkillConfigID = pKernel->GetConfig(skill);

	pSkillObj->SetString(SKILL_CONFIGID, szSkillConfigID);
	if (0 == pSkillObj->QueryInt("Level"))
	{
		pSkillObj->SetInt("Level", 1);
	}

	const char* szSkillConfigID2 = pSkillObj->QueryString(SKILL_CONFIGID);
 
	//刷新技能属性
	m_pInstance->RefreshSkillConfigProperty(pKernel, skill);

	// 添加属性回调
	if (pSkillObj->FindAttr("Level"))
	{
		pKernel->AddCritical(skill, "Level", "SkillModule::C_OnSkillLevelChanged");
	}

	return 0;
}

//技能加载配置文件完成
int SkillModule::OnSkillLoad(IKernel* pKernel, const PERSISTID& skill,
							 const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	IGameObj* pSenderObj = pKernel->GetGameObj(sender);
	if (NULL == pSkillObj || NULL == pSenderObj)
	{
		return 0;
	}

	fast_string strSkillConfigID = pKernel->GetConfig(skill);
   
	pSkillObj->SetString(SKILL_CONFIGID, strSkillConfigID.c_str());

	// 添加属性回调
	if (pSkillObj->FindAttr("Level"))
	{
		pKernel->AddCritical(skill, "Level", "SkillModule::C_OnSkillLevelChanged");
	}

	m_pInstance->RefreshSkillConfigProperty(pKernel, skill);
	return 0;
}

//技能容器类创建
int SkillModule::OnSkillContainerCreateClass(IKernel* pKernel, int index)
{
	pKernel->SetSaving(index, "Name", true);//需要保存，可能同时存在多个SkillContainer对象
	return 0;
}


void SkillModule::DoUseSkillParamProcess(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || !pKernel->Exists(target))
	{
#ifdef _DEBUG
		if (target.IsNull())
		{
			return;
		}
		/*char strObj[32] = {0};
		char strInfo[128] = {0};
		SPRINTF_S(strInfo, "[SkillModule::DoUseSkillParamProcess] target %s no exist", ObjectToString(target, strObj, sizeof(strObj)));
		::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, strInfo, CVarList());*/
#endif
		return ;
	}

	// 设为当前选中对象
	pSelfObj->SetObject("LastObject", target);

	pSelfObj->SetObject(FIGHTER_CUR_SKILL_TARGET, target);
	return ;
}

// ------------------------------自定义消息回调-----------------------------
//使用技能——相应客户端消息
int SkillModule::OnCustomUseSkill(IKernel* pKernel, const PERSISTID& self,
								  const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 检查参数个数及参数类型
	if (args.GetCount() < 8)
	{
		::extend_warning(LOG_TRACE, "[SkillModule::OnCustomUseSkill] the param args cout error!");
		return 0;
	}
	
	if (args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_STRING || args.GetType(2) != VTYPE_FLOAT 
		|| args.GetType(3) != VTYPE_FLOAT || args.GetType(4) != VTYPE_FLOAT || args.GetType(5) != VTYPE_FLOAT
		|| args.GetType(6) != VTYPE_FLOAT || args.GetType(7) != VTYPE_OBJECT)
	{
		return 0;
	}

	// 关于带有位移技能的处理:1.先位移,再释放技能的,sx,sy,sz
	fast_string strSkillConfigID = args.StringVal(1);
	float sx = args.FloatVal(2);
	float sy = args.FloatVal(3);
	float sz = args.FloatVal(4);
	float dx = args.FloatVal(5);
	float dz = args.FloatVal(6);
	PERSISTID target = args.ObjectVal(7);

	// 防止客户端y有误
	float fPosY = pKernel->GetMapHeight(sx, sz);
	sy = __max(sy, fPosY);
	
#ifdef _DEBUG
	char strInfo1[128] = {0};
	SPRINTF_S(strInfo1, "[SkillModule::OnCustomUseSkill] use skill %s use", strSkillConfigID.c_str());
	::extend_warning(LOG_INFO, strInfo1);
#endif
	

	if (StringUtil::CharIsNull(strSkillConfigID.c_str()))
	{
		return 0;
	}

	// 处理客户端发送过来的参数信息
	m_pInstance->DoUseSkillParamProcess(pKernel, self, target);

	PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, strSkillConfigID.c_str());
	if (pKernel->Exists(skill))
	{
		// 技能的释放类型(默认是无目标的释放)
		PERSISTID selectLockTarget;
		float dy = pKernel->GetMapHeight(dx, dz);
		float dorient = ::util_dot_angle(sx, sz, dx, dz);
		int nUseSkillResult = SkillFlowSingleton::Instance()->UseSkill(pKernel, self, sx, sy, sz, skill, selectLockTarget, dx, dy, dz, dorient);
		if (1 != nUseSkillResult)
		{
		#ifdef _DEBUG
			/*char strInfo[128] = {0};
			SPRINTF_S(strInfo, "[SkillModule::OnCustomUseSkill] free skill %s use error. result:%d", strSkillConfigID.c_str(), nUseSkillResult);
			::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, strInfo, CVarList());*/
		#endif
			::extend_warning(LOG_INFO, "[SkillModule::OnCustomUseSkill] free skill %s use error. result:%d", strSkillConfigID.c_str(), nUseSkillResult);
		}
	}

	return 0;
}

// 使用技能(客户端通知命中处理)
/*!
	 * @brief	使用技能(客户端处理流程)
	 * @param	string		技能的id
	 * @param	int64		技能的uid
	 * @param	int			当前命中次数索引
	 * @param	int			打击的目标数n
	 以下循环n次
	 * @param	PERSISTID	目标对象号
	 */
//	CLIENT_CUSTOMMSG_USE_SKILL_BY_CLIENT = 706,
int SkillModule::OnCustomDoHitByClient(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetType(0) != VTYPE_INT || args.GetType(1) != VTYPE_STRING || args.GetType(2) != VTYPE_INT64 || args.GetType(3) != VTYPE_INT)
	{
		return 0;
	}

	int nIndex = 1;
	fast_string strSkillConfigID = args.StringVal(nIndex++);
	int64_t nSkilluid = args.Int64Val(nIndex++);
	int nCurHitIndex = args.IntVal(nIndex++);

	if (!IsCanUseSkill(pKernel, self, strSkillConfigID.c_str()))
	{
		return 0;
	}

	// 统计无位移的受击目标
	int nTargetNum = args.IntVal(nIndex++);
	CVarList targetList;
	LoopBeginCheck(b)
	for (int i = 0;i < nTargetNum;++i)
	{
		LoopDoCheck(b)
		targetList << args.ObjectVal(nIndex++);
	}

	// 统计带位移的受击目标
	int nMoveTargetNum = args.IntVal(nIndex++);
	CVarList moveTargetList;
	LoopBeginCheck(c)
	for (int i = 0;i < nMoveTargetNum;++i)
	{
		LoopDoCheck(c)
		PERSISTID target = args.ObjectVal(nIndex++);
		float x = args.FloatVal(nIndex++);
		float z = args.FloatVal(nIndex++);
		float speed = args.FloatVal(nIndex++);
		moveTargetList << target << x << z << speed;
	}
	
	if (StringUtil::CharIsNull(strSkillConfigID.c_str()))
	{
		return 0;
	}
	// 找到技能对象
	PERSISTID skill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, strSkillConfigID.c_str());
	if (!pKernel->Exists(skill))
	{
		return 0;
	}

	char strInfo[128] = {0};
	SPRINTF_S(strInfo, "HitByClient skillid = %s, HitIndex=%d, TargetNum=%d", strSkillConfigID.c_str(), nCurHitIndex, nTargetNum + nMoveTargetNum);
	::extend_warning(LOG_INFO, strInfo);

	SkillFlowSingleton::Instance()->DoSkillHitByClient(pKernel, self, skill, nSkilluid, nCurHitIndex, targetList, moveTargetList);

	return 0;
}

// 响应客户端打断技能请求
int SkillModule::OnCustomBreakCurSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetType(0) != VTYPE_INT)
	{
		return 0;
	}

	//发送打断命令
	pKernel->Command(self, self, CVarList() << COMMAND_ON_CLIENT_BREAK_SKILL);
	return 0;
}

// 准备随机使用一个技能
bool SkillModule::ReadyUseRandomSkill(IKernel* pKernel, const PERSISTID& self, float& fSkillMinDis, float& fSkillMaxDis, const char* strSkillId)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	if (SkillFlowSingleton::Instance()->IsSkillUseBusy(pKernel, self) != USESKILL_RESULT_SUCCEED)
	{
		return false;
	}
	// 首先处理优先释放的技能
	if (!StringUtil::CharIsNull(strSkillId))
	{
		PERSISTID curskill = SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, strSkillId);
		if (!SkillAccessSingleton::Instance()->IsCoolDown(pKernel, self, curskill))
		{
			fSkillMinDis = 0.0f;	// 目前最小技能距离都为0
			// 取出最新的待释放技能
			fSkillMaxDis = SkillAccessSingleton::Instance()->GetSkillHitDistance(pKernel, strSkillId);
			return true;
		}
	}
	
	fSkillMinDis = 0.0f;	// 目前最小技能距离都为0
	// 保存准备释放的技能
	if (!pSelfObj->FindData(READY_RANDOM_SKILL))
	{
		pSelfObj->AddDataObject(READY_RANDOM_SKILL, PERSISTID());
	}

	PERSISTID readyskill = pSelfObj->QueryDataObject(READY_RANDOM_SKILL);
	// 没有待释放技能就随机一个
	if(!pKernel->Exists(readyskill))
	{
		const PERSISTID skill = SKillToSpriteSingleton::Instance()->FindRandomSkillNotInCD(pKernel, self, fSkillMaxDis);
		if (skill.IsNull())
		{
			return false;
		}
		pSelfObj->SetDataObject(READY_RANDOM_SKILL, skill);
	}
	else
	{
		const char* strReadySkillId = pKernel->GetConfig(readyskill);
		fSkillMaxDis = SkillAccessSingleton::Instance()->GetSkillHitDistance(pKernel, strReadySkillId);
	}

	return true;
}