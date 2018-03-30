include "model.thrift"
include "exception.thrift"

namespace csharp world
namespace java jamboy.charge.common.protocol.thrift.rpc

service GMCCServer {
	 /**
	  * 获取区服信息
	  */
	 model.AreaInfo getAreaInfo(),

	 /**
	  * 关闭游戏服务器
	  */
	 bool closeServer(1:i32 serverId) throws (1:exception.InvalidOperationException ouch) ,

	/**
	 * 游戏状态变更
	 */
	bool modifyGameStatus(1:model.GameStatus status),

	/**
	 * 游戏阶段变更
	 */
	bool modifyGameStep(1:model.GameStep step),

	/**
     * 增加邮件 mailInfo的ID为空
	 */
	bool addMail(1:model.MailInfo mailInfo),

	/**
	 * 查询游戏角色信息
	 */
	model.RoleInfo queryRoleInfo(1:i64 roleId),

    /**
     * 发送GM命令， 踢掉在线玩家 -- 废弃由GM替代  禁止在线玩家发言 -- 废弃由GM替代
     */
    bool sendCommand(1:model.CommandInfo commandInfo),

	/**
	 * 发送系统通知
	 */
	bool sendSysNotice(1:model.SystemNotice systemMessage),

	/**
	 * 同步公告状态
	 */
	model.HandleResult syncNoticeStatus(1:i64 noticeId),

	/**
     * 订单通知
     */
    void notifyOrder(1:model.Order order),

	/**
     *  商品更新
     */
    void notifyGoods(),

    /**
     *  配置更新
     */
    void notifyConfig(1:model.ConfigType type, 2:i64 id),

    /**
     *  商城更新
     */
    void notifyShop(1:string id),
}
