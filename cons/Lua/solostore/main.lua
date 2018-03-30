--存储服务器主控脚本

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
function main(store)
	nx_set_value("store", store)
	
	--快速日志系统
	store.UseQuickLog = true
	--应用进程记录
	store.UseAppRecord = true
	
	--屏幕输出字符颜色
	store:SetConsoleColor("LightYellow")
	
	--读取程序配置
	local ini = nx_create("IniDocument")
	
	--配置文件名
	--ini.FileName = "game_store.ini"
	ini.FileName = nx_main_config()
	
	--加载配置
	if not ini:LoadFromFile() then
		nx_msgbox("can't read config file!")
		nx_destroy(ini)
		return 0
	end
	
	--游戏分区编号
	local district_id = ini:ReadInteger("Main", "DistrictID", 1)
	--服务器编号
	local server_id = ini:ReadInteger("Main", "ServerID", 1)
	--组内存储编号
	local store_id = ini:ReadInteger("Main", "StoreID", 0)
	--使用MYSQL数据库
	local use_mysql = nx_boolean(ini:ReadString("Main", "UseMysql", "false"))
	--记录数据库存取性能
	local log_db_perf = nx_boolean(ini:ReadString("Main", "LogDBPerformance", "false"))
        --WSASend函数发送数组的个数(最大16个,默认是8)
	local wsasend_arrays = ini:ReadInteger("Main", "WSASendArrays", 8)
	--WSASend函数发送缓存大小(最大64M,默认是1M)
	local wsasend_buflen = ini:ReadInteger("Main", "WSASendBufLen", 1048576)
	--日志表批处理数量
	local log_batch_num = ini:ReadInteger("Main", "SqlBatchNum", 0)
	--日志表自动切分时间
	local log_split_week = ini:ReadInteger("Main", "SplitWeek", 0)
	--日志是否使用int自增主键以加快插入速度
	local log_auto_serial = nx_boolean(ini:ReadString("Main", "LogAutoSerial", "false"))
	
	--内网地址
	local inner_addr = "127.0.0.1"
	local inner_port = 0
	--local inner_port = 16100 + store_id
	
	inner_addr = ini:ReadString("Inner", "Addr", inner_addr)
	inner_port = ini:ReadInteger("Inner", "Port", inner_port)
	
	--广播地址
	local broad_addr = "127.255.255.255"
	local broad_port = 15000
	
	broad_addr = ini:ReadString("Broadcast", "Addr", broad_addr)
	broad_port = ini:ReadInteger("Broadcast", "Port", broad_port)
	
	--维护连接地址
	local echo_addr = "127.0.0.1"
	local echo_port = 15100 + store_id
	
	echo_addr = ini:ReadString("Echo", "Addr", echo_addr)
	echo_port = ini:ReadInteger("Echo", "Port", echo_port)
	
	--唤醒连接地址
	local wake_addr = "127.0.0.1"
	local wake_port = 0
	--local wake_port = 16800 + store_id
	
	wake_addr = ini:ReadString("Wake", "Addr", wake_addr)
	wake_port = ini:ReadInteger("Wake", "Port", wake_port)
	
	--数据库连接
	local account_base = ini:ReadString("Account", "ConnectString", "")
	local account_pool = ini:ReadInteger("Account", "ConnectPool", 0)
	
	local role_base = ini:ReadString("Role", "ConnectString", "")
	local role_pool = ini:ReadInteger("Role", "ConnectPool", 0)
	
	local log_base = ini:ReadString("Log", "ConnectString", "")
	local log_pool = ini:ReadInteger("Log", "ConnectPool", 0)
	
	local public_base = ini:ReadString("Public", "ConnectString", "")
	local public_pool = ini:ReadInteger("Public", "ConnectPool", 0)
	
	local manage_base = ini:ReadString("Manage", "ConnectString", "")
	local manage_pool = ini:ReadInteger("Manage", "ConnectPool", 0)
	
	nx_destroy(ini)
	
	--创建数据库系统
	local db_system
	
	if use_mysql then
		db_system = nx_create("MysqlDBSystem")
	else
		db_system = nx_create("SqlDBSystem")
	end
	
	--绑定脚本
	nx_bind_script(store, "main", "echo_init")
	
	store.AppName = "store_" .. nx_string(server_id) .. "_" .. nx_string(store_id)
	store.DistrictId = district_id
	store.ServerId = server_id
	store.StoreId = store_id
	store.AccountBase = account_base
	store.AccountPools = account_pool
	store.WSASendArrays = wsasend_arrays
	store.WSASendBufLen = wsasend_buflen
	store.RoleBase = role_base
	store.RolePools = role_pool
	store.LogBase = log_base
	store.LogPools = log_pool
	store.PublicBase = public_base
	store.PublicPools = public_pool
	store.ManageBase = manage_base
	store.ManagePools = manage_pool
	store.DBSystem = db_system
	store.InnerAddr = inner_addr
	store.InnerPort = inner_port
	store.BroadAddr = broad_addr
	store.BroadPort = broad_port
	store.EchoAddr = echo_addr
	store.EchoPort = echo_port
	store.WakeAddr = wake_addr
	store.WakePort = wake_port
	--记录数据库存取性能日志 
	store.LogDBPerformance = log_db_perf
	--保证启动日志服务
	store.LogLauncher = "game_log game_log.ini daemon"
	store.SqlBatchNum = log_batch_num
	store.SplitWeek = log_split_week
	
	init_custom_log(store)
	init_split_log(store, log_auto_serial)
	
	--启动存储服务
	store:Start()
	
	--实际使用的内网侦听端口
	inner_port = store.InnerPort
	--实际使用的唤醒侦听端口
	wake_port = store.WakePort
	
	--显示应用程序名
	store:WriteLine("app name " .. store.AppName)
	--显示通讯地址和端口
	store:WriteLine("echo socket " .. echo_addr .. ":" .. nx_string(echo_port))
	store:WriteLine("inner socket " .. inner_addr .. ":" .. nx_string(inner_port))
	store:WriteLine("broadcast socket " .. broad_addr .. ":" .. nx_string(broad_port))
	store:WriteLine("wake socket " .. wake_addr .. ":" .. nx_string(wake_port))
	--启动完成
	store:WriteLine("game server " .. nx_string(server_id) .. " store "
		.. nx_string(store_id) .. " started")
	
	--设置进程开启状态
	store:SetAppState(WAPS_OPENED);
	
	return 1
