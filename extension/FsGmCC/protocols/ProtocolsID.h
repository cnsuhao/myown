//--------------------------------------------------------------------
// 文件名:		ProtocolKey.h
// 内  容:		Gmcc World协议key定义
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_PROTOCOL_KEY_H__
#define __NET_PROTOCOL_KEY_H__
#include "utils/enum.h"

enum
{
	PROTOCOL_ID_CONFIRM_ACK = 127,	 // 游戏确认收到消息	GAME->EXT 
	PROTOCOL_ID_GAME_READY	= 128,	// 游戏服务器OK		GAME->EXT
	PROTOCOL_ID_GAME_CHECK_READY,	// gmcc请求游戏ok状态	EXT->GAME
	PROTOCOL_ID_GAME_REQ_SRV_INFO,	// 游戏服务器请求初始化服务信息		GAME->EXT
	RPOTOCOL_ID_GAME_INIT_INFO,		// gmcc初始化完成		EXT->GAME

	PROTOCOL_ID_GM_EXEC,			// 执行gm		EXT->GAME
	PROTOCOL_ID_GM_REPLY,			// 执行gm回应		GAME->EXT

	PROTOCOL_ID_MAIL_POST,			// 执行发送邮件	EXT->GAME
	PROTOCOL_ID_MAIL_RESULT,		// 执行发送邮件结果GAME->EXT

	PROTOCOL_ID_NOTICE_SYNC,		// 同步所有公告数据(通过PROTOCOL_ID_NOTICE_POST返回） GAME->EXT
	PROTOCOL_ID_NOTICE_DELETE,		// 删除公告		EXT->GAME
	PROTOCOL_ID_NOTICE_POST,		// 执行发送公告	EXT->GAME
	PROTOCOL_ID_NOTICE_RESULT,		// 执行发送公告结果GAME->EXT 

	PROTOCOL_ID_GIFT_EXCH,			// 兑换礼品		GAME->EXT
	PROTOCOL_ID_GIFT_RESULT,		// 礼品结果		EXT->GAME

	PROTOCOL_ID_ROLE_PROP_QUERY,	// 角色属性查询	EXT->GAME
	PROTOCOL_ID_ROLE_PROP_DATA,		// 角色属性查询返回	GAME->EXT

	PROTOCOL_ID_ROLE_RECORD_QUERY,	// 角色表数据查询		EXT->GAME
	PROTOCOL_ID_ROLE_RECORD_DATA,	// 角色表数据查询返回	GAME->EXT

	PROTOCOL_ID_PUSH_NOTIFICATION,	// 推送通知服务		GAME->EXT

	PROTOCOL_ID_PUSH_SHOP_ITEM,		// 推送礼包商城商品		EXT->GAME
	PROTOCOL_ID_DEL_SHOP_ITEM,		// 删除商城商品		EXT->GAME
	PROTOCOL_ID_QUERY_SHOP_ITEM,	// 查询礼包商城商品		EXT->GAME
	PROTOCOL_ID_SHOP_ITEM_LIST,		// 查询礼包返回商品		GAME->EXT

	PROTOCOL_ID_SET_ACTIVITY_CONFIG,// 设置活动配置表		EXT->GAME 具体类型见EmActivityConfigType

	PROTOCOL_ID_REPORT_MEMBER_INFO,	// 上报服务器状态		GAME->EXT

	PROTOCOL_ID_LOAD_CONFIG,		// 加载活动配置表		EXT->GAME 具体类型见EmActivityConfigType

	PROTOCOL_ID_PRODUCTS_PSH,				// 商品列表推送	EXT->GAME
	PROTOCOL_ID_PAYMENT_PSH,				// 支付通知		EXT->GAME
	PROTOCOL_ID_ORDER_REQ,					// 支付下单		GAME->EXT
	PROTOCOL_ID_ORDER_INFO,					// 支付下单回应	EXT->GAME
	PROTOCOL_ID_ORDER_NOTIFY,				// 订单确定通知	GAME->EXT

	PROTOCOL_ID_REBATE_QUERY,				// 查询充值返利	GAME->EXT
	PROTOCOL_ID_REBATE_INFO,				// 返回充值返利信息	EXT->GAME
	PROTOCOL_ID_REBATE_PICK,				// 领取过充值返利通知	GAME->EXT
	PROTOCOL_ID_REBATE_RESULT,				// 领取过充值返利结过	EXT->GAME
};

