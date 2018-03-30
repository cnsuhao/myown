#ifndef  __SKILLDEFINE_H__
#define  __SKILLDEFINE_H__

#include <cstring>
//////////////////////////////////////////////////////////////////////////
//
// 玩家技能容器最大容量
const static int MAX_PLAYER_SKILL_CAPACITY = 512;

// NPC技能容器最大容量
const static int MAX_NPC_SKILL_CAPACITY = 32;

// 通用回复HP的技能
const static char* const RESTORE_HP_SKILL_ID = "10000";

// 技能最大等级
const static int MAX_SKILL_LEVEL = 60;

// 技能释放选择类型
enum ESkillSelectType
{
    SKILL_SELECT_TYPE_DEFAULT = 0,          // 瞬发（无选择）
    SKILL_SELECT_TYPE_SELECT_OBJECT = 1,    // 对象选取辅助（目标）
	SKILL_SELECT_TYPE_POS = 2,				// 选取某个位置
};

// 技能作用范围类型
enum ESkillHitRangeType
{
    SKILL_HIT_RANGE_TYPE_SELECT_OBJECT = 0,     // 仅作用于锁定对象
    SKILL_HIT_RANGE_TYPE_SPHERE = 1,            // 作用于球形范围
    SKILL_HIT_RANGE_TYPE_CYLINDER = 2,          // 作用于圆柱范围
    SKILL_HIT_RANGE_TYPE_RECTANGULAR = 3,       // 作用于矩形范围
    SKILL_HIT_RANGE_TYPE_POS_CYLINDER = 4,      // 作用某个位置的圆柱范围
};


// 技能射程参数结构
struct SkillCastRangeParams
{	
	SkillCastRangeParams(): fRangeDistMax(0.0f), fRangeDistMin(0.0f){}
    float fRangeDistMax;            // 最大有效距离
    float fRangeDistMin;            // 最小有效距离
};

// 技能作用范围参数结构
struct SkillHitRangeParams
{
    SkillHitRangeParams()
    {
        memset((void*)this, 0, sizeof(SkillHitRangeParams));
    }

    ESkillHitRangeType eHitRangeType;    // 技能作用范围类型
    union SkillHitRangeUnion
    {
        struct HitRangeSphere // 球形范围
        {
            float fHitRangeSphereDistCentre;    // 球形范围-施法者距球形中心距离
            float fHitRangeSphereDistMax;       // 球形范围-最大有效距离
            float fHitRangeSphereDistMin;       // 球形范围-最小有效距离
            float fHitRangeSphereTurnAngle;     // 球形范围-面向有效转角
            float fHitRangeSphereUpAngle;       // 球形范围-面向有效仰角
            float fHitRangeSphereOffsetOrient;  // 球形范围-顺时针转角
            float fHitRangeSphereBasepointOffsetOrient;// 球形范围-基点顺时针转角
        } sphere;

        struct HitRangeCylinder // 圆柱范围
        {
            float fHitRangeCylinderDistCentre;          // 圆柱范围-施法者距圆柱中心线距离
            float fHitRangeCylinderDistMax;             // 圆柱范围-最大有效距离
            float fHitRangeCylinderDistMin;             // 圆柱范围-最小有效距离
            float fHitRangeCylinderTurnAngle;           // 圆柱范围-面向有效转角
            float fHitRangeCylinderHeight;              // 圆柱范围-圆柱有效高度
            float fHitRangeCylinderOffsetOrient;        // 圆柱范围-顺时针转角
            float fHitRangeCylinderBasepointOffsetOrient;// 圆柱范围-基点顺时针转角
        } cylinder;

        struct HitRangeRectangular      // 矩形范围
        {
            float fHitRangeRectangularDistCentre;   // 矩形范围-施法者距矩形中心距离
            float fHitRangeRectangularLength;       // 矩形范围-有效长度
            float fHitRangeRectangularWidth;        // 矩形范围-有效宽度
            float fHitRangeRectangularHeight;       // 矩形范围-有效高度
            float fHitRangeRectangularOffsetOrient; // 矩形范围-顺时针转角
            float fHitRangeRectangularBasepointOffsetOrient;// 矩形范围-基点顺时针转角
        } rectangular;

		struct HitRangePosCylinder // 以某个点为圆心的圆形范围内
		{
			float fHitRangeRadius;					// 圆的半径
		}poscylinder;
    } skillhitrangeunion ;
};


