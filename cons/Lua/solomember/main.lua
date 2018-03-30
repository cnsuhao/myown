--场景服务器主控脚本

--程序状态
local WAPS_UNKNOWN = 0	--未知
local WAPS_STARTING = 1	--正在启动
local WAPS_READY = 2	--就绪（但是未开放服务）
local WAPS_OPENING = 3	--正在开放服务
local WAPS_OPENED = 4	--已开放服务
local WAPS_CLOSING = 5	--正在关闭
local WAPS_CLOSED = 6	--已关闭服务
local WAPS_QUITING = 7	--正在退出

--资源配置类型
local USE_XML = 0   --使用xml
local USE_INI = 1   --使用ini

--入口函数
function main(member)
	nx_set_value("member", member)

	--快速日志系统
	member.UseQuickLog = true
	--应用进程记录
	member.UseAppRecord = true

	--事件循环超时时间
	member.EventLoopExceedTime = 2000
	member.CompletionStatusExceedTime = 2000

	--从本地副本出来强制刷新大厅npc
	member.ForceRefreshVisual = false

	--屏幕输出字符颜色
	member:SetConsoleColor("Gray")

	--读取程序配置
	local ini = nx_create("IniDocument")

	--配置文件名
	--ini.FileName = "game_member.ini"
	ini.FileName = nx_main_config()

	--加载配置
	if not ini:LoadFromFile() then
		nx_msgbox("can't read config file!")
		nx_destroy(ini)
		return 0
	end

	--绑定CPU核心
	local bind_core = ini:ReadInteger("Main", "BindCore", -1)
	--游戏分区编号
	local district_id = ini:ReadInteger("Main", "DistrictID", 1)
	--游戏服务器编号
	local server_id = ini:ReadInteger("Main", "ServerID", 1)
	--场景服务器编号
	local member_id = ini:ReadInteger("Main", "MemberID", 0)
	--场景碰撞信息文件的根目录
	local collide_root_path = ini:ReadString("Main", "CollideRootPath", "")
	--出现错误是否提示
	local debug_halt = nx_boolean(ini:ReadString("Main", "DebugHalt", "true"))
  --WSASend函数发送数组的个数(最大16个,默认是8)
	local wsasend_arrays = ini:ReadInteger("Main", "WSASendArrays", 8)
	--WSASend函数发送缓存大小(最大64M,默认是1M)
	local wsasend_buflen = ini:ReadInteger("Main", "WSASendBufLen", 1048576)
	if collide_root_path ~= "" then
		collide_root_path = nx_function("ext_get_full_path", collide_root_path)
	end

	--内网地址
	local inner_addr = "127.0.0.1"
	local inner_port = 0
	--local inner_port = 16300 + member_id

	inner_addr = ini:ReadString("Inner", "Addr", inner_addr)
	inner_port = ini:ReadInteger("Inner", "Port", inner_port)

	--广播地址
	local broad_addr = "127.255.255.255"
	local broad_port = 15000

	broad_addr = ini:ReadString("Broadcast", "Addr", broad_addr)
	broad_port = ini:ReadInteger("Broadcast", "Port", broad_port)

	--维护连接地址
	local echo_addr = "127.0.0.1"
	local echo_port = 15300 + member_id

	echo_addr = ini:ReadString("Echo", "Addr", echo_addr)
	echo_port = ini:ReadInteger("Echo", "Port", echo_port)

	--逻辑模块
	local logic_loader = nx_create("LogicLoader")

	local logic_path = ini:ReadString("Logic", "Path", "")
	local modules = ini:ReadInteger("Logic", "Modules", 0)

	--sns
	local use_sns = nx_boolean(ini:ReadString("Main", "UseSns", "false"))
	local sns_server_count = ini:ReadInteger("Main", "SnsServerCount", 0)

	--自定义消息回复
	local custom_ack = nx_boolean(ini:ReadString("Main", "CustomAck", "false"))
	--逻辑模块
	local logic_loader = nx_create("LogicLoader")

	local logic_path = ini:ReadString("Logic", "Path", "")
	local modules = ini:ReadInteger("Logic", "Modules", 0)

	if logic_path ~= "" and modules == 0 then
		local fs = nx_create("FileSearch")

		fs:SearchFile(nx_work_path() .. logic_path .. "\\", "*.dll")

		local dll_table = fs:GetFileList()

		for i = 1, table.getn(dll_table) do
			if logic_loader:Load(nx_work_path() .. logic_path .. "\\", dll_table[i]) then
				member:WriteLine("Load loigc module " .. dll_table[i])
			else
				member:WriteLine("Load loigc module " .. dll_table[i] .. " failed")
			end
		end

		nx_destroy(fs)
	else
		for i = 0, modules do
			local dll_name = ini:ReadString("Logic", "Module" .. nx_string(i), "")

			if dll_name ~= "" then
				if logic_loader:Load(nx_work_path() .. logic_path, dll_name) then
					member:WriteLine("Load loigc module " .. dll_name)
				else
					member:WriteLine("Load loigc module " .. dll_name .. " failed")
				end
			end
		end
	end

	nx_destroy(ini)

	--场景碰撞信息
	local collide_manager = nx_create("CollideManager")

	--绑定脚本
	nx_bind_script(member, "main", "echo_init")

	member.AppName = "member_" .. nx_string(server_id) .. "_" .. nx_string(member_id)
	member.BindCore = bind_core

	--服务器
	--member.FastScanTime = 200

	member.DistrictId = district_id
  member.WSASendArrays = wsasend_arrays
  member.WSASendBufLen = wsasend_buflen
	member.ServerId = server_id
	member.MemberId = member_id
	member.LogicLoader = logic_loader
	member.CollideManager = collide_manager
	member.ScriptPath = nx_resource_path() .. "lua\\"
	member.DebugHalt = debug_halt
	member.CollideRootPath = collide_root_path
	member.InnerAddr = inner_addr
	member.InnerPort = inner_port
	member.OuterAddr = outer_addr
	member.OuterPort = outer_port
	member.BroadAddr = broad_addr
	member.BroadPort = broad_port
	member.EchoAddr = echo_addr
	member.EchoPort = echo_port
	--保证启动日志服务
	member.LogLauncher = "game_log game_log.ini daemon"
	--是否开启视野消息优化
	--member.SightOptimize = true
	--是否不发送为缺省空值的对象属性
	member.HideEmptyProperty = false
	member.UseSns = use_sns

	--合并消息
	member.MergeProperty = true
	member.MergeObjects = false

	--自定义消息回复
	member.CustomAck = custom_ack

	--场景分格大小
	member.SceneGridSize = 16
	--member.SlowScanTime = 5000
	--member.SceneTimerRandLimit = 300

	--节省流量
	member.ObjectPropertyFameUpdate = true
	member.DelayClientReadyResendView = true

	--不延时刷新
	member.DelayCreateFrame = 0

	member.AppearAtOnce = true

	member.UseConfigType = USE_XML

	--加载名字过滤文件
	member:LoadNameFilter(nx_resource_path() .. "filter.txt")
	--加载全局点信息
	member:LoadLocations(nx_resource_path() .. "location.xml")

	--启动场景服务
	member:Start()

	--实际使用的内网侦听端口
	inner_port = member.InnerPort

	--显示应用程序名
	member:WriteLine("app name " .. member.AppName)
	--显示通讯地址和端口
	member:WriteLine("echo socket " .. echo_addr .. ":" .. nx_string(echo_port))
	member:WriteLine("inner socket " .. inner_addr .. ":" .. nx_string(inner_port))
	member:WriteLine("broadcast socket " .. broad_addr .. ":" .. nx_string(broad_port))
	--启动完成
	member:WriteLine("game server " .. nx_string(server_id)
		.. " member " .. nx_string(member_id) .. " started")

	--设置进程开启状态
	member:SetAppState(WAPS_OPENED);

	return 1
