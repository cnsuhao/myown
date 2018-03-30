//--------------------------------------------------------------------
// 文件名:		GuildMsgDefine.h
// 内  容:		公会系统-消息定义
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __GuildMsgDefine_H__
#define __GuildMsgDefine_H__

// 客户端向服务器发送的消息
enum CSGuildMsg
{
	CS_GUILD_CREATE            	= 0,  				// 创建公会 wstring公会名
	CS_GUILD_GUILD_LIST        	= 1,  				// 请求公会列表 int 第几页0(0首页)...
    CS_GUILD_SEARCH            	= 2,  				// 搜索公会 wstring公会名
	CS_GUILD_APPLY_JOIN        	= 3,  				// 请求加入公会 wstring公会名
    CS_GUILD_CANCEL_APPLY_JOIN 	= 4,  				// 请求取消加入公会申请 wstring公会名
    CS_GUILD_ACCEPT_APPLY		= 5,  				// 接受加入公会的申请 wstring玩家姓名
    CS_GUILD_REFUSE_APPLY		= 6,  				// 拒绝加入公会的申请 wstring玩家姓名
    CS_GUILD_GUILD_INFO			= 7,  				// 请求公会信息
    CS_GUILD_MEMBER_LIST		= 8,  				// 请求成员列表
    CS_GUILD_APPLY_LIST			= 9,  				// 请求申请者列表
    CS_GUILD_NOTICE            	= 10, 				// 请求修改公告 wstring公告
    CS_GUILD_QUIT              	= 11, 				// 请求退出公会
    CS_GUILD_FIRE              	= 12, 				// 请求踢出公会 wstring玩家名
    CS_GUILD_DISMISS           	= 13, 				// 请求解散公会
    CS_GUILD_DONATE            	= 14, 				// 请求捐献 int捐献id
    CS_GUILD_PROMOTION         	= 15, 				// 请求升职 wstring玩家名
    CS_GUILD_DEMOTION          	= 16, 				// 请求降职 wstring玩家名
    CS_GUILD_SHOP_LIST         	= 17,				// 请求公会商店列表
    CS_GUILD_BUY_ITEM          	= 18, 				// 请求购买商店商品 wstring道具号, int 数量
    CS_GUILD_CAPTAIN_TRANSFER  	= 19,				// 帮主移交 wstring玩家名
    CS_GUILD_JOIN_CD           	= 20, 				// 请求加入公会CD时间
	CS_GUILD_IS_IN             	= 21, 				// 请求玩家是否在公会中（查看自已是否在公会中）
	CS_GUILD_LOG				= 22,				// 请求帮会日志	int 标记 (上一页数据携带的值, 第一次请求给-1)
	CS_GUILD_LOG_DONATE			= 23,				// 请求捐献日志 无参数
	CS_GUILD_DECLARATION      	= 24, 				// 请求修改宣言 wstring宣言
	//其他的见 GUILD_IMPEACH_CLIENT_MSG
	CS_GUILD_ONE_KEY_APPLY_JOIN = 31,				// 一键请求公会
	CS_GUILD_AUTO_AGREE			= 32,				// 自动加入公会
	CS_GUILD_UNLOCK_SKILL		= 33,				// 解锁公会技能
	CS_GUILD_GET_SKILL_LIST		= 34,				// 获取公会技能列表
	CS_GUILD_SKILL_LEVEL_UP		= 35,				// 技能升级
	CS_GUILD_BUILDING_OPERATE_LV  = 42,				// 建筑升级操作 int 建筑类型 int 操作 0 升级，1暂停
	CS_GUILD_NUM_BUY            = 43,				// 组织数值购买 int   数值类型
	CS_GUILD_CHANGE_SHORT_NAME	= 44,				// 修改组织简称 wstring 组织名
	CS_GUILD_GET_JIAN_KANG_DU_GIF = 45,			    // 健康度礼包
	CS_GUILD_GET_GUILD_NUM		= 46,				// 请求组织数值 wstring 组织名
	CS_GUILD_GET_GUILD_SYSMBOL  = 47,				// 请求组织简称 wstring 组织名
	CS_GUILD_GET_GUILD_BUILDING = 48,				// 组织建筑信息
	CS_GUILD_GET_GUILD_SET_INFO = 49,				// 组织设置信息
	CS_GUILD_START_OPERATE_SKILL	= 50,			 //组织研究技能操作 string 技能id int 操作 0 升级，1暂停 
	CS_GUILD_USE_RARE_TREASURE     = 51,			 // 使用秘宝 int 秘宝类型
	CS_GUILD_GET_RARE_TREASURE_LIST     = 52,		 // 使用秘宝列表
	CS_GUILD_GET_NUM_BUY_RECORD = 53,                //获得组织数值购买记录
	CS_GUILD_GET_BONUS_INFO		= 54,				// 获取分红信息
	CS_GUILD_GET_WELFARE_INFO       = 55,				// 获取福利信息
	CS_GUILD_GIVE_OUT_BONUS		= 56,				 //组织分红 int 分红类型，int 每个人分多少，
	CS_GUILD_INVITE_JOIN_GUILD  = 57,                // 邀请加入组织 wstring 玩家名
	CS_GUILD_INVITE_JOIN_GUILD_RSP = 58,             // 同意邀请加入组织
	CS_GUILD_ADD_PETITION = 59,						 // 添加请愿 int 类型
	CS_GUILD_GET_PETITION_LIST = 60,				// 获取请愿 int 类型 (-1 表示所有)
	CS_GUILD_GET_OTHER_GUILD_INFO		= 61,       // 获得其他组织信息