// 技能作用对象关系
enum EHitTargetRelationType
{
    HIT_TARGET_RELATION_TYPE_NONE = 0x00,
    HIT_TARGET_RELATION_TYPE_SELF = 0x01,       // 作用于自身
    HIT_TARGET_RELATION_TYPE_FRIEND = 0x02,     // 作用于友方
    HIT_TARGET_RELATION_TYPE_ENIMY = 0x04,      // 作用于敌方

    HIT_TARGET_RELATION_TYPE_ALL = 0xFF,        // 包含以上所以类别
};

// 技能作用对象类型
enum EHitTargetType
{
    HIT_TARGET_TYPE_NONE = 0x00,
    HIT_TARGET_TYPE_PLAYER = 0x01,              // 玩家
    HIT_TARGET_TYPE_MONSTER = 0x02,             // 怪物
    HIT_TARGET_TYPE_FUNCTION_NPC = 0x04,        // 功能NPC
    HIT_TARGET_TYPE_TASK_NPC = 0x08,            // 任务NPC
    HIT_TARGET_TYPE_BROKEN_NPC = 0x10,          // 破碎NPC
    HIT_TARGET_TYPE_PET_NPC = 0x20,             // 宠物NPC

    HIT_TARGET_TYPE_ALL = 0xFF,                 // 包含以上所以类别
};

// 公共CD的最小百分比
#define  PUBLIC_MIN_VALUE_CD_PERCENT  40

//////////////////////////////////////////////////////////////////////////


// 技能阶段定义
enum ESkillStageType
{
    SKILL_STAGE_TYPE_NONE = 0,
    SKILL_STAGE_TYPE_BEGIN,         // 开始使用技能：使用者，目标类型
    SKILL_STAGE_TYPE_PREPARE,       // 吟唱：截至时间
    SKILL_STAGE_TYPE_LEAD,          // 引导：截至时间
    SKILL_STAGE_TYPE_EFFECT,        // 造成效果：类型
    SKILL_STAGE_TYPE_BREAK,         // 打断：原因
    SKILL_STAGE_TYPE_FINISH,        // 结束：原因 	
	
    SKILL_STAGE_TYPE_MAX,
};

// 技能使用目标类型定义
enum ESkillStageTargetType
{
    SKILL_STAGE_TARGET_TYPE_NONE = 0,			// 没有目标
    SKILL_STAGE_TARGET_TYPE_TARGET,             // 目标者
    SKILL_STAGE_TARGET_TYPE_POSITION,           // 位置
};

// 技能造成效果类型定义
enum ESkillStageEffectType
{
    eSkillStageEffectType_None = 0,
    eSkillStageEffectType_DecHP,            // 扣除HP：hp
    eSkillStageEffectType_Miss,             // 未命中:
    eSkillStageEffectType_Hits,             // 击中次数：hits
    eSkillStageEffectType_AddHP,            // 医疗：hp
    eSkillStageEffectType_VA,               // 爆击：hp, times
	eSkillStageEffectType_Dodge,            // 闪避：
//     eSkillStageEffectType_PlayEffect,       // 播放效果
// 	eSkillStageEffectType_BeHitAttack,		// 反击
// 	eSkillStageEffectType_Gaint,            // 免疫

	eSkillStageEffectType_Max,
};

// 伤害类型
enum DamageType
{
	NONE_DAMAGE_TYPE,						// 非攻击技能
	PHYS_DAMAGE_TYPE,						// 物理攻击伤害
	MAGIC_DAMAGE_TYPE,						// 魔法攻击伤害
	BOTH_DAMAGE_TYPE,						// 物魔都有

	MAX_DAMAGE_TYPE_NUM
};
//技能使用的目标类型
enum
{
    USESKILL_TYPE_TARGET,   //目标对象
    USESKILL_TYPE_POSITION, //目标位置
};

//技能使用结果
enum EUseSkillErrorCode
{
	USESKILL_RESULT_ERR_OTHER = 0,			//其它错误

	USESKILL_RESULT_SUCCEED,				//释放成功
	USESKILL_RESULT_ERR_FIND_PROPERTY,		//查找不到属性
	USESKILL_RESULT_NO_OBJECT,				//释放者不存在
	USESKILL_RESULT_ERR_NOSKILL,			//对象身上没有这个技能
	USESKILL_RESULT_ERR_CANTBEATTACK,		//目标不能释放被攻击
	USESKILL_RESULT_ERR_DEAD,               //死亡后不能释放
	USESKILL_RESULT_ERR_COOLDOWN,			//技能在冷却中
	USESKILL_RESULT_ERR_MP,					//蓝不够

	USESKILL_RESULT_ERR_BUFFER_PROPETY,     //buffer属性不满足
	USESKILL_RESULT_ERR_BUFFER_CONTAINER,   //buffer容器不存在
	USESKILL_RESULT_ERR_BUFFER,				//buffer对象不存在

