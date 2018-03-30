local _helper = {};

function _helper.tojson(tb)
	local function serialize(tbl)  
			local tmp = {}  
			for k, v in pairs(tbl) do  
					local k_type = type(k)  
					local v_type = type(v)  
					local key = (k_type == "string" and "\"" .. k .. "\":")  
						or (k_type == "number" and "")  
					local value = (v_type == "table" and serialize(v))  
						or (v_type == "boolean" and tostring(v))  
						or (v_type == "string" and "\"" .. v .. "\"")  
						or (v_type == "number" and v)  
					tmp[#tmp + 1] = key and value and tostring(key) .. tostring(value) or nil  
			end  
			if table.maxn(tbl) == 0 then  
					return "{" .. table.concat(tmp, ",") .. "}"  
			else  
					return "[" .. table.concat(tmp, ",") .. "]"  
			end  
	end
	--local p = type(tb);
	--nx_msgbox(nx_string(type(tb)));
	assert(type(tb) == "table")  
	return serialize(tb);
end

function _helper.error( err )
	local result = {};
	result["error"] = err;
	return _helper.tojson( result );
end

function _helper.content(data)
	local result = {};
	result["content"] = data;
	return _helper.tojson( result );
end

function _helper.succeed()
	return _helper.content( "success" );
end

return _helper;