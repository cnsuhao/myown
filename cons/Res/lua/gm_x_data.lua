require("gm_x_util")
require("define")
local gm_helper = require("gm_x_helper")

------------------------------------------------------------------
-- 游戏玩家数据区
--查询当前属性操作的对象的属性值
--x_query 属性名
function x_query(gm, ...)
	if table.getn(arg) < 1 then
        return gm_helper.error("format:x_query <property>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return gm_helper.error("not found target");
    end

    local prop = nx_string(arg[1]);
    if not nx_find_property(target, prop) then
        return gm_helper.error("not found property");
    end

    local value = nx_query_property(target, prop);
	return gm_helper.content(value);
end

--查询当前属性操作的对象的所有属性
function x_property(gm)
    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return gm_helper.error("not found target");
    end

	local data = {};
	local proplist = nx_get_property_list(target);
	for i = 1, table.getn(proplist) do
		--[{@0:属性}]={@1:数值}
		data[ proplist[i] ] = nx_query_property(target, proplist[i]);
	end
	
	return gm_helper.content(data);
end

--设置当前属性操作的对象的属性值
--x_set 属性名 属性值
function x_set(gm, ...)
    if table.getn(arg) < 1 then
        return gm_helper.error("format:x_set <property> <value>");	
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return gm_helper.error("not found target");
    end

    local prop = nx_string(arg[1]);
	
    --不能设置的属性列表
    local cant_set_prop_lst = {"GameMaster"};
    for i, prop_temp in pairs(cant_set_prop_lst) do
        if prop == prop_temp then
			return gm_helper.error("can't set this property");
        end
    end

    if not nx_find_property(target, prop) then
        return gm_helper.error( "property not found" );
    end

	local type = nx_get_property_type(target, prop);
    if table.getn(arg) == 1 then
        if type == VTYPE_STRING then
            nx_set_property(target, prop, "");
        elseif type == VTYPE_WIDESTR then
            nx_set_property(target, prop, nx_widestr(""));
        else
			return gm_helper.error( "format: x_set <property> <value>" );
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
	
	return gm_helper.succeed();
end

function x_adddata(gm, ...)
    if table.getn(arg) < 2 then
		return gm_helper.error("format:x_adddata <dataname> <type>");	
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return gm_helper.error("not found target");
    end

    local prop = nx_string(arg[1]);
    local type = arg[2];

    if nx_find_data(target, prop) then
	   return gm_helper.error("not found data");
    end

    if nx_int(type) == VTYPE_INT or
        nx_int(type) == VTYPE_INT64 or
        nx_int(type) == VTYPE_FLOAT or
        nx_int(type) == VTYPE_DOUBLE or
        nx_int(type) == VTYPE_STRING or
        nx_int(type) == VTYPE_WIDESTR or
        nx_int(type) == VTYPE_OBJECT then

        nx_add_data(target, prop, nx_int(type))
		return gm_helper.succeed();
    else
		return gm_helper.error("data type error");
    end
end

function x_removedata(gm, ...)

    if table.getn(arg) < 1 then
		return gm_helper.error("format:x_removedata <dataname>");	
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
        return gm_helper.error( "not found target" );
    end

    local prop = nx_string(arg[1]);
    if nx_find_data(target, prop) then
		return gm_helper.error("not found data");	
    end
	
	nx_remove_data(target, prop);
	return gm_helper.succeed();
end

--设置当前属性操作的对象的属性值
function x_setdata(gm, ...)
	local result = {};
    if table.getn(arg) < 2 then
        return gm_helper.error("format:x_setdata <dataname> <value>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local prop = nx_string(arg[1]);
    local value = arg[2];

    if not nx_find_data(target, prop) then
		return gm_helper.error("not found data");
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
  
	return gm_helper.succeed();	
end

-- 查询临时属性
function x_querydata(gm, ...)
	if table.getn(arg) < 1 then
		return gm_helper.error("format:x_querydata <dataname>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local prop = nx_string(arg[1]);
    if not nx_find_data(target, prop) then
		return gm_helper.error("not found data");
    end

    local value = nx_query_data(target, prop);
	return gm_helper.content(value);
end

--查询当前属性操作的对象的所有表格名(对应gm.lua的record(gm))
function x_record_list(gm)
    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

	local recordset = {};
    recordset.reclist = {};
	local reclist = nx_get_record_list(target);	
	for i = 1, table.getn(reclist) do
		table.insert(recordset.reclist, nx_string(reclist[i]));
	end
	recordset.number = #recordset.reclist;
    
	return gm_helper.content( recordset );
end

--设置当前属性操作的对象的表格值
--x_record_set_value 表格名 行号 列号 值
function x_record_set_value(gm, ...)
    if table.getn(arg) < 4 then
        return gm_helper.error("format: x_record_set_value <recname> <row> <col> <value>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    local row = nx_int(arg[2]);
    local col = nx_int(arg[3]);
    local value = arg[4];

    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    --行 列 校验
    local rec_row = nx_get_record_rows(target, recname)
    local rec_col = nx_get_record_cols(target, recname)

    if nx_number(row) >= nx_number(rec_row) then
        return gm_helper.error("row index overflow(index:" .. row .. "rows:" .. rec_row .. ")");
    end

    if nx_number(col) >= nx_number(rec_col) then
		return gm_helper.error("col index overflow(index:" .. col .. "cols:" .. rec_col .. ")");
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
	else
		return gm_helper.error("col type error");
    end

	return gm_helper.succeed();
end

--获取当前属性操作的对象的表格值
--x_record_get_value 表格名 行号 列号 值
function x_record_get_value(gm, ...)
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_record_get_value <recname> <row> <col>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    local row = nx_int(arg[2]);
    local col = nx_int(arg[3]);

    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    --行 列 校验
    local rec_row = nx_get_record_rows(target, recname)
    local rec_col = nx_get_record_cols(target, recname)

    if nx_number(row) >= nx_number(rec_row) then
        return gm_helper.error("row index overflow(index:" .. row .. "rows:" .. rec_row .. ")");
    end

    if nx_number(col) >= nx_number(rec_col) then
		return gm_helper.error("col index overflow(index:" .. col .. "cols:" .. rec_col .. ")");
    end

    local value = nx_string( nx_query_record_value(target, recname, row, col) );
	return gm_helper.content(value);
end

--查询当前属性操作的对象的表格内容
--x_record_data 表格名<对应recordquery>
function x_record_data(gm, ...)
    if table.getn(arg) < 1 then
        return gm_helper.error("format: x_record_data <recname>");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    local cols = nx_get_record_cols(target, recname);
    local rows = nx_get_record_rows(target, recname);
	local data = {};
    for r = 0, rows - 1 do
        local row = {};
        for c = 0, cols - 1 do
            row[c+1] = nx_string(nx_query_record_value(target, recname, r, c));
        end
        
		data[r+1] = row;
    end
	
	return gm_helper.content(data);
end

--查询当前属性操作的对象的特定行内容
--x_record_rowdata 表格名 行号(默认为第1行)
function x_record_rowdata(gm, ...)
    if table.getn(arg) < 1 then
        return gm_helper.error("format: x_record_rowdata <recname> [row]");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end
	
	local row = 0;
	if table.getn(arg) > 1 then
		row = nx_int(arg[2]);
	end
	
    local cols = nx_get_record_cols(target, recname);
    local rows = nx_get_record_rows(target, recname);
	if row >= rows then
		return gm_helper.error("row index overflow, record:" .. recname .. " rows:" .. rows);
	end
	
	local data = {};
	for col = 0, cols - 1 do
		data[col+1] = nx_string(nx_query_record_value(target, recname, row, col));
	end
	
	return gm_helper.content(data);
end

--添加一行
--record_addrow 表格名 ...
function x_record_addrow(gm, ...)
    if table.getn(arg) < 1 then
        return gm_helper.error("format: x_record_addrow recname <row data>...");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    local cols = nx_get_record_cols(target, recname);
    if table.getn(arg) < (1 + cols) then
        return gm_helper.error("param error, " .. recname .. " columns is " .. cols);
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
	
    return gm_helper.succeed();
end

--删除表格的指定行的数据
--record_removerow 表格名 行号
function x_record_removerow(gm, ...)
    if table.getn(arg) < 2 then
        return gm_helper.error("format: x_record_removerow recname row");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    local row = nx_int(arg[2]);

    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    if not nx_remove_record_row(target, recname, row) then
		return gm_helper.error("remove failed." );
    else
		return gm_helper.succeed();
    end
end

--清除指定表格的内容
--recordclear 表格名
function x_record_clear(gm, ...)
    if table.getn(arg) < 1 then
		return gm_helper.error("format: x_record_clear recname");
    end

    local target = get_prop_target(gm)
    if not nx_exists(target) then
		return gm_helper.error("not found target");
    end

    local recname = nx_string(arg[1]);
    if not nx_find_record(target, recname) then
		return gm_helper.error("not found record:" .. recname);
    end

    if not nx_clear_record(target, recname) then
		return gm_helper.error("execute failed");
    else
		return gm_helper.succeed();
    end
end


------------------------------------------------------------------
-- 公共数据区
--公共数据表所有数据
function x_pub_rec_data(gm, ...)
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_pub_rec_data <pubname> <dataname> <recname>");
    end
	
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
 
    if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end

	local data = {};
    local rows = nx_get_pubdata_record_rows(pubname, dataname, recname);
    for row = 0, rows - 1 do
        local rdata_ = {};
        local value = nx_query_pubdata_record_row_values(pubname, dataname, recname, row);
        for i = 1, table.getn(value) do
            rdata_[i] = nx_string(value[i]);
        end
        
		data[row+1] = rdata_;
    end
	return gm_helper.content(data);
end

--查询公共表数据(返回找到的行号)
function x_pub_rec_findrow(gm, ...)
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_pub_rec_findrow <pubname> <dataname> <recname> <col> <colvalue>");
    end
	
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    local col = nx_int(arg[4]);
	local colvalue = arg[5];
	
	if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end
	
	-- 根据列类型取值
	local coltype = nx_get_pubdata_record_coltype(pubname, dataname, recname, col);
	-- 查找所在行
	local row = nx_find_pubdata_record_value(pubname, dataname, recname, col, nx_to_type(coltype, colvalue));
	return gm_helper.content(row);
end

--清除公共数据表
function x_pub_rec_clear(gm, ...)
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_pub_rec_clear <pubname> <dataname> <recname>");
    end

    local pubname = nx_string(arg[1])
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
	
    if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end	

    if nx_clear_pub_rec(pubname, dataname, recname) then
        return gm_helper.succeed();
    end

    gm_helper.error("clean failed.");
end

--添加一行
--x_pub_rec_addrow 表格名 ...
function x_pub_rec_addrow(gm, ...)
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_pub_rec_addrow <pubname> <dataname> <recname> ...");
    end

    local pubname = nx_string(arg[1])
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);

    if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end	

    local cols = nx_get_pubdata_record_cols(pubname, dataname, recname);
    if table.getn(arg) < (3 + cols) then
        return gm_helper.error("pub record " .. recname .. " columns is " .. cols);
    end

	local var = {};
    for col = 0, cols - 1 do
        local coltype = nx_get_pubdata_record_coltype(pubname, dataname, recname, col);
		var[col+1] = nx_to_type(coltype, arg[col+4])
    end

    if nx_add_pub_rec_row_value(pubname, dataname, recname, unpack(var)) then
        return gm_helper.succeed();
    end
	
	return gm_helper.error("add row failed.");
end

--删除公共数据区指定表格指定行的数据
function x_pub_rec_removerow(gm, ...)
    if table.getn(arg) < 4 then
		gm_helper.error("format: x_pub_rec_removerow <pubname> <dataname> <recname> <rownum>");
    end
    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local recname = nx_string(arg[3]);
    local row = nx_int(arg[4]);
	
    if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end		

    local rows = nx_get_pubdata_record_rows(pubname, dataname, recname);
    if row < 0 or row > rows - 1 then
		return gm_helper.error("index overflow, max rows: " .. rows .. " index:" .. row);
    end

    if not nx_remove_pub_rec_row(pubname, dataname, recname, row) then
		return gm_helper.error("not found data: " .. recname);
    end
	
    return gm_helper.succeed();
end

--设置公共数据的表格值
--x_pub_rec_set_value 空间名称 表格名称 行号 列号 值
function x_pub_rec_set_value(gm, ...)

    if table.getn(arg) < 6 then
        return gm_helper.error("format: x_pub_rec_set_value <pubname> <dataname> <recname> <row> <col> <value>");
    end

    local pubname = nx_string(arg[1])
    local dataname = nx_widestr(arg[2])
    local recname = nx_string(arg[3])
    local row = nx_int(arg[4])
    local col = nx_int(arg[5])
    local value = arg[6];
	
	if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end	

    local type = nx_get_pubdata_record_coltype(pubname, dataname, recname, col)
    if nx_int(type) == nx_int(0) then
       return gm_helper.error("record not found or col index overflow.");
    end

	local result = false;
    if type == VTYPE_INT then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_int(value))
    elseif type == VTYPE_FLOAT then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_float(value))
    elseif type == VTYPE_DOUBLE then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_double(value))
    elseif type == VTYPE_STRING then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_string(value))
    elseif type == VTYPE_WIDESTR then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_widestr(value))
    elseif type == VTYPE_OBJECT then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_object(value))
    elseif type == VTYPE_INT64 then
        result = nx_set_pub_rec_value(pubname, dataname, recname, row, col, nx_int64(value))
    end

	if result == true then
		return gm_helper.succeed();
	end
	return gm_helper.error("set value failed.");