	USESKILL_RESULT_ERR_HITING_BUSY ,		//正在命中技能，不能使用其他技能
	USESKILL_RESULT_ERR_LEADING_BUSY,		//正在引导技能，不能使用其他技能
	USESKILL_RESULT_ERR_PREPARE_BUSY,		//正在准备技能，不能使用其他技能
	
	USESKILL_RESULT_ERR_CONFIG_DATA,		//技能数据配置错误
	USESKILL_RESULT_ERR_OTHERSKILL_ISHITING, //其他技能正在使用中
	USESKILL_RESULT_ERR_CANTUSESKILL,		 //逻辑上使用技能失败
	USESKILL_RESULT_ERR_PUBLICCD,			//公共冷却中使用技能失败
	USESKILL_RESULT_ERR_STIFF_STATE,		// 处于技能硬直状态
// 	USESKILL_RESULT_ERR_SILENT,				// 在沉默中
// 	USESKILL_RESULT_ERR_STORAGE_NUM,		// 蓄力次数不够
// 	USESKILL_RESULT_ERR_STIFF,				// 硬直中

	USESKILL_RESULT_ERR_MAX,				//最大错误号
};

//判断某个对象是否在某个范围内的过滤结果

enum ETargetFilterResult
{
	TARGETFILTER_RESULT_ERR_OTHER = 0,

	TARGETFILTER_RESULT_SUCCEED,				  //成功
	TARGETFILTER_RESULT_ERR_TOONEAR,              //距离太近
	TARGETFILTER_RESULT_ERR_TOOFAR,               //距离太远
	TARGETFILTER_RESULT_ERR_HORI,                 //不在水平角度范围
	TARGETFILTER_RESULT_ERR_VERT,                 //不在垂直角度范围
	TARGETFILTER_RESULT_ERR_TARGETTYPE_REEOR,     //目标类型配置错误

};

enum ESkillBreakReasonType
{
    SKILL_BREAK_REASON_TYPE_NONE = 0,
	SKILL_BREAK_REASON_TYPE_DEAD,				  // 死亡打断技能
    SKILL_BREAK_REASON_TYPE_CANTUSESKILL,         // CantUseSkill属性变化，打断正在释放的技能
    SKILL_BREAK_REASON_TYPE_NEW_SKILL,            // 对象新放一个技能，打断当前技能
    SKILL_BREAK_REASON_TYPE_CLIENT		          // 客户端主动打断技能
};

const char* const CONST_SKILL_NPC_NORMAL_ATTACK = "Skill_Npc_Normal_Attack";
const char* const READY_RANDOM_SKILL = "ReadySkill";

//击退效果导致的Motion
#define  MOTION_MODE_HITBACK  0x0101

//只同步朝向的Motion
#define  MOTION_MODE_ROTATE 0x0201

//拉扯效果的motion
#define  MOTION_MODE_PULL 0x0301

// 客户端技能设置消息
enum CustomSkillSetupMsg
{
    // 升级技能
    C2S_ESKILL_UPGRADE_SKILL = 1,

	// 一次升多级
	C2S_ESKILL_UPGRADE_MULTI,

    // 技能解锁
    S2C_ESKILL_SETUP_SKILL_UNLOCKED,

	// 被动技能重置成功
	S2C_ESKILL_PASSIVE_SKILL_RESET_SUC,
};

// 技能位移对象类型
enum SkillDisplaceObject
{
	DISPLACE_SELF_TYPE,					// 施法者位移
	DISPLACE_TARGET_TYPE				// 目标位移
};

// 技能位移类型
enum SkillDisplaceType
{
	DISPLACE_NONE_TYPE = 0,				// 无效值
	DISPLACE_USE_SKILL_MOVETO = 1,		// 使用技能后MOVETO效果
	DISPLACE_USE_SKILL_MOTION = 2,		// 使用技能后MOTION效果
};

// 技能位移的motion速度
#define SKILL_HITBACK_MOTION_SPEED 15.0f

// 0 CoolDownTime     1 PublicCDTime     2 LeadTime          3 HitTime 
// 技能时间差
enum SkillTime
{
	COOL_DOWN_TIME_DIFF,			// CD时间差
	PUBLIC_CD_TIME_DIFF,			// 公共CD时间差
	LEAD_TIME_DIFF,					// 引导时间差
	HIT_TIME_DIFF,					// 命中时间差

	MAX_SKILL_TIME_DIFF_NUM
};

