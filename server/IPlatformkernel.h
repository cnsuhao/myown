//--------------------------------------------------------------------
// 文件名:		IPlatformKernel.h
// 内  容:		平台逻辑核心接口
// 说  明:		
// 创建日期:	2013年11月26日	
// 创建人:		丁有进
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IPLATFORMKERNEL_H
#define _SERVER_IPLATFORMKERNEL_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
//#include "IPlatformData.h"

// 平台数据逻辑接口版本号
#define PLATFORM_LOGIC_MODULE_VERSION 0x1001

class IPlatformKernel;
class IRecord;

// 定时器函数
typedef void (*Platform_TIMER_FUNC)(void* context, int timer_id, float seconds) ;


// sqlite 结果集
typedef int (*sqlite3_callback)(void*, int, char**, char**);

typedef int (*sqlite3_rs_callback)(void*, int, int, char**);


// 平台逻辑核心接口

class IPlatformKernel
{
public:

	virtual ~IPlatformKernel() = 0;

	/// \brief 获得资源文件路径
	virtual const char* GetResourcePath() = 0;

	/// \brief sqlite 操作 无返回值类型
	/// \param pSql sql语句
	virtual bool Excute(const char* pSql)	= 0;

	/// \brief sqlite 操作有返回值类型
	/// \param pSql	  sql语句
	/// \param pback  回调函数
	/// \param pdata  
	/// \param errmsg 错误信息
	virtual bool Excute(const char* pSql, sqlite3_callback pback, void* pdata, char** perrmsg) =0;	

	/// \brief sqlite 操作有返回结果集
	/// \param pSql	  sql语句
	/// \param pback  回调函数
	/// \param pdata  
	/// \param errmsg 错误信息
	virtual bool Excute(const char* pSql, sqlite3_rs_callback pback, void* pdata, char**perrmsg) = 0;

	/// \brief sqlite 操作 
	/// \param pSql sql语句
	/// \return IRecord（注意 IRecord数据都是string 类型）
	virtual IRecord* ExcuteEx(const char* pSql) = 0;

	/// \brief 获取一个房间号
	virtual int GetRoomId() = 0;

	/// \brief 进入战斗房间
	/// \param room_id  房间id
	/// \param map_id   场景地图
	/// \param role_uid 角色id列表
	/// \param args		附加参数
	/// \param bPoll	是否使用轮询的方式（默认引擎底层使用动态负载方式分配）
	virtual bool EntryRoom(int room_id, int map_id, const char* role_uid, const IVarList & args, bool bPoll = false) = 0;

	/// \brief 发送自定义消息到 member
	/// \param role_uid 角色uid
	/// \param args 参数集 （player的OnCustomPlatform 回调）
	virtual bool SendCustomToMember(const char* role_uid, const IVarList& args) = 0;

	/// \brief 发送消息到指定服务器 
	/// \param server_id  指定的服务器id （-1 发给所有服务器）
	/// \param args 参数集 （第一场景的 OnCommand 回调）
	virtual bool SendCustomToMember(int server_id, const IVarList& args) = 0;

	/// \brief 发送消息到指定服务器的uid角色 
	/// \param world_id  指定的世界服务器id
	/// \param role_uid 指定的角色uid
	/// \param args 参数集 （GamePlayer的 OnCustomPlatform 回调），world_id与role_uid由逻辑自己保证匹配，否者不保证能收到自定义消息
	virtual bool SendCustomToMember(int world_id, const char* role_uid,const IVarList& args) = 0;

	/// \brief 发送自定义消息到 room
	/// \param room_id 房间id
	/// \param role_uid 指定的角色uid
	/// \param args 参数集（GamePlayer的 OnCustomPlatform 回调）
	virtual bool SendCustomToRoom(int room_id, const char* role_uid, const IVarList& args) = 0;

	/// \brief 发送自定义消息到 room
	/// \param room_id 房间id
	/// \param args 参数集（场景的 OnCommand 回调）
	virtual bool SendCustomToRoom(int room_id, const IVarList& args) = 0;

	/// \brief 设置房间时长
	/// \param room_id 房间id
	/// \param nSeconds 房间的秒数
	virtual bool SetRoomContinueTime(int room_id, int nSeconds) = 0;

	/// \brief 添加心跳函数
	/// \param func 心跳函数名
	/// \param time 心跳时长(建议大于 0.005秒)
	/// 注：所加的函数参数是固定的，如下
	/// int func(void* context, int timer_id, float seconds) 
	/// time的单位为秒
	/// \ret 返回心跳索引， 如果是 -1， 说明心跳添加失败
	virtual int AddHeartBeat(Platform_TIMER_FUNC func, float seconds) = 0;

	/// \brief 删除心跳
	/// \param time_id  心跳索引
	virtual bool RemoveHeartBeat(int timer_id) = 0;

	//\brief  房间开打人数
	virtual bool SetRoomMaxPlayers(int room_id, int nAmount)  = 0;
	
	// \brief 获取world 服务器的个数
	virtual int  GetWorldNumber()   = 0;
	
	// \brief 获取战斗服务器列表，返回server_id
	virtual bool  GetBattleServerList(IVarList& args)   = 0;

	// \brief 获取战斗服务器里面的房间列表
	// \param room_server_id room服务器id
	// \param res 返回的房间号列表
	virtual int GetBattleRoomList(int room_server_id, IVarList& res) = 0;

	// \brief 获取战斗服的房间数
	virtual int  GetBattleServerRooms(int server_id)   = 0;

	// \brief 获取房间内玩家个数
	virtual int  GetRoomPlayers(int room_id)   = 0;

	// \brief 获取战斗服内玩家个数
	virtual int  GetBattleServerPlayers(int server_id)   = 0;

	/// \brief 发送消息到附加服务器
	/// \param index 附加服务器索引
	/// \param room_id 房间id
	/// \param msg 消息参数列表
	virtual bool SendExtraMessage(int index, const IVarList& msg)=0; 	

	/// \brief 输出到系统LOG文件
	/// \param info 信息内容
	virtual void Trace(const char* info) = 0;
	/// \brief 输出到控制台屏幕
	/// \param info 信息内容
	virtual void Echo(const char* info) = 0;

};

inline IPlatformKernel::~IPlatformKernel() {}

#endif // _SERVER_IPlatformKERNEL_H

