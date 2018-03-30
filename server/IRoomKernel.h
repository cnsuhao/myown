//--------------------------------------------------------------------
// 文件名:		IKernel.h
// 内  容:		核心功能接口
// 说  明:		
// 创建日期:	2004年1月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IROOMKERNEL_H
#define _SERVER_IROOMKERNEL_H
#define _SERVER_IKERNEL_H


#include "../public/Macros.h"
#include "../public/Var.h"
#include "../public/VarList.h"
#include "LenType.h"
#include "RoomKnlConst.h"
#include "IGameObj.h"
#include "IRoomLogicModule.h"

#ifdef _DEBUG
	// 服务器游戏逻辑接口版本
	#define GAME_LOGIC_MODULE_VERSION 0x1000
#else
	// 服务器游戏逻辑接口版本
	#define GAME_LOGIC_MODULE_VERSION 0x1001
#endif




// 核心功能接口

class IKernel
{
public:
	virtual ~IKernel() = 0;

	/// \brief 获得资源文件路径
	virtual const char* GetResourcePath() = 0;

	/// \brief 获得指定名称的逻辑模块
	/// \param name 逻辑模块名称
	virtual IRoomLogicModule* GetLogicModule(const char* name) = 0;

	/// \brief 添加逻辑类
	/// \param logic_class 逻辑类名
	/// \param type 对象类型（见KnlConst.h）
	/// \param parent_class 父逻辑类
	virtual bool AddLogicClass(const char* logic_class, int type,
		const char* parent_class) = 0;
	/// \brief 添加逻辑类回调
	/// \param logic_class 逻辑类名
	/// \param event 事件名称
	/// \param func 回调函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddClassCallback(const char* logic_class, 
		const char* event, ROOM_LOGIC_CLASS_FUNC func, int prior = 0) = 0;
	/// \brief 添加逻辑事件回调
	/// \param logic_class 逻辑类名
	/// \param event 事件名称
	/// \param func 回调函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddEventCallback(const char* logic_class,
		const char* event, ROOM_LOGIC_EVENT_FUNC func, int prior = 0) = 0;
	/// \brief 添加字符串编号的对象命令处理函数
	/// \param logic_class 逻辑类名
	/// \param msg_id 消息编号
	/// \param func 处理函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddCommandHook(const char* logic_class,
		const char* msg_id, ROOM_LOGIC_EVENT_FUNC func, int prior = 0) = 0;
	/// \brief 添加整数编号的对象命令处理函数
	/// \param logic_class 逻辑类名
	/// \param msg_id 消息编号(0-1023)
	/// \param func 处理函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddIntCommandHook(const char* logic_class,
		int msg_id, ROOM_LOGIC_EVENT_FUNC func, int prior = 0) = 0;
	/// \brief 添加字符串编号的对象客户端消息处理函数
	/// \param logic_class 逻辑类名
	/// \param msg_id 消息编号
	/// \param func 处理函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddCustomHook(const char* logic_class,
		const char* msg_id, ROOM_LOGIC_EVENT_FUNC func, int prior = 0) = 0;
	/// \brief 添加整数编号的对象客户端消息处理函数
	/// \param logic_class 逻辑类名
	/// \param msg_id 消息编号(0-1023)
	/// \param func 处理函数指针
	/// \param prior 优先级（数值越小优先级越高）
	virtual bool AddIntCustomHook(const char* logic_class,
		int msg_id, ROOM_LOGIC_EVENT_FUNC func, int prior = 0) = 0;
	/// \brief 执行指定对象的事件回调函数
	/// \param event 事件名称
	/// \param self 本对象
	/// \param sender 关联对象
	/// \param args 参数列表
	virtual bool RunEventCallback(const char* event, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args) = 0;

	/// \brief 定义LUA扩展函数
	/// \param name 函数名
	/// \param func 函数指针
	virtual bool DeclareLuaExt(const char* name, ROOM_LUA_EXT_FUNC func) = 0;
	/// \brief 定义心跳函数
	/// \param name 函数名
	/// \param func 函数指针
	virtual bool DeclareHeartBeat(const char* name, ROOM_HEARTBEAT_FUNC func) = 0;
	/// \brief 定义属性回调函数
	/// \param name 函数名
	/// \param func 函数指针
	virtual bool DeclareCritical(const char* name, ROOM_CRITICAL_FUNC func) = 0;
	/// \brief 定义表格回调函数
	/// \param name 函数名
	/// \param func 函数指针
	virtual bool DeclareRecHook(const char* name, ROOM_RECHOOK_FUNC func) = 0;

	/// \brief 查找心跳函数
	/// \param name 函数名
	virtual ROOM_HEARTBEAT_FUNC LookupHeartBeat(const char* name) = 0;
	/// \brief 查找属性回调函数
	/// \param name 函数名
	virtual ROOM_CRITICAL_FUNC LookupCritical(const char* name) = 0;
	/// \brief 查找表格回调函数
	/// \param name 函数名
	virtual ROOM_RECHOOK_FUNC LookupRecHook(const char* name) = 0;

	/// \brief 输出到系统LOG文件
	/// \param info 信息内容
	virtual void Trace(const char* info) = 0;
	/// \brief 输出到控制台屏幕
	/// \param info 信息内容
	virtual void Echo(const char* info) = 0;
	
	/// \brief 过滤名字
	/// \param 名字
	virtual bool CheckName(const wchar_t* name) = 0;

	/// \brief 获得当前平台号
	virtual int GetPlatformId() = 0;
	/// \brief 获得场景服务器编号
	virtual int GetRoomId() = 0;

	/// \brief 获得对象指针
	/// \param id 对象号
	virtual IGameObj* GetGameObj(const PERSISTID& id) = 0;
	/// \brief 获得场景对象指针
	virtual IGameObj* GetSceneObj() = 0;

	/// \brief 取当前场景对象
	virtual PERSISTID GetScene() = 0;
	/// \brief 获得当前场景ID
	virtual int GetSceneId() = 0;
	/// \brief 获得最大的普通场景ID
	virtual int GetSceneMaxId() = 0;
	/// \brief 获得普通场景逻辑类名
	/// \param id 场景编号
	virtual const char* GetSceneScript(int id) = 0;
	/// \brief 获得普通场景配置名
	/// \param id 场景编号
	virtual const char* GetSceneConfig(int id) = 0;
	/// \brief 查找指定配置名的普通场景编号，返回-1表示没找到
	/// \param config 场景配置名
	virtual int FindSceneId(const char* config) = 0;
	/// \brief 判断指定编号的场景是否存在
	/// \param id 场景编号
	virtual bool GetSceneExists(int id) = 0;
	/// \brief 获得指定场景内的在线玩家数量
	/// \param id 场景编号
	virtual int GetSceneOnlineCount(int id) = 0;
	/// \brief 获得指定场景内的玩家数量（包括在线和离线）
	/// \param id 场景编号
	virtual int GetScenePlayerCount(int id) = 0;

	/// \brief 获得指定场景内的在线玩家名称列表（id为0表示所有场景）
	/// \param id 场景编号
	/// \param result 在线玩家名称列表
	virtual int GetSceneOnlinePlayerList(int id, IVarList& result) = 0;

	/// \brief 获得指定场景内的玩家名称列表
	/// \param id 场景编号
	/// \param result 玩家名称列表（内存玩家 包括在线和离线的）
	virtual int GetScenePlayerList(int id, IVarList& result) = 0;

	/// \brief 取当前场景的类型（0-普通场景，1-副本场景的原型，2-副本场景）
	virtual int GetSceneClass() = 0;
	/// \brief 请求创建副本场景，在OnCloneScene回调里返回结果
	/// \param prototype_scene_id 原型场景编号
	/// \param owner_name 所有者名称
	/// \param down_time 回收时间（秒）
	/// \param reuse 是否可以重复使用未回收的副本
	virtual bool RequestCloneScene(int prototype_scene_id, 
		const wchar_t* owner_name, int down_time, bool reuse) = 0;
	/// \brief 设置副本场景的回收时间
	/// \param clone_scene_id 副本场景编号
	/// \param down_time 回收时间（秒）
	virtual bool SetCloneSceneDownTime(int clone_scene_id, int down_time) = 0;
	/// \brief 获得指定原型场景号的副本数量
	/// \param prototype_scene_id 原型场景编号，为0表示返回所有的副本数量
	virtual int GetCloneSceneCount(int prototype_scene_id) = 0;
	/// \brief 获得指定原型场景号的副本列表
	/// \param prototype_scene_id 原型场景编号，为0表示返回所有的副本
	/// \param result 结果场景编号列表
	virtual int GetCloneSceneList(int prototype_scene_id, IVarList& result) = 0;
	/// \brief 获得副本场景的原型场景编号
	/// \param clone_scene_id 副本场景编号
	virtual int GetPrototypeSceneId(int clone_scene_id) = 0;
	/// \brief 指定场景号是否是副本场景的原型场景
	/// \param scene_id 普通场景编号
	virtual bool IsPrototypeScene(int scene_id) = 0;

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
	
	/// \brief 取地图范围
	/// \param left,top,right,bottom 返回左上和右下角的坐标
	virtual bool GetMapBound(len_t& left, len_t& top, len_t& right, 
		len_t& bottom) = 0;
	/// \brief 取地图高度y值（多层地图返回的是最高的高度）
	/// \param x,z 位置
	virtual len_t GetMapHeight(len_t x, len_t z) = 0;
	/// \brief 获得范围标记是否有效
	/// \param name 逻辑范围名
	/// \param x,z 位置
	virtual bool GetMapRegion(const char* name, len_t x, len_t z) = 0;
	/// \brief 取区域名称
	/// \param x,z 位置
	virtual const char* GetMapArea(len_t x, len_t z) = 0;
	/// \brief 取地表类型
	/// \param x,z 位置
	virtual int GetMapType(len_t x, len_t z) = 0;
	/// \brief 取行走类型（多层地图只返回当前位置是否有绿色主行走层）
	/// \param x,z 位置
	virtual int GetWalkType(len_t x, len_t z) = 0;
	/// \brief 测试一个点是否可行走区域（多层地图只返回当前位置是否有绿色主行走层）
	/// \param x,z 位置
	virtual bool CanWalk(len_t x, len_t z) = 0;
	/// \brief 测试一个点对象是否可行走（不可在多层地图使用此函数）
	/// \param obj 对象
	/// \param x,z 位置
	virtual bool ObjectCanWalk(const PERSISTID& obj, len_t x, len_t z) = 0;
	/// \brief 直线可行走（不可在多层地图使用此函数）
	/// \param step 步长
	/// \param src_x,src_z 起始点
	/// \param dst_x,dst_z 结束点
	virtual bool LineCanWalk(len_t step, len_t src_x, len_t src_z, 
		len_t dst_x, len_t dst_z) = 0;
	/// \brief 测试一条直线对象可行走（不可在多层地图使用此函数）
	/// \param obj 对象
	/// \param step 步长
	/// \param src_x,src_z 起始点
	/// \param dst_x,dst_z 结束点
	virtual bool ObjectLineCanWalk(const PERSISTID& obj, len_t step, 
		len_t src_x, len_t src_z, len_t dst_x, len_t dst_z) = 0;

	/// \brief 直线移动测试（能到达目标点返回true，否则返回false和可以到达的位置）
	/// \param walk_step 可行走高度差
	/// \param src_x,src_y,src_z 起始点
	/// \param dst_x,dst_z 目标点
	/// \return new_x,new_y,new_z 可以到达的点位置
	virtual bool TraceLineWalk(len_t walk_step, len_t src_x, len_t src_y,
		len_t src_z, len_t dst_x, len_t dst_z, len_t& new_x, len_t& new_y,
		len_t& new_z) = 0;

	/// \brief 碰撞数据是否就绪
	/// \param x,z 位置
	virtual bool GetCollideEnable(len_t x, len_t z) = 0;
	/// \brief 获得指定位置的最高点高度
	/// \param x,z 位置
	virtual len_t GetApexHeight(len_t x, len_t z) = 0;
	/// \brief 获得指定位置的最高点所在层
	/// \param x,z 位置
	virtual int GetApexFloor(len_t x, len_t z) = 0;
	/// \brief 获得是否可行走
	/// \param x,z 位置
	virtual bool GetWalkEnable(len_t x, len_t z) = 0;
	/// \brief 获得可行走的高度
	/// \param x,z 位置
	virtual len_t GetWalkHeight(len_t x, len_t z) = 0;
	/// \brief 水面是否存在
	/// \param x,z 位置
	virtual bool GetWalkWaterExists(len_t x, len_t z) = 0;
	/// \brief 获得精确的水面高度
	/// \param x,z 位置
	virtual len_t GetWalkWaterHeight(len_t x, len_t z) = 0;
	/// \brief 获得总的层数量
	/// \param x,z 位置
	virtual int GetFloorCount(len_t x, len_t z) = 0;
	/// \brief 获得层是否存在
	/// \param x,z 位置
	/// \param floor 层索引（0到floor_count-1）
	virtual bool GetFloorExists(len_t x, len_t z, int floor) = 0;
	/// \brief 获得层是否可移动
	/// \param x,z 位置
	/// \param floor 层索引（0到floor_count-1）
	virtual bool GetFloorCanMove(len_t x, len_t z, int floor) = 0;
	/// \brief 获得层是否可站立
	/// \param x,z 位置
	/// \param floor 层索引（0到floor_count-1）
	virtual bool GetFloorCanStand(len_t x, len_t z, int floor) = 0;
	/// \brief 获得层高度
	/// \param x,z 位置
	/// \param floor 层索引（0到floor_count-1）
	virtual len_t GetFloorHeight(len_t x, len_t z, int floor) = 0;
	/// \brief 获得层的无障碍空间高度
	/// \param x,z 位置
	/// \param floor 层索引（0到floor_count-1）
	virtual len_t GetFloorSpace(len_t x, len_t z, int floor) = 0;
	/// \brief 获得层是否有墙面
	/// \param x,z 位置
	/// \param floor 层索引（1到floor_count-1）
	virtual bool GetFloorHasWall(len_t x, len_t z, int floor) = 0;
	/// \brief 获得指定高度位置是否存在墙面
	/// \param x,y,z 位置
	virtual bool GetWallExists(len_t x, len_t y, len_t z) = 0;

	// 处理场景中的点列表
	/// \brief 保存场景中的点
	/// \param name 点名
	/// \param x,y,z,orient 位置和方向
	virtual bool SavePoint(const char* name, len_t x, len_t y, len_t z, 
		len_t orient) = 0;
	/// \brief 重设场景中的点
	/// \param name 点名
	/// \param x,y,z,orient 位置和方向
	virtual bool ResetPoint(const char* name, len_t x, len_t y, len_t z, 
		len_t orient) = 0;
	/// \brief 删除场景中的点
	/// \param name 点名
	virtual bool DeletePoint(const char* name) = 0;
	/// \brief 取得场景中的点坐标
	/// \param name 点名
	/// \param x,y,z,orient 返回的位置和方向
	virtual bool GetPointCoord(const char* name, len_t& x, len_t& y, 
		len_t& z, len_t& orient) = 0;
	/// \brief 获得场景中的所有点名称
	/// \param result 结果点名列表
	virtual int GetScenePointList(IVarList& result) = 0;

	// 处理场景中的对象列表
	/// \brief 加载场景中的对象配置文件
	/// \param ini_name 对象配置文件名
	virtual bool LoadObjects(const char* ini_name) = 0;
	/// \brief 保存对象
	/// \param name 对象名
	/// \param script 逻辑类名
	/// \param config 配置名
	/// \param point 场景中的点名
	/// \param capacity 容量
	virtual PERSISTID SaveObject(const char* name, const char* script, 
		const char* config, const char* point, int capacity) = 0;
	/// \brief 重设对象
	/// \param name 对象名
	/// \param script 逻辑类名
	/// \param config 配置名
	/// \param point 场景中的点名
	/// \param capacity 容量
	virtual PERSISTID ResetObject(const char* name, const char* script, 
		const char* config, const char* point, int capacity) = 0;
	/// \brief 删除对象
	/// \param name 对象名
	virtual bool DeleteObject(const char* name) = 0;
	
	/// \brief 场景中创建对象
	/// \param script 逻辑类名，如果为空则从配置数据里读取"script"键的值
	/// \param config 配置名
	/// \param capacity 容量
	/// \param x,y,z,orient 位置和方向
	virtual PERSISTID CreateObject(const char* script, const char* config, 
		int capacity, len_t x, len_t y, len_t z, len_t orient) = 0;
	/// \brief 带初始化参数场景中创建容器
	/// \param script 逻辑类名，如果为空则从配置数据里读取"script"键的值
	/// \param config 配置名
	/// \param capacity 容量
	/// \param x,y,z,orient 位置和方向
	/// \param args 创建参数列表
	virtual PERSISTID CreateObjectArgs(const char* script, 
		const char* config, int capacity, len_t x, len_t y, len_t z, 
		len_t orient, const IVarList& args) = 0;

	/// \brief 预加载对象配置数据集合文件
	/// \param name 数据集合文件名
	virtual bool PreloadConfig(const char* name) = 0;
	/// \brief 预加载TXT格式的对象配置数据集合文件
	/// \param name 数据集合文件名
	/// \param start_line 有效数据起始行号
	virtual bool PreloadConfigTxt(const char* name, int start_line = 3) = 0;
	/// \brief 查找预加载的配置属性
	/// \param config 配置名
	/// \param prop 属性名
	virtual bool FindConfigProperty(const char* config, const char* prop) = 0;
	/// \brief 读取预加载的配置属性值
	/// \param config 配置名
	/// \param prop 属性名
	virtual const char* GetConfigProperty(const char* config, 
		const char* prop) = 0;
	/// \brief 设置预加载的配置属性值
	/// \param config 配置名
	/// \param prop 属性名
	/// \param value 属性值
	virtual bool SetConfigProperty(const char* config, const char* prop, 
		const char* value) = 0;
	/// \brief 获得配置的属性名列表
	/// \param config 配置名
	/// \param result 返回的属性名列表
	virtual int GetConfigPropertyList(const char* config, IVarList& result) = 0;
	/// \brief 查找预加载的配置表格
	/// \param config 配置名
	/// \param rec 表格名
	virtual bool FindConfigRecord(const char* config, const char* rec) = 0;
	/// \brief 读取预加载的配置表格值
	/// \param config 配置名
	/// \param rec 表格名
	/// \param rows 表格行数据（字符串）
	virtual int GetConfigRecord(const char* config, const char* rec, 
		IVarList& result) = 0;
	/// \brief 设置预加载的配置表格值
	/// \param config 配置名
	/// \param rec 表格名
	/// \param rows 表格行数据（字符串）
	virtual bool SetConfigRecord(const char* config, const char* rec, 
		const IVarList& rows) = 0;
	/// \brief 获得配置的表格名列表
	/// \param config 配置名
	/// \param result 返回的表格名列表
	virtual int GetConfigRecordList(const char* config, IVarList& result) = 0;
	/// \brief 装载对象的配置数据
	/// \param obj 本对象
	/// \param config 配置名
	/// \param reload 是否要重新加载文件
	virtual bool LoadConfig(const PERSISTID& obj, const char* config, 
		bool reload = false) = 0;
	/// \brief 在容器中创建对象
	/// \param obj 容器对象
	/// \param script 逻辑类名，如果为空则从配置数据里读取"script"键的值
	/// \param config 配置名
	/// \param capacity 容量
	virtual PERSISTID CreateFromConfig(const PERSISTID& obj, 
		const char* script, const char* config, int capacity = 0) = 0;
	/// \brief 带初始化参数在容器中创建对象
	/// \param obj 容器对象
	/// \param script 逻辑类名，如果为空则从配置数据里读取"script"键的值
	/// \param config 配置名
	/// \param args 创建参数列表
	/// \param capacity 容量
	virtual PERSISTID CreateFromConfigArgs(const PERSISTID& obj, 
		const char* script, const char* config, const IVarList& args, 
		int capacity = 0) = 0;

	/// \brief 获得逻辑类的索引值（返回-1表示未找到）
	/// \param class_name 逻辑类名
	virtual int GetClassIndex(const char* class_name) = 0;
	/// \brief 获得逻辑类的属性索引值（返回-1表示未找到，用索引值操作属性的函数在IGameObj中）
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	virtual int GetClassAttrIndex(int class_index, const char* name) = 0;
	/// \brief 增加属性
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	/// \param type 数据类型
	/// \param saving 是否要保存
	/// \param not_save_empty 是否为空值时不保存到数据库
	/// \return 返回属性索引，失败返回-1
	virtual int Add(int class_index, const char* name, int type, 
		bool saving = false, bool not_save_empty = false) = 0;
	/// \brief 添加可视属性
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	/// \param type 数据类型
	/// \param vis_type 可视类型（整数类型可以设置"BYTE"，"WORD"，其他可以为空）
	/// \param bPublic 公共域是否可见
	/// \param bPrivate 私有域是否可见
	/// \param saving 是否要保存
	/// \param not_save_empty 是否为空值时不保存到数据库
	/// \return 返回属性索引，失败返回-1
	virtual int AddVisible(int class_index, const char* name, int type, 
		const char* vis_type, bool bPublic, bool bPrivate, 
		bool saving = false, bool not_save_empty = false) = 0;
	/// \brief 设置属性可视
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	/// \param vis_type 可视类型（整数类型可以设置"BYTE"，"WORD"，其他可以为空）
	/// \param bPublic 公共域是否可见
	/// \param bPrivate 私有域是否可见
	virtual bool SetVisible(int class_index, const char* name, 
		const char* vis_type, bool bPublic, bool bPrivate) = 0;
	/// \brief 可视属性是否即时刷新
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	/// \param realtime 是否要及时刷新
	virtual bool SetRealtime(int class_index, const char* name, 
		bool realtime) = 0;
	/// \brief 属性是否保存
	/// \param class_index 逻辑类索引
	/// \param name 属性名
	/// \param saving 是否要保存
	virtual bool SetSaving(int class_index, const char* name, bool saving) = 0;
	
	/// \brief 添加一个表
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param cols 列数
	/// \param max_rows 最大行数
	/// \param saving 是否保存
	/// \param not_save_empty 表格数据为空时不需要保存
	virtual bool AddRecord(int class_index, const char* name, int cols, 
		int max_rows, bool saving = false, bool not_save_empty = false) = 0;
	/// \brief 为表设置一个列索引键（列类型必须为字符串或宽字符串）
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param col 索引列号
	/// \param case_insensitive 是否大小写不敏感
	virtual bool SetRecordKey(int class_index, const char* name, int col,
		bool case_insensitive = false) = 0;
	/// \brief 设置列属性，表名，列序号，数据类型
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param col 列序号
	/// \param type 数据类型
	virtual bool SetRecordColType(int class_index, const char* name, int col, 
		int type) = 0;
	/// \brief 表是否显示
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param bPublic 公共域是否可见
	/// \param bPrivate 私有域是否可见
	virtual bool SetRecordVisible(int class_index, const char* name, 
		bool bPublic, bool bPrivate) = 0;
	/// \brief 设置列的显示类型
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param col 列序号
	/// \param vis_type 可视类型（设置为"HIDE"表示此列不可见，整数类型可以设置"BYTE"，"WORD"，其他可以为空）
	virtual bool SetRecordColVisType(int class_index, const char* name, 
		int col, const char* vis_type) = 0;
	/// \brief 表是否保存
	/// \param class_index 逻辑类索引
	/// \param name 表格名
	/// \param saving 是否保存
	/// \param not_save_empty 表格数据为空时不需要保存
	virtual bool SetRecordSaving(int class_index, const char* name, 
		bool saving, bool not_save_empty = false) = 0;

	/// \brief 获得全局场景切换点的场景号和坐标信息
	/// \param location 全局切换点名
	/// \param scene 返回的场景编号
	/// \param x,y,z,orient 返回的位置和方向
	virtual bool GetSwitchLocation(const char* location, int& scene, 
		len_t& x, len_t& y, len_t& z, len_t& orient) = 0;
	/// \brief 切换场景到指定的场景号和坐标
	/// \param obj 玩家对象
	/// \param scene 场景编号
	/// \param x,y,z,orient 位置和方向
	virtual bool SwitchLocate(const PERSISTID& obj, int scene, len_t x, 
		len_t y, len_t z, len_t orient) = 0;
	/// \brief 切换到指定场景的出生点
	/// \param obj 玩家对象
	/// \param scene 场景编号
	virtual bool SwitchBorn(const PERSISTID& obj, int scene) = 0;
	/// \brief 设置场景的出生点位置
	/// \param x,y,z,orient 位置和方向
	virtual bool SetSceneBorn(len_t x, len_t y, len_t z, len_t orient) = 0;
	/// \brief 获得场景的出生点位置
	/// \param x,y,z,orient 返回的位置和方向
	virtual bool GetSceneBorn(len_t& x, len_t& y, len_t& z, len_t& orient) = 0;

	/// \brief 两对象的二维空间距离（x, z）
	/// \param obj 对象一
	/// \param other 对象二
	virtual len_t Distance2D(const PERSISTID& obj, const PERSISTID& other) = 0;
	/// \brief 两对象的三维空间距离（x, y, z）
	/// \param obj 对象一
	/// \param other 对象二
	virtual len_t Distance3D(const PERSISTID& obj, const PERSISTID& other) = 0;
	/// \brief 一个对象相对本对象前方向的角度
	/// \param obj 本对象
	/// \param other 其他对象
	virtual len_t Angle(const PERSISTID& obj, const PERSISTID& other) = 0;
	/// \brief 一个点相对本对象前方向的角度
	/// \param obj 本对象
	/// \param x,z 点位置
	virtual len_t DotAngle(const PERSISTID& obj, len_t x, len_t z) = 0;

	/// \brief 切换到本场景的指定位置
	/// \param 本对象
	/// \param x,y,z,orient 位置和方向
	virtual bool MoveTo(const PERSISTID& obj, len_t x, len_t y, len_t z, 
		len_t orient) = 0;
	/// \brief 定位到本场景的指定位置（效果和MoveTo相似，只是发送的是Moving消息）
	/// \param 本对象
	/// \param x,y,z,orient 位置和方向
	virtual bool Locate(const PERSISTID& obj, len_t x, len_t y, len_t z, 
		len_t orient) = 0;
	/// \brief 旋转角度
	/// \param obj 对象
	/// \param angle 旋转角度
	/// \param rotate_speed 转身速度
	/// \param mode 附加运动模式
	virtual bool Rotate(const PERSISTID& obj, len_t angle, 
		len_t rotate_speed, int mode = 0) = 0;
	/// \brief 控制对象在地表移动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Motion(const PERSISTID& obj, len_t move_speed, 
		len_t roate_speed, len_t x, len_t z, int mode = 0) = 0; 
	/// \brief 控制对象在地表移动，不做旋转
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool MotionNoRotate(const PERSISTID& obj, len_t move_speed, 
		len_t x, len_t z, int mode = 0) = 0;
	/// \brief 控制对象跳跃
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param jump_speed 起跳速度
	/// \param gravity 重力加速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Jump(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t jump_speed, len_t gravity, len_t x, len_t z,
		int mode = 0) = 0;
	/// \brief 改变对象跳跃的速度和目标方向
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool JumpTo(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t x, len_t z, int mode = 0) = 0;
	/// \brief 控制对象在空中移动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,y,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Fly(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t x, len_t y, len_t z, int mode = 0) = 0;
	/// \brief 控制对象在水中移动
	/// \param 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,y,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Swim(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t x, len_t y, len_t z, int mode = 0) = 0;
	/// \brief 控制对象在水面移动
	/// \param 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Drift(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t x, len_t z, int mode = 0) = 0;
	/// \brief 控制对象在墙面移动
	/// \param 对象
	/// \param move_speed 移动速度
	/// \param x,y,z 目标点位置
	/// \param orient 人物面向墙面的方向
	/// \param mode 附加运动模式
	virtual bool Climb(const PERSISTID& obj, len_t move_speed, len_t x, 
		len_t y, len_t z, len_t orient, int mode = 0) = 0;
	/// \brief 控制对象滑行
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Slide(const PERSISTID& obj, len_t move_speed, 
		len_t roate_speed, len_t x, len_t z, int mode = 0) = 0; 
	/// \brief 控制对象下沉
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sink_speed 初始下沉速度
	/// \param floatage 浮力加速度
	/// \param x,z 目标点位置
	/// \param mode 附加运动模式
	virtual bool Sink(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sink_speed, len_t floatage, len_t x, 
		len_t z, int mode = 0) = 0;
	/// \brief 停止运动（移动和旋转）
	/// \param obj 对象
	/// \param mode 附加运动模式
	virtual bool Stop(const PERSISTID& obj, int mode = 0) = 0;
	/// \brief 停止移动
	/// \param obj 对象
	virtual bool StopWalk(const PERSISTID& obj) = 0;
	/// \brief 停止旋转
	/// \param obj 对象
	virtual bool StopRotate(const PERSISTID& obj) = 0;

	/// \brief 校验水平方向移动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckMotion(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dz,
		int mode = 0) = 0;
	/// \brief 校验跳跃
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param jump_speed 起跳速度
	/// \param gravity 重力加速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckJump(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t jump_speed, len_t gravity, len_t sx, 
		len_t sy, len_t sz, len_t dx, len_t dz, int mode = 0) = 0;
	/// \brief 校验跳跃中改变目标方向
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckJumpTo(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dz,
		int mode = 0) = 0;
	/// \brief 校验空中运动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dy,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckFly(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dy, 
		len_t dz, int mode = 0) = 0;
	/// \brief 校验水中运动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dy,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckSwim(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dy, 
		len_t dz, int mode = 0) = 0;
	/// \brief 校验水面运动
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckDrift(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dz, 
		int mode = 0) = 0;
	/// \brief 校验爬墙
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dy,dz 目标点位置
	/// \param orient 人物面向墙面的方向
	/// \param mode 附加运动模式
	virtual bool CheckClimb(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dy, 
		len_t dz, len_t orient, int mode = 0) = 0;
	/// \brief 校验滑行
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckSlide(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sx, len_t sy, len_t sz, len_t dx, len_t dz, 
		int mode = 0) = 0;
	/// \brief 校验下沉
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param rotate_speed 转身速度
	/// \param sink_speed 初始下沉速度
	/// \param floatage 浮力加速度
	/// \param sx,sy,sz 起始位置
	/// \param dx,dz 目标点位置
	/// \param mode 附加运动模式
	virtual bool CheckSink(const PERSISTID& obj, len_t move_speed, 
		len_t rotate_speed, len_t sink_speed, len_t floatage, len_t sx, 
		len_t sy, len_t sz, len_t dx, len_t dz, int mode = 0) = 0;
	/// \brief 校验停止
	/// \param obj 对象
	/// \param move_speed 移动速度
	/// \param x,y,z,orient 停止时的目标和方向
	/// \param mode 附加运动模式
	virtual bool CheckStop(const PERSISTID& obj, len_t move_speed, len_t x, 
		len_t y, len_t z, len_t orient, int mode = 0) = 0;

	/// \brief 连接到其他对象
	/// \param obj 本对象
	/// \param link 被连接的对象
	/// \param x,y,z,orient 连接的相对位置和方向
	virtual bool LinkTo(const PERSISTID& obj, const PERSISTID& link, 
		len_t x, len_t y, len_t z, len_t orient) = 0;
	/// \brief 改变连接到其他对象的位置
	/// \param obj 本对象
	/// \param link 被连接的对象
	/// \param x,y,z,orient 连接的相对位置和方向
	virtual bool LinkMove(const PERSISTID& obj, const PERSISTID& link, 
		len_t x, len_t y, len_t z, len_t orient) = 0;
	/// \brief 取消连接到其他对象
	/// \param obj 本对象
	virtual bool Unlink(const PERSISTID& obj) = 0;
	/// \brief 获得当前连接的其他对象
	/// \param obj 本对象
	virtual PERSISTID GetLinkObject(const PERSISTID& obj) = 0;
	/// \brief 获得当前连接到其他对象的位置
	/// \param obj 本对象
	/// \param x,y,z,orient 位置和朝向
	virtual bool GetLinkPosition(const PERSISTID& obj, len_t& x, len_t& y,
		len_t& z, len_t& orient) = 0;

	/// \brief 添加对象的视野优先级,该策略仅在当前场景有效。
	/// \param obj 本对象
	/// \param prior 优先看见的对象
	/// \param force 强制添加(如到数量上限就按照先进先出原则进行移除添加)
	virtual bool AddVisualPriority(const PERSISTID& obj, const PERSISTID& prior, bool force = false) = 0;
	/// \brief 移除对象的视野优先级
	/// \param obj 本对象
	/// \param prior 优先看见的对象
	virtual bool RemoveVisualPriority(const PERSISTID& obj, const PERSISTID& prior) = 0;
	/// \brief 获取对象的视野优先级的对象列表
	/// \param obj 本对象
	/// \param result 视野优先级列表
	virtual int GetVisualPriority(const PERSISTID& obj, IVarList& result) = 0;

	/// \brief 对象坐标X
	/// \param obj 对象
	virtual len_t GetPosiX(const PERSISTID& obj) = 0;
	/// \brief 对象坐标Y
	/// \param obj 对象
	virtual len_t GetPosiY(const PERSISTID& obj) = 0;
	/// \brief 对象坐标Z
	/// \param obj 对象
	virtual len_t GetPosiZ(const PERSISTID& obj) = 0;
	/// \brief 对象方向
	/// \param obj 对象
	virtual len_t GetOrient(const PERSISTID& obj) = 0;
	/// \brief 取对象坐标和方向
	/// \param obj 对象
	/// \param x,y,z,orient 目标和方向返回值
	virtual bool GetLocation(const PERSISTID& obj, len_t& x, len_t& y, 
		len_t& z, len_t& orient) = 0;
	/// \brief 取对象当前所在层（0-15普通层，200水面，300空中，400水下）
	/// \param obj 对象
	virtual int GetFloor(const PERSISTID& obj) = 0;

	/// \brief 目标点X
	/// \param obj 对象
	virtual len_t GetDestX(const PERSISTID& obj) = 0;
	/// \brief 目标点Y
	/// \param obj 对象
	virtual len_t GetDestY(const PERSISTID& obj) = 0;
	/// \brief 目标点Z
	/// \param obj 对象
	virtual len_t GetDestZ(const PERSISTID& obj) = 0;
	/// \brief 目标方向
	/// \param obj 对象
	virtual len_t GetDestOrient(const PERSISTID& obj) = 0;
	/// \brief 移动模式
	/// \param obj 对象
	virtual int GetMoveMode(const PERSISTID& obj) = 0;
	/// \brief 移动速度
	/// \param obj 对象
	virtual len_t GetMoveSpeed(const PERSISTID& obj) = 0;
	/// \brief 旋转速度
	/// \param obj 对象
	virtual len_t GetRotateSpeed(const PERSISTID& obj) = 0;
	/// \brief 起跳速度
	/// \param obj 对象
	virtual len_t GetJumpSpeed(const PERSISTID& obj) = 0;
	/// \brief 重力加速度
	/// \param obj 对象
	virtual len_t GetGravity(const PERSISTID& obj) = 0;

	/// \brief 创建一个子对象
	/// \param obj 父容器对象
	/// \param script 逻辑类名
	virtual PERSISTID Create(const PERSISTID& obj, const char* script) = 0;
	/// \brief 带初始化参数创建一个子对象
	/// \param obj 父容器对象
	/// \param script 逻辑类名
	/// \param args 创建参数
	virtual PERSISTID CreateArgs(const PERSISTID& obj, const char* script, 
		const IVarList& args) = 0;
	/// \brief 创建一个容器
	/// \param obj 父容器对象
	/// \param script 逻辑类名
	/// \param capacity 容量
	virtual PERSISTID CreateContainer(const PERSISTID& obj, 
		const char* script, int capacity) = 0;
	/// \brief 带初始化参数创建一个容器
	/// \param obj 父容器对象
	/// \param script 逻辑类名
	/// \param capacity 容量
	/// \param args 创建参数
	virtual PERSISTID CreateContainerArgs(const PERSISTID& obj, 
		const char* script, int capacity, const IVarList& args) = 0;
	/// \brief 增大容器的容量
	/// \param obj 容器对象
	/// \param capacity 目标容量
	virtual bool ExpandContainer(const PERSISTID& obj, int capacity) = 0;
	/// \brief 缩小容器的容量
	/// \param obj 容器对象
	/// \param capacity 目标容量
	virtual bool ShrinkContainer(const PERSISTID& obj, int capacity) = 0;
	/// \brief 复制对象(目标容器，对象原型)
	/// \param container 目标容器对象
	/// \param prototype 原型对象
	/// \param pos 放置在目标容器中的位置
	virtual PERSISTID CreateClone(const PERSISTID& container, 
		const PERSISTID& prototype, int pos) = 0;
	/// \brief 在指定地点创建子对象
	/// \param obj 容器对象
	/// \param script 逻辑类名
	/// \param x,y,z,orient 位置和方向
	/// \param capacity 容量
	virtual PERSISTID CreateTo(const PERSISTID& obj, const char* script, 
		len_t x, len_t y, len_t z, len_t orient, int capacity = 0) = 0;
	/// \brief 带初始化参数在指定地点创建子对象
	/// \param obj 容器对象
	/// \param script 逻辑类名
	/// \param x,y,z,orient 位置和方向
	/// \param args 创建参数
	/// \param capacity 容量
	virtual PERSISTID CreateToArgs(const PERSISTID& obj, const char* script, 
		len_t x, len_t y, len_t z, len_t orient, const IVarList& args, 
		int capacity = 0) = 0;
	/// \brief 销毁一个子对象(父对象，子对象)
	/// \param obj 父对象
	/// \param target 被销毁的子对象
	virtual bool Destroy(const PERSISTID& obj, const PERSISTID& target) = 0;
	/// \brief 销毁对象自身
	/// \param obj 被销毁的对象
	virtual bool DestroySelf(const PERSISTID& obj) = 0;
	/// \brief 选择对象触发本对象的OnDoSelect回调和目标对象的OnSelect回调
	/// \param self 本对象
	/// \param obj 目标对象
	/// \param funcid 功能编号
	virtual int Select(const PERSISTID& self, const PERSISTID& obj, 
		int funcid) = 0;
	
	/// \brief 设置对象是否不保存到数据库
	/// \param obj 对象
	/// \param value 是否不保存
	virtual bool SetUnsave(const PERSISTID& obj, bool value) = 0;
	/// \brief 获得对象是否不保存到数据库
	/// \param obj 对象
	virtual bool GetUnsave(const PERSISTID& obj) = 0;

	/// \brief 将对象移动另一个容器中
	/// \param obj 被移动的对象
	/// \param container 容器对象
	virtual bool Place(const PERSISTID& obj, const PERSISTID& container) = 0;
	/// \brief 将对象放到容器的指定位置
	/// \param obj 被移动的对象
	/// \param container 容器对象
	/// \param pos 容器中的位置（从1开始）
	virtual bool PlacePos(const PERSISTID& obj, const PERSISTID& container, 
		int pos) = 0;
	/// \brief 将对象放到场景的指定位置
	/// \param obj 被移动的对象
	/// \param container 场景容器对象
	/// \param x,y,z,orient 位置和方向
	virtual bool PlaceTo(const PERSISTID& obj, const PERSISTID& container, 
		len_t x, len_t y, len_t z, len_t orient) = 0;
	/// \brief 容器之间交换指定位置上的子对象
	/// \param container1 容器一
	/// \param pos1 容器一的位置
	/// \param container2 容器二
	/// \param pos2 容器二的位置
	virtual bool Exchange(const PERSISTID& container1, int pos1, 
		const PERSISTID& container2, int pos2) = 0;
	/// \brief 移动对象到容器的指定位置
	/// \param obj 被移动的对象
	/// \param new_pos 新的容器位置（从1开始）
	virtual bool ChangePos(const PERSISTID& obj, int new_pos) = 0;

	/// \brief 返回容器的容量
	/// \param container 容器对象
	virtual int GetCapacity(const PERSISTID& container) = 0;
	/// \brief 取对象脚本名
	/// \param obj 对象
	virtual const char* GetScript(const PERSISTID& obj) = 0;
	/// \brief 取对象配置名
	/// \param obj 对象
	virtual const char* GetConfig(const PERSISTID& obj) = 0;
	/// \brief 取对象在容器中的位置
	/// \param obj 对象
	/// \return 从1开始
	virtual int GetIndex(const PERSISTID& obj) = 0;

	/// \brief 获得对象类型
	/// \param obj 对象
	/// \return 对象类型（见KnlConst.h）
	virtual int Type(const PERSISTID& obj) = 0;
	/// \brief 对象是否存在
	/// \param obj 对象
	virtual bool Exists(const PERSISTID& obj) = 0;
	/// \brief 取父对象
	/// \param obj 对象
	virtual PERSISTID Parent(const PERSISTID& obj) = 0;
	/// \brief 获得被弱关联的次数
	/// \param obj 对象
	virtual int GetWeakRefs(const PERSISTID& obj) = 0;
	/// \brief 获得被弱关联的容器列表
	/// \param obj 对象
	/// \param result 容器列表
	virtual int GetWeakBoxList(const PERSISTID& obj, IVarList& result) = 0;

	/// \brief 取容器指定位置的子对象
	/// \param obj 容器对象
	/// \param pos 容器中的位置（从1开始）
	virtual PERSISTID GetItem(const PERSISTID& obj, int pos) = 0;
	/// \brief 用名字查找子对象
	/// \param obj 容器对象
	/// \param name 子对象的名字
	virtual PERSISTID GetChild(const PERSISTID& obj, const wchar_t* name) = 0;
	/// \brief 获得第一个子对象
	/// \param obj 容器对象
	/// \param it 迭代器变量
	virtual PERSISTID GetFirst(const PERSISTID& obj, unsigned int& it) = 0;
	/// \brief 获得下一个子对象
	/// \param obj 容器对象
	/// \param it 迭代器变量
	virtual PERSISTID GetNext(const PERSISTID& obj, unsigned int& it) = 0;
	/// \brief 返回子对象数量
	/// \param obj 容器对象
	virtual int GetChildCount(const PERSISTID& obj) = 0;
	/// \brief 获得子对象列表
	/// \param obj 容器对象
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param result 结果对象列表
	virtual int GetChildList(const PERSISTID& obj, int classtype, 
		IVarList& result) = 0;
	/// \brief 清除所有子对象
	/// \param obj 容器对象
	virtual bool ClearChild(const PERSISTID& obj) = 0;
	/// \brief 查找指定名称和类型的子对象
	/// \param obj 容器对象
	/// \param name 子对象的名字
	/// \param classtype 对象类型（为0表示所有类型）
	virtual PERSISTID FindChild(const PERSISTID& obj, const wchar_t* name, 
		int classtype) = 0;
	/// \brief 查找多个指定名称和类型的子对象，返回数量
	/// \param obj 容器对象
	/// \param name 子对象的名字
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param result 结果对象列表
	virtual int FindChildMore(const PERSISTID& obj, const wchar_t* name, 
		int classtype, IVarList& result) = 0;
	/// \brief 使用配置名获得子对象
	/// \param obj 容器对象
	/// \param config 配置名
	/// \param classtype 对象类型（为0表示所有类型）
	virtual PERSISTID FindChildByConfig(const PERSISTID& obj, 
		const char* config, int classtype) = 0;
	/// \brief 使用配置名获得多个子对象
	/// \param obj 容器对象
	/// \param config 配置名
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param result 结果对象列表
	virtual int FindChildMoreByConfig(const PERSISTID& obj, 
		const char* config, int classtype, IVarList& result) = 0;

	/// \brief 弱关联容器添加引用对象
	/// \param obj 引用对象
	/// \param container 弱关联容器
	/// \param pos 位置（从1开始）
	virtual bool AddWeakChild(const PERSISTID& obj, 
		const PERSISTID& container, int pos) = 0;
	/// \brief 弱关联容器移除引用对象
	/// \param obj 引用对象
	/// \param container 弱关联容器
	virtual bool RemoveWeakChild(const PERSISTID& obj, 
		const PERSISTID& container) = 0;
	/// \brief 弱关联容器清空引用对象
	/// \param container 弱关联容器
	virtual bool ClearWeakChild(const PERSISTID& container) = 0;
	
	/// \brief 获得指定对象周围的对象列表
	/// \param obj 中心对象
	/// \param radius 半径
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param max 最大数量（为0表示不限制数量）
	/// \param result 结果对象列表
	/// \param invisible 是否包含不可见物体
	virtual int GetAroundList(const PERSISTID& obj, len_t radius, 
		int classtype, int max, IVarList& result, bool invisible = false) = 0;
	/// \brief 获得点周围的对象列表
	/// \param obj 对象（在非分组场景可以为空）
	/// \param x,z 位置
	/// \param radius 半径
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param max 最大数量（为0表示不限制数量）
	/// \param result 结果对象列表
	/// \param invisible 是否包含不可见物体
	virtual int GetPointAroundList(const PERSISTID& obj, len_t x, len_t z, 
		len_t radius, int classtype, int max, IVarList& result, 
		bool invisible = false) = 0;

	/// \brief 获得指定对象周围3D范围内的对象列表
	/// \param obj 中心对象
	/// \param radius 半径
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param max 最大数量（为0表示不限制数量）
	/// \param result 结果对象列表
	/// \param invisible 是否包含不可见物体
	virtual int GetAroundList3D(const PERSISTID& obj, len_t radius, 
		int classtype, int max, IVarList& result, bool invisible = false) = 0;
	/// \brief 获得点周围3D范围内的对象列表
	/// \param obj 对象（在非分组场景可以为空）
	/// \param x,y,z 位置
	/// \param radius 半径
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param max 最大数量（为0表示不限制数量）
	/// \param result 结果对象列表
	/// \param invisible 是否包含不可见物体
	virtual int GetPointAroundList3D(const PERSISTID& obj, len_t x, len_t y,
		len_t z, len_t radius, int classtype, int max, IVarList& result, 
		bool invisible = false) = 0;

	/// \brief 获得与指定线段碰撞的对象列表
	/// \param obj 对象（在非分组场景可以为空）
	/// \param beg_x,beg_y,beg_z 线段起始点
	/// \param end_x,end_y,end_z 线段结束点
	/// \param line_radius 线段半径
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param max 最大数量（为0表示不限制数量）
	/// \param result 结果对象列表
	/// \param invisible 是否包含不可见物体
	virtual int TraceObjectList(const PERSISTID& obj, len_t beg_x, 
		len_t beg_y, len_t beg_z, len_t end_x, len_t end_y, len_t end_z, 
		len_t line_radius, int classtype, int max, IVarList& result, 
		bool invisible = false) = 0;
	
	/// \brief 获得唯一的组号
	virtual int NewGroupId() = 0;
	/// \brief 获得指定组的第一个子对象
	/// \param group 组号
	/// \param it 迭代器变量
	virtual PERSISTID GroupFirst(int group, unsigned int& it) = 0;
	/// \brief 获得指定组的下一个子对象
	/// \param group 组号
	/// \param it 迭代器变量
	virtual PERSISTID GroupNext(int group, unsigned int& it) = 0;
	/// \brief 获得指定组的子对象列表
	/// \param group 组号
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param result 结果对象列表
	virtual int GroupChildList(int group, int classtype, IVarList& result) = 0;
	/// \brief 查找组内指定名字和类型的对象
	/// \param group 组号
	/// \param name 子对象的名字
	/// \param classtype 对象类型（为0表示所有类型）
	virtual PERSISTID GroupFind(int group, const wchar_t* name, 
		int classtype) = 0;
	/// \brief 查找组内多个指定名字和类型的对象
	/// \param group 组号
	/// \param name 子对象的名字
	/// \param classtype 对象类型（为0表示所有类型）
	/// \param result 结果对象列表
	virtual int GroupFindMore(int group, const wchar_t* name, int classtype, 
		IVarList& result) = 0;
	/// \brief 清除组中的非玩家对象
	/// \param group 组号
	virtual bool ClearGroup(int group) = 0;
	
	/// \brief 添加心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// 注：所加的函数参数是固定的，如下
	/// int func(const PERSISTID& obj, int time) 
	/// time的单位为毫秒
	// 对象被删除时，它身上的各种回调会被自动删除(包括心跳)
	virtual bool AddHeartBeat(const PERSISTID& obj, const char* func, 
		int time) = 0;
	/// \brief 添加计数心跳
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// \param count 心跳次数
	virtual bool AddCountBeat(const PERSISTID& obj, const char* func, 
		int time, int count) = 0;
	/// \brief 删除心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool RemoveHeartBeat(const PERSISTID& obj, const char* func) = 0;
	/// \brief 清除心跳函数
	/// \param obj 对象
	virtual bool ClearHeartBeat(const PERSISTID& obj) = 0;
	/// \brief 查找心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool FindHeartBeat(const PERSISTID& obj, const char* func) = 0;
	/// \brief 获得心跳时间
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetBeatTime(const PERSISTID& obj, const char* func) = 0;
	/// \brief 设置心跳次数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param count 心跳次数
	virtual bool SetBeatCount(const PERSISTID& obj, const char* func, 
		int count) = 0;
	/// \brief 获得心跳次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetBeatCount(const PERSISTID& obj, const char* func) = 0;
	/// \brief 查找关键属性回调是否存在
	/// \param obj 对象
	/// \param property 属性名
	/// \param func 回调函数名
	virtual bool FindCritical(const PERSISTID& obj, const char* property, 
		const char* func) = 0;
	/// \brief 添加关键属性回调：
	/// \param obj 对象
	/// \param property 属性名
	/// \param func 回调函数名
	/// 注：所加的函数参数是固定的，如下
	/// int func(IKernel* pKernel, const PERSISTID& obj, const char* property, const IVar& old_value)
	/// old_value为属性改变前的值
	virtual bool AddCritical(const PERSISTID& obj, const char* property, 
		const char* func) = 0;
	/// \brief 删除关键属性回调
	/// \param obj 对象
	/// \param property 属性名
	virtual bool RemoveCritical(const PERSISTID& obj, const char* property) = 0;
	/// \brief 删除指定函数名的关键属性回调
	/// \param obj 对象
	/// \param property 属性名
	/// \param func 回调函数名
	virtual bool RemoveCriticalFunc(const PERSISTID& obj, 
		const char* property, const char* func) = 0;
	
	/// \brief 查找表钩子回调是否存在
	/// \param obj 对象
	/// \param record 表格名称
	/// \param func 回调函数名
	virtual bool FindRecHook(const PERSISTID& obj, const char* record, 
		const char* func) = 0;
	/// \brief 添加表钩子回调：
	/// \param obj 对象
	/// \param record 表格名称
	/// \param func 回调函数名
	/// 注：所加的函数参数是固定的，如下
	/// int func(IKernel* pKernel, const PERSISTID& obj, const char* record, 
	///		int op_type, int row, int col)
	/// op_type为表操作类型，row,col是行列值
	virtual bool AddRecHook(const PERSISTID& obj, const char* record, 
		const char* func) = 0;
	/// \brief 删除表钩子回调
	/// \param obj 对象
	/// \param record 表格名称
	virtual bool RemoveRecHook(const PERSISTID& obj, const char* record) = 0;
	/// \brief 删除指定函数名的表钩子回调
	/// \param obj 对象
	/// \param record 表格名称
	/// \param func 回调函数名
	virtual bool RemoveRecHookFunc(const PERSISTID& obj, const char* record,
		const char* func) = 0;

	/// \brief 给玩家添加容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	/// \param container 容器对象
	virtual bool AddViewport(const PERSISTID& player, int id, 
		const PERSISTID& container) = 0;
	/// \brief 玩家删除容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool RemoveViewport(const PERSISTID& player, int id) = 0;
	/// \brief 玩家的视窗是否存在
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool FindViewport(const PERSISTID& player, int id) = 0;
	/// \brief 获得玩家视窗对应的容器
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual PERSISTID GetViewportContainer(const PERSISTID& player, int id) = 0;
	/// \brief 清除玩家所有视窗
	/// \param player 玩家对象
	virtual bool ClearViewport(const PERSISTID& player) = 0;
	/// \brief 取容器的视窗数
	/// \param container 容器对象
	virtual int GetViewers(const PERSISTID& container) = 0;
	/// \brief 关闭容器的所有视窗
	/// \param container 容器对象
	virtual bool CloseViewers(const PERSISTID& container) = 0;
	
	/// \brief 普通聊天
	/// \param obj 玩家或其他场景内对象
	/// \param info 聊天信息
	virtual void Speech(const PERSISTID& obj, const wchar_t* info) = 0;
	
	/// \brief 设置登陆点名
	/// \param obj 玩家对象
	/// \param point 全局点名（为空表示清除登陆点）
	virtual bool SetLandPoint(const PERSISTID& obj, const char* point) = 0;
	/// \brief 设置登陆位置
	/// \param obj 玩家对象
	/// \param scene 场景编号
	/// \param x,y,z,orient 位置和方向
	virtual bool SetLandPosi(const PERSISTID& obj, int scene, len_t x, 
		len_t y, len_t z, len_t orient) = 0;
	

	/// \brief 在当前场景内查找玩家
	/// \param name 玩家名字
	virtual PERSISTID FindPlayer(const wchar_t* name) = 0;
	/// \brief 取玩家所在场景号
	/// \param name 玩家名字
	virtual int GetPlayerScene(const wchar_t* name) = 0;
	/// \brief 取游戏总玩家数量
	virtual int GetPlayerCount() = 0;


	/// \brief 发系统信息到玩家客户端
	/// \param type 消息类型
	/// \param info 信息内容
	virtual bool SysInfo(const PERSISTID& player, int type, 
		const wchar_t* info) = 0;
	/// \brief 发系统信息到指定名字的玩家客户端
	/// \param name 玩家名字
	/// \param type 消息类型
	/// \param info 信息内容
	virtual bool SysInfoByName(const wchar_t* name, int type, 
		const wchar_t* info) = 0;
	/// \brief 发系统信息到指定对象视野范围内的玩家客户端
	/// \param obj 参考对象
	/// \param type 消息类型
	/// \param info 信息内容
	virtual bool SysInfoByKen(const PERSISTID& obj, int type, 
		const wchar_t* info) = 0;

	/// \brief 发自定义消息到客户端
	/// \param player 玩家对象
	/// \param msg 消息参数列表
