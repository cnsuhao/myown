require "../utils/string"
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

--日志类型
local	EM_LOGGER_LEVEL_FATAL = 1
local	EM_LOGGER_LEVEL_ERROR = 2
local	EM_LOGGER_LEVEL_WARN = 3
local	EM_LOGGER_LEVEL_INFO = 4
local	EM_LOGGER_LEVEL_DEBUG = 5
local	EM_LOGGER_LEVEL_TRACE = 6

--入口函数
function main(SoloCharge)

	nx_set_value("SoloCharge", SoloCharge)
	
	--屏幕输出字符颜色
	--SoloCharge:SetConsoleColor("LightGreen")

	
	--创建日志目录
	if not nx_path_exists(nx_work_path() .. "log") then
		nx_path_create(nx_work_path() .. "log")
	end
	if not nx_path_exists(nx_work_path() .. "log\\login") then
		nx_path_create(nx_work_path() .. "log\\login")
	end
	
	--日志缓冲池
	local log_pool = nx_create("LogPool")
	local logger = nx_create("SoloLog")
	nx_set_value("logger", logger)
	nx_set_value("log_pool", log_pool)
	
	--日志文件前缀
	log_pool.FilePrefix = nx_work_path() .. "log\\login\\"
	--缓冲数量
	log_pool.TraceMax = 256
	--是否每天一个日志文件
	log_pool.EveryDayAlone = true
	
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
	ini.FileName = "game_charge.ini"
	
	--加载配置
	if not ini:LoadFromFile() then
		nx_msgbox("[charge] 无法读取程序配置文件")
		nx_destroy(ini)
		return 0
	end
	
	--http服务实体
	local httpService = nx_create("EntHttpService")
	if ( nil == httpService ) then
		SoloCharge:WriteMessage("create http service failed.", EM_LOGGER_LEVEL_FATAL);
		nx_msgbox("[charge] create http service failed");
		return 0;
	end
	nx_set_value("HttpService", httpService)
		
	-- 加载配置
	if not load_config( ini, SoloCharge, httpService, true ) then
		nx_msgbox("[charge] load config failed");
		return 0;
	end
	
	-- 启动服务
	local bIsOK = httpService:Startup();
	if bIsOK then
		SoloCharge:WriteMessage("httpService:Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_INFO);
	else
		SoloCharge:WriteMessage("httpService:Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_FATAL);
		nx_msgbox("start httpService failed");
		return 0;
	end
	
	local bIsOK = SoloCharge:Startup();
	if bIsOK then
		SoloCharge:WriteMessage("SoloCharge:Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_INFO);
	else
		SoloCharge:WriteMessage("SoloCharge:Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_FATAL);
		nx_msgbox("[charge] start SoloCharge failed");
		return 0;		
	end
	--SoloCharge:AddExcuteEntity( httpService.ID );
	
	--维护连接地址
	local echo_addr = "127.0.0.1"
	local echo_port = 1232
	echo_addr = ini:ReadString("Echo",  "addr", echo_addr)
	echo_port = ini:ReadInteger("Echo", "port", echo_port)
	
	--创建维护连接服务
	local echo_sock = nx_create("SoloEcho")
	nx_set_value("SoloEcho", echo_sock)
	echo_sock.IP = echo_addr
	echo_sock.Port = echo_port
	--绑定维护脚本(echo)
	nx_bind_script(echo_sock, "main", "echo_init")
	bIsOK = echo_sock:Startup();
	--SoloCharge:AddExcuteEntity( echo_sock.ID );
	if bIsOK then
		SoloCharge:WriteMessage("EntEcho::Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_INFO);
	else
		SoloCharge:WriteMessage("EntEcho:Startup() result:" .. nx_string(bIsOK), EM_LOGGER_LEVEL_FATAL);
		nx_msgbox("[charge] start EntEcho failed");
	end	
	return 1
end

--加载配置
function load_config( ini, charge, http, bInit )
	if nil == charge or nil == http or nil == ini then
		return false;
	end

	-- 配置主实体
	if bInit then
		charge.IP = ini:ReadString("Main", "ip", "0.0.0.0")
		charge.Port = ini:ReadInteger("Main", "port", 1231)
		charge.ReadBuf = ini:ReadInteger("Main", "msgsize", 102400)
		charge.SendBuf = ini:ReadInteger("Main", "msgsize", 102400)
	end
	charge.Verify = ini:ReadString("Main", "verify_acc", "true") == "true"
	charge.LogLevel = ini:ReadInteger("Main", "log_level", 4)	
	charge.ClientVersion = ini:ReadInteger("Main", "client_ver", 0)	
	charge:SetWhiteTables(ini:ReadString("Main", "allowip", "*"))
	local isReplyKeep = ini:ReadString("Main", "reply_keep", "false")
	if ( isReplyKeep == "true") then
		charge.IsReplyKeep = true	
	else
		charge.IsReplyKeep = false	
	end
	
	--配置http服务实体
	if bInit then
		http.IP = ini:ReadString("Http", "ip", "0.0.0.0")
		http.Port = ini:ReadInteger("Http", "port", 8080)
		http.ReadBuf = ini:ReadInteger("Http", "msgsize", 10240)
	end
	http.ReqReuseNum = ini:ReadInteger("Http", "req_reuse_num", 1)
	http.ReqRetries = ini:ReadInteger("Http", "req_retries_num", 0)
	http:SetWhiteTables(ini:ReadString("Http", "allowip", "*"))
	http.LoginVerifyUrl = ini:ReadString("Cas","verifyUrl","")
	
	-- 添加用户限制
	http:ClearPostUser()
	local postList = ini:ReadString("Cas","allowUserPost", "")
	if nil ~= postList and #postList > 0 then
		local allows = postList:split( "," );
		for _, post in pairs(allows) do
			http:AddPostUser(post);
		end
	end

	return true;
end

--关闭
function close()	
	local httpService = nx_value("HttpService")
	local echo_sock = nx_value("SoloEcho")
	local SoloCharge = nx_value("SoloCharge")
	
	if nil ~= echo_sock then
		echo_sock:Stop();
	end
		
	if nil ~= httpService then
		httpService:Stop()
	end	
	
	if nil ~= SoloCharge then
		SoloCharge:Stop()
	end
	
	local log_pool = nx_value("log_pool")
	local logger = nx_value("logger")
	if nil ~= logger then
		logger:Stop()
	end	
	if nil ~= log_pool then
		log_pool:Stop()
	end	
	return true
end

--强制关闭
function force_close()
	close()
	nx_quit()
	return true
end

-- 清除创建资源
function on_cleanup()
	local httpService = nx_value("HttpService")
	local echo_sock = nx_value("SoloEcho")

	if nil ~= httpService then
		nx_destroy( httpService  );
	end
	
	if nil ~= echo_sock then
		nx_destroy( echo_sock );
	end
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
	--self:WriteLine("echo client " .. nx_string(addr) 
	--	.. ":" .. nx_string(port) .. " connected")
	echo_response( self, "welcome server SoloEcho...")
	return 1
end

--维护连接关闭
function echo_close(self, addr, port)
	--self:WriteLine("echo client " .. nx_string(addr) 
	--	.. ":" .. nx_string(port) .. " closed")
	return 1
end

--显示帮助文档
function echo_help(self)
	echo_response(self, "	quit		exit program" );
	echo_response(self, "	fquit 		force exit program, call nx_quit" );
	echo_response(self, "	help		show command desc" );
	echo_response(self, "	reload		reload all config" );
	echo_response(self, "	verify	[true/false]	enable or disable account verify" );
	echo_response(self, "	state		show current config and state" );
	echo_response(self, "	dbguser <clear/add/remove> 	control debug user list" );
	echo_response(self, "	posuser <clear/add/remove> 	control post user list" );
	echo_response(self, "	end			close this echo" );
end

-- 重新加载配置
function echo_reload(self)
	--读取程序配置
	local ini = nx_create("IniDocument")
	
	--配置文件名
	ini.FileName = "game_charge.ini"
	
	--加载配置
	if not ini:LoadFromFile() then
		echo_response(self, "can't load game_charge.ini");
		nx_destroy(ini)
		return
	end

	local httpService = nx_value("HttpService")
	local SoloCharge = nx_value("SoloCharge")
	
	if load_config( ini, SoloCharge, httpService, false) then
		echo_response(self, "reload config succeed");
	else
		echo_response(self, "reload config failed");
	end
end

--显示当前状态
function echo_show_state(self, insName)
	if insName ~= nil and insName ~= "charge" and insName ~= "http" then
		echo_response(self, "insName '" .. insName .. "' don't exists");
		return;
	end
	
	local http = nx_value("HttpService")
	local charge = nx_value("SoloCharge")
	
	if nil == insName or insName == "charge" then
		echo_response(self, "charge status:");
		if nil == charge then
			echo_response(self, "	not found charge instance");
		else
			echo_response(self, "	ip:" .. charge.IP);
			echo_response(self, "	port:" .. charge.Port);
			echo_response(self, "	msg size:" .. charge.ReadBuf);
			echo_response(self, "	logLevel:" .. charge.LogLevel);
			echo_response(self, "	client version:" .. charge.ClientVersion);
			echo_response(self, "	appid:" .. charge.AppId);
			echo_response(self, "	reply_keep:" .. nx_string(charge.IsReplyKeep) );
			echo_response(self, "	verify:" .. nx_string(charge.Verify) );
			echo_response(self, "	no verify:" .. charge:DumpNoVerifyUser() );
		end
	end
	
	if nil == insName or insName == "http" then
		echo_response(self, "http status:");
		if nil == http then
			echo_response(self, "	not found http instance");
		else
			echo_response(self, "	ip:" .. http.IP);
			echo_response(self, "	port:" .. http.Port);
			echo_response(self, "	msg size:" .. http.ReadBuf);
			echo_response(self, "	login url:" .. http.LoginVerifyUrl);
			echo_response(self, "	post users:" .. http:DumpPostUser());
		end
	end
end

--验证功能
function echo_enable_verify( self, p )
	local SoloCharge = nx_value("SoloCharge")
	if nil ~= SoloCharge then 
		if p == "false" then
			SoloCharge.Verify = false
			echo_response(self, "disable verify");
		else
			SoloCharge.Verify = true
			echo_response(self, "enable verify");
		end
	else
		echo_response(self, "get charge instance failed");
	end
end

--调试用户
function echo_dbg_user(self, args)
	local charge = nx_value("SoloCharge")
	if nil == charge then 
		echo_response(self, "not found charge instance");
		return;	
	end
	
	--解析子命令
	if table.getn(args) < 2 then
		echo_response(self, "command args error.");
		return;
	end
	
	local cmd = args[2];
	local txt="execute dbguser " .. cmd .. " succeed";
	if cmd == "clear" then
		charge:ClearNoVerifyUser();
	elseif cmd == "remove" then
		if table.getn(args) < 3 then
			txt = "param error, eg: dbguser remove <accid>";
		else
			charge:SetNoVerifyUser( args[3], 0 );
		end
	elseif cmd == "add" then
		if table.getn(args) < 3 then
			txt = "param error, eg: dbguser add <accid> [num](default is 1)";
		else
			local num = 1;
			if table.getn(args) >= 4 then
				num = nx_int( args[4] )
			end
			charge:SetNoVerifyUser( args[3], num );
		end
	else
		txt = "invalid command.";
	end
	
	echo_response(self, txt);
end

-- 登陆后缀用户限制
function echo_post_user(self, args)
	local http = nx_value("HttpService")
	if nil == http then 
		echo_response(self, "not found http instance");
		return;	
	end
	
	--解析子命令
	if table.getn(args) < 2 then
		echo_response(self, "command args error.");
		return;
	end
	
	local cmd = args[2];
	local txt="execute postuser " .. cmd .. " succeed";
	if cmd == "clear" then
		http:ClearPostUser();
	elseif cmd == "remove" then
		if table.getn(args) < 3 then
			txt = "param error, eg: postuser remove <post>";
		else
			http:RemovePostUser( args[3]);
		end
	elseif cmd == "add" then
		if table.getn(args) < 3 then
			txt = "param error, eg: postuser add <post>";
		else
			http:AddPostUser( args[3] );
		end
	else
		txt = "invalid command.";
	end
	
	echo_response(self, txt);
end

function echo_end( self )
	echo_response( self, "disconnect..." );
	
	self:EndEcho();
end

--字符串分隔方法  
function string:split(sep)  
    local sep, fields = sep or " ", {}  
    local pattern = string.format("([^%s]+)", sep)  
    self:gsub(pattern, function (c) fields[#fields + 1] = c end)  
    return fields  
end 

--处理维护输入命令
function echo_input(self, command)
	-- 分割参数
	local args = command:split(nil);
	local cmd = args[1];
	echo_response(self, "ready execute:" .. cmd)
	if cmd == "quit" then
		--退出程序
		echo_response(self, "SoloEcho server will shutdown...")
		close()
	elseif cmd == "fquit" then
		--强制退出
		echo_response(self, "SoloEcho server will shutdown...")
		force_close()
	elseif cmd == "help" then
		--显示支持的命令
		echo_help(self);
	elseif cmd == "reload" then
		--重新加载配置
		echo_reload(self);
	elseif cmd == "verify" then
		--验证功能
		echo_enable_verify(self, args[2] );
	elseif cmd == "state" then
		--显示状态
		echo_show_state(self, args[2]);
	elseif cmd == "dbguser" then
		--调试用户
		echo_dbg_user( self, args );
	elseif cmd == "postuser" then
		-- 登陆后缀限制
		echo_post_user( self, args );
	elseif cmd == "end" then
		-- 断开echo连接
		echo_end( self );
	else
		echo_response(self, "undefined command: " .. command)
	end
	
	return 1
end


--发送回应信息
function echo_response(self, info)
	self:SendEchoLine(">" .. info .. "\r\n")
	self:WriteMessage(info, EM_LOGGER_LEVEL_INFO)
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

