#ifndef PlayerBaseDefine_h__
#define PlayerBaseDefine_h__


//玩家职业
enum
{
	PLAYER_JOB_MIN		= 0,

	PLAYER_JOB_WARRIOR,	// 战士
	PLAYER_JOB_STABBER,	// 刺客
	PLAYER_JOB_ARCHER,	// 弓箭手
	PLAYER_JOB_MAGES,	// 法师
	PLAYER_JOB_DOCTOR,	// 药师

	PLAYER_JOB_MAX,
};

// 检测职业是否合法
inline bool IsValidJob(int job)
{
	return (job > PLAYER_JOB_MIN && job < PLAYER_JOB_MAX);
}

// 性别类型定义
enum SexType
{
	SEX_MAN = 1,
	SEX_WOMAN = 2,
};

// 检测性别是否合法
inline bool IsValidSex(int sex)
{
	return (sex == SEX_MAN || sex == SEX_WOMAN);
}

// 角色类型区别
enum RoleTypeFlag
{
    ROLE_FLAG_NORMAL_PLAYER = 0, // 正常玩家
    ROLE_FLAG_ROBOT         = 1, // 机器人
};

// 玩家全局标志值(值不能超过63)
enum EPlayerGlobalFlag
{
	EM_PLAYER_GLOBAL_FIRST_PAYED,			// 首充标志
	EM_PLAYER_GLOBAL_LIMIT_GIFT_FLAG,		// 限时礼包标志
	EM_PLAYER_GLOBAL_PAYED_REBATE_PICKED,	// 充值返利领取(是否领取过了, 查询返回没有返利会立即设为领取)

	EM_PLAYER_GLOBAL_LIMIT_GIFT_FLAG_1,		// 限时礼包标志1
	EM_PLAYER_GLOBAL_LIMIT_GIFT_FLAG_2,		// 限时礼包标志2
	EM_PLAYER_GLOBAL_LIMIT_GIFT_FLAG_3,		// 限时礼包标志3
	EM_PLAYER_GLOBAL_LIMIT_GIFT_FLAG_4,		// 限时礼包标志4
};


enum EMsgSwitchRole
{
	EMSG_SWITCH_ROLE_REPLY,					// 切换返回 param:result(0:成功 其它值为错误码(暂无))
	EMSG_SWITCH_STATUS_1,					// 成功切换状态1(OnStore发送) param:none
	EMSG_SWITCH_STATUS_2,					// 成功切换状态2(OnLeave发送) param:none
};

enum EmSwitchRoleResult
{
	EM_SWITCH_ROLE_SUCCEED,			// 成功
	EM_SWITCH_ROLE_FAILED,			// 失败
};

enum EmPropQueryResult
{
	EM_PROP_QUERY_RESULT_SUCCEED,			// 成功
	EM_PROP_QUERY_RESULT_OFFLINE,			// 不在线
};
#endif // PlayerBaseDefine_h__
