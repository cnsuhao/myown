
require("define")
require("gm_reload")
require("gm_test")
require("gm_helper")
local COMMAND_PLAYER_IS_FIGHT = 6;
local TIPSTYPE_GMINFO = 0;

--获得当前属性操作对象
function get_prop_target(gm)
    if not nx_find_data(gm, "GMPropTarget") then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not exist property operate target!"));
        return nx_null();
    end

    local target = nx_query_data(gm, "GMPropTarget");
    if not nx_exists(target) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not exist property operate target!"));
        return nx_null();
    end

    return target
end

--属性操作  设置当前属性操作的对象 setobj [对象] [可选项]
--                  setobj          设置自身为属性设置操作的对象
--                  setobj gm       设置自身为属性设置操作的对象
--                  setobj select       设置当前选中的对象为属性操作对象
--                  setobj scene        设置当前场景为属性操作的对象
--                  setobj child XXXXX  设置当前属性操作对象的指定名称的子对象为当前属性操作的对象
--                  setobj index XXXXX  设置当前属性操作对象指定位置的子对象为当前属性操作的对象
--                  setobj id XXXXX     设置当前属性操作对象为指定PERSISTID的对象
--                  setobj config XXXXX 设置当前属性操作对象为指定CONFIGID的对象
--                  setobj script XXXXX 设置当前属性操作对象为指定Script的对象
--                  setobj view XXX XXXX设置当前属性操作对象为指定视窗ID的指定位置的对象

function setobj(gm, ...)
    if not nx_find_data(gm, "GMPropTarget") then
        nx_add_data(gm, "GMPropTarget", VTYPE_OBJECT);
    end
    if table.getn(arg) == 0 then
        nx_set_data(gm, "GMPropTarget", gm);
        --设置属性操作对象成功！
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Set object property succeed!"));
        return;
    end

    local para0 = nx_string(arg[1]);
    if para0 == "gm" then
        nx_set_data(gm, "GMPropTarget", gm);
    elseif para0 == "select" then
        nx_set_data(gm, "GMPropTarget", nx_query_property(gm, "LastObject"));
    elseif para0 == "scene" then
        nx_set_data(gm, "GMPropTarget", nx_get_scene());
    elseif para0 == "id" then
        if table.getn(arg) < 2 then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setobj id <objectid>"));--格式：setobj id 序列号(xx.xx)
            return;
        end
        local child = nx_object(arg[2]);
        if not nx_exists(child) then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found object"));--未找到对象[{@0:名称}]
            return;
        end
        nx_set_data(gm, "GMPropTarget", child);
    elseif para0 == "view" then
        if table.getn(arg) < 3 then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setobj view <view_id> <obj_pos>"));--格式：setobj view 视窗id 对象位置
            return;
        end

        local target = get_prop_target(gm)
        if not nx_exists(target) or nx_type(target) ~= 2 then --TYPE_PLAYER = 2
            --nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("cur obj not player object"));--当前选取对象不是玩家对象
            return
        end

        local container = nx_get_viewport_container(target,nx_int(arg[2]));
        if not nx_exists(container) then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found object"));--未找到对象[{@0:名称}]
            return;
        end

        local item = nx_get_item(container, nx_int(arg[3]))
        if not nx_exists(item) then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found object"));--未找到对象[{@0:名称}]
            return;
        end

        nx_set_data(gm, "GMPropTarget", item);
    else
        local target = get_prop_target(gm)
        if not nx_exists(target) then
            return
        end

        if para0 == "child" then
            if table.getn(arg) < 2 then
                --格式：setobj child 子对象名称
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("format：setobj child name"));
                return;
            end
            local child = nx_get_child(target, nx_widestr(arg[2]))
            if not nx_exists(child) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("not found child[{@0:名称}]"));
                return;
            end
            nx_set_data(gm, "GMPropTarget", child);
        elseif para0 == "index" then
            if table.getn(arg) < 2 then
                --格式：setobj index 序号
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setoj index <no>"));
                return;
            end
            local child = nx_get_item(target, nx_int(arg[2]))
            if not nx_exists(child) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("No object at index:" .. nx_string(arg[2])));--未找到子对象[{@0:名称}]
                return;
            end
            nx_set_data(gm, "GMPropTarget", child);
        elseif para0 == "parent" then
            local parent = nx_parent(target);
            if not nx_exists(parent) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("No parent"));--未找到父对象
                return;
            end
            nx_set_data(gm, "GMPropTarget", parent);
        elseif para0 == "config" then
            if table.getn(arg) < 2 then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setobj config <configd>"));--格式：setobj config configid
                return;
            end
            local configid = nx_string(arg[2]);
            local childs = nx_get_child_list(target, 0);
            local proptarget = nx_null();
            for i, child in pairs(childs) do
                if nx_get_config(child) == configid then
                    proptarget = child;
                    break;
                end
            end
            if not nx_exists(proptarget) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found child object configid is:" .. configid));--未找到子对象
                return;
            end
            nx_set_data(gm, "GMPropTarget", proptarget);
        elseif para0 == "script" then
            if table.getn(arg) < 2 then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setobj script <sctipt>"));--格式：setobj script <script>
                return;
            end
            local script = nx_string(arg[2]);
            local childs = nx_get_child_list(target, 0);
            local proptarget = nx_null();
            for i, child in pairs(childs) do
                if nx_get_script(child) == script then
                    proptarget = child;
                    break;
                end
            end
            if not nx_exists(proptarget) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found child object script is:" .. script));--未找到子对象
                return;
            end
            nx_set_data(gm, "GMPropTarget", proptarget);
        else
            --直接根据名称查找子对象
            local child = nx_get_child(target, nx_widestr(arg[1]))
            if not nx_exists(child) then
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found child object name:" .. nx_string(arg[1])));--未找到子对象[{@0:名称}]
                return;
            end
            nx_set_data(gm, "GMPropTarget", child);
        end
    end
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Set object property succeed!");
end

