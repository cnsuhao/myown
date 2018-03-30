#ifndef EquipDefine_h__
#define EquipDefine_h__


// 一件装备是否可以装备的返回结果
typedef enum
{
    EQUIPCHECK_RESULT_SUCCEED = 1,              //成功

    //以下为检查失败
    EQUIPCHECK_RESULT_ERR_NOTEQUIPITEM = 11,    //该物品不是一件装备
    EQUIPCHECK_RESULT_ERR_NOEQUIPTYPE = 12,     //没有定义此装备类型
    EQUIPCHECK_RESULT_ERR_POSITION = 20,        //该物品的装备位置不匹配
    EQUIPCHECK_RESULT_ERR_SEX = 21,				//该物品的性别限制条件不匹配
    EQUIPCHECK_RESULT_ERR_LEVEL = 22,           //该物品的级别限制条件不匹配
    EQUIPCHECK_RESULT_ERR_JOB = 23,             //该物品的职业限制条件不匹配

} EQUIPCHECK_RESULT;

enum EQUIP_POS
{
	EQUIP_POS_STR_MIN	= 1,	// 需要改造的装备的最小位置(包括普通装备不包括时装)

	EQUIP_POS_WEAPON	= 1,	// 武器
	EQUIP_POS_CLOTH		= 2,	// 衣服
	EQUIP_POS_HELMET	= 3,	// 头盔
	EQUIP_POS_NECKLACE	= 4,	// 项链
	EQUIP_POS_RING		= 5,	// 戒指
	EQUIP_POS_CHARM		= 6,	// 护符
    
	EQUIP_POS_STR_MAX	= 6,	// 需要改造的装备的最大位置
	//end
	EQUIP_MAX_PLACEHOLDER,
};

#define EQUIP_STRHENTHEN_REC	 "EquStrengthenRec"
enum
{
	EQUIP_STRHENTHEN_INDEX	= 0,	//装备格子索引,byte,(1-10)
	EQUIP_STRHENTHEN_LEVEL	= 1,	//格子强化等级,int
	EQUIP_STRHENTHEN_SOUL	= 2,	//格子灵魂,string(id)
	EQUIP_STRHENTHEN_SOULDEVOUR	= 3,//格子灵魂吞噬灵魂数量,int
	EQUIP_STRHENTHEN_JEWEL1	= 4,	//格子宝石,string(id1)
	EQUIP_STRHENTHEN_JEWEL2	= 5,	//格子宝石,string(id2)
	EQUIP_STRHENTHEN_JEWEL3	= 6,	//格子宝石,string(id3)
	EQUIP_STRHENTHEN_JEWEL4	= 7,	//格子宝石,string(id4)
};

#define EQUIP_BATTLE_ABILITY_REC "EquBattleAbilityRec"  // 装备战斗力表

// 战斗力表的列定义
enum BattleAbilityRecCol
{
    EQUIP_BA_MIN_COL        = 0,
    EQUIP_BA_EQUIP_POS	    = 0,	//装备格子索引,byte,(1-10)
    EQUIP_BA_EQUIPMENT      = 1,	//格装备战斗力,int
    EQUIP_BA_STRHENTHEN     = 2,	//强化战斗力,int
    EQUIP_BA_JEWEL1	        = 3,	//宝石1战斗力, int
    EQUIP_BA_JEWEL2	        = 4,	//宝石2战斗力, int
    EQUIP_BA_JEWEL3	        = 5,	//宝石3战斗力, int
    EQUIP_BA_JEWEL4	        = 6,	//宝石4战斗力, int
    EQUIP_BA_SOUL           = 7,    // 灵魂战斗力
	EQUIP_BA_SOUL_CLOTH     = 8,	// 灵魂神装战斗力, 灵魂神装记录在武器那一行
	EQUIP_BA_BAPTISE        = 9,    // 洗炼战斗力
	EQUIP_BA_FASHION        = 10,   // 对应部位的时装的战斗力
    EQUIP_BA_MAX_COL        = 10,
    
};

enum
{
	EQUIP_STRPROP_OPTION_ADD	= 1,//添加
	EQUIP_STRPROP_OPTION_REMOVE	= 2,//删除	
	EQUIP_STRPROP_OPTION_UPDATE	= 3,//更新
};

#endif // EquipDefine_h__
