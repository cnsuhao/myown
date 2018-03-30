-- 文件名: robot_ui_cmd_func.lua
-- 说明: 机器人相应界面 cmd 的函数
-- 作者:
-- 版权所有:

require("./robot_ui_def")
require("./robot_func_list")
-- 响应 界面 cmd 消息的handlers
tb_cmd_msg_handler = {}
----------------------------------------------------------------
-- 函數
----------------------------------------------------------------
local function string_split(str, delimiter)
    if str==nil or str=='' or delimiter==nil then
    	return nil
    end
	
    local result = {}
    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match)
    end
    return result
end
local function get_interval_time()
    if ROBOT_MODE_TIME_RANDOM == false then
        return ROBOT_MODE_TIME_INTERVAL
    end
    if ROBOT_MODE_TIME_INTERVAL_ADDIT <=0 then
        return ROBOT_MODE_TIME_INTERVAL
    end
    return ROBOT_MODE_TIME_INTERVAL+ex_random(ROBOT_MODE_TIME_INTERVAL_ADDIT)+1
end
-- 解析參數
local function parse_func_param_ex(str_param,is_first_int)
    local arg_list = {}
    local cmd_list = string_split(str_param,";")
    local cmd_len = table.getn(cmd_list)
    if cmd_len <=0 then
        return false,arg_list
    end
    if is_first_int == true then
        table.insert(arg_list,VTYPE_INT)
        table.insert(arg_list, tonumber(cmd_list[1]) )
    else
        table.insert(arg_list,VTYPE_STRING)
        table.insert(arg_list, tostring(cmd_list[1]) )
    end
    if cmd_len>=2 then
        local n_index = 2
        local temp_list = {}
        local str_type = ""
        while n_index<= cmd_len do
            -- 
            temp_list = string_split(tostring(cmd_list[n_index]),":")
            if table.getn(temp_list) == 2 then
                str_type = tostring(temp_list[1])
                if str_type == "b" then
                    table.insert(arg_list,VTYPE_BOOL)
                    if "true" == tostring(temp_list[2]) then
                        table.insert(arg_list, 1 )
                    else
                        table.insert(arg_list, 0 )
                    end
                elseif str_type == "i" then
                    table.insert(arg_list,VTYPE_INT)
                    table.insert(arg_list, tonumber(temp_list[2]) )
                elseif str_type == "f" then
                    table.insert(arg_list,VTYPE_FLOAT)
                    table.insert(arg_list, tonumber(temp_list[2]) )
                elseif str_type == "d" then
                    table.insert(arg_list,VTYPE_DOUBLE)
                    table.insert(arg_list, tonumber(temp_list[2]) )
                elseif str_type == "s" then
                    table.insert(arg_list,VTYPE_STRING)
                    table.insert(arg_list, tostring(temp_list[2]) )
                elseif str_type == "ws" then
                    table.insert(arg_list,VTYPE_WIDESTR)
                    table.insert(arg_list, tostring(temp_list[2]) )
                end
            end
            n_index = n_index + 1
        end
    end
    return true,arg_list
end
local function parse_func_param_msg(str_param)
    local list_msg = varlist()
    local p_result,p_arg_list = parse_func_param_ex(str_param,true)
    if p_result == false then
        return false,list_msg
    end
    -- 驗證輸入參數
    local cmd_len = table.getn(p_arg_list)
    if cmd_len%2 ~= 0 then
        return false,list_msg
    end
    --開始發送自定義消息   

    local n_index =1
    local param_type = 0
    while n_index <= cmd_len do
        --寫入參數
        param_type = p_arg_list[n_index]
        if param_type == VTYPE_INT then
            list_msg.write_int(list_msg,p_arg_list[n_index+1] )
        elseif param_type == VTYPE_FLOAT then
            list_msg.write_float(list_msg,p_arg_list[n_index+1] )
        elseif param_type == VTYPE_DOUBLE then
            list_msg.write_double(list_msg,p_arg_list[n_index+1] )
        elseif param_type == VTYPE_BOOL then
            list_msg.write_int(list_msg,p_arg_list[n_index+1] )
        elseif param_type == VTYPE_STRING then
            list_msg.write_string(list_msg,p_arg_list[n_index+1] )
        elseif param_type == VTYPE_WIDESTR then
            list_msg.write_widstr(list_msg,p_arg_list[n_index+1] )
        end
        n_index = n_index+2
    end
    return true,list_msg