--查询当前属性操作的对象的所有子对象
function child(gm, ...)
	local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

	local script = "";
	if table.getn(arg) > 1 then
		script = nx_string(arg[1]);
	end

	local childlist = nx_get_child_list(target, 0);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(nx_string(nx_query_property(target, "Name")) .. " Child Object List:"));
	for i = 1, table.getn(childlist) do
		if string.len(script) == 0 or nx_get_script(childlist[i]) == script then
			nx_sysinfo(gm, TIPSTYPE_GMINFO, "0",  nx_widestr("Config:" .. nx_get_config(childlist[i]) .. " Name:" .. nx_string(nx_query_property(childlist[i], "Name")) .. " Object:" .. nx_string(childlist[i]) .. " Script:" .. nx_get_script(childlist[i])));
		end
	end
end

--查询当前属性操作的对象的创建脚本和配置文件名称
function see(gm)
	local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Script: " .. nx_get_script(target) .. " Config:" .. nx_get_config(target) .. " PERSISTID:" .. nx_string(target)));
end

--查询当前属性操作的对象的所有属性
function property(gm)
	local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

	local proplist = nx_get_property_list(target);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("=====" .. nx_string(nx_query_property(target, "Name")) .. " Property List Begin:" .. "====="));
	for i = 1, table.getn(proplist) do
		--[{@0:属性}]={@1:数值}
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(proplist[i] .. "=" .. nx_string(nx_query_property(target, proplist[i]))));
	end
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("======" .. nx_string(nx_query_property(target, "Name")) .. " Property List End" .. "======"));
end

