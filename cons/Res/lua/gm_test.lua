
local TIPSTYPE_GMINFO = 0;

--查找指定名称的功能模块是否已被加载
function findmodule(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:findmodule modulename"));
        return;
    end

    local modulename = nx_string(arg[1]);

    if nx_findmodule(modulename) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Module [" .. modulename .. "] loaded succeed!"));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Module [" .. modulename .. "] had not be loaded!"));
    end
end

--获取物品属性
function getconfigprop(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:getconfigprop configid prop."));
		return;
	end
	local configid = nx_int(arg[1]);
	local prop = nx_string(arg[2]);

	local value = nx_get_config_property(configid, prop);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ConfigID:" .. configid .. " " .. prop .. "=" .. value));
end

--增加一个玩家的技能
function addskill(gm, ...)
	local id = nx_string(arg[1])
	local skill = nx_create_from_config(nx_get_scene(), id)
	if not nx_exists(skill) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("create skill " .. id .. " failed"))
		return
	end

	if not nx_add_skill(gm, skill) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add skill " .. id .. " failed"))
		nx_destroy_self(skill)
		return
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add skill " .. id .. " succeed"))
end

--删除技能
function delskill(gm, ...)

	local id = nx_string(arg[1]);

	local skill = nx_find_skill(gm, id);

	if not nx_exists(skill) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("delskill not found skill " .. id));
		return
	end

	nx_remove_skill(gm,skill)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("delskill " .. id .. " success"));
end

--升级技能 type 0 升一级 1 升多级
function upgradeskill(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("upgradeskill failed"));
	return;
	end

	local id = nx_string(arg[1]);
	local type  = nx_int(arg[2]);

	nx_upgrade_skill(gm,id,type);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("upgradeskill " .. id .. " success"));
end

--清空所学技能
function clear_all_skill(gm, ...)

	if nx_clear_all_skill(gm) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("clear all skill success."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("clear all skill error!"))
	end
end

--重复做任务：repeat_task taskid
function repeat_task(gm, ...)
	if table.getn(arg) == 0 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: repeat_task <taskid>"))
		return
	end

	local taskid = nx_int(arg[1])
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end

	if nx_repeat_task(target, taskid) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("repeat_task " .. nx_string(taskid) .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("repeat_task " .. nx_string(taskid) .. "failed."))
	end
end

--refresh_task -1/0/1 taskid(可选)
function refresh_task(gm, ...)
	if table.getn(arg) == 0 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: refresg_task <type><taskid>"))
		return
	end

	local type = nx_int(arg[1])
	local taskid = nx_int(arg[2])
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end

	if nx_refresh_task(target, type,taskid) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("refresg_task " .. nx_string(type) .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("refresg_task " .. nx_string(type) .. "failed."))
	end
end

--refresh_task -1/0/1 taskid(可选)
function reset_repeat_task(gm, ...)
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end

	if nx_reset_repeat_task(target) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_repeat_task succeed"))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_repeat_task failed"))
	end
end

--给事件拥有者添加一个buff
function add_buff(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: add_buff buffid"));--格式：set 属性名称 属性值
		return;
	end

	local target = gm
	if nx_find_data(gm, "GMPropTarget") then
		local gmproptarget = nx_query_data(gm, "GMPropTarget");
		if nx_exists(gmproptarget) then
			target = gmproptarget
		end
	end

	local buff_id = nx_string(arg[1]);
	if not nx_add_buffer(target, gm, buff_id) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add buff " .. buff_id .. " failed"))
		return
	end
end

--移除一指定的BUFF
function remove_buff(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: remove_buff buffid"))
		return
	end

	local target = gm
	if nx_find_data(gm, "GMPropTarget") then
		target = nx_query_data(gm, "GMPropTarget");
	end
	if not nx_exists(target) then
		return
	end

	local buff_id = nx_string(arg[1])

	nx_remove_buffer(target, gm, buff_id)
end

-- 原地复活
function relive(gm, ...)
    if nx_relive(gm) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("player relive succeed ..."));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("player relive fail ..."));
    end
end

-- 设置副本剩余次数
function set_clone_times(gm, ...)

	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:set_clone_times [type] [times]"));
		return;
	end

	local types = nx_int(arg[1]);
	local times = nx_int(arg[2]);
	nx_set_clone_times(gm, types, times);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("set_clone_times "  .. " ok."));

end

-- 发送测试邮件
function send_test_mail(gm, ...)
	local mailID = ""
	if(table.getn(arg) == 1)then 
		mailID = nx_string(arg[1]);
	end
	nx_send_test_mail(gm,mailID);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("send_test_mail "  .. " ok."));

end

-- 给玩家发奖励
function reward(gm, ...)

	if table.getn(arg) == 0 then

		nx_reward(gm);

		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Reward self send."))

	end

	if table.getn(arg) == 1 then

		local role_name = nx_widestr(arg[1]);

		nx_reward(gm, role_name);

		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Reward other send."))

		return
	end

end

-- 清空灵魂
function clear_all_soul(gm, ...)

	nx_clear_all_soul(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("clear_all_soul "  .. " ok."));

end

--测试掉落
-- 测试掉落ID, 多个id用逗号分隔
-- drop id[,id]
function drop(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:drop_id id[,id] times"));
		return;
	end

	local id = nx_string(arg[1]);
	local cnt = nx_int(arg[2]);

	local result = nx_test_drop_id(gm, id, cnt);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("drop items ========= begin"));

	local str = nx_string("");
	for i = 1, table.getn(result), 2 do
		str = str .. ">>" .. nx_string(result[i]) .. ":" .. nx_string(result[i + 1]) .. "\n";
	end
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(str));

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("drop items ========= end"));


end

--清空包袱
function clearbagitem(gm)

	local itembox = nx_get_child(gm, nx_widestr("ItemBox"));
	local all_items = nx_get_child_list(itembox, TYPE_ITEM);
	for i = 1, table.getn(all_items) do
		nx_destroy_self(all_items[i]);
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("clearbagitem succeed!"));
end

--设置背包属性
function setLockSize(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:setLockSize [boxname] {value}"));
		return;
	end

	local boxname = nx_widestr(arg[1]);
	local value = nx_int(arg[2]);

	nx_set_container(gm, boxname, value);

	nx_sysinfo(gm, "gm_info", nx_widestr("set prop success!"));
end

--清空装备
function clearequipitem(gm)

	local equipbox = nx_get_child(gm, nx_widestr("EquipBox"));
	local all_items = nx_get_child_list(equipbox, TYPE_ITEM);
	for i = 1, table.getn(all_items) do
		nx_destroy_self(all_items[i]);
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("clearequipitem succeed!"));
end

--自己释放技能
function useskill(gm, ...)
	local id = nx_string(arg[1]);
	local target = nx_query_data(gm, "GMPropTarget");
	local skill = nx_find_skill(target, id);
	if not nx_exists(skill) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("not found skill " .. id));
		return
	end
	nx_use_skill(target, skill);
end


-- 给玩家增加经验 格式： add_exp exp_amount
-- exp_amount 经验数
function add_exp(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: add_exp exp_amount"));--格式：add_exp  经验值
		return;
	end

	local exp_amt = nx_int(arg[1]);

	if not nx_add_exp(gm, 0, exp_amt) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add_exp failed"))
		return
	end
end

--增加钱币
function inccapital(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: inccapital type Vlaue"));
		return;
	end

	local type = nx_int(arg[1]);
	local value = nx_int(arg[2]);

	if not nx_inc_capital(gm, type, value) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("inccapital failed"))
		return
	end
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("inccapital succeed"))
end