end
local function parse_func_param_func(str_param)
    local func_name = ""
    local has_arg = false
    local func_arg = {}
    local p_result,p_arg_list = parse_func_param_ex(str_param,false)
    if p_result == false then
        return false,func_name,has_arg,func_arg
    end
    -- 驗證輸入參數
    local cmd_len = table.getn(p_arg_list)
    if cmd_len%2 ~= 0 then
        return false,func_name,has_arg,func_arg
    end
    --開始發送自定義消息   
    func_name = p_arg_list[2]

    if cmd_len>3 then
        local n_index =3
        local param_type = 0
        while n_index <= cmd_len do
            --寫入參數
            table.insert(func_arg,p_arg_list[n_index+1])
            if has_arg == false then
                has_arg = true
            end
            n_index = n_index+2
        end
    end
    
    return true,func_name,has_arg,func_arg
end
local function on_robot_exec_proc_func(robot_obj,cmd_content)
    local p_result,func_name,has_arg,arg_list=parse_func_param_func(cmd_content)
    if p_result == false then
        robot_obj.ex_log( robot_obj, "[--lua--][func]is invalid, ", tostring(cmd_content))
        return false
    end
    local cmd_handler = tb_func_list_handler[func_name]
	if cmd_handler ~= nil then
        robot_obj.ex_log( robot_obj, "[--lua--][func] ", tostring(cmd_content))
        if has_arg == true then
            cmd_handler(robot_obj, unpack(arg_list) )
        else
            cmd_handler(robot_obj )
        end
	else
		robot_obj.ex_log( robot_obj, "[--lua--][func]not find ", tostring(cmd_content))
	end
    return true
end
local function on_robot_exec_proc_msg(robot_obj,cmd_content)
    -- 解析輸入參數
    local p_result,list_msg = parse_func_param_msg(cmd_content)
    if p_result == false then
        robot_obj.ex_log( robot_obj, "[--lua--][msg]is invalid, ", tostring(cmd_content))
        return false
    end
    --send
    p_result = robot_obj.ex_send_custom(robot_obj, list_msg)
    if p_result == false then
        robot_obj.ex_log( robot_obj, "[--lua--][msg]send error, ", tostring(cmd_content))
    end
    return true
end
----------------------------------------------------------------
-- 登录，角色创建，选择等
----------------------------------------------------------------
-- 连接接入服务器
tb_cmd_msg_handler[CMD_ID_CONNECT] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begin connect entry.")
    robot_obj.ex_connect_entry(robot_obj)
end

-- 计费登陆
function charge_login(robot_obj)
    local bOK = robot_obj.ex_begin(robot_obj, "login")
    if (bOK == false) then
        robot_obj.ex_log(robot_obj, "robot state error, it's testing.")
        return 
    end
    
    robot_obj.logoning = true
    if false == robot_obj.ex_charge_login(robot_obj) then
        robot_obj.ex_end(robot_obj, false)
        robot_obj.logoning = false
        return 
    end
    
    -- 启动登陆超时定时器 30000ms
    robot_obj.ex_add_hb(robot_obj, "logon_timeout_timer", "logon_timeout_timer", 30000, 1, 0)
end
tb_cmd_msg_handler[CMD_ID_CHARGE_LOGIN] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begin charge login.")
    charge_login(robot_obj)
end

-- 注册
tb_cmd_msg_handler[CMD_ID_CHARGE_REGISTER] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begin register.")
    robot_obj.ex_register(robot_obj)
end
-- 停止工作
tb_cmd_msg_handler[CMD_ID_STOP_TASK_WORK] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] stop work.")
    ROBOT_TASK_IS_WORKING=false
end

-- 虚拟币付费
tb_cmd_msg_handler[CMD_ID_PAY_ITEM] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begin pay item.")
    robot_obj.ex_pay_item(robot_obj)
end

-- 创建角色
tb_cmd_msg_handler[CMD_ID_CREATE_ROLE] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begine create role.")
    robot_obj.ex_create_role(robot_obj)
end

-- 选择角色
tb_cmd_msg_handler[CMD_ID_CHOOSE_ROLE] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begine choose role.")
    robot_obj.ex_choose_role(robot_obj)