--设置当前属性操作的对象的属性值
--set 属性名 属性值
function set(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: set <property> <value>"));--格式：set 属性名称 属性值
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);

    --不能设置的属性列表
    local cant_set_prop_lst = {"GameMaster"};
    for i, prop_temp in pairs(cant_set_prop_lst) do
        if prop == prop_temp then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Can not set this property.");--不能设置此属性
            return;
        end
    end

    if not nx_find_property(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found property:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

	local type = nx_get_property_type(target, prop);
    if table.getn(arg) == 1 then
        if type == VTYPE_STRING then
            nx_set_property(target, prop, "");
        elseif type == VTYPE_WIDESTR then
            nx_set_property(target, prop, nx_widestr(""));
        else
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: set <property> <value>"));--格式：set 属性名称 属性值
            return
        end

    else
        local value = arg[2];
        if type == VTYPE_INT then
            nx_set_property(target, prop, nx_int(value));
        elseif type == VTYPE_INT64 then
            nx_set_property(target, prop, nx_int64(value));
        elseif type == VTYPE_FLOAT then
            nx_set_property(target, prop, nx_float(value));
        elseif type == VTYPE_DOUBLE then
            nx_set_property(target, prop, nx_double(value));
        elseif type == VTYPE_STRING then
            nx_set_property(target, prop, nx_string(value));
        elseif type == VTYPE_WIDESTR then
            nx_set_property(target, prop, nx_widestr(value));
        elseif type == VTYPE_OBJECT then
            nx_set_property(target, prop, nx_object(value));
        end
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Set " .. prop .. " succeed."));--设置属性{@0:属性名}成功
end

function adddata(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: adddata <dataname> <type>"));--格式：set 属性名称 属性类型
        return;
    end

    local target = get_prop_target(gm)

    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);
    local type = arg[2];

    if nx_find_data(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("have same data named:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

    if nx_int(type) == VTYPE_INT or
        nx_int(type) == VTYPE_INT64 or
        nx_int(type) == VTYPE_FLOAT or
        nx_int(type) == VTYPE_DOUBLE or
        nx_int(type) == VTYPE_STRING or
        nx_int(type) == VTYPE_WIDESTR or
        nx_int(type) == VTYPE_OBJECT then

        nx_add_data(target, prop, nx_int(type))
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Add data " .. prop .. " succeed."));--设置Data{@0:属性名}成功
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("data type is wrong"));--未找到属性：{@0:属性名}
        return;
    end

end

function removedata(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: adddata <dataname>"));--格式：set 属性名称 属性类型
        return;
    end

    local target = get_prop_target(gm)

    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);

    if not nx_find_data(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data named:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

    nx_remove_data(target, prop)
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Remove data " .. prop .. " succeed."));--设置Data{@0:属性名}成功

end

--设置当前属性操作的对象的属性值
--setdata 属性名 属性值
function setdata(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: setdata <dataname> <value>"));--格式：set 属性名称 属性值
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);
    local value = arg[2];

    if not nx_find_data(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data named:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

    local type = nx_get_data_type(target, nx_string(prop));
    if type == VTYPE_INT then
        nx_set_data(target, prop, nx_int(value));
    elseif type == VTYPE_INT64 then
        nx_set_data(target, prop, nx_int64(value));
    elseif type == VTYPE_FLOAT then
        nx_set_data(target, prop, nx_float(value));
    elseif type == VTYPE_DOUBLE then
        nx_set_data(target, prop, nx_number(value));
    elseif type == VTYPE_STRING then
        nx_set_data(target, prop, nx_string(value));
    elseif type == VTYPE_WIDESTR then
        nx_set_data(target, prop, nx_widestr(value));
    elseif type == VTYPE_OBJECT then
        nx_set_data(target, prop, nx_object(value));
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Set data " .. prop .. " succeed."));--设置Data{@0:属性名}成功
end


--查询当前属性操作的对象的属性值
--query 属性名
function query(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:query <property>"));--格式：query 属性名称
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);

    if not nx_find_property(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found property:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

    local value = nx_query_property(target, prop);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(prop .. "=" .. nx_string(value)));--[{@0:属性}]={@1:数值}
end

--查询当前属性操作的对象的属性值
--querydata 属性名
function querydata(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:querydata <dataname>"));--格式：querydata Data名称
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local prop = nx_string(arg[1]);
    if not nx_find_data(target, prop) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data named:" .. prop));--未找到属性：{@0:属性名}
        return;
    end

    local value = nx_query_data(target, prop);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(prop.. "=" .. nx_string(value)));--[{@0:属性}]={@1:数值}
end

--查询当前属性操作的对象的所有表格名
function record(gm)
    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local total = 0
    local reclist = nx_get_record_list(target);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("=====" .. nx_string(nx_query_property(target, "Name")) .. " Record List Begin:" .. "====="));
    for i = 1, table.getn(reclist) do
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(reclist[i]));
        total = i
    end
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("======" .. nx_string(nx_query_property(target, "Name")) .. " Record List End.total:" ..nx_string(total).. "======"));
end

--设置当前属性操作的对象的表格值
--recordset 表格名 行号 列号 值
function recordset(gm, ...)
    if table.getn(arg) < 4 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: recordset <recname> <row> <col> <value>"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1]);
    local row = nx_int(arg[2]);
    local col = nx_int(arg[3]);
    local value = arg[4];

    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

    --行 列 校验
    local rec_row = nx_get_record_rows(target, recname)
    local rec_col = nx_get_record_cols(target, recname)

    if nx_number(row) >= nx_number(rec_row) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("recordset row error"))
        return
    end

    if nx_number(col) >= nx_number(rec_col) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("recordset col error"))
        return
    end

    local type = nx_get_record_coltype(target, recname, col);
    if type == VTYPE_INT then
        nx_set_record_value(target, recname, row, col, nx_int(value));
    elseif type == VTYPE_INT64 then
        nx_set_record_value(target, recname, row, col, nx_int64(value));
    elseif type == VTYPE_FLOAT then
        nx_set_record_value(target, recname, row, col, nx_float(value));
    elseif type == VTYPE_DOUBLE then
        nx_set_record_value(target, recname, row, col, nx_double(value));
    elseif type == VTYPE_STRING then
        nx_set_record_value(target, recname, row, col, nx_string(value));
    elseif type == VTYPE_WIDESTR then
        nx_set_record_value(target, recname, row, col, nx_widestr(value));
    elseif type == VTYPE_OBJECT then
        nx_set_record_value(target, recname, row, col, nx_object(value));
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set record " .. recname .. " value succeed!"));
end