end

--关闭
function close()
	local member = nx_value("member")

	--关闭场景服务
	member:WriteLine("member server shutdown...")
	member:Stop()

	--结束程序
	nx_quit()

	return true
end

--维护连接脚本

--初始化
function echo_init(self)
	nx_callback(self, "on_open", "echo_open")
	nx_callback(self, "on_close", "echo_close")
	nx_callback(self, "on_input", "echo_input")

	nx_callback(self, "on_input_getp", "echo_input_getp")
	nx_callback(self, "on_input_setp", "echo_input_setp")
	nx_callback(self, "on_input_getc", "echo_input_getc")
	nx_callback(self, "on_input_setc", "echo_input_setc")
	nx_callback(self, "on_input_method", "echo_input_method")
	nx_callback(self, "on_input_listp", "echo_input_listp")
	nx_callback(self, "on_input_listc", "echo_input_listc")
	nx_callback(self, "on_input_listm", "echo_input_listm")
	nx_callback(self, "on_input_listg", "echo_input_listg")
	nx_callback(self, "on_input_function", "echo_input_function")
	nx_callback(self, "on_input_debugger", "echo_input_debugger")

	return 1
end

--维护连接打开
function echo_open(self, addr, port)
	self:WriteLine("echo client " .. nx_string(addr)
		.. ":" .. nx_string(port) .. " connected")
	self:SendEchoLine("welcome server " .. nx_string(self.ServerId)
		.. " member " .. nx_string(self.MemberId) .. "\r")
	return 1
