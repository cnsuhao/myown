//--------------------------------------------------------------------
// 文件名:      AIDefine.h
// 内  容:      AI系统
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef AIDefine_h__
#define AIDefine_h__
#include <map>
#include <vector>
#include <string>

using namespace std;

/***********************************AI模板和执行相关定义BEGIN***************************************/

//所有AI模板类型编号定义
enum AI_TEMPLATE_TYPE
{
    AI_TEMP_BASE = 0,       //基础模板
    AI_TEMP_NORMAL_FIGHT,   //普通小怪
    AI_TEMP_BOSS,	        //BOSS（带技能，释放规则）
    AI_TEMP_CONVOY,         //护送
    AI_TEMP_INTERCEPT,      //拦截
    AI_TEMP_PATROLONLY_NPC,	//巡逻
    AI_TEMP_BATTLELEAD,		//国战隐身队长(离线玩家）
    AI_TEMP_BATTLENPC,		//国战队员（离线玩家）
    AI_TEMP_INTERCEPTPLAYER,//拦截NPC（离线玩家）
    AI_TEMP_OUTLINEPLAYER,	//挂机打怪（离线玩家）
	AI_TEMP_MYSTICALBOX,	//神秘宝箱				（没用到）
    AI_TEMP_NATION_GUARD,	//国家守卫将军
    AI_TEMP_ARENA_NPC,      //竞技场NPC    
    AI_TEMP_PET,	        //宠物NPC
    AI_TEMP_PATROLMONSTER,  //巡逻野怪NPC
	AI_TEMP_SECTOR_SPRING_RANGE,	//扇形警戒怪
	AI_TEMP_SCERET_GUARD,	//秘境守护npc
	AI_TEMP_SCERET_PROJECT_OBJ,//受保护目标
	AI_TEMP_ESCAPE_NPC,      //逃跑npc
	AI_TEMP_FIGHT_PATROL_NPC,//巡逻野怪npc能够战斗
	AI_TEMP_BATTLE_BUFF_NPC,// 阵营战buffer Npc
	AI_TEMP_GVG_DOGFACE,	// 跨服帮会战小兵
	AI_TEMP_DEAD_GEN_TRAP,	// 死亡生成陷阱
	AI_TEMP_ESCORT,         // 押囚
	AI_TEMP_START_SCENE_BOSS,//体验场景boss
	AI_TEMP_FRO_TOWER,		 // 边境箭塔
	AI_TEMP_CONTROL_HP_BOSS, //控制血量停止boss
	AI_TEMP_INVADE_BOSS,	//阵营护送boss
    AI_TEMP_MAX,
};


//所有现有AI执行规则类型定义
enum AI_RULE_TYPE
{
    AI_RULE_SPRING = 0,      //触发
    AI_RULE_PATROL,          //执行巡逻规则
    AI_RULE_SEL_TARTGET,     //选择目标
    AI_RULE_BEGIN_FIGHT,     //开始战斗
    AI_RULE_FIGHT,           //战斗
    AI_RULE_BE_DAMAGED,      //被伤害
    AI_RULE_END_FIGHT,       //结束战斗
    AI_RULE_ONDEAD,		     //死亡
    AI_RULE_CHASE_TARGET,    //追击攻击对象
    AI_RULE_CHECK_ATTACKERS, //检查攻击者表
    AI_RULE_RELIVE,          //复活
    AI_RULE_END_PATROL,      //停止巡逻
    AI_RULE_BACK_BORN_POS,   //回出生点
    AI_RULE_BORN,            //出生
    AI_RULE_FOLLOW,          //跟随
    AI_RULE_RECUPERATE,     //休养复原中
	AI_RULE_DESTORY,        //销毁
	AI_RULE_ESCAPE,         //逃跑

    AI_RULE_MAX,
};