--查询当前属性操作的对象的表格内容
--recordquery 表格名
function recordquery(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: recordquery <recname>"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("================" .. nx_string(nx_query_property(target, "Name")) .. ":" .. recname .. "================"));
    local cols = nx_get_record_cols(target, recname);
    local rows = nx_get_record_rows(target, recname);
    for row = 0, rows - 1 do
        local text = nx_string(row) .. "|";
        for col = 0, cols - 1 do
            text = text .. nx_string(nx_query_record_value(target, recname, row, col)) .. "|";
        end
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(text));
    end
end

-- 格式化当前属性操作的对象的表格内容
-- recordshow 表格名
function recordshow(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: recordshow <recname>"))
        return
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1])
    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname))
        return
    end

    -- 打印表名
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("================"
     .. nx_string(nx_query_property(target, "Name")) .. ":" .. recname .. "================"))

    -- 宽度表
    local width_table = {}

    -- 遍历取得每列的最大宽度
    local rows = nx_get_record_rows(target, recname)
    local cols = nx_get_record_cols(target, recname)
    local total_width = cols + 1

    for i = 0, cols - 1 do
        local max_width = 1

        for j = 0, rows - 1 do
            local width = string.len(tostring(nx_query_record_value(target, recname, j, i)))
            if width > max_width then
                max_width = width
            end
        end

        width_table[i] = max_width
        total_width = total_width + max_width
    end

    -- 行分割线
    local row_split_text = ""
    for e = 1, total_width do
        row_split_text = row_split_text .. "-"
    end

    -- 格式化输出表内容
    for row = 0, rows - 1 do
        -- 分割线
        --nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(row_split_text))

        local text = "|"

        for col = 0, cols - 1 do
            local value = nx_string(nx_query_record_value(target, recname, row, col))
            text = text .. value
            local len = string.len(value)
            local need_space = width_table[col] - len
            for k = 1, need_space do
                text = text .. " "
            end
            text = text .. "|"
        end

        -- 输出
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(text))
    end

    -- 分割线
    --nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(row_split_text))
end

--添加一行
--recordaddrow 表格名 ...
function recordaddrow(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Format: recordaddrow recname <row> ...");
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record " .. recname));
        return;
    end

    local cols = nx_get_record_cols(target, recname);
    if table.getn(arg) < (1 + cols) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Record " .. recname .. " columns is " .. cols));
        return;
    end

    local row = nx_add_record_row(target, recname);

    for col = 0, cols - 1 do
        local coltype = nx_get_record_coltype(target, recname, col);
        if coltype == VTYPE_INT then
            nx_set_record_value(target, recname, row, col, nx_int(arg[col + 2]));
        elseif coltype == VTYPE_INT64 then
            nx_set_record_value(target, recname, row, col, nx_int64(arg[col + 2]));
        elseif coltype == VTYPE_FLOAT then
            nx_set_record_value(target, recname, row, col, nx_float(arg[col + 2]));
        elseif coltype == VTYPE_DOUBLE then
            nx_set_record_value(target, recname, row, col, nx_number(arg[col + 2]));
        elseif coltype == VTYPE_STRING then
            nx_set_record_value(target, recname, row, col, nx_string(arg[col + 2]));
        elseif coltype == VTYPE_WIDESTR then
            nx_set_record_value(target, recname, row, col, nx_widestr(arg[col + 2]));
        elseif coltype == VTYPE_OBJECT then
            nx_set_record_value(target, recname, row, col, nx_object(arg[col + 2]));
        end
    end
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Add row succeed!"));
end

--删除表格的指定行的数据
--recordremoverow 表格名 行号
function recordremoverow(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: recordremoverow recname row"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1]);
    local row = nx_int(arg[2]);

    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Not found record " .. recname);
        return;
    end

    if not nx_remove_record_row(target, recname, row) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Remove record " .. recname .. " row " .. row .. " failed!");
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Remove record " .. recname .. " row " .. row .. " succeed!");
    end
end

--清除指定表格的内容
--recordclear 表格名
function recordclear(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Format: recordclear recname");
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local recname = nx_string(arg[1]);

    if not nx_find_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Not found record " .. recname);
        return;
    end

    if not nx_clear_record(target, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Clear record " .. recname .. " failed!");
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Clear record " .. recname .. " succeed!");
    end
end

--选择当前属性操作对象为选中对象
--select 对象名称
function select(gm)
    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    nx_set_property(gm, "LastObject", target);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Selected " .. nx_string(nx_query_property(target, "Name")) .. " succeed!"));
end

--删除当前选中的对象
function del(gm)
    local target = nx_query_property(gm, "LastObject");
    if not nx_exists(target) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("LastObject not exists!"));
        return;
    end
    if target ~= gm then
        if nx_destroy_self(target) then
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Delete selected object succeed!");
        else
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", "Delete selected object failed!");
        end
    end
end

--瞬移到某个点: goto [x] [z]
function goto(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: goto <x> <z>"));
        return;
    end

    local x, y, z, orient = nx_get_position(gm);
    x = nx_float(arg[1]);
    z = nx_float(arg[2]);
    y = nx_get_height(x, z, 0);
    --nx_msgbox(nx_string(y))

    if nx_moveto(gm, x, y, z, orient) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("goto to " .. nx_int(x) .. " " .. nx_int(z) .. " succeed." ));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("goto to " .. nx_int(x) .. " " .. nx_int(z) .. " failed." ));
    end
end

--瞬移到某个点: goto [x] [z]
function gotoex(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: goto <x> <z> <floor>"));
        return;
    end

    local x, y, z, orient = nx_get_position(gm);
    x = nx_float(arg[1]);
    z = nx_float(arg[2]);
    floor = nx_int(arg[3]);
    
    y = nx_get_height(x, z, floor);
    --nx_msgbox(nx_string(y))

    if nx_moveto(gm, x, y, z, orient) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("gotoex to " .. nx_int(x) .. " " .. nx_int(y) .." " .. nx_int(z) .. " succeed." ));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("gotoex to " .. nx_int(x) .. " " .. nx_int(y) .." ".. nx_int(z) .. " failed." ));
    end
end

--location
function location(gm, ...)
    if table.getn(arg) < 2 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: goto <x> <z>"));
        return;
    end

    local x, y, z, orient = nx_get_position(gm);
    x = nx_float(arg[1]);
    z = nx_float(arg[2]);
	orient = nx_float(arg[3]);
    y = nx_get_height(x, z, 0);

    if nx_location(gm, x, y, z, orient) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("location to " .. nx_int(x) .. " " .. nx_int(z) .. " succeed." ));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("location to " .. nx_int(x) .. " " .. nx_int(z) .. " failed." ));
    end
