//--------------------------------------------------------------------
// 文件名:      AIHeartBeat.cpp
// 内  容:      AI系统心跳
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#include "AIHeartBeat.h"
#include "utils/string_util.h"
#include "Template/AITemplateBase.h"
#include "FsGame/Define/CommandDefine.h"
#include "AISystem.h"
#include <ctime>
#include "FsGame\Interface\FightInterface.h"
//#include "FsGame\Define\NationDefine.h"
#include "utils/util.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "utils/custom_func.h"

#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "utils/custom_func.h"
#include "FsGame/SystemFunctionModule/DropModule.h"
#include "Define/GameDefine.h"
//#include "FsGame/SkillModule/inner/SkillEventExecuteUtil.h"

AIHeartBeat::AIHeartBeat(void)
{
}

//战斗心跳
int AIHeartBeat::HB_AIFighting(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if(pSelfObj->QueryInt("Dead") > 0)
	{
		return 1;
	}

	CVarList args;
	args << slice;

    return AISystem::Instance()->GetAITemlate(pKernel,self).OnProcess(pKernel, AI_RULE_FIGHT,
            self, PERSISTID(), args);
}


//巡逻心跳
int AIHeartBeat::HB_AIPatrol(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if(pSelfObj->QueryInt("Dead") > 0)
	{
		return 1;
	}

	CVarList args;
	args << slice;

    AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_PATROL, self, PERSISTID(), args);
	return 1;
}

// 护送Npc行动检查
int AIHeartBeat::HB_AIEndFight(IKernel* pKernel, const PERSISTID& self, int slice)
{
	if (!pKernel->Exists(self))
	{
		return 1;
	}

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }

    if(pSelf->QueryInt("Dead") > 0)
    {
        return 1;
    }

	// 玩家与押送Npc安全距离
	float chaseRange = pSelf->QueryFloat("ChaseRange");
    
    // 查看玩家是否在范围内
//     const wchar_t* player_name = pSelf->QueryWideStr("ConvoyPlayer");
//     if (!StringUtil::CharIsNull(player_name))
//     {
//         PERSISTID player = pKernel->FindPlayer(player_name);
//         if(pKernel->Exists(player))
//         {
//             float distance = pKernel->Distance2D(player, self);
//             if(distance > chaseRange)
//             {
// 				// 停止前进
// 				pKernel->Command(self, self, CVarList() << COMMAND_NPC_CONVOY_STOP);				
//             }
// 			else
// 			{
// 				// 重新出发
// 				pKernel->Command(self, self, CVarList() << COMMAND_NPC_CONVOY_RESTART);
// 			}
//         }
// 		else
// 		{
// 			// 停止前进
// 			pKernel->Command(self, self, CVarList() << COMMAND_NPC_CONVOY_STOP);
// 		}
//     }
	
	return 1;
}

//出生心跳完成
int AIHeartBeat::HB_AIBornEnd(IKernel* pKernel, const PERSISTID& self, int slice)
{
	if(!pKernel->Exists(self))
    {
		return 1;
    }

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }
    
    if(pSelf->QueryInt("Dead") > 0)
    {
        return 1;
    }

	pSelf->SetInt("BornState", 0);

	//如果还是出生状态则进行巡逻
	int iCurState = pSelf->QueryInt("CurState");
	if (iCurState == AI_STATE_BORN)
	{
		//进入巡逻
    	AISystem::Instance()->GetAITemlate(pKernel, self).BeginPatrol(pKernel, self);
	}

	return 1;
}

//转身心跳
int AIHeartBeat::HB_AIRatate(IKernel* pKernel, const PERSISTID& self, int slice)
{
    if (pKernel->FindHeartBeat(self, "AIHeartBeat::HB_AIRatate"))
    {
        pKernel->RemoveHeartBeat(self, "AIHeartBeat::HB_AIRatate");
    }

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }

    //强制打断技能
    if(FightInterfaceInstance != NULL)
    {
        FightInterfaceInstance->BreakSkill(pKernel, self, self, true);
    }

	//调整怪物朝向
	float fPosx = pSelf->GetPosiX();
	float fPosy = pSelf->GetPosiY();
	float fPosz = pSelf->GetPosiZ();
	float fOrient = pSelf->QueryFloat("BornOrient");
	pKernel->MoveTo(self, fPosx, fPosy, fPosz, fOrient);

