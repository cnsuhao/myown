--接入服务器主控脚本

--程序状态
local WAPS_UNKNOWN = 0	--未知
local WAPS_STARTING = 1	--正在启动
local WAPS_READY = 2	--就绪（但是未开放服务）
local WAPS_OPENING = 3	--正在开放服务
local WAPS_OPENED = 4	--已开放服务
local WAPS_CLOSING = 5	--正在关闭
local WAPS_CLOSED = 6	--已关闭服务
local WAPS_QUITING = 7	--正在退出

--入口函数
function main(SoloHttp)

	nx_set_value("SoloHttp", SoloHttp)
	
	--屏幕输出字符颜色
	SoloHttp:SetConsoleColor("LightGreen")

	
	--创建日志目录
	if not nx_path_exists(nx_work_path() .. "log") then
		nx_path_create(nx_work_path() .. "log")
	end
	
	--日志缓冲池
	local log_pool = nx_create("LogPool")
	local logger = nx_create("SoloLog")

	
	--日志文件前缀
	log_pool.FilePrefix = nx_work_path() .. "log\\Server"
	--缓冲数量
	log_pool.TraceMax = 256
	--是否每天一个日志文件
	log_pool.EveryDayAlone = false
	
	if nx_is_debug() then
		--在调试版里将日志信息输出到控制台
		log_pool.Console = console
	end

	logger.UseThread = true
	--日志文件存放目录
	logger.LogFileDir = nx_work_path() .. "log\\"
	--是否每天一个日志文件
	logger.EveryDayAlone = true
	--按照小时切分日志
	logger.EveryHourLog = 1
	

	--启动日志记录
	log_pool:Start()
	
	nx_set_value("log_pool", log_pool)
	nx_set_value("logger", logger)

	--读取程序配置
	local ini = nx_create("IniDocument")
	
	--配置文件名
	ini.FileName = "FsSoloHttp.ini"
	
	--加载配置
	if not ini:LoadFromFile() then
		nx_msgbox("无法读取程序配置文件")
		nx_destroy(ini)
		return 0
	end
	
	local tasks_config = ini:ReadString("Tasks", "Config", "")

	--App名字
	local app_name = ini:ReadString("Main", "AppName", "SoloHttp_0")	
	--日志进程名
	local log_launcher = ini:ReadString("Main", "LogLauncher", "FsSoloLog.exe")

	--外网地址
	local outer_addr = ini:ReadString("Outer", "Addr", "127.0.0.1")
	local outer_port = ini:ReadInteger("Outer", "Port", 2001)
	
	--内网地址
	local inner_addr = "127.0.0.1"
	local inner_port = 16200 
	
	inner_addr = ini:ReadString("Inner", "Addr", inner_addr)
	inner_port = ini:ReadInteger("Inner", "Port", inner_port)
	
	--维护连接地址
	local echo_addr = "127.0.0.1"
	local echo_port = 15209
	
	echo_addr = ini:ReadString("Echo", "Addr", echo_addr)
	echo_port = ini:ReadInteger("Echo", "Port", echo_port)
	

	--数据转储逻辑DLL文件
	local apple_logic_dll = ini:ReadString("Main", "AppleLogicDll", "")
	local levelpush_logic_dll = ini:ReadString("Main", "LevelPushLogicDll", "")	
	local wp_logic_dll = ini:ReadString("Main", "WPLogicDll","")
	local pk_logic_dll = ini:ReadString("Main", "PKLogicDll","")

	nx_destroy(ini)
	
	--创建维护连接服务
	--local echo_sock = nx_create("EchoSock")
	SoloHttp.EchoAddr = echo_addr
	SoloHttp.EchoPort = echo_port
	
	nx_bind_script(SoloHttp, "main", "echo_init")

	
	--创建广播通讯
	--local broad_sock = nx_create("BroadSock")
	
	--broad_sock.BindAddress = inner_addr
	--broad_sock.BroadcastAddress = broad_addr
	--broad_sock.BroadcastPort = broad_port
	

	
	--创建进程监控
	local app_manager = nx_create("AppManager")
	
	app_manager.AppName = "manager" .. app_name
	app_manager.AppAddress = echo_addr .. ":" .. nx_string(echo_port)
	
	--创建内部通讯
	--local adapter = nx_create("Adapter")
	
	--内部通讯地址
	--adapter.Address = inner_addr
	--adapter.Port = inner_port
	--最大内网连接数量
	--adapter.ConnectMax = 64
	--工作线程数量
	--adapter.WorkThreads = 1
	--缓冲区尺寸
	--adapter.InBufLen = 1024 * 1024
	--adapter.OutBufLen = 1024 * 1024
	
	--创建主服务
	--local game_server = nx_create("SoloHttp")
	--nx_set_value("game_server", game_server)
	
	--逻辑模块
	local logic_loader = nx_create("LogicLoader")
	
	if apple_logic_dll ~= "" then
		if logic_loader:Load(nx_work_path(), apple_logic_dll ) then
			SoloHttp:WriteLine("Load loigc module " .. apple_logic_dll )
		else
			SoloHttp:WriteLine("Load loigc module " .. apple_logic_dll .. " failed")
		end
	end

	if levelpush_logic_dll ~= "" then
		if logic_loader:Load(nx_work_path(), levelpush_logic_dll ) then
			SoloHttp:WriteLine("Load loigc module " .. levelpush_logic_dll )
		else
			SoloHttp:WriteLine("Load loigc module " .. levelpush_logic_dll .. " failed")
		end
	end

	if wp_logic_dll ~= "" then
		if logic_loader:Load(nx_work_path(), wp_logic_dll ) then
			SoloHttp:WriteLine("Load loigc module " .. wp_logic_dll  )
		else
			SoloHttp:WriteLine("Load loigc module " .. wp_logic_dll .. " failed")
		end
	end

	if pk_logic_dll ~= "" then
		if logic_loader:Load(nx_work_path(), pk_logic_dll ) then
			SoloHttp:WriteLine("Load loigc module " .. pk_logic_dll  )
		else
			SoloHttp:WriteLine("Load loigc module " .. pk_logic_dll .. " failed")
		end
	end


	SoloHttp.TaskConfig = tasks_config	

	SoloHttp.AppName = app_name
	SoloHttp.LogLauncher = log_launcher
	--SoloHttp.Console = console
	SoloHttp.EchoSock = echo_sock
	SoloHttp.LogicLoader = logic_loader
	SoloHttp.OuterAddr =	 outer_addr
	SoloHttp.OuterPort = outer_port
	--SoloHttp.EchoAddr  = echo_addr
	--SoloHttp.EchoPort  = echo_port	
	SoloHttp.UseThread = nx_boolean("false")

	--SoloHttp.AppManager = app_manager
	--SoloHttp.Adapter = adapter
	
	--记录与客户端的收发消息到message.log文件
	--SoloHttp.LogConnect = true
	--SoloHttp.LogSend = true
	--SoloHttp.LogReceive = true
	
	--启动进程监控
	app_manager:Start()
	
	--设置进程启动状态
	app_manager:SetAppState(WAPS_STARTING)
	
	--启动内部通讯
	--adapter:Start()
	
	--启动维护连接
	--echo_sock:Start()

	--启动日志服务
	--logger:Start()

	--启动接入服务
	SoloHttp:Start()
	
	--显示应用程序名
	SoloHttp:WriteLine("App name " .. app_manager.AppName)
	--显示通讯地址和端口
	SoloHttp:WriteLine("Echo socket " .. echo_addr .. ":" .. nx_string(echo_port))
	--SoloHttp:WriteLine("Inner socket " .. inner_addr .. ":" .. nx_string(inner_port))
	SoloHttp:WriteLine("Outer socket " .. outer_addr .. ":" .. nx_string(outer_port))
	
	--启动完成
	SoloHttp:WriteLine(" SoloHttp started")
	
	--设置进程开启状态
	app_manager:SetAppState(WAPS_OPENED);
	
	return 1