//触发规则下的子规则类型定义
enum
{
    AIRS_SPRING_BASE = 0,        //基础规则
    AIRS_SPRING_INTERCEPT,       //拦截护送NPC
    AIRS_SPRING_BATTLENPC,		 //国战NPC
    AIRS_SPRING_OUTLINEPLAYER,	 //离线玩家挂机
    AIRS_SPRING_NATION_GUARD,    //国家守卫将军战触发规则
	AIRS_SPRING_SECTOR_SPRING,	 //扇形响应区npc
	AIRS_SPRING_DEFEND_TOWER,    //防御塔触发规则
	AIRS_SPRING_GVG,             //帮会战
    AIRS_SPRING_MAX
};
//跟随规则下的子规则类型定义
enum
{
    AIRS_FOLLOW_PET,       //宠物
    AIRS_FOLLOW_MAX
};
//巡逻规则下的子规则类型定义
enum 
{
    AIRS_PATROL_BASE = 0,        //基础规则
    AIRS_PATROL_CONVOY,          //护送
    AIRS_PATROL_BATTLENPC,		 //国战NPC
    AIRS_PATROL_PET,			//宠物
	ATRS_PATROL_SECTOR,			//扇形响应ncp
	AIRS_PATROL_GVG,			//gvg
    AIRS_PATROL_MAX
};
//选择目标规则下的子规则类型定义
enum 
{
    AIRS_SEL_TARGET_BASE = 0,     //基础规则
    AIRS_SEL_TARGET_BATTLENPC,	  //国战NPC
	AIRS_SEL_TARGET_TOWER,	      //防御塔设置目标规则
	AIRS_SEL_GVG,	              //gvg帮会战
    AIRS_SEL_TARGET_MAX
};
//开始战斗规则下的子规则类型定义
enum 
{
    AIRS_BEGIN_FIGHT_BASE = 0,    //基础规则
    AIRS_BEGIN_FIGHT_BATTLENPC,	  //国战NPC
    AIRS_BEGIN_FIGHT_MAX,
};
//攻击AI规则类型下的子规则类型定义
enum 
{
    AIRS_FIGHT_BASE = 0,         //基础规则
    AIRS_FIGHT_BATTLENPC,		 //国战NPC
    AIRS_FIGHT_PET,
	AIRS_FIGHT_STARTSCENEBOSS,	// 体验场景boss
	AIRS_FIGHT_CONTROL_BOSS,	// 血量boss控制
    AIRS_FIGHT_MAX
};
//被伤害时的规则类型下的子规则类型定义
enum 
{
    AIRS_BE_DAMAGED_BASE = 0,            //基础规则
    AIRS_BE_DAMAGED_CONVOY,				 //护送
    AIRS_BE_DAMAGED_INTERCEPT,			 //拦截
    AIRS_BE_DAMAGED_MAX
};
//结束战斗AI规则类型下的子规则类型定义
enum 
{
    AIRS_END_FIGHT_BASE = 0,              //基础规则
    AIRS_END_FIGHT_BATTLENPC = 1,		  //国战NPC脱离战斗
    AIRS_END_FIGHT_INTERCEPT,			//拦截
	AIRS_END_FIGHT_GVG,			        //GVG
    AIRS_END_FIGHT_MAX
};
//被杀死规则类型下的子规则类型定义
enum 
{
    AIRS_ONDEAD_BASE = 0,               //基础规则
    AIRS_ONDEAD_CONVOY,                 //护送
    AIRS_ONDEAD_BATTLENPC,				//国战NPC
	AIRS_ONDEAD_MYSTICALBOX,			//宝箱
	AIRS_ONDEAD_GVG,					//GVG
	AIRS_ONDEAD_GEN_TRAP,				// 死后生成陷阱
	AIRS_ONDEAD_ESCORT,                 //押囚
	AIRS_ONDEAD_INVADE,                 //入侵boss
    AIRS_ONDEAD_MAX
};
//追击攻击者Ai规则类型下的子规则类型定义
enum 
{
    AIRS_CHASE_TARGET_BASE = 0,
	AIRS_CHASE_TARGET_TOWER,			// 塔类追击规则
    AIRS_CHASE_TARGET_MAX
};
//整理候选攻击目标表AI规则类型下的子规则类型定义
enum 
{
    AIRS_CHECK_ATTACKERS_BASE = 0,
	AIRS_CHECK_ATTACKERS_BATTLENPC,
    AIRS_CHECK_ATTACKERS_MAX
};
//复活规则类型下的子规则类型定义
enum
{
    AIRS_RELIVE_BATTLENPC = 0,
    AIRS_RELIVE_MAX
};