// #ifndef FSROOMLOGIC_EXPORTS
// 	BattleTeamModule::m_pBattleTeamModule->AddLittleStepGridRec(pKernel, self, fPosx, fPosz);
// #endif // FSROOMLOGIC_EXPORTS

	AISystem::OnAutoSpring(pKernel, self);
	return 1;
}

//搜索心跳
int AIHeartBeat::HB_AIFind(IKernel* pKernel, const PERSISTID& self, int slice)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }

	IRecord* pAttackList = pSelf->GetRecord("AttackerList");
	if (pAttackList == NULL)
	{
		return 1;
	}

	//查看触发范围
	len_t iSpringRange = AISystem::GetSpringRange(pSelf);
	CVarList res_obj;
	pKernel->GetAroundList(self, iSpringRange, TYPE_NPC|TYPE_PLAYER, 60, res_obj);
    LoopBeginCheck(b);
	for (int i = 0; i < (int)res_obj.GetCount(); ++i)
	{
        LoopDoCheck(b);
		PERSISTID obj = res_obj.ObjectVal(i);
		if (!pKernel->Exists(obj))
		{
			continue;
		}

		//查看能否攻击
		if (!FightModule::m_pInstance->CanDamageTarget(pKernel, self, obj))
		{
			continue;
		}

		//如果已经存在不添加
		if (pAttackList->FindObject(AI_ATTACKER_REC_OBJECT, obj) >= 0)
		{
			continue;
		}

		//能否添加的塞进去
		int iRows = pAttackList->GetRows();
		int iTotalRows = pAttackList->GetRowMax();
		if (iRows >= iTotalRows)
		{
			break;
		}

		CVarList arg_list;
		arg_list << obj << 0 << time(NULL) << (int64_t)0;
		pAttackList->AddRowValue(-1, arg_list);
	}

	if (pSelf->QueryInt("CurState") != AI_STATE_FIGHT)
	{
		CVarList args;
		//不在战斗中，进入战斗
        AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_BEGIN_FIGHT, self, PERSISTID(), args);
	}
	return 1;
}

//延时拷贝玩家数据
int AIHeartBeat::HB_AIClonePlayer(IKernel* pKernel, const PERSISTID& self, int slice)
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }

	int iTemplate = pSelf->QueryInt("AITemplate");
	switch(iTemplate)
	{
	case AI_TEMP_INTERCEPTPLAYER:
		{
			/*if (!IsValidNation(pSelf->QueryInt("Nation")))
			{
				IGameObj *pScene = pKernel->GetSceneObj();
				if (pScene != NULL)
				{
					int sceneNation = pScene->QueryInt("Nation");
					if (sceneNation == NATION_COURT)
					{
						pSelf->SetInt("Nation", NATION_JIANGHU);
					}
					else if (sceneNation == NATION_JIANGHU)
					{
						pSelf->SetInt("Nation", NATION_COURT);
					}
				}
			}*/
           
            
			//获取删除对象
// 			PERSISTID ConvoyNpc = pSelf->QueryDataObject("InterceptNpc");
// 			if (!pKernel->Exists(ConvoyNpc))
// 			{
// 				return 1;
// 			}
// 			const wchar_t* player_name = pKernel->QueryWideStr(ConvoyNpc,"ConvoyPlayer");
			//查看对象
//			const PERSISTID Player = pKernel->FindPlayer(player_name);
// 			if (pKernel->Exists(Player))
// 			{
//                 IGameObj *pPlayer = pKernel->GetGameObj(Player);
//                 if(pPlayer == NULL)
//                 {
//                     return 1;
//                 }
// 				int iNation = pPlayer->QueryInt("Nation");
// 				int iLevel = pPlayer->QueryInt("Level");
// 				//设置对象的属性(敌对国家)
// 				pSelf->SetInt("Nation", iNation%2 + 1);
// 				pSelf->SetInt("Level", iLevel);
// 			}
 		}
		break;
	default:
		break;
	}

