//--------------------------------------------------------------------
// 文件名:      ToolItemDefine.h
// 内  容:      ToolItemModule相关定义
// 说  明:		
// 创建日期:    2014年12月11日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef FSGAME_DEFINE_TOOL_ITEM_DEFINE_H
#define FSGAME_DEFINE_TOOL_ITEM_DEFINE_H

// 道具绑定状态
enum EmItemBindState
{
	EM_ITEM_BIND_NONE,		// 未绑定
	EM_ITEM_BIND_LOCK,		// 绑定
};

// 状具交易标志
enum EmItemTradeFlag
{
	EM_ITEM_TRADE_NONE,		// 不可交易
	EM_ITEM_TRADE_ABLE,		// 可交易
};

// 是否是希有物品
enum EmItemRareFlag
{
	EM_ITEM_RARE_NO,		// 非珍惜物品
	EM_ITEM_RARE_YES,		// 珍惜物品
};

// 道具的使用效果
enum ItemEffectType
{
	ITEM_EFFECT_NONE	= -1,
	ITEM_EFFECT_GIFT,		// 使用后获得礼包掉落

	MAX_ITEM_EFFECT_NUM
};

// 物品批量使用 最大个数
const int USE_ITEM_MAX_COUNT = 50;

// 物品效果参数
struct ItemEffectParam
{
	int effect_type;            // 效果类型
	std::string effect_value;   // 效果值
	std::string ext_param01;    // 扩展参数1
	std::string ext_param02;    // 扩展参数2
	ItemEffectParam():effect_type(0),
		effect_value(""),
		ext_param01(""),
		ext_param02("")
	{

	}
};

#include "utils/util_config.h"
// 物品绑定进一步限制
DECLARE_CONFIG_OBJECT_BEGIN_T(CfgItemBindLimit, std::string)

#define CONFIG_CLASS_NAME CfgItemBindLimit
#define PROPLIST      \
	DECLARE_CONFIG_PROP_FIELD(ID, "配置id", string)	\
	DECLARE_CONFIG_PROP_FIELD_EX(Color, "最低可绑定品阶", 0, int32)

#include "utils/util_config_template.h"
DECLARE_CONFIG_OBJECT_END()

// 物品绑定进一步限制
DECLARE_CONFIG_OBJECT_BEGIN_T(CfgPatchItem, std::string)

#define CONFIG_CLASS_NAME CfgPatchItem
#define PROPLIST      \
	DECLARE_CONFIG_PROP_FIELD(ID, "orig item id", string)	\
	DECLARE_CONFIG_PROP_FIELD(NewItem, "replace new item id", string) \
	DECLARE_CONFIG_PROP_FIELD_EX(Color, "item color", 0, int32 )

#include "utils/util_config_template.h"
DECLARE_CONFIG_OBJECT_END()
#endif // 