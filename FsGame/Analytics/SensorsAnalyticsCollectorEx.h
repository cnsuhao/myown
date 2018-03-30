//--------------------------------------------------------------------
// 文件名:	SensorsAnalyticsCollectorEx.h
// 内  容:	神策游戏数据分析收集器
// 说  明:	
// 创建日期:	2016年08月24日
// 创建人:	李海罗
// 整理人:            
//--------------------------------------------------------------------
#ifndef __SENSORS_ANALYTICS_COLLECTOR_IMP_H__
#define __SENSORS_ANALYTICS_COLLECTOR_IMP_H__

#include "IGameAnalyticsCollector.h"
#include "SensorsAnalyticsDef.h"
#include "utils/json.h"
#include "utils/string_util.h"
#include <vector>

class SensorsAnalyticsCollectorEx //: public IGameAnalyticsCollector
{
private:
	int64_t m_nDeployId;					// 游戏布署id
	int64_t m_nGameId;						// 游戏id
	int64_t m_nSrvId;						// 服务器id
	int64_t m_nDistrictId;					// 分区id
	int64_t m_nChannelId;					// 游戏归类渠道id
	int64_t m_nProductionId;				// 生产id: 正式环境还是开发环境 ...
			
	std::string m_strGameVer;			// 游戏服务器版本
	std::string m_strGameName;			// 游戏名
	std::string m_strChannelName;		// 分区

	IAnalytisLogger m_anaLogger;

	// 最后填充时间

	
	// 使用过增加时间的玩家, 并且时间戳未超过最后时间
	std::map<std::string, int64_t> m_strRoleCache;
public:

	SensorsAnalyticsCollectorEx( IAnalytisLogger& logger, 
							int64_t nDeployId,			// 布署id
							int64_t nGameid,
							int64_t nSrvId,
							int64_t nDistrictId,
							int64_t nChannelId,				// 游戏归类渠道： 联运或专用渠道
							int64_t nProductionId,			// 生产id: 正式环境还是开发环境 ...
							const char* gameVer,
							const char* gameName, 
							const char* m_strDistrictName);
	//初始化
	bool InitAction();

	IAnalytisLogger& GetLogger()
	{
		return m_anaLogger;
	}

	void SetDeployId(int64_t nDeployId)
	{
		m_nDeployId = nDeployId;
	}
	int64_t GetDeployId() const
	{
		return m_nDeployId;
	}

	void SetGameId(int64_t nGameId)
	{
		m_nGameId = nGameId;
	}
	int64_t GetGameId() const
	{
		return m_nGameId;
	}

	void SetServerId(int64_t nSrvId)
	{
		m_nSrvId = nSrvId;
	}
	int64_t GetServerId() const
	{
		return m_nSrvId;
	}

	void SetDistrictId(int64_t nDistrictId)
	{
		m_nDistrictId = nDistrictId;
	}
	int64_t GetDistrictId() const
	{
		return m_nDistrictId;
	}

	void SetChannelId(int64_t nChannelId)
	{
		m_nChannelId = nChannelId;
	}
	int64_t GetChannelId() const
	{
		return m_nChannelId;
	}

	void SetProductionId(int64_t nProductionId)
	{
		m_nProductionId = nProductionId;
	}
	int64_t GetProductionId() const
	{
		return m_nProductionId;
	}


