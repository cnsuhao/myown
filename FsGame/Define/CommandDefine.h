#ifndef __COMMANDDEFINE_H__ 
#define __COMMANDDEFINE_H__

enum
{
#pragma region //CommonModule(1-100)
	//GMCC命令
	COMMAND_GMCC,

	// 玩家移动
	COMMAND_PLAYER_MOTION,

    // SNS 数据相关消息
    COMMAND_SNS_DATA,

	/*!
	* @brief	修改循环最大次数 
	* @param	int count 最大次数
	*/
	COMMAND_SET_MAX_CIRCULATE_COUNT,

	//重置活动互斥标识
	COMMAND_RESET_ACTION_MUTEX,

	// 客户端请求指定玩家属性数据
	COMMAND_CLIENT_QUERY_PROP_DATA,

	/*!
	* @brief	玩家分组改变
	* @param	int 改变方式（GroupChangeType）
	*/
	COMMAND_GROUP_CHANGE,

	// 通知玩家击杀Npc（按特定规则）
	COMMAND_KILL_NPC_WITH_RULE,

	// 通知玩家击杀玩家（按特定规则）
	COMMAND_KILL_PLAYER_WITH_RULE,

	/*!
	* @brief	Cg播放结束
	* @param	string CGID
	*/
	COMMAND_PLAY_CG_DONE,
#pragma endregion

#pragma region //SystemFunctionModule(101-200)
    // 与排行榜相关的消息的ID, 子消息ID见 RankingDefine.h
    COMMAND_RANK_LIST = 101,

	//玩家等级变化 命令格式：usgined cmdid, int oldlevel
	COMMAND_LEVELUP_CHANGE,

	// 活动
	COMMAND_DYNAMIC_ACT,

	// 充值
	COMMAND_MSG_PAY,

    // 重载配置文件GM命令
    COMMAND_MSG_REREAD_CFG_GM,

	// 分发GM命令
	COMMAND_MSG_DISPATCH_GM,

	//商店命令
	COMMAND_MSG_SHOP,

	// 保存roleinfo
	COMMAND_SAVE_ROLE_INFO,

	// 翅膀升级
	COMMAND_WING_LEVEL_UP,
	
	// 战斗力更新 命令格式：usgined cmdid, int nType(BatttleAbilityType)
	COMMAND_MSG_UPDATE_BATTLE_ABILITY,

	// 功能开启 命令格式：usgined cmdid,int nFuncType(AFMFunction)
	COMMAND_MSG_ACTIVATE_FUNCTION,

	// 第一次进入游戏
	COMMAND_MSG_FIRST_ENTER,
#pragma endregion

#pragma region //SocialSystemModule(201-300)
	//好友
	COMMAND_FRIEND_MSG = 201,

	// 查阅信件unsigned cmd_id
	COMMAND_QUERY_LETTER,

	// 聊天
	COMMAND_CHAT,

	// 组队
	COMMAND_TEAM_MSG,
	//玩家之间交互请求RequestModule
	//再次确认请求，命令格式：usgined cmdid, int request_type, widestr targetname, int result  ...(参数:暂时只有组队用到)
	COMMAND_GET_ANSWER_PARA,
	//再次确认被请求命令格式：usgined cmdid, int request_type, widestr targetname, int result(0:失败，1:成功,-1:拒绝接收)
	COMMAND_RET_ANSWER_PARA,
	// 响应其他玩家发送的请求，命令格式：usgined cmdid, int request_type, widestr srcname, ...(参数，根据request_type的不同而不同)
	COMMAND_BEREQUEST,
	// 被请求玩家回复请求结果，命令格式：usgined cmdid, int request_type, widestr targetname, int result;
	COMMAND_REQUEST_ANSWER,
	// 响应其他玩家发送的请求的回复(COMMAND_BEREQUST可能会被异步处理，通过此命令判断是否正确的向对方发送请求的逻辑)
	// 命令格式：usgined cmdid, int request_type, widestr targetname, int result(0:失败，1:成功,-1:拒绝接收)
	COMMAND_BEREQUEST_ECHO,
	//再次确认请求，命令格式：usgined cmdid, int request_type, widestr targetname, int result  ...(参数:暂时只有组队用到)
	COMMAND_GET_REQUEST_PARA,
	//再次确认被请求命令格式：usgined cmdid, int request_type, widestr targetname, int result(0:失败，1:成功,-1:拒绝接收)		
	COMMAND_RET_REQUEST_PARA,
	// 队伍相关命令
	// 创建队伍成功
	COMMAND_TEAM_CREATE,
	// 加入队伍
	COMMAND_TEAM_JOIN,
	// 添加成员
	COMMAND_TEAM_ADD_MEMBER,
	// 删除成员
	COMMAND_TEAM_REMOVE_MEMBER,
	// 清除队伍信息
	COMMAND_TEAM_CLEAR,
	// 更新成员信息
	COMMAND_TEAM_UPDATE_MEMBER_INFO,
	// 更新队伍信息
	COMMAND_TEAM_UPDATE_TEAM_INFO,
	// 队伍成员位置或职位改变
	COMMAND_TEAM_MEMBER_CHANGE,
	// 队伍成员上线时候的状态判定
	COMMAND_TEAM_MEMBER_RECOVER,
	// 玩家清除附近队伍
	COMMAND_NEARBYTEAM_CLEAR,
	//队伍成员是否能够参与该玩法的检测
	COMMAND_TEAM_CHECK_CONDITION,