end

--查询GM当前位置信息
function pos(gm)
    local x, y, z, orient, col, row = nx_get_position(gm);
    local height = nx_get_height(x, z, 0);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("X:" .. nx_number(x) .. " Y:" .. nx_number(y) .. " Z:" .. nx_number(z) .. " Orient:" .. nx_number(orient).."Col"..nx_number(col).."Row"..nx_number(row)));
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Map Height :" .. nx_number(height)));
    --nx_get_grid_index(gm,x, z);
end

--查询选中对象的位置信息
function posobj(gm)
    local target = nx_query_data(gm, "GMPropTarget");
    if not nx_exists(target) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("LastObject not exists!"));
        return;
    end

    local x, y, z, orient = nx_get_position(target);
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("X:" .. nx_number(x) .. " Y:" .. nx_number(y) .. " Z:" .. nx_number(z) .. " Orient:" .. nx_number(orient)));
end

-- 查找对象身上是否有某心跳
function findhb(gm,...)
    if table.getn(arg) == 0 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("format : findhb <hb_name>"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end
    local hb_name = nx_string(arg[1]);

    if nx_find_heartbeat(gm, hb_name) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("find the hb!"))
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("not find the hb!"))
    end
end

--给当前属性操作对象加一个心跳
function addhb(gm, ...)
    if table.getn(arg) == 0 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("format : addhb <function name> <interval>"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local funcname = nx_string(arg[1]);
    local sep = nx_int(arg[2]);

    if nx_add_heartbeat(target, funcname, sep) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("add heartbeat " .. funcname .. "success !"));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("add heartbeat " .. funcname .. "failed !"));
    end
end

--删除当前属性操作对象的一个心跳
function removehb(gm, ...)
    if table.getn(arg) == 0 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("format : removehb <function name>"));
        return;
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

    local funcname = nx_string(arg[1]);
    if nx_remove_heartbeat(target, funcname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove heartbeat " .. funcname .. "success !"));
    else
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove heartbeat " .. funcname .. "failed !"));
    end
end

--在包裹中创建一个物品
--cobj 对象ID
function cobjitem(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format:cobjitem <ID>"));--格式：cobj 编号
		return;
	end

	local id = nx_string(arg[1]);
	local num = 1;

	if table.getn(arg) > 1 then
		num = nx_int(arg[2]);
	end


	local item = nx_create_item(gm, id, num);
	if not nx_exists(item) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Create object " .. id .. " failed!"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Create object " .. id .." succeed!"));
	end
end

