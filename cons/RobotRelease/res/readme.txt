参数配置文件说明

1.service_config.xml
-- 机器人全局配置参数，运行期可更改。

2.service_param.xml
--  机器人参数配置。
-- 当前主要用于：场景行走路线（pathlist_time，pathlist）参数，切换场景（scene_config）配置

3.service_proc.xml
-- 测试流程，功能点配置。
-- 注意：当前最大支持20个，超出20个在控制菜单中不显示。可以使用valid="0"把不常用的功能屏蔽掉。

4.参考类配置文件，仅用于说明
-- service_msg.xml 自定义消息格式说明，开发人员自定义消息说明，和使用。
-- service_func.xml 自定义函数格式说明，开发人员提供/当前支持的自定义函数，及其使用说明。