end

-- 退出账号
function logout_account(robot_obj)
    robot_obj.ex_add_hb(robot_obj, "log_out_timer", "log_out_timer", 1000, 1, 0)
end
tb_cmd_msg_handler[CMD_ID_LOG_OUT] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] log out.")
    logout_account(robot_obj)
end
function goto_loation(robot_obj,dst_x,dst_z)
    local list = varlist()
    --list.write_
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --target scene id
    list.write_string(list, "goto " .. tostring(dst_x).." "..tostring(dst_z))
    robot_obj.ex_send_custom(robot_obj, list)
end
----------------------------------------------------------------
-- 场景相关操作
----------------------------------------------------------------
--发送客户端进入场景定时器
function switch_scene_timer(robot_obj, hb_name, elapse_time)
    --开始切场景
    robot_obj.ex_log(robot_obj, "[--lua--] begin switch scene.")
    local list = varlist()
    --list.write_
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --target scene id
    list.write_string(list, "set GroupId " .. robot_obj.groupid)
    robot_obj.ex_send_custom(robot_obj, list)
    --list.write_
    --msg id
    local list = varlist()
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --target scene id
    list.write_string( list, "ss ".. robot_obj.nextSceneId) --reload_scene_info_module_config
    
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    if ROBOT_MODE_TYPE_SYNC == true then
        robot_obj.ex_remove_hb(robot_obj, "switch_scene_timer")
    end
end
tb_cmd_msg_handler[CMD_ID_SWITCH_SCENE_SELECT] = function(robot_obj, cmd_id, scene_id, group_id)
    robot_obj.ex_log(robot_obj, "[--lua--] switch to scene="..tostring(scene_id)..", group_id="..tostring(group_id) )
    
    robot_obj.groupid = group_id
    robot_obj.nextSceneId = scene_id

    if ROBOT_MODE_TYPE_SYNC == true then
        -- 添加1s后切场景的定时器
        robot_obj.ex_add_hb(robot_obj, "switch_scene_timer", "switch_scene_timer", 1000, 1, 0)
    else
        -- 立即执行
        switch_scene_timer(robot_obj,"switch_scene_timer",1000)
    end
end

tb_cmd_msg_handler[CMD_ID_GM_CMD] = function(robot_obj, cmd_id,gm_cmd )
    robot_obj.ex_log(robot_obj, "[--lua--] run gm_cmd = "..tostring(gm_cmd) )

    local list = varlist()
    --list.write_
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --target scene id
    list.write_string(list, gm_cmd)
    
    robot_obj.ex_send_custom(robot_obj, list)
end

tb_cmd_msg_handler[CMD_ID_MSG_CMD] = function(robot_obj, cmd_id,gm_cmd )
    robot_obj.ex_log(robot_obj, "[--lua--] run msg = "..tostring(gm_cmd) )
    on_robot_exec_proc_msg(robot_obj,gm_cmd)
end

----------------------------------------------------------------
-- 移动行为
----------------------------------------------------------------
-- 开始行走
local function robot_move_begin( robot_obj )
    if robot_obj == nil or robot_obj.sceneId == nil then
        ROBOT_MOVE_IS_WORKING=false
        robot_obj.ex_log( robot_obj, "run move error in scene=nil" )
        return false
    end
    p_result,p_param,p_index = robot_obj.ex_robot_cur_param(robot_obj, "pathlist_time",0,0 )
    if p_result == false then
        return false
    end
    if p_param == "1" then
        ROBOT_MOVE_VERIFY_TARGET_RANGE=true
    else
        ROBOT_MOVE_VERIFY_TARGET_RANGE=false
    end

    p_result,p_param,p_index = robot_obj.ex_robot_cur_param(robot_obj, "pathlist_time",0,1 )
    if p_result == false then
        return false
    end
    ROBOT_MOVE_VALID_TARGET_RANGE = tonumber(p_param)
    ROBOT_MOVE_IS_WORKING=true
    return true
end
local function robot_move_update( robot_obj,is_first)
    if is_first == false then
        robot_obj.ex_remove_hb(robot_obj, "move_timer")
    end
    if ROBOT_MOVE_IS_WORKING == false then
        return false
    end
    local next_time = get_interval_time()
    robot_obj.ex_add_hb(robot_obj, "move_timer", "move_timer", next_time, 0, 0)
    return true
