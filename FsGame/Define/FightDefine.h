//------------------------------------------------------------------------------
// 文件名:      FightDefine.h
// 内  容:      战斗结算相关宏定义
// 说  明:
// 创建日期:    2013年3月18日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __FIGHTDEFINE_H__
#define __FIGHTDEFINE_H__

// 战斗行为状态 (见 : FightActionState)
#define  FIGHT_ACTION_STATE "FightActionState"

//#define LEAVE_FIGHT_CD_TIME 5   //5秒

//伤害种类
enum DAMAGE_CATE
{
    DAMAGE_CATE_DEFAULT = 0, //缺省伤害种类
    DAMAGE_CATE_SKILL = 1,  //技能伤害
    DAMAGE_CATE_BUFFER = 2, //buffer伤害
};

//治疗种类
enum HEALTHY_CATE
{
    HEALTHY_CATE_DEFAULT = 0, //缺省治疗种类
    HEALTHY_CATE_SKILL = 1, //技能治疗
    HEALTHY_CATE_BUFFER = 2, //buffer治疗
};

// 攻击结果
enum EHIT_TYPE
{
    HIT_TYPE_INVALID = -1,
    //物理攻击
    PHY_HIT_TYPE_MISS,//未命中
//    PHY_HIT_TYPE_DODGE,//闪避
    PHY_HIT_TYPE_VA,//暴击
    PHY_HIT_TYPE_NORMAL,//一般命中
};
//控制类buff时间类型
enum {
	CONTROL_BUFF_FOREVER = 0, //永久类buff
	CONTROL_BUFF_TIMES = 1,  //时间类buff

};

// 控制类buff类型
enum ControlBuff
{
	CB_CANT_MOVE,			// 不能移动
	CB_CANT_ATTACK,			// 不能攻击
	CB_CANT_BE_ATTACK,		// 不能被攻击
	CB_CANT_USE_SKILL,		// 不能使用技能
	CB_STIFF_STATE,			// 硬直状态中

	MAX_CB_BUFF_NUM		
};

// 控制类buffid有时效
const char* const CONTROL_TIME_BUFF_ID[MAX_CB_BUFF_NUM] = {"buff_CantMove",
													"buff_CantAttack",
													"buff_CantBeAttack",
													"buff_CantUseSkill",
													"buff_StiffState"};

// 控制类buffid永久
const char* const CONTROL_FOREVER_BUFF_ID[MAX_CB_BUFF_NUM] = {"buff_Forever_CantMove",
													"buff_Forever_CantAttack",
													"buff_Forever_CantBeAttack",
													"buff_Forever_CantUseSkill",
													"buff_Forever_StiffState"};

// 战斗行为状态
enum FightActionState
{
	FAS_NORMAL_STATE = 0,			// 正常状态
	FAS_HIT_BACK_STATE,				// 击退状态
	FAS_HIT_DOWN_STATE,				// 击倒状态
	FAS_HIT_FLY_STATE,				// 击飞状态
	FAS_FLOAT_STATE,				// 浮空状态

	FAS_STATE_NUM
};

// 目标受击动作
enum HitTargetType
{
	HTT_NORMAL = 0,        			//原地受击
	HTT_HIT_BACK,     	   			//击退
	HTT_HIT_DOWN,     	   			//击倒
	HTT_HIT_FLY,      	   			//击飞
	HTT_FLOAT,        	   			//浮空
	HTT_HIT_LEFT,     	   			//左受击
	HTT_HIT_RIGHT,    	   			//右受击
};

// 人物状态
enum PlayerStatus
{
	PLAYER_STATUS_LIVING = 0,			// 正常
	PLAYER_STATUS_DEAD = 1,		        // 死亡
};

#endif // __FIGHTDEFINE_H__