#ifndef FSROOMLOGIC_EXPORTS
	//AISystem::m_pCopyOffLineModule->CopyOffLineData(pKernel, self);
#endif // FSROOMLOGIC_EXPORTS

	return 1;
}

//普通怪回出生点超时保护
int AIHeartBeat::HB_BACK_BORN_TIMEOUT(IKernel *pKernel, const PERSISTID &self, int slice)
{
    if (pKernel->FindHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT"))
    {
        pKernel->RemoveHeartBeat(self, "AIHeartBeat::HB_BACK_BORN_TIMEOUT");
    }

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return 1;
    }

    if(pSelf->QueryInt("CurState") == AI_STATE_BACK)
    {
        float bornX = pSelf->QueryFloat("BornX");
        float bornZ = pSelf->QueryFloat("BornZ");
        float hight  = pKernel->GetWalkHeight(bornX, bornZ);
        float orient = pSelf->QueryFloat("BornOrient");

        //回出生点完成，清除攻击表
		IRecord* pRecord = pSelf->GetRecord("AttackerList");
		if (pRecord != NULL)
		{
			pRecord->ClearRow();
		}

        pKernel->MoveTo(self, bornX, hight, bornZ, orient);

// #ifndef FSROOMLOGIC_EXPORTS
// 		BattleTeamModule::m_pBattleTeamModule->AddLittleStepGridRec(pKernel, self, bornX, bornZ);
// #endif // FSROOMLOGIC_EXPORTS

        if(pSelf->QueryInt("BornStand") == 1)
        {
            AISystem::Instance()->GetAITemlate(pKernel, self).ChangeState(pKernel, self, AI_STATE_BORN);
        }
        AIFunction::MotionCallBack(pKernel, self, PERSISTID(), CVarList());

        //多加的判断防止不触发
        //检测周围玩家进行一轮触发
		float SpringRange = AISystem::GetSpringRange(pSelf);
        CVarList arg_list;
        CVarList temp;
        bool bPatrol = true;
        pKernel->GetAroundList(self, SpringRange, TYPE_PLAYER, 30, arg_list);
        LoopBeginCheck(c);
        for ( size_t i = 0; i < arg_list.GetCount(); ++i )
        {
            LoopDoCheck(c);
            PERSISTID player = arg_list.ObjectVal(i);
            if ( pKernel->Exists(player) )
            {
                if(AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_SPRING, self, player, temp) == AI_RT_SUCCESS)
                {
                    bPatrol = false;
                }
            }
        }

        if(bPatrol)
        {
            //进入巡逻
            AISystem::Instance()->GetAITemlate(pKernel, self).BeginPatrol(pKernel, self);
        }
    }
    return 1;
}