	CS_GUILD_TEACH_LIST = 62,			// 请求传功列表 args: none
	CS_GUILD_TEACH_REQ = 63,			// 请求对方传功 args: nick[widestr] type[1:req 2:push]
	CS_GUILD_TEACH_ACK = 64,			// 回应给对方请求 args: nick[widestr] type[1:req 2:push] agree[EmTeachAckResult]
	CS_GUILD_TEACH_READY = 65,			// 客户端告诉服务器准备好了 args: none

};

// 场景服内部命令消息
enum SSGuildMsg
{
	SS_GUILD_CREATE_RESULT			= 0,        	// 公会创建结果
	SS_GUILD_APPLY_JOIN_RESULT		= 1,     		// 申请加入公会的结果
	SS_GUILD_CANCEL_APPLY_RESULT	= 2,     		// 取消申请加入公会的结果wstring公会名，int 0失败1成功
	SS_GUILD_ACCEPT_RESULT		    = 3,     		// 通知审批者接受申请的结果 int 0失败1成功，wstring 申请人，wstring公会名
	SS_GUILD_ACCEPT_JOIN_RESULT		= 4,     		// 通知申请者加入公会结果
	SS_GUILD_REFUSE_JOIN           	= 5,    		// 通知审批者拒绝加入公会的申请
	SS_GUILD_REFUSE_JOIN_ALL		= 6,     		// 通知审批者拒绝全部加入公会的申请
	SS_GUILD_NOTICE_RESULT			= 7,    		// 通知修改公告结果
	SS_GUILD_QUIT_RESULT			= 8,    		// 退出公会结果
	SS_GUILD_FIRE_RESULT			= 9,    		// 踢出公会结果
	SS_GUILD_DISMISS_RESULT			= 10,     		// 解散公会结果
	SS_GUILD_LEAVE					= 11,    		// 通知玩家离开了公会
    SS_GUILD_DONATE_RESULT			= 12,    		// 捐献结果
	SS_GUILD_MODIFY_DEVOTE			= 13,    		// 通知玩家修改贡献度
	SS_GUILD_BUY_ITEM_RESULT		= 14,    		// 通知玩家珍宝阁购买商品扣除贡献值结果
	SS_GUILD_BUY_GEM_ITEM_RESULT	= 15,    		// 通知玩家珍宝阁购买商品扣除个人属性值值结果
    SS_GUILD_PROMOTION_RESULT		= 16,    		// 请求升职
	SS_GUILD_BE_PROMOTION			= 17,			// 职位晋升
    SS_GUILD_DEMOTION_RESULT		= 18,    		// 请求降职
	SS_GUILD_BE_DEMOTION			= 19,			// 职位下降
    SS_GUILD_CAPTAIN_TRANSFER		= 20,    		// 帮主移交
	SS_GUILD_CAPTAIN_BE_TRANSFER	= 21,    		// 成为帮主
	SS_GUILD_DECLARATION_RESULT		= 22,    		// 通知修改宣言结果
	SS_GUILD_NEW_APPLAY				= 23,    		// 通知有新的帮会申请
	SS_GUILD_SKILL_UNLOCK			= 25,			// 技能解锁
	SS_GUILD_REFRESH_GUILD_SYSMBOL   = 28,			// 刷新组织短名
	SS_GUILD_ADD_GUILD_BUILDING_EXP = 30,			// 增加组织建筑经验
	SS_GUILD_INVITE_JOIN_GUILD      = 31,          //  邀请加入组织