--创建一个游戏对象
--cobj 对象ID
function cobj(gm, ...)
    if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:cobj <ID>"));--格式：cobj 编号
        return;
    end

    local x,y,z,o = nx_get_position(gm);
    local id = nx_string(arg[1]);

    local item = nil
    local scene = nx_get_scene()
    if nx_query_property(scene, "Grouping") >= 1 then
        local groupid = nx_query_property(gm,"GroupID")
        item = nx_create_obj(id, 0, x, y, z, o, groupid);
    else
        item = nx_create_obj(id, 0, x, y, z, o);
    end



    if not nx_exists(item) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Create object " .. id .. " failed!"));
    else
        nx_set_property(item, "GroupID", groupid)
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Create object " .. id .." succeed!"));
    end
end

--cobjchild 对象ID
function cobjchild(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: cobjchild <id> [num]"));--格式：cobjchild id
		return;
	end

	local target = get_prop_target(gm)
	if not nx_exists(target) then
		return
	end

	local id = nx_string(arg[1]);
	local count = nx_int(arg[2]);

	--nx_msgbox("123");

	local item = nx_createitem_from_config(target,id,count);
	if not nx_exists(item) then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Create object " .. id .. " failed!"));
	else
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Create object " .. id .." succeed!"));
	end
end

function servertime(gm)
    local time = os.time();
    local timeTab = os.date("*t",time)

    local sInfo = "time: ";
    sInfo = sInfo .. timeTab.year .. "." .. timeTab.month .. "." .. timeTab.day .. " ";
    sInfo = sInfo .. timeTab.hour .. ":" .. timeTab.min .. ":" .. timeTab.sec
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(sInfo));
end

-------------------公共数据区相关GM命令---------------------------

--公共数据表查询
function pubrecquery(gm, ...)
    if table.getn(arg) < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecquery <pubname> <dataname> <recname>"));
        return;
    end
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    --nx_msgbox(nx_string(pubname))

    if not nx_find_pubdata_record(pubname, dataname, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("================" .. recname .. "================"));

    local rows = nx_get_pubdata_record_rows(pubname, dataname, recname);
    --nx_msgbox(nx_string(rows))
    for row = 0, rows - 1 do
        local text = nx_string(row) .. "|";
        local value = nx_query_pubdata_record_row_values(pubname, dataname, recname, row);
        for i = 1, table.getn(value) do
            text = text .. nx_string(value[i]) .. "|";
        end
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(text));
    end
end

--公共数据表查询
function pubrecfindrow(gm, ...)
    if table.getn(arg) < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecfindrow <pubname> <dataname> <recname> <col> <colvalue>"));
        return;
    end
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    local col = nx_int(arg[4]);
	local colvalue = arg[5];
	-- 根据列类型取值
	local coltype = nx_get_pubdata_record_coltype(pubname, dataname, recname, col);

	-- 查找所在行
	local row = nx_find_pubdata_record_value(pubname, dataname, recname, col, nx_to_type(coltype, colvalue));
    if row < 0 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found "));
        return;
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("GuildName: "..nx_string(colvalue).." row:"..row));
end

function pubrecclear(gm, ...)
    if table.getn(arg) < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecclear <pubspace> <pubname> <recname>"));
        return;
    end

    local space = nx_string(arg[1])
    local pubname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);

    if not nx_find_pubdata_record(space, pubname, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

    if not nx_clear_pub_rec(space, pubname, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Clear record: " .. recname .. " fail!") );         return;
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Clear record: " .. recname .. " success!") );
end

--添加一行
--recordaddrow 表格名 ...
function pubrecaddrow(gm, ...)
    if table.getn(arg) < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecaddrow <pubname> <recname> ..."));
        return;
    end

    local space = nx_string(arg[1])
    local pubname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);

    if not nx_find_pubdata_record(space, pubname, recname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found record: " .. recname));
        return;
    end

    local cols = nx_get_pubdata_record_cols(space, pubname, recname);
	-- nx_msgbox(nx_string(cols));
    if table.getn(arg) < (3 + cols) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pub record " .. recname .. " columns is " .. cols));
        return;
    end

	local var = {};
    for col = 0, cols - 1 do
        local coltype = nx_get_pubdata_record_coltype(space, pubname, recname, col);
		var[col+1] = nx_to_type(coltype, arg[col+4])
    end

    if nx_add_pub_rec_row_value(space, pubname, recname, unpack(var)) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Add row succeed!"));
    end
end

--设置公共数据的表格值
--pubrecset 空间名称 表格名称 行号 列号 值
function pubrecset(gm, ...)

    if table.getn(arg) < 6 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecset <spacename> <pubname> <recname> <row> <col> <value>"));
        return;
    end

    local spacename = nx_string(arg[1])
    local pubname = nx_widestr(arg[2])
    local recname = nx_string(arg[3])
    local row = nx_int(arg[4])
    local col = nx_int(arg[5])
    local value = arg[6];

    local type = nx_get_pubdata_record_coltype(spacename, pubname, recname, col)

    if nx_int(type) == nx_int(0) then
       nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pubrecset: no find rec!"))
       return
    end

    if type == VTYPE_INT then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_int(value))
    elseif type == VTYPE_FLOAT then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_float(value))
    elseif type == VTYPE_DOUBLE then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_double(value))
    elseif type == VTYPE_STRING then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_string(value))
    elseif type == VTYPE_WIDESTR then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_widestr(value))
    elseif type == VTYPE_OBJECT then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_object(value))
    elseif type == VTYPE_INT64 then
        nx_set_pub_rec_value(spacename, pubname, recname, row, col, nx_int64(value))
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set record " .. recname .. " value succeed!"));
end

