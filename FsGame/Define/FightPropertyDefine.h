//------------------------------------------------------------------------------
// 文件名:      FightPropertyDefine.h
// 内  容:      战斗相关属性定义
// 说  明:
// 创建日期:    2014年11月4日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------


#ifndef __FIGHTPROPERTYDEFINE_H__
#define __FIGHTPROPERTYDEFINE_H__


const char* const AMBER_BUFFER = "Buffer";
const char* const AMBER_SKILL_NORMAL = "SkillNormal";
const char* const AMBER_PASSIVE_SKILL = "SkillPassive";

//技能容器名(客户端可视)
const wchar_t* const SKILL_CONTAINER_NAME = L"SkillContainer";

const wchar_t* const wszBufferContainerName = L"BufferContainer";

// 战斗者属性
const char* const FIGHTER_CUR_SKILL = "CurSkill"; // 目前使用技能
const char* const FIGHTER_CUR_SKILL_UUID = "CurSkillUUID";
const char* const FIGHTER_PRE_SKILL_UUID = "PreSkillUUID";
const char* const FIGHTER_CUR_SKILL_TARGET = "CurSkillTarget";   //
const char* const FIGHTER_CUR_SKILL_TARGET_X = "CurSkillTargetX";
const char* const FIGHTER_CUR_SKILL_TARGET_Y = "CurSkillTargetY";
const char* const FIGHTER_CUR_SKILL_TARGET_Z = "CurSkillTargetZ";
const char* const FIGHTER_PUBLIC_CD_TIME = "PublicCDTime";
const char* const FIGHTER_CUR_SKILL_START_TIME = "CurSkillStartTime";


const char* const FIGHTER_DEAD = "Dead";
const char* const FIGHTER_CANT_USE_SKILL = "CantUseSkill";

const char* const FIGHTER_FLOWING = "Flowing";
//const char* const FIGHTER_FLOW_PAUSING = "FlowPausing";
const char* const FIGHTER_FLOW_CATEGORY = "FlowCategory";
const char* const FIGHTER_FLOW_BEGIN_TIME = "FlowBeginTime";
const char* const FIGHTER_FLOW_PREPARE_TIME = "FlowPrepareTime";
const char* const FIGHTER_FLOW_LEAD_TIME = "FlowLeadTime";
const char* const FIGHTER_FLOW_LEAD_SEP_TIME = "FlowLeadSepTime";
//const char* const FIGHTER_FLOW_TOTAL_HIT_TIME = "FlowTotalHitTime";
//const char* const FIGHTER_FLOW_PAUSE_BREAK = "FlowPauseBreak";
//const char* const FIGHTER_FLOW_PAUSE_TIME = "FlowPauseTime";
//const char* const FIGHTER_FLOW_RESUME_RESTORE = "FlowResumeRestore";
const char* const FIGHTER_FLOW_TYPE = "FlowType";
const char* const FIGHTER_FLOW_HIT_TIMES = "FlowHitTimes";
const char* const FIGHTER_FLOW_HITS = "FlowHits";
const char* const FIGHTER_FLOW_LEAD_START_TIME = "FlowLeadStartTime";

const char* const FIGHTER_FLOW_MUTIL_HIT_TIMER_REC = "flow_mutli_hit_timer_rec";
const char* const FIGHTER_FLOW_HITTIME_REC = "flow_hittime_rec";

// 技能属性
const char* const SKILL_CONFIGID = "Config";
const char* const SKILL_CUR_HITS = "CurHits";
const char* const SKILL_CUR_BULLET_HIT = "CurBulletHit";
const char* const SKILL_HURT_COUNT = "HurtCount";

const char* const SKILL_TRAP_POS_REC_NAME = "trap_pos_rec";	// 创建陷阱位置表
const char* const SKILL_EXIST_TRAP_REC_NAME = "exist_trap_rec";	// 当前存在陷阱列表


#endif // __FIGHTPROPERTYDEFINE_H__

