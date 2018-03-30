//---------------------------------------------------------
//文件名:       ToolBoxDefine.h
//内  容:       
//说  明:       
//          
//创建日期:      2014年11月22日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef _FSGAME_TOOL_BOX_DEFINE_H_
#define _FSGAME_TOOL_BOX_DEFINE_H_

#include <string>

enum EmToolBoxResult
{
	EM_TOOL_BOX_RESULT_SUCCEED,		// 成功
	EM_TOOL_BOX_RESULT_NO_SPACE,	// 背包空间不足

	EM_TOOL_BOX_RESULT_ERROR,		// 服务器未知错
};

// 客户端通信的子消息定义
enum
{
    TOOL_BOX_SUBMSG_SELL     = 1,   // 出售
    TOOL_BOX_SUBMSG_UNLOCK   = 2,   // 解锁
    TOOL_BOX_SUBMSG_ARRANGE  = 3,   // 整理
    TOOL_BOX_SUBMSG_EXPAND   = 4,   // 扩充容器
	TOOL_BOX_SUBMSG_MOVEITEM = 5,   // 移动容器物品(穿装备等)
    TOOL_BOX_SUBMSG_USE_ITEM = 6,   // 使用物品
    TOOL_BOX_SUBMSG_USE_ITEM_ON_ITEM = 7,   // 对物品使用物品(比如把毒药道具使用到武器上)
    TOOL_BOX_SUBMSG_USE_ITEM_MERGE = 8,     // 物品合成，例如把多个道具合成一个道具
	TOOL_BOX_SUBMSG_PICKUP_TEMPORAR_BOX,	// 从临时背包提出物品 参数: 格子索引列表(VarList[int] 1, 2, 3, 4...) 服务器返回参数: result[EmToolBoxResult] 成功数[int]
};

// 获取装备提醒
enum
{
	TOOL_BOX_SUBMSG_SHOW	 = 1,	// 展示新获得物品 int 数量 {string configid, string UniqueID, int 物品数量}
	TOOL_BOX_SUBMSG_NEW_USE	 = 2,	// 新获得物品提示使用 string configid, string UniqueID, int 物品数量
};

// 消息发送参数的物品属性数量
const int TOOL_BOX_SUBMSG_SHOW_ITEM_ATTR_NUM = 3;
// 用于展示的物品属性
enum ShowItem
{
	SHOW_ITEM_CONFIG_ID = 0,
	SHOW_ITEM_UNIQUE_ID,
	SHOW_ITEM_AMOUNT,
};

// 服务器之间通信的子消息定义
enum
{
    TOOL_BOX_SUBCOMMAND_USE_ITEM = 1,         // 使用道具
    TOOL_BOX_SUBCOMMAND_USE_ITEM_ON_ITEM = 2, // 对道具使用道具
};


#define TEMPORARY_BOX_DEFAULT_CAPACITY		256	// 默认临时背包大小

#endif // _FSGAME_TOOL_BOX_DEFINE_H_
