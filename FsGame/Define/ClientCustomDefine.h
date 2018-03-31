// 客户端消息定义
#ifndef _CLIENT_CUSTOM_DEFINE_H_
#define _CLIENT_CUSTOM_DEFINE_H_

enum
{
#pragma region // CommonModule(1-100)
	//发送GM命令，格式：int msgid,
	CLIENT_CUSTOMMSG_GM = 1,

	//Gmcc 指令
	CLIENT_CUSTOMMSG_GMCC_MSG,

	// 与容器背包相关的消息ID, 子消息ID见 ContainerDefine.h
	CLIENT_CUSTOMMSG_CONTAINER,

	/*!
	* @brief	获取设备记录
	* @param	string		clientType
	* @param	string		token
	* @param	wstring		serverName
	* @param	string		AppPackage

	* @param	string		platform 注册平台
	* @param	int			wifi 是否使用wifi 1用 0不用
	* @param	string		manufacturer 制造商
	* @param	string		devid		终端设备唯一标识
	* @param	string		model		终端类型
	* @param	string		os			终端系统
	* @param	string		os_ver		终端系统版本
	*/
	CLIENT_CUSTOMMSG_TOKEN,

	//脱离卡死
	CLIENT_CUSTOMMSG_ESCAPE_LOCK,

	// 与SNS 数据相关的消息
	CLIENT_CUSTOMMSG_SNS_DATA,

	// 与SNS 推送模块数据相关的消息
	CLIENT_CUSTOMMSG_SNS_PUSH_MODULE,

	// 客户端向服务器发送心跳
	CLIENT_CUSTOMMSG_HEART_BEAT,

	//客户端转到后台
	CLIENT_CUSTOMMSG_BACKGROUND,

	// GM命令 参数为多字节
	CLIENT_CUSTOMMSG_GM_MULTIBYTE,

	/*!
	* @brief	CG播放结束
	* @param	string CGID
	*/
	CLIENT_CUSTOMMSG_PLAY_CG_DONE,
#pragma endregion

#pragma region //SystemFunctionModule(101-200)

	// 与排行榜相关的消息的ID, 子消息ID见 RankingDefine.h
	CLIENT_CUSTOMMSG_RANKING = 101,

	//真实掉落，物品拾取消息
	CLIENT_CUSTOMMSG_PICK_UP_DROP_ITEM,

	// 商店管理客户端消息
	CLIENT_CUSTOMMSG_SHOP_MANAGER,

	// 运营活动
	CLIENT_CUSTOMMSG_DYNAMIC_ACTIVITY,

	// 全服排行榜
	CLIENT_CUSTOMMSG_GLOBAL_RANK_LIST,

	/*!
	* @brief	货币兑换
	* @param	int	兑换规则
	* @param	int	消耗高级货币数量
	*/
	CLIENT_CUSTOMMSG_EXCHANGE_CAPITAL,

	// 后台可管理活动
	CLIENT_CUSTOMMSG_DYNAMIC_ACTIVITY_EX,

	// 返回选择人物
	CLIENT_CUSTOMMSG_CHECKED_SELECTROLE,

	//请求服务器时间
	CLIENT_CUSTOMMSG_GET_SERVERTIME,

	//客户端请求设置屏幕可视玩家数量
	CLIENT_CUSTOMMSG_SET_VISUAL_PLAYERS,

	// 根据客户端传入字段获取玩家属性数据 格式: 客户端自定义标识 要查询的玩家(属性列表[属性名 属性名 属性名]) ...
	// 返回: 结果(0成功 1 玩家不在线) 客户自定义标识 玩家名（属性名+类型+值） ....
	CLIENT_CUSTOMMSG_GET_PLAYER_PROP_DATA,

	// 坐骑系统
	CLIENT_CUSTOMMSG_RIDE,

	// 日常活动相关消息
	CLIENT_CUSTOMMSG_DAILY_ACTIVITY,

	// 断线重连后客户端准备就绪，通知客户端
	// (相当于OnReady回调，但是断线重连的时候没有此回调，所以与客户端自定义此消息)
	CLIENT_CUSTOMMSG_CONTINUE_ON_READY,

	//宠物消息，具体操作在二级
	CLIENT_CUSTOMMSG_PET,

	// 交易
	CLIENT_CUSTOMMSG_AUCTION,

	//角色死亡系统
	CLIENT_CUSTOMMSG_DEAD_SYSTEM,

	// 聊天显示物品
	CLIENT_CUSTOMMSG_LOOK_CHAT_ITEM,

	// 死亡复活
	CLIENT_CUSTOMMSG_REVIVE,

	// 翅膀
	CLIENT_CUSTOMMSG_WING,
#pragma endregion


#pragma region //SocialSystemModule(201-700)
	/*!
	* @brief	客户端请求聊天
	* @param	int			channeltype(0私聊，1可视范围，2场景，3国家)
	* @param	int			contenttype (0文字 1语音)
	文字-> @param	wstring		content  (私聊:@param	wstring		targetname)
	语音-> @param	int64		voicemsgid  @param	int		voicemsgtime (私聊:@param	wstring		targetname)
	*/
	CLIENT_CUSTOMMSG_CHAT = 201,

	//好友
	CLIENT_CUSTOMMSG_FRIEND,

	// 客户端请求创建公会 int msg_id,int msg_id(二级消息),对应二级消息参数
	CLIENT_CUSTOMMSG_GUILD,

	// 系统邮件消息
	CLIENT_CUSTOMMSG_SYSTEM_MAIL,

