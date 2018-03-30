#ifndef __RideDefine_h__
#define __RideDefine_h__

#include <vector>
#include <map>
#include "FsGame/CommonModule/CommRuleDefine.h"

#define MAX_RIDE_NUMBER 1

// 坐骑阶级配置
struct RideStepCfg
{
	RideStepCfg(int _step) : 
		step(_step), 
		need_star(0)
	{
		upgrade_items_list.clear();
	}

	int step;	// 阶级
	int need_star;	// 升阶所需星级
	std::vector<Consume_Vec> upgrade_items_list;
	std::string skin_id;	// 皮肤ID

	bool operator == (const RideStepCfg& step_cfg) const
	{
		return step_cfg.step == step;
	}
};
typedef std::vector<RideStepCfg> RideStepCfgList;

// 坐骑皮肤
struct RideSkin
{
	int skin_id;	// 皮肤id
	int att_add_pck_id;	// 激活修正包
	int special_item;
	int skin_type;	// 皮肤种类

	RideSkin() : 
		skin_id(0),
		att_add_pck_id(0), 
		special_item(0), 
		skin_type(0)
		{

		}
};
typedef std::map<int, RideSkin> RideSkinMap;

// 皮肤解锁配置
struct SkinUnlockCfg
{
	SkinUnlockCfg(int _id) :
	id(_id), 
	pre_skin(0)
	{
		unlock_fee.clear();
		unlock_items.clear();
	}

	int id;	// ConfigID
	Consume_Vec unlock_fee;
	Consume_Vec unlock_items;
	int pre_skin;

	bool operator == (const SkinUnlockCfg& cfg) const
	{
		return cfg.id == id;
	}
};
typedef std::vector<SkinUnlockCfg> SkinUnlockCfgVec;

// 坐骑激活状态
enum RideState 
{
    RideState_None,
    RideState_Actived,
};

// 上下坐骑操作
enum RideUpDownOperate
{
	RIDE_UP_DOWN_OPERATE_UP = 0,	// 上
	RIDE_UP_DOWN_OPERATE_DOWN,		// 下
};

// 皮肤使用状态
enum SkinUseState
{
	SKIN_USE_STATE_UNUSE = 0,	// 未使用
	SKIN_USE_STATE_INUSE = 1,	// 使用中
};

// 更新修正包方式
enum UpdatePkgTypes
{
	UPDATE_PKG_TYPE_NONE,
	UPDATE_PKG_TYPE_ALL = 1,	// 更新修正包所有属性
	UPDATE_PKG_TYPE_SPEED,	// 更新修正包的speed属性
	UPDATE_PKG_TYPE_EXCEPT_SPEED,	// 更新修正包除speed外的属性
};

// 皮肤类型
enum RideSkinTypes
{
	RIDE_SKIN_TYPE_STEP = 0,	// 升阶
	RIDE_SKIN_TYPE_FASION = 1,	// 时装
};


// 坐骑2级消息ID  客户端发服务端
enum CSRideMsg
{
	/*!
	* @brief	坐骑皮肤信息请求
	* @param int 坐骑索引
	*/
	CS_RIDE_SKIN_MSG_REQ = 1,

	/*!
	* @brief	坐骑更换皮肤
	* @param int 坐骑皮肤id
	* @param int 坐骑索引
	*/
	CS_RIDE_CHANGE_SKIN,

	/*!
	* @brief	手动上下坐骑
	* @param int 操作类型（0：上坐骑 1：下坐骑）
	*/
	CS_RIDE_UP_DOWN,

	/*!
	* @brief	去除新皮肤标识
	* @param int 坐骑皮肤id
	*/
	CS_RIDE_CLEAR_SKIN_FLAG,

	/*!
	* @brief	解锁坐骑皮肤
	* @param int 皮肤ID
	*/
	CS_RIDE_SKIN_UNLOCK,

	/*!
	* @brief	坐骑升级
	*/
	CS_RIDE_UPGRADE_LEVEL,

	/*!
	* @brief	坐骑升阶
	*/
	CS_RIDE_UPGRADE_STEP,
};

// 坐骑2级消息ID  服务端发客户端
enum SCRideMsg
{
	/*!
	 * @brief	坐骑皮肤信息
	 * @param int 坐骑索引
	 * @param int 数据量n
	 * @param	以下循环n次
		int 皮肤id
		int 状态
		int 新皮肤标识（0：新 1：旧）
	 */
	SC_RIDE_SKIN_MSG_RES = 0, 
};


// 坐骑修正包 = 基础包 + 阶级*RIDE_BASE_PACKAGE_COEFFICIENT + 等级
const int RIDE_BASE_PACKAGE_COEFFICIENT = 1000; 

#endif // __RideDefine_h__