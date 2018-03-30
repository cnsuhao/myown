----------------------------游戏配置文件GM重加载---------------------------------

local TIPSTYPE_GMINFO = 0;

--重新载入当前属性操作对象的配置文件
function reload(gm)
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return
	end

	if not nx_load_config(target) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Reload config file failed!"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Reload config file succeed!"));
	end
end

-- 重新加载某个配置文件
function reload_config(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:reload_config [configkey]"));
		return;
	end

	local configkey = nx_string(arg[1]);
	nx_reload_config(configkey);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_reload_config "  .. " ok."));

end


