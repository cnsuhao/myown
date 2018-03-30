

local g_obj_list = {
	"DumpTool_RoleInfo",
	"DumpTool_Config",
	"DumpTool_ParseConfig",
	--"DumpTool_Guild",
	--"DumpTool_Rank",
	"DumpTool_BackupRole",
}

function main()
	-- 启动各个模块
	for i = 1, table.getn(g_obj_list) do
		local name = g_obj_list[i]
		if name ~= nil then
			local obj = nx_create(name)
			if nx_is_valid(obj) then
				nx_set_single(obj, name)
				nx_set_value(name, obj)
				obj:Start()
			else
				nx_msgbox("create module "..name.." failed")
				return i;
			end
		end
	end

	local parse = nx_value("DumpTool_ParseConfig")
	if nx_is_valid(parse) then
		parse:LoadText()
		parse:DumpConfigToFile(-1)
		parse:ClearText()
	else
		local config = nx_value("DumpTool_Config")
		if nx_is_valid(config) then
			config:BeginUpdate()
		end
	end
	
	return 0;
end