end

--维护连接关闭
function echo_close(self, addr, port)
	self:WriteLine("echo client " .. nx_string(addr)
		.. ":" .. nx_string(port) .. " closed")
	return 1
end

--处理维护输入命令
function echo_input(self, command)
	if command == "quit" then
		--退出程序
		echo_response(self, "server will shutdown...")
		close()
	elseif command == "end" then
		--关闭连接
		self:CloseEcho()
	elseif command == "perf" then
		--导出逻辑模块性能统计数据
		self:DumpPerformance("perf" .. nx_string(self.MemberId) .. ".txt")
		echo_response(self, "dump logic module performance...")
	else
		echo_response(self, "undefined command: " .. command)
	end

	return 1
end

--发送回应信息
function echo_response(self, info)
	self:SendEchoLine(">" .. info .. "\r")
	self:WriteLine(info)
	return true
end

--查找实体
function get_entity(entity_name)
	local id = nx_value(entity_name)

	if id ~= nil then
		return id
	end

	id = nx_lookup(entity_name)

	if not nx_is_null(id) then
		return id
	end

	id = nx_object(entity_name)

	if nx_is_valid(id) then
		return id
	end

	return nil
end

--转换到合适的类型
function get_new_value(old_v, value)
	local type = nx_type(old_v)

	if type == "boolean" then
		return nx_boolean(value)
	elseif type == "number" then
		return nx_number(value)
	elseif type == "string" then
		return nx_string(value)
	elseif type == "widestr" then
		return nx_widestr(value)
	elseif type == "int" then
		return nx_int(value)
	elseif type == "int64" then
		return nx_int64(value)
	elseif type == "float" then
		return nx_float(value)
	elseif type == "double" then
		return nx_double(value)
	elseif type == "object" then
		return nx_object(value)
	end

	return nil
end

--查询实体内部属性
function echo_input_getp(self, entity, prop)
	if entity == nil or prop == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local value = nx_property(obj, nx_string(prop))

	echo_response(self, "entity " .. nx_name(obj) .. " property "
		.. nx_string(prop) .. " value is " .. nx_string(value))

	return 1
end

--设置实体内部属性
function echo_input_setp(self, entity, prop, value)
	if entity == nil or prop == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local old_v = nx_property(obj, nx_string(prop))

	local new_v = get_new_value(old_v, value)

	if not nx_set_property(obj, nx_string(prop), new_v) then
		echo_response(self, "set property failed")
		return 0
	end

	new_v = nx_property(obj, nx_string(prop))

	echo_response(self, "set entity " .. nx_name(obj) .. " property "
		.. nx_string(prop) .. " value " .. nx_string(new_v))

	return 1
end

--查询实体自定义属性
function echo_input_getc(self, entity, prop)
	if entity == nil or prop == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local value = nx_custom(obj, nx_string(prop))

	echo_response(self, "entity " .. nx_name(obj) .. " custom "
		.. nx_string(prop) .. " value is " .. nx_string(value))

	return 1
end

