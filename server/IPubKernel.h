//--------------------------------------------------------------------
// 文件名:		IPubKernel.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月12日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IPUBKERNEL_H
#define _SERVER_IPUBKERNEL_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "IPubSpace.h"

// 公共数据逻辑接口版本号
#define PUBLIC_LOGIC_MODULE_VERSION 0x3010

class IPubKernel;

// 定时器函数
typedef int (__cdecl* PUBLIC_TIMER_FUNC)(IPubKernel* pPubKernel, 
	const char* space_name, const wchar_t* data_name, int time) ;
	
// 公共数据核心接口

class IPubKernel
{
public:
	virtual ~IPubKernel() = 0;
	
	/// \brief 查找公共数据空间
	/// \param space_name 空间名称
	virtual bool FindPubSpace(const char* space_name) = 0;
	/// \brief 获得公共数据空间数量
	virtual int GetPubSpaceCount() = 0;
	/// \brief 获得公共数据空间列表
	/// \param result 空间名称列表
	virtual int GetPubSpaceList(IVarList& result) = 0;
	/// \brief 获得指定名字的公共数据空间
	/// \param space_name 空间名称
	virtual IPubSpace* GetPubSpace(const char* space_name) = 0;
	/// \brief 保存公共数据空间中有改变的数据项
	/// \param space_name 空间名称
	virtual bool SavePubSpace(const char* space_name) = 0;
	/// \brief 保存公共数据项 
	/// \param space_name 空间名称
	/// \param data_name 数据项名称
	virtual bool SavePubData(const char* space_name, 
		const wchar_t* data_name) = 0;
	
	/// \brief 发消息到场景服务器
	/// \param source_id 服务器标识，为0表示0号场景服务器
	/// \param scene_id 场景编号，为0表示第一个场景
	/// \param msg 消息内容
	virtual bool SendPublicMessage(int source_id, int scene_id, 
		const IVarList& msg) = 0;
	
	/// \brief 查找定时器
	/// \param name 定时器名称
	/// \param space_name 关联公共数据空间名称
	/// \param data_name 关联数据项名称
	virtual bool FindTimer(const char* name, const char* space_name, 
		const wchar_t* data_name) = 0;
	/// \brief 添加定时器
	/// \param name 定时器名称
	/// \param func 定时器函数指针
	/// \param space_name 关联公共数据空间名称
	/// \param data_name 关联数据项名称
	/// \param time 定时时长
	/// \param count 定时次数
	virtual bool AddTimer(const char* name, PUBLIC_TIMER_FUNC func, 
		const char* space_name, const wchar_t* data_name, int time, 
		int count) = 0;
	/// \brief 删除定时器
	/// \param name 定时器名称
	/// \param space_name 关联公共数据空间名称
	/// \param data_name 关联数据项名称
	virtual bool RemoveTimer(const char* name, const char* space_name, 
		const wchar_t* data_name) = 0;
	
	/// \brief 请求获得主服务器状态和玩家数量信息
	virtual bool RequestServerState() = 0;
	/// \brief 请求获得可以加载到服务器的离线角色数据
	/// \param amount 最大数量
	/// \param year,month,day,hour,minute,second 基准时间
	/// \param update_resume_time 是否要更新角色的重新上线时间为基准时间
	/// \param scene_id 只获取指定场景的离线角色，为0表示全部场景
	virtual bool RequestRoleOffline(int amount, int year, int month, int day,
		int hour, int minute, int second, bool update_resume_time,
		int scene_id = 0) = 0;
	/// \brief 请求主服务器加载离线玩家
	/// \param account 玩家帐号
	/// \param role_name 玩家角色名
	/// \param live_seconds 在游戏中存在的时间（秒数）
	virtual bool RequestLoadPlayer(const char* account, 
		const wchar_t* role_name, int live_seconds) = 0;
		
	/// \brief 保存游戏日志
	/// \param name 名字
	/// \param type 类型
	/// \param content 内容
	/// \param comment 备注
	virtual bool SaveLog(const wchar_t* name, int type, 
		const wchar_t* content, const wchar_t* comment) = 0;
	/// \brief 保存自定义表格式的日志（自动生成日志流水号）
	/// \param table_name 日志数据库表名
	/// \param fields 数据字段内容
	/// 字段数据类型：整数(int)，数值(double)，字符串，宽字符串，日期字符串（格式"YYYYMMDD hh:mm:ss"）
	virtual bool CustomLog(const char* table_name, const IVarList& fields) = 0;
	
	/// \brief 获得资源文件路径
	virtual const char* GetResourcePath() = 0;
	
	/// \brief 获得当前分区号
	virtual int GetDistrictId() = 0;
	/// \brief 获得当前服务器编号
	virtual int GetServerId() = 0;
	