// 押送Npc跟随心跳
// int AIHeartBeat::HB_EscortNpcFollow(IKernel *pKernel, const PERSISTID &self, int slice)
// {
// 	IGameObj *pSelfObj = pKernel->GetGameObj(self);
// 	if (NULL == pSelfObj)
// 	{
// 		return 0;
// 	}
// 
// 	std::string err_msg = "";
// 	err_msg.append("    Npc配置有误，请相关策划认领！ NpcID=");
// 	err_msg.append(pSelfObj->GetConfig());
// 	bool is_err = false;
// 
// 
// 	// 属性包是否有效
// 	bool ret = true;
// 	const char *strBasePackage = pKernel->GetConfigProperty(pSelfObj->GetConfig(), "NpcBasePackage");
// 	if (!StringUtil::CharIsNull(strBasePackage))
// 	{
// 		CVarList vPropNameList;
// 		CVarList vPropValueList;
// 
// 		//查看对应包的数据
// 		ret = StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_NPC_BASE_PACK,
// 			strBasePackage, vPropNameList, vPropValueList);
// 	}
// 
// 	if (!ret)
// 	{
// 		err_msg.append("    无效的属性包ID!!");
// 		err_msg.append("    属性包ID=");
// 		err_msg.append(strBasePackage);
// 		is_err = true;
// 	}
// 
// 	// 技能是否有效
// 	const char* skillStr = pKernel->GetConfigProperty(pSelfObj->GetConfig(), "SkillStr");
// 	bool ret1 = true;
// 	if (!StringUtil::CharIsNull(skillStr))
// 	{
// 		if (!AISystem::IsSkillPkgExist(skillStr))
// 		{
// 			ret1 = false;
// 			err_msg.append("    无效的技能包!!");
// 			err_msg.append("    技能包ID=");
// 			err_msg.append(skillStr);
// 		}
// 	}
// 
// 	if (!ret1)
// 	{
// 		is_err = true;
// 	}
// 
// 	// 掉落包是否有效
// 	const char* drop_pkg = pKernel->GetConfigProperty(pSelfObj->GetConfig(), "DropID");
// 	const char *config = pSelfObj->GetConfig();
// 	bool ret2 = true;
// 	if (!StringUtil::CharIsNull(drop_pkg))
// 	{
// 		CVarList drop_id_list;
// 		util_split_string(drop_id_list, drop_pkg, ",");
// 		LoopBeginCheck(a);
// 		for (int i = 0; i < (int)drop_id_list.GetCount(); ++i)
// 		{
// 			LoopDoCheck(a);
// 			if (!DropModule::IsDropPkgExist(drop_id_list.StringVal(i)))
// 			{
// 				ret2 = false;
// 				err_msg.append("    无效的掉落包!!");
// 				err_msg.append("    掉落包ID=");
// 				err_msg.append(drop_id_list.StringVal(i));
// 			}
// 		}
// 		
// 	}
// 
// 	if (!ret2)
// 	{
// 		is_err = true;
// 	}
// 
// 	if (is_err)
// 	{
// 		::CustomSysInfoBroadcast(pKernel, TIPSTYPE_EVENT_CHAT_BROAD_MESSAGE, 
// 			err_msg.c_str(), CVarList());
// 	}
// 	else
// 	{
// 		DELETE_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_EscortNpcFollow");
// 	}
// 
// 	return 0;
// }