--减少钱币
function deccapital(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: deccapital type Vlaue"));
		return;
	end

	local type = nx_int(arg[1]);
	local value = nx_int(arg[2]);

	if not nx_dec_capital(gm, type, value) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("deccapital failed"))
		return
	end
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("deccapital succeed"))
end

--创建帮会
function createguild(gm, ...)
	if table.getn(arg) ~= 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: createguild Parameters should be [GUILDNAME]"));
		return;
	end

	local name = nx_widestr(arg[1]);

	if not nx_guild_create(gm, name) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("createguild failed"));
		return;
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("createguild succeed"));
end

--解散帮会
function dismissguild(gm, ...)
	if table.getn(arg) ~= 0 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: dismissguild no Parameters"));
		return;
	end

	if not nx_guild_dismiss(gm) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("dismissguild failed"));
		return;
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("dismissguild succeed"));
end

--去除冷去时间
function set_no_cooldown(gm, ...)
	nx_set_no_cooldown(gm)
end

--围点
function lineup(gm, ...)
	local target = get_prop_target(gm);
	local scene = nx_get_scene();
	if not nx_exists(target) then
		return;
	end
	if not nx_exists(scene) then
		return;
	end
	nx_line_up(target, scene);
end

--推送
function push(gm, ...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:push <pushID>"));
	return;
	end
	local pushID = nx_int(arg[1]);
	nx_push(target,pushID);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("push succeed!"));

end

--加BUFF
function addbuff(gm, ...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:addbuff <倍数>"));
	return;
	end
	local param = nx_int(arg[1]);
	nx_addbuff(target,param);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("addbuff succeed!"));

end

-- 使用物品
-- 格式: use_item config_id amount
function use_item(gm, ...)

	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: use_item config_id amount "));
		return;
	end

	local config_id = nx_string(arg[1]);
	local amount = nx_int(arg[2]);

	if not nx_use_item(gm, config_id, amount) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("use_item failed"))
		return
	end

end

-- 合并物品
-- 格式: merge_item config_id
function merge_item(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: merge_item config_id"));
		return;
	end

	local config_id = nx_string(arg[1]);

	if not nx_merge_item(gm, config_id) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("merge_item failed"))
		return
	end
end

-- 放入物品到背包
function place_item(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("place_item config_id amount"));
		return;
	end

	local config_id = nx_string(arg[1]);
	local amount = 1;
	local bind = 0;

	if table.getn(arg) == 2 then

		amount = nx_int(arg[2]);

	end
	
	if table.getn(arg) == 3 then

		bind = nx_int(arg[3]);

	end

	local ret = nx_place_item(gm, config_id, amount, bind);
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("item saved succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("item saved failed"));

	end
end

-- 查看玩家数据
function player_data(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("player_data <target_player_name>"));
		return;
	end

	local target_name = nx_string(arg[1]);


	if not nx_player_data(gm, target_name) then

		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("> player_data request failed"));

	else

		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("> player_data request send"));

	end


end

--坐骑相关
function ride(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ride <[add|del|on|off|show|hide]>"));
		return;
	end
	local opt = nx_string(arg[1]);
	local var;
	if table.getn(arg) > 1 then
		var = nx_string(arg[2]);
	else
		var = "";
	end

	if not nx_ride(gm, opt, var) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ride failed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ride succeed"));
	end
end

--导入一个语音服务用户
function voice_import_user(gm, ...)
	local target = gm
	if nx_find_data(gm, "GMPropTarget") then
		local gmproptarget = nx_query_data(gm, "GMPropTarget");
		if nx_exists(gmproptarget) then
			target = gmproptarget
		end
	end

	if not nx_voice_import_user(target) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add voice_import_user " .. " failed"))
		return
	end
end

--创建即时语音房间 1.roomtype 0世界1国家2帮会3队伍  2.国家聊天室 name为国家类型 帮会聊天室 name为帮会名 其余为空
function create_imchat_room(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("create_imchat_room"));
	return;
	end
	local roomtype = nx_int(arg[1]);
	local name = nx_string(arg[2]);
	if not nx_create_imchat_room(roomtype, name) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add create_imchat_room " .. " failed"))
		return
	end
end

--创建实时语音房间 1.roomtype 0国战1队伍  2.国战聊天室 name为国家类型 队伍聊天室 name
function create_voice_room(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("create_voice_room"));
	return;
	end
	local roomtype = nx_int(arg[1]);
	local name = nx_string(arg[2]);
	if not nx_create_voice_room(roomtype, name) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add create_voice_room " .. " failed"))
		return
	end
end

--创建实时语音房间 1.roomtype 0国战1队伍  2.国战聊天室 name为国家类型 队伍聊天室 name
function del_voice_room(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("del_voice_room"));
	return;
	end
	local roomtype = nx_int(arg[1]);
	local name = nx_string(arg[2]);
	if not nx_delete_voice_room(roomtype, name) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("del_voice_room " .. " failed"))
		return
	end
end

--创建队伍
function createbattleteam(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("createbattleteam camp movetype"));
	end

	local teamid = nx_int(arg[1]);
	nx_init_battle_npc(teamid);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("createbattleteam succeed."));
end

--随机使用一个技能
function random_useskill(gm, ...)
	local gmproptarget = nx_query_data(gm, "GMPropTarget");
	nx_use_random_skill(gm, gmproptarget);
end

--日常活动相关
--日常活动重置
function reset_daily_activity(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_daily_activity "  .. " begin."));
	nx_reset_activity(gm)

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_daily_activity "  .. " ok."));
end

--召唤一组离线玩家上线
function load_offline_player(gm,...)

	if not nx_load_offline_player() then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_load_offline_player " .. " failed"));
		return;
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_load_offline_player "  .. " ok."));
end

function secretfinish(gm,...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:secret_finish <成功或失败>"));
	return;
	end
	local param = nx_int(arg[1]);
	secret_finish(target,param);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("secret_finish"));
end


function AddSecret(gm,...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:AddSecret <秘境类型> <子类型> <值>"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local param3 = nx_int(arg[3]);
	gm_add_secret(target,param1,param2,param3);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("AddSecret success"));
end

function OpenAll(gm,...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	open_all_secret_scene(target);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Open All secret scene"));
end

function setclone(gm,...)
            local target = get_prop_target(gm);

            if table.getn(arg) < 1 then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("setclone failed"));
                return;
            end
            local value = nx_int(arg[1]);

            if not nx_exists(target) then
                return;
            end
            set_clone_coin(target,value);
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Set Clone Coin"));
end

--推送消息GM 自已
function pushmsg(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:SelfPushMsg <标题> <内容>"));
	return;
	end
	local param1 = nx_widestr(arg[1]);
	local param2 = nx_widestr(arg[2]);
	nx_push_self_msg(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("SelfPushMsg success!"));
end

--广播消息
--type:Tips类型 msgid:消息ID
function sysinfo_broadcast(gm, ...)
	if table.getn(arg) == 0 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: sysinfo_broadcast <type><msgid>"))
		return
	end

	local type = nx_int(arg[1]);
	local msgid = nx_string(arg[2]);

	if nx_sysinfo_broadcast(type,msgid) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_sysinfo_broadcast " .. nx_string(type) .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_sysinfo_broadcast " .. nx_string(type) .. "failed."))
	end
end

