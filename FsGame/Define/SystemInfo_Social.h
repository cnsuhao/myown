//--------------------------------------------------------------------
// 文件名:	SystemInfo_Sociale.h
// 内 容:	社交系统信息定义
// 说 明:		
// 创建日期:
// 创建人:	tzt    
//--------------------------------------------------------------------
#ifndef _SystemInfo_Social_H_
#define _SystemInfo_Social_H_


// SocialSystemModule 社交系统(17001-19000)【子系统详细划分】
enum SystemInfo_Social
{	
#pragma region // ChatModule聊天（17001-17100）
	SYSTEM_INFO_ID_17001 = 17001,	// 你被禁言了，请反思一会儿
	SYSTEM_INFO_ID_17002,			// 消息发送失败
	SYSTEM_INFO_ID_17003,			// 当前大喇叭数量不足
	SYSTEM_INFO_ID_17004,			// 世界频道发言需要人物等级15级
	SYSTEM_INFO_ID_17005,			// 你还需要{0}秒才能在该频道发言
	SYSTEM_INFO_ID_17006,			// 玩家[{0}]不存在
	SYSTEM_INFO_ID_17007,			// 玩家[{0}]不在线
	SYSTEM_INFO_ID_17008,			// 不能与自己密聊
	SYSTEM_INFO_ID_17009,			// 你没有处于组队中，不能使用该频道。

#pragma endregion	

#pragma region // FriendModule好友（17101-17300）
	SYSTEM_INFO_ID_17101 = 17101,	// 好友请求已经发出
	SYSTEM_INFO_ID_17102,			// {0}已经和你解除好友关系
	SYSTEM_INFO_ID_17103,			// {0}拒绝了你的好友申请
	SYSTEM_INFO_ID_17104,			// 自己好友表满
	SYSTEM_INFO_ID_17105,			// {0}接受了你的好友申请
	SYSTEM_INFO_ID_17106,			// 礼物赠送成功
	SYSTEM_INFO_ID_17107,			// 收到好友{0}的礼物
	SYSTEM_INFO_ID_17108,           // 你与{0}不是好友
	SYSTEM_INFO_ID_17109,			// 你和{0}结义成功
	SYSTEM_INFO_ID_17110,			// 你和{0}结缘成功
	SYSTEM_INFO_ID_17111,			// {0}拒绝了你的结义请求
	SYSTEM_INFO_ID_17112,			// {0}拒绝了你的结缘请求
	SYSTEM_INFO_ID_17113,			// 结义失败
	SYSTEM_INFO_ID_17114,			// 结缘失败
	SYSTEM_INFO_ID_17115,			// 你和{0}解除了结义关系
	SYSTEM_INFO_ID_17116,			// 你和{0}解除了结缘关系
	SYSTEM_INFO_ID_17117,			// 与好友{0}亲密度增加
	SYSTEM_INFO_ID_17118,			// 仇恨列表已满,无法添加仇人
	SYSTEM_INFO_ID_17119,			// 好友礼物不存在
	SYSTEM_INFO_ID_17120,			// 对方在黑名单中
	SYSTEM_INFO_ID_17121,			// 自己黑名单满了
	SYSTEM_INFO_ID_17122,			// 寻求援助成功
	SYSTEM_INFO_ID_17123,			// 你与{0}不是好友
	SYSTEM_INFO_ID_17124,			// 你与{0}的亲密度已达上限，结拜后可继续提升
	SYSTEM_INFO_ID_17125,			// 你与{0}的亲密度已达上限，无法送礼
	SYSTEM_INFO_ID_17126,			// {0}已经是好友
	SYSTEM_INFO_ID_17127,			// {0}在黑名单中

#pragma endregion	
	
#pragma region // RequestModule请求处理模块（17301-17400）
	SYSTEM_INFO_ID_17301 = 17301,	// 玩家{0}不存在或已下线, 请求失败
	SYSTEM_INFO_ID_17302,			// {0}拒绝了你的{1}请求
	SYSTEM_INFO_ID_17303,			// {0}拒绝接受此请求
	SYSTEM_INFO_ID_17304,			// 你对{0}的{1}请求已超时...
	SYSTEM_INFO_ID_17305,			// 请求已失效
	SYSTEM_INFO_ID_17306,			// 已向[{0}]发出了请求信息，等待回应。。。。


#pragma endregion	

#pragma region // TeamModule组队模块（17401-17600）
	SYSTEM_INFO_ID_17401 = 17401,	// {0}的队伍已满，加入队伍失败
	SYSTEM_INFO_ID_17402,			// 当前场景禁止组队
	SYSTEM_INFO_ID_17403,		    // PVP战场中不可以操作队伍
	SYSTEM_INFO_ID_17404,			// {0}不存在或不在线，邀请失败
	SYSTEM_INFO_ID_17405,			// {0}已经是队伍成员，无需发出组队申请
	SYSTEM_INFO_ID_17406,			// 你已经在团队中,不能发出入队申请
	SYSTEM_INFO_ID_17407,			// 不满足等级要求
	SYSTEM_INFO_ID_17408,			// 禁止入队
	SYSTEM_INFO_ID_17409,			// 队伍已满，不能发出组队邀请
	SYSTEM_INFO_ID_17410,			// {0}不在线，组队失败
	SYSTEM_INFO_ID_17411,			// 玩家在特殊场景
	SYSTEM_INFO_ID_17412,			// {0}已经在队伍中，不能发出入队请求
	SYSTEM_INFO_ID_17413,			// {0}已经在其他队伍中，不能发出组队邀请
	SYSTEM_INFO_ID_17414,			// {0}不满足等级需求
	SYSTEM_INFO_ID_17415,			// {0}已经不是队长，请重新申请
	SYSTEM_INFO_ID_17416,			// 对方队伍已满，不能发送入队请求
	SYSTEM_INFO_ID_17417,			// 对方没有队伍，申请失败
	SYSTEM_INFO_ID_17418,			// {0}已经有队伍，组队失败
	SYSTEM_INFO_ID_17419,			// {0}在其他队伍中，加入队伍失败
	SYSTEM_INFO_ID_17420,			// 你没有这个权利
	SYSTEM_INFO_ID_17421,			// {0}不是队友，不能移交
	SYSTEM_INFO_ID_17422,			// {0}不在线，移交失败！
	SYSTEM_INFO_ID_17423,			// 组建队伍成功
	SYSTEM_INFO_ID_17424,			// 加入队伍成功
	SYSTEM_INFO_ID_17425,			// {0}加入队伍
	SYSTEM_INFO_ID_17426,			// {0}离开队伍
	SYSTEM_INFO_ID_17427,			// 队长将{0}从队伍中踢出
	SYSTEM_INFO_ID_17428,			// 队长将你从队伍中踢出
	SYSTEM_INFO_ID_17429,			// 队伍解散
	SYSTEM_INFO_ID_17430,			// 你退出了队伍
	SYSTEM_INFO_ID_17431,			// 你成为队长
	SYSTEM_INFO_ID_17432,			// {0}成为队长
	SYSTEM_INFO_ID_17433,			// 队友{0}掉线
	SYSTEM_INFO_ID_17434,			// 队伍已经在自动匹配中不能更改目标
	SYSTEM_INFO_ID_17435,			// 队长不在线
	SYSTEM_INFO_ID_17436,			// 队伍目标已更改，自动取消匹配
	SYSTEM_INFO_ID_17437,			// 活动未开启

#pragma endregion
#pragma region // GuildModule 公会模块（17601-17800）
	SYSTEM_INFO_ID_17601 = 17601,	// 该帮会人数已达上限
	SYSTEM_INFO_ID_17602,			// 该职位人数已达上限，职位变动失败
	SYSTEM_INFO_ID_17603,			// 帮会名称包含特殊符号或非法字符
	SYSTEM_INFO_ID_17604,			// 此功能{0}级开放
	SYSTEM_INFO_ID_17605,			// 玩家已加入帮会
	SYSTEM_INFO_ID_17606,			// 帮会名称字符超限
	SYSTEM_INFO_ID_17607,			// 元宝不足
	SYSTEM_INFO_ID_17608,			// 帮会数量已达上限
	SYSTEM_INFO_ID_17609,			// 帮会名称重复，无法创建	 
	SYSTEM_INFO_ID_17610,			// 无权审批
	SYSTEM_INFO_ID_17611,			// 无权踢人
	SYSTEM_INFO_ID_17612,			// 踢人过多,不能踢了
	SYSTEM_INFO_ID_17613,			// 修改公告成功
	SYSTEM_INFO_ID_17614,			// 修改公告失败
	SYSTEM_INFO_ID_17615,			// 修改宣言成功
	SYSTEM_INFO_ID_17616,			// 修改宣言失败
	SYSTEM_INFO_ID_17617,			// {0}组织拒绝了你加入公会的申请
	SYSTEM_INFO_ID_17618,			//  组织短名已存在
	SYSTEM_INFO_ID_17619,			//  玩家申请帮会数量已达上限
	SYSTEM_INFO_ID_17620,			//  已经申请过该帮会
	SYSTEM_INFO_ID_17621,			//  该帮会申请列表已满
	SYSTEM_INFO_ID_17622,			//  该帮会人数已达上限
	SYSTEM_INFO_ID_17623,			//  处于退会冷却时间，无法申请或者创建帮会
	SYSTEM_INFO_ID_17624,			//  审批失败，审批数据已过期
	SYSTEM_INFO_ID_17625,			//  所在职位无权发布公告
	SYSTEM_INFO_ID_17626,			//  公告包含非法字符
	SYSTEM_INFO_ID_17627,			//  组织资金不足
	SYSTEM_INFO_ID_17628,			//  宣言包含非法字符
	SYSTEM_INFO_ID_17629,			//  无权任命
	SYSTEM_INFO_ID_17630,			//  商店物品数量不足购买
	SYSTEM_INFO_ID_17631,			//  无权解锁技能
	SYSTEM_INFO_ID_17632,			//  解锁技能所需公会等级不足
	SYSTEM_INFO_ID_17633,			//  帮会帮贡不足
	SYSTEM_INFO_ID_17634,			//  技能已解锁
	SYSTEM_INFO_ID_17635,			//  当前组织没有该技能无法升级
	SYSTEM_INFO_ID_17636,			//  组织技能已到达玩家等级无法升级
	SYSTEM_INFO_ID_17637,			//  你没有这个权限
	SYSTEM_INFO_ID_17638,			//  已经有技能在研究
	SYSTEM_INFO_ID_17639,			//  该帮会人数已达上限
	SYSTEM_INFO_ID_17640,			//  玩家不在线不能邀请进组织
#pragma endregion	


};


#endif