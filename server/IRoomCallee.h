//--------------------------------------------------------------------
// 文件名:		ICallee.h
// 内  容:		
// 说  明:		
// 创建日期:	2004年1月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IROOMCALLEE_H
#define _SERVER_IROOMCALLEE_H

#include "../public/Macros.h"
#include "../public/PersistId.h"
#include "../public/IVarList.h"
#include "LenType.h"

// 对象回调接口

class IKernel;
class IGameObj;

class IRoomCallee
{
public:
	virtual ~IRoomCallee() = 0;

	/// \brief 类创建，在系统初始化时调用
	/// \param pKernel 核心指针，下同
	/// \param index 逻辑类的顺序索引值
	virtual int OnCreateClass(IKernel* pKernel, int index) { return 0; }

	/// \brief 创建角色，返回非0表示创建失败
	/// \param pSelf 玩家角色对象
	/// \param msg 创建角色时客户端发来的参数
	/// \return 返回发送到客户端的错误码
	virtual int OnCreateRole(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }

	/// \brief 对象创建，对象刚被系统创建时调用
	/// \param pSelf 对象自身，下同
	/// \param sender 父对象
	virtual int OnCreate(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 带初始化参数的对象创建
	/// \param sender 父对象
	/// \param args 其他参数
	virtual int OnCreateArgs(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, const IVarList& args) { return 0; }

	/// \brief 对象销毁，对象被系统删除时调用
	/// \param sender 父对象
	virtual int OnDestroy(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 选择对象，相当于发一个整数命令给本对象处理
	/// \param sender 发出选择的对象
	/// \param function_id 选择功能号
	virtual int OnSelect(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int function_id) { return 0; } 

	/// \brief 触发对象，其他对象进入本对象的触发范围时调用
	/// \param sender 其他对象
	virtual int OnSpring(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; } 

	/// \brief 停止触发，其他对象离开本对象的触发范围时调用
	/// \param sender 其他对象
	virtual int OnEndSpring(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; } 

	/// \brief 进入容器，本对象被加入到容器中时调用
	/// \param sender 容器
	virtual int OnEntry(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; } 

	/// \brief 离开容器，本对象被移出容器时调用
	/// \param sender 容器
	virtual int OnLeave(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; } 

	/// \brief 测试本容器是否不可以加入其他对象，返回非0表示不能加入，加入行为停止
	///		在此回调中不能做任何改变对象状态的操作
	/// \param sender 要加入容器的对象
	/// \param pos 加入容器的位置，从1开始，0表示任意位置
	virtual int OnNoAdd(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int pos) { return 0; }

	/// \brief 本容器中加入其他对象
	/// \param sender 被加入容器的对象
	/// \param pos 加入容器的位置，从1开始
	virtual int OnAdd(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int pos) { return 0; }

	/// \brief 本容器中加入其他对象之后
	/// \param sender 被加入容器的对象
	/// \param pos 加入容器的位置，从1开始
	virtual int OnAfterAdd(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int pos) { return 0; }

	/// \brief 测试本容器是否不可以移走对象，返回非0表示不能移走
	///		在此回调中不能做任何改变对象状态的操作
	/// \param sender 要移出容器的对象
	virtual int OnNoRemove(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 本容器中移走其他对象之前
	/// \param sender 被移出容器的对象
	virtual int OnBeforeRemove(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 本容器中移走其他对象
	/// \param sender 被移出容器的对象
	virtual int OnRemove(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 本容器中移动对象位置之后
	/// \param sender 被移动的对象
	/// \param old_pos 旧的位置，从1开始
	/// \param new_pos 新的位置，从1开始
	virtual int OnChange(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int old_pos, int new_pos) { return 0; }

	/// \brief 客户端就绪，当收到客户端发的就绪消息时玩家对象才被加入到场景容器中
	/// \param sender 场景对象
	/// \param first 本次登录是否第一次进入场景
	virtual int OnReady(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int first) { return 0; }

	/// \brief 处理其他对象或系统发来的命令
	/// \param sender 发送命令的对象
	/// \param msg 命令包含的参数
	virtual int OnCommand(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, const IVarList& msg) { return 0; }

	/// \brief 对象加载配置文件完成
	/// \param sender 关联对象
	virtual int OnLoad(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 对象从数据库恢复数据完成
	/// \param sender 关联对象
	virtual int OnRecover(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 玩家数据将被保存到数据库之前，一般要在这个回调里设置玩家的RoleInfo内部属性
	/// \param sender 场景对象
	/// \param type 是在何种情况下被保存，见KnlConst.h
	virtual int OnStore(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int type) { return 0; }

	/// \brief 系统内部产生的对象通知信息
	/// \param sender 关联对象
	/// \param msg 通知信息参数
	virtual int OnSysInfo(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, const IVarList& msg) { return 0; }

	/// \brief 收到客户端普通聊天信息，返回0表示逻辑部分未处理，由核心部分做缺省处理
	/// \param content 聊天信息内容
	virtual int OnSpeech(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* content) { return 0; }

	/// \brief 收到客户端自定义消息
	/// \param msg 消息参数
	virtual int OnCustom(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, const IVarList& msg) { return 0; }

	/// \brief 玩家选择其他对象之前，返回非0表示不允许选择
	/// \param sender 被选择的对象
	/// \param function_id 选择功能号
	virtual int OnDoSelect(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int function_id) { return 0; }

	/// \brief 对象移动结束
	/// \param mode 移动模式，见KnlConst.h
	/// \param res 移动结果，见KnlConst.h
	virtual int OnMotion(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender, int mode, int res) { return 0; }

	/// \brief 控制对象转向完成
	virtual int OnRotate(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 收到客户端的移动请求
	/// \param mode 移动模式
	/// \param arg_num 浮点参数数量
	/// \param args 浮点参数数组
	virtual int OnRequestMove(IKernel* pKernel, IGameObj* pSelf, 
		int mode, int arg_num, len_t args[], const char* info) { return 0; }

	/// \brief 客户端断线，返回值表示玩家对象继续存在游戏中的秒数
	/// \param sender 场景对象
	virtual int OnDisconnect(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 客户端继续上线
	/// \param sender 场景对象
	virtual int OnContinue(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 玩家进入游戏
	virtual int OnEntryGame(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 玩家进入场景之前，客户端收到进入场景消息之前
	/// \param sender 场景对象
	virtual int OnBeforeEntryScene(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 玩家进入场景之后，已收到进入场景和主角信息
	/// \param sender 场景对象
	virtual int OnAfterEntryScene(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 玩家离开场景
	/// \param sender 场景对象
	virtual int OnLeaveScene(IKernel* pKernel, IGameObj* pSelf, 
		const PERSISTID& sender) { return 0; }

	/// \brief 应客户端的要求生成当前游戏世界信息（场景）
	/// \param type 客户端需要的信息类型
	/// \param info 附加信息
	virtual int OnGetWorldInfo(IKernel* pKernel, IGameObj* pSelf,
		int type, const wchar_t* info) { return 0; }

	/// \brief 发送信件完成
	/// \param name 收件人名字
	/// \param uid 收件人唯一标识
	/// \param account 收信人帐号（在发送针对帐号的系统邮件时）
	/// \param result 结果，1表示成功，其他值失败
	/// \param serial_no 如发信成功则是此信件在数据库中的流水号
	/// \param type: 信件类型
	virtual int OnSendLetter(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* name, const char* uid, const char* account, int result, 
		const char* serial_no, int type) { return 0; }

	/// \brief 查询信件数量完成
	/// \param total 本玩家当前的信件总数
	virtual int OnQueryLetter(IKernel* pKernel, IGameObj* pSelf,
		int total) { return 0; }

	/// \brief 接收到信件（数据库中已删除）
	/// \param source 发件人名字
	/// \param source_uid 发件人唯一标识（可能为空）
	/// \param time 信件发送时间
	/// \param type 信件类型
	/// \param content 信件内容
	/// \param appendix 附件内容
	/// \param send_date 真实的发送时间
	/// \param serial_no 信件的流水号
	virtual int OnRecvLetter(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* source, const char* source_uid, const wchar_t* time, 
		int type, const wchar_t* content, const wchar_t* appendix,
		double send_date, const char* serial_no) { return 0; }

	/// \brief 接收信件失败
	/// \param serial_no 信件的流水号
	virtual int OnRecvLetterFail(IKernel* pKernel, IGameObj* pSelf, 
		const char* serial_no) { return 0; }

	/// \breif 查看信件（数据库不删除）
	/// \param source 发件人名字
	/// \param source_uid 发件人唯一标识（可能为空）
	/// \param time 信件发送时间
	/// \param type 信件类型
	/// \param content 信件内容
	/// \param appendix 附件内容
	/// \param send_date 真实的发送时间
	/// \param serial_no 信件的流水号
	virtual int OnLookLetter(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* source, const char* source_uid, const wchar_t* time, 
		int type, const wchar_t* content, const wchar_t* appendix,
		double send_date, const char* serial_no) { return 0; }

	/// \brief 发信人清理信件结果
	/// \param type 清理信件的类型
	/// \param amount 清理信件的数量
	/// \param serial_no 指定信件的流水号
	virtual int OnCleanLetter(IKernel* pKernel, IGameObj* pSelf,
		int type, int amount, const char* serial_no) { return 0; }

	/// \brief 收信人退回信件结果
	/// \param serial_no 信件的流水号
	/// \param result 操作结果（成功为1）
	virtual int OnBackLetter(IKernel* pKernel, IGameObj* pSelf,
		const char* serial_no, int result) { return 0; }

	/// \brief 查询到赠送物品（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param id 赠品编号
	/// \param name 赠品名字
	/// \param num 赠品数量
	virtual int OnQueryGift(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int id, const char* name, 
		int num) { return 0; }

	/// \brief 获得赠送物品（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param id 赠品编号
	/// \param name 赠品名字
	/// \param num 赠品数量
	virtual int OnGetGift(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int result, int id, 
		const char* name, int num) { return 0; }

	/// \brief 出售点卡（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param card_no 点卡号
	/// \param price 价格
	/// \param card_type 点卡类型
	virtual int OnSellCard(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int result, 
		const char* card_no, int price, const char* card_type) { return 0; }

	/// \brief 撤消出售点卡（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param card_no 点卡号
	virtual int OnUnsellCard(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* player, const char* uid, int result, 
		const char* card_no) { return 0; }

	/// \brief 购买点卡（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param card_no 点卡号
	/// \param fill_account 冲入点数的帐号
	virtual int OnBuyCard(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* player, const char* uid, int result, 
		const char* card_no, const char* fill_account) { return 0; }

	// 点数购买物品（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param item_no 购买物品编号
	/// \param amount 购买物品数量
	/// \param points 物品单价
	/// \param acc_points 玩家帐户剩余点数
	virtual int OnBuyItem(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* player, const char* uid, int result, const char* item_no, 
		int amount, int points, int acc_points) { return 0; }

	/// \brief 点数购买物品赠送给其他帐号（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param item_no 购买物品编号
	/// \param amount 购买物品数量
	/// \param points 物品单价
	/// \param acc_points 玩家帐户剩余点数
	/// \param give_account 给予物品的帐号
	virtual int OnBuyItemGive(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* player, const char* uid, int result, 
		const char* item_no, int amount, int points, int acc_points, 
		const char* give_account) { return 0; }

	// 点数购买物品（第二代）（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param prop_id 综合ID
	/// \param item_no 购买物品编号
	/// \param amount 购买物品数量
	/// \param points 物品单价
	/// \param acc_points 玩家帐户剩余点数
	/// \param give_account 给予物品的帐号
	/// \param order_id  购买的订单ID
	virtual int OnBuyItem2(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* player, const char* uid, int result, int prop_id,
		const char* item_no, int amount, int points, int acc_points,
		const char* give_account, int order_id) { return 0; }

	/// \brief 获得帐户上的剩余点数（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param acc_points 玩家帐户剩余点数
	virtual int OnGetPoints(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int result, 
		int acc_points) { return 0; }

	/// \brief 获得帐户上的月卡截止时间（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param type 月卡类型 [JIANGHU_LIMIT_TIME(2) 江湖明俊, ZHIZUN_LIMIT_TIME(3) 至尊明俊]
	/// \param result 结果，1表示成功，其他值失败
	/// \param limit_time 月卡截止时间
	virtual int OnGetLimitTime(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int type, int result, 
		double limit_time) { return 0; }

	/// \brief 获得帐户信息（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param level 账户等级
	/// \param points 账户余额
	/// \param properties 账户属性
	virtual int OnGetAccountInfo(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int result, 
		int level, int points, const char* properties) { return 0; }

	/// \brief 获得帐户上的防沉迷游戏时间（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param result 结果，1表示成功，其他值失败
	/// \param play_time 防沉迷游戏时间
	virtual int OnGetPlayTime(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int result, 
		int play_time) { return 0; }

	/// \brief 收到计费系统的通知（场景）
	/// \param pSelf 场景对象
	/// \param player 玩家名字
	/// \param uid 玩家唯一标识
	/// \param notify_type 通知类型
	/// \param remark 备注信息
	virtual int OnChargeNotify(IKernel* pKernel, IGameObj* pSelf, 
		const wchar_t* player, const char* uid, int notify_type, 
		const char* remark) { return 0; }

	/// \brief 已获取出售物品的信息（场景）
	/// \param pSelf 场景对象
	/// \param item_no 为空表示是全部物品的信息，否则是单个物品的信息
	/// \param amount 收到物品信息数量
	virtual int OnItemInfo(IKernel* pKernel, IGameObj* pSelf, 
		const char* item_no, int amount) { return 0; }

	/// \brief 对象移动时地图区域改变
	/// \param pSelf 玩家或NPC对象
	/// \param old_area 移动前的区域类型
	/// \param new_area 移动后的区域类型
	virtual int OnMapChanged(IKernel* pKernel, IGameObj* pSelf,
		const char* old_area, const char* new_area) { return 0; }
	
	/// \brief 对象进入玩家视野
	/// \param pSelf 玩家对象
	/// \param sender 其他对象
	virtual int OnEntryVisual(IKernel* pKernel, IGameObj* pSelf,
		const PERSISTID& sender) { return 0; }

	/// \brief 对象离开玩家视野
	/// \param pSelf 玩家对象
	/// \param sender 其他对象
	virtual int OnLeaveVisual(IKernel* pKernel, IGameObj* pSelf,
		const PERSISTID& sender) { return 0; }
	
	/// \brief 来自客服系统的自定义消息（每个场景服务器的第一个场景处理）
	/// \param pSelf 场景对象
	/// \param msg 信息参数
	virtual int OnGmccCustom(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }

	/// \brief 系统内部产生的统计信息（场景）
	/// \param pSelf 场景对象
	/// \param msg 信息参数
	/// 创建人物信息<"create_role"><account><role_name>
	/// 删除人物信息<"delete_role"><account><role_name>
	virtual int OnSystemStatInfo(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }

	/// \brief 收到计费系统的自定义信息
	/// \param pSelf 场景对象
	/// \param msg 消息参数
	virtual int OnChargeMessage(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }
	
	/// \brief 收到管理系统的消息（第一个场景处理）
	/// \param pSelf 场景对象
	/// \param index,serial 通讯连接标识
	/// \param msg 消息参数
	virtual int OnManageMessage(IKernel* pKernel, IGameObj* pSelf,
		int index, int serial, const IVarList& msg) { return 0; }

	/// \brief 收到附加服务器的消息（第一个场景处理）
	/// \param pSelf 场景对象
	/// \param index 服务器索引
	/// \param msg 消息参数
	virtual int OnExtraMessage(IKernel* pKernel, IGameObj* pSelf,
		int index, const IVarList& msg) { return 0; }

	/// \brief 收到公共数据系统的自定义信息
	/// \param pSelf 场景对象
	/// \param msg 消息参数
	virtual int OnPublicMessage(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }
	/// \brief 公共数据系统重新启动后同步数据完成
	/// \param pSelf 场景对象
	virtual int OnPublicComplete(IKernel* pKernel, IGameObj* pSelf)
		{ return 0; }

	/// \brief 游戏服务器关闭以前消息通知
	/// \param pSelf 场景对象
	/// \param nServerCloseDelay 还剩余的多少时间服务器关闭
	virtual int OnServerCloseBeforeNotify(IKernel* pKernel, IGameObj* pSelf, int nServerCloseDelay)
		{ return 0; }

	/// \brief 游戏服务器关闭通知
	/// \param pSelf 场景对象
	virtual int OnServerClose(IKernel* pKernel, IGameObj* pSelf)
		{ return 0; }

	/// \brief 收到创建副本场景成功
	/// \param pSelf 场景对象
	/// \param owner_name 申请创建副本的所有者名称
	/// \param proto_scene_id 原型场景号
	/// \param clone_scene_id 副本场景号，为0表示创建失败
	virtual int OnCloneScene(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* owner_name, int proto_scene_id, int clone_scene_id) 
		{ return 0; }
	/// \brief 副本场景被获取后的复位
	/// \param pSelf 副本场景对象
	/// \param owner_name 申请创建副本的所有者名称
	/// \param down_time 副本场景回收时间（秒）
	virtual int OnCloneReset(IKernel* pKernel, IGameObj* pSelf,
		const wchar_t* owner_name, int down_time) { return 0; }

	/// \brief 修改不在线的玩家数据（返回非0值表示修改成功）
	/// \param pSelf 玩家对象
	/// \param command 修改命令参数
	virtual int OnEditPlayer(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command) { return 0; }
	/// \brief 恢复玩家数据（返回非0值表示修改成功）
	/// \param pSelf 玩家对象
	/// \param command 修改命令参数
	virtual int OnRecreatePlayer(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command) { return 0; }

	/// \brief 交易玩家角色
	/// \param pSelf 场景对象
	/// \param result 操作结果（返回1值表示修改成功,其他值为错误码）
	/// \param name  角色名称
	/// \param account 角色账号
	/// \param new_account 角色的新账号
	/// \param command 修改命令参数
	virtual int OnTradeRole(IKernel* pKernel, IGameObj* pSelf, int result,
		const wchar_t* name, const char* account,
		const char* new_account, IVarList& command) { return 0; }

	/// \brief 角色改名
	/// \param pSelf 玩家对象
	/// \param result 操作结果（返回1值表示修改成功,其他值为错误码）
	/// \param name  角色名称
	/// \param new_account 角色的新账号
	/// \param command 修改命令参数
	virtual int OnChangeName(IKernel* pKernel, IGameObj* pSelf, int result, 
		const wchar_t* name, const wchar_t* new_name, IVarList& command) { return 0; }

	/// \brief 获得指定帐号的角色名
	/// \param pSelf 场景对象
	/// \param command 命令参数
	/// \param account 角色帐号
	/// \param role_num 角色数量
	/// \param role_names 角色名列表
	virtual int OnGetAccountRole(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command, const char* account, int role_num, 
		const IVarList& role_names) { return 0; }

	/// \brief 获得玩家的角色信息
	/// \param pSelf 场景对象
	/// \param command 命令参数
	/// \param role_name 角色名称
	/// \param account 角色帐号
	/// \param role_info 角色信息
	virtual int OnGetRoleInfo(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command, const wchar_t* role_name, 
		const char* account, const wchar_t* role_info) { return 0; }

	/// \brief 创建跨服角色的逻辑回调
	/// \param pSelf 场景对象
	/// \param name 角色名称
	/// \param account 角色账号
	/// \param original_name 原始角色名称
	/// \param original_serverid 原始角色的服务器id
	/// \param result 创建结果，1表示成功，其他值为错误编号
	virtual int OnCreateCrossRole(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command, const wchar_t* name, const char* account, 
		const wchar_t* original_name, int original_serverid, int result) { return 0; }
	/// \brief 删除跨服角色的逻辑回调
	/// \param pSelf 场景对象
	/// \param original_name 原始角色名称
	/// \param original_serverid 原始角色的服务器id
	/// \param result 删除结果，1表示成功，其他值为错误编号
	virtual int OnDeleteCrossRole(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command, const wchar_t* original_name, 
		int original_serverid, int result) { return 0; }
	/// \brief 清理跨服角色的逻辑回调
	/// \param pSelf 场景对象
	/// \param result 清理结果，1表示成功，其他值为错误编号
	virtual int OnClearCrossRole(IKernel* pKernel, IGameObj* pSelf,
		int result) { return 0; }
	/// \brief 收到其他游戏服务器的消息
	/// \param pSelf 场景对象
	/// \param serverid 发送者的服务器id
	/// \param sceneid 发送者的场景id
	/// \param msg 命令包含的参数
	virtual int OnCrossCustom(IKernel* pKernel, IGameObj* pSelf,
		int serverid, int sceneid, const IVarList& msg) { return 0; }
	/// \brief 收到跨服服务器自定义消息
	/// \param pSelf 场景对象
	/// \param msg 命令包含的参数
	virtual int OnCrossMessage(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }
	/// \brief 跨服请求的逻辑回调
	/// \param pSelf 场景对象
	/// \param player 玩家的连接id
	/// \param addrinfo 目标服务器的登录地址信息
	virtual int OnTransferRole(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& command, const PERSISTID& player,
		const char* addrinfo, int result) { return 0; }
	/// \brief 跨服合并的逻辑回调
	/// \param pSelf 玩家对象，回调结束后仅保存该玩家数据。
	/// \param pMergePlayer 待合并的玩家对象
	virtual int OnMergeRole(IKernel* pKernel, IGameObj* pSelf,
		IGameObj* pMergePlayer) { return 0; }
	/// \brief 重新投递消息给玩家失败
	/// \param pSelf 场景
	/// \param msg 包含参数, 类型，当事人名字，数据
	//逻辑层先判断数量是否大于1，判断第一个参数是不是int ,不是不用处理（引擎维护漏添加的bug）,第一个参数应该是 RESEND_TO_PLAYER_ENUM 枚举值
	virtual int OnResendToPlayerFailed(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }
	/// \brief 收到Sns服务器自定义消息
	/// \param pSelf 场景对象
	/// \param msg 命令包含的参数
	virtual int OnSnsMessage(IKernel* pKernel, IGameObj* pSelf,
		const IVarList& msg) { return 0; }
	

	/// \brief stub数据验证回调
	/// \param pSelf 场景对象
	/// \param check_type 查询类型
	/// \param dataList 数据结果集
	virtual int OnStubMsgToMember(IKernel* pKernel, IGameObj* pSelf, int msg_type, const IVarList& dataList) { return 0; }


	//战场相关回调
 	/// \brief 战场合并的逻辑回调
	/// \param pSelf 玩家对象，回调结束后仅保存该玩家数据。
	/// \param pMergePlayer 待合并的玩家对象
	virtual int OnMergeBattleRole(IKernel* pKernel, IGameObj* pSelf,
		IGameObj* pMergePlayer){return 0;}
 
	/// \brief member发送的custom 消息
	/// \param pSelf 玩家对象
	/// \param command 消息体
	virtual int OnCustomMember(IKernel* pKernel, IGameObj* pSelf,const IVarList& command){return 0;}

	/// \brief 平台发回的custom 消息
	/// \param pSelf 玩家对象
	/// \param command 消息体
	virtual int OnCustomPlatform(IKernel* pKernel, IGameObj* pSelf, const IVarList& command){return 0;}

	/// \brief 房间对象销毁之前，对象被系统删除时调用
	/// \param sender 父对象
	virtual int OnBeforeDestroyRoom(IKernel* pKernel, IGameObj* pSelf, 
		const IVarList& command) { return 0; }
};

inline IRoomCallee::~IRoomCallee() {}

#endif // _SERVER_ICALLEE_H