--删除公共数据区指定表格指定行的数据
function pubrecremove(gm, ...)
    if table.getn(arg) < 4 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrecremove <pubname> <dataname> <recname> <rownum>"));
	return;
    end
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    local row = nx_int(arg[4]);

    local rows = nx_get_pubdata_record_rows(pubname, dataname, recname);
    if row < rows - 1 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data: " .. row));
	return;
    end

    if not nx_remove_pub_rec_row(pubname, dataname, recname, row) then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data: " .. recname));
	return;
    end
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove data success!"));

end

--删除公共数据区指定表某个数据行
function pubrovbykey(gm, ...)
    if table.getn(arg) < 5 then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubrovbykey <pubname> <dataname> <recname> <col> <value>"));
	return;
    end
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    local col = nx_int(arg[4]);

    local type = nx_get_pubdata_record_coltype(pubname, dataname, recname, col);

    if nx_int(type) == nx_int(0) then
       nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pubrecremovebykey: no find col!"))
       return
    end

    local value
    if type == VTYPE_INT then
        value = nx_int(arg[5])
    elseif type == VTYPE_FLOAT then
        value = nx_float(arg[5])
    elseif type == VTYPE_DOUBLE then
        value = nx_double(arg[5])
    elseif type == VTYPE_STRING then
        value = nx_string(arg[5])
    elseif type == VTYPE_WIDESTR then
        value = nx_widestr(arg[5])
    elseif type == VTYPE_OBJECT then
        value = nx_object(arg[5])
    elseif type == VTYPE_INT64 then
        value = nx_int64(arg[5])
    end

    if not nx_remove_pub_rec_row_bykey(pubname, dataname, recname, col, value) then
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data: " .. recname));
	return;
    end
    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("remove data success!"));

end

function pubpropquery(gm, ...)
    if table.getn(arg) < 3 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubpropquery <pubname> <dataname> <propname>"));
        return;
    end


    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local propname = nx_string(arg[3]);

    if not nx_find_pubdata_property(pubname, dataname, propname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found data: " .. propname));
        return;
    end

    local value = nx_query_pubdata_property(pubname, dataname, propname);

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(propname .. "=" .. nx_string(value)));
end

-- 设置公共区属性的值
function pubpropset(gm, ...)
    if table.getn(arg) < 4 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format: pubpropset <pubname> <dataname> <propname><value>"));
        return;
    end

    local spacename = nx_string(arg[1])
    local pubname = nx_widestr(arg[2])
    local propname = nx_string(arg[3]);
    local value = arg[4]

    if not nx_find_pubdata_property(spacename, pubname, propname) then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Not found property: " .. propname));
        return;
    end

    local type = nx_get_pubdata_property_type(spacename, pubname, propname)

    if nx_int(type) == nx_int(0) then
       nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("pubpropset: no find prop!"))
       return
    end

    if type == VTYPE_INT then
        nx_set_pub_property(spacename, pubname, propname, nx_int(value))
    elseif type == VTYPE_INT64 then
        nx_set_pub_property(spacename, pubname, propname, nx_int64(value))
    elseif type == VTYPE_FLOAT then
        nx_set_pub_property(spacename, pubname, propname, nx_float(value))
    elseif type == VTYPE_DOUBLE then
        nx_set_pub_property(spacename, pubname, propname, nx_double(value))
    elseif type == VTYPE_STRING then
        nx_set_pub_property(spacename, pubname, propname, nx_string(value))
    elseif type == VTYPE_WIDESTR then
        nx_set_pub_property(spacename, pubname, propname, nx_widestr(value))
    elseif type == VTYPE_OBJECT then
        nx_set_pub_property(spacename, pubname, propname, nx_object(value))
    end

    nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("set pub property: " .. propname .. " value is succeed!"));
end
-------------------公共数据区相关GM命令---------------------------

