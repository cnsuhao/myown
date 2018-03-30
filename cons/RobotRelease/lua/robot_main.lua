-- 文件名: robot_main.lua
-- 说明: 机器人函数定义
-- 作者:
-- 版权所有:  

-- 机器人对象表 member
-- client_id
-- this
-- account
-- password
-- entry_addr
-- entry_port
-- login_type

-- 机器人对象表 function

-- 连接接入服务器
-- robot_obj.ex_connect_entry( robot_obj )

-- 登录
-- robot_obj.ex_login

-- 断开跟接入服务器的链接
-- robot_obj.ex_disconnect( robot_obj )

-- 打印log到editbox ( string, number, bool)
-- robot_obj.ex_log( robot_obj, ...)

-- 给服务器发送消息
-- robot_obj.ex_send( robot_obj, bytes)

-- 设置机器人的状态
-- robot_obj.ex_set_state( robot_obj, str_state)

-- 添加定时器
-- robot_obj.ex_add_hb( robot_obj, hb_name, func_name, time_gap, hb_max)

-- 移除定时器
-- robot_obj.ex_remove_hb( robot_obj, hb_name)

require("./robot_def")
require("./robot_cmd_define")
require("./robot_ui_cmd_func")
require("./robot_srv_msg_func")

----------------------------------------------------------------
-- 事件回调
----------------------------------------------------------------
--机器人创建回调
function on_create( robot_obj )

	-- robot 账号密码 client_id 从1 开始递增
	robot_obj.account = ROBOT_PREFIX_ACCOUNT .. ex_string(robot_obj.client_id)
	-- 账号密码一样
	robot_obj.password = ROBOT_PREFIX_ACCOUNT .. ex_string(robot_obj.client_id)
	robot_obj.name = ROBOT_PREFIX_NAME .. ex_string(robot_obj.client_id)

	-- 接入的 ip port
	robot_obj.entry_addr = ROBOT_ENTRY_IP
	robot_obj.entry_port = ROBOT_ENTRY_PORT
	robot_obj.login_type = ROBOT_LOGIN_TYPE
	-- 设置重连次数
	robot_obj.reconnect_times = 3
	-- 机器人行走下一路点index
	robot_obj.next_pt_index = 0;
	-- 机器人从起点往终点还是从终点往起点
	robot_obj.from_start = 1;
	
    -- 设置group，server_id
    robot_obj.group      = CHARGE_GROUP;
    robot_obj.server_id  = CHARGE_SERVER_ID;
    -- 设置 charge
	robot_obj.trans_url  = TRANS_URL;
	robot_obj.charge_url = CHARGE_URL;
	robot_obj.register_url = REGISTER_URL;
	
	robot_obj.check_ld_time = CHECK_AUTO_LOGIN_DISCONNECT_TIME;
	robot_obj.login_exceed_time = EXCEED_LOGIN_TIME;
	robot_obj.is_random = IS_RANDOM_TYPE;
	
	
--	robot_obj.st_url = ST_URL;
	

	-- 控制任务交接
	--robot_obj.task_ctrl = 0;
	-- 任务序列号
	--robot_obj.task_sequence = 1
	-- 最大任务序列号
	--robot_obj.max_task_seq = 30
end

--机器人连接回调
function on_connect( robot_obj, login_result )
	-- 连接成功
	if login_result == true then
		robot_obj.ex_log( robot_obj, "connect entry OK")
		robot_obj.ex_set_state(robot_obj,"connect entry OK")

		--请求登录
		local bOK = robot_obj.ex_login(robot_obj)
		if ( bOK == false ) then
			robot_obj.ex_end( robot_obj, false );
		end
	-- 连接失败
	else
		robot_obj.ex_log( robot_obj, "connect entry fail")
		robot_obj.ex_set_state(robot_obj,"connect entry fail")
		robot_obj.ex_end( robot_obj, false );
		
		--尝试重新连接
		if robot_obj.reconnect_times > 0 then
			-- 1s 后重新连接
			robot_obj.ex_add_hb( robot_obj, "reconnect_entry", "reconnect_entry", 1000, 1, 0)
			-- 递减重连次数
			robot_obj.reconnect_times = robot_obj.reconnect_times - 1
		else
			robot_obj.ex_log( robot_obj, "reconnect time exhaust")
		end
	end
	
end

--机器人连接断开回调
function on_disconnect( robot_obj )		
	-- 如果未完成 当作失败处理
	robot_obj.ex_end(robot_obj, false );
	robot_obj.logoning = false;
	robot_obj.ex_log( robot_obj, "disconnect entry")
	robot_obj.ex_set_state(robot_obj,"disconnect entry")
end

--登录成功
function on_login_succeed( robot_obj, ... )
	robot_obj.ex_log( robot_obj, "[--lua--]login sucess")
	
	-- 登陆测试结束
	robot_obj.ex_end( robot_obj, true);
	robot_obj.logoning = false;
end

