-- NOTE: 所有函数必须返回json格式，包含content或error字段
require("gm_x_data")
require("gm_x_object")
require("gm_x_game")

local gm_helper = require("gm_x_helper")
-- [不需要扩展名]
function x_reload_lua(gm, name)
	local data = {};
	data["before"] = {};
	for k, _ in pairs(package.loaded) do
		table.insert(data.before, k );
	end
	if nil ~= name then 
		local pkgName = nx_string(name);
		package.loaded[pkgName] = nil;
		require(pkgName);
	else
		package.loaded["gm_x_data"] = nil;
		package.loaded["gm_x_object"] = nil;
		package.loaded["gm_x_game"] = nil;
	end	
	
	data["after"] = {};
	for k, _ in pairs(package.loaded) do
		table.insert(data.after, k );
	end
	return gm_helper.content(data);
end

function x_reload_lua_func(gm, func)
	_G[func] = nil;
	return gm_helper.succeed();
end