end
local function robot_move_end( robot_obj,is_del_timer )
    ROBOT_MOVE_IS_WORKING = false
    if is_del_timer == true then
        -- 移除行走定时器
        robot_obj.ex_remove_hb(robot_obj, "move_timer")
    end
end
tb_cmd_msg_handler[CMD_ID_START_MOVE] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] begin move.")
    if ROBOT_MOVE_IS_WORKING == false then
        robot_move_begin( robot_obj )
    end
    if ROBOT_MOVE_IS_WORKING == false then
        robot_obj.ex_log( robot_obj, "run move error!" )
    else
        robot_obj.ex_log( robot_obj, "run move verify_range="..tostring(ROBOT_MOVE_VERIFY_TARGET_RANGE)..",valid_range="..tostring(ROBOT_MOVE_VALID_TARGET_RANGE) )
    end
    -- 初始化坐標
    local curSceneId = robot_obj.sceneId
    local p_result,p_param,p_index = robot_obj.ex_robot_prev_param(robot_obj, "pathlist",curSceneId )
    local dst_list = string_split(p_param,",")
    local r_count,r_pos_old = robot_obj.ex_is_move_to_target( robot_obj,tonumber(dst_list[1]), tonumber(dst_list[2]) ,tonumber(dst_list[3]),ROBOT_MOVE_VALID_TARGET_RANGE,true )
    --更新計時器
    robot_move_update( robot_obj,true )

    -- 增加随机时间
    --local robot_num = robot_obj.ex_get_robot_num(robot_obj)
    --local num_max = (robot_num / 1000 + 1) * 100
    --local ticks = ex_random(num_max) * 100
--	local ticks = ex_random( 1000 ) + 3000
    -- 4000ms 行走一次
--    robot_obj.ex_add_hb(robot_obj, "move_timer", "move_timer", 3000, 0, ticks)
end

-- 停止行走
tb_cmd_msg_handler[CMD_ID_STOP_MOVE] = function(robot_obj)
    robot_obj.ex_log(robot_obj, "[--lua--] stop move.")
    
    robot_move_end(robot_obj,false)
end

----------------------------------------------------------------
-- 容器,属性操作
----------------------------------------------------------------

-- gm 背包 80
tb_cmd_msg_handler[CMD_ID_GM_FULL_PACK] = function(robot_obj)
    -- 添加1s后执行
    robot_obj.ex_add_hb(robot_obj, "timer_gm_full_pack", "timer_gm_full_pack", 1000, 1, 0)
end

-- gm 背包 70 上下浮动
tb_cmd_msg_handler[CMD_ID_GM_ADJUST_PACK] = function(robot_obj)
    -- 添加1s后执行
    robot_obj.ex_add_hb(robot_obj, "timer_gm_adjust_pack", "timer_gm_adjust_pack", 1000, 1, 0)
end

-- gm 背包 40
tb_cmd_msg_handler[CMD_ID_GM_NORMAL_PACK] = function(robot_obj)
    -- 添加1s后执行
    robot_obj.ex_add_hb(robot_obj, "timer_gm_normal_pack", "timer_gm_normal_pack", 1000, 1, 0)
end

-- gm 背包 40 上下浮动
tb_cmd_msg_handler[CMD_ID_GM_ADJUST_PACK_40] = function(robot_obj)
    -- 添加1s后执行
    robot_obj.ex_add_hb(robot_obj, "timer_gm_adjust_pack_40", "timer_gm_adjust_pack_40", 1000, 1, 0)
end

-- 一身神装
tb_cmd_msg_handler[CMD_ID_GM_PERFECT_EQUIP] = function(robot_obj)
    -- 添加1s后执行
    robot_obj.ex_add_hb(robot_obj, "timer_gm_perfect_equip", "timer_gm_perfect_equip", 1000, 1, 0)
end

----------------------------------------------------------------
-- 副本战斗相关
----------------------------------------------------------------
-- 切换到战斗场景 50 号翠竹岛1
tb_cmd_msg_handler[CMD_ID_SWITCH_TO_SCENE] = function(robot_obj)
    -- 添加1s后执行
	robot_obj.groupid = -1
	robot_obj.nextSceneId = 50
    robot_obj.ex_add_hb(robot_obj, "switch_scene_timer", "switch_scene_timer", 1000, 1, 0)
