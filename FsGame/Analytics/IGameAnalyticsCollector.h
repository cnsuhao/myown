//--------------------------------------------------------------------
// 文件名:	IGameAnalyticsCollector.h
// 内  容:	游戏数据分析收集器
// 说  明:	
// 创建日期:	2016年08月24日
// 创建人:	李海罗
// 整理人:            
//--------------------------------------------------------------------
#ifndef __IGAME_ANALYTICS_COLLECTOR_H__
#define __IGAME_ANALYTICS_COLLECTOR_H__

#include <string>
#include <vector>
#include "public/Macros.h"
#include "utils/json.h"

typedef Json::Value	KVArgs;
class  IGameAnalyticsCollector	
{
public:
	virtual ~IGameAnalyticsCollector() = 0;

	//-------------------------------------------------------------------------
	// 服务器
	//-------------------------------------------------------------------------
	// 服务器启动
	virtual void OnServerStart() = 0;
	// 服务器关闭 
	virtual void OnServerClose() = 0;
	// 设置客户端终端信息
	virtual void OnClientInfo(	const char* roleid,			// 角色id(require)
								bool bfirst,				// 是否是首次
								const char* clientver,
								const char* ip,				// 终端id(option)
								const char* platform,		// 注册平台(option)
								bool wifi,					// 是否使用wifi(option)
								const char* manufacturer,	// 制造商(option)
								const char* devid,			// 终端设备唯一标识(option)
								const char* model,			// 终端类型(option)
								const char* os,				// 终端系统(option)
								const char* os_ver			// 终端系统版本(option)
								) = 0;

	//-------------------------------------------------------------------------
	// 账号
	//-------------------------------------------------------------------------
	// 注册用户
	virtual void OnNewUser(
				const char* uid,		// 用户唯一id (require)
				const char* anid,		// 用户原匿名id(option)
				const char* nick,		// 昵称/用户名(require)
				int sex,				// 性别(option)
				__int64 bron			// 出生日期(option)
			) = 0;
	// 删除账号
	virtual void OnDelUser(const char* uid) = 0;
	// 登陆成功
	virtual void OnUserLogin( const char* uid ) = 0;
	// 退出登陆
	virtual void OnUserLogout( const char* uid, int64_t onlinetime  ) = 0;

	//-------------------------------------------------------------------------
	// 角色
	//-------------------------------------------------------------------------
	// 创建角色
	virtual void OnNewRole(	const char* roleid,			// 角色id(require)
								const char* uid,		// 角色所属uid(require)
								const char* roleName,	// 角色名(option)
								int job,				// 职业(option)
								int sex,				// 性别(option)
								int flag,				// 其它标志(option) 国家/阵营
								const KVArgs* initArgs	// 初始化数据
								) = 0;	
	// 删除角色 roleid(require)
	virtual void OnDelRole(const char* roleid, int level, const char* uid) = 0;
	// 进入角色 roleid(require)
	virtual void OnRoleEntry(const char* roleid,		// 角色id
							int level,					// 角色等级
							int vip,					// 角色vip
							int64_t gold,				// 当前所拥有的金币
							const KVArgs* args			// 参数列表(属性名键值对)
							) = 0;
	// 退出角色 roleid(require)
	virtual void OnRoleExit(const char* roleid, 		// 角色id
							int level,					// 角色等级
							int vip,					// 角色vip
							int64_t gold,				// 当前所拥有的金币
							int64_t onlinetime,			// 在线时长
							const KVArgs* args			// 参数列表(属性名键值对)
							) = 0;