--设置实体自定义属性
function echo_input_setc(self, entity, prop, value)
	if entity == nil or prop == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local old_v = nx_custom(obj, nx_string(prop))

	local new_v = get_new_value(old_v, value)

	if not nx_set_custom(obj, nx_string(prop), new_v) then
		echo_response(self, "set custom failed")
		return 0
	end

	new_v = nx_custom(obj, nx_string(prop))

	echo_response(self, "set entity " .. nx_name(obj) .. " custom "
		.. nx_string(prop) .. " value " .. nx_string(new_v))

	return 1
end

--执行实体方法
function echo_input_method(self, entity, method, ...)
	if entity == nil or method == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local res = nx_method(obj, nx_string(method), unpack(arg))

	if type(res) == "table" then
		echo_response(self, "run entity " .. nx_name(obj) .. " method "
			.. nx_string(method) .. " return table")

		for i = 1, table.getn(res) do
			echo_response(self, "table value " .. nx_string(i) .. ": " .. nx_string(res[i]))
		end
	else
		echo_response(self, "run entity " .. nx_name(obj) .. " method "
			.. nx_string(method) .. " return " .. nx_string(res))
	end

	return 1
end

--列出实体的所有内部属性
function echo_input_listp(self, entity)
	if entity == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local prop_table = nx_property_list(obj)

	local num = table.getn(prop_table)

	echo_response(self, "entity " .. nx_name(obj) .. " property number is " .. nx_string(num))

	for i = 1, num do
		echo_response(self, nx_string(prop_table[i]))
	end

	return 1
end

--列出实体的所有自定义属性
function echo_input_listc(self, entity)
	if entity == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local prop_table = nx_custom_list(obj)

	local num = table.getn(prop_table)

	echo_response(self, "entity " .. nx_name(obj) .. " custom number is " .. nx_string(num))

	for i = 1, num do
		echo_response(self, nx_string(prop_table[i]))
	end

	return 1
end

--列出实体的所有方法
function echo_input_listm(self, entity)
	if entity == nil then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local prop_table = nx_method_list(obj)

	local num = table.getn(prop_table)

	echo_response(self, "entity " .. nx_name(obj) .. " method number is " .. nx_string(num))

	for i = 1, num do
		echo_response(self, nx_string(prop_table[i]))
	end

	return 1
end

--列出所有全局变量
function echo_input_listg(self)
	local global_table = nx_value_list()

	local num = table.getn(global_table)

	echo_response(self, "global number is " .. nx_string(num))

	for i = 1, num do
		local name = global_table[i]
		local value = nx_value(name)

		echo_response(self, nx_string(name) .. "[" .. nx_type(value) .. "]: " .. nx_string(value))
	end

	return 1
end

--执行脚本扩展函数
function echo_input_function(self, func, ...)
	if func == nil or func == "" then
		return 0
	end

	local res = nx_function(func, unpack(arg))

	if type(res) == "table" then
		echo_response(self, "run function " .. nx_string(func) .. " return table")

		for i = 1, table.getn(res) do
			echo_response(self, "result" .. nx_string(i) .. ": " .. nx_string(res[i]))
		end
	else
		echo_response(self, "run function " .. nx_string(func) .. " return " .. nx_string(res))
	end

	return 1
end


--调试函数
function echo_input_debugger(self, entity, method, ...)
	if entity == nil or entity == "" or method == nil or method == "" then
		return 0
	end

	local obj = get_entity(nx_string(entity))

	if obj == nil then
		return 0
	end

	local res = nx_method(obj, nx_string(method), unpack(arg))

	if type(res) == "table" then
		--echo_response(self, "run entity " .. nx_string(entity) .. " method " .. nx_string(method) .. " return table")
			self:SendEchoLine(">" .. "run entity " .. nx_string(entity) .. " method " .. nx_string(method) .. " return table" .. "\r")

		for i = 1, table.getn(res) do
			--echo_response(self, "result" .. nx_string(i) .. ": " .. nx_string(res[i]))
				self:SendEchoLine(">" .. "result" .. nx_string(i) .. ": " .. nx_string(res[i]) .. "\r")
		end
	else
		--echo_response(self, "run entity " .. nx_string(entity) .. " method " .. nx_string(method) .. " return " .. nx_string(res))
		self:SendEchoLine(">" .. "run entity " .. nx_string(entity) .. " method " .. nx_string(method) .. " return " .. nx_string(res) .. "\r")
	end

	return 1
end