end


-- 创建房间
-- 配对规则
-- 1-3
-- 4-6
-- 7-9
-- ...
tb_cmd_msg_handler[CMD_ID_CREATE_ROOM] = function(robot_obj)
    local is_leader = robot_obj.ex_is_leader(robot_obj)
    
    if is_leader == true then
        robot_obj.ex_log(robot_obj, "[--lua--] create room.")
        local list = varlist()
        --msg id
        list.write_int(list, CLIENT_CUSTOMMSG_TEAM_CREATE_ROOM)
        --proto id
        list.write_int(list, 409)
        --房间名
        local room_name = "test_room_" .. ex_number(robot_obj.client_id)
        list.write_string(list, room_name)
        
        robot_obj.ex_send_custom(robot_obj, list)
    end
end

-- 进入房间
tb_cmd_msg_handler[CMD_ID_JOIN_ROOM] = function(robot_obj)
    local is_leader = robot_obj.ex_is_leader(robot_obj)
    if is_leader == false then
        robot_obj.ex_log(robot_obj, "[--lua--] enter room.")
        local list = varlist()
        --msg id
        list.write_int(list, CLIENT_CUSTOMMSG_TEAM_CHOOSE_JOIN_ROOM)
        --proto id
        list.write_int(list, 409)
        --队长名
        local leader_id = robot_obj.ex_get_leader_id(robot_obj)
        local leader_name = "test" .. ex_number(leader_id)
        list.write_string(list, leader_name)
        
        robot_obj.ex_send_custom(robot_obj, list)
    end
end

-- 开始战斗
tb_cmd_msg_handler[CMD_ID_BEGIN_FIGHT] = function(robot_obj)
    local is_leader = robot_obj.ex_is_leader(robot_obj)
    
    if is_leader == true then
        robot_obj.ex_log(robot_obj, "[--lua--] begin room fight.")
        local list = varlist()
        --msg id
        list.write_int(list, CLIENT_CUSTOMMSG_TEAM_BEGIN_CLONE_SCENE)
        
        robot_obj.ex_send_custom(robot_obj, list)
    end
end

-- 執行自定義任務
function exec_proc_timer( robot_obj, hb_name, elapse_time )
    robot_obj.ex_remove_hb(robot_obj, "exec_proc_timer")
    local p_result,p_completed,p_index,p_timer_interval = robot_obj.ex_robot_exec_proc(robot_obj, -1,false )
    if p_result == false then
        robot_obj.ex_log( robot_obj, "run proc["..tostring(p_index).."]: is error!" )
        return false
    end
    if p_completed == true then
        robot_obj.ex_log( robot_obj, "run proc["..tostring(p_index).."]: is success!" )
        return true
    end
    if ROBOT_TASK_IS_WORKING == true then
        robot_obj.ex_add_hb(robot_obj, "exec_proc_timer", "exec_proc_timer", p_timer_interval, 0, 0)
    end
    return true
end
function on_custom_task_case(robot_obj,cmd_id)
    local proc_index = cmd_id-CMD_ID_ROBOT_PROCESS_BASE
    local proc_name = robot_obj.ex_robot_name_proc(robot_obj,proc_index)
    robot_obj.ex_log( robot_obj, "run proc["..tostring(proc_name).."]: is begin!" )

    local p_result,p_completed,p_index,p_timer_interval = robot_obj.ex_robot_exec_proc(robot_obj, proc_index,true )
    if p_result == false then
        robot_obj.ex_log( robot_obj, "run proc["..tostring(p_index).."]: is error!" )
        return false
    end
    if p_completed == true then
        robot_obj.ex_log( robot_obj, "run proc["..tostring(p_index).."]: is success!" )
        return true
    end
    if ROBOT_TASK_IS_WORKING == false then
        ROBOT_TASK_IS_WORKING=true
    end
    -- 等待執行下一條語句
    robot_obj.ex_add_hb(robot_obj, "exec_proc_timer", "exec_proc_timer", p_timer_interval, 0, 0)
    return true