--被动技能升级(仅测试用)
function passive_skill_level_up(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: sysinfo_broadcast <skillid> <level>"));
		return
	end
	local skill = nx_int(arg[1]);
	local level = nx_int(arg[2]);
	nx_passive_skill_level_up(gm, skill,level);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("passive_skill_level_up"));
end

--被动技能升级(仅测试用)
function passive_skill_reset(gm,...)
	nx_passive_skill_reset(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("passive_skill_reset!"));
end

-- 设置一个成就条件
-- 参数：成就条件id，目标值
function achieve_cond(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("achieve_cond cond value"));
		return;
	end

	local int_cond = nx_int(arg[1]);
	local int_value = nx_int(arg[2]);
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("achieve_cond "  .. " begin."))
	nx_set_achieve_cond(target, int_cond, int_value)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("achieve_cond"  .. " ok."))

end

--进行竞技场挑战
function challenge(gm,...)
	nx_challenge(gm);
end

--获取竞技场候选挑战者
function get_candidate(gm,...)
	nx_get_candidate(gm);
end

--竞技场排行奖励
function arena_reward(gm,...)
	nx_arena_reward(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Reward  send."))
end

--GM阵营表格操作-清空
function ClearCampVisualNum(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ClearCampVisualNum "  .. " begin."))
	nx_clear_camp_visual_num(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("ClearCampVisualNum"  .. " ok."))
end

--GM阵营表格操作-设置
function SetCampVisualNum(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("SetCampVisualNum "  .. " begin."))
	if table.getn(arg) < 4 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "SetCampVisualNum", nx_widestr("Format: SetCampVisualNum fail"));
		return;
	end
	local nCampType = nx_int(arg[1]);
	local nPlayerNum = nx_int(arg[2]);
	local nNpcNum = nx_int(arg[3]);
	local nPrior = nx_int(arg[4]);
	nx_set_camp_visual_num(gm, nCampType, nPlayerNum, nNpcNum, nPrior);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("SetCampVisualNum"  .. " ok."))
end

--世界BOSS活动重置
function worldboss_reset(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("worldboss_reset "  .. " begin."))
	local param1 = nx_int(arg[1]);
	nx_worldboss_start(gm, param1);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("worldboss_reset"  .. " ok."))
end

--世界BOSS活动重置
function worldboss_end(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("worldboss_end "  .. " begin."))
	nx_worldboss_end(gm, ...);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("worldboss_end"  .. " ok."))
end


--重新加载商城配置
function shopReloadRes(gm,...)

	local ret = nx_shop_reload_resource(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("shop reload resource succeed") or nx_widestr("shop reload resource failed"));
end