enum EmHttpRequest
{
	PROTOCOL_HTTP_PROFILE = 1,			// 请求profile
	PROTOCOL_HTTP_NOTICE,				// 请求notice
	PROTOCOL_HTTP_GIFT,					// 请求gift
	PROTOCOL_HTTP_PUSH_NOTIFICATION,	// 推送通知
	PROTOCOL_HTTP_CONFIG_ACTIVITY,		// 活动配置请求
	PROTOCOL_HTTP_MALL_ITEMS,			// 运营商城列表
	PROTOCOL_HTTP_MAIL_RESULT,			// 邮件结果
	PROTOCOL_HTTP_GMCMD_RESULT,			// gm结果
	PROTOCOL_HTTP_PRODUCTS,				// 商品列表
	PROTOCOL_HTTP_ORDER_STATE,			// 订单通知
	PROTOCOL_HTTP_ORDER_PREPARE,		// 支付下单
	PROTOCOL_HTTP_REBATE_INFO,			// 充值返利信息
	PROTOCOL_HTTP_REBATE_PICK,			// 发放返利请求
};

// 返利请求结果
enum EmRebateResult
{
	EM_REBATE_RESULT_SUCCEED,			// 成功
	EM_REBATE_RESULT_PICKED,			// 已领取
	EM_REBATE_RESULT_ERROR,				// 发生错误
};

enum EmPayedResult
{
	PAYED_RESULT_SUCCED,				// 游戏服务器接收成功
	PAYED_RESULT_RECVED,				// 订单重复
	PAYED_RESULT_DATA_ERR,				// 数据错
};

enum EmDataQueryResult
{
	DATA_QUERY_SUCCESS,				// 成功
	DATA_QUERY_ROLEID_ERR,			// 角色id不正确
	DATA_QUERY_ROLEIN_OFFLINE,		// 角色不在线
	DATA_QUERY_KEY_ERROR,			// 属性或表名错误
	DATA_QUERY_SERVER_DISCONNECT,	// 服务器断开
};

// 活动类型
enum EmActivityConfigType
{
	EM_ACT_TYPE_NONE = 0,			// 无类型
	EM_ACT_TYPE_LOGIN_DAY_TOTAL = 1,			// 累积登录天
	EM_ACT_TYPE_ON_LINE_TIME = 2,			// 在线活动奖励
	EM_ACT_TYPE_PLAYER_LEVEL = 3,			// 玩家等级活动	
	EM_ACT_TYPE_KILL_PLAYER = 4,			// 玩家击杀活动
	EM_ACT_TYPE_BATTLE_RANK = 5,			// 战斗力排行榜
	EM_ACT_TYPE_BATTLE = 6,			// 战斗力达成
	EM_ACT_TYPE_PAY_DAILY = 7,			// 每日充值
	EM_ACT_TYPE_PAY_TOTAL = 8,			// 累积充值
	EM_ACT_TYPE_PLAYER_LEVEL_MATCH = 9,            // 冲级比赛	
	EM_ACT_TYPE_RIDE_STEP_LV_UP = 10,           // 坐骑升阶活动
	EM_ACT_TYPE_MERIDIAN_LV_UP = 11,           // 经脉升级活动
	EM_ACT_TYPE_JEWEL_INLAY = 12,           // 宝石镶嵌活动
	EM_ACT_TYPE_RIDE_STEP_SHOP = 13,           // 坐骑升阶商店活动
	EM_ACT_TYPE_MERIDIAN_SHOP = 14,           // 经脉升级商店活动
	EM_ACT_TYPE_JEWEL_SHOP = 15,           // 宝石镶嵌商店活动
	EM_ACT_TYPE_WISH = 16,			// 祈福功能

	EM_ACT_TYPE_PRINCESS_BLESS = 30,			// 九公主的祝福

	EM_ACT_TYPE_TOTAL,
};

// 配置分类
enum EmConfigCatalog
{
	EM_CONFIG_CATALOG_INVALID = 0,		// 无效配置id
	
	EM_CONFIG_CATALOG_ACTIVITY = 1,		// 通用活动配置
	EM_CONFIG_CATALOG_MALL = 2,			// 运营商城配置
	EM_CONFIG_CATALOG_FIRST_PAY = 3,	// 首充配置
	EM_CONFIG_CATALOG_LEVELING = 4,		// 冲级配置

	EM_CONFIG_CATALOG_END = 1024,
};

#define ERROR_CODE_SUCCESS	"10000"
#define ERROR_CODE_CONFIRM_PAY_TIMEOUT	"102022"	// 支付确认超时重发


// 礼品兑换错误码
#define ERROR_CODE_GIFT_INVALID		"11001"		// 礼品码不正确
#define ERROR_CODE_GIFT_TAKED		"11002"		// 礼品码已领取
#define ERROR_CODE_GIFT_EXPIRED		"11003"		// 礼品码已过期
#define ERROR_CODE_GIFT_LIMIT		"11004"		// 礼品码领取次数限制

enum EmGiftCode
{
	EM_GIFT_CODE_SUCCESS,	
	EM_GIFT_CODE_INVLID,			// 礼品码不正确
	EM_GIFT_CODE_TAKED,				// 礼品码已领取
	EM_GIFT_CODE_EXPIRED,			// 礼品码已过期
	EM_GIFT_CODE_LIMIT,				// 礼品码领取次数限制

