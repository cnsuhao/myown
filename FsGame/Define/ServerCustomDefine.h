// 服务器消息定义
#ifndef _SERVER_CUSTOM_DEFINE_H_
#define _SERVER_CUSTOM_DEFINE_H_

enum
{
#pragma region //CommonModule (1-100)
	// 系统消息，格式：int msgid,  ...(参数表)
	SERVER_CUSTOMMSG_SYSINFO = 1,

	/*!
	* @brief	发出聊天信息
	* @param	int			channeltype(0私聊，1可视范围，2场景，3国家)
	* @param	int			contenttype (0文字 1语音)
	文字-> @param	wstring		content @param	wstring		playername
	语音-> @param	int64		voicemsgid  @param	int		voicemsgtime  @param	wstring		playername 
	*/
	SERVER_CUSTOMMSG_SPEECH,

	//向客户端发送自定义的虚拟表格的内容，消息格式：
	//unsigned msgid, string recname, ...(第一行数据), ..., ...(第n行数据)
	SERVER_CUSTOMMSG_VIRTUALREC_ADD,

	//通知客户端清除自定义的虚拟表格的内容，消息格式：unsigned msgid, string recname
	SERVER_CUSTOMMSG_VIRTUALREC_CLEAR,

	//通知客户端更新自定义的虚拟表格的某行某列的内容，消息格式：
	//unsigned msgid, string recname, int row(行号), int col(列号),(数据)
	SERVER_CUSTOMMSG_VIRTUALREC_UPDATE,

	//通知客户端更新自定义的虚拟表格的某行的内容，消息格式：
	//unsigned msgid, string recname, int row(行号), ...(行数据)
	SERVER_CUSTOMMSG_VIRTUALREC_UPDATE_ROW,

	//通知客户端删除自定义的虚拟表格的某行，消息格式：
	//unsigned msgid, string recname, int row(行号)
	SERVER_CUSTOMMSG_VIRTUALREC_REMOVE_ROW,

	//自定义特效，消息格式：unsigned msgid, object target, ...(由客户端程序确定),如果只有一个对象的行为发生改变，只发送object target 这个ID
	//对于技能或者BUFF，数据统一由客户端读取配置文件。基本格式
	SERVER_CUSTOMMSG_EFFECT,// "effect";

	/*!
	 * @brief	显示效果消息
	 * @param	int	消息触发类型(CustomDisplayType)
	 * @param	PERSISTID	目标对象
	 以下为自定义参数
	 * @param	CVarlist
	 */
	SERVER_CUSTOMMSG_DISPLAY,
    
    //背包容器相关的消息, 子消息ID见 ContainerDefine.h
    SERVER_CUSTOMMSG_CONTAINER,

	//返回延时消息
	SERVER_CUSTOMMSG_DELAY_TIME,
	
	//脱离卡死
	SERVER_CUSTOMMSG_ESCAPE_LOCK,
	//推送设置
	SERVER_CUSTOMMSG_QUERY_PUSH_CONFIG,

    // 与SNS 数据相关的消息
    SERVER_CUSTOMMSG_SNS_DATA,
    
	// 与SNS 推送模块相关的消息
	SERVER_CUSTOMMSG_SNS_PUSH_MODULE,

	// 发出的聊天历史信息 格式: int channel, wstring content, wstring name
	SERVER_CUSTOMMSG_HISTORY_SPEECH,

	// 战场报名相关
	SERVER_CUSTOMMSG_BATTLE_APPLY_MODULE,

	// 关闭断线重连
	SERVER_CUSTOMMSG_CONNECT_CLOSE,

    //客户端转到后台
    SERVER_CUSTOMMSG_BACKGROUND,

	// gmcc消息
	SERVER_CUSTOMMSG_GMCC,

	// 更换角色返回消息
	SERVER_CUSTOMMSG_SWITCH_ROLE,

	// 获取服务器玩家数据消息 args: 结果(0成功 1 玩家不在线) 客户自定义标识 玩家名（属性名+类型+值） ....
	SERVER_CUSTOMMSG_GET_PLAYER_PROP_RSP,

	/*!
	* @brief	播放CG
	* @param	string CGID
	* @param	args 可变参数（需要时扩充）
	*/
	SERVER_CUSTOMMSG_PLYA_CG,

	// 聊天查询结果
	SERVER_CUSTOMMSG_CHAT_QUERY_RST,
#pragma endregion

#pragma region //SystemFunctionModule(101-200)

    // 与排行榜相关的消息的ID, 子消息ID见 RankingDefine.h
    SERVER_CUSTOMMSG_RANKING = 101,

    // 与物品掉落相关的消息
	// @param	int64	死亡对象uid
	// @param	string	特殊掉落，普通掉落为""	["box_drop"宝箱掉落]
	// @param	string	物品id
	// @param	int		物品数量
    SERVER_CUSTOMMSG_ITEM_DROP,

	// 货币兑换成功	 int nType int nNum
	SERVER_CUSTOMMSG_CAPITAL_EXCHANGE_SUC,

	//物品获得
	SERVER_CUSTOMMSG_GET_ITEM,

	// 获取物品提醒
	SERVER_CUSTOMMSG_ITEM_REMIND,

	// 返回系统时间点
	SERVER_CUSTOMMSG_SERVER_TIME,

