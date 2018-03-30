namespace csharp world
namespace java jamboy.charge.common.protocol.thrift.model

/**
 * 操作枚举
 */ 
enum Operation {
  CLOSE = 1,
  START = 2,
  RESTART = 3,
  LOAD_DATA = 4
}

/**
 * 服务器状态
 */
enum Status {
  CLOSING = 1, //关闭进行中
  CLOSED = 2,  //己关闭
  OPENING = 3, //打开中
  OPENED = 4, //己打开
  DEAD  = 5  //己死亡
}

/**
 * 服务运行状态
 */
enum OperationStatus {
  NORMAL = 1, //正常 
  IDLE  = 2, //空闲
  BUSY = 3 //繁忙
}

/**
 * 游戏状态
 */
enum GameStatus {
  READY = 1, //准备开放 
  OPEN  = 2, //开放
  CLOSING = 3, //即将停服
  CLOSED = 4 //停服维护中
}


/**
 * 游戏阶段
 */
enum GameStep {
  INNER_TEST = 1, //内测试阶段 
  OUTER_TEST  = 2, //对外测试阶段 
  NOMAL = 3 //正试运营阶段 
}

/**
 * 系统通知分类
 */
enum SystemNoticeType {
  NOTICE = 1, //公告
  ACTIVITY  = 2, //活动
  ITEMORDER = 3, //道具订单
  SIGNIN = 4,//签到
  STOREITEM = 5,//商城道具信息
  CONFIG = 6,//游戏配置
}

/**
 * 操作类型
 */
enum OperateType {
  ADD = 0,//增加
  DEL  = 1, //删除
  EDIT = 2, //更改
  HANDLED  = 9,//需要被处理
}


/**
 * 操作状态 （用于异步）
 */
enum OperateStatus {
  UNSTART = 0,//操作还未开始
  DOING = 1, //操作进行中
  COMPLETED  = 9, //操作己完成
}

/**
 * 数据变化状态 
 */
enum ChangeStatus {
  UNCHANGE = 1,//未变化
  CHANGED = 9 //己变化
}

/**
 * 邮件类型  公告 消息 奖励
 */
enum MailType {
  NOMAL = 1,//普通
  NOTICE = 2, //通知
}

enum ConfigType {
  ACTIVITY = 1, // 活动
}

/**
 * 处理结果  
 */
struct HandleResult {
	1: OperateStatus operateStatus,
	2: ChangeStatus changeStatus,
	3: optional string mess,
	4: optional i32  code,
}

/**
 * 系统通知
 */
struct SystemNotice {
	1: optional string content, //通知内容 可选  可设置公告ID等
	2: SystemNoticeType resourceType, //通知分类
	3: optional string roleId, //通知接收的角色ID 可选 
	4: optional i64 accId, //通知接收的数字账号 可选 
	5: OperateType  operateType ,//操作类型 
}

/**
 * 机器配置信息
 */
struct MachineConfig {
  1: string ip ,
  2: i32 port,
  3: string user,
  4: string pwd,
}

/**
 * 游戏服务信息
 */
struct GameServerInfo {
  1: string name ,
  2: i64 serverId,
  4: OperationStatus operationStatus,
  5: Status status,
  6: optional i32 currentCnt,
  7: optional MachineConfig configure,
}

/**
 * 邮件信息
 */
struct MailInfo {
  1: string title ,  //标题
  2: optional i64 id,  //邮件ID 可选
  3: string content, //内容 
  4: i64 createTime, //创建时间
  5: bool isAttach, //是否有附件
  6: optional string  attachInfo, //附件信息，可选
  7: MailType type, //类型，邮件类型 
  8: optional string senderId, //发送者 可选 -1为系统 其它为角色ID
  9: optional list<string> recieverId, //如果为系统邮件的话 为NULL ,可为多个接收者ROLEID
  10: optional string config, //配置
}

/**
 *GM命令信息
 */
struct CommandInfo{
    1:string roleId, //被执行的角色ID
	2:string command, //命令 
	3:list<string> cmdArgs, //运营平台组装cmd命令和 gameserver的 gm命令格式兼容
	4:string operateRoleId, //操作角色ID ，系统管理员用-1表示
	5:i64 cmdId,	// 命令序列id
}

/**
 * 角色信息
 */
struct RoleInfo{
    1:i32 areaId,    //区服ID
	2:bool isOnline,   //是否在线
	3:string name,     //角色名称
}

/**
 *  区服信息
 */
struct AreaInfo{
    1:i32 areaId,      //区服ID
	2:optional string name,   //区服务名称
	3:optional list<GameServerInfo> serverList ,   //所有服务器列表
	4:i32 currentCnt, //当前在线人数
}

/**
 * 订单
 */
struct Order {
    1:string roleId,     // 角色 Id
    2:string orderId,    // 订单 Id
    3:string type,       // 订单类型
    4:string goodsCode,  // 商品编号
    5:double price,      // 订单价格
    6:string state,      // 订单状态
    7:optional i64 createAt,// 创建时间
    8:optional i64 payAt,// 支付时间
}

