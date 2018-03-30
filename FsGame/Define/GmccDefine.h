//--------------------------------------------------------------------
// 文件名:		GmccDefine.h
// 内  容:		GmccDefine的相关定义
// 说  明:		
// 创建日期:    2016年9月19日
//--------------------------------------------------------------------
#ifndef __GMCC_XX_DEFINE_H__
#define __GMCC_XX_DEFINE_H__

enum GmccCmd
{
	GMCC_CMD_EXEC_GM_COMMAND,			// 执行gm命令
// 	GMCC_CMD_EXEC_FORBID_SPEAK,			// 禁言玩家
// 	GMCC_CMD_EXEC_ALLOW_SPEAK,			// 解除玩家禁言
// 	GMCC_CMD_EXEC_KICK_OUT_IN_TIMER,	// 限时踢人
	GMCC_CMD_EXEC_PAYED,				// 支付成功 参数: order id
	GMCC_CMD_EXEC_PROP_QUERY,			// 查询属性 参数: queryid, keys
	GMCC_CMD_EXEC_RECORD_QUERY,			// 查询表 参数: queryid, recordName, offset, count
	GMCC_CMD_SYNC_SEV_INFO,				// gmcc同步服务器信息
	GMCC_CMD_RECV_GLOBAL_MAIL,			// 收取全局邮件
	GMCC_CMD_REBATE_INFO,				// 充值返利信息 args: rebate[int]
	GMCC_CMD_REBATE_RESULT,				// 充值返利结果 args: result[int]
};

// --- 客户端发送关于GMCC类型的消息 ----
enum GMCC2SMsg
{
	GMCC_C2S_NULL = 0,			// 某人消息类型 
	GMCC_C2S_ACCUSE,			// 举报
	GMCC_C2S_BUG, 				// BUG提交
	GMCC_C2S_EXP, 				// 体验提交
	GMCC_C2S_EXCH_GIFT,			// 兑换礼品
	GMCC_C2S_PICK_PAYED_REBATE,	// 领取测试充值返利 args: none
};

enum GmccS2CMsg
{
	/*!
	* @brief	增加gmcc公告
	* @param	int			通告个数n
	以下循环n次
	* @param	int64		通告id
	* @param	string		通告内容
	* @param	int			是否循环播放
	* @param	int			播放间隔时间
	*/
	GMCC_S2C_ADD_NOTICE,				// 增加公告

	/*!
	* @brief	增加gmcc公告
	* @param	int64		通告id
	*/
	GMCC_S2C_DELETE_NOTICE,				// 删除公告

	GMCC_S2C_PICK_PAYED_REBATE,			// 领取测试充值返利 args: 是否成功(int: EmRebateResult)
	GMCC_S2C_PAYED_REBATE_NOTIFY,		// 充值返利通知	args: 返利元宝数[int]
};

#define  PUB_GMCC_NOTICE_REC "gmcc_notice_rec" // gmcc通告表
// gmcc通告表
enum GmccNoticeRec
{
	PUB_COL_GNR_NOTICE_ID = 0,		// 通告id
	PUB_COL_GNR_NOTICE_NAME,		// 通告名字
	PUB_COL_GNR_NOTICE_CONTENT,		// 通告内容
	PUB_COL_GNR_NOTICE_CREATE_TIME,	// 通告创建时间
	PUB_COL_GNR_NOTICE_END_TIME,	// 通告结束时间
	PUB_COL_GNR_NOTICE_INTERVAL,	// 通告播放间隔时间
	PUB_COL_GNR_NOTICE_LOOP,		// 是否循环
	PUB_COL_GNR_NOTICE_TYPE,		// 通知类型

	PUB_GNR_NOTICE_MAX_COL,
	PUB_GNR_NOTICE_MAX_ROW = 100
};

#define PUB_GLOBAMAIL_REC	"pub_global_mail_rec"	// 全局邮件表
#define PUB_GLOBAMAIL_MAX_ROWS 10240			// 最大存放记录
enum EmGlobalMail
{
	EM_PUB_COL_GLOBAL_MAIL_ID = 0,		// 邮件唯一id
	EM_PUB_COL_GLOBAL_MAIL_ORI_ID,		// 发送邮件ID, 查阅用
	EM_PUB_COL_GLOBAL_MAIL_FROM,		// 邮件来源
	EM_PUB_COL_GLOBAL_MAIL_SENDER,		// 发送者
	EM_PUB_COL_GLOBAL_MAIL_TITLE,		// 标题
	EM_PUB_COL_GLOBAL_MAIL_CONTENT,		// 内容
	EM_PUB_COL_GLOBAL_MAIL_ATTACHEMENT,	// 附件
	EM_PUB_COL_GLOBAL_MAIL_CTIME,		// 创建时间
	EM_PUB_COL_GLOBAL_MAIL_ETIME,		// 有效终止时间
};

enum EmAddGlobalMailResult
{
	EM_GLOBAL_MAIL_ADD_SUCCESS = 0,		// 成功
	EM_GLOBAL_MAIL_ADD_FAILED_PARAM,	// 参数错
	EM_GLOBAL_MAIL_ADD_FAILED_UNKOWN,	// 插入表出错
};

