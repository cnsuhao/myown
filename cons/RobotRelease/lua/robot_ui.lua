-- 文件名: robot_ui.lua
-- 说明: ui 使用函数定义
-- 作者:
-- 版权所有:

require("./robot_def")
require("./robot_ui_def")
require("./robot_cmd_define")
require("./robot_menu_cmd_func")

--点击菜单相应处理
function on_new_menue_clicked( menu_id )
    if menu_id >= M_ID_ROBOT_PROCESS_BASE and menu_id<= M_ID_ROBOT_PROCESS_BASE+ROBOT_PROCESS_CUSTOM_COUNT then
        menu_custom_task_case(menu_id)
        return true
    end
	-- 调用message handler
	local cmd_handler = tb_menu_cmd_handler[menu_id]
	if cmd_handler ~= nil then
		cmd_handler()
	end
    return true
end

--弹出菜单前处理
function on_new_popup_menue()
    -- 计费注册
	ex_add_menu_item( M_ID_CHARGE_REGISTER, "Charge=>计费注册", false,2)
    -- 计费登陆
	ex_add_menu_item( M_ID_CHARGE_LOGIN, "Charge=>计费登陆", false,2)
	-- 登录
	ex_add_menu_item( M_ID_LOGIN, "Normal=>普通登录", false,1)
	-- 创建角色
	ex_add_menu_item( M_ID_CREATE_ROLE, "创建角色", false)
	-- 选择角色
	ex_add_menu_item( M_ID_CHOOSE_ROLE, "选择角色", false)
    -- 退出账号
	ex_add_menu_item( M_ID_LOG_OUT, "退出账号", false)
    ex_add_menu_item( 0, "", false)
    
    -- 切换场景
    ex_add_menu_item( M_ID_ROBOT_SWITCH_SCENE, "切换场景", false)
    -- GM命令 
    ex_add_menu_item( M_ID_ROBOT_GM_CMD, "GM命令", false)
    -- 自定义消息
    ex_add_menu_item( M_ID_ROBOT_MSG_CMD, "自定义消息", false)
    ex_add_menu_item( 0, "", false)

    -- 开始行走
	ex_add_menu_item( M_ID_START_MOVE, "开始行走", false)
	ex_add_menu_item( MD_ID_STOP_MOVE, "停止行走", false) 
    ex_add_menu_item( 0, "", false)
    -- 流程
    ex_add_menu_item( M_ID_STOP_TASK_WORK, "停止任务", false)
    ex_add_menu_item( M_ID_ROBOT_PROCESS_BASE, "流程", false)
   

	
	-- 付费
	-- ex_add_menu_item( M_ID_PAY_ITEM, "虚拟币付费", false)
	
	-- ------------------------------------------------------------------------
	-- gm 背包 80
	--ex_add_menu_item( M_ID_GM_FULL_PACK, "gm 背包 80", false)
	-- gm 背包 20 上下浮动
	--ex_add_menu_item( M_ID_GM_ADJUST_PACK, "gm 背包 70 上下浮动", false)
	-- gm 背包 40
	--ex_add_menu_item( M_ID_GM_NORMAL_PACK, "gm 背包 40", false)
	-- gm 背包 10 上下浮动
	--ex_add_menu_item( M_ID_GM_ADJUST_PACK_40, "gm 背包 40 上下浮动", false)
	-- 一身神装
	--ex_add_menu_item( M_ID_GM_PERFECT_EQUIP, "gm 一身神装", false)

	-- 请求切换到克隆场景战斗
	--ex_add_menu_item( M_ID_SWITCH_TO_SCENE, "请求切换到克隆场景战斗", false)


	-- 创建房间
	--ex_add_menu_item( M_ID_CREATE_ROOM, "创建房间", false)
	-- 进入房间
	--ex_add_menu_item( M_ID_JOIN_ROOM, "进入房间", false)
	-- 开始副本
	--ex_add_menu_item( M_ID_BEGIN_FIGHT, "开始副本", false)

	-- 离线加载1-1000号机器人
	--ex_add_menu_item( M_ID_GM_LOAD_OFFLINE_ROBOT, "gm 离线加载1-1000号机器人", false)

	-- 开始接交任务
	--ex_add_menu_item( M_ID_BEGIN_TASK, "开始接交任务", false)
	-- 停止交接任务
	--ex_add_menu_item( M_ID_END_TASK, "停止交接任务", false)

	--排名竞技场
	--ex_add_menu_item( M_ID_GET_ARENA_INFO, "打开排名竞技界面", false)

	--开始测量逻辑延迟
	--ex_add_menu_item( M_ID_BEGIN_MEASURE_LOGIC_LATENCY, "开始测量逻辑延迟", false)
	--停止测量逻辑延迟
	--ex_add_menu_item( M_ID_END_MEASURE_LOGIC_LATENCY, "停止测量逻辑延迟", false)

	--创建公会
	--ex_add_menu_item( M_ID_CREATE_GUILD, "创建公会", false)
	--公会数据塞满
	--ex_add_menu_item( M_ID_FULL_GUILD_DATA, "公会数据塞满", false)
	--保存公会数据
	--ex_add_menu_item( M_ID_SAVE_GUILD_DATA, "保存公会数据", false)

	--随机请求 role info
	--ex_add_menu_item( M_ID_REQ_ROLE_INFO, "随机请求 role info", false)
	--随机请求 fight info
	--ex_add_menu_item( M_ID_REQ_FIGHT_INFO, "随机请求 fight info", false)
	--随机请求 full role data
	--ex_add_menu_item( M_ID_REQ_FULL_ROLE_DATA, "随机请求 full role data", false)

	--开始拉取订单号
	--ex_add_menu_item( M_ID_BEGIN_PULL_ORDERNO, "开始拉取订单号", false)
	--结束拉取订单号
	--ex_add_menu_item( M_ID_END_PULL_ORDERNO, "结束拉取订单号", false)

end