// 定位护送Npc心跳
// int AIHeartBeat::HB_LocationConvyNpc(IKernel* pKernel, const PERSISTID& self, int slice)
// {
// 	IGameObj *pSelfObj = pKernel->GetGameObj(self);
// 	if (NULL == pSelfObj)
// 	{
// 		return 0;
// 	}
// 
// 	IRecord *pPatrolPointRec = pSelfObj->GetRecord("PatrolPointRec");
// 	if (pPatrolPointRec == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	// 护送玩家
// 	const wchar_t *player_name = pSelfObj->QueryWideStr(FIELD_PROP_CONVOY_PLAYER);
// 	if (StringUtil::CharIsNull(player_name))
// 	{
// 		return 0;
// 	}
// 
// 	PERSISTID player = pKernel->FindPlayer(player_name);
// 	IGameObj *pPlayer = pKernel->GetGameObj(player);
// 	if (NULL == pPlayer)
// 	{
// 		return 0;
// 	}
// 
// 	// 实时同步玩家速度
// 	float player_speed = pPlayer->QueryFloat(FIELD_PROP_MOVE_SPEED);
// 	float npc_speed = player_speed * 0.9f;
// 	pSelfObj->SetFloat(FIELD_PROP_MOVE_SPEED, npc_speed);
// 
// 	// 超出护送范围预警
// 	float chase_range = pSelfObj->QueryFloat(FIELD_PROP_CHASE_RANGE);
// 	float distance = pKernel->Distance2D(player, self);
// 	if (distance > chase_range*0.8f)
// 	{
// 		// 快要超出护送范围
// 		::CustomSysInfo(pKernel, player, 
// 			TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, CONVOY_TOO_FAR, CVarList());
// 	}
// 
// 	CVarList arg_list;
// 	pKernel->GetAroundList(self, 10.0f, TYPE_NPC, 0, arg_list);
// 
// 	bool beInterceptNpc = false;
// 	LoopBeginCheck(aa);
// 	for (size_t i = 0; i < arg_list.GetCount(); ++i)
// 	{
// 		LoopDoCheck(aa);
// 		PERSISTID npc = arg_list.ObjectVal(i);
// 		IGameObj *pNpc = pKernel->GetGameObj(npc);
// 		if (pNpc == NULL)
// 		{
// 			continue;
// 		}
// 
// 		if (pNpc->FindData("InterceptNpc"))
// 		{
// 			const PERSISTID InterceptNpc = pNpc->QueryDataObject("InterceptNpc");
// 			if (InterceptNpc == self && pNpc->QueryInt("Dead") == 0)
// 			{
// 				beInterceptNpc = true;
// 				break;
// 			}
// 		}
// 	}
// 
// 	// 实时坐标同步
// 	float x = pSelfObj->GetPosiX();
// 	//float y = pSelfObj->GetPosiY();
// 	float z = pSelfObj->GetPosiZ();
// 	//float o = pSelfObj->GetOrient();
// 
// 	// 玩家与npc距离
// 	//float range = util_dot_distance(x, z, pPlayer->GetPosiX(), pPlayer->GetPosiZ());
// 
// 	// 距离小于速度时，取npc向前range的坐标
// 	/*if (range < player_speed*2.0f)
// 	{
// 	SkillEventExecuteUtil::FindPosByDisAndOrient(pKernel, pSelfObj, player_speed*2.0f, o, x, y, z);
// 	}*/
// 
// 	// 有拦截怪
// 	if (beInterceptNpc)
// 	{		
// 		MotionModule::MotionStop(pKernel, self);
// 
// 		ADD_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_AIEndFight", 1000);
// 
// 		pKernel->Command(self, self, CVarList() << COMMAND_NPC_CONVOY_STOP);
// 
// 		// 有拦截怪时，取npc当前坐标
// 		//x = pSelfObj->GetPosiX();
// 		//z = pSelfObj->GetPosiZ();
// 	}
// 	else // 无拦截怪，取下一个路点
// 	{
// 		// 拦截点
// 		int step = pSelfObj->QueryDataInt("PatrolStep");
// 		//CVarList list;
// 		//AISystem::GetInterceptNpc(pKernel, self, pSelfObj->GetConfig(), step+1, list);
// 
// 		x = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_X);
// 		z = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_Z);
// 
// 		// 路点有拦截怪，使用该路点坐标
// 		/*if (!list.IsEmpty())
// 		{
// 		x = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_X);
// 		z = pPatrolPointRec->QueryFloat(step, AI_PATROL_REC_POS_Z);
// 		}*/		
// 	}
// 
// 	CVarList s2c_msg;
// 	s2c_msg << SERVER_CUSTOMMSG_LOCATION_CONVOY
// 			<< CONVOY_STATE_DOING
// 			<< x
// 			<< z
// 			<< pSelfObj->QueryInt(FIELD_PROP_ESCORT_ID);
// 	pKernel->Custom(player, s2c_msg);
// 
// 	return 0;
// }
// 
// // 延迟护送Npc移动
// int AIHeartBeat::HB_DelayConvoy(IKernel* pKernel, const PERSISTID& self, int slice)
// {
// 	if (!pKernel->Exists(self))
// 	{
// 		return 0;
// 	}
// 
// 	// 发送护送Npc行动指令
// 	pKernel->Command(self, self, CVarList() << COMMAND_NPC_CONVOY_BEGIN);
// 
// 	// 护送Npc定位
// 	//AIHeartBeat::HB_LocationConvyNpc(pKernel, self, 0);
// 	ADD_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_LocationConvyNpc", 1000);
// 
// 	return 0;
// }