#define PUB_GMCC_PAYED_REC	"gmcc_payed_rec"	// 支付成功消息表
#define PUB_GMCC_PAYED_MAX_ROWS	10240			// 最大存放记录
enum EmGmccPayRecCols
{
	PUB_COL_GNR_PAYED_ROLE_ID = 0,		// 角色uid string
	PUB_COL_GNR_PAYED_ORDER,			// 订单号 string 
	PUB_COL_GNR_PAYED_PRODUCT_ID,		// 商品id string
	PUB_COL_GNR_PAYED_AMOUNT,			// 支付的金额 float
	PUB_COL_GNR_PAYED_ITEMS,			// 商品标志的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PAYED_REWARDS,			// 赠送的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PAYED_GIFTS,			// 首充赠送的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PAYED_EXTRAS,			// 购买次数超出后的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PAYED_BILL_TYPE,		// 账单类型 int
	PUB_COL_GNR_PAYED_PRODUCT_RULE,		// 购买规则类型 int
	PUB_COL_GNR_PAYED_PRODUCT_NUMBER,	// 购买规则对应的数量 int
	PUB_COL_GNR_PAYED_ORDER_TIME,		// 订单创建时间 int64
	PUB_COL_GNR_PAYED_PAYED_TIME,		// 订单支付时间 int64

	PUB_COL_GNR_PAYEED_MAX_COL,
};

#define PUB_REC_GMCC_PRODUCTS		"pub_rec_products"	// 支付购买商品列表
enum EmGmccProductsRecCols
{
	PUB_COL_GNR_PRODUCTS_PRODUCT_ID = 0,	// 商品id string
	PUB_COL_GNR_PRODUCTS_PRICE,				// 商品价格 float
	PUB_COL_GNR_PRODUCTS_ITEMS,				// 商品标志的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PRODUCTS_REWARDS,			// 赠送的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PRODUCTS_GIFTS,				// 首充赠送的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PRODUCTS_EXTRAS,			// 购买次数超出后的物品id列表(格式:item1:num,item2:num...) string
	PUB_COL_GNR_PRODUCTS_RULE,				// 购买规则 int
	PUB_COL_GNR_PRODUCTS_STATE,				// 商品状态 int
	PUB_COL_GNR_PRODUCTS_NUMBER,			// 规则控制的可购买次数 int(小于等于0 不限制)

	PUB_COL_GNR_PRODUCTS_MAX_COL,
};

// 与EmPayedResult(gmcc->ProtocolsID)对应
enum EmGmccPayedResult
{
	EM_PAYED_RESULT_SUCCESS,				// 成功插入
	EM_PAYED_RESULT_REPEAT,					// 重复插入
	EM_PAYED_RESULT_DATA_ERR,				// 数据错
};

enum EmGmccMondifyMode
{
	EM_GMCC_MONDIFY_ADD,				// 新增
	EM_GMCC_MONDIFY_PUSH,				// 推送
	EM_GMCC_MONDIFY_REMOVE,				// 移除
};

// 与EmProductState(gmcc->ProtocolsID)对应
enum EmGmccProductState
{
	EM_GMCC_PRODUCT_STATE_NONE,			// 未发布
	EM_GMCC_PRODUCT_STATE_NORMAL,		// 正常出售中
	EM_GMCC_PRODUCT_STATE_OBSOLETE,		// 过期状态
};

const char* const GMCC_GM_COMMAND_PRI = "gm_";
// 禁言 string uid int nSilenceTime(单位:minute)
const char* const GMCC_EXEC_FORBID_SPEAK = "forbid_speak";
// 解除禁言 string uid
const char* const GMCC_EXEC_ALLOW_SPEAK = "allow_speak";
// 踢人 string uid int time 秒
const char* const GMCC_EXEC_KICK_OUT_IN_TIMER = "kick_out_int_timer";
// 封号 string uid int time 秒
const char* const GMCC_EXEC_LOCK_PLAYER = "lock_player";
// 解除上线 string uid delay(s延迟时间 0立即生效)
const char* const GMCC_EXEC_UNLOCK_PLAYER = "unlock_player";
// gm提权指令 level
const char* const GMCC_EXEC_GRANT_AUTH = "grant_auth";
// 重新加载lua
const char* const GMCC_EXEC_RELOAD_LUA = "reload_lua";


// 提示信息
#define TIP_EXCH_GIFT_SUCCEED			"sys_tip_exch_gift_succeed"		// 成功
#define TIP_EXCH_GIFT_UNKOWN			"sys_tip_exch_gift_unkown"		// 未定义错
#define TIP_EXCH_GIFT_INVALID			"sys_tip_exch_gift_invalid"		// 无效key
#define TIP_EXCH_GIFT_TAKED				"sys_tip_exch_gift_taked"		// 已领取
#define TIP_EXCH_GIFT_EXPIRED			"sys_tip_exch_gift_expired"		// 已过期
#define TIP_EXCH_GIFT_LIMIT				"sys_tip_exch_gift_limit"		// 礼包账取次数受限制

// 邮件标题
#define MAIL_NAME_PAYED							"mail_name_payed"				// 支付成功邮件
#define MAIL_NAME_PAYED_ITEM					"mail_name_payed_item"			// 支付成功发送物品邮件
#define MAIL_NAME_PAYED_REWORD					"mail_name_payed_reword"		// 支付成功赠送奖励
#define MAIL_NAME_PAYED_FIRST_PRODUCT			"mail_name_payed_first_product"	// 商品首充支付成功赠送奖励


#endif // __GMCC_XX_DEFINE_H__