	// 公会相关内部命令: unsigned cmd_id, unsigned sub_cmd, ...
	COMMAND_GUILD_BY_NAME,
	// 帮会升级
	COMMAND_GUILD_UP_LEVEL,
	// 帮会降级
	COMMAND_GUILD_DOWN_LEVEL,
#pragma endregion

#pragma region //ItemModule(301-400)

	// 使用物品
	COMMAND_USE_ITEM = 301,

#pragma endregion

#pragma region //NpcBaseModule(401-500)
	//传送门延迟传送
	COMMAND_NPC_AUTO_TRANSFER = 401,

	//NPC技能硬直状态结束
	COMMAND_NPC_STIFF_OVER = 402,

#pragma endregion

#pragma region //SceneBaseModule(601-700)

#pragma endregion

#pragma region //SkillModule(701-800) 战斗技能相关
	// 被击杀，命令格式：unsigned cmdid(sender:src)
	COMMAND_BEKILL = 701,
	// 击杀目标，命令格式：unsigned cmdid, object target
	COMMAND_KILLTARGET,

	// player使用技能失败
	COMMAND_PLAYER_USESKILL_FAILED,

	// 开始使用某项技能，命令格式：unsigned cmdid, object skill
	COMMAND_SKILL_BEGIN,
	// 技能被打断，命令格式：unsigned cmdid, object skill
	COMMAND_SKILL_BREAKED,
	// 正在使用的技能流程的结束：unsigned cmdid, object skill, object self
	COMMAND_SKILL_FINISH,
	// 使用技能命中前：unsigned cmdid, object skill
	COMMAND_SKILL_BEFORE_HIT,

	// 被使用技能尝试攻击：unsigned cmdid, object target, object skill
	COMMAND_SKILL_TRY_BE_ATTACK,

	// 攻击目标，命令格式：unsigned cmdid, object target
	COMMAND_ATTACKTARGET,
	// 伤害目标之前，命令格式：unsigned cmdid, object target, int damage, int damagetype
	COMMAND_BEFORE_DAMAGETARGET,
	// 被伤害之前，命令格式：unsigned cmdid, int damage, int damagetype(sender:src)
	COMMAND_BEFORE_BEDAMAGE,
	// 伤害目标，命令格式：unsigned cmdid, object target, int damage, int damagetype
	COMMAND_DAMAGETARGET,

	// 玩家请求移动内部命令 命令格式：unsigned cmdid, motion mode
	COMMAND_REQ_MOVE,

	// 被攻击，命令格式：unsigned cmdid, int damage, int damagetype(sender:src)
	COMMAND_BEDAMAGE,
	//修改以前的做法：一个技能发两个COMMAND_BEDAMAGE命令（第一个没有伤害，用于通知被攻击）
	COMMAND_BEATTACK,

	//被暴击
	COMMAND_BEVA,

	//攻击未命中	unsigned cmd_id, object target
	COMMAND_HIT_MISS,
	//暴击			unsigned cmd_id, object target
	COMMAND_SELF_VA,
	//响应客户端请求打断技能 unsigned cmd_id
	COMMAND_ON_CLIENT_BREAK_SKILL,
	// 响应属性变化打断技能 unsigned cmd_id
	COMMAND_CANT_USE_SKILL_BREAK_SKILL,
	/*!
	 * @brief	// 响应被动技能升级人物属性提升
	 * @param	unsigned cmd_id, 消息号
	 * @param	int		技能id
	 * @param	int		影响技能属性的个数n
	 以下循环n次
	 * @param	string  属性名
	 * @param	float	value
	 * @return	void
	 */
	COMMAND_PASSIVE_SKILL_ADD_PROP,

	/*!
	 * @brief	// 响应被动技能移除
	 * @param	unsigned cmd_id, 消息号
	 * @param	int		技能id
	 */
	 COMMAND_PASSIVE_SKILL_REMOVE_PROP,

	 /*!
	 * @brief	// 响应被动技能升级,影响主动技能配置
	 * @param	unsigned cmd_id, 消息号
	 * @param	int 0删除1添加或升级
	 * @param	string	影响主动技能id
	 * @param	string	影响属性
	 */
	 COMMAND_PASSIVE_SKILL_AFFECT_SKILL_CONFIG,
#pragma endregion

#pragma region //TaskModule(801-850)

	/*!
	 * @brief	任务动作消息
	 * @param int 状态（TaskActions）
	 * @param int 任务ID
	 */
	 COMMAND_TASK_ACTION_MSG = 801,

	// 任务相关消息
	COMMAND_TASK_COMMAND,

#pragma endregion

#pragma region //EquipmentModule(851-900)
	// 刷新装备战斗力
	COMMAND_EQUIP_ABILITY = 851,

	// 装备强化等级变化
	COMMAND_EQUIP_STRENGTHEN_CHANGE_LEVEL,

	// 装备附加属性修正包改变
	COMMAND_EQUIPMENT_ADDMODIFY,

	// 更新宝石属性包
	COMMAND_EQUIPMENT_JEWEL_PKG,

	// 装备强化套装属性包
	COMMAND_EQUIP_STR_SUIT_PKG,

	// 装备强化属性包
	COMMAND_EQUIP_STRENGTHEN_PKG,

	// 装备洗练属性包
	COMMAND_EQUIP_BAPTISE_PKG,

	// 外观属性包
	COMMAND_APPERANCE_PKG,

#pragma endregion

#pragma region //OPActivity运营活动(901-951)
#pragma endregion


// 注意：消息ID不能超过1024, 最大允许的ID是1023

};

#endif //__COMMANDDEFINE_H__