	//-------------------------------------------------------------------------
	// 购买
	//-------------------------------------------------------------------------
	// 发送充值 roleid(require)
	virtual void OnPaying(
						const char* roleid,			// 角色id(require)
						int level,					// 角色等级
						int vip,					// 角色vip等级
						const char* orderid,		// 订单id(require)
						const char* itemid,			// 物品id(require) 自行定义
						int count,					// 购买的数量(require)
						float price,				// 产品价格(require)
						bool first,					// 是否首充
						const char* platform		// 客户端平台
						) = 0;
	// 支付结束
	virtual void OnPayCompleted(
						const char* roleid,				// 角色id(require)
						int level,						// 角色等级
						int vip,						// 角色vip等级
						const char* orderid,			// 订单id(require)
						float amount,					// 实际支付金额(require)
						int result,						// 支付结果(require) 0:成功 1:失败 2:取消
						bool first,						// 是否首充
						const char* platform,			// 支付平台(require)(xy, xiaomi,baidu....)
						const char* os					// 客户端os
						) = 0;

	//-------------------------------------------------------------------------
	// 成长/消耗 (游戏定义分为模块和事件)
	//-------------------------------------------------------------------------
	// 增加经验
	virtual void OnIncExp(	
							const char* roleid,		// 角色id(require)
							int level,				// 角色等级
							int value,				// 增加值(require)
							int module,				// 功能模块(option), 游戏中定义(在什么模块)
							int func,				// 功能事件(option), 游戏中定义(做什么事获得)
							const char* extra		// 额外说明(option), 对event进一步说明
							) = 0;
	// 等级增加
	virtual void OnLevelup(	
							const char* roleid,			// 角色id(require)
							int level,				// 当前值(require)
							int original			// 原始值(require)
							) = 0;
	// 获得游戏币
	virtual void OnGainCoin(
								const char* roleid,			// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int coid,				// 游戏币id(金/银/铜/声望....)(require), 游戏中定义
								int64_t value,			// 增加值(require)
								int64_t newvalue,		// 当前新值(require)
								int64_t original,		// 原始值(require)
								int module,				// 功能模块(option), 游戏中定义(在什么模块)
								int func,				// 功能事件(option), 游戏中定义(做什么事获得)
								const char* extra		// 额外说明(option), 对event进一步说明
								) = 0;
	// 消耗游戏币
	virtual void OnCostCoin(
								const char* roleid,			// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int coid,				// 游戏币id(金/银/铜/声望....)(require), 游戏中定义
								int64_t value,			// 消耗值(require)
								int64_t newvalue,		// 当前新值
								int64_t original,		// 原始值(require)
								int module,				// 功能模块(option), 游戏中定义(在什么模块)
								int func,				// 功能事件(option), 游戏中定义(做什么事获得)
								const char* extra		// 额外说明(option), 对event进一步说明
								) = 0;

	// 获得物品
	virtual void OnGainItem(
							const char* roleid,			// 角色id(require)
							int level,				// 角色当前等级(require)
							int vip,				// vip等级(require)
							const char* itemid,		// 物品id(require)
							int num,				// 获得数值(require)
							int count,				// 当前拥有数
							int orig,				// 原来拥有数
							int module,				// 功能模块(option), 游戏中定义(在什么模块)
							int func,				// 功能事件(option), 游戏中定义(做什么事获得)
							const char* extra		// 额外说明(option), 对event进一步说明
							) = 0;
	// 使用物品
	virtual void OnCostItem(
							const char* roleid,			// 角色id(require)
							int level,				// 角色当前等级(require)
							int vip,				// vip等级(require)
							const char* itemid,		// 物品id(require)
							int num,				// 获得数值(require)
							int count,				// 当前拥有数
							int orig,				// 原来拥有数
							int module,				// 功能模块(option), 游戏中定义(在什么模块)
							int func,				// 功能事件(option), 游戏中定义(做什么事获得)
							const char* extra		// 额外说明(option), 对event进一步说明
							) = 0;