-- 购买商店中的物品
function buyitem(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("buy item failed"));
		return;
	end
	local param1 = nx_string(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_buy_item(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("buy item succeed") or nx_widestr("buy item failed"));
end

--购买VIP礼包
function buyvipitem(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("buy vip item failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_buy_vip_item(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("buy vip item succeed") or nx_widestr("buy vip item failed"));
end

--购买周卡/月卡/年卡/终身卡
function buycard(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("buy card failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_buy_welfare_card(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("buy card succeed") or nx_widestr("buy card failed"));
end

-- 充值
function recharge(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("recharge failed"));
		return;
	end
	local param1 = nx_float(arg[1]);
	local param2 = "TEST_ORDER_0001";
	if table.getn(arg) >= 2 then
		param2 = nx_string(arg[2]);
	end	
	
	local ret = nx_recharge(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("recharge item succeed") or nx_widestr("recharge item failed"));
end

-- 充值购买物品
function pay_item(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pay_item failed"));
		return;
	end
	local param1 = nx_float(arg[1]);		-- 支付金额
	local param2 = "TEST_ORDER_0001";		-- 订单id
	local param3 = "TEST_PRODUCT_ID";		-- 产品id
	local param4 = "CapitalSilver:100";		-- 获得物品:(captial;items) 内部数据以','分割, eg: CopperCaptial:100;Item0001:12,Item002:1
	local param5 = "";						-- 首充奖励:(captial;items) 内部数据以','分割, eg: CopperCaptial:100;Item0001:12,Item002:1
	
	if table.getn(arg) >= 2 then
		param2 = nx_string(arg[2]);
	end	
	
	if table.getn(arg) >= 3 then
		param3 = nx_string(arg[3]);
	end	
	
	if table.getn(arg) >= 4 then
		param4 = nx_string(arg[4]);
	end	
	
	if table.getn(arg) >= 5 then
		param5 = nx_string(arg[5]);		
	end;
	local ret = nx_pay_item(gm,param1,param2, param3, param4, param5);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("pay item succeed") or nx_widestr("pay item failed"));
end

-- 重新加载VIP配置
function vipReloadRes(gm, ...)

	local ret = nx_vip_reload_resource(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("vip reload resource succeed") or nx_widestr("vip reload resource failed"));
end

--发红包
function giveoutredpacket(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "giveoutredpacket", nx_widestr("Format: giveoutredpacket fail"));
		return;
	end

	local type = nx_int(arg[1]);
	nx_getoutredpacket(gm, scene, type);
end

--抢红包
function getredpacket(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "getredpacket", nx_widestr("Format: getredpacket fail"));
		return;
	end

	local name = nx_widestr(arg[1]);
	local nation = nx_int(arg[2]);
	local type = nx_int(arg[3]);
	nx_getredpacket(gm, scene, name, nation, type);
end

--查红包
function lookupredpacket(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end

	nx_lookupredpacket(gm, scene);
end

--运营活动发红包
function sysgiveoutredpacket(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "sysgiveoutredpacket", nx_widestr("Format: sysgiveoutredpacket fail"));
		return;
	end

	local iNation = nx_int(arg[1]);
	local iType = nx_int(arg[2]);
	local iCost = nx_int(arg[3]);
	nx_sys_giveout_red_packet(gm, scene, iNation, iType, iCost);
end

--运营红包活动开启
function sysredpacketactivestart(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end
	if table.getn(arg) < 5 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "sysredpacketactivestart", nx_widestr("Format: sysredpacketactivestart fail"));
		return;
	end

	local id_version = nx_string(arg[1]);
	local total_times = nx_int(arg[2]);
	local per = nx_int(arg[3]);
	local type = nx_int(arg[4]);
	local nation = nx_int(arg[5]);
	nx_sys_red_packet_active_start(gm, scene, id_version, total_times, per, type, nation);
end

--运营红包活动结束
function sysredpacketactiveend(gm, ...)
	local scene = nx_get_scene();
	if not nx_exists(scene) then
		return;
	end
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "sysredpacketactiveend", nx_widestr("Format: sysredpacketactiveend fail"));
		return;
	end

	local id_version = nx_string(arg[1]);

	nx_sys_red_packet_active_end(gm, scene, id_version);
end

-- 获取运营活动奖励
function getdynamicactivityaward(gm, ...)

	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("getdynamicactivityaward failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_get_dynamic_activity_award(gm,param1,param2);
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("getdynamicactivityaward succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("getdynamicactivityaward failed"));
	end
end

-- 重新加载运营配置
function reloaddynamicactivity(gm, ...)
	local ret = nx_reload_resource_dynamic_activity(gm);
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("reloaddynamicactivity succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("reloaddynamicactivity failed"));
	end
end

-- 添加金币、木头
function addGoldWood(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("addGoldWood failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_add_gold_wood(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("addGoldWood succeed") or nx_widestr("addGoldWood failed"));
end

-- 帮会守护开启(难度等级)
function startGD(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("startGD failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_start_gd(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("startGD succeed") or nx_widestr("startGD failed"));
end

-- 帮会守护立即结束(0:失败 1:完成 2:完美完成)
function endGD(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("endGD failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_end_gd(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("endGD succeed") or nx_widestr("endGD failed"));
end

-- 帮会守护设置灵石HP
function setRockHP(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("setRockHP failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_set_rock_hp(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("setRockHP succeed") or nx_widestr("setRockHP failed"));
end

-- 帮会守护清空活动数据
function cleanGuildDefend(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("cleanGuildDefend failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_clean_guild_defend(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("cleanGuildDefend succeed") or nx_widestr("cleanGuildDefend failed"));
end

function setActiveType(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("type <[1|2|3]>"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_set_active_type(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("setactivetype succeed") or nx_widestr("setactivetype failed"));
end

-- 设置功能开关
function set_function(gm, ...)

	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set_function id state"));
		return;
	end

	local function_id = nx_int(arg[1]);
	local state = nx_int(arg[2]);
	local ret = nx_set_function(function_id, state);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("setfunction succeed") or nx_widestr("setfunction failed"));
end

-- 重置帮会守护活动数据
function resetGD(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("resetGD failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_reset_gd(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("resetGD succeed") or nx_widestr("resetGD failed"));
end

-- 跳过引导步骤
function skipstep(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("skipstep failed"));
		return;
	end

	local param1 = nx_string(arg[1]);
	local ret = nx_skip_guid_step(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("skipstep succeed") or nx_widestr("skipstep failed"));
end

--  跳过引导步骤（所有）
function skipstepall(gm,...)
	local ret = nx_skip_guid_step_all(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("skipstepall succeed") or nx_widestr("skipstepall failed"));
end

-- 检查所有商店的所有物品和价格
function checkshopitem(gm,...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("checkshopitem "  .. " begin."))
	nx_check_all_item(gm)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("checkshopitem"  .. " end."))
end

-- 修改循环最大次数
function set_max_circulate_count(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set_max_circulate_count failed"));
		return;
	end

	local maxcount = nx_int(arg[1]);
	local ret = nx_set_max_circulate_count(gm, maxcount);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("set_max_circulate_count succeed") or nx_widestr("set_max_circulate_count failed"));
end

function OpenScene(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("open secret scene failed"));
		return;
	end
	local sceneID = nx_int(arg[1]);
	open_one_secret_scene(gm, sceneID);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Open secret scene success"));
end

-- 学习所有的被动技能
function learn_all_passive_skill(gm, ...)
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("learn_all_passive_skill begin."));
	nx_learn_all_passive_skill(gm)

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("learn_all_passive_skill succeed."));
end

-- 通过寻路模块走到目标点
function motion(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("motion failed"));
		return;
	end

	local x = nx_float(arg[1]);
	local z = nx_float(arg[2]);
	if (nx_motion_point(gm, x, z)) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("motion succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("motion failed"));
	end
end

-- 清空自身的CD表
function clear_cd_rec(gm, ...)
	if (nx_clear_cd_rec(gm)) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("clear_cd_rec succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("clear_cd_rec failed"));
	end
end

--//设置每日增加资金上限
function SetCapitalMonitorMaxVal(gm, ...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("SetCapitalMonitorMaxVal failed"));
		return;
	end

	if(nx_set_capital_monitor(nx_int(arg[1]), nx_int64(arg[2]), nx_int64(arg[3]))) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("SetCapitalMonitorMaxVal succeed"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("SetCapitalMonitorMaxVal failed"));
	end
end

function forge_equip(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("forging_equip <pos>"));
		return;
	end

	local drawing = nx_string(arg[1])

	nx_forge_equip(gm, drawing);

end

-- 进入帮会驻地
function EntryGS(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("EntryGS failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_entry_gs(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("EntryGS succeed") or nx_widestr("EntryGS failed"));
end

-- 重置任务次数reset_task_num 次数
function reset_task_num(gm, ...)
	local count = nx_int(arg[1]);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("reset_task_num begin."));
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end
	nx_reset_task_num(target, count)

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("reset_task_num succeed."));
end

--GM补单
function apple_charge_resend(gm,...)
	if table.getn(arg) < 6 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("param is missing"))
		return
	end

	local playername = nx_string(arg[1])
	local account = nx_string(arg[2])
	local serverid = nx_string(arg[3])
	local productid = nx_string(arg[4])
	local orderid = nx_string(arg[5])
	local serial = nx_string(arg[6])

	local ret = nx_apple_charge_resend(gm, playername, account, serverid, productid, orderid, serial)
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("apple_charge_resend success"))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("apple_charge_resend failed"))
	end
end

--重置当前场景指定分线的人数
--格式：reset_branch_num 第几个分线 重置后数量
function reset_branch_num(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("param is missing"))
		return
	end

	local branch_index = nx_int(arg[1])
	local reset_num = nx_int(arg[2])


	local ret = nx_reset_branch_num(gm, branch_index, reset_num)
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_reset_branch_num success"))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_reset_branch_num failed"))
	end
end

--开始答题
--格式：start_answer 类型（1：任务答题 2：帮会答题）
function start_answer(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("param is missing"))
		return
	end

	local function_type = nx_int(arg[1])


	local ret = nx_start_answer(gm, function_type)
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_start_answer success"))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_start_answer failed"))
	end
end

-- 依次测试答题
-- check_answer 题目ID（可选项，不填默认从第一题开始，否则以当前输入题目开始）
function check_answer(gm,...)

	local question = nx_int(arg[1])

	local ret = nx_check_answer(gm, question)
	if ret then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_check_answer success"))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_check_answer failed"))
	end
end

-- 重置玩家竞技场状态
function reset_arena_state(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("reset_arena_state failed"));
		return;
	end
	local param1 = nx_widestr(arg[1]);
	local ret = nx_reset_arena_state(param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("reset_arena_state succeed") or nx_widestr("reset_arena_state failed"));
end

-- 锁定玩家竞技场状态
function lock_arena_state(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("lock_arena_state failed"));
		return;
	end
	local param1 = nx_widestr(arg[1]);
	local ret = nx_lock_arena_state(param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("lock_arena_state succeed") or nx_widestr("lock_arena_state failed"));
end

-- 重置玩家位置到本国皇城(/ssplayer 场景ID 玩家昵称)
function ssplayer(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("ssplayer failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_widestr(arg[2]);
	local ret = nx_reset_player_position(param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("ssplayer succeed") or nx_widestr("ssplayer failed"));
end

--pvp个人报名
function apply_alone_pvp(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("apply_alone_pvp failed"));
		return;
	end
	local param1 = nx_int(arg[1]);
	nx_pvp_alone_apply(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "pvp_alone_apply succeed");
end

-- 改名
function changename(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("change name failed"));
		return;
	end
	local param1 = nx_widestr(arg[1]);
	local ret = nx_change_name(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("change name succeed") or nx_widestr("change name failed"));
end

--改签名
function setSign(gm, ...)
	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	local param = nx_widestr(arg[1]);
	nx_player_sign(target,param);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("setSign succeed!"));

end

--重置试炼塔
function reset_tower(gm, ...)
	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return;
	end
	if nx_reset_world_tower(target) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_world_tower " .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_world_tower " .. "failed."))
	end
end

--挑战试炼塔：challenge_tower 塔层
function challenge_tower(gm, ...)
	local d_floor = nx_int(arg[1]);
	if nx_challenge_tower(gm, d_floor) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_challenge_tower " .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_challenge_tower " .. "failed."))
	end
end

--扫荡试炼塔
function sweep_tower(gm, ...)
	if nx_sweep_tower(gm) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_sweep_tower " .. "succeed."))
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_sweep_tower " .. "failed."))
	end
end

--重置洗炼次数
function refresh_baptise_nums(gm, ...)

	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	refresh_baptise_nums(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("refresh_baptise_nums "  .. " succeed"));
end

--添加等级排行榜数据(/addlrdata 需插入数量,不填默认30条)
function addlrdata(gm, ...)
    local count = 30;
	if table.getn(arg) >= 1 then
		count = nx_int(arg[1]);
	end

	local param1 = "domain";
	local param2 = nx_widestr("Domain_RankingList_7440010");
	local param3 = "player_level_rank_list";

	-- 表是否存在
    if not nx_find_pubdata_record(param1, param2, param3) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

	-- 定义等级排行榜数据列  [player_uid] [player_account] [player_name]
	local default_val = {"robot_uid","robot_account","robot_name"};

	-- 表列数
	local cols = nx_get_pubdata_record_cols(param1, param2, param3);
	-- nx_msgbox(nx_string(cols))
	if cols < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pub record " .. recname .. " columns is " .. cols));
        return;
    end

	local var = {};
	for i = 1, count do
	    var[1] = nx_string(default_val[1] .. "_" .. i);   -- player_uid
		var[2] = nx_string(default_val[2] .. "_"  .. i);  -- player_account
		var[3] = nx_widestr(default_val[3] .. "_" .. i);  -- player_name
		var[4] = math.random(1, 2);                       -- player_career
		var[5] = math.random(1, 2);                       -- player_sex
		var[6] = math.random(1, 2);                       -- player_nation
		var[7] = math.random(30, 80);                     -- player_level
		var[8] = 1;                                       -- player_online_type
		var[9] = 1000;                                    -- player_online
		-- nx_msgbox(var[1]);

		pubrecaddrow(gm, param1, param2, param3, unpack(var));
	end
end

-- 添加时装
function add_fashion(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add_fashion config_id"));
		return;
	end

	local config_id = nx_string(arg[1]);

	local ret = nx_add_fashion(gm, config_id);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("done"));

end

-- 穿上时装
function puton_fashion(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add_fashion config_id"));
		return;
	end

	local config_id = nx_string(arg[1]);

	local ret = nx_puton_fashion(gm, config_id);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("done"));

end

-- 脱下时装
function takeoff_fashion(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add_fashion config_id"));
		return;
	end

	local config_id = nx_string(arg[1]);

	local ret = nx_takeoff_fashion(gm, config_id);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("done"));

end

-- 锁定时装
function lock_fashion(gm, ...)

	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("add_fashion config_id"));
		return;
	end

	local config_id = nx_string(arg[1]);

	local ret = nx_lock_fashion(gm, config_id);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("done"));

end

-- 改变帮会战活动状态
function change_gvg_state(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">state"));
		return;
	end

	local state = nx_int(arg[1]);
	local ret = nx_change_gvg_state(state);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("change_gvg_state done"));
end

-- 改变帮会战活动状态
function gvg_sign_up(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">groupid"));
		return;
	end

	local groupid = nx_int(arg[1]);
	local ret = nx_gvg_sign_up(gm, groupid);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("gvg_sign_up done"));
end

-- 帮会战调配
function gvg_adjust(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">groupid"));
		return;
	end

	local groupid = nx_int(arg[1]);
	local ret = nx_gvg_adjust(gm, groupid);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("gvg_adjust done"));