#ifdef _DEBUG
	virtual bool Custom(const PERSISTID& player, const IVarList& msg) = 0;
#else
	virtual bool Custom(const PERSISTID& player, const IVarList& msg, CUSTOM_MERGE_FLAG_ENUM merge_flag = CMF_NOT_MERGE) = 0;
#endif
	/// \brief 发自定义消息到客户端
	/// \param player 玩家对象
	/// \param msg1 消息参数列表1
	/// \param msg2 消息参数列表2
	virtual bool Custom2(const PERSISTID& player, const IVarList& msg1, 
		const IVarList& msg2) = 0;
	/// \brief 发自定义消息到指定名字玩家的客户端
	/// \param name 玩家名字
	/// \param msg 消息参数列表
	virtual bool CustomByName(const wchar_t* name, const IVarList& msg) = 0;
	/// \brief 发自定义消息到指定名字玩家的客户端
	/// \param name 玩家名字
	/// \param msg1 消息参数列表1
	/// \param msg2 消息参数列表2
	virtual bool CustomByName2(const wchar_t* name, const IVarList& msg1, 
		const IVarList& msg2) = 0;
	/// \brief 发自定义消息到指定对象视野内玩家的客户端
	/// \param obj 参考对象
	/// \param msg 消息参数列表
#ifdef _DEBUG
	virtual bool CustomByKen(const PERSISTID& obj, const IVarList& msg) = 0;