	/// \brief 输出到系统LOG文件
	/// \param info 信息内容
	virtual void Trace(const char* info) = 0;
	/// \brief 输出到控制台屏幕
	/// \param info 信息内容
	virtual void Echo(const char* info) = 0;

	/// \brief 根据角色名获得角色唯一标识，返回空字符串表示角色不存在
	/// \param role_name 角色名
	virtual const char* SeekRoleUid(const wchar_t* role_name) = 0;
	/// \brief 根据角色唯一标识获得角色名，返回空字符串表示角色不存在
	/// \param role_uid 角色唯一标识
	virtual const wchar_t* SeekRoleName(const char* role_uid) = 0;
	/// \brief 获得角色是否已被删除
	/// \param role_name 角色名
	virtual bool GetRoleDeleted(const wchar_t* role_name) = 0;
	/// \brief 获得角色是否处于删除保护期
	/// \param role_name 角色名
	virtual bool GetRoleDeleteProtect(const wchar_t* role_name) = 0;

	/// \brief 查找关系
	/// \param uid 玩家编号
	/// \param target_uid 目标玩家编号
	virtual bool FindRelation(const char* uid, const char* target_uid) = 0;
	/// \brief 获得关系范围
	/// \param uid 玩家编号
	/// \param target_uid 目标玩家编号
	virtual int GetRelation(const char* uid, const char* target_uid) = 0;
	/// \brief 添加关系
	/// \param uid 玩家编号
	/// \param target_uid 目标玩家编号
	/// \param range 关系范围
	virtual bool AddRelation(const char* uid, const char* target_uid, 
		int range) = 0;
	/// \brief 删除关系
	/// \param uid 玩家编号
	/// \param target_uid 目标玩家编号
	virtual bool RemoveRelation(const char* uid, const char* target_uid) = 0;
	/// \brief 获得指定类型的关系数量
	/// \param uid 玩家编号
	/// \param range 关系范围
	virtual int GetRelationCount(const char* uid, int range) = 0;
	/// \brief 获得指定范围关系玩家列表
	/// \param uid 玩家编号
	/// \param range 关系范围
	/// \param result 返回玩家编号列表
	/// \param max_num 最大玩家数量
	virtual int GetRelationList(const char* uid, int range, IVarList& result, 
		int max_num) = 0;
	/// \brief 获得指定类型的被动关系数量
	/// \param uid 玩家编号
	/// \param range 关系范围
	virtual int GetPassiveRelationCount(const char* uid, int range) = 0;

	/// \brief 设置行为缓存数量
	/// \param value 数量
	virtual bool SetFeedCacheMax(int value) = 0;
	/// \brief 设置行为自定义消息编号
	/// \param value 自定义消息编号
	virtual bool SetFeedCustomId(int value) = 0;
	/// \brief 设置不同级别行为的生存时间
	/// \param level 行为级别
	/// \param live_days 生存时间，单位天
	virtual bool SetFeedLiveTime(int level, double live_days) = 0;
	/// \brief 发起行为
	/// \param uid 玩家编号
	/// \param target_uid 目标玩家编号
	/// \param range 关系范围
	/// \param level 行为级别
	/// \param category 行为类型
	/// \param desc 行为描述
	/// \param comment_at 引用行为编号
	/// \param need_save 是否需要保存到数据库
	/// \return feed_id 新的行为编号
	virtual const char* NewFeed(const char* uid, const char* target_uid, 
		int range, int level, int category, const wchar_t* desc, 
		const char* comment_at, bool need_save = true) = 0;
	/// \brief 获得最近行为列表
	/// \param uid 玩家编号
	/// \param range 关系范围
	/// \param max_num 最大数量
	/// \param max_time_span 最大时间差(为0表示无时间限制)
	/// \param filter_uid 指定特定的行为发起人
	virtual bool SyncRecentFeeds(const char* uid, int range, int max_num,
		double max_time_span, const char* filter_uid) = 0;
	/// \brief 获得行为的评论
	/// \param uid 玩家编号
	/// \param feed_id 主行为编号
	/// \param max_num 最大数量
	/// \param max_time_span 最大时间差(为0表示无时间限制)
	virtual bool SyncFeedComments(const char* uid, const char* feed_id, 
		int max_num, double max_time_span) = 0;
	/// \brief 获得行为的反馈信息计数
	/// \param uid 玩家编号
	/// \param feed_id 主行为编号
	virtual bool SyncFeedBack(const char* uid, const char* feed_id) = 0;
	/// \brief 改变行为描述
	/// \param feed_id 行为编号
	/// \param desc 行为描述
	/// \param source_uid 来源玩家编号（必须发起行为的玩家）
	virtual bool SetFeedDesc(const char* feed_id, const wchar_t* desc,
		const char* source_uid) = 0;
	/// \brief 增加好评计数	
	/// \param feed_id 行为编号
	/// \param source_uid 来源玩家编号
	virtual bool IncFeedGoodCount(const char* feed_id, 
		const char* source_uid) = 0;
	/// \brief 增加差评计数	
	/// \param feed_id 行为编号
	/// \param source_uid 来源玩家编号
	virtual bool IncFeedBadCount(const char* feed_id, 
		const char* source_uid) = 0;
	/// \brief 获得好评计数	
	/// \param feed_id 行为编号
	virtual int GetFeedGoodCount(const char* feed_id) = 0;
	/// \brief 获得差评计数	
	/// \param feed_id 行为编号
	virtual int GetFeedBadCount(const char* feed_id) = 0;
	/// \brief 获得评论计数	
	/// \param feed_id 行为编号
	virtual int GetFeedCommentCount(const char* feed_id) = 0;
	
