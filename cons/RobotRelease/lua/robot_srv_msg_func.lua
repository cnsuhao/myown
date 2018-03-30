-- 文件名: robot_srv_msg_func.lua
-- 说明: 服务器自定义消息处理handler
-- 作者:
-- 版权所有:

require("./robot_var_type_def")
require("./robot_def")

--读取自定义消息参数
function read_custom_bytes(tb_bytes)
	local type = tb_bytes.read_char(tb_bytes)

	if (type == VTYPE_BOOL or type == VTYPE_BYTE) then
		return tb_bytes.read_char(tb_bytes)

	elseif (type == VTYPE_SHORT) then
		return tb_bytes.read_short(tb_bytes)

	elseif (type == VTYPE_INT) then
		return tb_bytes.read_int(tb_bytes)

	elseif (type == VTYPE_INT64) then
		return tb_bytes.read_int64(tb_bytes)

	elseif (type == VTYPE_FLOAT) then
		return tb_bytes.read_float(tb_bytes)

	elseif (type == VTYPE_DOUBLE) then
		return tb_bytes.read_double(tb_bytes)

	elseif (type == VTYPE_STRING) then
		return tb_bytes.read_len_string(tb_bytes)

	elseif  (type == VTYPE_OBJECT) then
		return tb_bytes.read_objectid(tb_bytes)
	end
end

-- 响应 界面 cmd 消息的handlers
tb_srv_msg_handler = {}

-- 自定义消息，单独写
tb_srv_msg_handler[STOC_CUSTOM] = function (robot_obj,tb_bytes)
	-- 自定义参数个数
	local args_num = tb_bytes.read_short(tb_bytes)

	-- 读取自定义参数
	local msg_args = {}
	for i = 1, args_num do
		msg_args[i] = read_custom_bytes(tb_bytes)
	end

	-- 相应自定义消息处理函数
	local custom_msg_id = msg_args[1];
	local custom_msg_handler = custom_msg_handler[custom_msg_id]

	if custom_msg_handler ~= nil then
		custom_msg_handler(robot_obj, msg_args)
	end
end

-- 相应具体某个自定义消息的处理函数 handlers
custom_msg_handler = {}

-- 收到排名竞技场可挑战玩家列表
custom_msg_handler[SERVER_CUSTOMMSG_ARENA_FIGHTER_LIST] = function ( robot_obj, args )
	local args_num = table.getn(args)

	if args_num > 5 then
		local rank = args[2]
		local robot_name = robot_obj.ex_get_property(robot_obj, "Name")

		local list = varlist();
		list.write_int(list, CLIENT_CUSTOMMSG_ARENA_FIGHT)
		list.write_int(list, rank)

		-- 挑战该排名玩家
		robot_obj.ex_send_custom(robot_obj, list)

		robot_obj.ex_log( robot_obj, "[--lua--] Robot " .. robot_name .. " arena challenge rank ", rank)
	end
end

-- 收到测量延迟返回的消息
custom_msg_handler[SERVER_CUSTOMMSG_ROUND_TRIP] = function ( robot_obj, args )
	local args_num = table.getn(args)

	if args_num == 2 then
		local send_time = args[2]
		local cur_time = ex_get_tickcount()
		local time_gap = cur_time - send_time

		if time_gap > 10 then
			robot_obj.ex_log( robot_obj, "[--lua--] measure send time = ", send_time, "cur_time =", cur_time, "round trip time = ", time_gap)
		end	
	end
end
