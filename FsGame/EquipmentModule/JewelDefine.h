
#ifndef JewelDefine_h__
#define JewelDefine_h__
#include <string>
#include <map>
#include <vector>

// 宝石使用位置
struct JewelUsePos
{
	int jewel_type;
	std::vector<int> equip_pos_list;

	JewelUsePos(int _type) : jewel_type(_type)
	{
		equip_pos_list.clear();
	}

	bool operator == (const JewelUsePos& temp) const
	{
		return temp.jewel_type == jewel_type;
	}
};
typedef std::vector<JewelUsePos> JewelUsePosVec;

// 宝石索引关联表
typedef std::map<int, std::string> JewelIndexMap;

// 宝石数量关联表
typedef std::map<std::string, int> JewelMap;

// 宝石合成消耗
struct JewelComposeConsume
{
	int point_values;
	int compose_fee;
	int buy_cost;
	int buy_capital;

	JewelMap jewel_map;

	JewelComposeConsume() :
		point_values(0),
		compose_fee(0),
		buy_cost(0),
		buy_capital(0)
	{
		jewel_map.clear();
	}
};

// 客户端->服务器
enum C2S_Jewel_Submsg
{
	/*!
	* @brief	镶嵌
	* @param int 宝石出处容器
	* @param int 在容器中的位置
	* @param int 要镶嵌的装备格子索引
	* @param int 要镶嵌的装备的孔位索引
	*/
	C2S_JEWEL_SUBMSG_INLAY = 1,

	/*!
	* @brief	拆下
	* @param int 要拆下的装备格子索引
	* @param int 要拆下的装备的孔位索引
	*/
	C2S_JEWEL_SUBMSG_DISMANTLE,

	/*!
	* @brief	合成
	* @param int 要合成的宝石索引号
	* @param int 合成个数
	* @param int 已选合成材料数n * 2（一对：序号、数量）
	* @param 以下循环n次
	* @param int 已选材料序号
	* @param int 个数
	* @param int 需购买合成材料数n * 2（一对：序号、数量）
	* @param int 需购买材料序号
	* @param int 个数
	*/
	C2S_JEWEL_SUBMSG_COMPOSE,
};

// 服务器->客户端
enum S2C_Jewel_SubMsg
{
	/*!
	* @brief	合成成功
	* @param string 宝石id
	* @param int 数量
	*/
	S2C_JEWEL_SUBMSG_COMPOSE_SUCCESS = 1,
};

// 是否自动购买
enum 
{
	AUTO_BUY_YES = 1,	// 自动购买
	AUTO_BUY_NO = 2,	// 不自动购买
};


#endif // JewelDefine_h__
