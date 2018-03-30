local gm_helper = require("gm_x_helper")
local area_data_ = require "gm_x_arena_data"
local level_data_3035_ = require "gm_x_level_list_3035"
local level_data_3540_ = require "gm_x_level_list_3540"
local level_data_4045_ = require "gm_x_level_list_4045"
local level_data_4550_ = require "gm_x_level_list_4550"
local level_data_5055_ = require "gm_x_level_list_5055"
local level_data_5560_ = require "gm_x_level_list_5560"
local level_data_6065_ = require "gm_x_level_list_6065"
local level_data_6570_ = require "gm_x_level_list_6570"
local level_data_7075_ = require "gm_x_level_list_7075"
local level_data_7580 = require "gm_x_level_list_7580"
local datas = {
	area_data = area_data_,
	level_30 = level_data_3035_,
	level_35 = level_data_3540_,
	level_40 = level_data_4045_,
	level_45 = level_data_4550_,
	level_50 = level_data_5055_,
	level_55 = level_data_5560_,
	level_60 = level_data_6065_,
	level_65 = level_data_6570_,
	level_70 = level_data_7075_,
	level_75 = level_data_7580_,
	};

--添加一行
function imp_add_pub_row_data(  pubname, dataname, recname, rowvalue   )
	local cols = nx_get_pubdata_record_cols(pubname, dataname, recname);
    if table.getn(rowvalue) <  cols then
		return false;
    end
	local var = {};
    for col = 0, cols - 1 do
        local coltype = nx_get_pubdata_record_coltype(pubname, dataname, recname, col);
		var[col+1] = nx_to_type(coltype, rowvalue[col+1])
    end

    if nx_add_pub_rec_row_value(pubname, dataname, recname, unpack(var)) then
        return true;
    end
	
	return false;
end

function imp_import_data( pubname, dataname, recname, data )
	local data_name_ = nx_widestr(dataname);
	for i, row in ipairs( data ) do
		if ( not imp_add_pub_row_data( pubname, data_name_, recname, row ) ) then
			return false, i;
		end
	end
	
	return true;
end

function x_import_robot_data( self, name )
	local data_ = datas[nx_string(name)];
	local result, index = imp_import_data(data_.pubname, data_.dataname, data_.recname, data_.data);
	if result then
		return gm_helper.succeed();
	end
	
	return gm_helper.error( string.format("import data error(name:%s index:%d)", name, index) );
end

-- 修复指定表某一列值
function x_repair_pub_rec_data( self, pubname, dataname, recname, col, value )
	dataname = nx_widestr( dataname );
	col = nx_int( col );
	if not nx_find_pubdata_record(pubname, dataname, recname) then
        return gm_helper.error("not found record: " .. recname);
    end	
	
    local type = nx_get_pubdata_record_coltype(pubname, dataname, recname, col)
    if nx_int(type) == nx_int(0) then
       return gm_helper.error("record not found or col index overflow.");
    end	
	
	local rows = nx_get_pubdata_record_rows(pubname, dataname, recname);
    for row = 0, rows - 1 do
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
    end
	
	return gm_helper.succeed();
end

-- 修复arena_rank_list
function x_repair_arena_rank_list14( self )
	return x_repair_pub_rec_data( self, "domain", "Domain_RankingList_100", "arena_rank_list", 14, "" );
end