end

-- 改变帮会战活动状态
function gvg_request_info(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">msgid"));
		return;
	end

	local msgid = nx_int(arg[1]);
	local ret = nx_gvg_request_info(gm, msgid);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("gvg_request_info done"));
end

-- 锁定/解锁调配权限
function gvg_lock_adjust(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">islock"));
		return;
	end

	local islock = nx_int(arg[1]);
	local ret = nx_gvg_lock_adjust(gm, islock);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("gvg_lock_adjust done"));
end

-- 锁定/解锁调配权限
function gvg_reply_call(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(">nResult"));
		return;
	end

	local nResult = nx_int(arg[1]);
	local ret = nx_gvg_lock_adjust(gm, nResult);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("gvg_reply_call done"));
end

-- 获取全服排行榜排名
function query_global_rank(gm,...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("query_global_rank failed"));
		return;
	end

	local nType = nx_int(arg[1]);
	local rankListName = nx_string(arg[2]);
	local key = nx_widestr(arg[3]);

	local ret = nx_query_global_rank(gm, nType, rankListName, key);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("query_global_rank wait...") or nx_widestr("query_global_rank failed"));
end

-- 获取全服排行榜范围
function query_global_range(gm,...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("query_global_range failed"));
		return;
	end

	local rankListName = nx_string(arg[1]);
	local rankstart = nx_int(arg[2]);
	local rankend = nx_int(arg[3]);

	local ret = nx_query_global_range(gm, rankListName, rankstart, rankend);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("query_global_range wait...") or nx_widestr("query_global_range failed"));
end

-- 获取全服排行榜积分
function query_global_score(gm,...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("query_global_score failed"));
		return;
	end

	local nType = nx_int(arg[1]);
	local rankListName = nx_string(arg[2]);
	local key = nx_widestr(arg[3]);

	local ret = nx_query_global_score(gm, nType, rankListName, key);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("query_global_score wait...") or nx_widestr("query_global_score failed"));
end

-- 获取全服排行榜附加数据
function query_global_extra(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("query_global_extra failed"));
		return;
	end

	local nType = nx_int(arg[1]);
	local rankListName = nx_string(arg[2]);
	local key = nx_widestr(arg[3]);

	local ret = nx_query_global_extra(gm, nType, rankListName, key);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("query_global_extra wait...") or nx_widestr("query_global_extra failed"));
end

-- 获取全服排行榜全局数据
function query_global_data(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("query_global_data failed"));
		return;
	end

	local nType = nx_int(arg[1]);
	local key = nx_widestr(arg[2]);

	local ret = nx_query_global_data(gm, nType, key);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("query_global_data wait...") or nx_widestr("query_global_data failed"));
end

-- 删除排行榜
function remove_global_rank_list(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove_global_rank_list failed"));
		return;
	end

	local rankListName = nx_string(arg[1]);

	local ret = nx_remove_global_rank_list(gm, rankListName);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("remove_global_rank_list wait...") or nx_widestr("remove_global_rank_list failed"));
end

-- 删除排行榜某一条记录
function remove_global_rank_row(gm,...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove_global_rank_row failed"));
		return;
	end

	local ntype = nx_int(arg[1]);
	local rankListName = nx_string(arg[2]);
	local key = nx_widestr(arg[3]);

	local ret = nx_remove_global_rank_row(gm, ntype, rankListName, key);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("remove_global_rank_row wait...") or nx_widestr("remove_global_rank_row failed"));
end

-- 设置全局排行消息属性
function set_global_prop(gm,...)
	if table.getn(arg) < 4 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set_global_prop failed"));
		return;
	end

	local nType = nx_int(arg[1]);
	local key = nx_widestr(arg[2]);
	local propName = nx_string(arg[3]);
	local propValue = nx_widestr(arg[4]);

	local ret = nx_set_global_prop(gm, nType, key, propName, propValue);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("set_global_prop succeed") or nx_widestr("set_global_prop failed"));