// 逃跑结束心跳
int AIHeartBeat::HB_EndEscape(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!pSelfObj->FindData("EB_HateObject"))
	{
		return 0;
	}

	PERSISTID target = pSelfObj->QueryDataObject("EB_HateObject");

	DELETE_HEART_BEAT(pKernel, self, "AIHeartBeat::HB_CheckEscape");

	
	// 没找到仇恨对象 转为巡逻
	if (!pSelfObj->FindData("EB_HateObject"))
	{
		AISystem::Instance()->GetAITemlate(pKernel, self).BeginPatrol(pKernel, self);
	}
	else
	{
		PERSISTID target = pSelfObj->QueryDataObject("EB_HateObject");
		if (pKernel->Exists(target))
		{
			AISystem::Instance()->GetAITemlate(pKernel, self).ChangeState(pKernel, self, AI_STATE_PATROL);
			AISystem::Instance()->GetAITemlate(pKernel, self).OnProcess(pKernel, AI_RULE_SPRING, self, target, CVarList());
		}
		else
		{
			AISystem::Instance()->GetAITemlate(pKernel, self).BeginPatrol(pKernel, self);
		}

		pSelfObj->SetDataObject("EB_HateObject", PERSISTID());
	}

	return 0;
}

// 逃跑用到的常量

// 每次寻路的最大搜索次数
static const int MAX_ESCAPE_SEARCH_COUNT = 6;

// 每次转的角度
static const float PER_CHANGE_ANGLE = PI2 / (float)MAX_ESCAPE_SEARCH_COUNT;

// 每次逃跑的搜索距离
static const float ESCAPE_SEARCH_DISTANCE = 5.0f;

// 判定为同一点的差值
static const float SAME_POS_DIS2 = 4.0f;

// 检测逃跑心跳
int AIHeartBeat::HB_CheckEscape(IKernel* pKernel, const PERSISTID& self, int slice)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	if (!pSelfObj->FindData("EB_HateObject"))
	{
		return 0;
	}

	if (FightInterfaceInstance->IsSkillUseBusy(pKernel, self) != USESKILL_RESULT_SUCCEED)
	{
		return 0;
	}

	PERSISTID target = pSelfObj->QueryDataObject("EB_HateObject");
	IGameObj* pTarget = pKernel->GetGameObj(target);
	if (NULL == pTarget)
	{
		return 0;
	}
	// 自己坐标
	float fSelfX = pSelfObj->GetPosiX();
	float fSelfY = pSelfObj->GetPosiY();
	float fSelfZ = pSelfObj->GetPosiZ();
	// 敌人坐标
	float fTargetX = pTarget->GetPosiX();
	float fTargetZ = pTarget->GetPosiZ();

	bool bFindPoint = false;
//	float fDesX, fDesY, fDesZ;
//	LoopBeginCheck(a)
//	float fEscapeOrient = util_dot_angle(fTargetX, fTargetZ, fSelfX, fSelfZ);
// 	for (int i = 0; i < MAX_ESCAPE_SEARCH_COUNT; ++i)
// 	{
// 		LoopDoCheck(a)
// 		SkillEventExecuteUtil::GetFinalPosByOrient(pKernel, fSelfX, fSelfY, fSelfZ, fEscapeOrient, ESCAPE_SEARCH_DISTANCE, fDesX, fDesY, fDesZ);
// 		float fDis2 = util_dot_distance2(fSelfX, fSelfZ, fDesX, fDesZ);
// 		if (fDis2 > SAME_POS_DIS2)
// 		{
// 			bFindPoint = true;
// 			break;
// 		}
// 		// 搜到的点与自己太近,则进行下次搜索
// 		fEscapeOrient += float(i + 1) * PER_CHANGE_ANGLE;
// 	}

// 	if (bFindPoint)
// 	{
// 		AIFunction::StartPatrolToPoint(pKernel, self, fDesX, fDesZ, false);
// 	}

	return 0;
}
