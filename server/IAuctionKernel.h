//--------------------------------------------------------------------
// 文件名:		IAuctionKernel.h
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
#include "../server/IAuctionData.h"

// 拍卖行数据逻辑接口版本号
#define AUCTION_LOGIC_MODULE_VERSION 0x3002

class IAuctionKernel;

// 定时器函数
typedef int (__cdecl* AUCTION_TIMER_FUNC)(IAuctionKernel* pPubKernel, const wchar_t* data_name, int time) ;
	
// 拍卖行数据核心接口

class IAuctionKernel
{
public:
	virtual ~IAuctionKernel() = 0;
	
	/// \brief 拍卖行数据
	/// \param name 拍卖行数据名称
	virtual bool AddAuctionData(const wchar_t* name, bool save) = 0;
	/// \brief 删除拍卖行数据
	/// \param name 拍卖行数据名
	virtual bool RemoveAuctionData(const wchar_t* name) = 0;
	/// \brief 查找拍卖行数据
	/// \param name 拍卖行数据名称
	virtual bool FindAuctionData(const wchar_t* name) = 0;
	/// \brief 获得拍卖行数据数量
	virtual int GetAuctionDataCount() = 0;
	/// \brief 获得拍卖行数据名称列表
	/// \param result 拍卖行数据名称列表
	virtual int GetAuctionDataList(IVarList& result) = 0;
	/// \brief 获得指定名称的拍卖行数据
	/// \param name 拍卖行数据名
	virtual IAuctionData* GetAuctionData(const wchar_t* name) = 0;
	/// \brief 保存拍卖行数据
	/// \param name 数据项名称
	virtual bool SaveAuctionData(const wchar_t* name) = 0;
		
	/// \brief 发消息到场景服务器
	/// \param source_id 服务器标识，为0表示0号场景服务器
	/// \param scene_id 场景编号，为0表示第一个场景
	/// \param msg 消息内容
	virtual bool SendAuctionMessage(int source_id, int scene_id, 
		const IVarList& msg) = 0;
	
	/// \brief 查找定时器
	/// \param name 定时器名称
	/// \param data_name 关联数据项名称
	virtual bool FindTimer(const char* name, const wchar_t* data_name) = 0;
	/// \brief 添加定时器
	/// \param name 定时器名称
	/// \param func 定时器函数指针
	/// \param data_name 关联数据项名称
	/// \param time 定时时长
	/// \param count 定时次数
	virtual bool AddTimer(const char* name, AUCTION_TIMER_FUNC func, 
		const wchar_t* data_name, int time, int count) = 0;
	/// \brief 删除定时器
	/// \param name 定时器名称
	/// \param data_name 关联数据项名称
	virtual bool RemoveTimer(const char* name, const wchar_t* data_name) = 0;
	
	/// \brief 请求获得主服务器状态和玩家数量信息
	virtual bool RequestServerState() = 0;
		
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

	/// \brief 发消息到指定场景
	/// \param scene_id 场景编号
	/// \param msg 消息内容
	virtual bool SendToScene(int scene_id, const IVarList& msg) = 0;
	
	/// \brief 发命令到指定名字的场景服务器玩家对象
	/// \param name 玩家名字
	/// \param msg 消息内容
	virtual bool CommandByName(const wchar_t* name, const IVarList& msg) = 0;
	
	/// \brief 获取World的统一时间
	virtual __time64_t GetWorldTime_64( void ) = 0;
	virtual time_t GetWorldTime( void ) = 0;
};

inline IAuctionKernel::~IAuctionKernel() {}

#endif // _SERVER_IPUBKERNEL_H