--进入场景
function on_entry_scene( robot_obj, ident, propcount, sceneId )
	robot_obj.ex_log( robot_obj, "[--lua--]entry scene " .. sceneId )
	-- 登陆测试结束
	if robot_obj.logoning == true then
		robot_obj.ex_end( robot_obj, true);
	end

	-- 设置当前场景属性
	robot_obj.preSceneId = robot_obj.sceneId
	robot_obj.sceneId = sceneId

	-- 添加ready定时器 2000ms后
	robot_obj.ex_add_hb( robot_obj, "entry_scene_timer", "entry_scene_timer", 2000, 1, 0)
end

--接收排队消息
function on_queue( robot_obj, nQueue, nPosition, nPriorCount )

	local player_before = nPosition+nPriorCount

	if player_before ~= 0 then
		-- vip player
		if nQueue == 1 then
			robot_obj.ex_log( robot_obj, "[--lua--] on_queue vip queue players before = ", player_before)
		-- normal player
		else
			robot_obj.ex_log( robot_obj, "[--lua--] on_queue normal queue players before = ", player_before)
		end		
	else
		robot_obj.ex_log( robot_obj, "[--lua--] on_queue end queue, ready to login")
	end
end

--接受到消息回调
function on_message( robot_obj, tb_bytes )

	-- 读取消息头
	local msg_id = tb_bytes.read_char(tb_bytes)

	-- 调用 srv msg handler
	local srv_msg_handler = tb_srv_msg_handler[msg_id]
	if srv_msg_handler ~= nil then
		srv_msg_handler(robot_obj,tb_bytes)
	else
		--robot_obj.ex_log( robot_obj, "[--lua--] not has srv msg handler msg_id = ", msg_id)
	end
end

--接受cmd回调
function on_ui_command( robot_obj, cmd_id, ...)
    if cmd_id >= CMD_ID_ROBOT_PROCESS_BASE and cmd_id<=CMD_ID_ROBOT_PROCESS_BASE+ROBOT_PROCESS_CUSTOM_COUNT then
        on_custom_task_case(robot_obj, cmd_id, ...)
        return true
    end
    -- 打印log
	robot_obj.ex_log( robot_obj, "[--lua--] recv cmdid = ", cmd_id)
	-- 调用message handler
	local cmd_handler = tb_cmd_msg_handler[cmd_id]
	if cmd_handler ~= nil then
		cmd_handler(robot_obj, cmd_id, ...)
	else
		robot_obj.ex_log( robot_obj, "[--lua--] not has ui cmd handler cmdid = ", cmd_id)
	end
    return true
end
-- for system config
function on_system_config( robot_obj, is_sync, is_param_random,param_interval_time,param_interval_addit,entry_ip,entry_port,login_type,prefix_account,prefix_name)
    ROBOT_MODE_TYPE_SYNC = is_sync
    ROBOT_MODE_TIME_RANDOM = is_param_random
    ROBOT_MODE_TIME_INTERVAL=param_interval_time
    ROBOT_MODE_TIME_INTERVAL_ADDIT = param_interval_addit

    ROBOT_ENTRY_IP = entry_ip
    ROBOT_ENTRY_PORT = entry_port
    ROBOT_LOGIN_TYPE = login_type

    ROBOT_PREFIX_ACCOUNT = prefix_account
    ROBOT_PREFIX_NAME    = prefix_name

    robot_obj.ex_log( robot_obj, "system config: mode_sync="..tostring(ROBOT_MODE_TYPE_SYNC)..",is_time_random="..tostring(ROBOT_MODE_TIME_RANDOM)..",interval_time="..tostring(ROBOT_MODE_TIME_INTERVAL)..",interval_addit="..tostring(ROBOT_MODE_TIME_INTERVAL_ADDIT) )
    robot_obj.ex_log( robot_obj, "system config: entry_ip="..tostring(ROBOT_ENTRY_IP)..",port="..tostring(ROBOT_ENTRY_PORT)..",login_type="..tostring(ROBOT_LOGIN_TYPE)..",prefix_account="..tostring(ROBOT_PREFIX_ACCOUNT)..",prefix_name="..tostring(ROBOT_PREFIX_NAME) )
end
----------------------------------------------------------------
-- 延迟定时器操作
----------------------------------------------------------------
--发送客户端ready定时器
function entry_scene_timer( robot_obj, hb_name, elapse_time )
	--进入场景
	robot_obj.ex_client_ready(robot_obj)
	--移除定时器
	robot_obj.ex_remove_hb(robot_obj,"entry_scene_timer")
end

--发送客户端ready定时器
function reconnect_entry( robot_obj, hb_name, elapse_time )

	--重新连接
	robot_obj.ex_log( robot_obj, "[--lua--] reconnect entry.")
	robot_obj.ex_connect_entry( robot_obj )

	--移除定时器
	robot_obj.ex_remove_hb(robot_obj,"reconnect_entry")
end

-- 登陆超时定时器
function logon_timeout_timer( robot_obj, hb_name, elapse_time )
	-- 超时断开连接
	if robot_obj.logoning == true then
		robot_obj.ex_disconnect( robot_obj);
	end
	
	--移除定时器
	robot_obj.ex_remove_hb(robot_obj,"logon_timeout_timer")
end