end

--设置自定义的游戏日志数据库表格结构
function init_custom_log(game_store)

	--游戏玩法日志表
	game_store:AddLogTable("game_player_action_log")
	game_store:AddLogField("game_player_action_log", "c_account", "varchar", 50)
	game_store:AddLogField("game_player_action_log", "n_nation", "int", 11)
	game_store:AddLogField("game_player_action_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("game_player_action_log", "d_create", "datetime", 8)
	game_store:AddLogField("game_player_action_log", "n_action_type", "int", 11)
	game_store:AddLogField("game_player_action_log", "n_scene_id", "int", 11)
	game_store:AddLogField("game_player_action_log", "n_action_state", "int", 4)
	game_store:AddLogField("game_player_action_log", "s_comment", "nvarchar", 255)

	
	--玩家登录日志表
	game_store:AddLogTable("role_log")
	game_store:AddLogField("role_log", "c_account", "varchar", 50)
	game_store:AddLogField("role_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("role_log", "c_ip", "varchar", 50)
	game_store:AddLogField("role_log", "d_login_time", "datetime", 8)
	game_store:AddLogField("role_log", "d_logout_time", "datetime", 8)
	game_store:AddLogField("role_log", "c_mac", "varchar", 200)
	game_store:AddLogField("role_log", "c_os_type", "varchar", 50)	
	game_store:AddLogField("role_log", "s_comment", "nvarchar", 255)
	
	
	--任务日志表
	game_store:AddLogTable("task_log")
	game_store:AddLogField("task_log", "c_account", "varchar", 50)
	game_store:AddLogField("task_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("task_log", "d_create", "datetime", 8)		
	game_store:AddLogField("task_log", "c_task_id", "varchar", 50)	
	game_store:AddLogField("task_log", "n_type", "int", 4)
	game_store:AddLogField("task_log", "n_state", "int", 4)
	game_store:AddLogField("task_log", "s_comment", "nvarchar", 255)

	
    --游戏金钱异动表
    game_store:AddLogTable("money_log")
	game_store:AddLogField("money_log", "time", "datetime", 8)
	game_store:AddLogField("money_log", "store_id", "int", 4)
	game_store:AddLogField("money_log", "recover_id", "int", 4)
	game_store:AddLogField("money_log", "self_account", "varchar", 50)
	game_store:AddLogField("money_log", "self_name", "nvarchar", 50)
	game_store:AddLogField("money_log", "self_uid", "varchar", 32)
	game_store:AddLogField("money_log", "event_id", "int", 4)
	game_store:AddLogField("money_log", "event_type", "int", 4)
	game_store:AddLogField("money_log", "event_serial", "varchar", 32)
	game_store:AddLogField("money_log", "money_type", "int", 4)
	game_store:AddLogField("money_log", "`before`", "int", 11)
	game_store:AddLogField("money_log", "num", "int", 11)
	game_store:AddLogField("money_log", "`after`", "int", 11)
	game_store:AddLogField("money_log", "sender_name", "nvarchar", 50)
	game_store:AddLogField("money_log", "sender_uid", "varchar", 32)
	game_store:AddLogField("money_log", "scene_id", "int", 11)
	game_store:AddLogField("money_log", "guild_id", "varchar", 50)
    game_store:AddLogField("money_log", "guild_name", "nvarchar", 50)
    game_store:AddLogField("money_log", "n_state", "int", 4)
	game_store:AddLogField("money_log", "comment", "nvarchar", 255)


	--物品异动日志表
    game_store:AddLogTable("item_log")
	game_store:AddLogField("item_log", "time", "datetime", 8)
	game_store:AddLogField("item_log", "store_id", "int", 4)
	game_store:AddLogField("item_log", "recover_id", "int", 4)
	game_store:AddLogField("item_log", "self_account", "varchar", 50)
	game_store:AddLogField("item_log", "self_name", "nvarchar", 50)
	game_store:AddLogField("item_log", "self_uid", "varchar", 32)
	game_store:AddLogField("item_log", "event_id", "int", 4)
	game_store:AddLogField("item_log", "event_type", "int", 4)
	game_store:AddLogField("item_log", "event_serial", "varchar", 32)
	game_store:AddLogField("item_log", "item_id", "varchar", 128)
	game_store:AddLogField("item_log", "item_uid", "varchar", 32)
	game_store:AddLogField("item_log", "item_lifetime", "int", 4)
	game_store:AddLogField("item_log", "`before`", "int", 4)
	game_store:AddLogField("item_log", "num", "int", 4)
	game_store:AddLogField("item_log", "`after`", "int", 4)
	game_store:AddLogField("item_log", "n_color_level", "int", 4)
	game_store:AddLogField("item_log", "sender_name", "nvarchar", 50)
	game_store:AddLogField("item_log", "sender_uid", "varchar", 32)
	game_store:AddLogField("item_log", "scene_id", "int", 11)
	game_store:AddLogField("item_log", "guild_id", "varchar", 50)
    game_store:AddLogField("item_log", "guild_name", "nvarchar", 50)
	game_store:AddLogField("item_log", "n_state", "int", 4)
	game_store:AddLogField("item_log", "comment", "nvarchar", 255)

		
	--玩家升级日志表
	game_store:AddLogTable("role_upgrade_log")
	game_store:AddLogField("role_upgrade_log", "c_account", "varchar", 50)
	game_store:AddLogField("role_upgrade_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("role_upgrade_log", "d_create", "datetime", 8)
	game_store:AddLogField("role_upgrade_log", "n_upgrade_type", "int", 11)
	game_store:AddLogField("role_upgrade_log", "n_level_before", "int", 11)
	game_store:AddLogField("role_upgrade_log", "n_level_after", "int", 11)
	game_store:AddLogField("role_upgrade_log", "c_skill_id", "varchar", 50)
	game_store:AddLogField("role_upgrade_log", "s_comment", "nvarchar", 255)

	--惩罚日志表
	game_store:AddLogTable("punish_log")
	game_store:AddLogField("punish_log", "c_gm_account", "varchar", 50)
	game_store:AddLogField("punish_log", "c_account", "varchar", 50)
	game_store:AddLogField("punish_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("punish_log", "d_create", "datetime", 8)
	game_store:AddLogField("punish_log", "n_type", "int", 11)
	game_store:AddLogField("punish_log", "n_minutes", "int", 11)
	game_store:AddLogField("punish_log", "s_comment", "nvarchar", 255)

	--装备日志表
	game_store:AddLogTable("equip_log")
	game_store:AddLogField("equip_log", "c_account", "varchar", 50)
	game_store:AddLogField("equip_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("equip_log", "d_create", "datetime", 8)
	game_store:AddLogField("equip_log", "n_operate_type", "int", 11)
	game_store:AddLogField("equip_log", "n_equip_pos", "int", 4)
	game_store:AddLogField("equip_log", "n_equip_slot", "int", 4)
	game_store:AddLogField("equip_log", "n_before_grade", "int", 4)
	game_store:AddLogField("equip_log", "n_after_grade", "int", 4)	
	game_store:AddLogField("equip_log", "c_source_item_id", "varchar", 50)
	game_store:AddLogField("equip_log", "c_target_item_id", "varchar", 50)
	game_store:AddLogField("equip_log", "c_deduct_item_ids", "varchar", 200)
	game_store:AddLogField("equip_log", "c_deduct_item_nums", "varchar", 50)
	game_store:AddLogField("equip_log", "n_capital_type", "int", 4)	
	game_store:AddLogField("equip_log", "n_capital_num", "int", 11)	
	game_store:AddLogField("equip_log", "s_comment", "nvarchar", 255)
	
	--好友日志表
	game_store:AddLogTable("friend_log")
	game_store:AddLogField("friend_log", "c_account", "varchar", 50)
	game_store:AddLogField("friend_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("friend_log", "d_create", "datetime", 8)
	game_store:AddLogField("friend_log", "s_friend_name", "nvarchar", 50)
	game_store:AddLogField("friend_log", "n_type", "int", 11)
	game_store:AddLogField("friend_log", "s_comment", "nvarchar", 255)
	
	--帮会日志表
	game_store:AddLogTable("guild_log")
	game_store:AddLogField("guild_log", "c_account", "varchar", 50)
	game_store:AddLogField("guild_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("guild_log", "d_create", "datetime", 8)
	game_store:AddLogField("guild_log", "s_guild_name", "nvarchar", 50)	
	game_store:AddLogField("guild_log", "n_grade", "int", 4)
	game_store:AddLogField("guild_log", "s_by_role_name", "nvarchar", 50)
	game_store:AddLogField("guild_log", "n_operate_type", "int", 11)
	game_store:AddLogField("guild_log", "n_capital_type", "int", 11)
	game_store:AddLogField("guild_log", "n_capital_num", "int", 11)
	game_store:AddLogField("guild_log", "n_donate_num", "int", 11)
	game_store:AddLogField("guild_log", "s_comment", "nvarchar", 255)


	--场景日志表
	game_store:AddLogTable("scene_log")
	game_store:AddLogField("scene_log", "c_account", "varchar", 50)
	game_store:AddLogField("scene_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("scene_log", "n_scene_id", "int", 11)	
	game_store:AddLogField("scene_log", "d_login_time", "datetime", 8)
	game_store:AddLogField("scene_log", "d_logout_time", "datetime", 8)
	game_store:AddLogField("scene_log", "n_total_time", "int", 11)
	game_store:AddLogField("scene_log", "s_comment", "nvarchar", 255)

	
	--BOSS日志表
	game_store:AddLogTable("boss_log")
	game_store:AddLogField("boss_log", "c_account", "varchar", 50)
	game_store:AddLogField("boss_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("boss_log", "d_create", "datetime", 8)
	game_store:AddLogField("boss_log", "n_scene_id", "int", 11)	
	game_store:AddLogField("boss_log", "c_boss_id", "varchar", 50)
	game_store:AddLogField("boss_log", "s_comment", "nvarchar", 255)

    --宠物坐骑日志表
	game_store:AddLogTable("pet_ride_log")
	game_store:AddLogField("pet_ride_log", "c_account", "varchar", 50)
	game_store:AddLogField("pet_ride_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("pet_ride_log", "d_create", "datetime", 8)
	game_store:AddLogField("pet_ride_log", "c_pet_id", "varchar", 50)
	game_store:AddLogField("pet_ride_log", "n_operate_type", "int", 11)
	game_store:AddLogField("pet_ride_log", "n_level_before", "int", 11)
	game_store:AddLogField("pet_ride_log", "n_level_after", "int", 11)
	game_store:AddLogField("pet_ride_log", "c_skill_id", "varchar", 50)
	game_store:AddLogField("pet_ride_log", "s_comment", "nvarchar", 255)

	--PVP日志表
	game_store:AddLogTable("pvp_log")
	game_store:AddLogField("pvp_log", "c_account", "varchar", 50)
	game_store:AddLogField("pvp_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("pvp_log", "d_create", "datetime", 8)
	game_store:AddLogField("pvp_log", "d_end", "datetime", 8)
	game_store:AddLogField("pvp_log", "s_be_challenge", "nvarchar", 50)
	game_store:AddLogField("pvp_log", "n_rank_before", "int", 11)
	game_store:AddLogField("pvp_log", "n_rank_after", "int", 11)
	game_store:AddLogField("pvp_log", "n_result", "int", 4)
	game_store:AddLogField("pvp_log", "s_comment", "nvarchar", 255)

	--国家系统日志表
	game_store:AddLogTable("nation_log")
	game_store:AddLogField("nation_log", "c_account", "varchar", 50)
	game_store:AddLogField("nation_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("nation_log", "d_create", "datetime", 8)
	game_store:AddLogField("nation_log", "n_operate_type", "int", 11)
	game_store:AddLogField("nation_log", "c_params", "varchar", 50)
	game_store:AddLogField("nation_log", "s_comment", "nvarchar", 255)

	--密境日志表
	game_store:AddLogTable("sceret_log")
	game_store:AddLogField("sceret_log", "c_account", "varchar", 50)
	game_store:AddLogField("sceret_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("sceret_log", "d_create", "datetime", 8)
	game_store:AddLogField("sceret_log", "d_end", "datetime", 8)
	game_store:AddLogField("sceret_log", "n_sceret_id", "int", 11)
	game_store:AddLogField("sceret_log", "n_dead", "int", 11)
	game_store:AddLogField("sceret_log", "n_result", "int", 4)
	game_store:AddLogField("sceret_log", "s_comment", "nvarchar", 255)
	
	
	--称号日志表
	game_store:AddLogTable("title_log")
	game_store:AddLogField("title_log", "c_account", "varchar", 50)
	game_store:AddLogField("title_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("title_log", "d_create", "datetime", 8)
	game_store:AddLogField("title_log", "n_title_id", "int", 11)
	game_store:AddLogField("title_log", "n_operate_type", "int", 11)
	game_store:AddLogField("title_log", "s_comment", "nvarchar", 255)
		
		
	--抽奖日志表
	game_store:AddLogTable("extract_award_log")
	game_store:AddLogField("extract_award_log", "c_account", "varchar", 50)
	game_store:AddLogField("extract_award_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("extract_award_log", "d_create", "datetime", 8)
	game_store:AddLogField("extract_award_log", "n_battle_ability", "int", 11)
	game_store:AddLogField("extract_award_log", "n_extract_type", "int", 11)
	game_store:AddLogField("extract_award_log", "c_params", "varchar", 50)
	game_store:AddLogField("extract_award_log", "n_award_level", "int", 11)
	game_store:AddLogField("extract_award_log", "s_comment", "nvarchar", 255)
				
				
	--充值日志表
	game_store:AddLogTable("traceorder_log")
	game_store:AddLogField("traceorder_log", "c_account", "varchar", 50)
	game_store:AddLogField("traceorder_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("traceorder_log", "d_create", "datetime", 8)
	game_store:AddLogField("traceorder_log", "n_custom_type", "int", 11)
	game_store:AddLogField("traceorder_log", "c_buy_index", "varchar", 50)
	game_store:AddLogField("traceorder_log", "c_trace_order", "varchar", 128)
	game_store:AddLogField("traceorder_log", "c_money", "varchar", 50)
	game_store:AddLogField("traceorder_log", "s_comment", "nvarchar", 255)
	
	
    --邮件日志表
	game_store:AddLogTable("mail_log")
	game_store:AddLogField("mail_log", "c_account", "varchar", 50)
	game_store:AddLogField("mail_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("mail_log", "d_create", "datetime", 8)
	game_store:AddLogField("mail_log", "c_template_type", "varchar", 50)
	game_store:AddLogField("mail_log", "c_capital_params", "varchar", 255)
	game_store:AddLogField("mail_log", "c_item_params", "varchar", 255)
	game_store:AddLogField("mail_log", "s_comment", "nvarchar", 255)

	--经脉日志表
	game_store:AddLogTable("meridian_log")
	game_store:AddLogField("meridian_log", "c_account", "varchar", 50)
	game_store:AddLogField("meridian_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("meridian_log", "d_create", "datetime", 8)
	game_store:AddLogField("meridian_log", "n_operate_type", "int", 11)
	game_store:AddLogField("meridian_log", "n_stone_id", "varchar", 50)
	game_store:AddLogField("meridian_log", "n_before_grade", "int", 11)
	game_store:AddLogField("meridian_log", "n_after_grade", "int", 11)
	game_store:AddLogField("meridian_log", "n_before_exp", "int", 11)
	game_store:AddLogField("meridian_log", "n_after_exp", "int", 11)
	game_store:AddLogField("meridian_log", "c_deduct_item_ids", "varchar", 200)
	game_store:AddLogField("meridian_log", "c_deduct_item_nums", "varchar", 50)
	game_store:AddLogField("meridian_log", "n_capital_type", "int", 4)
	game_store:AddLogField("meridian_log", "n_capital_num", "int", 11)
	game_store:AddLogField("meridian_log", "n_pak_list", "varchar", 50)

	--生死擂日志表
	game_store:AddLogTable("revenge_arena_log")
	game_store:AddLogField("revenge_arena_log", "c_account", "varchar", 50)
	game_store:AddLogField("revenge_arena_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("revenge_arena_log", "d_start_time", "datetime", 8)
	game_store:AddLogField("revenge_arena_log", "d_end_time", "datetime", 8)
	game_store:AddLogField("revenge_arena_log", "s_enemy_name", "nvarchar", 50)
	game_store:AddLogField("revenge_arena_log", "n_result", "int", 4)
	
	--时装日志表
	game_store:AddLogTable("fashion_log")
	game_store:AddLogField("fashion_log", "c_account", "varchar", 50)
	game_store:AddLogField("fashion_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("fashion_log", "d_create", "datetime", 8)
	game_store:AddLogField("fashion_log", "n_operate_type", "int", 11)
	game_store:AddLogField("fashion_log", "c_fashion_id", "varchar", 50)
	game_store:AddLogField("fashion_log", "c_unlock_items", "varchar", 255)
	game_store:AddLogField("fashion_log", "c_unlock_capitals", "varchar", 255)
	game_store:AddLogField("fashion_log", "s_comment", "nvarchar", 255)
	
	-- 交易记录
	game_store:AddLogTable("transction_log")
	game_store:AddLogField("transction_log", "c_account", "varchar", 50)
	game_store:AddLogField("transction_log", "s_role_name", "nvarchar", 50)
	game_store:AddLogField("transction_log", "s_target_name", "nvarchar", 50)
	game_store:AddLogField("transction_log", "c_item_id", "varchar", 255)
	game_store:AddLogField("transction_log", "n_item_num", "int", 11)
	game_store:AddLogField("transction_log", "n_item_price", "int", 11)
	game_store:AddLogField("transction_log", "n_operate_type", "int", 11)
	game_store:AddLogField("transction_log", "d_timestamp", "datetime", 8)
	
	return true
end

--关闭
function close()
	local store = nx_value("store")
	
	--设置进程正在关闭状态
	--store:SetAppState(WAPS_CLOSING);
	
	--等待所有数据库操作完成
	--store:WriteLine("waiting all access finish...")
	
	--while not store:AccessEmpty() do
	--	nx_pause(0.0)
	--end
	
	--关闭存储服务
	store:WriteLine("store server shutdown...")
	store:Stop()
	
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
		.. " store " .. nx_string(self.StoreId) .. "\r")
	return 1
end

--维护连接关闭
function echo_close(self, addr, port)
	self:WriteLine("echo client " .. nx_string(addr) 
		.. ":" .. nx_string(port) .. " closed")
	return 1
end

--发送回应信息
function echo_response(self, info)
	self:SendEchoLine(">" .. info .. "\r")
	self:WriteLine(info)
	return true
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
	elseif command == "es" then
		--生成加密的数据库连接串
		local fp = io.open("es.txt", "wb")
		
		if fp ~= nil then
			fp:write("[Account]\r\nConnectString=")
			fp:write(self:EncodeConnectString(self.AccountBase) .. "\r\n\r\n")
			fp:write("[Role]\r\nConnectString=")
			fp:write(self:EncodeConnectString(self.RoleBase) .. "\r\n\r\n")
			fp:write("[Log]\r\nConnectString=")
			fp:write(self:EncodeConnectString(self.LogBase) .. "\r\n\r\n")
			fp:write("[Public]\r\nConnectString=")
			fp:write(self:EncodeConnectString(self.PublicBase) .. "\r\n\r\n")
			fp:write("[Manage]\r\nConnectString=")
			fp:write(self:EncodeConnectString(self.ManageBase) .. "\r\n\r\n")
			fp:close()
			
			echo_response(self, "write file es.txt succeed")
		else
			echo_response(self, "write file es.txt failed")
		end
	else
		echo_response(self, "undefined command: " .. command)
	end
	
	return 1
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



--设置自动创建分割表模板
function init_split_log(store, log_auto_serial)

local bAutoInc = log_auto_serial
local auto_inc_key = "`auto_int_serial`  int(8) NOT NULL AUTO_INCREMENT,"
local auto_prim_key = "PRIMARY KEY (`auto_int_serial`)"

if not bAutoInc then
    auto_inc_key = ""
    auto_prim_key = "PRIMARY KEY (`serial`)"
end

--游戏玩法日志表
store:AddLogCreator("game_player_action_log", "CREATE TABLE `game_player_action_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_nation` int(11) DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_action_type` int(11) DEFAULT NULL," ..
	"`n_scene_id` int(11) DEFAULT NULL," ..
	"`n_action_state` int(4) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_game_player_action_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_game_player_action_log_s_role_name` (`s_role_name`) USING BTREE,"..
    "KEY `idx_game_player_action_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_game_player_action_log_n_action_type` (`n_action_type`) USING BTREE," ..
    "KEY `idx_game_player_action_log_n_action_state` (`n_action_state`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
	
--玩家登录日志表
store:AddLogCreator("role_log", "CREATE TABLE `role_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_ip` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_login_time` datetime DEFAULT NULL," ..
	"`d_logout_time` datetime DEFAULT NULL," ..
	"`c_mac` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_os_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_role_log_d_login_time` (`d_login_time`) USING BTREE," ..
	"KEY `idx_role_log_s_role_name` (`s_role_name`) USING BTREE," ..
	"KEY `idx_role_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
--任务日志表
store:AddLogCreator("task_log", "CREATE TABLE `task_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`c_task_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_type` int(4) DEFAULT NULL," ..
	"`n_state` int(4) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_task_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_task_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_task_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_task_log_n_type` (`n_type`) USING BTREE," ..
	"KEY `idx_task_log_n_state` (`n_state`) USING BTREE," ..
    "KEY `idx_task_log_c_task_id` (`c_task_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
	
--金钱日志表
store:AddLogCreator("money_log", "CREATE TABLE `money_log%s` (" ..
    auto_inc_key ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`time` datetime DEFAULT NULL," ..
	"`store_id` int(4) DEFAULT NULL," ..
	"`recover_id` int(4) DEFAULT NULL," ..
	"`self_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`self_name` varchar(64) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`self_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`event_id` int(4) DEFAULT NULL," ..
	"`event_type` int(11) DEFAULT NULL," ..
	"`event_serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`money_type` int(4) DEFAULT NULL," ..
	"`before` int(11) DEFAULT NULL," ..
	"`num` int(11) DEFAULT NULL," ..
	"`after` int(11) DEFAULT NULL," ..
	"`sender_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`sender_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`scene_id` int(11) DEFAULT NULL," ..
	"`guild_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
    "`guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_state` int(4) DEFAULT NULL," ..
	"`comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	auto_prim_key .. "," ..
	"KEY `idx_money_log_self_name` (`self_name`) USING BTREE," ..
    "KEY `idx_money_log_self_account` (`self_account`) USING BTREE," ..
    "KEY `idx_money_log_time` (`time`) USING BTREE," ..
    "KEY `idx_money_log_event_id` (`event_id`) USING BTREE," ..
    "KEY `idx_money_log_self_uid` (`self_uid`) USING BTREE," ..
    "KEY `idx_money_log_scene_id` (`scene_id`) USING BTREE," ..
    "KEY `idx_money_log_money_type` (`money_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

	
	
--物品日志表
store:AddLogCreator("item_log", "CREATE TABLE `item_log%s` (" ..
    auto_inc_key ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`time` datetime DEFAULT NULL," ..
	"`store_id` int(4) DEFAULT NULL," ..
	"`recover_id` int(4) DEFAULT NULL," ..
	"`self_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`self_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`self_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`event_id` int(4) DEFAULT NULL," ..
	"`event_type` int(4) DEFAULT NULL," ..
	"`event_serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`item_id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`item_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`item_lifetime` int(4) DEFAULT NULL," ..
	"`before` int(4) DEFAULT NULL," ..
	"`num` int(4) DEFAULT NULL," ..
	"`after` int(4) DEFAULT NULL," ..
	"`n_color_level` int(4) DEFAULT NULL," ..
	"`sender_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`sender_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`scene_id` int(11) DEFAULT NULL," ..
	"`guild_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
    "`guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_state` int(4) DEFAULT NULL," ..
	"`comment` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	auto_prim_key .. "," ..
	"KEY `idx_item_log_self_name` (`self_name`) USING BTREE," ..
    "KEY `idx_item_log_item_id` (`item_id`) USING BTREE," ..
    "KEY `idx_item_log_self_account` (`self_account`) USING BTREE," ..
    "KEY `idx_item_log_time` (`time`) USING BTREE," ..
    "KEY `idx_item_log_event_id` (`event_id`) USING BTREE," ..
    "KEY `idx_item_log_self_uid` (`self_uid`) USING BTREE," ..
    "KEY `idx_item_log_scene_id` (`scene_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

	
	
--玩家升级日志表
store:AddLogCreator("role_upgrade_log", "CREATE TABLE `role_upgrade_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_upgrade_type` int(11) DEFAULT NULL," ..
	"`n_level_before` int(11) DEFAULT NULL," ..
	"`n_level_after` int(11) DEFAULT NULL," ..
	"`c_skill_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_role_upgrade_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_role_upgrade_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_role_upgrade_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_role_upgrade_log_n_upgrade_type` (`n_upgrade_type`) USING BTREE," ..
    "KEY `idx_role_upgrade_log_c_skill_id` (`c_skill_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
--惩罚日志表
store:AddLogCreator("punish_log", "CREATE TABLE `punish_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_gm_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_type` int(11) DEFAULT NULL," ..
	"`n_minutes` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_punish_log_d_create` (`d_create`) USING BTREE," ..
	"KEY `idx_punish_log_s_role_name` (`s_role_name`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
--装备日志表
store:AddLogCreator("equip_log", "CREATE TABLE `equip_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`n_equip_pos` int(11) DEFAULT NULL," ..
	"`n_equip_slot` int(11) DEFAULT NULL," ..
	"`n_before_grade` int(11) DEFAULT NULL," ..
	"`n_after_grade` int(11) DEFAULT NULL," ..
	"`c_source_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_target_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..	
	"`c_deduct_item_ids` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_deduct_item_nums` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_capital_type` int(11) DEFAULT NULL," ..
	"`n_capital_num` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_equip_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_equip_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_equip_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_equip_log_n_operate_type` (`n_operate_type`) USING BTREE," ..
    "KEY `idx_equip_log_c_source_item_id` (`c_source_item_id`) USING BTREE," ..
    "KEY `idx_equip_log_c_target_item_id` (`c_target_item_id`) USING BTREE," ..
    "KEY `idx_equip_log_c_deduct_item_ids` (`c_deduct_item_ids`) USING BTREE," ..
    "KEY `idx_equip_log_n_capital_type` (`n_capital_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	

--好友日志表
store:AddLogCreator("friend_log", "CREATE TABLE `friend_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`s_friend_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_type` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_friend_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_friend_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_friend_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_friend_log_n_type` (`n_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
--公会日志表
store:AddLogCreator("guild_log", "CREATE TABLE `guild_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`s_guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_grade` int(4) DEFAULT NULL," ..
    "`s_by_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`n_capital_type` int(11) DEFAULT NULL," ..
	"`n_capital_num` int(11) DEFAULT NULL," ..
	"`n_donate_num` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_guild_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_guild_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_guild_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_guild_log_s_guild_name` (`s_guild_name`) USING BTREE," ..
    "KEY `idx_guild_log_n_operate_type` (`n_operate_type`) USING BTREE," ..
    "KEY `idx_guild_log_n_capital_type` (`n_capital_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	

--Boss日志表
store:AddLogCreator("boss_log", "CREATE TABLE `boss_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_scene_id` int(11) DEFAULT NULL," ..	
    "`c_boss_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_boss_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_boss_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_boss_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_boss_log_n_scene_id` (`n_scene_id`) USING BTREE," ..
    "KEY `idx_boss_log_c_boss_id` (`c_boss_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
--场景日志表
store:AddLogCreator("scene_log", "CREATE TABLE `scene_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_scene_id` int(11) DEFAULT NULL," ..
	"`d_login_time` datetime DEFAULT NULL," ..
	"`d_logout_time` datetime DEFAULT NULL," ..
	"`n_total_time` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..	
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_scene_log_d_login_time` (`d_login_time`) USING BTREE," ..
    "KEY `idx_scene_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_scene_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_scene_log_n_scene_id` (`n_scene_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")	

--聊天日志
store:AddLogCreator("nx_chatlog", "CREATE TABLE `nx_chatlog%s` (" ..
	"`serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`chat_time` datetime DEFAULT NULL," ..
	"`chat_from` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`chat_to` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`chat_type` int(11) DEFAULT NULL," ..
	"`chat_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`chat_comment` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial_no`)," ..
	"KEY `IX_nx_chatlog_1` (`chat_time`) USING BTREE," ..
	"KEY `IX_nx_chatlog_2` (`chat_from`) USING BTREE," ..
	"KEY `IX_nx_chatlog_3` (`chat_to`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

--GM日志表
store:AddLogCreator("nx_gmlog", "CREATE TABLE `nx_gmlog%s` (" ..
	"`serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`log_time` datetime DEFAULT NULL," ..
	"`log_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`log_source` int(11) DEFAULT NULL," ..
	"`log_type` int(11) DEFAULT NULL," ..
	"`log_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`log_comment` varchar(2048) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial_no`)," ..
	"KEY `IX_nx_gmlog_1` (`log_time`) USING BTREE," ..
	"KEY `IX_nx_gmlog_2` (`log_name`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

--log数据
store:AddLogCreator("nx_logdata", "CREATE TABLE `nx_logdata%s` (" ..
	"`serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`log_time` datetime DEFAULT NULL," ..
	"`log_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`log_source` int(11) DEFAULT NULL," ..
	"`log_type` int(11) DEFAULT NULL," ..
	"`log_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`log_comment` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial_no`)," ..
	"KEY `idx_nx_logdata_log_time` (`log_time`) USING BTREE," ..
    "KEY `idx_nx_logdata_log_name` (`log_name`) USING BTREE," ..
    "KEY `idx_nx_logdata_log_type` (`log_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	

--宠物坐骑日志表
store:AddLogCreator("pet_ride_log", "CREATE TABLE `pet_ride_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`c_pet_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`n_level_before` int(11) DEFAULT NULL," ..
	"`n_level_after` int(11) DEFAULT NULL," ..
	"`c_skill_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_pet_ride_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_pet_ride_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_pet_ride_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_pet_ride_log_n_operate_type` (`n_operate_type`) USING BTREE," ..
    "KEY `idx_pet_ride_log_c_pet_id` (`c_pet_id`) USING BTREE," ..
    "KEY `idx_pet_ride_log_c_skill_id` (`c_skill_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
--PVP日志表
store:AddLogCreator("pvp_log", "CREATE TABLE `pvp_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`d_end` datetime DEFAULT NULL," ..
	"`s_be_challenge` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_rank_before` int(11) DEFAULT NULL," ..
	"`n_rank_after` int(11) DEFAULT NULL," ..
	"`n_result` int(4) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_pvp_log_d_create` (`d_create`) USING BTREE," ..
	"KEY `idx_pvp_log_s_role_name` (`s_role_name`) USING BTREE," ..
	"KEY `idx_pvp_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	

--国家日志表
store:AddLogCreator("nation_log", "CREATE TABLE `nation_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
    "`c_params` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_nation_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_nation_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_nation_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_nation_log_n_operate_type` (`n_operate_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")


--密境日志表
store:AddLogCreator("sceret_log", "CREATE TABLE `sceret_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`d_end` datetime DEFAULT NULL," ..
	"`n_sceret_id` int(11) DEFAULT NULL," ..
	"`n_dead` int(11) DEFAULT NULL," ..
    "`n_result` int(4) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_sceret_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_sceret_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_sceret_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_sceret_log_n_sceret_id` (`n_sceret_id`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

	
--称号日志表
store:AddLogCreator("title_log", "CREATE TABLE `title_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_title_id` int(11) DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_title_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_title_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_title_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_title_log_n_operate_type` (`n_operate_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

--抽奖日志表
store:AddLogCreator("extract_award_log", "CREATE TABLE `extract_award_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_battle_ability` int(11) DEFAULT NULL," ..
	"`n_extract_type` int(11) DEFAULT NULL," ..
	"`c_params` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_award_level` int(11) DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_extract_award_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_extract_award_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_extract_award_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_extract_award_log_n_extract_type` (`n_extract_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

	
--订单日志表
store:AddLogCreator("traceorder_log", "CREATE TABLE `traceorder_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_custom_type` int(11) DEFAULT NULL," ..
	"`c_buy_index` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_trace_order` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_money` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_traceorder_log_d_create` (`d_create`) USING BTREE," ..
    "KEY `idx_traceorder_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_traceorder_log_c_account` (`c_account`) USING BTREE," ..
    "KEY `idx_traceorder_log_n_custom_type` (`n_custom_type`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")


--邮件日志表
store:AddLogCreator("mail_log", "CREATE TABLE `mail_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
    "`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`c_template_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_capital_params` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_item_params` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"INDEX `idx_mail_log_d_create` (`d_create`) USING BTREE," ..
	"INDEX `idx_mail_log_s_role_name` (`s_role_name`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
--经脉日志表
store:AddLogCreator("meridian_log", "CREATE TABLE `meridian_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`n_stone_id` varchar(50) DEFAULT NULL," ..
	"`n_before_grade` int(11) DEFAULT NULL," ..
	"`n_after_grade` int(11) DEFAULT NULL," ..
	"`n_before_exp` int(11) DEFAULT NULL," ..
	"`n_after_exp` int(11) DEFAULT NULL," ..
	"`c_deduct_item_ids` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_deduct_item_nums` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_capital_type` int(4) DEFAULT NULL," ..
	"`n_capital_num` int(11) DEFAULT NULL," ..
	"`n_pak_list` varchar(50) DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
    "KEY `idx_wing_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_wing_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")

--生死擂日志表
store:AddLogCreator("revenge_arena_log", "CREATE TABLE `revenge_arena_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_start_time` datetime DEFAULT NULL," ..
	"`d_end_time` datetime DEFAULT NULL," ..
	"`s_enemy_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_result` int(4) DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
	"KEY `idx_revenge_log_d_start` (`d_start_time`) USING BTREE," ..
    "KEY `idx_revenge_arena_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_revenge_arena_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
--时装日志表
store:AddLogCreator("fashion_log", "CREATE TABLE `fashion_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`d_create` datetime DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`c_fashion_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_unlock_items` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_unlock_capitals` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
    "KEY `idx_fashion_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_fashion_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
	
--交易表
store:AddLogCreator("transction_log", "CREATE TABLE `transction_log%s` (" ..
	"`serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL," ..
	"`c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`s_target_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`c_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL," ..
	"`n_item_num` int(11) DEFAULT NULL," ..
	"`n_item_price` int(11) DEFAULT NULL," ..
	"`n_operate_type` int(11) DEFAULT NULL," ..
	"`d_timestamp` datetime DEFAULT NULL," ..
	"PRIMARY KEY (`serial`)," ..
    "KEY `idx_transcation_log_s_role_name` (`s_role_name`) USING BTREE," ..
    "KEY `idx_transcation_log_c_account` (`c_account`) USING BTREE" ..
	") ENGINE=MyISAM DEFAULT CHARSET=utf8;")
	
   return true
end