// 客户端命中失败类型
enum ClientHitFailed
{
	VALID_TIME_FAILED,				// 验证命中时间失败
	VALID_HIT_INDEX_FAILED,			// 验证命中索引失败
	VALID_DISTANCE_FAILED			// 验证距离失败
};

// 技能携带对象类型
enum SkillForObject
{
	PLAYER_SKILL,					// 玩家技能
	PET_SKILL,						// 宠物技能
	RIDE_SKILL,						// 坐骑技能
	NPC_SKILL						// npc技能
};

// 成长属性类型
enum SkillGrowPropertyType
{
	SKILL_GP_ATTACK_ADD,			// 附加攻击力
	SKILL_GP_COOL_DOWN_TIME,		// cd
	SKILL_GP_RESTORE_HP_RATE,		// 技能回复HP比率
	SKILL_GP_DAMAGE_RATE,			// 技能伤害倍率

	MAX_SKILL_GP_NUM
};

// 成长属性无效值
#define NO_VALID_GROW_PROPERTY -1

// 被动技能类型
enum PassiveSkillEffect
{
	PS_ADD_PLAYER_PROPERTY,				// 增加玩家属性
	PS_ADD_SKILL_PROPERTY,				// 增加技能属性("SkillAttackAdd","SkillDamageRate")
	PS_CHANGE_SKILL_CONFIG,				// 改变技能配置属性(例如:CD,leadtime等)
	PS_ADD_SKILL_EFFECT,				// 附加技能效果 SkillEffect
	PS_ADD_NEW_BUFF,					// 附加新BUFF

	MAX_PASSIVE_SKILL_TYPE_NUM
};

// 增加玩家属性配置标识
const char* const ADD_PLAYER_PROPERTY_FLAG = "PlayerPropAdd";

// 增加技能对象属性
const char* const ADD_SKILL_PROPERTY_FLAG = "SkillPropAdd";

// 改变技能配置属性
const char* const CHANGE_SKILL_CONFIG_FLAG = "SkillConfigChange";

// 附加技能效果 SkillEffect
const char* const ADD_SKILL_EFFECT_FLAG = "SkillEffectAdd";

// 新增加Buff
const char* const ADD_NEW_BUFF_FLAG = "AddNewBuff";

// 被动技能修改的属性名
const char* const SKILL_ATTACK_ADD = "SkillAttackAdd";			// 技能攻击力
const char* const SKILL_DAMAGE_RATE = "SkillDamageRate";		// 技能伤害百分比
const char* const SKILL_CD_TIME_NAME = "CoolDownTime";			// 冷却时间
const char* const SKILL_LEAD_TIME_NAME = "LeadTime";			// 持续时间

// 玩家被动技能表名
const char* const PASSIVE_SKILL_REC_NAME = "passive_skill_rec";

// 技能点属性名
const char* const SKILL_POINT_NAME = "SkillPoint";

// 被动技能来源
enum PassiveSkillSource
{
	PSS_BY_STUDY,					// 学习获得的被动技能
	PSS_BY_PET,						// 通过宠物获得的被动技能
 	//PSS_BY_ACUPOINT,				// 通过激活奇穴获得的被动技能
	PSS_BY_RIDE,					// 通过坐骑进阶获得的被动技能
	PSS_BY_GUILD,					// 组织
	PSS_BY_SOUL,					// 通过附魔获得的被动技能
// 	PSS_BY_WING,					// 通过翅膀被动技能
// 	PSS_BY_EVOLVE,					// 通过技能进化获得被动技能
};

// 被动技能表
enum PassiveSkillRec
{
	PS_COL_SKILL_ID,				// 被动技能id
	PS_COL_SKILL_LEVEL,				// 被动技能等级
	PS_COL_SKILL_SRC,				// 被动技能来源

	MAX_PS_COL_NUM
};

// 使用技能时的操控状态
// enum USOperationState
// {
// 	USO_NONE_OPERATION = 0,			// 不可操控
// 	USO_CHANGE_ORIENT,				// 可转向操控
// 	USO_CAN_MOTION					// 可移动操控
// };

// 技能类型
enum SkillType
{
	NONE_SKILL_TYPE = -1,
	USE_SKILL_TYPE,					// 主动技能
	PASSIVE_SKILL_TYPE,				// 被动技能
	INTERACT_SKILL_TYPE				// 交互技能
};

// 被动技能类型
enum PassiveSkillType
{
	NONE_PASSIVE_SKILL_TYPE = -1,
	PST_REN_MAI,					// 任脉
	PST_DU_MAI,						// 督脉
	PST_DAI_MAI						// 带脉
};

#endif //__SKILLDEFINE_H__