end

-- 设置全局排行消息属性
function init_global_rank_by_all_player(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("init_global_rank_by_all_player failed"));
		return;
	end

	local rankListName = nx_string(arg[1]);

	local ret = nx_init_global_rank_by_all_player(gm, rankListName);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("init_global_rank_by_all_player succeed") or nx_widestr("init_global_rank_by_all_player failed"));
end

-- 清除全服排行公共状态
function clear_pub_global_award_state(gm,...)

	local ret = nx_clear_pub_global_award_state(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("clear_pub_global_award_state succeed") or nx_widestr("clear_pub_global_award_state failed"));
end

-- 清除全服排行个人状态
function clear_self_global_award_state(gm,...)

	local ret = nx_clear_self_global_award_state(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("clear_self_global_award_state succeed") or nx_widestr("clear_self_global_award_state failed"));
end

-- 初始化集体类型的全局属性
function init_command_global_data(gm,...)

	local ret = nx_init_command_global_data(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("init_command_global_data succeed") or nx_widestr("init_command_global_data failed"));
end

--结束GVG战场
-- EndGVG 参数1（1胜利 2平局 3失败）
function EndGVG(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	return;
	end

	local result = nx_int(arg[1]);
	local ret = nx_end_gvg(gm,result);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("EndGVG succeed") or nx_widestr("EndGVG failed"));
end

--设置荣耀等级
-- set_glory_lvl 参数1（目标等级）
function set_glory_lvl(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	return;
	end

	local dest_lvl = nx_int(arg[1]);
	local ret = nx_set_glory_lvl(gm,dest_lvl);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("set_glory_lvl succeed") or nx_widestr("set_glory_lvl failed"));
end

--添加荣耀技能（测试用）
-- add_glory_skill 参数1（目标等级）
function add_glory_skill(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	return;
	end

	local skill = nx_string(arg[1]);
	local ret = nx_add_glory_skill(gm,skill);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_glory_skill succeed") or nx_widestr("add_glory_skill failed"));
end

--帮战个人结算
function gvg_player_settle(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("gvg_player_settle failed"));
	return;
	end

	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_gvg_player_settle(gm, param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("gvg_player_settle succeed") or nx_widestr("gvg_player_settle failed"));
end

--清空玩家参赛次数
function gvg_player_clear_playdata(gm, ...)
	local ret = nx_gvg_player_set_playdata(gm, 0, 0);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("gvg_player_clear_playdata succeed") or nx_widestr("gvg_player_clear_playdata failed"));
end

--设置玩家参赛次数
function gvg_player_set_playdata(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("gvg_player_clear_playdata failed"));
	return;
	end

	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_gvg_player_set_playdata(gm, param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("gvg_player_clear_playdata succeed") or nx_widestr("gvg_player_clear_playdata failed"));
end

--设置敌对阵营击杀和死亡数（测试用）
-- set_kill_dead_num 参数1（类型：1击杀 2死亡）参数2（数量）
function set_kill_dead_num(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	return;
	end

	local rule_type = nx_int(arg[1]);
	local num = nx_int(arg[2]);
	local ret = nx_set_kill_dead_num(gm,rule_type,num);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("set_kill_dead_num succeed") or nx_widestr("set_kill_dead_num failed"));
end


-- 添加印记
-- add_marker args：印记id
function add_marker(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	
	local cfgid = nx_string( arg[1] );
	local ret = nx_marker_add( gm, cfgid );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_marker succeed") or nx_widestr("add_marker failed"));	
end

-- 移除印记
-- remove_marker args：印记id
function remove_marker(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	
	local cfgid = nx_string( arg[1] );
	local ret = nx_marker_remove( gm, cfgid );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("remove_marker succeed") or nx_widestr("remove_marker failed"));	
end

--重置印记
--reset_marker:印记id
function reset_marker(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local cfgid = nx_string( arg[1] );
	local ret = nx_marker_remove( gm, cfgid );
	if ret then
		ret = nx_marker_add( gm, cfgid );
	end
	 
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr(" reset succeed") or nx_widestr("reset failed"));	
end

-- 物品合成
-- compose_item args：消耗的物品id
function compose_item(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	
	local itemid = nx_string( arg[1] );

	local ret = nx_compose_item( gm, itemid );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("compose_item succeed") or nx_widestr("compose_item failed"));	
end

-- 挑战生死擂
function revenge_challenge(gm, ...)
	local ret = nx_revenge_challenge( gm );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("revenge_challenge succeed") or nx_widestr("revenge_challenge failed"));	
end

-- 回复生死擂挑战
function answer_re_challenge(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	
	local answer = nx_int( arg[1] );
	local ret = nx_answer_re_challenge( gm, answer);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("answer_re_challenge succeed") or nx_widestr("answer_re_challenge failed"));	
end

-- 初始化掠夺战
-- init_battle 守方阵营 守方组织名 攻方组织名
function init_battle(gm, ...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local nation = nx_int(arg[1]);
	local def_guild = nx_widestr(arg[2]);
	local att_guild = nx_widestr(arg[3]);
	local ret = nx_init_battle( gm, nation, def_guild, att_guild);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("init_battle succeed") or nx_widestr("init_battle failed"));	
end

-- 初始化势力战
-- init_power_battle 守方阵营 守方组织名 攻方组织名
function init_power_battle(gm, ...)
	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local nation = nx_int(arg[1]);
	local def_guild = nx_widestr(arg[2]);
	local att_guild = nx_widestr(arg[3]);
	local ret = nx_init_power_battle( gm, nation, def_guild, att_guild);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("init_power_battle succeed") or nx_widestr("init_power_battle failed"));	
end

-- 发布悬赏任务
-- issue_or_task 赏金金额
function issue_or_task(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local reward = nx_int(arg[1]);
	local ret = nx_issue_or_task( gm, reward );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("issue_or_task succeed") or nx_widestr("issue_or_task failed"));	
end

-- 接取悬赏任务
-- accept_or_task 索引行号
function accept_or_task(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local row = nx_int(arg[1]);
	local ret = nx_accept_or_task( gm, row );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("accept_or_task succeed") or nx_widestr("accept_or_task failed"));	
end

-- 开始押送
-- start_escort 囚车id
function start_escort(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end
	local escort_id = nx_int(arg[1]);
	local ret = nx_start_escort( gm, escort_id);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("start_escort succeed") or nx_widestr("start_escort failed"));	
end

-- 结束押送
-- end_escort 结果（2：超时失败 3：被劫 4：释放 5：成功）
function end_escort(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("parameter is missing!"));
	end

	local result = nx_int(arg[1]);
	local ret = nx_end_escort( gm, result);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("end_escort succeed") or nx_widestr("end_escort failed"));	
end

-- 追踪目标
-- track_target
function track_target(gm, ...)
	local ret = nx_track_target( gm );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("track_target succeed") or nx_widestr("track_target failed"));	
end

-- 输出地形数据
-- floor_info
function floor_info(gm, ...)
	local ret = nx_floor_info( gm );
	--nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("floor_info succeed") or nx_widestr("floor_info failed"));	
end

-- 增加边境积分
function inc_frontier_score(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:inc_frontier_score [score]"));
		return;
	end

	local score = nx_int(arg[1]);
	nx_frontier_inc_score(gm, score);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("inc_frontier_score "  .. " ok."));

