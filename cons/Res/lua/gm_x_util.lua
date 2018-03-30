--获得当前属性操作对象
function get_prop_target(gm)
    if not nx_find_data(gm, "GMPropTarget") then
        return nx_null();
    end
	
    local target = nx_query_data(gm, "GMPropTarget");
    return target
end