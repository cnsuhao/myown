#ifndef _PK_MODEL_DEFINE_H_
#define _PK_MODEL_DEFINE_H_

// 关系类型
enum TestRelationType
{		
	TRT_FRIEND,			// 验证是否为友方
	TRT_ENEMY			// 验证是否为敌方
};

//场景攻击模式类型
enum SceneAttackType
{
	SCENE_ATTACK_PUBLIC_SCENE = 0,	//野外公共场景(通过玩家自身的pk模式确定攻击目标)
	SCENE_ATTACK_CAMP,				//只要阵营不同就可攻击
	SCENE_ATTACK_GUILD,				//只要公会不同就可攻击
	SCENE_ATTACK_PEACE,				//和平模式
	SCENE_ATTACK_PVE,				//只能打怪模式

	SCENE_ATTACK_TYPE_MAX
};

// 玩家个人攻击模式
enum PKType
{							
    PT_PEACE,					// 和平模式
	PT_GUILD,					// 公会模式
	PT_KILL_ALL,				// 杀戮模式
	PT_ATT_CRIME,				// 善恶模式

	PT_MAX_NUM
};

// 罪恶状态
enum CrimeState
{				
	CS_NORMAL_STATE,				// 正常状态
	CS_GRAY_STATE,					// 灰名(临时)
	CS_RED_STATE					// 红名
};

// 罪恶惩罚
enum CrimePublish
{
	CP_CAPITAL_RATE,				// 铜钱惩罚系数
	CP_EXP_RATE						// 经验惩罚系数
};

enum PKC2SMsg
{
	PK_C2S_CHANGE_PK_MODEL,				// 改变PK模式 int model
	PK_C2S_COST_MONEY_CLEAN_CRIME,		// 花费元宝清除罪恶值 int cost
	PK_C2S_SET_DEFAULT_MODEL,			// 设置场景pk默认模式 int 场景id int 模式类型
};

enum PKS2CMsg
{
	PK_S2C_COST_MONEY_SUCCESS,			// 花费元宝消除罪恶值成功
};

#define	SYS_INFO_CLEAR_CRIME_VALUE_FAILED_GREATER		"sys_info_clear_crime_value_failed1"		// 罪恶值太大
#define	SYS_INFO_CLEAR_CRIME_VALUE_FAILED_MONEY			"sys_info_clear_crime_value_failed2"		// 货币不够
#endif