end

-- 边境上载具
function frontier_get_up_vehicle(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:nx_frontier_get_up_vehicle"));
		return;
	end

	local type = nx_int(arg[1]);
	nx_frontier_get_up_vehicle(gm, type);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("nx_frontier_get_up_vehicle "  .. " ok."));

end

--结束边境排行榜 结算
function calc_frontier_result(gm, ...)
	nx_frontier_end(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("calc_frontier_result "  .. " ok."));
end

--开启宝库
function frontier_treasury_open( gm, ... )
	local mode = 0;
	if table.getn(arg) >= 1 then
		mode = nx_int(arg[1]);
	end

	nx_frontier_treasury_open(gm, mode);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("frontier_treasury_open "  .. " ok."));
end

function frontier_treasury_close( gm, ... )
	local mode = 0;
	if table.getn(arg) >= 1 then
		mode = nx_int(arg[1]);
	end

	nx_frontier_treasury_close(gm, mode);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("frontier_treasury_close "  .. " ok."));
end

--查询场景左上和右下的坐标
function query_map_bound(gm, ...)
	nx_query_map_bound(gm);
end
	
-- 重置爵位俸禄
function reset_daily_wage(gm, ...)
	local ret = nx_reset_daily_wage( gm );
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("reset_daily_wage succeed") or nx_widestr("reset_daily_wage failed"));	
end

--领取悬赏日常任务奖励
function take_offer_reward(gm, ...)
	nx_take_offer_reward(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("take_offer_reward "  .. " ok."));
end
-- 重置签到
function reset_signin(gm, ...)
	nx_reset_sign_in(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("reset_sign_In"  .. " ok."));
end

-- 兑换cdkey礼包
function exchange_gift(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:exchange_gift [key]"));
		return;
	end

	local key = nx_string(arg[1]);
	nx_exchange_gift(gm, key);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("exchange_gift "  .. " ok."));
end

-- 开启所有功能
function open_all_function(gm,...)
         local ret = nx_activate_all_function(gm);
         nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("open_all_functoin succeed") or nx_widestr("open_all_functoin failed"));
end
-- 关闭所有功能
function close_all_function(gm,...)
         local ret = nx_close_all_function(gm);
         nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("nx_close_all_function succeed") or nx_widestr("nx_close_all_function failed"));
end

--移除称号GM命令
-- remove_title 称号ID
function remove_title(gm, ...)
    if table.getn(arg) < 1 then
   	 nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove_title failed"));
    	 return;
    end

    local param1 = nx_int(arg[1]);

    local ret = nx_remove_title(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("remove_title succeed") or nx_widestr("remove_title failed"));
end

--设置称号
-- set_title 称号ID 
function set_title(gm, ...)
	if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set_title failed"));
		return;
	end

    local param1 = nx_int(arg[1]);
    local ret = nx_set_title(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("set_title succeed") or nx_widestr("set_title failed"));
end
--条件称号
-- set_title 条件id 条件值
function cond_title(gm, ...)
	if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("cond_title failed"));
		return;
	end

    local param1 = nx_int(arg[1]);
    local param2 = nx_int(arg[2]);
    local ret = nx_cond_title(gm, param1,param2);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("cond_title succeed") or nx_widestr("cond_title failed"));
end

--增加囚犯
function add_prisoner(gm, ...)
    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local ret = nx_add_prisoner(target);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_prisoner success") or nx_widestr("add_prisoner failed"));
end

--为囚犯求情
function plead_prisoner(gm, ...)
    if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("plead_prisoner failed"));
		return;
	end

    local param1 = nx_widestr(arg[1]);
    local ret = nx_plead_prisoner(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("plead_prisoner succeed") or nx_widestr("plead_prisoner failed"));
end

--侮辱囚犯
function insult_prisoner(gm, ...)
    if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("insult_prisoner failed"));
		return;
	end

    local param1 = nx_widestr(arg[1]);
    local ret = nx_insult_prisoner(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("insult_prisoner succeed") or nx_widestr("insult_prisoner failed"));
end

--阵营求助
function call_help(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("call_help failed"));
		return;
	end
    local param1 = nx_int(arg[1]);
    local ret = nx_call_help(gm, param1);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("call_help succeed") or nx_widestr("call_help failed"));
end

--成为正式徒弟
function become_official(gm, ...)
    local ret = nx_become_official(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("nx_become_official succeed") or nx_widestr("nx_become_official failed"));
end

-- 重置师徒冷却时间
function reset_master_cool(gm,...)
    local ret = nx_reset_master_cool_time(gm,...)
     nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("reset_master_cool succeed") or nx_widestr("reset_master_cool failed"));
end

--为所有下属加情义值
-- add_friendship 情义值
function add_friendship(gm, ...)
	if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("add_friendship failed"));
		return;
	end

    local param1 = nx_int(arg[1]);
    local ret = nx_add_friendship(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_friendship succeed") or nx_widestr("add_friendship failed"));
end

--重置下属次数相关
-- reset_pet
function reset_pet(gm, ...)
    local ret = nx_reset_pet(gm);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("reset_pet succeed") or nx_widestr("reset_pet failed"));
end

--增加边境囚犯
function add_fro_prisoner(gm, ...)
    local ret = nx_frontier_add_kill_prisoner(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_fro_persion succeed") or nx_widestr("add_fro_persion failed"));
end

--选择阵营
function select_nation(gm, ...)
    if table.getn(arg) < 1 then
 		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("select_nation failed"));
		return;
	end

    local param1 = nx_int(arg[1]);
    local ret = nx_select_nation(gm, param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("select_nation succeed") or nx_widestr("select_nation failed"));
end

--边境箭塔
function frotower(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("fro_tower failed"));
        return;
    end

    local param1 = nx_int(arg[1]);
    local ret = nx_fro_tower(gm, param1);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("fro_tower succeed") or nx_widestr("fro_tower failed"));
end