//休养规则类型下的子规则类型定义
enum
{
    AIRS_RECUPERATE_BOSS = 0,
    AIRS_RECUPERATE_MAX
};

//销毁规则类型下的子规则类型定义
enum
{
	AIRS_DESTROY_BATTLE_LEAD_NPC = 0,
	AIRS_DESTROY_MAX
};

// 逃跑规则类型下的子规则类型定义
enum
{
	AIRS_ESCAPE_BASE = 0,
	AIRS_ESCAPE_MAX
};

//AI处理返回结果
enum
{
	AI_RT_NULL = 0, //空值
	AI_RT_SUCCESS,  //成功
	AI_RT_IGNORE,   //忽略，不处理
	AI_RT_END_FIGHT,//结束战斗
	AI_RT_TRUE ,    //返回真
	AI_RT_FALSE,    //返回假
	AI_RT_OVER,     //完成状态
	AI_RT_FAIL = -1, //失败
};

/***********************************AI模板和执行相关定义END***************************************/
//攻击者数据表列的位标
enum
{
	AI_ATTACKER_REC_OBJECT = 0,//攻击对象
	AI_ATTACKER_REC_DAMAGE,    //总仇恨值
	AI_ATTACKER_REC_LASTTIME,  //最近攻击时间
	AI_ATTACKER_REC_FIRSTTIME,	//开始攻击时间
	
	AI_ATTACKER_REC_COL_NUM,
};

//AI巡逻模式
enum
{
	AI_PATROL_MODE_ROUND = 0, //0,环形走模式
	AI_PATROL_MODE_BACK,      //1,来回
	AI_PATROL_MODE_RANDOM,    //2,随机
	AI_PATROL_MODE_MAX
};

//AI巡逻点表
enum
{
	AI_PATROL_REC_POS_X = 0,  //x,y,z
	AI_PATROL_REC_POS_Z,
	AI_PATROL_REC_POS_TIME,   //停留时间
};

//发起走动行为的标志  
//用于识别是什么状态下发起的移动行为  MoveState
enum
{
	MOVE_PATROL = 0,
	MOVE_FIGHT,
	MOVE_BACK_BORN,
};

enum 
{
	AI_RANGE_OK,    //正好
	AI_RANGE_NEAR,  //太近
	AI_RANGE_OVER   //太远
};

//行走判定
enum 
{
	BACTMSG_MOTION_SUCCEED, //成功到达
	BACTMSG_MOTION_FAIL		//行走失败
};

//队伍状态
enum 
{
	//初始
	AI_BATTLETEAM_STATUS_BORN = 0,
	//移动
	AI_BATTLETEAM_STATUS_MOVE = 1,
	//战斗
	AI_BATTLETEAM_STATUS_FIGHT = 2,
};

//小队表结构
enum
{
	AI_BATTLEPLAYER_OBJECT = 0, //对象
	AI_BATTLEPLAYER_STATUS = 1, //状态
};

//小队成员状态
enum
{
	//初始
	AI_BATTLEPLAYER_STATUS_BORN = 0,
	//移动
	AI_BATTLEPLAYER_STATUS_MOVE = 1,
	//战斗
	AI_BATTLEPLAYER_STATUS_FIGHT = 2,
	//回追
	AI_BATTLEPLAYER_STATUS_BACK = 3,
};

//国战队伍结构
enum
{
	AI_BATTLETEAM_SERIAL = 0, //序号
	AI_BATTLETEAM_LEADER, //队长
	AI_BATTLETEAM_STATUS, //状态
	AI_BATTLETEAM_MOVETYPE, //移动策略
	AI_BATTLETEAM_NEXT_POSX, //下个X
	AI_BATTLETEAM_NEXT_POSZ, //下个Z
	AI_BATTLETEAM_FIGHT_POSX, //触发点X
	AI_BATTLETEAM_FIGHT_POSZ, //触发点Z
};