end

--获取公共数据的表格值
--x_pub_rec_get_value 空间名称 表格名称 行号 列号
function x_pub_rec_get_value(gm, ...)
    if table.getn(arg) < 5 then
        return gm_helper.error("format: x_pub_rec_get_value <pubname> <dataname> <recname> <row> <col>");
    end

    local pubname = nx_string(arg[1])
    local dataname = nx_widestr(arg[2])
    local recname = nx_string(arg[3])
    local row = nx_int(arg[4])
    local col = nx_int(arg[5])
	
	if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end	

	local result = nx_string( nx_query_pubdata_record_value(pubname, dataname, recname, row, col) );
    return gm_helper.content(result);
end

-- 查询公共区属性
function x_pub_prop_query( gm, ... )
    if table.getn(arg) < 3 then
        return gm_helper.error("format: x_pub_prop_query <pubname> <dataname> <propname>");
    end


    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local propname = nx_string(arg[3]);

    if not nx_find_pubdata_property(pubname, dataname, propname) then
       return gm_helper.error("not found data: " .. propname);
    end
	
    local value = nx_query_pubdata_property(pubname, dataname, propname);
    return gm_helper.content(value);
end

-- 设置公共区属性
function x_pub_prop_set( gm, ... )
	
    if table.getn(arg) < 4 then
        return gm_helper.error("format: x_pub_prop_set <pubname> <dataname> <propname> <value>");
    end

    local pubname = nx_string(arg[1]);
    local dataname = nx_widestr(arg[2]);
    local propname = nx_string(arg[3]);
    local value = arg[4]

    local type = nx_get_pubdata_property_type(pubname, dataname, propname)

    if nx_int(type) == nx_int(0) then
       return gm_helper.error("not found data: " .. propname);
    end

	local result = false;
    if type == VTYPE_INT then
        result = nx_set_pub_property(pubname, dataname, propname, nx_int(value))
    elseif type == VTYPE_INT64 then
        result = nx_set_pub_property(pubname, dataname, propname, nx_int64(value))
    elseif type == VTYPE_FLOAT then
        result = nx_set_pub_property(pubname, dataname, propname, nx_float(value))
    elseif type == VTYPE_DOUBLE then
        result = nx_set_pub_property(pubname, dataname, propname, nx_double(value))
    elseif type == VTYPE_STRING then
        result = nx_set_pub_property(pubname, dataname, propname, nx_string(value))
    elseif type == VTYPE_WIDESTR then
        result = nx_set_pub_property(pubname, dataname, propname, nx_widestr(value))
    elseif type == VTYPE_OBJECT then
        result = nx_set_pub_property(pubname, dataname, propname, nx_object(value))
    end
	
    if not result then
       return gm_helper.error("set pub data:" .. propname .. " failed.");
    end

    return gm_helper.succeed();
end

function x_new_test_function_1(gm, ...)
	return gm_helper.succeed();
end