end
local function get_cmd_type_name(cmd_type)
    if cmd_type == TaskCmd_Sleep then
        return true,"Sleep"
    elseif cmd_type == TaskCmd_GM then
        return true,"GM"
    elseif cmd_type == TaskCmd_Func then
        return true,"Func"
    elseif cmd_type == TaskCmd_Msg then
        return true,"Msg"
    else
        return false,"Unknown"
    end
end
local function on_robot_exec_proc_gm(robot_obj,cmd_content)
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --target content
    list.write_string(list, cmd_content)
    
    robot_obj.ex_send_custom(robot_obj, list)
end
function on_robot_exec_proc(robot_obj,proc_index,proc_step,cmd_type,cmd_content)
    local p_rsult,p_type_name = get_cmd_type_name(cmd_type)
    if p_result == false then
        robot_obj.ex_log( robot_obj, "run proc["..tostring(proc_index).."]["..tostring(proc_step).."]:error, type="..tostring(p_type_name)..",cmd="..tostring(cmd_content) )
        return false
    end
    robot_obj.ex_log( robot_obj, "run proc["..tostring(proc_index).."]["..tostring(proc_step).."]: type="..tostring(p_type_name)..",cmd="..tostring(cmd_content) )
    if cmd_type == TaskCmd_GM then
        on_robot_exec_proc_gm(robot_obj,cmd_content)
    elseif cmd_type == TaskCmd_Func then
        on_robot_exec_proc_func(robot_obj,cmd_content)
    elseif cmd_type == TaskCmd_Msg then
        on_robot_exec_proc_msg(robot_obj,cmd_content)
    end
    return true
end

----------------------------------------------------------------
-- 延迟执行功能
----------------------------------------------------------------

-- 退出账号
function log_out_timer(robot_obj, hb_name, elapse_time)
    --开始切场景
    robot_obj.ex_log(robot_obj, "[--lua--] send gm command cs 64.")
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_QUIT_GAME)
    
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "log_out_timer")
end

--移动定时器
function move_timer(robot_obj, hb_name, elapse_time)
    if ROBOT_MOVE_IS_WORKING == false then
        robot_move_end( robot_obj,true )
        return false
    end
    local curSceneId = robot_obj.sceneId
    -- 匹配當前坐標是否滿足條件
    local p_result,p_param,p_index = robot_obj.ex_robot_prev_param(robot_obj, "pathlist",curSceneId )
    local dst_list = string_split(p_param,",")
    if ROBOT_MOVE_VERIFY_TARGET_RANGE then
        -- 檢測robot是否走到預定範圍，嘗試n次後直接走下一個坐標
        local r_count,r_pos_old = robot_obj.ex_is_move_to_target( robot_obj,tonumber(dst_list[1]), tonumber(dst_list[2]) ,tonumber(dst_list[3]),ROBOT_MOVE_VALID_TARGET_RANGE,false )
        if r_count > 1 then
            robot_obj.ex_log( robot_obj, "run move wait ".."["..tostring(curSceneId).."]".."["..tostring(p_index).."] xyz="..tostring(p_param).." cur xyz="..tostring(r_pos_old) )
            --robot_obj.ex_request_move_target(robot_obj,tonumber(dst_list[1]), tonumber(dst_list[2]) ,tonumber(dst_list[3]) );
            return false
        end
    end
    -- 取下一個坐標
    local p_result,p_param,p_index = robot_obj.ex_robot_next_param(robot_obj, "pathlist",curSceneId,true )
    if p_result == false then
        robot_obj.ex_log( robot_obj, "run move error in scene="..tostring[curSceneId] )
        robot_move_end( robot_obj,true )
        return false
    end
    robot_obj.ex_log( robot_obj, "run move to ".."["..tostring(curSceneId).."]".."["..tostring(p_index).."] xyz="..tostring(p_param) )
    local pos_list = string_split(p_param,",")
    -- robot_obj.ex_log( robot_obj, "run move to".."["..tostring(p_index).."]x="..tostring(pos_list[1])..",y="..tostring(pos_list[2])..",z="..tostring(pos_list[3]) )

    --開始行走
    robot_obj.ex_request_move_target(robot_obj,tonumber(pos_list[1]), tonumber(pos_list[2]) ,tonumber(pos_list[3]),tonumber(dst_list[1]), tonumber(dst_list[2]) ,tonumber(dst_list[3]) );

    -- 更新下一个
    robot_move_update( robot_obj,false)
    return true
end