	//-------------------------------------------------------------------------
	// 服务器
	//-------------------------------------------------------------------------
	// 服务器启动
	virtual void OnServerStart();
	// 服务器关闭 
	virtual void OnServerClose();
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
								const char* os_ver,			// 终端系统版本(option)
								const KVArgs * args);

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
							);
	// 删除账号
	virtual void OnDelUser(const char* uid);
	// 登陆成功
	virtual void OnUserLogin(const char* uid,			// 用户唯一id (require)
								const char* nick,		// 昵称/用户名(require)
								const char* ip,			// 终端ip(option)
								const char* platform,	// 平台(option)
								const char* manufacturer,	// 制造商(option)
								const char* devid,		// 终端设备唯一标识(option)
								const char* model,		// 终端类型(option)
								const char* os,			// 终端系统(option)
								const char* os_ver,		// 终端系统版本(option)
								bool bReset = false );
	// 退出登陆
	virtual void OnUserLogout(const char* uid, int64_t nTimestamp = 0, bool bReset = false);


	//-------------------------------------------------------------------------
	// 角色
	//-------------------------------------------------------------------------
	// 创建角色
	virtual void OnNewRole(const char* roleid,			// 角色id(require)
								const char* uid,		// 角色所属uid(require)
								const char* roleName,	// 角色名(option)
								int job,				// 职业(option)
								int sex,				// 性别(option)
								int flag,				// 其它标志(option) 国家/阵营
								const char* ip,			// ip
								const KVArgs* initArgs	// 初始化数据
								);
	// 删除角色 roleid(require)
	virtual void OnDelRole(const char* roleid, int level, const char* uid);
	// 进入角色 roleid(require)
	virtual void OnRoleEntry(const char* roleid,			// 角色id
								const char* uid,			// 角色所属uid(require)
								int level,					// 角色等级
								int vip,					// 角色vip
								int64_t gold,				// 当前所拥有的金币
								const char* ip,				// ip
								const KVArgs* args,			// 参数列表(属性名键值对)
								bool bReset = false);
	// 退出角色 roleid(require)
	virtual void OnRoleExit(const char* roleid, 		// 角色id
								const char* uid,		// 角色所属uid(require)
								int level,					// 角色等级
								int vip,					// 角色vip
								int64_t gold,				// 当前所拥有的金币
								int64_t onlinetime,			// 在线时长
								int64_t totaltime,			// 总在线时长
								const char* ip,				// ip
								const KVArgs* args,			// 参数列表(属性名键值对)
								bool bReset = false);


	//-------------------------------------------------------------------------
	// 购买
	//-------------------------------------------------------------------------
	// 发送充值 roleid(require)
	virtual void OnPaying(
							const char* roleid,			// 角色id(require)
							const char* uid,			// 角色所属uid(require)
							int level,					// 角色等级
							int vip,					// 角色vip等级
							const char* orderid,		// 订单id(require)
							const char* itemid,			// 物品id(require) 自行定义
							int count,					// 购买的数量(require)
							float price,				// 产品价格(require)
							bool first,					// 是否首充
							const char* platform		// 客户端平台
							);
	// 支付结束
	virtual void OnPayCompleted(
									const char* roleid,			// 角色id(require)
									const char* uid,			// 角色所属uid(require)
									int level,					// 角色等级
									int vip,					// 角色vip等级
									const char* orderid,			// 订单id(require)
									float amount,					// 实际支付金额(require)
									float totalamount,				// 总支付金额
									float realamount,				// 真实支付金额
									int paytimes,					// 总充值次数
									int result,						// 支付结果(require) 0:成功 1:失败 2:取消
									bool first,						// 是否首充
									bool bIsReal,					// 是否虚拟币充值
									const char* platform,			// 支付平台(require)(xy, xiaomi,baidu....)
									const char* os					// 客户端os
									);

	//-------------------------------------------------------------------------
	// 成长/消耗 (游戏定义分为模块和事件)
	//-------------------------------------------------------------------------
	// 增加经验
	virtual void OnIncExp(
		const char* roleid,		// 角色id(require)
		int level,				// 角色等级
		int value,					// 增加值(require)
		int newValue,				// 增加后的值
		int origValue,				// 原始值
		int source	,				// 来源
		const KVArgs * args);
	// 等级增加
	virtual void OnLevelup( 
									const char* roleid,		// 角色id(require)
									int level,				// 当前值(require)
									int original,			// 原始值(require)
									const KVArgs * args
									);

	// 经脉等级增加
	void OnMeridianLevelup(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int value,				// 当前值(require)
									int original,			// 原始值(require)
									const KVArgs * args
									);

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
									const char* extra,		// 额外说明(option), 对event进一步说明
									const KVArgs * args
									);
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
									const char* extra,		// 额外说明(option), 对event进一步说明
									const KVArgs * args
									);

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
									const char* extra,		// 额外说明(option), 对event进一步说明
									const KVArgs * args
									);
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
									const char* extra,		// 额外说明(option), 对event进一步说明
									const KVArgs * args
									);

	// vip升级
	virtual void OnVipLevelup(
										const char* roleid,			// 角色id(require)
										int level,				// 角色当前等级(require)
										int value,				// 新值(require)
										int original,			// 原始值(require)
										int module,				// 功能模块(option), 游戏中定义(在什么模块)
										int func,				// 功能事件(option), 游戏中定义(做什么事获得)
										const char* extra,		// 额外说明(option), 对event进一步说明
										const KVArgs * args
										);
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
										const char* extra,		// 额外说明(option), 对event进一步说明
										const KVArgs * args
										);
	// 玩家交易记录
	virtual void OnPlayerTransaction(const char* transid,	// 交易id(require)
										int transtype,			// 交易类型(require), 游戏内定义, 如0自由交易， 1好友之间送花......
										const char* sender,		// 角色id1(require)
										const char* reciver,	// 角色id2(require)
										const char* senderItem,	// 物品1id(require)
										int senderCount,		// 物品1数量(require)
										const char* reciverItem,// 物品2id(option) 为空表示单方赠送
										int reciverCount,		// 物品2数量(option)
										const KVArgs * args
										);
	// 新手引导
	virtual void OnGuideEvent(
								const char* roleid,			// 角色id(require)
								int level,				// 角色当前等级(require)
								int vip,				// vip等级(require)
								int guideid,				// 引导id
								int step,					// 引导步骤
								const KVArgs * args
								);

	//-------------------------------------------------------------------------
	// 任务统计
	//-------------------------------------------------------------------------
	// 接任务
	virtual void OnTaskStart(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int taskid,				// 接取的任务id(require)
									int type,				// 任务类型(option) 游戏中定义
									int opt_type,			// 任务操作方式(option) 游戏中定义
									const int state,		// 任务状态	
									const KVArgs * args
									);
	// 完成任务
	virtual void OnTaskCompleted(
									const char* roleid,		// 角色id(require)
									int level,				// 角色当前等级(require)
									int vip,				// vip等级(require)
									int taskid,				// 接取的任务id(require)
									int type,				// 任务类型(option) 游戏中定义
									int opt_type,			// 任务操作方式(option) 游戏中定义
									const int state,		// 任务状态	
									const KVArgs * args
									);
	// 交任务
	virtual void OnTaskSubmit(
		const char* roleid,		// 角色id(require)
		int level,				// 角色当前等级(require)
		int vip,				// vip等级(require)
		int taskid,				// 接取的任务id(require)
		int type,				// 任务类型(option) 游戏中定义
		int opt_type,			// 任务操作方式(option) 游戏中定义
		const int state,		// 任务状态	
		const KVArgs * args
		);

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
											int type ,					// 副本类型(option) 游戏中定义
											const KVArgs * args
											) ;
	// 进入副本
	virtual void OnSceneEntry(
											const char* roleid,		// 角色id(require)
											int level,				// 角色当前等级(require)
											int vip,				// vip等级(require)
											int sid,				// 副本id(require)
											int chapter,			// 副本章节(option)
											int type,				// 副本类型(option) 游戏中定义
											const KVArgs * args
											);
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
											int result,				// 战斗结果(require)
											const KVArgs * args
											) ;


	//-------------------------------------------------------------------------
	// 其它功能模块统计
	//-------------------------------------------------------------------------
	// 访问某个功能(比如说功能开启)
	virtual void OnVisitFunction(
								const char* roleid,		// 角色id(require)
								int level,				// 角色等级(require)
								int vip,				// vip等级(require)
								int module,				// 功能模块(require), 游戏中定义(在什么模块)
								int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
								bool result,			// 结果(require) true:成功 false: 失败
								int value,				// 额外说明(option), 对func进一步说明
								const char* extra,		// 额外说明(option), 对func进一步说明
								const KVArgs * args
								);
	// 扩展自定义
	virtual void OnCustomEvent( 
								const char* roleid,		// 角色id(option)
								int level,				// 角色等级(require)
								int vip,				// vip等级(require)
								const char* evtName,		// 事件名(require)
								int module,				// 功能模块(require), 游戏中定义(在什么模块)
								int func,				// 功能名(require) 游戏中定义(建议跟上面的func一致)
								const KVArgs* args		// 参数列表(属性名键值对)
								);


	// 设置玩家属性
	virtual void SetProfile(const char* roleid, const KVArgs& args);

	