	SS_GUILD_TEACH_REQUEST = 32,				// 传功请求命令 type[请求类型] sender[widestr 发起者昵称]
	SS_GUILD_TEACH_ACK = 33,					// 传功回应命令 result[int] type[请求类型] responser[widestr 回应者昵称]
	SS_GUILD_TEACH_FIN = 34,					// 传功完成命令 result[int] type[请求类型] sender[widestr 发起者昵称]
	SS_GUILD_TEACH_ENT_FAILED = 35,				// 传功进入失败 type[请求类型] sender[widestr 发起者昵称]
};
									
// 服务器向客户端发送的消息
enum SCGuildMsg
{
	SC_GUILD_CREATE_RESULT		= 0,                // 公会创建结果 int 0失败1成功，wstring公会名
    SC_GUILD_SEARCH_RESULT		= 1,                // 公会搜索结果 int行数 array帮会信息列
	SC_GUILD_APPLY_RESULT		= 2,                // 申请加入公会结果 int 0失败1成功，wstring公会名
	SC_GUILD_CANCEL_APPLY		= 3,                // 取消申请加入公会的结果 int 0失败1成功，wstring公会名
    SC_GUILD_ACCEPT_RESULT		= 4,                // 接受申请的结果 int 0失败1成功，int职位, wstring申请者名称, int申请者性别, int申请者等级, 
													// int申请者职业, int当前贡献度, int最大贡献度, int战斗力, int是否在线, int玩家离线时间 
    SC_GUILD_REFUSE_RESULT		= 5,                // 拒绝加入公会的申请 int 0失败1成功 wstring申请者名称
    SC_GUILD_GUILD_INFO			= 6,		        // 公会信息
	SC_GUILD_MEMBER_LIST		= 7,		        // 成员列表 int 类型 ，信息列(参看公会成员列表行）
    SC_GUILD_APPLY_LIST			= 8,		        // 申请者列表 int 类型 ，信息列(参看公会成员列表行）
    SC_GUILD_NOTICE_RESULT		= 9,                // 修改公告结果 int 0失败1成功 wstring公告内容
    SC_GUILD_QUIT_RESULT		= 10,               // 退出公会结果 int 0失败1成功
    SC_GUILD_FIRE_RESULT		= 11,               // 踢出公会结果 int 0失败1成功，wstring踢出公会玩家名
    SC_GUILD_DISMISS_RESULT		= 12,               // 解散公会结果 int 0失败1成功
    SC_GUILD_LEAVE_GUILD		= 13,               // 离开公会 wstring公会名
    SC_GUILD_DONATE_RESULT		= 14,               // 公会捐献结果 int 0失败1成功 int公会等级 int公会经验值 int自身当前贡献度 int自身最大贡献度
    SC_GUILD_PROMOTION			= 15,               // 公会升职结果 int 0失败1成功 wstring玩家名 int职位
    SC_GUILD_DEMOTION			= 16,               // 公会降职结果 int 0失败1成功 wstring玩家名 int职位
    SC_GUILD_SHOP_LIST			= 17,               // 公会商店列表
    SC_GUILD_BUY_ITEM_RESULT	= 18,				// 购买商品结果 int 0失败1成功，wstring商品名
	SC_GUILD_CAPTAIN_TRANSFER	= 19,				// 帮主移交结果 int 0失败1成功, wstring新帮主名字，int自己的当前职位
	SC_GUILD_JOIN_CD			= 20,               // 加入公会CD时间 int剩余CD分钟
	SC_GUILD_IS_IN				= 21,               // 玩家是否在公会中 int 0不在1在 wstring 公会名字
	SC_GUILD_LOG				= 22,				// 下发帮会日志 int 标记(下一页请求时返回) int 循环次数 {int 日志类型 int64 时间戳 wstring 参数,以逗号分隔}
	SC_GUILD_LOG_DONATE			= 23,				// 下发帮会捐献日志 int 循环次数 {int 日志类型 int64 时间戳 wstring 参数,以逗号分隔}
	SC_GUILD_DECLARATION_RESULT	= 24,               // 修改宣言结果 int 0失败1成功 wstring内容
	SC_GUILD_IS_DONATED      	= 25, 				// 是否已经捐献过, int 0没有1捐献过
	SC_GUILD_NEW_APPLY      	= 26, 				// 有新的申请 ,int 0 隐藏 1是显示
	SC_GUILD_BOSS_DONATE		= 28,				// boss捐献 ,int 0 隐藏 1是显示
	SC_GUILD_SKILL_LIST			= 29,				// 公会技能列表 string 列表
	SC_GUILD_NUM_VALUE			= 30,			    // 公会当前数值	int 数值类型  int 当前值  
	SS_GUILD_ISSUE_MUTUAL_LIST	= 32,			   //  互助列表
	SS_GUILD_ISSUE_MUTUAL_RANK	= 33,			   //  互助排行 
	SS_GUILD_TOMOROW_SHOP_LIST  = 34,			   //  明日商店的物品列表 int 数量 string 物品id
	SS_GUILD_BUILDING_LEVEL_UP  = 35,			   //  建筑升级 int 结果
	SS_GUILD_NUM_BUY		    = 36,			   //  组织数值购买	int 数值类型 int 结果 int当前值
	SS_GUILD_CHANGE_GUILD_SYSMBOL     = 37,			// 组织标识修改 int 0 失败，1成功
	SC_GUILD_GUILD_NUM_INFO     = 38,				//组织数值请求 // [int 数值类型 int 可使用资金 int 锁定资金],[...]
	SC_GUILD_GUILD_SYSMBOL      = 39,               //组织标识信息 int 组织标识 wstring 组织简称
	SC_GUILD_BUILDING_INFO      = 40,				//组织建筑信息 [int 建筑类型, int 建筑等级 ,int 建筑当前经验, int 建设状态],[...]
	SC_GUILD_SET_INFO			= 41,				//组织设置信息 见[GUILD_SET_REC_COL]
	SC_GUILD_JIAN_KANG_DU_GIT   = 42,				//健康度奖励[itemid:num,]
	SC_GUILD_SKILL_OPERATE_RET  = 43,               // 技能操作结果 int[结果] int[技能id] int[技能等级]
	SC_GUILD_USE_GUILD_RARE_TREASURE     = 44,		// 使用秘宝 int 结果 
	SC_GUILD_RARE_TREASURE_LIST = 45,               // 秘宝列表 int 参数数量 [int 类型 int 数量]
	SC_GUILD_NUM_BUY_RECORD    = 46,               // 组织数值购买记录 int参数数量 [int 类型 int 数量] 