function timer_gm_full_pack(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm full_backpack.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- full_backpack
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "full_backpack 80")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_full_pack")
end

function timer_gm_adjust_pack(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm full_backpack_opt.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- full_backpack
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "full_backpack_opt 20")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_adjust_pack")
end

function timer_gm_normal_pack(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm full_backpack 40.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- full_backpack
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "full_backpack 40")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_normal_pack")
end

function timer_gm_adjust_pack_40(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm full_backpack_opt.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- full_backpack
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "full_backpack_opt 10 40")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_adjust_pack_40")
end

function timer_gm_perfect_equip(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm full_backpack_opt.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- 添加武器
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "weapon_mark")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- 添加装备
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20110205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20109205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20108205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20107205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20106205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20105205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20104206")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20103205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20102205")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "addequip 20101305")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- 添加技能
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "skill")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_perfect_equip")
end

function timer_load_player(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] load player.")
    
    for n = 1, 1 do
        -- load_player testn testn 3600 1 0 0 0 0
        local list = varlist()
        --msg id
        list.write_int(list, CLIENT_CUSTOMMSG_GM)
        local gm_cmd = "load_player"
        local robot_acct = "test" .. ex_string(robot_obj.client_id - 1000)
        local robot_name = robot_acct
        local robot_live_time = 3600
        local offline_scene = 999
        local gm_msg = gm_cmd .. " " .. robot_acct .. " " .. robot_name .. " " .. ex_string(robot_live_time) .. " " .. ex_string(offline_scene)
        --gm command
        list.write_string(list, gm_msg)
        --send
        robot_obj.ex_send_custom(robot_obj, list)
    end
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_load_player")
end

tb_task_cmd = {}
tb_task_cmd[0] = "task_accept"
tb_task_cmd[1] = "task_complete"

function timer_task(robot_obj, hb_name, elapse_time)
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    
    -- 接任务
    if robot_obj.task_ctrl == 0 then
        -- 交任务
        -- 环回的做任务
        if robot_obj.task_sequence >= robot_obj.max_task_seq then
            robot_obj.task_sequence = 1
        else
            robot_obj.task_sequence = robot_obj.task_sequence + 1
        end
        
        --gm command
        local gm_cmd_str = tb_task_cmd[robot_obj.task_ctrl] .. " " .. ex_number(robot_obj.task_sequence)
        list.write_string(list, gm_cmd_str)
        
        -- 切换到交任务
        robot_obj.task_ctrl = 1
    else
        --gm command
        local gm_cmd_str = tb_task_cmd[robot_obj.task_ctrl] .. " " .. ex_number(robot_obj.task_sequence)
        list.write_string(list, gm_cmd_str)
        
        -- 切换到接任务
        robot_obj.task_ctrl = 0
    end
    
    robot_obj.ex_send_custom(robot_obj, list)
end

function timer_measure_logic_delay(robot_obj, hb_name, elapse_time)
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_ROUND_TRIP)
    --获取当前时间
    list.write_int(list, ex_get_tickcount())
    
    robot_obj.ex_send_custom(robot_obj, list)
end

function timer_gm_create_guild(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm guild.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "guild")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_create_guild")
end

function timer_gm_full_guild_data(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm fullguild.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "fullguild")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_full_guild_data")
end

function timer_gm_save_guild_data(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm savepub.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "savepub")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_save_guild_data")
end

function timer_gm_req_role_info(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm ldr_role_info.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "ldr_role_info")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_req_role_info")
end

function timer_gm_req_fight_info(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm ldr_role_fight_info.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "getxml")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_req_fight_info")
end

function timer_gm_req_full_role_data(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm ldr_full_role.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "ldr_full_role")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    --移除定时器
    robot_obj.ex_remove_hb(robot_obj, "timer_gm_req_full_role_data")
end

function timer_pull_orderno(robot_obj, hb_name, elapse_time)
    --设置robot为当前对象
    robot_obj.ex_log(robot_obj, "[--lua--] gm pull_orderno.")
    
    -- setobj
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "setobj")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
    
    -- task
    local list = varlist()
    --msg id
    list.write_int(list, CLIENT_CUSTOMMSG_GM)
    --gm command
    list.write_string(list, "pull_orderno")
    --send
    robot_obj.ex_send_custom(robot_obj, list)
end