	// vip升级
	virtual void OnVipLevelup(
							const char* roleid,			// 角色id(require)
							int level,				// 角色当前等级(require)							
							int value,				// 新值(require)
							int original,			// 原始值(require)
							int module,				// 功能模块(option), 游戏中定义(在什么模块)
							int func,				// 功能事件(option), 游戏中定义(做什么事获得)
							const char* extra		// 额外说明(option), 对event进一步说明
							) = 0;
	// 关键属性变化
	virtual void OnPlayerPropertyChanged(
							const char* roleid,			// 角色id(require)
							int level,				// 角色当前等级(require)
							int vip,				// vip等级(require)
							const char* propery,					// 属性名
							float value,								// 增加值(require)
							float original,							// 原始值(require)
							int module,				// 功能模块(option), 游戏中定义(在什么模块)
							int func,				// 功能事件(option), 游戏中定义(做什么事获得)
							const char* extra		// 额外说明(option), 对event进一步说明
							) = 0;
	// 玩家交易记录
	virtual void OnPlayerTransaction( const char* transid,	// 交易id(require)
									int transtype,			// 交易类型(require), 游戏内定义, 如0自由交易， 1好友之间送花......
									const char* sender,		// 角色id1(require)
									const char* reciver,	// 角色id2(require)
									const char* senderItem,	// 物品1id(require)
									int senderCount,		// 物品1数量(require)
									const char* reciverItem,// 物品2id(option) 为空表示单方赠送
									int reciverCount		// 物品2数量(option)
									) = 0;
	// 新手引导
	virtual void OnGuideEvent(
								const char* roleid,			// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int guideid,				// 引导id
								int step					// 引导步骤
								) = 0;

	//-------------------------------------------------------------------------
	// 任务统计
	//-------------------------------------------------------------------------
	// 接任务
	virtual void OnTaskStart(
								const char* roleid,		// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int taskid,				// 接取的任务id(require)
								int type				// 任务类型(option) 游戏中定义
								) = 0;
	// 任务完成
	virtual void OnTaskCompleted(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int taskid,				// 接取的任务id(require)
									int type				// 任务类型(option) 游戏中定义
									) = 0;


	//-------------------------------------------------------------------------
	// 副本统计
	//-------------------------------------------------------------------------
	// 开启副本
	virtual void OnSceneOpen(
								const char* roleid,		// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int sid,					// 副本id(require)
								int chapter,				// 副本章节(option)
								int type 					// 副本类型(option) 游戏中定义
								) = 0;
	// 进入副本
	virtual void OnSceneEntry(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int sid,				// 副本id(require)
									int chapter,			// 副本章节(option)
									int type				// 副本类型(option) 游戏中定义
									) = 0;
	// 退出副本
	virtual void OnSceneExit(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int sid,				// 副本id(require)
									int chapter,			// 副本章节(option)
									int type,				// 副本类型(option) 游戏中定义
									int64_t staytime,		// 驻留时间
									int diedNum,			// 死亡次数
									int result				// 战斗结果(require)
									) = 0;


	//-------------------------------------------------------------------------
	// 其它功能模块统计
	//-------------------------------------------------------------------------
	// 访问某个功能(比如说功能开启)
	virtual void OnVisitFunction(
								const char* roleid,	// 角色id(require)
								int level,				// 角色等级(require)
								int vip,				// vip等级(require)
								int module,				// 功能模块(require), 游戏中定义(在什么模块)
								int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
								bool result,			// 结果(require) true:成功 false: 失败
								int value,				// 额外说明(option), 对func进一步说明
								const char* extra		// 额外说明(option), 对func进一步说明
								) = 0;
	// 扩展自定义
	virtual void OnCustomEvent(
								const char* roleid,		// 角色id(option)
								int level,				// 角色等级(require)
								int vip,				// vip等级(require)
								const char* evtName,		// 事件名(require)
								int module,				// 功能模块(require), 游戏中定义(在什么模块)
								int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
								const KVArgs* args		// 参数列表(属性名键值对)
								) = 0;

	// 设置玩家属性
	virtual void SetProfile(const char* roleid, const KVArgs& args) = 0;
};
inline IGameAnalyticsCollector::~IGameAnalyticsCollector(){};

#endif