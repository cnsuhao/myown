-- 文件名: robot_menu_cmd_func.lua
-- 说明: 响应菜单的消息函数
-- 作者:
-- 版权所有:

-- 响应菜单的handlers
tb_menu_cmd_handler = {}

----------------------------------------------------------------
-- 登录，角色创建，选择等
----------------------------------------------------------------
-- 通知机器人连接接入服务器并登录
tb_menu_cmd_handler[M_ID_LOGIN] = function ()
		ex_send_ui_command(CMD_ID_CONNECT)
end

-- 通知创建角色
tb_menu_cmd_handler[M_ID_CREATE_ROLE] = function ()
		ex_send_ui_command(CMD_ID_CREATE_ROLE)
end

-- 通知选择角色
tb_menu_cmd_handler[M_ID_CHOOSE_ROLE] = function ()
		ex_send_ui_command(CMD_ID_CHOOSE_ROLE)
end

-- 退出账号
tb_menu_cmd_handler[M_ID_LOG_OUT] = function ()
		ex_send_ui_command(CMD_ID_LOG_OUT)
end

-- 计费登陆
tb_menu_cmd_handler[M_ID_CHARGE_LOGIN] = function ()
		ex_send_ui_command(CMD_ID_CHARGE_LOGIN)
end

-- 注册
tb_menu_cmd_handler[M_ID_CHARGE_REGISTER] = function ()
		ex_send_ui_command(CMD_ID_CHARGE_REGISTER)
end

tb_menu_cmd_handler[M_ID_STOP_TASK_WORK] = function ()
		ex_send_control_command(CMD_ID_STOP_TASK_WORK)
end


--虚拟币付费
tb_menu_cmd_handler[M_ID_PAY_ITEM] = function ()
		ex_send_ui_command(CMD_ID_PAY_ITEM)
end
----------------------------------------------------------------
-- 场景相关操作
----------------------------------------------------------------
-- tb_menu_cmd_handler[M_ID_SWITCH_SCENE_SELECT] = function ()
-- 		ex_send_ui_command(CMD_ID_SWITCH_SCENE_SELECT)
-- end

----------------------------------------------------------------
-- 移动行为
----------------------------------------------------------------
-- 开始行走
tb_menu_cmd_handler[M_ID_START_MOVE] = function ()
		ex_send_ui_command(CMD_ID_START_MOVE)
end

-- 停止行走
tb_menu_cmd_handler[MD_ID_STOP_MOVE] = function ()
		ex_send_ui_command(CMD_ID_STOP_MOVE)
end

----------------------------------------------------------------
-- 容器,属性操作
----------------------------------------------------------------


-- gm 背包 80
tb_menu_cmd_handler[M_ID_GM_FULL_PACK] = function ()
		ex_send_ui_command(CMD_ID_GM_FULL_PACK)
end

-- gm 背包 70 上下浮动
tb_menu_cmd_handler[M_ID_GM_ADJUST_PACK] = function ()
		ex_send_ui_command(CMD_ID_GM_ADJUST_PACK)
end

-- gm 背包 40
tb_menu_cmd_handler[M_ID_GM_NORMAL_PACK] = function ()
		ex_send_ui_command(CMD_ID_GM_NORMAL_PACK)
end

-- gm 背包 40 上下浮动
tb_menu_cmd_handler[M_ID_GM_ADJUST_PACK_40] = function ()
		ex_send_ui_command(CMD_ID_GM_ADJUST_PACK_40)
end

-- 一身神装
tb_menu_cmd_handler[M_ID_GM_PERFECT_EQUIP] = function ()
		ex_send_ui_command(CMD_ID_GM_PERFECT_EQUIP)
end


----------------------------------------------------------------
-- 副本战斗相关
----------------------------------------------------------------
-- 请求切换到克隆场景战斗
tb_menu_cmd_handler[M_ID_SWITCH_TO_SCENE] = function ()
		ex_send_ui_command(CMD_ID_SWITCH_TO_SCENE)
end


-- 创建房间
tb_menu_cmd_handler[M_ID_CREATE_ROOM] = function ()
		ex_send_ui_command(CMD_ID_CREATE_ROOM)
end

-- 进入房间
tb_menu_cmd_handler[M_ID_JOIN_ROOM] = function ()
		ex_send_ui_command(CMD_ID_JOIN_ROOM)
end

-- 开始副本
tb_menu_cmd_handler[M_ID_BEGIN_FIGHT] = function ()
		ex_send_ui_command(CMD_ID_BEGIN_FIGHT)
end

----------------------------------------------------------------
-- 功能相关 
----------------------------------------------------------------
function menu_custom_task_case(menu_id)
    local menu_index= menu_id-M_ID_ROBOT_PROCESS_BASE
    ex_send_ui_command( CMD_ID_ROBOT_PROCESS_BASE+menu_index )
end