#else
	virtual bool CustomByKen(const PERSISTID& obj, const IVarList& msg, CUSTOM_MERGE_FLAG_ENUM merge_flag = CMF_NOT_MERGE) = 0;
#endif
	/// \brief 发自定义消息到当前房间内所有玩家的客户端
	/// \param msg 消息参数列表
	virtual bool CustomByRoom(const IVarList& msg) = 0;
	/// \brief 发自定义消息到指定组内所有玩家的客户端
	/// \param group 组号
	/// \param msg 消息参数列表
	virtual bool CustomByGroup(int group, const IVarList& msg) = 0;

	/// \brief 发命令给对象
	/// \param obj 发出命令的对象
	/// \param target 接收命令的对象
	/// \param msg 命令参数列表
	virtual int Command(const PERSISTID& obj, const PERSISTID& target, 
		const IVarList& msg) = 0;
	/// \brief 发命令给指定名字的玩家对象
	/// \param name 玩家名字
	/// \param msg 命令参数列表
	virtual int CommandByName(const wchar_t* name, const IVarList& msg) = 0;
	/// \brief 发命令给指定对象视野内的玩家对象
	/// \param obj 参考对象
	/// \param msg 命令参数列表
	virtual int CommandByKen(const PERSISTID& obj, const IVarList& msg) = 0;
	/// \brief 发命令给指定场景内的玩家对象
	/// \param scene 场景编号
	/// \param msg 命令参数列表
	virtual int CommandByScene(int scene, const IVarList& msg) = 0;
	/// \brief 发命令给指定组内的玩家对象
	/// \param group 组号
	/// \param msg 命令参数列表
	virtual int CommandByGroup(int group, const IVarList& msg) = 0;
	/// \brief 发命令给指定场景的某个组的玩家对象
	/// \param scene 场景编号
	/// \param group 组号
	/// \param msg 命令参数列表
	virtual int CommandBySceneGroup(int scene, int group, 
		const IVarList& msg) = 0;

	/// \brief 发命令给指定场景对象
	/// \param scene 场景编号
	/// \param msg 命令参数列表
	virtual int CommandToScene(int scene, const IVarList& msg) = 0;

	
	/// \brief 保存游戏日志
	/// \param name 名字
	/// \param type 类型
	/// \param content 内容
	/// \param comment 备注
	virtual bool SaveLog(const wchar_t* name, int type, 
		const wchar_t* content, const wchar_t* comment) = 0;
	/// \brief 保存物品日志
	/// \param id 物品编号
	/// \param name 物品名字
	/// \param user 用户名字
	/// \param op 物品操作
	/// \param reason 操作原因
	/// \param comment 备注
	virtual bool ItemLog(const wchar_t* id, const wchar_t* name, 
		const wchar_t* user, int op, const wchar_t* reason, 
		const wchar_t* comment) = 0;
	/// \brief 保存聊天日志
	/// \param from 发出聊天信息的玩家名
	/// \param to 接收聊天信息的玩家名
	/// \param type 信息类型
	/// \param content 内容
	/// \param comment 备注
	virtual bool ChatLog(const wchar_t* from, const wchar_t* to, int type, 
		const wchar_t* content, const wchar_t* comment) = 0;
	/// \brief 保存GM操作日志
	/// \param name 名字
	/// \param type 类型
	/// \param content 内容
	/// \param comment 备注
	virtual bool GmLog(const wchar_t* name, int type, const wchar_t* content, 
		const wchar_t* comment) = 0;
	/// \brief 保存自定义表格式的日志（自动生成日志流水号）
	/// \param table_name 日志数据库表名
	/// \param fields 数据字段内容
	/// 字段数据类型：整数(int)，数值(double)，字符串，宽字符串，日期字符串（格式"YYYYMMDD hh:mm:ss"）
	virtual bool CustomLog(const char* table_name, const IVarList& fields) = 0;
	/// \brief 保存与角色同时存储的自定义表格式日志（自动生成日志流水号）
	/// \param table_name 日志数据库表名
	/// \param fields 数据字段内容
	/// 字段数据类型：整数(int)，数值(double)，字符串，宽字符串，日期字符串（格式"YYYYMMDD hh:mm:ss"）
	virtual bool CustomLogWithRole(const char* table_name, 
		const IVarList& fields) = 0;
	
	/// \brief 加载LUA脚本
	/// \param script 脚本名
	virtual bool LuaLoadScript(const char* script) = 0;
	/// \brief 查找LUA脚本
	/// \param script 脚本名
	virtual bool LuaFindScript(const char* script) = 0;
	/// \brief 查找LUA脚本的函数
	/// \param script 脚本名
	/// \param func 函数名
	virtual bool LuaFindFunc(const char* script, const char* func) = 0;
	/// \brief 同步运行LUA脚本函数（res_num返回值的数量，如为-1表示不定数量）
	/// \param script 脚本名
	/// \param func 函数名
	/// \param args 参数列表
	/// \param res_num 需要返回的结果数量
	/// \param result 函数执行结果（可以为空）
	virtual bool LuaRunFunc(const char* script, const char* func, 
		const IVarList& args, int res_num = 0, IVarList* result = NULL) = 0;
	/// \brief LUA脚本错误捕获
	/// \param state 脚本状态机
	/// \param error 错误信息 
	virtual void LuaErrorHandler(void* state, const char* error) = 0;
	/// \brief 获得LUA脚本参数数量
	/// \param state 脚本状态机
	virtual int LuaGetArgCount(void* state) = 0;
	/// \brief 判断LUA脚本参数类型
	/// \param state 脚本状态机
	/// \param index 参数索引（从1开始）
	virtual bool LuaIsInt(void* state, int index) = 0;
	virtual bool LuaIsInt64(void* state, int index) = 0;
	virtual bool LuaIsFloat(void* state, int index) = 0;
	virtual bool LuaIsDouble(void* state, int index) = 0;
	virtual bool LuaIsString(void* state, int index) = 0;
	virtual bool LuaIsWideStr(void* state, int index) = 0;
	virtual bool LuaIsObject(void* state, int index) = 0;
	/// \brief 获得LUA脚本参数
	/// \param state 脚本状态机
	/// \param index 参数索引（从1开始）
	virtual int LuaToInt(void* state, int index) = 0;
	virtual int64_t LuaToInt64(void* state, int index) = 0;
	virtual float LuaToFloat(void* state, int index) = 0;
	virtual double LuaToDouble(void* state, int index) = 0;
	virtual const char* LuaToString(void* state, int index) = 0;
	virtual const wchar_t* LuaToWideStr(void* state, int index) = 0;
	virtual PERSISTID LuaToObject(void* state, int index) = 0;
	/// \brief 压入LUA脚本执行结果（按从左到右的顺序）
	/// \param state 脚本状态机
	/// \param value 结果数据
	virtual void LuaPushBool(void* state, bool value) = 0;
	virtual void LuaPushNumber(void* state, double value) = 0;
	virtual void LuaPushInt(void* state, int value) = 0;
	virtual void LuaPushInt64(void* state, int64_t value) = 0;
	virtual void LuaPushFloat(void* state, float value) = 0;
	virtual void LuaPushDouble(void* state, double value) = 0;
	virtual void LuaPushString(void* state, const char* value) = 0;
	virtual void LuaPushWideStr(void* state, const wchar_t* value) = 0;
	virtual void LuaPushObject(void* state, const PERSISTID& value) = 0;
	virtual void LuaPushTable(void* state, const IVarList& value) = 0;


	/// \brief 查找运营配置信息
	/// \param id 键名
	virtual bool FindManageInfo(const char* id) = 0;
	/// \brief 获得运营配置信息键值
	/// \param id 键名
	virtual const wchar_t* GetManageInfoName(const char* id) = 0;
	virtual int GetManageInfoType(const char* id) = 0;
	virtual const wchar_t* GetManageInfoProperty(const char* id) = 0;
	virtual int GetManageInfoStatus(const char* id) = 0;
	virtual int GetManageInfoVersion(const char* id) = 0;
	/// \brief 获得运营配置信息当前最大的版本号
	virtual int GetManageInfoMaxVersion() = 0;
	/// \brief 更新基本配置信息键值（nx_config表）
	/// \param id 键名
	/// \param name 名字(可选)
	/// \param prop 键值
	/// \param type 分类
	/// \param status 状态
	virtual bool UpdateManageBase(const char* id, const wchar_t* name, 
		int type, const wchar_t* prop, int status) = 0;
	/// \brief 更新运营配置信息键值（nx_config表）
	/// \param id 键名
	/// \param name 名字（可选）
	/// \param prop 键值
	/// \param type 分类
	/// \param status 状态
	/// \param version 版本号
	virtual bool UpdateManageInfo(const char* id, const wchar_t* name, 
		int type, const wchar_t* prop, int status, int version) = 0;
	/// \brief 删除运营配置信息键值
	/// \param id 键名
	virtual bool RemoveManageInfo(const char* id) = 0;
	/// \brief 获得所有的运营配置信息键名
	/// \param result 键名字符串结果列表
	virtual int GetManageInfoList(IVarList& result) = 0;
	/// \brief 获得所有大于指定版本号的的运营配置信息键名
	/// \brief version 版本号
	/// \param result 键名字符串结果列表
	virtual int GetManageInfoListAboveVersion(int version, 
		IVarList& result) = 0;

	/// \brief 获取计数心跳当前次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetCountBeatIndex(const PERSISTID& obj, const char* func) = 0;

	/// \brief 战场服务相关操作
 
	/// \brief 战斗结束， 传回战斗结果
 	/// \param name 角色名
	/// \param command 结果参数集 (在OnRequestLeaveRoom() 回调中原样返回)
	/// \param bDelRole 是否删除角色( true 删除角色 false 在房间被销毁的时候统一删除)
	virtual bool  RequestLeaveRoom(const wchar_t* name, const IVarList& command, bool bDelRole = false) = 0;

	/// \brief 发送custom 消息到member
	/// \brief room_id	房间id
	/// \brief role_uid 角色uid
	/// \brief command 结果参数集
	virtual bool SendCustomToMeber(int room_id, const char* role_uid, const IVarList& command) = 0;

	/// \brief 发送custom 消息到 platform
	/// \brief command 结果参数集
	virtual bool SendCustomToPlatform(const IVarList& command) = 0;
	/// \brief 发送消息到附加服务器
	/// \param index 附加服务器索引
	/// \param room_id 房间id
	/// \param msg 消息参数列表
	virtual bool SendExtraMessage(int index, const IVarList& msg) = 0;
	/// \brief 格子相关
	/// \brief 获得路径格子大小
	virtual len_t GetPathGridSize() = 0;
	/// \brief 获得格子行数
	virtual size_t GetPathGridRows() = 0;
	/// \brief 获得格子列数
	virtual size_t GetPathGridCols() = 0;
	/// \brief 获得格子占有对象
	/// \brief 返回值对象数量
	/// \param ret 返回对象PERSISTID
	virtual int GetPathGridBalker(size_t index, IVarList& ret, int object_groupid) = 0;
	/// \brief 逻辑占用格子（对于已经在格子的对象不影响）
	virtual void SetPathGridLogicBalk(size_t index, bool balk) = 0;
	/// \brief 查询指定格子状态
	/// \brief 返回值 索引非法返回 false
	/// \param index 索引
	/// \param can_walk 地形是否可走
	/// \param logic_balk 是否逻辑占用
	/// \param object_occupy 是否对象占用
	virtual bool QueryPathGridStatus(size_t index, bool& can_walk, bool& logic_balk, bool& object_occupy, int object_groupid) = 0;
	/// \brief 获得对象所在格子
	virtual void QueryObjectGridIndex(const PERSISTID& obj, IVarList& ret) = 0;
	/// \brief 获得坐标所在格子
	virtual int GetGridIndexByPos(len_t x,len_t z) = 0;
	/// \brief 获得格子所对应的坐标
	virtual void GetPosByIndex(int index, len_t& x, len_t& z) = 0;
	/// \brief 获得周边的格子 
	/// \param index,
	/// \param ret,返回周边有效格子索引;
	virtual void GetAroundGridIndex(int index, IVarList& ret) = 0;

	/// \brief对象是否能站立在某点
	/// \return MOTION_RESULT_ENUM 里的值
	/// \param x 
	/// \param z 
	/// \param ret 返回占用的格子
	virtual int CanStandPointByPathGrid(const PERSISTID& obj,len_t x,len_t z, IVarList& ret) = 0;

	/// \brief获得寻路路径	
	/// \briefA* 寻路 比较耗，逻辑层慎用，返回值是消耗的毫秒数
	/// \param nBeginIndex 起点格子索引 （左上角是0，右下角是最大格子数-1）
	/// \param nEndIndex 目标格子索引 （左上角是0，右下角是最大格子数-1）
	/// \param path 返回的结果，有可能为空
	/// \param get_range 距离起点最大搜索范围，0则全地图
	/// \param max_try 最大尝试次数 0 则不受限制，目标点不可去切场景有点大会导致假死
	/// \param slash 是否支持斜线（这将导致探索方位增加一倍，寻路时间稍微长一点点点点，斜线穿过条件是反斜线都可走才可穿过）
	/// \param nFormula  启发式算法，目前支持（0,1）0是曼哈顿算法，经大量测试效果稍微好些，项目上可以用 0 和 1 测试，不同的地图结果可能不同
	/// \param close_path  在目的点不可走情况下是否返回最近路程（开启有明显消耗）

	virtual int GetPath(int nBeginIndex, int nEndIndex, int object_groupid, IVarList & path, 
		unsigned int get_range = 5, unsigned int max_try = 50,	int slash = 1, int nFormula = 0, int close_path =0) = 0;

#ifndef _DEBUG
	/// \brief 通过玩家uid,获取服务器名称，
	/// \param uid	玩家id
	virtual const wchar_t* GetServerNameByPlayerUid( const char *uid) = 0;
#endif

};

inline IKernel::~IKernel() {}

#endif // _SERVER_IKERNEL_H