	/// \brief 发自定义消息到指定名字玩家的客户端
	/// \param name 玩家名字
	/// \param msg 消息参数列表
	virtual bool CustomByName(const wchar_t* name, const IVarList& msg) = 0;
	/// \brief 发自定义消息到游戏内所有玩家的客户端
	/// \param msg 消息参数列表
	virtual bool CustomByWorld(const IVarList& msg) = 0;
	/// \brief 发自定义消息到指定频道内的客户端
	/// \param channel 频道编号
	/// \param msg 消息参数列表
	virtual bool CustomByChannel(int channel, const IVarList& msg) = 0;

	/// \brief 设置交易自定义消息编号
	/// \param value 自定义消息编号
	virtual bool SetTradeCustomId(int value) = 0;
	/// \brief 搜索物品（返回符合条件的物品总数量）
	/// \param role_name 角色名字
	/// \param item_name 指定物品名字
	/// \param item_type 指定物品类型
	/// \param max_num 一次发送到客户端的最大数量
	/// \param start 发送起始序号
	virtual int TradeSearch(const wchar_t* role_name, 
		const wchar_t* item_name, int item_type, int max_num, int start) = 0;
	/// \brief 出售物品（返回出售物品的标识符）
	/// \param seller_name 出售者名字
	/// \param seller_uid 出售者标识符
	/// \param item_type 物品类型
	/// \param item_name 物品名字
	/// \param item_info 物品信息
	/// \param price 出售价格
	/// \param time_len 出售时长（单位天）
	virtual const char* TradeSell(const wchar_t* seller_name, 
		const char* seller_uid,	int item_type, const wchar_t* item_name, 
		const wchar_t* item_info, int price, double time_len) = 0;
	/// \brief 取消出售（已成交物品不可取消）
	/// \param serial_no 出售物品标识符
	/// \param seller_name 出售者名字
	/// \param seller_uid 出售者标识符
	virtual bool TradeUnsell(const char* serial_no, 
		const wchar_t* seller_name, const char* seller_uid) = 0;
	/// \brief 购买在售物品
	/// \param serial_no 出售物品标识符
	/// \param buyer_name 购买者名字
	/// \param buyer_uid 购买者标识符
	/// \param price 购买价格
	virtual bool TradeBuy(const char* serial_no, const wchar_t* buyer_name, 
		const char* buyer_uid, int price) = 0;
	/// \brief 获得出售物品状态
	/// \param serial_no 出售物品标识符
	/// \return 1-在售状态，2-成交状态，3-已超时
	virtual int TradeGetStatus(const char* serial_no) = 0;
	/// \brief 获得出售物品名字
	/// \param serial_no 出售物品标识符
	virtual const wchar_t* TradeGetItemName(const char* serial_no) = 0;
	/// \brief 获得出售物品信息
	/// \param serial_no 出售物品标识符
	virtual const wchar_t* TradeGetItemInfo(const char* serial_no) = 0;
	/// \brief 获得已经成交物品的收益
	/// \param serial_no 出售物品标识符
	/// \param seller_name 出售者名字
	/// \param seller_uid 出售者标识符
	virtual int TradeGainProfit(const char* serial_no, 
		const wchar_t* seller_name, const char* seller_uid) = 0;
	/// \brief 检查超过出售期限的物品
	/// \return 第一个超时的物品序号，为空字符串表示都没有超时
	virtual const char* TradeCheckTimeout() = 0;
	
	/// \brief 发消息到指定场景
	/// \param scene_id 场景编号
	/// \param msg 消息内容
	virtual bool SendToScene(int scene_id, const IVarList& msg) = 0;
	
	/// \brief 发命令到指定名字的场景服务器玩家对象
	/// \param name 玩家名字
	/// \param msg 消息内容
	virtual bool CommandByName(const wchar_t* name, const IVarList& msg) = 0;
};

inline IPubKernel::~IPubKernel() {}

#endif // _SERVER_IPUBKERNEL_H