private:
	void FillSensorsTrackHeader(Json::Value& root, 
								const char* type,
								const char* evt, 
								const char* roleid,
								int level = 0);
	void FillModuleInfo(Json::Value& root, int module, int func, const char* extra, const KVArgs* args = NULL) const;
	void FillSensorsProfileHeader(Json::Value& root, const char* roleid, const char* type);
	static void FillSensorsProfileArgs( Json::Value& root, int job, const KVArgs* args)
	{
		if (NULL != args)
		{
			for (auto itr = args->begin(); itr != args->end(); ++itr)
			{		
				root[ itr.memberName() ] = *itr;
				/*
				switch (itr->second.GetType())
				{
				case VTYPE_BOOL:
				{
					bool value = itr->second.BoolVal();
					root[itr->first.c_str()] = value;
				}
				break;
				case VTYPE_INT:
				{
					int value = itr->second.IntVal();
					root[itr->first.c_str()] = value;
				}
				break;
				case VTYPE_INT64:
				{
					int64_t value = itr->second.Int64Val();
					root[itr->first.c_str()] = value;
				}
				break;
				case VTYPE_FLOAT:
				{
					float value = itr->second.FloatVal();
					root[itr->first.c_str()] = value;
				}
				break;
				case VTYPE_DOUBLE:
				{
					double value = itr->second.DoubleVal();
					root[itr->first.c_str()] = value;
				}
				break;
				case VTYPE_STRING:
				{
					const char* value = itr->second.StringVal();
					InsertString(root, itr->first.c_str(), value);
				}
				break;
				case VTYPE_WIDESTR:
				{
					const char* value = StringUtil::WideStrAsUTF8String(itr->second.WideStrVal()).c_str();
					InsertString(root, itr->first.c_str(), value);
				}
				break;
				default:
					break;
				}
				*/
			}
		}
	}
	static void FillCustomArgs(Json::Value& root, const KVArgs* args)
	{
		if (NULL != args)
		{
			for (auto itr = args->begin(); itr != args->end(); ++itr)
			{
				root[itr.memberName()] = *itr;
			}
		}
	}
	//const char* GetModuleName(int nModule) const;
	//const char* GetFuncName(int nFuncName) const;
	static void InsertString(Json::Value& root, const char* key, const char* value)
	{
		if (NULL != value)
		{
			root[key] = value;
		}
	}

	void WriteAnalyticsLog(const Json::Value& root) const
	{
		Json::FastWriter writter;
		ANALYTICS_LOG(m_anaLogger, writter.write(root));
	}

	struct LogActionType
	{
		std::string m_actionName; //名字
		int m_actionMin;		  //最小范围
		int m_actionMax;		  //最大范围
	};

	std::vector<LogActionType> m_action;

	inline const char* GetActionName(int actionType)const
	{
		std::vector<LogActionType>::const_iterator it = m_action.begin();
		for (; it != m_action.end(); ++it)
		{
			if (it->m_actionMin <= actionType && actionType <= it->m_actionMax){
				return (it->m_actionName).c_str();
			}
		}
		return "log_event";
	}

};

#endif