// 创建参数分类
enum
{
	CREATE_TYPE_BATTLELEAD_NPC = 0,	//小队队长
	CREATE_TYPE_BATTLE_NPC,			//队员
	CREATE_TYPE_SKILL_NPC,			//技能NPC
	CREATE_TYPE_PROPERTY_VALUE,	    //带属性值创建对象 Key-Value值对方式（只支持替换）
};

//掉落归属类型定义
enum AI_DROP_ASCRIPTION
{
    AI_DROP_FIRST_ATTACKER = 0, //第一次攻击
    AI_DROP_DAMAGE_MAX,			//总伤害最大
    AI_DROP_LAST_ATTACKER,		//最后一次攻击
};

//神秘宝箱掉落类型
enum
{
	MYSTICALBOX_BOSS = 1,	//Boss
	MYSTICALBOX_ITEM,		//物品
	MYSTICALBOX_SNARE,		//陷阱
	MYSTICALBOX_OWNER,		//物品只给所有者
};

//NPC类型
enum NPC_TYPE:int
{
    NPC_TYPE_ORDINARY = 1,  //普通怪
    NPC_TYPE_ELITE,         //精英
    NPC_TYPE_BOSS,          //BOSS

    NPC_TYPE_BOX = 10,     //宝箱

	NPC_TYPE_BUILDING = 100, // 建筑npc
	NPC_TYPE_CONVOY = 122, //护送npc
	NPC_TYPE_FUNCTION = 200,  // 功能npc

	NPC_TYPE_FUNCTION_FORCE_VISUAL = 300 // 强制可见的静态npc
};

// 护送状态
enum ConvoyStates
{
	CONVOY_STATE_DOING,	// 进行中
	CONVOY_STATE_END,	// 结束
};

#define NPC_SKILL_CONFIG "ini/npc/npc_config/npc_skill.xml"
#define NPC_SKILL_PACKAGE_CONFIG "ini/npc/npc_config/npc_skill_pak.xml"
#define CONVOY_PATH_CONFIG "ini/npc/npc_config/npc_convoy_path.xml"
#define CONVOY_INTERCEPT_CONFIG "ini/npc/npc_config/npc_convoy_intercept.xml"
#define	PATROL_PATH_CONFIG "ini/npc/npc_config/npc_patrol_path.xml"

#define BATTLE_NPC_CONFIG "ini/npc/npc_config/nation_battle/battle_npc_config.xml"
#define BATTLE_NPC_MOVETYPE_CONFIG "ini/npc/npc_config/nation_battle/battle_npc_movetype.xml"
#define BATTLE_TEAM_CONFIG "ini/npc/npc_config/nation_battle/battle_team_config.xml"
#define BATTLE_TEAM_FORMATION_CONFIG "ini/npc/npc_config/nation_battle/battle_formation.xml"

#define BOX_WEIGHT_CONFIG "ini/npc/npc_config/npc_box.xml"
#define GUILD_DEFEND_PATH_CONFIG "ini/npc/npc_config/npc_guild_defend_path.xml"
#define GUILD_DEFEND_NPC_PACK_KEY "ini/ModifyPackage/guild_defend_npc_pack_key.xml"
#define NPC_LEVEL_PACKAGE_FILE "ini/npc/npc_config/npc_level_package.xml"

#define AI_PARTOL_COUNT 4  //生成路径点个数
#define AI_PARTOL_RANDOM_SPACE 1  //生成路径点个数

#define NPC_DROP_INFO_EXP    "npc_drop_info_exp"          //经验值增加[@0:num]
#define NPC_DROP_INFO_ITEM   "npc_drop_info_item"         //物品：[@0:config] [@1:num]
#define NPC_DROP_INFO_MONEY  "npc_drop_info_money"		  //金钱增加[@0:num]

// NPC技能表
#define NPC_SKILL_REC "boss_skill_rec"
//NPC技能表
enum
{
    BOSS_SKILL_ID = 0,            //技能编号
    BOSS_SKILL_VALUE_CUR,         //当前权值
};
// 怪物技能使用权值结构
struct  NpcSkill
{   
    NpcSkill() : skillID(""), maxWeighting(0), addWeighting(0), prior(0), condition(0), time(0), initWeighting(0){}
    //技能名
	std::string skillID;
    //释放权值
	int maxWeighting;
    //每次攻击增加权值
	int addWeighting;
    //优先级
	int prior;
    //小于等血量百分比才释放
	int condition;
    //每追击一次增加权值
    int time;
    //初始权值
    int initWeighting;
};