function ss(gm, ...)
	local serverid = nx_get_server_id(gm);
	local num = table.getn(arg);
    if num == 0 then
        local rows = nx_get_pubdata_record_rows("domain", nx_widestr("Domain_SceneList"..serverid), "scene_list_rec");
        for i = 0, rows - 1 do
            local values = nx_query_pubdata_record_row_values("domain", nx_widestr("Domain_SceneList"..serverid), "scene_list_rec", i);
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(nx_string(values[1]).." "..nx_string(values[2])));
        end
    else
        local id = nx_int(arg[1]);
        nx_switch_object(id, gm);
		--nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("ss to " .. scene .. " succeed!"));
    end
end

--移到某个玩家所在坐标: goplayer [玩家名]
function goplayer(gm, ...)
    --if table.getn(arg) == 0 then
        --格式：
        --nx_sysinfo(gm, TIPSTYPE_GMINFO, 0, 1943);--格式：goplayer 玩家名称
        --return;
    --    arg[1] = getcurobjname(gm);
   -- end

    local player = nx_find_player( arg[1] );
    if not nx_exists(player) then
        scene_id = nx_get_player_scene(arg[1]); --取玩家场景号
        if scene_id > 0 then
            --[{@0:名称}]在{@1:数值}号场景中，请先切换到该场景
            nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(nx_string(arg[1]) .. " is in scene:" .. scene_id));
        else
            local serverid = nx_get_player_serverid(arg[1]);
            if (serverid > 0) then
                local servername = nx_get_servername(serverid);
                --[{@0:名称}]在[{@1:服务器名}]
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(nx_string(arg[1]) .. " is in server:" .. nx_string(servername)));
            else
                --找不到名称为[{@0:名称}]的玩家
                nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("can't find player:" .. nx_string(arg[1])));
            end
        end
    else
        local x, y, z, orient = nx_get_position(player);
        nx_moveto(gm, x, y, z, orient);
    end
end

--移动到某个点
function movetopoint(gm, ... )
	 local target = get_prop_target(gm)
    if not nx_exists(target) then
        return
    end

	if table.getn(arg) < 1 then
        nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("Format:movetopoint dxdz"));
        return;
        end
	local dx = nx_float(arg[1]);
	local dz = nx_float(arg[2]);
	nx_movetopoint(target,dx,dz);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("movetopoint succeed!"));
end

--获取对应格子坐标
function getpos(gm, ...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: getpos index"));
		return;
	end
	local index = nx_int(arg[1]);
	local value = nx_getpos_bygrid(index);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("getpos pos x: " .. nx_string(value[1]) .. " z: ".. nx_string(value[2]) .. "  Index:" .. index));
end

--获取对应格子
function getindex(gm, ...)
	if table.getn(arg) < 2 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: getindex posx posz"));
		return;
	end
	local posx = nx_float(arg[1]);
	local posz = nx_float(arg[2]);
	local value = nx_getgrid_bypos(posx, posz);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("getindex pos x: " .. nx_string(posx) .. " z: ".. nx_string(posz).. " Index:" .. nx_int(value)));
end

--旋转怪物
function rotate(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: rotate angle"));
		return;
	end

	local target = get_prop_target(gm);
	if not nx_exists(target) then
		return;
	end

	local angle = nx_int(arg[1]);
	nx_rotate(target,angle,6.28);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("rotate succeed"));
end

--获取场景里指定怪物坐标位置
function getnpcpos(gm,...)
	if table.getn(arg) < 1 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr("Format: getnpcpos configid"));
		return;
	end

	--获取对应怪物信息
	local configid = nx_string(arg[1]);
	local scene = nx_get_scene();
	local npclist = nx_get_child_list(scene, TYPE_NPC);
	local posinfo = "";
	for i, child in pairs(npclist) do
		if nx_get_config(child) == configid then
			local x, y, z, orient = nx_get_position(child);
			posinfo = posinfo .. " x:" .. nx_string(x) .. " y:" .. nx_string(y) .. " z:" .. nx_string(z) .." \n";
		end
	end
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "gm_info", nx_widestr(posinfo));
end

-- 拉取玩家离线登陆
function offline_player_login(gm, ...)

	if table.getn(arg) < 3 then
		nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("offline_player_login failed"));
		return;
	end

	local name = nx_widestr(arg[1]);
	local account = nx_string(arg[2]);
	local time = nx_string(arg[3]);
	local info = "name"..nx_string(arg[1]).. "account" ..nx_string(arg[2]).. "time"..nx_string(arg[3]);
	nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr(info));
	if (nx_offline_player_login(name, account, time)) then
		--nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("offline_player_login succeed"));
	else
		--nx_sysinfo(gm, TIPSTYPE_GMINFO, "0", nx_widestr("offline_player_login failed"));
	end

end