	//查看其他角色的全身装备信息，消息格式：unsigned msgid, wstring name(被请求者角色名称)
	CLIENT_CUSTOMMSG_LOOK_EQUIPS,

	//ReviveManagerModule
	//角色请求复活 格式：string msgid int 0原地复活 1复活点复活,int 几号复活点 1~6[原地复活传0，0：指定复活点复活1，1~6，当传1，0时为就近可以点复活]
	CLIENT_CUSTOMMSG_REQUEST_RELIVE,	

	// 组队客户端消息
	//用户交互请求，消息格式：unsigned msgid, int request_type, wstring name(被请求者角色名称)
	CLIENT_CUSTOMMSG_REQUEST,

	//用户交互请求回应，消息格式：unsigned msgid, int request_type, wstring name(被请求者角色名称), int result(0:拒绝,1:同意,2:超时)
	CLIENT_CUSTOMMSG_REQUEST_ANSWER,

	// 组队客户端消息，消息格式：unsigned msgid
	CLIENT_CUSTOMMSG_TEAM,

	// 请求历史聊天数据
	CLIENT_CUSTOMMSG_REQUEST_CHAT_HISTORY,

	// 查询聊天相关信息消息
	CLIENT_CUSTOMMSG_CHAT_QUERY,
	
#pragma endregion

#pragma region //ItemModule(701-800)
	// 背包相关的消息, 例如使用道具之类
	CLIENT_CUSTOMMSG_TOOL_BOX = 701,

	// 玉珏消息 
	CLIENT_CUSTOMMSG_JADE,

#pragma endregion

#pragma region //NpcBaseModule(801-900)
	//使用道具进入副本
	CLIENT_CUSTOMMSG_ITEM_SCENE = 801,

	// 查询boss数据
	CLIENT_CUSTOMMSG_QUERY_BOSS_INFO,
#pragma endregion

#pragma region //SceneBaseModule(1001-1100)
#pragma endregion

#pragma region //SkillModule(1101-1200)
	/*!
	 * @brief	使用技能
	 * @param	string		技能的id
	 * @param	float sx, sz, dx, dz
	 * @param	PERSISTID	位移的目标
	 * @param	int			开始技能移动效果
	 */
	CLIENT_CUSTOMMSG_USE_SKILL  = 1101,

    // 技能升级
    CLIENT_CUSTOMMSG_SKILL_UPGRADE,

	/*!
	 * @brief	使用技能时,对象产生了位移
	 * @param	int64		技能的uid
	 * @param	string		技能的id
	 * @param	PERSISTID	位移的目标
	 * @param	float x,z,orient
	 */
	CLIENT_CUSTOMMSG_SKILL_LOCATE_OBJECT,

	/*!
	 * @brief	使用技能(客户端处理流程)
	 * @param	string		技能的id
	 * @param	int64		技能的uid
	 * @param	int			当前命中次数索引
	 * @param	int			打击无位移目标数n
	 以下循环n次
	 * @param	PERSISTID	目标对象号
	 * @param	int			打击有位移目标数m
	 以下循环m次
	 * @param	PERSISTID	目标对象号
	 * @param	float x,z,speed
	 * @param   int action 受击动作
	 */
	CLIENT_CUSTOMMSG_USE_SKILL_BY_CLIENT,

	/*!
	 * @brief	打断技能
	 */
	CLIENT_CUSTOMMSG_BREAK_SKILL_FINISH,

	/*!
	* @brief	被动技能升级
	* @param	int			被动技能id
	*/
	CLIENT_CUSTOMMSG_PASSIVE_SKILL_UPGRADE,

	/*!
	* @brief	重置被动技能
	*/
	CLIENT_CUSTOMMSG_RESET_PASSIVE_SKILL,

	/*!
	* @brief	pk战斗一级消息
	*/
	CLIENT_CUSTOMMSG_PK_MODEL,
#pragma endregion

#pragma region // 任务以及引导系统相关(1201-1300)
	// 任务相关消息
	CLIENT_CUSTOMMSG_TASK_MSG = 1201,

#pragma endregion


#pragma region // 支付系统相关(1301-1400)
	// 充值
	CLIENT_CUSTOMMSG_PAY = 1301,
	// 商城
	CLIENT_CUSTOMMSG_SHOP,
    //购买各个功能次数统一处理
    CLIENT_CUSTOMMSG_BUY_INFO,
#pragma endregion

#pragma region //EquipmentModule(1401-1500)
	// 客户端宝石相关消息
	CLIENT_CUSTOMMSG_JEWEL = 1401,

	//客户端强化相关消息
	CLIENT_CUSTOMMSG_STRENGTHEN,

	// 客户端装备熔炼相关消息
	CLIENT_CUSTOMMSG_SMELT,

	// 装备锻造相关的消息
	CLIENT_CUSTOMMSG_FORGING,

	// 装备洗练相关的消息
	CLIENT_CUSTOMMSG_BAPTISE,

	// 时装相关
	CLIENT_CUSTOMMSG_FASHION,

#pragma endregion

#pragma region //OPActivity运营活动(1501-1600)
#pragma endregion

#pragma region //CampaignModule玩法(1601-1800)
	// 比武场消息
	CLIENT_CUSTOMMSG_ARENA_MSG = 1601,

	// 世界BOSS活动相关消息
	CLIENT_CUSTOMMSG_WORLD_BOSS_ACTIVE,

	// 修罗战场活动
	CLIENT_CUSTOMMSG_ASURA_BATTLE,
#pragma endregion
// 消息不能超过2048
};

#endif