//NPC技能包
struct NpcSkillPackage
{
	NpcSkillPackage(): id(""), normalSkill(""), randCount(0)
	{
		skillVec.clear();
	}
    std::string id;     //包ID
    std::string normalSkill;    //NPC普通攻击技能
	std::string bornskill;		// 出生后立即使用的技能
    int randCount;  //随机增加个数
    std::vector<std::string> skillVec;   //随机技能库
};

//拦截NPC结构
struct Intercept
{
	Intercept(): id(0), npcId(""), num(0), monsterX(0.0f), monsterZ(0.0f){}
	int id;
	std::string npcId;
	int num;
	float monsterX;
	float monsterZ;
};

//寻路NPC结构
struct PatrolPos 
{
	PatrolPos(): PosX(0.0f), PosZ(0.0f), iRestTime(0){}
	float PosX;
	float PosZ;
	int iRestTime;
};

//路径点坐标结构
struct BattlePos
{
	BattlePos(): fPosX(0.0f), fPosZ(0.0f), fOrient(0.0f){}
	float fPosX;
	float fPosZ;
	float fOrient;
};

//小队NPC结构
struct BattleNpcStruct
{
	BattleNpcStruct(): BattleLeadNpc(""), BattleNpc("")
	{
		m_MapBattleNpc.clear();
	}
	std::string BattleLeadNpc;
	std::string BattleNpc;
	std::map<int, std::map<int, BattlePos>> m_MapBattleNpc;
};

//行走方式结构
struct MoveTypePos
{
	MoveTypePos(): fPosX(0.0f), fPosZ(0.0f){}
	float fPosX;
	float fPosZ;
};

typedef std::map<int, std::map<int, std::vector<MoveTypePos>>> MoveTypeDef;

// 小队数据
struct BattleTeam 
{
	BattleTeam() : nTeamId(0),
				nCamp(0),
				nMoveType(0),
				nBornPosIndex(0),
				nLifeTime(0)
	{

	}
	int		nTeamId;			// 小队id
	int		nCamp;				// 攻守方
	int		nMoveType;			// AI行走类型
	int		nBornPosIndex;		// 出生位置索引
	int		nLifeTime;			// 小队存在时间
};

typedef std::vector<BattleTeam>	BattleTeamVec;

// 队形规则
struct BattleTeamFormation
{
	int scene_id;	// 场景ID
	int row;
	int column;	

	BattleTeamFormation(int _id) :
		scene_id(_id),
		row(0),
		column(0)
	{
	}

	bool operator == (const BattleTeamFormation& formation) const
	{
		return formation.scene_id == scene_id;
	}
};
typedef std::vector<BattleTeamFormation> BattleTeamFormationVec;

//单个掉落权重区间
struct DropWeight
{
	DropWeight()
		:iType(0),
		DropId(""),
		iWeightBegin(0),
		iWeightEnd(0)
	{}

	int iType;
	std::string DropId;
	int iWeightBegin;
	int iWeightEnd;
};
typedef std::vector<DropWeight> VEC_DROPWEIGHT;

//宝箱掉落集合
struct BoxWeight
{
	BoxWeight(): configId(""), WeightSum(0)
	{
		vec_drop.clear();
	}
	std::string configId;
	int WeightSum;	//权重总和
	VEC_DROPWEIGHT vec_drop;
};
typedef std::map<std::string, BoxWeight>	MAP_BOXWEIGHT;

// 等级属性包
struct NpcLevelPackage
{
	NpcLevelPackage() : 
		n_package_id(0), 
		n_upper_level(0), 
		n_lower_level(0)
		{}

	std::string s_config_id;
	int n_package_id;
	int n_upper_level;
	int n_lower_level;
};
typedef std::vector<NpcLevelPackage> NpcLevelPackageVec;

//普通怪追击和选择攻击目标思考频率
const unsigned int AI_NORMAL_CHASE_RATE = 500;

#endif