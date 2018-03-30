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

local MSG_NOT_COMPRESS = 0    --不压缩消息
local MSG_SINGLE_COMPRESS = 1 --使用消息单个压缩
local MSG_BATCH_COMPRESS = 2  --使用消息批量压缩

--资源配置类型
local USE_XML = 0   --使用xml
local USE_INI = 1   --使用ini

--入口函数
function main(room)
	nx_set_value("Room", room)
	
	--快速日志系统
	room.UseQuickLog = true
	--应用进程记录
	room.UseAppRecord = true
	
	--屏幕输出字符颜色
	room:SetConsoleColor("Gray")
	
	--读取程序配置
	local ini = nx_create("IniDocument")
	
	--配置文件名
	--ini.FileName = "game_room.ini"
	ini.FileName = nx_main_config()
	
	--加载配置
	if not ini:LoadFromFile() then
		nx_msgbox("can't read config file!")
		nx_destroy(ini)
		return 0
	end
	
	--绑定CPU核心
	local bind_core = ini:ReadInteger("Main", "BindCore", -1)
	--平台分区编号
	local platform_id = ini:ReadInteger("Main", "PlatformID", 1)
	--房间游戏服务器编号
	local room_id = ini:ReadInteger("Main", "RoomServerID", 1)
	--出现错误是否提示
	local debug_halt = nx_boolean(ini:ReadString("Main", "DebugHalt", "true"))
	--场景碰撞信息文件的根目录
	local collide_root_path = ini:ReadString("Main", "CollideRootPath", "")
	
	if collide_root_path ~= "" then
		collide_root_path = nx_function("ext_get_full_path", collide_root_path)
	end
	
	--最大连接数
	local max_connects = ini:ReadInteger("Main", "MaxConnects", 1024)
	--通讯加密键
	local msg_encode_key = ini:ReadString("Main", "MessageEncodeKey", "")
	--通讯校验DLL
	local msg_verify_dll = ini:ReadString("Main", "MessageVerifyDll", "")

	--内网地址
	local inner_addr = "127.0.0.1"
	local inner_port = 0
	--local inner_port = 16300 + room_id
	
	inner_addr = ini:ReadString("Inner", "Addr", inner_addr)
	inner_port = ini:ReadInteger("Inner", "Port", inner_port)
	--广播地址
	local broad_addr = "127.255.255.255"
	local broad_port = 15000
	
	broad_addr = ini:ReadString("Broadcast", "Addr", broad_addr)
	broad_port = ini:ReadInteger("Broadcast", "Port", broad_port)
	
	--维护连接地址
	local echo_addr = "127.0.0.1"
	local echo_port = 15600 + room_id
	
	echo_addr = ini:ReadString("Echo", "Addr", echo_addr)
	echo_port = ini:ReadInteger("Echo", "Port", echo_port)
	
	--外网映射地址
	local outer_addr = ini:ReadString("Room_Outer", "Addr", "127.0.0.1")
	local outer_port = ini:ReadInteger("Room_Outer", "Port", 2001)
	
	--外网地址
	local wlan_addr = ini:ReadString("WLAN", "Addr", "127.0.0.1")
	local wlan_port = ini:ReadInteger("WLAN", "Port", 0)

	--extra服务器
	local extra_inuse = nx_boolean(ini:ReadString("Extra","Inuse",""))
	local extra_index = ini:ReadInteger("Extra","Index",0)
	local extra_addr  = ini:ReadString("Extra","Addr","")	
	local extra_port  = ini:ReadInteger("Extra","Port",0)
	
	--语音地址
	local voice_addr = "127.0.0.1:80"
	voice_addr = ini:ReadString("Voice", "Addr", voice_addr)
	
        --WSASend函数发送数组的个数(最大16个,默认是8)
	local wsasend_arrays = ini:ReadInteger("Main", "WSASendArrays", 8)
	--WSASend函数发送缓存大小(最大64M,默认是1M)
	local wsasend_buflen = ini:ReadInteger("Main", "WSASendBufLen", 1048576)	
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
				room:WriteLine("Load loigc module " .. dll_table[i])
			else
				room:WriteLine("Load loigc module " .. dll_table[i] .. " failed")
			end
		end
		
		nx_destroy(fs)
	else
		for i = 0, modules do
			local dll_name = ini:ReadString("Logic", "Module" .. nx_string(i), "")
			if dll_name ~= "" then
				if logic_loader:Load(nx_work_path() .. logic_path, dll_name) then
					room:WriteLine("Load loigc module " .. dll_name)
				else
					room:WriteLine("Load loigc module " .. dll_name .. " failed")
				end
			end
		end
	end
	
	nx_destroy(ini)
	
	--场景碰撞信息
	local collide_manager = nx_create("CollideManager")
	
	--绑定脚本
	nx_bind_script(room, "main", "echo_init")
	
	room.AppName = "room_" .. nx_string(platform_id) .."_"..nx_string(room_id)
	room.BindCore = bind_core
	room.PlatformId = platform_id
	room.ServerId = room_id
	room.LogicLoader = logic_loader
	room.CollideManager = collide_manager
	room.ScriptPath = nx_resource_path() .. "lua\\"
	room.DebugHalt = debug_halt
	room.CollideRootPath = collide_root_path
	room.InnerAddr = inner_addr
	room.InnerPort = inner_port
	room.OuterAddr = outer_addr
	room.OuterPort = outer_port
	room.WLanAddr = wlan_addr
	room.WLanPort = wlan_port
	room.BroadAddr = broad_addr
	room.BroadPort = broad_port
	room.EchoAddr = echo_addr
	room.EchoPort = echo_port
	room.SoundAddr = voice_addr
	room.WSASendArrays = wsasend_arrays
	room.WSASendBufLen = wsasend_buflen
	--合并消息
	room.MergeProperty = true
	room.MergeObjects = true
	
	--保证启动日志服务
	-- room.LogLauncher = "game_log game_log.ini daemon"
	--是否开启视野消息优化
	--room.SightOptimize = true
	--是否不发送为缺省空值的对象属性
	room.HideEmptyProperty = false
	
	--节省流量
	room.ObjectPropertyFameUpdate = true
	
	room.WaitRetEncodeInterval = 0
	
	room.MaxConnects = max_connects
	room.EncodeKey = room:ConvertEncodeKey(msg_encode_key)
	room.DecodeKey = room:ConvertEncodeKey(msg_encode_key)
	room.VerifyDll = msg_verify_dll
	--记录与客户端的收发消息到message.log文件
	--room.LogConnect = true
	--room.LogSend = true
	--room.LogReceive = true
	room.SimpleProtocal = true
	room.UseBufferPool = true	

    room.CompressType = MSG_SINGLE_COMPRESS
    --room.CompressType = MSG_BATCH_COMPRESS

	--加载名字过滤文件
	room:LoadNameFilter(nx_resource_path() .. "filter.txt")
	--加载全局点信息
	room:LoadLocations(nx_resource_path() .. "location.xml")
	
	--心跳
	room.ClientBeatType=2
	room.ClientBeatInterval=5
	room.ClientCheckInterval=45
	room.WaitRetEncodeInterval=0
	
	room.UseConfigType = USE_XML
	--网络统计
	--1表示输出,0表示关闭,默认关闭
	--maxXXX表示输出阀值
	room.OutputDelay = 1
	--OutBufferMax = 256 * 1024;
	room.MaxDataLen = 200*1024
	--毫秒
	room.MaxDelayTime  = 200	
			
	--设置extra服务器
	if extra_inuse then
           room:SetExtraAddress(extra_index, extra_addr, extra_port)
        end
	--设置extra服务器
	if extra_inuse then
           room:SetExtraAddress(extra_index, extra_addr, extra_port)
        end
	--启动场景服务
	room:Start()
	
	--显示应用程序名
	room:WriteLine("app name " .. room.AppName)
	--显示通讯地址和端口
	room:WriteLine("echo socket " .. echo_addr .. ":" .. nx_string(echo_port))
	room:WriteLine("broadcast socket " .. broad_addr .. ":" .. nx_string(broad_port))
	--启动完成
	room:WriteLine("game room server " .. nx_string(room_id)  .. " started")
	
	--设置进程开启状态
	room:SetAppState(WAPS_OPENED);
	
	return 1
end

--关闭
function close()
	local room = nx_value("Room")
	
	--关闭场景服务
	room:WriteLine("room server shutdown...")
	room:Stop()
	
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
	self:SendEchoLine("welcome room " .. nx_string(self.ServerId).. "\r")
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
		self:DumpPerformance("perf" .. nx_string(self.ServerId) .. ".txt")
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