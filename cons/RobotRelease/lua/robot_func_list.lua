-- 文件名: robot_func_list.lua
-- 说明: 自定义函数列表，自定义消息
-- 作者:
-- 版权所有:

require("./robot_var_type_def")
require("./robot_def")

-- 响应自定义函数的handlers
tb_func_list_handler = {}

-- 计费注册
tb_func_list_handler["charge_register"] = function (robot_obj)
    robot_obj.ex_register(robot_obj)
end

-- 计费登录
tb_func_list_handler["charge_login"] = function (robot_obj)
    charge_login(robot_obj)
end

-- 普通登录
tb_func_list_handler["normal_login"] = function (robot_obj)
    robot_obj.ex_connect_entry(robot_obj)
end

-- 创建角色
tb_func_list_handler["create_role"] = function (robot_obj)
    robot_obj.ex_create_role(robot_obj)
end


-- 选择角色
tb_func_list_handler["choose_role"] = function (robot_obj)
    robot_obj.ex_choose_role(robot_obj)
end

-- 退出帐号
tb_func_list_handler["logout_account"] = function (robot_obj)
    logout_account(robot_obj)
end


-- 使用技能
tb_func_list_handler["use_skill"] = function (robot_obj)
    robot_obj.ex_use_skill(robot_obj)
end

-- 测试参数
tb_func_list_handler["func_test"] = function (robot_obj,s_value,i_value,f_value,d_value,b_value)
    local new_string = s_value
end