	SC_GUILD_CREATED_BROADCAST = 47,				// 创建公会消息广播 args: 创建人[widestr] 创建人vip[int]
	SC_GUILD_MEMBER_BEKILL_BROADCAST = 48,			// 组织成员被杀广播 args: 组织玩家[widestr] vip[int] 杀人玩家[widestr] vip[int] 坐标[场景id(int), x(float) y(float) z(float)]
	SC_GUILD_MEMBER_JOIN_BROADCAST = 49,			// 组织成员加入广播 args: 组织玩家[widestr] vip[int]

	SC_GUILD_BONUS_INFO = 50,						 //组织分红信息 int 分红的资金 int锁定资金 int 数量 【玩家名,分配次数】
	SC_GUILD_WELFARE_INFO= 51,						 //组织福利信息 int 是否已领取礼包 （0未领取，1已领取） int 已获取组织资金，int 已获取个人资金
	SC_GUILD_GIVE_OUT_BONUS = 52,					 //组织分红成功
	SC_GUILD_INVITE_JOIN_GUILD = 53,				 //邀请加入组织 玩家名 组织名
	SC_GUILD_GET_PETTION_LIST = 54,					 //请愿列表 int 数量 玩家名
	SC_GUILD_OTHER_GUILD_INFO = 55,					 //其它组织信息int 繁荣度 int 组织资金 int 占领城池id（0为未占领任何城池） int 组织黑夜城插旗数量 [int flagType,int flagID] int 建筑数量[建筑信息]

	SC_GUILD_BUILDING_LVUP_BROADCAST = 56,			// 建筑升级广播 args: 建筑id[int] Level[int]
	SC_GUILD_DECLARATION_BROADCAST = 57,            // 修改宣言广播 args: wstring内容

	SC_GUILD_TEACH_LIST = 58,           // 传功列表 args: 传功能列表[ 用户数[int] [uid nick sex job level power].... ] 被传功列表[ 用户数[int] [uid nick sex job level power].... ]
	SC_GUILD_TEACH_TARGET_REQ = 59,     // 对方发起请求 args: nick[widestr] type[1:req 2:push]
	SC_GUILD_TEACH_RSP_REQUEST = 60,	// 回应主动方(包括请求和发起的主动行为) args: result[0 成功， 非0错误码] type[1:req 2:push] nick[widestr]
	SC_GUILD_TEACH_RSP_PASSIVE = 61,	// 回应被动方(包括请求和发起的被动接受行为) args: result[0 成功， 非0错误码] type[1:req 2:push] nick[widestr]
	SC_GUILD_TEACH_PREPARE_PUSH = 62,	// 服务器广传功开始 args:  target[nick sex job level teachtype]
	SC_GUILD_TEACH_PLAY_CG = 63,		// 服务器广播传功CG args: cgid[int] sender[widestr] reciver[widestr] 
	SC_GUILD_TEACH_COUNT = 64,			// 传功次数 args: sendtime[int] gettime[int]
};





#endif //__GuildMsgDefine_H__