end

--关闭
function close()	
	local SoloHttp = nx_value("SoloHttp")
	
	if SoloHttp.State == "Closed" then
		force_close()	
	else		
		--进入关闭状态后自动退出程序
		SoloHttp.QuitOnClosed = true
		--发出关闭服务器命令
		SoloHttp:CloseServer()
		SoloHttp:WriteLine("waiting SoloHttp server close...")
	end

	return true
end

--强制关闭
function force_close()
	local SoloHttp = nx_value("SoloHttp")

	--关闭接入服务
	SoloHttp:WriteLine("SoloHttp server shutdown...")
	
	SoloHttp:Stop()

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
	--nx_callback(self, "on_input_function", "echo_input_function")
	nx_callback(self, "on_input_debugger", "echo_input_debugger")
	
	return 1
end

--维护连接打开
function echo_open(self, addr, port)
	self:WriteLine("echo client " .. nx_string(addr) 
		.. ":" .. nx_string(port) .. " connected")
	self:SendEchoLine("welcome server SoloHttp \r")
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
		echo_response(self, "solohttp server will shutdown...")
		close()

	elseif command == "fquit" then
		--强制退出
		echo_response(self, "solocache server will shutdown...")
		force_close()

	elseif command == "end" then
		--关闭连接
		self:CloseEcho()

	elseif command == "close" then
		--关闭服务
		if self.State ~= "Opened" then
			echo_response(self, "game server not open")
			return 0
		end
		
		if self:CloseServer() then
			echo_response(self, "send close command to game server")
		end
	elseif command == "status" then
		--当前服务状态
		echo_response(self, "game server state is " .. self.State)
	elseif command == "perf" then
		--导出逻辑模块性能统计数据
		self:DumpPerformance("")
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

