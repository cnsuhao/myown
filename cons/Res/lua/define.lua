TYPE_SCENE	= 1;	-- 场景
TYPE_PLAYER	= 2;	-- 玩家
TYPE_NPC	= 4;	-- NPC
TYPE_ITEM	= 8;	-- 物品
TYPE_HELPER	= 16;	-- 辅助对象

--属性数据类型
VTYPE_UNKNOWN = 0;	-- 未知
VTYPE_BOOL = 1;		-- 布尔
VTYPE_INT = 2;		-- 32位整数
VTYPE_INT64 = 3;	-- 64位整数
VTYPE_FLOAT = 4;	-- 单精度浮点数
VTYPE_DOUBLE = 5;	-- 双精度浮点数
VTYPE_STRING = 6;	-- 字符串
VTYPE_WIDESTR = 7;	-- 宽字符串
VTYPE_OBJECT = 8;	-- 对象号
VTYPE_POINTER = 9;	-- 指针
VTYPE_USERDATA = 10;-- 用户数据
VTYPE_TABLE = 11;	-- 表
VTYPE_MAX = 12;

--提示信息类型
TIPSTYPE_SYSTEMINFO = 0;	--系统信息(引擎模块发送的提示信息用，逻辑模块不要使用)
TIPSTYPE_SYSTEMNO = 1;		--系统提示(客户端显示加[提示]前缀)
TIPSTYPE_TIPSINFO = 2;		--主画面向上滚动的提示信息
TIPSTYPE_GMINFO = 99;		--GM调试信息

-- 转换格式
function nx_to_type(vtype, value)

	if vtype == VTYPE_INT then
		return nx_int(value);
	elseif vtype == VTYPE_INT64 then
		return nx_int64(value);
	elseif vtype == VTYPE_FLOAT then
		return nx_float(value);
	elseif vtype == VTYPE_DOUBLE then
		return nx_double(value);
	elseif vtype == VTYPE_STRING then
		return nx_string(value);
	elseif vtype == VTYPE_WIDESTR then
		return nx_widestr(value);
	elseif vtype == VTYPE_OBJECT then
		return nx_objet(value);
	end

end
