-- gm命令规则定义，成功返回"ok" 服务器内部不支持整数传递，数字返回double值

-- 不需要玩家支持的命令
local rule_no_player={
	x_reload_config=true,		-- 加载配置
	x_get_config_prop=true,		-- 获取物品配置
	x_servertime=true,			-- 服务器时间
	x_pub_rec_data=true,		-- 取公共数据表所有数据
	x_pub_rec_findrow=true,		-- 查找公共数据表数据行号
	x_pub_rec_clear=true,		-- 清除公共数据表
	x_pub_rec_addrow=true,		-- 添加公共数据行
	x_pub_rec_removerow=true,	-- 移除公共数据表行
	x_pub_rec_set_value=true,	-- 设置公共数据表字段值
	x_pub_rec_get_value=true,	-- 获取公共数据表字段值
	x_pub_prop_query=true,		-- 查询公共数据属性值
	x_pub_prop_set=true,		-- 设置公共数据属性值
	x_reload_lua=true,			-- 重置lua脚本[不需要扩展名]
	reload_lua=true,			-- 重置lua脚本[文件名要带.lua]
	x_import_robot_data=true,
	x_repair_pub_rec_data=true,
	x_repair_arena_rank_list14=true,
};
function run_no_player(cmd)
	if rule_no_player[cmd] == true then 
		return "ok";
	end
	
	return "no";
end

-- 可以离线执行的命令
local rule_offline = {
	x_offline_player_login = true
};
function run_offline(cmd)
	if rule_offline[cmd] == true then 
		return "ok";
	end
	
	return "no";
end

print(run_no_player("gm_x_reload_lua"));