--装备砭石
function wear_bianstone(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("wear_bianstone failed"));
	return;
	end
	local param1 = nx_string(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_wear_bianstone(gm, param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("wear_bianstone succeed") or nx_widestr("wear_bianstone failed"));
end

--卸下砭石
function take_off_bianstone(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("take_off_bianstone failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_take_off_bianstone(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("take_off_bianstone succeed") or nx_widestr("take_off_bianstone failed"));
end

--升级砭石
function level_up_bianstone(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("level_up_bianstone failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_level_up_bianstone(gm, param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("level_up_bianstone succeed") or nx_widestr("level_up_bianstone failed"));
end

--发红包
function send_red_packet(gm, ...)
	if table.getn(arg) < 3 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("send_red_packet failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local param3 = nx_int(arg[3]);
	local ret = nx_send_red_packet(gm, param1, param2, param3);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("send_red_packet succeed") or nx_widestr("send_red_packet failed"));
end

--抢红包
function take_red_packet(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("take_red_packet failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_take_red_packet(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("take_red_packet succeed") or nx_widestr("take_red_packet failed"));
end

--充值系统发红包
function send_charge_red_packet(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("send_charge_red_packet failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_send_charge_red_packet(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("send_charge_red_packet succeed") or nx_widestr("send_charge_red_packet failed"));
end

-- 增加组织数值
function guild_add_value(gm, ...)
	if table.getn(arg ) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("add_guild_value failed"));
	return;
	end

	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_guild_add_value(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("add_guild_value succeed") or nx_widestr("add_guild_value failed"));
end

function guild_dec_value(gm, ...)
	if table.getn(arg ) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("guild_dec_value failed"));
	return;
	end

	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_guild_dec_value(gm,param1,param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("guild_dec_value succeed") or nx_widestr("guild_dec_value failed"));
end

-- 增加组织建筑经验
function guild_add_building_exp(gm, ...)
	if table.getn(arg ) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("guild_add_building_exp failed"));
	return;
	end

	local param1 = nx_int(arg[1]);
	
	local ret = nx_guild_add_builing_exp(gm,param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("guild_add_building_exp succeed") or nx_widestr("guild_add_building_exp failed"));
end

--增加技能经验
function guild_add_skill_exp(gm, ...)
	if table.getn(arg)<1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("guild_add_skill_exp failed"));
		return
	end
	local param1=nx_int(arg[1]);
	local ret=nx_guild_add_skill_exp(gm,param1);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("guild_add_skill_exp succeed") or nx_widestr("guild_add_skill_exp failed"));
end

--增加组织秘宝
function guild_add_rare(gm,...)
	if table.getn(arg)<2 then
	    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("guild_add_rare failed"));
		return
	end
	local param1=nx_int(arg[1]);
	local param2=nx_int(arg[2]);
	local ret=nx_guild_add_rare_treasure(gm,param1,param2);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("guild_add_rare succeed") or nx_widestr("guild_add_rare failed"));
end

--触发组织计时器
function tigger_guild_timer(gm, ...)
	local ret = nx_tigger_guild_timer(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("nx_tigger_guild_timer succeed") or nx_widestr("nx_tigger_guild_timer failed"));
end

--改变活动状态
function change_campaign_state(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("change_campaign_state failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_int(arg[2]);
	local ret = nx_change_campaign_state(param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("change_campaign_state succeed") or nx_widestr("change_campaign_state failed"));
end

-- 改变活动开启，关闭 param: activity_id,true/false
function activity_logic_open(gm, ...)
	if table.getn(arg) < 2 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("activity_logic_open failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local param2 = nx_string(arg[2]);
	local ret = nx_activity_logic_open(param1, param2);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("activity_logic_open succeed") or nx_widestr("activity_logic_open failed"));
end

-- 一键自动分配组织城 参数无
function auto_assign_guild_town(gm, ...)
	local ret = nx_auto_assign_guild_town();

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("auto_assign_guild_town succeed") or nx_widestr("auto_assign_guild_town failed"));
end

--创建boss入侵关卡npc
function create_bi_stage_npc(gm, ...)
	if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("create_bi_stage_npc failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_create_bi_stage_npc(param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("create_bi_stage_npc succeed") or nx_widestr("create_bi_stage_npc failed"));
end

--印记洗练
function marker_baptise(gm, ...)
    if table.getn(arg) < 1 then
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("marker_baptise failed"));
    return;
    end
    local param1 = nx_int(arg[1]);
    local ret = nx_marker_baptise(gm,param1);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("marker_baptise succeed") or nx_widestr("marker_baptise failed"));
end

--开启 限时福利场景
function time_limit_scene(gm, ...)
    if table.getn(arg) < 2 then
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("time_limit_scene failed"));
    return;
    end
    local param1 = nx_int(arg[1]);
    local param2 = nx_int(arg[2]);
    local ret = nx_open_time_limit_scene(gm,param1,param2);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("time_limit_scene succeed") or nx_widestr("time_limit_scene failed"));
end


function friend_add_enemy(gm, ...)
    local ret = nx_add_friend_enemy(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("time_limit_scene succeed") or nx_widestr("time_limit_scene failed"));
end

--发起组队悬赏
function launch_teamoffer(gm, ...)
    local ret = nx_launch_teamoffer(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("nx_launch_teamoffer succeed") or nx_widestr("nx_launch_teamoffer failed"));
end

--进入组队悬赏副本
function entry_teamoffer(gm, ...)
    local ret = nx_entry_group(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("nx_entry_group succeed") or nx_widestr("nx_entry_group failed"));
end

--训练boss
function train_invade_boss(gm, ...)
    if table.getn(arg) < 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("train_invade_boss failed"));
	return;
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_train_boss(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("train_invade_boss succeed") or nx_widestr("train_invade_boss failed"));
end

--填充玩家数据
function fill_player_data(gm, ...)
	local param1 = "Item2010001";
    if table.getn(arg) >= 1 then
		param1 = nx_string(arg[1]);
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("fill_player_data default item is Item2010001"));
	end
	local ret = nx_fill_player_full_data(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("fill_player_data succeed") or nx_widestr("fill_player_data failed"));
end

--测试修炼秘境消息
function test_practice_scene_msg(gm, ...)
    if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("test_practice_scene_msg failed"));
	end
	local param1 = nx_int(arg[1]);
	local ret = nx_test_practice_scene_msg(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("test_practice_scene_msg succeed") or nx_widestr("test_practice_scene_msg failed"));
end

--创建队伍
function create_team(gm,...)
    local ret = GMCreateTeam(gm,...);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("create_team succeed") or nx_widestr("create_team failed"));
    
end

--七日
function seven_obj(gm,...)
		if table.getn(arg) <1 then
			return
		end
		param1 = nx_int(arg[1]);
       local ret = nx_seven_da_obj_open_all(gm,param1);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("create_team succeed") or nx_widestr("create_team failed"));
    
end

--限时礼包信息
function limit_gift_info( gm, ... )
    local ret = nx_limit_gift_info(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "limit gift info:" .. ret);
end

--开启礼包
function limit_gift_open( gm, ... )
    local ret = nx_limit_gift_open(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and "open limit gift succeed" or "open limit gift failed.");
end

--关闭礼包
function limit_gift_close( gm, ... )
    local ret = nx_limit_gift_close(gm);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and "close limit gift succeed" or "close limit gift failed.");
end

--开始坐牢
function begin_jail(gm, ...)
    if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("begin_jail failed"));
	end

	local ret = nx_begin_jail(gm);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("begin_jail succeed") or nx_widestr("begin_jail failed"));
end

--发放阵营红包
function send_nation_red_packet(gm, ...)
    if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("send_nation_red_packet failed"));
	end

	local param1 = nx_int(arg[1]);
	local ret = nx_send_nation_red_packet(gm, param1);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("send_nation_red_packet succeed") or nx_widestr("send_nation_red_packet failed"));
end

-- 开启活动
function open_activity(gm,...)
	if table.getn(arg) <1 then
		return
	end
	local actid = nx_int(arg[1]);
    local ret = nx_open_activity(actid);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("open activity succeed") or nx_widestr("open activity failed"));
end

-- 关闭活动
function close_activity(gm,...)
	if table.getn(arg) <1 then
		return
	end
	local actid = nx_int(arg[1]);
    local ret = nx_close_activity(actid);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("close activity succeed") or nx_widestr("close activity failed"));
end

--开始坐牢
function jade_client_msg(gm, ...)
    if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("jade_client_msg failed"));
	end

	local submsg = nx_int(arg[1]);
	local jade = nx_string(arg[2]);
	
	local ret = nx_jade_client_msg(gm, submsg, jade);

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("jade_client_msg succeed") or nx_widestr("jade_client_msg failed"));
end

--商店刷新
function refresh_shop(gm, ...)
	local ret = nx_refresh_shop(gm);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", ret and nx_widestr("refresh_shop succeed") or nx_widestr("refresh_shop failed"));
end