	// 跟背包相关的消息, 包括物品的使用, 出售等等
	SERVER_CUSTOMMSG_TOOL_BOX,

	// 商店相关消息
	SERVER_CUSTOMMSG_SHOP_INFO,

	// 物品展示
	SERVER_CUSTOMMSG_SHOWOFF_ITEM,

	// 复活
	SERVER_CUSTOMMSG_REVIVE,

	// 翅膀
	SERVER_CUSTOMMSG_WING,

	// 坐骑
	SERVER_CUSTOMMSG_RIDE,
#pragma endregion

#pragma region //SocialSystemModule(301-600)
 	// 公会相关下发消息主消息 string msg_id, int sub_id, ...
 	SERVER_CUSTOMMSG_GUILD = 301,
    
    // 用户邮件
    SERVER_CUSTOMMSG_SYSTEM_MAIL,

	// 切换分组特效消息 args << 0（切进）/1（切出）
	SERVER_CUSTOMMSG_SWITCH_GROUP_EFFECT,

	// 好友功能
	SERVER_CUSTOMMSG_FRIEND,

	//组队功能
	SERVER_CUSTOMMSG_TEAM,
#pragma endregion

#pragma region //ItemModule(601-700)
	// 玉珏系统
	SERVER_CUSTOMMSG_JADE = 601,
#pragma endregion

#pragma region //NpcBaseModule(701-800)
    //场景内BOSS信息
    SERVER_CUSTOMMSG_BOSS_INFO = 701,
#pragma endregion

#pragma region //SceneBaseModule(1001-1100)

#pragma endregion

#pragma region //SkillModule(1101-1200)
	//技能流程消息，消息格式：unsigned msgid, unsigned submsgid, object target, ...
	SERVER_CUSTOMMSG_SKILL  = 1101,// "skill";

	/*!
	 * @brief	请求技能使用中某个对象产生位移的位置
	 * @param	int64 	技能的uid
	 * @param	string	效果id
	 * @param	PERSISTID 技能的目标
	 */
	SERVER_CUSTOMMSG_SKILL_REQUEST_TARGET_POS,

	/*!
	 * @brief	使用技能时,对象产生了位移
	 * @param	int64 	技能的uid
	 * @param	PERSISTID 位移的目标
	 */
	SERVER_CUSTOMMSG_SKILL_LOCATE_OBJECT,

	/*!
	 * @brief	多次随机攻击效果数据
	 * @param	int64 	技能的uid
	 * @param	int 	攻击总次数 
	 * @param	int 	攻击次数n
	 以下循环n次
	 * @param	PERSISTID 攻击目标
	 * @param	int	命中类型
	 * @param	int	伤害值
	 */
	SERVER_CUSTOMMSG_SKILL_RANDOM_MULTI_ATTACK,

	/*!
	 * @brief	NPC死亡,播放击飞效果
	 * @param	PERSISTID 击飞的目标
	 */
	SERVER_CUSTOMMSG_SKILL_DEAD_EFFECT,

	/*!
	 * @brief	客户端命中失败消息,仅为测试使用
	 * @param	wstring 施法者名字
	 * @param	string 技能id
	 * @param	int 失败原因(0 时间验证失败 1命中索引验证失败 2距离验证失败)
	 * @param	float x,z
	 * @param	int 命中目标个数n
	 以下循环n次
	 *  @param	PERSISTID 目标
	 *  @param	float x,z
	 */
	SERVER_CUSTOMMSG_SKILL_HIT_FAILED,

	/// 技能升级, , 子消息ID见 SkillDefine.h
	SERVER_CUSTOMMSG_SKILL_UPGRADE,

	// PK模式
	SERVER_CUSTOMMSG_PK_MODEL,
#pragma endregion

#pragma region //TaskModule(1301-1400)
	// 任务系统主消息 string msg， int subType.....
	SERVER_CUSTOMMSG_TASK_MSG = 1301,
#pragma endregion

#pragma region //CampaignModule(1401-1500)

    // 竞技场主消息
    SERVER_CUSTOMMSG_ARENA_MSG = 1401,

	// 世界BOSS活动相关
	SERVER_CUSTOMMSG_WORLD_BOSS_ACTIVE,

	// 修罗战场逻辑消息
	SERVER_CUSTOMMSG_ASURA_BATTLE,

	// 组队悬赏消息
	SERVER_CUSTOMMSG_TEAMOFFER,

#pragma endregion

#pragma region //PayModule(1501-1600)
	// 充值
	SERVER_CUSTOMMSG_PAY = 1501,
	// 商城
	SERVER_CUSTOMMSG_SHOP,
    //VIP购买各个功能次数统一处理 [消息头][二级消息][金额][已购买次数][可购买总次数]
    SERVER_CUSTOMMSG_BUY_INFO,
#pragma endregion

#pragma region //EquipmentModule(1601-1650)
	// 宝石相关消息
	SERVER_CUSTOMMSG_JEWEL = 1601,

	// 装备强化成功消息
	SERVER_CUSTOMMSG_STRENTHEN,

	// 装备熔炼消息
	SERVER_CUSTOMMSG_SMELT,

	// 装备锻造
	SERVER_CUSTOMMSG_FORGING,

	// 时装消息
	SERVER_CUSTOMMSG_FASHION,
#pragma endregion

#pragma region //OPActivity运营活动(1651-1700)
#pragma endregion
};

#endif