	EM_GIFT_CODE_UNKOWN,			// 发生未定义错误
};

inline int GetGiftErrorCode(const char* pszCode)
{
	static const char* szArrCode[] = {
		ERROR_CODE_SUCCESS,
		ERROR_CODE_GIFT_INVALID,				// 礼品码不正确
		ERROR_CODE_GIFT_TAKED,					// 礼品码已领取
		ERROR_CODE_GIFT_EXPIRED,				// 礼品码已过期
		ERROR_CODE_GIFT_LIMIT,					// 礼品码领取次数限制
	};

	static const int nCount = sizeof(szArrCode) / sizeof(szArrCode[0]);
	for (int i = 0; i < nCount; ++i)
	{
		if (strcmp(pszCode, szArrCode[i]) == 0)
		{
			return EM_GIFT_CODE_SUCCESS + i;
		}
	}

	return EM_GIFT_CODE_UNKOWN;
}

// GMCC公告类型
enum EmNoticeType
{
	EM_NOTICE_TYPE_TOP = 1,			// 顶部走马灯公告
	EM_NOTICE_INDEX,				// 首页公告
	EM_NOTICE_TYPE_CENTER,			// 屏幕中间公告
	EM_NOTICE_TYPE_HORN,			// 小啦叭公告
};

// 游戏服务器返回确认结果
enum EmGmccResult
{
	EM_GMCC_RESULT_SUCCEED = 1,		// 成功
	EM_GMCC_RESULT_NO_PLAYER,		// 没找到玩家
	EM_GMCC_RESULT_PLAYER_OFFLINE,	// 玩家不在线
	EM_GMCC_RESULT_COMMAND_ERROR,	// 命令错误

	EM_GMCC_RESULT_ERROR = 10000,	// 其它错误
};

enum EmBillType
{
	EM_BILL_TYPE_VIRTUAL = 0,	// 虚拟币充值代码  VIRTUAL_CURRENCY("V"),
	EM_BILL_TYPE_REAL,			// 真实充值代码	 THIRD_CURRENCY("T")

	EM_BILL_TYPE_MAX_END,		// 
};

ENUM_TO_STRING_EX(EmBillType, EM_BILL_TYPE_MAX_END,
	ENUM_ITEM_STR(V)
	ENUM_ITEM_STR(T)
);

enum EmOrderState
{
	EM_ORDER_STATE_UNKOWN = 0,				// 错误状态
	EM_ORDER_STATE_INIT,					// 已创建 INIT("0"),
	EM_ORDER_STATE_PAYED,					// 已支付 PAY("P"),
	EM_ORDER_STATE_ARRIVED,					// 游戏服务器已经收到 ARRIVED("A")
	EM_ORDER_STATE_TAKE,					// 已领取TAKE("T")
	EM_ORDER_STATE_FINISH,					// 已完成_FINISH("F")

	EM_ORDER_STATE_MAX_END,					// 
};

ENUM_TO_STRING_EX(EmOrderState, EM_ORDER_STATE_MAX_END,
	ENUM_ITEM_STR(U)
	ENUM_ITEM_STR(0)
	ENUM_ITEM_STR(P)
	ENUM_ITEM_STR(A)
	ENUM_ITEM_STR(T)
	ENUM_ITEM_STR(F)
	);

enum EmProductRule
{
	EM_PRODUCT_RULE_NORMAL,				// 正常购买
	EM_PRODUCT_RULE_DAILY,				// 每天DAILY("D")
	EM_PRODUCT_RULE_WEEK,				// 每周WEEK("W")
	EM_PRODUCT_RULE_MONTH,				// 每月MONTH("M")
	EM_PRODUCT_RULE_YEAR,				// 每年MONTH("Y")
	EM_PRODUCT_RULE_ONCE,				// 只能买一次ONCE("F")

	EM_PRODUCT_RULE_MAX_END,			// 
};
ENUM_TO_STRING_EX(EmProductRule, EM_PRODUCT_RULE_MAX_END,
	ENUM_ITEM_STR(N)
	ENUM_ITEM_STR(D)
	ENUM_ITEM_STR(W)
	ENUM_ITEM_STR(M)
	ENUM_ITEM_STR(Y)
	ENUM_ITEM_STR(F)
	);
// 与gmccdefine的EmGmccProductState保持一次
enum EmProductState
{
	EM_PRODUCT_STATE_NONE,			// 未发布
	EM_PRODUCT_STATE_ACTIVE,		// 正常出售中
	EM_PRODUCT_STATE_INACTIVE,		// 过期状态

	EM_PRODUCT_STATE_END,			// 过期状态
};
ENUM_TO_STRING_EX(EmProductState, EM_PRODUCT_STATE_END,
	ENUM_ITEM_STR(N)
	ENUM_ITEM_STR(A)
	ENUM_ITEM_STR(I)
	);

#endif 